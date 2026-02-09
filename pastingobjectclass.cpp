#include "pastingobjectclass.h"
#include "box.hpp"
#include "databaseclass.h"

#include <QImage>
#include <QPainter>

int ToInt(double d) {return static_cast<int>(d + 0.5);}


PastingObjectClass::PastingObjectClass() {
    RecentlyPastedObjectValid = false;
    PasteStatus = None;

}

void PastingObjectClass::Draw(QPainter &painter, QPointF Offset) {
    if (PasteStatus != None) {
        QSizeF DestSize = ImageToPaste.size();
        DestSize *= ScalingFactorOfImageToPaste;
        QRectF Destination(Offset-Offset, DestSize);
        painter.drawImage(Destination, ImageToPaste);
    }

}

void PastingObjectClass::DrawRecentlyPasted(QPainter &painter) {
    if (RecentlyPastedObjectValid == true) {
        painter.drawImage(RecentlyPastedObjectPosition, RecentlyPastedObject);
    }
}

void PastingObjectClass::SetImageToPaste(QImage Image, QSize ParentSize)
{
    //TransferLastDrawnShape();
    PasteStatus = Drawing;
    ImageToPaste = Image;
    ScalingFactorOfImageToPaste = 1.0;
    QSizeF DestSize = ImageToPaste.size();

    bool ShrinkToFit = true;
    if (ShrinkToFit) {
        if (DestSize.width() > ParentSize.width()) {
            ScalingFactorOfImageToPaste = static_cast<double>(ParentSize.width())/ImageToPaste.width();
        }
        if (DestSize.height() > ParentSize.height()) {
            double ScalingFactorYOfImageToPaste = static_cast<double>(ParentSize.height())/ImageToPaste.height();
            if (ScalingFactorYOfImageToPaste < ScalingFactorOfImageToPaste) {
                ScalingFactorOfImageToPaste = ScalingFactorYOfImageToPaste;
            }
        }
    }
}

auto PastingObjectClass::PrepareToPasteImage(QImage &image, DatabaseClass &UglyWorkaroundForRefactoring, QPointF Origin) {
    QSizeF DestSize = ImageToPaste.size();
    DestSize *= ScalingFactorOfImageToPaste;
    QRectF Destination(Origin, DestSize);
    QSize RequiredSize = image.size().expandedTo(DestSize.toSize() + QSize(ToInt(Origin.x()), ToInt(Origin.y())));
    DatabaseClass::resizeImage(&image, RequiredSize);
    // Should also resize all layers
    // ToDo: required size is based on origin, should probably be based on BackgroundImagesOrigin
    UglyWorkaroundForRefactoring.getBackgroundImages().Expand(RequiredSize);
    return std::make_tuple(RequiredSize, Destination);
}

QImage PastingObjectClass::CreateImage(QImage &image, DatabaseClass &UglyWorkaroundForRefactoring, QPointF Origin) {
    auto [RequiredSize, Destination] = PrepareToPasteImage(image, UglyWorkaroundForRefactoring, Origin);
    QImage NewImage(RequiredSize, QImage::Format_ARGB32);
    NewImage.fill(DatabaseClass::getTransparentColor());
    QPainter painter(&NewImage);
    painter.drawImage(Destination, ImageToPaste);
    PasteStatus = None;

    return NewImage;
}

void PastingObjectClass::DrawIntoImage(QImage &image, DatabaseClass &UglyWorkaroundForRefactoring, QPointF Origin) {
    auto [RequiredSize, Destination] = PrepareToPasteImage(image, UglyWorkaroundForRefactoring, Origin);
    QPainter painter(&image);
    painter.drawImage(Destination, ImageToPaste);
    PasteStatus = None;

}

void PastingObjectClass::Cancel()
{
    PasteStatus = None;
}

void PastingObjectClass::Scale(double ScalingFactor)
{
    if (ScalingFactor > 0.1) {
        ScalingFactorOfImageToPaste *= ScalingFactor;
    } else {
        ScalingFactorOfImageToPaste = 1.0;
    }
}
