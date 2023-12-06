#ifndef DISCASTERCDMSF_H
#define DISCASTERCDMSF_H


namespace Discaster {

  
  struct CdMsf {
  public:
    CdMsf();
    int min;
    int sec;
    int frame;
    
    void fromSectorNum(int sectorNum);
    int toSectorNum() const;
    
    void fromSectorNumToBcd(int sectorNum);
  };
  

}


#endif
