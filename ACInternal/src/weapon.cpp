

#include <Windows.h>
#include <iostream>
#include "tools/mem.h"
#include "hooks/hook.h"

uintptr_t localPlayerAddr;

uintptr_t headshotGateway = 0;
void __declspec(naked) headshotHook()
{
	__asm {
		// ebp is our weapon ID and is later checked to be 5 because only 
		// snipers can do headshots, so we spoof it to be 5
		mov ebp, 5

		// cmp [esp + 0x1C],02 is another one of the conditions, I would assume
		// it stored information about where the enemy was hit (in the head if its 2)
		mov[esp + 0x1C], 02
		jmp[headshotGateway]
	}
}

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
uintptr_t ammoHookGateway;
void __declspec(naked) ammoHook()
{
	__asm {
		mov eax, [esi + 0x8]
		mov whoFired, eax
	}

	if (whoFired == localPlayerAddr)
	{
		__asm {
			mov eax, [esi + 0x14]
			inc[eax]
		}
	}
	__asm {
		jmp[ammoHookGateway];
	}
}

void ToggleAlwaysHeadshot(bool toggle, uintptr_t modBaseAddr)
{
	uintptr_t headshotCheckAddr = modBaseAddr + 0x61755;

	if (toggle)
	{
		headshotGateway = (uintptr_t)hooks::TrampHook32((BYTE*)headshotCheckAddr, (BYTE*)headshotHook, 5);
		std::cout << "[+] Always headshot enabled. Headshot hook gateway at 0x" << std::uppercase << std::hex << headshotGateway << std::endl;
	}
	else
	{
		Patch((BYTE*)headshotCheckAddr, (BYTE*)"\x83\xFD\x05\x75\x18", 5);
		Sleep(5);
		Nop((BYTE*)headshotGateway, 10);
		bool freed = VirtualFree((BYTE*)headshotGateway, 0, MEM_RELEASE);
		std::cout << "[+] Always headshot disabled. VirtualFree: " << (freed ? "Success" : "Failed") << std::endl;
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

void ToggleInfiniteAmmo(bool toggle, uintptr_t modBaseAddr, uintptr_t localPlayerAddress)
{
	uintptr_t ammoAddr = modBaseAddr + 0x637E6;

	if (toggle)
	{
		localPlayerAddr = localPlayerAddress;
		ammoHookGateway = (uintptr_t)hooks::TrampHook32((BYTE*)ammoAddr, (BYTE*)ammoHook, 5);
	}
	else
	{
		Patch((BYTE*)ammoAddr, (BYTE*)"\x8B\x76\x14\xFF\x0E", 5);
		Sleep(5);
		Nop((BYTE*)ammoHookGateway, 10);
		VirtualFree((BYTE*)ammoHookGateway, 0, MEM_RELEASE);
	}
}

