#pragma once

#include <Windows.h>
#include <iostream>
#include "../tools/mem.h"

namespace hooks
{
	inline uintptr_t localPlayerAddress;

	struct Hook
	{
		Hook(BYTE* src, BYTE* dst, BYTE* gatewayPointer, int size);
		Hook(const char* exportName, const char* moduleName, BYTE* dst, BYTE* gatewayPointer, int size);

		bool isHooked{ false };

		BYTE* src{ nullptr };
		BYTE* dst{ nullptr };
		BYTE* gatewayPointer{ nullptr };
		int size{ 0 };

		BYTE stolenBytesBuffer[10]{ 0 };

		void Enable();
		void Disable();
		void Toggle();
	};

	struct Patcher
	{
		Patcher(BYTE* src, BYTE* dst, int size);

		bool isPatched{ false };
		BYTE* src{ nullptr };
		BYTE* dst{ nullptr };

		int size{ 0 };

		BYTE stolenBytesBuffer[10]{ 0 };

		void Enable();
		void Disable();
		void Toggle();
	};

	extern Hook health;
	extern Hook ammo;
	extern Hook headshot;
	extern Hook rapidFire;
	extern Hook console;

	extern Patcher noRecoil;

	void ToggleSpeed(bool toggle);

	bool Detour32(BYTE* src, BYTE* dst, int length);
	BYTE* TrampHook32(BYTE* src, BYTE* dst, int length);
}








