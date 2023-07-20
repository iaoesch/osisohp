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

class ScribbleArea;

class DatabaseClass : public QObject
{
   Q_OBJECT


   ScribbleArea &Parent;
   SettingClass &Settings;
   struct PostIt {
      static int NextId;
      QImage Image;
      QPointF Position;
      BoundingBoxClass Box;
      QPainterPath BorderPath;
      int Id;
      PostIt(const QImage &NewImage, const QPointF &Pos, BoundingBoxClass NewBox, QPainterPath &Path) : Image(NewImage), Position(Pos), Box(NewBox), BorderPath(Path), Id(NextId++) {}
      PostIt(const PostIt &Src) : Image(Src.Image), Position(Src.Position), Box(Src.Box), BorderPath(Src.BorderPath), Id(NextId++) {}
   };

   std::list<PostIt> PostIts;
   struct PostItDescriptor{
      std::list<PostIt>::iterator postit;
      QPointF StartPosition;
   } ;
   std::list<PostItDescriptor> SelectedPostit;

   BackgroundImageManagerClass BackgroundImages;

   int myPenWidth;
   int myEraserWidth;
   int SelectedPenWidth;
   QColor myPenColor;
   QImage image;
   QImage LastDrawnObject;


   QPointF Origin;

   bool EraseLastDrawnObject;
   bool modified;
   bool AutosaveNeeded;
   bool LastDrawingValid;
   bool DiscardSelection;
   bool MarkerActive;
   bool ShowPostitsFrame;
   bool ShowOverview;

   bool CutMode;


   QPointF SelectedOffset;
   QPointF SelectedCurrentPosition;
   QPointF lastPointDrawn;
   QPointF ButtonDownPosition;


   QColor TransparentColor;
   QColor BackGroundColor;
   QColor Markercolor;
   QColor DefaultBackGroundColor;
   QColor ScrollHintColor;
   QColor ScrollHintBorderColor;
   QColor SelectionHintColor;
   QColor SelectionHintBorderColor;

   QColor PostItBackgroundColor;

   QPolygonF LastDrawnObjectPoints;
   QImage SelectedImagePart;
   QPainterPath SelectedImagePartPath;
   QImage HintSelectedImagePart;

   QImage ImageToPaste;
   double ScalingFactorOfImageToPaste;
   enum   PasteImage{None, Drawing, TopLayer, BottomLayer} PasteStatus;

   bool RecentlyPastedObjectValid;
   QPointF RecentlyPastedObjectPosition;
   QImage RecentlyPastedObject;
   BoundingBoxClass RecentlyPastedObjectBoundingBox;

   BoundingBoxClass LastPaintedObjectBoundingBox;
   BoundingBoxClass SelectedImageBoundingBox;
   BoundingBoxClass CurrentPaintedObjectBoundingBox;


public:
   enum PasteEvent {PasteTopLayer, PasteBottomLayer, PasteDrawing, CancelPasting, MakeBigger, MakeSmaller, MakeOriginalSize};


   void SetSelectedOffset() {
      SelectedOffset = QPoint(SelectedImageBoundingBox.GetLeft(), SelectedImageBoundingBox.GetTop()) - lastPointDrawn;
   }

   void RestartCurrentPaintedObjectBoundingBox(QPointF const &StartPoint)
   {
      CurrentPaintedObjectBoundingBox.Clear();
      CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(StartPoint.x(), StartPoint.y()));
   }

   bool IsInsideLastPaintedObjectBoundingBox(QPointF const &Point)
   {
      return LastPaintedObjectBoundingBox.IsInside(PositionClass(Point.x(), Point.y()));
   }
   bool IsCutoutActive() {return CutMode;}
   void ClearLastPaintedObjectBoundingBox() { LastPaintedObjectBoundingBox.Clear();}
   void MoveOrigin(QPointF Offset) {
      Origin -= Offset;
      BackgroundImages.MoveOrigin(Offset);
   }
   QPointF GetOrigin() {return Origin;}

   DatabaseClass(ScribbleArea &Parent, class SettingClass &MySettings);
   bool ImportImage(const QString &fileName);

   void drawLineTo(const QPointF &endPoint, double Pressure);
   void EraseLineTo(const QPointF &endPoint, double Pressure);
   void drawrectangle(const BoundingBoxClass &Region);
   void DrawMovedSelection(const QPointF Offset);
   void MakeSreenMoveHint();

   void DrawLastDrawnPicture();
   void resizeImage(QImage *image, const QSize &newSize, QPoint Offset = {0,0});
   void resizeScrolledImage();
   void MakeSelectionFromLastDrawnObject(bool Cutout = false);
   void CompleteImage();
   void FilllastDrawnShape();

   bool IsAnySelectedPostit() {return !SelectedPostit.empty();}
   void ClearSelectedPostit() {SelectedPostit.clear();}
   void CreeatePostitFromSelection();
   void MoveSelectedPostits(QPointF Position);
   void FinishMovingSelectedPostits(QPointF Position);

   void SetImageToPaste(QImage Image);
   void DoPasteImage(PasteEvent Event);
   void CancelPasteImage();
   void ScaleImageToPaste(double ScalingFactor);


   void GetOffsetAndAdjustOrigin(QImage &Image, QPointF &Origin, QPoint &Offset, QSize &Size);


   bool SaveImage(const QString &fileName, const char *fileFormat);
   bool SaveDatabase(const QString &fileName);
   bool LoadDatabase(const QString &fileName);
   void setPenColor(const QColor &newColor);
   bool SetLayerVisibility(unsigned int SelectedLayer, bool Visibility);
   void setPenWidth(int newWidth);
   void UseSpongeAsEraser(bool UseSponge) {if (UseSponge) {myEraserWidth = Settings.SpongeSize;} else {myEraserWidth = Settings.EraserSize;}}
   void CutSelection(bool DoCut) {CutMode = DoCut;}
   void RestorePenWidth() {myPenWidth = SelectedPenWidth;}
   void ExtendPenWidthForMarker() {myPenWidth = SelectedPenWidth * 5 + 2;}
   void PasteImage(QImage ImageToPaste);
   void CopyImageToClipboard();


   void MoveImageToBackgroundLayer_();
   void MoveTopBackgroundLayerToImage_();
   void CollapseBackgroundLayers_();
   void CollapseAllVisibleLayersToTop_();
   void clearImage();

   bool isModified() const { return modified; }
   void SetModified() {modified =  true; AutosaveNeeded = true;}
   QColor penColor() const { return myPenColor; }
   int penWidth() const { return SelectedPenWidth; }
   void Freeze(bool Mode) {BackgroundImages.Freeze(Mode);}
   bool GetShowOverview() {return ShowOverview;}
   void ToggleShowOverview(bool Mode);

   QColor GetBackGroundColor() const { return BackGroundColor; }
   void setBackGroundColor(const QColor &newColor) {BackGroundColor = newColor; AdjustMarkercolor(); update();}
   void AdjustMarkercolor() {Markercolor = QColor(BackGroundColor.red()^0xFF, BackGroundColor.green()^0xFF, BackGroundColor.blue()^0xFF);}
   void setPostItBackgroundColor(const QColor &newColor) {PostItBackgroundColor = newColor; update();}
   void setSelectionHintColor(const QColor &newColor) {SelectionHintColor = newColor;}

   enum SelectMode {All, First, Last};
   bool FindSelectedPostIts(QPointF Position, SelectMode Mode = First);
   bool IsInsideAnyPostIt(QPointF Position);

   void PaintVisibleDrawing(QPainter &painter, const QRect &dirtyRect, const QPointF &Origin, const QPointF &BackgroundOffset);
   void PaintVisibleDrawing(QPainter &painter, const QRect &dirtyRect) {PaintVisibleDrawing(painter, dirtyRect, Origin, {0,0});}



   void setMarkerActive(bool newMarkerActive)
   {
      MarkerActive = newMarkerActive;
      if (MarkerActive) {
         ExtendPenWidthForMarker();
      } else {
         RestorePenWidth();
      }
   }

   void FlushLastDrawnPicture();
   void ClearLastDrawnObjectPoints() {LastDrawnObjectPoints.clear();}

   void ResizeAll(int width, int height);
   void UpdateBoundingboxesForFinishedShape(QPointF Position);
   void PaintOverview(QPainter &p, const QSize &OutputSize);
   bool getLastDrawingValid() const
   {
      return LastDrawingValid;
   }
   void setLastDrawingValid(bool newLastDrawingValid)
   {
      LastDrawingValid = newLastDrawingValid;
   }

   void setLastPoint(QPointF newLastPoint)
   {
      lastPointDrawn = newLastPoint;
   }
   QPointF getLastPointDrawn() {return lastPointDrawn;}

   void setButtonDownPosition(QPointF newButtonDownPosition)
   {
      ButtonDownPosition = newButtonDownPosition;
   }

   bool getDiscardSelection() const
   {
      return DiscardSelection;
   }
   void setDiscardSelection(bool newDiscardSelection)
   {
      DiscardSelection = newDiscardSelection;
   }

   QPointF getSelectedCurrentPosition() const
   {
      return SelectedCurrentPosition;
   }
   void setSelectedCurrentPosition(QPointF newSelectedCurrentPosition)
   {
      SelectedCurrentPosition = newSelectedCurrentPosition;
   }
   void MoveSelectedCurrentPosition(QPointF delta) {SelectedCurrentPosition += delta;}

   QPointF getButtonDownPosition() const
   {
      return ButtonDownPosition;
   }

   int getMyPenWidth() const
   {
      return myPenWidth;
   }

   void ExtendBoundingboxAndShape(QPointF Position);
   void setShowPostitsFrame(bool newShowPostitsFrame)
   {
      ShowPostitsFrame = newShowPostitsFrame;
   }

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
      return ((Pressure < JitterPressureLimit) && ((OldPoint-NewPoint).manhattanLength() < (getMyPenWidth()*3+2)));
   }
   bool IsSelectionJitter(QPointF OldPoint, QPointF NewPoint, double Pressure /* [[maybe_unused]] */) {
      return ((OldPoint-NewPoint).manhattanLength() < (getMyPenWidth()+2));
   }

   bool ImportImageToBackgroundLayer(const QString &fileName);
   void ClearLastDrawnPicture();

   void DeleteSelectedPostits();
   void DuplicateSelectedPostits();
private:
public:
   void SetAutosaveName(const QString &Name) {AutosaveName = Name;}
   const QColor &getTransparentColor() const
   {
      return TransparentColor;
   }

private:
   QString AutosaveName;
   QString GetAutoSaveName();
public slots:
   void AutoSaveDatabase();


private:
   double CalculatePenWidthLinear(double Pressure, int BaseWidth);
   double CalculatePenWidthQuadratic(double Pressure, int BaseWidth);
   void update();
   void update(const QRect &r);
   void UpdateGUIElements();
   void CreatePostit(QImage BackgroundImage, QImage Image, QPointF Position, BoundingBoxClass Box, QPainterPath Path);
};

#endif // DATABASECLASS_H
