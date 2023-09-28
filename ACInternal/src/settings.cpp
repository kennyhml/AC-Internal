#include "settings.h"


namespace settings
{
	namespace player
	{
		int speed = 3;
		bool godMode = false;
		bool antiGravity = false;
		bool flyHack = false;
		bool ghostMode = false;
		bool bSpeed = false;
	}
	namespace weapon
	{
		bool alwaysHeadshot = false;
		bool noRecoil = false;
		bool rapidFire = false;
		bool infiniteAmmo = false;
	}

	namespace esp
	{
		bool enabled = true;
		bool displayName = true;
		bool displayArmor = true;
		bool displayHealth = true;
		bool displayWeapon = true;
		bool displayDistance = true;
		bool displayLine = true;
		int maxRenderDistance = 200;
	}

	namespace aimbot
	{
		int radius = 30;
		int maxDistance = 200;
		int minDistance = 0;
		bool autoFire = false;
		bool turretMode = false;
		bool wallhack = false;

	}

}