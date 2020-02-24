#pragma once

#include <string>

namespace tools
{
    /**
     * Путь к рабочему каталогу
     * @return Строка содержащая путь к директории
     */
    std::string WorkingDir();

    /**
     * Путь к каталогу с исполняемым файлом (директория содержащая запущенный .exe)
     * @return Строка содержащая путь к директории
     */
    std::string ExeDir();

    /**
     * Абсолютный путь к папке с шейдерами
     * @return Строка содержащая путь к директории
     */
    std::string ShaderDir();

    /**
     * Загрузка текстового файла в строку
     * @param path Путь к текстовому файлу
     * @return Строка с содержимым
     */
    std::string LoadStringFromFile(const std::string& path);
}