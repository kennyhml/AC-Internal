#include "esp.h"
#include <vector>
#include "data.h"
#include "sdk/player.h"
#include "gl/glDraw.h"
#include "gl/glText.h"

#include <iostream>

// scaling 
const int GAME_UNIT_MAGIC = 400;

const float PLAYER_HEIGHT = 5.25f;
const float PLAYER_WIDTH = 2.f;
const float EYE_HEIGHT = 4.5f;

const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;


const int FONT_SMALL_HEIGHT = 7;
const int FONT_SMALL_WIDTH = 4;

const int FONT_MEDIUM_HEIGHT = 15;
const int FONT_MEDIUM_WIDTH = 9;

const int FONT_LARGE_HEIGHT = 20;
const int FONT_LARGE_WIDTH = 11;


GL::Font fontSmall;
GL::Font fontMedium;
GL::Font fontLarge;

static uint32_t GetPlayerCount()
{
	return *(uintptr_t*)(data::moduleBaseAddress + 0x10F500);
}

static std::vector<SDK::Player*> LoadPlayers()
{
	uint32_t count = GetPlayerCount();

	std::vector<SDK::Player*> players;
	players.reserve(count);

	uintptr_t entityList = *(uintptr_t*)(data::moduleBaseAddress + 0x10F4F8);

	for (int i = 1; i < count; i++)
	{
		int offset = 4 * i;
		players.push_back((SDK::Player*)*(uintptr_t*)(entityList + offset));
	}
	return players;
}


void DrawHealth(SDK::Player*, Rect boudingRect)
{






}

Rect GetBoundingBox(const Vector2& screen, float distance)
{
	float scale = (GAME_UNIT_MAGIC / distance);

	float x = screen.x - scale;
	float y = screen.y - scale * PLAYER_ASPECT_RATIO;
	float width = scale * 2;
	float height = scale * PLAYER_ASPECT_RATIO * 2;

	return Rect{ x, y, width, height };
}

void esp::DrawPlayer(SDK::Player* player, float matrix[16])
{
	if (!player->isAlive()) { return; }

	Vector2 screen;

	Vector3 center = player->headPos;
	center.z = center.z - EYE_HEIGHT + PLAYER_HEIGHT / 2;

	if (!WorldToScreen(center, screen, matrix, data::gameRect.right, data::gameRect.bottom)) { return; }

	SDK::Player* localPlayer = (SDK::Player*)*(uintptr_t*)(data::moduleBaseAddress + 0x10F4F4);
	SDK::GameMode mode = static_cast<SDK::GameMode>(*(uintptr_t*)(data::moduleBaseAddress + 0x10F49C));

	auto color = player->isEnemy(localPlayer->team, mode) ? rgb::red : rgb::green;
	float distance = GetDistance(localPlayer->headPos, player->headPos);

	Rect box = GetBoundingBox(screen, distance);

	float thickness = distance > 55 ? 1.f : 1.5f;

	GL::DrawOutline(box.x, box.y, box.w, box.h, thickness, color);
	GL::DrawLine(data::gameRect.right / 2, data::gameRect.bottom, screen.x, screen.y + box.h / 2, 1.f, color);

	GL::Font& font = distance > 55 ? fontSmall : distance > 20 ? fontMedium : fontLarge;

	float textX = font.centerText(box.x, box.w, font.GetTextWidth(player->name));
	float textY = box.y - font.height / 2;
	font.Print(textX, textY, color, "%s", player->name);
}

void esp::Draw()
{
	HDC currHDC = wglGetCurrentDC();

	if (!fontSmall.built || currHDC != fontSmall.hdc) { fontSmall.Build(FONT_SMALL_HEIGHT, FONT_SMALL_WIDTH); }
	if (!fontMedium.built || currHDC != fontMedium.hdc) { fontMedium.Build(FONT_MEDIUM_HEIGHT, FONT_MEDIUM_WIDTH); }
	if (!fontLarge.built || currHDC != fontLarge.hdc) { fontLarge.Build(FONT_LARGE_HEIGHT, FONT_LARGE_WIDTH); }

	GL::SetupOrtho();

	float matrix[16];
	glGetIntegerv(GL_VIEWPORT, GL::viewport);

	memcpy((BYTE*)matrix, (BYTE*)(data::moduleBaseAddress + 0x101AE8), sizeof(matrix));
	int32_t playerCount = *(uintptr_t*)(data::moduleBaseAddress + 0x10F500);
	uintptr_t entityList = *(uintptr_t*)(data::moduleBaseAddress + 0x10F4F8);

	for (int i = 1; i < playerCount; i++)
	{
		int offset = 4 * i;
		DrawPlayer((SDK::Player*)*(uintptr_t*)(entityList + offset), matrix);
	}

	GL::RestoreGL();
}