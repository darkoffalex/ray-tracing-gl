# Версия CMake
cmake_minimum_required(VERSION 3.15)

# Определить битность платформы
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(PLATFORM_BIT_SUFFIX "x86")
else()
    set(PLATFORM_BIT_SUFFIX "x64")
endif()

# Название библиотеки
set(TARGET_NAME "Renderer")
set(TARGET_BIN_NAME "Renderer")

# Добавляем .dll (проект в Visual Studio)
add_library(${TARGET_NAME} SHARED
        "Renderer.h"
        "Renderer.cpp"
        "Resources/FrameBuffer.cpp"
        "Resources/FrameBuffer.h"
        "Resources/ShaderProgram.cpp"
        "Resources/ShaderProgram.h"
        "Resources/GeometryBuffer.cpp"
        "Resources/GeometryBuffer.h"
        "Scene/SceneElement.cpp"
        "Scene/SceneElement.h"
        "Scene/Camera.cpp"
        "Scene/Camera.h"
        "Scene/Mesh.cpp"
        "Scene/Mesh.h"
        "Interface/SceneElementInterface.cpp"
        "Interface/SceneElementInterface.h"
        "Interface/GeometryBufferInterface.cpp"
        "Interface/GeometryBufferInterface.h")

# Добавляем символ RENDERER_LIB_EXPORTS для экспорта функций
target_compile_definitions(${TARGET_NAME} PUBLIC RENDERER_LIB_EXPORTS)

# Директории с библиотеками (.lib)
target_link_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Lib/glew/${PLATFORM_BIT_SUFFIX})

# Директории с включаемыми файлами (.h)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Include)

# Имя выходгоного файла (меняется в зависимости от конфишурации сборщика)
set_property(TARGET ${TARGET_NAME} PROPERTY OUTPUT_NAME "${TARGET_BIN_NAME}$<$<CONFIG:Debug>:_Debug>_${PLATFORM_BIT_SUFFIX}")

# Изменение префиксов и расширений файлов библиотек
set_property(TARGET ${TARGET_NAME} PROPERTY PREFIX "")
set_property(TARGET ${TARGET_NAME} PROPERTY IMPORT_PREFIX "")

if(WIN32)
    set_property(TARGET ${TARGET_NAME} PROPERTY SUFFIX ".dll")
    set_property(TARGET ${TARGET_NAME} PROPERTY IMPORT_SUFFIX ".lib")
endif()

# Статическая линковка рантайма и стандартных библиотек
if(MSVC)
    set_property(TARGET ${TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    set_property(TARGET ${TARGET_NAME} PROPERTY LINK_FLAGS "-Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif()

# Линковка OpenGL и GLEW с библиотекой
target_link_libraries(${TARGET_NAME} glew32.lib Opengl32)