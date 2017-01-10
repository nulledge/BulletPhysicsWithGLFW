CC=g++
OUTPUT=exe.out

LIB_COLLISION=BulletCollision_gmake_x64_release
LIB_DYNAMICS=BulletDynamics_gmake_x64_release
LIB_LINEARMATH=LinearMath_gmake_x64_release

BULLET_PHYSICS=BulletPhysics
BULLET_PHYSICS_DEPENDENCY=-L$(LIB_PATH) -l$(LIB_COLLISION) -l$(LIB_DYNAMICS) -l$(LIB_LINEARMATH)

LIB_PATH=$(BULLET_PHYSICS)/bin
INC_PATH=$(BULLET_PHYSICS)/src
SRC_PATH=src
OBJ_PATH=obj

all : $(OBJ_PATH)/main.o
	$(CC) $(OBJ_PATH)/main.o -o $(OUTPUT) $(BULLET_PHYSICS_DEPENDENCY) -I$(INC_PATH)

$(OBJ_PATH)/main.o : $(SRC_PATH)/main.cpp
	$(CC) -c $(SRC_PATH)/main.cpp -o $(OBJ_PATH)/main.o -I$(INC_PATH)
