#pragma once

#include <string>

namespace rtr
{
    /// Сообщение о последней ошибке
    const char* _strLastErrorMsg;

    /// Готова ли библиотека GLEW
    bool _bGlewInitialized = false;
}