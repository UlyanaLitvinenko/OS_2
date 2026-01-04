#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace boost::interprocess;

class BoostMessageManager {
private:
    managed_shared_memory segment;

    typedef allocator<char, managed_shared_memory::segment_manager> CharAllocator;
    typedef basic_string<char, std::char_traits<char>, CharAllocator> shared_string;

    typedef allocator<shared_string, managed_shared_memory::segment_manager> StringAllocator;
    typedef circular_buffer<shared_string, StringAllocator> shared_circular_buffer;

    shared_circular_buffer* message_buffer;
    named_mutex* buffer_mutex;
    named_semaphore* empty_slots;
    named_semaphore* full_slots;

    int buffer_capacity;
    bool initialized;

public:
    BoostMessageManager(int bufferSize)
        : segment(open_or_create, "MessageSystemMemory",
            sizeof(shared_circular_buffer) + bufferSize * 100),
        buffer_capacity(bufferSize),
        initialized(false) {

        initialize_shared_components();
    }

private:
    void initialize_shared_components() {
        try {
            CharAllocator char_alloc(segment.get_segment_manager());
            StringAllocator string_alloc(segment.get_segment_manager());

            message_buffer = segment.find_or_construct<shared_circular_buffer>
                ("MessageBuffer")(buffer_capacity, string_alloc);

            buffer_mutex = segment.find_or_construct<named_mutex>
                ("BufferMutex")();

            empty_slots = segment.find_or_construct<named_semaphore>
                ("EmptySlotsSemaphore")(buffer_capacity);

            full_slots = segment.find_or_construct<named_semaphore>
                ("FullSlotsSemaphore")(0);

            initialized = true;
        }
        catch (const interprocess_exception& e) {
            std::cerr << "Ошибка инициализации: " << e.what() << std::endl;
            initialized = false;
        }
    }

public:
    bool is_initialized() const { return initialized; }

    void process_incoming_messages() {
        if (!initialized) {
            std::cout << "Система не инициализирована!" << std::endl;
            return;
        }

        std::cout << "\n=== СИСТЕМА УПРАВЛЕНИЯ СООБЩЕНИЯМИ (Boost) ===" << std::endl;
        std::cout << "Буфер: " << buffer_capacity << " слотов" << std::endl;

        while (true) {
            display_menu();
            int choice;
            std::cin >> choice;

            if (choice == 0) {
                std::cout << "Завершение работы..." << std::endl;
                break;
            }
            else if (choice == 1) {
                retrieve_message();
            }
            else {
                std::cout << "Неверная команда!" << std::endl;
            }
        }
    }

private:
    void display_menu() {
        std::cout << "\n--- Меню управления ---" << std::endl;
        std::cout << "1 - Получить сообщение" << std::endl;
        std::cout << "0 - Завершить работу" << std::endl;
        std::cout << "Выберите действие: ";
    }

    void retrieve_message() {
        if (!full_slots->try_wait()) {
            std::cout << "Очередь сообщений пуста!" << std::endl;
            return;
        }

        scoped_lock<named_mutex> lock(*buffer_mutex);

        if (!message_buffer->empty()) {
            shared_string msg = message_buffer->front();
            message_buffer->pop_front();

            std::cout << "Получено сообщение: " << msg.c_str() << std::endl;

            empty_slots->post();
        }
        else {
            full_slots->post();
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    int buffer_size;
    std::cout << "Введите размер буфера: ";
    std::cin >> buffer_size;

    BoostMessageManager manager(buffer_size);

    if (!manager.is_initialized()) {
        std::cerr << "Ошибка инициализации системы!" << std::endl;
        return 1;
    }

    std::cout << "\nСистема готова к работе!" << std::endl;
    std::cout << "Запущены процессы-отправители..." << std::endl;

    manager.process_incoming_messages();

    return 0;
}