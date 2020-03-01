#version 430 core

// Максимальное кол-во лучей
#define MAX_RAYS 5
// Максимальное кол-во источников
#define MAX_LIGHTS 10

/*Схема входа-выхода*/

layout (location = 0) out vec4 color;

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

struct Ray
{
    vec3 origin;
    vec3 direction;
    float weight;
};

struct LightSource
{
    vec3 position;
    float radius;
    vec3 color;
    vec3 orientation;
    float attenuationQuadratic;
    float attenuationLinear;
    float cutOffAngleCos;
    float cutOffOuterAngleCos;
    uint type;
};

/*Uniform*/

uniform vec3 _camPosition;
uniform float _aspectRatio;
uniform float _fov;

/*SSBO-буферы*/

layout(std140, binding = 0) buffer triangleBuffer {
    Triangle _triangles[];
};

layout(binding = 1, offset = 0) uniform atomic_uint _triangleCounter;

/*Uniform-буферы*/

layout (std140, binding = 2) uniform lights
{
    LightSource _lightSources[MAX_LIGHTS];
};

layout (std140, binding = 3) uniform commonSettings
{
    uint _totalLights;
};

/*Вход*/

in VS_OUT {
    vec2 uv;
} fs_in;

/*Функции*/

// Функция пересечения треугольника и луча
bool intersectsTriangle(Vertex[3] vertices, Ray ray, out vec3 intersectionPoint, out float distance, out vec2 barycentric)
{
    // Два ребра треугольника
    vec3 e1 = vertices[1].position - vertices[0].position;
    vec3 e2 = vertices[2].position - vertices[0].position;

    // Нормаль треугольника (обход вершин по часовой стрелке)
    vec3 normal = normalize(cross(e2,e1));

    // Скалярное произведение нормали и вектора направления луча
    float dotVal = dot(ray.direction,normal);

    // Если луч не параллелен плоскости
    if(dotVal != 0.0f)
    {
        // Вычисляем выраженный параметр t для параметрического уравнения прямой
        // Поскольку скалярное произведение нормали и луча (dot) совпадает со знаменателем выражения - вычислять знаменатель не нужно
        float t = (
            (normal.x * vertices[0].position.x - normal.x * ray.origin.x) +
            (normal.y * vertices[0].position.y - normal.y * ray.origin.y) +
            (normal.z * vertices[0].position.z - normal.z * ray.origin.z)
        ) / dotVal;

        // Значение t (параметр уравнения) по сути представляет из себя длину вектора от начала луча, до пересечения
        // Если t отрицательно - луч направлен в обратную сторону от плоскости и пересечение не засчитывается
        if(t > 0)
        {
            // Передаем t как расстояние
            distance = t;

            // Вычисляем положение точки пересечения
            intersectionPoint = ray.origin + (ray.direction * t);

            // Далее нужно определить находится ли точка нутри треугольника а также получить ее
            // барицентрические координаты для дальнейшего вычисления интерполированных значений
            // Для этого нужно использовать ребра треугольника как базисные вектора нового пространства
            // и оценить где в этом пространстве находится точка

            // Поскольку базсные вектора смещены на чало координат с точкой следует сделать то же самое
            vec3 pt = intersectionPoint - vertices[0].position;
            // Матрица 3*3 описывающая пространство треугольника
            mat3 triangleSpace = inverse(mat3(e2,e1,normal));
            // Получаем точку в пространстве треугольника
            vec3 result = triangleSpace * pt;

            // Если точка находится внутри треугольника
            if(result.x >= 0.0f && result.y >= 0.0f && result.x + result.y <= 1.0f)
            {
                barycentric.x = result.x;
                barycentric.y = result.y;
                return true;
            }
        }
    }

    return false;
}

// Получить вектор направления исходящий из конкретного фрагмента с учетом угла обзора и пропорций экрана
vec3 rayDirection(float fov, float aspectRatio, vec2 fragCoord)
{
    // Преобразовать текстурные координаты (0;1) в клип-координаты экрана (-1;1)
    vec2 fragClipCoords = (fragCoord * 2.0) - vec2(1.0);

    // Вектор направления с учетом угла обзора (fov) и пропорций экрана (aspectRatio)
    vec3 direction = vec3(
        fragClipCoords.x * tan(radians(fov) / 2.0) * aspectRatio,
        fragClipCoords.y * tan(radians(fov) / 2.0),
        -1.0);

    // Вернуть нормализованный вектор
    return normalize(direction);
}

// Получить интерполированные значения вершины
Vertex interpolatedVertex(Vertex[3] vertices, vec2 barycentric)
{
    // Результирующий объект с интерполированными значениями
    Vertex result;

    // Значения полей структуры по отдельности
    vec3 pos[3] = vec3[3](vertices[0].position,vertices[1].position,vertices[2].position);
    vec3 col[3] = vec3[3](vertices[0].color,vertices[1].color,vertices[2].color);
    vec2 uvs[3] = vec2[3](vertices[0].uv,vertices[1].uv,vertices[2].uv);
    vec3 nor[3] = vec3[3](vertices[0].normal,vertices[1].normal,vertices[2].normal);

    // Интерполяция с использованием барицентрических координат
    result.position = pos[0] + ((pos[1] - pos[0]) * barycentric.y) + ((pos[2] - pos[0]) * barycentric.x);
    result.color = col[0] + ((col[1] - col[0]) * barycentric.y) + ((col[2] - col[0]) * barycentric.x);
    result.uv = uvs[0] + ((uvs[1] - uvs[0]) * barycentric.y) + ((uvs[2] - uvs[0]) * barycentric.x);
    result.normal = nor[0] + ((nor[1] - nor[0]) * barycentric.y) + ((nor[2] - nor[0]) * barycentric.x);

    return result;
}

// Основная функция каста луча
bool castRay(Ray ray, out vec3 resultColor, out Ray rays[MAX_RAYS], out uint totalRays)
{
    // Засчитано ли пересечение треугольником
    bool intersceted = false;

    // Минимальное расстояение до пересечения изначально "бесконечно" велико
    float minIntersectionDist = 3.402823466e+38;

    // Количество треугольников на основании данных атомарного счетчика
    uint triangleCount = atomicCounter(_triangleCounter);

    // Пройтись по треугольникам
    for(int i = 0; i < triangleCount; i++)
    {
        // Точка пересечения в (в пространстве наблюдателя)
        vec3 intersectionPoint;
        // Дистанция до точки пересечения
        float distance;
        // Барицентрические координаты треугольника (для интреполяции)
        vec2 barycentric;

        // Если пересечение засчитано
        if(intersectsTriangle(_triangles[i].vertices,ray,intersectionPoint,distance,barycentric))
        {
            // Если расстояние до треугольника меньше расстояния до прежнего пересечния
            if(distance < minIntersectionDist)
            {
                // Вычисление итогового цвета
                Vertex interpolated = interpolatedVertex(_triangles[i].vertices,barycentric);
                vec3 finalyCalculatedColor = interpolated.color;

                // Учитваем "вес" луча и отдаем цвет
                resultColor += (finalyCalculatedColor * ray.weight);

                // Считать засчитанным
                intersceted = true;
                minIntersectionDist = distance;
            }
        }
    }

    return intersceted;
}

// Основная функция фрагментного шейдера
// Здесь осуществляется трассировка луча исходящего из конкретного фрагмента
void main()
{
    // Результирующий цвет
    vec3 resultColor = vec3(0.0f);

    // Изначальный набор лучей содержит только один луч начальный луч
    // В процессе каста в случае н еобходимости набор может пополнятся лучами
    Ray rays[MAX_RAYS];
    rays[0] = Ray(vec3(0.0f),rayDirection(_fov,_aspectRatio,fs_in.uv),1.0f);

    // Всего лучей на данный момент
    uint totalRays = 1;

    // Проход по всем лучам
    for(uint i = 0; i < MAX_RAYS; i++)
    {
        if(i < totalRays) castRay(rays[i], resultColor, rays, totalRays);
    }

    color = vec4(resultColor, 1.0f);
}