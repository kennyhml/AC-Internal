#pragma once
#include "sdk/sdk.h"

namespace aimbot
{
	void TargetClosest();
	bool IsPlayerVisible(SDK::Player* localPlayer, SDK::Player* player);
}