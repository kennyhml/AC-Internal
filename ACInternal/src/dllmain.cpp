#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "tools/mem.h"


DWORD jmpBackAddress;
DWORD localHealthAddress;

void __declspec(naked) healthHook()
{
	__asm {
		// OI: consider armor absorbation
		sub edi, eax

		// Check whether the target health address is our players health
		// If it is, set edi to 0 and then run the original damage instruction
		cmp ebx, [localHealthAddress]
		jne dealDamage
		mov edi, 0

		dealDamage :
		sub[ebx + 0x4], edi;
		jmp[jmpBackAddress]
	}
}



DWORD WINAPI HackThread(HMODULE hModule)
{

	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	std::cout << "DLL injected!\n";

	uintptr_t modBaseAddress = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	bool bHealth = false, bAmmo = false, bRecoil = false;

	uintptr_t* localPlayerPtr = (uintptr_t*)(modBaseAddress + 0x10f4f4);
	localHealthAddress = (DWORD)*localPlayerPtr + 0xF4;
	std::cout << std::hex << localHealthAddress;

	while (true)
	{
		if (GetAsyncKeyState(VK_DELETE) & 1) { break; }

		if (GetAsyncKeyState(VK_F1) & 1) {
			bHealth = !bHealth;

			uintptr_t hookAddress = modBaseAddress + 0x29D1D;
			jmpBackAddress = hookAddress + 5;

			if (bHealth)
			{
				Hook((void*)hookAddress, healthHook, 5);
			}
		}

		Sleep(5);
	}

	fclose(f);
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
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, NULL, nullptr));
	}

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}