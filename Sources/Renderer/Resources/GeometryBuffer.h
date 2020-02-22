/**
 * Класс геометрического буфера - обертка для работы с OpenGL сущностями VAO
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace rtgl
{
    class GeometryBuffer final
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec2 uv;
            glm::vec3 normal;
        };

    private:
        /// OpenGL дескриптор вершинного буфера
        GLuint vboId_;
        /// OpenGL дескриптор индексного буфера
        GLuint eboId_;
        /// OpenGL дескриптор VAO объекта
        GLuint vaoId_;

        /// Кол-во вершин
        GLsizei vertexCount_;
        /// Кол-во индексов основной геометрии
        GLsizei indexCount_;

        /**
         * Конфигурация вершинных атрибутов
         * Пояснения шейдеру как понимать данные из активного VBO (буфера вершин) в контексте активного VAO
         */
        static void setupVertexAttributes();

    public:
        /**
         * Запрет копирования через инициализацию
         * @param other Ссылка на копируемый объекта
         */
        GeometryBuffer(const GeometryBuffer& other) = delete;

        /**
         * Запрет копирования через присваивание
         * @param other Ссылка на копируемый объекта
         * @return Ссылка на текущий объект
         */
        GeometryBuffer& operator=(const GeometryBuffer& other) = delete;

        /**
         * Конструктор перемещения
         * @param other R-value ссылка на другой объект
         * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
         */
        GeometryBuffer(GeometryBuffer&& other) noexcept;

        /**
         * Перемещение через присваивание
         * @param other other R-value ссылка на другой объект
         * @return Нельзя копировать объект, но можно обменяться с ним ресурсом
         */
        GeometryBuffer& operator=(GeometryBuffer&& other) noexcept;

        /**
         * Конструктор ресурса
         * @param vertices Массив вершин
         * @param indices Массив индексов
         */
        GeometryBuffer(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);

        /**
         * Очистка ресурса
         */
        ~GeometryBuffer();

        /**
         * Получить кол-во вершин
         * @return Число вершин
         */
        [[nodiscard]] GLuint getVertexCount() const;

        /**
         * Получить кол-во индексов
         * @return Число индексов
         */
        [[nodiscard]] GLuint getIndexCount() const;

        /**
         * Получить ID OpenGL объекта VAO (Vertex Array Object)
         * @return Дескриптор OpenGL объекта
         */
        [[nodiscard]] GLuint getVaoId() const;
    };
}
