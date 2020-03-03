#pragma once

#include "Controls.h"

#include <stdexcept>
#include <windows.h>
#include <GL/gl.h>

namespace win_tools
{

    /**
     * Регистрация оконного класса
     * @param strClassName Наименование класса
     * @param pfnWindowProcedure Указатель на оконную процедуру
     * @return Состояние регистрации класса
     */
    bool RegisterWindowClass(HINSTANCE hInstance, const char* strClassName, WNDPROC pfnWindowProcedure = WindowProcedure);

    /**
     * Создание OpenGL контекста и его активация
     * @param drawContext Контекст рисования окна Windows
     * @return Дескриптор контекста OpenGL
     */
    HGLRC OpenGlCreateContext(HDC drawContext);

    /**
     * Получение адреса функции OpenGL
     * @param name Имя функции
     * @return Указатель на функцию
     */
    void *OpenGlGetFunction(const char *name);

    /**
     * Управление вертикальной синхронизацией OpenGL
     * @param status
     * @return Состояние операции
     */
    bool OpenGlSetVSync(bool status);
}