/**
 * Основной файл библиотеки рендерера с поддержкой трассировки лучей
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#include "Renderer.h"
#include "Globals.h"

#include <GL/glew.h>
#include <stdexcept>

namespace rtgl
{
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
                        {GL_VERTEX_SHADER,shaderSourcesBundle.geometryPrepareVS},
                        {GL_GEOMETRY_SHADER,shaderSourcesBundle.geometryPrepareGS},
                        {GL_FRAGMENT_SHADER,shaderSourcesBundle.geometryPrepareFS}
                });

                // Программа для стадии трассировки
                _shaderPrograms[RS_RAY_TRACING] = new ShaderProgram({
                        {GL_VERTEX_SHADER,shaderSourcesBundle.rayTracingVS},
                        {GL_FRAGMENT_SHADER,shaderSourcesBundle.rayTracingFS}
                });

                // Программа для стадии пост-процессинга
                _shaderPrograms[RS_POST_PROCESS] = new ShaderProgram({
                        {GL_VERTEX_SHADER,shaderSourcesBundle.postProcessVS},
                        {GL_FRAGMENT_SHADER,shaderSourcesBundle.postProcessFS}
                });
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

            /// Камера (установка камеры по умолчанию)
            {
                //TODO: настройка камеры
            }

            /// Инициализация shader-storage-буферов
            {
                //TODO: инициализация storage-буферов
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
                if(_screenFrameBuffer->prepareBuffer({GL_COLOR_ATTACHMENT0})){
                    throw std::runtime_error("Can't initialize screen frame buffer");
                }
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
        // Уничтожение фрейм-буферов
        delete _screenFrameBuffer;

        // Уничтожение геометрии по умолчанию
        delete _geometryQuad;

        // Уничтожение шейдерных программ
        delete _shaderPrograms[RS_GEOMETRY_PREPARE];
        delete _shaderPrograms[RS_RAY_TRACING];
        delete _shaderPrograms[RS_POST_PROCESS];
    }
}
