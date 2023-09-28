#include "hook.h"

namespace hooks
{
	Patcher mapReveal = Patcher((BYTE*)"\x90\x90\x90\x90\x90\x90", (BYTE*)0x04096A1, 6);
	Patcher minimapReveal = Patcher((BYTE*)"\x90\x90\x90\x90\x90\x90", (BYTE*)0x0409FB3, 6);
}


