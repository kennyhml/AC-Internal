#include "hook.h"

namespace hooks
{
	BYTE nops[6]{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	Patcher mapReveal = Patcher(reinterpret_cast<BYTE*>(nops), reinterpret_cast<BYTE*>(0x04096A1), 6);
	Patcher minimapReveal = Patcher(reinterpret_cast<BYTE*>(nops), reinterpret_cast <BYTE*>(0x0409FB3), 6);
}


