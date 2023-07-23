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


int ToInt(double d) {return static_cast<int>(d + 0.5);}




DatabaseClass::DatabaseClass(ScribbleArea &Parent, class SettingClass &MySettings)
   : Parent(Parent), Settings(MySettings)
{
   modified = false;
   AutosaveNeeded = false;
   ShowOverview = false;
   CutMode = true;

   PasteStatus = None;


   TransparentColor = QColor(255, 255, 255, 0);
   BackGroundColor = QColor(230,230, 200,255);
   AdjustMarkercolor();
   ScrollHintColor = QColor(0, 30, 0, 50);
   ScrollHintBorderColor = QColor(90, 0, 0, 50);
   SelectionHintColor = QColor(0, 30, 0, 50);
   SelectionHintBorderColor = QColor(0, 30, 0, 50);

   DefaultBackGroundColor = BackGroundColor;

   RecentlyPastedObjectValid = false;
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
    BackgroundImages.Clear();
    CurrentlyDrawnObject.Clear();
    update();
    return true;
}

void DatabaseClass::MakeSelectionFromLastDrawnObject(bool Cutout)
{
   SelectedImagePart =  image.copy(LastPaintedObjectBoundingBox.CurrentPaintedObjectBoundingBox.QRectangle().translated(Origin.toPoint()));
   SelectedImageBoundingBox = LastPaintedObjectBoundingBox.CurrentPaintedObjectBoundingBox;

   HintSelectedImagePart = SelectedImagePart;
   HintSelectedImagePart.fill(qRgba(0, 0, 0, 0));
   DiscardSelection = false;

   if (Cutout == true) {
      QPainter painter2(&image);
      CurrentlyDrawnObject.CutOut(painter2, Origin);
   }
   QPainter painter(&HintSelectedImagePart);
   painter.setPen(QPen(SelectionHintBorderColor, HintBorderPenWidth, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
   painter.setBrush(QBrush(SelectionHintColor));
   LastPaintedObjectBoundingBox.LastDrawnObjectPoints.translate(-SelectedImageBoundingBox.GetLeft(), -SelectedImageBoundingBox.GetTop());
   painter.drawPolygon(LastPaintedObjectBoundingBox.LastDrawnObjectPoints);
   QPainterPath Path;
   Path.addPolygon(LastPaintedObjectBoundingBox.LastDrawnObjectPoints);
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
   //std::cout << "new postit (" << PostIts.size() << ")" << std::flush;

   BoundingBoxClass TranslatedBoundingBox (SelectedImageBoundingBox);
   TranslatedBoundingBox.Move(PositionClass(Origin.x(), Origin.y()));
   Postits.CreatePostitAndSelect(HintSelectedImagePart, SelectedImagePart, Origin + SelectedCurrentPosition+SelectedOffset, TranslatedBoundingBox, SelectedImagePartPath);
   SetModified();
}


#if 0
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
#endif

void DatabaseClass::drawLineTo(const QPointF &endPoint, double Pressure)
{
   DEBUG_LOG << "Drawing Pressure: " << Pressure << std::endl;

    SetModified();
    update(CurrentlyDrawnObject.drawLineTo(endPoint, Pressure));
}

void DatabaseClass::EraseLineTo(const QPointF &endPoint, double Pressure)
{
    DEBUG_LOG << "Erasing ";
    SetModified();
    update(CurrentlyDrawnObject.EraseLineTo(endPoint, Pressure));
}

void DatabaseClass::DrawLastDrawnPicture()
{
    QPainter painter(&image);

    CurrentlyDrawnObject.DrawLastDrawnPicture(painter, Origin);
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
    BackgroundImages.resizeScrolledImage(Size, Offset, *this);

    // Now adjust all postits
    Postits.MoveAllPostits(Offset);
   // for (auto &&Picture: PostIts) {
   //    Picture.Position += Offset;
   //    Picture.Box.Move(PositionClass(Offset.x(), Offset.y()));
   // }
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
    CurrentlyDrawnObject.setPenColor(newColor);
}

void DatabaseClass::UpdateGUIElements()
{
   Parent.UpdateGUI(BackgroundImages.GetLayervisibilities());
}

bool DatabaseClass::SetLayerVisibility(unsigned int SelectedLayer, bool Visibility)
{
   return BackgroundImages.SetLayerVisibility(SelectedLayer, Visibility);
}

void DatabaseClass::setPenWidth(int newWidth)
{
    CurrentlyDrawnObject.setPenWidth(newWidth);
}

bool DatabaseClass::ImportImageToBackgroundLayer(const QString &fileName)
{
   QImage loadedImage;
   if (!loadedImage.load(fileName)) {
       return false;
   }

      // QSize newSize = loadedImage.size().expandedTo(Parent.size());
      // resizeImage(&loadedImage, newSize);

   BackgroundImages.AddLayerTop(loadedImage);
   UpdateGUIElements();
   return true;
}

void DatabaseClass::MoveImageToBackgroundLayer()
{
   CompleteImage();
   BackgroundImages.AddLayerTop(image);
   clearImage();
   UpdateGUIElements();
}

void DatabaseClass::MoveTopBackgroundLayerToImage()
{
   if (BackgroundImages.MoveTopBackgroundLayerToImage(image, *this)) {
      SetModified();
   }
   UpdateGUIElements();
}

void DatabaseClass::CollapseBackgroundLayers()
{
   if (BackgroundImages.CollapseBackgroundLayers()) {
      SetModified();
   }
   UpdateGUIElements();
}

void DatabaseClass::CollapseAllVisibleLayersToTop()
{
   if (!BackgroundImages.CollapseAllVisibleLayersToTop(image, *this)) {
      SetModified();
   }
   UpdateGUIElements();
}

void DatabaseClass::clearImage()
{
    image.fill(TransparentColor);
    SetModified();
    CurrentlyDrawnObject.Clear();
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
   PaintVisibleDrawing(painter, image.rect(), {0,0}, -Origin);

   QApplication::clipboard()->setImage(ImageToSave);
}

bool DatabaseClass::SaveImage(const QString &fileName, const char *fileFormat)
{
   QImage ImageToSave(image.size(), QImage::Format_ARGB32);
   QPainter painter(&ImageToSave);
   PaintVisibleDrawing(painter, image.rect(), {0,0}, -Origin);

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

      // Now save all postits
      Postits.Save(out);

      // Now save all planes
      BackgroundImages.Save(out);

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

   // Now read all postits
   Postits.Load(in);

   // Now read all planes
   std::vector<bool> Visibilities = BackgroundImages.Load(in);
   Parent.UpdateGUI(Visibilities);

   modified = false;
   AutosaveNeeded = false;
   CurrentlyDrawnObject.Clear();

   update();
   return true;
#endif
}





void DatabaseClass::PaintOverview(QPainter &p, QSize const &OutputSize)
{
   QImage Overview(image.size(), QImage::Format_ARGB32);
   QPainter painter(&Overview);
   PaintVisibleDrawing(painter, Overview.rect(), {0,0}, -Origin);
   painter.setPen(QPen(ScrollHintBorderColor, HintBorderPenWidth, Qt::SolidLine, Qt::RoundCap,
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

void DatabaseClass::PaintVisibleDrawing(QPainter &painter, QRect const &dirtyRect, QPointF const &Origin, QPointF const &BackgroundOffset)
{
    //QPainter painter(this);

    // First draw the background
    //QRect dirtyRect = event->rect();
    painter.setPen(QPen(QColor(0,0,0,0), 1, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.setBrush(QBrush(BackGroundColor));
    //painter.setBrush(QBrush(QColor(50,0,0,100)));

    painter.drawRect(dirtyRect);

   // for (auto &p: BackgroundImages) {
   //    if (p.IsVisible()) {

    BackgroundImages.DrawAllVisible(painter, dirtyRect, BackgroundOffset);
 //         painter.drawImage(dirtyRect, *p, dirtyRect.translated(BackgroundImagOrigin.toPoint()));
   //    }
   // }

    // In marker mode, last drawn objec belongs into background
    CurrentlyDrawnObject.DrawIfMarking(painter, dirtyRect);

    // Then draw our current image (Without the currently drawn object)
    CurrentlyDrawnObject.DrawIfErasing(painter, image, Origin, dirtyRect);

#if 0
    // Probably nonsense, as widget cannot be transparent ???
    if (EraseLastDrawnObject) {

       painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    }
#endif
    // Now draw the currently drawn object, in marker mode it was already drawn earlier in background
    CurrentlyDrawnObject.DrawNormal(painter, dirtyRect);
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
    Postits.DrawAll(painter, Origin);

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
   CurrentlyDrawnObject.FillLastDrawnShape(QPainter(&image), Origin);
   SetModified();
}

void DatabaseClass::CompleteImage()
{
   QPainter painter(&image);

   if (CurrentlyDrawnObject.CompleteImage(painter, Origin)) {
      SetModified();
      update();
   }
}

void DatabaseClass::ResizeAll(int width, int height)
{

   if (width > LastDrawnObject.width() || height > LastDrawnObject.height()) {
      int newWidth = qMax(width + 128, image.width());
      int newHeight = qMax(height + 128, image.height());
      resizeImage(&image, QSize(newWidth+ToInt(Origin.x()), newHeight+ToInt(Origin.y())));
      resizeImage(&LastDrawnObject, QSize(newWidth, newHeight));
      BackgroundImages.Resize(newWidth, newHeight, *this);

      update();
   }
}


void DatabaseClass::FlushLastDrawnPicture()
{
   QPainter painter(&image);
   if (CurrentlyDrawnObject.FlushLastDrawnPicture(painter, Origin)) {
      SetModified();
      update();
   }
}
void DatabaseClass::ClearLastDrawnPicture()
{
   CurrentlyDrawnObject.Clear();
}

void DatabaseClass::MoveSelectedPostits(QPointF Position)
{
   Postits.MoveSelectedPostits(Position - ButtonDownPosition);
}

void DatabaseClass::FinishMovingSelectedPostits(QPointF Position)
{
   Postits.FinishMovingSelectedPostits(Position - ButtonDownPosition);
   SetModified();
}

void DatabaseClass::DuplicateSelectedPostits()
{
   Postits.DuplicateSelectedPostits();
   SetModified();
}

void DatabaseClass::DeleteSelectedPostits()
{
   Postits.DeleteSelectedPostits();
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
   // ToDo: required size is based on origin, should probably be based on BackgroundImagesOrigin
   BackgroundImages.Expand(RequiredSize, *this);
   //QPainter painter(&image);

   switch(Event) {
      case DatabaseClass::PasteTopLayer:
         {
            QImage NewImage(RequiredSize, QImage::Format_ARGB32);
            NewImage.fill(TransparentColor);
            QPainter painter(&NewImage);
            painter.drawImage(Destination, ImageToPaste);
            BackgroundImages.AddLayerTop(NewImage);
            UpdateGUIElements();
            SetModified();
         }
         break;
      case DatabaseClass::PasteBottomLayer:
         {
            QImage NewImage(RequiredSize, QImage::Format_ARGB32);
            NewImage.fill(TransparentColor);
            QPainter painter(&NewImage);
            painter.drawImage(Destination, ImageToPaste);
            BackgroundImages.AddLayerBottom(NewImage);
            UpdateGUIElements();
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
   CurrentlyDrawnObject.ExtendBoundingboxAndShape(Position);
}

void DatabaseClass::UpdateBoundingboxesForFinishedShape(QPointF Position)
{
   LastPaintedObjectBoundingBox = CurrentlyDrawnObject.UpdateBoundingboxesForFinishedShape(Position);

}


//! [2]
