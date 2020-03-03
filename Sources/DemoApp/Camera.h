#pragma once

#include "Math.hpp"

class Camera
{
private:
    /// Скорость движения (локальная)
    math::Vec3<float> translationVectorRelative_ = { 0.0f,0.0f,0.0f };
    /// Скорость движения (абсолютная)
    math::Vec3<float> translationVectorAbsolute_ = { 0.0f,0.0f,0.0f };
    /// Скорость вращения
    math::Vec3<float> rotation_ = { 0.0f,0.0f,0.0f };

public:
    /// Абсолютное положение в пространстве
    math::Vec3<float> position = { 0.0f,0.0f,0.0f };
    /// Ориентация
    math::Vec3<float> orientation = { 0.0f,0.0f,0.0f };

    /**
     * Задать вектор скорости движения
     * @param localTranslationVector Локальный вектор скорости
     */
    void setTranslation(const math::Vec3<float>& localTranslationVector);

    /**
     * Задать абсолютный вектор скорости движения
     * @param absoluteTranslationVector Абсолютный вектор скорости
     */
    void setTranslationAbsolute(const math::Vec3<float>& absoluteTranslationVector);

    /**
     * Задать вращение
     * @param rotationSpeed Вектор угловой скорости
     */
    void setRotation(const math::Vec3<float>& rotationSpeed);

    /**
     * Движение - прирастить положение и ориентацию с учетом пройденного времени
     * @param deltaTime Время кадра
     */
    void translate(const float& deltaTime);
};
