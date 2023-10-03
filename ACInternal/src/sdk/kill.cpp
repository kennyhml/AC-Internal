#include "sdk.h"
#include "../data.h"

/**
 * @brief Calls assault cubes kill functionality to kill the given entity without conditions,
 * doesnt work online.
 *
 * Little explanation how I found this functionality: I was first lead to this function by
 * checking what writes to the death status of entities which lead me to 0x00425260 which
 * I reversed in IDA and considered it to be something like a killEvent function since
 * it essentially update alot of the entities states such as movement speed, death state, weapon etc.
 *
 * What I did not see is the message that should appear when you kill an entity, so I knew there still
 * has to be a bigger function that calls this killEvent function, so I jumped through the xrefs and
 * quickly found an interesting function at 0x426C60 with a bunch of format strings in memory that look
 * like our kill messages.
 *
 * Didnt take me long to figure out that this is indeed the function of interest, later down the line it
 * calls the killEvent and then increments the death counter of the entity, so yup this is what we are looking for.
 *
 * Now the hard part, figuring out how to call the function. First I had to reverse the parameters, IDA for some
 * reason believed that the first argument was the result which is later returned so that kinda threw me off, I then
 * checked where this function was called and to no surprise, the argument that was passed as 'result' was actually
 * just the weapon ID of the weapon used and in reality its most likely a void function. So the signature turned out as:
 *
 * void __usercall kill(int weaponID@<eax>, Player *entity@<ecx>, int dmgSource, bool someBool)
 *
 * Still dont know what someBool does, I just pass true for it and it doesnt really seem to matter.
 *
 * So first we move 5 into eax which is the sniper weapon ID, then true into edx which is our unknown bool, we have to push edx
 * because its part of the new frame stack. Then we load ebx as our local player address for the dmgSource argument since we
 * are trying to tell the game that we killed this entity and push it as its, again, part of the new frame stack.
 *
 * Last but not least we have to load the target entity into esi and then load esi into exc, we cant load it directly into exc
 * for whatever stupid reason. Once thats done we have setup our stack and can just call the function now, after that we clean
 * up the stack by moving the stack pointer down 8 bytes (since we pushed edx and ebx).
 *
 * @param entity The entity to kill
 */
void SDK::kill(SDK::Player* entity)
{
	DWORD killFunc = 0x426C60;
	__asm {
		mov eax, 5;
		mov edx, 1;
		push edx;
		mov ebx, data::localPlayer;
		push ebx;

		mov esi, entity;
		mov ecx, esi;

		call killFunc;
		add esp, 8;
	}
}