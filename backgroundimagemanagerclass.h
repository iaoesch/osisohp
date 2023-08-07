#ifndef BACKGROUNDIMAGEMANAGERCLASS_H
#define BACKGROUNDIMAGEMANAGERCLASS_H

#include <QImage>
#include <deque>

class DatabaseClass;

class BackgroundImageManagerClass
{
   class ImageDescriptor {
     std::unique_ptr<QImage> Image;
     bool Visible;

     public:
     ImageDescriptor(std::unique_ptr<QImage> TheImage) : Image(std::move(TheImage)), Visible(true) {}
     ImageDescriptor(std::unique_ptr<QImage> TheImage, bool v) : Image(std::move(TheImage)), Visible(v) {}
     QImage &operator * () {return *Image;}
     QImage *operator -> () {return Image.operator ->();}
     bool IsVisible() {return Visible;}
     void SetVisible(bool v) {Visible = v;}

  };

  //std::vector<std::unique_ptr<QImage>> BackgroundImages;
  std::deque<ImageDescriptor> BackgroundImages;

  QPointF BackgroundImagesOrigin;
  bool BackgroundFrozen;



public:
   BackgroundImageManagerClass();

   void MoveImageToBackgroundLayer();
   bool MoveTopBackgroundLayerToImage(QImage &Image, DatabaseClass &UglyPatchNeedsFixing);
   bool CollapseBackgroundLayers();
   bool CollapseAllVisibleLayersToTop(QImage &Image, DatabaseClass &UglyPatchNeedsFixing);

   void MoveOrigin(QPointF Offset)  {
      if (!BackgroundFrozen) {
         BackgroundImagesOrigin -= Offset;
      }
   }
   void Clear();

   void Freeze(bool Mode) {BackgroundFrozen = Mode;}

   void resizeScrolledImage(QSize Size, QPoint Offset, DatabaseClass &UglyPatchNeedsFixing);

   std::vector<bool> GetLayervisibilities();

   bool SetLayerVisibility(unsigned int SelectedLayer, bool Visibility);
   void AddLayerTop(QImage NewImage);
   void DrawAllVisible(QPainter &painter, const QRect &dirtyRect, const QPointF Offset);
   void Save(QDataStream &out);
   std::vector<bool> Load(QDataStream &in);
   void Resize(int width, int height, DatabaseClass &UglyPatchNeedsFixing);
   void Expand(QSize RequiredSize, DatabaseClass &UglyPatchNeedsFixing);
   void AddLayerBottom(QImage NewImage);
};



#endif // BACKGROUNDIMAGEMANAGERCLASS_H
