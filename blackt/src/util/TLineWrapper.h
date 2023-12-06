#ifndef TLINEWRAPPER_H
#define TLINEWRAPPER_H


#include "util/TStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace BlackT {


class TLineWrapper {
protected:
  struct Symbol {
    int id;
    bool raw;
    std::string litstr;
    
    std::string getContent(BlackT::TThingyTable& thingy) {
      if (raw) return litstr;
      
      return thingy.getEntry(id);
    }
  };
  
  enum BoxClearSource {
    boxClearManual,
    boxClearBoxFull
  };
  
  enum LinebreakSource {
    linebreakManual,
    linebreakLineFull,
    linebreakBoxEnd
  };

public:

  struct ResultString {
    std::string str;
  };
  
  typedef std::vector<ResultString> ResultCollection;

  TLineWrapper(BlackT::TStream& src__,
//                  BlackT::TStream& dst__,
                  ResultCollection& dst__,
                  const BlackT::TThingyTable& thingy__,
                  int xSize__ = -1,
                  int ySize__ = -1);
  
  virtual void operator()();
  
  /**
   * DEPRECATED -- DO NOT USE
   * Return width of a given symbol ID in "units" --
   * pixels, characters, whatever is compatible with the specified xSize.
   */
  virtual int widthOfKey(int key);
  
  /**
   * Return glyph width of a given symbol ID.
   */
  virtual int glyphWidthOfKey(int key);
  
  /**
   * Return advance width of a given symbol ID.
   */
  virtual int advanceWidthOfKey(int key);
  
  /**
   * Return true if a given symbol ID is considered a word boundary.
   * For English text, this will usually be whitespace characters.
   * Linebreaks can and should be included in this category.
   */
  virtual bool isWordDivider(int key) =0;
  
  /**
   * Return true if a given symbol ID is considered a visible word divider.
   * These are characters which divide words, but are not whitespace
   * (e.g. dashes, ellipses).
   * If an automatic line break occurs at one of these, it should appear
   * at the end of the line.
   * The symbol ID of the immediately following character is also passed
   * in for the callee to "peek" at; this is for disambiguating certain
   * cases where a symbol may or may not be a divider, such as an ellipsis
   * followed by an exclamation or question mark.
   */
  virtual bool isVisibleWordDivider(int key, int nextKey);
  
  /**
   * Return true if a given symbol ID constitutes a linebreak.
   * A linebreak is, by default, considered to do the following:
   *   - increment the yPos
   *   - reset the xPos to zero
   */
  virtual bool isLinebreak(int key) =0;
  
  /**
   * Return true if a given symbol ID constitutes a box clear.
   * A box clear is, by default, considered to do the following:
   *   - reset the xPos to zero
   *   - reset the yPos to zero
   */
  virtual bool isBoxClear(int key) =0;
  
  /**
   * This function is called immediately before the next word would normally
   * be output when the following conditions are met:
   *   a.) yPos == ySize, and
   *   b.) the current word's computed width will, when added to xPos, exceed
   *       xSize (necessitating a linebreak)
   * The implementation should handle this as appropriate for the target,
   * such as by outputting a wait/clear command, emitting an error, etc.
   */
  virtual void onBoxFull() =0;
  
  /**
   * Returns the key of the linebreak symbol.
   */
//  virtual int linebreakKey() =0;
  
  /**
   * Returns the string used for linebreaks.
   */
  virtual std::string linebreakString() const =0;
  
//  virtual int linebreakHeight() const;
  
  virtual void onSymbolAdded(BlackT::TStream& ifs, int key);
  virtual void afterSymbolAdded(BlackT::TStream& ifs, int key);
  virtual void onLineContentStarted();
  virtual void beforeLinebreak(LinebreakSource clearSrc,
                             int key);
  virtual void afterLinebreak(LinebreakSource clearSrc,
                             int key);
  virtual void beforeBoxClear(BoxClearSource clearSrc,
                             int key);
  virtual void afterBoxClear(BoxClearSource clearSrc,
                             int key);
  virtual void beforeWordFlushed();
  
protected:

  BlackT::TStream& src;
  ResultCollection& dst;
  BlackT::TThingyTable thingy;
  int lineNum;
  int xSize;
  int ySize;
  int xPos;
  int yPos;
  int currentWordWidth;
  int maxCurrentWordWidth;
  int pendingAdvanceWidth;
  bool nonDividerEncountered;
  bool lineHasContent;
  bool copyEverythingLiterally;
  int currentLineContentStartInBuffer;
  
  BlackT::TBufStream currentScriptBuffer;
  BlackT::TBufStream currentWordBuffer;
  
  const static int literalSize = 5;
  
  virtual TThingyTable::MatchResult getSymbolId(BlackT::TStream& ifs);
  virtual void outputNextSymbol(BlackT::TStream& ifs);
  virtual Symbol fetchNextSymbol(BlackT::TStream& ifs);
  virtual void handleManualLinebreak(TLineWrapper::Symbol result, int key);
  virtual void outputLinebreak();
  virtual void outputLinebreak(std::string str);
  
  virtual void flushCurrentPreLiterals();
  virtual void stripCurrentPreDividers();
  
  virtual int fromLitString(const std::string& litstr);
  
//  bool checkSymbol(BlackT::TStream& ifs, std::string& symbol);
  
  virtual void flushActiveScript();
  virtual void flushActiveWord();
  
  virtual bool processUserDirective(BlackT::TStream& ifs);
  void processDirective(BlackT::TStream& ifs);
  void processLoadTable(BlackT::TStream& ifs);
  void processEndMsg(BlackT::TStream& ifs);
//  void processIncBin(BlackT::TStream& ifs);
  void processSetWidth(BlackT::TStream& ifs);
  void processSetHeight(BlackT::TStream& ifs);
  void processSetSize(BlackT::TStream& ifs);
  
  void loadThingy(const BlackT::TThingyTable& thingy__);
  
  std::string streamAsString(TStream& stream);
  
};


}


#endif
