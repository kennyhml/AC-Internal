

#include <Windows.h>
#include <iostream>
#include "tools/mem.h"
#include "hooks/hook.h"



void ToggleRecoil(bool toggle, uintptr_t modBaseAddr)
{
	uintptr_t recoilAddr = modBaseAddr + 0x62020;

	if (toggle) { Patch((BYTE*)recoilAddr, (BYTE*)"\xC2\x08\x00", 3); }
	else { Patch((BYTE*)recoilAddr, (BYTE*)"\x55\x8B\xEC", 3); }
}
