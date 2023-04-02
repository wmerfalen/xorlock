#include <iostream>
#include "npc-id.hpp"

namespace npc_id {
	npc_id_t next() {
		return id++;
	}
};
