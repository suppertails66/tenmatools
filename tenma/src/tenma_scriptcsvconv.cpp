#include "tenma/TenmaTranslationSheet.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TFileManip.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TParse.h"
#include "util/TThingyTable.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;
using namespace BlackT;
using namespace Pce;

TenmaTranslationSheet scriptSheet;
//TThingyTable tableStd;

std::string getNumStr(int num) {
  std::string str = TStringConversion::intToString(num);
  while (str.size() < 2) str = string("0") + str;
  return str;
}

std::string getHexWordNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 4) str = string("0") + str;
  return string("$") + str;
}

int getSubmsgNum(std::string id) {
  int pos = 0;
  for (pos = id.size() - 1; pos >= 0; pos--) {
    if (id[pos] == '_') break;
  }
  
  if (pos < 0) return -1;
  
  return TStringConversion::stringToInt(id.substr(pos + 1, std::string::npos));
}

int getMsgRowCount(TCsv& csv, int startingRow) {
  int initialSubmsgNum = getSubmsgNum(csv.cell(1, startingRow));
  int endingRow = startingRow + 1;
  while ((endingRow < csv.numRows())
         && (csv.cell(0, endingRow).compare("string") == 0)) {
    int nextSubmsgNum = getSubmsgNum(csv.cell(1, endingRow));
    if (nextSubmsgNum <= initialSubmsgNum) break;
    ++endingRow;
  }
  
  return endingRow - startingRow;
}

struct MsgEntry {
  std::string en;
  std::string jp;
  std::string note;
  int rowCount;
};

std::string replaceLinebreaks(std::string str) {
  std::string finalStr;
  for (int i = 0; i < str.size(); i++) {
    if (str[i] == '\n') {
//    if ((str[i] == '\\') && (str[i + 1] == 'n') && (str[i + 2] == '\n')) {
      finalStr += "<br>";
    }
    else {
      finalStr += str[i];
    }
  }
  return finalStr;
}

std::string replaceEscapedLinebreaks(std::string str) {
  std::string finalStr;
  for (int i = 0; i < str.size(); i++) {
    if ((str[i] == '\\') && (str[i + 1] == 'n')) {
      finalStr += "<br>";
      // also skips assumed 1b linebreak character
      i += 2;
    }
    else {
      finalStr += str[i];
    }
  }
  return finalStr;
}

std::string replaceEscapedLinebreaksJp(std::string str) {
  std::string finalStr;
  for (int i = 0; i < str.size(); i++) {
    if ((str[i] == '\\') && (str[i + 1] == 'n')) {
      finalStr += "\n";
      // also skips assumed 1b linebreak character
      i += 2;
    }
    else {
      finalStr += str[i];
    }
  }
  return finalStr;
}

MsgEntry makeMsgEntry(TCsv& csv, int startingRow) {
  MsgEntry result;
  result.rowCount = getMsgRowCount(csv, startingRow);
  
  for (int i = 0; i < result.rowCount; i++) {
    result.en += csv.cell(5, startingRow + i);
    result.jp += csv.cell(4, startingRow + i);
    
    if (i != (result.rowCount - 1)) {
      result.en += "<br><br>";
//      result.jp += "<br><br>";
      result.jp += "\n\n";
    }
  }
  
  result.en = replaceEscapedLinebreaks(result.en);
  result.jp = replaceEscapedLinebreaksJp(result.jp);
  result.note = csv.cell(3, startingRow);
  
  return result;
}



int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Tengai Makyou Ziria script CSV converter" << endl;
    cout << "Usage: " << argv[0] << " <infile>"
      << endl;
    return 0;
  }
  
  std::string inFile(argv[1]);
  
  TCsv csv;
  {
    TBufStream ifs;
    ifs.open(inFile.c_str());
    csv.readUtf8(ifs);
  }
  
  std::ostream& output = std::cout;
  
  std::map<std::string, TGraphic> imgMap;
  
  std::string tableName;
  std::string imgName;
  std::string description;
  std::string header;
  for (int j = 0; j < csv.numRows(); ) {
    std::string cmd = csv.cell(0, j);
    
    if (cmd.compare("string") == 0) {
//      output << tableName << std::endl << std::endl;
      
      if (!header.empty()) {
        output << header << std::endl;
      }
      
      if (!imgName.empty()) {
//        output << "[[File:" << imgName << "|640px]]" << std::endl << std::endl;
//        int imageW = imgMap[imgName].w();
        // HACK
        int imageW = imgMap[imgName].w();
        {
          std::string filename
            = std::string("scrap/tcrf_images/maps/") + imgName;
          if (TFileManip::fileExists(filename)) {
            TGraphic temp;
            TPngConversion::RGBAPngToGraphic(filename, temp);
            imageW = temp.w();
          }
        }
        output << "[[File:" << imgName;
        std::cerr << imgName << " " << imageW << std::endl;
        if (imageW > 640) {
          output << "|640px";
        }
        output << "]]" << std::endl << std::endl;
      }
      
      if (!description.empty()) {
        output << description << std::endl << std::endl;
      }
      
      output << "{| class=\"wikitable\" style=\"margin-left: auto; margin-right: auto;\"" << std::endl;
      output << "|-" << std::endl;
      output << "! style=\"width: 320px\" | Original !!  style=\"width: 320px\" | Translated !! style=\"width: 320px\" | Notes" << std::endl;
      
      bool firstStringSet = false;
      MsgEntry firstMsg;
      while (j < csv.numRows()) {
        std::string cmd = csv.cell(0, j);
        if (cmd.compare("string") != 0) break;
        
        MsgEntry msg = makeMsgEntry(csv, j);
        if (!firstStringSet) {
          firstMsg = msg;
          firstStringSet = true;
        }
        
        output << "|-" << std::endl;
        output << "|";
          output << " <pre>" << msg.jp << "</pre>";
          output << " || " << msg.en;
          output << " || " << msg.note;
          output << std::endl;
        
        j += msg.rowCount;
      }
      output << "|}" << std::endl << std::endl;
      
/*      if (!tableName.empty()) {
        std::string mapLabel = tableName.substr(40, std::string::npos);
        for (int i = 0; i < mapLabel.size(); i++) {
          if (mapLabel[i] == '\n') {
            mapLabel = mapLabel.substr(0, i);
          }
        }
        
        int mapNum = TStringConversion::stringToInt(
          mapLabel.substr(4, 6));
        
        std::cerr << ",,,,,";
        std::cerr << "=\"===";
          std::cerr << firstMsg.en << " (";
          if (mapLabel.size() > 10) std::cerr << "partial map";
          else std::cerr << "map";
          std::cerr << " " << TStringConversion::intToString(
            mapNum, TStringConversion::baseHex);
          std::cerr << ")";
        std::cerr << "===\"";
        std::cerr << std::endl;
      }*/
    }
    else {
      description = csv.cell(3, j);
      header = csv.cell(5, j);
      tableName = csv.cell(4, j);
      imgName = csv.cell(7, j);
      ++j;
      
      // HACK: now this is some fuckin' magic
      if (!imgName.empty()) {
        std::string srcImgName = std::string("scrap/test_maps_tcrf/")
          + imgName;
        TGraphic grp;
        TPngConversion::RGBAPngToGraphic(srcImgName, grp);
        bool matched = false;
        for (auto& item: imgMap) {
          if (item.second == grp) {
            std::cerr << imgName << " " << item.first << std::endl;
            imgName = item.first;
            matched = true;
            break;
          }
        }
        
        if (!matched) {
          imgMap[imgName] = grp;
          std::string dstImgName = std::string("scrap/test_maps_tcrf_nodupe/")
            + imgName;
          TFileManip::createDirectoryForFile(dstImgName);
          TPngConversion::graphicToRGBAPng(dstImgName, grp);
        }
      }
    }
  }
  
  return 0;
}
