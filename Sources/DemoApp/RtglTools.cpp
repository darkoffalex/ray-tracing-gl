#include "RtglTools.h"

#include <vector>
#include <cmath>
#include <iostream>

namespace rtgl
{
    const float PI = 3.14159265359f;

    /**
     * Генерация геометрии сферы
     * @param segments Кол-во сегментов
     * @param radius Радиус сферы
     * @return Дескриптор ресурса геометрии
     */
    HGeometryBuffer GenerateSphereGeometry(unsigned segments, float radius)
    {
        std::vector<Vertex<float>> vertices = {};
        std::vector<unsigned> indices = {};

        // Вершины
        for (unsigned int y = 0; y <= segments; ++y)
        {
            for (unsigned int x = 0; x <= segments; ++x)
            {
                // В какой части окружности (по 2 осям) мы находимся на конкретной итерации
                const float xSegment = static_cast<float>(x) / static_cast<float>(segments);
                const float ySegment = static_cast<float>(y) / static_cast<float>(segments);

                // Вычисляем положение вершины
                const float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                const float yPos = std::cos(ySegment * PI);
                const float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                // Добавляем вершину
                vertices.push_back({
                        {xPos * radius,yPos * radius,zPos * radius},
                        {1.0f,1.0f,1.0f},
                        {xSegment,ySegment},
                        {xPos,yPos,zPos}
                });
            }
        }

        // Индексы
        for (unsigned latNumber = 0; latNumber < segments; latNumber++) {
            for (unsigned int longNumber = 0; longNumber < segments; longNumber++) {
                const unsigned first = latNumber * (segments + 1) + longNumber;
                const unsigned second = first + segments + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        return CreateGeometryBuffer(vertices.data(),vertices.size(),indices.data(),indices.size());
    }

    /**
     * Генерация геометрии куба
     * @param size Размер стороны куба
     * @return Дескриптор ресурса геометрии
     */
    HGeometryBuffer GenerateCubeGeometry(float size)
    {
        // Вершины
        std::vector<Vertex<float>> vertices = {
                { { (size / 2),(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f }, {0.0f,0.0f,1.0f} },
                { { (size / 2),-(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f }, {0.0f,0.0f,1.0f} },
                { { -(size / 2),-(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f }, {0.0f,0.0f,1.0f} },
                { { -(size / 2),(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f }, {0.0f,0.0f,1.0f} },

                { { (size / 2),(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f }, {1.0f,0.0f,0.0f} },
                { { (size / 2),-(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f }, {1.0f,0.0f,0.0f} },
                { { (size / 2),-(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f }, {1.0f,0.0f,0.0f} },
                { { (size / 2),(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f }, {1.0f,0.0f,0.0f} },

                { { (size / 2),  (size / 2), -(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f },{0.0f,1.0f,0.0f} },
                { { (size / 2), (size / 2), (size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f },{0.0f,1.0f,0.0f} },
                { { -(size / 2), (size / 2), (size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f }, {0.0f,1.0f,0.0f} },
                { { -(size / 2),  (size / 2), -(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f }, {0.0f,1.0f,0.0f} },

                { { -(size / 2),(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f }, {0.0f,0.0f,-1.0f}  },
                { { -(size / 2),-(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f }, {0.0f,0.0f,-1.0f}  },
                { { (size / 2),-(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f }, {0.0f,0.0f,-1.0f}  },
                { { (size / 2),(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f }, {0.0f,0.0f,-1.0f}  },

                { { -(size / 2),(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f }, {-1.0f,0.0f,0.0f} },
                { { -(size / 2),-(size / 2),(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f }, {-1.0f,0.0f,0.0f} },
                { { -(size / 2),-(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f }, {-1.0f,0.0f,0.0f} },
                { { -(size / 2),(size / 2),-(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f }, {-1.0f,0.0f,0.0f} },

                { { -(size / 2),  -(size / 2), -(size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f }, {0.0f,-1.0f,0.0f} },
                { { -(size / 2), -(size / 2), (size / 2) },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f }, {0.0f,-1.0f,0.0f}  },
                { { (size / 2), -(size / 2), (size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f }, {0.0f,-1.0f,0.0f}  },
                { { (size / 2),  -(size / 2), -(size / 2) },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f }, {0.0f,-1.0f,0.0f}  },
        };

        // Индексы
        std::vector<unsigned> indices = {
                0,1,2, 0,2,3,
                4,5,6, 4,6,7,
                8,9,10, 8,10,11,
                12,13,14, 12,14,15,
                16,17,18, 16,18,19,
                20,21,22, 20,22,23,
        };

        return CreateGeometryBuffer(vertices.data(),vertices.size(),indices.data(),indices.size());
    }

    /**
     * Генерация геометрии квадрата
     * @param size Размер стороны квадрата
     * @return Дескриптор ресурса геометрии
     */
    HGeometryBuffer GenerateQuadGeometry(float size)
    {
        // Вершины
        std::vector<Vertex<float>> vertices = {
                { { (size / 2),(size / 2),0.0f },{ 1.0f,1.0f,1.0f },{ 1.0f,1.0f }, {0.0f,0.0f,1.0f} },
                { { (size / 2),-(size / 2),0.0f },{ 1.0f,1.0f,1.0f },{ 1.0f,0.0f }, {0.0f,0.0f,1.0f} },
                { { -(size / 2),-(size / 2),0.0f },{ 1.0f,1.0f,1.0f },{ 0.0f,0.0f }, {0.0f,0.0f,1.0f} },
                { { -(size / 2),(size / 2),0.0f },{ 1.0f,1.0f,1.0f },{ 0.0f,1.0f }, {0.0f,0.0f,1.0f} },
        };

        // Индексы
        std::vector<unsigned> indices = {
                0,1,2, 0,2,3,
        };

        return CreateGeometryBuffer(vertices.data(),vertices.size(),indices.data(),indices.size());
    }
}

