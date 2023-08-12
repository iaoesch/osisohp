#ifndef BACKGROUNDIMAGEMANAGERCLASS_H
#define BACKGROUNDIMAGEMANAGERCLASS_H

#include "box.hpp"

#include <QImage>
#include <deque>

class DatabaseClass;

class BackgroundImageManagerClass
{
   class ImageDescriptor {
     std::unique_ptr<QImage> Image;
     QPointF Offset;
     bool Visible;

     public:
     ImageDescriptor(std::unique_ptr<QImage> TheImage, QPointF TheOffset) : Image(std::move(TheImage)), Offset(TheOffset), Visible(true) {}
     ImageDescriptor(std::unique_ptr<QImage> TheImage, QPointF TheOffset, bool v) : Image(std::move(TheImage)), Offset(TheOffset), Visible(v) {}
   //  QImage &operator * () {return *Image;}
   //  QImage *operator -> () {return Image.operator ->();}
     bool IsVisible() const {return Visible;}
     void SetVisible(bool v) {Visible = v;}

     void Move(QPointF DeltaOffset)  {
         Offset -= DeltaOffset;
     }

     void Draw(QPainter &painter) const;
     BoundingBoxClass Box() const {return BoundingBoxClass(Offset.x(), Offset.y(), Offset.x() + Image->width(), Offset.y() + Image->height());}

     void Draw(QPainter &painter, QPointF Shift) const;
     void Draw(QPainter &painter, QRect DirtyRect) const;

     friend QDataStream &operator << (QDataStream &, const ImageDescriptor &Data);
     friend QDataStream &operator >> (QDataStream &, ImageDescriptor &Data);
   };

   friend QDataStream &operator << (QDataStream &, const ImageDescriptor &Data);
   friend QDataStream &operator >> (QDataStream &, ImageDescriptor &Data);


  //std::vector<std::unique_ptr<QImage>> BackgroundImages;
  std::deque<ImageDescriptor> BackgroundImages;

  QPointF BackgroundImagesOrigin;
  bool BackgroundFrozen;



public:
   BackgroundImageManagerClass();

   void MoveImageToBackgroundLayer();
   bool MoveTopBackgroundLayerToImage(QImage &Image, DatabaseClass &UglyPatchNeedsFixing);
   bool CollapseBackgroundLayers();
   bool CollapseAllVisibleLayersToTop(QImage &Image);

   void MoveOrigin(QPointF Offset)  {
      if (!BackgroundFrozen) {
         BackgroundImagesOrigin -= Offset;
          for (auto &p: BackgroundImages) {
              p.Move(Offset);
          }
      }
   }
   void Clear();

   void Freeze(bool Mode) {BackgroundFrozen = Mode;}

   //void resizeScrolledImage(QSize Size, QPoint Offset, DatabaseClass &UglyWorkaroundNeedsFixing);

   std::vector<bool> GetLayervisibilities();

   bool SetLayerVisibility(unsigned int SelectedLayer, bool Visibility);
   void AddLayerTop(QImage NewImage, QPointF Offset);
   void DrawAllVisible(QPainter &painter, const QRect &dirtyRect, const QPointF Offset);
   void Save(QDataStream &out);
   std::vector<bool> Load(QDataStream &in);
   //void Resize(int width, int height);
   //void Expand(QSize RequiredSize);
   void AddLayerBottom(QImage NewImage, QPointF Offset);
};



#endif // BACKGROUNDIMAGEMANAGERCLASS_H
