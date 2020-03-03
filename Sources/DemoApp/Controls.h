#pragma once

#include <windows.h>

/**
 * Обработчик оконных сообщений
 * @param hWnd Дескриптор окна
 * @param message Сообщение
 * @param wParam Параметр сообщения
 * @param lParam Параметр сообщения
 * @return Код выполнения
 */
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);