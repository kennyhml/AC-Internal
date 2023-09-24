#include "tools/mem.h"
#include "player.h"
#include "settings.h"
#include <windows.h>
#include "hooks/hook.h"

#include <iostream>
#include <TlHelp32.h>

// f0: green, f1: blue, f2: yellow, f3: red, f4: gray, f5: white, f6: brown, f7: ugly red
// f8: puple, f9: orange, fa: pink, fb: darker red fc: darker brown
typedef void(__stdcall* printConsole) (const char* formatString, ...);
typedef void(__stdcall* printAll) (const char* formatString, ...);
typedef void(__thiscall* printMiddle) (const char* string);

printAll hkPrintAll;
printConsole hkPrintConsole;
printMiddle hkPrintMiddle;

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

twglSwapBuffers wglSwapBuffersGateway;
bool eject = false;

/**
 * @brief wglSwapBuffers hook function to execute our cheats code as the
 * function gets executed once every frame.
 *
 * A trampoline hook is placed in the original flo of the function which jumps
 * to this function, after we finished our execution we pass execution on to our
 * gateway which will execute the bytes we originally stole and then jump back
 * to the appropriate location in the original function.
 */
BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
	uintptr_t modBaseAddress = GetMBA();
	Player* localPlayer = (Player*)*(uintptr_t*)(modBaseAddress + 0x10F4F4);

	if (GetAsyncKeyState(VK_DELETE) & 1) {
		eject = true;
		// if (settings::player::godMode) { hooks::health.Disable(); }
		if (settings::weapon::alwaysHeadshot) { ToggleAlwaysHeadshot(false, modBaseAddress); }
		if (settings::weapon::noRecoil) { ToggleRecoil(false, modBaseAddress); }
		if (settings::weapon::rapidFire) { ToggleRapidFire(false, modBaseAddress, (uintptr_t)localPlayer); }
		if (settings::weapon::infiniteAmmo) { ToggleInfiniteAmmo(false, modBaseAddress, (uintptr_t)localPlayer); }
	}

	if (GetAsyncKeyState(VK_F2) & 1) {
		settings::player::godMode = !settings::player::godMode;
		hkPrintAll(settings::player::godMode ? "<Godmode \f0[ON]\f5!>" : "<Godmode \f3[OFF]\f5!>");
		// hooks::health.Toggle();
	}

	if (GetAsyncKeyState(VK_F3) & 1) {
		settings::weapon::alwaysHeadshot = !settings::weapon::alwaysHeadshot;
		hkPrintAll(settings::weapon::alwaysHeadshot ? "<Headshots \f0[ON]\f5!>" : "<Headshots \f3[OFF]\f5!>");
		ToggleAlwaysHeadshot(settings::weapon::alwaysHeadshot, modBaseAddress);
	}

	if (GetAsyncKeyState(VK_F4) & 1) {
		settings::weapon::noRecoil = !settings::weapon::noRecoil;
		hkPrintAll(settings::weapon::noRecoil ? "<No Recoil \f0[ON]\f5!>" : "<No Recoil \f3[OFF]\f5!>");
		ToggleRecoil(settings::weapon::noRecoil, modBaseAddress);
	}

	if (GetAsyncKeyState(VK_F5) & 1) {
		settings::weapon::rapidFire = !settings::weapon::rapidFire;
		hkPrintAll(settings::weapon::rapidFire ? "<Rapid Fire \f0[ON]\f5!>" : "<Rapid Fire \f3[OFF]\f5!>");
		ToggleRapidFire(settings::weapon::rapidFire, modBaseAddress, (uintptr_t)localPlayer);
	}

	if (GetAsyncKeyState(VK_F6) & 1) {
		settings::weapon::infiniteAmmo = !settings::weapon::infiniteAmmo;
		hkPrintAll(settings::weapon::infiniteAmmo ? "<Inf. Ammo \f0[ON]\f5!>" : "<Inf. Ammo \f3[OFF]\f5!>");
		ToggleInfiniteAmmo(settings::weapon::infiniteAmmo, modBaseAddress, (uintptr_t)localPlayer);
	}

	return wglSwapBuffersGateway(hDc);
}


uintptr_t consoleHookGateway;
char* string;
char resultStr[256];
char* found;
void __declspec(naked) consoleHook()
{
	uintptr_t stringAddr;
	uintptr_t oldEcx;

	__asm {
		mov oldEcx, ecx
		mov stringAddr, ecx
	}

	string = (char*)(stringAddr);
	found = strstr(string, "headshot");

	if (found) {
		size_t startPos = found - string;
		strncpy_s(resultStr, string, startPos);
		resultStr[startPos] = '\0';
		strcat_s(resultStr, "fucked");
		strcat_s(resultStr, found + strlen("headshot"));
		oldEcx = (uintptr_t)resultStr;
	}

	__asm {
		mov ecx, oldEcx
		jmp[consoleHookGateway]
	}
}


void hookIngameConsole(bool hook)
{

	if (hook)
	{
		consoleHookGateway = (uintptr_t)hooks::TrampHook32((BYTE*)GetMBA() + 0x911B, (BYTE*)consoleHook, 8);
		std::cout << "[+] Ingame console hooked. Gateway at 0x" << std::uppercase << std::hex << consoleHookGateway << std::endl;
	}
	else
	{
		Patch((BYTE*)GetMBA() + 0x911B, (BYTE*)"\xC6\x84\x24\x03\x01\x00\x00\x00", 8);
		Sleep(20);
		Nop((BYTE*)consoleHookGateway, 10);
		bool freed = VirtualFree((BYTE*)consoleHookGateway, 0, MEM_RELEASE);
		std::cout << "[+] Ingame console unhooked. VirualFree: " << (freed ? "Success" : "Failure") << std::endl;
	}
}

DWORD WINAPI HackThread(HMODULE hModule)
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "DLL injected!\n";

	uintptr_t modBaseAddress = GetMBA();
	hkPrintConsole = (printConsole)(modBaseAddress + 0x6B060);
	hkPrintMiddle = (printMiddle)(modBaseAddress + 0x8E80);
	hkPrintAll = (printAll)(modBaseAddress + 0x90F0);

	hkPrintAll("\f0<Injected successfully!>");

	Player* localPlayer = (Player*)*(uintptr_t*)(modBaseAddress + 0x10F4F4);
	hooks::localPlayerAddress = (uintptr_t)localPlayer;

	auto swapBuffersHook = hooks::Hook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);

	swapBuffersHook.Enable();
	std::cout << "[+] Gateway at 0x" << std::uppercase << std::hex << wglSwapBuffersGateway << std::endl;

	while (!eject) { Sleep(100); }
	std::cout << "[+] Ejecting!" << std::endl;

	swapBuffersHook.Disable();

	if (f) { fclose(f); }
	FreeConsole();
	hkPrintAll("\f0<Ejected successfully!>");

	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, NULL, nullptr);
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