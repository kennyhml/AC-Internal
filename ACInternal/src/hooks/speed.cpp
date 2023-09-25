#include "hook.h"
#include "../settings.h"

namespace hooks
{
	void ToggleSpeed(bool toggle)
	{
		int value = toggle ? settings::player::speed : 1;

		Patch((BYTE*)(data::moduleBaseAddress + 0x5BEA1), (BYTE*)&value, 4);
		Patch((BYTE*)(data::moduleBaseAddress + 0x5BF01), (BYTE*)&value, 4);

		value = -value;

		Patch((BYTE*)(data::moduleBaseAddress + 0x5BE41), (BYTE*)&value, 4);
		Patch((BYTE*)(data::moduleBaseAddress + 0x5BF61), (BYTE*)&value, 4);
	}
}