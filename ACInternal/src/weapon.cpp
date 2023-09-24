

#include <Windows.h>
#include "tools/mem.h"

void ToggleRecoil(bool toggle)
{
	uintptr_t recoilAddr = GetMBA() + 0x62020;

	if (toggle) { Patch((BYTE*)recoilAddr, (BYTE*)"\xC2\x08\x00", 3); }
	else { Patch((BYTE*)recoilAddr, (BYTE*)"\x55\x8B\xEC", 3); }
}
