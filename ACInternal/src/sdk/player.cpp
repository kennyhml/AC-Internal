#pragma once

#include "player.h"
#include "enums.h"

/**
 * @brief Returns whether this player is an enemy given the local players team & mode.
 *
 * @param localPlayerTeam The Team of the local player (our team)
 * @param mode The GameMode of the current game as some modes dont have teams.
 */
bool SDK::Player::isEnemy(Team localPlayerTeam, GameMode mode) const
{
	return (
		team != localPlayerTeam
		|| mode == GameMode::BotDeathmatch || mode == GameMode::Deathmatch
		|| mode == GameMode::BotOneShotOneKill || mode == GameMode::OneShotOneKill
		|| mode == GameMode::BotPistolFrenz || mode == GameMode::PistolFrenzy
		|| mode == GameMode::BotLastSwissStanding || mode == GameMode::LastSwissStanding);
}

bool SDK::Player::isAlive() const
{
	return status == Status::Alive;
}