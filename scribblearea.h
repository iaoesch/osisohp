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

class ControllingStateMachine;

class StateBaseClass {
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
   virtual ~StateBaseClass();
};

template<int State>
class IdleStateClass : public StateBaseClass {
public:
   IdleStateClass(ControllingStateMachine &sm) : StateBaseClass(sm) {}
   virtual void HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp) override;
   virtual void HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure) override;
   virtual void HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure) override;
   virtual void HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition) override;
   virtual void HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition) override;
   virtual void HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition) override;
   virtual void timeoutSM() override;
   virtual ~IdleStateClass() override;
};


class ControllingStateMachine {
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

public:
   void HandlePressEventSM(Qt::MouseButton Button, QPointF Position, ulong Timestamp);
   void HandleMoveEventSM(Qt::MouseButtons Buttons, QPointF Position, ulong Timestamp, bool Erasing, double Pressure);
   void HandleReleaseEventSM(Qt::MouseButton Button, QPointF Position, bool Erasing, double Pressure);
   void HandleTouchPressEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   void HandleTouchMoveEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
   void HandleTouchReleaseEventSM(int NumberOfTouchpoints, QPointF MeanPosition);
public slots:
    void timeoutSM();

};

class ScribbleArea : public QWidget
{
    Q_OBJECT

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


    void drawLineTo(const QPointF &endPoint, double Pressure);
    void drawrectangle(const BoundingBoxClass &Region);
    void DrawMovedSelection(const QPointF Offset);

    void DrawLastDrawnPicture();
    void resizeImage(QImage *image, const QSize &newSize, QPoint Offset = {0,0});
    void resizeScrolledImage();

    Settings Settings;

    struct PostIt {
       QImage Image;
       QPointF Position;
       BoundingBoxClass Box;
       PostIt(const QImage &NewImage, const QPointF &Pos, BoundingBoxClass NewBox) : Image(NewImage), Position(Pos), Box(NewBox) {}
    };

    std::list<PostIt> PostIts;
    struct PostItDescriptor{
       std::list<PostIt>::iterator postit;
       QPointF StartPosition;
    } ;
    std::list<PostItDescriptor> SelectedPostit;
    QPoint StartPositionSelectedPostIt;

    bool modified;
    bool SelectPostitsDirectly;
    bool ShowPostitsFrame;
    bool LastDrawingValid;
    bool DownInsideObject;
    bool DiscardSelection;
    int myPenWidth;
    int SelectedPenWidth;
    QColor myPenColor;
    QImage image;
    QImage LastDrawnObject;
    QImage PointerShape;
    QImage EraserShape;
    QImage SpongeShape;
    bool ShowOverview;
    bool Showeraser;
    bool MarkerActive;
    bool EraseLastDrawnObject;

    class ImageDescriptor {
       bool Visible;
       std::unique_ptr<QImage> Image;

       public:
       ImageDescriptor(std::unique_ptr<QImage> TheImage) : Visible(true), Image(std::move(TheImage)) {}
       ImageDescriptor(std::unique_ptr<QImage> TheImage, bool v) : Visible(v), Image(std::move(TheImage)) {}
       QImage &operator * () {return *Image;}
       QImage *operator -> () {return Image.operator ->();}
       bool IsVisible() {return Visible;}
       void SetVisible(bool v) {Visible = v;}

    };

    //std::vector<std::unique_ptr<QImage>> BackgroundImages;
    std::vector<ImageDescriptor> BackgroundImages;

    QColor TransparentColor;
    QColor BackGroundColor;
    QColor DefaultBackGroundColor;
    QColor PostItBackgroundColor;

    QPolygonF LastDrawnObjectPoints;
    QImage SelectedImagePart;
    QImage HintSelectedImagePart;
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

    GestureTrackerClass Tracker;

    QPointF Origin;
    bool Frozen;
    QPointF BackgroundImagesOrigin;
/*
    int CopyTimeout;
    int GestureTimeout;
    int PostItTimeout;
    int SelectTimeout;*/


    QPointF LastTablettMovePosition;



    QTimer MyTimer;
    QTimer PointerTimer;

    bool RecentlyPastedObjectValid;
    QPointF RecentlyPastedObjectPosition;
    QImage RecentlyPastedObject;
    BoundingBoxClass RecentlyPastedObjectBoundingBox;

    BoundingBoxClass LastPaintedObjectBoundingBox;
    BoundingBoxClass CurrentPaintedObjectBoundingBox;
    bool PostItSelected(QPointF Position);
    void EraseLineTo(const QPointF &endPoint, double Pressure);
    bool IsInsideAnyPostIt(QPointF Position);
    bool TouchEvent(QTouchEvent *event);
    void GetOffsetAndAdjustOrigin(QImage &Image, QPointF &Origin, QPoint &Offset, QSize &Size);
    void CompleteImage();
    void PaintVisibleDrawing(QPainter &painter, const QRect &dirtyRect, const QPointF &Origin, const QPointF &BackgroundImagesOrigin);
    int CollapseAllVisibleLayersToTop();
    void UpdateGUI(int NumberOfLayers);
};
//! [0]

#endif
