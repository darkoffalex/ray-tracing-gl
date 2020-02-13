#pragma once

#ifdef RENDERER_LIB_EXPORTS
#define RENDERER_LIB_API __declspec(dllexport)
#else
#define RENDERER_LIB_API __declspec(dllimport)
#endif

namespace rtr
{
    extern "C"
    {
        /**
         * Получить последнее сообщение об ошибке
         * @return Строка с сообщением
         */
        RENDERER_LIB_API const char* __cdecl GetLastErrorMessage();

        /**
         * Инициализация библиотеки GLEW
         * @return Статус инициализации
         */
        RENDERER_LIB_API bool __cdecl InitGlewLib();
    }
}

