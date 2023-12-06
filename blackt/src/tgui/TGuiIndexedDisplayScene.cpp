#include "tgui/TGuiIndexedDisplayScene.h"
#include "util/MiscMath.h"
#include <iostream>

namespace BlackT {


TGuiIndexedDisplayScene::~TGuiIndexedDisplayScene() {

}

TGuiIndexedDisplayScene::TGuiIndexedDisplayScene()
  : TGuiIndexedPickerScene() {
  pickerVisible_ = false;
  highlighterVisible_ = false;
//  pickerLayer_.setVisible(false);
//  pickerLayer_.setActive(false);
}


}
