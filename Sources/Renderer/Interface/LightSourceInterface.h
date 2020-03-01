/**
 * С-интерфейс для взаимодействия с объектами класса источника света
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include "../Types.h"

namespace rtgl
{
    extern "C"
    {
        /**
         * Создание источника света
         * @param type Тип источника света
         * @param position Положение источника
         * @param radius Радиус (влияет на мягкие тени, для резких теней можно оставить 0)
         * @param color Цвет источника
         * @param orientation Ориентация
         * @param attenuationLinear Линейный коэффициент затухания
         * @param attenuationQuadratic Квадратичный коэффициент затухания
         * @param cutOffAngle Угол отсечения (для типа SPOT_LIGHT)
         * @param cutOffOuterAngle Внешний угол отсечения (для типа SPOT_LIGHT)
         * @return Дескриптор источника
         */
        RENDERER_LIB_API HLightSource __cdecl CreateLightSource(
                const Vec3<float>& position,
                const Vec3<float>& orientation = {0.0f,0.0f,0.0f},
                const LightSourceType& type = LightSourceType::LIGHT_POINT,
                const float& radius = 0.0f,
                const Vec3<float>& color = {1.0f,1.0f,1.0f},
                const float& attenuationLinear = 0.2f,
                const float& attenuationQuadratic = 0.22f,
                const float& cutOffAngle = 40.0f,
                const float& cutOffOuterAngle = 45.0f);

        /**
         * Уничтожение источника света
         * @param pLightSourceHandle Указатель на дескриптор источника
         * @return Состояние операции
         */
        RENDERER_LIB_API bool __cdecl DestroyLightSource(HLightSource* pLightSourceHandle);

        /**
         * Установить параметры источника света
         * @param lightSource Источник света
         * @param type Тип источника света
         * @param radius Радиус (влияет на мягкие тени, для резких теней можно оставить 0)
         * @param color Цвет источника
         * @param orientation Ориентация
         * @param attenuationLinear Линейный коэффициент затухания
         * @param attenuationQuadratic Квадратичный коэффициент затухания
         * @param cutOffAngle Угол отсечения (для типа SPOT_LIGHT)
         * @param cutOffOuterAngle Внешний угол отсечения (для типа SPOT_LIGHT)
         * @return Состояние операции
         */
        RENDERER_LIB_API bool __cdecl SetLightSourceSettings(HLightSource lightSource,
                const LightSourceType& type,
                const float& radius,
                const Vec3<float>& color,
                const float& attenuationLinear,
                const float& attenuationQuadratic,
                const float& cutOffAngle,
                const float& cutOffOuterAngle);
    }
}
