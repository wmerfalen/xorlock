#ifndef __BEHAVIOUR_TREE_HEADER__
#define __BEHAVIOUR_TREE_HEADER__
#include <iostream>
#include <forward_list>
#include <functional>
#include "debug.hpp"
#include "extern.hpp"

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

  void run(BehaviourTree* t){
    for(const auto& n : t->nodes){
      for(auto& c : n.logic){
        if(c(t->data)){
          if(n.type == NODE_SELECTOR){
            return;
          }
        }else{
          return;
        }
      }
    }
  }
};
#endif
