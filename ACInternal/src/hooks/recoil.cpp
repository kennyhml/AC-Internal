#include "hook.h"

namespace hooks
{
	void ToggleRecoil(bool toggle)
	{
		uintptr_t targetAddress = GetMBA() + 0x62020;
		const char* instructions = toggle ? "\xC2\x08\x00" : "\x55\x8B\xEC";
		Patch((BYTE*)targetAddress, (BYTE*)instructions, 3);
	}
}


