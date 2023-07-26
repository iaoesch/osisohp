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

   public:
      ShapeClass() = default;
      ShapeClass(const ShapeClass &src) : LastDrawnObjectPoints(src.LastDrawnObjectPoints) ,CurrentPaintedObjectBoundingBox(src.CurrentPaintedObjectBoundingBox) {}
      ShapeClass &operator = (const ShapeClass &src) = default; //: LastDrawnObjectPoints(src.LastDrawnObjectPoints) ,CurrentPaintedObjectBoundingBox(src.CurrentPaintedObjectBoundingBox) {}

      const BoundingBoxClass &Box() { return CurrentPaintedObjectBoundingBox;}
      const QPolygonF &Points() const { return LastDrawnObjectPoints;}
      QPolygonF &Points() { return LastDrawnObjectPoints;}

      void AddPoint(QPointF Position)
      {
         LastDrawnObjectPoints.append(Position);
         CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));
      }
      void Clear()
      {
         LastDrawnObjectPoints.clear();
         CurrentPaintedObjectBoundingBox.Clear();
      }
   };

private:


   int myPenWidth;
   int myEraserWidth;
   int SelectedPenWidth;
   bool EraseLastDrawnObject;
   bool LastDrawingValid;
   bool MarkerActive;

   const QColor &TransparentColor;
   SettingClass &Settings;

   QPointF lastPointDrawn;

   ShapeClass CurrentShape;

   QColor myPenColor;
   QImage CurrentImage;


public:
   DrawingObjectClass(const QColor &Transparent, SettingClass &MySettings);
   void RestartCurrentPaintedObjectBoundingBox(QPointF const &StartPoint)
   {
      CurrentShape.CurrentPaintedObjectBoundingBox.Clear();
      CurrentShape.CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(StartPoint.x(), StartPoint.y()));
   }

   QRect drawLineTo(const QPointF &endPoint, float Pressure);
   QRect EraseLineTo(const QPointF &endPoint, float Pressure, QColor &BackGroundColor);
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
  // void ExtendBoundingboxAndShape(QPointF Position);
   ShapeClass EndShape(QPointF Position);
   void CutOut(QPainter &painter2, QPointF Offset);
   QImage &Image() {return CurrentImage;}
private:
   double CalculatePenWidthLinear(double Pressure, int BaseWidth);
   double CalculatePenWidthQuadratic(double Pressure, int BaseWidth);
   void ExtendPenWidthForMarker() {myPenWidth = SelectedPenWidth * 5 + 2;}
  // void DrawLastDrawnPicture(QPainter &painter, const QPointF &Offset);


};

#endif // DRAWINGOBJECTCLASS_H
