#include <iostream>
#include <fstream>
#include <iomanip>
#include <locale>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    if (argc < 4) {
        cout << "Usage: Reporter <binary_file> <report_file> <hourly_rate>\n";
        return 1;
    }

    const char* binFile = argv[1];
    const char* reportFile = argv[2];
    double rate = atof(argv[3]);

    ifstream in(binFile, ios::binary);
    ofstream out(reportFile); 

    if (!in || !out) {
        cout << "File error.\n";
        return 1;
    }

    out << "Report for file \"" << binFile << "\"\n";
    out << "ID\tName\tHours\tSalary\n";

    employee emp;
    while (in.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        double salary = emp.hours * rate;
        out << emp.num << "\t" << emp.name << "\t"
            << fixed << setprecision(2) << emp.hours << "\t"
            << fixed << setprecision(2) << salary << "\n";
    }

    in.close();
    out.close();

    cout << "Report saved to: " << reportFile << endl;
    return 0;
}
