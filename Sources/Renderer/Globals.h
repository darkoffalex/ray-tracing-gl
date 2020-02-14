/**
 * Файл с глобальными переменными, определяемыми в момент инициализации библиотеки
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include <string>
#include <GL/glew.h>

#include "FrameBuffer.h"
#include "GeometryBuffer.h"
#include "ShaderProgram.h"

namespace rtgl
{
    /// Готова ли библиотека к использованию
    bool _bInitialized = false;

    /// Сообщение о последней ошибке
    std::string _strLastErrorMsg;

    /// Размеры основного фрейм-буфера (оконного/системного, в который осуществляется окончательный вывод)
    GLsizei _defaultScreenWidth = 0;
    GLsizei _defaultScreenHeight = 0;

    /// Разрешение экрана (кадрового буфера)
    GLsizei _screenWidth = 0;
    GLsizei _screenHeight = 0;

    /// Оснвной кадровый буфер экрана
    FrameBuffer* _screenFrameBuffer;

    /// Шейдерные программы для каждого этапа
    ShaderProgram* _shaderPrograms[3] = {};

    /// Ресурсы геометрии по умолчанию
    GeometryBuffer* _geometryQuad = nullptr;
}