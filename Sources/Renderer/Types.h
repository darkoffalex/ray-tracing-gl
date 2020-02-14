/**
 * Типы использующиеся как для взаимодействия с библиотекой извне, так и внутри нее
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

namespace rtgl
{
    /// Д Е С К Р И П Т О Р Ы ( H A N D L E S)

    typedef void* HGeometry;
    typedef void* HMesh;

    /// П Е Р Е Ч И С Л Я Е М Ы Е

    /**
     * Оси координат
     * Используется при указании порядка во время поворотом на углы Эйлера
     */
    enum Axis { AXIS_X, AXIS_Y, AXIS_Z };

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
        const char* geometryPrepareVS = nullptr;
        const char* geometryPrepareGS = nullptr;
        const char* geometryPrepareFS = nullptr;

        // Этап трасировки геометрии
        const char* rayTracingVS = nullptr;
        const char* rayTracingFS = nullptr;

        // Этап пост-процессинга
        const char* postProcessVS = nullptr;
        const char* postProcessFS = nullptr;
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
        union { T x, r; };
        union { T y, g; };
        union { T z, b; };

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