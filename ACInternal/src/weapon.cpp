

#include <Windows.h>
#include <iostream>
#include "tools/mem.h"
#include "hooks/hook.h"

uintptr_t localPlayerAddr;



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
uintptr_t cooldownHookGateway = 0;
uintptr_t whoFired;
void __declspec(naked) fireCooldownHook()
{
	__asm {
		mov eax, [esi + 0x8]
		mov whoFired, eax
	}

	if (whoFired == localPlayerAddr)
	{
		__asm {
			mov ecx, 0
		}
	}
	__asm {
		jmp[cooldownHookGateway];
	}
}




void ToggleRecoil(bool toggle, uintptr_t modBaseAddr)
{
	uintptr_t recoilAddr = modBaseAddr + 0x62020;

	if (toggle) { Patch((BYTE*)recoilAddr, (BYTE*)"\xC2\x08\x00", 3); }
	else { Patch((BYTE*)recoilAddr, (BYTE*)"\x55\x8B\xEC", 3); }
}

void ToggleRapidFire(bool toggle, uintptr_t modBaseAddr, uintptr_t localPlayerAddress)
{
	uintptr_t cooldownAddr = modBaseAddr + 0x637E1;
	if (toggle)
	{
		localPlayerAddr = localPlayerAddress;
		cooldownHookGateway = (uintptr_t)hooks::TrampHook32((BYTE*)cooldownAddr, (BYTE*)fireCooldownHook, 5);
	}
	else
	{
		Patch((BYTE*)cooldownAddr, (BYTE*)"\x8B\x56\x18\x89\x0A", 5);
		Sleep(5);
		Nop((BYTE*)cooldownHookGateway, 10);
		VirtualFree((BYTE*)cooldownHookGateway, 0, MEM_RELEASE);
	}
}

