#ifndef TFILEMANIP_H
#define TFILEMANIP_H


#include "util/TByte.h"
#include "util/TArray.h"
#include "util/TFileInfo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace BlackT {


class TFileManip {
public:
  typedef std::vector<std::string> NameCollection;
  typedef std::vector<TFileInfo> FileInfoCollection;
  
  static unsigned int fileExists(const std::string& filename);
  
  static unsigned int getFileSize(std::istream& ifs);
  
  static void readEntireFile(TArray<TByte>& dst,
                             const std::string& filename);
  
  static void readEntireFile(TArray<TByte>& dst,
                             const char* filename);
  
  static void readEntireStream(TArray<TByte>& dst,
                              std::istream& ifs);
  
  // TODO: not yet implemented for windows
  static TFileInfo getFileInfo(const std::string& filename);
  
  //================================================
  // moved from deprecated TFolderManip
  //================================================
  
  /**
   * Given a file path, returns the directory portion of the name.
   * For instance, "folder/subfolder/file.bin" will return "folder/subfolder".
   * Note that if the path identifies a directory, this will return the parent
   * of that directory, i.e. "folder/subfolder/subsubfolder" will still return
   * "folder/subfolder".
   * If no containing directory exists, returns an empty string.
   */
  static std::string getDirectoryName(std::string filepath);

  /**
   * Given a path, returns the file portion of the name.
   * For instance, "folder/subfolder/file.bin" will return "file.bin".
   * The same caveats as with getDirectoryName() apply.
   */
  static std::string getFilename(std::string filepath);
  
  /**
   * Given a file path, splits it into its slash-separated components.
   * "folder/file.bin" = "folder", "file.bin"
   * "/folder/file.bin" = "folder", "file.bin"
   * "./folder/file.bin" = ".", "folder", "file.bin"
   * etc.
   */
  static void splitPath(std::string filepath, NameCollection& dst);
  
  /**
   * Fills dst with a list of all files (not directories) in directory dirname.
   * TODO: windows
   */
//  static void getFileListing(const char* dirname,
//                       NameCollection& dst);
  
  /**
   * Fills dst with a list of all subdirectories in directory dirname.
   * TODO: windows
   */
//  static void getSubFolderListing(const char* dirname,
//                                NameCollection& dst);
  
  /**
   * Fills dst with information on all files and subdirectories in directory
   * dirname.
   * TODO: windows
   */
  static void getDirectoryListing(const char* dirname,
                                  FileInfoCollection& dst);
  
  /**
   * Creates a directory of the corresponding path, if possible.
   * This acts recursively: if necessary, multiple hierarchical directories
   * will be created. 
   */
  static void createDirectory(std::string name);

  /**
   * Given a path to a file which does not yet exist, creates the necessary
   * directory or directories needed to hold it (if possible).
   * Acts recursively.
   */
  static void createDirectoryForFile(std::string name);
  
protected:
  
};


};


#endif
