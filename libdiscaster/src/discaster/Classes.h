#ifndef DISCASTERCLASSES_H
#define DISCASTERCLASSES_H


#include <cstdlib>

namespace Discaster {


  //=====================================
  // built-in DiscasterScript classes
  //=====================================
  
  class Object;
  
  typedef Object (*ClassConstructor)();
  
  /**
   * Struct to contain entries in master class table.
   */
  struct MasterClassTableEntry {
    const char* name;
    const ClassConstructor constructor;
  };
  
  /**
   * Container for master table of classes.
   */
  class MasterClassTable {
  public:
    
    const static int numEntries();
    const static MasterClassTableEntry& entry(int index);
  
  protected:
    const static MasterClassTableEntry masterClassTable[];
  };


}


#endif
