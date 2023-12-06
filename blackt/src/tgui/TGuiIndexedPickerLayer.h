#ifndef TGUIINDEXEDPICKERLAYER_H
#define TGUIINDEXEDPICKERLAYER_H


#include "tgui/TGuiSceneLayer.h"
#include "util/TGraphic.h"

namespace BlackT {


class TGuiIndexedPickerScene;

class TGuiIndexedPickerLayer : public TGuiSceneLayer {
public:
  friend class TGuiIndexedPickerScene;

  TGuiIndexedPickerLayer(TGuiIndexedPickerScene& parent__);
  virtual ~TGuiIndexedPickerLayer();
  
  virtual void drawNative(TGraphic& dst);
  virtual void drawScaled(TGraphic& dst);
  
  virtual void setPickedIndex(int pickedIndex__);
  virtual void setHighlightedIndex(int highlightedIndex__);
  
protected:
  virtual int getItemsPerRow() const;
  virtual int getItemAtScenePos(int x, int y) const;
  virtual int getItemX(int index) const;
  virtual int getItemY(int index) const;

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
  
  TGuiIndexedPickerScene* parentPicker;
  int pickedIndex_;
  int highlightedIndex_;
  
};


}


#endif
