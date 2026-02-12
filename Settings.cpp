#include "Settings.hpp"
//#include "SettingsDialog.h"

#include <QDir>

#ifdef UseFlatSettings
#error "Old variant, just for reference, throw away soon"

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
#endif

SettingClass::SettingClass()
{
   SetDefaultValues();
   SetCurrentGroup("General");
   InitInfoEntry(&Touchscaling, "Touchscaling", "Lausanne: Mobilis 2 Zonen Badi Fleurie Wandern Uferweg");
   InitInfoEntry(&ScrollScaling, "ScrollScaling", "ScrollScaling");
   InitInfoEntry(&DrawScaling, "DrawScaling", "DrawScaling");

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
   InitInfoEntry(&AutoSavePath, "AutoSavePath", "AutoSavePath");
   InitInfoEntry(&DefaultAutoSaveIntervall, "DefaultAutoSaveIntervall", "DefaultAutoSaveIntervall");

}

void SettingClass::SetDefaultValues()
{
   constexpr int DebugFactor = 1;
   Touchscaling = 4.0;
   ScrollScaling = 5.0;
   DrawScaling = 1.0;
   ThrowingSpeedLimit = 0.25;
   GestureTrackerTimeout = 500;
   DirectSelectTimeout = 10.0;
   CopyTimeout = 500 * DebugFactor;
   DeletePostItTimeout = 2000 * DebugFactor;
   GestureTimeout = 2000 * DebugFactor;
   GoingToFillTimeout = 800*DebugFactor;
   CancelFillTimeout = 800*DebugFactor;
   GoingToKillTimeout = 1000*DebugFactor;
   SelectTimeout = 2000 * DebugFactor;
   PostItTimeout = 1000 * DebugFactor;
   MovePostitPauseTimeout = 500 * DebugFactor;
   PostitCopyTimeout = 1000 * DebugFactor;
   PointerHoldon = 250;
   EraserSize = 2;
   SpongeSize = 50;

   DefaultAutoSaveIntervall = 1;
   AutoSavePath = QDir::homePath().toStdString();
}

GroupDescriptor::CallbackType SettingClass::RegisterCallback(void *Value, GroupDescriptor::CallbackType Callback)
{
   if (Infos[Value].Descriptor != nullptr) {
      return Infos[Value].Descriptor->RegisterCallback(Callback);
   } else {
      // Should not happen, probably good idea to throw here..
      return Callback;
   }
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
