#include "hook.h"
#include "../settings.h"

namespace hooks
{
	void ToggleSpeed(bool toggle)
	{
		int value = toggle ? settings::player::speed : 1;

		Patch(reinterpret_cast<BYTE*>(data::moduleBaseAddress + 0x5BEA1), reinterpret_cast<BYTE*>(&value), 4);
		Patch(reinterpret_cast<BYTE*>(data::moduleBaseAddress + 0x5BF01), reinterpret_cast<BYTE*>(&value), 4);

		value = -value;
		Patch(reinterpret_cast<BYTE*>(data::moduleBaseAddress + 0x5BE41), reinterpret_cast<BYTE*>(&value), 4);
		Patch(reinterpret_cast<BYTE*>(data::moduleBaseAddress + 0x5BF61), reinterpret_cast<BYTE*>(&value), 4);
	}
}