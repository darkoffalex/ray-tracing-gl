#include <iostream>
#include <windows.h>

#include "WinTools.h"
#include "../Renderer/Renderer.h"

/// Дескриптор исполняемого модуля программы
HINSTANCE _hInstance = nullptr;
/// Дескриптор осноного окна отрисовки
HWND _hwnd = nullptr;
/// Дескриптор контекста отрисовки
HDC _hdc = nullptr;
/// Дескриптор контекста OpenGL
HGLRC _hglrc = nullptr;

/**
 * Точка входа
 * @param argc Кол-во аргументов
 * @param argv Аргументы-строки
 * @return Код исполнения
 */
int main(int argc, char* argv[])
{
    try
    {
        // Получение дескриптора исполняемого модуля программы
        _hInstance = GetModuleHandle(nullptr);

        // Пытаемся зарегистрировать оконный класс
        if (!win_tools::RegisterWindowClass(_hInstance,"AppWindowClass")) {
            throw std::runtime_error("ERROR: Can't register window class.");
        }

        // Создание окна
        _hwnd = CreateWindow(
                "AppWindowClass",
                "Ray Tracing",
                WS_OVERLAPPEDWINDOW,
                0, 0,
                800, 600,
                nullptr,
                nullptr,
                _hInstance,
                nullptr);

        // Если не удалось создать окно
        if (!_hwnd) {
            throw std::runtime_error("ERROR: Can't create main application window.");
        }

        // Показать окно
        ShowWindow(_hwnd, SW_SHOWNORMAL);

        // Получение контекста отрисовки
        _hdc = GetDC(_hwnd);
        _hglrc = win_tools::CreateOpenGlContext(_hdc);

        // Проверка работоспособности либы
        bool initialized = rtr::InitGlewLib();
        if(initialized) std::cout << "GLEW is ready!" << std::endl;
        else std::cout << "GLEW initialization error" << std::endl;

        // Оконное сообщение
        MSG msg = {};

        // Запуск цикла
        while (true)
        {
            // Обработка оконных сообщений
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT) {
                    break;
                }
            }
        }
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        system("pause");
    }

    // Уничтожение контекста OpenGL
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(_hglrc);
    // Уничтожение окна
    DestroyWindow(_hwnd);
    // Вырегистрировать класс окна
    UnregisterClass("AppWindowClass",_hInstance);

    return 0;
}


