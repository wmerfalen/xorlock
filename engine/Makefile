CXX=g++-10
gdbflags = -ggdb -g
DEBUG_FLAGS=-DSHOW_RETICLE=1 \
						-DDRAW_SPETSNAZ_PREFIRE_LINE=1
BUILD_DIR=build
C_STD_FLAGS=-std=c++20
SRC_DIR=src

build-dir:
	if [[ ! -d $(BUILD_DIR) ]]; then mkdir -p $(BUILD_DIR); fi ; exit 0

all: actor background behaviour-tree bullet \
	circle clock coordinates cursor draw gameplay \
	graphical-decay hallway line map movement \
	npc-spetsnaz
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
		$(BUILD_DIR)/gameplay.o \
		$(BUILD_DIR)/graphical-decay.o \
		$(BUILD_DIR)/hallway.o \
		$(BUILD_DIR)/line.o \
		$(BUILD_DIR)/map.o \
		$(BUILD_DIR)/movement.o \
		$(BUILD_DIR)/npc-spetsnaz.o \
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


gameplay: $(SRC_DIR)/gameplay.cpp
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS) \
		$(SRC_DIR)/gameplay.cpp \
		-c -o $(BUILD_DIR)/gameplay.o

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

