#ifndef PASTINGOBJECTCLASS_H
#define PASTINGOBJECTCLASS_H

#include "box.hpp"

#include <QImage>

class DatabaseClass;

class PastingObjectClass {

    QImage ImageToPaste;
    double ScalingFactorOfImageToPaste;
    enum   PasteImage{None, Drawing, TopLayer, BottomLayer} PasteStatus;

    bool RecentlyPastedObjectValid;
    QPointF RecentlyPastedObjectPosition;
    QImage RecentlyPastedObject;
    BoundingBoxClass RecentlyPastedObjectBoundingBox;
public:
    PastingObjectClass();


    void Draw(QPainter &painter, QPointF Offset);

    void DrawRecentlyPasted(QPainter &painter);

    void SetImageToPaste(QImage Image, QSize ParentSize);

    auto PrepareToPasteImage(QImage &image, DatabaseClass &UglyWorkaroundForRefactoring, QPointF Origin);

    QImage CreateImage(QImage &image, DatabaseClass &UglyWorkaroundForRefactoring, QPointF Origin);

    void DrawIntoImage(QImage &image, DatabaseClass &UglyWorkaroundForRefactoring, QPointF Origin);
    void Cancel();
    void Scale(double ScalingFactor);
};



#endif // PASTINGOBJECTCLASS_H
