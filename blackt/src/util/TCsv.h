#ifndef TCSV_H
#define TCSV_H


#include "util/TStream.h"
#include "util/TString.h"
#include "util/TTwoDArray.h"
#include <string>
#include <vector>

namespace BlackT {


class TCsv {
public:

  TCsv();
  
  void readUtf8(TStream& ifs);
  void readSjis(TStream& ifs);
  
  int numCols() const;
  int numColsInRow(int row) const;
  int numRows() const;
  const TString& cell(int x, int y) const;
  TString& cell(int x, int y);
  
  static std::string coordinatePosToCellId(int x, int y);

protected:
  typedef std::vector<TString> CellCollection;
  typedef std::vector<CellCollection> RowCollection;

  RowCollection rows;
  
  bool readCsvCellSjis(TStream& ifs, TString& dst);
  void readCsvRowSjis(TStream& ifs, CellCollection& dst);
  
  bool readCsvCellUtf8(TStream& ifs, TString& dst);
  void readCsvRowUtf8(TStream& ifs, CellCollection& dst);
  
};


}


#endif
