CXX=clang++
CXXFLAGS=-O0 -g --std=c++17 -DDEBUG
LDFLAGS=-fsanitize=address

#GLFW STUFF
GL_DIR=$(shell pwd)/../GL

GL_INCLUDE_DIR=${GL_DIR}/include
GL_LIBS_DIR=${GL_DIR}/src

#BUILD
BUILD= build/

IMGUI_DIR = ../imgui
IMGUI_ADDONS_DIR = ../ImGui-Addons/FileBrowser

CXXFLAGS += -I${GL_INCLUDE_DIR}
CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_ADDONS_DIR) -I$(IMGUI_DIR)/backends -DIMGUI_IMPL_OPENGL_LOADER_GLAD
LDFLAGS += -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -L${GL_LIBS_DIR}

SOURCES = glad.c $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

all: build_dir imgui_addons
	${CXX} main.cpp ${SOURCES} ${CXXFLAGS} ${BUILD_DIR}/*.o -o ${BUILD_DIR}/main ${LDFLAGS}

${BUILD}%.o: ext/imgui/backends/*.cpp
	${CXX} -c $< ${CXXFLAGS} -o $@ ${LDFLAGS}

${BUILD}%.o: ext/imgui/%.cpp
	${CXX} -c $< ${CXXFLAGS} -o $@ ${LDFLAGS}

${BUILD}%.o: ext/ImGui-Addons/FileBrowser/%.cpp
	${CXX} -c $< ${CXXFLAGS} -o $@ ${LDFLAGS}

build_dir:
	mkdir ${BUILD_DIR} || true


run:
	${BUILD_DIR}/main

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR} || true
