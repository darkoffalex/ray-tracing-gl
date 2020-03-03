#include "WinTools.h"
#include <stdexcept>

namespace win_tools
{
    /// Указатель на OpenGL функцию для управления вертикальной синхронизацией
    typedef BOOL(APIENTRY * PfnWglSwapInterval)(int);

    /// Указатель на OpenGL функцию для получения строки с перечисленными свойствами OpenGL
    typedef GLubyte*(APIENTRY * PfnGlGetString)(GLenum);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    HGLRC OpenGlCreateContext(HDC drawContext) {

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
        const int pixelFormatId = ChoosePixelFormat(drawContext, &pfd);

        // Если не удалось найти подходящего формата пикселей
        if (!pixelFormatId) {
            throw std::runtime_error("Can't find suitable pixel format");
        }

        // Найти наиболее подходящее описание формата
        PIXELFORMATDESCRIPTOR bestMatchPfd;
        DescribePixelFormat(drawContext, pixelFormatId, sizeof(PIXELFORMATDESCRIPTOR), &bestMatchPfd);

        // Если не удалось найти подходящего формата пикселей
        if (bestMatchPfd.cDepthBits < pfd.cDepthBits) {
            throw std::runtime_error("Can't find suitable pixel format for depth");
        }

        // Если не удалось установить формат пикселей
        if (!SetPixelFormat(drawContext, pixelFormatId, &pfd)) {
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

    /**
     * Получение адреса функции OpenGL
     * @param name Имя функции
     * @return Указатель на функцию
     */
    void *OpenGlGetFunction(const char *name)
    {
        void *p = (void *)wglGetProcAddress(name);
        if(p == nullptr ||
           (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
           (p == (void*)-1) )
        {
            HMODULE module = LoadLibraryA("opengl32.dll");
            p = (void *)GetProcAddress(module, name);
        }

        return p;
    }

    /**
     * Управление вертикальной синхронизацией OpenGL
     * @param status
     * @return Состояние операции
     */
    bool OpenGlSetVSync(bool status)
    {
        // Получаем OpenGL функцию для получения OpenGL строки
        const auto glGetString = reinterpret_cast<PfnGlGetString>(OpenGlGetFunction("glGetString"));

        // Получаем при помощи glGetString строку с доступными расширениями
        const char* extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

        // Если есть нужное расширение
        if (strstr(extensions, "WGL_EXT_swap_control"))
        {
            // Получить указатель на функцию для управления вертикальной синхронизацией
            const auto wglSwapInterval = reinterpret_cast<PfnWglSwapInterval>(OpenGlGetFunction("wglSwapIntervalEXT"));
            // Если удалось получить - использовать функцию, в противном случае сгенерировать исключение
            if (wglSwapInterval) wglSwapInterval(status);
            else return false;
        }
        else
        {
            return false;
        }

        return true;
    }
}