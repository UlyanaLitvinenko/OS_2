#pragma once
#include "pch.h"

#include <vector>
#include <thread>
#include <mutex>

constexpr int max_size = 1000;

extern std::vector<int> a;
extern int n;
extern int min_val;
extern int max_val;
extern double avg_val;

void find_min_max();
void find_avg();

void replace_min_max_with_avg(std::vector<int>& arr, int minv, int maxv, double avg);
int sum_array(const std::vector<int>& arr);
double average_array(const std::vector<int>& arr);
void reverse_array(std::vector<int>& arr);
