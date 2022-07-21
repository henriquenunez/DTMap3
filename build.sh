
if [[ $1 = "windows" ]]
then
  CXX=x86_64-w64-mingw32-g++
  LDFLAGS=""
else
  CXX=g++
  LDFLAGS="-fsanitize=address -fno-omit-frame-pointer "
fi

echo Using $CXX as the C++ compiler.

SRC=$(pwd)/src
BUILD=$(pwd)/build/

CXXFLAGS="-O0 -g --std=c++17 -DDEBUG "

EXT="$(pwd)/ext"

#GLFW STUFF
GL_DIR="$(pwd)/ext/GL"

GL_INCLUDE_DIR="$GL_DIR/include"
GL_LIBS_DIR="$GL_DIR/src"

IMGUI_DIR="$(pwd)/ext/imgui"
IMGUI_ADDONS_DIR="$(pwd)/ext/ImGui-Addons/FileBrowser"

#CXXFLAGS+="-I$GL_INCLUDE_DIR "
CXXFLAGS+="-I$EXT/glm "
CXXFLAGS+="-I$IMGUI_DIR "
CXXFLAGS+="-I$EXT/glad/include "
CXXFLAGS+="-I$EXT -I$IMGUI_ADDONS_DIR -I$IMGUI_DIR/backends -DIMGUI_IMPL_OPENGL_LOADER_GLAD "
#LDFLAGS+="-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -L$GL_LIBS_DIR "

if [[ $1 = "windows" ]]
then
  # Setup the libraries and stuff to search for glfw
  LDFLAGS+="-L$EXT/glfw-3.3.6.bin.WIN64/lib-static-ucrt $EXT/glfw-3.3.6.bin.WIN64/lib-static-ucrt/glfw3dll.lib  "
  CXXFLAGS+="-I$EXT/glfw-3.3.6.bin.WIN64/include "
else
  LDFLAGS+=$(pkg-config --static --libs glfw3)
fi

SOURCES="$(pwd)/ext/glad/src/glad.c "

# TODO: inject files for unity building
#SOURCES+="$IMGUI_DIR/imgui.cpp $IMGUI_DIR/imgui_demo.cpp $IMGUI_DIR/imgui_draw.cpp $IMGUI_DIR/imgui_tables.cpp $IMGUI_DIR/imgui_widgets.cpp "
#SOURCES+="$IMGUI_DIR/backends/imgui_impl_glfw.cpp $IMGUI_DIR/backends/imgui_impl_opengl3.cpp "
#SOURCES+="$IMGUI_DIR/*.cpp "
#SOURCES+="$IMGUI_DIR/backends/imgui_impl_glfw.cpp "
#SOURCES+="$IMGUI_DIR/backends/imgui_impl_opengl3.cpp "
#SOURCES+="$IMGUI_ADDONS_DIR/*.cpp "
#SOURCES+="$SRC/importer.cpp "
#SOURCES+="$SRC/colorbar.cpp "

MAIN="$(pwd)/src/main.cpp"

if [ ! -d "$BUILD" ]
then
  mkdir "$BUILD";
fi

cd "$BUILD" || exit # In case cd fails

# Build libs
$CXX $CXXFLAGS -c $SOURCES

# Copy shaders
cp -r ${SRC}/shaders .

# Build main source
$CXX $MAIN *.o $CXXFLAGS -I$EXT -o heatmap $LDFLAGS
