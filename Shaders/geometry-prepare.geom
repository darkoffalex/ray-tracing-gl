#version 430 core

// Максимальное кол-во хранимых треугольников
#define MAX_TRIANGLES_PREPARE 10000
// Максимальное кол-во мешей
#define MAX_MESHES 10

/*Схема входа-выхода*/

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

/*Вспомогательные типы*/

struct Vertex
{
    vec3 position;
    vec3 color;
    vec2 uv;
    vec3 normal;
};

struct Triangle
{
    Vertex[3] vertices;
    vec3 albedo;
    float metallic;
    float roughness;
};

/*Uniform*/

uniform vec3 _materialAlbedo;                      // Альбедо-цвет материала
uniform float _materialMetallic;                   // Металличность материала
uniform float _materialRoughness;                  // Шероховатость материала
uniform uint _meshIndex;                           // Индекс текущего меша

/*SSBO-буферы*/

layout(std140, binding = 0) buffer triangleBuffer {
    Triangle _triangles[];
};

layout(binding = 1, offset = 0) uniform atomic_uint _triangleCounterPerMesh[MAX_MESHES];
layout(binding = 4, offset = 0) uniform atomic_uint _triangleCounterGlobal;

/*Вход*/

in VS_OUT
{
    vec3 color;            // Цвет вершины
    vec3 position;         // Положение фрагмента в мировых координатах
    vec3 positionLocal;    // Положение фрагмента в локальных координатах
    vec3 normal;           // Вектор нормали фрагмента
    vec2 uv;               // Текстурные координаты
    mat3 normalMatrix;     // Матрица преобразования нормалей (не интерполируется, наверное)
} gs_in[];

/*Функции*/

// Основная функция геометрического шейдера
// Вычисление TBN матрицы для карт нормалей и предача остальных данных в следующий этап
void main()
{
    // Треугольник для записи в SSBO
    Triangle triangle;
    triangle.albedo = _materialAlbedo;
    triangle.metallic = _materialMetallic;
    triangle.roughness = _materialRoughness;

    // Пройтись по всем вершинам
    for(int i = 0; i < gl_in.length(); i++)
    {
        // Добавить вершины в треугольник
        triangle.vertices[i].position = gl_in[i].gl_Position.xyz;
        triangle.vertices[i].color = gs_in[i].color;
        triangle.vertices[i].normal = gs_in[i].normal;
        triangle.vertices[i].uv = gs_in[i].uv;
    }

    // Увеличить кол-во треугольников для конкретного меша
    atomicCounterIncrement(_triangleCounterPerMesh[_meshIndex]);
    // Увеличить общее кол-во треугольников
    uint triangleIndex = atomicCounterIncrement(_triangleCounterGlobal);
    // Записать информацию о треугольнике в SSBO
    if(triangleIndex <= MAX_TRIANGLES_PREPARE){
        _triangles[triangleIndex] = triangle;
    }
}