#ifndef TLAYEREDGUISCENE_H
#define TLAYEREDGUISCENE_H


#include "tgui/TGuiScene.h"
#include "tgui/TLayeredGuiScene.h"
//#include "tgui/TGuiSceneLayer.h"
#include "tgui/TGuiEvent.h"
#include "util/TGraphic.h"
#include <vector>

namespace BlackT {


class TGuiSceneLayer;

class TLayeredGuiScene : public TGuiScene {
public:
  friend class TGuiSceneLayer;

  virtual ~TLayeredGuiScene();
  
protected:
  typedef std::vector<TGuiSceneLayer*> LayerCollection;

  TLayeredGuiScene();
  
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
  
  virtual void addLayer(TGuiSceneLayer* layer);
  
  virtual void preEventEvaluation(TGuiEvent event);
  virtual void postEventEvaluation(TGuiEvent event);
  
  LayerCollection layers_;
};


}


#endif
