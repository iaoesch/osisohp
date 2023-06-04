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

#include "databaseclass.h"

class ScribbleArea : public QWidget
{
    Q_OBJECT

    friend class GuiInterface;
    DatabaseClass MyDatas;

public:
    ScribbleArea(QWidget *parent = 0);

    bool ImportImage(const QString &fileName);
    bool LoadImage(const QString &fileName);
    QString GetFileName();
    bool ExportImage(const QString &fileName, const char *fileFormat);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);
    void setDirectSelect(bool Mode) {SelectPostitsDirectly = Mode;}
    void setShowPostitsFrame(bool Mode) {MyDatas.setShowPostitsFrame(Mode); update();}

    QColor GetBackGroundColor() const { return MyDatas.GetBackGroundColor(); }
    void setBackGroundColor(const QColor &newColor) {MyDatas.setBackGroundColor(newColor);}
    void setPostItBackgroundColor(const QColor &newColor) {MyDatas.setPostItBackgroundColor(newColor);}

    void PasteImage(QImage ImageToPaste);

    bool isModified() const { return MyDatas.isModified();}
    QColor penColor() const { return MyDatas.penColor();}
    int penWidth() const { return MyDatas.penWidth();}

    void Freeze(bool Mode) {MyDatas.Freeze(Mode);}
    void ToggleShowOverview(bool Mode) {ShowOverview = Mode; update();}
    void MoveImageToBackgroundLayer() {MyDatas.MoveImageToBackgroundLayer();}
    void MoveTopBackgroundLayerToImage() {MyDatas.MoveTopBackgroundLayerToImage();}
    void CollapseBackgroundLayers() {MyDatas.CollapseBackgroundLayers();}
    void CollapseAllVisibleLayersToTop() {MyDatas.CollapseAllVisibleLayersToTop();}

    bool SaveImage(const QString &fileName);
public slots:
    void clearImage()  {MyDatas.clearImage();}
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
    bool DownInsideObject;
    bool ShowOverview;
    bool Showeraser;

    //QPointF SelectedPoint;
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
    bool TouchEvent(QTouchEvent *event);

public:
    void UpdateGUI(const std::vector<bool> &Visibilities);
    bool IsInSelectingState() {return ((State == MovingSelection)||(State == WaitingToLeaveJitterProtectionWithSelectedAreaForMoving)||(State == MovingSelectionPaused));}

};
//! [0]

#endif
