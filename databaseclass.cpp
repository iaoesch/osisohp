#include "databaseclass.h"
#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QPainterPath>
#include <QImage>
#include <QPoint>
#include <QFile>
#include <QTimer>
#include <QPolygon>
#include <QTouchEvent>
#include <list>
#include "scribblearea.h"

DatabaseClass::DatabaseClass(ScribbleArea &Parent)
   : Parent(Parent)
{
   modified = false;
   LastDrawingValid = false;
   EraseLastDrawnObject = false;
   Frozen = false;
   myPenWidth = 2;
   SelectedPenWidth = myPenWidth;
   myPenColor = Qt::blue;

   TransparentColor = QColor(255, 255, 255, 0);
   BackGroundColor = QColor(230,230, 200,255);
   DefaultBackGroundColor = BackGroundColor;
   PostItBackgroundColor = QColor(100, 0, 0, 50);


   RecentlyPastedObjectValid = false;
   MarkerActive = false;

   SelectedPostit.clear();


}

void DatabaseClass::update()
{
   /// @todo propagate
}

void DatabaseClass::update(QRect const &r)
{
   /// @todo propagate
}
//! [1]
bool DatabaseClass::ImportImage(const QString &fileName)
//! [1] //! [2]
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(Parent.size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    Origin = {0,0};
    BackgroundImagesOrigin = {0,0};
    modified = false;
    LastDrawingValid = false;
    EraseLastDrawnObject = false;
    BackgroundImages.clear();
    update();
    return true;
}

void DatabaseClass::MakeSelectionFromLastDrawnObject()
{
   SelectedImagePart =  image.copy(LastPaintedObjectBoundingBox.QRectangle().translated(Origin.toPoint()));
   HintSelectedImagePart = SelectedImagePart;
   HintSelectedImagePart.fill(qRgba(0, 0, 0, 0));
   DiscardSelection = false;

   QPainter painter2(&image);
   painter2.setPen(QPen(QColor(0, 0, 0, 0), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
   painter2.setBrush(QBrush(QColor(0, 0, 0, 0)));
   painter2.setCompositionMode(QPainter::CompositionMode_Source);
   // LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
   painter2.drawPolygon(LastDrawnObjectPoints.translated(Origin));

   QPainter painter(&HintSelectedImagePart);
   painter.setPen(QPen(QColor(0, 30, 0, 50), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
   painter.setBrush(QBrush(QColor(0, 30, 0, 50)));
   LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetLeft(), -LastPaintedObjectBoundingBox.GetTop());
   painter.drawPolygon(LastDrawnObjectPoints);
   QPainterPath Path;
   Path.addPolygon(LastDrawnObjectPoints);
   QImage MaskedSelectedImagePart = SelectedImagePart;
   MaskedSelectedImagePart.fill(qRgba(0, 0, 0, 0));
   QPainter painter3(&MaskedSelectedImagePart);
   painter3.setClipPath(Path);
   painter3.drawImage(QPoint(0,0), SelectedImagePart);
   SelectedImagePart = MaskedSelectedImagePart;
   LastDrawnObject.fill(qRgba(0, 0, 0, 0));
}

void DatabaseClass::CreeatePostitFromSelection()
{
   QImage NewPostit(HintSelectedImagePart);
   // Here we could add a different background for postits
   QPainter painter(&NewPostit);
   painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
   painter.setBrush(QBrush(PostItBackgroundColor));
   painter.drawRect(NewPostit.rect());
   painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
   painter.drawImage(0,0,SelectedImagePart);
   BoundingBoxClass TranslatedBoundingBox (LastPaintedObjectBoundingBox);
   TranslatedBoundingBox.Move(PositionClass(Origin.x(), Origin.y()));
   PostIts.push_back(PostIt(NewPostit, Origin + SelectedCurrentPosition+SelectedOffset, TranslatedBoundingBox));
   SelectedPostit.push_back({std::prev(PostIts.end()), PostIts.back().Position});
}

void DatabaseClass::drawLineTo(const QPointF &endPoint, double Pressure)
//! [17] //! [18]
{
   std::cout << "Drawing ";
   int ModifiedPenWidth = myPenWidth * (1.0 + Pressure*Pressure*4);
    QPainter painter(&LastDrawnObject);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setPen(QPen(myPenColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);
    modified = true;

    int rad = (ModifiedPenWidth / 2) + 2;
    update(QRect(lastPoint.toPoint(), endPoint.toPoint()).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
    EraseLastDrawnObject = false;

}

void DatabaseClass::EraseLineTo(const QPointF &endPoint, double Pressure)
//! [17] //! [18]
{
    std::cout << "Erasing ";
    QPainter painter(&LastDrawnObject);
    int ModifiedPenWidth = (myPenWidth+2)*3*(1.0 + Pressure*Pressure*10);
    painter.setPen(QPen(BackGroundColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
   // painter.setCompositionMode(QPainter::CompositionMode_Source);
    //painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.drawLine(lastPoint, endPoint);
    modified = true;
    EraseLastDrawnObject = true;

    int rad = (ModifiedPenWidth / 2) + 2;
    update(QRect(lastPoint.toPoint(), endPoint.toPoint()).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}

/*
void ScribbleArea::drawrectangle(const BoundingBoxClass &Region)
//! [17] //! [18]
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    QRect Rectangle(Region.QRectangle());
    painter.drawRect(Rectangle);
    modified = true;

    int rad = (myPenWidth / 2) + 2;
    update(Rectangle.normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));

}
*/
//! [17]
void DatabaseClass::DrawLastDrawnPicture()
{
    QPainter painter(&image);
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    if (EraseLastDrawnObject) {
       painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    }
    if (MarkerActive) {
       painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);

    }
    painter.drawImage(Origin, LastDrawnObject);
    LastDrawnObject.fill(TransparentColor);
    modified = true;

    update();

}


void DatabaseClass::GetOffsetAndAdjustOrigin(QImage &Image, QPointF &Origin, QPoint &Offset, QSize &Size)
{
   if (Origin.x() < 0) {
       Size.setWidth(image.size().width() - Origin.x());
       Offset.setX(- Origin.x());
       Origin.setX(0);
   } else if (Origin.x()+Parent.width() > image.size().width()){
       Size.setWidth(Origin.x()+Parent.width());
   } else {
       Size.setWidth(image.size().width());
   }
   if (Origin.y() < 0) {
       Size.setHeight(image.size().height() - Origin.y());
       Offset.setY(- Origin.y());
       Origin.setY(0);
   } else if (Origin.y()+Parent.height() > image.size().height()){
       Size.setHeight(Origin.y()+Parent.height());
   } else {
       Size.setHeight(image.size().height());
   }
}



void DatabaseClass::resizeScrolledImage()
//! [19] //! [20]
{
    int NewWidth;
    int NewHeight;
    int OffsetX = 0;
    int OffsetY = 0;
    QPoint Offset;
    QSize Size;

    GetOffsetAndAdjustOrigin(image, Origin, Offset, Size);
#if 1
    resizeImage(&image, Size, Offset);
    if (!Frozen && !BackgroundImages.empty()) {
       GetOffsetAndAdjustOrigin(*BackgroundImages[0], BackgroundImagesOrigin, Offset, Size);

       for (auto &p: BackgroundImages) {
          resizeImage(&*p, Size, Offset);
       }
    }

#else
    QImage newImage(QSize(NewWidth ,NewHeight), QImage::Format_ARGB32);
    newImage.fill(TransparentColor);
    QPainter painter(&newImage);
    painter.drawImage(QPoint(OffsetX, OffsetY), image);
    image = newImage;
#endif
    // Now adjust all postits
    for (auto &&Picture: PostIts) {
       Picture.Position += QPoint(OffsetX, OffsetY);
       Picture.Box.Move(PositionClass(OffsetX, OffsetY));

    }

}



//! [19]
void DatabaseClass::resizeImage(QImage *image, const QSize &newSize, QPoint Offset)
//! [19] //! [20]
{
    if (image->size() == newSize && Offset == QPoint(0,0))
        return;

    QImage newImage(newSize, QImage::Format_ARGB32);
    newImage.fill(TransparentColor);
    QPainter painter(&newImage);
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(Offset, *image);
    *image = newImage;
}

bool DatabaseClass::SaveImage(const QString &fileName, const char *fileFormat)
{
   QImage ImageToSave(image.size(), QImage::Format_ARGB32);
   QPainter painter(&ImageToSave);
   PaintVisibleDrawing(painter, image.rect(), {0,0}, BackgroundImagesOrigin-Origin);

    if (ImageToSave.save(fileName, fileFormat)) {
        return true;
    } else {
        return false;
    }
}

//! [5] //! [6]
void DatabaseClass::setPenColor(const QColor &newColor)
{
    auto alpha = myPenColor.alpha();
    myPenColor = newColor;
    myPenColor.setAlpha(alpha);
    //myPenWidth = SelectedPenWidth;
}
//! [6]
//!

void DatabaseClass::UpdateGUI(int NumberOfLayers)
{
   std::vector<bool> Visibilities;
   for (int i = 0; i < NumberOfLayers; i++) {
            Visibilities.push_back( BackgroundImages[i].IsVisible());
   }
   Parent.UpdateGUI(Visibilities);
}

bool DatabaseClass::SetLayerVisibility(int SelectedLayer, bool Visibility)
{
   if (SelectedLayer < BackgroundImages.size()) {
      BackgroundImages[SelectedLayer].SetVisible(Visibility);
      return true;
   }
   return false;
}


bool DatabaseClass::SaveDatabase(const QString &fileName)
{
   // todo: read and save colors pen and background and backgroundimages

   QFile file(fileName);
   file.open(QIODevice::WriteOnly);
   QDataStream out(&file);

   // Write a header with a "magic number" and a version
   out << (quint32)0x139A1A7F;
   out << (qint32)110;

   out.setVersion(QDataStream::Qt_6_0);
   CompleteImage();

   // Write the data
   out << image;
   out << Origin;
   out << BackgroundImagesOrigin;
   // Now save all postits
   out << (qint32)(PostIts.size());
   for (auto &&Picture: PostIts) {
      out << Picture.Position;
      out << Picture.Image;
      out << Picture.Box;
   }
   out << (qint32)(BackgroundImages.size());
   for (auto &Picture: BackgroundImages) {
      out << Picture.IsVisible();
      out << *Picture;
   }
   return true;


}

bool DatabaseClass::LoadDatabase(const QString &fileName)
{
#if 1
   // todo: read and save colors pen and background and backgroundimages
   //Then read it in with:

   QFile file(fileName);
   file.open(QIODevice::ReadOnly);
   QDataStream in(&file);

   // Read and check the header
   quint32 magic = 0;
   in >> magic;
   if (magic != 0x139A1A7F)
       return ImportImage(fileName);

   // Read the version
   qint32 version = 0;
   in >> version;
   if (version < 90)
       return false; // too old
   if (version > 110)
       return false; // too new

   if (version <= 100) {
       in.setVersion(QDataStream::Qt_5_0);
   }
   else {
       in.setVersion(QDataStream::Qt_6_0);
   }

   // Write the data
   in >> image;
   in >> Origin;
   in >> BackgroundImagesOrigin;

   // Now read all postits
   qint32 NumberOfSavedPostits = 0;
   in >> NumberOfSavedPostits;
   QImage NewImage;
   QPoint Position;
   BoundingBoxClass NewBox;
   PostIts.clear();
   for (int i = 0; i < NumberOfSavedPostits; i++) {
      in >> Position;
      in >> NewImage;
      in >> NewBox;
      PostIts.push_back(PostIt(NewImage, Position, NewBox));
   }
   qint32 NumberOfBackgroundLayers = 0;
   in >> NumberOfBackgroundLayers;
   BackgroundImages.clear();
   bool Visible;
   emit(NumberOfLayerChanged(NumberOfBackgroundLayers));
   for (int i = 0; i < NumberOfBackgroundLayers; i++) {
      in >> Visible;
      in >> NewImage;
      BackgroundImages.push_back(ImageDescriptor(std::make_unique<QImage>(NewImage), Visible));
      emit(SetVisibilityIndicatorOfLayer(i, Visible));
   }


   modified = false;
   LastDrawingValid = false;
   EraseLastDrawnObject = false;

   update();
   return true;
#endif
}


//! [2]
