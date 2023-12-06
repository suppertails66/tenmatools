#ifndef TCOORDPAIR_H
#define TCOORDPAIR_H


namespace BlackT {


class TCoordPair {
public:
  TCoordPair();
  
  TCoordPair(int x__, int y__);
  
  int x() const;
  void setX(int x__);
  
  int y() const;
  void setY(int y__);
protected:
  int x_;
  int y_;
};


}


#endif
