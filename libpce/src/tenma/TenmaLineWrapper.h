#ifndef TENMALINEWRAPPER_H
#define TENMALINEWRAPPER_H


#include "util/TStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include "util/TLineWrapper.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace Pce {


class TenmaLineWrapper : public BlackT::TLineWrapper {
public:
  typedef std::map<int, int> CharSizeTable;

  TenmaLineWrapper(BlackT::TStream& src__,
                  ResultCollection& dst__,
                  const BlackT::TThingyTable& thingy__,
                  CharSizeTable sizeTable__,
                  CharSizeTable sizeTableEmph__ = CharSizeTable(),
                  bool isNonstd = false,
                  int xSize__ = -1,
                  int ySize__ = -1,
                  int controlOpsStart__ = default_controlOpsStart);
  
  /**
   * Return width of a given symbol ID in "units" --
   * pixels, characters, whatever is compatible with the specified xSize.
   */
  virtual int widthOfKey(int key);
  
  virtual int advanceWidthOfKey(int key);
  
  /**
   * Return true if a given symbol ID is considered a word boundary.
   * For English text, this will usually be whitespace characters.
   * Linebreaks can and should be included in this category.
   */
  virtual bool isWordDivider(int key);
  
  virtual bool isVisibleWordDivider(int key, int nextKey);
  
  /**
   * Return true if a given symbol ID constitutes a linebreak.
   * A linebreak is, by default, considered to do the following:
   *   - increment the yPos
   *   - reset the xPos to zero
   */
  virtual bool isLinebreak(int key);
  
  /**
   * Return true if a given symbol ID constitutes a box clear.
   * A box clear is, by default, considered to do the following:
   *   - reset the xPos to zero
   *   - reset the yPos to zero
   */
  virtual bool isBoxClear(int key);
  
  /**
   * This function is called immediately before the next word would normally
   * be output when the following conditions are met:
   *   a.) yPos == ySize, and
   *   b.) the current word's computed width will, when added to xPos, exceed
   *       xSize (necessitating a linebreak)
   * The implementation should handle this as appropriate for the target,
   * such as by outputting a wait/clear command, emitting an error, etc.
   */
  virtual void onBoxFull();
  
  /**
   * Returns the string used for linebreaks symbol.
   */
  virtual std::string linebreakString() const;
  
//  virtual int linebreakHeight() const;
  virtual void handleManualLinebreak(TLineWrapper::Symbol result, int key);
  
  virtual void onSymbolAdded(BlackT::TStream& ifs, int key);
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
  virtual char literalOpenSymbol() const;
  virtual char literalCloseSymbol() const;
  
protected:

//  enum ClearMode {
//    clearMode_default,
//    clearMode_messageSplit
//  };

  const static char myLiteralOpenSymbol = '<';
  const static char myLiteralCloseSymbol = '>';
//  const static int default_controlOpsStart = 0xEF;
  const static int default_controlOpsStart = 0xE8;
  
  int controlOpsStart;

  enum BreakMode {
    breakMode_single,
    breakMode_double
  };

//  bool waitPending;
  CharSizeTable sizeTable;
  CharSizeTable sizeTableEmph;
  bool isNonstd;
  int xBeforeWait;
//  ClearMode clearMode;
  BreakMode breakMode;
  bool failOnBoxOverflow;
  bool doubleWidthModeOn;
  bool doubleWidthCharsInWord;
  bool padAndCenter_on;
  int padAndCenter_leftPad;
  int padAndCenter_width;
  // list of starting positions in current script buffer of each line
  std::vector<int> lineContentStartOffsets;
  int longestLineLen;
  bool emphModeOn;
  bool br4ModeOn;
  
  int getLocalBrId() const;
  int getLocalSpaceId() const;
//  int getLocalEspId() const;
  
  virtual bool processUserDirective(BlackT::TStream& ifs);
  
  std::string getPadString(int width);
  void doLineEndPadChecks();
  void applyPadAndCenterToCurrentLine();
  void applyPadAndCenterToCurrentBlock();
};


}


#endif
