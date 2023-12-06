#ifndef TGUIEVENT_H
#define TGUIEVENT_H


namespace BlackT {


class TGuiEvent {
public:
  enum MouseEventType {
    mouseEventNone,
    mouseEventMove,
    mouseEventButtonPress,
    mouseEventButtonRelease,
    mouseEventButtonDoubleClick,
    mouseEventEnter,
    mouseEventExit
  };

  TGuiEvent();
  virtual ~TGuiEvent();
  
  bool modifierShift;
  bool modifierControl;
  bool modifierAlt;
  
  MouseEventType mouseEventType;
  bool mouseLeftButton;
  bool mouseRightButton;
  bool mouseMiddleButton;
  double mouseX;
  double mouseY;
  double rawMouseX;
  double rawMouseY;
  
  /**
   * Morphs widget-local coordinates to scene-local coordinates.
   * This accounts for zoom and scrolling, turning the raw coordinates into
   * a position within the virtual scene.
   * Coordinates are also bounded to within the scene's area.
   */
/*  void makeCoordinatesVirtual(double scrollX, double scrollY,
                               double width, double height,
                               double zoom);
  
  void makeCoordinatesReal(double scrollX, double scrollY,
//                           double width, double height,
                           double zoom); */
  
protected:
/*  double morphCoordinateToSceneInternal(double source,
                               double scrollAmount,
                               double lowerLimit,
                               double upperLimit,
                               double zoom);
  double morphCoordinateFromSceneInternal(double source,
                               double scrollAmount,
                               double zoom); */
  
};


}


#endif
