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
#include <QApplication>
#include <QClipboard>
#include "scribblearea.h"

#include "DebugStream.hpp"

int DatabaseClass::PostIt::NextId = 0;

int ToInt(double d) {return static_cast<int>(d + 0.5);}


DatabaseClass::DatabaseClass(ScribbleArea &Parent, class SettingClass &MySettings)
   : Parent(Parent), Settings(MySettings)
{
   modified = false;
   AutosaveNeeded = false;
   LastDrawingValid = false;
   EraseLastDrawnObject = false;
   Frozen = false;
   ShowOverview = false;
   CutMode = true;

   PasteStatus = None;

   myPenWidth = 2;
   myEraserWidth = Settings.EraserSize;
   SelectedPenWidth = myPenWidth;
   myPenColor = Qt::blue;

   TransparentColor = QColor(255, 255, 255, 0);
   BackGroundColor = QColor(230,230, 200,255);
   AdjustMarkercolor();
   ScrollHintColor = QColor(0, 30, 0, 50);
   ScrollHintBorderColor = QColor(90, 0, 0, 50);
   SelectionHintColor = QColor(0, 30, 0, 50);
   SelectionHintBorderColor = QColor(0, 30, 0, 50);

   DefaultBackGroundColor = BackGroundColor;
   PostItBackgroundColor = QColor(100, 0, 0, 50);

   RecentlyPastedObjectValid = false;
   MarkerActive = false;
   ShowPostitsFrame = false;

   SelectedPostit.clear();
}

void DatabaseClass::update()
{
   /// @todo propagate
   Parent.update();
}

void DatabaseClass::update(const QRect &r)
{
   /// @todo propagate
   Parent.update(r);
}

bool DatabaseClass::ImportImage(const QString &fileName)
{
   QImage loadedImage;
   if (!loadedImage.load(fileName)) {
        return false;
    }
    QSize newSize = loadedImage.size().expandedTo(Parent.size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    AutosaveNeeded = false;
    Origin = {0,0};
    BackgroundImagesOrigin = {0,0};
    LastDrawingValid = false;
    EraseLastDrawnObject = false;
    BackgroundImages.clear();
    update();
    return true;
}

void DatabaseClass::MakeSelectionFromLastDrawnObject(bool Cutout)
{
   SelectedImagePart =  image.copy(LastPaintedObjectBoundingBox.QRectangle().translated(Origin.toPoint()));
   SelectedImageBoundingBox = LastPaintedObjectBoundingBox;

   HintSelectedImagePart = SelectedImagePart;
   HintSelectedImagePart.fill(qRgba(0, 0, 0, 0));
   DiscardSelection = false;

   if (Cutout == true) {
      QPainter painter2(&image);
      painter2.setPen(QPen(QColor(0, 0, 0, 0), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                           Qt::RoundJoin));
      painter2.setBrush(QBrush(QColor(0, 0, 0, 0)));
      painter2.setCompositionMode(QPainter::CompositionMode_Source);
      // LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
      painter2.drawPolygon(LastDrawnObjectPoints.translated(Origin));
   }
   QPainter painter(&HintSelectedImagePart);
   painter.setPen(QPen(SelectionHintBorderColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
   painter.setBrush(QBrush(SelectionHintColor));
   LastDrawnObjectPoints.translate(-SelectedImageBoundingBox.GetLeft(), -SelectedImageBoundingBox.GetTop());
   painter.drawPolygon(LastDrawnObjectPoints);
   QPainterPath Path;
   Path.addPolygon(LastDrawnObjectPoints);
   QImage MaskedSelectedImagePart = SelectedImagePart;
   MaskedSelectedImagePart.fill(qRgba(0, 0, 0, 0));
   QPainter painter3(&MaskedSelectedImagePart);
   painter3.setClipPath(Path);
   painter3.drawImage(QPoint(0,0), SelectedImagePart);
   SelectedImagePart = MaskedSelectedImagePart;
   SelectedImagePartPath = Path;
   LastDrawnObject.fill(qRgba(0, 0, 0, 0));
}

void DatabaseClass::CreeatePostitFromSelection()
{
   std::cout << "new postit (" << PostIts.size() << ")" << std::flush;

   BoundingBoxClass TranslatedBoundingBox (SelectedImageBoundingBox);
   TranslatedBoundingBox.Move(PositionClass(Origin.x(), Origin.y()));
   CreatePostit(HintSelectedImagePart, SelectedImagePart, Origin + SelectedCurrentPosition+SelectedOffset, TranslatedBoundingBox, SelectedImagePartPath);
   SelectedPostit.push_back({std::prev(PostIts.end()), PostIts.back().Position});
   SetModified();
}

void DatabaseClass::CreatePostit(QImage BackgroundImage, QImage Image, QPointF Position, BoundingBoxClass Box, QPainterPath Path)
{
   QImage NewPostit(BackgroundImage);
   // Here we could add a different background for postits
   QPainter painter(&NewPostit);
   painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
   painter.setBrush(QBrush(PostItBackgroundColor));
   painter.drawRect(Image.rect());
   painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
   painter.drawImage(0,0,Image);
   //BoundingBoxClass TranslatedBoundingBox (SelectedImageBoundingBox);
   //TranslatedBoundingBox.Move(PositionClass(Origin.x(), Origin.y()));
   PostIts.push_back(PostIt(NewPostit, Position, Box, Path));
}

double DatabaseClass::CalculatePenWidthQuadratic(double Pressure, int BaseWidth)
{
   constexpr double MaxPenScaling = 11.0;
   constexpr double MinPenScaling = 1.0;
   constexpr double MaxPenForce = 0.9;
   constexpr double MinPenForce = DatabaseClass::JitterPressureLimit;
   constexpr double dx = MaxPenForce*MaxPenForce - MinPenForce*MinPenForce;
   constexpr double dy = MaxPenScaling - MinPenScaling;

   return (BaseWidth * qMax(MinPenScaling, Pressure*Pressure*(dy/dx)+(MinPenScaling-(MinPenForce*MinPenForce*dy/dx))) + 0.5);
}

double DatabaseClass::CalculatePenWidthLinear(double Pressure, int BaseWidth)
{
   constexpr double MaxPenScaling = 11.0;
   constexpr double MinPenScaling = 1.0;
   constexpr double MaxPenForce = 0.9;
   constexpr double MinPenForce = DatabaseClass::JitterPressureLimit;
   constexpr double dx = MaxPenForce - MinPenForce;
   constexpr double dy = MaxPenScaling - MinPenScaling;

   return (BaseWidth * qMax(MinPenScaling, Pressure*(dy/dx)+(MinPenScaling-(MinPenForce*dy/dx))) + 0.5);
}

void DatabaseClass::drawLineTo(const QPointF &endPoint, double Pressure)
{
   DEBUG_LOG << "Drawing Pressure: " << Pressure << std::endl;

    double ModifiedPenWidth = CalculatePenWidthLinear(Pressure, myPenWidth);
//    double ModifiedPenWidth = CalculatePenWidthQuadratic(Pressure, myPenWidth);
//   int ModifiedPenWidth = myPenWidth * qMax(1.0, Pressure*Pressure*4);
    QPainter painter(&LastDrawnObject);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setPen(QPen(myPenColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(lastPointDrawn, endPoint);
    SetModified();

    int rad = (ModifiedPenWidth / 2) + 2;
    update(QRect(lastPointDrawn.toPoint(), endPoint.toPoint()).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    lastPointDrawn = endPoint;
    EraseLastDrawnObject = false;
}

void DatabaseClass::EraseLineTo(const QPointF &endPoint, double Pressure)
{
    DEBUG_LOG << "Erasing ";
    QPainter painter(&LastDrawnObject);
    int ModifiedPenWidth = static_cast<int>((myPenWidth+myEraserWidth)*3*(1.0 + Pressure*Pressure*10) + 0.5);
    painter.setPen(QPen(BackGroundColor, ModifiedPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
   // painter.setCompositionMode(QPainter::CompositionMode_Source);
    //painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.drawLine(lastPointDrawn, endPoint);
    SetModified();
    EraseLastDrawnObject = true;

    int rad = (ModifiedPenWidth / 2) + 2;
    update(QRect(lastPointDrawn.toPoint(), endPoint.toPoint()).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    lastPointDrawn = endPoint;
}

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
    SetModified();
    update();
}

void DatabaseClass::GetOffsetAndAdjustOrigin(QImage &Image, QPointF &Origin, QPoint &Offset, QSize &Size)
{
   if (Origin.x() < 0) {
       Size.setWidth(ToInt(Image.size().width() - Origin.x()));
       Offset.setX(ToInt(- Origin.x()));
       Origin.setX(0);
   } else if (Origin.x()+Parent.width() > Image.size().width()){
       Size.setWidth(ToInt(Origin.x()+Parent.width()));
   } else {
       Size.setWidth(Image.size().width());
   }
   if (Origin.y() < 0) {
       Size.setHeight(ToInt(Image.size().height() - Origin.y()));
       Offset.setY(ToInt(- Origin.y()));
       Origin.setY(0);
   } else if (Origin.y()+Parent.height() > Image.size().height()){
       Size.setHeight(ToInt(Origin.y()+Parent.height()));
   } else {
       Size.setHeight(Image.size().height());
   }
}

void DatabaseClass::resizeScrolledImage()
//! [19] //! [20]
{
   // int NewWidth;
   // int NewHeight;
   // int OffsetX = 0;
   // int OffsetY = 0;
    QPoint Offset;
    QSize Size;

    GetOffsetAndAdjustOrigin(image, Origin, Offset, Size);

    resizeImage(&image, Size, Offset);
    if (!Frozen && !BackgroundImages.empty()) {
       GetOffsetAndAdjustOrigin(*BackgroundImages[0], BackgroundImagesOrigin, Offset, Size);

       for (auto &p: BackgroundImages) {
          resizeImage(&*p, Size, Offset);
       }
    }

    // Now adjust all postits
    for (auto &&Picture: PostIts) {
       Picture.Position += Offset;
       Picture.Box.Move(PositionClass(Offset.x(), Offset.y()));
    }
}


void DatabaseClass::resizeImage(QImage *image, const QSize &newSize, QPoint Offset)
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

void DatabaseClass::setPenColor(const QColor &newColor)
{
    auto alpha = myPenColor.alpha();
    myPenColor = newColor;
    myPenColor.setAlpha(alpha);
    //myPenWidth = SelectedPenWidth;
}

void DatabaseClass::UpdateGUIElements(unsigned long NumberOfLayers)
{
   std::vector<bool> Visibilities;
   for (unsigned int i = 0; i < NumberOfLayers; i++) {
            Visibilities.push_back( BackgroundImages[i].IsVisible());
   }
   Parent.UpdateGUI(Visibilities);
}

bool DatabaseClass::SetLayerVisibility(unsigned int SelectedLayer, bool Visibility)
{
   if (SelectedLayer < BackgroundImages.size()) {
      BackgroundImages[SelectedLayer].SetVisible(Visibility);
      return true;
   }
   return false;
}

void DatabaseClass::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
    SelectedPenWidth = myPenWidth;
    if (MarkerActive) {
       ExtendPenWidthForMarker();
    }
}

bool DatabaseClass::ImportImageToBackgroundLayer(const QString &fileName)
{
   QImage loadedImage;
   if (!loadedImage.load(fileName)) {
       return false;
   }

      // QSize newSize = loadedImage.size().expandedTo(Parent.size());
      // resizeImage(&loadedImage, newSize);

   BackgroundImages.push_back(std::make_unique<QImage>(loadedImage));
   UpdateGUIElements(BackgroundImages.size());
}

void DatabaseClass::MoveImageToBackgroundLayer()
{
   CompleteImage();
   BackgroundImages.push_back(std::make_unique<QImage>(image));
   clearImage();
   UpdateGUIElements(BackgroundImages.size());
}

void DatabaseClass::MoveTopBackgroundLayerToImage()
{
   if (!BackgroundImages.empty()) {
      // create empty image
      QImage newImage(image.size(), QImage::Format_ARGB32);
      newImage.fill(TransparentColor);
      QPainter painter(&newImage);

      // draw top bg layer onto it
      painter.drawImage((BackgroundImagesOrigin-Origin).toPoint(), *BackgroundImages.back());
      BackgroundImages.pop_back();

      // Then draw our current image (Without the currently drawn object)
      painter.drawImage(QPoint(0,0), image);

      //Set combined image as new image
      image = newImage;
      SetModified();
   }
   UpdateGUIElements(BackgroundImages.size());

}

void DatabaseClass::CollapseBackgroundLayers()
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
      SetModified();
   }
   UpdateGUIElements(BackgroundImages.size());
}

void DatabaseClass::CollapseAllVisibleLayersToTop()
{
   if (!BackgroundImages.empty()) {
      QImage newImage(image.size(), QImage::Format_ARGB32);
      newImage.fill(TransparentColor);
      QPainter painter(&newImage);
      auto it = BackgroundImages.begin();
      while (it != BackgroundImages.end()) {
         if (it->IsVisible()) {
            painter.drawImage(BackgroundImagesOrigin-Origin, **it);
            it = BackgroundImages.erase(it);
         } else {
            it++;
         }
      }
      painter.drawImage(QPoint(0,0), image);
      image = newImage;
      SetModified();
   }
   UpdateGUIElements(BackgroundImages.size());
}

void DatabaseClass::clearImage()
{
    image.fill(TransparentColor);
    SetModified();
    LastDrawingValid = false;
    LastDrawnObjectPoints.clear();
    LastDrawnObject.fill(TransparentColor);

    update();
}

void DatabaseClass::ToggleShowOverview(bool Mode)
{
   ShowOverview = Mode;
   Parent.UpdateShowOverviewChanged(Mode);
   update();
}


void DatabaseClass::PasteImage(QImage ImageToPaste)
{
#if 0
   CompleteImage();
   ControllingStateMachine
#else
   QSizeF DestSize = ImageToPaste.size();
   CompleteImage();

   bool ShrinkToFit = true;
   if (ShrinkToFit) {
   if (DestSize.width() > Parent.width()) {
      DestSize.setWidth(Parent.width());
      DestSize.setHeight(DestSize.height()*Parent.width()/ImageToPaste.width());
   }
   if (DestSize.height() > Parent.height()) {
      DestSize.setHeight(Parent.height());
      DestSize.setWidth(DestSize.width()*Parent.height()/ImageToPaste.height());
   }
   }


   QRectF Destination(Origin, DestSize);
   QSize RequiredSize = image.size().expandedTo(DestSize.toSize() + QSize(ToInt(Origin.x()), ToInt(Origin.y())));
    resizeImage(&image, RequiredSize);
    QPainter painter(&image);
    painter.drawImage(Destination, ImageToPaste);
    SetModified();
    update();
#endif
}

void DatabaseClass::CopyImageToClipboard()
{
   QImage ImageToSave(image.size(), QImage::Format_ARGB32);
   QPainter painter(&ImageToSave);
   PaintVisibleDrawing(painter, image.rect(), {0,0}, BackgroundImagesOrigin-Origin);

   QApplication::clipboard()->setImage(ImageToSave);
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

QString DatabaseClass::GetAutoSaveName()
{
   return AutosaveName;
}


void DatabaseClass::AutoSaveDatabase()
{
   std::cout << "Autosaving " << std::endl;
   if (AutosaveNeeded) {
      QString AutoSavefileName = GetAutoSaveName();
      std::cout << "saving to " << AutoSavefileName.toStdString() << std::endl;
      if (SaveDatabase(AutoSavefileName)) {
         AutosaveNeeded = false;
         // Und clear of autosave from SaveDataBasae()
         modified = true;
      }
   }
}


bool DatabaseClass::SaveDatabase(const QString &fileName)
{
   // todo: read and save colors pen and background and backgroundimages

   QFile file(fileName);
   if (file.open(QIODevice::WriteOnly)) {
      QDataStream out(&file);

      // Write a header with a "magic number" and a version
      out << static_cast<quint32>(0x139A1A7F);
      out << static_cast<quint32>(110);

      out.setVersion(QDataStream::Qt_6_0);
      CompleteImage();

      // Write the data
      out << image;
      out << Origin;
      out << BackgroundImagesOrigin;
      // Now save all postits
      out << static_cast<quint32>(PostIts.size());
      for (auto &&Picture: PostIts) {
         out << Picture.Position;
         out << Picture.Image;
         out << Picture.Box;
         out << Picture.BorderPath;
      }
      out << static_cast<quint32>(BackgroundImages.size());
      for (auto &Picture: BackgroundImages) {
         out << Picture.IsVisible();
         out << *Picture;
      }
      modified = false;
      AutosaveNeeded = false;
      return true;
   } else {
      return false;
   }
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
   QPainterPath BorderPath;
   PostIts.clear();
   for (int i = 0; i < NumberOfSavedPostits; i++) {
      in >> Position;
      in >> NewImage;
      in >> NewBox;
      in >> BorderPath;
      PostIts.push_back(PostIt(NewImage, Position, NewBox, BorderPath));
   }
   qint32 NumberOfBackgroundLayers = 0;
   in >> NumberOfBackgroundLayers;
   BackgroundImages.clear();
   bool Visible;
   std::vector<bool> Visibilities;
   //emit(NumberOfLayerChanged(NumberOfBackgroundLayers));
   for (int i = 0; i < NumberOfBackgroundLayers; i++) {
      in >> Visible;
      in >> NewImage;
      BackgroundImages.push_back(ImageDescriptor(std::make_unique<QImage>(NewImage), Visible));
      Visibilities.push_back(Visible);
      //emit(SetVisibilityIndicatorOfLayer(i, Visible));
   }
   Parent.UpdateGUI(Visibilities);

   modified = false;
   AutosaveNeeded = false;
   LastDrawingValid = false;
   EraseLastDrawnObject = false;

   update();
   return true;
#endif
}

bool DatabaseClass::FindSelectedPostIts(QPointF Position, SelectMode Mode)
{
   bool Found = false;
   SelectedPostit.clear();
   Position += Origin;
#if 0
   std::list<PostIt>::iterator Start = PostIts.begin();
   auto End = PostIts.end();
   if (Mode == Last) {
      Start = PostIts.rbegin();
      End = PostIts.rend();
   }
#endif

   for (auto PostIt = PostIts.begin(); PostIt != PostIts.end(); PostIt++) {
     if (PostIt->Box.IsInside(PositionClass(Position.x(), Position.y()))) {
        SelectedPostit.push_back({PostIt, PostIt->Position});
        Found = true;
        if (Mode == First) {
           // Only select first (Bottom one)
           return true;
        }
        // Select last not implemented yet (Probably use reverse iterator for this case)
     }
   }
   return Found;
}


bool DatabaseClass::IsInsideAnyPostIt(QPointF Position)
{
   Position += Origin;
   for (auto &&PostIt: PostIts) {
     if (PostIt.Box.IsInside(PositionClass(Position.x(), Position.y()))) {
        return true;
     }
   }
   return false;
}


void DatabaseClass::PaintOverview(QPainter &p, QSize const &OutputSize)
{
   QImage Overview(image.size(), QImage::Format_ARGB32);
   QPainter painter(&Overview);
   PaintVisibleDrawing(painter, Overview.rect(), {0,0}, BackgroundImagesOrigin-Origin);
   painter.setPen(QPen(ScrollHintBorderColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
   painter.setBrush(QBrush(ScrollHintColor));

   painter.drawRect(ToInt(Origin.x()), ToInt(Origin.y()), OutputSize.width(), OutputSize.height());
   p.drawImage(QPointF(0,0), Overview.scaled(OutputSize, Qt::KeepAspectRatio));
}

QPointF DatabaseClass::TranslateCoordinateOffsetFromOverview(QPointF Coordinates)
{
    double ScalingFactorX = image.width() / static_cast<double>(Parent.width());
    double ScalingFactorY = image.height() / static_cast<double>(Parent.height());
    double ScalingFactor;
    if (ScalingFactorY > ScalingFactorX) {
       ScalingFactor = ScalingFactorX;
    } else {
       ScalingFactor = ScalingFactorY;
    }
    QPointF UpScaledCoordinates = Coordinates * ScalingFactor;
    UpScaledCoordinates -= QPointF(Parent.width()/2, Parent.height()/2);
    if (UpScaledCoordinates.x() < 0) {
       UpScaledCoordinates.setX(0);
    } else if (UpScaledCoordinates.x() > (image.width() - Parent.width()/2)) {
       UpScaledCoordinates.setX(image.width() - Parent.width()/2);
    }
    if (UpScaledCoordinates.y() < 0) {
       UpScaledCoordinates.setY(0);
    } else if (UpScaledCoordinates.y() > (image.height() - Parent.height()/2)) {
       UpScaledCoordinates.setY(image.height() - Parent.height()/2);
    }
    return UpScaledCoordinates;
}

void DatabaseClass::PaintVisibleDrawing(QPainter &painter, QRect const &dirtyRect, QPointF const &Origin, QPointF const &BackgroundImagesOrigin)
{
    //QPainter painter(this);

    // First draw the background
    //QRect dirtyRect = event->rect();
    painter.setPen(QPen(QColor(0,0,0,0), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.setBrush(QBrush(BackGroundColor));
    //painter.setBrush(QBrush(QColor(50,0,0,100)));

    painter.drawRect(dirtyRect);

    for (auto &p: BackgroundImages) {
       if (p.IsVisible()) {
          painter.drawImage(dirtyRect, *p, dirtyRect.translated(BackgroundImagesOrigin.toPoint()));
       }
    }

    // In marker mode, last drawn objec belongs into background
    if (MarkerActive) {
        painter.drawImage(dirtyRect, LastDrawnObject, dirtyRect);
    }

    // Then draw our current image (Without the currently drawn object)
    if (EraseLastDrawnObject) {
       QImage ModifiedImage(image);
       QPainter MIpainter(&ModifiedImage);
       MIpainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
       MIpainter.drawImage(Origin, LastDrawnObject);
       painter.drawImage(dirtyRect, ModifiedImage, dirtyRect.translated(Origin.toPoint()));

    } else {
      painter.drawImage(dirtyRect, image, dirtyRect.translated(Origin.toPoint()));
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    }
#if 0
    // Probably nonsense, as widget cannot be transparent ???
    if (EraseLastDrawnObject) {

       painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    }
#endif
    // Now draw the currently drawn object, in marker mode it was already drawn earlier in background
    if (!MarkerActive && !EraseLastDrawnObject) {
       painter.drawImage(dirtyRect, LastDrawnObject, dirtyRect);
    }
#if 0
    if (EraseLastDrawnObject) {
       painter.setCompositionMode(QPainter::CompositionMode_SourceOut);
    }
#endif
    // Now draw pasted object, if availlable
    if (PasteStatus != None) {
       QSizeF DestSize = ImageToPaste.size();
       DestSize *= ScalingFactorOfImageToPaste;
       QRectF Destination(Origin, DestSize);
       painter.drawImage(Destination, ImageToPaste);
    }

    // Now draw all postits
    for (auto &&Picture: PostIts) {
       painter.drawImage(Picture.Position-Origin, Picture.Image);
       if (ShowPostitsFrame == true) {
          painter.setBrush(QBrush(Qt::NoBrush));
          painter.setPen(Markercolor);
          painter.drawRect(Picture.Box.QRectangle().translated(-Origin.toPoint()));
          painter.drawText(Picture.Box.QRectangle().translated(-Origin.toPoint()), QString::number(Picture.Id));
       }
    }

    // If we have something selected, draw it
    if ((Parent.IsInSelectingState()) && (DiscardSelection == false)) {
       painter.drawImage(SelectedCurrentPosition+SelectedOffset, HintSelectedImagePart);
       painter.drawImage(SelectedCurrentPosition+SelectedOffset, SelectedImagePart);
    }

    if (RecentlyPastedObjectValid == true) {
        painter.drawImage(RecentlyPastedObjectPosition, RecentlyPastedObject);
    }
}

void DatabaseClass::DrawMovedSelection(const QPointF Offset)
{
    QPainter painter(&image);

    painter.drawImage(SelectedOffset+Offset+Origin, SelectedImagePart);
    SetModified();
    update();
};


void DatabaseClass::MakeSreenMoveHint()
{
   SelectedImagePart =  image.copy();
   HintSelectedImagePart = SelectedImagePart;
 //  HintSelectedImagePart.fill(qRgba(200, 200, 200, 140));
  HintSelectedImagePart.fill(qRgba(0, 0, 0, 40));
}


void DatabaseClass::FilllastDrawnShape()
{
   QPainter painter2(&image);
   painter2.setPen(QPen(QColor(0, 0, 0, 0), myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
   painter2.setBrush(QBrush(myPenColor));
   painter2.setCompositionMode(QPainter::CompositionMode_Source);
   // LastDrawnObjectPoints.translate(-LastPaintedObjectBoundingBox.GetTop(), -LastPaintedObjectBoundingBox.GetLeft());
   painter2.drawPolygon(LastDrawnObjectPoints.translated(Origin));
   SetModified();
}

void DatabaseClass::CompleteImage()
{
   if (LastDrawingValid) {
      DrawLastDrawnPicture();
      LastDrawingValid = false;
      LastDrawnObjectPoints.clear();
   }
}

void DatabaseClass::ResizeAll(int width, int height)
{

   if (width > LastDrawnObject.width() || height > LastDrawnObject.height()) {
      int newWidth = qMax(width + 128, image.width());
      int newHeight = qMax(height + 128, image.height());
      resizeImage(&image, QSize(newWidth+ToInt(Origin.x()), newHeight+ToInt(Origin.y())));
      resizeImage(&LastDrawnObject, QSize(newWidth, newHeight));
      if (!Frozen) {
         for (auto &p: BackgroundImages) {
            resizeImage(&*p, QSize(newWidth+ToInt(BackgroundImagesOrigin.x()), ToInt(newHeight+BackgroundImagesOrigin.y())));
         }
      }
      update();
   }
}


void DatabaseClass::FlushLastDrawnPicture()
{
   if (LastDrawingValid) {
      DrawLastDrawnPicture();

      LastDrawingValid = false;
      LastDrawnObjectPoints.clear();
      LastDrawnObjectPoints.append(lastPointDrawn);
   }
}
void DatabaseClass::ClearLastDrawnPicture()
{
      LastDrawnObject.fill(TransparentColor);
      LastDrawingValid = false;
      LastDrawnObjectPoints.clear();
}

void DatabaseClass::MoveSelectedPostits(QPointF Position)
{
   for(auto &r: SelectedPostit) {
      //SelectedPostit->Position = Position;
      QPointF LastPosition = r.postit->Position;
      r.postit->Position = r.StartPosition + (Position - ButtonDownPosition);

      r.postit->Box.Move(PositionClass(r.postit->Position.x()-LastPosition.x(), r.postit->Position.y()-LastPosition.y()));
  //  SelectedPostit->Position = SelectedPostit->Position  Origin + Position;
   }
}

void DatabaseClass::FinishMovingSelectedPostits(QPointF Position)
{
   for (auto &r: SelectedPostit) {
      QPointF LastPosition = r.postit->Position;
      r.postit->Position = r.StartPosition + (Position - ButtonDownPosition);

      r.postit->Box.Move(PositionClass(r.postit->Position.x()-LastPosition.x(), r.postit->Position.y()-LastPosition.y()));
      // Place moved postits on top of each other
      PostIts.splice( PostIts.end(), PostIts, r.postit);
   }
   SetModified();
}

void DatabaseClass::DuplicateSelectedPostits()
{
   for (auto &r: SelectedPostit) {
      PostIts.push_back(*r.postit);
   }
   SetModified();
}

void DatabaseClass::DeleteSelectedPostits()
{
   for (auto &r: SelectedPostit) {
      PostIts.erase(r.postit);
   }
   SetModified();
}

void DatabaseClass::SetImageToPaste(QImage Image)
{
   CompleteImage();
   PasteStatus = Drawing;
   ImageToPaste = Image;
   ScalingFactorOfImageToPaste = 1.0;
   QSizeF DestSize = ImageToPaste.size();

   bool ShrinkToFit = true;
   if (ShrinkToFit) {
      if (DestSize.width() > Parent.width()) {
         ScalingFactorOfImageToPaste = static_cast<double>(Parent.width())/ImageToPaste.width();
      }
      if (DestSize.height() > Parent.height()) {
         double ScalingFactorYOfImageToPaste = static_cast<double>(Parent.height())/ImageToPaste.height();
         if (ScalingFactorYOfImageToPaste < ScalingFactorOfImageToPaste) {
            ScalingFactorOfImageToPaste = ScalingFactorYOfImageToPaste;
         }
      }
   }
}

void DatabaseClass::DoPasteImage(PasteEvent Event)
{
   QSizeF DestSize = ImageToPaste.size();
   DestSize *= ScalingFactorOfImageToPaste;
   QRectF Destination(Origin, DestSize);
   QSize RequiredSize = image.size().expandedTo(DestSize.toSize() + QSize(ToInt(Origin.x()), ToInt(Origin.y())));
   resizeImage(&image, RequiredSize);
   // Should also resize all layers
   if (!Frozen && !BackgroundImages.empty()) {
      for (auto &p: BackgroundImages) {
         resizeImage(&*p, RequiredSize);
      }
   }
   //QPainter painter(&image);

   switch(Event) {
      case DatabaseClass::PasteTopLayer:
         {
            QImage NewImage(RequiredSize, QImage::Format_ARGB32);
            NewImage.fill(TransparentColor);
            QPainter painter(&NewImage);
            painter.drawImage(Destination, ImageToPaste);
            BackgroundImages.push_back(std::make_unique<QImage>(NewImage));
            UpdateGUIElements(BackgroundImages.size());
            SetModified();
         }
         break;
      case DatabaseClass::PasteBottomLayer:
         {
            QImage NewImage(RequiredSize, QImage::Format_ARGB32);
            NewImage.fill(TransparentColor);
            QPainter painter(&NewImage);
            painter.drawImage(Destination, ImageToPaste);
            BackgroundImages.push_front(std::make_unique<QImage>(NewImage));
            UpdateGUIElements(BackgroundImages.size());
            SetModified();
         }
         break;
      case DatabaseClass::PasteDrawing:
         {
            QPainter painter(&image);
            painter.drawImage(Destination, ImageToPaste);
            SetModified();
         }

         break;
      case DatabaseClass::CancelPasting:
      case DatabaseClass::MakeBigger:
      case DatabaseClass::MakeSmaller:
      case DatabaseClass::MakeOriginalSize:
         // Ignore
         break;
   }
   PasteStatus = None;

   update();
}

void DatabaseClass::CancelPasteImage()
{
   PasteStatus = None;
   update();
}

void DatabaseClass::ScaleImageToPaste(double ScalingFactor)
{
   if (ScalingFactor > 0.1) {
      ScalingFactorOfImageToPaste *= ScalingFactor;
   } else {
      ScalingFactorOfImageToPaste = 1.0;
   }
   update();
}


void DatabaseClass::ExtendBoundingboxAndShape(QPointF Position)
{
   LastDrawnObjectPoints.append(Position);
   CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));
}

void DatabaseClass::UpdateBoundingboxesForFinishedShape(QPointF Position)
{
   LastDrawnObjectPoints.append(Position);
   LastDrawingValid = true;
   CurrentPaintedObjectBoundingBox.AddPoint(PositionClass(Position.x(), Position.y()));
   LastPaintedObjectBoundingBox = CurrentPaintedObjectBoundingBox;
   CurrentPaintedObjectBoundingBox.Clear();
}


//! [2]
