#include "Renderer.h"
#include "Globals.h"

#include <GL/glew.h>

namespace rtr
{
    /**
     * Получить последнее сообщение об ошибке
     * @return Строка с сообщением
     */
    const char *GetLastErrorMessage() {
        return _strLastErrorMsg;
    }

    /**
     * Инициализация библиотеки GLEW
     * @return Статус инициализации
     */
    bool __cdecl InitGlewLib() {
        glewExperimental = GL_TRUE;
        GLuint initStatus = glewInit();

        if(initStatus != GLEW_OK){
            _strLastErrorMsg = reinterpret_cast<const char*>(glewGetErrorString(initStatus));
            return false;
        }

        _bGlewInitialized = true;
        return true;
    }
}
