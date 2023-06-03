#ifndef DATABASECLASS_H
#define DATABASECLASS_H

#include <QImage>
#include <QPoint>
#include <list>
#include <vector>

#include "box.hpp"
#include "Settings.hpp"


class DatabaseClass
{
   QWidget &Parent;
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
   DatabaseClass(QWidget &Parent);
   bool ImportImage(const QString &fileName);

   void drawLineTo(const QPointF &endPoint, double Pressure);
   void EraseLineTo(const QPointF &endPoint, double Pressure);
   void drawrectangle(const BoundingBoxClass &Region);
   void DrawMovedSelection(const QPointF Offset);

   void DrawLastDrawnPicture();
   void resizeImage(QImage *image, const QSize &newSize, QPoint Offset = {0,0});
   void resizeScrolledImage();
   void MakeSelectionFromLastDrawnObject();

   void CreeatePostitFromSelection();

   void GetOffsetAndAdjustOrigin(QImage &Image, QPointF &Origin, QPoint &Offset, QSize &Size);


private:
   void update();
   void update(const QRect &r);
};

#endif // DATABASECLASS_H
