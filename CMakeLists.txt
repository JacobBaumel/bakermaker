cmake_minimum_required(VERSION 3.27)
project(bakermaker)

include(${CMAKE_TOOLCHAIN_FILE})
set(CMAKE_CXX_STANDARD 20)

set(LIBROMFS_PROJECT_NAME bakermaker)
set(LIBROMFS_RESOURCE_LOCATION ${PROJECT_SOURCE_DIR}/resources)
add_subdirectory(libs/libromfs)

set(LIBZIP_DO_INSTALL OFF)
add_subdirectory(libs/miniz)

add_subdirectory(libs/glfw)
add_subdirectory(libs/json)

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
        src/UI/CreateAdminKey.cpp
        src/UI/LibsNotFound.cpp
        src/libs/unzip_utils.cpp
        src/utils.cpp
        src/UI/RepoManage.cpp
        src/UI/UserManage.cpp
        src/UI/SyncToServer.cpp
        src/libs/STStringJson.cpp
        src/UI/ClientExtract.cpp
        windowsrc/icon.rc
)

set(string_theory_FOUND 1)
find_package(string_theory CONFIG REQUIRED)

find_package(OpenSSL REQUIRED)
find_package(libssh REQUIRED)

target_link_libraries(bakermaker PUBLIC glfw opengl32 gdi32 imm32 user32 advapi32 string_theory ${LIBROMFS_LIBRARY}
    ssh miniz nlohmann_json PRIVATE delayimp)
target_include_directories(bakermaker PUBLIC libs/glfw/include libs/imgui
        libs/imgui/backends libs/json/include libs/miniz ${string_theory_INCLUDE_DIRS} ${LIBSSH_INCLUDE_DIRS} include)

target_link_options(bakermaker PRIVATE "/DELAYLOAD:$<TARGET_FILE_BASE_NAME:ssh>.dll")

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "gitolitesetup")