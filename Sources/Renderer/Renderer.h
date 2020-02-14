/**
 * Основной заголовочный файл библиотеки рендерера с поддержкой трассировки лучей
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include "Types.h"

#ifdef RENDERER_LIB_EXPORTS
#define RENDERER_LIB_API __declspec(dllexport)
#else
#define RENDERER_LIB_API __declspec(dllimport)
#endif

namespace rtgl
{
    extern "C"
    {
        /**
         * Получить последнее сообщение об ошибке
         * @return Строка с сообщением
         */
        RENDERER_LIB_API const char* __cdecl GetLastErrorMessage();

        /**
         * Инициализация
         * @param screenWidth Ширина области отрисовки
         * @param screenHeight Высота области отрисовки
         * @param shaderSourcesBundle Исходные коды шейдеров
         * @return Состояние инициализации
         */
        RENDERER_LIB_API bool __cdecl Init(unsigned screenWidth, unsigned screenHeight, const ShaderSourcesBundle& shaderSourcesBundle);

        /**
         * Уничтожение ресурсов, освобождение памяти
         */
        RENDERER_LIB_API void __cdecl DeInit();
    }
}

