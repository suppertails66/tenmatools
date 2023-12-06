#ifndef DISCASTERFILELISTING_H
#define DISCASTERFILELISTING_H


#include "discaster/Object.h"
#include "discaster/CdConsts.h"
#include "discaster/ParseNode.h"
#include "util/TStream.h"
#include <string>

namespace Discaster {


  class FileListing : public BlobObject {
  public:
    static bool sortByIso9660Name(const Object& first,
                                  const Object& second);
    static bool sortByIso9660NameP(const Object* first,
                                  const Object* second);
    
    FileListing();
    FileListing(std::string name, bool isDirectory);
  
    static void writeDirectoryRecord(
        const Object* env, const Object* self, BlackT::TStream& dst,
        CdFormatIds::CdFormatId cdFormat);
    
    /**
     * Returns an Object whose members consists of FileListings identifying
     * contained files.
     */
    static Object* getDirectoryMap(Object* env, Object* self);
    
    static Object* addToDirectoryMap(Object* env, Object* self,
                                     std::string name, Object* newObj);
    
    static bool hasFile(Object* env, Object* self,
                        std::string name);
    
    static Object* getFile(Object* env, Object* self,
                        std::string name);
    
    static Object* addFile(Object* env, Object* self,
                           std::string name, Object& newFile);
    
    /**
     * Given a path identifying an existing file or subdirectory of "self",
     * return the corresponding object.
     * All components of the path must exist (i.e. this is an existing
     * file or directory).
     *
     * @return Pointer to the target Object.
     */
    static Object* getObjectFromPath(
        Object* env, Object* self, const std::string& path);
  
    /**
     * Given a path identifying a nonexistent file or subdirectory of "self",
     * add the given Object to the hierarchy in the appropriate position.
     * Any necessary subdirectories will be created.
     *
     * @return Pointer to the newly-added Object.
     */
    static Object* addObjectToPath(
        Object* env, Object* self, const std::string& path, Object* newObj);
  protected:
    void defaultInit();
  };


}


#endif
