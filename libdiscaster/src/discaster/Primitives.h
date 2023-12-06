#ifndef DISCASTERPRIMITIVES_H
#define DISCASTERPRIMITIVES_H


#include <string>

namespace Discaster {


  /**
   * Primitive int type.
   * This is a long mostly to make sure we can contain 64-bit representations
   * of Unix time in an Object.
   */
  typedef long int DiscasterInt;

  /**
   * Primitive string type.
   */
  typedef std::string DiscasterString;


}


#endif
