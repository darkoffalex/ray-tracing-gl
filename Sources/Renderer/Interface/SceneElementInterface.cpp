/**
 * С-интерфейс для взаимодействия с объектами класса (или его потомков) элемента сцены
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#include "SceneElementInterface.h"
#include "../Scene/SceneElement.h"

#include <stdexcept>

namespace rtgl
{
    /// Сообщение о последней ошибке (объявлено в Globals.h->Renderer.cpp)
    extern std::string _strLastErrorMsg;
    /// Инициализирована ли библиотека (объявлено в Globals.h->Renderer.cpp)
    extern bool _bInitialized;

    /**
     * Настройка параметров элемента сцены
     * @param sceneElement Хендл элемента сцены
     * @param position Положение
     * @param orientation Ориентация
     * @param scale Масштаб
     * @param origin Локальный центр
     * @return Состояние операции
     */
    bool __cdecl SetSpatialSettings(HSceneElement sceneElement,
            const Vec3<float> &position,
            const Vec3<float> &orientation,
            const Vec3<float> &scale,
            const Vec3<float> &origin)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            auto element = reinterpret_cast<SceneElement*>(sceneElement);
            element->setPosition({position.x,position.y,position.z}, false);
            element->setOrientation({orientation.x,orientation.y,orientation.z}, false);
            element->setScale({scale.x,scale.y,scale.z}, false);
            element->setOrigin({origin.x, origin.y, origin.z},true);
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }

    /**
     * Настройка положение объекта сцены
     * @param sceneElement Хендл элемента сцены
     * @param position Положение
     * @return Состояние операции
     */
    bool __cdecl SetPosition(HSceneElement sceneElement, const Vec3<float> &position)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            auto element = reinterpret_cast<SceneElement*>(sceneElement);
            element->setPosition({position.x,position.y,position.z});
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }

    /**
     * Настройка ориентации объекта сцены
     * @param sceneElement
     * @param orientation
     * @return Состояние операции
     */
    bool __cdecl SetOrientation(HSceneElement sceneElement, const Vec3<float> &orientation)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            auto element = reinterpret_cast<SceneElement*>(sceneElement);
            element->setOrientation({orientation.x,orientation.y,orientation.z});
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }

    /**
     * Настройка масштаба объекта сцены
     * @param sceneElement Хендл элемента сцены
     * @param scale Масштаб
     * @return Состояние операции
     */
    bool __cdecl SetScale(HSceneElement sceneElement, const Vec3<float> &scale)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            auto element = reinterpret_cast<SceneElement*>(sceneElement);
            element->setOrientation({scale.x,scale.y,scale.z});
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }

    /**
     * Настройка локального центра объекта сцены
     * @param sceneElement Хендл элемента сцены
     * @param origin Локальный центр
     * @return Состояние операции
     */
    bool __cdecl SetOrigin(HSceneElement sceneElement, const Vec3<float> &origin)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");

            auto element = reinterpret_cast<SceneElement*>(sceneElement);
            element->setOrigin({origin.x, origin.y, origin.z});
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }
}