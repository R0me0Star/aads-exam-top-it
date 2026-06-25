#include "interactive.hpp"
#include <cctype>
#include <fstream>

namespace pozdnyakov
{

  AppArgs2 parseArguments2(const int argc, char *argv[])
  {
    AppArgs2 args;
    args.hasIn = false;
    args.hasData = false;
    args.isError = false;
    for (int i = 1; i < argc; ++i) {
      const std::string argStr = argv[i];
      if (argStr.find("in:") == 0) {
        if (args.hasIn) {
          args.isError = true;
          return args;
        }
        args.inFilename = argStr.substr(3);
        args.hasIn = true;
      } else if (argStr.find("data:") == 0) {
        if (args.hasData) {
          args.isError = true;
          return args;
        }
        args.dataFilename = argStr.substr(5);
        args.hasData = true;
      } else {
        args.isError = true;
        return args;
      }
    }
    if (!args.hasData) {
      args.isError = true;
    }
    return args;
  }

  size_t findPersonIdx(const DynamicArray< Person > &arr, const size_t id)
  {
    for (size_t i = 0; i < arr.size; ++i) {
      if (arr.data[i].id == id) {
        return i;
      }
    }
    return arr.size;
  }

  bool personExists(const DynamicArray< Person > &arr, const size_t id)
  {
    return findPersonIdx(arr, id) < arr.size;
  }

  bool hasDescription(const DynamicArray< Person > &arr, const size_t id)
  {
    const size_t idx = findPersonIdx(arr, id);
    if (idx == arr.size) {
      return false;
    }
    return !arr.data[idx].info.empty();
  }

  void ensurePersonExists(DynamicArray< Person > &arr, const size_t id)
  {
    if (!personExists(arr, id)) {
      Person p;
      p.id = id;
      p.info = "";
      pushBack(arr, p);
    }
  }

  bool readMeets(std::istream &in, DynamicArray< Meet > &meets)
  {
    std::string line;
    while (std::getline(in, line)) {
      if (!line.empty() && line[line.length() - 1] == '\r') {
        line.erase(line.length() - 1);
      }
      bool isBlank = true;
      for (size_t i = 0; i < line.length(); ++i) {
        if (!std::isspace(static_cast< unsigned char >(line[i]))) {
          isBlank = false;
          break;
        }
      }
      if (isBlank) {
        continue;
      }
      size_t pos = 0;
      const size_t len = line.length();
      while (pos < len && std::isspace(static_cast< unsigned char >(line[pos]))) {
        ++pos;
      }
      size_t id1 = 0;
      bool hasId1 = false;
      while (pos < len && std::isdigit(static_cast< unsigned char >(line[pos]))) {
        id1 = id1 * 10 + static_cast< size_t >(line[pos] - '0');
        ++pos;
        hasId1 = true;
      }
      if (!hasId1) {
        return false;
      }
      while (pos < len && std::isspace(static_cast< unsigned char >(line[pos]))) {
        ++pos;
      }
      size_t id2 = 0;
      bool hasId2 = false;
      while (pos < len && std::isdigit(static_cast< unsigned char >(line[pos]))) {
        id2 = id2 * 10 + static_cast< size_t >(line[pos] - '0');
        ++pos;
        hasId2 = true;
      }
      if (!hasId2) {
        return false;
      }
      while (pos < len && std::isspace(static_cast< unsigned char >(line[pos]))) {
        ++pos;
      }
      size_t duration = 0;
      bool hasDuration = false;
      while (pos < len && std::isdigit(static_cast< unsigned char >(line[pos]))) {
        duration = duration * 10 + static_cast< size_t >(line[pos] - '0');
        ++pos;
        hasDuration = true;
      }
      if (!hasDuration) {
        return false;
      }
      while (pos < len && std::isspace(static_cast< unsigned char >(line[pos]))) {
        ++pos;
      }
      if (pos != len) {
        return false;
      }
      if (id1 == id2) {
        continue;
      }
      Meet meet;
      meet.id1 = id1;
      meet.id2 = id2;
      meet.duration = duration;
      pushBack(meets, meet);
    }
    return true;
  }

  void cmdAnons(std::ostream &out, const DynamicArray< Person > &persons)
  {
    DynamicArray< size_t > ids;
    initArray(ids);
    for (size_t i = 0; i < persons.size; ++i) {
      if (persons.data[i].info.empty()) {
        pushBack(ids, persons.data[i].id);
      }
    }
    for (size_t i = 1; i < ids.size; ++i) {
      const size_t key = ids.data[i];
      size_t j = i;
      while (j > 0 && ids.data[j - 1] > key) {
        ids.data[j] = ids.data[j - 1];
        --j;
      }
      ids.data[j] = key;
    }
    for (size_t i = 0; i < ids.size; ++i) {
      out << ids.data[i] << "\n";
    }
    destroyArray(ids);
  }

  bool cmdDeanon(
    DynamicArray< Person > &persons,
    DynamicArray< Meet > &meets,
    const size_t anonId,
    const size_t id
  )
  {
    if (!personExists(persons, anonId)) {
      return false;
    }
    if (hasDescription(persons, anonId)) {
      return false;
    }
    if (!hasDescription(persons, id)) {
      return false;
    }
    for (size_t i = 0; i < meets.size; ++i) {
      if (meets.data[i].id1 == anonId) {
        meets.data[i].id1 = id;
      }
      if (meets.data[i].id2 == anonId) {
        meets.data[i].id2 = id;
      }
    }
    size_t writeIdx = 0;
    for (size_t i = 0; i < meets.size; ++i) {
      if (meets.data[i].id1 != meets.data[i].id2) {
        meets.data[writeIdx] = meets.data[i];
        ++writeIdx;
      }
    }
    meets.size = writeIdx;
    size_t anonIdx = findPersonIdx(persons, anonId);
    for (size_t i = anonIdx; i + 1 < persons.size; ++i) {
      persons.data[i] = persons.data[i + 1];
    }
    --persons.size;
    return true;
  }

  bool cmdRedesc(
    DynamicArray< Person > &persons,
    const size_t id,
    const std::string &desc
  )
  {
    if (!personExists(persons, id)) {
      return false;
    }
    const size_t idx = findPersonIdx(persons, id);
    persons.data[idx].info = desc;
    return true;
  }

  bool cmdDesc(
    std::ostream &out,
    const DynamicArray< Person > &persons,
    const size_t id
  )
  {
    if (!personExists(persons, id)) {
      return false;
    }
    const size_t idx = findPersonIdx(persons, id);
    if (persons.data[idx].info.empty()) {
      out << "<ANON>\n";
    } else {
      out << persons.data[idx].info << "\n";
    }
    return true;
  }

  bool cmdMeets(
    std::ostream &out,
    const DynamicArray< Person > &persons,
    const DynamicArray< Meet > &meets,
    const size_t id
  )
  {
    if (!personExists(persons, id)) {
      return false;
    }
    DynamicArray< Meet > found;
    initArray(found);
    for (size_t i = 0; i < meets.size; ++i) {
      Meet m;
      m.id1 = id;
      m.duration = meets.data[i].duration;
      if (meets.data[i].id1 == id) {
        m.id2 = meets.data[i].id2;
        pushBack(found, m);
      } else if (meets.data[i].id2 == id) {
        m.id2 = meets.data[i].id1;
        pushBack(found, m);
      }
    }
    for (size_t i = 1; i < found.size; ++i) {
      const Meet key = found.data[i];
      size_t j = i;
      while (j > 0) {
        const Meet &prev = found.data[j - 1];
        const bool less = prev.id2 > key.id2
          || (prev.id2 == key.id2 && prev.duration > key.duration);
        if (!less) {
          break;
        }
        found.data[j] = found.data[j - 1];
        --j;
      }
      found.data[j] = key;
    }
    for (size_t i = 0; i < found.size; ++i) {
      out << found.data[i].id2 << " " << found.data[i].duration << "\n";
    }
    destroyArray(found);
    return true;
  }

  bool cmdCommons(
    std::ostream &out,
    const DynamicArray< Person > &persons,
    const DynamicArray< Meet > &meets,
    const size_t id1,
    const size_t id2
  )
  {
    if (!personExists(persons, id1) || !personExists(persons, id2)) {
      return false;
    }
    DynamicArray< size_t > met1;
    initArray(met1);
    DynamicArray< size_t > met2;
    initArray(met2);
    for (size_t i = 0; i < meets.size; ++i) {
      if (meets.data[i].id1 == id1) {
        pushBack(met1, meets.data[i].id2);
      } else if (meets.data[i].id2 == id1) {
        pushBack(met1, meets.data[i].id1);
      }
      if (meets.data[i].id1 == id2) {
        pushBack(met2, meets.data[i].id2);
      } else if (meets.data[i].id2 == id2) {
        pushBack(met2, meets.data[i].id1);
      }
    }
    DynamicArray< size_t > common;
    initArray(common);
    for (size_t i = 0; i < met1.size; ++i) {
      const size_t candidate = met1.data[i];
      if (candidate == id1 || candidate == id2) {
        continue;
      }
      bool inMet2 = false;
      for (size_t j = 0; j < met2.size; ++j) {
        if (met2.data[j] == candidate) {
          inMet2 = true;
          break;
        }
      }
      if (!inMet2) {
        continue;
      }
      bool alreadyAdded = false;
      for (size_t j = 0; j < common.size; ++j) {
        if (common.data[j] == candidate) {
          alreadyAdded = true;
          break;
        }
      }
      if (!alreadyAdded) {
        pushBack(common, candidate);
      }
    }
    for (size_t i = 1; i < common.size; ++i) {
      const size_t key = common.data[i];
      size_t j = i;
      while (j > 0 && common.data[j - 1] > key) {
        common.data[j] = common.data[j - 1];
        --j;
      }
      common.data[j] = key;
    }
    for (size_t i = 0; i < common.size; ++i) {
      out << common.data[i] << "\n";
    }
    destroyArray(met1);
    destroyArray(met2);
    destroyArray(common);
    return true;
  }

  bool cmdLess(
    std::ostream &out,
    const DynamicArray< Person > &persons,
    const DynamicArray< Meet > &meets,
    const size_t time,
    const size_t id
  )
  {
    if (!personExists(persons, id)) {
      return false;
    }
    DynamicArray< Meet > found;
    initArray(found);
    for (size_t i = 0; i < meets.size; ++i) {
      if (meets.data[i].duration >= time) {
        continue;
      }
      Meet m;
      m.id1 = id;
      m.duration = meets.data[i].duration;
      if (meets.data[i].id1 == id) {
        m.id2 = meets.data[i].id2;
        pushBack(found, m);
      } else if (meets.data[i].id2 == id) {
        m.id2 = meets.data[i].id1;
        pushBack(found, m);
      }
    }
    for (size_t i = 1; i < found.size; ++i) {
      const Meet key = found.data[i];
      size_t j = i;
      while (j > 0) {
        const Meet &prev = found.data[j - 1];
        const bool less = prev.id2 > key.id2
          || (prev.id2 == key.id2 && prev.duration > key.duration);
        if (!less) {
          break;
        }
        found.data[j] = found.data[j - 1];
        --j;
      }
      found.data[j] = key;
    }
    for (size_t i = 0; i < found.size; ++i) {
      out << found.data[i].id2 << " " << found.data[i].duration << "\n";
    }
    destroyArray(found);
    return true;
  }

  bool cmdGreater(
    std::ostream &out,
    const DynamicArray< Person > &persons,
    const DynamicArray< Meet > &meets,
    const size_t time,
    const size_t id
  )
  {
    if (!personExists(persons, id)) {
      return false;
    }
    DynamicArray< Meet > found;
    initArray(found);
    for (size_t i = 0; i < meets.size; ++i) {
      if (meets.data[i].duration <= time) {
        continue;
      }
      Meet m;
      m.id1 = id;
      m.duration = meets.data[i].duration;
      if (meets.data[i].id1 == id) {
        m.id2 = meets.data[i].id2;
        pushBack(found, m);
      } else if (meets.data[i].id2 == id) {
        m.id2 = meets.data[i].id1;
        pushBack(found, m);
      }
    }
    for (size_t i = 1; i < found.size; ++i) {
      const Meet key = found.data[i];
      size_t j = i;
      while (j > 0) {
        const Meet &prev = found.data[j - 1];
        const bool less = prev.id2 > key.id2
          || (prev.id2 == key.id2 && prev.duration > key.duration);
        if (!less) {
          break;
        }
        found.data[j] = found.data[j - 1];
        --j;
      }
      found.data[j] = key;
    }
    for (size_t i = 0; i < found.size; ++i) {
      out << found.data[i].id2 << " " << found.data[i].duration << "\n";
    }
    destroyArray(found);
    return true;
  }

  bool cmdOutPersons(
    const DynamicArray< Person > &persons,
    const std::string &filename
  )
  {
    std::ofstream out(filename.c_str());
    if (!out.is_open()) {
      return false;
    }
    for (size_t i = 0; i < persons.size; ++i) {
      if (!persons.data[i].info.empty()) {
        out << persons.data[i].id << " " << persons.data[i].info << "\n";
      }
    }
    return true;
  }

  void runInteractive(
    std::istream &in,
    std::ostream &out,
    DynamicArray< Person > &persons,
    DynamicArray< Meet > &meets
  )
  {
    std::string cmd;
    while (in >> cmd) {
      if (cmd == "anons") {
        cmdAnons(out, persons);
      } else if (cmd == "deanon") {
        size_t anonId = 0;
        size_t id = 0;
        if (!(in >> anonId >> id)) {
          out << "<INVALID COMMAND>\n";
          continue;
        }
        if (!cmdDeanon(persons, meets, anonId, id)) {
          out << "<INVALID COMMAND>\n";
        }
      } else if (cmd == "redesc") {
        size_t id = 0;
        if (!(in >> id)) {
          out << "<INVALID COMMAND>\n";
          continue;
        }
        std::string token;
        if (!(in >> token) || token.empty() || token[0] != '"') {
          out << "<INVALID COMMAND>\n";
          continue;
        }
        std::string desc = token.substr(1);
        if (!desc.empty() && desc[desc.length() - 1] == '"') {
          desc.erase(desc.length() - 1);
        } else {
          std::string part;
          bool closed = false;
          while (in >> part) {
            desc += " " + part;
            if (!part.empty() && part[part.length() - 1] == '"') {
              desc.erase(desc.length() - 1);
              closed = true;
              break;
            }
          }
          if (!closed) {
            out << "<INVALID COMMAND>\n";
            continue;
          }
        }
        if (!cmdRedesc(persons, id, desc)) {
          out << "<INVALID COMMAND>\n";
        }
      } else if (cmd == "desc") {
        size_t id = 0;
        if (!(in >> id)) {
          out << "<INVALID COMMAND>\n";
          continue;
        }
        if (!cmdDesc(out, persons, id)) {
          out << "<INVALID COMMAND>\n";
        }
      } else if (cmd == "meets") {
        size_t id = 0;
        if (!(in >> id)) {
          out << "<INVALID COMMAND>\n";
          continue;
        }
        if (!cmdMeets(out, persons, meets, id)) {
          out << "<INVALID COMMAND>\n";
        }
      } else if (cmd == "commons") {
        size_t id1 = 0;
        size_t id2 = 0;
        if (!(in >> id1 >> id2)) {
          out << "<INVALID COMMAND>\n";
          continue;
        }
        if (!cmdCommons(out, persons, meets, id1, id2)) {
          out << "<INVALID COMMAND>\n";
        }
      } else if (cmd == "less") {
        size_t time = 0;
        size_t id = 0;
        if (!(in >> time >> id)) {
          out << "<INVALID COMMAND>\n";
          continue;
        }
        if (!cmdLess(out, persons, meets, time, id)) {
          out << "<INVALID COMMAND>\n";
        }
      } else if (cmd == "greater") {
        size_t time = 0;
        size_t id = 0;
        if (!(in >> time >> id)) {
          out << "<INVALID COMMAND>\n";
          continue;
        }
        if (!cmdGreater(out, persons, meets, time, id)) {
          out << "<INVALID COMMAND>\n";
        }
      } else if (cmd == "out-persons") {
        std::string filename;
        if (!(in >> filename)) {
          out << "<INVALID COMMAND>\n";
          continue;
        }
        if (!cmdOutPersons(persons, filename)) {
          out << "<INVALID COMMAND>\n";
        }
      } else {
        out << "<INVALID COMMAND>\n";
      }
    }
  }

}
