#include "pce/okiadpcm.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TOpt.h"
#include "util/TSoundFile.h"
#include <string>
#include <iostream>

using namespace std;
using namespace BlackT;
//using namespace Pce;

int consecutiveResetThreshold = 64;

int main(int argc, char* argv[]) {
  if (argc < 5) {
    std::cout << "PC-Engine ADPCM dumper" << std::endl;
    std::cout << "Usage: " << argv[0] << " <infile> <srcsector> <numsectors>"
      << " <outfile> [options]" << std::endl;
    
    std::cout << "Options: " << std::endl;
    std::cout << "   --rate    Set output sample rate in Hz (default: 16000)"
      << std::endl;
    std::cout << "   --smart   Enable smart ADPCM reset"
      << std::endl;
    
    return 0;
  }
  
  std::string inFile(argv[1]);
  int offset = TStringConversion::stringToInt(std::string(argv[2]));
  int numSectors = TStringConversion::stringToInt(std::string(argv[3]));
  std::string outFile(argv[4]);
  
  int sampleRate = 16000;
  TOpt::readNumericOpt(argc, argv, "--rate", &sampleRate);
  
  bool smartModeOn = false;
  if (TOpt::hasFlag(argc, argv, "--smart")) smartModeOn = true;
    
  TSoundFile sound;
  sound.setChannels(1);
  sound.setRate(sampleRate);
  
/*  TBufStream inBuffer;
  {
    TBufStream ifs;
    ifs.open(inFile.c_str());
    ifs.seek(offset * 0x800);
    inBuffer.writeFrom(ifs, numSectors * 0x800);
  }*/
  
  OKIADPCM_Decoder<OKIADPCM_MSM5205> dec;
  dec.SetSample(0x800);
  dec.SetSSI(0);
  
  TBufStream ifs;
  ifs.open(inFile.c_str());
  ifs.seek(offset * 0x800);
  
//  inBuffer.seek(0);
  int consecutiveZeroCount = 0;
  int remaining = numSectors * 0x800;
  while (remaining > 0) {
    unsigned char next = ifs.get();
    --remaining;
    unsigned char nyb1 = (next & 0xF0) >> 4;
    unsigned char nyb2 = (next & 0x0F);
    
    int samp1raw = dec.Decode(nyb1);
    int samp2raw = dec.Decode(nyb2);
    
    double samp1 = (((double)samp1raw / (double)0xFFF) - 0.5) * 2;
    double samp2 = (((double)samp2raw / (double)0xFFF) - 0.5) * 2;
    
    sound.addSample(samp1);
    sound.addSample(samp2);
    
    if (smartModeOn) {
      if (next == 0) {
        ++consecutiveZeroCount;
        if (consecutiveZeroCount >= consecutiveResetThreshold) {
          dec = OKIADPCM_Decoder<OKIADPCM_MSM5205>();
          dec.SetSample(0x800);
          dec.SetSSI(0);
        }
      }
      else consecutiveZeroCount = 0;
    }
  }
  
  {
    TBufStream ofs;
    sound.exportWav(ofs);
    ofs.save(outFile.c_str());
  }
  
  return 0;
}
