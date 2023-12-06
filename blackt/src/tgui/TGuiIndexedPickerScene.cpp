#include "tgui/TGuiIndexedPickerScene.h"
#include "util/MiscMath.h"
#include <iostream>

namespace BlackT {


void TGuiIndexedPickerSceneCallback::onItemPicked(int index) {
  
}

void TGuiIndexedPickerSceneCallback::onItemHighlighted(int index) {
  
}
  
TGuiIndexedPickerScene::~TGuiIndexedPickerScene() { }

TGuiIndexedPickerScene::TGuiIndexedPickerScene()
  : TLayeredGuiScene(),
    callback_(&defaultCallback_),
    pickerVisible_(true),
    highlighterVisible_(true),
    itemsPerRow_(-1),
    pickerLayer_(*this),
    gridLayer_(*this) {
//  gridLayer_.changeSpacingX(this->itemW());
//  gridLayer_.changeSpacingY(this->itemH());
  addLayer(&gridLayer_);
  addLayer(&pickerLayer_);
}

void TGuiIndexedPickerScene::phase2Init() {
  gridLayer_.changeSpacingX(this->itemW());
  gridLayer_.changeSpacingY(this->itemH());
}

int TGuiIndexedPickerScene::nativeW() const {
  return actualItemsPerRow() * itemW();
}

int TGuiIndexedPickerScene::nativeH() const {
  int itemsPer = actualItemsPerRow();
  int result = (numItems() / itemsPer);
  // account for non-full final row
  if (numItems() % itemsPer) ++result;
  result *= itemH();
  return result;
}

void TGuiIndexedPickerScene
  ::setPickerCallback(TGuiIndexedPickerSceneCallback& callback__) {
  callback_ = &callback__;
}

int TGuiIndexedPickerScene::itemsPerRow() const {
  return itemsPerRow_;
}

void TGuiIndexedPickerScene::notifyItemHighlighted(int index) {
  callback_->onItemHighlighted(index);
}
                      
void TGuiIndexedPickerScene::notifyItemPicked(int index) {
  callback_->onItemPicked(index);
}

void TGuiIndexedPickerScene
    ::changePickerVisible(bool pickerVisible__) {
  pickerVisible_ = pickerVisible__;
  signalRedraw();
}

void TGuiIndexedPickerScene
    ::changeHighlighterVisible(bool highlighterVisible__) {
  highlighterVisible_ = highlighterVisible__;
  signalRedraw();
}

void TGuiIndexedPickerScene
    ::changeSelectorsVisible(bool visible__) {
  pickerVisible_ = visible__;
  highlighterVisible_ = visible__;
  signalRedraw();
}

void TGuiIndexedPickerScene
    ::changeItemsPerRow(int itemsPerRow__) {
  itemsPerRow_ = itemsPerRow__;
  signalRedraw();
}
  
int TGuiIndexedPickerScene::actualItemsPerRow() const {
  int itemsPer = itemsPerRow();
  
  // if itemsPerRow is -1, draw as many items per row as will fit in the
  // drawing area
  if (itemsPer == -1) {
    itemsPer = drawW() / (itemW() * zoom()) - freeItemLayoutRightTolerance;
    if (itemsPer <= 0) itemsPer = 1; 
  }
  
  return itemsPer;
}
    
void TGuiIndexedPickerScene::pickIndex(int index, bool triggerCallback) {
  pickerLayer_.pickedIndex_ = index;
  if (triggerCallback) notifyItemPicked(index);
}

void TGuiIndexedPickerScene::highlightIndex(int index, bool triggerCallback) {
  pickerLayer_.highlightedIndex_ = index;
  if (triggerCallback) notifyItemHighlighted(index);
}

/*void TGuiIndexedPickerScene::onMouseMove(TGuiEvent event) {
  
}

void TGuiIndexedPickerScene::onMouseButtonPress(TGuiEvent event) {
  
}

void TGuiIndexedPickerScene::onMouseButtonRelease(TGuiEvent event) {
  
}

void TGuiIndexedPickerScene::onMouseDoubleClick(TGuiEvent event) {
  
}

void TGuiIndexedPickerScene::onMouseEnter(TGuiEvent event) {
  
}

void TGuiIndexedPickerScene::onMouseExit(TGuiEvent event) {
  
}

void TGuiIndexedPickerScene::onDrawSizeChanged(int drawW__, int drawH__) {
  
}

void TGuiIndexedPickerScene::onScrollPosChanged(double xScroll__, double yScroll__) {
  
}

void TGuiIndexedPickerScene::onZoomChanged(double zoom__) {
  
} */


}
