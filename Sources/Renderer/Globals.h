/**
 * Файл с глобальными переменными, определяемыми в момент инициализации библиотеки
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include <string>
#include <GL/glew.h>

#include "Resources/FrameBuffer.h"
#include "Resources/GeometryBuffer.h"
#include "Resources/ShaderProgram.h"
#include "Scene/Camera.h"

namespace rtgl
{
    /// Максимальное кол-во треугольников которые могут быть записаны в буфер на этапе подготовки геометрии
    const unsigned MAX_TRIANGLES_PREPARE = 1000;


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

    /// Основной класс камеры
    Camera* _camera = nullptr;

    /// Оснвной кадровый буфер экрана
    FrameBuffer* _screenFrameBuffer = nullptr;

    /// Шейдерные программы для каждого этапа
    ShaderProgram* _shaderPrograms[3] = {};

    /// Ресурсы геометрии по умолчанию
    GeometryBuffer* _geometryQuad = nullptr;

    /// Буфер хранения (SSBO) для геометрии (для параллельной записи используется атомарный счетчик)
    GLuint _triangleBuffer = 0;
    GLuint _triangleBufferCounter = 0;

    /// Идентификатор последнего этапа (прохода)
    RenderingStage _lastRenderingStage = RenderingStage::RS_NONE;

}