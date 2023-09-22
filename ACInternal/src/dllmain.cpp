#include "tools/mem.h"
#include "player.h"
#include "settings.h"

#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>


typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

twglSwapBuffers wglSwapBuffersGateway;
static uintptr_t modBaseAddress;
static bool eject = false;

BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
	Player* localPlayer = (Player*)*(uintptr_t*)(modBaseAddress + 0x10F4F4);

	if (GetAsyncKeyState(VK_DELETE) & 1) {
		eject = true;
		if (settings::player::godMode)
		{
			ToggleGodmode(false, modBaseAddress, localPlayer);
		}
	}

	if (GetAsyncKeyState(VK_F1) & 1) {
		settings::player::godMode = !settings::player::godMode;
		ToggleGodmode(settings::player::godMode, modBaseAddress, localPlayer);
	}

	return wglSwapBuffersGateway(hDc);
}

void hookSwapBuffers(bool hook)
{
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

	hookSwapBuffers(true);
	while (!eject) { Sleep(100); }
	std::cout << "[+] Ejecting...";

	hookSwapBuffers(false);

	if (f) { fclose(f); }
	FreeConsole();
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