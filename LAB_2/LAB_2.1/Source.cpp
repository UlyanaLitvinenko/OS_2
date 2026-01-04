#include "pch.h"
#include <iostream>
#include "Header.h"

using namespace std;

int a[max_size];
int n;
int min_val;
int max_val;
double avg_val;

DWORD WINAPI find_min_max(LPVOID param) {
	min_val = a[0];
	max_val = a[0];
	for (int i = 1; i < n; i++) {
		if (a[i] < min_val) min_val = a[i];
		if (a[i] > max_val) max_val = a[i];
		Sleep(7);
	}
	cout << "Минимум: " << min_val << endl;
	cout << "Максимум: " << max_val << endl;
	return 0;
}

DWORD WINAPI find_avg(LPVOID param) {
	long long sum = 0;
	for (int i = 0; i < n; i++) {
		sum += a[i];
		Sleep(12);
	}
	avg_val = static_cast<double>(sum) / n;
	cout << "Среднее: " << avg_val << endl;
	return 0;
}

void replace_min_max_with_avg(int* arr, int size, int minv, int maxv, double avg) {
	for (int i = 0; i < size; i++) {
		if (arr[i] == minv || arr[i] == maxv) {
			arr[i] = static_cast<int>(avg);
		}
	}
}

int sum_array(int* arr, int size) {
	int sum = 0;
	for (int i = 0; i < size; i++) sum += arr[i];
	return sum;
}

double average_array(int* arr, int size) {
	return static_cast<double>(sum_array(arr, size)) / size;
}

void reverse_array(int* arr, int size) {
	for (int i = 0; i < size / 2; i++) {
		swap(arr[i], arr[size - 1 - i]);
	}
}
