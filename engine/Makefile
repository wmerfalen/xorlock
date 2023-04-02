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
	circle npc-spetsnaz
	$(CXX) $(DEBUG_FLAGS) $(gdbflags) $(C_STD_FLAGS)  \
		./src/main.cpp -lSDL2 \
		$(BUILD_DIR)/npc-spetsnaz.o \
		$(BUILD_DIR)/actor.o \
		$(BUILD_DIR)/background.o \
		$(BUILD_DIR)/behaviour-tree.o \
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

