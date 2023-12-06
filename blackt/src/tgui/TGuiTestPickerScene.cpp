#include "tgui/TGuiTestPickerScene.h"

namespace BlackT {


TGuiTestPickerScene::TGuiTestPickerScene()
  : TGuiIndexedPickerScene() {
  phase2Init();
  changeZoom(1.00);
}
  
int TGuiTestPickerScene::numItems() const {
  return 99;
}

int TGuiTestPickerScene::itemW() const {
  return 16;
}

int TGuiTestPickerScene::itemH() const {
  return 16;
}

void TGuiTestPickerScene::drawItem(int index,
              TGraphic& dst,
              int x, int y) {
  int level = (index * 16) % 256;
  TColor color = TColor(level, level, level);
  dst.fillRect(x, y, 16, 16, color, TGraphic::noTransUpdate);
}

int TGuiTestPickerScene::itemsPerRow() const {
  return -1;
}


}
