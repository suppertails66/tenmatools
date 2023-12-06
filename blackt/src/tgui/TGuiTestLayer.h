#ifndef TGUITESTLAYER_H
#define TGUITESTLAYER_H


#include "tgui/TGuiSceneLayer.h"
#include "util/TGraphic.h"

namespace BlackT {


class TGuiTestLayer : public TGuiSceneLayer {
public:
  TGuiTestLayer(TLayeredGuiScene& parent__);
  virtual ~TGuiTestLayer();
  
  virtual void drawNative(TGraphic& dst);
  
protected:
  /*
   * Internal event handler functions. Reimplement for GUI functionality.
   * Event coordinates are pre-morphed.
   */
  virtual void onMouseMove(TGuiEvent event);
  virtual void onMouseButtonPress(TGuiEvent event);
  virtual void onMouseButtonRelease(TGuiEvent event);
  virtual void onMouseDoubleClick(TGuiEvent event);
  virtual void onMouseEnter(TGuiEvent event);
  virtual void onMouseExit(TGuiEvent event);
  virtual void onDrawSizeChanged(int drawW__, int drawH__);
  virtual void onScrollPosChanged(double xScroll__, double yScroll__);
  virtual void onZoomChanged(double zoom__);
  
};


}


#endif
