#include "Settings.hpp"
#include "SettingsDialog.h"


void SettingClass::getSettings(GroupDescriptor &Descriptor)
{
   AddSettingsEntry(Descriptor, Touchscaling);
   AddSettingsEntry(Descriptor, ThrowingSpeedLimit);

   AddSettingsEntry(Descriptor, GestureTrackerTimeout);
   AddSettingsEntry(Descriptor, DirectSelectTimeout);
   AddSettingsEntry(Descriptor, GestureTimeout);
   AddSettingsEntry(Descriptor, GoingToFillTimeout);
   AddSettingsEntry(Descriptor, CancelFillTimeout);
   AddSettingsEntry(Descriptor, GoingToKillTimeout);
   AddSettingsEntry(Descriptor, CopyTimeout);
   AddSettingsEntry(Descriptor, SelectTimeout);
   AddSettingsEntry(Descriptor, DeletePostItTimeout);
   AddSettingsEntry(Descriptor, PostItTimeout);
   AddSettingsEntry(Descriptor, MovePostitPauseTimeout);
   AddSettingsEntry(Descriptor, PostitCopyTimeout);
   AddSettingsEntry(Descriptor, PointerHoldon);
   AddSettingsEntry(Descriptor, EraserSize);
   AddSettingsEntry(Descriptor, SpongeSize);
#if 0
   Descriptor.AddEntry("Touchscaling", "Touchscaling", Touchscaling);
   Descriptor.AddEntry("DirectSelectTimeout", "DirectSelectTimeout", DirectSelectTimeout);
   Descriptor.AddEntry("GestureTimeout", "GestureTimeout", GestureTimeout);
   Descriptor.AddEntry("CopyTimeout", "CopyTimeout", CopyTimeout);
   Descriptor.AddEntry("SelectTimeout", "SelectTimeout", SelectTimeout);
   Descriptor.AddEntry("PostItTimeout", "PostItTimeout", PostItTimeout);
   Descriptor.AddEntry("MovePostitPauseTimeout", "MovePostitPauseTimeout", MovePostitPauseTimeout);
   Descriptor.AddEntry("PostitCopyTimeout", "PostitCopyTimeout", PostitCopyTimeout);
   Descriptor.AddEntry("PointerHoldon", "PointerHoldon", PointerHoldon);
   Descriptor.AddEntry("EraserSize", "EraserSize", EraserSize);
   Descriptor.AddEntry("SpongeSize", "SpongeSize", SpongeSize);
#endif
}
//Lausanne: Mobilis 2 Zonen Badi Fleurie Wandern Uferweg

SettingClass::SettingClass()
{
   SetDefaultValues();
   SetCurrentGroup("General");
   InitInfoEntry(&Touchscaling, "Touchscaling", "Lausanne: Mobilis 2 Zonen Badi Fleurie Wandern Uferweg");
   SetCurrentGroup("Gestures");
   InitInfoEntry(&GestureTrackerTimeout, "GestureTrackerTimeout", "GestureTrackerTimeout");
   InitInfoEntry(&ThrowingSpeedLimit, "ThrowingSpeedLimit", "ThrowingSpeedLimit");
   SetCurrentGroup("Timings");
   InitInfoEntry(&DirectSelectTimeout, "DirectSelectTimeout", "DirectSelectTimeout");
   InitInfoEntry(&GestureTimeout, "GestureTimeout", "GestureTimeout");
   InitInfoEntry(&GoingToFillTimeout, "GoingToFillTimeout", "GoingToFillTimeoutGoingToFillTimeout");
   InitInfoEntry(&CancelFillTimeout, "CancelFillTimeout", "CancelFillTimeout");
   InitInfoEntry(&GoingToKillTimeout, "GoingToKillTimeout", "GoingToKillTimeout");
   InitInfoEntry(&CopyTimeout, "CopyTimeout", "CopyTimeout");
   InitInfoEntry(&SelectTimeout, "SelectTimeout", "SelectTimeout");
   InitInfoEntry(&PostItTimeout, "PostItTimeout", "PostItTimeout");
   InitInfoEntry(&MovePostitPauseTimeout, "MovePostitPauseTimeout", "MovePostitPauseTimeout");
   InitInfoEntry(&PostitCopyTimeout, "PostitCopyTimeout", "PostitCopyTimeout");
   InitInfoEntry(&DeletePostItTimeout, "DeletePostItTimeout", "DeletePostItTimeout");
   InitInfoEntry(&PointerHoldon, "PointerHoldon", "PointerHoldon");
   SetCurrentGroup("General");

   InitInfoEntry(&EraserSize, "EraserSize", "EraserSize");
   InitInfoEntry(&SpongeSize, "SpongeSize", "SpongeSize");

}

void SettingClass::SetDefaultValues()
{
   constexpr int DebugFactor = 1;
   Touchscaling = 4.0;
   ThrowingSpeedLimit = 0.25;
   GestureTrackerTimeout = 500;
   DirectSelectTimeout = 10.0;
   CopyTimeout = 500 * DebugFactor;
   DeletePostItTimeout = 2000 * DebugFactor;
   GestureTimeout = 800 * DebugFactor;
   GoingToFillTimeout = 800*DebugFactor;
   CancelFillTimeout = 800*DebugFactor;
   GoingToKillTimeout = 1000*DebugFactor;
   SelectTimeout = 500 * DebugFactor;
   PostItTimeout = 1000 * DebugFactor;
   MovePostitPauseTimeout = 500 * DebugFactor;
   PostitCopyTimeout = 1000 * DebugFactor;
   PointerHoldon = 250;
   EraserSize = 2;
   SpongeSize = 15;
}

void SettingClass::SetCurrentGroup(std::string Group)
{
   auto it = GroupMapping.find(Group);
   if (it != GroupMapping.end()) {
      CurrentGroup = &Groups[it->second];
   } else {
      Groups.push_back(GroupDescriptor(Group));
      CurrentGroup = &Groups.back();
      GroupMapping[Group] = Groups.size()-1;
   }
}
