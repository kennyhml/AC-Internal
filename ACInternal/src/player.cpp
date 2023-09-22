
#include "player.h"
#include "tools/mem.h"
#include <Windows.h>
#include <iostream>

uintptr_t healthGateway;
uintptr_t myHealthEbx;

void __declspec(naked) healthHook()
{
	__asm {
		// Check whether the target health address is our players health
		// If it is, set edi and eax to 0
		cmp ebx, [myHealthEbx]
		jne finish
		mov edi, 0
		mov eax, 0

		finish:
		jmp healthGateway
	}
}


/**
 * @brief Returns whether this player is an enemy given the local players team & mode.
 *
 * @param localPlayerTeam The Team of the local player (our team)
 * @param mode The GameMode of the current game as some modes dont have teams.
 */
bool Player::isEnemy(Team localPlayerTeam, GameMode mode) const
{
	return (
		team != localPlayerTeam
		|| mode == GameMode::BotDeathmatch || mode == GameMode::Deathmatch
		|| mode == GameMode::BotOneShotOneKill || mode == GameMode::OneShotOneKill
		|| mode == GameMode::BotPistolFrenz || mode == GameMode::PistolFrenzy
		|| mode == GameMode::BotLastSwissStanding || mode == GameMode::LastSwissStanding);
}

bool Player::isAlive() const
{
	return status == Status::Alive;
}

void ToggleGodmode(bool toggle, uintptr_t modBaseAddress, const Player* localPlayer)
{
	int size = 5;
	uintptr_t addressToHook = modBaseAddress + 0x29D1D;

	if (toggle)
	{
		myHealthEbx = (DWORD)localPlayer + 0xF4;
		healthGateway = (uintptr_t)TrampHook32((BYTE*)addressToHook, (BYTE*)healthHook, size);
		std::cout << "[+] Godmode enabled. Health hook gateway at 0x" << std::uppercase << std::hex << healthGateway << std::endl;
	}
	else
	{
		// Write the original instructions back instead of our hook
		Patch((BYTE*)addressToHook, (BYTE*)"\x2B\xF8\x29\x7B\x04", size);
		Nop((BYTE*)healthGateway, size + 5);
		bool freed = VirtualFree((BYTE*)healthGateway, 0, MEM_RELEASE);

		std::cout << "[+] Godmode disabled. VirtualFree: " << (freed ? "Success" : "Failed") << std::endl;
	}
}
