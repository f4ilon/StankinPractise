#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>


class ThreadPool {
private:
    std::vector<std::thread> workers;         // Массив рабочих потоков
    std::queue<std::function<void()>> tasks;  // Очередь задач

    std::mutex queue_mutex;                   // Мьютекс для защиты очереди
    std::condition_variable condition;        // Условная переменная (звоночек)
    bool stop;                                // Флаг остановки пула

public:
    // Конструктор: запускаем N потоков
    ThreadPool(size_t threads) : stop(false) {
        for(size_t i = 0; i < threads; ++i) {
            // Каждый поток крутится в бесконечном цикле
            workers.emplace_back([this] {
                while(true) {
                    std::function<void()> task;

                    {
                        // Блокируем мьютекс перед доступом к очереди
                        std::unique_lock<std::mutex> lock(this->queue_mutex);

                        // Поток ЗАСЫПАЕТ и не жрет CPU, пока очередь пуста и мы не останавливаем пул
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });

                        // Если пул остановлен и задач больше нет — завершаем поток
                        if(this->stop && this->tasks.empty())
                            return;

                        // Берем задачу из очереди
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    } // Мьютекс автоматически освобождается здесь

                    // Выполняем задачу (уже без мьютекса, параллельно с другими)
                    task();
                }
            });
        }
    }

    // Метод добавления новой задачи в пул
    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.push(task); // Кладем задачу в очередь
        }
        condition.notify_one(); // "Звеним в звоночек", будим один из спящих потоков
    }

    // Деструктор: корректно завершаем все потоки
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all(); // Будим ВСЕ потоки, чтобы они увидели flag stop=true

        for(std::thread &worker : workers) {
            worker.join(); // Ждем завершения каждого (правило join!)
        }
    }
};