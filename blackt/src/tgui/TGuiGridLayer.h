#ifndef TGUIGRIDLAYER_H
#define TGUIGRIDLAYER_H


#include "tgui/TGuiSceneLayer.h"
#include "util/TGraphic.h"

namespace BlackT {


class TGuiGridLayer : public TGuiSceneLayer {
public:
  TGuiGridLayer(TLayeredGuiScene& parent__);
  virtual ~TGuiGridLayer();
  
  virtual void drawNative(TGraphic& dst);
  virtual void drawScaled(TGraphic& dst);
  
  void changeSpacingX(int spacingX__);
  void changeSpacingY(int spacingY__);
  void changeColor(TColor color__);
  void changeLineWidth(int lineWidth__);
  
protected:
  
  int spacingX_;
  int spacingY_;
  TColor color_;
  int lineWidth_;
  
};


}


#endif
