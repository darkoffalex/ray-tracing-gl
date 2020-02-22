/**
 * Класс шейдерной программы - обертка для работы с шейдерами OpenGL
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include <GL/glew.h>
#include <unordered_map>

namespace rtgl
{
    class ShaderProgram final
    {
        /// Набор идентификаторов локаций uniform-переменных в шейдерной программе
        struct UniformLocations
        {
            GLuint model = 0;
            GLuint view = 0;
            GLuint fov = 0;
            GLuint camPosition = 0;
            GLuint screenTexture = 0;
            GLuint screenResolution = 0;
        };

    private:
        /// Идентификатор рескрса шейдерной программы
        GLuint id_;
        /// Индексы локаций uniform-переменных в шейдерной программе
        UniformLocations locations_;

        /**
         * Компиляция исходного кода шейдера
         * @param shaderSource Исходный код шейдера
         * @param type Тип шейдера
         * @return Идентификатор ресурса шейдера
         */
        static GLuint compileShader(const GLchar* shaderSource, GLuint type);

        /**
         * Получение локаций uniform-переменных
         * @details Чтобы не получать информацию об uniform-переменных во время рендеринга это делается при инициализации
         */
        void obtainLocations();

    public:
        /**
         * Запрет копирования через инициализацию
         * @param other Ссылка на копируемый объект
         */
        ShaderProgram(const ShaderProgram& other) = delete;

        /**
         * Запрет копирования через присваивание
         * @param other Ссылка на копируемый объект
         * @return Ссылка на текущий объект
         */
        ShaderProgram& operator=(const ShaderProgram& other) = delete;

        /**
         * Конструктор перемещения
         * @param other R-value ссылка на другой объект
         * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
         */
        ShaderProgram(ShaderProgram&& other) noexcept;

        /**
         * Перемещение через присваивание
         * @param other R-value ссылка на другой объект
         * @return Нельзя копировать объект, но можно обменяться с ним ресурсом
         */
        ShaderProgram& operator=(ShaderProgram&& other) noexcept;

        /**
         * Конструктор ресурса
         * @param shaderSources Ассоциативный массив (тип => исходный код шейдера)
         */
        explicit ShaderProgram(const std::unordered_map<GLuint, std::string>& shaderSources);

        /**
         * Получить дескриптор шейдерной программы
         * @return OpenGL дескриптор
         */
        [[nodiscard]] GLuint getId() const;

        /**
         * Получить набор локаций uniform-переменных
         * @return Указатель на структуру с набором локаций
         */
        [[nodiscard]] const UniformLocations* getUniformLocations() const;
    };
}
