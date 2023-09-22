

#include <Windows.h>
#include <iostream>
#include "tools/mem.h"

uintptr_t headshotGateway;

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


void ToggleAlwaysHeadshot(bool toggle, uintptr_t modBaseAddr)
{
	uintptr_t headshotCheckAddr = modBaseAddr + 0x61755;

	if (toggle)
	{
		headshotGateway = (uintptr_t)TrampHook32((BYTE*)headshotCheckAddr, (BYTE*)headshotHook, 5);
		std::cout << "[+] Always headshot enabled. Headshot hook gateway at 0x" << std::uppercase << std::hex << headshotGateway << std::endl;
	}
	else
	{
		Patch((BYTE*)headshotCheckAddr, (BYTE*)"\x83\xFD\x05\x75\x18", 5);
		Nop((BYTE*)headshotGateway, 10);
		bool freed = VirtualFree((BYTE*)headshotGateway, 0, MEM_RELEASE);
		std::cout << "[+] Always headshot disabled. VirtualFree: " << (freed ? "Success" : "Failed") << std::endl;
	}
}
