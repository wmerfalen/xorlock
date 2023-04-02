#include "behaviour-tree.hpp"

namespace btree {
	void run(BehaviourTree* t) {
		for(const auto& n : t->nodes) {
			for(auto& c : n.logic) {
				if(c(t->data)) {
					if(n.type == NODE_SELECTOR) {
						return;
					}
				} else {
					return;
				}
			}
		}
	}
};
