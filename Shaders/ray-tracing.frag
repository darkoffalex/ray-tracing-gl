#version 330 core

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

/*Uniform*/

uniform vec3 _camPosition;
uniform float _aspectRatio;
uniform float _fov;

/*Вход*/

in VS_OUT {
    vec2 uv;
} fs_in;

/*Константы*/

// Отладочный буфер из двух треугольников
const Triangle triangles[2] = Triangle[2](
    Triangle(Vertex[3](
        Vertex(vec3(1.0f,1.0f,-5.0f),vec3(1.0f,0.0f,0.0f),vec2(1.0f,1.0f),vec3(0.0f,0.0f,1.0f)),
        Vertex(vec3(1.0f,-1.0f,-5.0f),vec3(0.0f,1.0f,0.0f),vec2(1.0f,0.0f),vec3(0.0f,0.0f,1.0f)),
        Vertex(vec3(-1.0f,-1.0f,-5.0f),vec3(0.0f,0.0f,1.0f),vec2(0.0f,0.0f),vec3(0.0f,0.0f,1.0f))
    ),
    vec3(1.0f),0.0f,1.0f),
    Triangle(Vertex[3](
        Vertex(vec3(1.0f,1.0f,-5.0f),vec3(1.0f,0.0f,0.0f),vec2(1.0f,1.0f),vec3(0.0f,0.0f,1.0f)),
        Vertex(vec3(-1.0f,-1.0f,-5.0f),vec3(0.0f,0.0f,1.0f),vec2(0.0f,0.0f),vec3(0.0f,0.0f,1.0f)),
        Vertex(vec3(-1.0f,1.0f,-5.0f),vec3(1.0f,1.0f,0.0f),vec2(0.0f,1.0f),vec3(0.0f,0.0f,1.0f))
    ),vec3(1.0f),0.0f,1.0f)
);

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

// Основная функция фрагментного шейдера
// Здесь осуществляется трассировка луча исходящего из конкретного фрагмента
void main()
{
    // Луч исходящий из текущего фрагмента
    Ray ray = Ray(vec3(0.0f),rayDirection(_fov,_aspectRatio,fs_in.uv),1.0f);

    // Результирующий цвет
    vec3 resultColor = vec3(0.0f);

    // Пройтись по треугольникам
    for(int i = 0; i < 2; i++)
    {
        // Точка пересечения в (в пространстве наблюдателя)
        vec3 intersectionPoint;
        // Дистанция до точки пересечения
        float distance;
        // Барицентрические координаты треугольника (для интреполяции)
        vec2 barycentric;

        // Если пересечение засчитано
        if(intersectsTriangle(triangles[i].vertices,ray,intersectionPoint,distance,barycentric))
        {
            resultColor = vec3(1.0f,0.0f,0.0f);
        }
    }

    color = vec4(resultColor, 1.0f);
}