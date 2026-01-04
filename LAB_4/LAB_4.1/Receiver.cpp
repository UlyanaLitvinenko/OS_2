#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define BUFFER_SIZE 21
#define HEADER_SIZE 8

class MessageManager {
private:
    std::string dataFile;
    int bufferSlots;
    int activeSenders;

public:
    MessageManager(const std::string& file, int slots, int senders)
        : dataFile(file), bufferSlots(slots), activeSenders(senders) {
    }

    bool initializeStorage() {
        std::fstream file(dataFile, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file) return false;

        int writePos = 0;
        int readPos = 0;
        file.write(reinterpret_cast<char*>(&writePos), sizeof(int));
        file.write(reinterpret_cast<char*>(&readPos), sizeof(int));

        char emptyData[BUFFER_SIZE] = { 0 };
        for (int i = 0; i < bufferSlots; i++) {
            file.write(emptyData, BUFFER_SIZE);
        }
        file.close();
        return true;
    }

    void processIncomingMessages() {
        while (true) {
            displayMenu();
            int userChoice;
            std::cin >> userChoice;

            if (userChoice == 0) {
                std::cout << "Завершение работы получателя..." << std::endl;
                break;
            }
            else if (userChoice == 1) {
                retrieveMessage();
            }
            else {
                std::cout << "Неверная команда!" << std::endl;
            }
        }
    }

private:
    void displayMenu() {
        std::cout << "\n--- Меню управления ---" << std::endl;
        std::cout << "1 - Получить сообщение из очереди" << std::endl;
        std::cout << "0 - Завершить работу" << std::endl;
        std::cout << "Выберите действие: ";
    }

    void retrieveMessage() {
        HANDLE fullSemaphore = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "FullBufferCount");
        HANDLE fileLock = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "FileLockObject");

        if (WaitForSingleObject(fullSemaphore, 0) == WAIT_TIMEOUT) {
            std::cout << "Очередь сообщений пуста!" << std::endl;
            CloseHandle(fullSemaphore);
            CloseHandle(fileLock);
            return;
        }

        WaitForSingleObject(fileLock, INFINITE);

        std::fstream file(dataFile, std::ios::binary | std::ios::in | std::ios::out);
        int currentRead, currentWrite;
        file.read(reinterpret_cast<char*>(&currentWrite), sizeof(int));
        file.read(reinterpret_cast<char*>(&currentRead), sizeof(int));

        int messageOffset = HEADER_SIZE + currentRead * BUFFER_SIZE;
        file.seekg(messageOffset);

        char messageBuffer[BUFFER_SIZE];
        file.read(messageBuffer, BUFFER_SIZE - 1);
        messageBuffer[BUFFER_SIZE - 1] = '\0';

        std::string receivedMessage(messageBuffer);

        file.seekp(messageOffset);
        char clearBuffer[BUFFER_SIZE] = { 0 };
        file.write(clearBuffer, BUFFER_SIZE);

        currentRead = (currentRead + 1) % bufferSlots;
        file.seekp(sizeof(int));
        file.write(reinterpret_cast<char*>(&currentRead), sizeof(int));
        file.close();

        ReleaseMutex(fileLock);

        HANDLE emptySemaphore = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "EmptyBufferCount");
        ReleaseSemaphore(emptySemaphore, 1, NULL);

        std::cout << "Получено сообщение: " << receivedMessage << std::endl;

        CloseHandle(fullSemaphore);
        CloseHandle(emptySemaphore);
        CloseHandle(fileLock);
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    std::string fileName;
    int queueSize, processCount;

    std::cout << "=== СИСТЕМА УПРАВЛЕНИЯ СООБЩЕНИЯМИ ===" << std::endl;
    std::cout << "Введите имя файла для обмена: ";
    std::cin >> fileName;
    std::cout << "Введите размер буфера: ";
    std::cin >> queueSize;
    std::cout << "Введите количество процессов-отправителей: ";
    std::cin >> processCount;

    MessageManager manager(fileName, queueSize, processCount);

    if (!manager.initializeStorage()) {
        std::cerr << "Ошибка инициализации файла!" << std::endl;
        return 1;
    }

    HANDLE emptySemaphore = CreateSemaphoreA(NULL, queueSize, queueSize, "EmptyBufferCount");
    HANDLE fullSemaphore = CreateSemaphoreA(NULL, 0, queueSize, "FullBufferCount");
    HANDLE fileLock = CreateMutexA(NULL, FALSE, "FileLockObject");
    HANDLE systemReady = CreateEventA(NULL, TRUE, FALSE, "SystemReadyEvent");

    for (int i = 0; i < processCount; i++) {
        std::string command = "Sender.exe " + fileName;

        STARTUPINFOA startupInfo;
        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);

        PROCESS_INFORMATION processInfo;
        ZeroMemory(&processInfo, sizeof(processInfo));

        if (CreateProcessA(NULL,
            const_cast<LPSTR>(command.c_str()),
            NULL, NULL, FALSE,
            CREATE_NEW_CONSOLE,
            NULL, NULL,
            &startupInfo,
            &processInfo)) {
            std::cout << "Запущен процесс-отправитель #" << (i + 1) << std::endl;
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        }
        else {
            std::cerr << "Ошибка запуска процесса!" << std::endl;
        }
    }

    SetEvent(systemReady);
    std::cout << "\nСистема готова к работе!" << std::endl;

    manager.processIncomingMessages();

    CloseHandle(emptySemaphore);
    CloseHandle(fullSemaphore);
    CloseHandle(fileLock);
    CloseHandle(systemReady);

    return 0;
}
