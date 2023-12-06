#include "util/TFileManip.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <stack>
#include <iostream>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <dirent.h>
  #include <sys/stat.h>
#endif

namespace BlackT {


unsigned int TFileManip::fileExists(const std::string& filename) {
  std::ifstream ifs(filename,
                    std::ios_base::binary);
                    
  if ((int)(getFileSize(ifs)) <= 0) {
    return false;
  }
  
  return true;
}

unsigned int TFileManip::getFileSize(std::istream& ifs) {
  std::ifstream::pos_type pos = ifs.tellg();
  ifs.seekg(0, std::ios_base::end);
  std::ifstream::pos_type size = ifs.tellg();
  ifs.seekg(pos);
  
  return size;
}
  
void TFileManip::readEntireFile(TArray<TByte>& dst,
                           const std::string& filename) {
  std::ifstream ifs(filename, std::ios_base::binary);
  readEntireStream(dst, ifs);
}
  
void TFileManip::readEntireFile(TArray<TByte>& dst,
                           const char* filename) {
  readEntireFile(dst, std::string(filename));
}
  
void TFileManip::readEntireStream(TArray<TByte>& dst,
                            std::istream& ifs) {
  int sz = getFileSize(ifs);
//  ifs.seekg(0, std::ios_base::beg);
  dst.resize(sz);
  ifs.read((char*)(dst.data()), sz);
}

std::string TFileManip::getDirectoryName(std::string filepath) {
  for (int i = filepath.size() - 1; i >= 0; i--) {
    if (filepath[i] == '/') {
      return filepath.substr(0, i);
    }
  }
  
  return std::string();
}

std::string TFileManip::getFilename(std::string filepath) {
  for (int i = filepath.size() - 1; i >= 0; i--) {
    if (filepath[i] == '/') {
      return filepath.substr(i + 1, filepath.size() - (i + 1));
    }
  }
  
//  return std::string();
  return filepath;
}

void TFileManip::splitPath(std::string filepath, NameCollection& dst) {
  if (filepath.size() == 0) return;
  
  // removing initial/trailing slashes
  while ((filepath.size() > 0) && (filepath[0] == '/'))
    filepath = filepath.substr(1, std::string::npos);
  while ((filepath.size() > 0) && (filepath[filepath.size() - 1] == '/'))
    filepath = filepath.substr(0, filepath.size() - 1);

  std::stack<std::string> components;
  while (filepath.size() > 0) {
    // inefficient but good enough for now
    components.push(getFilename(filepath));
    filepath = getDirectoryName(filepath);
  }
  
  while (!components.empty()) {
    dst.push_back(components.top());
    components.pop();
  }
}

/*#ifdef _WIN32
#else
  void TFileManip::getFileListing(const char* dirname,
                         NameCollection& dst) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) return;
    
    dirent* dirent;
    while ((dirent = readdir(dir)) != NULL) {
      struct stat buf;
      stat((std::string(dirname) + "/" + dirent->d_name).c_str(), &buf);
      if (S_ISREG(buf.st_mode)) {
        dst.push_back(std::string(dirent->d_name));
      }
    }
    
    closedir(dir);
  }
#endif
  
#ifdef _WIN32
#else
  void TFileManip::getSubFolderListing(const char* dirname,
                                  NameCollection& dst) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) return;
    
    dirent* dirent;
    while ((dirent = readdir(dir)) != NULL) {
      struct stat buf;
      stat((std::string(dirname) + "/" + dirent->d_name).c_str(), &buf);
      if (S_ISDIR(buf.st_mode)) {
        // don't return current/previous directory entries
        if ((strcmp(dirent->d_name, ".") != 0)
            && (strcmp(dirent->d_name, "..") != 0)) {
          dst.push_back(std::string(dirent->d_name));
        }
      }
    }
    
    closedir(dir);
  }
#endif */

#ifdef _WIN32
#else
  TFileInfo TFileManip::getFileInfo(const std::string& filename) {
/*    if (!fileExists(filename)) {
      throw TGenericException(T_SRCANDLINE,
                              "TFileManip::getFileInfo(const std::string&)",
                              std::string("File does not exist: ")
                                + filename);
    } */
    
    
    struct stat buf;
    int fileOpenResult = stat(filename.c_str(), &buf);
    
    if (fileOpenResult != 0) {
      throw TGenericException(T_SRCANDLINE,
                              "TFileManip::getFileInfo(const std::string&)",
                              std::string("Error opening file ")
                              + "(does not exist?): "
                                + filename);
    }
    
    TFileInfo result;
    
    // path to file from current directory
    result.setPath(filename);
    // basename of file/folder
    result.setName(getFilename(filename));
    
    // check if file or directory
    if (S_ISDIR(buf.st_mode)) {
      result.setIsDirectory(true);
      result.setSize(0);
    }
    else {
      result.setIsDirectory(false);
      result.setSize(buf.st_size);
    }
    
    result.setAccessedTime(buf.st_atime);
    result.setModifiedTime(buf.st_mtime);
    result.setStatusChangedTime(buf.st_ctime);
    
    return result;
  }
#endif

#ifdef _WIN32
#else
  void TFileManip::getDirectoryListing(const char* dirname,
                                  FileInfoCollection& dst) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) return;
    
    std::string dirBase = std::string(dirname);
    
//    if (dirBase.size() > 0) dirBase += "/";
    // make some token effort to detect trailing slashes
    while ((dirBase.size() > 0)
        && (dirBase[dirBase.size() - 1] == '/'))
      dirBase = dirBase.substr(0, dirBase.size() - 1);
    
    dirBase += "/";
    
    // check for special cases "." and ".."
//    if (dirBase.compare(".") == 0) dirBase = std::string("./");
//    else if (dirBase.compare("..") == 0) dirBase = std::string("../");
    
    // remove any initial "./" sequence; we can't open files through the
    // standard library if they have one
    // (lies)
/*    while (((dirBase.size() >= 2)
              && (dirBase[0] == '.')
              && (dirBase[1] != '.'))) {
      dirBase = dirBase.substr(2, std::string::npos);
    }
    
    // add a slash if not targeting current directory
    if (dirBase.size() > 0) dirBase += '/';
    
//    std::cerr << "dirBase: " << dirBase << std::endl; */
    
    dirent* dirent;
    while ((dirent = readdir(dir)) != NULL) {
      std::string filename = dirBase + dirent->d_name;
      
      {
        struct stat buf;
        stat(filename.c_str(), &buf);
        // ignore current/previous directory entries
        if (S_ISDIR(buf.st_mode)
            && ((strcmp(dirent->d_name, ".") == 0)
                || (strcmp(dirent->d_name, "..") == 0))) {
          continue;
        }
        
        // ignore everything but regular files and directories
        if (!(S_ISREG(buf.st_mode) || S_ISDIR(buf.st_mode))) continue;
      }
      
      TFileInfo info = getFileInfo(filename.c_str());
      dst.push_back(info);
    }
  }
#endif

void TFileManip::createDirectory(std::string name) {
  if (name.size() != 0) {
    // first, create every parent directory, since mkdir
    // will not create subdirectories
    createDirectory(getDirectoryName(name));
    
    #ifdef _WIN32
	    CreateDirectoryA(name.c_str(), NULL);
    #else
      mkdir(name.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
    #endif
  }
}

void TFileManip::createDirectoryForFile(std::string name) {
  createDirectory(getDirectoryName(name));
}


};
