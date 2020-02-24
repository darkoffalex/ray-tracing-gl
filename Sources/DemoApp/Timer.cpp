#include "Timer.h"

/**
 * При создании таймера currentTick_ устанавливается в текущее время
 * @details Создавать таймер следует до цикла
 */
Timer::Timer():
        currentFrameTick_(std::chrono::high_resolution_clock::now()),
        lastFpsCounterUpdatedTime_(std::chrono::high_resolution_clock::now()),
        framesCount_(0),
        fps_(0),
        delta_(0.0f),
        fpsCounterReady_(false) {}

/**
 * Получить разницу во времени между текущим и предыдущим кадром
 * @return Значение разницы в миллисекундах
 */
float Timer::getDelta() const
{
    return delta_;
}

/**
 * Обновить таймер
 * @details Время предыдущего кадра - текущее время ПРЕДЫДУЩЕГО кадра, время текущего кадра - НАСТОЯЩЕЕ время
 */
void Timer::updateTimer()
{
    // Время предыдущего кадра это текущее время предыдущего кадра (до обновления таймера)
    previousFrameTick_ = currentFrameTick_;
    // Время текущего кадра это НАСТОЯЩЕЕ время
    currentFrameTick_ = std::chrono::high_resolution_clock::now();
    // Считать счетчик FPS не готовым к показу
    fpsCounterReady_ = false;

    // Сколько времени прошло с прошлого кадра
    const int64_t delta = std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTick_ - previousFrameTick_).count();
    delta_ = static_cast<float>(delta) / 1000.0f;

    // Подсчет FPS
    // Если прошла секунда с прошлого обновления счетчика FPS
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTick_ - lastFpsCounterUpdatedTime_).count() > 1000)
    {
        // Счетчик FPS равен кол-ву кадров набранных за секунду с прошлого обновления счетчика
        fps_ = framesCount_;
        // Обнулить кол-во кадров
        framesCount_ = 0;
        // Последнее обновление счетчика произошло сейчас
        lastFpsCounterUpdatedTime_ = currentFrameTick_;
        // FPS готов показу (пока таймер не обновлен)
        fpsCounterReady_ = true;
    }

    // Увеличить счетчик кадров
    framesCount_++;
}

/**
 * Получить FPS
 * @details Для корректного значения таймер должен обновляться в каждом кадре
 * @return Кол-во кадров за секунду с прошлого обновления счетчика
 */
unsigned Timer::getFps() const
{
    return fps_;
}

/**
 * \brief Готов ли счетчик FPS к показу
 * \details Если показ FPS занимает время, стоит показывать его только тогда когда счетчик обновлен
 * \return Да или нет
 */
bool Timer::isFpsCounterReady() const
{
    return fpsCounterReady_;
}
