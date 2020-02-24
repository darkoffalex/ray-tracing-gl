#include "Tools.h"

#include <fstream>
#include <sstream>
#include <shlwapi.h>

namespace tools
{
    /**
     * Путь к рабочему каталогу
     * @return Строка содержащая путь к директории
     */
    std::string WorkingDir()
    {
        char path[MAX_PATH] = {};
        GetCurrentDirectoryA(MAX_PATH, path);
        PathAddBackslashA(path);
        return std::string(path);
    }

    /**
     * Путь к каталогу с исполняемым файлом (директория содержащая запущенный .exe)
     * @return Строка содержащая путь к директории
     */
    std::string ExeDir()
    {
        char path[MAX_PATH] = {};
        GetModuleFileNameA(nullptr, path, MAX_PATH);
        PathRemoveFileSpecA(path);
        PathAddBackslashA(path);
        return std::string(path);
    }

    /**
     * Абсолютный путь к папке с шейдерами
     * @return Строка содержащая путь к директории
     */
    std::string ShaderDir()
    {
        std::string exeDir = ExeDir();
        return exeDir.append("..\\Shaders\\");
    }

    /**
     * Загрузка текстового файла в строку
     * @param path Путь к текстовому файлу
     * @return Строка с содержимым
     */
    std::string LoadStringFromFile(const std::string &path)
    {
        // Чтение файла
        std::ifstream shaderFileStream;

        // Открыть файл для текстового чтения
        shaderFileStream.open(path);

        if (!shaderFileStream.fail())
        {
            // Строковой поток
            std::stringstream sourceStringStream;
            // Считать в строковой поток из файла
            sourceStringStream << shaderFileStream.rdbuf();
            // Закрыть файл
            shaderFileStream.close();
            // Получить данные из строкового поток в строку
            return sourceStringStream.str();
        }

        return "";
    }
}