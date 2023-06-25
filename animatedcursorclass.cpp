#include "animatedcursorclass.h"

#include <QPainter>

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


