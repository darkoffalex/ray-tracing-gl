#pragma once

#include <chrono>

using namespace std::chrono;

class Timer
{
private:
    time_point<high_resolution_clock> currentFrameTick_;
    time_point<high_resolution_clock> previousFrameTick_;
    time_point<high_resolution_clock> lastFpsCounterUpdatedTime_;
    unsigned framesCount_;
    unsigned fps_;
    float delta_;
    bool fpsCounterReady_;

public:

    /**
     * При создании таймера currentTick_ устанавливается в текущее время
     * @details Создавать таймер следует до цикла
     */
    Timer();

    /**
     * Получить разницу во времени между текущим и предыдущим кадром
     * @return Значение разницы в миллисекундах
     */
    [[nodiscard]] float getDelta() const;

    /**
     * Обновить таймер
     * @details Время предыдущего кадра - текущее время ПРЕДЫДУЩЕГО кадра, время текущего кадра - НАСТОЯЩЕЕ время
     */
    void updateTimer();

    /**
     * Получить FPS
     * @details Для корректного значения таймер должен обновляться в каждом кадре
     * @return Кол-во кадров за секунду с прошлого обновления счетчика
     */
    [[nodiscard]] unsigned getFps() const;

    /**
     * Готов ли счетчик FPS к показу
     * @details Если показ FPS занимает время, стоит показывать его только тогда когда счетчик обновлен
     * @return Да или нет
     */
    [[nodiscard]] bool isFpsCounterReady() const;
};
