#ifndef TGUISCENE_H
#define TGUISCENE_H


#include "tgui/TGuiEvent.h"
#include "util/TGraphic.h"

namespace BlackT {


class TGuiSceneCallbackTarget {
public:
  virtual ~TGuiSceneCallbackTarget();
  
  virtual void notifySceneNativeSizeChanged(int w__, int h__);
  virtual void notifySceneRedrawNeeded();
protected:
  
};

class TGuiScene {
public:
  virtual ~TGuiScene();
  
  virtual int nativeW() const =0;
  virtual int nativeH() const =0;
  
  virtual double xScroll() const;
  virtual double yScroll() const;
  virtual double zoom() const;
  
  virtual void draw();
  virtual const TGraphic& getOutputGraphic() const;
  
  virtual void changeDrawSize(int drawW__, int drawH__);
  virtual void changeScrollPos(double xScroll__, double yScroll__);
  virtual void changeZoom(double zoom__);
  virtual void setCallbackObj(TGuiSceneCallbackTarget& callback__);
  
  /*
   * Public event handler functions. User code calls these, using whatever
   * coordinates the system GUI system provides.
   * Internally, these morph the coordinates to the local scene and then
   * call the internal handlers.
   */
  void mouseMove(TGuiEvent event);
  void mouseButtonPress(TGuiEvent event);
  void mouseButtonRelease(TGuiEvent event);
  void mouseDoubleClick(TGuiEvent event);
  void mouseEnter(TGuiEvent event);
  void mouseExit(TGuiEvent event);
  
  virtual int drawW() const;
  virtual int drawH() const;
  
  virtual void signalRedraw();
  virtual void signalNativeSizeChanged();
  
  virtual bool mouseInScene() const;
  
  void sceneToDrawCoordinatesNative(double& x, double& y) const;
  void drawToSceneCoordinatesNative(double& x, double& y) const;
  void sceneToDrawCoordinatesScaled(double& x, double& y) const;
  void drawToSceneCoordinatesScaled(double& x, double& y) const;
  
protected:
  TGuiScene();
  
  void morphCoordinates(double& x, double& y) const;
  void unmorphCoordinates(double& x, double& y) const;
  
  void morphCoordinates(TGuiEvent& event) const;
  void unmorphCoordinates(TGuiEvent& event) const;
  
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
  
  virtual void drawNative(TGraphic& dst);
  virtual void drawScaled(TGraphic& dst);
  
  TGuiSceneCallbackTarget defaultCallbackTarget_;
  TGuiSceneCallbackTarget* callback_;
  
  TGraphic grp_;
  TGraphic nativeGrp_;
  
  double xScroll_;
  double yScroll_;
  
  double zoom_;
  
  bool mouseInScene_;
  
};


}


#endif
