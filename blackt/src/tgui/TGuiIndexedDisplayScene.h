#ifndef TGUIINDEXEDDISPLAYSCENE_H
#define TGUIINDEXEDDISPLAYSCENE_H


#include "tgui/TGuiIndexedPickerScene.h"

namespace BlackT {

class TGuiIndexedDisplayScene : public TGuiIndexedPickerScene {
public:
  virtual ~TGuiIndexedDisplayScene();
  
protected:
  TGuiIndexedDisplayScene();

  /*
   * Internal event handler functions. Reimplement for GUI functionality.
   * Event coordinates are pre-morphed.
   */
/*  virtual void onMouseMove(TGuiEvent event);
  virtual void onMouseButtonPress(TGuiEvent event);
  virtual void onMouseButtonRelease(TGuiEvent event);
  virtual void onMouseDoubleClick(TGuiEvent event);
  virtual void onMouseEnter(TGuiEvent event);
  virtual void onMouseExit(TGuiEvent event);
  virtual void onDrawSizeChanged(int drawW__, int drawH__);
  virtual void onScrollPosChanged(double xScroll__, double yScroll__);
  virtual void onZoomChanged(double zoom__); */
  
};


}


#endif
