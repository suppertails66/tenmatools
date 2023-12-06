#ifndef DISCASTERISOFILESYSTEM_H
#define DISCASTERISOFILESYSTEM_H


#include "discaster/Object.h"
#include <string>

namespace Discaster {

  
  class IsoFilesystem : public BlobObject {
  public:
    IsoFilesystem();
  
    /**
     * Given a path identifying an existing file or subdirectory of self's
     * file listing, return the corresponding object.
     * All components of the path must exist (i.e. this is an existing
     * file or directory).
     *
     * @return Pointer to the target Object.
     */
    static Object* getDirectoryListingObjectFromPath(
        Object* env, Object* self, const std::string& path);
  
    /**
     * Given a path identifying a nonexistent file or subdirectory of self's
     * file listing, add the given Object to the hierarchy in the appropriate
     * position.
     * Any necessary subdirectories will be created.
     *
     * @return Pointer to the newly-added Object.
     */
    static Object* addDirectoryListingObjectToPath(
        Object* env, Object* self, const std::string& path, Object* newObj);
    
    /**
     * Adds self/parent reference dummy "files" to directory listing.
     */
    static void addSelfAndParentPointers(
        Object* env, Object* self);
  };


}


#endif
