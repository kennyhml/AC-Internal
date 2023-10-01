#include "esp.h"
#include "data.h"
#include "settings.h"
#include "sdk/player.h"
#include "sdk/weapon.h"
#include "gl/glDraw.h"
#include "gl/glText.h"
#include <vector>
#include <iostream>

// This needs adjustment for every game depending on how they scale, it is
// used to calculate how much we scale the drawings based on the distance.
const int GAME_UNIT_MAGIC = 400;

const float PLAYER_HEIGHT = 5.25f;
const float PLAYER_WIDTH = 2.f;
const float EYE_HEIGHT = 4.5f;

const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;

// Create 3 different fonts to use depending on how far the target is away.
const int FONT_SMALL_HEIGHT = 7;
const int FONT_SMALL_WIDTH = 4;

const int FONT_MEDIUM_HEIGHT = 15;
const int FONT_MEDIUM_WIDTH = 9;

const int FONT_LARGE_HEIGHT = 20;
const int FONT_LARGE_WIDTH = 11;

GL::Font fontSmall;
GL::Font fontMedium;
GL::Font fontLarge;

/**
 * @brief Draws the stats of a player, i.e the health & armor.
 * @brief - Armor is only drawn when above 0 & enabled in ESP settings.
 * @brief - Health is only drawn when below 100 & enabled in ESP settings.
 *
 * @param player The player to draw the stats of.
 * @param box The box of the player esp to add the stats to.
 */
void DrawStats(SDK::Player* player, Rect box)
{
	float width = box.w / 5;

	if (player->armor > 0 && settings::esp::displayArmor) {
		float filledHeight = (box.h / 100) * player->armor;
		float emptyHeight = box.h - filledHeight;
		Rect emptyPart = { box.x - width - 2, box.y, width, emptyHeight };
		Rect FilledPart = { emptyPart.x, emptyPart.y + emptyHeight, width, filledHeight };

		GL::DrawFilledRect(emptyPart, rgb::gray);
		GL::DrawFilledRect(FilledPart, rgb::lightgray);
	}

	if (player->health < 100 && settings::esp::displayHealth) {
		float filledHeight = (box.h / 100) * player->health;
		float emptyHeight = box.h - filledHeight;
		Rect emptyPart = { box.x + box.w + 2, box.y, width, emptyHeight };
		Rect FilledPart = { emptyPart.x, emptyPart.y + emptyHeight, width, filledHeight };

		GL::DrawFilledRect(emptyPart, rgb::red);
		GL::DrawFilledRect(FilledPart, rgb::green);
	}
}

/**
 * @brief Draws the name of a player + the distance above its bounding box.
 * @brief - The distance is only drawn if enabled in ESP settings.
 *
 * @param name The player name to draw.
 * @param box The box of the player esp to add the name to.
 * @param distance The distance from the local player to this player.
 * @param color The color to draw the name with.
 * @param font The font to draw the name with.
 */
void DrawName(const char* name, Rect box, float distance, const GLubyte* color, GL::Font& font)
{
	char info[100];

	if (settings::esp::displayDistance) {
		sprintf_s(info, "%s (%dm)", name, (int)distance);
	}
	else {
		sprintf_s(info, "%s", name);
	}
	float textX = font.centerText(box.x, box.w, font.GetTextWidth(info));
	float textY = box.y - font.height / 2;
	font.Print(textX, textY, color, info);
}

/**
 * @brief Draws the current weapon name of a player + the ammo inside it.
 *
 * @param weapon The weapon the player has equipped.
 * @param box The box of the player esp to add the name to.
 * @param font The font to draw the weapon with.
 */
void DrawWeapon(SDK::Weapon* weapon, Rect box, GL::Font& font)
{
	char info[100];
	sprintf_s(info, "%s (%d)", weapon->data->name, weapon->reserveData->ammo);
	float textX = font.centerText(box.x, box.w, font.GetTextWidth(info));
	float textY = box.y + box.h + 5 + font.height / 2;
	font.Print(textX, textY, rgb::lightgray, info);

}

/**
 * @brief Calculates the bounding box around position of a player on the screen
 * given it's distance.
 *
 * @param screen The position of the player on the screen.
 * @param distance The distance to the player from our local player.
 */
Rect GetBoundingBox(const Vector2& screen, float distance)
{
	float scale = (GAME_UNIT_MAGIC / distance);

	float x = screen.x - scale;
	float y = screen.y - scale * PLAYER_ASPECT_RATIO;
	float width = scale * 2;
	float height = scale * PLAYER_ASPECT_RATIO * 2;

	return Rect{ x, y, width, height };
}

/**
 * @brief Draws a single player's ESP box on the screen. Does not draw:
 * @brief - Dead players
 * @brief - Players not within our FOV
 * @brief - Players not matching the ESP settings
 *
 * @param player The player to draw.
 * @param matrix The current view matrix of the game.
 */
void esp::DrawPlayer(SDK::Player* player, float matrix[16])
{
	// Don't draw dead players, theres no reason to in this game.
	if (!player->isAlive()) { return; }

	Vector2 screen;
	Vector3 center = player->headPos;
	center.z = center.z - EYE_HEIGHT + PLAYER_HEIGHT / 2;

	// Get the position of the enemy on our screen, returns false if the player isn't within our fov.
	if (!WorldToScreen(center, screen, matrix, data::gameRect.right, data::gameRect.bottom)) { return; }

	// Check whether we are drawing an enemy or a friendly.
	SDK::GameMode mode = *reinterpret_cast<SDK::GameMode*>(data::moduleBaseAddress + 0x10F49C);
	auto color = player->isEnemy(data::localPlayer->team, mode) ? rgb::red : rgb::green;

	// Distance to the player is important to do the drawing math later on
	float distance = GetDistance(data::localPlayer->headPos, player->headPos);
	if (distance > settings::esp::maxRenderDistance) { return; }

	// Get the box dimensions and determine what tickness and font to use based on distance.
	Rect box = GetBoundingBox(screen, distance);
	float thickness = distance > 55 ? 1.f : 1.5f;
	GL::Font& font = distance > 55 ? fontSmall : distance > 20 ? fontMedium : fontLarge;
	Vector2 origin{ data::gameRect.right / 2, data::gameRect.bottom };

	// Do the actual drawing
	GL::DrawOutline(box, thickness, color);
	DrawStats(player, box);
	if (settings::esp::displayLine) { GL::DrawLine(origin.x, origin.y, screen.x, screen.y + box.h / 2, 1.f, color); }
	if (settings::esp::displayName) { DrawName(player->name, box, distance, color, font); }
	if (settings::esp::displayWeapon) { DrawWeapon(player->currentWeapon, box, font); }
}

/**
 * @brief Draws all players in the lobby on the screen.
 * @brief - Dead players
 * @brief - Players not within our FOV
 * @brief - Players not matching the ESP settings
 */
void esp::DrawAllPlayers()
{
	HDC currHDC = wglGetCurrentDC();
	// Check whether we need to rebuild any of our fonts (if the hdc went out of sync)
	if (!fontSmall.built || currHDC != fontSmall.hdc) { fontSmall.Build(FONT_SMALL_HEIGHT, FONT_SMALL_WIDTH); }
	if (!fontMedium.built || currHDC != fontMedium.hdc) { fontMedium.Build(FONT_MEDIUM_HEIGHT, FONT_MEDIUM_WIDTH); }
	if (!fontLarge.built || currHDC != fontLarge.hdc) { fontLarge.Build(FONT_LARGE_HEIGHT, FONT_LARGE_WIDTH); }

	GL::SetupOrtho();
	float matrix[16];
	glGetIntegerv(GL_VIEWPORT, GL::viewport);

	uintptr_t matrixAddr = data::moduleBaseAddress + 0x101AE8;
	int32_t playerCount = *reinterpret_cast<uintptr_t*>(data::moduleBaseAddress + 0x10F500);
	uintptr_t entityList = *reinterpret_cast<uintptr_t*>(data::moduleBaseAddress + 0x10F4F8);
	memcpy(reinterpret_cast<BYTE*>(matrix), reinterpret_cast<BYTE*>(matrixAddr), sizeof(matrix));

	// Draw each entity.
	for (int i = 1; i < playerCount; i++)
	{
		int offset = 4 * i;
		DrawPlayer(*reinterpret_cast<SDK::Player**>(entityList + offset), matrix);
	}

	GL::DrawCircle(data::gameRect.right / 2, data::gameRect.bottom / 2, settings::aimbot::radius * 3, 100, 1, rgb::green);
	GL::RestoreGL();
	if (settings::aimbot::wallhack) { glDisable(GL_DEPTH_TEST); }
}