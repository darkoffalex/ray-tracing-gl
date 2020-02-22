/**
 * Класс кадрового буфера - обертка для работы с кадровыми буферами OpenGL
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace rtgl
{
    class FrameBuffer final
    {
    private:
        /// Идентификатор кадрового буфера
        GLuint id_;
        /// Размеры буфера
        struct { GLsizei width; GLsizei height; } sizes_;
        /// Текстурные вложения (как правило это цветовые вложения из которых затем можно делать выборку в шейдере)
        std::vector<GLuint> textureAttachments_;
        /// Идентификаторы вложений для каждого текстурного вложения
        std::vector<GLuint> textureAttachmentBindings_;
        /// Буферные вложения (рендер-буферы аналогичны текстурам, но не пригодны для выборки в шейдерах)
        std::vector<GLuint> renderBufferAttachments_;
        /// Идентификаторы вложений для каждого рендер-буфера
        std::vector<GLuint> renderBufferAttachmentBindings_;

    public:
        /**
         * Запрет копирования через инициализацию
         * @param other Ссылка на копируемый объекта
         */
        FrameBuffer(const FrameBuffer& other) = delete;

        /**
         * Запрет копирования через присваивание
         * @param other Ссылка на копируемый объекта
         * @return Ссылка на текущий объект
         */
        FrameBuffer& operator=(const FrameBuffer& other) = delete;

        /**
         * Конструктор перемещения
         * @param other R-value ссылка на другой объект
         * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
         */
        FrameBuffer(FrameBuffer&& other) noexcept;

        /**
         * Перемещение через присваивание
         * @param other R-value ссылка на другой объект
         * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
         * @return Ссылка на текущий объект
         */
        FrameBuffer& operator=(FrameBuffer&& other) noexcept;

        /**
         * Конструктор буфера
         * @param width Ширина
         * @param height Высота
         */
        FrameBuffer(GLsizei width, GLsizei height);

        /**
         * Очистка памяти, удаление текстур
         */
        ~FrameBuffer();

        /**
         * Получить дескриптор ресурса
         * @return Дескриптор
         */
        [[nodiscard]] GLuint getId() const;

        /**
         * Получить массив дескрипторов текстурных вложений
         * @return Ссылка на массив
         */
        [[nodiscard]] const std::vector<GLuint>& getTextureAttachments() const;

        /**
         * Получить массив дескрипторов рендер-буферных вложений
         * @return Ссылка на массив
         */
        [[nodiscard]] const std::vector<GLuint>& getRenderBufferAttachments() const;

        /**
         * Добавить текстурное вложение
         * @param internalFormat Внутренний формат (формат хранения)
         * @param format Внешний формат (формат при выборке/записи)
         * @param attachmentBindingId Идентификатор вложения у фрейм-буфера
         * @param mip Использовать мип-уровни
         */
        void addTextureAttachment(
                GLuint internalFormat = GL_RGBA,
                GLuint format = GL_RGBA,
                GLuint attachmentBindingId = GL_COLOR_ATTACHMENT0,
                bool mip = false);

        /**
         * Добавить рендер-буферное вложение
         * @param internalFormat Внутренний формат (формат хранения)
         * @param attachmentBindingId Идентификатор вложения у фрейм-буфера
         */
        void addRenderBufferAttachment(
                GLuint internalFormat = GL_DEPTH24_STENCIL8,
                GLuint attachmentBindingId = GL_DEPTH_STENCIL_ATTACHMENT);

        /**
         * Собрать кадровый буфер используя добавленные ранее компоненты
         * @param drawToAttachments Список номеров вложений доступных для рендеринга
         * @return Состояния сборки
         */
        bool prepareBuffer(const std::vector<GLuint>& drawToAttachments);

        /**
         * Получить ширину кадра в пикселях
         * @return Ширина
         */
        [[nodiscard]] const GLsizei& getWidth() const;

        /**
         * Получить высоту кадра в пикселях
         * @return Высота
         */
        [[nodiscard]] const GLsizei& getHeight() const;
    };
}

