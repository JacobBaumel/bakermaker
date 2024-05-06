cmake_minimum_required(VERSION 3.27)
project(bakermaker)

include(${CMAKE_TOOLCHAIN_FILE})
set(CMAKE_CXX_STANDARD 20)

#set(OPENSSL_USE_STATIC_LIBS TRUE)
#add_definitions(-DLIBSSH_STATIC=1)

set(LIBROMFS_PROJECT_NAME bakermaker)
set(LIBROMFS_RESOURCE_LOCATION ${PROJECT_SOURCE_DIR}/resources)
add_subdirectory(libs/libromfs)

add_subdirectory(libs/glfw)

add_executable(bakermaker src/main.cpp libs/imgui/imgui.cpp libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp libs/imgui/imgui_widgets.cpp
        libs/imgui/backends/imgui_impl_glfw.cpp libs/imgui/backends/imgui_impl_opengl3.cpp
        src/libs/imfilebrowser.cpp
        src/libs/improgress.cpp
        src/setup.cpp
        src/UI/BaseUIScreen.cpp
        src/ImguiMarkdownRender.cpp
        src/libs/stbi.cpp
        src/UI/ServerSetup.cpp
        src/UI/ServerConnect.cpp
        src/libs/ssh_helper.cpp
        src/UI/ServerInstall.cpp
        src/UI/CreateSSHKeys.cpp
)

set(string_theory_FOUND 1)
find_package(string_theory CONFIG REQUIRED)

find_package(OpenSSL REQUIRED)
find_package(libssh REQUIRED)

target_link_libraries(bakermaker PUBLIC glfw opengl32 gdi32 imm32 user32 advapi32 string_theory ${LIBROMFS_LIBRARY}
    ssh)
target_include_directories(bakermaker PUBLIC libs/glfw/include libs/imgui
        libs/imgui/backends ${string_theory_INCLUDE_DIRS} ${LIBSSH_INCLUDE_DIRS} include)