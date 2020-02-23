/**
 * С-интерфейс для взаимодействия с объектами класса (или его потомков) элемента сцены
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include "../Scene/SceneElement.h"
#include "../Types.h"

namespace rtgl
{
    /**
     * Настройка параметров объекта сцены
     * @param sceneElement Хендл элемента сцены
     * @param position Положение
     * @param orientation Ориентация
     * @param scale Масштаб
     * @param origin Локальный центр
     * @return Состояние операции
     */
    RENDERER_LIB_API bool __cdecl SetSpatialSettings(HSceneElement sceneElement,
            const Vec3<float>& position,
            const Vec3<float>& orientation,
            const Vec3<float>& scale,
            const Vec3<float>& origin);

    /**
     * Настройка положения объекта сцены
     * @param sceneElement Хендл элемента сцены
     * @param position Положение
     * @return Состояние операции
     */
    RENDERER_LIB_API bool __cdecl SetPosition(HSceneElement sceneElement, const Vec3<float>& position);

    /**
     * Настройка ориентации объекта сцены
     * @param sceneElement Хендл элемента сцены
     * @param orientation Ориентация
     * @return Состояние операции
     */
    RENDERER_LIB_API bool __cdecl SetOrientation(HSceneElement sceneElement, const Vec3<float>& orientation);

    /**
     * Настройка масштаба объекта сцены
     * @param sceneElement Хендл элемента сцены
     * @param scale Масштаб
     * @return Состояние операции
     */
    RENDERER_LIB_API bool __cdecl SetScale(HSceneElement sceneElement, const Vec3<float>& scale);

    /**
     * Настройка локального центра объекта сцены
     * @param sceneElement Хендл элемента сцены
     * @param origin Локальный центр
     * @return Состояние операции
     */
    RENDERER_LIB_API bool __cdecl SetOrigin(HSceneElement sceneElement, const Vec3<float>& origin);
}
