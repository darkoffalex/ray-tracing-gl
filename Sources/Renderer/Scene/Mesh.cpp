/**
 * Класс геометрического меша. Каждый меш обладает своей геометрией, метериалом и текстурой
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#include "Mesh.h"
#include <glm/gtc/type_ptr.inl>

namespace rtgl
{
    /**
     * Передача информации о текстуре в шейдер
     * @param shaderProgram Указатель на шейдерную программу
     */
    void Mesh::passTextureInfoToShader(const ShaderProgram *shaderProgram) const
    {
        //TODO: реализация метода
    }

    /**
     * Передача информации о метариале в шейдерную программу
     * @param shaderProgram Указатель на шейдерную программу
     */
    void Mesh::passMaterialToShader(const ShaderProgram *shaderProgram) const
    {
        glUniform3fv(shaderProgram->getUniformLocations()->materialAlbedo, 1, glm::value_ptr(this->material.albedo));
        glUniform1f(shaderProgram->getUniformLocations()->materialMetallic, this->material.metallic);
        glUniform1f(shaderProgram->getUniformLocations()->materialRoughness, this->material.roughness);
    }
}
