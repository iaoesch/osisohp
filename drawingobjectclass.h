#ifndef DRAWINGOBJECTCLASS_H
#define DRAWINGOBJECTCLASS_H

#include "box.hpp"
#include <QImage>
#include "Settings.hpp"


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
   SettingClass &Settings;

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
   QColor penColor() const { return myPenColor; }
   int penWidth() const { return SelectedPenWidth; }

   void UseSpongeAsEraser(bool UseSponge) {if (UseSponge) {myEraserWidth = Settings.SpongeSize;} else {myEraserWidth = Settings.EraserSize;}}
   void RestorePenWidth() {myPenWidth = SelectedPenWidth;}

   void setMarkerActive(bool newMarkerActive)
   {
      MarkerActive = newMarkerActive;
      if (MarkerActive) {
         ExtendPenWidthForMarker();
      } else {
         RestorePenWidth();
      }
   }

   bool getLastDrawingValid() const
   {
      return LastDrawingValid;
   }
   void setLastDrawingValid(bool newLastDrawingValid)
   {
      LastDrawingValid = newLastDrawingValid;
   }
   int getMyPenWidth() const
   {
      return myPenWidth;
   }
   void setLastPoint(QPointF newLastPoint)
   {
      lastPointDrawn = newLastPoint;
   }


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
   QImage &Image() {return LastDrawnObject;}
private:
   double CalculatePenWidthLinear(double Pressure, int BaseWidth);
   double CalculatePenWidthQuadratic(double Pressure, int BaseWidth);
   void ExtendPenWidthForMarker() {myPenWidth = SelectedPenWidth * 5 + 2;}


};

#endif // DRAWINGOBJECTCLASS_H
