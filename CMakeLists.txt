cmake_minimum_required(VERSION 3.16)

project(Dewy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(GLEW REQUIRED)
endif()

add_library(Renderer STATIC
    Renderer/code/src/IndexBuffer.cpp
    Renderer/code/src/Renderer.cpp
    Renderer/code/src/Shader.cpp
    Renderer/code/src/Sprite.cpp
    Renderer/code/src/SpriteRenderer.cpp
    Renderer/code/src/Texture.cpp
    Renderer/code/src/VertexArray.cpp
    Renderer/code/src/VertexBuffer.cpp
    Renderer/code/vendor/stb_image/stb_image.cpp
)

add_library(CircuitPersistence STATIC
    Dewy/src/CircuitSerializer.cpp
)

target_include_directories(CircuitPersistence PUBLIC Dewy/include)

target_include_directories(Renderer
    PUBLIC
        Renderer/code/include
        Renderer/code/vendor
)

target_link_libraries(Renderer
    PUBLIC
        OpenGL::GL
        glfw
)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(Renderer PUBLIC GLEW::GLEW)
endif()

add_executable(Dewy
    Dewy/src/Button.cpp
    Dewy/src/CollisionManager.cpp
    Dewy/src/ConnectionComponent.cpp
    Dewy/src/Entity.cpp
    Dewy/src/Gui.cpp
    Dewy/src/InputHandler.cpp
    Dewy/src/LightBulb.cpp
    Dewy/src/Program.cpp
    Dewy/src/SpriteManager.cpp
    Dewy/src/logicgate.cpp
    Dewy/src/main.cpp
    Dewy/dependencies/ImGui/src/imgui.cpp
    Dewy/dependencies/ImGui/src/imgui_draw.cpp
    Dewy/dependencies/ImGui/src/imgui_impl_glfw.cpp
    Dewy/dependencies/ImGui/src/imgui_impl_opengl3.cpp
    Dewy/dependencies/ImGui/src/imgui_tables.cpp
    Dewy/dependencies/ImGui/src/imgui_widgets.cpp
)

target_include_directories(Dewy
    PRIVATE
        Dewy/include
        Dewy/vendor
        Dewy/dependencies/ImGui/include
)

target_link_libraries(Dewy PRIVATE Renderer CircuitPersistence)
target_compile_definitions(Dewy PRIVATE DEWY_SHADER_PATH="shaders/basic.shader")

add_custom_command(TARGET Dewy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Dewy/resources"
            "$<TARGET_FILE_DIR:Dewy>/resources"
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:Dewy>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Renderer/res/shaders/basic.shader"
            "$<TARGET_FILE_DIR:Dewy>/shaders/basic.shader"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Renderer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(CircuitPersistence PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(Dewy PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)

if(BUILD_TESTING)
    add_executable(CircuitSerializerTests tests/CircuitSerializerTests.cpp)
    target_link_libraries(CircuitSerializerTests PRIVATE CircuitPersistence)
    add_test(NAME circuit_serializer COMMAND CircuitSerializerTests)

    add_executable(TopologicalOrderTests tests/TopologicalOrderTests.cpp)
    target_include_directories(TopologicalOrderTests PRIVATE Dewy/include)
    add_test(NAME topological_order COMMAND TopologicalOrderTests)
endif()
