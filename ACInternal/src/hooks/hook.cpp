#include "hook.h"
#include <stdexcept>
#include "../tools/mem.h"

hooks::Hook::Hook(BYTE* src, BYTE* dst, BYTE* gatewayPointer, int size)
	: src(src), dst(dst), gatewayPointer(gatewayPointer), size(size) {};


hooks::Hook::Hook(const char* exportName, const char* moduleName, BYTE* dst, BYTE* gatewayPointer, int size)
	: dst(dst), gatewayPointer(gatewayPointer), size(size)
{
	HMODULE hModule = GetModuleHandleA(moduleName);
	if (!hModule) { throw std::exception((std::string("GetModuleHandleA failed for ") + moduleName).c_str()); }

	FARPROC exportAddr = GetProcAddress(hModule, exportName);
	if (!exportAddr) { throw std::exception((std::string("GetProcAddress failed for ") + exportName).c_str()); }

	this->src = (BYTE*)exportAddr;
}


void hooks::Hook::Enable()
{
	if (isHooked) { return; }

	memcpy(stolenBytesBuffer, src, size);
	*(uintptr_t*)gatewayPointer = (uintptr_t)TrampHook32(src, dst, size);
	isHooked = true;
}

void hooks::Hook::Disable()
{
	if (!isHooked) { return; }
	Patch(src, stolenBytesBuffer, size);
	Sleep(10);
	bool freed = VirtualFree((BYTE*)*(uintptr_t*)gatewayPointer, 0, MEM_RELEASE);
	isHooked = false;
}

void hooks::Hook::Toggle()
{
	isHooked ? Disable() : Enable();
}

bool hooks::Detour32(BYTE* src, BYTE* dst, int length)
{
	// A JMP instruction (E9) is one byte, in a 32-bit process an address is
	// 32-bit or 4 bytes. Meaning we need at least 5 bytes of space to ourselves
	// in order to be able to write our jmp instruction.
	if (length < 5) { return false; }

	// Set the protection level to allow us to modify the memory
	DWORD oldProtect;
	VirtualProtect(src, length, PAGE_EXECUTE_READWRITE, &oldProtect);

	// NOP all the bytes
	memset(src, 0x90, length);

	// JMP instructions use relative addresses, which means we need to find the offset from
	// the instruction we are hooking to the address of the function we want to jump to.
	DWORD relativeAddress = ((DWORD)dst - (DWORD)src) - 5;

	// Replace the first byte with the JMP instruction, then the DWORD (bytes 2 - 5) with the address
	*src = 0xE9;
	*(uintptr_t*)(src + 1) = relativeAddress;

	VirtualProtect(src, length, oldProtect, &oldProtect);
	return true;
}

BYTE* hooks::TrampHook32(BYTE* src, BYTE* dst, int length)
{

	if (length < 5) { return 0; }

	// Allocate memory for our gateway function that will execute the original instructions
	// we will overwrite with our jmp, then jump back execution to the instruction after the jump
	BYTE* gateway = (BYTE*)VirtualAlloc(0, length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// Copy the instructions at our source (where we will later place the jump to our hook)
	// to our gateway so we can execute them there later.
	memcpy_s(gateway, length, src, length);

	// Get the relative offset to the src from our gateway
	uintptr_t gatewayRelativeAddr = src - gateway - 5;

	// jump back to the original program execution
	*(gateway + length) = 0xE9;
	*(uintptr_t*)(gateway + length + 1) = gatewayRelativeAddr;

	Detour32(src, dst, length);
	return gateway;
}