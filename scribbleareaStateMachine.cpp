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

#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif

#include "scribbleareaStateMachine.h"
#ifdef USE_NEW_STATEMACHINE


//! [0]
ControllingStateMachine::ControllingStateMachine()
   :   Idle(*this),
       WaitingToLeaveJitterProtectionForDrawing(*this),
       WaitingToLeaveJitterProtectionWithSelectedAreaForMoving(*this),
       WaitingToLeaveJitterProtectionForScrolling(*this),
       WaitingToLeaveJitterProtectionWithCreatedPostitForMoving(*this),
       WaitingToLeaveJitterProtectionWithSelectedPostitForMoving(*this),
       Drawing(*this),
       DrawingPaused(*this),
       DrawingFillRequested(*this),
       MovingSelection(*this),
       MovingSelectionPaused(*this),
       MovingPostit(*this),
       ScrollingDrawingArea(*this),
       WaitingForTouchScrolling(*this),
       TouchScrollingDrawingArea(*this)
{
    Context.ShowPointer = false;

    //Context.ShowOverview = false;

    Context.LastDrawingValid = false;
    //Context.EraseLastDrawnObject = false;
    Context.Frozen = false;
    Context.myPenWidth = 2;
    //Context.SelectedPenWidth = StateMachine.Context.myPenWidth;

    QObject::connect(&MyTimer, &QTimer::timeout, this, &ControllingStateMachine::Timeout);
    MyTimer.setSingleShot(true);

    QObject::connect(&PointerTimer, &QTimer::timeout, this, &ControllingStateMachine::PointerTimeout);
    PointerTimer.setSingleShot(true);

    //Context.SelectedPostit.clear();
    Context.SelectPostitsDirectly = false;
    //Context.ShowPostitsFrame = false;



    //Context.RecentlyPastedObjectValid = false;
    //Context.MarkerActive = false;
   // QImage RecentlyMovedObject;
   // BoundingBoxClass RecentlyMovedObjectBoundingBox;


}

//! [0]
//! [0]

//! [1]

//! [2]

//! [3]

template<>
void StateClass<State::ScribblingState::Idle>::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
    std::cout << "Button Down: " << Button  << std::endl;
    if (Button == Qt::LeftButton) {
       StateMachine.Interface.setCursor(Qt::ArrowCursor);

        StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionForDrawing);


        StateMachine.Tracker.StartTracking(Position, Timestamp);


        StateMachine.Context.lastPoint = Position;
        StateMachine.Context.ButtonDownPosition = Position;
        StateMachine.Context.CurrentPaintedObjectBoundingBox.Clear();
        StateMachine.Context.CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(StateMachine.Context.lastPoint.x(), StateMachine.Context.lastPoint.y()));
        StateMachine.Context.SelectedCurrentPosition = Position;

        if ((StateMachine.Context.SelectPostitsDirectly == true) &&
            (StateMachine.Interface.PostItSelected(StateMachine.Context.SelectedCurrentPosition))) {
              StateMachine.Context.DownInsideObject = false;
              StateMachine.MyTimer.start(StateMachine.Settings.DirectSelectTimeout);

        } else {
           StateMachine.MyTimer.start(StateMachine.Settings.SelectTimeout);


           if (StateMachine.Context.LastPaintedObjectBoundingBox.IsInside(PositionClass(Position.x(), Position.y()))) {
              StateMachine.Context.DownInsideObject = true;
           } else {
              StateMachine.Context.DownInsideObject = false;
           }
        }
    }
}

void StateBaseClass::HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp)
{
   std::cout << "HandlePressEventSM: unexpected State: " << StateId()  << std::endl;
   StateMachine.Idle.HandlePressEventSM(Button, Position, Timestamp);
}

void ControllingStateMachine::PointerTimeout()
{
   Context.ShowPointer = false;
   Interface.UpdateRequest();
}

void ControllingStateMachine::Timeout()
{
}





void StateBaseClass::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{

   StateMachine.Tracker.Trackmovement(Position, Timestamp);
   //WaitForPostIt = false;
   StateMachine.Context.Showeraser = Erasing;
   StateMachine.Context.LastPointerPosition = Position;
}

void ControllingStateMachine::ShowBigPointer()
{
   if (Settings.PointerHoldon >= 0) {
   Context.ShowPointer = true;
   PointerTimer.start(Settings.PointerHoldon);
   Interface.UpdateRequest();
   }
}

void StateBaseClass::HandleMoveNoLeftButtonEvent(Qt::MouseButtons Buttons, QPointF Position)
{
   if (Buttons == Qt::NoButton) {
      if (StateMachine.Interface.IsInsideAnyPostIt(Position)) {
         StateMachine.Interface.setCursor(Qt::PointingHandCursor);
      }  else {
         StateMachine.Interface.setCursor(Qt::ArrowCursor);
      }
   } else {
     StateMachine.Interface.setCursor(Qt::ArrowCursor);
   }
}

template<>
void StateClass<State::ScribblingState::Idle>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();
   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::Drawing>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.MyTimer.stop();
       StateMachine.MyTimer.start(StateMachine.Settings.GestureTimeout);
       if (StateMachine.Context.LastDrawingValid) {
         StateMachine.Interface.DrawLastDrawnPicture();

         StateMachine.Context.LastDrawingValid = false;
         StateMachine.Context.LastDrawnObjectPoints.clear();
         StateMachine.Context.LastDrawnObjectPoints.append(StateMachine.Context.lastPoint);

      }
      //LastDrawnObject.fill(BackgroundColor);
      if (Erasing) {
         StateMachine.Interface.EraseLineTo(Position, Pressure);
      } else {
         StateMachine.Interface.drawLineTo(Position, Pressure);
      }
      StateMachine.Context.LastDrawnObjectPoints.append(Position);
      StateMachine.Context.CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForDrawing>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.Drawing);
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }
}

template<>
void StateClass<State::ScribblingState::DrawingPaused>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.DrawingFillRequested);
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::DrawingFillRequested>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if ((Position-StateMachine.Context.FillPolygonStartPosition).manhattanLength() > (StateMachine.Context.myPenWidth*3+2)) {
          StateMachine.SetNewState(&StateMachine.Drawing);
          StateMachine.Interface.setCursor(Qt::ArrowCursor);

       }
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::MovingSelection>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       // QPoint Offset = event->pos() - SelectedPoint;
         StateMachine.MyTimer.stop();
         StateMachine.MyTimer.start(StateMachine.Settings.CopyTimeout);

         StateMachine.Context.SelectedCurrentPosition = Position;
         StateMachine.Interface.UpdateRequest();
       // DrawMovedSelection(Offset);
       // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
       // MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
       // drawrectangle(MovedRectangle);
        //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.MovingSelection);
       StateMachine.MovingSelection.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}


template<>
void StateClass<State::ScribblingState::MovingSelectionPaused>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.MovingSelection);
       StateMachine.MovingSelection.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::MovingPostit>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (StateMachine.Interface.AreAnyPostitsSelected()) {
          std::cout << "Moving postit " << std::endl;
          StateMachine.Interface.MoveSelectedPostits(Position);
          StateMachine.Interface.UpdateRequest();
       }

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.MovingPostit);
       StateMachine.MovingPostit.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.MovingPostit);
       StateMachine.MovingPostit.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::ScrollingDrawingArea>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.Interface.CompleteImage();
      StateMachine.Context.Origin -= Position- StateMachine.Context.ScrollingLastPosition;
      if (!StateMachine.Context.Frozen) {
         StateMachine.Context.BackgroundImagesOrigin -= Position- StateMachine.Context.ScrollingLastPosition;
      }

      StateMachine.Context.ScrollingLastPosition = Position;
      StateMachine.Interface.UpdateRequest();

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForScrolling>::HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);

   if ((Buttons & Qt::LeftButton)) {
       if ((Position-StateMachine.Context.lastPoint).manhattanLength() < StateMachine.Context.myPenWidth+2) {
           return; // ignore small movements (probably use penwidth*2)
       }
       if (((Position-StateMachine.Context.ButtonDownPosition).manhattanLength() < (StateMachine.Context.myPenWidth*3+2))) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.ScrollingDrawingArea);
       StateMachine.ScrollingDrawingArea.HandleMoveEventSM(Buttons, Position, Timestamp, Erasing, Pressure);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}




template<State::ScribblingState State>
State::ScribblingState StateClass<State>::StateId() {return TheState;}


template<>
void StateClass<State::ScribblingState::Idle>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      // Do Nothing
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::Drawing>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.MyTimer.stop();
      if (Erasing) {
         StateMachine.Interface.EraseLineTo(Position, Pressure);
      } else {
         StateMachine.Interface.drawLineTo(Position, Pressure);
      }
      StateMachine.Context.LastDrawnObjectPoints.append(Position);
      StateMachine.Context.LastDrawingValid = true;
      StateMachine.Context.CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));
      StateMachine.Context.LastPaintedObjectBoundingBox = StateMachine.Context.CurrentPaintedObjectBoundingBox;
      StateMachine.Context.CurrentPaintedObjectBoundingBox.Clear();

      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForDrawing>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Drawing.HandleReleaseEventSM(Button, Position, Erasing, Pressure);
   }
}

template<>
void StateClass<State::ScribblingState::DrawingFillRequested>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Drawing.HandleReleaseEventSM(Button, Position, Erasing, Pressure);
      StateMachine.Interface.FilllastDrawnShape();
      StateMachine.Interface.setCursor(Qt::ArrowCursor);
      //FillPolygon = false;

      StateMachine.Interface.UpdateRequest();
   }
}

template<>
void StateClass<State::ScribblingState::DrawingPaused>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.DrawingFillRequested.HandleReleaseEventSM(Button, Position, Erasing, Pressure);

   }
}

template<>
void StateClass<State::ScribblingState::MovingSelection>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      //WaitForPostIt = false;
       if (StateMachine.Tracker.GetCurrentSpeed() > 0.25f) {
           std::cout << "LeavingSpeed " << StateMachine.Tracker.GetCurrentSpeed() << std::endl;
           StateMachine.Context.DiscardSelection = true;
           StateMachine.Interface.UpdateRequest();
       }
       //std::cout << "LeavingSpeed " << (LastDistance + CurrentDistance) << " / " << (DeltaTLastDistance + DeltaTCurrentDistance) << " = " << ((LastDistance + CurrentDistance) / (float)(DeltaTLastDistance + DeltaTCurrentDistance)) << std::endl;
           ;
     // QPoint Offset =  - SelectedPoint;
       if (StateMachine.Context.DiscardSelection == false) {
          StateMachine.Interface.DrawMovedSelection(Position);
       }
     // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
    //  MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
    //  drawrectangle(MovedRectangle);
     // MoveSelected = false;
      StateMachine.Context.LastDrawingValid = false;
      //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}
template<>
void StateClass<State::ScribblingState::MovingSelectionPaused>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.MovingSelection.HandleReleaseEventSM(Button, Position, Erasing, Pressure);

   }
}
template<>
void StateClass<State::ScribblingState::MovingPostit>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      if ((StateMachine.Interface.AreAnyPostitsSelected())) {
         std::cout << "Fixing postit " << std::endl;
         StateMachine.Interface.FinishMovingSelectedPostits(Position);

         //  MoveSelected = false;
         StateMachine.Context.LastDrawingValid = false;
         StateMachine.Interface.ClearSelectedPostits();
         StateMachine.Interface.setCursor(Qt::ArrowCursor);
         StateMachine.Interface.UpdateRequest();
      }
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Interface.ClearSelectedPostits();
      StateMachine.Interface.setCursor(Qt::ArrowCursor);

      StateMachine.SetNewState(&StateMachine.Idle);

   }
}

template<>
void StateClass<State::ScribblingState::ScrollingDrawingArea>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.Origin -= Position - StateMachine.Context.ScrollingLastPosition;
      if (!StateMachine.Context.Frozen) {
         StateMachine.Context.BackgroundImagesOrigin -= Position- StateMachine.Context.ScrollingLastPosition;
      }
      StateMachine.Interface.resizeScrolledImage();
      StateMachine.Interface.UpdateRequest();

      StateMachine.SetNewState(&StateMachine.Idle);

   }
}
template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForScrolling>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   if (Button == Qt::LeftButton) {
      StateMachine.ScrollingDrawingArea.HandleReleaseEventSM(Button, Position, Erasing, Pressure);

   }
}


void ControllingStateMachine::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure)
{
   std::cout << "Button Up: " << Button  << std::endl;

}


void StateBaseClass::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   StateMachine.Context.SelectedCurrentPosition = MeanPosition;
   StateMachine.Interface.MakeSreenMoveHint();
   //Scrolling = true;
   StateMachine.Context.ScrollingLastPosition = StateMachine.Context.SelectedCurrentPosition;
   StateMachine.Context.ScrollingOldOrigin = StateMachine.Context.Origin;
   //scribbling = false;
   StateMachine.Interface.UpdateRequest();
   StateMachine.SetNewState(&StateMachine.WaitingForTouchScrolling);
   }

}
void ControllingStateMachine::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
}

template<>
void StateClass<State::ScribblingState::TouchScrollingDrawingArea>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   std::cout << "TM(" << MeanPosition.x() <<":" << MeanPosition.y() << ")";

         StateMachine.Interface.CompleteImage();
         StateMachine.Context.Origin -= MeanPosition- StateMachine.Context.ScrollingLastPosition;
         if (!StateMachine.Context.Frozen) {
            StateMachine.Context.BackgroundImagesOrigin -= MeanPosition- StateMachine.Context.ScrollingLastPosition;
         }
         StateMachine.Context.ScrollingLastPosition = MeanPosition;
         StateMachine.Interface.UpdateRequest();
   }

}
template<>
void StateClass<State::ScribblingState::WaitingForTouchScrolling>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   std::cout << "TM(" << MeanPosition.x() <<":" << MeanPosition.y() << ")";

   StateMachine.SetNewState(&StateMachine.TouchScrollingDrawingArea);
   StateMachine.TouchScrollingDrawingArea.HandleTouchMoveEventSM(NumberOfTouchpoints, MeanPosition);
   }
}


void ControllingStateMachine::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
}

template<>
void StateClass<State::ScribblingState::TouchScrollingDrawingArea>::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
      StateMachine.Context.Origin -= MeanPosition - StateMachine.Context.ScrollingLastPosition;
      if (!StateMachine.Context.Frozen) {
         StateMachine.Context.BackgroundImagesOrigin -= MeanPosition- StateMachine.Context.ScrollingLastPosition;
      }
      StateMachine.Interface.resizeScrolledImage();
      StateMachine.Interface.UpdateRequest();

      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::ScribblingState::WaitingForTouchScrolling>::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {

      StateMachine.TouchScrollingDrawingArea.HandleTouchMoveEventSM(NumberOfTouchpoints, MeanPosition);
   }
}

void ControllingStateMachine::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{

}



void StateBaseClass::timeoutSM()
{
   // Default does nothing
}


template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionForDrawing>::timeoutSM()
{
   if (StateMachine.Context.DownInsideObject) {

      StateMachine.Interface.MakeSelectionFromLastDrawnObject();

      StateMachine.Context.LastDrawnObjectPoints.clear();
      StateMachine.Interface.modified();

      //MoveSelected = true;
      //NewDrawingStarted = false;
      StateMachine.Context.SelectedPoint = StateMachine.Context.lastPoint;
      StateMachine.Context.SelectedOffset = QPoint(StateMachine.Context.LastPaintedObjectBoundingBox.GetLeft(), StateMachine.Context.LastPaintedObjectBoundingBox.GetTop()) - StateMachine.Context.lastPoint;
      //scribbling = false;
      StateMachine.Interface.UpdateRequest();
      //WaitForPostIt = true;
      StateMachine.MyTimer.start(StateMachine.Settings.PostItTimeout);
      StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithSelectedAreaForMoving);

   } else {

      if (StateMachine.Interface.PostItSelected(StateMachine.Context.SelectedCurrentPosition)) {
         //scribbling = false;
         //NewDrawingStarted = false;
         StateMachine.Interface.setCursor(Qt::ClosedHandCursor);
         std::cout << "Selected postit " << std::endl;
         StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithSelectedPostitForMoving);


      } else {
         StateMachine.Interface.MakeSreenMoveHint();
         //Scrolling = true;
         StateMachine.Context.ScrollingLastPosition = StateMachine.Context.SelectedCurrentPosition;
         StateMachine.Context.ScrollingOldOrigin = StateMachine.Context.Origin;
         //scribbling = false;
         StateMachine.Interface.UpdateRequest();
         StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionForScrolling);
      }
   }
}


template<>
void StateClass<State::ScribblingState::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>::timeoutSM()
{
   {
      std::cout << "Creating postit " << std::endl;

      //WaitForPostIt = false;
      StateMachine.Interface.CreeatePostitFromSelection();
      StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithCreatedPostitForMoving);
      StateMachine.Interface.UpdateRequest();
   }

}

template<>
void StateClass<State::ScribblingState::Drawing>::timeoutSM()
{
   //   QTextStream(stdout) << "<" << GestureTrackerAccumulatedSpeed.x() << "; " << GestureTrackerAccumulatedSpeed.y() << "> <"  << GestureTrackerAccumulatedSquaredSpeed.x() << ";" << GestureTrackerAccumulatedSquaredSpeed.y() << endl;

   //FillPolygon = true;
   StateMachine.Context.FillPolygonStartPosition = StateMachine.Context.lastPoint;
   StateMachine.Interface.setCursor(Qt::BusyCursor);
   StateMachine.Interface.UpdateRequest();
   StateMachine.SetNewState(&StateMachine.DrawingPaused);

}
template<>
void StateClass<State::ScribblingState::MovingSelection>::timeoutSM()
{
   if (StateMachine.Context.DiscardSelection == false) {

      // Fast shaking followed by a pause means throw away selection
      if (StateMachine.Tracker.IsFastShaking()) {
         StateMachine.Context.DiscardSelection = true;
         StateMachine.Interface.UpdateRequest();
      } else {
         QPointF CopyPos(StateMachine.Context.SelectedCurrentPosition);
         StateMachine.Context.SelectedCurrentPosition += QPointF(3,3);
         StateMachine.Interface.DrawMovedSelection(CopyPos);
      }
      StateMachine.SetNewState(&StateMachine.MovingSelectionPaused);
   }

}

#endif

