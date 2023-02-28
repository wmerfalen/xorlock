#ifndef __NPC_ID_HEADER__
#define __NPC_ID_HEADER__
#include <iostream>

using npc_id_t = uint64_t;
namespace npc_id {
	static npc_id_t id = 0;
	npc_id_t next() {
		return id++;
	}
};

#endif
