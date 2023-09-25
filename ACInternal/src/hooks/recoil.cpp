#include "hook.h"

namespace hooks
{
	Patcher noRecoil = Patcher((BYTE*)"\xC2\x08\x00", (BYTE*)(GetMBA() + 0x62020), 3);
}