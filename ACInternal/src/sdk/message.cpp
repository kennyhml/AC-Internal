#include "sdk.h"
#include "../tools/mem.h"

namespace SDK
{
	// f0: green, f1: blue, f2: yellow, f3: red, f4: gray, f5: white, f6: brown, f7: ugly red
	// f8: puple, f9: orange, fa: pink, fb: darker red fc: darker brown
	tServerMessage sendServerMessage = (tServerMessage)(GetMBA() + 0x6B060);
	tPlayerMessage sendPlayerMesage = (tPlayerMessage)(GetMBA() + 0x8E80);
	tAllMessage sendAllMessage = (tAllMessage)(GetMBA() + 0x90F0);
}