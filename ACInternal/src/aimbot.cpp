#include "aimbot.h"
#include "data.h"
#include "geometry.h"
#include "settings.h"
#include "sdk/player.h"
#include "gl/glDraw.h"

namespace aimbot
{
	struct TraceLineResult
	{
		Vector3 end;
		bool collided;
	};

	bool IsPlayerVisible(SDK::Player* player)
	{
		DWORD traceLine = 0x048a310;
		TraceLineResult traceresult;
		traceresult.collided = false;
		Vector3 src = data::localPlayer->headPos;
		Vector3 dst = player->headPos;

		__asm
		{
			push 0; bSkipTags
			push 0; bCheckPlayers
			push data::localPlayer
			push src.z
			push src.y
			push src.x
			push dst.z
			push dst.y
			push dst.x
			lea eax, [traceresult]
			call traceLine;
			add esp, 36
		}
		return !traceresult.collided;
	}

	bool IsValidTarget(SDK::Player* entity, SDK::GameMode mode)
	{
		return (entity->isAlive()
			&& entity->isEnemy(data::localPlayer->team, mode)
			&& (IsPlayerVisible(entity) || settings::aimbot::wallhack));
	}

	void TargetClosest()
	{
		int32_t playerCount = *reinterpret_cast<uintptr_t*>(data::moduleBaseAddress + 0x10F500);
		uintptr_t entityList = *reinterpret_cast<uintptr_t*>(data::moduleBaseAddress + 0x10F4F8);
		SDK::GameMode mode = *reinterpret_cast<SDK::GameMode*>(data::moduleBaseAddress + 0x10F49C);

		Vector3 bestAngle;
		float closestDiff = -1.f;
		SDK::Player* closestEnemy = nullptr;

		for (int i = 1; i < playerCount; i++)
		{
			SDK::Player* entity = *reinterpret_cast<SDK::Player**>(entityList + (i * 4));

			// Filter out entities that are dead, one the same team or simply not visible to us
			if (!IsValidTarget(entity, mode)) { continue; }


			// Filter out entities that are further than our max range to aimbot
			float distance = GetDistance(data::localPlayer->headPos, entity->headPos);
			if (distance > settings::aimbot::maxDistance) { continue; }

			// Calculate the angle required to headshot this enemy and check how much
			// it varies from our current angle so we can aimbot the closest head to 
			// our current crosshair position.
			Vector3 perfectAngle = CalculateAngle(data::localPlayer->headPos, entity->headPos);
			Vector3 viewDiff = perfectAngle - data::localPlayer->viewAngle;
			float maxDiff = max(abs(viewDiff.x), abs(viewDiff.y));

			// Filter out entites whose head is not within our aimbot radius
			if (maxDiff > settings::aimbot::radius && !settings::aimbot::turretMode) { continue; }
			float totalDiff = abs(viewDiff.x) + abs(viewDiff.y);

			// Check whether this entity is a better match than the previous entities
			if (totalDiff < closestDiff || closestDiff == -1) {
				closestDiff = totalDiff;
				closestEnemy = entity;
				bestAngle = perfectAngle;
			}
		}
		// Target only if a valid enemy was actually found
		if (closestEnemy) { data::localPlayer->viewAngle = Vector3(bestAngle.x, bestAngle.y - 0.5f, 0); }
		// Fire if auto fire is enabled and an enemy was targetted
		if (settings::aimbot::autoFire) {
			data::localPlayer->isShooting = settings::aimbot::autoFire && closestEnemy;
		}
	}

}