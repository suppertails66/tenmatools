#include "util/TIniFile.h"
#include "exception/TGenericException.h"
#include <iostream>
#include <fstream>

namespace BlackT {


TIniFile::TIniFile() { };
TIniFile::TIniFile(const std::string& filename)
  : sourceFileName_(filename) {
  readFile(filename);
}

void TIniFile::readFile(const std::string& filename) {
  std::ifstream ifs(filename);
  std::string currentSection = "";
  while (ifs.good()) {
    std::string line;
    getline(ifs, line);
    
    if (line.size() <= 1) {
      continue;
    }
    
    // Comment
    if ((line[0] == ';') || (line[0] == '#')) {
      continue;
    }
    
    int start = 0;
    int end = 0;
    
    // Section
    if (line[0] == '[') {
      start = 1;
      end = 1;
      bool failed = false;
      while (line[end] != ']') {
        
        ++end;
      
        // No closing bracket: ignore line
        if (end >= line.size()) {
          failed = true;
          break;
        }
      }
      
      if (failed) {
        continue;
      }
      
      currentSection = line.substr(start, (end - start));
    }
    // Key-value pair
    else {
      start = 0;
      end = 0;
      bool failed = false;
      while (line[end] != '=') {
      
        // No equals sign: ignore line
        if (end >= line.size()) {
          failed = true;
          break;
        }
        
        ++end;
      }
      
      if (failed) {
        continue;
      }
      
      std::string key = line.substr(start, (end - start));
    
      start = end + 1;
      end = start;
      
      if (start > line.size()) {
        continue;
      }
      
      std::string value = line.substr(start, (line.size() - start));
      
      setValue(currentSection, key, value);
    }
    
  }
}

void TIniFile::writeFile(const std::string& filename) {
  std::ofstream ofs(filename);
  for (SectionKeysMap::iterator it = values_.begin();
       it != values_.end();
       ++it) {
     ofs << '[' << it->first << ']' << std::endl;
     
     for (StringMap::iterator jt = it->second.begin();
          jt != it->second.end();
          ++jt) {
       ofs << jt->first << '=' << jt->second << std::endl;
     }
     
     ofs << std::endl;
  }
}

std::string TIniFile::sourceFileName() const {
  return sourceFileName_;
}

bool TIniFile::hasSection(const std::string& section) const {
  if (values_.find(section) != values_.end()) {
    return true;
  }
  
  return false;
}

bool TIniFile::hasKey(const std::string& section,
            const std::string& key) const {
  SectionKeysMap::const_iterator findIt = values_.find(section);
  if (findIt == values_.end()) {
    return false;
  }
  
  StringMap::const_iterator valueIt = findIt->second.find(key);
  
  if (valueIt == findIt->second.end()) {
    return false;
  }
  
  return true;
}

std::string TIniFile::valueOfKey(const std::string& section,
                       const std::string& key) const {
//  return values_[section][key];

  SectionKeysMap::const_iterator findIt = values_.find(section);
  
  if (findIt == values_.end()) {
    throw TGenericException(T_SRCANDLINE,
                            "TIniFile::valueOfKey()",
                            "Accessed non-existent section: "
                              + section);
  }
  
  StringMap::const_iterator valueIt = findIt->second.find(key);
  
  if (valueIt == findIt->second.end()) {
    throw TGenericException(T_SRCANDLINE,
                            "TIniFile::valueOfKey()",
                            "Accessed non-existent key: ('"
                              + section + "', '"
                              + key + "')");
  }
  
  return valueIt->second;
}

void TIniFile::setValue(const std::string& section,
              const std::string& key,
              const std::string& value) {
  values_[section][key] = value;
}

void TIniFile::clearSection(const std::string& section) {
  SectionKeysMap::iterator findIt = values_.find(section);
  
  if (findIt == values_.end()) {
    return;
  }

  values_.erase(values_.find(section));
}

void TIniFile::clearKey(const std::string& section,
              const std::string& key) {
  SectionKeysMap::iterator findIt = values_.find(section);
  
  if (findIt == values_.end()) {
    return;
  }
  
  StringMap::iterator valueIt = findIt->second.find(key);
  
  if (valueIt == findIt->second.end()) {
    return;
  }
  
  findIt->second.erase(valueIt);
}

int TIniFile::numSections() const {
  return values_.size();
}
  
SectionKeysMap::iterator TIniFile::begin() {
  return values_.begin();
}

SectionKeysMap::const_iterator TIniFile::cbegin() const {
  return values_.cbegin();
}

SectionKeysMap::iterator TIniFile::end() {
  return values_.end();
}

SectionKeysMap::const_iterator TIniFile::cend() const {
  return values_.cend();
}
  
int TIniFile::numKeysInSection(const std::string& section) const {
  SectionKeysMap::const_iterator findIt = values_.find(section);
  if (findIt == values_.cend()) return 0;
  return findIt->second.size();
}
  
KeyValueMap::iterator TIniFile::sectionBegin(
    const std::string& section) {
  SectionKeysMap::iterator findIt = values_.find(section);
  if (findIt == values_.end()) {
    throw TGenericException(T_SRCANDLINE,
                            "TIniFile::sectionBegin()",
                            "Nonexistent section: " + section);
  }
  
  return findIt->second.begin();
}

KeyValueMap::const_iterator TIniFile::sectionCbegin(
    const std::string& section) const {
  SectionKeysMap::const_iterator findIt = values_.find(section);
  if (findIt == values_.cend()) {
    throw TGenericException(T_SRCANDLINE,
                            "TIniFile::sectionCbegin()",
                            "Nonexistent section: " + section);
  }
  
  return findIt->second.begin();
}

KeyValueMap::iterator TIniFile::sectionEnd(
    const std::string& section) {
  SectionKeysMap::iterator findIt = values_.find(section);
  if (findIt == values_.end()) {
    throw TGenericException(T_SRCANDLINE,
                            "TIniFile::sectionEnd()",
                            "Nonexistent section: " + section);
  }
  
  return findIt->second.end();
}

KeyValueMap::const_iterator TIniFile::sectionCend(
    const std::string& section) const {
  SectionKeysMap::const_iterator findIt = values_.find(section);
  if (findIt == values_.cend()) {
    throw TGenericException(T_SRCANDLINE,
                            "TIniFile::sectionEnd()",
                            "Nonexistent section: " + section);
  }
  
  return findIt->second.cend();
}


};
