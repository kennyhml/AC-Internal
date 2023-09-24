#pragma once

#include <Windows.h>
#include <memory>



namespace SDK
{
	typedef void(__stdcall* tServerMessage) (const char* formatString, ...);
	typedef void(__stdcall* tAllMessage) (const char* formatString, ...);
	typedef void(__thiscall* tPlayerMessage) (const char* string);

	struct Player;
	struct Weapon;
	struct WeaponData;
	struct ReserveData;

	enum class MovementFlags;
	enum class Team;
	enum class Status;
	enum class GameMode;

	extern tServerMessage sendServerMessage;
	extern tAllMessage sendAllMessage;
	extern tPlayerMessage sendPlayerMesage;

}