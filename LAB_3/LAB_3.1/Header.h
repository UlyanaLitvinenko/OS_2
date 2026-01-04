#pragma once
#include "pch.h"
#include <vector>

bool try_mark(std::vector<int>& arr, int id, int rand_val);
void clear_marks(std::vector<int>& arr, const std::vector<int>& marked);
