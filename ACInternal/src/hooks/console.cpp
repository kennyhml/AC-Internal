#include "hook.h"

namespace hooks
{

	static uintptr_t gateway;
	static char* string;
	static char* found;
	static char resultStr[256];

	static uintptr_t stringAddr;
	static uintptr_t oldEcx;

	static void __declspec(naked) consoleHook()
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

	static Hook GetConsoleHook()
	{
		uintptr_t targetAddress = data::moduleBaseAddress + 0x911B;
		return Hook((BYTE*)targetAddress, (BYTE*)consoleHook, (BYTE*)&gateway, 8);
	}

	Hook console = GetConsoleHook();
}
