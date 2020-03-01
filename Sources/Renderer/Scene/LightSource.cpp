/**
 * Класс источника света.
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#include "LightSource.h"
#include <glm/gtc/type_ptr.inl>

namespace rtgl
{
    /**
     * Передать параметры источника в шейдер (запись в uniform-буфер с выравниванием std-140)
     * @param bufferId ID буфера
     * @param offset Байтовое смещение
     */
    void LightSource::writeToUniformBufferStd140(const GLuint &bufferId, GLsizei offset) const
    {
        // Вектор направления источника с учетом текущих углов
        //auto orientationVector = glm::vec3(this->getModelMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
        auto orientationVector = glm::vec3(0.0f, 0.0f, -1.0f);

        // Косинусы углов отсечения для источника-прожектора
        auto cutOffAngleCos = glm::cos(glm::radians(this->cutOffAngle));
        auto cutOffOuterAngleCos = glm::cos(glm::radians(this->cutOffOuterAngle));

        // Запись в UBO
        glBindBuffer(GL_UNIFORM_BUFFER, bufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, glm::value_ptr(this->getPosition()));
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 12, 4, &this->radius);
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, 12, glm::value_ptr(this->color));
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 32, 12, glm::value_ptr(orientationVector));
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 44, 4, &this->attenuationQuadratic);
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 48, 4, &this->attenuationLinear);
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 52, 4, &cutOffAngleCos);
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 56, 4, &cutOffOuterAngleCos);
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 60, 4, &this->type);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}
