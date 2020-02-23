/**
 * Типы использующиеся как для взаимодействия с библиотекой извне, так и внутри нее
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include <cstdlib>

#ifdef RENDERER_LIB_EXPORTS
#define RENDERER_LIB_API __declspec(dllexport)
#else
#define RENDERER_LIB_API __declspec(dllimport)
#endif

namespace rtgl
{
    /// Д Е С К Р И П Т О Р Ы ( H A N D L E S)

    typedef void* HGeometryBuffer;
    typedef void* HSceneElement;
    typedef void* HMesh;

    /// П Е Р Е Ч И С Л Я Е М Ы Е

    /**
     * Оси координат
     * Используется при указании порядка во время поворотом на углы Эйлера
     */
    enum Axis { AXIS_X, AXIS_Y, AXIS_Z };

    /**
     * Тип проекции
     * Используется в классе камеры при построении матрицы проекции
     */
    enum ProjectionType { PROJ_PERSPECTIVE, PROJ_ORTHOGRAPHIC };

    /**
     * Этапы рендеринга сцены (проходы)
     * Рендеринг состоит из нескольких отдельных этапов, у каждого может быть своя шейдерная программа
     */
    enum RenderingStage { RS_GEOMETRY_PREPARE, RS_RAY_TRACING, RS_POST_PROCESS };

    /// С Т Р У К Т У Р Ы

    /**
     * Связка исходных кодов шейдеров
     * Объединяет все шейдеры одной шейдерной программы для конкретного этапа
     */
    struct ShaderSourcesBundle
    {
        // Этап подготовки геометрии
        const char* geometryPrepareVs = nullptr;
        const char* geometryPrepareGs = nullptr;
        const char* geometryPrepareFs = nullptr;

        // Этап трасировки геометрии
        const char* rayTracingVs = nullptr;
        const char* rayTracingFs = nullptr;

        // Этап пост-процессинга
        const char* postProcessVs = nullptr;
        const char* postProcessFs = nullptr;
    };

    /**
     * 2-мерный вектор или точка на плоскости
     * @tparam T Тип компонентов вектора
     */
    template <typename T = float>
    struct Vec2
    {
        T x, y;

        Vec2():x(0.0f),y(0.0f){};
        Vec2(const T& s1, const T& s2) :x(s1), y(s2) {}
    };

    /**
     * 3-мерный вектор или точка в пространстве
     * @tparam T Тип компонентов вектора
     */
    template <typename T = float>
    struct Vec3
    {
        union { public: T x, r; };
        union { public: T y, g; };
        union { public: T z, b; };

        Vec3():x(0.0f),y(0.0f),z(0.0f){};
        Vec3(const T& s1, const T& s2, const T& s3) :x(s1), y(s2), z(s3) {}
    };

    /**
     * Структура описывающая вершину геометрического меша передаваемую библиотеке
     * @tparam T Тип компонентов векторов
     * @warning Не менять порядок! Порядок полей должен соответствовать структуре определенной в GeometryBuffer
     */
    template <typename T = float>
    struct Vertex
    {
        Vec3<T> position;
        Vec3<T> color;
        Vec2<T> uv;
        Vec3<T> normal;
    };
}