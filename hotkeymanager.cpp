#include "pch.h"
#include "hotkeymanager.h"

// Example function if you are reloading keybinds from a config file
void ReloadHotkeys()
{
    // First clear out all of our hotkeys
    HotkeyManager::Instance()->Clear();

    // Your code that loads stuff from a file or whatever

    // This is needed when the function is clearing out the hotkeys, this sets the flag back to true so it doesn't fire off unwantedly.
    // If you change the key data for this it may not fire off the first time it's pressed but that's w/e, it'll reset once released and pressed again.
    HotkeyCallbacks_ReloadSettings = { ReloadHotkeys, nullptr, nullptr, true };

    // Re-register our hotkeys
    HotkeyManager::Instance()->Register(Hotkey_ReloadSettings, HotkeyCallbacks_ReloadSettings);
    HotkeyManager::Instance()->Register(Hotkey_ToggleConsole, HotkeyCallbacks_ToggleConsole);
    HotkeyManager::Instance()->Register(Hotkey_FullCallbacksExample, HotkeyCallbacks_FullCallbackExample);
}

void ToggleConsole()
{
    // Allocate console window, if false it means one already exists so free it.
    if (!AllocConsole())
        FreeConsole();
}

void MyKeyPressed()
{
    std::cout << "My key was pressed!" << std::endl;
}
void MyKeyReleased()
{
    std::cout << "My key was released!" << std::endl;
}

void MyKeyRepeat()
{
    std::cout < "My key is repeating (held down)" << std::endl;
}

void __fastcall hkBInputKey(BInputKey* This, EInputKeyEvent* InputKeyEvent)
{
    HotkeyManager::Instance()->ProcessInput(InputKeyEvent);
    return oBInputKey(This, InputKeyEvent);
}