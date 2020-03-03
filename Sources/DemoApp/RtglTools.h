#pragma once

#include "../Renderer/Renderer.h"

namespace rtgl
{
    /**
     * Генерация геометрии сферы
     * @param segments Кол-во сегментов
     * @param radius Радиус сферы
     * @return Дескриптор ресурса геометрии
     */
    HGeometryBuffer GenerateSphereGeometry(unsigned segments, float radius);

    /**
     * Генерация геометрии куба
     * @param size Размер стороны куба
     * @return Дескриптор ресурса геометрии
     */
    HGeometryBuffer GenerateCubeGeometry(float size);

    /**
     * Генерация геометрии квадрата
     * @param size Размер стороны квадрата
     * @return Дескриптор ресурса геометрии
     */
    HGeometryBuffer GenerateQuadGeometry(float size);
}