#ifndef TGUIINDEXEDPICKERSCENE_H
#define TGUIINDEXEDPICKERSCENE_H


#include "tgui/TLayeredGuiScene.h"
#include "tgui/TGuiIndexedPickerLayer.h"
#include "tgui/TGuiTestLayer.h"
#include "tgui/TGuiGridLayer.h"
#include "util/TGraphic.h"

namespace BlackT {


class TGuiIndexedPickerSceneCallback {
public:
  virtual void onItemPicked(int index);
  virtual void onItemHighlighted(int index);
protected:
  
};

class TGuiIndexedPickerScene : public TLayeredGuiScene {
public:
  friend class TGuiIndexedPickerLayer;

  virtual ~TGuiIndexedPickerScene();
  
  virtual int nativeW() const;
  virtual int nativeH() const;
  
  virtual void setPickerCallback(
      TGuiIndexedPickerSceneCallback& callback__);
      
  virtual void pickIndex(int index, bool triggerCallback = true);
  virtual void highlightIndex(int index, bool triggerCallback = true);
  
  virtual void changePickerVisible(bool pickerVisible__);
  virtual void changeHighlighterVisible(bool highlighterVisible__);
  virtual void changeSelectorsVisible(bool visible__);
  
  virtual void changeItemsPerRow(int itemsPerRow__);
  
protected:
  TGuiIndexedPickerScene();
  virtual void phase2Init();

  const static int freeItemLayoutRightTolerance = 0;
  
  virtual int numItems() const =0;
  virtual int itemW() const =0;
  virtual int itemH() const =0;
  virtual void drawItem(int index,
                        BlackT::TGraphic& dst,
                        int x, int y) =0;
  virtual int itemsPerRow() const;
                        
  virtual void notifyItemHighlighted(int index);
  virtual void notifyItemPicked(int index);
  
  virtual int actualItemsPerRow() const;

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
  
  TGuiIndexedPickerSceneCallback defaultCallback_;
  TGuiIndexedPickerSceneCallback* callback_;
  
  bool pickerVisible_;
  bool highlighterVisible_;
  
  int itemsPerRow_;
  
  TGuiIndexedPickerLayer pickerLayer_;
  TGuiGridLayer gridLayer_;
  
};


}


#endif
