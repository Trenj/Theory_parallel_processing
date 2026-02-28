#include <windows.h>
#include <iostream>

int main() {
    const char* mappingName = "Global\\MySharedMemory";
    const int size = 256;

    HANDLE hMapFile;

    // Ждём пока mapping появится
    while (true) {
        hMapFile = OpenFileMapping(
            FILE_MAP_ALL_ACCESS,
            FALSE,
            mappingName);

        if (hMapFile) break;
        Sleep(500);
    }

    char* data = (char*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0, 0, size);

    if (!data) {
        std::cout << "Ошибка MapViewOfFile\n";
        return 1;
    }

    std::cout << "Получено сообщение: " << data << std::endl;

    UnmapViewOfFile(data);
    CloseHandle(hMapFile);

    return 0;
}