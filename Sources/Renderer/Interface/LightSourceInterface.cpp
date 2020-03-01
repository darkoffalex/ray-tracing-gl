/**
 * С-интерфейс для взаимодействия с объектами класса источника света
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#include "LightSourceInterface.h"
#include "../Scene/LightSource.h"
#include <string>
#include <stdexcept>

namespace rtgl
{
    /// Сообщение о последней ошибке (объявлено в Globals.h->Renderer.cpp)
    extern std::string _strLastErrorMsg;
    /// Инициализирована ли библиотека (объявлено в Globals.h->Renderer.cpp)
    extern bool _bInitialized;

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
    HLightSource __cdecl CreateLightSource(const Vec3<float> &position, const Vec3<float> &orientation, const LightSourceType &type,
                      const float &radius, const Vec3<float> &color, const float &attenuationLinear,
                      const float &attenuationQuadratic, const float &cutOffAngle, const float &cutOffOuterAngle)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            auto lightSource = new LightSource();
            lightSource->setPosition({position.x,position.y,position.z}, false);
            lightSource->setOrientation({orientation.x,orientation.y,orientation.z},true);
            lightSource->type = type;
            lightSource->radius = radius;
            lightSource->color = {color.r,color.g,color.b};
            lightSource->attenuationLinear = attenuationLinear;
            lightSource->attenuationQuadratic = attenuationQuadratic;
            lightSource->cutOffAngle = cutOffAngle;
            lightSource->cutOffOuterAngle = cutOffOuterAngle;

            return reinterpret_cast<HLightSource>(lightSource);
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
        }

        return nullptr;
    }

    /**
     * Уничтожение источника света
     * @param pLightSourceHandle Указатель на дескриптор источника
     * @return Состояние операции
     */
    bool __cdecl DestroyLightSource(HLightSource *pLightSourceHandle)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            const auto pResource = reinterpret_cast<LightSource*>(*pLightSourceHandle);
            delete pResource;
            *pLightSourceHandle = nullptr;
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }

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
    bool __cdecl SetLightSourceSettings(HLightSource lightSource, const LightSourceType &type, const float &radius,
            const Vec3<float> &color, const float &attenuationLinear,
            const float &attenuationQuadratic, const float &cutOffAngle,
            const float &cutOffOuterAngle)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            const auto pMesh = reinterpret_cast<LightSource*>(lightSource);
            pMesh->type = type;
            pMesh->radius = radius;
            pMesh->color = {color.r,color.g,color.b};
            pMesh->attenuationLinear = attenuationLinear;
            pMesh->attenuationQuadratic = attenuationQuadratic;
            pMesh->cutOffAngle = cutOffAngle;
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }
}
