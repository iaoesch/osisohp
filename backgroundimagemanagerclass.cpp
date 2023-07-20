#include "backgroundimagemanagerclass.h"
#include "databaseclass.h"
#include <QPainter>

BackgroundImageManagerClass::BackgroundImageManagerClass()
{
   BackgroundFrozen = false;
}

bool BackgroundImageManagerClass::MoveTopBackgroundLayerToImage(QImage &Image, DatabaseClass &UglyPatchNeedsFixing)
{
   if (!BackgroundImages.empty()) {
      // create empty image
      QImage newImage(Image.size(), QImage::Format_ARGB32);
      newImage.fill(UglyPatchNeedsFixing.getTransparentColor());
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

void BackgroundImageManagerClass::DrawAllVisible(QPainter &painter, QRect const &dirtyRect, QPointF const Offset)
{
   for (auto &p: BackgroundImages) {
      if (p.IsVisible()) {
         painter.drawImage(dirtyRect, *p, dirtyRect.translated((BackgroundImagesOrigin + Offset).toPoint()));
      }
   }
}



void BackgroundImageManagerClass::Clear()
{
   BackgroundImagesOrigin = {0,0};
   BackgroundImages.clear();
}

void BackgroundImageManagerClass::resizeScrolledImage(QSize Size, QPoint Offset, DatabaseClass &UglyPatchNeedsFixing)
{

   if (!BackgroundFrozen && !BackgroundImages.empty()) {
      UglyPatchNeedsFixing.GetOffsetAndAdjustOrigin(*BackgroundImages[0], BackgroundImagesOrigin, Offset, Size);

      for (auto &p: BackgroundImages) {
         UglyPatchNeedsFixing.resizeImage(&*p, Size, Offset);
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

bool BackgroundImageManagerClass::AddLayer(QImage NewImage)
{
   BackgroundImages.push_back(std::make_unique<QImage>(NewImage));
}
