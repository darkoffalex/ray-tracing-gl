/**
 * Основной заголовочный файл библиотеки рендерера с поддержкой трассировки лучей
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include "Types.h"
#include "Interface/SceneElementInterface.h"
#include "Interface/GeometryBufferInterface.h"
#include "Interface/MeshInterface.h"

namespace rtgl
{
    extern "C"
    {
        /// О С Н О В Н О Е

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

        /// К А М Е Р А

        /**
         * Установка параметров камеры
         * @param position Положение
         * @param orientation Ориентация
         * @return Состояние операции
         */
        RENDERER_LIB_API bool __cdecl SetCameraSettings(const Vec3<float>& position, const Vec3<float>& orientation);

        /**
         * Установка положения камеры
         * @param position Положение
         * @return Состояние операции
         */
        RENDERER_LIB_API bool __cdecl SetCameraPosition(const Vec3<float>& position);

        /**
         * Установка ориентации камеры
         * @param orientation Ориентация
         * @return Состояние операции
         */
        RENDERER_LIB_API bool __cdecl SetCameraOrientation(const Vec3<float>& orientation);
    }
}

