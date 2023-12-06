#ifndef TSTRINGSEARCH_H
#define TSTRINGSEARCH_H


#include "util/TStream.h"
#include "util/TByte.h"
#include <string>
#include <vector>

namespace BlackT {


struct TStringSearchResult {
  int offset;
};

typedef std::vector<TStringSearchResult> TStringSearchResultList;

struct TStringSearchToken {
public:
  enum Type {
    type_literal,
    type_star
  };
  
  Type type;
  TByte value;
  
  bool operator==(const TStringSearchToken& other) const;
  bool operator!=(const TStringSearchToken& other) const;
};

typedef std::vector<TStringSearchToken> TStringSearchTokenList;

class TStringSearch {
public:
  // returns a list of matches for the specified search string,
  // starting at the current position in ifs
  static TStringSearchResultList search(TStream& ifs, std::string searchStr);
  
  static TStringSearchResult searchForUnique(
    TStream& ifs, std::string searchStr);
  
  static TStringSearchResultList searchFullStream(
    TStream& ifs, std::string searchStr);
  
  static TStringSearchResult searchFullStreamForUnique(
    TStream& ifs, std::string searchStr);
  
  // returns true if the content at the current position in ifs
  // matches the specified search string
  static bool checkString(
    TStream& ifs, std::string searchStr);
  
  // returns true if either of the given search strings is a
  // "matching subset" of the other (i.e. if a search for the
  // shorter string will also match any instance of the longer)
  static bool areMatchingSubsets(
    std::string searchStrA, std::string searchStrB);
  
protected:
  static TStringSearchTokenList getSearchList(std::string searchStr);
  
  static bool check(TStream& ifs, const TStringSearchTokenList& tokenList);
  
};


};


#endif
