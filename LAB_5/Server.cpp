#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

#define PIPE_NAME "\\\\.\\pipe\\EmployeePipe"

struct employee {
    int num;
    char name[10];
    double hours;
};

enum OperationType {
    OP_READ = 1,
    OP_WRITE = 2,
    OP_EXIT = 3
};

struct ClientRequest {
    OperationType operation;
    int employeeId;
    employee empData;
};

class EmployeeServer {
private:
    std::string filename;
    std::vector<employee> employees;
    HANDLE readSem;
    HANDLE writeSem;
    int readerCount;
    bool running;

public:
    EmployeeServer() : readSem(NULL), writeSem(NULL), readerCount(0), running(true) {}

    bool initialize() {
        std::cout << "Имя файла: ";
        std::cin >> filename;

        int n;
        std::cout << "Количество сотрудников: ";
        std::cin >> n;

        employees.resize(n);
        for (int i = 0; i < n; i++) {
            std::cout << "\nID: ";
            std::cin >> employees[i].num;
            std::cout << "Имя: ";
            std::cin >> employees[i].name;
            std::cout << "Часы: ";
            std::cin >> employees[i].hours;
        }

        save();

        readSem = CreateSemaphoreA(NULL, 1, 1, "ReadSem");
        writeSem = CreateSemaphoreA(NULL, 1, 1, "WriteSem");

        show();
        return true;
    }

    void run() {
        HANDLE pipe = CreateNamedPipeA(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,
            sizeof(ClientRequest),
            sizeof(employee),
            0,
            NULL
        );

        ConnectNamedPipe(pipe, NULL);
        handle(pipe);
        DisconnectNamedPipe(pipe);
        CloseHandle(pipe);

        show();
    }

private:
    void save() {
        std::ofstream f(filename, std::ios::binary);
        for (auto& e : employees) f.write((char*)&e, sizeof(employee));
    }

    void load() {
        employees.clear();
        std::ifstream f(filename, std::ios::binary);
        employee e;
        while (f.read((char*)&e, sizeof(employee))) employees.push_back(e);
    }

    void show() {
        std::cout << "\n=== ФАЙЛ ===\n";
        for (auto& e : employees) {
            std::cout << e.num << "  " << e.name << "  " << e.hours << "\n";
        }
    }

    employee* find(int id) {
        for (auto& e : employees) if (e.num == id) return &e;
        return nullptr;
    }

    void lockRead() {
        WaitForSingleObject(readSem, INFINITE);
        readerCount++;
        if (readerCount == 1) WaitForSingleObject(writeSem, INFINITE);
        ReleaseSemaphore(readSem, 1, NULL);
    }

    void unlockRead() {
        WaitForSingleObject(readSem, INFINITE);
        readerCount--;
        if (readerCount == 0) ReleaseSemaphore(writeSem, 1, NULL);
        ReleaseSemaphore(readSem, 1, NULL);
    }

    void lockWrite() {
        WaitForSingleObject(writeSem, INFINITE);
    }

    void unlockWrite() {
        ReleaseSemaphore(writeSem, 1, NULL);
    }

    void handle(HANDLE pipe) {
        ClientRequest req;
        DWORD br;

        while (running) {
            if (!ReadFile(pipe, &req, sizeof(req), &br, NULL)) break;

            if (req.operation == OP_EXIT) break;

            if (req.operation == OP_READ) {
                lockRead();
                employee* e = find(req.employeeId);
                employee out{};
                if (e) out = *e;

                DWORD bw;
                WriteFile(pipe, &out, sizeof(out), &bw, NULL);

                ReadFile(pipe, &req, sizeof(req), &br, NULL);
                unlockRead();
            }

            else if (req.operation == OP_WRITE) {
                lockWrite();
                employee* e = find(req.employeeId);
                employee out{};
                if (e) out = *e;

                DWORD bw;
                WriteFile(pipe, &out, sizeof(out), &bw, NULL);

                ReadFile(pipe, &req, sizeof(req), &br, NULL);
                if (e) *e = req.empData;
                save();

                ReadFile(pipe, &req, sizeof(req), &br, NULL);
                unlockWrite();
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    EmployeeServer s;
    if (s.initialize()) s.run();
    return 0;
}
