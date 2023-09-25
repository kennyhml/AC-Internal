#include "hook.h"

namespace hooks
{
	static uintptr_t gateway;
	static uintptr_t myHealthEbx;

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

	static hooks::Hook getHealthHook()
	{
		uintptr_t targetAddr = data::moduleBaseAddress + 0x29D1D;
		myHealthEbx = *(uintptr_t*)(data::moduleBaseAddress + 0x10F4F4) + 0xF4;

		hooks::Hook health = hooks::Hook((BYTE*)targetAddr, (BYTE*)healthHook, (BYTE*)&gateway, 5);
		return health;
	}

	Hook health = getHealthHook();
}






