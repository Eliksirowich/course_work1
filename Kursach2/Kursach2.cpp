#include <windows.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <clocale>
#include <sstream>
#include <iomanip>
using namespace std;

const int INF = 1000000;  


void displayMatrix(const vector<vector<int>>& matrix, int V, HWND hwnd) {
    wstringstream ss;

  
    int maxWidth = 0;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            int value = matrix[i][j];
            if (value == INF) {
                value = 999999;  
            }
            maxWidth = max(maxWidth, (int)log10(value) + 1);
        }
    }

    
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (matrix[i][j] == INF) {
                ss << L"INF";  
            }
            else {
                ss << setw(maxWidth) << matrix[i][j]; 
            }
            ss << L" ";  
        }
        ss << L"\r\n";  
    }

    
    SetWindowText(hwnd, ss.str().c_str());
}


void floydWarshall(vector<vector<int>>& graph, int V, HWND hwndResult) {
    vector<vector<int>> dist = graph;  

 
    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (dist[i][k] != INF && dist[k][j] != INF && dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    displayMatrix(dist, V, hwndResult);
}


void generateRandomGraph(vector<vector<int>>& graph, int V, int minWeight, int maxWeight, HWND hwndEditM) {
    srand(time(0));
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) {
                graph[i][j] = 0;  
            }
            else {
                graph[i][j] = rand() % (maxWeight - minWeight + 1) + minWeight;
                if (rand() % 10 < 3) {  
                    graph[i][j] = INF;
                }
            }
        }
    }

   
    displayMatrix(graph, V, hwndEditM);
}


void inputGraphManually(vector<vector<int>>& graph, int V, HWND hwndEditM) {
    wchar_t buf[1000];
    GetWindowText(hwndEditM, buf, sizeof(buf) / sizeof(wchar_t));
    wstringstream ss(buf);
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            ss >> graph[i][j];
            if (i != j && graph[i][j] == 0) {
                graph[i][j] = INF; 
            }
        }
    }
}


void clearFields(HWND hwndEditV, HWND hwndEditM, HWND hwndResult, HWND hwndEditMin, HWND hwndEditMax) {
    SetWindowText(hwndEditV, L"");
    SetWindowText(hwndEditM, L"");
    SetWindowText(hwndResult, L"");
    SetWindowText(hwndEditMin, L"");
    SetWindowText(hwndEditMax, L"");
}


void showErrorMessage(const wchar_t* message) {
    MessageBox(NULL, message, L"Ошибка", MB_OK | MB_ICONERROR);
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndEditV, hwndEditM, hwndResult, hwndEditMin, hwndEditMax;
    static int V, minWeight, maxWeight; 
    static bool isRandomGeneration = true;
    vector<vector<int>> graph;

    switch (uMsg) {
    case WM_CREATE:
        CreateWindow(L"STATIC", L"Количество вершин:", WS_VISIBLE | WS_CHILD, 10, 10, 150, 20, hwnd, NULL, NULL, NULL);
        hwndEditV = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 160, 10, 50, 20, hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Матрица смежности:", WS_VISIBLE | WS_CHILD, 10, 40, 250, 20, hwnd, NULL, NULL, NULL);
        hwndEditM = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL, 10, 70, 300, 100, hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Минимальный вес:", WS_VISIBLE | WS_CHILD, 10, 180, 150, 20, hwnd, NULL, NULL, NULL);
        hwndEditMin = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 160, 180, 50, 20, hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Максимальный вес:", WS_VISIBLE | WS_CHILD, 10, 210, 150, 20, hwnd, NULL, NULL, NULL);
        hwndEditMax = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 160, 210, 50, 20, hwnd, NULL, NULL, NULL);

      
        CreateWindow(L"BUTTON", L"Очистить", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 120, 240, 100, 30, hwnd, (HMENU)2, NULL, NULL);
        CreateWindow(L"BUTTON", L"Выход", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 230, 240, 100, 30, hwnd, (HMENU)3, NULL, NULL);

        CreateWindow(L"STATIC", L"Результат:", WS_VISIBLE | WS_CHILD, 10, 270, 150, 20, hwnd, NULL, NULL, NULL);
        hwndResult = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_HSCROLL, 10, 300, 300, 100, hwnd, NULL, NULL, NULL);

        CreateWindow(L"BUTTON", L"Сгенерировать случайно", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 420, 150, 30, hwnd, (HMENU)4, NULL, NULL);
        CreateWindow(L"BUTTON", L"Ввести вручную", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 170, 420, 150, 30, hwnd, (HMENU)5, NULL, NULL);

        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) { 
            wchar_t buf[100];
            GetWindowText(hwndEditV, buf, sizeof(buf) / sizeof(wchar_t));
            V = _wtoi(buf);

            if (V <= 0) {
                showErrorMessage(L"Количество вершин не может быть отрицательным или нулевым.");
                return 0;
            }

            GetWindowText(hwndEditMin, buf, sizeof(buf) / sizeof(wchar_t));
            minWeight = _wtoi(buf);

            GetWindowText(hwndEditMax, buf, sizeof(buf) / sizeof(wchar_t));
            maxWeight = _wtoi(buf);

            if (minWeight >= maxWeight) {
                showErrorMessage(L"Минимальный вес должен быть меньше максимального.");
                return 0;
            }

            graph.resize(V, vector<int>(V));

          
            inputGraphManually(graph, V, hwndEditM);

            floydWarshall(graph, V, hwndResult);
        }
        if (LOWORD(wParam) == 2) { 
            clearFields(hwndEditV, hwndEditM, hwndResult, hwndEditMin, hwndEditMax);
        }
        if (LOWORD(wParam) == 3) { 
            PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        if (LOWORD(wParam) == 4) { 
            wchar_t buf[100];
            GetWindowText(hwndEditV, buf, sizeof(buf) / sizeof(wchar_t));
            V = _wtoi(buf);

            if (V <= 0) {
                showErrorMessage(L"Количество вершин не может быть отрицательным или нулевым.");
                return 0;
            }

            GetWindowText(hwndEditMin, buf, sizeof(buf) / sizeof(wchar_t));
            minWeight = _wtoi(buf);

            GetWindowText(hwndEditMax, buf, sizeof(buf) / sizeof(wchar_t));
            maxWeight = _wtoi(buf);

            if (minWeight >= maxWeight) {
                showErrorMessage(L"Минимальный вес должен быть меньше максимального.");
                return 0;
            }

            graph.resize(V, vector<int>(V));

          
            generateRandomGraph(graph, V, minWeight, maxWeight, hwndEditM);

            floydWarshall(graph, V, hwndResult);
        }
        if (LOWORD(wParam) == 5) {
            wchar_t buf[100];
            GetWindowText(hwndEditV, buf, sizeof(buf) / sizeof(wchar_t));
            V = _wtoi(buf);

            if (V <= 0) {
                showErrorMessage(L"Количество вершин не может быть отрицательным или нулевым.");
                return 0;
            }

            graph.resize(V, vector<int>(V));

            
            inputGraphManually(graph, V, hwndEditM);

            
            floydWarshall(graph, V, hwndResult);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main() {
   
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"GraphApp";

    if (!RegisterClass(&wc)) {
        return 1;
    }

    HWND hwnd = CreateWindowEx(0, L"GraphApp", L"Алгоритм Флойда", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 350, 500, NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        return 1;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

