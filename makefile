CPPC=g++ -std=c++11
CC=gcc
MKDIR=mkdir
OUTPUT=exe.out


BULLET_PHYSICS=BulletPhysics

LIB_COLLISION=BulletCollision_gmake_x64_release
LIB_DYNAMICS=BulletDynamics_gmake_x64_release
LIB_LINEARMATH=LinearMath_gmake_x64_release

BULLET_LIB_PATH=$(BULLET_PHYSICS)/bin
BULLET_INC_PATH=$(BULLET_PHYSICS)/src

BULLET_PHYSICS_DEPENDENCY=-L$(BULLET_LIB_PATH) -l$(LIB_COLLISION) -l$(LIB_DYNAMICS) -l$(LIB_LINEARMATH)


GLFW=glfw

LIB_GLFW=glfw3

GLFW_LIB_PATH=$(GLFW)/src
GLFW_INC_PATH=$(GLFW)/include

GLFW_BASIC_DEPENDENCY=-L$(GLFW_LIB_PATH) -l$(LIB_GLFW)
GLFW_MAC_DEPENDENCY=$(GLFW_BASIC_DEPENDENCY) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
GLFW_DEPENDENCY=$(GLFW_MAC_DEPENDENCY)


GLAD=$(GLFW)/deps

GLAD_SRC_PATH=$(GLAD)
GLAD_INC_PATH=$(GLAD)


LINMATH=$(GLFW)/deps

LINMATH_INC_PATH=$(LINMATH)


GLM=glm

GLM_INC_PATH=$(GLM)


BIN_PATH=bin
SRC_PATH=src
OBJ_PATH=obj

final : $(OBJ_PATH)/main.o $(OBJ_PATH)/glad.o $(OBJ_PATH)/config.o $(OBJ_PATH)/app.o $(BIN_PATH)
	$(CPPC) $(OBJ_PATH)/main.o $(OBJ_PATH)/glad.o $(OBJ_PATH)/config.o $(OBJ_PATH)/app.o -o $(BIN_PATH)/$(OUTPUT) $(BULLET_PHYSICS_DEPENDENCY) $(GLFW_DEPENDENCY)

$(OBJ_PATH)/main.o : $(SRC_PATH)/main.cpp $(SRC_PATH)/UTIL.h $(GLM)/glm/glm.hpp $(OBJ_PATH)
	$(CPPC) -c $(SRC_PATH)/main.cpp -o $(OBJ_PATH)/main.o -I$(BULLET_INC_PATH) -I$(GLFW_INC_PATH) -I$(GLAD_INC_PATH) -I$(LINMATH_INC_PATH) -I$(SRC_PATH) -I$(GLM_INC_PATH)

$(OBJ_PATH)/app.o : $(SRC_PATH)/Application/Application.hpp $(SRC_PATH)/Application/Application.cpp $(OBJ_PATH)
	$(CPPC) -c $(SRC_PATH)/Application/Application.cpp -o $(OBJ_PATH)/app.o -I$(GLFW_INC_PATH) -I$(SRC_PATH)

$(OBJ_PATH)/config.o : $(SRC_PATH)/Application/WindowConfig.hpp $(SRC_PATH)/Application/WindowConfig.cpp $(OBJ_PATH)
	$(CPPC) -c $(SRC_PATH)/Application/WindowConfig.cpp -o $(OBJ_PATH)/config.o -I$(GLFW_INC_PATH) -I$(SRC_PATH)

$(OBJ_PATH)/glad.o : $(GLAD_SRC_PATH)/glad.c $(OBJ_PATH)
	$(CC) -c $(GLAD_SRC_PATH)/glad.c -o $(OBJ_PATH)/glad.o -I$(GLAD_INC_PATH)

$(OBJ_PATH) :
	$(MKDIR) $(OBJ_PATH)

$(BIN_PATH) :
	$(MKDIR) $(BIN_PATH)