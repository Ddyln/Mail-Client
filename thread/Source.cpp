#include <iostream>
#include <chrono>
#include <thread>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <vector>
using namespace std;

// Hàm để reload danh sách mail
void reloadMailList(vector <int>& a, int& cnt) {
    // Thực hiện logic reload ở đây
    std::cout << "Reloading mail list...\n";
    a.push_back(cnt++);
}

int main() {
    vector <int> a;
    int cnt = 0;

    std::thread mailThread([&a, &cnt]() {
        while (true) {
            reloadMailList(a, cnt);
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    });

    while (true) {
        if (_kbhit()) {
            char c = _getch();
            for (int& x : a)
                cout << x << ' ';
            cout << endl;
        }
    }

    mailThread.join();

    return 0;
}
