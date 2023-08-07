#ifndef DATABASECLASS_H
#define DATABASECLASS_H

#include <QImage>
#include <QPoint>
#include <QPainterPath>
#include <list>
#include <vector>
#include <deque>

#include "box.hpp"
#include "Settings.hpp"
#include "backgroundimagemanagerclass.h"
#include "postitmanagerclass.h"
#include "drawingobjectclass.h"
#include "selectionclass.h"
#include "pastingobjectclass.h"

class ScribbleArea;


class DatabaseClass : public QObject
{
   Q_OBJECT


   ScribbleArea &Parent;
   SettingClass &Settings;

   BackgroundImageManagerClass BackgroundImages;

   PostitManagerClass Postits;
   DrawingObjectClass CurrentlyDrawnObject;
   SelectionClass CurrentSeelection;
   PastingObjectClass PastingObject;

   QImage image;


   QPointF Origin;

   bool modified;
   bool AutosaveNeeded;
   bool ShowOverview;

   bool CutMode;

   QPointF lastPointDrawn;
   QPointF ButtonDownPosition;


   static constexpr QColor TransparentColor = QColor(255, 255, 255, 0);
   QColor BackGroundColor;
//   QColor Markercolor;
   QColor DefaultBackGroundColor;

   static constexpr int HintBorderPenWidth = 2;
   QColor ScrollHintColor;
   QColor ScrollHintBorderColor;



   //DrawingObjectClass::ShapeClass LastPaintedObject;

   struct LineSegments {
       QPointF endPoint;
       float Pressure;
       bool  Erasing;
   };

   std::list<LineSegments> StoredLineSegments;


public:
   enum PasteEvent {PasteTopLayer, PasteBottomLayer, PasteDrawing, CancelPasting, MakeBigger, MakeSmaller, MakeOriginalSize};

    static constexpr int getHintBorderPenWidth() {return HintBorderPenWidth;}
    static constexpr QColor getTransparentColor() { return TransparentColor;}

   void SetSelectedOffset() {
       CurrentSeelection.SetSelectedOffset(lastPointDrawn);
      //SelectedOffset = QPoint(SelectedImageBoundingBox.GetLeft(), SelectedImageBoundingBox.GetTop()) - lastPointDrawn;
   }


   bool IsInsideLastPaintedObjectBoundingBox(QPointF const &Point)
   {
      //return LastPaintedObject.Box().IsInside(PositionClass(Point.x(), Point.y()));
      return CurrentlyDrawnObject.IsInside(Point);
   }
   bool IsCutoutActive() {return CutMode;}
   void MoveOrigin(QPointF Offset) {
      Origin -= Offset;
      BackgroundImages.MoveOrigin(Offset);
   }
   QPointF GetOrigin() {return Origin;}

   DatabaseClass(ScribbleArea &Parent, class SettingClass &MySettings);
   bool ImportImage(const QString &fileName);

   void drawLineTo(const QPointF &endPoint, float Pressure);
   void EraseLineTo(const QPointF &endPoint, float Pressure);
   void StoredrawLineTo(const QPointF &endPoint, float Pressure);
   void StoreEraseLineTo(const QPointF &endPoint, float Pressure);
   void ClearStoredLineSegments() {StoredLineSegments.clear();}
   void drawrectangle(const BoundingBoxClass &Region);
   void DrawMovedSelection(const QPointF Offset);
   void MakeSreenMoveHint();

private:
   //void DrawLastDrawnPicture();
public:

   void resizeImage(QImage *image, const QSize &newSize, QPoint Offset = {0,0});
   void resizeScrolledImage();
   void MakeSelectionFromLastDrawnObject(bool Cutout = false);
   void TransferLastDrawnShape();
   void FilllastDrawnShape();
   void CancelShape();


   // Postits
   bool IsAnySelectedPostit() {return Postits.IsAnySelectedPostit();}
   void ClearSelectedPostit() {Postits.ClearSelectedPostit();}
   void CreeatePostitFromSelection();
   void MoveSelectedPostits(QPointF Position);
   void FinishMovingSelectedPostits(QPointF Position);
   void setPostItBackgroundColor(const QColor &newColor) {Postits.setPostItBackgroundColor(newColor); update();}
   void AdjustMarkercolor() {Postits.setMarkercolor(QColor(BackGroundColor.red()^0xFF, BackGroundColor.green()^0xFF, BackGroundColor.blue()^0xFF));}
   typedef PostitManagerClass::SelectMode SelectMode;
   bool FindSelectedPostIts(QPointF Position, SelectMode Mode = SelectMode::First)
   {
      return Postits.FindSelectedPostIts(Position + Origin, Mode);
   }
   bool IsInsideAnyPostIt(QPointF Position)
   {
      return Postits.IsInsideAnyPostIt(Position + Origin);
   }
   void setShowPostitsFrame(bool newShowPostitsFrame)
   {
      Postits.setShowPostitsFrame(newShowPostitsFrame);
   }
   void DeleteSelectedPostits();
   void DuplicateSelectedPostits();
private:
   //void CreatePostit_(QImage BackgroundImage, QImage Image, QPointF Position, BoundingBoxClass Box, QPainterPath Path);
public:



   // Copy / Paste
   void SetImageToPaste(QImage Image);
   void DoPasteImage(PasteEvent Event);
   void CancelPasteImage();
   void ScaleImageToPaste(double ScalingFactor);
   void PasteImage(QImage ImageToPaste);
   void CopyImageToClipboard();



   void GetOffsetAndAdjustOrigin(QImage &Image, QPointF &Origin, QPoint &Offset, QSize &Size);

   // Persistence
   bool SaveImage(const QString &fileName, const char *fileFormat);
   bool SaveDatabase(const QString &fileName);
   bool LoadDatabase(const QString &fileName);
private:
   QString AutosaveName;
    QString GetAutoSaveName();
    bool DrawStoredSegments();
public slots:
   void AutoSaveDatabase();
public:
   void SetAutosaveName(const QString &Name) {AutosaveName = Name;}


   void setPenColor(const QColor &newColor);
   void setPenWidth(int newWidth);
   void UseSpongeAsEraser(bool UseSponge) {CurrentlyDrawnObject.UseSpongeAsEraser(UseSponge);}
   void CutSelection(bool DoCut) {CutMode = DoCut;}
   void RestorePenWidth() {CurrentlyDrawnObject.RestorePenWidth();}

   // Planes
   void MoveImageToBackgroundLayer();
   void MoveTopBackgroundLayerToImage();
   void CollapseBackgroundLayers();
   void CollapseAllVisibleLayersToTop();
   bool SetLayerVisibility(unsigned int SelectedLayer, bool Visibility);
   void Freeze(bool Mode) {BackgroundImages.Freeze(Mode);}
   bool ImportImageToBackgroundLayer(const QString &fileName);


   void clearImage();

   bool isModified() const { return modified; }
   void SetModified() {modified =  true; AutosaveNeeded = true;}
   QColor penColor() const {return CurrentlyDrawnObject.penColor(); }
   int penWidth() const { return CurrentlyDrawnObject.penWidth(); }

   bool GetShowOverview() {return ShowOverview;}
   void ToggleShowOverview(bool Mode);
   void PaintOverview(QPainter &p, const QSize &OutputSize);


   QColor GetBackGroundColor() const { return BackGroundColor; }
   void setBackGroundColor(const QColor &newColor) {BackGroundColor = newColor; AdjustMarkercolor(); update();}
   void setSelectionHintColor(const QColor &newColor) {CurrentSeelection.setSelectionHintColor(newColor);}

   void PaintVisibleDrawing(QPainter &painter, const QRect &dirtyRect, const QPointF &Origin, const QPointF &BackgroundOffset);
   void PaintVisibleDrawing(QPainter &painter, const QRect &dirtyRect) {PaintVisibleDrawing(painter, dirtyRect, Origin, {0,0});}

   void setMarkerActive(bool newMarkerActive)
   {
      CurrentlyDrawnObject.setMarkerActive(newMarkerActive);
   }

   void DrawLastDrawnShapeAndStartNewShape();
  // void ClearLastDrawnObjectPoints() {LastDrawnObjectPoints.clear();}

   void ResizeAll(int width, int height);
   void EndShape();

   void setLastPoint(QPointF newLastPoint)
   {
      //CurrentlyDrawnObject.setLastPoint(newLastPoint);
      lastPointDrawn = newLastPoint;
   }
   QPointF getLastPointDrawn() {return lastPointDrawn;}

   void setButtonDownPosition(QPointF newButtonDownPosition)
   {
      ButtonDownPosition = newButtonDownPosition;
   }

   bool getDiscardSelection() const
   {
      return CurrentSeelection.getDiscardSelection();
   }
   void setDiscardSelection(bool newDiscardSelection)
   {
      CurrentSeelection.setDiscardSelection(newDiscardSelection);
   }

   QPointF getSelectedCurrentPosition() const
   {
      return CurrentSeelection.getSelectedCurrentPosition();
   }
   void setSelectedCurrentPosition(QPointF newSelectedCurrentPosition)
   {
      CurrentSeelection.setSelectedCurrentPosition(newSelectedCurrentPosition);
   }
   void MoveSelectedCurrentPosition(QPointF delta) {CurrentSeelection.MoveSelectedCurrentPosition(delta);}

   QPointF getButtonDownPosition() const
   {
      return ButtonDownPosition;
   }


   BackgroundImageManagerClass &getBackgroundImages()
   {
      return BackgroundImages;
   }


#if 0
   int getMyPenWidth() const
   {
      return CurrentlyDrawnObject.getMyPenWidth();
   }
#endif

  // void ExtendBoundingboxAndShape(QPointF Position);

   QPointF TranslateCoordinateOffsetFromOverview(QPointF Coordinates);
   const QColor &getScrollHintColor() const
   {
      return ScrollHintColor;
   }
   void setScrollHintColor(const QColor &newScrollHintColor)
   {
      ScrollHintColor = newScrollHintColor;
   }

   static constexpr double JitterPressureLimit = 0.6;
   bool IsJitter(QPointF OldPoint, QPointF NewPoint, double Pressure) {
      return ((Pressure < JitterPressureLimit) && ((OldPoint-NewPoint).manhattanLength() < (CurrentlyDrawnObject.getMyPenWidth()*3+2)));
   }
   bool IsSelectionJitter(QPointF OldPoint, QPointF NewPoint, float Pressure  [[maybe_unused]] ) {
      return ((OldPoint-NewPoint).manhattanLength() < (CurrentlyDrawnObject.getMyPenWidth()+2));
   }
   bool IsSelectionJitter(QPointF OldPoint, float Pressure  [[maybe_unused]] ) {
      return IsSelectionJitter(OldPoint, lastPointDrawn, Pressure);
   }


private:
public:

private:
   void update();
   void update(const QRect &r);
   void UpdateGUIElements();
};

#endif // DATABASECLASS_H
