#ifndef TGUITESTPICKERSCENE_H
#define TGUITESTPICKERSCENE_H


#include "tgui/TGuiIndexedPickerScene.h"

namespace BlackT {


class TGuiTestPickerScene : public TGuiIndexedPickerScene {
public:
  TGuiTestPickerScene();
  
  
protected:
  
  virtual int numItems() const;
  virtual int itemW() const;
  virtual int itemH() const;
  virtual void drawItem(int index,
                        BlackT::TGraphic& dst,
                        int x, int y);
  virtual int itemsPerRow() const;
  
};


}


#endif
