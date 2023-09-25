#include "hook.h"

namespace hooks
{
	uintptr_t gateway;
	char* string;
	char* found;
	char resultStr[256];

	uintptr_t stringAddr;
	uintptr_t oldEcx;

	void __declspec(naked) consoleHook()
	{
		__asm {
			mov oldEcx, ecx
			mov stringAddr, ecx
		}

		string = (char*)(stringAddr);
		found = strstr(string, "headshot");

		if (found) {
			size_t startPos = found - string;
			strncpy_s(resultStr, string, startPos);
			resultStr[startPos] = '\0';
			strcat_s(resultStr, "fucked");
			strcat_s(resultStr, found + strlen("headshot"));
			oldEcx = (uintptr_t)resultStr;
		}

		__asm {
			mov ecx, oldEcx
			jmp[gateway]
		}
	}

	Hook GetConsoleHook()
	{
		uintptr_t targetAddress = data::moduleBaseAddress + 0x911B;
		return Hook((BYTE*)targetAddress, (BYTE*)consoleHook, (BYTE*)&gateway, 8);
	}

	Hook console = GetConsoleHook();
}
