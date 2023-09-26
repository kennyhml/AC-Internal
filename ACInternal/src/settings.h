#pragma once

namespace settings
{
	namespace player
	{
		extern int speed;
		extern bool godMode;
		extern bool antiGravity;
		extern bool flyHack;
		extern bool ghostMode;
		extern bool bSpeed;
	}

	namespace weapon
	{
		extern bool alwaysHeadshot;
		extern bool noRecoil;
		extern bool rapidFire;
		extern bool infiniteAmmo;
	}

	namespace esp
	{
		extern bool enabled;
		extern bool displayName;
		extern bool displayArmor;
		extern bool displayHealth;
		extern bool displayWeapon;
		extern bool displayDistance;
		extern bool displayLine;
		extern int maxRenderDistance;
	}

	namespace aimbot
	{
		extern int radius;
		extern int maxDistance;
		extern int minDistance;
	}


	// movement related settings
	// extern int knockback;
	// extern const char* selectedWeapon;
	// 
	// 
	// // weapon related settings
	// extern bool noRecoil;
	// extern bool infiniteAmmo;
	// extern bool instantReload;
	// extern bool rapidFire;
	// extern bool noSpray;
	// extern bool noShake;
	// extern bool fullAuto;
	// extern bool oneTap;
	// extern bool aimbot;
	// extern bool freezeEnemies;
	// extern bool speedEnabled;
	// extern bool esp;
	// 
	// extern int aimbotMinDistance;
	// extern int aimbotMaxDistance;
	// extern int aimbotRadius;
}