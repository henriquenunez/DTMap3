CXX=clang++
BUILD=$(pwd)/build/

CXXFLAGS="-O0 -g --std=c++17 -DDEBUG "
LDFLAGS="-fsanitize=address "

EXT="$(pwd)/ext"

#GLFW STUFF
GL_DIR="$(pwd)/ext/GL"

GL_INCLUDE_DIR="$GL_DIR/include"
GL_LIBS_DIR="$GL_DIR/src"

IMGUI_DIR="$(pwd)/ext/imgui"
IMGUI_ADDONS_DIR="$(pwd)/ext/ImGui-Addons/FileBrowser"

CXXFLAGS+="-I$GL_INCLUDE_DIR "
CXXFLAGS+="-I$IMGUI_DIR -I$IMGUI_ADDONS_DIR -I$IMGUI_DIR/backends -DIMGUI_IMPL_OPENGL_LOADER_GLAD "
LDFLAGS+="-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -L$GL_LIBS_DIR "

SOURCES="$(pwd)/ext/glad.c "
#SOURCES+="$IMGUI_DIR/imgui.cpp $IMGUI_DIR/imgui_demo.cpp $IMGUI_DIR/imgui_draw.cpp $IMGUI_DIR/imgui_tables.cpp $IMGUI_DIR/imgui_widgets.cpp "
#SOURCES+="$IMGUI_DIR/backends/imgui_impl_glfw.cpp $IMGUI_DIR/backends/imgui_impl_opengl3.cpp "
SOURCES+="$IMGUI_DIR/*.cpp "
SOURCES+="$IMGUI_ADDONS_DIR/*.cpp "

MAIN="$(pwd)/src/main.cpp"

#mkdir $BUILD
cd $BUILD

# Build libs
#$CXX -c $SOURCES $CXXFLAGS

# Build main source
$CXX $MAIN ${BUILD}*.o $CXXFLAGS -I$EXT -o heatmap $LDFLAGS
