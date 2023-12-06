#include "tgui/TLayeredGuiScene.h"
#include "tgui/TGuiSceneLayer.h"
#include "util/MiscMath.h"

namespace BlackT {


TLayeredGuiScene::~TLayeredGuiScene() {
//  for (unsigned int i = 0; i < layers_.size(); i++) {
//    delete layers_[i];
//  }
}

TLayeredGuiScene::TLayeredGuiScene()
  : TGuiScene() { }

void TLayeredGuiScene::onMouseMove(TGuiEvent event) {
  preEventEvaluation(event);
  for (unsigned int i = 0; i < layers_.size(); i++) {
    if (layers_[i]->active()) layers_[i]->onMouseMove(event);
  }
  postEventEvaluation(event);
}

void TLayeredGuiScene::onMouseButtonPress(TGuiEvent event) {
  preEventEvaluation(event);
  for (unsigned int i = 0; i < layers_.size(); i++) {
    if (layers_[i]->active()) layers_[i]->onMouseButtonPress(event);
  }
  postEventEvaluation(event);
}

void TLayeredGuiScene::onMouseButtonRelease(TGuiEvent event) {
  preEventEvaluation(event);
  for (unsigned int i = 0; i < layers_.size(); i++) {
    if (layers_[i]->active()) layers_[i]->onMouseButtonRelease(event);
  }
  postEventEvaluation(event);
}

void TLayeredGuiScene::onMouseDoubleClick(TGuiEvent event) {
  preEventEvaluation(event);
  for (unsigned int i = 0; i < layers_.size(); i++) {
    if (layers_[i]->active()) layers_[i]->onMouseDoubleClick(event);
  }
  postEventEvaluation(event);
}

void TLayeredGuiScene::onMouseEnter(TGuiEvent event) {
  preEventEvaluation(event);
  for (unsigned int i = 0; i < layers_.size(); i++) {
    if (layers_[i]->active()) layers_[i]->onMouseEnter(event);
  }
  postEventEvaluation(event);
}

void TLayeredGuiScene::onMouseExit(TGuiEvent event) {
  preEventEvaluation(event);
  for (unsigned int i = 0; i < layers_.size(); i++) {
    if (layers_[i]->active()) layers_[i]->onMouseExit(event);
  }
  postEventEvaluation(event);
}

void TLayeredGuiScene::onDrawSizeChanged(int drawW__, int drawH__) {
  for (unsigned int i = 0; i < layers_.size(); i++) {
    layers_[i]->onDrawSizeChanged(drawW__, drawH__);
  }
}

void TLayeredGuiScene::onScrollPosChanged(double xScroll__, double yScroll__) {
  for (unsigned int i = 0; i < layers_.size(); i++) {
    layers_[i]->onScrollPosChanged(xScroll__, yScroll__);
  }
}

void TLayeredGuiScene::onZoomChanged(double zoom__) {
  for (unsigned int i = 0; i < layers_.size(); i++) {
    layers_[i]->onZoomChanged(zoom__);
  }
}

void TLayeredGuiScene::drawNative(TGraphic& dst) {
  for (unsigned int i = 0; i < layers_.size(); i++) {
    if (layers_[i]->visible()) layers_[i]->drawNative(dst);
  }
}

void TLayeredGuiScene::drawScaled(TGraphic& dst) {
  for (unsigned int i = 0; i < layers_.size(); i++) {
    if (layers_[i]->visible()) layers_[i]->drawScaled(dst);
  }
}

void TLayeredGuiScene::addLayer(TGuiSceneLayer* layer) {
  layers_.push_back(layer);
}

void TLayeredGuiScene::preEventEvaluation(TGuiEvent event) {
  
}

void TLayeredGuiScene::postEventEvaluation(TGuiEvent event) {
  
}


}
