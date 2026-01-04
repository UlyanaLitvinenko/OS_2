#include "pch.h"
#include "Header.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <algorithm>

int min_val;
int max_val;
double avg_val;

void find_min_max(std::vector<int>& arr) {
    if (arr.empty()) return;

    min_val = arr[0];
    max_val = arr[0];

    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] < min_val) min_val = arr[i];
        if (arr[i] > max_val) max_val = arr[i];
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
    }

    std::cout << "Минимум: " << min_val << std::endl;
    std::cout << "Максимум: " << max_val << std::endl;
}

void find_avg(std::vector<int>& arr) {
    if (arr.empty()) return;

    long long sum = 0;
    for (int val : arr) {
        sum += val;
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }

    avg_val = static_cast<double>(sum) / arr.size();
    std::cout << "Среднее: " << avg_val << std::endl;
}

void replace_min_max_with_avg(std::vector<int>& arr, int minv, int maxv, double avg) {
    for (int& val : arr) {
        if (val == minv || val == maxv) {
            val = static_cast<int>(avg);
        }
    }
}

int sum_array(const std::vector<int>& arr) {
    int sum = 0;
    for (int val : arr) sum += val;
    return sum;
}

double average_array(const std::vector<int>& arr) {
    if (arr.empty()) return 0.0;
    return static_cast<double>(sum_array(arr)) / arr.size();
}

void reverse_array(std::vector<int>& arr) {
    std::reverse(arr.begin(), arr.end());
}
