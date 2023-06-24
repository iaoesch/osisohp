#ifndef ANIMATEDCURSORCLASS_H
#define ANIMATEDCURSORCLASS_H

#include <QPixmap>

class AnimatedCursorClass
{
   int HotSpotX;
   int HotSpotY;
   int NumberOfFramesToPlay;
   std::vector<QPixmap> AnimatedPointerCursor;
   int CurrentAnimatedPointerShape;

public:
   AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QString FileName, int Hot_x = -1, int Hot_y = -1);
   const QPixmap &GetPictureForPercentage(int Percentage)
   {
      // + 50 for Rounding
      return (AnimatedPointerCursor[(Percentage * NumberOfFramesToPlay + 50)/100]);
   }
};

#endif // ANIMATEDCURSORCLASS_H
