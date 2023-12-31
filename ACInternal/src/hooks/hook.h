#pragma once
#include "../tools/mem.h"
#include "../data.h"
#include <Windows.h>
#include <iostream>
#include <vector>


namespace hooks
{
	struct Hook
	{
		inline static std::vector<Hook*> allHooks;

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
		inline static std::vector<Patcher*> allPatches;

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
	extern Patcher mapReveal;
	extern Patcher minimapReveal;

	void ToggleSpeed(bool toggle);

	bool Detour32(BYTE* src, BYTE* dst, int length);
	BYTE* TrampHook32(BYTE* src, BYTE* dst, int length);
}








