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

#ifdef USE_NEW_STATEMACHINE

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QTimer>
#include <QPolygon>
#include <QTouchEvent>
#include <list>

#include "box.hpp"
#include "gesturetracker.hpp"

//! [0]
//!
class Settings {
public:
   double Touchscaling = 4.0;
   double DirectSelectTimeout = 10.0;
   double CopyTimeout = 500;
   double GestureTimeout = 500;
   double SelectTimeout = 500;
   double PostItTimeout = 1000;
   double PointerHoldon = 250;

};
#define MakeStateObject(S) StateClass<State::ScribblingState::S> S

class StateBaseClass;
class ControllingStateMachine;

namespace State {
enum ScribblingState {
   Idle,
   WaitingToLeaveJitterProtectionForDrawing,
   WaitingToLeaveJitterProtectionWithSelectedAreaForMoving,
   WaitingToLeaveJitterProtectionForScrolling,
   WaitingToLeaveJitterProtectionWithCreatedPostitForMoving,
   WaitingToLeaveJitterProtectionWithSelectedPostitForMoving,
   Drawing,
   DrawingPaused,
   DrawingFillRequested,
   MovingSelection,
   MovingSelectionPaused,
   MovingPostit,
   ScrollingDrawingArea,
   WaitingForTouchScrolling,
   TouchScrollingDrawingArea
};
}

class StateBaseClass {
protected:
   ControllingStateMachine &StateMachine;
public:
   StateBaseClass(ControllingStateMachine &sm) : StateMachine(sm) {}
   virtual void HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp);
   virtual void HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure);
   virtual void HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure);
   virtual void HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   virtual void HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   virtual void HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   virtual void timeoutSM();
   virtual State::ScribblingState StateId() = 0;
   virtual ~StateBaseClass();
   void    HandleMoveNoLeftButtonEvent();
};

template<State::ScribblingState State>
class StateClass : public StateBaseClass {
   static constexpr State::ScribblingState TheState = State;
public:
   StateClass(ControllingStateMachine &sm) : StateBaseClass(sm) {}
   virtual void HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp) override;
   virtual void HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure) override;
   virtual void HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure) override;
   virtual void HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition) override;
   virtual void HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition) override;
   virtual void HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition) override;
   virtual void timeoutSM() override;
   virtual ~StateClass() override;
   virtual State::ScribblingState StateId() override;
};

class ControllingStateMachine {
private:
   template<State::ScribblingState S> friend  class StateClass;
   friend  class StateBaseClass;
   enum State::ScribblingState State;

   GestureTrackerClass Tracker;

   QTimer MyTimer;
   QTimer PointerTimer;


   struct Context {
      QPointF lastPoint;
      QPointF ButtonDownPosition;
      QPointF SelectedPoint;
      QPointF SelectedOffset;
      QPointF SelectedCurrentPosition;
      QPointF ScrollingLastPosition;
      QPointF ScrollingOldOrigin;
      QPointF FillPolygonStartPosition;

      QPointF LastPointerPosition;
      bool    ShowPointer;
      bool    Showeraser;
      bool    DownInsideObject;
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


   } Context;
   Settings Settings;

   StateBaseClass *CurrentState;

   MakeStateObject(Idle);
   MakeStateObject(WaitingToLeaveJitterProtectionForDrawing);
   MakeStateObject(WaitingToLeaveJitterProtectionWithSelectedAreaForMoving);
   MakeStateObject(WaitingToLeaveJitterProtectionForScrolling);
   MakeStateObject(WaitingToLeaveJitterProtectionWithCreatedPostitForMoving);
   MakeStateObject(WaitingToLeaveJitterProtectionWithSelectedPostitForMoving);
   MakeStateObject(Drawing);
   MakeStateObject(DrawingPaused);
   MakeStateObject(DrawingFillRequested);
   MakeStateObject(MovingSelection);
   MakeStateObject(MovingSelectionPaused);
   MakeStateObject(MovingPostit);
   MakeStateObject(ScrollingDrawingArea);
   MakeStateObject(WaitingForTouchScrolling);
   MakeStateObject(TouchScrollingDrawingArea);

public:
   void ShowBigPointer();

   void SetNewState(StateBaseClass *NewState);

   void HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp);
   void HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure);
   void HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure);
   void HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   void HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   void HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   ControllingStateMachine();
public slots:
   void timeoutSM();

};
#endif


#endif
