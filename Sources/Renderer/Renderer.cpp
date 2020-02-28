/**
 * Основной файл библиотеки рендерера с поддержкой трассировки лучей
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#include "Renderer.h"
#include "Globals.h"

#include "Scene/Mesh.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.inl>
#include <stdexcept>
#include <cstring>

namespace rtgl
{
    /// О С Н О В Н О Е

    /**
     * Получить последнее сообщение об ошибке
     * @return Строка с сообщением
     */
    const char* __cdecl GetLastErrorMessage()
    {
        return _strLastErrorMsg.c_str();
    }

    /**
     * Инициализация
     * @param screenWidth Ширина области отрисовки
     * @param screenHeight Высота области отрисовки
     * @param shaderSourcesBundle Исходные коды шейдеров
     * @return Состояние инициализации
     */
    bool __cdecl Init(unsigned screenWidth, unsigned screenHeight, const ShaderSourcesBundle &shaderSourcesBundle)
    {
        // Убеждаемся что типы совместимы
        assert(sizeof(GLubyte) == sizeof(unsigned char));
        assert(sizeof(GLchar) == sizeof(char));
        assert(sizeof(GLfloat) == sizeof(float));
        assert(sizeof(GLuint) == sizeof(unsigned));

        // Изначально считаем что инициализация успешна
        _bInitialized = true;

        // Попытка инициализации
        try
        {
            /// Библиотека GLEW
            {
                glewExperimental = GL_TRUE;
                GLuint initStatus = glewInit();

                if(initStatus != GLEW_OK){
                    std::string message = "GLEW initialization error: ";
                    message.append(reinterpret_cast<const char*>(glewGetErrorString(initStatus)));
                    throw std::runtime_error(message);
                }
            }

            /// Компиляция и установка шейдеров
            {
                // Программа для стадии подготовки геометрии
                _shaderPrograms[RS_GEOMETRY_PREPARE] = new ShaderProgram({
                        {GL_VERTEX_SHADER,shaderSourcesBundle.geometryPrepareVs},
                        {GL_GEOMETRY_SHADER,shaderSourcesBundle.geometryPrepareGs},
                        {GL_FRAGMENT_SHADER,shaderSourcesBundle.geometryPrepareFs}
                });

                // Программа для стадии трассировки
                _shaderPrograms[RS_RAY_TRACING] = new ShaderProgram({
                        {GL_VERTEX_SHADER,shaderSourcesBundle.rayTracingVs},
                        {GL_FRAGMENT_SHADER,shaderSourcesBundle.rayTracingFs}
                });

                /*
                // Программа для стадии пост-процессинга
                _shaderPrograms[RS_POST_PROCESS] = new ShaderProgram({
                        {GL_VERTEX_SHADER,shaderSourcesBundle.postProcessVs},
                        {GL_FRAGMENT_SHADER,shaderSourcesBundle.postProcessFs}
                });
                */
            }

            /// Ресурсы по умолчанию - геометрия
            {
                // Квадрат
                _geometryQuad = new GeometryBuffer({
                        { { 1.0f,1.0f,0.0f },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f }, {0.0f,0.0f,1.0f} },
                        { { 1.0f,-1.0f,0.0f },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f }, {0.0f,0.0f,1.0f} },
                        { { -1.0f,-1.0f,0.0f },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f }, {0.0f,0.0f,1.0f} },
                        { { -1.0f,1.0f,0.0f },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f }, {0.0f,0.0f,1.0f} },
                }, { 0,1,2, 0,2,3 });

                assert(_geometryQuad != nullptr);
            }

            /// Ресурсы по умолчанию - текстуры
            {
                //TODO: инициализация текстур по умолчанию
            }

            /// Инициализация shader-storage-буферов
            {
                // Считаем что индексы привязок заданы в шейдере явно
                GLuint triangleBufferBinding = 0;
                GLuint triangleBufferCounterBinding = 1;

                // Создать SSBO для структур треугольников
                // Данные записываются в буфер треугольников на этапе подготовки геометрии (RS_GEOMETRY_PREPARE)
                // Записанные данные используются на этапе трассировки (RS_RAY_TRACING)
                glGenBuffers(1, &_triangleBuffer);
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, _triangleBuffer);
                // По предварительным подсчетам на треугольник нужно 224 байта (с учетом выравнивания std 140)
                glBufferData(GL_SHADER_STORAGE_BUFFER, 224 * MAX_TRIANGLES_PREPARE, nullptr, GL_DYNAMIC_DRAW);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, triangleBufferBinding, _triangleBuffer);
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

                // Создать атомарный счетчик элементов, который будет увеличиваться при записи элемента в буфер
                // Атомарные операции позволят корректно увеличивать число невзирая на параллелизм выполнения
                glGenBuffers(1, &_triangleCounterBuffer);
                glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _triangleCounterBuffer);
                glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
                // Обнулить счетчик
                GLuint zero = 0;
                glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
                // Активировать привязку и завершить работу с атомарным счетчиком
                glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, triangleBufferCounterBinding, _triangleCounterBuffer);
                glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
            }

            /// Uniform-буферы
            {
                // Считаем что индексы привязок заданы в шейдере явно
                GLuint meshesBufferBinding = 2;
                GLuint meshesBufferBindingCount = 3;

                // Создать UBO для структур мешей
                // При вызове каждой функции отрисовеи меша его треугольники записываются буфер треугольников
                // А буфер мешей пополняется на одну структуру, содержающую offset и count в буфере треугольников
                // Также структура меша может содержать информацию о bounding box'е меша
                glGenBuffers(1, &_meshesBuffer);
                glBindBuffer(GL_UNIFORM_BUFFER, _meshesBuffer);
                // По предварительным подсчетам на одну структуру надо 48 байт (с учетом выравнивания std 140)
                glBufferData(GL_UNIFORM_BUFFER, 48 * MAX_MESHES_PREPARE, nullptr, GL_DYNAMIC_DRAW);
                glBindBufferBase(GL_UNIFORM_BUFFER,meshesBufferBinding,_meshesBuffer);
                glBindBuffer(GL_UNIFORM_BUFFER, 0);

                // Создать UBO для текущего кол-ва мешей (для возможности итерации в шейдере)
                glGenBuffers(1,&_meshesCountBuffer);
                glBindBuffer(GL_UNIFORM_BUFFER, _meshesCountBuffer);
                glBufferData(GL_UNIFORM_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
                glBindBufferBase(GL_UNIFORM_BUFFER,meshesBufferBindingCount,_meshesCountBuffer);
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
            }

            /// Вспомогательные буферы
            {
                // Используется для промежуточного копирования данных о текущем кол-ве треугольников в буфере треугольников
                // Мапинг буфера атомарного счетчика напрямую, чтобы счиать значение - очень медленная операция
                // Быстрее будет скопировать во временный буфер, для последующего извлечения
                glGenBuffers(1, &_triangleBufferCounterTmp);
                glBindBuffer(GL_COPY_WRITE_BUFFER, _triangleBufferCounterTmp);
                glBufferData(GL_COPY_WRITE_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_COPY);
                glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            }

            /// Кадровые буферы
            {
                // Разрешение экрана
                _screenWidth = _defaultScreenWidth = static_cast<GLsizei>(screenWidth);
                _screenHeight = _defaultScreenHeight = static_cast<GLsizei>(screenHeight);

                // Кадровый буфер экрана - основной текстурный буфер в который осуществляется запись картинки
                // Буфер состоит только из цветового вложения, поскольку нам НЕ нужен буфер глубины (ray tracing)
                _screenFrameBuffer = new FrameBuffer(_screenWidth,_screenHeight);
                _screenFrameBuffer -> addTextureAttachment(GL_RGB32F,GL_RGB,GL_COLOR_ATTACHMENT0,false);
                if(!_screenFrameBuffer->prepareBuffer({GL_COLOR_ATTACHMENT0})){
                    throw std::runtime_error("Can't initialize screen frame buffer");
                }
            }

            /// Камера (установка камеры по умолчанию)
            {
                const auto aspectRatio = static_cast<GLfloat>(_screenWidth) / static_cast<GLfloat>(_screenHeight);
                _camera = new Camera({0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},aspectRatio);
            }

            /// Основные OpenGL настройки по умолчанию
            {
                // Передними считаются грани описанные по часовой стрелки
                glFrontFace(GL_CW);
                // Включить отсечение граней
                glEnable(GL_CULL_FACE);
                // Отсекать задние грани
                glCullFace(GL_BACK);
                // Тип рендеринга треугольников по умолчанию
                glPolygonMode(GL_FRONT, GL_FILL);
                // Функция тест глубины (Z-тест нужен лишь для отладки, для трассировки не используется)
                glDepthFunc(GL_LEQUAL);
                // Тест ножниц по умолчнию включен
                glEnable(GL_SCISSOR_TEST);
            }
        }
        catch(std::exception& ex)
        {
            _bInitialized = false;
            _strLastErrorMsg = ex.what();
        }

        return _bInitialized;
    }

    /**
     * Уничтожение ресурсов, освобождение памяти
     */
    void __cdecl DeInit()
    {
        // Уничтожение камеры
        delete _camera;

        // Уничтожение фрейм-буферов
        delete _screenFrameBuffer;

        // Уничтожение SSBO (Storage Buffer)
        GLuint ssbo[2] = {_triangleBuffer, _triangleCounterBuffer};
        glDeleteBuffers(2, ssbo);

        // Уничтожение геометрии по умолчанию
        delete _geometryQuad;

        // Уничтожение шейдерных программ
        delete _shaderPrograms[RS_GEOMETRY_PREPARE];
        delete _shaderPrograms[RS_RAY_TRACING];
        delete _shaderPrograms[RS_POST_PROCESS];
    }

    /// К А М Е Р А

    /**
     * Установка параметров камеры
     * @param position Положение
     * @param orientation Ориентация
     * @return Состояние операции
     */
    bool __cdecl SetCameraSettings(const Vec3<float> &position, const Vec3<float> &orientation)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");
            _camera->setPosition({position.x,position.y,position.z}, false);
            _camera->setOrientation({orientation.x,orientation.y,orientation.z});
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }

    /**
     * Установка положения камеры
     * @param position Положение
     * @return Состояние операции
     */
    bool __cdecl SetCameraPosition(const Vec3<float> &position)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");
            _camera->setPosition({position.x,position.y,position.z});
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }

    /**
     * Установка ориентации камеры
     * @param orientation Ориентация
     * @return Состояние операции
     */
    bool __cdecl SetCameraOrientation(const Vec3<float> &orientation)
    {
        try
        {
            if(!_bInitialized) throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");
            _camera->setOrientation({orientation.x,orientation.y,orientation.z});
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }

    /// Р Е Н Д Е Р И Н Г

    /**
     * Добавление меша в геометрический буфер (SSBO-буфер треугольников)
     * @param mesh Хендл меша
     * @return Состояние операции
     */
    bool __cdecl SetMesh(HMesh mesh)
    {
        try
        {
            // Указатель на меш
            auto pMesh = reinterpret_cast<Mesh*>(mesh);

            // Проверка на готовность к операции
            if(!_bInitialized)
                throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");
            if(_shaderPrograms[RS_GEOMETRY_PREPARE] == nullptr)
                throw std::runtime_error("No required shader set");
            if(pMesh == nullptr)
                throw std::runtime_error("No mesh provided");

            // Если пердыдущий проход был другим - установить необходимые параметры
            if(_lastRenderingStage != RS_GEOMETRY_PREPARE)
            {
                // Привязываемся ко фрейм-буфферу (временно используем основной)
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                // Использовать шейдер
                glUseProgram(_shaderPrograms[RS_GEOMETRY_PREPARE]->getId());

                // Сброс атомарного счетчика треугольников в SSBO
                _triangleCount = 0;
                _triangleCountPrevious = 0;
                glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _triangleCounterBuffer);
                glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &_triangleCount);
                glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

                // Сброс счетчика мешей
                _meshesCount = 0;
                glBindBuffer(GL_UNIFORM_BUFFER, _meshesCountBuffer);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLuint), &_meshesCount);
                glBindBuffer(GL_UNIFORM_BUFFER, 0);

                // Сменить идентификатор последного прохода
                _lastRenderingStage = RS_GEOMETRY_PREPARE;
            }

            // Передача матриц в шейдер
            glUniformMatrix4fv(
                    _shaderPrograms[RS_GEOMETRY_PREPARE]->getUniformLocations()->view,
                    1, GL_FALSE, glm::value_ptr(_camera->getViewMatrix()));

            glUniformMatrix4fv(
                    _shaderPrograms[RS_GEOMETRY_PREPARE]->getUniformLocations()->model,
                    1, GL_FALSE, glm::value_ptr(pMesh->getModelMatrix()));

            // Привязать геометрию и нарисовать ее
            glBindVertexArray(pMesh->geometry->getVaoId());
            glDrawElements(GL_TRIANGLES, pMesh->geometry->getIndexCount(), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);

            // Ожидаем завершения работы с вершинами
            glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

            // Получить доступ к кол-ву треугольников записанных в буфер на данный момент
            glCopyNamedBufferSubData(_triangleCounterBuffer,_triangleBufferCounterTmp,0,0,sizeof(GLuint));
            glBindBuffer(GL_COPY_WRITE_BUFFER, _triangleBufferCounterTmp);
            _triangleCount = *(reinterpret_cast<GLuint*>(glMapBufferRange(GL_COPY_WRITE_BUFFER, 0, sizeof(GLuint),GL_MAP_READ_BIT)));
            glUnmapBuffer(GL_COPY_WRITE_BUFFER);

            // Запись очередного меша со смещением в UBO
            GLuint offset = _meshesCount * 48;
            glBindBuffer(GL_UNIFORM_BUFFER, _meshesBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, offset, 4, &_triangleCountPrevious);
            glBufferSubData(GL_UNIFORM_BUFFER, offset + 4, 4, &_triangleCount);
            glBufferSubData(GL_UNIFORM_BUFFER, offset + 8, 16, nullptr);
            glBufferSubData(GL_UNIFORM_BUFFER, offset + 24, 16, nullptr);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            // Увеличиваем кол-во нарисованых мешей
            _meshesCount++;

            // Запись кол-во отрисованных мешей в UBO счетчика мешей
            glBindBuffer(GL_UNIFORM_BUFFER, _meshesCountBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLuint), &_meshesCount);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);


            // Кол-во треугольников предыдущего раза теперь равно кол-ву треугольников текущего раза
            _triangleCountPrevious = _triangleCountPrevious;
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }

    /**
     * Отрисовка всей сцены (проход трассировки лучей)
     * @return Состояние операции
     */
    bool __cdecl RenderScene()
    {
        try
        {
            // Проверка на готовность к операции
            if(!_bInitialized)
                throw std::runtime_error("Library isn't initialized. Please call rtgl::Init fist.");
            if(_shaderPrograms[RS_RAY_TRACING] == nullptr)
                throw std::runtime_error("No required shader set");

            // Если пердыдущий проход был другим - установить необходимые параметры
            if(_lastRenderingStage != RS_RAY_TRACING)
            {
                // Привязываемся ко фрейм-буфферу (временно используем основной)
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                // Использовать шейдер
                glUseProgram(_shaderPrograms[RS_RAY_TRACING]->getId());

                // Включить запись в цветовой буфер
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

                // Указать область кадра доступную для отрисовки
                glScissor(0, 0, _screenWidth, _screenHeight);
                glViewport(0, 0, _screenWidth, _screenHeight);

                // Сменить идентификатор последного прохода
                _lastRenderingStage = RS_RAY_TRACING;
            }

            // Очистка буфера
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Передать FOV
            glUniform1f(_shaderPrograms[RS_RAY_TRACING]->getUniformLocations()->fov,_camera->getFov());
            // Передать соотношение сторон
            glUniform1f(_shaderPrograms[RS_RAY_TRACING]->getUniformLocations()->aspectRatio,_camera->getAspectRatio());
            // Передать положение камеры
            glUniform3fv(_shaderPrograms[RS_RAY_TRACING]->getUniformLocations()->camPosition, 1, glm::value_ptr(_camera->getPosition()));

            // Привязать геометрию и нарисовать ее
            glBindVertexArray(_geometryQuad->getVaoId());
            glDrawElements(GL_TRIANGLES, _geometryQuad->getIndexCount(), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
        catch(std::exception& ex)
        {
            _strLastErrorMsg = ex.what();
            return false;
        }

        return true;
    }
}
