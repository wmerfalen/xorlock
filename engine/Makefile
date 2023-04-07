CXX=g++-10
GDB_FLAGS = -ggdb -g
DEBUG_FLAGS=-DSHOW_RETICLE=1 \
						-DDRAW_SPETSNAZ_PREFIRE_LINE=1 \
						-DDRAW_COLLISIONS=1
DEBUG_FLAGS=
GDB_FLAGS=
BUILD_DIR=build
C_STD_FLAGS=-std=c++20
SRC_DIR=src
buildobj = actor background behaviour-tree bullet \
	circle clock coordinates cursor draw \
	draw-state/init draw-state/player \
	draw-state/reticle gameplay gameplay/npc-spawning \
	graphical-decay hallway line map movement \
	mp5 npc-id npc-spetsnaz player \
	randomized-maps/building-generator \
	randomized-maps/fragments/square \
	rng tick timeline \
	triangle viewport world

default: all
objects = $(patsubst %,$(BUILD_DIR)/%.o,$(buildobj))

clean: 
	rm -f $(BUILD_DIR)/* $(BUILD_DIR)/gameplay/* $(BUILD_DIR)/draw-state/* \
		$(BUILD_DIR)/randomized-maps/fragments/* $(BUILD_DIR)/randomized-maps/*

build-dir:
	mkdir -p $(BUILD_DIR) $(BUILD_DIR)/gameplay $(BUILD_DIR)/draw-state $(BUILD_DIR)/randomized-maps/fragments

all: $(objects)
	$(CXX) $(DEBUG_FLAGS) $(GDB_FLAGS) $(C_STD_FLAGS)  \
		./src/main.cpp -lSDL2 \
		$(objects) \
		-o ./build/xorlock

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) $(DEBUG_FLAGS) $(GDB_FLAGS) $(C_STD_FLAGS)  \
		-c $(FLAGS) -o $@ $<

-include $(objects:.o=.d)
