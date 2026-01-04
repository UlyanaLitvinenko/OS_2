#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <cstdlib>

using namespace std;

struct MarkerData {
    mutex mtx;
    condition_variable cv;
    atomic<bool> can_continue{ false };
    atomic<bool> should_terminate{ false };
    atomic<bool> waiting{ false };
    vector<int> marked;
};

vector<int> arr;
vector<thread> markers;
vector<unique_ptr<MarkerData>> marker_data;
atomic<int> waiting_count = 0;
atomic<bool> start_signal = false;

void marker(int id, int size, MarkerData* data) {
    srand(id);
    while (!start_signal) this_thread::sleep_for(chrono::milliseconds(1));

    while (true) {
        int idx = rand() % size;
        if (arr[idx] == 0) {
            this_thread::sleep_for(chrono::milliseconds(5));
            arr[idx] = id;
            data->marked.push_back(idx);
            this_thread::sleep_for(chrono::milliseconds(5));
        }
        else {
            cout << "Marker " << id << " blocked. Marked: " << data->marked.size()
                << ", blocked at index: " << idx << endl;
            data->waiting = true;
            waiting_count++;

            unique_lock<mutex> lock(data->mtx);
            data->cv.wait(lock, [&] { return data->can_continue || data->should_terminate; });

            if (data->should_terminate) {
                for (int i : data->marked) arr[i] = 0;
                return;
            }

            data->waiting = false;
            waiting_count--;
            data->can_continue = false;
        }
    }
}

int main() {
    int size, count;
    cout << "Array size: ";
    cin >> size;
    arr.assign(size, 0);

    cout << "Number of marker threads: ";
    cin >> count;

    marker_data.resize(count);
    for (int i = 0; i < count; ++i)
        marker_data[i] = make_unique<MarkerData>();

    for (int i = 0; i < count; ++i)
        markers.emplace_back(marker, i, size, marker_data[i].get());

    start_signal = true;

    while (true) {
        while (waiting_count < count) this_thread::sleep_for(chrono::milliseconds(10));

        cout << "\nArray state:\n";
        for (int x : arr) cout << x << " ";
        cout << "\n";

        int id;
        cout << "Enter marker ID to terminate: ";
        cin >> id;

        marker_data[id]->should_terminate = true;
        {
            lock_guard<mutex> lock(marker_data[id]->mtx);
            marker_data[id]->cv.notify_one();
        }

        markers[id].join();

        cout << "\nArray after termination:\n";
        for (int x : arr) cout << x << " ";
        cout << "\n";

        bool all_done = true;
        for (int i = 0; i < count; ++i)
            if (!marker_data[i]->should_terminate) all_done = false;

        if (all_done) break;

        for (int i = 0; i < count; ++i) {
            if (!marker_data[i]->should_terminate) {
                marker_data[i]->can_continue = true;
                lock_guard<mutex> lock(marker_data[i]->mtx);
                marker_data[i]->cv.notify_one();
            }
        }
    }

    cout << "Main thread finished.\n";
    return 0;
}

