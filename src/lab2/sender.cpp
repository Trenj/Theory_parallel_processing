#include <windows.h>
#include <iostream>

int main() {
    const char* mappingName = "Global\\MySharedMemory";
    const int size = 256;

    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,   // без файла
        NULL,
        PAGE_READWRITE,
        0,
        size,
        mappingName);

    if (!hMapFile) {
        std::cout << "Ошибка CreateFileMapping\n";
        return 1;
    }

    char* data = (char*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0, 0, size);

    if (!data) {
        std::cout << "Ошибка MapViewOfFile\n";
        return 1;
    }

    const char* message = "Привет из первого процесса!";
    strcpy_s(data, size, message);

    std::cout << "Сообщение отправлено.\n";

    Sleep(5000); // чтобы второй процесс успел прочитать

    UnmapViewOfFile(data);
    CloseHandle(hMapFile);

    return 0;
}