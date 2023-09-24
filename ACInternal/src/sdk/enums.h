#pragma once

#include "sdk.h"

enum class SDK::MovementFlags : int32_t
{
	Forward = 1,
	Back = 255,
	Left = 256,
	Right = 65280,
	Respawn = 65536,
	NoClip = 262144,
	Ghost = 327680
};

enum class SDK::Team : int32_t
{
	CLA = 0,
	RVSF = 1,
	Spectate = 5
};

enum class SDK::Status : int32_t
{
	Alive = 0,
	Dead = 1,
	Spectate = 5
};


enum class SDK::GameMode : int32_t
{
	TeamDeathmatch,
	Coop,
	Deathmatch,
	Survivor,
	TeamSurvivor,
	CaptureTheFlag,
	PistolFrenzy,
	BotTeamDeathmatch,
	BotDeathmatch,
	LastSwissStanding,
	OneShotOneKill,
	TeamOneShotOneKill,
	BotOneShotOneKill,
	HuntTheFlag,
	TeamKeepTheFlag,
	KeepTheFlag,
	TeamPistolFrenzy,
	TeamLastSwissStanding,
	BotPistolFrenz,
	BotLastSwissStanding,
	BotTeamSurvivor,
	BotTeamOneShotOneKill,
};