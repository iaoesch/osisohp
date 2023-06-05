#ifndef DATABASECLASS_H
#define DATABASECLASS_H

#include <QImage>
#include <QPoint>
#include <list>
#include <vector>

#include "box.hpp"
#include "Settings.hpp"

class ScribbleArea;

class DatabaseClass
{
   ScribbleArea &Parent;
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

   int myPenWidth;
   int myEraserWidth;
   int SelectedPenWidth;
   QColor myPenColor;
   QImage image;
   QImage LastDrawnObject;
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

   QPointF Origin;
   bool Frozen;
   QPointF BackgroundImagesOrigin;


   bool EraseLastDrawnObject;
   bool modified;
   bool LastDrawingValid;
   bool DiscardSelection;
   bool MarkerActive;
   bool ShowPostitsFrame;


   QPointF SelectedOffset;
   QPointF SelectedCurrentPosition;
   QPointF lastPoint;
   QPointF ButtonDownPosition;


   QColor TransparentColor;
   QColor BackGroundColor;
   QColor DefaultBackGroundColor;
   QColor PostItBackgroundColor;

   QPolygonF LastDrawnObjectPoints;
   QImage SelectedImagePart;
   QImage HintSelectedImagePart;

   bool RecentlyPastedObjectValid;
   QPointF RecentlyPastedObjectPosition;
   QImage RecentlyPastedObject;
   BoundingBoxClass RecentlyPastedObjectBoundingBox;

   BoundingBoxClass LastPaintedObjectBoundingBox;
   BoundingBoxClass CurrentPaintedObjectBoundingBox;


public:
   void SetSelectedOffset() {SelectedOffset = QPoint(LastPaintedObjectBoundingBox.GetLeft(), LastPaintedObjectBoundingBox.GetTop()) - lastPoint;}

   void RestartCurrentPaintedObjectBoundingBox(QPointF const &StartPoint) {  CurrentPaintedObjectBoundingBox.Clear();
                                                          CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(StartPoint.x(), StartPoint.y()));
   }

   bool IsInsideLstPaintedObjectBoundingBox(QPointF const &Point) { return LastPaintedObjectBoundingBox.IsInside(PositionClass(Point.x(), Point.y()));}
   void MoveOrigin(QPointF Offset) {

      Origin -= Offset;
      if (!Frozen) {
         BackgroundImagesOrigin -= Offset;
      }
   }
   QPointF GetOrigin() {return Origin;}

   DatabaseClass(ScribbleArea &Parent);
   bool ImportImage(const QString &fileName);

   void drawLineTo(const QPointF &endPoint, double Pressure);
   void EraseLineTo(const QPointF &endPoint, double Pressure);
   void drawrectangle(const BoundingBoxClass &Region);
   void DrawMovedSelection(const QPointF Offset);
   void MakeSreenMoveHint();

   void DrawLastDrawnPicture();
   void resizeImage(QImage *image, const QSize &newSize, QPoint Offset = {0,0});
   void resizeScrolledImage();
   void MakeSelectionFromLastDrawnObject();
   void CompleteImage();
   void FilllastDrawnShape();

   bool IsAnySelectedPostit() {return !SelectedPostit.empty();}
   void ClearSelectedPostit() {SelectedPostit.clear();}
   void CreeatePostitFromSelection();
   void MoveSelectedPostits(QPointF Position);
   void FinishMovingSelectedPostits(QPointF Position);

   void GetOffsetAndAdjustOrigin(QImage &Image, QPointF &Origin, QPoint &Offset, QSize &Size);


   bool SaveImage(const QString &fileName, const char *fileFormat);
   bool SaveDatabase(const QString &fileName);
   bool LoadDatabase(const QString &fileName);
   void setPenColor(const QColor &newColor);
   bool SetLayerVisibility(int SelectedLayer, bool Visibility);
   void setPenWidth(int newWidth);
   void UseSpongeAsEraser(bool UseSponge) {if (UseSponge) {myEraserWidth = Settings.SpongeSize;} else {myEraserWidth = Settings.EraserSize;}}
   void RestorePenWidth() {myPenWidth = SelectedPenWidth;}
   void ExtendPenWidthForMarker() {myPenWidth = SelectedPenWidth * 5 + 2;}
   void PasteImage(QImage ImageToPaste);
   void CopyImageToClipboard();


   void MoveImageToBackgroundLayer();
   void MoveTopBackgroundLayerToImage();
   void CollapseBackgroundLayers();
   void CollapseAllVisibleLayersToTop();
   void clearImage();

   bool isModified() const { return modified; }
   void SetModified() {modified =  true;}
   QColor penColor() const { return myPenColor; }
   int penWidth() const { return SelectedPenWidth; }
   void Freeze(bool Mode) {Frozen = Mode;}

   QColor GetBackGroundColor() const { return BackGroundColor; }
   void setBackGroundColor(const QColor &newColor) {BackGroundColor = newColor; update();}
   void setPostItBackgroundColor(const QColor &newColor) {PostItBackgroundColor = newColor; update();}

   bool PostItSelected(QPointF Position);
   bool IsInsideAnyPostIt(QPointF Position);

   void PaintVisibleDrawing(QPainter &painter, const QRect &dirtyRect, const QPointF &Origin, const QPointF &BackgroundImagesOrigin);
   void PaintVisibleDrawing(QPainter &painter, const QRect &dirtyRect) {PaintVisibleDrawing(painter, dirtyRect, Origin, BackgroundImagesOrigin);}



   void setMarkerActive(bool newMarkerActive)
   {
      MarkerActive = newMarkerActive;
   }

   void FlushLastDrawnPicture();
   void ClearLastDrawnObjectPoints() {LastDrawnObjectPoints.clear();}

   void ResizeAll(int width, int height);
   void UpdateBoundingboxesForFinishedShape(QPointF Position);
   void PaintOverview(QPainter &p, const QSize &OutputSize);
   bool getLastDrawingValid() const;
   void setLastDrawingValid(bool newLastDrawingValid);

   void setLastPoint(QPointF newLastPoint);
   QPointF getLastPoint() {return lastPoint;}

   void setButtonDownPosition(QPointF newButtonDownPosition);

   bool getDiscardSelection() const;
   void setDiscardSelection(bool newDiscardSelection);

   QPointF getSelectedCurrentPosition() const;
   void setSelectedCurrentPosition(QPointF newSelectedCurrentPosition);
   void MoveSelectedCurrentPosition(QPointF delta) {SelectedCurrentPosition += delta;}

   QPointF getButtonDownPosition() const;

   int getMyPenWidth() const;

   void ExtendBoundingboxAndShape(QPointF Position);
   void setShowPostitsFrame(bool newShowPostitsFrame);

private:
   void update();
   void update(const QRect &r);
   void UpdateGUI(int NumberOfLayers);
};

#endif // DATABASECLASS_H
