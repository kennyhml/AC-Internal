#include "hook.h"

namespace hooks
{
	static uintptr_t gateway;
	static uintptr_t myHealthEbx = reinterpret_cast<uintptr_t>(data::localPlayer) + 0xF4;

	static void __declspec(naked) healthHook()
	{
		__asm {
			// Check whether the target health address is our players health
			// If it is, set edi and eax to 0
			cmp ebx, [myHealthEbx]
			jne finish
			mov edi, 0
			mov eax, 0

			finish:
			jmp gateway
		}
	}

	Hook health = Hook(
		reinterpret_cast<BYTE*>(data::moduleBaseAddress + 0x29D1D),
		reinterpret_cast<BYTE*>(healthHook),
		reinterpret_cast<BYTE*>(&gateway),
		5);

}






