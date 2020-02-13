# Версия CMake
cmake_minimum_required(VERSION 3.15)

# Название проекта (решение в Visual Studio)
project(RayTracing)

# Стандарт С/С++
set(CMAKE_CXX_STANDARD 17)

# Устанавливаем каталоги для бинарников
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Bin)

# Определить битность платформы
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(PLATFORM_BIT_SUFFIX "x86")
else()
    set(PLATFORM_BIT_SUFFIX "x64")
endif()

# Старндартные C/C++ библиотеки для MinGW/GCC (не для Visual Studio)
if(NOT MSVC)
    set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
endif()

# ----------------------------------------------------------------------------------------------------------------------
# Б И Б Л И О Т Е К А (S H A R E D  D L L)

# Название библиотеки
set(LIB_NAME "Renderer")
set(LIB_BIN_NAME "Renderer")

# Добавляем .dll (проект в Visual Studio)
add_library(${LIB_NAME} SHARED
        Sources/Renderer/Renderer.h
        Sources/Renderer/Renderer.cpp)

# Добавляем символ RENDERER_LIB_EXPORTS для экспорта функций
target_compile_definitions(${LIB_NAME} PUBLIC RENDERER_LIB_EXPORTS)

# Директории с библиотеками (.lib)
target_link_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Lib/glew/${PLATFORM_BIT_SUFFIX})

# Директории с включаемыми файлами (.h)
target_include_directories(${LIB_NAME} PUBLIC Include)

# Имя выходгоного файла (меняется в зависимости от конфишурации сборщика)
set_property(TARGET ${LIB_NAME} PROPERTY OUTPUT_NAME "${LIB_BIN_NAME}$<$<CONFIG:Debug>:_Debug>_${PLATFORM_BIT_SUFFIX}")

# Изменение префиксов и расширений файлов библиотек
set_property(TARGET ${LIB_NAME} PROPERTY PREFIX "")
set_property(TARGET ${LIB_NAME} PROPERTY IMPORT_PREFIX "")
set_property(TARGET ${LIB_NAME} PROPERTY SUFFIX ".dll")
set_property(TARGET ${LIB_NAME} PROPERTY IMPORT_SUFFIX ".lib")

# Линковка OpenGL и GLEW с библиотекой
target_link_libraries(${LIB_NAME} glew32.lib Opengl32)

# ----------------------------------------------------------------------------------------------------------------------
# П Р И Л О Ж Е Н И Е

# Название приложения
set(APP_NAME "DemoApp")
set(APP_BIN_NAME "DemoApp")

# Добавляем .exe (проект в Visual Studio)
add_executable(${APP_NAME}
        Sources/DemoApp/Main.cpp
        Sources/DemoApp/WinTools.h
        Sources/DemoApp/WinTools.cpp)

# Меняем название запускаемого файла в зависимости от типа сборки
set_property(TARGET ${APP_NAME} PROPERTY OUTPUT_NAME "${APP_BIN_NAME}$<$<CONFIG:Debug>:_Debug>")

# Статическая линковка рантайма и стандартных библиотек
if(MSVC)
    set_property(TARGET ${APP_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    set_property(TARGET ${APP_NAME} PROPERTY LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif()

# Линковка приложения и библиотеки
target_link_libraries(${APP_NAME} PUBLIC ${LIB_NAME})