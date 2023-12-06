#include "util/TFileInfo.h"
#include <cctype>

namespace BlackT {

  TFileInfo::TFileInfo()
    : size_(0),
      isDirectory_(false),
      accessedTime_(0),
      modifiedTime_(0),
      statusChangedTime_(0) { }
  
  std::string TFileInfo::path() const
    { return path_; }
  std::string TFileInfo::name() const
    { return name_; }
  unsigned int TFileInfo::size() const
    { return size_; }
  bool TFileInfo::isDirectory() const
    { return isDirectory_; }
  std::time_t TFileInfo::accessedTime() const
    { return accessedTime_; }
  std::time_t TFileInfo::modifiedTime() const
    { return modifiedTime_; }
  std::time_t TFileInfo::statusChangedTime() const
    { return statusChangedTime_; }
  
  void TFileInfo::setPath(std::string path__)
    { path_ = path__; }
  void TFileInfo::setName(std::string name__)
    { name_ = name__; }
  void TFileInfo::setSize(int size__)
    { size_ = size__; }
  void TFileInfo::setIsDirectory(bool isDirectory__)
    { isDirectory_ = isDirectory__; }
  void TFileInfo::setAccessedTime(std::time_t accessedTime__)
    { accessedTime_ = accessedTime__; }
  void TFileInfo::setModifiedTime(std::time_t modifiedTime__)
    { modifiedTime_ = modifiedTime__; }
  void TFileInfo::setStatusChangedTime(std::time_t statusChangedTime__)
    { statusChangedTime_ = statusChangedTime__; }
  
  bool TFileInfo::sortByIso9660Name(const TFileInfo& first,
                                const TFileInfo& second) {
    // FIXME: according to the spec, we should process the filename
    //        and the extension separately by this same rule
    //        plus a bunch of others for more obscure cases
    
//    if (first.name_.size() < second.name_.size()) return true;
//    else if (first.name_.size() > second.name_.size()) return false;
    
    std::string firstC = first.name_;
    std::string secondC = second.name_;
    
    while (firstC.size() < secondC.size()) firstC += ' ';
    while (firstC.size() > secondC.size()) secondC += ' ';
    
    for (unsigned int i = 0; i < firstC.size(); i++) {
      firstC[i] = toupper(firstC[i]);
      secondC[i] = toupper(secondC[i]);
      if ((unsigned char)firstC[i] < (unsigned char)secondC[i])
        return true;
      else if ((unsigned char)firstC[i] > (unsigned char)secondC[i])
        return false;
    }
    
    return false;
  }


}
