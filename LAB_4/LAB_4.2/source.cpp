#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <iostream>
#include <string>

using namespace boost::interprocess;

class BoostMessageSender {
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

    bool initialized;

public:
    BoostMessageSender()
        : segment(open_only, "MessageSystemMemory"),
        initialized(false) {

        try {
            message_buffer = segment.find<shared_circular_buffer>("MessageBuffer").first;
            buffer_mutex = segment.find<named_mutex>("BufferMutex").first;
            empty_slots = segment.find<named_semaphore>("EmptySlotsSemaphore").first;
            full_slots = segment.find<named_semaphore>("FullSlotsSemaphore").first;

            if (message_buffer && buffer_mutex && empty_slots && full_slots) {
                initialized = true;
                std::cout << "Подключено к системе сообщений" << std::endl;
            }
        }
        catch (const interprocess_exception& e) {
            std::cerr << "Ошибка подключения: " << e.what() << std::endl;
        }
    }

    bool is_initialized() const { return initialized; }

    void run_message_loop() {
        if (!initialized) {
            std::cout << "Не удалось подключиться к системе!" << std::endl;
            return;
        }

        std::cout << "=== ПРОЦЕСС-ОТПРАВИТЕЛЬ (Boost) ===" << std::endl;

        while (true) {
            show_options();
            int choice;
            std::cin >> choice;

            if (choice == 0) {
                std::cout << "Завершение работы отправителя..." << std::endl;
                break;
            }
            else if (choice == 1) {
                send_new_message();
            }
            else {
                std::cout << "Неверная команда!" << std::endl;
            }
        }
    }

private:
    void show_options() {
        std::cout << "\n--- Панель отправки ---" << std::endl;
        std::cout << "1 - Отправить новое сообщение" << std::endl;
        std::cout << "0 - Выйти из программы" << std::endl;
        std::cout << "Выберите действие: ";
    }

    void send_new_message() {
        if (!empty_slots->try_wait()) {
            std::cout << "Буфер заполнен! Нельзя отправить сообщение." << std::endl;
            return;
        }

        std::string user_message;
        std::cout << "Введите текст сообщения: ";
        std::cin.ignore();
        std::getline(std::cin, user_message);

        if (user_message.empty()) {
            empty_slots->post();
            std::cout << "Сообщение не может быть пустым!" << std::endl;
            return;
        }

        if (user_message.length() > 100) {
            user_message = user_message.substr(0, 100);
            std::cout << "Сообщение обрезано до 100 символов" << std::endl;
        }

        scoped_lock<named_mutex> lock(*buffer_mutex);

        CharAllocator char_alloc(segment.get_segment_manager());
        shared_string shared_msg(user_message.c_str(), char_alloc);

        message_buffer->push_back(shared_msg);

        std::cout << "Сообщение успешно отправлено!" << std::endl;

        full_slots->post();
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    BoostMessageSender sender;

    if (!sender.is_initialized()) {
        std::cerr << "Ошибка инициализации отправителя!" << std::endl;
        return 1;
    }

    sender.run_message_loop();

    return 0;
}