#include "animatedcursorclass.h"

#include <QPainter>
#include <QWidget>

AnimatedCursorClass::AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QString FileName, int Hot_x, int Hot_y)
   : HotSpotX(Hot_x), HotSpotY(Hot_y)
{
   this->NumberOfFramesToPlay = NumberOfFramesToPlay;
   AnimatedPointerCursor.resize(NumberOfFramesToPlay);
   //QPixmap BasePicture = QPixmap(FileName).scaled(Width, Height);
   QPixmap IconPicture = QPixmap(FileName).scaled(Width, Height);
   QPixmap BasePicture(32, 32);
   BasePicture.fill(QColor(0,0,0,0));
   QPainter IconPainter(&BasePicture);
   IconPainter.drawPixmap(0, 0, IconPicture);

   for (size_t i = 0; i < NumberOfFramesToPlay; i++) {
      int TimerPosX = 16;
      int TimerPosY = 16;
      int TimerWidth = 10;
      int TimerHeight = 10;
      AnimatedPointerCursor[i] = BasePicture;
      QPainter IconPainter(&AnimatedPointerCursor[i]);
      IconPainter.setPen(Qt::white);
      IconPainter.setBrush(QBrush(Qt::white));
      IconPainter.drawEllipse(TimerPosX, TimerPosY, TimerWidth, TimerHeight);
      IconPainter.setPen(Qt::black);
      IconPainter.setBrush(QBrush(Qt::black));
      int StartAngle = 0*16;
      int SpanAngle = 16*360*i/NumberOfFramesToPlay;

      IconPainter.drawPie(TimerPosX+1, TimerPosY+1, TimerWidth-2, TimerHeight-2, StartAngle, SpanAngle);
      //IconPainter.drawEllipse(6,6,8,8);
   }
   CurrentAnimatedPointerShape = 0;
}



void CursorManager::SetCursor(CursorType Cursor)
{
   switch(Cursor) {
      case CursorManager::TimedPointerForCopying:
         Parent->setCursor(QCursor(Qt::DragCopyCursor));
         break;
      case CursorManager::MovingMultiplePostitPointer:
         break;
      case CursorManager::StandardPointer:
         Parent->setCursor(QCursor(Qt::ArrowCursor));
         break;
      case CursorManager::TimedPointerForScrolling:
         Parent->setCursor(QCursor(Qt::SizeAllCursor));
         break;
      case CursorManager::TimedPointerForCutting:
         break;

      case CursorManager::TimedPointerForSelectingSingle:
         break;
      case CursorManager::TimedPointerForSelectingMultiple:
         Parent->setCursor(QCursor(Qt::ClosedHandCursor));
         break;
      case CursorManager::SelectPossiblePointer:
         Parent->setCursor(QCursor(Qt::PointingHandCursor));
         break;
      case CursorManager::CutPossiblePointer:
         Parent->setCursor(QCursor(Qt::UpArrowCursor));
         break;
      case CursorManager::MovingCutoutPointer:
         Parent->setCursor(QCursor(Qt::DragMoveCursor));
         break;
      case CursorManager::DrawingPinter:
         Parent->setCursor(QCursor(Qt::ArrowCursor));
         break;
      case CursorManager::GoingToFillTimer:
         break;
      case CursorManager::FillingPointer:
         Parent->setCursor(QCursor(Qt::BusyCursor));
         break;
      case CursorManager::ScrollingPointer:
         Parent->setCursor(QCursor(Qt::SizeAllCursor));
         break;
      case CursorManager::MovingPostitPointer:
         Parent->setCursor(QCursor(Qt::ArrowCursor));
         break;
      case CursorManager::TimedPointerForCopyPostit:
         Parent->setCursor(QCursor(Qt::DragCopyCursor));
         break;
      case CursorManager::TimedPointerForDeletePostit:
         break;

   }
}

CursorManager::CursorManager(QWidget *p) :  QObject(p) ,Parent(p)
{
   connect(&AnimatedPointerTimer, &QTimer::timeout, this, &CursorManager::AnimatedPointerTimetick);
   AnimatedPointerTimer.setInterval(20);
   CurrentAnimatedPointerPercentage = 0;

}

void CursorManager::AnimatedPointerTimetick()
{

   CurrentAnimatedPointerPercentage++;
   if (CurrentAnimatedPointerPercentage < 100) {
      Parent->setCursor(QCursor(CurrentAnimatedCursor->GetPictureForPercentage(CurrentAnimatedPointerPercentage), CurrentAnimatedCursor->HotX(), CurrentAnimatedCursor->HotY()));
   } else {
      AnimatedPointerTimer.stop();
      CurrentAnimatedPointerPercentage = 0;
   }
}

void CursorManager::StartAnimation()
{
   CurrentAnimatedPointerPercentage = 0;
   Parent->setCursor(QCursor(CurrentAnimatedCursor->GetPictureForPercentage(CurrentAnimatedPointerPercentage), CurrentAnimatedCursor->HotX(), CurrentAnimatedCursor->HotY()));
   AnimatedPointerTimer.start();
}

CursorManager::~CursorManager()
{

}

