#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

const int BUFFER_SIZE = 10; // Размер буферы

const int ITEMS_PER_WRITER = 10;    // Кол-во чисел создаваемое писателями

// Общий буфер и синхронизация
std::queue<int> buffer;
std::mutex mtx;
std::condition_variable not_full;
std::condition_variable not_empty;

void writer(int id, int delay_ms) {
    for (int i = 0; i < ITEMS_PER_WRITER; ++i) {

        int value = id * 100 + i;

        std::unique_lock<std::mutex> lock(mtx);

        // Ждём если буфер заполнен
        not_full.wait(lock, [] {
            return buffer.size() < BUFFER_SIZE;
        });

        buffer.push(value);

        std::cout << "Писатель " << id
                  << " записал: " << value << std::endl;

        not_empty.notify_one();
        lock.unlock();

        std::this_thread::sleep_for(
            std::chrono::milliseconds(delay_ms));
    }
}

void reader(int total_items) {
    for (int i = 0; i < total_items; ++i) {

        std::unique_lock<std::mutex> lock(mtx);

        // Ждём если буфер пуст
        not_empty.wait(lock, [] {
            return !buffer.empty();
        });

        int value = buffer.front();
        buffer.pop();

        std::cout << "Читатель прочёл: "
                  << value << std::endl;

        not_full.notify_one();
        lock.unlock();

        std::this_thread::sleep_for(
            std::chrono::milliseconds(150));
    }
}

int main() {

    int total_items = ITEMS_PER_WRITER * 2;

    std::thread writer1(writer, 1, 50);   // быстрый 
    std::thread writer2(writer, 2, 200);  // медленный
    std::thread reader_thread(reader, total_items);

    writer1.join();
    writer2.join();
    reader_thread.join();

    std::cout << "Программа завершилась корректно." << std::endl;

    return 0;
}