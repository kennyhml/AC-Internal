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
		extern bool mapReveal;
	}

	namespace aimbot
	{
		extern int radius;
		extern int maxDistance;
		extern int minDistance;
		extern bool autoFire;
		extern bool turretMode;
		extern bool wallhack;
	}
}