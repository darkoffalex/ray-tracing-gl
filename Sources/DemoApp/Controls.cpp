#include "Controls.h"
#include "Camera.h"

#include <iostream>

// Положение мыши
static math::Vec2<int> _mousePositions;

// Вектор движения камеры
static math::Vec3<float> _camMovementRel = {0.0f, 0.0f, 0.0f};
static math::Vec3<float> _camMovementAbs = {0.0f, 0.0f, 0.0f};

// Камера (объявлено в Main.cpp)
extern Camera* _camera;

/**
 * Обработчик оконных сообщений
 * @param hWnd Дескриптор окна
 * @param message Сообщение
 * @param wParam Параметр сообщения
 * @param lParam Параметр сообщения
 * @return Код выполнения
 */
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool rotatingCamera = false;

    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_KEYDOWN:
            // При нажатии кнопок (WASD)
            switch (wParam)
            {
                case 0x57: // W
                    _camMovementRel.z = -1.0f;
                    break;
                case 0x41: // A
                    _camMovementRel.x = -1.0f;
                    break;
                case 0x53: // S
                    _camMovementRel.z = 1.0f;
                    break;
                case 0x44: // D
                    _camMovementRel.x = 1.0f;
                    break;
                case VK_SPACE:
                    _camMovementAbs.y = 1.0f;
                    break;
                case 0x43: // C
                    _camMovementAbs.y = -1.0f;
                    break;
                default:
                    break;
            }
            break;

        case WM_KEYUP:
            // При отжатии кнопок (WASD)
            switch (wParam)
            {
                case 0x57: // W
                    _camMovementRel.z = 0.0f;
                    break;
                case 0x41: // A
                    _camMovementRel.x = 0.0f;
                    break;
                case 0x53: // S
                    _camMovementRel.z = 0.0f;
                    break;
                case 0x44: // D
                    _camMovementRel.x = 0.0f;
                    break;
                case VK_SPACE:
                case 0x43:
                    _camMovementAbs.y = 0.0f;
                    break;
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
                default:
                    break;
            }
            break;

        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
            // При нажатии любой кнопки мыши
            _mousePositions.x = static_cast<int>(LOWORD(lParam));
            _mousePositions.y = static_cast<int>(HIWORD(lParam));
            break;

        case WM_MOUSEMOVE:
            // При движении мыши
            // Если зажата левая кнопка мыши
            if (wParam & MK_LBUTTON) {

                const int posX = static_cast<int>(LOWORD(lParam));
                const int posY = static_cast<int>(HIWORD(lParam));

                if(_camera != nullptr){
                    _camera->orientation.x += static_cast<float>(_mousePositions.y - posY) * 0.2f;
                    _camera->orientation.y += static_cast<float>(_mousePositions.x - posX) * 0.2f;
                }

                _mousePositions.x = posX;
                _mousePositions.y = posY;
            }
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    if(_camera != nullptr){
        _camera->setTranslation(math::Normalize(_camMovementRel) * 0.001f);
        _camera->setTranslationAbsolute(_camMovementAbs * 0.001f);
    }

    return 0;
}
