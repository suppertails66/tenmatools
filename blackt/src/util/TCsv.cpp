#include "util/TCsv.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

namespace BlackT {


TCsv::TCsv() { }

void TCsv::readUtf8(TStream& ifs) {
  while (!ifs.nextIsEof()) {
    rows.push_back(CellCollection());
    CellCollection& row = rows[rows.size() - 1];
    readCsvRowUtf8(ifs, row);
  }
}
  
void TCsv::readSjis(TStream& ifs) {
//  while (ifs.good()) {
  while (!ifs.eof()) {
    ifs.get();
//    if (!ifs.good()) break;
    if (ifs.eof()) break;
    ifs.unget();
    
    rows.push_back(CellCollection());
    CellCollection& row = rows[rows.size() - 1];
    readCsvRowSjis(ifs, row);
  }
}

int TCsv::numCols() const {
  return (rows.size() > 0) ? rows[0].size() : 0;
}

int TCsv::numColsInRow(int row) const {
  return rows[row].size();
}

int TCsv::numRows() const {
  return rows.size();
}

const TString& TCsv::cell(int x, int y) const {
  if ((x >= numCols()) || (y >= numRows())) {
    throw TGenericException(T_SRCANDLINE,
                            "TCsv::cell(int,int) const",
                            std::string("Out-of-range access: (")
                              + TStringConversion::toString(x)
                              + ", "
                              + TStringConversion::toString(y)
                              + ")");
  }
  
  return rows[y][x];
}

TString& TCsv::cell(int x, int y) {
  if ((x >= numCols()) || (y >= numRows())) {
    throw TGenericException(T_SRCANDLINE,
                            "TCsv::cell(int,int)",
                            std::string("Out-of-range access: (")
                              + TStringConversion::toString(x)
                              + ", "
                              + TStringConversion::toString(y)
                              + ")");
  }
  
  return rows[y][x];
}

bool TCsv::readCsvCellSjis(TStream& ifs, TString& dst) {
  dst = "";

  bool escaping = false;
//  while (ifs.good()) {
  char prev = 0x00;
  while (!ifs.eof()) {
    char next = ifs.get();
    // escaped literal
    if (next == '"') {
      // double-quote for escaped quote causes problems
      // if not specially handled
      if (!ifs.eof() && (ifs.peek() == '"')) {
        dst += ifs.get();
      }
      else {
        escaping = !escaping;
      }
      
/*      if (prev == '"') {
        dst += next;
      }
      
      escaping = !escaping; */
    }
    else {
      // check for end of cell
      if (!escaping) {
        // return true if more columns remain in row, false otherwise
        if (next == ',') return true;
        else if (next == '\n') return false;
      }
      
      dst += next;
      
      // 0x80+ introduces a 2-byte sjis sequence
      if (next < 0) {
        dst += ifs.get();
      }
    }
    
    prev = next;
  }
  
  // reached EOF (or ifs went bad)
  return false;
}

void TCsv::readCsvRowSjis(TStream& ifs, CellCollection& dst) {
  TString str;
  while (readCsvCellSjis(ifs, str)) dst.push_back(str);
  // add final cell
  dst.push_back(str);
}

bool TCsv::readCsvCellUtf8(TStream& ifs, TString& dst) {
  dst = "";

  bool escaping = false;
//  while (ifs.good()) {
  while (!ifs.eof()) {
    char next = ifs.get();
    // escaped literal
    if (next == '"') {
      // double-quote for escaped quote causes problems
      // if not specially handled
      if (!ifs.eof() && (ifs.peek() == '"')) {
        dst += ifs.get();
      }
      else {
        escaping = !escaping;
      }
      
//      escaping = !escaping;
    }
    else {
      // check for end of cell
      if (!escaping) {
        // return true if more columns remain in row, false otherwise
        if (next == ',') return true;
        else if (next == '\n') return false;
      }
      
      dst += next;
      
      // check for presence of additional bytes
      
      int trailbytes = 0;
      if ((next & 0xE0) == 0xC0) {
        trailbytes = 1;
      }
      else if ((next & 0xF0) == 0xE0) {
        trailbytes = 2;
      }
      else if ((next & 0xF8) == 0xF0) {
        trailbytes = 3;
      }
      
      for (int i = 0; i < trailbytes; i++) {
        dst += ifs.get();
      }
      
      // 0x80+ introduces a 2-byte sjis sequence
//      if (next < 0) {
//        dst += ifs.get();
//      }
    }
  }
  
  // reached EOF (or ifs went bad)
  return false;
}

void TCsv::readCsvRowUtf8(TStream& ifs, CellCollection& dst) {
  TString str;
  while (readCsvCellUtf8(ifs, str)) dst.push_back(str);
  // add final cell
  dst.push_back(str);
}

std::string TCsv::coordinatePosToCellId(int x, int y) {
  // easy
  std::string rowPart = TStringConversion::intToString(y + 1);
  
  // hard
  // FIXME: holy shit this is hellish, you think it's just a base-26
  // conversion but then you realize that A is not equivalent to zero.
  // it acts as zero in the ones column, but as one in the tens...
  std::string colPart;
  colPart += ((char)(x % 26) + 'A');
  x /= 26;
  while (x > 0) {
    char output = ((char)(x % 26) + 'A' - 1);
    colPart = output + colPart;
    x /= 26;
  }
  
  return colPart + rowPart;
}


} 
