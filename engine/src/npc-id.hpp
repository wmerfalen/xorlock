#ifndef __NPC_ID_HEADER__
#define __NPC_ID_HEADER__
#include <iostream>

using npc_id_t = uint64_t;
namespace npc_id {
	npc_id_t next() {
		static npc_id_t id = 0;
		return id++;
	}
};

#endif
