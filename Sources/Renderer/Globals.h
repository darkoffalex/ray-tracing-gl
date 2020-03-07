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
    /** Константны **/
    // Константы значения которых используются для обнуления буферов
    const unsigned INITIAL_ZERO = 0;
    const unsigned INITIAL_MAX_INT = INT_MAX;
    const unsigned INITIAL_MIN_INT = INT_MIN;
    // Максимальное кол-во треугольников, которые могут быть записаны в буфер на этапе подготовки геометрии
    const unsigned MAX_TRIANGLES_PREPARE = 10000;
    // Максимальное кол-во источников освещения (определяет размер UBO для структуры источника)
    const unsigned MAX_LIGHTS = 10;
    // Максимальное кол-во мешей
    const unsigned MAX_MESHES = 10;

    /** Состояние и инициализация **/

    // Готова ли библиотека к использованию
    bool _bInitialized = false;

    // Сообщение о последней ошибке
    std::string _strLastErrorMsg;

    /** Разрешение кадровых буферов **/

    // Размеры основного фрейм-буфера (оконного/системного, в который осуществляется окончательный вывод)
    GLsizei _defaultScreenWidth = 0;
    GLsizei _defaultScreenHeight = 0;

    // Разрешение экрана (кадрового буфера)
    GLsizei _screenWidth = 0;
    GLsizei _screenHeight = 0;

    /** Объекты сцены **/

    // Основной класс камеры
    Camera* _camera = nullptr;

    /** Основные ресурсы и ресурсы по умолчанию **/

    // Оснвной кадровый буфер экрана
    FrameBuffer* _screenFrameBuffer = nullptr;

    // Шейдерные программы для каждого этапа
    ShaderProgram* _shaderPrograms[3] = {};

    // Ресурсы геометрии по умолчанию
    GeometryBuffer* _geometryQuad = nullptr;

    /** Хендлы буферов **/

    // Буфер хранения (SSBO) для геометрии (для параллельной записи используется атомарный счетчик)
    GLuint _triangleBuffer = 0;
    GLuint _triangleCounterPerMeshBuffer = 0;
    GLuint _triangleCounterGlobalBuffer = 0;
    GLuint _meshBoundsMinBuffer = 0;
    GLuint _meshBoundsMaxBuffer = 0;

    // Буферы UBO для передачи информации об источниках света и прочих настрйоках
    GLuint _lightSourcesBuffer = 0;
    GLuint _commonSettingsBuffer = 0;

    /** Рендеринг **/

    // Идентификатор последнего этапа (прохода)
    RenderingStage _lastRenderingStage = RenderingStage::RS_NONE;

    // Кол-во источников света добавленных на сцену в данный момент
    GLuint _lightSourceCount = 0;

    // Кол-во мешей добавленных на сцену в данный момент
    GLuint _meshesCount = 0;

}