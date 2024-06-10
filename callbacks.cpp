#include "pch.h"
#include "pluginsdk.h"
#include "searchers.h"
#include "hotkeymanager.h"

bool  __cdecl init([[maybe_unused]] const Version version);
void __cdecl oep_notify([[maybe_unused]] const Version version);

extern "C" __declspec(dllexport) PluginInfo GPluginInfo = {
#ifdef NDEBUG
  .hide_from_peb = true,
  .erase_pe_header = true,
#endif
  .init = init,
  .oep_notify = oep_notify,
  .priority = 1, // Higher = more priority
  .target_apps = L"Client.exe;BNSR.exe"
};

/*
  CALLED FIRST
  Good use to setup configuration, pattern scanning and other general checks
  If something doesn't add up you can return false and oep_notify & engine_init will not execute
*/
bool  __cdecl init([[maybe_unused]] const Version version)
{
    DetourTransactionBegin();
	DetourUpdateThread(NtCurrentThread());

	if (const auto module = pe::get_module()) {
		uintptr_t handle = module->handle();
		const auto sections = module->segments();
		const auto& s1 = std::find_if(sections.begin(), sections.end(), [](const IMAGE_SECTION_HEADER& x) {
			return x.Characteristics & IMAGE_SCN_CNT_CODE;
			});
		const auto data = s1->as_bytes();

        LONG response;

        auto result = std::search(data.begin(), data.end(), pattern_searcher(xorstr_("0F B6 47 18 48 8D 4C 24 30 89 03")));
		if (result != data.end()) {
			oBInputKey = module->rva_to<std::remove_pointer_t<decltype(oBInputKey)>>((uintptr_t)&result[0] - 0x38 - handle);
			response = DetourAttach(&(PVOID&)oBInputKey, &hkBInputKey);

            // There was an issue trying to attach the detour
            if (response != NO_ERROR) {
                return false;
            }
		}

        auto result = DetourTransactionCommit();
        // Error applying detours
        if (result != 0)
            return false;
    }
    return true;
}

void __cdecl oep_notify([[maybe_unused]] const Version version)
{
    // Register our hotkeys
    HotkeyManager::Instance()->Register(Hotkey_ReloadSettings, HotkeyCallbacks_ReloadSettings);
    HotkeyManager::Instance()->Register(Hotkey_ToggleConsole, HotkeyCallbacks_ToggleConsole);
    HotkeyManager::Instance()->Register(Hotkey_FullCallbacksExample, HotkeyCallbacks_FullCallbackExample);
}