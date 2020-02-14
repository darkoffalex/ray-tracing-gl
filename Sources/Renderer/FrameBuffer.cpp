/**
 * Класс кадрового буфера - обертка для работы с кадровыми буферами OpenGL
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#include "FrameBuffer.h"

namespace rtgl
{
    /**
     * Конструктор перемещения
     * @param other R-value ссылка на другой объект
     * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
     */
    FrameBuffer::FrameBuffer(FrameBuffer &&other) noexcept :
            id_(other.id_),
            sizes_(other.sizes_)
    {
        other.id_ = 0;
        other.sizes_ = {};
        std::swap(this->textureAttachments_, other.textureAttachments_);
        std::swap(this->textureAttachmentBindings_, other.textureAttachmentBindings_);
        std::swap(this->renderBufferAttachments_, other.renderBufferAttachments_);
        std::swap(this->renderBufferAttachmentBindings_, other.renderBufferAttachmentBindings_);
    }

    /**
     * Перемещение через присваивание
     * @param other R-value ссылка на другой объект
     * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
     * @return Ссылка на текущий объект
     */
    FrameBuffer &FrameBuffer::operator=(FrameBuffer &&other) noexcept
    {
        // Если присваивание самому себе - просто вернуть ссылку на этот объект
        if (&other == this) return *this;

        // Удалить ресурсы которыми владеет текущий объект
        if (!textureAttachments_.empty()) glDeleteTextures(textureAttachments_.size(), textureAttachments_.data());
        if (!renderBufferAttachments_.empty()) glDeleteRenderbuffers(renderBufferAttachments_.size(), renderBufferAttachments_.data());
        if (id_ != 0) glDeleteFramebuffers(1, &id_);

        // Очистить дескрипторы
        textureAttachments_.clear(); textureAttachments_.shrink_to_fit();
        textureAttachmentBindings_.clear(); textureAttachmentBindings_.shrink_to_fit();
        renderBufferAttachments_.clear(); renderBufferAttachments_.shrink_to_fit();
        renderBufferAttachmentBindings_.clear(); renderBufferAttachmentBindings_.shrink_to_fit();
        id_ = 0;

        // Обмен
        std::swap(id_, other.id_);
        std::swap(sizes_, other.sizes_);
        std::swap(this->textureAttachments_, other.textureAttachments_);
        std::swap(this->textureAttachmentBindings_, other.textureAttachmentBindings_);
        std::swap(this->renderBufferAttachments_, other.renderBufferAttachments_);
        std::swap(this->renderBufferAttachmentBindings_, other.renderBufferAttachmentBindings_);

        // Вернуть ссылку на этот объект
        return *this;
    }

    /**
     * Конструктор буфера
     * @param width Ширина
     * @param height Высота
     */
    FrameBuffer::FrameBuffer(GLsizei width, GLsizei height) :
            id_(0),
            sizes_({width,height})
    {
        glGenFramebuffers(1, &id_);
    }

    /**
     * Очистка памяти, удаление текстур
     */
    FrameBuffer::~FrameBuffer()
    {
        // Удалить ресурсы которыми владеет текущий объект
        if (!textureAttachments_.empty()) glDeleteTextures(textureAttachments_.size(), textureAttachments_.data());
        if (!renderBufferAttachments_.empty()) glDeleteRenderbuffers(renderBufferAttachments_.size(), renderBufferAttachments_.data());
        if (id_ != 0) glDeleteFramebuffers(1, &id_);
    }

    /**
     * Получить дескриптор ресурса
     * @return Дескриптор
     */
    GLuint FrameBuffer::getId() const
    {
        return id_;
    }

    /**
     * Получить массив дескрипторов текстурных вложений
     * @return Ссылка на массив
     */
    const std::vector<GLuint> &FrameBuffer::getTextureAttachments() const
    {
        return this->textureAttachments_;
    }

    /**
     * Получить массив дескрипторов рендер-буферных вложений
     * @return Ссылка на массив
     */
    const std::vector<GLuint> &FrameBuffer::getRenderBufferAttachments() const
    {
        return this->renderBufferAttachments_;
    }

    /**
     * Добавить текстурное вложение
     * @param internalFormat Внутренний формат (формат хранения)
     * @param format Внешний формат (формат при выборке/записи)
     * @param attachmentBindingId Идентификатор вложения у фрейм-буфера
     * @param mip Использовать мип-уровни
     */
    void FrameBuffer::addTextureAttachment(GLuint internalFormat, GLuint format, GLuint attachmentBindingId, bool mip)
    {
        GLuint id;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, sizes_.width, sizes_.height, 0, format, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mip ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mip ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        if (mip) glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        this->textureAttachments_.push_back(id);
        this->textureAttachmentBindings_.push_back(attachmentBindingId);
    }

    /**
     * Добавить рендер-буферное вложение
     * @param internalFormat Внутренний формат (формат хранения)
     * @param attachmentBindingId Идентификатор вложения у фрейм-буфера
     */
    void FrameBuffer::addRenderBufferAttachment(GLuint internalFormat, GLuint attachmentBindingId)
    {
        GLuint id;
        glGenRenderbuffers(1, &id);
        glBindRenderbuffer(GL_RENDERBUFFER, id);
        glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, sizes_.width, sizes_.height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        this->renderBufferAttachments_.push_back(id);
        this->renderBufferAttachmentBindings_.push_back(attachmentBindingId);
    }

    /**
     * Собрать кадровый буфер используя добавленные ранее компоненты
     * @param drawToAttachments Список номеров вложений доступных для рендеринга
     * @return Состояния сборки
     */
    bool FrameBuffer::prepareBuffer(const std::vector<GLuint> &drawToAttachments)
    {
        // Если пусты массивы дескрипторов вложений
        if (this->textureAttachments_.empty() && this->renderBufferAttachments_.empty()) return false;

        // Работает с кадровым буфером
        glBindFramebuffer(GL_FRAMEBUFFER, id_);

        // Добавить текстурные вложения
        for (unsigned i = 0; i < this->textureAttachments_.size(); i++)
        {
            glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    this->textureAttachmentBindings_[i],
                    GL_TEXTURE_2D, this->textureAttachments_[i],
                    0);
        }

        // Добавить рендер-буферные вложения
        for (unsigned i = 0; i < this->renderBufferAttachments_.size(); i++)
        {
            glFramebufferRenderbuffer(
                    GL_FRAMEBUFFER,
                    this->renderBufferAttachmentBindings_[i],
                    GL_RENDERBUFFER,
                    this->renderBufferAttachments_[i]);
        }

        // Указать какие вложения будут использованы для рендеринга
        if(!drawToAttachments.empty()) glDrawBuffers(drawToAttachments.size(), drawToAttachments.data());

        // Если фрейм-буфер не готов
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            return false;
        }

        // Прекращаем работу с фрейм-буфером
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Буфер готов
        return true;
    }

    /**
     * Получить ширину кадра в пикселях
     * @return Ширина
     */
    const GLsizei &FrameBuffer::getWidth() const
    {
        return this->sizes_.width;
    }

    /**
     * Получить высоту кадра в пикселях
     * @return Высота
     */
    const GLsizei &FrameBuffer::getHeight() const
    {
        return this->sizes_.height;
    }
}