#ifndef ANIMATEDCURSORCLASS_H
#define ANIMATEDCURSORCLASS_H

#include <QPixmap>

class AnimatedCursorClass
{
   int HotSpotX;
   int HotSpotY;
   unsigned int NumberOfFramesToPlay;
   std::vector<QPixmap> AnimatedPointerCursor;
   int CurrentAnimatedPointerShape;

public:
   AnimatedCursorClass(size_t Width, size_t Height, size_t NumberOfFramesToPlay, QString FileName, int Hot_x = -1, int Hot_y = -1);
   const QPixmap &GetPictureForPercentage(unsigned int Percentage)
   {
      // + 50 for Rounding
      unsigned int Index = (Percentage * NumberOfFramesToPlay + 50u)/100u;
      if (Index >= NumberOfFramesToPlay) {
         Index = NumberOfFramesToPlay-1;
      }
      return (AnimatedPointerCursor[Index]);
   }
   int HotX() {return HotSpotX;}
   int HotY() {return HotSpotY;}
};

#endif // ANIMATEDCURSORCLASS_H
