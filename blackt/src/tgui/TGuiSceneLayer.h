#ifndef TGUISCENELAYER_H
#define TGUISCENELAYER_H


#include "tgui/TGuiSceneLayer.h"
#include "tgui/TLayeredGuiScene.h"
#include "tgui/TGuiEvent.h"
#include "util/TGraphic.h"

namespace BlackT {


class TGuiSceneLayer {
public:
  virtual ~TGuiSceneLayer();
  
  virtual void drawNative(TGraphic& dst);
  virtual void drawScaled(TGraphic& dst);
  
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
  
  bool visible() const;
  void setVisible(bool visible__);
  
  bool active() const;
  void setActive(bool active__);
  
protected:
  TLayeredGuiScene* parent;
  bool visible_;
  bool active_;
  
  template <class T>
  void redrawIfChanged(const T& before, const T& after) {
    if (before != after) parent->signalRedraw();
  }
  
  template <class T>
  void setAndRedrawIfChanged(T& dst, const T& newValue) {
    T old = dst;
    dst = newValue;
    redrawIfChanged(old, dst);
  }

  TGuiSceneLayer(TLayeredGuiScene& parent__);
  
};


}


#endif
