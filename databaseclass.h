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

   void CreeatePostitFromSelection();

   void GetOffsetAndAdjustOrigin(QImage &Image, QPointF &Origin, QPoint &Offset, QSize &Size);


   bool SaveImage(const QString &fileName, const char *fileFormat);
   bool SaveDatabase(const QString &fileName);
   bool LoadDatabase(const QString &fileName);
   void setPenColor(const QColor &newColor);
   bool SetLayerVisibility(int SelectedLayer, bool Visibility);
   void setPenWidth(int newWidth);
   void PasteImage(QImage ImageToPaste);
   void CopyImageToClipboard();

   int MoveImageToBackgroundLayer();
   int MoveTopBackgroundLayerToImage();
   int CollapseBackgroundLayers();
   void clearImage();

   bool isModified() const { return modified; }
   QColor penColor() const { return myPenColor; }
   int penWidth() const { return SelectedPenWidth; }
   void Freeze(bool Mode) {Frozen = Mode;}

   QColor GetBackGroundColor() const { return BackGroundColor; }
   void setBackGroundColor(const QColor &newColor) {BackGroundColor = newColor; update();}
   void setPostItBackgroundColor(const QColor &newColor) {PostItBackgroundColor = newColor; update();}

   bool PostItSelected(QPointF Position);
   bool IsInsideAnyPostIt(QPointF Position);

   void PaintVisibleDrawing(QPainter &painter, const QRect &dirtyRect, const QPointF &Origin, const QPointF &BackgroundImagesOrigin);



private:
   void update();
   void update(const QRect &r);
   void UpdateGUI(int NumberOfLayers);
};

#endif // DATABASECLASS_H
