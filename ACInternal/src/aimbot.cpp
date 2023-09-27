#include "aimbot.h"
#include "sdk/player.h"
#include "data.h"
#include "geometry.h"
#include "settings.h"
#include "gl/glDraw.h"

namespace aimbot
{
	void TargetClosest()
	{
		SDK::Player* localPlayer = (SDK::Player*)*(uintptr_t*)(data::moduleBaseAddress + 0x10F4F4);

		int32_t playerCount = *(uintptr_t*)(data::moduleBaseAddress + 0x10F500);
		uintptr_t entityList = *(uintptr_t*)(data::moduleBaseAddress + 0x10F4F8);
		SDK::GameMode mode = static_cast<SDK::GameMode>(*(uintptr_t*)(data::moduleBaseAddress + 0x10F49C));

		Vector3 bestAngle;
		float closestDiff = -1;
		SDK::Player* closestEnemy = nullptr;

		for (int i = 1; i < playerCount; i++)
		{
			int offset = 4 * i;
			SDK::Player* entity = (SDK::Player*)*(uintptr_t*)(entityList + offset);

			if (!entity->isAlive() || !entity->isEnemy(localPlayer->team, mode) || !IsPlayerVisible(localPlayer, entity)) { continue; }

			float distance = GetDistance(localPlayer->headPos, entity->headPos);

			if (distance < settings::aimbot::minDistance || distance > settings::aimbot::maxDistance) { continue; }

			Vector3 perfectAngle = CalculateAngle(localPlayer->headPos, entity->headPos);
			Vector3 currAngle = localPlayer->viewAngle;

			Vector3 viewDiff = perfectAngle - currAngle;

			if (max(abs(viewDiff.x), abs(viewDiff.y)) > settings::aimbot::radius && !settings::aimbot::turretMode) { continue; }

			float totalDiff = abs(viewDiff.x) + abs(viewDiff.y);

			if (totalDiff < closestDiff || closestDiff == -1) {
				closestDiff = totalDiff;
				closestEnemy = entity;
				bestAngle = perfectAngle;
			}
		}
		if (closestEnemy) { localPlayer->viewAngle = bestAngle; }

		localPlayer->isShooting = settings::aimbot::autoFire && closestEnemy != nullptr;
	}

	struct traceresult_s
	{
		Vector3 end;
		bool collided;
	};

	bool IsPlayerVisible(SDK::Player* localPlayer, SDK::Player* player)
	{
		DWORD traceLine = 0x048a310;
		traceresult_s traceresult;
		traceresult.collided = false;
		Vector3 from = localPlayer->headPos;
		Vector3 to = player->headPos;

		__asm
		{
			push 0; bSkipTags
			push 0; bCheckPlayers
			push localPlayer
			push to.z
			push to.y
			push to.x
			push from.z
			push from.y
			push from.x
			lea eax, [traceresult]
			call traceLine;
			add esp, 36
		}
		return !traceresult.collided;

	}
}