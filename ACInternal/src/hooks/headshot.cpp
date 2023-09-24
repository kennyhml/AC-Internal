#include "hook.h"

namespace hooks
{
	static uintptr_t gateway;

	static void __declspec(naked) headshotHook()
	{
		__asm {
			// ebp is our weapon ID and is later checked to be 5 because only 
			// snipers can do headshots, so we spoof it to be 5
			mov ebp, 5

			// cmp [esp + 0x1C],02 is another one of the conditions, I would assume
			// it stored information about where the enemy was hit (in the head if its 2)
			mov[esp + 0x1C], 02
			jmp[gateway]
		}
	}

	static Hook GetHeadshotHook()
	{
		uintptr_t targetAddress = GetMBA() + 0x61755;
		return Hook((BYTE*)targetAddress, (BYTE*)headshotHook, (BYTE*)&gateway, 5);
	}

	Hook headshot = GetHeadshotHook();
}