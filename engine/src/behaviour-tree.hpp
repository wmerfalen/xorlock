#ifndef __BEHAVIOUR_TREE_HEADER__
#define __BEHAVIOUR_TREE_HEADER__
#include <iostream>
#include <forward_list>
#include <functional>
#include "debug.hpp"


namespace btree {
	enum NodeType : uint16_t {
		NODE_SELECTOR,
		NODE_SEQUENCE,
	};
	struct Node {
		using FunctionType = std::function<bool(void*)>;
		std::forward_list<FunctionType> logic;
		NodeType type;
	};
	struct BehaviourTree {
		std::forward_list<Node> nodes;
		void* data;
	};

	void run(BehaviourTree* t);
};
#endif
