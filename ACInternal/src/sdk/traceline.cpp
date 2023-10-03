#include "sdk.h"
#include "player.h"
#include "../data.h"

struct TracelineResult
{
	Vector3 end;
	bool collided;
};

bool SDK::IsVisible(SDK::Player* entity)
{
	DWORD traceLine = 0x048A310;
	TracelineResult traceresult;
	traceresult.collided = false;
	Vector3 src = data::localPlayer->headPos;
	Vector3 dst = entity->headPos;

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