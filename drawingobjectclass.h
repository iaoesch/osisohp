#ifndef DRAWINGOBJECTCLASS_H
#define DRAWINGOBJECTCLASS_H

#include "box.hpp"
#include <QImage>


class DrawingObjectClass
{
public:
   class ShapeClass
   {
   public:
      QPolygonF LastDrawnObjectPoints;
      BoundingBoxClass CurrentPaintedObjectBoundingBox;
   };

private:


   int myPenWidth;
   int myEraserWidth;
   int SelectedPenWidth;
   bool EraseLastDrawnObject;
   bool LastDrawingValid;
   bool MarkerActive;

   QColor &TransparentColor;

   QPointF lastPointDrawn;

   ShapeClass CurrentShape;

   QColor myPenColor;
   QImage LastDrawnObject;


public:
   DrawingObjectClass();
   void RestartCurrentPaintedObjectBoundingBox(QPointF const &StartPoint)
   {
      CurrentShape.CurrentPaintedObjectBoundingBox.Clear();
      CurrentShape.CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(StartPoint.x(), StartPoint.y()));
   }

   QRect drawLineTo(const QPointF &endPoint, double Pressure);
   QRect EraseLineTo(const QPointF &endPoint, double Pressure);
   void DrawLastDrawnPicture(QPainter &painter, const QPointF &Offset);
   void setPenColor(const QColor &newColor);
   void setPenWidth(int newWidth);
   void Clear();
   void DrawIfMarking(QPainter &painter, const QRect &dirtyRect);
   void DrawIfErasing(QPainter &painter, const QImage &image, const QPointF &Offset, const QRect &dirtyRect);
   void DrawNormal(QPainter &painter, const QRect &dirtyRect);
   void FillLastDrawnShape(QPainter &&painter2, const QPointF &Offset);
   bool CompleteImage(QPainter &painter, const QPointF &Offset);
   bool FlushLastDrawnPicture(QPainter &painter, const QPointF &Offset);
   void ExtendBoundingboxAndShape(QPointF Position);
   ShapeClass UpdateBoundingboxesForFinishedShape(QPointF Position);
   void CutOut(QPainter &painter2, QPointF Offset);
private:
   double CalculatePenWidthLinear(double Pressure, int BaseWidth);
   double CalculatePenWidthQuadratic(double Pressure, int BaseWidth);
   void ExtendPenWidthForMarker() {myPenWidth = SelectedPenWidth * 5 + 2;}


};

#endif // DRAWINGOBJECTCLASS_H
