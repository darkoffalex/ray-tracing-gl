#include "Camera.h"

/**
 * Задать вектор скорости движения
 * @param localTranslationVector Локальный вектор скорости
 */
void Camera::setTranslation(const math::Vec3<float> &localTranslationVector)
{
    translationVectorRelative_ = localTranslationVector;
}

/**
 * Задать абсолютный вектор скорости движения
 * @param absoluteTranslationVector Абсолютный вектор скорости
 */
void Camera::setTranslationAbsolute(const math::Vec3<float> &absoluteTranslationVector)
{
    translationVectorAbsolute_ = absoluteTranslationVector;
}

/**
 * Задать вращение
 * @param rotationSpeed Вектор угловой скорости
 */
void Camera::setRotation(const math::Vec3<float> &rotationSpeed)
{
    rotation_ = rotationSpeed;
}

/**
 * Движение - прирастить положение и ориентацию с учетом пройденного времени
 * @param deltaTime Время кадра
 */
void Camera::translate(const float &deltaTime)
{
    // Получить значение абсолютного вектора движения с учетом поворота камеры и локального вектора движения
    const math::Mat3<float> rotMatrix = math::GetRotationMat(this->orientation);
    const math::Vec3<float> absTranslation = rotMatrix * this->translationVectorRelative_;

    // Приращение абсолютных значений
    this->position = this->position + ((this->translationVectorAbsolute_ + absTranslation) * deltaTime);
    this->orientation = this->orientation + (this->rotation_ * deltaTime);
}
