#include "tools/mem.h"
#include "sdk/sdk.h"
#include "settings.h"
#include "hooks/hook.h"
#include "data.h"
#include "sdk/player.h"
#include "esp.h"
#include "aimbot.h"
#include <windows.h>
#include <iostream>

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
twglSwapBuffers wglSwapBuffersGateway;
bool eject = false;

BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
	if (GetAsyncKeyState(VK_DELETE) & 1) {
		eject = true;
		if (settings::player::godMode) { hooks::health.Disable(); }
		if (settings::weapon::alwaysHeadshot) { hooks::headshot.Disable(); }
		if (settings::weapon::noRecoil) { hooks::noRecoil.Disable(); }
		if (settings::weapon::rapidFire) { hooks::rapidFire.Disable(); }
		if (settings::weapon::infiniteAmmo) { hooks::ammo.Disable(); }
		if (settings::player::bSpeed) { hooks::ToggleSpeed(false); }
		if (settings::esp::mapReveal) { hooks::mapReveal.Toggle(), hooks::minimapReveal.Toggle(); }
	}

	if (GetAsyncKeyState(VK_F1) & 1) {
		settings::esp::enabled ^= 1;
		SDK::sendAllMessage(settings::esp::enabled ? "<ESP \f0[ON]\f5!>" : "<ESP \f3[OFF]\f5!>");
	}

	if (GetAsyncKeyState(VK_F2) & 1) {
		settings::player::godMode ^= 1;
		SDK::sendAllMessage(settings::player::godMode ? "<Godmode \f0[ON]\f5!>" : "<Godmode \f3[OFF]\f5!>");
		hooks::health.Toggle();
	}

	if (GetAsyncKeyState(VK_F3) & 1) {
		settings::weapon::alwaysHeadshot ^= 1;
		SDK::sendAllMessage(settings::weapon::alwaysHeadshot ? "<Headshots \f0[ON]\f5!>" : "<Headshots \f3[OFF]\f5!>");
		hooks::headshot.Toggle();
	}

	if (GetAsyncKeyState(VK_F4) & 1) {
		settings::weapon::noRecoil ^= 1;
		SDK::sendAllMessage(settings::weapon::noRecoil ? "<No Recoil \f0[ON]\f5!>" : "<No Recoil \f3[OFF]\f5!>");
		hooks::noRecoil.Toggle();
	}

	if (GetAsyncKeyState(VK_F5) & 1) {
		settings::weapon::rapidFire ^= 1;
		SDK::sendAllMessage(settings::weapon::rapidFire ? "<Rapid Fire \f0[ON]\f5!>" : "<Rapid Fire \f3[OFF]\f5!>");
		hooks::rapidFire.Toggle();
	}

	if (GetAsyncKeyState(VK_F6) & 1) {
		settings::weapon::infiniteAmmo ^= 1;
		SDK::sendAllMessage(settings::weapon::infiniteAmmo ? "<Inf. Ammo \f0[ON]\f5!>" : "<Inf. Ammo \f3[OFF]\f5!>");
		hooks::ammo.Toggle();
	}

	if (GetAsyncKeyState(VK_F7) & 1) {
		settings::player::bSpeed ^= 1;
		SDK::sendAllMessage(settings::player::bSpeed ? "<Speed \f0[ON]\f5!>" : "<Speed \f3[OFF]\f5!>");
		hooks::ToggleSpeed(settings::player::bSpeed);
	}

	if (GetAsyncKeyState(VK_F8) & 1) {
		settings::aimbot::autoFire ^= 1;
		SDK::sendAllMessage(settings::aimbot::autoFire ? "<Auto Fire \f0[ON]\f5!>" : "<Auto Fire \f3[OFF]\f5!>");
	}

	if (GetAsyncKeyState(VK_F9) & 1) {
		settings::aimbot::turretMode ^= 1;
		SDK::sendAllMessage(settings::aimbot::turretMode ? "<Turret Mode \f0[ON]\f5!>" : "<Turret Mode \f3[OFF]\f5!>");
	}

	if (GetAsyncKeyState(VK_HOME) & 1) {
		settings::aimbot::wallhack ^= 1;
		SDK::sendAllMessage(settings::aimbot::wallhack ? "<Wallhack \f0[ON]\f5!>" : "<Wallhack \f3[OFF]\f5!>");
	}

	if (GetAsyncKeyState(VK_END) & 1) {
		settings::esp::mapReveal ^= 1;
		SDK::sendAllMessage(settings::esp::mapReveal ? "<Map Reveal \f0[ON]\f5!>" : "<Map Reveal \f3[OFF]\f5!>");
		hooks::mapReveal.Toggle(), hooks::minimapReveal.Toggle();
	}

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		aimbot::TargetClosest();
	}

	if (settings::esp::enabled) {
		esp::DrawAllPlayers();
	}

	return wglSwapBuffersGateway(hDc);
}

DWORD WINAPI HackThread(HMODULE hModule)
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "DLL injected!\n";
	SDK::sendAllMessage("\f0<Injected successfully!>");

	auto swapBuffersHook = hooks::Hook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);

	swapBuffersHook.Enable();
	hooks::console.Enable();

	while (!eject) { Sleep(100); }
	std::cout << "[+] Ejecting!" << std::endl;

	Sleep(100);
	swapBuffersHook.Disable();
	hooks::console.Disable();

	if (f) { fclose(f); }
	Sleep(200);
	FreeConsole();
	SDK::sendAllMessage("\f0<Ejected successfully!>");

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