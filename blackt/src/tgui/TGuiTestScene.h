#ifndef TGUITESTSCENE_H
#define TGUITESTSCENE_H


#include "tgui/TLayeredGuiScene.h"
#include "tgui/TGuiTestLayer.h"
#include "tgui/TGuiGridLayer.h"
#include "util/TGraphic.h"

namespace BlackT {


class TGuiTestScene : public TLayeredGuiScene {
public:
  TGuiTestScene();
  virtual ~TGuiTestScene();
  
  virtual int nativeW() const;
  virtual int nativeH() const;
  
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
  
  TGuiTestLayer testLayer_;
  TGuiGridLayer gridLayer_;
  int nativeW_;
  int nativeH_;
  
};


}


#endif
