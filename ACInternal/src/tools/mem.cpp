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

static uintptr_t modBaseAddr = -1;
uintptr_t GetMBA()
{
	if (modBaseAddr != -1) { return modBaseAddr; }
	modBaseAddr = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	return modBaseAddr;

}