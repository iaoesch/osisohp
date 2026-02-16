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
#include "global.h"
#include "databaseclass.h"
#include "DebugStream.hpp"


template<>
void StateClass<State::Idle>::HandleProximityEventSM(QPointF Position, bool InProximity);

void StateBaseClass::HandleProximityEventSM(QPointF Position, bool InProximity)
{
    DEBUG_LOG << "HandleProximityEventSM: unexpected State: (Baseclass)" << StateId()  << std::endl;
    StateMachine.Idle.HandleProximityEventSM(Position, InProximity);
}

template<>
void StateClass<State::Idle>::HandlePressEventSM(Qt::MouseButton Button, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp);

void StateBaseClass::HandlePressEventSM(Qt::MouseButton Button, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp)
{
   DEBUG_LOG << "HandlePressEventSM: unexpected State: (Baseclass)" << StateId()  << std::endl;
   StateMachine.Idle.HandlePressEventSM(Button, Modifiers, Position, Timestamp);
}

void StateBaseClass::HandleMoveEventSM(MAY_BE_UNUSED Qt::MouseButtons Buttons, MAY_BE_UNUSED Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{

   StateMachine.Tracker.Trackmovement(Position, Timestamp);
   //WaitForPostIt = false;
   StateMachine.Context.Showeraser = PenInfo.Erasing;
   StateMachine.Context.MyDatas.setLastPointerPosition(Position);
}
#if 0
void StateBaseClass::HandleMoveEventSM(MAY_BE_UNUSED Qt::MouseButtons Buttons, MAY_BE_UNUSED Qt::KeyboardModifiers Modifiers, QPointF Position, QPointF ScaledPosition, Milliseconds Timestamp, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{

    StateMachine.Tracker.Trackmovement(Position, Timestamp);
    //WaitForPostIt = false;
    StateMachine.Context.Showeraser = PenInfo.Erasing;
    StateMachine.Context.LastPointerPosition = ScaledPosition;
}
#endif

void StateBaseClass::HandleReleaseEventSM(MAY_BE_UNUSED Qt::MouseButton Button, MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{

}

void StateBaseClass::HandleMoveNoLeftButtonEvent(Qt::MouseButtons Buttons, QPointF Position)
{
    if (Buttons == Qt::NoButton) {
        if (StateMachine.Context.SimpleInterface == false) {
            if (StateMachine.Context.MyDatas.IsInsideLastPaintedObjectBoundingBox(Position)) {
                if (StateMachine.Context.MyDatas.IsCutoutActive()) {
                    StateMachine.Interface.SetCursor(CursorManager::CutPossiblePointer );
                } else {
                    StateMachine.Interface.SetCursor(CursorManager::CopySelectionPossiblePointer );
                }
            }  else if (StateMachine.Context.MyDatas.IsInsideAnyPostIt(Position)) {
                StateMachine.Interface.SetCursor(CursorManager::SelectPossiblePointer);
            }  else {
                StateMachine.Interface.SetCursor(CursorManager::StandardPointer);
            }
        } else {
            StateMachine.Interface.SetCursor(CursorManager::StandardPointer);
        }
   } else {
      StateMachine.Interface.SetCursor(CursorManager::StandardPointer);
   }
}


void StateBaseClass::timeoutSM()
{
   // Default does nothing
}



void StateBaseClass::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
      StateMachine.Context.MyDatas.setSelectedCurrentPosition(MeanPosition);
      StateMachine.Context.MyDatas.MakeSreenMoveHint();
   //Scrolling = true;
   StateMachine.Context.ScrollingLastPosition = StateMachine.Context.MyDatas.getSelectedCurrentPosition();
   StateMachine.Context.ScrollingOldOrigin = StateMachine.Context.MyDatas.GetOrigin();
   //scribbling = false;
   StateMachine.Interface.UpdateRequest();
   StateMachine.SetNewState(&StateMachine.WaitingForTouchScrolling);
   }

}

void StateBaseClass::HandleTouchMoveEventSM(MAY_BE_UNUSED int NumberOfTouchpoints, MAY_BE_UNUSED QPointF MeanPosition)
{

}

void StateBaseClass::HandleTouchReleaseEventSM(MAY_BE_UNUSED int NumberOfTouchpoints, MAY_BE_UNUSED QPointF MeanPosition)
{
}

void StateBaseClass::HandleOverviewEventSM(MAY_BE_UNUSED bool Enabled)
{

}

void StateBaseClass::HandlePasteEventSM(MAY_BE_UNUSED QImage Image)
{

}

void StateBaseClass::HandleKeyEventSM(MAY_BE_UNUSED PasteEvent Event)
{

}

template<State::ScribblingState State>
State::ScribblingState StateClass<State>::StateId() {return TheState;}

template<State::ScribblingState State>
void StateClass<State>::HandleProximityEventSM(MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED bool InProximity)
{
    DEBUG_LOG << "Unexpected event HandleProximityEventSM() in default<> State " << StateId() << std::endl;
}


template<State::ScribblingState State>
void StateClass<State>::HandlePressEventSM(MAY_BE_UNUSED Qt::MouseButton Button, MAY_BE_UNUSED Qt::KeyboardModifiers Modifiers, MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED Milliseconds Timestamp)
{
   DEBUG_LOG << "Unexpected event HandlePressEventSM() in default<> State " << StateId() << std::endl;
}

template<State::ScribblingState State>
void StateClass<State>::HandleMoveEventSM(MAY_BE_UNUSED Qt::MouseButtons Buttons, MAY_BE_UNUSED Qt::KeyboardModifiers Modifiers, MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED Milliseconds Timestamp, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{
   DEBUG_LOG << "Unexpected event HandleMoveEventSM() in default<> State " << StateId() << std::endl;
}

template<State::ScribblingState State>
void StateClass<State>::HandleReleaseEventSM(MAY_BE_UNUSED Qt::MouseButton Button, MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED const PenInfoClass &PenInfo)

{
   DEBUG_LOG << "Unexpected event HandleReleaseEventSM() in default<>  State " << StateId() << std::endl;
}


template<State::ScribblingState State>
void StateClass<State>::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   //DEBUG_LOG << "Unexpected event HandlePressEventSM() in State " << StateId() << std::endl;
   StateBaseClass::HandleTouchPressEventSM(NumberOfTouchpoints, MeanPosition);
}

   template<State::ScribblingState State>
   void StateClass<State>::HandleTouchMoveEventSM(MAY_BE_UNUSED int NumberOfTouchpoints, MAY_BE_UNUSED QPointF MeanPosition)
   {
      DEBUG_LOG << "Unexpected event HandleTouchMoveEventSM() in default<> State " << StateId() << std::endl;
   }

   template<State::ScribblingState State>
   void StateClass<State>::HandleTouchReleaseEventSM(MAY_BE_UNUSED int NumberOfTouchpoints, MAY_BE_UNUSED QPointF MeanPosition)
   {
      DEBUG_LOG << "Unexpected event HandleTouchRelaseEventSM() in default<> State " << StateId() << std::endl;
   }

   template<State::ScribblingState State>
   void StateClass<State>::HandleOverviewEventSM(MAY_BE_UNUSED bool Enabled)
   {
      DEBUG_LOG << "Unexpected event HandleOverviewEventSM() in default<> State " << StateId() << std::endl;
   }

   template<State::ScribblingState State>
   void StateClass<State>::HandlePasteEventSM(MAY_BE_UNUSED QImage Image)
   {
      DEBUG_LOG << "Unexpected event HandlePasteEventSM() in default<> State " << StateId() << std::endl;
   }

   template<State::ScribblingState State>
   void StateClass<State>::HandleKeyEventSM(MAY_BE_UNUSED PasteEvent Event)
   {
      DEBUG_LOG << "Unexpected event HandleKeyEventSM() in default<> State " << StateId() << std::endl;
   }


   template<State::ScribblingState State>
   void StateClass<State>::timeoutSM()
   {
      DEBUG_LOG << "Unexpected event timeoutSM() in default<> State " << StateId() << std::endl;
   }



/***************  IDLE  *****************/
//! [0]
template<>
void StateClass<State::Idle>::HandlePressEventSM(Qt::MouseButton Button, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp)
{
    DEBUG_LOG << "Button Down: " << Button  << std::endl;
    if (Button == Qt::LeftButton) {
        StateMachine.Interface.SetCursor(CursorManager::DrawingPinter);

        StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionForDrawing);


        StateMachine.Tracker.StartTracking(Position, Timestamp);

        StateMachine.Context.MyDatas.setLastPoint(Position);
        StateMachine.Context.MyDatas.ClearStoredLineSegments();
        StateMachine.Context.MyDatas.setButtonDownPosition(Position);
        if (StateMachine.Context.MyDatas.InScalingMode == false) {
            StateMachine.Context.MyDatas.setGlyphButtonDownPosition(Position);
        }

        // StateMachine.Context.MyDatas.RestartCurrentPaintedObjectBoundingBox(Position);
        StateMachine.Context.MyDatas.setSelectedCurrentPosition(Position);
        if ((Modifiers & Qt::KeyboardModifier::ShiftModifier) != 0) {
           StateMachine.Context.MyDatas.MakeSreenMoveHint();
           //Scrolling = true;
           StateMachine.Context.ScrollingLastPosition = StateMachine.Context.MyDatas.getSelectedCurrentPosition();
           StateMachine.Context.ScrollingOldOrigin = StateMachine.Context.MyDatas.GetOrigin();
           StateMachine.Interface.SetCursor(CursorManager::ScrollingPointer);
           //scribbling = false;
           StateMachine.Interface.UpdateRequest();
           StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionForScrolling);
        } else {
        if(StateMachine.Context.SimpleInterface) {
            StartTimer(StateMachine.Settings.SelectTimeout);
            StateMachine.Interface.SetCursor(CursorManager::TimedPointerForScrolling, StateMachine.Settings.SelectTimeout);
            StateMachine.Context.DownInsideObject = false;
        } else {
            if ((StateMachine.Context.SelectPostitsDirectly == true) &&
                (StateMachine.Context.MyDatas.FindSelectedPostIts(Position))) {
                StateMachine.Context.DownInsideObject = false;
                StartTimer(StateMachine.Settings.DirectSelectTimeout);

            } else {
                StartTimer(StateMachine.Settings.SelectTimeout);


                if (StateMachine.Context.MyDatas.IsInsideLastPaintedObjectBoundingBox(Position)) {
                    if (StateMachine.Context.MyDatas.IsCutoutActive()) {
                        StateMachine.Interface.SetCursor(CursorManager::TimedPointerForCutting, StateMachine.Settings.SelectTimeout);
                    } else  {
                        StateMachine.Interface.SetCursor(CursorManager::TimedPointerForSelecting, StateMachine.Settings.SelectTimeout);
                    }
                    StateMachine.Context.DownInsideObject = true;
                } else {
                    if (StateMachine.Context.MyDatas.FindSelectedPostIts(Position)) {
                        StateMachine.Interface.SetCursor(CursorManager::TimedPointerForSelectingSingle, StateMachine.Settings.SelectTimeout);
                    } else {
                        StateMachine.Interface.SetCursor(CursorManager::TimedPointerForScrolling, StateMachine.Settings.SelectTimeout);
                    }
                    StateMachine.Context.DownInsideObject = false;
                }
            }
        }
        }
    }
}


template<>
void StateClass<State::Idle>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
    StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);
    StateMachine.ShowBigPointer();
    if ((Buttons & Qt::LeftButton)) {
        // Should never get here, as Buttondown in idle changees state
        if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
            return; // ignore small movements (probably use penwidth*2)
        }
    }  else {
        HandleMoveNoLeftButtonEvent(Buttons, Position);
    }

}



template<>
void StateClass<State::Idle>::HandleReleaseEventSM(Qt::MouseButton Button, MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{
    if (Button == Qt::LeftButton) {
        // Do Nothing
    }
}

template<>
void StateClass<State::Idle>::HandleOverviewEventSM(bool Enabled)
{
    if (Enabled) {
        StateMachine.Context.MyDatas.TransferLastDrawnShape();
        StateMachine.SetNewState(&StateMachine.WaitingToSelectRegionFromOverview);
        StateMachine.Context.MyDatas.ToggleShowOverview(true);
    }
}

template<>
void StateClass<State::Idle>::HandlePasteEventSM(QImage Image)
{
    StateMachine.Context.MyDatas.SetImageToPaste(Image);
    StateMachine.SetNewState(&StateMachine.WaitingToPasteClippboardImage);
}

template<>
void StateClass<State::Idle>::timeoutSM()
{
    StateMachine.Context.MyDatas.InScalingMode = false;
}

template<>
void StateClass<State::Idle>::HandleProximityEventSM(MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED bool InProximity)
{
    // Pen has left sensitive area of tablet
    if (InProximity == false) {
       StoppTimer();
       StateMachine.Context.MyDatas.InScalingMode = false;
    }
    DEBUG_LOG << "Unexpected event HandleProximityEventSM() in default<> State " << StateId() << std::endl;
}


/***************  WaitingToLeaveJitterProtectionForDrawing  *****************/
template <>
void StateClass<State::Drawing>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo);
template<>
void StateClass<State::Drawing>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo);

template<>
void StateClass<State::WaitingToLeaveJitterProtectionForDrawing>
    ::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
    StateBaseClass::HandleMoveEventSM(Buttons, Modifiers,  Position, Timestamp, PenInfo);
    StateMachine.ShowBigPointer();

    if ((Buttons & Qt::LeftButton)) {

        if (StateMachine.Context.MyDatas.IsJitter(Position, StateMachine.Context.MyDatas.getButtonDownPosition(), PenInfo.Pressure)) {
            if (PenInfo.Erasing) {
                StateMachine.Context.MyDatas.StoreEraseLineTo(Position, PenInfo.Pressure);
            } else {
                StateMachine.Context.MyDatas.StoredrawLineTo(Position, PenInfo.Pressure);
            }
            return; // ignore small movements (probably use penwidth*2)
        }
        StateMachine.SetNewState(&StateMachine.Drawing);

        StoppTimer();
        StartTimer(StateMachine.Settings.GestureTimeout);
        StateMachine.Interface.RestartAnimatedCursor();
        if (! PenInfo.Erasing) {
           StateMachine.Context.MyDatas.InScalingMode = true;
        }

        StateMachine.Context.MyDatas.DrawLastDrawnShapeAndStartNewShape();
        //LastDrawnObject.fill(BackgroundColor);
        if (PenInfo.Erasing) {
            StateMachine.Context.MyDatas.EraseLineTo(Position, PenInfo.Pressure);
        } else {
            StateMachine.Context.MyDatas.drawLineTo(Position, PenInfo.Pressure);
        }
        //  StateMachine.Context.MyDatas.ExtendBoundingboxAndShape(Position);

        //StateMachine.Drawing.HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

    }  else {
        HandleMoveNoLeftButtonEvent(Buttons, Position);
    }
}


template<>
void StateClass<State::WaitingToLeaveJitterProtectionForDrawing>
    ::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo)
{
    if (Button == Qt::LeftButton) {
        if (! PenInfo.Erasing) {
           StateMachine.Context.MyDatas.InScalingMode = true;
        }
        StateMachine.Context.MyDatas.DrawLastDrawnShapeAndStartNewShape();
        // Add small delta, as lines of length 0 are not drawn, probably
        // add only if Position == Butttondownposition
        StateMachine.Drawing.HandleReleaseEventSM(Button, Position + QPointF(1,1), PenInfo);
    }
}


template<>
void StateClass<State::WaitingToLeaveJitterProtectionForDrawing>
    ::timeoutSM()
{
    if (StateMachine.Context.DownInsideObject) {

        StateMachine.Context.MyDatas.MakeSelectionFromLastDrawnObject(StateMachine.Context.MyDatas.IsCutoutActive());
        //  StateMachine.Context.MyDatas.ClearLastPaintedObjectBoundingBox();
        //  StateMachine.Context.MyDatas.ClearLastDrawnObjectPoints();
        //StateMachine.Context.MyDatas.SetModified();

        //MoveSelected = true;
        //NewDrawingStarted = false;
        // StateMachine.Context.MyDatas.SelectedPoint = StateMachine.Context.MyDatas.getLastPoint();
        StateMachine.Context.MyDatas.SetSelectedOffset();
        //scribbling = false;
        StateMachine.Interface.UpdateRequest();
        //WaitForPostIt = true;
        if (StateMachine.Context.MyDatas.IsCutoutActive()) {
            StateMachine.Interface.SetCursor(CursorManager::TimedPointerForCreatingPostitFromCutting, StateMachine.Settings.PostItTimeout);
        } else {
            StateMachine.Interface.SetCursor(CursorManager::TimedPointerForCreatingPostitFromSelection, StateMachine.Settings.PostItTimeout);
      }
      StartTimer(StateMachine.Settings.PostItTimeout);
      StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithSelectedAreaForMoving);

   } else {

      if ((StateMachine.Context.SimpleInterface==false) && StateMachine.Context.MyDatas.FindSelectedPostIts(StateMachine.Context.MyDatas.getSelectedCurrentPosition())) {
         //scribbling = false;
         //NewDrawingStarted = false;
         StateMachine.Interface.SetCursor(CursorManager::TimedPointerForSelectingMultiple, StateMachine.Settings.PostItTimeout);
         DEBUG_LOG << "Selected postit " << std::endl;
         StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithSelectedPostitForMoving);
         StartTimer(StateMachine.Settings.PostItTimeout);


      } else {
         StateMachine.Context.MyDatas.MakeSreenMoveHint();
         //Scrolling = true;
         StateMachine.Context.ScrollingLastPosition = StateMachine.Context.MyDatas.getSelectedCurrentPosition();
         StateMachine.Context.ScrollingOldOrigin = StateMachine.Context.MyDatas.GetOrigin();
         StateMachine.Interface.SetCursor(CursorManager::ScrollingPointer);
         //scribbling = false;
         StateMachine.Interface.UpdateRequest();
         StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionForScrolling);
      }
   }
}


/***************  Drawing  *****************/

template<>
void StateClass<State::Drawing>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
 //  QPointF ScaledPosition = StateMachine.Context.MyDatas.ScaleMovement(Position);
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers,  Position, Timestamp, PenInfo);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if ((StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure))) {
          // return; // ignore small movements (probably use penwidth*2)
          // Dont restart timer on small movements
       } else {
          StoppTimer();
          StartTimer(StateMachine.Settings.GestureTimeout);
          StateMachine.Interface.SetCursor(CursorManager::GoingToFillTimer, StateMachine.Settings.GestureTimeout);

       }
       // most probably unneeded, as allready flushed in transition...
       // StateMachine.Context.MyDatas.DrawLastDrawnShapeAndStartNewShape();

       //LastDrawnObject.fill(BackgroundColor);
      if (PenInfo.Erasing) {
         StateMachine.Context.MyDatas.EraseLineTo(Position, PenInfo.Pressure);
      } else {
         StateMachine.Context.MyDatas.drawLineTo(Position, PenInfo.Pressure);
      }
   //   StateMachine.Context.MyDatas.ExtendBoundingboxAndShape(Position);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::Drawing>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo)
{
 //  QPointF ScaledPosition = StateMachine.Context.MyDatas.ScaleMovement(Position);
   if (Button == Qt::LeftButton) {
      StoppTimer();
      if (PenInfo.Erasing) {
         StateMachine.Context.MyDatas.EraseLineTo(Position, PenInfo.Pressure);
      } else {
         StateMachine.Context.MyDatas.InScalingMode = true;
         StateMachine.Context.MyDatas.drawLineTo(Position, PenInfo.Pressure);
         StartTimer(StateMachine.Settings.GlyphContinuationTimeout);
      }
      StateMachine.Context.MyDatas.EndShape();
      StateMachine.SetNewState(&StateMachine.Idle);
      StateMachine.Interface.SetCursor(CursorManager::StandardPointer);
   }
}


template<>
void StateClass<State::Drawing>::timeoutSM()
{
   //   QTextStream(stdout) << "<" << GestureTrackerAccumulatedSpeed.x() << "; " << GestureTrackerAccumulatedSpeed.y() << "> <"  << GestureTrackerAccumulatedSquaredSpeed.x() << ";" << GestureTrackerAccumulatedSquaredSpeed.y() << endl;

   //FillPolygon = true;
   StateMachine.Context.FillPolygonStartPosition = StateMachine.Context.MyDatas.getLastPointDrawn();
  // StateMachine.Interface.SetCursor(CursorManager::FillingPointer);
   StateMachine.Interface.UpdateRequest();
   StateMachine.SetNewState(&StateMachine.DrawingFillRequested);
   StartTimer(StateMachine.Settings.CancelFillTimeout);
   StateMachine.Interface.SetCursor(CursorManager::TimedPointerForCancelFilling, StateMachine.Settings.CancelFillTimeout);
}

/***************  DrawingPaused  *****************/

#if 0
template<>
void StateClass<State::DrawingPaused>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       StateMachine.SetNewState(&StateMachine.DrawingFillRequested);
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::DrawingFillRequested>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo);

template<>
void StateClass<State::DrawingPaused>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      StateMachine.DrawingFillRequested.HandleReleaseEventSM(Button, Position, PenInfo);

   }
}

template<>
void StateClass<State::DrawingPaused>::timeoutSM()
{
   //   QTextStream(stdout) << "<" << GestureTrackerAccumulatedSpeed.x() << "; " << GestureTrackerAccumulatedSpeed.y() << "> <"  << GestureTrackerAccumulatedSquaredSpeed.x() << ";" << GestureTrackerAccumulatedSquaredSpeed.y() << endl;

   //FillPolygon = true;
   StateMachine.Context.FillPolygonStartPosition = StateMachine.Context.MyDatas.getLastPointDrawn();
  // StateMachine.Interface.SetCursor(CursorManager::FillingPointer);
   StateMachine.Interface.UpdateRequest();
   StateMachine.SetNewState(&StateMachine.DrawingKillRequested);
   StartTimer(StateMachine.Settings.GestureTimeout);
   StateMachine.Interface.SetCursor(CursorManager::TimedPointerForCancelFilling, StateMachine.Settings.GestureTimeout);
}
#endif

/***************  DrawingFillRequested  *****************/

template<>
void StateClass<State::DrawingFillRequested>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers,  Position, Timestamp, PenInfo);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       if (StateMachine.Context.MyDatas.IsJitter(Position, StateMachine.Context.FillPolygonStartPosition, PenInfo.Pressure)) {
          StateMachine.SetNewState(&StateMachine.Drawing);
          StateMachine.Interface.SetCursor(CursorManager::DrawingPinter);

       }
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Modifiers,  Position, Timestamp, PenInfo);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::DrawingFillRequested>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Drawing.HandleReleaseEventSM(Button, Position, PenInfo);
      StateMachine.Context.MyDatas.FilllastDrawnShape();
      StateMachine.Interface.SetCursor(CursorManager::StandardPointer);
      //FillPolygon = false;

      StateMachine.Interface.UpdateRequest();
   }
}

template<>
void StateClass<State::DrawingFillRequested>::timeoutSM()
{
   //   QTextStream(stdout) << "<" << GestureTrackerAccumulatedSpeed.x() << "; " << GestureTrackerAccumulatedSpeed.y() << "> <"  << GestureTrackerAccumulatedSquaredSpeed.x() << ";" << GestureTrackerAccumulatedSquaredSpeed.y() << endl;

   //FillPolygon = true;
   StateMachine.Context.FillPolygonStartPosition = StateMachine.Context.MyDatas.getLastPointDrawn();
  // StateMachine.Interface.SetCursor(CursorManager::FillingPointer);
   StateMachine.Interface.UpdateRequest();
   StateMachine.SetNewState(&StateMachine.DrawingKillRequested);
   StartTimer(StateMachine.Settings.GoingToKillTimeout);
   StateMachine.Interface.SetCursor(CursorManager::TimedPointerForDeleteCurrentDrawing, StateMachine.Settings.GoingToKillTimeout);
}

/***************  DrawingKillRequested  *****************/

template<>
void StateClass<State::DrawingKillRequested>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers,  Position, Timestamp, PenInfo);
   StateMachine.ShowBigPointer();

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       if (StateMachine.Context.MyDatas.IsJitter(Position, StateMachine.Context.FillPolygonStartPosition, PenInfo.Pressure)) {
          StateMachine.SetNewState(&StateMachine.Drawing);
          StateMachine.Interface.SetCursor(CursorManager::DrawingPinter);

       }
       StateMachine.Drawing.HandleMoveEventSM(Buttons, Modifiers,  Position, Timestamp, PenInfo);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::DrawingKillRequested>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      StoppTimer();
      if (PenInfo.Erasing) {
         StateMachine.Context.MyDatas.EraseLineTo(Position, PenInfo.Pressure);
      } else {
         StateMachine.Context.MyDatas.drawLineTo(Position, PenInfo.Pressure);
      }
      StateMachine.Context.MyDatas.EndShape();
      StateMachine.SetNewState(&StateMachine.Idle);
      StateMachine.Interface.SetCursor(CursorManager::StandardPointer);

      //FillPolygon = false;

      StateMachine.Interface.UpdateRequest();
   }
}

template<>
void StateClass<State::DrawingKillRequested>::timeoutSM()
{
   //   QTextStream(stdout) << "<" << GestureTrackerAccumulatedSpeed.x() << "; " << GestureTrackerAccumulatedSpeed.y() << "> <"  << GestureTrackerAccumulatedSquaredSpeed.x() << ";" << GestureTrackerAccumulatedSquaredSpeed.y() << endl;

   //FillPolygon = true;
   // StateMachine.Interface.SetCursor(CursorManager::FillingPointer);
   StateMachine.Interface.UpdateRequest();
   StateMachine.SetNewState(&StateMachine.Idle);
   StateMachine.Context.MyDatas.CancelShape();
   StateMachine.Interface.SetCursor(CursorManager::StandardPointer);
}


/***************  WaitingToLeaveJitterProtectionWithSelectedAreaForMoving  *****************/

template<>
void StateClass<State::MovingSelection>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo);

template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>
::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers,  Position, Timestamp, PenInfo);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.Interface.SetCursor(CursorManager::MovingCutoutPointer);

       StateMachine.SetNewState(&StateMachine.MovingSelection);
       StateMachine.MovingSelection.HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}


template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>
::HandleReleaseEventSM(Qt::MouseButton Button, MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      StateMachine.SetNewState(&StateMachine.Idle);
      StateMachine.Interface.SetCursor(CursorManager::StandardPointer);
   }
}

template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithSelectedAreaForMoving>
::timeoutSM()
{
   {
      DEBUG_LOG << "Creating postit " << std::endl;

      //WaitForPostIt = false;
      StateMachine.Context.MyDatas.CreeatePostitFromSelection();
      StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithCreatedPostitForMoving);
      StateMachine.Interface.SetCursor(CursorManager::MovingPostitPointer);
      StateMachine.Interface.UpdateRequest();
   }

}

/***************  MovingSelection  *****************/

template<>
void StateClass<State::MovingSelection>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       // QPoint Offset = event->pos() - SelectedPoint;
         StoppTimer();
         StartTimer(StateMachine.Settings.CopyTimeout);
         StateMachine.Interface.SetCursor(CursorManager::TimedPointerForCopying, StateMachine.Settings.CopyTimeout);
         StateMachine.Context.MyDatas.setSelectedCurrentPosition(Position);
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
void StateClass<State::MovingSelection>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      //WaitForPostIt = false;
       if (StateMachine.Tracker.GetCurrentSpeed() > 0.25f) {
           DEBUG_LOG << "LeavingSpeed " << StateMachine.Tracker.GetCurrentSpeed() << std::endl;
           StateMachine.Context.MyDatas.setDiscardSelection(true);
           StateMachine.Interface.UpdateRequest();
       }
       //DEBUG_LOG << "LeavingSpeed " << (LastDistance + CurrentDistance) << " / " << (DeltaTLastDistance + DeltaTCurrentDistance) << " = " << ((LastDistance + CurrentDistance) / (float)(DeltaTLastDistance + DeltaTCurrentDistance)) << std::endl;

     // QPoint Offset =  - SelectedPoint;
      // if (StateMachine.Context.MyDatas.getDiscardSelection() == false) {
          StateMachine.Context.MyDatas.DrawMovedSelection(Position);
     //  }
     // BoundingBoxClass MovedRectangle(LastPaintedObjectBoundingBox);
    //  MovedRectangle.Move(PositionClass(Offset.x(), Offset.y()));
    //  drawrectangle(MovedRectangle);
     // MoveSelected = false;
      // StateMachine.Context.MyDatas.ClearLastDrawingValid();
      //        drawrectangle(BoundingBoxClass(LastPaintedObjectBoundingBox).Move(PositionClass(Offset.x(), Offset.y())));
      StateMachine.SetNewState(&StateMachine.Idle);
      StateMachine.Interface.SetCursor(CursorManager::StandardPointer);
   }
}

template<>
void StateClass<State::MovingSelection>::timeoutSM()
{
   if (StateMachine.Context.MyDatas.getDiscardSelection() == false) {

      // Fast shaking followed by a pause means throw away selection
      if (StateMachine.Tracker.IsFastShaking()) {
         StateMachine.Context.MyDatas.setDiscardSelection(true);
         StateMachine.Interface.UpdateRequest();
      } else {
         QPointF CopyPos(StateMachine.Context.MyDatas.getSelectedCurrentPosition());
         StateMachine.Context.MyDatas.MoveSelectedCurrentPosition ( QPointF(3,3));
         StateMachine.Context.MyDatas.DrawMovedSelection(CopyPos);

      }
    // Just stay in State
    //  StateMachine.SetNewState(&StateMachine.MovingSelectionPaused);
      StateMachine.Interface.SetCursor(CursorManager::MovingCutoutPointer);

   }

}


/***************  MovingSelectionPaused  *****************/
#if 0 // Not required, stay just in movingselection
template<>
void StateClass<State::MovingSelectionPaused>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       StateMachine.SetNewState(&StateMachine.MovingSelection);
       StateMachine.MovingSelection.HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::MovingSelectionPaused>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      StateMachine.MovingSelection.HandleReleaseEventSM(Button, Position, PenInfo);

   }
}
#endif
/***************  WaitingToLeaveJitterProtectionWithCreatedPostitForMoving  *****************/
template<>
void StateClass<State::MovingPostit>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo);

template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.MovingPostit);
       StateMachine.MovingPostit.HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithCreatedPostitForMoving>::HandleReleaseEventSM(Qt::MouseButton Button, MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}


/***************  WaitingToLeaveJitterProtectionWithSelectedPostitForMoving  *****************/
template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       StateMachine.SetNewState(&StateMachine.MovingPostit);
       StateMachine.MovingPostit.HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving>::HandleReleaseEventSM(Qt::MouseButton Button, MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.MyDatas.ClearSelectedPostit();
      StateMachine.Interface.SetCursor(CursorManager::StandardPointer);

      StateMachine.SetNewState(&StateMachine.Idle);

   }
}

template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithSelectedPostitForMoving>
::timeoutSM()
{
   if (StateMachine.Context.MyDatas.FindSelectedPostIts(StateMachine.Context.MyDatas.getSelectedCurrentPosition(), DatabaseClass::SelectMode::All)) {
      //scribbling = false;
      //NewDrawingStarted = false;
      StateMachine.Interface.SetCursor(CursorManager::TimedPointerForDeletePostit, StateMachine.Settings.DeletePostItTimeout);
      DEBUG_LOG << "Selected postit " << std::endl;

      //Just Stay in State
      StateMachine.SetNewState(&StateMachine.WaitingToLeaveJitterProtectionWithSelectedPostitForDeletingOrMoving);
      StateMachine.MyTimer.start(StateMachine.Settings.DeletePostItTimeout);
   }

}

/***************  WaitingToLeaveJitterProtectionWithSelectedPostitForDeletingOrMoving  *****************/
template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithSelectedPostitForDeletingOrMoving>
::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }

       StateMachine.SetNewState(&StateMachine.MovingPostit);
       StateMachine.MovingPostit.HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithSelectedPostitForDeletingOrMoving>::HandleReleaseEventSM(Qt::MouseButton Button, MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.MyDatas.ClearSelectedPostit();
      StateMachine.Interface.SetCursor(CursorManager::StandardPointer);

      StateMachine.SetNewState(&StateMachine.Idle);

   }
}

template<>
void StateClass<State::WaitingToLeaveJitterProtectionWithSelectedPostitForDeletingOrMoving>
::timeoutSM()
{
   if (StateMachine.Context.MyDatas.FindSelectedPostIts(StateMachine.Context.MyDatas.getSelectedCurrentPosition(), DatabaseClass::SelectMode::All)) {
      //scribbling = false;
      //NewDrawingStarted = false;
      StateMachine.Interface.SetCursor(CursorManager::StandardPointer);
      DEBUG_LOG << "Selected postit " << std::endl;

      StateMachine.Context.MyDatas.DeleteSelectedPostits();
      StateMachine.Interface.UpdateRequest();
      StateMachine.SetNewState(&StateMachine.Idle);
      //StateMachine.MyTimer.start(StateMachine.Settings.DeletePostItTimeout);
   }

}


/***************  MovingPostit  *****************/

template<>
void StateClass<State::MovingPostit>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       // ToDo Start Timer to copy postit if position is held for long time
       if (StateMachine.Context.MyDatas.IsAnySelectedPostit()) {
          DEBUG_LOG << "Moving postit " << std::endl;
          StoppTimer();
          StartTimer(StateMachine.Settings.MovePostitPauseTimeout);
          StateMachine.Interface.SetCursor(CursorManager::TimedPointerForCopyPostit, StateMachine.Settings.MovePostitPauseTimeout);
          StateMachine.Context.MyDatas.MoveSelectedPostits(Position);
          StateMachine.Interface.UpdateRequest();
       }

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::MovingPostit>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      if ((StateMachine.Context.MyDatas.IsAnySelectedPostit())) {
         DEBUG_LOG << "Fixing postit " << std::endl;
         StateMachine.Context.MyDatas.FinishMovingSelectedPostits(Position);

         //  MoveSelected = false;
         // StateMachine.Context.MyDatas.ClearLastDrawingValid();
         StateMachine.Context.MyDatas.ClearSelectedPostit();
         StateMachine.Interface.SetCursor(CursorManager::StandardPointer);
         StateMachine.Interface.UpdateRequest();
      }
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::MovingPostitPaused>::timeoutSM();

template<>
void StateClass<State::MovingPostit>::timeoutSM()
{
      StateMachine.MovingPostitPaused.timeoutSM();
    //  StateMachine.Interface.SetCursor(CursorManager::TimedPointerForCopyPostit, StateMachine.Settings.PostitCopyTimeout);
    //  StartTimer(StateMachine.Settings.PostitCopyTimeout);
    //  StateMachine.SetNewState(&StateMachine.MovingPostitPaused);
}

/***************  MovingPostitPaused  *****************/

template<>
void StateClass<State::MovingPostitPaused>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.Interface.SetCursor(CursorManager::MovingPostitPointer);
       StateMachine.SetNewState(&StateMachine.MovingPostit);
       StateMachine.MovingPostit.HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::MovingPostitPaused>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {

      StateMachine.Interface.SetCursor(CursorManager::StandardPointer);

      StateMachine.SetNewState(&StateMachine.MovingPostit);
      StateMachine.MovingPostit.HandleReleaseEventSM(Button, Position, PenInfo);

   }
}

template<>
void StateClass<State::MovingPostitPaused>::timeoutSM()
{
      StateMachine.Interface.SetCursor(CursorManager::TimedPointerForCopyPostit, StateMachine.Settings.PostitCopyTimeout);
      //StateMachine.Interface.setSpeciallCursor();
      StartTimer(StateMachine.Settings.PostitCopyTimeout);
      StateMachine.SetNewState(&StateMachine.MovingPostitPaused);
      StateMachine.Context.MyDatas.DuplicateSelectedPostits();
      StateMachine.Context.MyDatas.MoveSelectedPostits(StateMachine.Context.MyDatas.getLastPointerPosition() + QPointF(4,4));
      StateMachine.Context.MyDatas.setLastPointerPosition(StateMachine.Context.MyDatas.getLastPointerPosition() + QPointF(4,4));
      StateMachine.Interface.UpdateRequest();
}


/***************  WaitingToLeaveJitterProtectionForScrolling  *****************/
template<>
void StateClass<State::ScrollingDrawingArea>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo);

template<>
void StateClass<State::WaitingToLeaveJitterProtectionForScrolling>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.SetNewState(&StateMachine.ScrollingDrawingArea);
       StateMachine.ScrollingDrawingArea.HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);


   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScrollingDrawingArea>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo);

template<>
void StateClass<State::WaitingToLeaveJitterProtectionForScrolling>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      StateMachine.ScrollingDrawingArea.HandleReleaseEventSM(Button, Position, PenInfo);

   }
}


/***************  ScrollingDrawingArea  *****************/

template<>
void StateClass<State::ScrollingDrawingArea>::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
   StateBaseClass::HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);

   if ((Buttons & Qt::LeftButton)) {
       if (StateMachine.Context.MyDatas.IsSelectionJitter(Position,  PenInfo.Pressure)) {
           return; // ignore small movements (probably use penwidth*2)
       }
       StateMachine.Context.MyDatas.TransferLastDrawnShape();
       if ((Modifiers & Qt::KeyboardModifier::ShiftModifier) == 0) {
          StateMachine.Context.MyDatas.ScrollImages(Position - StateMachine.Context.ScrollingLastPosition);
       } else {
          StateMachine.Context.MyDatas.ScrollImages((Position - StateMachine.Context.ScrollingLastPosition) * 4);
       }


      StateMachine.Context.ScrollingLastPosition = Position;
      StateMachine.Interface.UpdateRequest();

   }  else {
      HandleMoveNoLeftButtonEvent(Buttons, Position);
   }

}

template<>
void StateClass<State::ScrollingDrawingArea>::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, MAY_BE_UNUSED const PenInfoClass &PenInfo)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.MyDatas.ScrollImages(Position - StateMachine.Context.ScrollingLastPosition);

      StateMachine.Context.MyDatas.resizeScrolledImage();
      StateMachine.Interface.UpdateRequest();

      StateMachine.SetNewState(&StateMachine.Idle);

   }
}






template<>
void StateClass<State::TouchScrollingDrawingArea>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   DEBUG_LOG << "TM(" << MeanPosition.x() <<":" << MeanPosition.y() << ")";

         StateMachine.Context.MyDatas.TransferLastDrawnShape();
   StateMachine.Context.MyDatas.ScrollImages(MeanPosition- StateMachine.Context.ScrollingLastPosition);

         StateMachine.Context.ScrollingLastPosition = MeanPosition;
         StateMachine.Interface.UpdateRequest();
   }

}
template<>
void StateClass<State::WaitingForTouchScrolling>::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints == 2) {
   DEBUG_LOG << "TM(" << MeanPosition.x() <<":" << MeanPosition.y() << ")";

   StateMachine.SetNewState(&StateMachine.TouchScrollingDrawingArea);
   StateMachine.TouchScrollingDrawingArea.HandleTouchMoveEventSM(NumberOfTouchpoints, MeanPosition);
   }
}


template<>
void StateClass<State::TouchScrollingDrawingArea>::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints >= 2) {
   StateMachine.Context.MyDatas.ScrollImages(MeanPosition - StateMachine.Context.ScrollingLastPosition);

      StateMachine.Context.MyDatas.resizeScrolledImage();
      StateMachine.Interface.UpdateRequest();

      StateMachine.SetNewState(&StateMachine.Idle);
   }
}

template<>
void StateClass<State::TouchScrollingDrawingArea>::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints > 2) {

      HandleTouchReleaseEventSM(NumberOfTouchpoints, MeanPosition);
   }
}

template<>
void StateClass<State::WaitingForTouchScrolling>::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints >= 2) {

      StateMachine.TouchScrollingDrawingArea.HandleTouchReleaseEventSM(NumberOfTouchpoints, MeanPosition);
   }
}


template<>
void StateClass<State::WaitingForTouchScrolling>::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   if(NumberOfTouchpoints >= 2) {

      StateMachine.TouchScrollingDrawingArea.HandleTouchReleaseEventSM(NumberOfTouchpoints, MeanPosition);
   }
}
/*********** Overviev *******************************/

template<>
void StateClass<State::WaitingToSelectRegionFromOverview>::HandlePressEventSM(Qt::MouseButton Button, Qt::KeyboardModifiers Modifiers, QPointF Position, MAY_BE_UNUSED Milliseconds Timestamp)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.MyDatas.ScrollImages(-(StateMachine.Context.MyDatas.TranslateCoordinateOffsetFromOverview(Position)-StateMachine.Context.MyDatas.GetOrigin()));
      StateMachine.SetNewState(&StateMachine.Idle);
      StateMachine.Context.MyDatas.ToggleShowOverview(false);
   }
}

template<>
void StateClass<State::WaitingToSelectRegionFromOverview>::HandleOverviewEventSM(bool Enabled)
{
   if (!Enabled) {
      StateMachine.SetNewState(&StateMachine.Idle);
      StateMachine.Context.MyDatas.ToggleShowOverview(false);
   }
}

template<>
void StateClass<State::WaitingToSelectRegionFromOverview>::HandleTouchMoveEventSM(MAY_BE_UNUSED int NumberOfTouchpoints, MAY_BE_UNUSED QPointF MeanPosition)
{
   // Ignore in overview
}

template<>
void StateClass<State::WaitingToSelectRegionFromOverview>::HandleTouchPressEventSM(MAY_BE_UNUSED int NumberOfTouchpoints, MAY_BE_UNUSED QPointF MeanPosition)
{
   // Ignore in overview
}

/*********** Paste Clipboard *******************************/

template<>
void StateClass<State::WaitingToPasteClippboardImage>::HandleKeyEventSM(PasteEvent Event)
{
   switch(Event) {
      case DatabaseClass::PasteTopLayer:
      case DatabaseClass::PasteBottomLayer:
      case DatabaseClass::PasteDrawing:
         StateMachine.Context.MyDatas.DoPasteImage(Event);
         StateMachine.SetNewState(&StateMachine.Idle);
         break;
      case DatabaseClass::CancelPasting:
         StateMachine.Context.MyDatas.CancelPasteImage();
         StateMachine.SetNewState(&StateMachine.Idle);
         break;
      case DatabaseClass::MakeBigger:
         StateMachine.Context.MyDatas.ScaleImageToPaste(1.1);
         break;
      case DatabaseClass::MakeSmaller:
         StateMachine.Context.MyDatas.ScaleImageToPaste(0.9);
         break;

      case DatabaseClass::MakeOriginalSize:
         StateMachine.Context.MyDatas.ScaleImageToPaste(-1.0);
         break;
   }
}



template<>
void StateClass<State::WaitingToPasteClippboardImage>::HandlePressEventSM(Qt::MouseButton Button, Qt::KeyboardModifiers Modifiers, MAY_BE_UNUSED QPointF Position, MAY_BE_UNUSED Milliseconds Timestamp)
{
   if (Button == Qt::LeftButton) {
      StateMachine.Context.MyDatas.DoPasteImage(DatabaseClass::PasteDrawing);
      StateMachine.SetNewState(&StateMachine.Idle);
   }
}


template<>
void StateClass<State::WaitingToPasteClippboardImage>::HandleTouchMoveEventSM(MAY_BE_UNUSED int NumberOfTouchpoints, MAY_BE_UNUSED QPointF MeanPosition)
{
   // Ignore in overview
}

template<>
void StateClass<State::WaitingToPasteClippboardImage>::HandleTouchPressEventSM(MAY_BE_UNUSED int NumberOfTouchpoints, MAY_BE_UNUSED QPointF MeanPosition)
{
   // Ignore in overview
}

/*********** End of Statemachine *******************************/


ControllingStateMachine::PointerType ControllingStateMachine::PointerTypeToShow()
{
   if (Context.ShowPointer) {
      if (Context.Showeraser) {
         if (Context.SpongeAsEraser) {
            return WIPER;
         } else {
            return ERASER;
         }
      } else {
         return DRAWER;
      }
   }
   return NONE;
}
void ControllingStateMachine::ShowBigPointer()
{
   if (Settings.PointerHoldon >= 0) {
   Context.ShowPointer = true;
   PointerTimer.start(static_cast<int>(Settings.PointerHoldon));
   Interface.UpdateRequest();
   }
}

void ControllingStateMachine::PointerTimeout()
{
   Context.ShowPointer = false;
   Interface.UpdateRequest();
}

void ControllingStateMachine::SetNewState(StateBaseClass *NewState)
{
   CurrentState = NewState;
}

void ControllingStateMachine::HandleProximityEventSM(QPointF Position, bool InProximity)
{
    CurrentState->HandleProximityEventSM(Position, InProximity);
}


void ControllingStateMachine::HandlePressEventSM(Qt::MouseButton Button, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp)
{
    CurrentState->HandlePressEventSM(Button, Modifiers, Position, Timestamp);
}

void ControllingStateMachine::HandleMoveEventSM(Qt::MouseButtons Buttons, Qt::KeyboardModifiers Modifiers, QPointF Position, Milliseconds Timestamp, const PenInfoClass &PenInfo)
{
    CurrentState->HandleMoveEventSM(Buttons, Modifiers, Position, Timestamp, PenInfo);
}

void ControllingStateMachine::HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, const PenInfoClass &PenInfo)
{
   DEBUG_LOG << "Button Up: " << Button  << std::endl;
   CurrentState->HandleReleaseEventSM(Button, Position, PenInfo);
}

void ControllingStateMachine::HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   CurrentState->HandleTouchPressEventSM(NumberOfTouchpoints, MeanPosition);
}

void ControllingStateMachine::HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   CurrentState->HandleTouchMoveEventSM(NumberOfTouchpoints, MeanPosition);
}

void ControllingStateMachine::HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition)
{
   CurrentState->HandleTouchReleaseEventSM(NumberOfTouchpoints, MeanPosition);
}

void ControllingStateMachine::HandleOverviewEventSM(bool Enabled)
{
   CurrentState->HandleOverviewEventSM(Enabled);
}

void ControllingStateMachine::HandlePasteEventSM(QImage ImageToPaste)
{
   CurrentState->HandlePasteEventSM(ImageToPaste);
}

void ControllingStateMachine::HandleKeyEventSM(DatabaseClass::PasteEvent Event)
{
   CurrentState->HandleKeyEventSM(Event);
}

void ControllingStateMachine::Timeout()
{
   CurrentState->timeoutSM();

}

void ControllingStateMachine::HandleGesture()
{
   emit(GestureDetected());
   //std::cout << "!! gesture !!=";
}

#define InitStateObject(s) s(*this, ##s)


ControllingStateMachine::ControllingStateMachine(DatabaseClass &Database, GuiInterface &NewInterface, class SettingClass &MySettings)
   :   Tracker(MySettings),
       Context(Database),
       Settings(MySettings),
       Interface(NewInterface),
       Idle(*this),
       WaitingToLeaveJitterProtectionForDrawing(*this),
       WaitingToLeaveJitterProtectionWithSelectedAreaForMoving(*this),
       WaitingToLeaveJitterProtectionForScrolling(*this),
       WaitingToLeaveJitterProtectionWithCreatedPostitForMoving(*this),
       WaitingToLeaveJitterProtectionWithSelectedPostitForMoving(*this),
       WaitingToLeaveJitterProtectionWithSelectedPostitForDeletingOrMoving(*this),
       Drawing(*this),
     //  DrawingPaused(*this),
       DrawingFillRequested(*this),
       DrawingKillRequested(*this),
       MovingSelection(*this),
       MovingSelectionPaused(*this),
       MovingPostit(*this),
       MovingPostitPaused(*this),
       ScrollingDrawingArea(*this),
       WaitingForTouchScrolling(*this),
       TouchScrollingDrawingArea(*this),
       WaitingToSelectRegionFromOverview(*this),
       WaitingToPasteClippboardImage(*this)
{
    CurrentState = &Idle;
    Context.ShowPointer = false;
    Context.SpongeAsEraser = false;

    //Context.ShowOverview = false;

  //  Context.LastDrawingValid = false;
    //Context.EraseLastDrawnObject = false;
 //   Context.Frozen = false;
 //   Context.myPenWidth = 2;
    //Context.SelectedPenWidth = StateMachine.Context.myPenWidth;

    QObject::connect(&MyTimer, &QTimer::timeout, this, &ControllingStateMachine::Timeout);
    MyTimer.setSingleShot(true);

    QObject::connect(&PointerTimer, &QTimer::timeout, this, &ControllingStateMachine::PointerTimeout);
    PointerTimer.setSingleShot(true);

    connect(&Tracker, &GestureTrackerClass::GestureDetected, this, &ControllingStateMachine::HandleGesture);

    //Context.SelectedPostit.clear();
    Context.SelectPostitsDirectly = false;
    Context.SimpleInterface = true;
    //Context.ShowPostitsFrame = false;



    //Context.RecentlyPastedObjectValid = false;
    //Context.MarkerActive = false;
   // QImage RecentlyMovedObject;
   // BoundingBoxClass RecentlyMovedObjectBoundingBox;


}

void ControllingStateMachine::timeoutSM()
{
   CurrentState->timeoutSM();
}

//! [0]
//! [0]

//! [1]

//! [2]

//! [3]


#endif

