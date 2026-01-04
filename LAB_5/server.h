#pragma once
#include <windows.h>
#include <string>
#include <vector>

struct rabotnik {
    int id;
    char fio[10];
    double chasy;
};

enum komanda { CHITAT_NACHALO, CHITAT_KONETS, ZAPIS_ZABLOKIROVAT, ZAPIS_SOHRANIT, ZAPIS_RAZBLOKIROVAT, VYKHOD };
enum rezultat { OK, NE_NAYDENO, ZANYATO, OSHIBKA_PROTOKOLA };

struct zapros {
    komanda cmd;
    int id;
    rabotnik rec;
    DWORD pid;
};

struct otvet {
    rezultat res;
    rabotnik rec;
};

extern std::vector<rabotnik> dannye;
extern std::wstring fail;

int naytiIndex(int key);
bool sohranitZapic(int idx);
void obrabotat(const zapros& req, otvet& rep);
