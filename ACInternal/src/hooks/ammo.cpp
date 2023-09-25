#include "hook.h"

namespace hooks
{

	/**
	 * Hooks the instructions that are responsible for decreasing our ammo count.
	 *
	 * Detailed rundown:
	 *
	 *		mov esi, [esi + 14]   <<< Load the ammo count at [esi + 14] into esi
	 *		dec [esi]			  <<< Decrement esi to remove 1 bullet
	 *
	 * Considerations:
	 *
	 * Just NOPing the dec instruction would work, but apply to every other entity in the
	 * game as well meaning everyone will have infinite ammo. To prevent this our hook
	 * has to check whether the person who fired is our local player and if it is then
	 * add 1 to esi before the dec is called
	 */
	static uintptr_t gateway;
	static uintptr_t whoFired;

	static void __declspec(naked) ammoHook()
	{
		__asm {
			mov eax, [esi + 0x8]
			mov whoFired, eax
		}

		if (whoFired == localPlayerAddress)
		{
			__asm {
				mov eax, [esi + 0x14]
				inc[eax]
			}
		}
		__asm {
			jmp[gateway];
		}
	}

	static Hook GetAmmoHook()
	{
		uintptr_t targetAddress = data::moduleBaseAddress + 0x637E6;
		return Hook((BYTE*)targetAddress, (BYTE*)ammoHook, (BYTE*)&gateway, 5);

	}

	Hook ammo = GetAmmoHook();




}




