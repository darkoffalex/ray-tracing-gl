/**
 * Класс источника света.
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include "SceneElement.h"
#include <GL/glew.h>

namespace rtgl
{
    class LightSource final : public SceneElement
    {
    public:
        /// Тип источника света, от него зависит то как источник обрабатывается шейдером
        LightSourceType type = LightSourceType::LIGHT_POINT;

        /// Радиус сферы источника света (считаем источник света сферичкеским)
        GLfloat radius = 0.0f;

        /// Цвет источника света
        glm::vec3 color = {1.0f,1.0f,1.0f};

        /// Линейный коэффициент затухания
        GLfloat attenuationLinear = 0.20f;

        /// Квадратичный коэффициент затухания
        GLfloat attenuationQuadratic = 0.22f;

        /// Внутренний угол отсечения света (для направленных прожекторов)
        GLfloat cutOffAngle = 40.0f;

        /// Внешний угол отсечения света (для направленных прожекторов)
        GLfloat cutOffOuterAngle = 45.0f;

        /**
         * Передать параметры источника в шейдер (запись в uniform-буфер с выравниванием std-140)
         * @param bufferId ID буфера
         * @param offset Байтовое смещение
         */
        void writeToUniformBufferStd140(const GLuint& bufferId, GLsizei offset = 0) const;
    };
}


