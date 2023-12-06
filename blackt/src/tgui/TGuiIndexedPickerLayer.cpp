#include "tgui/TGuiIndexedPickerLayer.h"
#include "tgui/TGuiIndexedPickerScene.h"
#include "tgui/TLayeredGuiScene.h"
#include "util/MiscMath.h"
#include <iostream>

namespace BlackT {


TGuiIndexedPickerLayer
  ::TGuiIndexedPickerLayer(TGuiIndexedPickerScene& parent__)
  : TGuiSceneLayer((TLayeredGuiScene&)parent__),
    parentPicker(&parent__),
    pickedIndex_(-1),
    highlightedIndex_(-1) { }

TGuiIndexedPickerLayer::~TGuiIndexedPickerLayer() {
  
}

void TGuiIndexedPickerLayer::drawNative(TGraphic& dst) {
  int numItems = parentPicker->numItems();
//  int itemW = parentPicker->itemW();
//  int itemH = parentPicker->itemH();
//  int itemsPerRow = getItemsPerRow();
  
  for (int i = 0; i < numItems; i++) {
//    int rowNum = (i / itemsPerRow);
//    int colNum = (i % itemsPerRow);
//    double x = colNum * itemW;
//    double y = rowNum * itemH;
    double x = getItemX(i);
    double y = getItemY(i);
    parentPicker->sceneToDrawCoordinatesNative(x, y);
    
    parentPicker->drawItem(i, dst, x, y);
  }
}

void TGuiIndexedPickerLayer::drawScaled(TGraphic& dst) {
//  int numItems = parentPicker->numItems();
  int itemW = parentPicker->itemW();
  int itemH = parentPicker->itemH();
//  int itemsPerRow = getItemsPerRow();
  
  if ((highlightedIndex_ != -1) && (parentPicker->highlighterVisible_)) {
    double x = getItemX(highlightedIndex_);
    double y = getItemY(highlightedIndex_);
    parentPicker->sceneToDrawCoordinatesScaled(x, y);
    
    dst.drawRectBorder(x, y,
           itemW * parentPicker->zoom(), itemH * parentPicker->zoom(),
           TColor(0xFF, 0xFF, 0x00, TColor::fullAlphaOpacity),
           2,
           TGraphic::noTransUpdate);
  }
  
  if ((pickedIndex_ != -1) && (parentPicker->pickerVisible_)) {
    double x = getItemX(pickedIndex_);
    double y = getItemY(pickedIndex_);
    parentPicker->sceneToDrawCoordinatesScaled(x, y);
    
    dst.drawRectBorder(x, y,
           itemW * parentPicker->zoom(), itemH * parentPicker->zoom(),
           TColor(0xFF, 0x00, 0x00, TColor::fullAlphaOpacity),
           2,
           TGraphic::noTransUpdate);
  }
}

void TGuiIndexedPickerLayer::setPickedIndex(int pickedIndex__) {
  pickedIndex_ = pickedIndex__; 
}

void TGuiIndexedPickerLayer::setHighlightedIndex(int highlightedIndex__) {
  highlightedIndex_ = highlightedIndex__;
}

int TGuiIndexedPickerLayer::getItemsPerRow() const {
  return parentPicker->actualItemsPerRow();
}

int TGuiIndexedPickerLayer::getItemAtScenePos(int x, int y) const {
  int itemsPerRow = getItemsPerRow();
  
  int rowNum = (y / parentPicker->itemH());
  int colNum = (x / parentPicker->itemW());
  int index = (rowNum * itemsPerRow) + colNum;
  
  if ((index < 0) || (index >= parentPicker->numItems()))
    index = -1;
  if ((x >= parentPicker->nativeW()) || (y >= parentPicker->nativeH()))
    index = -1;
  
  return index;
}

int TGuiIndexedPickerLayer::getItemX(int index) const {
  int x = (index % getItemsPerRow()) * parentPicker->itemW();
  return x;
}

int TGuiIndexedPickerLayer::getItemY(int index) const {
  int y = (index / getItemsPerRow()) * parentPicker->itemH();
  return y;
}

void TGuiIndexedPickerLayer::onMouseMove(TGuiEvent event) {
  highlightedIndex_ = getItemAtScenePos(event.mouseX, event.mouseY);
  parentPicker->notifyItemHighlighted(highlightedIndex_);
}

void TGuiIndexedPickerLayer::onMouseButtonPress(TGuiEvent event) {
  if (event.mouseLeftButton) {
    pickedIndex_ = getItemAtScenePos(event.mouseX, event.mouseY);
    parentPicker->notifyItemPicked(pickedIndex_);
  }
  else if (event.mouseRightButton) {
    pickedIndex_ = -1;
    parentPicker->notifyItemPicked(pickedIndex_);
  }
}

void TGuiIndexedPickerLayer::onMouseButtonRelease(TGuiEvent event) {
  
}

void TGuiIndexedPickerLayer::onMouseDoubleClick(TGuiEvent event) {
  
}

void TGuiIndexedPickerLayer::onMouseEnter(TGuiEvent event) {
  
}

void TGuiIndexedPickerLayer::onMouseExit(TGuiEvent event) {
  highlightedIndex_ = -1;
}

void TGuiIndexedPickerLayer::onDrawSizeChanged(int drawW__, int drawH__) {
  
}

void TGuiIndexedPickerLayer::onScrollPosChanged(double xScroll__, double yScroll__) {
  
}

void TGuiIndexedPickerLayer::onZoomChanged(double zoom__) {
  
}


}
