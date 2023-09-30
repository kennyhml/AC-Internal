#include "hook.h"

namespace hooks
{
	Patcher mapReveal = Patcher(
		reinterpret_cast<BYTE*>("\x90\x90\x90\x90\x90\x90"),
		reinterpret_cast<BYTE*>(0x04096A1),
		6);

	Patcher minimapReveal = Patcher(
		reinterpret_cast<BYTE*>("\x90\x90\x90\x90\x90\x90"),
		reinterpret_cast<BYTE*>(0x0409FB3),
		6);
}


