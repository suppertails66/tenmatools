#include "tgui/TGuiSceneLayer.h"
#include "tgui/TLayeredGuiScene.h"
#include "util/MiscMath.h"

namespace BlackT {


TGuiSceneLayer::~TGuiSceneLayer() { }
  
TGuiSceneLayer::TGuiSceneLayer(TLayeredGuiScene& parent__)
  : parent(&parent__),
    visible_(true),
    active_(true) { }

bool TGuiSceneLayer::visible() const {
  return visible_;
}

void TGuiSceneLayer::setVisible(bool visible__) {
  setAndRedrawIfChanged(visible_, visible__);
}

bool TGuiSceneLayer::active() const {
  return active_;
}

void TGuiSceneLayer::setActive(bool active__) {
  active_ = active__;
}

void TGuiSceneLayer::onMouseMove(TGuiEvent event) {
  
}

void TGuiSceneLayer::onMouseButtonPress(TGuiEvent event) {
  
}

void TGuiSceneLayer::onMouseButtonRelease(TGuiEvent event) {
  
}

void TGuiSceneLayer::onMouseDoubleClick(TGuiEvent event) {
  
}

void TGuiSceneLayer::onMouseEnter(TGuiEvent event) {
  
}

void TGuiSceneLayer::onMouseExit(TGuiEvent event) {
  
}

void TGuiSceneLayer::onDrawSizeChanged(int drawW__, int drawH__) {
  
}

void TGuiSceneLayer::onScrollPosChanged(double xScroll__, double yScroll__) {
  
}

void TGuiSceneLayer::onZoomChanged(double zoom__) {
  
}

void TGuiSceneLayer::drawNative(TGraphic& dst) {
  
}

void TGuiSceneLayer::drawScaled(TGraphic& dst) {
  
}


}
