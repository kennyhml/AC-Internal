#include "hook.h"

namespace hooks
{
	/**
	 * Hooks the instructions that are responsible to apply a fire cooldown to the
	 * current weapon.
	 *
	 * Detailed rundown:
	 *
	 *		mov eax,[esi + 0C]				   <<< esi: the weapon that was shot,
	 *											   0C is the data ptr offset
	 *
	 *		movsx ecx, word ptr [eax + 10A]    <<< eax: the weapon data (set in prior call),
	 *											   10A is the offset for the cooldown to apply
	 *
	 *		mov edx,[esi + 18]				   <<< esi + 18: pointer to the current cooldown
	 *
	 *		[edx],ecx						   <<< Moves the cooldown of the weapon into the current cooldown
	 *
	 * Considerations:
	 *
	 * If we just NOPd the [edx], ecx instruction or detoured into mov ecx,0 it would apply
	 * for ALL players including bots. Which means we have to somehow check whether the person
	 * who fired the weapon is us and only us, then remove the cooldown. How convenient that
	 * the weapon pointer (esi) has a pointer to its owner at offset 0x8, which means we
	 * can access that and if it matches with our local player pointer, we remove the cooldown.
	 */
	static uintptr_t gateway;
	static uintptr_t whoFired;

	static void __declspec(naked) fireHook()
	{
		__asm {
			mov eax, [esi + 0x8]
			mov whoFired, eax
		}

		if (whoFired == localPlayerAddress)
		{
			__asm {
				mov ecx, 0
			}
		}
		__asm {
			jmp[gateway];
		}
	}

	static Hook GetRapidFireHook()
	{
		uintptr_t targetAddress = GetMBA() + 0x637E1;
		return Hook((BYTE*)targetAddress, (BYTE*)fireHook, (BYTE*)&gateway, 5);
	}

	Hook rapidFire = GetRapidFireHook();
}