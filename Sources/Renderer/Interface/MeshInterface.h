/**
 * С-интерфейс для взаимодействия с объектами класса меша
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include "../Types.h"

namespace rtgl
{
    extern "C"
    {
        /**
         * Создать меш
         * @param geometry Геометрия меша
         * @param position Положение
         * @param orientation Ориентация
         * @param scale Масштабирование
         * @param origin Локальный центр
         * @return Дескриптор меша
         */
        RENDERER_LIB_API HMesh __cdecl CreateMesh(HGeometryBuffer geometry,
                const Vec3<float>& position,
                const Vec3<float>& orientation,
                const Vec3<float>& scale,
                const Vec3<float>& origin = { 0.0f,0.0f,0.0f });

        /**
         * Уничтожение объекта меша
         * @param pMeshHandle Указатель на дескриптор объекта
         * @return Состояние операции
         */
        RENDERER_LIB_API bool __cdecl DestroyMesh(HMesh* pMeshHandle);

        /**
         * Уствнока параметров материала меша (PBR)
         * @param mesh Дескриптор меша
         * @param albedo Альбедо-цвет (аналог diffuse)
         * @param metallic Металличность
         * @param roughness Шероховатость
         * @param primaryCoff Соотношение собственного цвета к отражению/преломлению
         * @param reflectionToRefraction Соотношение отраженного к преломленному компоненту
         * @param refractionCoff Коэфициент преломления
         * @return Состояние операции
         */
        RENDERER_LIB_API bool __cdecl SetMeshMaterialSettings(HMesh mesh,
                const Vec3<float>& albedo,
                const float& metallic,
                const float& roughness,
                const float& primaryCoff = 1.0f,
                const float& reflectionToRefraction = 1.0f,
                const float& refractionCoff = 0.6f);
    }
}