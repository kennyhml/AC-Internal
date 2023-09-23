#include "tools/mem.h"
#include "player.h"
#include "settings.h"
#include <Windows.h>
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
static uintptr_t modBaseAddress;
static bool eject = false;

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
	Player* localPlayer = (Player*)*(uintptr_t*)(modBaseAddress + 0x10F4F4);

	if (GetAsyncKeyState(VK_DELETE) & 1) {
		eject = true;
		if (settings::player::godMode) { ToggleGodmode(false, modBaseAddress, localPlayer); }
		if (settings::weapon::alwaysHeadshot) { ToggleAlwaysHeadshot(false, modBaseAddress); }
		if (settings::weapon::noRecoil) { ToggleRecoil(false, modBaseAddress); }
		if (settings::weapon::rapidFire) { ToggleRapidFire(false, modBaseAddress); }
		if (settings::weapon::infiniteAmmo) { ToggleInfiniteAmmo(false, modBaseAddress); }
	}

	if (GetAsyncKeyState(VK_F2) & 1) {
		settings::player::godMode = !settings::player::godMode;
		hkPrintAll(settings::player::godMode ? "<Godmode \f0[ON]\f5!>" : "<Godmode \f3[OFF]\f5!>");
		ToggleGodmode(settings::player::godMode, modBaseAddress, localPlayer);
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
		ToggleRapidFire(settings::weapon::rapidFire, modBaseAddress);
	}

	if (GetAsyncKeyState(VK_F6) & 1) {
		settings::weapon::infiniteAmmo = !settings::weapon::infiniteAmmo;
		hkPrintAll(settings::weapon::infiniteAmmo ? "<Inf. Ammo \f0[ON]\f5!>" : "<Inf. Ammo \f3[OFF]\f5!>");
		ToggleInfiniteAmmo(settings::weapon::infiniteAmmo, modBaseAddress);
	}

	return wglSwapBuffersGateway(hDc);
}

/**
 * @brief Creates a trampoline hook from the games wglSwapBuffers function to
 * our hkwglSwapBuffers to the gateway and back to wglSwapBuffers.
 *
 * @param hook Whether to hook or unhook wglSwapBuffers.
 */
void hookSwapBuffers(bool hook)
{
	// Acquire the address of the wglSwapBuffers function in the opengl32.dll module
	HMODULE openglModule = GetModuleHandle(L"opengl32.dll");
	if (!openglModule) { throw std::runtime_error("GetModuleHandle(\"opengl32.dll\" failed."); }
	FARPROC wglSwapBuffersAddr = GetProcAddress(openglModule, "wglSwapBuffers");
	if (!wglSwapBuffersAddr) { throw std::runtime_error("GetProcAddress for wglSwapBuffers failed."); }

	if (hook)
	{
		BYTE* gateway = TrampHook32((BYTE*)wglSwapBuffersAddr, (BYTE*)hkwglSwapBuffers, 5);
		wglSwapBuffersGateway = reinterpret_cast<twglSwapBuffers>(gateway);
		std::cout << "[+] wglSwapBuffers hooked. Gateway at 0x" << std::uppercase << std::hex << wglSwapBuffersGateway << std::endl;
	}
	else
	{
		Patch((BYTE*)wglSwapBuffersAddr, (BYTE*)"\x8B\xFF\x55\x8B\xEC", 5);
		Nop((BYTE*)wglSwapBuffersGateway, 10);
		bool freed = VirtualFree((BYTE*)wglSwapBuffersGateway, 0, MEM_RELEASE);
		std::cout << "[+] wglSwapBuffers unhooked. VirualFree: " << (freed ? "Success" : "Failure") << std::endl;
	}
}

DWORD WINAPI HackThread(HMODULE hModule)
{

	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "DLL injected!\n";

	modBaseAddress = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	hkPrintConsole = (printConsole)(modBaseAddress + 0x6B060);
	hkPrintMiddle = (printMiddle)(modBaseAddress + 0x8E80);
	hkPrintAll = (printAll)(modBaseAddress + 0x90F0);

	hkPrintAll("\f0<Injected successfully!>");

	hookSwapBuffers(true);
	while (!eject) { Sleep(100); }
	std::cout << "[+] Ejecting...";

	hookSwapBuffers(false);

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