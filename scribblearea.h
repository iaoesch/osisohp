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

#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QTimer>
#include <QPolygon>
#include <QTouchEvent>
#include <list>

#include "box.hpp"
#include "Settings.hpp"

#include "gesturetracker.hpp"

//! [0]
//!



#include "scribbleareaStateMachine.h"

class ScribbleArea : public QWidget
{
    Q_OBJECT

    friend class GuiInterface;

public:
    ScribbleArea(QWidget *parent = 0);

    bool ImportImage(const QString &fileName);
    bool LoadImage(const QString &fileName);
    QString GetFileName();
    bool ExportImage(const QString &fileName, const char *fileFormat);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);
    void setDirectSelect(bool Mode) {SelectPostitsDirectly = Mode;}
    void setShowPostitsFrame(bool Mode) {ShowPostitsFrame = Mode; update();}

    QColor GetBackGroundColor() const { return BackGroundColor; }
    void setBackGroundColor(const QColor &newColor) {BackGroundColor = newColor; update();}
    void setPostItBackgroundColor(const QColor &newColor) {PostItBackgroundColor = newColor; update();}

    void PasteImage(QImage ImageToPaste);

    bool isModified() const { return modified; }
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return SelectedPenWidth; }

    void Freeze(bool Mode) {Frozen = Mode;}
    void ToggleShowOverview(bool Mode) {ShowOverview = Mode; update();}
    int MoveImageToBackgroundLayer();
    int MoveTopBackgroundLayerToImage();
    int CollapseBackgroundLayers();

    bool SaveImage(const QString &fileName);
public slots:
    void clearImage();
    void print();
    void HandleToolAction(QAction *action);
    void HandleLayerVisibilityAction(QAction *action);

    void timeoutSM();

    void CopyImageToClipboard();
signals:
    void NumberOfLayerChanged(int NumberOfLayers);
    void SetVisibilityIndicatorOfLayer(int Layer, bool Visibility);

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void tabletEvent(QTabletEvent * event) Q_DECL_OVERRIDE;
    bool event(QEvent *event) Q_DECL_OVERRIDE;

private slots:
    void PointerTimeout();
private:



    Settings Settings;

    QPoint StartPositionSelectedPostIt;

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
    enum ScribblingState State;
    bool SelectPostitsDirectly;
    bool ShowPostitsFrame;
    bool DownInsideObject;
    bool ShowOverview;
    bool Showeraser;

    QPointF ButtonDownPosition;
    QPointF SelectedPoint;
    QPointF ScrollingLastPosition;
    QPointF ScrollingOldOrigin;
    QPointF FillPolygonStartPosition;

    QImage PointerShape;
    QImage EraserShape;
    QImage SpongeShape;

    QPointF LastPointerPosition;
    bool    ShowPointer;

    GestureTrackerClass Tracker;

 /*
    int CopyTimeout;
    int GestureTimeout;
    int PostItTimeout;
    int SelectTimeout;*/


    QPointF LastTablettMovePosition;



    QTimer MyTimer;
    QTimer PointerTimer;

     void HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp);
    void HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure);
    void HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure);
    void HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
    void HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
    void HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
    bool PostItSelected(QPointF Position);
    bool IsInsideAnyPostIt(QPointF Position);
    bool TouchEvent(QTouchEvent *event);
    void CompleteImage();
    void PaintVisibleDrawing(QPainter &painter, const QRect &dirtyRect, const QPointF &Origin, const QPointF &BackgroundImagesOrigin);
    int CollapseAllVisibleLayersToTop();
    void UpdateGUI(int NumberOfLayers);
    void FilllastDrawnShape();
    void MakeSreenMoveHint();
    void MoveSelectedPostits(QPointF Position);
    void FinishMovingSelectedPostits(QPointF Position);
};
//! [0]

#endif
