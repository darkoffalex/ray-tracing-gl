#include <iostream>
#include <windows.h>

#include "WinTools.h"
#include "Tools.h"
#include "Timer.h"
#include "../Renderer/Renderer.h"

/// Дескриптор исполняемого модуля программы
HINSTANCE _hInstance = nullptr;
/// Дескриптор осноного окна отрисовки
HWND _hwnd = nullptr;
/// Дескриптор контекста отрисовки
HDC _hdc = nullptr;
/// Дескриптор контекста OpenGL
HGLRC _hglrc = nullptr;
/// Таймер
Timer* _timer = nullptr;

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

        // Размеры клиентской области окна
        RECT clientRect;
        GetClientRect(_hwnd, &clientRect);

        // Создание контекста OpenGL
        _hglrc = win_tools::OpenGlCreateContext(_hdc);

        // Отключение вертикальной синхронизации OpenGL
        win_tools::OpenGlSetVSync(false);

        /** Рендерер - инициализация **/

        // Загрзить исходный код шейдеров
        std::string gpv = tools::LoadStringFromFile(tools::ShaderDir().append("geometry-prepare.vert"));
        std::string gpg = tools::LoadStringFromFile(tools::ShaderDir().append("geometry-prepare.geom"));
        std::string gpf = tools::LoadStringFromFile(tools::ShaderDir().append("geometry-prepare.frag"));

        std::string rtv = tools::LoadStringFromFile(tools::ShaderDir().append("ray-tracing.vert"));
        std::string rtf = tools::LoadStringFromFile(tools::ShaderDir().append("ray-tracing.frag"));

        // Инициализация рендерера
        if(!rtgl::Init(clientRect.right, clientRect.bottom, {gpv.c_str(), gpg.c_str(), gpf.c_str(),rtv.c_str(),rtf.c_str()})){
            throw std::runtime_error(rtgl::GetLastErrorMessage());
        }

        /** Рендерер - загрузка ресурсов **/

        // Геометрия
        rtgl::Vertex<float> vertices[4] = {
                { { 1.0f,0.0f,-1.0f },{ 1.0f,0.0f,0.0f },{ 1.0f,1.0f }, {0.0f,0.0f,1.0f} },
                { { 1.0f,0.0f,1.0f },{ 0.0f,1.0f,0.0f },{ 1.0f,0.0f }, {0.0f,0.0f,1.0f} },
                { { -1.0f,0.0f,1.0f },{ 0.0f,0.0f,1.0f },{ 0.0f,0.0f }, {0.0f,0.0f,1.0f} },
                { { -1.0f,0.0f,-1.0f },{ 1.0f,1.0f,0.0f },{ 0.0f,1.0f }, {0.0f,0.0f,1.0f} },
        };
        unsigned indices[6] = { 0,1,2, 0,2,3 };

        rtgl::HGeometryBuffer quadBuffer = rtgl::CreateGeometryBuffer(vertices,4,indices,6);

        /** Рендерер - объекты сцены **/

        rtgl::HMesh quadMesh1 = rtgl::CreateMesh(quadBuffer,{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{3.0f,1.0f,3.0f});
        rtgl::HMesh lightSource1 = rtgl::CreateLightSource({0.0f,2.0f,0.0f});
        rtgl::SetCameraSettings({0.0f,2.0f,5.0f},{-20.0f,0.0f,0.0f});

        /** MAIN LOOP **/

        // Таймер основного цикла (для выясняения временной дельты и FPS)
        _timer = new Timer();

        // Оконное сообщение
        MSG msg = {};

        // Запуск цикла
        while (true)
        {
            // Обновить таймер
            _timer->updateTimer();

            // Обработка оконных сообщений
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT) {
                    break;
                }
            }

            // Поскольку показ FPS на окне уменьшает FPS - делаем это только тогда когда счетчик готов (примерно 1 раз в секунду)
            if (_timer->isFpsCounterReady()){
                std::string fps = std::string("Ray Tracing (").append(std::to_string(_timer->getFps())).append(" FPS)");
                SetWindowTextA(_hwnd, fps.c_str());
            }

            /// Обновление сцены

            //TODO: Обновление сцены и управление камерой

            /// Отрисовка и показ кадра

            rtgl::SetMesh(quadMesh1);

            rtgl::SetLightSource(lightSource1);

            // Трасировка сцены
            rtgl::RenderScene();

            // Смена буферов окна
            SwapBuffers(_hdc);
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


