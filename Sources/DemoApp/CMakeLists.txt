# Версия CMake
cmake_minimum_required(VERSION 3.15)

# Определить битность платформы
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(PLATFORM_BIT_SUFFIX "x86")
else()
    set(PLATFORM_BIT_SUFFIX "x64")
endif()

# Название приложения
set(TARGET_NAME "DemoApp")
set(TARGET_BIN_NAME "DemoApp")

# Добавляем .exe (проект в Visual Studio)
add_executable(${TARGET_NAME}
        "Main.cpp"
        "WinTools.h"
        "WinTools.cpp"
        "Tools.cpp"
        "Tools.h"
        "Timer.cpp"
        "Timer.h")

# Меняем название запускаемого файла в зависимости от типа сборки
set_property(TARGET ${TARGET_NAME} PROPERTY OUTPUT_NAME "${TARGET_BIN_NAME}$<$<CONFIG:Debug>:_Debug>")

# Статическая линковка рантайма и стандартных библиотек
if(MSVC)
    set_property(TARGET ${TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    set_property(TARGET ${TARGET_NAME} PROPERTY LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif()

# Линковка с библиотекой Renderer
target_link_libraries(${TARGET_NAME} PUBLIC "Renderer")
# Линковка с shlwapi.lib для получения путей к файлам (tools)
target_link_libraries(${TARGET_NAME} PUBLIC "shlwapi.lib")