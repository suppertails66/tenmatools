#include "tgui/TGuiEvent.h"
#include "util/MiscMath.h"

namespace BlackT {


TGuiEvent::TGuiEvent()
  : modifierShift(false),
    modifierControl(false),
    modifierAlt(false),
    mouseEventType(mouseEventNone),
    mouseLeftButton(false),
    mouseRightButton(false),
    mouseMiddleButton(false),
    mouseX(0),
    mouseY(0),
    rawMouseX(0),
    rawMouseY(0) { }

TGuiEvent::~TGuiEvent() { }

/*void TGuiEvent::makeCoordinatesVirtual(double scrollX, double scrollY,
                                 double width, double height,
                                 double zoom) {
  mouseX = morphCoordinateToSceneInternal(mouseX, scrollX, 0, width, zoom);
  mouseY = morphCoordinateToSceneInternal(mouseY, scrollY, 0, height, zoom);
}

void TGuiEvent::makeCoordinatesReal(double scrollX, double scrollY,
//                         double width, double height,
                         double zoom) {
  mouseX = morphCoordinateFromSceneInternal(mouseX, scrollX, zoom);
  mouseY = morphCoordinateFromSceneInternal(mouseY, scrollY, zoom);
}

double TGuiEvent::morphCoordinateToSceneInternal(double source,
                                 double scrollAmount,
                                 double lowerLimit,
                                 double upperLimit,
                                 double zoom) {
  source += scrollAmount;
  source /= zoom;
  MiscMath::clamp(source, lowerLimit, upperLimit);
  return source;
}

double TGuiEvent::morphCoordinateFromSceneInternal(double source,
                                 double scrollAmount,
                                 double zoom) {
  source *= zoom;
  source -= scrollAmount;
  return source;
} */


}
