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
#include <list>

#include "box.hpp"

//! [0]
class ScribbleArea : public QWidget
{
    Q_OBJECT

public:
    ScribbleArea(QWidget *parent = 0);

    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);

    bool isModified() const { return modified; }
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return SelectedPenWidth; }

public slots:
    void clearImage();
    void print();
    void timeout();
    void HandleToolAction(QAction *action);

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void tabletEvent(QTabletEvent * event) Q_DECL_OVERRIDE;

private:


    void drawLineTo(const QPoint &endPoint, double Pressure);
    void drawrectangle(const BoundingBoxClass &Region);
    void DrawMovedSelection(const QPoint Offset);

    void DrawLastDrawnPicture();
    void resizeImage(QImage *image, const QSize &newSize);
    void resizeScrolledImage();

    struct PostIt {
       QImage Image;
       QPoint Position;
       BoundingBoxClass Box;
       PostIt(const QImage &NewImage, const QPoint &Pos, BoundingBoxClass NewBox) : Image(NewImage), Position(Pos), Box(NewBox) {}
    };

    std::list<PostIt> PostIts;
    PostIt *SelectedPostit;
    QPoint StartPositionSelectedPostIt;

    bool modified;
    bool scribbling;
    bool ScribblingStarted;
    bool FillPolygon;
    bool MoveSelected;
    bool NewDrawingStarted;
    bool LastDrawingValid;
    bool DownInsideObject;
    bool WaitForPostIt;
    bool Scrolling;
    bool DiscardSelection;
    int myPenWidth;
    int SelectedPenWidth;
    QColor myPenColor;
    QImage image;
    QImage LastDrawnObject;
    bool EraseLastDrawnObject;

    QColor TransparentColor;
    QColor BackGroundColor;


    QPolygon LastDrawnObjectPoints;
    QImage SelectedImagePart;
    QImage HintSelectedImagePart;
    QPoint lastPoint;
    QPoint ScribblingStartPosition;
    QPoint SelectedPoint;
    QPoint SelectedOffset;
    QPoint SelectedCurrentPosition;
    QPoint ScrollingLastPosition;
    QPoint ScrollingOldOrigin;
    QPoint FillPolygonStartPosition;

    QPoint GestureTrackeStartPosition;
    ulong  GestureTrackerStartPositionTimeStamp;
    QPoint GestureTrackerLastPosition;
    ulong  GestureTrackerLastPositionTimeStamp;
    QPointF GestureTrackerAccumulatedSpeed;
    QPointF GestureTrackerAccumulatedSquaredSpeed;

    QPoint Origin;

    int CopyTimeout;
    int GestureTimeout;
    int PostItTimeout;
    int SelectTimeout;

    ulong CurrentDistance;
    ulong LastDistance;
    ulong DeltaTLastDistance;
    ulong DeltaTCurrentDistance;

    QPoint LastTablettMovePosition;



    QTimer MyTimer;

    bool RecentlyPastedObjectValid;
    QPoint RecentlyPastedObjectPosition;
    QImage RecentlyPastedObject;
    BoundingBoxClass RecentlyPastedObjectBoundingBox;

    BoundingBoxClass LastPaintedObjectBoundingBox;
    BoundingBoxClass CurrentPaintedObjectBoundingBox;
    void HandleReleaseEvent(Qt::MouseButton Button, QPoint Position, bool Erasing, double Pressure);
    void HandleMoveEvent(Qt::MouseButtons Buttons, QPoint Position, ulong Timestamp, bool Erasing, double Pressure);
    void HandlePressEvent(Qt::MouseButton Button, QPoint Position, ulong Timestamp);
    bool PostItSelected(QPoint Position);
    void EraseLineTo(const QPoint &endPoint, double Pressure);
};
//! [0]

#endif
