#include "hook.h"

namespace hooks
{
	static uintptr_t gateway;
	static char* string;
	static char* found;
	static char resultStr[256];
	static uintptr_t stringAddr;
	static uintptr_t oldEcx;

	/**
	* Hooks the console output function in order to replace any occurence of 'headshot'
	* in it with 'fucked'. For example 'you headshot Garry' -> 'you fucked Garry'.
	*
	* This is more of a proof of concept than an actual relevant feature of the cheat.
	*
	* Considerations:
	*
	* The ecx register holds the adress of the string thats going to be output to the
	* console and it will be used later in the original instructions again, so we have
	* to ensure that when our function finishes, exc holds the same value it did
	* before it was called. Otherwise the stack corrupts and we get undefined behavior
	* or more likely a crash.
	*
	* Thats why we move ecx into a temporary address in memory first and then restore it
	* at the end.
	*/
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

	Hook console = Hook(reinterpret_cast<BYTE*>(data::moduleBaseAddress + 0x911B), reinterpret_cast<BYTE*>(consoleHook), reinterpret_cast<BYTE*>(&gateway), 8);
}
