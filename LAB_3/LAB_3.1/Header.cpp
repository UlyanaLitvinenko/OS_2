#include "pch.h"
#include "Header.h"

bool try_mark(std::vector<int>& arr, int id, int rand_val) {
    int idx = rand_val % arr.size();
    if (arr[idx] == 0) {
        arr[idx] = id;
        return true;
    }
    return false;
}

void clear_marks(std::vector<int>& arr, const std::vector<int>& marked) {
    for (int i : marked) arr[i] = 0;
}
