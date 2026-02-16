/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SCRIBBLEAREASTATEMACHINE_H
#define SCRIBBLEAREASTATEMACHINE_H

#define USE_NEW_STATEMACHINE

#ifdef USE_NEW_STATEMACHINE

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QTimer>
#include <QPolygon>
#include <QTouchEvent>
#include <list>

#include "global.h"
//#include "box.hpp"
#include "Settings.hpp"
#include "gesturetracker.hpp"
#include "interface.hpp"
#include "databaseclass.h"

//! [0]
//!
//!
//!



#define MakeStateObject(S) StateClass<State::ScribblingState::S> S
class StateBaseClass;
class ControllingStateMachine;
//class DatabaseClass;

namespace State {
enum ScribblingState {
   Idle,
   WaitingToLeaveJitterProtectionForDrawing,
   WaitingToLeaveJitterProtectionWithSelectedAreaForMoving,
   WaitingToLeaveJitterProtectionForScrolling,
   WaitingToLeaveJitterProtectionWithCreatedPostitForMoving,
   WaitingToLeaveJitterProtectionWithSelectedPostitForMoving,
   WaitingToLeaveJitterProtectionWithSelectedPostitForDeletingOrMoving,
   Drawing,
 //  DrawingPaused,
   DrawingFillRequested,
   DrawingKillRequested,
   MovingSelection,
   MovingSelectionPaused,
   MovingPostit,
   MovingPostitPaused,
   ScrollingDrawingArea,
   WaitingForTouchScrolling,
   TouchScrollingDrawingArea,
   WaitingToSelectRegionFromOverview,
   WaitingToPasteClippboardImage
};
}
class PenInfoClass {
public:
   float tiltx;
   float tilty;
   float rotation;
   float Pressure;
   bool  Erasing;

   PenInfoClass(QTabletEvent * event) :
       tiltx(event->xTilt()),
       tilty(event->yTilt()),
       rotation(event->rotation()),
       Pressure(event->pressure()),
       Erasing(event->pointerType() == QPointingDevice::PointerType::Eraser)
   {}
   PenInfoClass(MAY_BE_UNUSED nullptr_t event) :
       tiltx(0),
       tilty(0),
       rotation(0),
       Pressure(0),
       Erasing(false)
   {}
   PenInfoClass(bool Eraser) :
       tiltx(0),
       tilty(0),
       rotation(0),
       Pressure(0),
       Erasing(Eraser)
   {}
#if 0
   /*
# [tiltXrad, tiltYrad] => [azimuthRad, altitudeRad]
# tiltX and tiltY should be in the range [0, pi/2]
def tilt2spherical(inVec):
    tiltXrad, tiltYrad = inVec[0], inVec[1]

    if tiltXrad == 0 and tiltYrad == 0:
        # pen perpendicular to the pad
        return [0, math.pi/2]

    # X and Y of a vector created by the intersection of tilt planes
    # first normal vectors of tiltX and tiltY planes are defined
    # from that cross product is done to find this vector perpendicular to both plane's normal vector
    # in this unit vector Z is ignored to get x y coords projected on the pad
    y = math.cos(tiltXrad) * math.sin(tiltYrad)
    x = -math.sin(tiltXrad) * -math.cos(tiltYrad)
    z = -math.cos(tiltXrad)*-math.cos(tiltYrad)
    # compute angle of the projected 2D vector to get azimuth in the proper direction
    azimuthRad = -math.atan2(y, x) + math.pi/2
    if azimuthRad < 0:
        # make always positive in range from 0 to 2*pi
        azimuthRad += 2*math.pi

    vecLenOn2DPad = math.sqrt(x*x+y*y)
    altitudeRad = math.atan(z / vecLenOn2DPad)

    # other possible, simpler way to get altitudeRad which is not 100% correct:
    # deviation: max(7.96°) / avg(2.00°) / median(0.91°)
    # not derived from anything, just two 2D situations combined by a multiplication
    # altitudeRad = math.pi/2-math.acos(math.cos(tiltXrad) * math.cos(tiltYrad))

    return [azimuthRad, altitudeRad]


*/
#endif
};

class StateBaseClass {
protected:
   ControllingStateMachine &StateMachine;
   void StartTimer(double TimeInms);
   void StoppTimer();

public:
   typedef DatabaseClass::PasteEvent PasteEvent;
   typedef std::chrono::milliseconds Milliseconds;

   StateBaseClass(ControllingStateMachine &sm) : StateMachine(sm) {}
   virtual void HandleProximityEventSM(QPointF Position, bool InProximity);
   virtual void HandlePressEventSM(Qt::MouseButton Button, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp);
   virtual void HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo);
  // void HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, QPointF ScaledPosition, Milliseconds Timestamp, const PenInfoClass &PenInfo);
   virtual void HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo);
   virtual void HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   virtual void HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   virtual void HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   virtual void HandleOverviewEventSM(bool Enabled);
   virtual void HandlePasteEventSM(QImage ImageToPaste);
   virtual void HandleKeyEventSM(PasteEvent Event);
   virtual void timeoutSM();
   virtual State::ScribblingState StateId() = 0;
 //  virtual std::string StateName() = 0;
   virtual ~StateBaseClass() {}

   void HandleMoveNoLeftButtonEvent(Qt::MouseButtons Buttons, QPointF Position);
};

template<State::ScribblingState State>
class StateClass : public StateBaseClass {
   static constexpr State::ScribblingState TheState = State;
public:
   StateClass(ControllingStateMachine &sm) : StateBaseClass(sm) {}
   virtual void HandleProximityEventSM(QPointF Position, bool InProximity) override;
   virtual void HandlePressEventSM(Qt::MouseButton Button, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp) override;
   virtual void HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo) override;
   virtual void HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo) override;
   virtual void HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition) override;
   virtual void HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition) override;
   virtual void HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition) override;
   virtual void HandleOverviewEventSM(bool Enabled) override;
   virtual void HandlePasteEventSM(QImage ImageToPaste) override;
   virtual void HandleKeyEventSM(PasteEvent Event) override;
   virtual void timeoutSM() override;
   virtual ~StateClass() override {}
   virtual State::ScribblingState StateId() override;
   //virtual std::string StateName() override;

};

class ControllingStateMachine : public QObject {
   Q_OBJECT
private:
   template<State::ScribblingState S> friend  class StateClass;
   friend  class StateBaseClass;
   enum State::ScribblingState State;

   GestureTrackerClass Tracker;

   QTimer MyTimer;
   QTimer PointerTimer;


   struct Context {

      DatabaseClass &MyDatas;
#if 0
      QPointF lastPoint;
      QPointF ButtonDownPosition;
      QPointF SelectedPoint;
      QPointF SelectedOffset;
      QPointF SelectedCurrentPosition;

      bool    DiscardSelection;
      bool    LastDrawingValid;
      int     myPenWidth;

      QPolygonF LastDrawnObjectPoints;

      QPointF Origin;
      bool Frozen;
      QPointF BackgroundImagesOrigin;



      BoundingBoxClass RecentlyPastedObjectBoundingBox;

      BoundingBoxClass LastPaintedObjectBoundingBox;
      BoundingBoxClass CurrentPaintedObjectBoundingBox;
#endif
      bool    SelectPostitsDirectly;
      bool    DownInsideObject;
      bool    SimpleInterface;

      bool    ShowPointer;
      bool    Showeraser;
      bool    SpongeAsEraser;

      QPointF FillPolygonStartPosition;
      QPointF ScrollingLastPosition;
      QPointF ScrollingOldOrigin;

   public:
      Context(DatabaseClass &db) : MyDatas(db) {}

   } Context;
   SettingClass &Settings;

   GuiInterface Interface;
   StateBaseClass *CurrentState;

   MakeStateObject(Idle);
   MakeStateObject(WaitingToLeaveJitterProtectionForDrawing);
   MakeStateObject(WaitingToLeaveJitterProtectionWithSelectedAreaForMoving);
   MakeStateObject(WaitingToLeaveJitterProtectionForScrolling);
   MakeStateObject(WaitingToLeaveJitterProtectionWithCreatedPostitForMoving);
   MakeStateObject(WaitingToLeaveJitterProtectionWithSelectedPostitForMoving);
   MakeStateObject(WaitingToLeaveJitterProtectionWithSelectedPostitForDeletingOrMoving);
   MakeStateObject(Drawing);
 //  MakeStateObject(DrawingPaused);
   MakeStateObject(DrawingFillRequested);
   MakeStateObject(DrawingKillRequested);
   MakeStateObject(MovingSelection);
   MakeStateObject(MovingSelectionPaused);
   MakeStateObject(MovingPostit);
   MakeStateObject(MovingPostitPaused);
   MakeStateObject(ScrollingDrawingArea);
   MakeStateObject(WaitingForTouchScrolling);
   MakeStateObject(TouchScrollingDrawingArea);
   MakeStateObject(WaitingToSelectRegionFromOverview);
   MakeStateObject(WaitingToPasteClippboardImage);

public:
   typedef std::chrono::milliseconds Milliseconds;

   void ShowBigPointer();
   void DrawGestureTrackerDebugInfo(QPainter &Painter, QPointF Offset) {Tracker.DrawDebugInfo(Painter, Offset);}

   void UseSpongeAsEraser(bool SpongeMode) {Context.SpongeAsEraser = SpongeMode;}

   void setDirectSelect(bool Mode) {Context.SelectPostitsDirectly = Mode;}
   void setSimpleInterface(bool Mode) {Context.SimpleInterface = Mode;}
   void SetNewState(StateBaseClass *NewState);
   enum PointerType {NONE, DRAWER, ERASER, WIPER};
   PointerType  PointerTypeToShow();
   QPointF getLastPointerPosition() {return Context.MyDatas.getLastPointerPosition();}
   bool IsScrollingState() {return (CurrentState == &ScrollingDrawingArea)||(CurrentState == &WaitingToLeaveJitterProtectionForScrolling)
   ||(CurrentState == &WaitingForTouchScrolling) ||(CurrentState == &TouchScrollingDrawingArea);}

   bool IsInSelectingState() {return ((CurrentState == &MovingSelection)||(CurrentState == &WaitingToLeaveJitterProtectionWithSelectedAreaForMoving)||(CurrentState == &MovingSelectionPaused));}

   void HandleProximityEventSM(QPointF Position, bool InProximity);
   void HandlePressEventSM(Qt::MouseButton Button, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp);
   void HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo);
   void HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo);
   void HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   void HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   void HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   void HandleOverviewEventSM(bool Enabled);
   void HandlePasteEventSM(QImage ImageToPaste);
   void HandleKeyEventSM(DatabaseClass::PasteEvent Event);

   ControllingStateMachine(DatabaseClass &Database, GuiInterface &Interface, class SettingClass &MySettings);


public slots:
   void timeoutSM();

private slots:
   void PointerTimeout();
   void Timeout();
   void HandleGesture();

signals:
   void GestureDetected();

};

inline void StateBaseClass::StartTimer(double TimeInms) {StateMachine.MyTimer.start(static_cast<int>(TimeInms));}
inline void StateBaseClass::StoppTimer() {StateMachine.MyTimer.stop();}

#endif


#endif
