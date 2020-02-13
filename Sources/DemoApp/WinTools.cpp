#include "WinTools.h"
#include <stdexcept>

namespace win_tools
{
    /**
     * Обработчик оконных сообщений
     * @param hWnd Дескриптор окна
     * @param message Сообщение
     * @param wParam Параметр сообщения
     * @param lParam Параметр сообщения
     * @return Код выполнения
     */
    LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        if(message == WM_DESTROY){
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hWnd,message,wParam,lParam);
    }

    /**
     * Регистрация оконного класса
     * @param strClassName Наименование класса
     * @param pfnWindowProcedure Указатель на оконную процедуру
     * @return Состояние регистрации класса
     */
    bool RegisterWindowClass(HINSTANCE hInstance, const char *strClassName, WNDPROC pfnWindowProcedure) {

        // Информация о классе
        WNDCLASSEX classInfo;
        classInfo.cbSize = sizeof(WNDCLASSEX);
        classInfo.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        classInfo.cbClsExtra = 0;
        classInfo.cbWndExtra = 0;
        classInfo.hInstance = hInstance;
        classInfo.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
        classInfo.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
        classInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
        classInfo.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
        classInfo.lpszMenuName = nullptr;
        classInfo.lpszClassName = strClassName;
        classInfo.lpfnWndProc = pfnWindowProcedure;

        // Вернуть состояние регистрации класса
        return RegisterClassEx(&classInfo) != 0;
    }

    /**
     * Создание OpenGL контекста и его активация
     * @param drawContext Контекст рисования окна Windows
     * @return Дескриптор контекста OpenGL
     */
    HGLRC CreateOpenGlContext(HDC drawContext) {

        // Описываем необходимый формат пикселей
        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        // Номер формата пикселей
        const int pixelFormatID = ChoosePixelFormat(drawContext, &pfd);

        // Если не удалось найти подходящего формата пикселей
        if (!pixelFormatID) {
            throw std::runtime_error("Can't find suitable pixel format");
        }

        // Найти наиболее подходящее описание формата
        PIXELFORMATDESCRIPTOR bestMatchPfd;
        DescribePixelFormat(drawContext, pixelFormatID, sizeof(PIXELFORMATDESCRIPTOR), &bestMatchPfd);

        // Если не удалось найти подходящего формата пикселей
        if (bestMatchPfd.cDepthBits < pfd.cDepthBits) {
            throw std::runtime_error("Can't find suitable pixel format for depth");
        }

        // Если не удалось установить формат пикселей
        if (!SetPixelFormat(drawContext, pixelFormatID, &pfd)) {
            throw std::runtime_error("Can't set selected pixel format");
        }

        // OpenGL контекст
        HGLRC glContext = wglCreateContext(drawContext);
        if (!glContext) {
            throw std::runtime_error("Can't create OpenGL rendering context");
        }

        // Активировать OpenGL контекст
        const BOOL success = wglMakeCurrent(drawContext, glContext);
        if (!success) {
            throw std::runtime_error("Can't setup rendering context");
        }

        return glContext;
    }
}