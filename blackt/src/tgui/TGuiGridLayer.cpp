#include "tgui/TGuiGridLayer.h"
#include "tgui/TLayeredGuiScene.h"
#include "util/MiscMath.h"
#include <iostream>

namespace BlackT {


TGuiGridLayer::TGuiGridLayer(TLayeredGuiScene& parent__)
  : TGuiSceneLayer(parent__),
    spacingX_(16),
    spacingY_(16),
    color_(0x88, 0x88, 0x88, TColor::fullAlphaOpacity),
    lineWidth_(1) { }

TGuiGridLayer::~TGuiGridLayer() {
  
}

void TGuiGridLayer::changeSpacingX(int spacingX__) {
  setAndRedrawIfChanged(spacingX_, spacingX__);
}

void TGuiGridLayer::changeSpacingY(int spacingY__) {
  setAndRedrawIfChanged(spacingY_, spacingY__);
}

void TGuiGridLayer::changeColor(TColor color__) {
  setAndRedrawIfChanged(color_, color__);
}

void TGuiGridLayer::changeLineWidth(int lineWidth__) {
  setAndRedrawIfChanged(lineWidth_, lineWidth__);
}

void TGuiGridLayer::drawNative(TGraphic& dst) {
/*  int limitX = parent->nativeW() - parent->xScroll();
  int limitY = parent->nativeH() - parent->yScroll();
  if (limitX > dst.w()) limitX = dst.w();
  if (limitY > dst.h()) limitY = dst.h();
  
  int gapX = spacingX;
  int gapY = spacingY;
  
  for (int i = -((int)parent->xScroll() % spacingX) - 1; i < limitX; i += gapX) {
    dst.drawLine(i, 0, i, limitY,
                 color, lineWidth,
                 TGraphic::noTransUpdate);
  }
  
  for (int i = -((int)parent->yScroll() % spacingY) - 1; i < limitY; i += gapY) {
    dst.drawLine(0, i, limitX, i,
                 color, lineWidth,
                 TGraphic::noTransUpdate);
  } */
  
  
}

void TGuiGridLayer::drawScaled(TGraphic& dst) {
  int limitX = (parent->nativeW() - parent->xScroll()) * parent->zoom();
  int limitY = (parent->nativeH() - parent->yScroll()) * parent->zoom();
  if (limitX > dst.w()) limitX = dst.w();
  if (limitY > dst.h()) limitY = dst.h();
  
  int gapX = spacingX_ * parent->zoom();
  int gapY = spacingY_ * parent->zoom();
  
  // note that initial coordinates are -1 in order to have solid lines at the
  // bottom and right boundaries rather than the top and left
  for (int i = -((int)parent->xScroll() % spacingX_) * parent->zoom() - 1;
       i < limitX;
       i += gapX) {
    dst.drawLine(i, 0, i, limitY,
                 color_, lineWidth_,
                 TGraphic::noTransUpdate);
  }
  
  for (int i = -((int)parent->yScroll() % spacingY_) * parent->zoom() - 1;
       i < limitY;
       i += gapY) {
    dst.drawLine(0, i, limitX, i,
                 color_, lineWidth_,
                 TGraphic::noTransUpdate);
  }
  
  
}


}
