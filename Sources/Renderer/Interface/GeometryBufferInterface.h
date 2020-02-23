#pragma once

#include "../Resources/GeometryBuffer.h"
#include "../Types.h"

namespace rtgl
{
    extern "C"
    {
        /**
         * Создать буфер геометрии
         * @param vertices Массив вершин
         * @param verticesQnt Кол-во вершин
         * @param indices Массив индексов
         * @param indicesQnt Кол-во индексов
         * @return Хендл геометрического буфера
         */
        RENDERER_LIB_API HGeometryBuffer __cdecl CreateGeometryBuffer(
                Vertex<float>* vertices,
                const size_t verticesQnt,
                unsigned* indices,
                const size_t indicesQnt);

        /**
         * Уничтожить геометрический буфер
         * @param pBufferHandle Хендл буфера
         * @return Состояние операции
         */
        RENDERER_LIB_API bool __cdecl DestroyGeometryBuffer(HGeometryBuffer* pBufferHandle);
    }
}
