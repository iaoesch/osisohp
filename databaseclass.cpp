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
#include "databaseclass.h"

#include "DebugStream.hpp"


static inline int ToInt(double d) {return static_cast<int>(d + 0.5);}


DatabaseClass::DatabaseClass(ScribbleArea &Parent, class SettingClass &MySettings)
   : Parent(Parent), Settings(MySettings),
     CurrentlyDrawnObject(TransparentColor, Settings)

{
   modified = false;
   AutosaveNeeded = false;
   ShowOverview = false;
   CutMode = true;

   ScalingFactor = 1.0;

   BackGroundColor = QColor(230,230, 200,255);
   AdjustMarkercolor();
   ScrollHintColor = QColor(0, 30, 0, 50);
   ScrollHintBorderColor = QColor(90, 0, 0, 50);

   DefaultBackGroundColor = BackGroundColor;
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
    DisplayOffset = {0,0};
    BackgroundImages.Clear();
    CurrentlyDrawnObject.CancelShape();
    update();
    return true;
}

void DatabaseClass::MakeSelectionFromLastDrawnObject(bool Cutout)
{
    CurrentSeelection.MakeSelectionFromLastDrawnObject(image, DisplayOffset, CurrentlyDrawnObject, Cutout);
   CurrentlyDrawnObject.CancelShape();
}

void DatabaseClass::CreeatePostitFromSelection()
{
   //std::cout << "new postit (" << PostIts.size() << ")" << std::flush;
   Postits.CreatePostitAndSelect(CurrentSeelection, DisplayOffset);
   SetModified();
}

void DatabaseClass::drawLineTo(const QPointF &endPoint, float Pressure)
{
   DEBUG_LOG << "Drawing Pressure: " << Pressure << std::endl;

    SetModified();
    update(CurrentlyDrawnObject.drawLineTo(endPoint, Pressure));
}

void DatabaseClass::EraseLineTo(const QPointF &endPoint, float Pressure)
{
    DEBUG_LOG << "Erasing ";
    SetModified();
    update(CurrentlyDrawnObject.EraseLineTo(endPoint, Pressure, BackGroundColor));
}

void DatabaseClass::StoredrawLineTo(const QPointF &endPoint, float Pressure)
{
    StoredLineSegments.push_back({endPoint, Pressure, false});
}

void DatabaseClass::StoreEraseLineTo(const QPointF &endPoint, float Pressure)
{
    StoredLineSegments.push_back({endPoint, Pressure, true});
}

bool DatabaseClass::DrawStoredSegments()
{
    for (auto &s: StoredLineSegments) {
      if (s.Erasing) {
          EraseLineTo(s.endPoint, s.Pressure);
      } else {
          drawLineTo(s.endPoint, s.Pressure);
      }
    }
    return !StoredLineSegments.empty();
}

void DatabaseClass::GetOffsetAndAdjustOrigin(QImage &Image, QPointF &Origin, QPoint &OriginAdjustement, QSize &Size)
{
   if (Origin.x() < 0) {
       Size.setWidth(ToInt(Image.size().width() - Origin.x()));
       OriginAdjustement.setX(ToInt(- Origin.x()));
       Origin.setX(0);
   } else if (Origin.x()+Parent.width() > Image.size().width()){
       Size.setWidth(ToInt(Origin.x()+Parent.width()));
   } else {
       Size.setWidth(Image.size().width());
   }
   if (Origin.y() < 0) {
       Size.setHeight(ToInt(Image.size().height() - Origin.y()));
       OriginAdjustement.setY(ToInt(- Origin.y()));
       Origin.setY(0);
   } else if (Origin.y()+Parent.height() > Image.size().height()){
       Size.setHeight(ToInt(Origin.y()+Parent.height()));
   } else {
       Size.setHeight(Image.size().height());
   }
}

void DatabaseClass::resizeScrolledImage()
{
    QPoint OffsetAdjustment;
    QSize Size;

    GetOffsetAndAdjustOrigin(image, DisplayOffset, OffsetAdjustment, Size);

    resizeImage(&image, Size, OffsetAdjustment);
   // BackgroundImages.resizeScrolledImage(Size, Offset, *this);
    BackgroundImages.MoveOrigin(OffsetAdjustment);

    // Now adjust all postits
    Postits.MoveAllPostits(OffsetAdjustment);
}


void DatabaseClass::resizeImage(QImage *image, const QSize &newSize, QPoint Offset)
{
    if (image->size() == newSize && Offset == QPoint(0,0))
        return;

    QImage newImage(newSize, QImage::Format_ARGB32);
    newImage.fill(TransparentColor);
    QPainter painter(&newImage);
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

   BackgroundImages.AddLayerTop(loadedImage, QPointF(0,0));
   UpdateGUIElements();
   return true;
}

void DatabaseClass::MoveImageToBackgroundLayer()
{
   TransferLastDrawnShape();
   BackgroundImages.AddLayerTop(image, QPointF(0,0));
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
   if (!BackgroundImages.CollapseAllVisibleLayersToTop(image)) {
      SetModified();
   }
   UpdateGUIElements();
}

void DatabaseClass::clearImage()
{
    image.fill(TransparentColor);
    SetModified();
    CurrentlyDrawnObject.CancelShape();
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
   QSizeF DestSize = ImageToPaste.size();
   TransferLastDrawnShape();

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


   QRectF Destination(DisplayOffset, DestSize);
   QSize RequiredSize = image.size().expandedTo(DestSize.toSize() + QSize(ToInt(DisplayOffset.x()), ToInt(DisplayOffset.y())));
   resizeImage(&image, RequiredSize);
   QPainter painter(&image);
   painter.drawImage(Destination, ImageToPaste);
   SetModified();
   update();
}

void DatabaseClass::CopyImageToClipboard()
{
   QImage ImageToSave(image.size(), QImage::Format_ARGB32);
   QPainter painter(&ImageToSave);
   PaintVisibleDrawing(painter, image.rect(), {0,0}, -DisplayOffset);

   QApplication::clipboard()->setImage(ImageToSave);
}

bool DatabaseClass::SaveImage(const QString &fileName, const char *fileFormat)
{
   QImage ImageToSave(image.size(), QImage::Format_ARGB32);
   QPainter painter(&ImageToSave);
   PaintVisibleDrawing(painter, image.rect(), {0,0}, -DisplayOffset);

   return ImageToSave.save(fileName, fileFormat);
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
      TransferLastDrawnShape();

      // Write the data
      out << image;
      out << DisplayOffset;

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
   in >> DisplayOffset;

   // Now read all postits
   Postits.Load(in);

   // Now read all planes
   std::vector<bool> Visibilities = BackgroundImages.Load(in);
   Parent.UpdateGUI(Visibilities);

   modified = false;
   AutosaveNeeded = false;
   CurrentlyDrawnObject.CancelShape();

   update();
   return true;
}





void DatabaseClass::PaintOverview(QPainter &p, QSize const &OutputSize)
{
   QImage Overview(image.size(), QImage::Format_ARGB32);
   QPainter painter(&Overview);
   PaintVisibleDrawing(painter, Overview.rect(), {0,0}, -DisplayOffset);
   painter.setPen(QPen(ScrollHintBorderColor, HintBorderPenWidth, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
   painter.setBrush(QBrush(ScrollHintColor));

   painter.drawRect(ToInt(DisplayOffset.x()), ToInt(DisplayOffset.y()), OutputSize.width(), OutputSize.height());
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

void DatabaseClass::PaintVisibleDrawing(QPainter &painter, QRect const &dirtyRect, QPointF const &Offset, QPointF const &BackgroundOffset_unused)
{
    // First draw the background
    painter.setPen(QPen(QColor(0,0,0,0), 1, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.setBrush(QBrush(BackGroundColor));

    painter.drawRect(dirtyRect);

   // BackgroundImages.DrawAllVisible(painter, dirtyRect, BackgroundOffset);
    BackgroundImages.DrawAllVisible(painter, dirtyRect, Offset);

    // In marker mode, last drawn objec belongs into background
    CurrentlyDrawnObject.DrawBackgroundPart(painter, dirtyRect);

    // Then draw our current image (Without the currently drawn object)
    CurrentlyDrawnObject.DrawForegroundPartAndModifiedImage(painter, image, Offset, dirtyRect);

    // Now draw the currently drawn object, in marker mode it was already drawn earlier in background

    // Now draw pasted object, if availlable
    PastingObject.Draw(painter, Offset);

    // Now draw all postits
    Postits.DrawAll(painter, Offset);

    // If we have something selected, draw it
    if (Parent.IsInSelectingState()) {
       CurrentSeelection.Draw(painter);
    }

    PastingObject.DrawRecentlyPasted(painter);
}

void DatabaseClass::DrawMovedSelection(const QPointF Offset)
{
    QPainter painter(&image);

    if (CurrentSeelection.DrawMovedSelection(painter, Offset+DisplayOffset)) {
        SetModified();
        update();
    }
};


void DatabaseClass::MakeSreenMoveHint()
{
    CurrentSeelection.MakeSreenMoveHint(image);
}


void DatabaseClass::FilllastDrawnShape()
{
    CurrentlyDrawnObject.FillLastDrawnShape(QPainter(&image), DisplayOffset);
   SetModified();
}

void DatabaseClass::TransferLastDrawnShape()
{
   QPainter painter(&image);

   if (CurrentlyDrawnObject.TransferLastDrawnShape(painter, DisplayOffset)) {
      SetModified();
      update();
   }
}

void DatabaseClass::ResizeAll(int width, int height)
{

   if (width > CurrentlyDrawnObject.Image().width() || height > CurrentlyDrawnObject.Image().height()) {
      int newWidth = qMax(width + 128, image.width());
      int newHeight = qMax(height + 128, image.height());
      resizeImage(&image, QSize(newWidth+ToInt(DisplayOffset.x()), newHeight+ToInt(DisplayOffset.y())));
      resizeImage(&CurrentlyDrawnObject.Image(), QSize(newWidth, newHeight));
      //BackgroundImages.Resize(newWidth, newHeight);

      update();
   }
}


void DatabaseClass::DrawLastDrawnShapeAndStartNewShape()
{
   QPainter painter(&image);
   if (CurrentlyDrawnObject.TransferLastDrawnShape(painter, DisplayOffset)) {
      SetModified();
      update();
   }
   CurrentlyDrawnObject.BeginNewShape(ButtonDownPosition);
   if (DrawStoredSegments()) {
      SetModified();
      update();
   }
}
void DatabaseClass::CancelShape()
{
   CurrentlyDrawnObject.CancelShape();
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
   TransferLastDrawnShape();
   PastingObject.SetImageToPaste(Image, Parent.size());
   update();
}

void DatabaseClass::DoPasteImage(PasteEvent Event)
{

   switch(Event) {
   case DatabaseClass::PasteTopLayer:
      BackgroundImages.AddLayerTop(PastingObject.CreateImage(image, *this, DisplayOffset), DisplayOffset);
      UpdateGUIElements();
      SetModified();
      break;

   case DatabaseClass::PasteBottomLayer:
      BackgroundImages.AddLayerBottom(PastingObject.CreateImage(image, *this, DisplayOffset), DisplayOffset);
      UpdateGUIElements();
      SetModified();
      break;
   case DatabaseClass::PasteDrawing:
      PastingObject.DrawIntoImage(image, *this, DisplayOffset);
      SetModified();
      break;

   case DatabaseClass::CancelPasting:
   case DatabaseClass::MakeBigger:
   case DatabaseClass::MakeSmaller:
   case DatabaseClass::MakeOriginalSize:
      // Ignore
      //PasteStatus = None;
      break;
   }
   update();
}

void DatabaseClass::CancelPasteImage()
{
   PastingObject.Cancel();
   update();
}

void DatabaseClass::ScaleImageToPaste(double ScalingFactor)
{
   PastingObject.Scale(ScalingFactor);
   update();
}




//! [2]
