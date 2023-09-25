#include "data.h"
#include "tools/mem.h"
#include <Windows.h>

namespace data
{
	uintptr_t moduleBaseAddress = GetMBA();

	RECT gameRect{ 0 };
	bool result = GetClientRect(FindWindowW(NULL, L"AssaultCube"), &gameRect);

}