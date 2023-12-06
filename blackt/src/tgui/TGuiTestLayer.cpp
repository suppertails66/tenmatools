#include "tgui/TGuiTestLayer.h"
#include "tgui/TLayeredGuiScene.h"
#include "util/MiscMath.h"
#include <iostream>

namespace BlackT {


TGuiTestLayer::TGuiTestLayer(TLayeredGuiScene& parent__)
  : TGuiSceneLayer(parent__) { }

TGuiTestLayer::~TGuiTestLayer() {
  
}

void TGuiTestLayer::drawNative(TGraphic& dst) {
//  dst.fillRect(50 - parent->xScroll(), 50 - parent->yScroll(),
//               25 * parent->zoom(), 25 * parent->zoom(),
//               TColor(255, 0, 0, TColor::fullAlphaOpacity),
//               TGraphic::noTransUpdate);
  dst.fillRect(50 - parent->xScroll(), 50 - parent->yScroll(),
               25, 25,
               TColor(255, 0, 0, TColor::fullAlphaOpacity),
               TGraphic::noTransUpdate);
}

void TGuiTestLayer::onMouseMove(TGuiEvent event) {
  
}

void TGuiTestLayer::onMouseButtonPress(TGuiEvent event) {
  
}

void TGuiTestLayer::onMouseButtonRelease(TGuiEvent event) {
  
}

void TGuiTestLayer::onMouseDoubleClick(TGuiEvent event) {
  
}

void TGuiTestLayer::onMouseEnter(TGuiEvent event) {
  
}

void TGuiTestLayer::onMouseExit(TGuiEvent event) {
  
}

void TGuiTestLayer::onDrawSizeChanged(int drawW__, int drawH__) {
  
}

void TGuiTestLayer::onScrollPosChanged(double xScroll__, double yScroll__) {
  
}

void TGuiTestLayer::onZoomChanged(double zoom__) {
  
}


}
