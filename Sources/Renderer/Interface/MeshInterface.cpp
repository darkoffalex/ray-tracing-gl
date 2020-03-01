/**
 * С-интерфейс для взаимодействия с объектами класса меша
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#include "MeshInterface.h"
#include "../Scene/Mesh.h"

#include <stdexcept>

namespace rtgl
{
    /// Сообщение о последней ошибке (объявлено в Globals.h->Renderer.cpp)
    extern std::string _strLastErrorMsg;
    /// Инициализирована ли библиотека (объявлено в Globals.h->Renderer.cpp)
    extern bool _bInitialized;

    /**
     * Создать меш
     * @param geometry Геометрия меша
     * @param position Положение
     * @param orientation Ориентация
     * @param scale Масштабирование
     * @param origin Локальный центр
     * @return Дескриптор меша
     */
    HMesh __cdecl CreateMesh(HGeometryBuffer geometry,
            const Vec3<float> &position,
            const Vec3<float> &orientation,
            const Vec3<float> &scale,
            const Vec3<float> &origin)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            auto mesh = new Mesh();
            mesh->geometry = reinterpret_cast<GeometryBuffer*>(geometry);
            mesh->setPosition({ position.x, position.y, position.z }, false);
            mesh->setOrientation({ orientation.x, orientation.y, orientation.z }, false);
            mesh->setScale({ scale.x, scale.y, scale.z }, false);
            mesh->setOrigin({ origin.x, origin.y, origin.z }, true);

            return reinterpret_cast<HMesh>(mesh);
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
        }

        return nullptr;
    }

    /**
     * Уничтожение объекта меша
     * @param pMeshHandle Указатель на дескриптор объекта
     * @return Состояние операции
     */
    bool __cdecl DestroyMesh(HMesh *pMeshHandle)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            const auto pResource = reinterpret_cast<Mesh*>(*pMeshHandle);
            delete pResource;
            *pMeshHandle = nullptr;
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }

    /**
     * Уствнока параметров материала меша (PBR)
     * @param mesh Дескриптор меша
     * @param albedo Альбедо-цвет (аналог diffuse)
     * @param metallic Металличность
     * @param roughness Шероховатость
     * @return Состояние операции
     */
    bool __cdecl SetMeshMaterialSettings(HMesh mesh, const Vec3<float> &albedo, const float &metallic, const float &roughness)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            const auto pMesh = reinterpret_cast<Mesh*>(mesh);
            pMesh->material.albedo = {albedo.r,albedo.g,albedo.b};
            pMesh->material.metallic = metallic;
            pMesh->material.roughness = roughness;
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }
}
