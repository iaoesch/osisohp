#ifndef DRAWINGOBJECTCLASS_H
#define DRAWINGOBJECTCLASS_H

#include "box.hpp"
#include <QImage>


class DrawingObjectClass
{
   int myPenWidth;
   int myEraserWidth;
   int SelectedPenWidth;
   bool EraseLastDrawnObject;
   bool LastDrawingValid;
   QPointF lastPointDrawn;
   QPolygonF LastDrawnObjectPoints;
   BoundingBoxClass CurrentPaintedObjectBoundingBox;




   QColor myPenColor;
   QImage LastDrawnObject;


public:
   DrawingObjectClass();
   void RestartCurrentPaintedObjectBoundingBox(QPointF const &StartPoint)
   {
      CurrentPaintedObjectBoundingBox.Clear();
      CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(StartPoint.x(), StartPoint.y()));
   }

};

#endif // DRAWINGOBJECTCLASS_H
