#version 330 core

/*Схема входа-выхода*/

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 normal;

/*Uniform*/

uniform mat4 _model;                       // Матрица модели
uniform mat4 _view;                        // Матрица вида
uniform mat4 _projection;                  // Матрица проекции

/*Выход*/

out VS_OUT
{
    vec3 color;            // Цвет вершины
    vec3 position;         // Положение вершины в мировых координатах
    vec3 positionLocal;    // Положение вершины в локальных координатах
    vec3 normal;           // Вектор нормали вершины
    vec2 uv;               // Текстурные координаты
    mat3 normalMatrix;     // Матрица преобразования нормалей (не интерполируется, наверное)
} vs_out;

/*Функции*/

// Основная функция вершинного шейдера
// Преобразование координат (и прочих параметров) вершин и передача их следующим этапам
void main()
{
    // Координаты вершины после всех преобразований (мировое пространство, видовое, проекция)
    // Полученое значение это 4D вектор, для которого, на этапе растеризации, выполняется перспективное деление (xyz на w)
    gl_Position = _projection * _view * _model * vec4(position, 1.0);

    // Матрица преобразования нормалей
    // Учитывает только поворот, без искажения нормалей в процессе масштабирования
    vs_out.normalMatrix = mat3(transpose(inverse(_model)));

    // Цвет вершины (далее фрагмента) передается как есть
    vs_out.color = color;

    // Положение вершины (далее фрагмента) в мировых координатах
    vs_out.position = (_model * vec4(position, 1.0)).xyz;

    // Положение вершины (далее фрагмента) в локальном пространстве
    vs_out.positionLocal = position;

    // UV координаты текстуры для вершины (далее фрагмента)
    vs_out.uv = uv;

    // Нормаль вершины (далее фрагмента) - используем матрицу нормалей для корректной передачи
    vs_out.normal = normalize(vs_out.normalMatrix * normal);
}
