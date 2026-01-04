#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

#define BUFFER_SIZE 21
#define HEADER_SIZE 8

class MessageSender {
private:
    std::string targetFile;

public:
    MessageSender(const std::string& file) : targetFile(file) {}

    void runMessageLoop() {
        while (true) {
            showOptions();
            int userChoice;
            std::cin >> userChoice;

            if (userChoice == 0) {
                std::cout << "Завершение работы отправителя..." << std::endl;
                break;
            }
            else if (userChoice == 1) {
                sendNewMessage();
            }
            else {
                std::cout << "Неверная команда!" << std::endl;
            }
        }
    }

private:
    void showOptions() {
        std::cout << "\n--- Панель отправки ---" << std::endl;
        std::cout << "1 - Отправить новое сообщение" << std::endl;
        std::cout << "0 - Выйти из программы" << std::endl;
        std::cout << "Выберите действие: ";
    }

    void sendNewMessage() {
        std::string userMessage;
        std::cout << "Введите текст сообщения: ";
        std::cin.ignore();
        std::getline(std::cin, userMessage);

        if (userMessage.empty()) {
            std::cout << "Сообщение не может быть пустым!" << std::endl;
            return;
        }

        if (userMessage.length() > BUFFER_SIZE - 1) {
            userMessage = userMessage.substr(0, BUFFER_SIZE - 1);
            std::cout << "Сообщение обрезано: " << userMessage << std::endl;
        }

        HANDLE emptySemaphore = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "EmptyBufferCount");
        HANDLE fileLock = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "FileLockObject");

        if (WaitForSingleObject(emptySemaphore, 0) == WAIT_TIMEOUT) {
            std::cout << "Буфер заполнен! Нельзя отправить сообщение." << std::endl;
            CloseHandle(emptySemaphore);
            CloseHandle(fileLock);
            return;
        }

        WaitForSingleObject(fileLock, INFINITE);

        std::fstream file(targetFile, std::ios::binary | std::ios::in | std::ios::out);
        int currentWrite, currentRead;
        file.read(reinterpret_cast<char*>(&currentWrite), sizeof(int));
        file.read(reinterpret_cast<char*>(&currentRead), sizeof(int));

        int messagePosition = HEADER_SIZE + currentWrite * BUFFER_SIZE;
        file.seekp(messagePosition);

        // Записываем строку как char* для корректной работы с русским текстом
        char messageData[BUFFER_SIZE] = { 0 };
        strncpy_s(messageData, BUFFER_SIZE, userMessage.c_str(), userMessage.length());
        file.write(messageData, BUFFER_SIZE);

        // Обновляем позицию записи
        int bufferCapacity = calculateBufferSize();
        currentWrite = (currentWrite + 1) % bufferCapacity;
        file.seekp(0);
        file.write(reinterpret_cast<char*>(&currentWrite), sizeof(int));
        file.close();

        ReleaseMutex(fileLock);

        HANDLE fullSemaphore = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "FullBufferCount");
        ReleaseSemaphore(fullSemaphore, 1, NULL);

        std::cout << "Сообщение успешно отправлено!" << std::endl;

        CloseHandle(emptySemaphore);
        CloseHandle(fullSemaphore);
        CloseHandle(fileLock);
    }

    int calculateBufferSize() {
        std::ifstream file(targetFile, std::ios::binary | std::ios::ate);
        std::streamsize fileSize = file.tellg();
        file.close();
        return static_cast<int>((fileSize - HEADER_SIZE) / BUFFER_SIZE);
    }
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc < 2) {
        std::cerr << "Ошибка: Не указан файл для работы!" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    std::cout << "=== ПРОЦЕСС-ОТПРАВИТЕЛЬ ===" << std::endl;
    std::cout << "Работа с файлом: " << fileName << std::endl;

    // Ожидаем готовности системы
    HANDLE systemReady = OpenEventA(EVENT_ALL_ACCESS, FALSE, "SystemReadyEvent");
    if (systemReady) {
        WaitForSingleObject(systemReady, INFINITE);
        CloseHandle(systemReady);
    }

    MessageSender sender(fileName);
    sender.runMessageLoop();

    return 0;
}