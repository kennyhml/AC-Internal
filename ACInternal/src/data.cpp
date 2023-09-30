#include "data.h"
#include "tools/mem.h"

namespace data
{
	RECT gameRect;
	bool result = GetClientRect(FindWindowW(NULL, L"AssaultCube"), &gameRect);

	uintptr_t moduleBaseAddress = GetMBA();
	SDK::Player* localPlayer = *reinterpret_cast<SDK::Player**>(moduleBaseAddress + 0x10F4F4);

}