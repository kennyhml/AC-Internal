# AC-Internal
An internal cheat for [Assault Cube v1.2.0.2.](https://assault.cubers.net/) written purely in C++ with help from [Guided Hacking](https://guidedhacking.com/).

The cooler brother of [AC-External](https://github.com/kennyhml/AC-External).
### Tools used to reserve the game:
- [Cheat Engine](https://github.com/cheat-engine/cheat-engine)
- [ReClassNET](https://github.com/ReClassNET/ReClass.NET)
- [IDA 8.3](https://hex-rays.com/ida-pro/)

### Current Features:
- ESP (armor, health, name, distance, weapon, ammo, team)
- Aimbot (much faster than the external variant)
- Wallhack (disable depth test to trick the game into thinking you can hit the target, credit to some guy on GH)
- [Thanos Snap][kill]; insta kill everyone (credit to myself and the 4h it took me to reverse the calling conventions XD)
- [Show enemy positions on the minimap and overlay map][map] (credit to myself)
- [Weapon and hit location spoofing to get 'headshot' multiplier][headshot] (credit to some guy on GH & myself)
- [Godmode][godmode] (better than external variant because it doesnt just freeze the value)
- No Recoil, Rapid Fire, Infinite Ammo... (same as the external variant, but only applies to local player this time)

Teleport to targetted entity: ㅤㅤㅤㅤㅤㅤㅤㅤㅤㅤㅤㅤㅤㅤAimbot & Wallhack (works online):
<p float="left">
  <img src="demo/demoTeleport.gif" width="410" height="245" />
  <img src="demo/demoWallhackAimbot.gif" width="410" height="245" />
</p>


[godmode]: https://github.com/kennyhml/AC-Internal/blob/bdcccaf0d971875973c4ae83b4609dd55523b33a/ACInternal/src/hooks/health.cpp#L8
[headshot]: https://github.com/kennyhml/AC-Internal/blob/bdcccaf0d971875973c4ae83b4609dd55523b33a/ACInternal/src/hooks/headshot.cpp#L7
[map]: https://github.com/kennyhml/AC-Internal/blob/bdcccaf0d971875973c4ae83b4609dd55523b33a/ACInternal/src/hooks/map.cpp#L6
[kill]: https://github.com/kennyhml/AC-Internal/blob/bf304a478de419e63c95180235ac05c4f34fb233/ACInternal/src/sdk/kill.cpp#L40
