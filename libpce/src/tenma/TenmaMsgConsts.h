#ifndef TENMAMSGCONSTS_H
#define TENMAMSGCONSTS_H


#include "util/TStream.h"
#include <string>
#include <vector>
#include <map>

namespace Pce {


class TenmaMsgConsts {
public:
  const static int sectorSize = 0x800;
  
  const static int sectorsPerMapTextBlock = 0x4;
  
  const static int opcodeBase = 0xEF;

  const static int opcode_opEF = 0xEF;
  const static int opcode_opF0 = 0xF0;
  const static int opcode_opF1 = 0xF1;
  const static int opcode_opF2 = 0xF2;
    const static int opcode_clear = 0xF2;
  const static int opcode_opF3 = 0xF3;
  const static int opcode_color = 0xF4;
  const static int opcode_adpcm = 0xF5;
  const static int opcode_opF6 = 0xF6;
  const static int opcode_opF7 = 0xF7;
  const static int opcode_opF8 = 0xF8;
  const static int opcode_opF9 = 0xF9;
  const static int opcode_opFA = 0xFA;
  const static int opcode_number = 0xFB;
  const static int opcode_string = 0xFC;
  const static int opcode_prompt = 0xFD;
  const static int opcode_br = 0xFE;
  const static int opcode_end = 0xFF;
  
  // number of sectors between each map in a sequence
  const static int mapSectorStdSpacing = 0x1C;
  const static int mapMsgBlockSize = 0x2000;
  const static int mapMsgBlockSectorSize = mapMsgBlockSize / sectorSize;
  
  static int getMsgHeaderArgsSize(int msgType);
  static int getOpArgsSize(int opcode);
  static int numDismPreOpLinebreaks(int opcode);
  static int numDismPostOpLinebreaks(int opcode);
  static bool isOp(int opcode);
  static bool isInlineableOp(int opcode);
  // return a list of sectors containing maps (on rev7 disc).
  // this is necessary because the maps are not (to my knowledge)
  // explicitly indexed anywhere, and they do not seem to
  // appear on the disc in any set pattern.
  static std::vector<int> getMapSectorIndex();
protected:
  
};


}


#endif
