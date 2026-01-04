#include <iostream>
#include <fstream>
#include <cstring>
#include <locale>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    if (argc < 3) {
        cout << "Usage: Creator <binary_file> <record_count>\n";
        return 1;
    }

    const char* filename = argv[1];
    int count = atoi(argv[2]);

    ofstream out(filename, ios::binary);
    if (!out) {
        cout << "Failed to open file for writing.\n";
        return 1;
    }

    for (int i = 0; i < count; ++i) {
        employee emp;
        cout << "Enter employee #" << (i + 1) << endl;
        cout << "ID: ";
        cin >> emp.num;
        cout << "Name (max 9 chars): ";
        cin >> emp.name;
        cout << "Hours worked: ";
        cin >> emp.hours;

        out.write(reinterpret_cast<char*>(&emp), sizeof(emp));
    }

    out.close();
    cout << "Binary file created: " << filename << endl;
    return 0;
}
