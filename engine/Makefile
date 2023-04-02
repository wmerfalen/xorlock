CXX=g++-10
gdbflags = -ggdb -g
DEBUG_FLAGS=-DSHOW_RETICLE=1 \
						-DDRAW_SPETSNAZ_PREFIRE_LINE=1
BUILD_DIR=build
C_STD_FLAGS=-std=c++20
SRC_DIR=src

build-dir:
	mkdir -p $(BUILD_DIR) $(BUILD_DIR)/gameplay $(BUILD_DIR)/draw-state $(BUILD_DIR)/randomized-maps/fragments

all: actor background behaviour-tree bullet \
	circle clock coordinates cursor draw \
	draw-state-init draw-state-player \
	draw-state-reticle gameplay gameplay-npc-spawning \
	graphical-decay hallway line map movement \
	mp5 npc-id npc-spetsnaz player \
	randomized-maps-building-generator \
	randomized-maps-fragments-square \
	rng tick timeline \
	triangle viewport world
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS)  \
		./src/main.cpp -lSDL2 \
		$(BUILD_DIR)/actor.o \
		$(BUILD_DIR)/background.o \
		$(BUILD_DIR)/behaviour-tree.o \
		$(BUILD_DIR)/bullet.o \
		$(BUILD_DIR)/circle.o \
		$(BUILD_DIR)/clock.o \
		$(BUILD_DIR)/coordinates.o \
		$(BUILD_DIR)/cursor.o \
		$(BUILD_DIR)/draw.o \
		$(BUILD_DIR)/draw-state/init.o \
		$(BUILD_DIR)/draw-state/player.o \
		$(BUILD_DIR)/draw-state/reticle.o \
		$(BUILD_DIR)/gameplay.o \
		$(BUILD_DIR)/gameplay/npc-spawning.o \
		$(BUILD_DIR)/graphical-decay.o \
		$(BUILD_DIR)/hallway.o \
		$(BUILD_DIR)/line.o \
		$(BUILD_DIR)/map.o \
		$(BUILD_DIR)/movement.o \
		$(BUILD_DIR)/mp5.o \
		$(BUILD_DIR)/npc-id.o \
		$(BUILD_DIR)/npc-spetsnaz.o \
		$(BUILD_DIR)/player.o \
		$(BUILD_DIR)/randomized-maps/building-generator.o \
		$(BUILD_DIR)/randomized-maps/fragments/square.o \
		$(BUILD_DIR)/rng.o \
		$(BUILD_DIR)/tick.o \
		$(BUILD_DIR)/timeline.o \
		$(BUILD_DIR)/triangle.o \
		$(BUILD_DIR)/viewport.o \
		$(BUILD_DIR)/world.o \
		-o ./build/xorlock

npc-spetsnaz: $(SRC_DIR)/npc-spetsnaz.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/npc-spetsnaz.cpp \
		-c -o $(BUILD_DIR)/npc-spetsnaz.o

actor: 
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/actor.cpp \
		-c -o $(BUILD_DIR)/actor.o

	
background: 
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/background.cpp \
		-c -o $(BUILD_DIR)/background.o

behaviour-tree: $(SRC_DIR)/behaviour-tree.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/behaviour-tree.cpp \
		-c -o $(BUILD_DIR)/behaviour-tree.o

bullet: $(SRC_DIR)/bullet.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/bullet.cpp \
		-c -o $(BUILD_DIR)/bullet.o

circle: $(SRC_DIR)/circle.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/circle.cpp \
		-c -o $(BUILD_DIR)/circle.o

clock: $(SRC_DIR)/clock.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/clock.cpp \
		-c -o $(BUILD_DIR)/clock.o

coordinates: $(SRC_DIR)/coordinates.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/coordinates.cpp \
		-c -o $(BUILD_DIR)/coordinates.o

cursor: $(SRC_DIR)/cursor.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/cursor.cpp \
		-c -o $(BUILD_DIR)/cursor.o

draw: $(SRC_DIR)/draw.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/draw.cpp \
		-c -o $(BUILD_DIR)/draw.o

draw-state-init: $(SRC_DIR)/draw-state/init.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/draw-state/init.cpp \
		-c -o $(BUILD_DIR)/draw-state/init.o

draw-state-player: $(SRC_DIR)/draw-state/player.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/draw-state/player.cpp \
		-c -o $(BUILD_DIR)/draw-state/player.o

draw-state-reticle: $(SRC_DIR)/draw-state/reticle.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/draw-state/reticle.cpp \
		-c -o $(BUILD_DIR)/draw-state/reticle.o


gameplay: $(SRC_DIR)/gameplay.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/gameplay.cpp \
		-c -o $(BUILD_DIR)/gameplay.o

gameplay-npc-spawning: $(SRC_DIR)/gameplay/npc-spawning.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/gameplay/npc-spawning.cpp \
		-c -o $(BUILD_DIR)/gameplay/npc-spawning.o


graphical-decay: $(SRC_DIR)/graphical-decay.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/graphical-decay.cpp \
		-c -o $(BUILD_DIR)/graphical-decay.o

hallway: $(SRC_DIR)/hallway.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/hallway.cpp \
		-c -o $(BUILD_DIR)/hallway.o

line: $(SRC_DIR)/line.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/line.cpp \
		-c -o $(BUILD_DIR)/line.o

map: $(SRC_DIR)/map.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/map.cpp \
		-c -o $(BUILD_DIR)/map.o

movement: $(SRC_DIR)/movement.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/movement.cpp \
		-c -o $(BUILD_DIR)/movement.o

mp5: $(SRC_DIR)/mp5.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/mp5.cpp \
		-c -o $(BUILD_DIR)/mp5.o

npc-id: $(SRC_DIR)/npc-id.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/npc-id.cpp \
		-c -o $(BUILD_DIR)/npc-id.o

player: $(SRC_DIR)/player.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/player.cpp \
		-c -o $(BUILD_DIR)/player.o

randomized-maps-building-generator: $(SRC_DIR)/randomized-maps/building-generator.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/randomized-maps/building-generator.cpp \
		-c -o $(BUILD_DIR)/randomized-maps/building-generator.o

randomized-maps-fragments-square: $(SRC_DIR)/randomized-maps/fragments/square.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/randomized-maps/fragments/square.cpp \
		-c -o $(BUILD_DIR)/randomized-maps/fragments/square.o

rng: $(SRC_DIR)/rng.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/rng.cpp \
		-c -o $(BUILD_DIR)/rng.o


tick: $(SRC_DIR)/tick.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/tick.cpp \
		-c -o $(BUILD_DIR)/tick.o

timeline: $(SRC_DIR)/timeline.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/timeline.cpp \
		-c -o $(BUILD_DIR)/timeline.o

triangle: $(SRC_DIR)/triangle.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/triangle.cpp \
		-c -o $(BUILD_DIR)/triangle.o

viewport: $(SRC_DIR)/viewport.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/viewport.cpp \
		-c -o $(BUILD_DIR)/viewport.o

world: $(SRC_DIR)/world.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/world.cpp \
		-c -o $(BUILD_DIR)/world.o

