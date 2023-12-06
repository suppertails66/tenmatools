#include "tgui/TGuiScene.h"
#include "util/MiscMath.h"

namespace BlackT {


TGuiSceneCallbackTarget::~TGuiSceneCallbackTarget() { }
  
void TGuiSceneCallbackTarget::notifySceneNativeSizeChanged(int w__, int h__) {
  
}

void TGuiSceneCallbackTarget::notifySceneRedrawNeeded() {
  
}

TGuiScene::~TGuiScene() { }

TGuiScene::TGuiScene()
  : callback_(NULL),
    xScroll_(0),
    yScroll_(0),
    zoom_(1.0),
    mouseInScene_(false) {
  callback_ = &defaultCallbackTarget_;
}

double TGuiScene::xScroll() const {
  return xScroll_;
}

double TGuiScene::yScroll() const {
  return yScroll_;
}

double TGuiScene::zoom() const {
  return zoom_;
}

void TGuiScene::draw() {
  // clear native drawing buffer
  nativeGrp_.clearTransparent();
  grp_.clearTransparent();
  
  // draw at native size
  drawNative(nativeGrp_);
  
  // scale native graphic by zoom factor to produce final output
  grp_.scale(nativeGrp_,
             TRect(0, 0, nativeGrp_.w() * zoom_, nativeGrp_.h() * zoom_),
             TGraphic::noTransUpdate);
  
  drawScaled(grp_);
}

const TGraphic& TGuiScene::getOutputGraphic() const {
  return grp_;
}

void TGuiScene::changeDrawSize(int drawW__, int drawH__) {
  grp_.resize(drawW__, drawH__);
  nativeGrp_.resize(drawW__ / zoom_, drawH__ / zoom_);
  
  onDrawSizeChanged(drawW__, drawH__);
  
  callback_->notifySceneRedrawNeeded();
}

void TGuiScene::changeScrollPos(double xScroll__, double yScroll__) {
  xScroll_ = xScroll__;
  yScroll_ = yScroll__;
  
  onScrollPosChanged(xScroll__, yScroll__);
  
  callback_->notifySceneRedrawNeeded();
}

void TGuiScene::changeZoom(double zoom__) {
  zoom_ = zoom__;
  
  onZoomChanged(zoom__);
  
  callback_->notifySceneRedrawNeeded();
}

void TGuiScene::setCallbackObj(TGuiSceneCallbackTarget& callback__) {
  callback_ = &callback__;
}

void TGuiScene::mouseMove(TGuiEvent event) {
  morphCoordinates(event);
  onMouseMove(event);
  callback_->notifySceneRedrawNeeded();
}

void TGuiScene::mouseButtonPress(TGuiEvent event) {
  morphCoordinates(event);
  onMouseButtonPress(event);
  callback_->notifySceneRedrawNeeded();
}

void TGuiScene::mouseButtonRelease(TGuiEvent event) {
  morphCoordinates(event);
  onMouseButtonRelease(event);
  callback_->notifySceneRedrawNeeded();
}

void TGuiScene::mouseDoubleClick(TGuiEvent event) {
  morphCoordinates(event);
  onMouseDoubleClick(event);
  callback_->notifySceneRedrawNeeded();
}

void TGuiScene::mouseEnter(TGuiEvent event) {
  mouseInScene_ = true;
  morphCoordinates(event);
  onMouseEnter(event);
  callback_->notifySceneRedrawNeeded();
}

void TGuiScene::mouseExit(TGuiEvent event) {
  mouseInScene_ = false;
  morphCoordinates(event);
  onMouseExit(event);
  callback_->notifySceneRedrawNeeded();
}

void TGuiScene::morphCoordinates(double& x, double& y) const {
  drawToSceneCoordinatesScaled(x, y);
}

void TGuiScene::unmorphCoordinates(double& x, double& y) const {
  sceneToDrawCoordinatesScaled(x, y);
}

void TGuiScene::morphCoordinates(TGuiEvent& event) const {
  morphCoordinates(event.mouseX, event.mouseY);
//  event.makeCoordinatesVirtual(xScroll_, yScroll_, w(), h(), zoom_);
}

void TGuiScene::unmorphCoordinates(TGuiEvent& event) const {
  unmorphCoordinates(event.mouseX, event.mouseY);
//  event.makeCoordinatesReal(xScroll_, yScroll_, zoom_);
}

void TGuiScene::sceneToDrawCoordinatesScaled(double& x, double& y) const {
  x -= xScroll_;
  x *= zoom_;
  
  y -= yScroll_;
  y *= zoom_;
}

void TGuiScene::drawToSceneCoordinatesScaled(double& x, double& y) const {
  x /= zoom_;
  x += xScroll_;
  MiscMath::clamp(x, (double)0, (double)nativeW() - 1);
  
  y /= zoom_;
  y += yScroll_;
  MiscMath::clamp(y, (double)0, (double)nativeH() - 1);
}

void TGuiScene::sceneToDrawCoordinatesNative(double& x, double& y) const {
  x -= xScroll_;
  
  y -= yScroll_;
}

void TGuiScene::drawToSceneCoordinatesNative(double& x, double& y) const {
  x += xScroll_;
  MiscMath::clamp(x, (double)0, (double)nativeW() - 1);
  
  y += yScroll_;
  MiscMath::clamp(y, (double)0, (double)nativeH() - 1);
}

void TGuiScene::signalRedraw() {
  callback_->notifySceneRedrawNeeded();
}

void TGuiScene::signalNativeSizeChanged() {
  callback_->notifySceneNativeSizeChanged(nativeW(), nativeH());
}

int TGuiScene::drawW() const {
  return grp_.w();
}

int TGuiScene::drawH() const {
  return grp_.h();
}

void TGuiScene::onMouseMove(TGuiEvent event) {
  
}

void TGuiScene::onMouseButtonPress(TGuiEvent event) {
  
}

void TGuiScene::onMouseButtonRelease(TGuiEvent event) {
  
}

void TGuiScene::onMouseDoubleClick(TGuiEvent event) {
  
}

void TGuiScene::onMouseEnter(TGuiEvent event) {
  
}

void TGuiScene::onMouseExit(TGuiEvent event) {
  
}

void TGuiScene::onDrawSizeChanged(int drawW__, int drawH__) {
  
}

void TGuiScene::onScrollPosChanged(double xScroll__, double yScroll__) {
  
}

void TGuiScene::onZoomChanged(double zoom__) {
  
}

void TGuiScene::drawNative(TGraphic& dst) {
  
}

void TGuiScene::drawScaled(TGraphic& dst) {
  
}

bool TGuiScene::mouseInScene() const {
  return mouseInScene_;
}


}
