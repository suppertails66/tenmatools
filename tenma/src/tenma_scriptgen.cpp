#include "tenma/TenmaScriptGenStream.h"
#include "tenma/TenmaTranslationSheet.h"
#include "tenma/TenmaMsgConsts.h"
#include "tenma/TenmaMapIndex.h"
#include "pce/PcePalette.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TSoundFile.h"
#include "util/TFileManip.h"
#include "util/TFreeSpace.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace BlackT;
using namespace Pce;

TThingyTable tableStd;
TThingyTable tableStdWithNull;
TThingyTable tableFull;
TThingyTable tableCredits;

std::string getNumStr(int num) {
  std::string str = TStringConversion::intToString(num);
  while (str.size() < 2) str = string("0") + str;
  return str;
}

std::string getHexByteNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  return string("$") + str;
}

std::string getHexWordNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 4) str = string("0") + str;
  return string("$") + str;
}

void addGenericString(TStream& src, int offset, TenmaScriptGenStream& dst,
                    TThingyTable& table,
                    std::string name = "") {
  src.seek(offset);
  dst.addGenericString(src, table, name);
}

void addGenericSeqStrings(TStream& src, int offset, int count,
                    TenmaScriptGenStream& dst,
                    TThingyTable& table,
                    std::string name = "") {
  src.seek(offset);
  for (int i = 0; i < count; i++) {
//    std::cerr << i << std::endl;
    dst.addGenericString(src, table, name);
  }
}

void addScriptString(TStream& src, int offset, TenmaScriptGenStream& dst,
                    TThingyTable& table,
                    std::string name = "",
                    TenmaMsgDism::MsgTypeMode mode
                      = TenmaMsgDism::msgTypeMode_noCheck) {
  src.seek(offset);
  dst.addScriptString(src, table, name, mode);
}

void addScriptString_directRedirect(
                    TStream& src, int offset, TenmaScriptGenStream& dst,
                    TThingyTable& table,
                    int directRedirectPtrBase,
                    int directRedirectPtrOffset,
                    std::string name = "",
                    TenmaMsgDism::MsgTypeMode mode
                      = TenmaMsgDism::msgTypeMode_noCheck) {
  src.seek(offset);
  int msgId = (unsigned char)src.peek();
  dst.addScriptString(src, table, name, mode);
  // HACK: make sure string's space isn't auto-freed
  dst.entries.back().offset = -1;
  dst.entries.back().size = -1;
  // set up direct redirect properties
  dst.entries.back().propertyMap["isDirectRedirect"] = "1";
  dst.entries.back().propertyMap["directRedirect_ptrBase"]
    = TStringConversion::intToString(directRedirectPtrBase,
        TStringConversion::baseHex);
  dst.entries.back().propertyMap["directRedirect_ptrOffset"]
    = TStringConversion::intToString(directRedirectPtrOffset,
        TStringConversion::baseHex);
  dst.entries.back().propertyMap["directRedirect_msgId"]
    = TStringConversion::intToString(msgId,
        TStringConversion::baseHex);
}

void addScriptSeqStrings(TStream& src, int offset, int count,
                    TenmaScriptGenStream& dst,
                    TThingyTable& table,
                    std::string name = "",
                    TenmaMsgDism::MsgTypeMode mode
                      = TenmaMsgDism::msgTypeMode_noCheck) {
  src.seek(offset);
  for (int i = 0; i < count; i++) {
//    std::cerr << i << std::endl;
    dst.addScriptString(src, table, name, mode);
  }
}

void addMenu(TStream& src, int offset, TenmaScriptGenStream& dst,
                    TThingyTable& table,
                    std::string name = "",
                    TenmaMsgDism::MenuTypeMode mode
                      = TenmaMsgDism::menuTypeMode_noParams) {
  src.seek(offset);
  dst.addMenu(src, table, name, mode);
}

void addSeqMenus(TStream& src, int offset, int count,
                    TenmaScriptGenStream& dst,
                    TThingyTable& table,
                    std::string name = "",
                    TenmaMsgDism::MenuTypeMode mode
                      = TenmaMsgDism::menuTypeMode_noParams) {
  src.seek(offset);
  for (int i = 0; i < count; i++) {
//    std::cerr << i << std::endl;
    dst.addMenu(src, table, name, mode);
  }
}

void addScriptPointerTableSet(TStream& src, int offset, int count,
                    int ptrBase,
                    TenmaScriptGenStream& dst,
                    TThingyTable& table,
                    std::string name = "",
                    TenmaMsgDism::MsgTypeMode mode
                      = TenmaMsgDism::msgTypeMode_noCheck) {
  src.seek(offset);
  std::map<int, int> usedPtrs;
  for (int i = 0; i < count; i++) {
//    std::cerr << i << std::endl;
    
    int offsetFromBase = src.tell() - offset;
    
    int tableStart = src.tell();
    int firstPtr = src.readu16le();
    int firstEntryRawOffset = (firstPtr - ptrBase) - offsetFromBase;
//    int firstEntryRealOffset = firstEntryRawOffset - tableStart;
    int tableSize = firstEntryRawOffset / 2;
    
//    std::cerr << std::hex << tableStart << " " << firstEntryRawOffset << std::endl;
    
    std::map<int, int> temp;
    src.seek(tableStart);
    for (int i = 0; i < tableSize; i++) {
      int nextPtr = src.readu16le();
      temp[nextPtr] = nextPtr;
    }
    
    for (auto& item: temp) {
      // ignore if already dumped
      int ptr = item.second;
      if (usedPtrs.find(ptr) != usedPtrs.end()) continue;
      usedPtrs[ptr] = ptr;
      
      int target = (item.second - ptrBase) + offset;
      src.seek(target);
//      std::cerr << std::hex << target << std::endl;
      dst.addScriptString(src, table, name, mode);
    }
    
//    std::cerr << std::hex << tableStart << " " << firstEntryRealOffset << " " << tableSize << std::endl;
//    addScriptSeqStrings(src, tableStart + (tableSize * 2), tableSize,
//                        dst, table, name, mode);
  }
}

void dumpCredits(TStream& ifs, TenmaScriptGenStream& dst,
                 TThingyTable& table,
                 std::ostream& asmOfs) {
  bool done = false;
  while (!done) {
    int cmd = ifs.readu8();
    
//    std::cerr << std::hex << ifs.tell() - 1 << ": " << std::hex << cmd << std::endl;
    
    if (cmd == 0xFF) {
      asmOfs << "  .db " << getHexByteNumStr(cmd) << std::endl;
      done = true;
      break;
    }
    
    int type = cmd & 0xC0;
    if (type == 0xC0) {
      // enables "tall sprite mode", which doesn't seem to be used
      asmOfs << "  .db " << getHexByteNumStr(cmd) << std::endl;
      done = true;
      break;
    }
    else if (type == 0x80) {
//      ifs.seekoff(2);
      int dispTime = (cmd & 0x3F);
      int xRaw = ifs.readu8();
      int yRaw = ifs.readu8();
      int xReal = xRaw * 8;
      int yReal = yRaw * 8;
      
      std::string strName = std::string("str-")
        + TStringConversion::intToString(
            ifs.tell(), TStringConversion::baseHex);
      dst.addGenericString(ifs, table, strName, true);
      asmOfs << "  makeCreditsString "
        << dispTime
        << ", " << getHexByteNumStr(xReal)
        << ", " << getHexByteNumStr(yReal)
        << ", \"out/script/credits/" << strName << ".bin\"" << std::endl;
      
//      const TenmaScriptGenStreamEntry& strEntry = dst.entries.back();
    }
    else if (type == 0x40) {
//      ifs.seekoff(2);
      int dispTime = (cmd & 0x3F);
      int xRaw = ifs.readu8();
      int yRaw = ifs.readu8();
      int xReal = xRaw * 8;
      int yReal = yRaw * 8;
      
      int initialPos = ifs.tell();
      
      std::string strName = std::string("str-")
        + TStringConversion::intToString(
            ifs.tell(), TStringConversion::baseHex);
      dst.addGenericString(ifs, table, strName, false);
      asmOfs << "  makeCreditsString "
        << dispTime
        << ", " << getHexByteNumStr(xReal)
        << ", " << getHexByteNumStr(yReal)
        << ", \"out/script/credits/" << strName << ".bin\"" << std::endl;
      
      // HACK: output raw string
      int finalPos = ifs.tell();
      ifs.seek(initialPos);
      {
        TBufStream ofs;
        while (!ifs.eof()) {
          unsigned char next = ifs.get();
          ofs.put(next);
          if (next == 0xFF) break;
        }
        std::string fileName
          = (std::string("rsrc_raw/credits/") + strName + ".bin");
        TFileManip::createDirectoryForFile(fileName);
        ofs.save(fileName.c_str());
      }
      ifs.seek(finalPos);
    }
    else {
      // wait time in seconds
      asmOfs << "  addCreditsDelay " << cmd << std::endl << std::endl;
    }
    
/*    switch (cmd) {
    case 0x02:
    case 0x04:
    case 0x05:
      for (int i = 0; i < 2; i++) {
        ifs.seekoff(3);
        dst.addScriptString(ifs, table, "", TenmaMsgDism::msgTypeMode_noCheck);
      }
      break;
    case 0x09:
//      done = true;
      break;
    case 0xFF:
      done = true;
      break;
    default:
      std::cerr << "unknown credits cmd at "
        << std::hex << ifs.tell() - 1 << ": "
        << std::hex << cmd << std::endl;
      done = true;
      throw TGenericException(T_SRCANDLINE,
                              "dumpCredits()",
                              std::string("unknown credits cmd: ")
                              + TStringConversion::intToString(cmd,
                                  TStringConversion::baseHex));
      break;
    }*/
  }
}

void seekNextTerminator(TStream& src) {
  while ((unsigned char)src.peek() != 0xFF) src.get();
  src.get();
}



int main(int argc, char* argv[]) {
  TBufStream isoIfs;
  isoIfs.open("tenma_02.iso");
  
  TFileManip::createDirectory("script/orig");
  
  TenmaMapIndex mapIndex(isoIfs);
/*  std::vector<int> mapSectorIndex = TenmaMsgConsts::getMapSectorIndex();
//  for (std::vector<int>::iterator it = mapSectorIndex.begin();
//       it != mapSectorIndex.end();
//       ++it) {
  for (int i = 0; i < mapSectorIndex.size(); i++) {
//  for (int i = 0; i < 5; i++) {
    int mapBaseSector = mapSectorIndex[i];
    std::cout << std::hex << mapBaseSector
      << " " << mapIndex.computeMapSize(isoIfs, mapBaseSector) << std::endl;
  }
  return 0; */
  
  tableStd.readUtf8("table/sjis_utf8_tenma_std.tbl");
  tableStdWithNull.readUtf8("table/sjis_utf8_tenma_std_with_null.tbl");
  tableFull.readUtf8("table/sjis_utf8_tenma_full.tbl");
  tableCredits.readUtf8("table/sjis_utf8_tenma_credits.tbl");
  
  TenmaTranslationSheet sheetMaps;
  TenmaTranslationSheet sheetItems;
  TenmaTranslationSheet sheetShop;
  TenmaTranslationSheet sheetEnemy;
  TenmaTranslationSheet sheetBattle;
  TenmaTranslationSheet sheetIntro;
  TenmaTranslationSheet sheetSaveLoad;
  TenmaTranslationSheet sheetGenericText;
  TenmaTranslationSheet sheetCredits;
  TenmaTranslationSheet sheetInfo;
  TenmaTranslationSheet sheetMenu;
  TenmaTranslationSheet sheetMisc;
  TenmaTranslationSheet sheetBoss;
  TenmaTranslationSheet sheetAltBoss;
  TenmaTranslationSheet sheetSubtitle;
  
  // TODO: what are these strange messages which appear in the blocks
  // starting around 0x55e844? i don't recall any of them appearing in
  // gameplay, and it's not obvious what the associated blocks are for.
  // a lot of them look like the boss intro/defeat messages for battles,
  // but those are in a completely different place...
  
  TBufStream kernelIfs;
  kernelIfs.open("base/kernel_9.bin");
  TBufStream overwIfs;
  overwIfs.open("base/overw_13.bin");
  TBufStream shopIfs;
  shopIfs.open("base/shop_19.bin");
  TBufStream battleExtraIfs;
  battleExtraIfs.open("base/battle_extra_27.bin");
  TBufStream battleTextIfs;
  battleTextIfs.open("base/battle_text_2D.bin");
  TBufStream battleText2Ifs;
  battleText2Ifs.open("base/battle_text2_31.bin");
  TBufStream introIfs;
  introIfs.open("base/intro_3B.bin");
  TBufStream saveLoadIfs;
  saveLoadIfs.open("base/saveload_45.bin");
  TBufStream genericTextIfs;
  genericTextIfs.open("base/generic_text_4B.bin");
  TBufStream creditsIfs;
  creditsIfs.open("base/credits_4F.bin");
  TBufStream specialIfs;
  specialIfs.open("base/special_51.bin");
  TBufStream infoIfs;
  infoIfs.open("base/info_5B.bin");
  
  //=============================================
  // maps
  //=============================================
  
  TenmaScriptGenStream streamMaps;
  streamMaps.addSetSize(216, 3);
  
  std::vector<int> mapSectorList = TenmaMsgConsts::getMapSectorIndex();
  for (int i = 0; i < mapSectorList.size(); i++) {
//  for (int i = 0; i < 5; i++) {
    int sectorNum = mapSectorList[i];
    
    int nextSectorNum = sectorNum + 0x1A;
    if (i != (mapSectorList.size() - 1)) {
      nextSectorNum = mapSectorList[i + 1];
    }
    
    bool isSmallMap = false;
    int mapSize = nextSectorNum - sectorNum;
    if (mapSize < 0x1A) {
      isSmallMap = true;
    }
    
    bool hasLargeSpacing = false;
    if (mapSize > 0x1C) {
      hasLargeSpacing = true;
    }
    
    std::cerr << std::dec << i
      << ": "
      << TStringConversion::intToString(mapSectorList[i],
          TStringConversion::baseHex)
      << (isSmallMap ? " (small)" : "")
      << (hasLargeSpacing ? " (large spacing)" : "")
      << std::endl;
    
    int flags = 0;
    if (isSmallMap) flags |= TenmaScriptGenStream::flag_isSmall;
    if (hasLargeSpacing) flags |= TenmaScriptGenStream::flag_hasLargeSpacing;
    
    streamMaps.addMapFromIsoSector(isoIfs, sectorNum, mapIndex, tableStd,
                                   flags);
  }
  
  //=============================================
  // item list
  //=============================================
  
  TenmaScriptGenStream streamItems;
  
  streamItems.addComment("item names (including technique scrolls)");
  addGenericSeqStrings(kernelIfs, 0x26D9, 212, streamItems, tableStd);
  
  //=============================================
  // shops (+ some special item messages)
  //=============================================
  
  TenmaScriptGenStream streamShop;
//  streamShop.addSetSize(216, 3);
  streamShop.addSetSize(-1, -1);
  
  streamShop.addComment("generic shop messages");
  // this could be a 3b header, but i don't know why there would be only one
  // for 10 sequential strings, and it shouldn't matter for the translation anyway
//  addScriptSeqStrings(shopIfs, 0x22D, 1, streamShop, tableStd);
  addScriptSeqStrings(shopIfs, 0x230, 10, streamShop, tableStd);
  
//  addScriptSeqStrings(shopIfs, 0x40C, 3, streamShop, tableStd,
//                      "", TenmaMsgDism::msgTypeMode_check);
  shopIfs.seek(0x40C);
  streamShop.addComment("orochimaru letter 1");
  streamShop.addScriptString(shopIfs, tableStd, "", TenmaMsgDism::msgTypeMode_check);
  streamShop.addComment("orochimaru letter 2");
  streamShop.addScriptString(shopIfs, tableStd, "", TenmaMsgDism::msgTypeMode_check);
  streamShop.addComment("examining toubei plans");
  streamShop.addScriptString(shopIfs, tableStd, "", TenmaMsgDism::msgTypeMode_check);
  
  streamShop.addComment("gambling game");
  addScriptSeqStrings(shopIfs, 0xD1C, 11, streamShop, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  
  streamShop.addComment("storage area");
  addScriptSeqStrings(shopIfs, 0x12D1, 22, streamShop, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  
  //=============================================
  // enemy list
  //=============================================
  
  TenmaScriptGenStream streamEnemy;
  
  streamEnemy.addComment("enemy names");
//  addGenericSeqStrings(battleExtraIfs, 0x2430, 93, streamEnemy, tableStd);
  battleExtraIfs.seek(0x2430);
  for (int i = 0; i < 189; i++) {
//    std::cerr << i << std::endl;
    // NOTE: various placeholder entries in this table contain the ascii
    // string "(null)".
    // the script dumper won't recognize this because the game can't actually
    // print it, so i've had to specially skip these over.
    if ((i == 93)
        || (i == 124)
        || (i == 125)
        || (i == 144))
      seekNextTerminator(battleExtraIfs);
    
    addGenericString(battleExtraIfs, battleExtraIfs.tell(), streamEnemy, tableStd);
  }
  
  //=============================================
  // battle
  //=============================================
  
  TenmaScriptGenStream streamBattle;
  
  streamBattle.addSetRegion("battle_std");
  
  streamBattle.addFreeSpace(0x1F60, 0xA0);
  
  streamBattle.addComment("ally names");
  addGenericSeqStrings(battleTextIfs, 0x13F0, 10, streamBattle, tableStd);
  
  streamBattle.addComment("stat/technique/magic names");
  addGenericSeqStrings(battleTextIfs, 0x144C, 5, streamBattle, tableStd);
  
//  streamBattle.addSetSize(216, 3);
  streamBattle.addSetSize(-1, -1);
  
  streamBattle.addComment("standard messages");
  addScriptSeqStrings(battleTextIfs, 0x1511, 85, streamBattle, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  
  streamBattle.addSetRegion("battle_boss");
  
  streamBattle.addComment("boss combat messages");
/*  addScriptSeqStrings(battleTextIfs, 0x2018, 5, streamBattle, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  addScriptSeqStrings(battleTextIfs, 0x2108, 10, streamBattle, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  addScriptSeqStrings(battleTextIfs, 0x226A, 5, streamBattle, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  addScriptSeqStrings(battleTextIfs, 0x233C, 7, streamBattle, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  addScriptSeqStrings(battleTextIfs, 0x233C, 32, streamBattle, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);*/
  addScriptPointerTableSet(battleText2Ifs, 0x0, 15, 0xC000, streamBattle, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  
  //=============================================
  // battle boss dialogue
  //=============================================
  
  TenmaScriptGenStream streamBoss;
  
//  streamBoss.addComment("boss dialogue");
  for (int i = 0xEFC4; i <= 0xF2D0; i++) {
    int baseIsoBytePos = i * 0x800;
    isoIfs.seek(baseIsoBytePos);
    
    bool valid = true;
//    int biggestPtr = 0;
    for (int j = 0; j < 6; j++) {
      int nextPtr = isoIfs.readu16le();
      if ((nextPtr < 0xC000) || (nextPtr >= 0xE000)) {
        valid = false;
        break;
      }
      
//      if (nextPtr >= biggestPtr) biggestPtr = nextPtr;
    }
    
    if (!valid) continue;
    
    int dataPtr = isoIfs.readu16le();
    if ((dataPtr < 0xC000) || (dataPtr >= 0xE000)) continue;
    
    isoIfs.seek(baseIsoBytePos + (dataPtr - 0xC000));
    
    int introPtrOffset = isoIfs.tell() - baseIsoBytePos;
    int introPtr = isoIfs.readu16le();
    
    int losePtrOffset = isoIfs.tell() - baseIsoBytePos;
    int losePtr = isoIfs.readu16le();
    
    int winPtrOffset = isoIfs.tell() - baseIsoBytePos;
    int winPtr = isoIfs.readu16le();
//    int unkPtr = isoIfs.readu16le();
    
    isoIfs.seek(baseIsoBytePos);
    TBufStream temp;
    temp.writeFrom(isoIfs, 0x6800);
    
    std::string bossNumStr = TStringConversion::intToString(i,
            TStringConversion::baseHex);
    
    if ((introPtr != 0xFFFF) || (losePtr != 0xFFFF) || (winPtr != 0xFFFF)) {
      streamBoss.addSetRegion(std::string("boss_sec_")
        + TStringConversion::intToString(i));
      streamBoss.addComment(std::string("boss ")
        + bossNumStr
        + " dialogue");
      streamBoss.addSetRegionProp("originSector",
        TStringConversion::intToString(i));
    }
    
    // used to defragment the intro/lose/win strings
    TFreeSpace bossStringFreeSpace;
    
    int introStrEndPos = -1;
    if (introPtr != 0xFFFF) {
      int introStrStartPos = introPtr - 0xC000;
      streamBoss.addSmallComment("intro");
      addScriptString_directRedirect(
                      temp, introStrStartPos, streamBoss, tableStd,
                      0xC000,
                      introPtrOffset,
                      std::string("boss_") + bossNumStr + "_intro",
                      TenmaMsgDism::msgTypeMode_check);
      introStrEndPos = temp.tell();
      bossStringFreeSpace.free(introStrStartPos, introStrEndPos - introStrStartPos);
    }
    
    int loseStrEndPos = -1;
    if (losePtr != 0xFFFF) {
//      streamBoss.addSmallComment(std::string("boss ")
//        + bossNumStr
//        + " player lose dialogue");
      int loseStrStartPos = losePtr - 0xC000;
      streamBoss.addSmallComment("player lose");
      addScriptString_directRedirect(
                      temp, loseStrStartPos, streamBoss, tableStd,
                      0xC000,
                      losePtrOffset,
                      std::string("boss_") + bossNumStr + "_lose",
                      TenmaMsgDism::msgTypeMode_check);
      loseStrEndPos = temp.tell();
      bossStringFreeSpace.free(loseStrStartPos, loseStrEndPos - loseStrStartPos);
    }
    
    int winStrEndPos = -1;
    if (winPtr != 0xFFFF) {
//      streamBoss.addSmallComment(std::string("boss ")
//        + bossNumStr
//        + " player win dialogue");
      int winStrStartPos = winPtr - 0xC000;
      streamBoss.addSmallComment("player win");
      addScriptString_directRedirect(
                      temp, winStrStartPos, streamBoss, tableStd,
                      0xC000,
                      winPtrOffset,
                      std::string("boss_") + bossNumStr + "_win",
                      TenmaMsgDism::msgTypeMode_check);
      winStrEndPos = temp.tell();
      bossStringFreeSpace.free(winStrStartPos, winStrEndPos - winStrStartPos);
    }
    
    int biggestEndPos = introStrEndPos;
    if (loseStrEndPos > biggestEndPos) biggestEndPos = loseStrEndPos;
    if (winStrEndPos > biggestEndPos) biggestEndPos = winStrEndPos;
    int endFreeSpace = -1;
    if (biggestEndPos > 0) {
      int nextSectorBound = ((biggestEndPos + 0x800) / 0x800) * 0x800;
      temp.seek(nextSectorBound - 1);
      int count = 0;
      while ((temp.tell() > biggestEndPos)
             && (temp.get() == 0x00)) {
        ++count;
        temp.seekoff(-2);
      }
      
      endFreeSpace = count - 32;
//      std::cerr << std::hex << i << ": " << std::hex << count << std::endl;

      // HACK
      if (i == 0xF14A) {
        endFreeSpace = count + 1;
      }
    
      if (endFreeSpace > 0) {
//        streamBoss.addFreeSpace(nextSectorBound - endFreeSpace,
//                                  endFreeSpace);
        bossStringFreeSpace.free(nextSectorBound - endFreeSpace, endFreeSpace);
      }
      
      // HACK
/*      if (i == 0xF14A) {
        streamBoss.addFreeSpace(0xE3E,
                                  (0x1000 - 0xE3E));
      }*/
      
      // HACK: every single boss works fine once defragmented...
      // except the final boss, which is still too big.
      // fortunately, these three sprites aren't used.
//      if (i == 0xF14A) {
//        streamBoss.addFreeSpace(0xA80, 0x180);
//      }
    }
    
    for (auto item: bossStringFreeSpace.freeSpace_) {
      streamBoss.addFreeSpace(item.first,
                              item.second);
    }
    
/*    if (unkPtr != 0xFFFF) {
//      streamBoss.addComment(std::string("boss ")
//        + bossNumStr
//        + " unk dialogue");
      streamBoss.addSmallComment("unknown");
      addScriptString(temp, unkPtr - 0xC000, streamBoss, tableStd,
                      std::string("boss_") + bossNumStr + "_unk",
                      TenmaMsgDism::msgTypeMode_check);
    }*/
    
/*    streamBoss.addComment(std::string("boss dialogue ")
      + TStringConversion::intToString(i,
          TStringConversion::baseHex));
    
    isoIfs.seek(i * 0x800);
    TBufStream temp;
    temp.writeFrom(isoIfs, size);
    
    temp.seek(0);
    streamBoss.addMap(temp, mapIndex, tableStd, i);*/
  }
  
  //=============================================
  // battle altboss dialogue
  //=============================================
  
  TenmaScriptGenStream streamAltBoss;
  
  // TODO: i have no idea what this data is.
  // i don't recognize any of the dialogue, so it may not even be used
  // (note that header format does not match the definitely-used
  // dialogue above, so it may even be left over from an older iteration
  // of the engine or something).
  // but i'd better include it in case it is in fact used.
  streamAltBoss.addSetRegion("battle_altboss");
  for (int i = 0xABD; i <= 0xAF9; i++) {
    isoIfs.seek(i * 0x800);
    
    bool valid = true;
    for (int j = 0; j < 5; j++) {
      int nextPtr = isoIfs.readu16le();
      if ((nextPtr < 0xC000) || (nextPtr >= 0xE000)) {
        valid = false;
        break;
      }
    }
    
    if (!valid) continue;
    
    int dataPtr = isoIfs.readu16le();
    if ((dataPtr < 0xC000) || (dataPtr >= 0xE000)) continue;
    
    isoIfs.seek((i * 0x800) + (dataPtr - 0xC000));
    
    int introPtr = isoIfs.readu16le();
    int losePtr = isoIfs.readu16le();
    int winPtr = isoIfs.readu16le();
    // FIXME: possibly not part of this data
//    int unkPtr = isoIfs.readu16le();
    
    isoIfs.seek(i * 0x800);
    TBufStream temp;
    temp.writeFrom(isoIfs, 0x6800);
    
    std::string bossNumStr = TStringConversion::intToString(i,
            TStringConversion::baseHex);
    
    if ((introPtr != 0xFFFF) || (losePtr != 0xFFFF) || (winPtr != 0xFFFF)) {
      streamAltBoss.addSetRegion(std::string("bossalt_sec_")
        + TStringConversion::intToString(i));
      streamAltBoss.addComment(std::string("??? altboss ")
        + bossNumStr
        + " dialogue?");
      streamAltBoss.addSetRegionProp("originSector",
        TStringConversion::intToString(i));
    }
    
    int introStrEndPos = -1;
    if (introPtr != 0xFFFF) {
      streamAltBoss.addSmallComment("intro");
      addScriptString(temp, introPtr - 0xC000, streamAltBoss, tableStd,
                      std::string("boss_") + bossNumStr + "_intro",
                      TenmaMsgDism::msgTypeMode_check);
      introStrEndPos = temp.tell();
    }
    
    int loseStrEndPos = -1;
    if (losePtr != 0xFFFF) {
//      streamAltBoss.addSmallComment(std::string("boss ")
//        + bossNumStr
//        + " player lose dialogue");
      streamAltBoss.addSmallComment("player lose");
      addScriptString(temp, losePtr - 0xC000, streamAltBoss, tableStd,
                      std::string("boss_") + bossNumStr + "_lose",
                      TenmaMsgDism::msgTypeMode_check);
      loseStrEndPos = temp.tell();
    }
    
    int winStrEndPos = -1;
    if (winPtr != 0xFFFF) {
//      streamAltBoss.addSmallComment(std::string("boss ")
//        + bossNumStr
//        + " player win dialogue");
      streamAltBoss.addSmallComment("player win");
      addScriptString(temp, winPtr - 0xC000, streamAltBoss, tableStd,
                      std::string("boss_") + bossNumStr + "_win",
                      TenmaMsgDism::msgTypeMode_check);
      winStrEndPos = temp.tell();
    }
    
    int biggestEndPos = introStrEndPos;
    if (loseStrEndPos > biggestEndPos) biggestEndPos = loseStrEndPos;
    if (winStrEndPos > biggestEndPos) biggestEndPos = winStrEndPos;
    int endFreeSpace = -1;
    if (biggestEndPos > 0) {
      int nextSectorBound = ((biggestEndPos + 0x800) / 0x800) * 0x800;
      temp.seek(nextSectorBound - 1);
      int count = 0;
      while ((temp.tell() > biggestEndPos)
             && (temp.get() == 0x00)) {
        ++count;
        temp.seekoff(-2);
      }
      
      endFreeSpace = count - 32;
//      std::cerr << std::hex << i << ": " << std::hex << count << std::endl;
    
      if (endFreeSpace > 0) {
        streamAltBoss.addFreeSpace(nextSectorBound - endFreeSpace,
                                  endFreeSpace);
      }
      
      // HACK
      if (i == 0xAE4) {
        streamAltBoss.addFreeSpace(0xE3E,
                                  (0x1000 - 0xE3E));
      }
      else if (i == 0xAEE) {
        streamAltBoss.addFreeSpace(0x93C,
                                  (0x1000 - 0x93C));
      }
      else if (i == 0xAF9) {
        streamAltBoss.addFreeSpace(0x8EC,
                                  (0x1000 - 0x8EC));
      }
    }
  }
  
  //=============================================
  // intro/ending
  //=============================================
  
  TenmaScriptGenStream streamIntro;
//  streamIntro.addSetSize(216, 3);
  
  streamIntro.addComment("intro");
  addScriptSeqStrings(introIfs, 0x13D2, 16, streamIntro, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  
  streamIntro.addComment("ending?");
  addScriptSeqStrings(introIfs, 0x1FFF, 5, streamIntro, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  
  //=============================================
  // save/load menu
  //=============================================
  
  TenmaScriptGenStream streamSaveLoad;
  streamSaveLoad.addSetSize(216, 3);
  
  streamSaveLoad.addComment("save/load menu + restart after party defeated");
  addScriptSeqStrings(saveLoadIfs, 0x171A, 40, streamSaveLoad, tableStd,
                      "", TenmaMsgDism::msgTypeMode_noCheck);
  
  //=============================================
  // generic text
  //=============================================
  
  TenmaScriptGenStream streamGenericText;
  
  streamGenericText.addSetRegion("generic_msg");
//  streamGenericText.addSetSize(216, 3);
  streamGenericText.addSetSize(-1, -1);
  
  streamGenericText.addComment("generic text (item use, etc.)");
  // possibly has one additional entry, but it's just an [fa] command,
  // so it shouldn't matter if we ignore it
//  addScriptSeqStrings(genericTextIfs, 0x0, 48, streamGenericText, tableStd,
  addScriptSeqStrings(genericTextIfs, 0x0, 47, streamGenericText, tableStd,
                      "", TenmaMsgDism::msgTypeMode_noCheck);
  streamGenericText.addComment("more use messages, plus placeholders?");
//  addScriptSeqStrings(genericTextIfs, 0x64E, 32, streamGenericText, tableStd,
//                      "", TenmaMsgDism::msgTypeMode_check);
//  addScriptSeqStrings(genericTextIfs, 0x64E, 33, streamGenericText, tableStd,
//                      "", TenmaMsgDism::msgTypeMode_check);
  // the "placeholders" are in fact actually that: they are copied over
  // with data for combat messages
  addScriptSeqStrings(genericTextIfs, 0xBFE, 20, streamGenericText, tableStd,
                      "", TenmaMsgDism::msgTypeMode_check);
  
  streamGenericText.addSetRegion("generic_techname");
  streamGenericText.addSetSize(-1, -1);
  
  streamGenericText.addComment("enemy spell/technique names");
  addScriptSeqStrings(genericTextIfs, 0xF6B, 32, streamGenericText, tableStd,
                      "", TenmaMsgDism::msgTypeMode_noCheck);
  
  //=============================================
  // credits
  //=============================================
  
  TenmaScriptGenStream streamCredits;
  
  {
    std::ofstream ofs("script/orig/credits_inc.s");
    streamCredits.addComment("credits");
    dumpCredits(creditsIfs, streamCredits, tableCredits, ofs);
  }
  
  //=============================================
  // info
  //=============================================
  
  TenmaScriptGenStream streamInfo;
//  streamInfo.addSetSize(216, 3);
  streamInfo.addSetSize(-1, -1);
  
  streamInfo.addComment("item/technique info menu");
  streamInfo.addFreeSpace(0x1800, 0x400);
  addScriptSeqStrings(infoIfs, 0x74C, 195, streamInfo, tableStd,
                      "", TenmaMsgDism::msgTypeMode_noCheck);
  
  //=============================================
  // menus
  //=============================================
  
  TenmaScriptGenStream streamMenu;
  
//  addSeqMenus(overwIfs, 0x1E6A, 1, streamMenu, tableFull,
//              "", TenmaMsgDism::menuTypeMode_noParams);
  
  //=====
  // kernel
  //=====

  streamMenu.addSetRegion("menu_kernel");
  streamMenu.addComment("kernel menus");
  
  streamMenu.addSmallComment("character select menu placeholder");
  addSeqMenus(kernelIfs, 0x17AB, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_noParams);
  
  streamMenu.addSmallComment("character names, top row");
  addSeqMenus(kernelIfs, 0x1893, 3, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_noParams);
  
  streamMenu.addSmallComment("character names, bottom row");
  addSeqMenus(kernelIfs, 0x18B4, 3, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_noParams);
  
  streamMenu.addSmallComment("character name/HP/TP display");
  addSeqMenus(kernelIfs, 0x18D5, 4, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_noParams);
  
  //=====
  // kernel: orochimaru name patches
  //=====

  streamMenu.addSetRegion("orochimaru_name");
  streamMenu.addComment("orochimaru name patches");
  
  streamMenu.addSmallComment("bottom row normal");
  kernelIfs.seek(0xD9E);
  streamMenu.addRawUnterminatedString(kernelIfs, 5, tableFull, "");
  
  streamMenu.addSmallComment("top row normal");
  kernelIfs.seek(0xDA8);
  streamMenu.addRawUnterminatedString(kernelIfs, 1, tableFull, "");
  
  streamMenu.addSmallComment("bottom row kuroi kage");
  kernelIfs.seek(0xDA3);
  streamMenu.addRawUnterminatedString(kernelIfs, 5, tableFull, "");
  
  streamMenu.addSmallComment("top row kuroi kage");
  kernelIfs.seek(0xDA9);
  streamMenu.addRawUnterminatedString(kernelIfs, 1, tableFull, "");
  
  // note that the patch strings in generic_text are stored as standard text,
  // so they're omitted here (hopefully they can be done through the
  // normal redirect-token method without the need for special handling)
  
  //=====
  // overw
  //=====

  streamMenu.addSetRegion("menu_overw");
  streamMenu.addComment("overworld menus");
  
  streamMenu.addSmallComment("?");
  addMenu(overwIfs, 0x1E6A, streamMenu, tableFull,
          "", TenmaMsgDism::menuTypeMode_noParams);
  addSeqMenus(overwIfs, 0x1F06, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_noParams);
  
  streamMenu.addSmallComment("main menu");
  addMenu(overwIfs, 0x2712, streamMenu, tableFull,
          "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("use/give menu");
  addMenu(overwIfs, 0x27B2, streamMenu, tableFull,
          "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("use/give/drop menu");
  addMenu(overwIfs, 0x2829, streamMenu, tableFull,
          "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("status display: top window");
  addSeqMenus(overwIfs, 0x2B0E, 4, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_noParams);
  
  streamMenu.addSmallComment("status display: bottom window");
  addSeqMenus(overwIfs, 0x2C2E, 4, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_noParams);
  
  streamMenu.addSmallComment("settings menu");
  addSeqMenus(overwIfs, 0x2D9C, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("text speed menu");
  addSeqMenus(overwIfs, 0x2E14, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("setting on/off menu");
  addSeqMenus(overwIfs, 0x2E5F, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("walk type menu");
  addSeqMenus(overwIfs, 0x2E97, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("map name window");
  addSeqMenus(overwIfs, 0x2EFE, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_noParams);
  
  //=====
  // shop
  //=====

  streamMenu.addSetRegion("menu_shop");
  streamMenu.addComment("shop menus");
  
  streamMenu.addSmallComment("buy/sell menu");
  addSeqMenus(shopIfs, 0x9CD, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("buyable item list entry");
  addSeqMenus(shopIfs, 0xC53, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_noParams);
  
  streamMenu.addSmallComment("player money window");
  addSeqMenus(shopIfs, 0xD06, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("storage menu");
  addSeqMenus(shopIfs, 0x1925, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("storage money window");
  addSeqMenus(shopIfs, 0x1D7A, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  //=====
  // battle
  //=====

  streamMenu.addSetRegion("menu_battle");
  streamMenu.addComment("battle menus");
  
  streamMenu.addSmallComment("action menu: ziria");
  addSeqMenus(battleTextIfs, 0x131, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("action menu: orochimaru");
  addSeqMenus(battleTextIfs, 0x227, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("action menu: tsunade");
  addSeqMenus(battleTextIfs, 0x294, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  //=====
  // save/load
  //=====

  streamMenu.addSetRegion("menu_saveload");
  streamMenu.addComment("save/load menus");
  
  streamMenu.addSmallComment("file continue/copy/erase menu");
  addSeqMenus(saveLoadIfs, 0xC2C, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("file select menu 1");
  addSeqMenus(saveLoadIfs, 0xFCE, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  streamMenu.addSmallComment("file select menu 2");
  addSeqMenus(saveLoadIfs, 0x116B, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  //=====
  // item info menu
  //=====

  streamMenu.addSetRegion("menu_info");
  streamMenu.addComment("item/technique info menus");
  
  streamMenu.addSmallComment("item/technique select menu");
  addSeqMenus(infoIfs, 0x6B, 1, streamMenu, tableFull,
              "", TenmaMsgDism::menuTypeMode_params);
  
  //=============================================
  // misc
  //=============================================
  
  TenmaScriptGenStream streamMisc;
  
  streamMisc.addSetRegion("misc_yesno_prompt");
  streamMisc.addSetSize(-1, -1);
  
  streamMisc.addComment("dialogue yes/no prompt");
//  streamMisc.addGenericLine("#LOADTABLE(\"table/tenma_std_en_with_null.tbl\")");
  addGenericString(kernelIfs, 0x1F94, streamMisc, tableStdWithNull, "yesno_prompt");
  
  streamMisc.addSetRegion("misc_gokumon");
  streamMisc.addFreeSpace(0x1A00, 0x600);
  streamMisc.addSetSize(216, 3);
  streamMisc.addComment("sent to gokumon event");
  
//  streamMisc.addGenericLine("#LOADTABLE(\"table/tenma_std_en.tbl\")");
  addScriptString(specialIfs, 0x187E, streamMisc, tableStd,
                  "", TenmaMsgDism::msgTypeMode_noCheck);
  
//  streamMisc.addGenericLine("#LOADTABLE(\"table/tenma_en_std.tbl\")");
  
  //=============================================
  // subtitle placeholders
  //=============================================
  
  TenmaScriptGenStream streamSubtitle;
  
  streamSubtitle.addSetSize(224, 2);
//  streamSubtitle.addGenericLine("#SETSIZE(224, 4)");
  streamSubtitle.addGenericLine("#SETFAILONBOXOVERFLOW(1)");
  
  streamSubtitle.addPlaceholderStringSet("subtitle", 256, true);
  
  //=============================================
  // export sheets
  //=============================================
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_dialogue.txt");
    streamMaps.exportToSheet(sheetMaps, ofs, "dlog");
    sheetMaps.exportCsv("script/orig/script_dialogue.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_items.txt");
    streamItems.exportToSheet(sheetItems, ofs, "items");
    sheetItems.exportCsv("script/orig/script_items.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_shop.txt");
    streamShop.exportToSheet(sheetShop, ofs, "shop");
    sheetShop.exportCsv("script/orig/script_shop.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_enemy.txt");
    streamEnemy.exportToSheet(sheetEnemy, ofs, "enemy");
    sheetEnemy.exportCsv("script/orig/script_enemy.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_battle.txt");
    streamBattle.exportToSheet(sheetBattle, ofs, "battle");
    sheetBattle.exportCsv("script/orig/script_battle.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_boss.txt");
    streamBoss.exportToSheet(sheetBoss, ofs, "boss");
    sheetBoss.exportCsv("script/orig/script_boss.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_bossalt.txt");
    streamAltBoss.exportToSheet(sheetAltBoss, ofs, "bossalt");
    sheetAltBoss.exportCsv("script/orig/script_bossalt.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_intro_ending.txt");
    streamIntro.exportToSheet(sheetIntro, ofs, "intro_ending");
    sheetIntro.exportCsv("script/orig/script_intro_ending.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_saveload.txt");
    streamSaveLoad.exportToSheet(sheetSaveLoad, ofs, "saveload");
    sheetSaveLoad.exportCsv("script/orig/script_saveload.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_generic_text.txt");
    streamGenericText.exportToSheet(sheetGenericText, ofs, "generic_text");
    sheetGenericText.exportCsv("script/orig/script_generic_text.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_credits.txt");
    streamCredits.exportToSheet(sheetCredits, ofs, "credits");
    sheetCredits.exportCsv("script/orig/script_credits.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_info.txt");
    streamInfo.exportToSheet(sheetInfo, ofs, "info");
    sheetInfo.exportCsv("script/orig/script_info.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_menu.txt");
    streamMenu.exportToSheet(sheetMenu, ofs, "menu");
    sheetMenu.exportCsv("script/orig/script_menu.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_misc.txt");
    streamMisc.exportToSheet(sheetMisc, ofs, "misc");
    sheetMisc.exportCsv("script/orig/script_misc.csv");
  }
  
  {
    std::ofstream ofs;
    ofs.open("script/orig/spec_subtitle.txt");
    streamSubtitle.exportToSheet(sheetSubtitle, ofs, "subtitle");
    sheetSubtitle.exportCsv("script/orig/script_subtitle.csv");
  }
  
  
  return 0;
}
