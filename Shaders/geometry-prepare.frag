#version 330 core

/*Схема входа-выхода*/

layout (location = 0) out vec4 color;        // Итоговый цвет фрагмента

/*Вход*/

in GS_OUT
{
    vec3 color;            // Цвет фрагмента
    vec3 position;         // Положение фрагмента в мировых координатах
    vec3 positionLocal;    // Положение фрагмента в локальных координатах
    vec3 normal;           // Вектор нормали фрагмента
    vec2 uv;               // Текстурные координаты фрагмента
    mat3 tbnMatrix;        // Матрица для преобразования из касательного пространства в мировое
} fs_in;

/*Функции*/

// Получить нормаль из карты нормалей
vec3 getNormalFromMap(sampler2D tex, vec2 uv)
{
    vec3 normal = texture(tex,uv).rgb * 2.0 - 1.0;
    return normalize(fs_in.tbnMatrix * normal);
}

// Основная функция фрагментного (пиксельного) шейдера
// Получение итогового цвета пикселя
void main()
{
    // Передаем цвет вершины как есть (после интерполяции)
    color = vec4(fs_in.color, 1.0f);
}