#ifndef TTHINGYTABLE_H
#define TTHINGYTABLE_H


#include "util/TStream.h"
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

namespace BlackT {


struct TThingyTable {   
public:
  typedef std::string TableEntry;
  
  struct TableEntryInfo {
    TableEntry entry;
    int id;
    int byteCount;
  };
  
  typedef std::unordered_map<int, TableEntryInfo> RawTable;
  typedef std::unordered_map<int, int> ReverseMap;
  typedef std::vector<TableEntryInfo> LengthCollection;
  typedef std::pair<int, int> IdLengthPair;
  typedef std::pair<std::string, int> TableEntryLengthPair;

  TThingyTable();
  TThingyTable(std::string filename);
  
  void readGeneric(std::string filename);
  void readSjis(std::string filename);
  void readUtf8(std::string filename);
  
  std::string getEntry(int charID) const;
  
  int getRevEntry(int charID) const;
  
  bool hasEntry(int charID) const;
  
  struct MatchResult {
    int id;
    int size;
  };
  
  MatchResult matchTableEntry(TableEntry entry) const;
  MatchResult matchTableEntry(TStream& ifs) const;
  MatchResult matchId(TStream& ifs, int bytesPerChar = -1) const;
  
  TThingyTable::TableEntryInfo tryMatchId(TStream& ifs, int bytesPerChar) const;
  
  RawTable entries;
  ReverseMap revEntries;
  LengthCollection entriesByLength;
  LengthCollection entriesByIdLength;

  void generateEntriesByLength();
  
  static bool sortByLength(const TableEntryInfo& first,
                           const TableEntryInfo& second);
  
  static bool sortByIdLength(const TableEntryInfo& first,
                             const TableEntryInfo& second);

protected:

  struct EntrySuffixMapEntry;
  
  typedef std::unordered_map<char, EntrySuffixMapEntry>
    EntrySuffixMap;
  
  struct EntrySuffixMapEntry {
    EntrySuffixMapEntry();
    ~EntrySuffixMapEntry();
    EntrySuffixMapEntry(const EntrySuffixMapEntry& other);
    
    EntrySuffixMap& suffixes();
    const EntrySuffixMap& suffixes() const;
    
    bool hasInfo;
    TableEntryInfo info;
    
    EntrySuffixMap* suffixes_;
  };

  void generateEntriesBySuffix();

  void addEntryBySuffix(EntrySuffixMap& dst, TableEntry entry,
      TableEntryInfo& info);

  MatchResult matchEntryBySuffix(TStream& ifs) const;
  MatchResult matchEntryBySuffix(TableEntry entry) const;
  
  // This is a map which organizes the values of the table
  // (those on the right-hand side of the "=") in the following manner:
  // 
  // An entry consisting of one byte has that byte placed as a key in this map,
  // with its corresponding TableEntryInfo placed in the EntrySuffixMapEntry
  // that serves as the value. The value's "hasInfo" field is set to true,
  // indicating that this is an entry with content.
  // 
  // An entry consisting of two bytes first creates an entry in this map
  // (if it does not exist) using its first byte as the key. "hasInfo"
  // is not set to true (though if such an entry exists, it may already be
  // or become true). Then, the second byte is used as a key in that
  // entry's "suffixes" table, with an EntrySuffixMapEntry placed as the
  // value.
  //
  // This pattern further generalizes to larger byte sizes.
  // Using this structure, when we need to match the longest matching entry
  // from an input stream, we simply take the first byte and check it against
  // the keys of this map. If it matches, and its hasInfo field is set, then
  // that becomes our tentative match. Regardless of whether hasInfo was set,
  // we then fetch another byte from the input and use it as a key into the
  // "suffixes" field of the match. If it has a matching key and hasInfo is set,
  // then that becomes the new tentative match. This repeats until we do not
  // find a matching key in "suffixes", at which point we return the last
  // tentative match. (If no tentative match is ever found, the entry is not
  // in the table.)
  EntrySuffixMap entriesBySuffix;
  
};


}


#endif 
