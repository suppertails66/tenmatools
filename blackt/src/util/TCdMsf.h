#ifndef TCDMSF_H
#define TCDMSF_H


namespace BlackT {

  
  struct TCdMsf {
  public:
    TCdMsf();
    int min;
    int sec;
    int frame;
    
    void fromSectorNum(int sectorNum);
    int toSectorNum() const;
    
    void fromSectorNumToBcd(int sectorNum);
  };
  

}


#endif
