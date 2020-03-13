/**
 * Класс геометрического меша. Каждый меш обладает своей геометрией, метериалом и текстурой
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include "SceneElement.h"
#include "../Resources/GeometryBuffer.h"
#include "../Resources/ShaderProgram.h"

#include <GL/gl.h>
#include <glm/glm.hpp>

namespace rtgl
{
    class Mesh : public SceneElement
    {
        /// Описание метариала (PBR)
        struct Material
        {
            glm::vec3 albedo = { 1.0f,1.0f,1.0f };
            GLfloat metallic = 0.0f;
            GLfloat roughness = 1.0f;
            GLfloat primaryToSecondary = 1.0f;
            GLfloat reflectionToRefraction = 1.0f;
            GLfloat refractionCoff = 0.6f;
        };

    public:
        /// Дескриптор буфера геометрии
        GeometryBuffer* geometry;

        /// Параметры материала меша
        Material material;

        /**
         * Передача информации о текстуре в шейдер
         * @param shaderProgram Указатель на шейдерную программу
         */
        void passTextureInfoToShader(const ShaderProgram* shaderProgram) const;

        /**
         * Передача информации о метариале в шейдерную программу
         * @param shaderProgram Указатель на шейдерную программу
         */
        void passMaterialToShader(const ShaderProgram* shaderProgram) const;
    };
}
