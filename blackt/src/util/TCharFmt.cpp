#include "util/TCharFmt.h"
#include "utf8.h"

namespace BlackT {


void TCharFmt::utf8To16(const std::string& src,
                 TUtf16Chars& dst) {
  utf8::utf8to16(src.begin(), src.end(), back_inserter(dst));
}


void TCharFmt::utf8To32(const std::string& src,
                 TUtf32Chars& dst) {
  utf8::utf8to32(src.begin(), src.end(), back_inserter(dst));
}


}
