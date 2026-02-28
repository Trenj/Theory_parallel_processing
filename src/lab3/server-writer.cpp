#include <windows.h>
#include <iostream>

int main() {
    const char* pipeName = "\\\\.\\pipe\\MyPipe";

    HANDLE hPipe = CreateNamedPipe(
        pipeName,
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        512,
        512,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cout << "Ошибка CreateNamedPipe\n";
        return 1;
    }

    std::cout << "Ожидание подключения клиента...\n";
    ConnectNamedPipe(hPipe, NULL);

    for (int i = 0; i < 10; ++i) {
        const char* message = "Hello!";
        DWORD bytesWritten;

        WriteFile(
            hPipe,
            message,
            strlen(message) + 1,
            &bytesWritten,
            NULL);

        std::cout << "Отправлено: " << message << std::endl;

        Sleep(2000); // 2 секунды
    }

    CloseHandle(hPipe);
    return 0;
}