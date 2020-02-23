/**
 * С-интерфейс для взаимодействия с объектами класса геометрического буфера
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#include "GeometryBufferInterface.h"
#include "../Resources/GeometryBuffer.h"

#include <stdexcept>
#include <GL/gl.h>

namespace rtgl
{

    /// Сообщение о последней ошибке (объявлено в Globals.h->Renderer.cpp)
    extern std::string _strLastErrorMsg;
    /// Инициализирована ли библиотека (объявлено в Globals.h->Renderer.cpp)
    extern bool _bInitialized;

    /**
     * Создать буфер геометрии
     * @param vertices Массив вершин
     * @param verticesQnt Кол-во вершин
     * @param indices Массив индексов
     * @param indicesQnt Кол-во индексов
     * @return Хендл геометрического буфера
     */
    HGeometryBuffer __cdecl CreateGeometryBuffer(
            Vertex<float> vertices[],
            const size_t verticesQnt,
            unsigned indices[],
            const size_t indicesQnt)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            auto const geometry = new GeometryBuffer(
                    std::vector<Vertex<GLfloat>>(vertices, vertices + verticesQnt),
                    std::vector<GLuint>(indices, indices + indicesQnt));

            return reinterpret_cast<HGeometryBuffer>(geometry);
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
        }

        return nullptr;
    }

    /**
     * Уничтожить геометрический буфер
     * @param pBufferHandle Хендл буфера
     * @return Состояние операции
     */
    bool __cdecl DestroyGeometryBuffer(HGeometryBuffer *pBufferHandle)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            const auto pResource = reinterpret_cast<GeometryBuffer*>(*pBufferHandle);
            delete pResource;
            *pBufferHandle = nullptr;
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }
}
