#include "tools/mem.h"
#include "sdk/sdk.h"
#include "settings.h"
#include <windows.h>
#include "hooks/hook.h"
#include <iostream>
#include <TlHelp32.h>
#include "glDraw.h"
#include "glText.h"
#include "data.h"
#include "sdk/player.h"


typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
twglSwapBuffers wglSwapBuffersGateway;
bool eject = false;

GL::Font glFont;
const int FONT_HEIGHT = 15;
const int FONT_WIDTH = 9;


const char* example = "ESP box";
const char* example2 = "Hello world";

std::vector<SDK::Player*> LoadPlayers()
{

	uint32_t count = *(uintptr_t*)(data::moduleBaseAddress + 0x10F500);
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

void DrawPlayer(SDK::Player* player, SDK::Player* localPlayer, float matrix[16])
{
	if (!player->isAlive()) { return; }

	Vector2 position;

	if (!WorldToScreen(player->feetPos, position, matrix, data::gameRect.right, data::gameRect.bottom)) { return; }


	int distance = (int)GetDistance(localPlayer->headPos, player->headPos);
	int rectWidth = 1100 / distance;
	int rectHeight = 2000 / distance;

	// calculate the topleft position of our bounding rectangle
	int x = position.x - (rectWidth / 2);
	int y = position.y - rectHeight;

	GL::DrawOutline(x, y, rectWidth, rectHeight, 2.f, rgb::red);
}

void Draw()
{
	HDC currHDC = wglGetCurrentDC();
	if (!glFont.built || currHDC != glFont.hdc)
	{
		glFont.Build(FONT_HEIGHT);
	}

	GL::SetupOrtho();

	SDK::Player* localPlayer = (SDK::Player*)*(uintptr_t*)(data::moduleBaseAddress + 0x10F4F4);

	float matrix[16];
	memcpy((BYTE*)matrix, (BYTE*)(data::moduleBaseAddress + 0x101AE8), sizeof(matrix));

	for (SDK::Player* player : LoadPlayers())
	{
		DrawPlayer(player, localPlayer, matrix);
	}

	GL::RestoreGL();
}

/**
 * @brief wglSwapBuffers hook function to execute our cheats code as the
 * function gets executed once every frame.
 *
 * A trampoline hook is placed in the original flo of the function which jumps
 * to this function, after we finished our execution we pass execution on to our
 * gateway which will execute the bytes we originally stole and then jump back
 * to the appropriate location in the original function.
 */
BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
	SDK::Player* localPlayer = (SDK::Player*)*(uintptr_t*)(data::moduleBaseAddress + 0x10F4F4);

	if (GetAsyncKeyState(VK_DELETE) & 1) {
		eject = true;
		if (settings::player::godMode) { hooks::health.Disable(); }
		if (settings::weapon::alwaysHeadshot) { hooks::headshot.Disable(); }
		if (settings::weapon::noRecoil) { hooks::noRecoil.Disable(); }
		if (settings::weapon::rapidFire) { hooks::rapidFire.Disable(); }
		if (settings::weapon::infiniteAmmo) { hooks::ammo.Disable(); }
		if (settings::player::bSpeed) { hooks::ToggleSpeed(false); }
	}

	if (GetAsyncKeyState(VK_F2) & 1) {
		settings::player::godMode = !settings::player::godMode;
		SDK::sendAllMessage(settings::player::godMode ? "<Godmode \f0[ON]\f5!>" : "<Godmode \f3[OFF]\f5!>");
		hooks::health.Toggle();
	}

	if (GetAsyncKeyState(VK_F3) & 1) {
		settings::weapon::alwaysHeadshot = !settings::weapon::alwaysHeadshot;
		SDK::sendAllMessage(settings::weapon::alwaysHeadshot ? "<Headshots \f0[ON]\f5!>" : "<Headshots \f3[OFF]\f5!>");
		hooks::headshot.Toggle();
	}

	if (GetAsyncKeyState(VK_F4) & 1) {
		settings::weapon::noRecoil = !settings::weapon::noRecoil;
		SDK::sendAllMessage(settings::weapon::noRecoil ? "<No Recoil \f0[ON]\f5!>" : "<No Recoil \f3[OFF]\f5!>");
		hooks::noRecoil.Toggle();
	}

	if (GetAsyncKeyState(VK_F5) & 1) {
		settings::weapon::rapidFire = !settings::weapon::rapidFire;
		SDK::sendAllMessage(settings::weapon::rapidFire ? "<Rapid Fire \f0[ON]\f5!>" : "<Rapid Fire \f3[OFF]\f5!>");
		hooks::rapidFire.Toggle();
	}

	if (GetAsyncKeyState(VK_F6) & 1) {
		settings::weapon::infiniteAmmo = !settings::weapon::infiniteAmmo;
		SDK::sendAllMessage(settings::weapon::infiniteAmmo ? "<Inf. Ammo \f0[ON]\f5!>" : "<Inf. Ammo \f3[OFF]\f5!>");
		hooks::ammo.Toggle();
	}

	if (GetAsyncKeyState(VK_F7) & 1) {
		settings::player::bSpeed = !settings::player::bSpeed;
		SDK::sendAllMessage(settings::player::bSpeed ? "<Speed \f0[ON]\f5!>" : "<Speed \f3[OFF]\f5!>");
		hooks::ToggleSpeed(settings::player::bSpeed);
	}

	Draw();

	return wglSwapBuffersGateway(hDc);
}

DWORD WINAPI HackThread(HMODULE hModule)
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "DLL injected!\n";

	SDK::sendAllMessage("\f0<Injected successfully!>");

	SDK::Player* localPlayer = (SDK::Player*)*(uintptr_t*)(data::moduleBaseAddress + 0x10F4F4);
	hooks::localPlayerAddress = (uintptr_t)localPlayer;
	auto swapBuffersHook = hooks::Hook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);

	swapBuffersHook.Enable();
	hooks::console.Enable();
	std::cout << "[+] Gateway at 0x" << std::uppercase << std::hex << wglSwapBuffersGateway << std::endl;

	while (!eject) { Sleep(100); }
	std::cout << "[+] Ejecting!" << std::endl;

	Sleep(100);
	swapBuffersHook.Disable();
	hooks::console.Disable();

	if (f) { fclose(f); }
	Sleep(200);
	FreeConsole();
	SDK::sendAllMessage("\f0<Ejected successfully!>");


	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, NULL, nullptr);
		if (!hThread) { return FALSE; }
		CloseHandle(hThread);
	}

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}