#include <windows.h>
#include <iostream>
#include <iomanip>
#include <string>

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

class EmployeeClient {
private:
    HANDLE hPipe;
    bool connected;

public:
    EmployeeClient() : hPipe(INVALID_HANDLE_VALUE), connected(false) {}

    bool connectToServer() {
        while (true) {
            hPipe = CreateFileA(
                PIPE_NAME,
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
            );

            if (hPipe != INVALID_HANDLE_VALUE) {
                DWORD mode = PIPE_READMODE_MESSAGE;
                SetNamedPipeHandleState(hPipe, &mode, NULL, NULL);
                connected = true;
                return true;
            }

            if (GetLastError() != ERROR_PIPE_BUSY) return false;
            if (!WaitNamedPipeA(PIPE_NAME, 5000)) return false;
        }
    }

    void run() {
        while (connected) {
            int choice;
            std::cout << "\n1 - Чтение\n2 - Запись\n3 - Выход\nВыберите: ";
            std::cin >> choice;

            if (choice == 1) readEmployee();
            else if (choice == 2) writeEmployee();
            else if (choice == 3) exitClient();
        }
    }

private:
    void readEmployee() {
        int id;
        std::cout << "ID: ";
        std::cin >> id;

        ClientRequest req{};
        req.operation = OP_READ;
        req.employeeId = id;

        DWORD bw;
        WriteFile(hPipe, &req, sizeof(req), &bw, NULL);

        employee emp{};
        DWORD br;
        ReadFile(hPipe, &emp, sizeof(emp), &br, NULL);

        if (emp.num == 0) {
            std::cout << "Нет такой записи\n";
        }
        else {
            std::cout << "\nID: " << emp.num
                << "\nИмя: " << emp.name
                << "\nЧасы: " << std::fixed << std::setprecision(2) << emp.hours << "\n";
        }

        std::cout << "Нажмите Enter...";
        std::cin.ignore();
        std::cin.get();

        ClientRequest endReq{};
        endReq.operation = OP_READ;
        endReq.employeeId = id;
        WriteFile(hPipe, &endReq, sizeof(endReq), &bw, NULL);
    }

    void writeEmployee() {
        int id;
        std::cout << "ID: ";
        std::cin >> id;

        ClientRequest req{};
        req.operation = OP_WRITE;
        req.employeeId = id;

        DWORD bw;
        WriteFile(hPipe, &req, sizeof(req), &bw, NULL);

        employee emp{};
        DWORD br;
        ReadFile(hPipe, &emp, sizeof(emp), &br, NULL);

        if (emp.num == 0) {
            std::cout << "Нет такой записи\n";
            return;
        }

        std::cout << "\nID: " << emp.num
            << "\nИмя: " << emp.name
            << "\nЧасы: " << emp.hours << "\n";

        std::cout << "\nНовое имя: ";
        std::cin >> emp.name;
        std::cout << "Новые часы: ";
        std::cin >> emp.hours;

        ClientRequest upd{};
        upd.operation = OP_WRITE;
        upd.employeeId = id;
        upd.empData = emp;

        WriteFile(hPipe, &upd, sizeof(upd), &bw, NULL);

        std::cout << "Нажмите Enter...";
        std::cin.ignore();
        std::cin.get();

        ClientRequest endReq{};
        endReq.operation = OP_WRITE;
        endReq.employeeId = id;
        WriteFile(hPipe, &endReq, sizeof(endReq), &bw, NULL);
    }

    void exitClient() {
        ClientRequest req{};
        req.operation = OP_EXIT;
        DWORD bw;
        WriteFile(hPipe, &req, sizeof(req), &bw, NULL);
        CloseHandle(hPipe);
        connected = false;
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    EmployeeClient c;
    if (c.connectToServer()) c.run();
    return 0;
}
