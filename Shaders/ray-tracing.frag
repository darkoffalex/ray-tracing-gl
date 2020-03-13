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
    float primaryCoff;
    float reflectToRefract;
    float refractionCoff;
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
    float refractionCoff;
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

struct AABBox
{
    vec3 min;
    vec3 max;
};

/*Uniform*/

uniform vec3 _camPosition;
uniform float _aspectRatio;
uniform float _fov;
uniform mat4 _view;
uniform mat4 _camModelMat;

/*SSBO-буферы*/

layout(std140, binding = 0) buffer triangleBuffer {
    Triangle _triangles[];
};

layout(binding = 1, offset = 0) uniform atomic_uint _triangleCounterPerMesh[MAX_MESHES];

layout(std140, binding = 5) buffer AABBoxMinBuffer {
    ivec3 _meshBoundsMin[MAX_MESHES];
};

layout(std140, binding = 6) buffer AABBoxMaxBuffer {
    ivec3 _meshBoundsMax[MAX_MESHES];
};

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

/*Глобальные переменные*/

// Набор лучей
Ray _rays[MAX_RAYS];
// Всего лучей
uint _totalRays = 0;

/*Функции*/

// Получить информацию о AABBox'е для конкретного меша
AABBox getAABBoxForMesh(uint meshIndex)
{
    // Значения min-max в целочисленном виде из буфера bounding box'ов
    ivec3 iMin = _meshBoundsMin[meshIndex];
    ivec3 iMax = _meshBoundsMax[meshIndex];

    AABBox box = AABBox(
        vec3(float(iMin.x),float(iMin.y),float(iMin.z)) / 1000.0f,
        vec3(float(iMax.x),float(iMax.y),float(iMax.z)) / 1000.0f
    );

    return box;
}

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

// Находится ли точка внутри axis-aligned bounding box
bool insideAAABBox(vec3 p, AABBox box)
{
    return
    p.x >= (box.min.x-0.001f) &&
    p.y >= (box.min.y-0.001f) &&
    p.z >= (box.min.z-0.001f) &&

    p.x <= (box.max.x+0.001f) &&
    p.y <= (box.max.y+0.001f) &&
    p.z <= (box.max.z+0.001f);
}

// Пересекается ли луч с axis-aligned bounding box
bool intersectsAABBox(Ray ray, AABBox box)
{
    // Засчитано ли пересечение
    bool intersection = false;

    // Проверяем не находится ли начало луча внутри коробки (если да - возвращаем true)
    if(insideAAABBox(ray.origin,box)){
        return true;
    }

    // Массив параметров t прямой луча для каждой плоскости параллелипипеда
    float params[3];

    // Выясняем какие первые 3 плоскости коробки попадутся лучу
    if(ray.origin.x <= box.min.x) params[0] = ray.direction.x != 0.0f ? ((ray.origin.x - box.min.x) / -ray.direction.x) : 0.0f;
    else params[0] = ray.direction.x != 0.0f ? ((box.max.x - ray.origin.x) / ray.direction.x) : 0.0f;

    if(ray.origin.y <= box.min.y) params[1] = ray.direction.y != 0.0f ? ((ray.origin.y - box.min.y) / -ray.direction.y) : 0.0f;
    else params[1] = ray.direction.y != 0.0f ? ((box.max.y - ray.origin.y) / ray.direction.y) : 0.0f;

    if(ray.origin.z <= box.min.z) params[2] = ray.direction.z != 0.0f ? ((box.min.z - ray.origin.z) / ray.direction.z) : 0.0f;
    else params[2] = ray.direction.z != 0.0f ? ((ray.origin.z - box.max.z) / -ray.direction.z) : 0.0f;

    // Последнее расстояние до точки пересечения
    float dist = 3.402823466e+38;

    // Вычисляем точки для каждого параметра и смотрим какие принадлежат bounding box'у
    for(uint i = 0; i < 3; i++)
    {
        float t = params[i];
        if(t > 0.0f){
            vec3 p = ray.origin + (ray.direction * t);

            if(insideAAABBox(p,box) && t < dist){
                dist = t;
                intersection = true;
            }
        }
    }

    return intersection;
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

    // Вектор в пространстве мира
    vec3 directionWorld = (_camModelMat * vec4(direction,0.0f)).xyz;

    // Вернуть нормализованный вектор
    return normalize(directionWorld);
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
vec3 castRay(Ray ray)
{
    // Засчитано ли пересечение треугольником
    bool intersceted = false;

    // Результирующий цвет каста данного луча
    vec3 resultColor = vec3(0.0f,0.0f,0.0f);

    // Минимальное расстояение до пересечения изначально "бесконечно" велико
    float minIntersectionDist = 3.402823466e+38;

    // Информация о ближайшем пересечении
    NearestIntersectionInfo nearestIntersection;

    // Сдвиг в общем буфере треугольников сцены
    uint triangleOffset = 0;

    // Проход по всем мешам сцены
    for(uint m = 0; m < _totalMeshes; m++)
    {
        // Количество треугольников текущего меша
        uint triangleCount = atomicCounter(_triangleCounterPerMesh[m]);

        // Получаем bounding box группы треугольников
        AABBox bounds = getAABBoxForMesh(m);

        // Если луч пересекает bounding box - перебираем треугольники группы
        if(intersectsAABBox(ray,bounds))
        {
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
                        nearestIntersection.primaryToSecondaryRatio = _triangles[i].primaryCoff;
                        nearestIntersection.reflectToRefractRatio = _triangles[i].reflectToRefract;
                        nearestIntersection.refractionCoff = _triangles[i].refractionCoff;
                        nearestIntersection.interpolated = interpolatedVertex(_triangles[i].vertices,barycentric);

                        // Считать засчитанным
                        intersceted = true;

                        minIntersectionDist = distance;
                    }
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

        // Нормаль в точке пересечения
        vec3 normal = normalize(nearestIntersection.interpolated.normal);

        // Сила базового цвета
        float baseColorStrength = nearestIntersection.primaryToSecondaryRatio;

        // Пройтись по источникам света
        for(uint i = 0; i < _totalLights; i++)
        {
            // Направление от точки пересечения к источнику
            vec3 toLight = normalize(_lightSources[i].position - nearestIntersection.position);

            // Отраженный вектор падения света на точку пересечения
            vec3 reflected = reflect(-toLight, normal);

            // Вычисление дифузной компоненты
            vec3 diffuse = nearestIntersection.albedo * max(dot(toLight,normal),0.0f);
            // Вычисление бликовой компоненты
            vec3 specular = vec3(1.0f,1.0f,1.0f) * pow(max(dot(-reflected, ray.direction),0.0f),32.0f);

            // Итоговый цвет
            finalyCalculatedColor = (diffuse + specular) * baseColorStrength;
        }

        // Дополнительные лучи (преломления и отражения)
        if(baseColorStrength < 1.0f)
        {
            // Сила второстепенного компонента (отраженный или преломленный)
            float secondaryColorRatio = 1.0f - baseColorStrength;
            // Сила отраженной компоненты
            float reflectionStrength = nearestIntersection.reflectToRefractRatio;
            // Сила преломленной компоненты
            float refractionStrength = 1.0f - reflectionStrength;

            // Если поверхность отражает
            if(reflectionStrength > 0.0f){
                // Положение чуть сдвигаем по нормали (чтобы луч не пересекся с поверзностью отражения)
                vec3 origin = nearestIntersection.position + (normal * 1e-3);
                // Направление нового луча (отраженный вектор текущего луча)
                vec3 reflectedDir = reflect(ray.direction,normal);
                // Добавляем луч
                _rays[_totalRays] = Ray(origin,reflectedDir,reflectionStrength*secondaryColorRatio);
                _totalRays++;
            }

            // Если поверхность преломляет
            if(refractionStrength > 0.0f){
                //TODO: добавить преломленный луч
            }
        }

        resultColor = finalyCalculatedColor * ray.weight;
    }

    return resultColor;
}

// Основная функция фрагментного шейдера
// Здесь осуществляется трассировка луча исходящего из конкретного фрагмента
void main()
{
    // Результирующий цвет
    vec3 resultColor = vec3(0.0f);


    // Начало луча в пространстве мира
    vec3 rayOriginWorld = (_camModelMat * vec4(0.0f,0.0f,0.0f,1.0f)).xyz;
    // Добавить старотвоый луч в набор
    _rays[0] = Ray(rayOriginWorld,rayDirection(_fov,_aspectRatio,fs_in.uv),1.0f);
    // Всего лучей на данный момент
    _totalRays = 1;

    // Проход по всем лучам
    for(uint i = 0; i < MAX_RAYS; i++)
    {
        if(i < _totalRays) {
            resultColor += castRay(_rays[i]);
        }
    }

    color = vec4(resultColor, 1.0f);
}