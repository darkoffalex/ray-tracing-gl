/**
 * Класс шейдерной программы - обертка для работы с шейдерами OpenGL
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#include "ShaderProgram.h"

#include <vector>

namespace rtgl
{
    /**
     * Компиляция исходного кода шейдера
     * @param shaderSource Исходный код шейдера
     * @param type Тип шейдера
     * @return Идентификатор ресурса шейдера
     */
    GLuint ShaderProgram::compileShader(const GLchar *shaderSource, GLuint type)
    {
        // Зарегистрировать шейдер нужного типа
        const GLuint id = glCreateShader(type);

        // Связать исходный код и шейдер
        glShaderSource(id, 1, &shaderSource, nullptr);

        // Компиляция шейдера
        glCompileShader(id);

        // Успешна ли компиляция
        GLint success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);

        // Если не скомпилирован - генерация исключения
        if (!success) {
            GLsizei messageLength = 0;
            GLchar message[1024];
            glGetShaderInfoLog(id, 1024, &messageLength, message);
            throw std::runtime_error(std::string("Shader compilation failed: ").append(message).c_str());
        }

        // Вернуть дескриптор шейдера
        return id;
    }

    /**
     * Получение локаций uniform-переменных
     * @details Чтобы не получать информацию об uniform-переменных во время рендеринга это делается при инициализации
     */
    void ShaderProgram::obtainLocations()
    {
        // Этап построения геометрии
        this->locations_.model = glGetUniformLocation(id_, "_model");
        this->locations_.view = glGetUniformLocation(id_, "_view");
        this->locations_.materialAlbedo = glGetUniformLocation(id_, "_materialAlbedo");
        this->locations_.materialMetallic = glGetUniformLocation(id_, "_materialMetallic");
        this->locations_.materialRoughness = glGetUniformLocation(id_, "_materialRoughness");

        // Этап трассировки
        this->locations_.aspectRatio = glGetUniformLocation(id_, "_aspectRatio");
        this->locations_.fov = glGetUniformLocation(id_, "_fov");
        this->locations_.camPosition = glGetUniformLocation(id_, "_camPosition");

        // Этап пост-процессинга
        this->locations_.screenTexture = glGetUniformLocation(id_, "_screenTexture");
    }

    /**
     * Конструктор перемещения
     * @param other R-value ссылка на другой объект
     * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
     */
    ShaderProgram::ShaderProgram(ShaderProgram &&other) noexcept
            :id_(other.id_)
    {
        // Поскольку подразумевается что обмен происходит с новым объектом
        // то объект, что отдал свой ресурс, получает в замен пустой дескриптор ресурса
        other.id_ = 0;
        other.locations_ = {};
    }

    /**
     * Перемещение через присваивание
     * @param other R-value ссылка на другой объект
     * @return Нельзя копировать объект, но можно обменяться с ним ресурсом
     */
    ShaderProgram &ShaderProgram::operator=(ShaderProgram &&other) noexcept
    {
        // Если присваивание самому себе - просто вернуть ссылку на этот объект
        if (&other == this) return *this;

        // Удалить ресурс которым владеет объект, обнулить дескриптор
        if (id_) glDeleteProgram(id_);
        id_ = 0;
        locations_ = {};

        // Обменять ресурсы объектов
        std::swap(this->id_, other.id_);
        std::swap(this->locations_,other.locations_);

        // Вернуть ссылку на этот объект
        return *this;
    }

    /**
     * Конструктор ресурса
     * @param shaderSources Ассоциативный массив (тип => исходный код шейдера)
     */
    ShaderProgram::ShaderProgram(const std::unordered_map<GLuint, std::string>& shaderSources)
    {
        // Зарегистрировать шейдерную программу
        this->id_ = glCreateProgram();

        // ID'ы шейдеров (чтобы освободить память после сборки программы)
        std::vector<GLuint> shaderIds;

        // Пройтись по всем элементам ассоциативного массива
        for (auto & shaderSource : shaderSources)
        {
            // Если код шейдера не пуст
            if (!shaderSource.second.empty())
            {
                // Скомпилировать шейдер
                GLuint shaderId = ShaderProgram::compileShader(shaderSource.second.c_str(), shaderSource.first);
                // Добавить шейдер к программе
                glAttachShader(this->id_, shaderId);
                // Добавить в список ID'ов
                shaderIds.push_back(shaderId);
            }
        }

        // Собрать шейдерную программу
        glLinkProgram(this->id_);

        // Удалить шейдеры (после сборки шейдерной программы они уже не нужны в памяти)
        for (const GLuint& shaderId : shaderIds) {
            glDeleteShader(shaderId);
        }

        // Проверка ошибок сборки шейдерной программы
        GLint success;
        glGetProgramiv(this->id_, GL_LINK_STATUS, &success);

        // Если не удалось собрать программу
        if (!success) {
            GLsizei messageLength = 0;
            GLchar message[1024];
            glGetProgramInfoLog(this->id_, 1024, &messageLength, message);

            throw std::runtime_error(std::string("Shader program linking error: ").append(message));
        }

        // Получить локации uniform-переменныхв
        this->obtainLocations();
    }

    /**
     * Получить дескриптор шейдерной программы
     * @return OpenGL дескриптор
     */
    GLuint ShaderProgram::getId() const
    {
        return this->id_;
    }

    /**
     * Получить набор локаций uniform-переменных
     * @return Указатель на структуру с набором локаций
     */
    const ShaderProgram::UniformLocations *ShaderProgram::getUniformLocations() const
    {
        return &locations_;
    }
}
