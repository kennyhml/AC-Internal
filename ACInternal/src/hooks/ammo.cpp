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
			mov eax, [esi + 0x8] // load weapon owner into tmp register
			mov whoFired, eax // load weapon owner into whoFired
		}

		if (whoFired == reinterpret_cast<uintptr_t>(data::localPlayer)) {
			__asm {
				mov eax, [esi + 0x14] // load ammo count into tmp register
				inc[eax] // increment the ammo count to nullify the decrement
			}
		}
		__asm {
			jmp[gateway];
		}
	}

	Hook ammo = Hook(
		reinterpret_cast<BYTE*>(data::moduleBaseAddress + 0x637E6),
		reinterpret_cast<BYTE*>(ammoHook),
		reinterpret_cast<BYTE*>(&gateway),
		5);
}




