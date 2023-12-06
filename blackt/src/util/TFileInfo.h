#ifndef TFILEINFO_H
#define TFILEINFO_H


#include <string>
#include <ctime>

namespace BlackT {

  
  /**
   * Class representing file metadata.
   */
  class TFileInfo {
  public:
    static bool sortByIso9660Name(const TFileInfo& first,
                                  const TFileInfo& second);
  
    TFileInfo();
    
    std::string path() const;
    std::string name() const;
    unsigned int size() const;
    bool isDirectory() const;
    std::time_t accessedTime() const;
    std::time_t modifiedTime() const;
    std::time_t statusChangedTime() const;
    
    void setPath(std::string path__);
    void setName(std::string name__);
    void setSize(int size__);
    void setIsDirectory(bool isDirectory__);
    void setAccessedTime(std::time_t accessedTime__);
    void setModifiedTime(std::time_t modifiedTime__);
    void setStatusChangedTime(std::time_t statusChangedTime__);
  protected:
    std::string path_;
    std::string name_;
    unsigned int size_;
    bool isDirectory_;
    std::time_t accessedTime_;
    std::time_t modifiedTime_;
    std::time_t statusChangedTime_;
  };


}


#endif
