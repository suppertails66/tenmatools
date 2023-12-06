#include "pce/PcePalette.h"
#include "pce/PcePattern.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TFileManip.h"
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

// nonzero to write build number to title screen
#define ADD_BUILD_STAMP 0

using namespace std;
using namespace BlackT;
using namespace Pce;

const static int sectorSize = 0x800;

struct SignPatternGroup {
  PcePattern ul;
  PcePattern ur;
  PcePattern ll;
  PcePattern lr;
};

std::map<int, SignPatternGroup> signs;

void readSign(int id, SignPatternGroup& dst) {
  std::string basePrefix = std::string("out/grp/sign_")
      + TStringConversion::intToString(id);
      
  {
    TBufStream ifs;
    ifs.open((basePrefix + std::string("_0.bin")).c_str());
    dst.ul.read(ifs);
  }
  
  {
    TBufStream ifs;
    ifs.open((basePrefix + std::string("_1.bin")).c_str());
    dst.ur.read(ifs);
  }
  
  {
    TBufStream ifs;
    ifs.open((basePrefix + std::string("_2.bin")).c_str());
    dst.ll.read(ifs);
  }
  
  {
    TBufStream ifs;
    ifs.open((basePrefix + std::string("_3.bin")).c_str());
    dst.lr.read(ifs);
  }
}

void loadSign(int id) {
  SignPatternGroup dst;
  readSign(id, dst);
  signs[id] = dst;
}

void patchSignPiece(TStream& ifs, int id, int subid, int pos) {
  ifs.seek(pos + (PcePattern::size * 0x00));
  switch (subid) {
  case 0:
    signs.at(id).ul.write(ifs);
    break;
  case 1:
    signs.at(id).ur.write(ifs);
    break;
  case 2:
    signs.at(id).ll.write(ifs);
    break;
  case 3:
    signs.at(id).lr.write(ifs);
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "patchSignPiece()",
                            "bad sign subid");
    break;
  }
}

void patchSignStd(TStream& ifs, int id, int pos) {
  ifs.seek(pos + (PcePattern::size * 0x00));
  signs.at(id).ul.write(ifs);
  ifs.seek(pos + (PcePattern::size * 0x01));
  signs.at(id).ur.write(ifs);
  ifs.seek(pos + (PcePattern::size * 0x10));
  signs.at(id).ll.write(ifs);
  ifs.seek(pos + (PcePattern::size * 0x11));
  signs.at(id).lr.write(ifs);
}

// this is what we have to do to patch over all the storefront signs
// in the game with translated versions
// (this was automatically generated, see tenma_signscan)
void patchSigns(TStream& ifs) {
  patchSignStd(ifs, 1, 0x5869C0);
  patchSignStd(ifs, 2, 0x586D80);
  patchSignStd(ifs, 1, 0x58CD00);
  patchSignStd(ifs, 5, 0x58CD80);
  patchSignStd(ifs, 1, 0x58FD80);
  patchSignStd(ifs, 1, 0x592D80);
  patchSignStd(ifs, 1, 0x595D80);
  patchSignStd(ifs, 1, 0x59C580);
  patchSignStd(ifs, 2, 0x5BBD80);
  patchSignStd(ifs, 5, 0x5C4920);
  patchSignStd(ifs, 5, 0x629920);
  patchSignStd(ifs, 2, 0x646900);
  patchSignStd(ifs, 1, 0x646980);
  patchSignStd(ifs, 0, 0x8DD180);
  patchSignStd(ifs, 1, 0x8DD1C0);
  patchSignStd(ifs, 2, 0x8DD580);
  patchSignStd(ifs, 3, 0x8DD5C0);
  patchSignStd(ifs, 0, 0x94D180);
  patchSignStd(ifs, 1, 0x94D1C0);
  patchSignStd(ifs, 2, 0x94D580);
  patchSignStd(ifs, 3, 0x94D5C0);
  patchSignStd(ifs, 3, 0x9D7FA0);
  patchSignStd(ifs, 0, 0x9D8920);
  patchSignStd(ifs, 2, 0x9D94C0);
  patchSignStd(ifs, 1, 0x9D9580);
  patchSignPiece(ifs, 2, 0, 0xA0F8E0);
  patchSignPiece(ifs, 2, 1, 0xA0F980);
  patchSignPiece(ifs, 2, 2, 0xA0FAE0);
  patchSignPiece(ifs, 2, 3, 0xA0FB80);
  patchSignPiece(ifs, 3, 0, 0xA0FD80);
  patchSignPiece(ifs, 3, 1, 0xA10280);
  patchSignPiece(ifs, 3, 2, 0xA0FF80);
  patchSignPiece(ifs, 3, 3, 0xA10480);
  patchSignStd(ifs, 0, 0xA0FFA0);
  patchSignStd(ifs, 1, 0xA11580);
  patchSignStd(ifs, 3, 0xA7FFA0);
  patchSignStd(ifs, 0, 0xA80920);
  patchSignStd(ifs, 2, 0xA814C0);
  patchSignStd(ifs, 1, 0xA81580);
  patchSignStd(ifs, 2, 0xAC8000);
  patchSignStd(ifs, 0, 0xAC8180);
  patchSignStd(ifs, 3, 0xAC81C0);
  patchSignStd(ifs, 1, 0xAC8D00);
  patchSignStd(ifs, 2, 0xAD6000);
  patchSignStd(ifs, 0, 0xAD6180);
  patchSignStd(ifs, 3, 0xAD61C0);
  patchSignStd(ifs, 1, 0xAD6D00);
  patchSignStd(ifs, 2, 0xAE4000);
  patchSignStd(ifs, 0, 0xAE4180);
  patchSignStd(ifs, 3, 0xAE41C0);
  patchSignStd(ifs, 1, 0xAE4D00);
  patchSignStd(ifs, 2, 0xAF2000);
  patchSignStd(ifs, 0, 0xAF2180);
  patchSignStd(ifs, 3, 0xAF21C0);
  patchSignStd(ifs, 1, 0xAF2D00);
  patchSignStd(ifs, 2, 0xB00000);
  patchSignStd(ifs, 0, 0xB00180);
  patchSignStd(ifs, 3, 0xB001C0);
  patchSignStd(ifs, 1, 0xB00D00);
  patchSignStd(ifs, 2, 0xB0E000);
  patchSignStd(ifs, 0, 0xB0E180);
  patchSignStd(ifs, 3, 0xB0E1C0);
  patchSignStd(ifs, 1, 0xB0ED00);
  patchSignStd(ifs, 3, 0xCF1FA0);
  patchSignStd(ifs, 0, 0xCF2920);
  patchSignStd(ifs, 2, 0xCF34C0);
  patchSignStd(ifs, 1, 0xCF3580);
  patchSignStd(ifs, 0, 0xD2B180);
  patchSignStd(ifs, 1, 0xD2B1C0);
  patchSignStd(ifs, 2, 0xD2B580);
  patchSignStd(ifs, 3, 0xD2B5C0);
  patchSignStd(ifs, 3, 0xD9B100);
  patchSignStd(ifs, 1, 0xD9B500);
  patchSignStd(ifs, 1, 0xDE0920);
  patchSignStd(ifs, 2, 0xE1C9A0);
  patchSignStd(ifs, 2, 0xE25840);
  patchSignStd(ifs, 3, 0xE25880);
  patchSignStd(ifs, 0, 0xE25C00);
  patchSignStd(ifs, 1, 0xE25C40);
  patchSignStd(ifs, 1, 0xE46D80);
  patchSignStd(ifs, 3, 0xEA3FA0);
  patchSignStd(ifs, 0, 0xEA4920);
  patchSignStd(ifs, 2, 0xEA54C0);
  patchSignStd(ifs, 1, 0xEA5580);
  patchSignStd(ifs, 1, 0xEA9580);
  patchSignPiece(ifs, 3, 0, 0xF05D80);
  patchSignPiece(ifs, 3, 1, 0xF06280);
  patchSignPiece(ifs, 3, 2, 0xF05F80);
  patchSignPiece(ifs, 3, 3, 0xF06480);
  patchSignStd(ifs, 0, 0xF05FA0);
  patchSignStd(ifs, 1, 0xF07580);
  patchSignStd(ifs, 2, 0xF35100);
  patchSignStd(ifs, 1, 0xF35180);
  patchSignStd(ifs, 2, 0xF6A000);
  patchSignStd(ifs, 0, 0xF6A180);
  patchSignStd(ifs, 3, 0xF6A1C0);
  patchSignStd(ifs, 1, 0xF6AD00);
  patchSignStd(ifs, 2, 0xF78000);
  patchSignStd(ifs, 0, 0xF78180);
  patchSignStd(ifs, 3, 0xF781C0);
  patchSignStd(ifs, 1, 0xF78D00);
  patchSignStd(ifs, 2, 0xF94000);
  patchSignStd(ifs, 0, 0xF94180);
  patchSignStd(ifs, 3, 0xF941C0);
  patchSignStd(ifs, 1, 0xF94D00);
  patchSignStd(ifs, 2, 0xFE8000);
  patchSignStd(ifs, 0, 0xFE8180);
  patchSignStd(ifs, 3, 0xFE81C0);
  patchSignStd(ifs, 1, 0xFE8D00);
  patchSignStd(ifs, 2, 0xFF6000);
  patchSignStd(ifs, 0, 0xFF6180);
  patchSignStd(ifs, 3, 0xFF61C0);
  patchSignStd(ifs, 1, 0xFF6D00);
  patchSignStd(ifs, 2, 0x1004000);
  patchSignStd(ifs, 0, 0x1004180);
  patchSignStd(ifs, 3, 0x10041C0);
  patchSignStd(ifs, 1, 0x1004D00);
  patchSignStd(ifs, 2, 0x1012000);
  patchSignStd(ifs, 0, 0x1012180);
  patchSignStd(ifs, 3, 0x10121C0);
  patchSignStd(ifs, 1, 0x1012D00);
  patchSignStd(ifs, 2, 0x1020000);
  patchSignStd(ifs, 0, 0x1020180);
  patchSignStd(ifs, 3, 0x10201C0);
  patchSignStd(ifs, 1, 0x1020D00);
  patchSignStd(ifs, 3, 0x12307A0);
  patchSignStd(ifs, 0, 0x1231120);
  patchSignStd(ifs, 2, 0x1231CC0);
  patchSignStd(ifs, 1, 0x1231D80);
  patchSignStd(ifs, 0, 0x126A120);
  patchSignStd(ifs, 1, 0x126A160);
  patchSignStd(ifs, 2, 0x126A520);
  patchSignStd(ifs, 3, 0x126A560);
  patchSignStd(ifs, 0, 0x1278120);
  patchSignStd(ifs, 1, 0x1278160);
  patchSignStd(ifs, 2, 0x1278520);
  patchSignStd(ifs, 3, 0x1278560);
  patchSignStd(ifs, 0, 0x1286120);
  patchSignStd(ifs, 1, 0x1286160);
  patchSignStd(ifs, 2, 0x1286520);
  patchSignStd(ifs, 3, 0x1286560);
  patchSignStd(ifs, 0, 0x12A2120);
  patchSignStd(ifs, 1, 0x12A2160);
  patchSignStd(ifs, 2, 0x12A2520);
  patchSignStd(ifs, 3, 0x12A2560);
  patchSignStd(ifs, 0, 0x12B0120);
  patchSignStd(ifs, 1, 0x12B0160);
  patchSignStd(ifs, 2, 0x12B0520);
  patchSignStd(ifs, 3, 0x12B0560);
  patchSignStd(ifs, 3, 0x12CA7A0);
  patchSignStd(ifs, 0, 0x12CB120);
  patchSignStd(ifs, 2, 0x12CBCC0);
  patchSignStd(ifs, 1, 0x12CBD80);
  patchSignPiece(ifs, 3, 0, 0x1302580);
  patchSignPiece(ifs, 3, 1, 0x1302A80);
  patchSignPiece(ifs, 3, 2, 0x1302780);
  patchSignPiece(ifs, 3, 3, 0x1302C80);
  patchSignStd(ifs, 1, 0x1303D80);
  patchSignStd(ifs, 0, 0x1366980);
  patchSignStd(ifs, 3, 0x13669C0);
  patchSignStd(ifs, 0, 0x1374980);
  patchSignStd(ifs, 3, 0x13749C0);
  patchSignStd(ifs, 0, 0x1390980);
  patchSignStd(ifs, 3, 0x13909C0);
  patchSignStd(ifs, 0, 0x13AC980);
  patchSignStd(ifs, 3, 0x13AC9C0);
  patchSignStd(ifs, 0, 0x13BA980);
  patchSignStd(ifs, 3, 0x13BA9C0);
  patchSignStd(ifs, 0, 0x13C8980);
  patchSignStd(ifs, 3, 0x13C89C0);
  patchSignStd(ifs, 0, 0x13D6980);
  patchSignStd(ifs, 3, 0x13D69C0);
  patchSignStd(ifs, 2, 0x13E29A0);
  patchSignStd(ifs, 1, 0x13E3D80);
  patchSignStd(ifs, 0, 0x13E4980);
  patchSignStd(ifs, 3, 0x13E49C0);
  patchSignStd(ifs, 1, 0x148B9C0);
  patchSignStd(ifs, 3, 0x14B47A0);
  patchSignStd(ifs, 0, 0x14B5120);
  patchSignStd(ifs, 2, 0x14B5CC0);
  patchSignStd(ifs, 1, 0x14B5D80);
  patchSignPiece(ifs, 2, 0, 0x14D00E0);
  patchSignPiece(ifs, 2, 1, 0x14D0180);
  patchSignPiece(ifs, 2, 2, 0x14D02E0);
  patchSignPiece(ifs, 2, 3, 0x14D0380);
  patchSignPiece(ifs, 3, 0, 0x14D0580);
  patchSignPiece(ifs, 3, 1, 0x14D0A80);
  patchSignPiece(ifs, 3, 2, 0x14D0780);
  patchSignPiece(ifs, 3, 3, 0x14D0C80);
  patchSignStd(ifs, 2, 0x157E100);
  patchSignStd(ifs, 2, 0x1580100);
  patchSignStd(ifs, 2, 0x15A1C80);
  patchSignStd(ifs, 2, 0x15A3900);
  patchSignStd(ifs, 1, 0x15A3980);
  patchSignStd(ifs, 3, 0x15CBFA0);
  patchSignStd(ifs, 0, 0x15CC920);
  patchSignStd(ifs, 2, 0x15CD4C0);
  patchSignStd(ifs, 1, 0x15CD580);
  patchSignStd(ifs, 0, 0x1659920);
  patchSignStd(ifs, 1, 0x1659960);
  patchSignStd(ifs, 2, 0x1659D20);
  patchSignStd(ifs, 3, 0x1659D60);
  patchSignStd(ifs, 3, 0x1665FA0);
  patchSignStd(ifs, 0, 0x1666920);
  patchSignStd(ifs, 2, 0x16674C0);
  patchSignStd(ifs, 1, 0x1667580);
  patchSignStd(ifs, 3, 0x17EA7A0);
  patchSignStd(ifs, 0, 0x17EB120);
  patchSignStd(ifs, 2, 0x17EBCC0);
  patchSignStd(ifs, 1, 0x17EBD80);
  patchSignPiece(ifs, 1, 0, 0x1869C60);
  patchSignPiece(ifs, 1, 1, 0x1869C80);
  patchSignPiece(ifs, 1, 2, 0x1869CA0);
  patchSignPiece(ifs, 1, 3, 0x1869CC0);
  patchSignStd(ifs, 1, 0x18BC9C0);
  patchSignStd(ifs, 2, 0x18BDC00);
  patchSignStd(ifs, 3, 0x18BDC40);
  patchSignStd(ifs, 1, 0x18CB980);
  patchSignStd(ifs, 0, 0x18CB9C0);
  patchSignStd(ifs, 3, 0x18CBD80);
  patchSignStd(ifs, 2, 0x18CBDC0);
  patchSignStd(ifs, 3, 0x19F07A0);
  patchSignStd(ifs, 0, 0x19F1120);
  patchSignStd(ifs, 2, 0x19F1CC0);
  patchSignStd(ifs, 1, 0x19F1D80);
  patchSignStd(ifs, 1, 0x1A619C0);
  patchSignStd(ifs, 3, 0x1A61DC0);
  patchSignPiece(ifs, 2, 0, 0x1A7C0E0);
  patchSignPiece(ifs, 2, 1, 0x1A7C180);
  patchSignPiece(ifs, 2, 2, 0x1A7C2E0);
  patchSignPiece(ifs, 2, 3, 0x1A7C380);
  patchSignPiece(ifs, 3, 0, 0x1A7C580);
  patchSignPiece(ifs, 3, 1, 0x1A7CA80);
  patchSignPiece(ifs, 3, 2, 0x1A7C780);
  patchSignPiece(ifs, 3, 3, 0x1A7CC80);
  patchSignStd(ifs, 1, 0x1A7DD80);
  patchSignStd(ifs, 1, 0x1A8BD80);
  patchSignPiece(ifs, 1, 0, 0x1B33C60);
  patchSignPiece(ifs, 1, 1, 0x1B33C80);
  patchSignPiece(ifs, 1, 2, 0x1B33CA0);
  patchSignPiece(ifs, 1, 3, 0x1B33CC0);
  patchSignStd(ifs, 1, 0x1B869C0);
  patchSignStd(ifs, 2, 0x1B87C00);
  patchSignStd(ifs, 3, 0x1B87C40);
  patchSignStd(ifs, 1, 0x1CA21C0);
  patchSignStd(ifs, 3, 0x1CA25C0);
  patchSignStd(ifs, 3, 0x1CD8FA0);
  patchSignStd(ifs, 0, 0x1CD9920);
  patchSignStd(ifs, 2, 0x1CDA4C0);
  patchSignStd(ifs, 1, 0x1CDA580);
  patchSignStd(ifs, 3, 0x1D04100);
  patchSignStd(ifs, 2, 0x1D043A0);
  patchSignStd(ifs, 1, 0x1D04500);
  patchSignStd(ifs, 1, 0x1D58580);
  patchSignStd(ifs, 0, 0x1D90180);
  patchSignStd(ifs, 1, 0x1D901C0);
  patchSignStd(ifs, 2, 0x1D90580);
  patchSignStd(ifs, 3, 0x1D905C0);
  patchSignStd(ifs, 2, 0x1D91000);
  patchSignStd(ifs, 0, 0x1D91180);
  patchSignStd(ifs, 3, 0x1D911C0);
  patchSignStd(ifs, 1, 0x1D91D00);
  patchSignStd(ifs, 2, 0x1D9F000);
  patchSignStd(ifs, 0, 0x1D9F180);
  patchSignStd(ifs, 3, 0x1D9F1C0);
  patchSignStd(ifs, 1, 0x1D9FD00);
  patchSignStd(ifs, 2, 0x1DAD000);
  patchSignStd(ifs, 0, 0x1DAD180);
  patchSignStd(ifs, 3, 0x1DAD1C0);
  patchSignStd(ifs, 1, 0x1DADD00);
  patchSignStd(ifs, 2, 0x1DC9000);
  patchSignStd(ifs, 0, 0x1DC9180);
  patchSignStd(ifs, 3, 0x1DC91C0);
  patchSignStd(ifs, 1, 0x1DC9D00);
  patchSignStd(ifs, 2, 0x1E0F000);
  patchSignStd(ifs, 0, 0x1E0F180);
  patchSignStd(ifs, 3, 0x1E0F1C0);
  patchSignStd(ifs, 1, 0x1E0FD00);
  patchSignStd(ifs, 2, 0x1E1D000);
  patchSignStd(ifs, 0, 0x1E1D180);
  patchSignStd(ifs, 3, 0x1E1D1C0);
  patchSignStd(ifs, 1, 0x1E1DD00);
  patchSignStd(ifs, 2, 0x1E2B000);
  patchSignStd(ifs, 0, 0x1E2B180);
  patchSignStd(ifs, 3, 0x1E2B1C0);
  patchSignStd(ifs, 1, 0x1E2BD00);
  patchSignStd(ifs, 2, 0x1E39000);
  patchSignStd(ifs, 0, 0x1E39180);
  patchSignStd(ifs, 3, 0x1E391C0);
  patchSignStd(ifs, 1, 0x1E39D00);
  patchSignPiece(ifs, 1, 0, 0x1E531A0);
  patchSignPiece(ifs, 1, 1, 0x1E531C0);
  patchSignPiece(ifs, 1, 2, 0x1E531E0);
  patchSignPiece(ifs, 1, 3, 0x1E52980);
  patchSignStd(ifs, 3, 0x1E53920);
  patchSignPiece(ifs, 1, 0, 0x1EA71A0);
  patchSignPiece(ifs, 1, 1, 0x1EA71C0);
  patchSignPiece(ifs, 1, 2, 0x1EA71E0);
  patchSignPiece(ifs, 1, 3, 0x1EA6980);
  patchSignStd(ifs, 3, 0x1EA7920);
  patchSignStd(ifs, 3, 0x1EFAFA0);
  patchSignStd(ifs, 0, 0x1EFB920);
  patchSignStd(ifs, 2, 0x1EFC4C0);
  patchSignStd(ifs, 1, 0x1EFC580);
  patchSignStd(ifs, 0, 0x1F6C180);
  patchSignStd(ifs, 1, 0x1F6C1C0);
  patchSignStd(ifs, 2, 0x1F6C580);
  patchSignStd(ifs, 3, 0x1F6C5C0);
  patchSignStd(ifs, 1, 0x22B41C0);
  patchSignStd(ifs, 0, 0x22B4400);
  patchSignStd(ifs, 2, 0x22B4480);
  patchSignStd(ifs, 3, 0x22B44C0);
  patchSignPiece(ifs, 1, 0, 0x22EA9E0);
  patchSignPiece(ifs, 1, 1, 0x22EB1E0);
  patchSignPiece(ifs, 1, 2, 0x22EABE0);
  patchSignPiece(ifs, 1, 3, 0x22EB3E0);
  patchSignStd(ifs, 3, 0x22EB5C0);
  patchSignPiece(ifs, 0, 0, 0x22EB800);
  patchSignPiece(ifs, 0, 1, 0x22EBC00);
  patchSignPiece(ifs, 0, 2, 0x22EBA00);
  patchSignPiece(ifs, 0, 3, 0x22EBE00);
  patchSignStd(ifs, 2, 0x22EB9C0);
  patchSignStd(ifs, 3, 0x2392FA0);
  patchSignStd(ifs, 0, 0x2393920);
  patchSignStd(ifs, 2, 0x23944C0);
  patchSignStd(ifs, 1, 0x2394580);
  patchSignPiece(ifs, 1, 0, 0x23AF1A0);
  patchSignPiece(ifs, 1, 1, 0x23AF1C0);
  patchSignPiece(ifs, 1, 2, 0x23AF1E0);
  patchSignPiece(ifs, 1, 3, 0x23AE980);
  patchSignStd(ifs, 2, 0x2405000);
  patchSignStd(ifs, 0, 0x2405180);
  patchSignStd(ifs, 3, 0x24051C0);
  patchSignStd(ifs, 1, 0x2405D00);
  patchSignStd(ifs, 2, 0x2413000);
  patchSignStd(ifs, 0, 0x2413180);
  patchSignStd(ifs, 3, 0x24131C0);
  patchSignStd(ifs, 1, 0x2413D00);
  patchSignStd(ifs, 2, 0x2421000);
  patchSignStd(ifs, 0, 0x2421180);
  patchSignStd(ifs, 3, 0x24211C0);
  patchSignStd(ifs, 1, 0x2421D00);
  patchSignStd(ifs, 2, 0x243D000);
  patchSignStd(ifs, 0, 0x243D180);
  patchSignStd(ifs, 3, 0x243D1C0);
  patchSignStd(ifs, 1, 0x243DD00);
  patchSignStd(ifs, 2, 0x2467000);
  patchSignStd(ifs, 0, 0x2467180);
  patchSignStd(ifs, 3, 0x24671C0);
  patchSignStd(ifs, 1, 0x2467D00);
  patchSignStd(ifs, 2, 0x2475000);
  patchSignStd(ifs, 0, 0x2475180);
  patchSignStd(ifs, 3, 0x24751C0);
  patchSignStd(ifs, 1, 0x2475D00);
  patchSignStd(ifs, 2, 0x2483000);
  patchSignStd(ifs, 0, 0x2483180);
  patchSignStd(ifs, 3, 0x24831C0);
  patchSignStd(ifs, 1, 0x2483D00);
  patchSignStd(ifs, 2, 0x2491000);
  patchSignStd(ifs, 0, 0x2491180);
  patchSignStd(ifs, 3, 0x24911C0);
  patchSignStd(ifs, 1, 0x2491D00);
  patchSignStd(ifs, 1, 0x24B9BA0);
  patchSignStd(ifs, 0, 0x24B9FC0);
  patchSignStd(ifs, 3, 0x24BA3C0);
  patchSignStd(ifs, 1, 0x2618480);
  patchSignStd(ifs, 1, 0x29411C0);
  patchSignStd(ifs, 0, 0x2941400);
  patchSignStd(ifs, 2, 0x2941480);
  patchSignStd(ifs, 3, 0x29414C0);
  patchSignStd(ifs, 0, 0x29A3180);
  patchSignStd(ifs, 1, 0x29A31C0);
  patchSignStd(ifs, 2, 0x29A3580);
  patchSignStd(ifs, 3, 0x29A35C0);
  patchSignStd(ifs, 0, 0x2A67180);
  patchSignStd(ifs, 1, 0x2A671C0);
  patchSignStd(ifs, 2, 0x2A67580);
  patchSignStd(ifs, 3, 0x2A675C0);
  patchSignStd(ifs, 1, 0x2B0E1C0);
  patchSignStd(ifs, 2, 0x2B0F400);
  patchSignStd(ifs, 3, 0x2B0F440);
  patchSignStd(ifs, 3, 0x2B6FFA0);
  patchSignStd(ifs, 0, 0x2B70920);
  patchSignStd(ifs, 2, 0x2B714C0);
  patchSignStd(ifs, 1, 0x2B71580);
  patchSignStd(ifs, 2, 0x2B9C000);
  patchSignStd(ifs, 0, 0x2B9C180);
  patchSignStd(ifs, 3, 0x2B9C1C0);
  patchSignStd(ifs, 1, 0x2B9CD00);
  patchSignStd(ifs, 2, 0x2BB8000);
  patchSignStd(ifs, 0, 0x2BB8180);
  patchSignStd(ifs, 3, 0x2BB81C0);
  patchSignStd(ifs, 1, 0x2BB8D00);
  patchSignStd(ifs, 2, 0x2BE2000);
  patchSignStd(ifs, 0, 0x2BE2180);
  patchSignStd(ifs, 3, 0x2BE21C0);
  patchSignStd(ifs, 1, 0x2BE2D00);
  patchSignStd(ifs, 2, 0x2BF0000);
  patchSignStd(ifs, 0, 0x2BF0180);
  patchSignStd(ifs, 3, 0x2BF01C0);
  patchSignStd(ifs, 1, 0x2BF0D00);
  patchSignStd(ifs, 2, 0x2BFE000);
  patchSignStd(ifs, 0, 0x2BFE180);
  patchSignStd(ifs, 3, 0x2BFE1C0);
  patchSignStd(ifs, 1, 0x2BFED00);
  patchSignStd(ifs, 2, 0x2C0C000);
  patchSignStd(ifs, 0, 0x2C0C180);
  patchSignStd(ifs, 3, 0x2C0C1C0);
  patchSignStd(ifs, 1, 0x2C0CD00);
  patchSignStd(ifs, 1, 0x2D841C0);
  patchSignStd(ifs, 2, 0x2D85400);
  patchSignStd(ifs, 3, 0x2D85440);
  patchSignPiece(ifs, 3, 0, 0x2D91980);
  patchSignPiece(ifs, 3, 1, 0x2D919C0);
  patchSignPiece(ifs, 3, 2, 0x2D91B80);
  patchSignPiece(ifs, 3, 3, 0x2D91A00);
  patchSignPiece(ifs, 1, 0, 0x2D92280);
  patchSignPiece(ifs, 1, 1, 0x2D918E0);
  patchSignPiece(ifs, 1, 2, 0x2D92480);
  patchSignPiece(ifs, 1, 3, 0x2D91AE0);
  patchSignPiece(ifs, 0, 0, 0x2D92A00);
  patchSignPiece(ifs, 0, 1, 0x2D92E00);
  patchSignPiece(ifs, 0, 2, 0x2D92C00);
  patchSignPiece(ifs, 0, 3, 0x2D92B40);
  patchSignPiece(ifs, 2, 0, 0x2D93C80);
  patchSignPiece(ifs, 2, 1, 0x2D93CA0);
  patchSignPiece(ifs, 2, 2, 0x2D93CC0);
  patchSignPiece(ifs, 2, 3, 0x2D93CE0);
  patchSignPiece(ifs, 2, 0, 0x2DAFC80);
  patchSignPiece(ifs, 2, 1, 0x2DAFCA0);
  patchSignPiece(ifs, 2, 2, 0x2DAFCC0);
  patchSignPiece(ifs, 2, 3, 0x2DAFCE0);
  patchSignPiece(ifs, 2, 0, 0x2DBDC80);
  patchSignPiece(ifs, 2, 1, 0x2DBDCA0);
  patchSignPiece(ifs, 2, 2, 0x2DBDCC0);
  patchSignPiece(ifs, 2, 3, 0x2DBDCE0);
  patchSignPiece(ifs, 2, 0, 0x2DCBC80);
  patchSignPiece(ifs, 2, 1, 0x2DCBCA0);
  patchSignPiece(ifs, 2, 2, 0x2DCBCC0);
  patchSignPiece(ifs, 2, 3, 0x2DCBCE0);
  patchSignPiece(ifs, 2, 0, 0x2DD9C80);
  patchSignPiece(ifs, 2, 1, 0x2DD9CA0);
  patchSignPiece(ifs, 2, 2, 0x2DD9CC0);
  patchSignPiece(ifs, 2, 3, 0x2DD9CE0);
  patchSignPiece(ifs, 2, 0, 0x2DE7C80);
  patchSignPiece(ifs, 2, 1, 0x2DE7CA0);
  patchSignPiece(ifs, 2, 2, 0x2DE7CC0);
  patchSignPiece(ifs, 2, 3, 0x2DE7CE0);
  patchSignPiece(ifs, 2, 0, 0x2DF5C80);
  patchSignPiece(ifs, 2, 1, 0x2DF5CA0);
  patchSignPiece(ifs, 2, 2, 0x2DF5CC0);
  patchSignPiece(ifs, 2, 3, 0x2DF5CE0);
  patchSignPiece(ifs, 2, 0, 0x2E03C80);
  patchSignPiece(ifs, 2, 1, 0x2E03CA0);
  patchSignPiece(ifs, 2, 2, 0x2E03CC0);
  patchSignPiece(ifs, 2, 3, 0x2E03CE0);
  patchSignPiece(ifs, 2, 0, 0x2E11C80);
  patchSignPiece(ifs, 2, 1, 0x2E11CA0);
  patchSignPiece(ifs, 2, 2, 0x2E11CC0);
  patchSignPiece(ifs, 2, 3, 0x2E11CE0);
  patchSignPiece(ifs, 2, 0, 0x2E1FC80);
  patchSignPiece(ifs, 2, 1, 0x2E1FCA0);
  patchSignPiece(ifs, 2, 2, 0x2E1FCC0);
  patchSignPiece(ifs, 2, 3, 0x2E1FCE0);
  patchSignPiece(ifs, 2, 0, 0x2E2DC80);
  patchSignPiece(ifs, 2, 1, 0x2E2DCA0);
  patchSignPiece(ifs, 2, 2, 0x2E2DCC0);
  patchSignPiece(ifs, 2, 3, 0x2E2DCE0);
  patchSignStd(ifs, 3, 0x2E39FA0);
  patchSignStd(ifs, 0, 0x2E3A920);
  patchSignStd(ifs, 2, 0x2E3B4C0);
  patchSignStd(ifs, 1, 0x2E3B580);
  patchSignPiece(ifs, 2, 0, 0x2E3BC80);
  patchSignPiece(ifs, 2, 1, 0x2E3B4E0);
  patchSignPiece(ifs, 2, 2, 0x2E3B6C0);
  patchSignPiece(ifs, 2, 3, 0x2E3B6E0);
  patchSignPiece(ifs, 2, 0, 0x2E49C80);
  patchSignPiece(ifs, 2, 1, 0x2E49CA0);
  patchSignPiece(ifs, 2, 2, 0x2E49CC0);
  patchSignPiece(ifs, 2, 3, 0x2E49CE0);
  patchSignPiece(ifs, 2, 0, 0x2E57C80);
  patchSignPiece(ifs, 2, 1, 0x2E57CA0);
  patchSignPiece(ifs, 2, 2, 0x2E57CC0);
  patchSignPiece(ifs, 2, 3, 0x2E57CE0);
  patchSignStd(ifs, 1, 0x2E65580);
  patchSignStd(ifs, 2, 0x2EBA000);
  patchSignStd(ifs, 0, 0x2EBA180);
  patchSignStd(ifs, 3, 0x2EBA1C0);
  patchSignStd(ifs, 1, 0x2EBAD00);
  patchSignStd(ifs, 2, 0x2EC8000);
  patchSignStd(ifs, 0, 0x2EC8180);
  patchSignStd(ifs, 3, 0x2EC81C0);
  patchSignStd(ifs, 1, 0x2EC8D00);
  patchSignStd(ifs, 2, 0x2ED6000);
  patchSignStd(ifs, 0, 0x2ED6180);
  patchSignStd(ifs, 3, 0x2ED61C0);
  patchSignStd(ifs, 1, 0x2ED6D00);
  patchSignStd(ifs, 2, 0x2EF2000);
  patchSignStd(ifs, 0, 0x2EF2180);
  patchSignStd(ifs, 3, 0x2EF21C0);
  patchSignStd(ifs, 1, 0x2EF2D00);
  patchSignStd(ifs, 3, 0x2FDA7A0);
  patchSignStd(ifs, 0, 0x2FDB120);
  patchSignStd(ifs, 2, 0x2FDBCC0);
  patchSignStd(ifs, 1, 0x2FDBD80);
  patchSignPiece(ifs, 2, 0, 0x30040E0);
  patchSignPiece(ifs, 2, 1, 0x3004180);
  patchSignPiece(ifs, 2, 2, 0x30042E0);
  patchSignPiece(ifs, 2, 3, 0x3004380);
  patchSignPiece(ifs, 3, 0, 0x3004580);
  patchSignPiece(ifs, 3, 1, 0x3004A80);
  patchSignPiece(ifs, 3, 2, 0x3004780);
  patchSignPiece(ifs, 3, 3, 0x3004C80);
  patchSignStd(ifs, 2, 0x3022800);
  patchSignStd(ifs, 0, 0x3022980);
  patchSignStd(ifs, 3, 0x30229C0);
  patchSignStd(ifs, 1, 0x3023500);
  patchSignStd(ifs, 2, 0x3041580);
  patchSignStd(ifs, 2, 0x305A800);
  patchSignStd(ifs, 0, 0x305A980);
  patchSignStd(ifs, 3, 0x305A9C0);
  patchSignStd(ifs, 1, 0x305B500);
  patchSignStd(ifs, 2, 0x3068800);
  patchSignStd(ifs, 0, 0x3068980);
  patchSignStd(ifs, 3, 0x30689C0);
  patchSignStd(ifs, 1, 0x3069500);
  patchSignStd(ifs, 2, 0x3087900);
  patchSignStd(ifs, 2, 0x30A0800);
  patchSignStd(ifs, 0, 0x30A0980);
  patchSignStd(ifs, 3, 0x30A09C0);
  patchSignStd(ifs, 1, 0x30A1500);
  patchSignStd(ifs, 5, 0x30BF120);
  patchSignStd(ifs, 1, 0x30BFD40);
  patchSignStd(ifs, 2, 0x30BFD80);
  patchSignStd(ifs, 2, 0x30D8800);
  patchSignStd(ifs, 0, 0x30D8980);
  patchSignStd(ifs, 3, 0x30D89C0);
  patchSignStd(ifs, 1, 0x30D9500);
  patchSignStd(ifs, 2, 0x30E6800);
  patchSignStd(ifs, 0, 0x30E6980);
  patchSignStd(ifs, 3, 0x30E69C0);
  patchSignStd(ifs, 1, 0x30E7500);
  patchSignStd(ifs, 2, 0x312C800);
  patchSignStd(ifs, 0, 0x312C980);
  patchSignStd(ifs, 3, 0x312C9C0);
  patchSignStd(ifs, 1, 0x312D500);
  patchSignStd(ifs, 0, 0x3139980);
  patchSignStd(ifs, 1, 0x31399C0);
  patchSignStd(ifs, 2, 0x3139D80);
  patchSignStd(ifs, 3, 0x3139DC0);
  patchSignStd(ifs, 4, 0x313A180);
  patchSignStd(ifs, 5, 0x313A1C0);
  patchSignStd(ifs, 4, 0x3148180);
  patchSignStd(ifs, 4, 0x3156180);
  patchSignStd(ifs, 4, 0x3164180);
  patchSignStd(ifs, 4, 0x3172180);
  patchSignStd(ifs, 4, 0x3180180);
  patchSignStd(ifs, 4, 0x318E180);
  patchSignStd(ifs, 4, 0x31AA180);
  patchSignStd(ifs, 4, 0x31B8180);
  patchSignStd(ifs, 4, 0x31C6180);
  patchSignStd(ifs, 4, 0x31D4180);
  patchSignStd(ifs, 4, 0x31E2180);
  patchSignStd(ifs, 4, 0x31F0180);
  patchSignStd(ifs, 4, 0x31FE180);
  patchSignStd(ifs, 4, 0x320C180);
  patchSignStd(ifs, 2, 0x320F480);
  patchSignStd(ifs, 4, 0x321A180);
  patchSignStd(ifs, 4, 0x3252180);
  patchSignStd(ifs, 4, 0x3260180);
  patchSignStd(ifs, 4, 0x326E180);
  patchSignStd(ifs, 4, 0x327C180);
  patchSignStd(ifs, 4, 0x328A180);
  patchSignStd(ifs, 4, 0x3298180);
  patchSignStd(ifs, 4, 0x32A6180);
  patchSignStd(ifs, 4, 0x32B4180);
  patchSignStd(ifs, 1, 0x6487D00);
  patchSignStd(ifs, 5, 0x6487D80);
  patchSignStd(ifs, 1, 0x649F580);
  patchSignStd(ifs, 3, 0x64A47A0);
  patchSignStd(ifs, 0, 0x64A5120);
  patchSignStd(ifs, 2, 0x64A5CC0);
  patchSignStd(ifs, 1, 0x64A5D80);
  patchSignStd(ifs, 0, 0x64BF980);
  patchSignStd(ifs, 1, 0x64BF9C0);
  patchSignStd(ifs, 2, 0x64BFD80);
  patchSignStd(ifs, 3, 0x64BFDC0);
  patchSignStd(ifs, 3, 0x64E37A0);
  patchSignStd(ifs, 0, 0x64E4120);
  patchSignStd(ifs, 2, 0x64E4CC0);
  patchSignStd(ifs, 1, 0x64E4D80);
  patchSignStd(ifs, 0, 0x64F4980);
  patchSignStd(ifs, 1, 0x64F49C0);
  patchSignStd(ifs, 2, 0x64F4D80);
  patchSignStd(ifs, 3, 0x64F4DC0);
  patchSignStd(ifs, 3, 0x6514900);
  patchSignStd(ifs, 1, 0x6514D00);
  patchSignStd(ifs, 1, 0x6528120);
  patchSignStd(ifs, 2, 0x6533040);
  patchSignStd(ifs, 3, 0x6533080);
  patchSignStd(ifs, 0, 0x6533400);
  patchSignStd(ifs, 1, 0x6533440);
  patchSignStd(ifs, 1, 0x653D920);
  patchSignStd(ifs, 5, 0x6546120);
  patchSignStd(ifs, 3, 0x65577A0);
  patchSignStd(ifs, 0, 0x6558120);
  patchSignStd(ifs, 2, 0x6558CC0);
  patchSignStd(ifs, 1, 0x6558D80);
  patchSignPiece(ifs, 3, 0, 0x6573580);
  patchSignPiece(ifs, 3, 1, 0x6573A80);
  patchSignPiece(ifs, 3, 2, 0x6573780);
  patchSignPiece(ifs, 3, 3, 0x6573C80);
  patchSignStd(ifs, 0, 0x65737A0);
  patchSignStd(ifs, 1, 0x6574D80);
  patchSignStd(ifs, 3, 0x65D37A0);
  patchSignStd(ifs, 0, 0x65D4120);
  patchSignStd(ifs, 2, 0x65D4CC0);
  patchSignStd(ifs, 1, 0x65D4D80);
  patchSignPiece(ifs, 2, 0, 0x65D6180);
  patchSignPiece(ifs, 2, 1, 0x65D6580);
  patchSignPiece(ifs, 2, 2, 0x65D6380);
  patchSignPiece(ifs, 2, 3, 0x65D69C0);
  patchSignStd(ifs, 1, 0x65EE540);
  patchSignStd(ifs, 2, 0x65EE580);
  patchSignStd(ifs, 3, 0x65FF7A0);
  patchSignStd(ifs, 0, 0x6600120);
  patchSignStd(ifs, 2, 0x6600CC0);
  patchSignStd(ifs, 1, 0x6600D80);
  patchSignStd(ifs, 1, 0x6606D40);
  patchSignPiece(ifs, 3, 0, 0x660F580);
  patchSignPiece(ifs, 3, 1, 0x660FA80);
  patchSignPiece(ifs, 3, 2, 0x660F780);
  patchSignPiece(ifs, 3, 3, 0x660FC80);
  patchSignStd(ifs, 1, 0x6610D80);
  patchSignStd(ifs, 1, 0x6616CC0);
  patchSignStd(ifs, 2, 0x662B9A0);
  patchSignStd(ifs, 1, 0x662CD80);
  patchSignStd(ifs, 2, 0x6632480);
  patchSignStd(ifs, 1, 0x66689C0);
  patchSignStd(ifs, 3, 0x667F7A0);
  patchSignStd(ifs, 0, 0x6680120);
  patchSignStd(ifs, 2, 0x6680CC0);
  patchSignStd(ifs, 1, 0x6680D80);
  patchSignStd(ifs, 2, 0x6682100);
  patchSignStd(ifs, 1, 0x6682180);
  patchSignPiece(ifs, 2, 0, 0x668B0E0);
  patchSignPiece(ifs, 2, 1, 0x668B180);
  patchSignPiece(ifs, 2, 2, 0x668B2E0);
  patchSignPiece(ifs, 2, 3, 0x668B380);
  patchSignPiece(ifs, 3, 0, 0x668B580);
  patchSignPiece(ifs, 3, 1, 0x668BA80);
  patchSignPiece(ifs, 3, 2, 0x668B780);
  patchSignPiece(ifs, 3, 3, 0x668BC80);
  patchSignStd(ifs, 0, 0x668B7A0);
  patchSignStd(ifs, 0, 0x66A3120);
  patchSignStd(ifs, 1, 0x66A3160);
  patchSignStd(ifs, 2, 0x66A3520);
  patchSignStd(ifs, 3, 0x66A3560);
  patchSignStd(ifs, 0, 0x66A5980);
  patchSignStd(ifs, 3, 0x66A59C0);
  patchSignStd(ifs, 3, 0x66CA7A0);
  patchSignStd(ifs, 0, 0x66CB120);
  patchSignStd(ifs, 2, 0x66CBCC0);
  patchSignStd(ifs, 1, 0x66CBD80);
  patchSignStd(ifs, 1, 0x66DD480);
  patchSignPiece(ifs, 1, 0, 0x66E3C60);
  patchSignPiece(ifs, 1, 1, 0x66E3C80);
  patchSignPiece(ifs, 1, 2, 0x66E3CA0);
  patchSignPiece(ifs, 1, 3, 0x66E3CC0);
  patchSignStd(ifs, 1, 0x66EA9C0);
  patchSignStd(ifs, 2, 0x66EBC00);
  patchSignStd(ifs, 3, 0x66EBC40);
  patchSignStd(ifs, 1, 0x6703980);
  patchSignStd(ifs, 0, 0x67039C0);
  patchSignStd(ifs, 3, 0x6703D80);
  patchSignStd(ifs, 2, 0x6703DC0);
  patchSignPiece(ifs, 2, 0, 0x67054C0);
  patchSignPiece(ifs, 2, 1, 0x67055C0);
  patchSignPiece(ifs, 2, 2, 0x67056C0);
  patchSignPiece(ifs, 2, 3, 0x67057C0);
  patchSignStd(ifs, 3, 0x674A7A0);
  patchSignStd(ifs, 0, 0x674B120);
  patchSignStd(ifs, 2, 0x674BCC0);
  patchSignStd(ifs, 1, 0x674BD80);
  patchSignStd(ifs, 1, 0x675F9C0);
  patchSignStd(ifs, 3, 0x675FDC0);
  patchSignStd(ifs, 2, 0x6764900);
  patchSignStd(ifs, 1, 0x6764980);
  patchSignPiece(ifs, 2, 0, 0x67720E0);
  patchSignPiece(ifs, 2, 1, 0x6772180);
  patchSignPiece(ifs, 2, 2, 0x67722E0);
  patchSignPiece(ifs, 2, 3, 0x6772380);
  patchSignPiece(ifs, 3, 0, 0x6772580);
  patchSignPiece(ifs, 3, 1, 0x6772A80);
  patchSignPiece(ifs, 3, 2, 0x6772780);
  patchSignPiece(ifs, 3, 3, 0x6772C80);
  patchSignStd(ifs, 1, 0x6773D80);
  patchSignStd(ifs, 3, 0x6774FA0);
  patchSignStd(ifs, 0, 0x6775920);
  patchSignStd(ifs, 1, 0x6787D80);
  patchSignStd(ifs, 1, 0x67951C0);
  patchSignStd(ifs, 1, 0x67A81C0);
  patchSignStd(ifs, 3, 0x67A85C0);
  patchSignStd(ifs, 3, 0x67AEFA0);
  patchSignStd(ifs, 0, 0x67AF920);
  patchSignStd(ifs, 2, 0x67B04C0);
  patchSignStd(ifs, 1, 0x67B0580);
  patchSignStd(ifs, 1, 0x67B1C80);
  patchSignStd(ifs, 5, 0x67B1D00);
  patchSignStd(ifs, 2, 0x67BE100);
  patchSignStd(ifs, 1, 0x67BE180);
  patchSignStd(ifs, 3, 0x67C4100);
  patchSignStd(ifs, 2, 0x67C43A0);
  patchSignStd(ifs, 1, 0x67C4500);
  patchSignStd(ifs, 1, 0x67D4580);
  patchSignStd(ifs, 2, 0x67EE1A0);
  patchSignPiece(ifs, 1, 0, 0x68071A0);
  patchSignPiece(ifs, 1, 1, 0x68071C0);
  patchSignPiece(ifs, 1, 2, 0x68071E0);
  patchSignPiece(ifs, 1, 3, 0x6806980);
  patchSignStd(ifs, 3, 0x6807920);
  patchSignPiece(ifs, 1, 0, 0x681F1A0);
  patchSignPiece(ifs, 1, 1, 0x681F1C0);
  patchSignPiece(ifs, 1, 2, 0x681F1E0);
  patchSignPiece(ifs, 1, 3, 0x681E980);
  patchSignStd(ifs, 3, 0x681F920);
  patchSignStd(ifs, 2, 0x6825100);
  patchSignStd(ifs, 1, 0x6825180);
  patchSignStd(ifs, 3, 0x6836FA0);
  patchSignStd(ifs, 0, 0x6837920);
  patchSignStd(ifs, 2, 0x68384C0);
  patchSignStd(ifs, 1, 0x6838580);
  patchSignStd(ifs, 0, 0x6844180);
  patchSignStd(ifs, 1, 0x68441C0);
  patchSignStd(ifs, 2, 0x6844580);
  patchSignStd(ifs, 3, 0x68445C0);
  patchSignStd(ifs, 1, 0x687DD40);
  patchSignStd(ifs, 1, 0x68839C0);
  patchSignStd(ifs, 0, 0x6883C00);
  patchSignStd(ifs, 2, 0x6883C80);
  patchSignStd(ifs, 3, 0x6883CC0);
  patchSignStd(ifs, 2, 0x688D9A0);
  patchSignPiece(ifs, 1, 0, 0x68921E0);
  patchSignPiece(ifs, 1, 1, 0x68929E0);
  patchSignPiece(ifs, 1, 2, 0x68923E0);
  patchSignPiece(ifs, 1, 3, 0x6892BE0);
  patchSignStd(ifs, 3, 0x6892DC0);
  patchSignPiece(ifs, 0, 0, 0x6893000);
  patchSignPiece(ifs, 0, 1, 0x6893400);
  patchSignPiece(ifs, 0, 2, 0x6893200);
  patchSignPiece(ifs, 0, 3, 0x6893600);
  patchSignStd(ifs, 2, 0x68931C0);
  patchSignStd(ifs, 2, 0x68AD9A0);
  patchSignStd(ifs, 3, 0x68B67A0);
  patchSignStd(ifs, 0, 0x68B7120);
  patchSignStd(ifs, 2, 0x68B7CC0);
  patchSignStd(ifs, 1, 0x68B7D80);
  patchSignStd(ifs, 2, 0x68BD9A0);
  patchSignPiece(ifs, 1, 0, 0x68C69A0);
  patchSignPiece(ifs, 1, 1, 0x68C69C0);
  patchSignPiece(ifs, 1, 2, 0x68C69E0);
  patchSignPiece(ifs, 1, 3, 0x68C6180);
  patchSignStd(ifs, 2, 0x68CC9A0);
  patchSignStd(ifs, 1, 0x68CDD80);
  patchSignStd(ifs, 1, 0x68D1D80);
  patchSignStd(ifs, 1, 0x68DB3A0);
  patchSignStd(ifs, 0, 0x68DB7C0);
  patchSignStd(ifs, 3, 0x68DBBC0);
  patchSignStd(ifs, 1, 0x68F9580);
  patchSignStd(ifs, 2, 0x69051A0);
  patchSignStd(ifs, 1, 0x6949480);
  patchSignStd(ifs, 1, 0x69C31C0);
  patchSignStd(ifs, 0, 0x69C3400);
  patchSignStd(ifs, 2, 0x69C3480);
  patchSignStd(ifs, 3, 0x69C34C0);
  patchSignStd(ifs, 1, 0x69C8D80);
  patchSignStd(ifs, 2, 0x69E11A0);
  patchSignStd(ifs, 0, 0x69E3180);
  patchSignStd(ifs, 1, 0x69E31C0);
  patchSignStd(ifs, 2, 0x69E3580);
  patchSignStd(ifs, 3, 0x69E35C0);
  patchSignStd(ifs, 2, 0x69E51A0);
  patchSignStd(ifs, 2, 0x69EC900);
  patchSignStd(ifs, 1, 0x69EC980);
  patchSignStd(ifs, 2, 0x69F49A0);
  patchSignStd(ifs, 2, 0x6A0C9A0);
  patchSignStd(ifs, 0, 0x6A1B180);
  patchSignStd(ifs, 1, 0x6A1B1C0);
  patchSignStd(ifs, 2, 0x6A1B580);
  patchSignStd(ifs, 3, 0x6A1B5C0);
  patchSignStd(ifs, 1, 0x6A461C0);
  patchSignStd(ifs, 2, 0x6A47400);
  patchSignStd(ifs, 3, 0x6A47440);
  patchSignStd(ifs, 3, 0x6A61FA0);
  patchSignStd(ifs, 0, 0x6A62920);
  patchSignStd(ifs, 2, 0x6A634C0);
  patchSignStd(ifs, 1, 0x6A63580);
  patchSignStd(ifs, 1, 0x6AA21C0);
  patchSignStd(ifs, 2, 0x6AA3400);
  patchSignStd(ifs, 3, 0x6AA3440);
  patchSignPiece(ifs, 3, 0, 0x6AA5980);
  patchSignPiece(ifs, 3, 1, 0x6AA59C0);
  patchSignPiece(ifs, 3, 2, 0x6AA5B80);
  patchSignPiece(ifs, 3, 3, 0x6AA5A00);
  patchSignPiece(ifs, 1, 0, 0x6AA6280);
  patchSignPiece(ifs, 1, 1, 0x6AA58E0);
  patchSignPiece(ifs, 1, 2, 0x6AA6480);
  patchSignPiece(ifs, 1, 3, 0x6AA5AE0);
  patchSignPiece(ifs, 0, 0, 0x6AA6A00);
  patchSignPiece(ifs, 0, 1, 0x6AA6E00);
  patchSignPiece(ifs, 0, 2, 0x6AA6C00);
  patchSignPiece(ifs, 0, 3, 0x6AA6B40);
  patchSignPiece(ifs, 2, 0, 0x6AA7C80);
  patchSignPiece(ifs, 2, 1, 0x6AA7CA0);
  patchSignPiece(ifs, 2, 2, 0x6AA7CC0);
  patchSignPiece(ifs, 2, 3, 0x6AA7CE0);
  patchSignStd(ifs, 3, 0x6AD5FA0);
  patchSignStd(ifs, 0, 0x6AD6920);
  patchSignStd(ifs, 2, 0x6AD74C0);
  patchSignStd(ifs, 1, 0x6AD7580);
  patchSignStd(ifs, 1, 0x6AE3580);
  patchSignStd(ifs, 3, 0x6B037A0);
  patchSignStd(ifs, 0, 0x6B04120);
  patchSignStd(ifs, 2, 0x6B04CC0);
  patchSignStd(ifs, 1, 0x6B04D80);
  patchSignStd(ifs, 1, 0x6B061C0);
  patchSignPiece(ifs, 2, 0, 0x6B0F0E0);
  patchSignPiece(ifs, 2, 1, 0x6B0F180);
  patchSignPiece(ifs, 2, 2, 0x6B0F2E0);
  patchSignPiece(ifs, 2, 3, 0x6B0F380);
  patchSignPiece(ifs, 3, 0, 0x6B0F580);
  patchSignPiece(ifs, 3, 1, 0x6B0FA80);
  patchSignPiece(ifs, 3, 2, 0x6B0F780);
  patchSignPiece(ifs, 3, 3, 0x6B0FC80);
  patchSignStd(ifs, 1, 0x6B16D80);
  patchSignStd(ifs, 1, 0x6B26580);
  patchSignStd(ifs, 1, 0x6B4A580);
  patchSignStd(ifs, 0, 0x6B68980);
  patchSignStd(ifs, 1, 0x6B689C0);
  patchSignStd(ifs, 2, 0x6B68D80);
  patchSignStd(ifs, 3, 0x6B68DC0);
  patchSignStd(ifs, 0, 0x6B6C980);
  patchSignStd(ifs, 1, 0x6B6C9C0);
  patchSignStd(ifs, 2, 0x6B6CD80);
  patchSignStd(ifs, 3, 0x6B6CDC0);
  patchSignStd(ifs, 2, 0x6B98900);
  patchSignStd(ifs, 5, 0x6B9DC20);
  patchSignStd(ifs, 5, 0x6B9FC20);
  patchSignStd(ifs, 5, 0x6BABC20);
  patchSignStd(ifs, 1, 0x6BBD580);
  patchSignStd(ifs, 1, 0x6BDE1C0);
  patchSignStd(ifs, 2, 0x6BDE580);
  patchSignStd(ifs, 1, 0x6BF5580);
  patchSignStd(ifs, 1, 0x6C09D00);
  patchSignStd(ifs, 5, 0x6C09D80);
  patchSignStd(ifs, 1, 0x6C1BD80);
  patchSignStd(ifs, 1, 0x6C21580);
  patchSignStd(ifs, 2, 0x6C2F1A0);
  patchSignStd(ifs, 1, 0x6C30580);
  patchSignStd(ifs, 2, 0x6C319A0);
  patchSignStd(ifs, 1, 0x6C32D80);
  patchSignStd(ifs, 2, 0x6C359A0);
  patchSignStd(ifs, 1, 0x6C36D80);
  patchSignStd(ifs, 2, 0x6C389A0);
  patchSignStd(ifs, 2, 0x6C449A0);
  patchSignStd(ifs, 1, 0x6C45D80);
  patchSignStd(ifs, 2, 0x6C4A9A0);
  patchSignStd(ifs, 1, 0x6C4BD80);
  patchSignStd(ifs, 2, 0x6C519A0);
  patchSignStd(ifs, 1, 0x6C52D80);
  patchSignStd(ifs, 1, 0x6C5BC80);
  patchSignStd(ifs, 0, 0x6D27180);
  patchSignStd(ifs, 1, 0x6D271C0);
  patchSignStd(ifs, 2, 0x6D27580);
  patchSignStd(ifs, 3, 0x6D275C0);
  patchSignStd(ifs, 3, 0x6DBFFA0);
  patchSignStd(ifs, 0, 0x6DC0920);
  patchSignStd(ifs, 2, 0x6DC14C0);
  patchSignStd(ifs, 1, 0x6DC1580);
  patchSignPiece(ifs, 2, 0, 0x6DE98E0);
  patchSignPiece(ifs, 2, 1, 0x6DE9980);
  patchSignPiece(ifs, 2, 2, 0x6DE9AE0);
  patchSignPiece(ifs, 2, 3, 0x6DE9B80);
  patchSignPiece(ifs, 3, 0, 0x6DE9D80);
  patchSignPiece(ifs, 3, 1, 0x6DEA280);
  patchSignPiece(ifs, 3, 2, 0x6DE9F80);
  patchSignPiece(ifs, 3, 3, 0x6DEA480);
  patchSignStd(ifs, 0, 0x6DE9FA0);
  patchSignStd(ifs, 1, 0x6DEB580);
  patchSignPiece(ifs, 2, 0, 0x701B8E0);
  patchSignPiece(ifs, 2, 1, 0x701B980);
  patchSignPiece(ifs, 2, 2, 0x701BAE0);
  patchSignPiece(ifs, 2, 3, 0x701BB80);
  patchSignPiece(ifs, 3, 0, 0x701BD80);
  patchSignPiece(ifs, 3, 1, 0x701C280);
  patchSignPiece(ifs, 3, 2, 0x701BF80);
  patchSignPiece(ifs, 3, 3, 0x701C480);
  patchSignStd(ifs, 0, 0x701BFA0);
  patchSignStd(ifs, 1, 0x701D580);
  patchSignStd(ifs, 2, 0x708C000);
  patchSignStd(ifs, 0, 0x708C180);
  patchSignStd(ifs, 3, 0x708C1C0);
  patchSignStd(ifs, 1, 0x708CD00);
  patchSignStd(ifs, 2, 0x709A000);
  patchSignStd(ifs, 0, 0x709A180);
  patchSignStd(ifs, 3, 0x709A1C0);
  patchSignStd(ifs, 1, 0x709AD00);
  patchSignStd(ifs, 2, 0x70A8000);
  patchSignStd(ifs, 0, 0x70A8180);
  patchSignStd(ifs, 3, 0x70A81C0);
  patchSignStd(ifs, 1, 0x70A8D00);
  patchSignPiece(ifs, 2, 0, 0x70DD8E0);
  patchSignPiece(ifs, 2, 1, 0x70DD980);
  patchSignPiece(ifs, 2, 2, 0x70DDAE0);
  patchSignPiece(ifs, 2, 3, 0x70DDB80);
  patchSignPiece(ifs, 3, 0, 0x70DDD80);
  patchSignPiece(ifs, 3, 1, 0x70DE280);
  patchSignPiece(ifs, 3, 2, 0x70DDF80);
  patchSignPiece(ifs, 3, 3, 0x70DE480);
  patchSignStd(ifs, 0, 0x70DDFA0);
  patchSignStd(ifs, 1, 0x70DF580);
  patchSignPiece(ifs, 2, 0, 0x714FC80);
  patchSignPiece(ifs, 2, 1, 0x714FCA0);
  patchSignPiece(ifs, 2, 2, 0x714FCC0);
  patchSignPiece(ifs, 2, 3, 0x714FCE0);
  patchSignStd(ifs, 1, 0x71B2920);
  patchSignStd(ifs, 2, 0x7233040);
  patchSignStd(ifs, 3, 0x7233080);
  patchSignStd(ifs, 0, 0x7233400);
  patchSignStd(ifs, 1, 0x7233440);
  patchSignStd(ifs, 4, 0x723B980);
  patchSignStd(ifs, 5, 0x723B9C0);
  patchSignStd(ifs, 4, 0x723D980);
  patchSignStd(ifs, 0, 0x728F180);
  patchSignStd(ifs, 1, 0x728F1C0);
  patchSignStd(ifs, 2, 0x728F580);
  patchSignStd(ifs, 3, 0x728F5C0);
  patchSignStd(ifs, 4, 0x728F980);
  patchSignStd(ifs, 4, 0x7361980);
  patchSignStd(ifs, 4, 0x736F980);
  patchSignStd(ifs, 2, 0x7400000);
  patchSignStd(ifs, 0, 0x7400180);
  patchSignStd(ifs, 3, 0x74001C0);
  patchSignStd(ifs, 1, 0x7400D00);
  patchSignStd(ifs, 2, 0x740D000);
  patchSignStd(ifs, 0, 0x740D180);
  patchSignStd(ifs, 3, 0x740D1C0);
  patchSignStd(ifs, 1, 0x740DD00);
  patchSignStd(ifs, 2, 0x741A000);
  patchSignStd(ifs, 0, 0x741A180);
  patchSignStd(ifs, 3, 0x741A1C0);
  patchSignStd(ifs, 1, 0x741AD00);
  patchSignStd(ifs, 2, 0x7427000);
  patchSignStd(ifs, 0, 0x7427180);
  patchSignStd(ifs, 3, 0x74271C0);
  patchSignStd(ifs, 1, 0x7427D00);
  patchSignStd(ifs, 2, 0x79AD000);
  patchSignStd(ifs, 0, 0x79AD180);
  patchSignStd(ifs, 3, 0x79AD1C0);
  patchSignStd(ifs, 1, 0x79ADD00);
  patchSignStd(ifs, 2, 0x79BA000);
  patchSignStd(ifs, 0, 0x79BA180);
  patchSignStd(ifs, 3, 0x79BA1C0);
  patchSignStd(ifs, 1, 0x79BAD00);
  patchSignStd(ifs, 2, 0x79C7000);
  patchSignStd(ifs, 0, 0x79C7180);
  patchSignStd(ifs, 3, 0x79C71C0);
  patchSignStd(ifs, 1, 0x79C7D00);
}

void patchFile(TBufStream& ofs,
               std::string filename,
               int offset,
               int sizeLimit = -1) {
  if (!TFileManip::fileExists(filename)) {
    throw TGenericException(T_SRCANDLINE,
                            "patchFile()",
                            std::string("File does not exist: ")
                              + filename);
  }
  
  std::cout << "patching '" << filename << "' to "
    << TStringConversion::intToString(offset,
        TStringConversion::baseHex)
    << std::endl;
  
  TBufStream ifs;
  ifs.open(filename.c_str());
  
  if (sizeLimit == -1) sizeLimit = ifs.size();
  
  ofs.seek(offset);
  ofs.writeFrom(ifs, sizeLimit);
}

void patchFileBySector(TBufStream& ofs,
               std::string filename,
               int sectorNum,
               int sizeLimit = -1) {
  patchFile(ofs, filename, sectorNum * sectorSize, sizeLimit);
}

int getFileNameSectorNum(std::string filename) {
  int hyphenPos = filename.size() - 1;
  for ( ; hyphenPos >= 0; hyphenPos--) {
    if (filename[hyphenPos] == '-') break;
  }
  
  if (hyphenPos < 0) return -1;
  
  std::string valueStr = filename.substr(hyphenPos + 1, std::string::npos);
  return TStringConversion::stringToInt(valueStr);
}

#if ADD_BUILD_STAMP
void writeBuildString(TBufStream& ofs, int pos, std::string buildIdStr) {
  ofs.seek(pos);
  for (int i = 0; i < buildIdStr.size(); i++) {
    int raw = (unsigned char)buildIdStr[i];
    // if space or lower case
    if ((raw == 0x20) || (raw >= 0x60)) raw += 0x50;
    
    // palette
    int baseValue = 0x1000;
    int outValue = 0x1000 + raw + 0x90;
    ofs.writeu16le(outValue);
  }
}
#endif

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Tengai Makyou Ziria ISO patcher" << endl;
    cout << "Usage: " << argv[0]
      << " <infile> <outfile>" << endl;
    return 0;
  }
  
  string infile(argv[1]);
  string outfile(argv[2]);
  
  TBufStream ofs;
  ofs.open(infile.c_str());
  
  //=======================
  // main code/data area
  //=======================
  
  patchFileBySector(ofs, "out/base/empty_2.bin", 0x2);
  patchFileBySector(ofs, "out/base/kernel_9.bin", 0x9);
  patchFileBySector(ofs, "out/base/overw_13.bin", 0x13);
  patchFileBySector(ofs, "out/base/shop_19.bin", 0x19);
  patchFileBySector(ofs, "out/base/battle_1D.bin", 0x1D);
  patchFileBySector(ofs, "out/base/unknown1_23.bin", 0x23);
  patchFileBySector(ofs, "out/base/battle_extra_27.bin", 0x27);
  patchFileBySector(ofs, "out/base/battle_text_2D.bin", 0x2D);
  patchFileBySector(ofs, "out/base/battle_text2_31.bin", 0x31);
  patchFileBySector(ofs, "out/base/unknown2_35.bin", 0x35);
  patchFileBySector(ofs, "out/base/intro_3B.bin", 0x3B);
  patchFileBySector(ofs, "out/base/saveload_45.bin", 0x45);
  patchFileBySector(ofs, "out/base/generic_text_4B.bin", 0x4B);
  patchFileBySector(ofs, "out/base/credits_4F.bin", 0x4F);
  patchFileBySector(ofs, "out/base/special_51.bin", 0x51);
  patchFileBySector(ofs, "out/base/unknown3_57.bin", 0x57);
  patchFileBySector(ofs, "out/base/info_5B.bin", 0x5B);
  patchFileBySector(ofs, "out/base/masakado_5F.bin", 0x5F);
  
  //=======================
  // new resources
  //=======================
  
  patchFileBySector(ofs, "out/base/new1.bin", 0x2);
  patchFileBySector(ofs, "out/base/new2.bin", 0x6);
/*  // overwrites unused map
  patchFileBySector(ofs, "out/base/new3.bin", 0x2B6B);
  
  // overwrites unused map
  patchFileBySector(ofs, "out/grp/theend_all_raw.bin", 0x4BC9);
  
  // overwrites unused map
  patchFileBySector(ofs, "out/base/new4.bin", 0x4C55, 0x1000);
  patchFileBySector(ofs, "out/grp/scderror_vram.bin", 0x4C57, 0x5000);*/
  
  // instead of unused maps, let's overwrite some unused adpcm data
  // that's closer to the start of the disc
  // (which is probably the result of that part of the disc not
  // being assigned any content and keeping whatever was there in
  // old versions of the game)
  patchFileBySector(ofs, "out/base/new3.bin", 0x10B0);
  patchFileBySector(ofs, "out/grp/theend_all_raw.bin", 0x10B0+0x4);
  patchFileBySector(ofs, "out/base/new4.bin", 0x10B0+0x9, 0x1000);
  patchFileBySector(ofs, "out/grp/scderror_vram.bin", 0x10B0+0xB, 0x5000);
  
  //=======================
  // graphics
  //=======================
  
  patchFile(ofs, "out/grp/interface3.bin", 0x6D02200, 0x1C00);
  // interface1 is loaded if you don't skip the intro,
  // so we need to overwrite it with the identical content
  // from interface3.
  // yeah, i dunno.
  patchFile(ofs, "out/grp/interface3.bin", 0x3B000, 0x1C00);
  
  patchFile(ofs, "out/grp/gamble_dice.bin", 0x6CA5000, 0xC00);
  patchFile(ofs, "out/grp/gamble_bg.bin", 0x6CA1000, 0x3000);
  
  patchFile(ofs, "out/rsrc_raw/grp/title_vram_bg_6000.bin", 0x6D07000);
  patchFile(ofs, "out/rsrc_raw/grp/title_vram_bg_8000.bin", 0x6D09000);
  patchFile(ofs, "out/rsrc_raw/grp/title_vram_sprites_2e00.bin", 0x6D03E00);
  patchFile(ofs, "out/rsrc_raw/grp/title_vram_sprites_7200.bin", 0x6D08200);
  
  patchFile(ofs, "out/grp/pyramid_n.bin", 0x1F36880);
  patchFile(ofs, "out/grp/pyramid_n.bin", 0x1F44880);
  patchFile(ofs, "out/grp/pyramid_n.bin", 0x1F52880);
  
  patchFile(ofs, "out/grp/stonemason_bonus.bin", 0x31ACB00);
  patchFile(ofs, "out/grp/stonemason_bonus.bin", 0x6F76300);
  
  patchFile(ofs, "out/rsrc_raw/grp/plans.bin", 0x75E3800);
  
  patchFile(ofs, "out/rsrc_raw/grp/omake.bin", 0x5CED800);
  
  // TODO: which of these is actually used?
  patchFile(ofs, "rsrc_raw/pal/theend_line_mod.pal", 0x5B39000);
  patchFile(ofs, "rsrc_raw/pal/theend_line_mod.pal", 0x5B39800);
  patchFile(ofs, "rsrc_raw/pal/theend_line_mod.pal", 0x5B3A000);
  
  patchFile(ofs, "out/grp/theater_credits_raw_1.bin", 0x637E800, 0x2000);
  patchFile(ofs, "out/grp/theater_credits_raw_2.bin", 0x6381000, 0x2000);
  
  patchFile(ofs, "out/grp/theater_acts_1_raw.bin", 0x6377000);
  patchFile(ofs, "out/grp/theater_acts_2_raw.bin", 0x6379800);
  patchFile(ofs, "out/grp/theater_acts_3_raw.bin", 0x637C000);
  
  patchFile(ofs, "out/rsrc_raw/pal/theater_acts_sprite_mod_cropped.pal", 0x6379002);
  // used? relevant?
//  patchFile(ofs, "out/rsrc_raw/pal/theater_acts_sprite_mod_cropped.pal", 0x637B802);
//  patchFile(ofs, "out/rsrc_raw/pal/theater_acts_sprite_mod_cropped.pal", 0x637E002);
  // used? relevant?
//  patchFile(ofs, "out/rsrc_raw/pal/theater_acts_sprite_mod_cropped.pal", 0x6380802);
//  patchFile(ofs, "out/rsrc_raw/pal/theater_acts_sprite_mod_cropped.pal", 0x6383002);
  
  patchFile(ofs, "out/maps/theater_title.bin", 0x6384800);
  patchFile(ofs, "out/grp/theater_title_1.bin", 0x6354000);
  patchFile(ofs, "out/grp/theater_title_2.bin", 0x6356800);
  
  //=======================
  // signs
  //=======================
  
  loadSign(0);
  loadSign(1);
  loadSign(2);
  loadSign(3);
  loadSign(4);
  loadSign(5);
  
  patchSigns(ofs);
  
  //=======================
  // build stamp
  //=======================
  
  #if ADD_BUILD_STAMP
  {
    std::ifstream ifs("build_counter.txt");
    int buildNum;
    ifs >> buildNum;
    std::string numStr = TStringConversion::intToString(buildNum);
    std::string buildStr = std::string("Build ") + numStr;
    
    writeBuildString(ofs, 0x5990388 + (4 * 0x80) + (17 * 2), buildStr);
    writeBuildString(ofs, 0x6D01B88 + (4 * 0x80) + (17 * 2), buildStr);
  }
  #endif
  
  ofs.save(outfile.c_str());
  
  return 0;
}

