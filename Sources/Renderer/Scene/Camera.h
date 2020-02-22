/**
 * Класс камеры. Используется для построения матрицы проекции с учетом всех установок
 * Матрица обновляется только после изменения параметров от которых она зависит
 * В данной версии рендерера большая часть функионала данного класса не используется, поскольку в проекции нет нужды
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include "SceneElement.h"
#include <GL/gl.h>

namespace rtgl
{
    class Camera : public SceneElement
    {

    private:
        /// Матрица проекции
        glm::mat4 projectionMatrix_;

        /// Тип проекции
        ProjectionType projectionType_;

        /// Ближняя грань отсечения
        GLfloat zNear_;

        /// Дальняя грань отсечения
        GLfloat zFar_;

        /// Угол обзора либо размер области видимости (для ортогональной проекции)
        GLfloat fov_;

        /// Соотношение сторон вью-порта
        GLfloat aspectRatio_;

        /**
         * Обновление матрицы проекции с учетом всех параметров камеры
         */
        void updateProjectionMatrix();

    public:
        /**
         * Конструктор по умолчанию
         */
        Camera();

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
        explicit Camera(
                const glm::vec3& position,
                const glm::vec3& orientation,
                const GLfloat& aspectRatio,
                const ProjectionType& projectionType = ProjectionType::PROJ_PERSPECTIVE,
                const GLfloat& zNear = 0.1f,
                const GLfloat& zFar = 1000.0f,
                const GLfloat& fov = 45.0f);

        /**
         * Получить матрицу проекции
         * @return Матрица 4*4
         */
        [[nodiscard]] const glm::mat4& getProjectionMatrix() const;

        /**
         * Установить тип проекции
         * @param projectionType Тип проекции (enum)
         */
        void setProjectionType(const ProjectionType& projectionType);

        /**
         * Получить тип проекции
         * @return Тип проекции (enum)
         */
        [[nodiscard]] const ProjectionType& getProjectionType() const;

        /**
         * Установить ближнюю грань используемую при построении матрицы проекции
         * @param zNear Ближняя грань
         */
        void setZNear(const float& zNear);

        /**
         * Получить ближнюю грань используемую при построении матрицы проекции
         * @return Ближняя грань
         */
        [[nodiscard]] const float& getZNear() const;

        /**
         * Установить дальнюю грань используемую при построении матрицы проекции
         * @param zFar Дальняя грань
         */
        void setZFar(const float& zFar);

        /**
         * Получить дальнюю грань используемую при построении матрицы проекции
         * @return Дальняя грань
         */
        [[nodiscard]] const float& getZFar() const ;

        /**
         * Установить угол (поле) обзора
         * @param fov Угол (при перспективной проекции) либо поле (при ортогональной) обзора
         */
        void setFov(const float& fov);

        /**
         * Получить угол (поле) обзора
         * @return Угол (при перспективной проекции) либо поле (при ортогональной) обзора
         */
        [[nodiscard]] const float& getFov() const;

        /**
         * Усиановить соотношение сторон
         * @param aspectRatio
         */
        void setAspectRatio(const float& aspectRatio);

        /**
         * Получить соотношение сторон
         * @return Отношение ширины к высоте
         */
        [[nodiscard]] const float& getAspectRatio() const;
    };
}


