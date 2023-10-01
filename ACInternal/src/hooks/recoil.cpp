#include "hook.h"

namespace hooks
{
	BYTE instructions[3]{ 0xC2, 0x08, 0x00 };
	Patcher noRecoil = Patcher(
		reinterpret_cast<BYTE*>(instructions),
		reinterpret_cast<BYTE*>(data::moduleBaseAddress + 0x62020),
		3);
}