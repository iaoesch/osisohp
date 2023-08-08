#include "backgroundimagemanagerclass.h"
#include "databaseclass.h"
#include <QPainter>

static inline int ToInt(double d) {return static_cast<int>(d + 0.5);}


BackgroundImageManagerClass::BackgroundImageManagerClass()
{
   BackgroundFrozen = false;
}

bool BackgroundImageManagerClass::MoveTopBackgroundLayerToImage(QImage &Image, DatabaseClass &UglyPatchNeedsFixing)
{
   if (!BackgroundImages.empty()) {
      // create empty image
      QImage newImage(Image.size(), QImage::Format_ARGB32);
      newImage.fill(DatabaseClass::getTransparentColor());
      QPainter painter(&newImage);

      // draw top bg layer onto it
      painter.drawImage((BackgroundImagesOrigin-UglyPatchNeedsFixing.GetOrigin()).toPoint(), *BackgroundImages.back());
      BackgroundImages.pop_back();

      // Then draw our current image (Without the currently drawn object)
      painter.drawImage(QPoint(0,0), Image);

      //Set combined image as new image
      Image = newImage;

      return true;
   }
   return false;
}

bool BackgroundImageManagerClass::CollapseBackgroundLayers()
{
   if (!BackgroundImages.empty()) {
      QImage newImage;
      QPainter painter;
      auto it = BackgroundImages.begin();
      while (it != BackgroundImages.end()) {
         if (it->IsVisible()) {
            if (newImage.isNull()) {
               newImage = **it;
               painter.begin(&newImage);
            } else {
               painter.drawImage(QPoint(0,0), **it);
            }
            it = BackgroundImages.erase(it);
         } else {
            it++;
         }
      }
      BackgroundImages.push_back(std::make_unique<QImage>(newImage));
      return true;
   }
   return false;
}

bool BackgroundImageManagerClass::CollapseAllVisibleLayersToTop(QImage &Image, DatabaseClass &UglyPatchNeedsFixing)
{
   if (!BackgroundImages.empty()) {
      QImage newImage(Image.size(), QImage::Format_ARGB32);
      newImage.fill(UglyPatchNeedsFixing.getTransparentColor());
      QPainter painter(&newImage);
      auto it = BackgroundImages.begin();
      while (it != BackgroundImages.end()) {
         if (it->IsVisible()) {
            painter.drawImage(BackgroundImagesOrigin-UglyPatchNeedsFixing.GetOrigin(), **it);
            it = BackgroundImages.erase(it);
         } else {
            it++;
         }
      }
      painter.drawImage(QPoint(0,0), Image);
      Image = newImage;
      return true;
   }
   return false;
}

void BackgroundImageManagerClass::Resize(int width, int height)
{
   if (!BackgroundFrozen) {
      for (auto &p: BackgroundImages) {
         DatabaseClass::resizeImage(&*p, QSize(width+ToInt(BackgroundImagesOrigin.x()), ToInt(height+BackgroundImagesOrigin.y())));
      }
   }
}

void BackgroundImageManagerClass::Expand(QSize RequiredSize)
{
if (!BackgroundFrozen && !BackgroundImages.empty()) {
   for (auto &p: BackgroundImages) {
      // ToDo: required size is based on origin, should probably be based on BackgroundImagesOrigin
         DatabaseClass::resizeImage(&*p, RequiredSize);
   }
}
}

void BackgroundImageManagerClass::DrawAllVisible(QPainter &painter, QRect const &dirtyRect, QPointF const Offset)
{
   for (auto &p: BackgroundImages) {
      if (p.IsVisible()) {
         painter.drawImage(dirtyRect, *p, dirtyRect.translated((BackgroundImagesOrigin + Offset).toPoint()));
      }
   }
}

void BackgroundImageManagerClass::Save(QDataStream &out)
{
   // todo: read and save colors pen and background and backgroundimages
      out << BackgroundImagesOrigin;
      out << static_cast<quint32>(BackgroundImages.size());
      for (auto &Picture: BackgroundImages) {
         out << Picture.IsVisible();
         out << *Picture;
      }
}

std::vector<bool> BackgroundImageManagerClass::Load(QDataStream &in)
{
   // Write the data
   in >> BackgroundImagesOrigin;

   qint32 NumberOfBackgroundLayers = 0;
   in >> NumberOfBackgroundLayers;
   BackgroundImages.clear();
   bool Visible;
   QImage NewImage;

   std::vector<bool> Visibilities;
   //emit(NumberOfLayerChanged(NumberOfBackgroundLayers));
   for (int i = 0; i < NumberOfBackgroundLayers; i++) {
      in >> Visible;
      in >> NewImage;
      BackgroundImages.push_back(ImageDescriptor(std::make_unique<QImage>(NewImage), Visible));
      Visibilities.push_back(Visible);
      //emit(SetVisibilityIndicatorOfLayer(i, Visible));
   }
   return Visibilities;
}

void BackgroundImageManagerClass::Clear()
{
   BackgroundImagesOrigin = {0,0};
   BackgroundImages.clear();
}

void BackgroundImageManagerClass::resizeScrolledImage(QSize Size, QPoint Offset, DatabaseClass &UglyWorkaroundNeedsFixing)
{

   if (!BackgroundFrozen && !BackgroundImages.empty()) {
      UglyWorkaroundNeedsFixing.GetOffsetAndAdjustOrigin(*BackgroundImages[0], BackgroundImagesOrigin, Offset, Size);

      for (auto &p: BackgroundImages) {
         DatabaseClass::resizeImage(&*p, Size, Offset);
      }
   }

}

std::vector<bool> BackgroundImageManagerClass::GetLayervisibilities()
{
   unsigned long NumberOfLayers = BackgroundImages.size();

   std::vector<bool> Visibilities;
   for (unsigned int i = 0; i < NumberOfLayers; i++) {
      Visibilities.push_back( BackgroundImages[i].IsVisible());
   }
   return Visibilities;
}

bool BackgroundImageManagerClass::SetLayerVisibility(unsigned int SelectedLayer, bool Visibility)
{
   if (SelectedLayer < BackgroundImages.size()) {
      BackgroundImages[SelectedLayer].SetVisible(Visibility);
      return true;
   }
   return false;
}

void BackgroundImageManagerClass::AddLayerTop(QImage NewImage)
{
   BackgroundImages.push_back(std::make_unique<QImage>(NewImage));
}

void BackgroundImageManagerClass::AddLayerBottom(QImage NewImage)
{
   BackgroundImages.push_front(std::make_unique<QImage>(NewImage));
}
