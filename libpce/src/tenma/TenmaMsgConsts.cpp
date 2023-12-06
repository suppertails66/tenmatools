#include "tenma/TenmaMsgConsts.h"
#include "tenma/TenmaMsgDismException.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TParse.h"
#include <iostream>

using namespace BlackT;

namespace Pce {


int TenmaMsgConsts::getMsgHeaderArgsSize(int msgType) {
  switch (msgType) {
  case 0:
    return 2;
  case 1:
    return 2;
  case 2:
    return 0;
  case 3:
    return 0;
  default:
    throw TenmaMsgDismException(T_SRCANDLINE,
                                "TenmaMsgConsts::getMsgHeaderSize()",
                                std::string("unknown msgType: ")
                                  + TStringConversion::intToString(msgType));
    break;
  }
}

int TenmaMsgConsts::getOpArgsSize(int opcode) {
  if ((opcode < opcodeBase) || (opcode >= 0x100)) {
    throw TenmaMsgDismException(T_SRCANDLINE,
                                "TenmaMsgConsts::getOpArgsSize()",
                                std::string("invalid opcode: ")
                                  + TStringConversion::intToString(opcode));
  }

  switch (opcode) {
  case opcode_opF3:
    return 1;
  case opcode_color:
    return 1;
  case opcode_adpcm:
    return 7;
  case opcode_opF6:
  case opcode_opF7:
    return 1;
  case opcode_opF8:
    return 3;
  case opcode_prompt:
    return 4;
  default:
    return 0;
    break;
  }
}

int TenmaMsgConsts::numDismPreOpLinebreaks(int opcode) {
  if ((opcode < opcodeBase) || (opcode >= 0x100)) {
    throw TenmaMsgDismException(T_SRCANDLINE,
                                "TenmaMsgConsts::numPostOpLinebreaks()",
                                std::string("invalid opcode: ")
                                  + TStringConversion::intToString(opcode));
  }
  
  switch (opcode) {
  case opcode_prompt:
    return 1;
  default:
    return 0;
    break;
  }
}

int TenmaMsgConsts::numDismPostOpLinebreaks(int opcode) {
  if ((opcode < opcodeBase) || (opcode >= 0x100)) {
    throw TenmaMsgDismException(T_SRCANDLINE,
                                "TenmaMsgConsts::numPostOpLinebreaks()",
                                std::string("invalid opcode: ")
                                  + TStringConversion::intToString(opcode));
  }
  
  switch (opcode) {
  case opcode_opF8:
  case opcode_opF3:
  case opcode_adpcm:
  case opcode_br:
    return 1;
  case opcode_prompt:
    return 2;
  default:
    return 0;
    break;
  }
}

bool TenmaMsgConsts::isOp(int opcode) {
  if (opcode >= opcodeBase) return true;
  return false;
}

bool TenmaMsgConsts::isInlineableOp(int opcode) {
  switch (opcode) {
  // ?
//  case opcode_opF2:
  // ?
//  case opcode_opF3:
  case opcode_color:
  // ?
  case opcode_opF6:
  // ? not sure if this is even used -- only 2 strings in main script
  // and one is almost definitely a misdetection
  case opcode_opF7:
  // ?
//  case opcode_opFA:
  case opcode_number:
  case opcode_string:
  case opcode_br:
    return true;
  default:
    return false;
    break;
  }
}

std::vector<int> TenmaMsgConsts::getMapSectorIndex() {
  std::vector<int> result;
  
/*  for (int i = 0; i < 64; i++) {
    result.push_back(0x1137 + (i * mapSectorStdSpacing));
  }
  
  for (int i = 0; i < 69; i++) {
    result.push_back(0x19B7 + (i * mapSectorStdSpacing));
  }
  
  for (int i = 0; i < 10; i++) {
    result.push_back(0x2144 + (i * mapSectorStdSpacing));
  }
  
  for (int i = 0; i < 62; i++) {
    result.push_back(0x2434 + (i * mapSectorStdSpacing));
  }
  
  for (int i = 0; i < 23; i++) {
    result.push_back(0x2B17 + (i * mapSectorStdSpacing));
  }
  
  for (int i = 0; i < 73; i++) {
    result.push_back(0x2FA8 + (i * mapSectorStdSpacing));
  }
  
  for (int i = 0; i < 65; i++) {
    result.push_back(0x3915 + (i * mapSectorStdSpacing));
  }
  
  for (int i = 0; i < 92; i++) {
    result.push_back(0x42ED + (i * mapSectorStdSpacing));
  }
  
  for (int i = 0; i < 56; i++) {
    result.push_back(0x526F + (i * mapSectorStdSpacing));
  }
  
  for (int i = 0; i < 36; i++) {
    result.push_back(0x5A33 + (i * mapSectorStdSpacing));
  }
  
  for (int i = 0; i < 54; i++) {
    result.push_back(0x5F88 + (i * mapSectorStdSpacing));
  }*/
  
  // i will not defend this code in any way...
  // but it's the easiest thing to do, and it really doesn't matter in the end
  result.push_back(0x1137);
  result.push_back(0x1153);
  result.push_back(0x116F);
  result.push_back(0x118B);
  result.push_back(0x11A7);
  result.push_back(0x11C3);
  result.push_back(0x11DF);
  result.push_back(0x11FB);
  result.push_back(0x1217);
  result.push_back(0x1233);
  result.push_back(0x124F);
  result.push_back(0x126B);
  result.push_back(0x1287);
  result.push_back(0x12A3);
  result.push_back(0x12BF);
  result.push_back(0x12DB);
  result.push_back(0x12F7);
  result.push_back(0x1313);
  result.push_back(0x132F);
  result.push_back(0x134B);
  result.push_back(0x1367);
  result.push_back(0x1383);
  result.push_back(0x139F);
  result.push_back(0x13BB);
  result.push_back(0x13D7);
  result.push_back(0x13F3);
  result.push_back(0x140F);
  result.push_back(0x142B);
  result.push_back(0x1445);
  result.push_back(0x1446);
  result.push_back(0x1447);
  result.push_back(0x1461);
  result.push_back(0x1462);
  result.push_back(0x1463);
  result.push_back(0x147D);
  result.push_back(0x147E);
  result.push_back(0x147F);
  result.push_back(0x1499);
  result.push_back(0x149A);
  result.push_back(0x149B);
  result.push_back(0x14B5);
  result.push_back(0x14B6);
  result.push_back(0x14B7);
  result.push_back(0x14D1);
  result.push_back(0x14D2);
  result.push_back(0x14D3);
  result.push_back(0x14EF);
  result.push_back(0x150B);
  result.push_back(0x1527);
  result.push_back(0x1543);
  result.push_back(0x155F);
  result.push_back(0x157B);
  result.push_back(0x1597);
  result.push_back(0x15B3);
  result.push_back(0x15CF);
  result.push_back(0x15EB);
  result.push_back(0x1607);
  result.push_back(0x1623);
  result.push_back(0x163F);
  result.push_back(0x165B);
  result.push_back(0x1677);
  result.push_back(0x1693);
  result.push_back(0x16AF);
  result.push_back(0x16CB);
  result.push_back(0x16E7);
  result.push_back(0x1703);
  result.push_back(0x171F);
  result.push_back(0x173B);
  result.push_back(0x1757);
  result.push_back(0x1773);
  result.push_back(0x178F);
  result.push_back(0x17AB);
  result.push_back(0x17C7);
  result.push_back(0x17E3);
  result.push_back(0x17FF);
  result.push_back(0x181B);
  result.push_back(0x19B7);
  result.push_back(0x19D3);
  result.push_back(0x19EF);
  result.push_back(0x1A0B);
  result.push_back(0x1A27);
  result.push_back(0x1A43);
  result.push_back(0x1A5F);
  result.push_back(0x1A7B);
  result.push_back(0x1A97);
  result.push_back(0x1AB3);
  result.push_back(0x1ACF);
  result.push_back(0x1AEB);
  result.push_back(0x1B07);
  result.push_back(0x1B23);
  result.push_back(0x1B3F);
  result.push_back(0x1B5B);
  result.push_back(0x1B77);
  result.push_back(0x1B93);
  result.push_back(0x1BAF);
  result.push_back(0x1BCB);
  result.push_back(0x1BE7);
  result.push_back(0x1C03);
  result.push_back(0x1C1F);
  result.push_back(0x1C3B);
  result.push_back(0x1C57);
  result.push_back(0x1C73);
  result.push_back(0x1C8F);
  result.push_back(0x1CAB);
  result.push_back(0x1CC7);
  result.push_back(0x1CE3);
  result.push_back(0x1CFF);
  result.push_back(0x1D1B);
  result.push_back(0x1D37);
  result.push_back(0x1D53);
  result.push_back(0x1D6F);
  result.push_back(0x1D8B);
  result.push_back(0x1DA7);
  result.push_back(0x1DC3);
  result.push_back(0x1DDF);
  result.push_back(0x1DFB);
  result.push_back(0x1E17);
  result.push_back(0x1E33);
  result.push_back(0x1E4F);
  result.push_back(0x1E6B);
  result.push_back(0x1E87);
  result.push_back(0x1EA3);
  result.push_back(0x1EBF);
  result.push_back(0x1EDB);
  result.push_back(0x1EF5);
  result.push_back(0x1EF6);
  result.push_back(0x1EF7);
  result.push_back(0x1F13);
  result.push_back(0x1F2D);
  result.push_back(0x1F2F);
  result.push_back(0x1F4B);
  result.push_back(0x1F67);
  result.push_back(0x1F83);
  result.push_back(0x1F9F);
  result.push_back(0x1FBB);
  result.push_back(0x1FD7);
  result.push_back(0x1FF3);
  result.push_back(0x200F);
  result.push_back(0x202B);
  result.push_back(0x2047);
  result.push_back(0x2063);
  result.push_back(0x207F);
  result.push_back(0x209B);
  result.push_back(0x20B7);
  result.push_back(0x20D3);
  result.push_back(0x20EF);
  result.push_back(0x210B);
  result.push_back(0x2127);
  result.push_back(0x2144);
  result.push_back(0x2160);
  result.push_back(0x217C);
  result.push_back(0x2198);
  result.push_back(0x21B4);
  result.push_back(0x21D0);
  result.push_back(0x21EC);
  result.push_back(0x2208);
  result.push_back(0x2224);
  result.push_back(0x2240);
  result.push_back(0x2273);
  result.push_back(0x2274);
  result.push_back(0x2275);
  result.push_back(0x2276);
  result.push_back(0x2434);
  result.push_back(0x2450);
  result.push_back(0x246C);
  result.push_back(0x2488);
  result.push_back(0x24A4);
  result.push_back(0x24C0);
  result.push_back(0x24DC);
  result.push_back(0x24F8);
  result.push_back(0x2514);
  result.push_back(0x2530);
  result.push_back(0x254C);
  result.push_back(0x2568);
  result.push_back(0x2584);
  result.push_back(0x25A0);
  result.push_back(0x25BC);
  result.push_back(0x25D8);
  result.push_back(0x25F4);
  result.push_back(0x2610);
  result.push_back(0x262C);
  result.push_back(0x2648);
  result.push_back(0x2664);
  result.push_back(0x2680);
  result.push_back(0x269C);
  result.push_back(0x26B8);
  result.push_back(0x26D4);
  result.push_back(0x26F0);
  result.push_back(0x270C);
  result.push_back(0x2728);
  result.push_back(0x2744);
  result.push_back(0x2760);
  result.push_back(0x277C);
  result.push_back(0x2798);
  result.push_back(0x27B4);
  result.push_back(0x27D0);
  result.push_back(0x27EC);
  result.push_back(0x2806);
  result.push_back(0x2807);
  result.push_back(0x2808);
  result.push_back(0x2824);
  result.push_back(0x2840);
  result.push_back(0x285C);
  result.push_back(0x2878);
  result.push_back(0x2894);
  result.push_back(0x28B0);
  result.push_back(0x28CC);
  result.push_back(0x28E8);
  result.push_back(0x2904);
  result.push_back(0x2920);
  result.push_back(0x293C);
  result.push_back(0x2958);
  result.push_back(0x2974);
  result.push_back(0x2990);
  result.push_back(0x29AC);
  result.push_back(0x29C8);
  result.push_back(0x29E4);
  result.push_back(0x2A00);
  result.push_back(0x2A1C);
  result.push_back(0x2A38);
  result.push_back(0x2A54);
  result.push_back(0x2A70);
  result.push_back(0x2A8C);
  result.push_back(0x2AA8);
  result.push_back(0x2AC4);
  result.push_back(0x2AE0);
  result.push_back(0x2B17);
  result.push_back(0x2B33);
  result.push_back(0x2B4F);
  result.push_back(0x2B6B);
  result.push_back(0x2B87);
  result.push_back(0x2BA3);
  result.push_back(0x2BBF);
  result.push_back(0x2BDB);
  result.push_back(0x2BF7);
  result.push_back(0x2C13);
  result.push_back(0x2C2F);
  result.push_back(0x2C4B);
  result.push_back(0x2C67);
  result.push_back(0x2C83);
  result.push_back(0x2C9F);
  result.push_back(0x2CBB);
  result.push_back(0x2CD7);
  result.push_back(0x2CF3);
  result.push_back(0x2D0F);
  result.push_back(0x2D2B);
  result.push_back(0x2D47);
  result.push_back(0x2D63);
  result.push_back(0x2D7F);
  result.push_back(0x2FA8);
  result.push_back(0x2FC4);
  result.push_back(0x2FE0);
  result.push_back(0x2FFC);
  result.push_back(0x3018);
  result.push_back(0x3034);
  result.push_back(0x3050);
  result.push_back(0x306C);
  result.push_back(0x3088);
  result.push_back(0x30A4);
  result.push_back(0x30C0);
  result.push_back(0x30DC);
  result.push_back(0x30F8);
  result.push_back(0x3114);
  result.push_back(0x3130);
  result.push_back(0x314C);
  result.push_back(0x3168);
  result.push_back(0x3184);
  result.push_back(0x31A0);
  result.push_back(0x31BC);
  result.push_back(0x31D8);
  result.push_back(0x31F4);
  result.push_back(0x3210);
  result.push_back(0x322C);
  result.push_back(0x3248);
  result.push_back(0x3264);
  result.push_back(0x3280);
  result.push_back(0x329C);
  result.push_back(0x32B8);
  result.push_back(0x32D4);
  result.push_back(0x32F0);
  result.push_back(0x330C);
  result.push_back(0x3328);
  result.push_back(0x3344);
  result.push_back(0x3360);
  result.push_back(0x337C);
  result.push_back(0x3398);
  result.push_back(0x33B4);
  result.push_back(0x33D0);
  result.push_back(0x33EC);
  result.push_back(0x3408);
  result.push_back(0x3424);
  result.push_back(0x3440);
  result.push_back(0x345C);
  result.push_back(0x3478);
  result.push_back(0x3494);
  result.push_back(0x34B0);
  result.push_back(0x34CC);
  result.push_back(0x34E8);
  result.push_back(0x3504);
  result.push_back(0x3520);
  result.push_back(0x353C);
  result.push_back(0x3558);
  result.push_back(0x3574);
  result.push_back(0x3590);
  result.push_back(0x35AC);
  result.push_back(0x35C8);
  result.push_back(0x35E4);
  result.push_back(0x3600);
  result.push_back(0x361C);
  result.push_back(0x3638);
  result.push_back(0x3654);
  result.push_back(0x3670);
  result.push_back(0x368C);
  result.push_back(0x36A8);
  result.push_back(0x36C4);
  result.push_back(0x36E0);
  result.push_back(0x36FC);
  result.push_back(0x3718);
  result.push_back(0x3734);
  result.push_back(0x3750);
  result.push_back(0x376C);
  result.push_back(0x3788);
  result.push_back(0x3915);
  result.push_back(0x3931);
  result.push_back(0x394D);
  result.push_back(0x3969);
  result.push_back(0x3985);
  result.push_back(0x39A1);
  result.push_back(0x39BD);
  result.push_back(0x39D9);
  result.push_back(0x39F5);
  result.push_back(0x3A11);
  result.push_back(0x3A2D);
  result.push_back(0x3A49);
  result.push_back(0x3A65);
  result.push_back(0x3A81);
  result.push_back(0x3A9D);
  result.push_back(0x3AB9);
  result.push_back(0x3AD5);
  result.push_back(0x3AF1);
  result.push_back(0x3B0D);
  result.push_back(0x3B29);
  result.push_back(0x3B45);
  result.push_back(0x3B61);
  result.push_back(0x3B7D);
  result.push_back(0x3B99);
  result.push_back(0x3BB5);
  result.push_back(0x3BD1);
  result.push_back(0x3BED);
  result.push_back(0x3C09);
  result.push_back(0x3C25);
  result.push_back(0x3C41);
  result.push_back(0x3C5D);
  result.push_back(0x3C79);
  result.push_back(0x3C95);
  result.push_back(0x3CB1);
  result.push_back(0x3CCD);
  result.push_back(0x3CE9);
  result.push_back(0x3D05);
  result.push_back(0x3D21);
  result.push_back(0x3D3D);
  result.push_back(0x3D59);
  result.push_back(0x3D75);
  result.push_back(0x3D91);
  result.push_back(0x3DAD);
  result.push_back(0x3DC9);
  result.push_back(0x3DE5);
  result.push_back(0x3E01);
  result.push_back(0x3E1D);
  result.push_back(0x3E39);
  result.push_back(0x3E55);
  result.push_back(0x3E71);
  result.push_back(0x3E8D);
  result.push_back(0x3EA9);
  result.push_back(0x3EC5);
  result.push_back(0x3EE1);
  result.push_back(0x3EFD);
  result.push_back(0x3F19);
  result.push_back(0x3F35);
  result.push_back(0x3F51);
  result.push_back(0x3F6D);
  result.push_back(0x3F89);
  result.push_back(0x3FA5);
  result.push_back(0x3FBF);
  result.push_back(0x3FC0);
  result.push_back(0x3FC1);
  result.push_back(0x3FDD);
  result.push_back(0x3FF9);
  result.push_back(0x4015);
  result.push_back(0x42ED);
  result.push_back(0x4309);
  result.push_back(0x4325);
  result.push_back(0x4341);
  result.push_back(0x435D);
  result.push_back(0x4379);
  result.push_back(0x4395);
  result.push_back(0x43B1);
  result.push_back(0x43CD);
  result.push_back(0x43E9);
  result.push_back(0x4405);
  result.push_back(0x4421);
  result.push_back(0x443D);
  result.push_back(0x4459);
  result.push_back(0x4475);
  result.push_back(0x4491);
  result.push_back(0x44AD);
  result.push_back(0x44C9);
  result.push_back(0x44E5);
  result.push_back(0x4501);
  result.push_back(0x451D);
  result.push_back(0x4539);
  result.push_back(0x4555);
  result.push_back(0x4571);
  result.push_back(0x458D);
  result.push_back(0x45A9);
  result.push_back(0x45C5);
  result.push_back(0x45E1);
  result.push_back(0x45FD);
  result.push_back(0x4619);
  result.push_back(0x4635);
  result.push_back(0x4651);
  result.push_back(0x466D);
  result.push_back(0x4689);
  result.push_back(0x46A5);
  result.push_back(0x46C1);
  result.push_back(0x46DD);
  result.push_back(0x46F9);
  result.push_back(0x4715);
  result.push_back(0x4731);
  result.push_back(0x474D);
  result.push_back(0x4769);
  result.push_back(0x4785);
  result.push_back(0x47A1);
  result.push_back(0x47BD);
  result.push_back(0x47D9);
  result.push_back(0x47F5);
  result.push_back(0x4811);
  result.push_back(0x482D);
  result.push_back(0x4849);
  result.push_back(0x4865);
  result.push_back(0x4881);
  result.push_back(0x489D);
  result.push_back(0x48B9);
  result.push_back(0x48D5);
  result.push_back(0x48F1);
  result.push_back(0x490D);
  result.push_back(0x4929);
  result.push_back(0x4945);
  result.push_back(0x4961);
  result.push_back(0x497D);
  result.push_back(0x4999);
  result.push_back(0x49B5);
  result.push_back(0x49D1);
  result.push_back(0x49ED);
  result.push_back(0x4A09);
  result.push_back(0x4A23);
  result.push_back(0x4A24);
  result.push_back(0x4A25);
  result.push_back(0x4A3F);
  result.push_back(0x4A40);
  result.push_back(0x4A41);
  result.push_back(0x4A5D);
  result.push_back(0x4A77);
  result.push_back(0x4A78);
  result.push_back(0x4A79);
  result.push_back(0x4A95);
  result.push_back(0x4AB1);
  result.push_back(0x4ACD);
  result.push_back(0x4AE9);
  result.push_back(0x4B05);
  result.push_back(0x4B21);
  result.push_back(0x4B3D);
  result.push_back(0x4B59);
  result.push_back(0x4B75);
  result.push_back(0x4B91);
  result.push_back(0x4BAD);
  result.push_back(0x4BC9);
  result.push_back(0x4BE5);
  result.push_back(0x4C01);
  result.push_back(0x4C1D);
  result.push_back(0x4C39);
  result.push_back(0x4C55);
  result.push_back(0x4C71);
  result.push_back(0x4C8D);
  result.push_back(0x4CA9);
  result.push_back(0x4CC5);
  result.push_back(0x4CE1);
  result.push_back(0x5253);
  result.push_back(0x526F);
  result.push_back(0x528B);
  result.push_back(0x52A7);
  result.push_back(0x52C3);
  result.push_back(0x52DF);
  result.push_back(0x52FB);
  result.push_back(0x5317);
  result.push_back(0x5333);
  result.push_back(0x534F);
  result.push_back(0x536B);
  result.push_back(0x5387);
  result.push_back(0x53A3);
  result.push_back(0x53BF);
  result.push_back(0x53DB);
  result.push_back(0x53F7);
  result.push_back(0x5413);
  result.push_back(0x542F);
  result.push_back(0x544B);
  result.push_back(0x5467);
  result.push_back(0x5483);
  result.push_back(0x549F);
  result.push_back(0x54BB);
  result.push_back(0x54D7);
  result.push_back(0x54F3);
  result.push_back(0x550F);
  result.push_back(0x552B);
  result.push_back(0x5547);
  result.push_back(0x5563);
  result.push_back(0x557F);
  result.push_back(0x559B);
  result.push_back(0x55B7);
  result.push_back(0x55D3);
  result.push_back(0x55EF);
  result.push_back(0x560B);
  result.push_back(0x5627);
  result.push_back(0x5643);
  result.push_back(0x565F);
  result.push_back(0x567B);
  result.push_back(0x5697);
  result.push_back(0x56B3);
  result.push_back(0x56CF);
  result.push_back(0x56EB);
  result.push_back(0x5707);
  result.push_back(0x5723);
  result.push_back(0x573F);
  result.push_back(0x575B);
  result.push_back(0x5777);
  result.push_back(0x5793);
  result.push_back(0x57AF);
  result.push_back(0x57CB);
  result.push_back(0x57E7);
  result.push_back(0x5803);
  result.push_back(0x581F);
  result.push_back(0x583B);
  result.push_back(0x5857);
  result.push_back(0x5873);
  result.push_back(0x5A33);
  result.push_back(0x5A4F);
  result.push_back(0x5A6B);
  result.push_back(0x5A87);
  result.push_back(0x5AA3);
  result.push_back(0x5ABF);
  result.push_back(0x5ADB);
  result.push_back(0x5AF7);
  result.push_back(0x5B13);
  result.push_back(0x5B2F);
  result.push_back(0x5B4B);
  result.push_back(0x5B67);
  result.push_back(0x5B81);
  result.push_back(0x5B82);
  result.push_back(0x5B83);
  result.push_back(0x5B9F);
  result.push_back(0x5BBB);
  result.push_back(0x5BD7);
  result.push_back(0x5BF3);
  result.push_back(0x5C0F);
  result.push_back(0x5C2B);
  result.push_back(0x5C47);
  result.push_back(0x5C63);
  result.push_back(0x5C7F);
  result.push_back(0x5C99);
  result.push_back(0x5C9B);
  result.push_back(0x5CB7);
  result.push_back(0x5CD3);
  result.push_back(0x5CEF);
  result.push_back(0x5D0B);
  result.push_back(0x5D27);
  result.push_back(0x5D43);
  result.push_back(0x5D5F);
  result.push_back(0x5D7B);
  result.push_back(0x5D97);
  result.push_back(0x5DB3);
  result.push_back(0x5DCF);
  result.push_back(0x5DEB);
  result.push_back(0x5E07);
  result.push_back(0x5F88);
  result.push_back(0x5FA2);
  result.push_back(0x5FA3);
  result.push_back(0x5FA4);
  result.push_back(0x5FC0);
  result.push_back(0x5FDC);
  result.push_back(0x5FF8);
  result.push_back(0x6014);
  result.push_back(0x6030);
  result.push_back(0x604C);
  result.push_back(0x6068);
  result.push_back(0x6084);
  result.push_back(0x60A0);
  result.push_back(0x60BC);
  result.push_back(0x60D8);
  result.push_back(0x60F4);
  result.push_back(0x6110);
  result.push_back(0x612A);
  result.push_back(0x612B);
  result.push_back(0x612C);
  result.push_back(0x6146);
  result.push_back(0x6147);
  result.push_back(0x6148);
  result.push_back(0x6164);
  result.push_back(0x6180);
  result.push_back(0x619C);
  result.push_back(0x61B8);
  result.push_back(0x61D4);
  result.push_back(0x61F0);
  result.push_back(0x620C);
  result.push_back(0x6228);
  result.push_back(0x6244);
  result.push_back(0x6260);
  result.push_back(0x627C);
  result.push_back(0x6298);
  result.push_back(0x62B4);
  result.push_back(0x62D0);
  result.push_back(0x62EB);
  result.push_back(0x62EC);
  result.push_back(0x6306);
  result.push_back(0x6307);
  result.push_back(0x6308);
  result.push_back(0x6324);
  result.push_back(0x6340);
  result.push_back(0x635C);
  result.push_back(0x6378);
  result.push_back(0x6394);
  result.push_back(0x63B0);
  result.push_back(0x63CC);
  result.push_back(0x63E8);
  result.push_back(0x6404);
  result.push_back(0x6420);
  result.push_back(0x643C);
  result.push_back(0x6458);
  result.push_back(0x6474);
  result.push_back(0x6490);
  result.push_back(0x64AC);
  result.push_back(0x64C6);
  result.push_back(0x64C7);
  result.push_back(0x64C8);
  result.push_back(0x64E2);
  result.push_back(0x64E3);
  result.push_back(0x64E4);
  result.push_back(0x6500);
  result.push_back(0x651C);
  result.push_back(0x6538);
  result.push_back(0x6554);
  result.push_back(0x657C);
  result.push_back(0x657D);
  result.push_back(0x657E);
  result.push_back(0x657F);
  result.push_back(0xDA3B);
  result.push_back(0xDA57);
  result.push_back(0xDA73);
  result.push_back(0xDA8F);
  result.push_back(0xDAAB);
  result.push_back(0xDAFF);
  result.push_back(0xDB1B);
  result.push_back(0xDB37);
  result.push_back(0xDB53);
  result.push_back(0xDB6F);
  result.push_back(0xDB8B);
  result.push_back(0xDBA7);
  result.push_back(0xDBC3);
  result.push_back(0xDBDF);
  result.push_back(0xDD9F);
  result.push_back(0xDF0B);
  result.push_back(0xDF27);
  result.push_back(0xDF43);
  result.push_back(0xE103);
  result.push_back(0xE11F);
  result.push_back(0xE13B);
  result.push_back(0xE157);
  result.push_back(0xE173);
  result.push_back(0xE18F);
  result.push_back(0xE1AB);
  result.push_back(0xE1C7);
  result.push_back(0xE1E3);
  result.push_back(0xE1FF);
  result.push_back(0xE21B);
  result.push_back(0xE237);
  result.push_back(0xE253);
  result.push_back(0xE26F);
  result.push_back(0xE28B);
  result.push_back(0xE4D3);
  result.push_back(0xE4EF);
  result.push_back(0xE50B);
  result.push_back(0xE527);
  result.push_back(0xE543);
  result.push_back(0xE55F);
  result.push_back(0xE57B);
  result.push_back(0xE597);
  result.push_back(0xE5B3);
  result.push_back(0xE5CF);
  result.push_back(0xE5EB);
  result.push_back(0xE607);
  result.push_back(0xE623);
  result.push_back(0xE63F);
  result.push_back(0xE65B);
  result.push_back(0xE677);
  result.push_back(0xE693);
  result.push_back(0xE6AF);
  result.push_back(0xE6CB);
  result.push_back(0xE6E7);
  result.push_back(0xE701);
  result.push_back(0xE71B);
  result.push_back(0xE735);
  result.push_back(0xE74F);
  result.push_back(0xE769);
  result.push_back(0xE783);
  result.push_back(0xE79D);
  result.push_back(0xE7B7);
  result.push_back(0xE7D1);
  result.push_back(0xE7EB);
  result.push_back(0xE805);
  result.push_back(0xE81F);
  result.push_back(0xE839);
  result.push_back(0xF345);
  result.push_back(0xF35F);
  result.push_back(0xF379);
  result.push_back(0xF393);
  result.push_back(0xF3AD);
  
  return result;
}


}
