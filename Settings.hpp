#ifndef SETTINGS_CLASS_HPP
#define SETTINGS_CLASS_HPP
/*****************************************************************************/
/*  Header     : Settings                               Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : This class holds the settings      */
/*                     */
/*                                                                           */
/*  Methodes   : BoundingBoxClass()                                          */
/*              ~BoundingBoxClass()                                          */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 05.12.1999  IO Created                                      */
/*                                                                           */
/*  File       : Settiings.hpp                                                     */
/*                                                                           */
/*****************************************************************************/
/* MagicSoft                                                                 */
/*****************************************************************************/

/* imports */
#include <map>
#include <string>
#include "SettingsDialog.h"


/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class SettingClass {
   struct SettingInfo {
      std::string Name;
      std::string Helptext;
      EntityDescriptor::VariantType DefaultValue;
   };

   std::map<void *, SettingInfo> Infos;
public:
   double Touchscaling = 4.0;
   double DirectSelectTimeout = 10.0;
   double CopyTimeout = 500*4;
   double GestureTimeout = 800*4;
   double GoingToFillTimeout = 800*4;
   double CancelFillTimeout = 1600*4;
   double GoingToKillTimeout = 1600*4;
   double SelectTimeout = 500*4;
   double PostItTimeout = 1000*4;
   double MovePostitPauseTimeout = 500*4;
   double PostitCopyTimeout = 1000*4;
   double DeletePostItTimeout = 2000*4;
   double PointerHoldon = 250;
   int    EraserSize = 2;
   int    SpongeSize = 15;

   void getSettings(GroupDescriptor &Descriptor);
   SettingClass();
   void SetDefaultValues();
private:
   template<class U>
   void AddSettingsEntry(GroupDescriptor &Descriptor, U &Value) {
      Descriptor.AddEntry(Infos[&Value].Name, Infos[&Value].Helptext, Value, std::get<U>(Infos[&Value].DefaultValue));
   }

   template<class U>
   void InitInfoEntry(U *Value, std::string Name, std::string HelpText) {
      Infos[Value] = {Name, HelpText, *Value};
   }

};
/*****************************************************************************/
/*  End Header  : Settings                                           */
/*****************************************************************************/
#endif

