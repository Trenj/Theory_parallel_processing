#include <windows.h>
#include <iostream>

int main() {
    const char* pipeName = "\\\\.\\pipe\\MyPipe";

    HANDLE hPipe;

    // Ждём пока сервер создаст канал
    while (true) {
        hPipe = CreateFile(
            pipeName,
            GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        Sleep(500);
    }

    char buffer[512];
    DWORD bytesRead;

    while (true) {
        BOOL success = ReadFile(
            hPipe,
            buffer,
            sizeof(buffer),
            &bytesRead,
            NULL);

        if (!success || bytesRead == 0)
            break;

        std::cout << "Получено: " << buffer << std::endl;
    }

    CloseHandle(hPipe);
    return 0;
}