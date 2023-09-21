#include "mem.h"

/**
 * @brief Replaces the bytes at the destination with the bytes at the source.
 *
 * @param dst A byte pointer to the memory address we want to patch bytes at.
 * @param src A byte pointer to the memory address containing the data to patch with.
 * @param size The size of the source data to patch with.
 */
void Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	// Remember the previous protection level so we can change it back after
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}

/**
 * @brief Replaces the bytes at the destination with NOPs (No operations).
 *
 * @param dst A byte pointer to the memory address we want to patch bytes at.
 * @param size The amount of NOPs to place at the destination.
 */
void Nop(BYTE* dst, unsigned int size)
{
	// Remember the previous protection level so we can change it back after
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}

/**
 * @brief Steps through a vector of offsets to reach the final destination of multi
 * level pointers.
 *
 * @param ptr The starting address of our multi-level pointer.
 * @param offsets The offset to apply after following each pointer.
 */
uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}
	return addr;
}

bool Hook(void* toHook, void* ourFunction, int length)
{
	// A JMP instruction (E9) is one byte, in a 32-bit process an address is
	// 32-bit or 4 bytes. Meaning we need at least 5 bytes of space to ourselves
	// in order to be able to write our jmp instruction.
	if (length < 5) { return false; }

	// Set the protection level to allow us to modify the memory
	DWORD oldProtect;
	VirtualProtect(toHook, length, PAGE_EXECUTE_READWRITE, &oldProtect);

	// NOP all the bytes
	memset(toHook, 0x90, length);

	// JMP instructions use relative addresses, which means we need to find the offset from
	// the instruction we are hooking to the address of the function we want to jump to.
	DWORD relativeAddress = ((DWORD)ourFunction - (DWORD)toHook) - 5;

	// Replace the first byte with the JMP instruction, then the DWORD (bytes 2 - 5) with the address
	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = relativeAddress;

	VirtualProtect(toHook, length, oldProtect, &oldProtect);
	return true;
}