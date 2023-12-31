#include "hook.h"
#include "../tools/mem.h"
#include <stdexcept>

hooks::Hook::Hook(BYTE* src, BYTE* dst, BYTE* gatewayPointer, int size)
	: src(src), dst(dst), gatewayPointer(gatewayPointer), size(size) {
	allHooks.push_back(this);
};

hooks::Hook::Hook(const char* exportName, const char* moduleName, BYTE* dst, BYTE* gatewayPointer, int size)
	: dst(dst), gatewayPointer(gatewayPointer), size(size)
{
	HMODULE hModule = GetModuleHandleA(moduleName);
	if (!hModule) { throw std::exception((std::string("GetModuleHandleA failed for ") + moduleName).c_str()); }

	FARPROC exportAddr = GetProcAddress(hModule, exportName);
	if (!exportAddr) { throw std::exception((std::string("GetProcAddress failed for ") + exportName).c_str()); }

	this->src = reinterpret_cast<BYTE*>(exportAddr);
	allHooks.push_back(this);
}

void hooks::Hook::Enable()
{
	if (isHooked) { return; }

	memcpy(stolenBytesBuffer, src, size);
	*reinterpret_cast<uintptr_t*>(gatewayPointer) = reinterpret_cast<uintptr_t>(TrampHook32(src, dst, size));

	std::cout << "[*] Hook enabled for dst at 0x" << std::uppercase << std::hex << reinterpret_cast<uintptr_t>(dst) << std::endl;
	std::cout << "\t[+] Gateway at 0x" << std::uppercase << std::hex << *reinterpret_cast<uintptr_t*>(gatewayPointer) << std::endl;
	std::cout << "\t[+] Stolen bytes: [";
	for (int i = 0; i < 10; i++)
	{
		std::cout << "0x" << static_cast<int>(stolenBytesBuffer[i]) << (i == 9 ? "]\n" : " ");
	}
	isHooked = true;
}

void hooks::Hook::Disable()
{
	if (!isHooked) { return; }

	Patch(src, stolenBytesBuffer, size);
	Sleep(10);
	bool freed = VirtualFree(*reinterpret_cast<BYTE**>(gatewayPointer), 0, MEM_RELEASE);

	std::cout << "[*] Hook disabled for dst at 0x" << std::uppercase << std::hex << reinterpret_cast<uintptr_t>(dst) << std::endl;
	std::cout << "\t[+] Gateway VirtualFree result: " << (freed ? "Succeeded" : "Failed") << std::endl;
	isHooked = false;
}

void hooks::Hook::Toggle()
{
	isHooked ? Disable() : Enable();
}

hooks::Patcher::Patcher(BYTE* src, BYTE* dst, int size)
	: src(src), dst(dst), size(size) {
	allPatches.push_back(this);
};

void hooks::Patcher::Enable()
{
	if (isPatched) { return; }
	Patch(stolenBytesBuffer, dst, size);
	Patch(dst, src, size);
	isPatched = true;
}

void hooks::Patcher::Disable()
{
	if (!isPatched) { return; }
	Patch(dst, stolenBytesBuffer, size);
	isPatched = false;
}

void hooks::Patcher::Toggle()
{
	isPatched ? Disable() : Enable();
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
	uintptr_t relativeAddress = reinterpret_cast<uintptr_t>(dst) - reinterpret_cast<uintptr_t>(src) - 5;

	// Replace the first byte with the JMP instruction, then the DWORD (bytes 2 - 5) with the address
	*src = 0xE9;
	*reinterpret_cast<uintptr_t*>(src + 1) = relativeAddress;

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
	*reinterpret_cast<uintptr_t*>(gateway + length + 1) = gatewayRelativeAddr;

	Detour32(src, dst, length);
	return gateway;
}