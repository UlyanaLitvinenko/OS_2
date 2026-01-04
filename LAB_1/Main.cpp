#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>
#include <locale>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

void printBinary(const string& filename) {
    ifstream in(filename, ios::binary);
    if (!in) {
        cout << "Cannot open binary file: " << filename << endl;
        return;
    }

    employee emp;
    while (in.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        cout << "ID: " << emp.num
            << ", Name: " << emp.name
            << ", Hours: " << fixed << setprecision(2) << emp.hours << endl;
    }

    in.close();
}

void printReport(const string& filename) {
    ifstream in(filename);
    if (!in) {
        cout << "Cannot open report file: " << filename << endl;
        return;
    }

    string line;
    while (getline(in, line)) {
        cout << line << endl;
    }

    in.close();
}

bool runProcess(const string& command) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(NULL, const_cast<char*>(command.c_str()),
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cout << "Failed to start process: " << command << endl;
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

int main() {
    setlocale(LC_ALL, "");

    string binFile, reportFile;
    int count;
    double rate;

    cout << "Enter binary file name: ";
    getline(cin, binFile);
    cout << "Enter number of records: ";
    cin >> count;
    cin.ignore();

    string creatorCmd = "Creator.exe \"" + binFile + "\" " + to_string(count);
    if (!runProcess(creatorCmd)) return 1;

    cout << "\nBinary file contents:\n";
    printBinary(binFile);

    cout << "\nEnter report file name: ";
    getline(cin, reportFile);
    cout << "Enter hourly rate: ";
    cin >> rate;

    string reporterCmd = "Reporter.exe \"" + binFile + "\" \"" + reportFile + "\" " + to_string(rate);
    if (!runProcess(reporterCmd)) return 1;

    cout << "\nReport contents:\n";
    printReport(reportFile);

    return 0;
}
