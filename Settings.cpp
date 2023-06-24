#include "Settings.hpp"
#include "SettingsDialog.h"


void SettingClass::getSettings(TabDescriptor &Descriptor)
{
   AddSettingsEntry(Descriptor, Touchscaling);
   AddSettingsEntry(Descriptor, DirectSelectTimeout);
   AddSettingsEntry(Descriptor, GestureTimeout);
   AddSettingsEntry(Descriptor, CopyTimeout);
   AddSettingsEntry(Descriptor, SelectTimeout);
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
   Infos[&Touchscaling] = {"Touchscaling", "Lausanne: Mobilis 2 Zonen Badi Fleurie Wandern Uferweg"};
   Infos[&DirectSelectTimeout] = {"DirectSelectTimeout", "DirectSelectTimeout"};
   Infos[&GestureTimeout] = {"GestureTimeout", "GestureTimeout"};
   Infos[&CopyTimeout] = {"CopyTimeout", "CopyTimeout"};
   Infos[&SelectTimeout] = {"SelectTimeout", "SelectTimeout"};
   Infos[&PostItTimeout] = {"PostItTimeout", "PostItTimeout"};
   Infos[&MovePostitPauseTimeout] = {"MovePostitPauseTimeout", "MovePostitPauseTimeout"};
   Infos[&PostitCopyTimeout] = {"PostitCopyTimeout", "PostitCopyTimeout"};
   Infos[&PointerHoldon] = {"PointerHoldon", "PointerHoldon"};
   Infos[&EraserSize] = {"EraserSize", "EraserSize"};
   Infos[&SpongeSize] = {"SpongeSize", "SpongeSize"};

}

void SettingClass::SetDefaultValues()
{
   Touchscaling = 4.0;
   DirectSelectTimeout = 10.0;
   CopyTimeout = 500;
   GestureTimeout = 800;
   SelectTimeout = 500;
   PostItTimeout = 1000;
   MovePostitPauseTimeout = 500;
   PostitCopyTimeout = 1000;
   PointerHoldon = 250;
   EraserSize = 2;
   SpongeSize = 15;
}
