#pragma once
struct BInputKey {
	int Key;
	bool bCtrlPressed;
	bool bShiftPressed;
	bool bAltPressed;
	bool bDoubleClicked;
	bool bTpsModeKey;
};

enum class EngineKeyStateType {
	EKS_PRESSED = 0,
	EKS_RELEASED = 1,
	EKS_REPEAT = 2,
	EKS_DOUBLECLICK = 3,
	EKS_AXIS = 4
};

struct EInputKeyEvent {
	char padding[0x18];
	char _vKey;
	char padd_2[0x2];
	EngineKeyStateType KeyState;
	bool bCtrlPressed;
	bool bShiftPressed;
	bool bAltPressed;
};

void(__fastcall* oBInputKey)(BInputKey* thisptr, EInputKeyEvent* InputKeyEvent);

using HotkeyCallback = std::function<void()>;
struct Hotkey {
	char _vKey;
	bool _control;
	bool _shift;
	bool _alt;

	bool operator==(const Hotkey& src) const {
		return _vKey == src._vKey && _control == src._control && _shift == src._shift && _alt == src._alt;
	}
};

struct HotkeyHash {
	std::size_t operator()(const Hotkey& h) const {
		return std::hash<char>()(h._vKey) ^ std::hash<bool>()(h._control) ^ std::hash<bool>()(h._alt) ^ std::hash<bool>()(h._shift);
	}
};

struct HotkeyCallbacks {
	HotkeyCallback KeyPressed;
	HotkeyCallback KeyReleased;
	HotkeyCallback KeyRepeat;
	bool bBlockHotkey = false;
};

class HotkeyManager {
public:
	static HotkeyManager* Instance()
	{
		static HotkeyManager handler;
		return &handler;
	}

	void Register(BInputKey const& InputData, HotkeyCallbacks const& Callbacks)
	{
		Hotkey key(InputData.Key, InputData.bCtrlPressed, InputData.bAltPressed, InputData.bShiftPressed);
		hotkeys.insert(std::pair<const Hotkey, HotkeyCallbacks>(key, Callbacks));
	}

	void ProcessInput(EInputKeyEvent* const& InputData)
	{
		Hotkey key(InputData->_vKey, InputData->bCtrlPressed, InputData->bAltPressed, InputData->bShiftPressed);
		auto result = hotkeys.find(key);
		if (result != hotkeys.end()) {
			switch (InputData->KeyState)
			{
				case EngineKeyStateType::EKS_PRESSED:
					if (result->second.KeyPressed && !result->second.bBlockHotkey) {
						result->second.bBlockHotkey = true;
						result->second.KeyPressed();
					}
					break;
				case EngineKeyStateType::EKS_RELEASED:
					result->second.bBlockHotkey = false;
					if (result->second.KeyReleased) {
						result->second.KeyReleased();
					}
					break;
				case EngineKeyStateType::EKS_REPEAT:
					if (result->second.KeyRepeat)
						result->second.KeyRepeat();
					break;
				default:
					break;
			}
		}
	}

	void Clear()
	{
		hotkeys.clear(); // clear the map
	}

private:
	std::unordered_map<Hotkey, HotkeyCallbacks, HotkeyHash> hotkeys;
	bool bHotkeyInUse = false;

	HotkeyManager() {};
	HotkeyManager(HotkeyManager&) = delete;
	HotkeyManager(HotkeyManager&&) = delete;
	~HotkeyManager() = default;
	HotkeyManager& operator=(const HotkeyManager&) = delete;
	HotkeyManager& operator=(HotkeyManager&&) = delete;
};

//Predefining functions
void ReloadSettings();
void ToggleConsole();
void MyKeyPressed();
void MyKeyReleased();
void MyKeyRepeat();

/*
	Hotkey callbacks
	If you don't need a specific callback you can set nullptr
	
	Structure:
		void func: Function to call when key is in PRESSED state, will set flag below to true to prevent it from firing off multiple times (engine bug)
		void func: Function to call when key is in RELEASED state, will reset flag below to false
		void func: Function to call when key is in REPEAT state, unlike the two above this is not bound to the flag below and it will always fire given its in REPEAT state.
		bool: You typically don't need to touch this unless your function clears the manager (Like reloading hotkey settings from a config file as example)
*/
HotkeyCallbacks HotkeyCallbacks_ReloadSettings = { ReloadHotkeys, nullptr, nullptr };
HotkeyCallbacks HotkeyCallbacks_ToggleConsole = { ToggleConsole, nullptr, nullptr };
HotkeyCallbacks HotkeyCallbacks_FullCallbackExample = { MyKeyPressed, MyKeyPressed, MyKeyRepeat };

/*
	Hotkeys
	This is where you'll setup all of your key combos, if you're reading from a config
	you can set them later and just declare the variables first.
*/
// CTRL + ALT + P
BInputKey Hotkey_ReloadSettings = {
	0x50, // VKey
	true, // Control Key
	false, // Shift key
	true // Alt Key
};

// CTRL + ALT + Q
BInputKey Hotkey_ToggleConsole = { 0x51, true, false, true };

// CTRL + ALT + T
BInputKey Hotkey_FullCallbacksExample = { 0x54, true, false, true };