/**
 * Класс камеры. Используется для построения матрицы проекции с учетом всех установок
 * Матрица обновляется только после изменения параметров от которых она зависит
 * В данной версии рендерера большая часть функионала данного класса не используется, поскольку в проекции нет нужды
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace rtgl
{
    /**
     * Конструктор по умолчанию
     */
    Camera::Camera():
            SceneElement({0.0f,0.0f,0.0f}),
            projectionMatrix_({}),
            aspectRatio_(1.0f),
            projectionType_(ProjectionType::PROJ_PERSPECTIVE),
            zNear_(0.1f),
            zFar_(1000.0f),
            fov_(45.0f)
    {
        this->updateProjectionMatrix();
    }

    /**
     * Явный конструктор
     * @param projectionType Тип проекции
     * @param position Положение
     * @param orientation Ориентация
     * @param aspectRatio Соотношение сторон
     * @param zNear Ближняя грань отсечения
     * @param zFar Дальняя грань отсечения
     * @param fov Угол (поле) обзора
     */
    Camera::Camera(const glm::vec3 &position, const glm::vec3 &orientation, const GLfloat& aspectRatio,
            const ProjectionType &projectionType, const GLfloat &zNear, const GLfloat &zFar,
            const GLfloat &fov):
            SceneElement(position,orientation),
            projectionMatrix_({}),
            aspectRatio_(aspectRatio),
            projectionType_(projectionType),
            zNear_(zNear),
            zFar_(zFar),
            fov_(fov)
    {
        this->updateProjectionMatrix();
    }

    /**
     * Обновление матрицы проекции с учетом всех параметров камеры
     */
    void Camera::updateProjectionMatrix()
    {
        switch (this->projectionType_)
        {
            case ProjectionType::PROJ_PERSPECTIVE:
                this->projectionMatrix_ = glm::perspective(glm::radians(fov_), aspectRatio_, zNear_, zFar_);
                break;
            case ProjectionType::PROJ_ORTHOGRAPHIC:
                this->projectionMatrix_ = glm::ortho(-fov_ * aspectRatio_ / 2.0f, fov_ * aspectRatio_ / 2.0f, -fov_ / 2.0f, fov_ / 2.0f, zNear_, zFar_);
                break;
            default:
                this->projectionMatrix_ = glm::mat4(1);
                break;
        }
    }

    /**
     * Получить матрицу проекции
     * @return Матрица 4*4
     */
    const glm::mat4 &Camera::getProjectionMatrix() const
    {
        return this->projectionMatrix_;
    }

    /**
     * Установить тип проекции
     * @param projectionType Тип проекции (enum)
     */
    void Camera::setProjectionType(const ProjectionType &projectionType)
    {
        this->projectionType_ = projectionType;
        this->updateProjectionMatrix();
    }

    /**
     * Получить тип проекции
     * @return Тип проекции (enum)
     */
    const ProjectionType &Camera::getProjectionType() const
    {
        return projectionType_;
    }

    /**
     * Установить ближнюю грань используемую при построении матрицы проекции
     * @param zNear Ближняя грань
     */
    void Camera::setZNear(const float &zNear)
    {
        this->zNear_ = zNear;
        this->updateProjectionMatrix();
    }

    /**
     * Получить ближнюю грань используемую при построении матрицы проекции
     * @return Ближняя грань
     */
    const float &Camera::getZNear() const
    {
        return zNear_;
    }

    /**
     * Установить дальнюю грань используемую при построении матрицы проекции
     * @param zFar Дальняя грань
     */
    void Camera::setZFar(const float &zFar)
    {
        this->zFar_ = zFar;
        this->updateProjectionMatrix();
    }

    /**
     * Получить дальнюю грань используемую при построении матрицы проекции
     * @return Дальняя грань
     */
    const float &Camera::getZFar() const
    {
        return zFar_;
    }

    /**
     * Установить угол (поле) обзора
     * @param fov Угол (при перспективной проекции) либо поле (при ортогональной) обзора
     */
    void Camera::setFov(const float &fov)
    {
        this->fov_ = fov;
        this->updateProjectionMatrix();
    }

    /**
     * Получить угол (поле) обзора
     * @return Угол (при перспективной проекции) либо поле (при ортогональной) обзора
     */
    const float &Camera::getFov() const
    {
        return fov_;
    }

    /**
     * Усиановить соотношение сторон
     * @param aspectRatio
     */
    void Camera::setAspectRatio(const float &aspectRatio)
    {
        this->aspectRatio_ = aspectRatio;
        this->updateProjectionMatrix();
    }

    /**
     * Получить соотношение сторон
     * @return Отношение ширины к высоте
     */
    const float &Camera::getAspectRatio() const
    {
        return aspectRatio_;
    }

}
