#include "sdk/sdk.h"
#include "hooks/hook.h"
#include "settings.h"
#include "esp.h"
#include "aimbot.h"
#include <iostream>
#include <unordered_map>

typedef void Func();
std::unordered_map<int, Func*> dispatchTable;
bool eject = false;
FILE* fConsole;

/**
 * @brief Prepares to eject our dll by unhooking and unpatching everything
 * we patched / hooked and freeing the console we allocated.
 *
 * If we did not do this step, some cheats will stay activated after ejecting and
 * the next time we inject and try to hook a function thats already hooked we will
 * crash.
 */
void PrepareEjection()
{
	for (hooks::Hook* hook : hooks::Hook::allHooks) { hook->Disable(); }
	for (hooks::Patcher* patch : hooks::Patcher::allPatches) { patch->Disable(); }

	if (fConsole) { fclose(fConsole); }
	Sleep(200);
	FreeConsole();
	SDK::sendAllMessage("\f0<Ejected successfully!>");

}

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
twglSwapBuffers wglSwapBuffersGateway;
/**
 * @brief Hooked to the wglSwapBuffers call of the game thread to execute our
 * cheat main code and allow us to draw with the games hdc.
 *
 * Checks whether we ejected and checks the dispatch table whether any key for a cheat
 * function was pressed, then executes that function.
 */
BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
	if (GetAsyncKeyState(VK_DELETE) & 1) { eject = true; }

	for (const auto& pair : dispatchTable) {
		int key = pair.first;
		Func* func = pair.second;

		if (GetAsyncKeyState(key) & 1) { func(); }
	}

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) { aimbot::TargetClosest(); }
	if (settings::esp::enabled) { esp::DrawAllPlayers(); }
	return wglSwapBuffersGateway(hDc);
}

/**
 * @brief Populates our dispatch table with the key mapped to what cheat setting
 * should get toggled.
 */
void SetupDispatchTable()
{
	dispatchTable[VK_F1] = []() {
		settings::esp::enabled ^= 1;
		SDK::sendAllMessage(settings::esp::enabled ? "<ESP \f0[ON]\f5!>" : "<ESP \f3[OFF]\f5!>");
		};

	dispatchTable[VK_F2] = []() {
		settings::player::godMode ^= 1;
		SDK::sendAllMessage(settings::player::godMode ? "<Godmode \f0[ON]\f5!>" : "<Godmode \f3[OFF]\f5!>");
		hooks::health.Toggle();
		};

	dispatchTable[VK_F3] = []() {
		settings::weapon::alwaysHeadshot ^= 1;
		SDK::sendAllMessage(settings::weapon::alwaysHeadshot ? "<Headshots \f0[ON]\f5!>" : "<Headshots \f3[OFF]\f5!>");
		hooks::headshot.Toggle();
		};

	dispatchTable[VK_F4] = []() {
		settings::weapon::noRecoil ^= 1;
		SDK::sendAllMessage(settings::weapon::noRecoil ? "<No Recoil \f0[ON]\f5!>" : "<No Recoil \f3[OFF]\f5!>");
		hooks::noRecoil.Toggle();
		};

	dispatchTable[VK_F5] = []() {
		settings::weapon::rapidFire ^= 1;
		SDK::sendAllMessage(settings::weapon::rapidFire ? "<Rapid Fire \f0[ON]\f5!>" : "<Rapid Fire \f3[OFF]\f5!>");
		hooks::rapidFire.Toggle();
		};

	dispatchTable[VK_F6] = []() {
		settings::weapon::infiniteAmmo ^= 1;
		SDK::sendAllMessage(settings::weapon::infiniteAmmo ? "<Inf. Ammo \f0[ON]\f5!>" : "<Inf. Ammo \f3[OFF]\f5!>");
		hooks::ammo.Toggle();
		};

	dispatchTable[VK_F7] = []() {
		settings::player::bSpeed ^= 1;
		SDK::sendAllMessage(settings::player::bSpeed ? "<Speed \f0[ON]\f5!>" : "<Speed \f3[OFF]\f5!>");
		hooks::ToggleSpeed(settings::player::bSpeed);
		};

	dispatchTable[VK_F8] = []() {
		settings::aimbot::autoFire ^= 1;
		SDK::sendAllMessage(settings::aimbot::autoFire ? "<Auto Fire \f0[ON]\f5!>" : "<Auto Fire \f3[OFF]\f5!>");
		};

	dispatchTable[VK_F9] = []() {
		settings::aimbot::turretMode ^= 1;
		SDK::sendAllMessage(settings::aimbot::turretMode ? "<Turret Mode \f0[ON]\f5!>" : "<Turret Mode \f3[OFF]\f5!>");
		};

	dispatchTable[VK_HOME] = []() {
		settings::aimbot::wallhack ^= 1;
		SDK::sendAllMessage(settings::aimbot::wallhack ? "<Wallhack \f0[ON]\f5!>" : "<Wallhack \f3[OFF]\f5!>");
		};

	dispatchTable[VK_END] = []() {
		settings::esp::mapReveal ^= 1;
		SDK::sendAllMessage(settings::esp::mapReveal ? "<Map Reveal \f0[ON]\f5!>" : "<Map Reveal \f3[OFF]\f5!>");
		hooks::mapReveal.Toggle(), hooks::minimapReveal.Toggle();
		};
}

/**
 * @brief Allocates a console for debugging purposes.
 */
void SetupConsole()
{
	AllocConsole();
	freopen_s(&fConsole, "CONOUT$", "w", stdout);
	std::cout << "DLL injected!\n";
	SDK::sendAllMessage("\f0<Injected successfully!>");

}

/**
 * @brief The main thread of our hack, this doesnt do anything but hook the wglSwapBuffers game
 * thread and then wait for the eject flag to eject the cheat.
 */
DWORD WINAPI HackThread(HMODULE hModule)
{
	SetupConsole();
	SetupDispatchTable();

	// We use the wglSwapBuffers function to execute our cheat code in the game thread as it also
	// allows us to draw using the games hdc.
	auto swapBuffersHook = hooks::Hook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);
	swapBuffersHook.Enable();
	hooks::console.Enable();

	// Wait until the other thread tells us to eject
	while (!eject) { Sleep(100); }
	std::cout << "[+] Ejecting!" << std::endl;
	swapBuffersHook.Disable();
	PrepareEjection();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		HANDLE hThread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(HackThread), hModule, NULL, nullptr);
		if (!hThread) { return FALSE; }
		CloseHandle(hThread);
	}

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}