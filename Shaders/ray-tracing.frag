#version 430 core

// Максимальное кол-во лучей
#define MAX_RAYS 5
// Максимальное кол-во источников
#define MAX_LIGHTS 10
// Максимальное кол-во мешей
#define MAX_MESHES 10

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

struct NearestIntersectionInfo
{
    vec3 position;
    vec3 albedo;
    float metallic;
    float roughness;
    float primaryToSecondaryRatio;
    float reflectToRefractRatio;
    Vertex interpolated;
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
uniform mat4 _view;

/*SSBO-буферы*/

layout(std140, binding = 0) buffer triangleBuffer {
    Triangle _triangles[];
};

layout(binding = 1, offset = 0) uniform atomic_uint _triangleCounterPerMesh[MAX_MESHES];

/*Uniform-буферы*/

layout (std140, binding = 2) uniform lights
{
    LightSource _lightSources[MAX_LIGHTS];
};

layout (std140, binding = 3) uniform commonSettings
{
    uint _totalLights;
    uint _totalMeshes;
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
            intersectionPoint = ray.origin + (normalize(ray.direction) * t);

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

// Функция пересечения треугольника и луча
// Таинственный алгоритм Моллера - Трумбора, работаете быстрее обычного, но что тут происходит - лучше меня не спрашивайте
bool intersectsTriangleMT(Vertex[3] vertices, Ray ray, out vec3 intersectionPoint, out float distance, out vec2 barycentric)
{
    // Два ребра треугольника
    vec3 e1 = vertices[1].position - vertices[0].position;
    vec3 e2 = vertices[2].position - vertices[0].position;

    vec3 pvec = cross(ray.direction, e2);
    float det = dot(e1, pvec);

    if (det < 1e-8 && det > -1e-8) {
        return false;
    }

    float inv_det = 1 / det;
    vec3 tvec = ray.origin - vertices[0].position;
    float u = dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) {
        return false;
    }

    vec3 qvec = cross(tvec, e1);
    float v = dot(ray.direction, qvec) * inv_det;
    if (v < 0 || u + v > 1) {
        return false;
    }

    distance = dot(e2, qvec) * inv_det;
    if(distance < 0){
        return false;
    }

    intersectionPoint = ray.origin + (normalize(ray.direction) * distance);
    barycentric.x = v;
    barycentric.y = u;

    return true;
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

    // Информация о ближайшем пересечении
    NearestIntersectionInfo nearestIntersection;

    // Сдвиг в общем буфере треугольников сцены
    uint triangleOffset = 0;

    // Проход по всем мешам сцены
    for(uint m = 0; m < _totalMeshes; m++)
    {
        //TODO: проверка на bounding box меша

        // Количество треугольников текущего меша
        uint triangleCount = atomicCounter(_triangleCounterPerMesh[m]);

        // Проход по всем треугольникам меша
        for(uint i = triangleOffset; i < triangleOffset + triangleCount; i++)
        {
            // Точка пересечения в (в пространстве наблюдателя)
            vec3 intersectionPoint;
            // Дистанция до точки пересечения
            float distance;
            // Барицентрические координаты треугольника (для интреполяции)
            vec2 barycentric;

            // Если пересечение засчитано
            if(intersectsTriangleMT(_triangles[i].vertices,ray,intersectionPoint,distance,barycentric))
            {
                // Если расстояние до треугольника меньше расстояния до прежнего пересечния
                if(distance < minIntersectionDist)
                {
                    // Информация о пересечениии
                    nearestIntersection.position = intersectionPoint;
                    nearestIntersection.albedo = _triangles[i].albedo;
                    nearestIntersection.metallic = _triangles[i].metallic;
                    nearestIntersection.roughness = _triangles[i].roughness;
                    nearestIntersection.primaryToSecondaryRatio = 1.0f;
                    nearestIntersection.reflectToRefractRatio = 1.0f;
                    nearestIntersection.interpolated = interpolatedVertex(_triangles[i].vertices,barycentric);

                    // Считать засчитанным
                    intersceted = true;

                    minIntersectionDist = distance;
                }
            }
        }

        // Увеличение сдвига на кол-во треугольников меша
        triangleOffset += triangleCount;
    }

    // Если пересечени засчитано
    if(intersceted)
    {
        // Собственный цвет поверхности
        vec3 finalyCalculatedColor = vec3(0.0f);

        // Пройтись по источникам света
        for(uint i = 0; i < _totalLights; i++)
        {
            // Нормаль в точке пересечения
            vec3 normal = normalize(nearestIntersection.interpolated.normal);

            // Положение источника в пространстве вида
            vec3 lightPosView = (_view * vec4(_lightSources[i].position,1.0f)).xyz;

            // Направление от точки пересечения к источнику
            vec3 toLight = normalize(lightPosView - nearestIntersection.position);

            // Отраженный вектор падения света на точку пересечения
            vec3 reflected = reflect(-toLight, normal);

            // Вычисление дифузной компоненты
            vec3 diffuse = nearestIntersection.albedo * max(dot(toLight,normal),0.0f);
            // Вычисление бликовой компоненты
            vec3 specular = vec3(1.0f,1.0f,1.0f) * pow(max(dot(-reflected, ray.direction),0.0f),32.0f);

            // Итоговый цвет
            finalyCalculatedColor = diffuse + specular;
        }

        resultColor += (finalyCalculatedColor * ray.weight);
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
        if(i < totalRays) {
            castRay(rays[i], resultColor, rays, totalRays);
        }
    }

    color = vec4(resultColor, 1.0f);
}