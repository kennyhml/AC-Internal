#include "hook.h"

namespace hooks
{
	Patcher noRecoil = Patcher(
		reinterpret_cast<BYTE*>("\xC2\x08\x00"),
		reinterpret_cast<BYTE*>(data::moduleBaseAddress + 0x62020),
		3);
}