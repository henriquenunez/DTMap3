# Install script

cd ext

wget 'https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip'
unzip 'glm-0.9.9.8.zip' 

# Imgui
git clone --depth 1 https://github.com/ocornut/imgui.git
git clone --depth 1 https://github.com/gallickgunner/ImGui-Addons.git

# CSV Parser
wget 'https://raw.githubusercontent.com/vincentlaucsb/csv-parser/master/single_include/csv.hpp'

# stb_image_write
wget 'https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h'

# Unzipping glad
unzip glad.zip -d glad

# if building for windows

if [[ $1 -eq "windows" ]]
then
  wget 'https://github.com/glfw/glfw/releases/download/3.3.6/glfw-3.3.6.bin.WIN64.zip'
  unzip 'glfw-3.3.6.bin.WIN64.zip'
elif [[ $1 -eq "mac" ]]
then
  wget 'https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.MACOS.zip'
  unzip 'glfw-3.3.8.bin.MACOS.zip'
fi
