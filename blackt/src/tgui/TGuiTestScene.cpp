#include "tgui/TGuiTestScene.h"
#include "util/MiscMath.h"
#include <iostream>

namespace BlackT {


TGuiTestScene::~TGuiTestScene() { }

TGuiTestScene::TGuiTestScene()
  : TLayeredGuiScene(),
    testLayer_(*this),
    gridLayer_(*this),
    nativeW_(512),
    nativeH_(256) {
  addLayer(&testLayer_);
  addLayer(&gridLayer_);
}

int TGuiTestScene::nativeW() const {
  return nativeW_;
}

int TGuiTestScene::nativeH() const {
  return nativeH_;
}

void TGuiTestScene::onMouseMove(TGuiEvent event) {
//  std::cerr << event.mouseX << " " << event.mouseY << std::endl;
}

void TGuiTestScene::onMouseButtonPress(TGuiEvent event) {
//  nativeW_ = 25;
//  nativeH_ = 10;
//  callback_->notifySceneNativeSizeChanged(nativeW_, nativeH_);
}

void TGuiTestScene::onMouseButtonRelease(TGuiEvent event) {
//  nativeW_ = 512;
//  nativeH_ = 256;
//  callback_->notifySceneNativeSizeChanged(nativeW_, nativeH_);
}

void TGuiTestScene::onMouseDoubleClick(TGuiEvent event) {
  
}

void TGuiTestScene::onMouseEnter(TGuiEvent event) {
  
}

void TGuiTestScene::onMouseExit(TGuiEvent event) {
  
}

void TGuiTestScene::onDrawSizeChanged(int drawW__, int drawH__) {
  
}

void TGuiTestScene::onScrollPosChanged(double xScroll__, double yScroll__) {
  
}

void TGuiTestScene::onZoomChanged(double zoom__) {
  
}


}
