#include "util/TSoundFile.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>

namespace BlackT {


TSoundFile::TSoundFile()
  : channels_(1),
    rate_(-1) {
  
}

TSoundFile::~TSoundFile() { }

void TSoundFile::importWav(TStream& ifs) {
  int filesize = ifs.size();
  if (filesize <= 0) {
    throw TGenericException(T_SRCANDLINE,
                            "importWav()",
                            std::string("Tried to import nonexistent ")
                            + "WAV file");
  }
  
  if (filesize < 36) {
    throw TGenericException(T_SRCANDLINE,
                            "importWav()",
                            std::string("Tried to import invalid ")
                            + "WAV file");
  }
  
  int basepos = ifs.tell();
  
  char header[4];
  ifs.read(header, sizeof(header));
  if (std::memcmp((char*)header, "RIFF", 4) != 0) {
    throw TGenericException(T_SRCANDLINE,
                            "importWav()",
                            std::string("Bad WAV header"));
  }
  
  // for now, only uncompressed 16-bit signed stereo data supported
  
  ifs.seek(basepos + 16);
  int subchunk1Size = ifs.readu32le();
  
  if (subchunk1Size != 16) {
    throw TGenericException(T_SRCANDLINE,
                            "importWav()",
                            std::string("Non-PCM WAV files not supported"));
  }
  
  ifs.seek(basepos + 20);
  int format = ifs.readu16le();
  int numChannels = ifs.readu16le();
  int sampleRate = ifs.readu32le();
  ifs.seek(basepos + 34);
  int bitsPerSample = ifs.readu16le();
  
  if ((format != 1)) {
    throw TGenericException(T_SRCANDLINE,
                            "importWav()",
                            std::string("WAV compression ")
                              + TStringConversion::toString(format)
                              + " not supported");
  }
  
  if ((bitsPerSample != 16)) {
    throw TGenericException(T_SRCANDLINE,
                            "importWav()",
                            std::string("Bits-per-sample value of ")
                              + TStringConversion::toString(bitsPerSample)
                              + " not supported");
  }
  
  int sampleRange = ((1 << bitsPerSample) / 2) - 1;
  
  ifs.seek(basepos + 40);
  int dataSize = ifs.readu32le();
  int numSamps = dataSize / (bitsPerSample / 8);
  data_.resize(numSamps);
  for (int i = 0; i < numSamps; i++) {
    int sample = ifs.readu16le();
    double dSample = (double)sample / (double)sampleRange;
    data_[i] = dSample;
  }
  
  channels_ = numChannels;
  rate_ = sampleRate;
}
  
void TSoundFile::exportWav(TStream& ofs) const {
  int bitsPerSample = 16;
  int bytesPerSample = bitsPerSample / 8;
  int sampleRange = ((1 << bitsPerSample) / 2) - 1;
  int numSamples = data_.size() / channels_;
  
  // RIFF
  ofs.write("RIFF", 4);
  ofs.writeu32le(36 + (data_.size() * bytesPerSample));
  ofs.write("WAVE", 4);
  
  // fmt
  ofs.write("fmt ", 4);
  ofs.writeu32le(16);
  ofs.writeu16le(1);    // format = PCM
  ofs.writeu16le(channels_);
  ofs.writeu32le(rate_);
  ofs.writeu32le(rate_ * channels_ * bytesPerSample);
  ofs.writeu16le(channels_ * bytesPerSample);
  ofs.writeu16le(bitsPerSample);
  
  // data
  ofs.write("data", 4);
  ofs.writeu32le((data_.size() * bytesPerSample));
  int pos = 0;
  for (int i = 0; i < numSamples; i++) {
    
    for (int j = 0; j < channels_; j++) {
      int rendered = data_[pos++] * (double)sampleRange;
      ofs.writeInt(rendered, bytesPerSample,
        EndiannessTypes::little, SignednessTypes::sign);
    }
    
  }
  
}

void TSoundFile::exportRaw16BitSigned(TStream& ofs) const {
  int bitsPerSample = 16;
  int bytesPerSample = bitsPerSample / 8;
  int sampleRange = ((1 << bitsPerSample) / 2) - 1;
  int numSamples = data_.size() / channels_;
  
  int pos = 0;
  for (int i = 0; i < numSamples; i++) {
    for (int j = 0; j < channels_; j++) {
      int rendered = data_[pos++] * (double)sampleRange;
      ofs.writeInt(rendered, bytesPerSample,
        EndiannessTypes::little, SignednessTypes::sign);
    }
  }
}

int TSoundFile::channels() const {
  return rate_;
}

void TSoundFile::setChannels(int channels__) {
  channels_ = channels__;
}

int TSoundFile::rate() const {
  return rate_;
}

void TSoundFile::setRate(int rate__) {
  rate_ = rate__;
}

void TSoundFile::addSample(TSample sample) {
  data_.push_back(sample);
}

std::vector<double>& TSoundFile::data() {
  return data_;
}

const std::vector<double>& TSoundFile::data() const {
  return data_;
}


}
