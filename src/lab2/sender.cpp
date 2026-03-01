#include <windows.h>
#include <wchar.h>

int main() {
    // Получаем дескриптор консоли для вывода
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    const char* mappingName = "MySharedMemory";
    const char* eventReadyName = "DataReadyEvent";
    const char* eventReadName = "DataReadEvent";
    const int size = 256 * sizeof(wchar_t);  // память для широких символов

    // Создаём события синхронизации
    HANDLE hEventReady = CreateEventW(NULL, FALSE, FALSE, L"DataReadyEvent");
    HANDLE hEventRead  = CreateEventW(NULL, FALSE, FALSE, L"DataReadEvent");
    if (!hEventReady || !hEventRead) {
        WriteConsoleW(hConsole, L"Ошибка создания событий\n", 27, NULL, NULL);
        _getwch();
        return 1;
    }

    // Создаём объект разделяемой памяти
    HANDLE hMapFile = CreateFileMappingW(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        size,
        L"MySharedMemory");

    if (!hMapFile) {
        wchar_t buf[100];
        wsprintfW(buf, L"Ошибка CreateFileMapping: %lu\n", GetLastError());
        WriteConsoleW(hConsole, buf, wcslen(buf), NULL, NULL);
        CloseHandle(hEventReady);
        CloseHandle(hEventRead);
        _getwch();
        return 1;
    }

    // Отображаем память
    wchar_t* data = (wchar_t*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0, 0, size);

    if (!data) {
        wchar_t buf[100];
        wsprintfW(buf, L"Ошибка MapViewOfFile: %lu\n", GetLastError());
        WriteConsoleW(hConsole, buf, wcslen(buf), NULL, NULL);
        CloseHandle(hMapFile);
        CloseHandle(hEventReady);
        CloseHandle(hEventRead);
        _getwch();
        return 1;
    }

    // Записываем сообщение
    const wchar_t* message = L"Привет из первого процесса!";
    wcscpy_s(data, size / sizeof(wchar_t), message);
    WriteConsoleW(hConsole, L"Сообщение отправлено.\n", 22, NULL, NULL);

    // Сигнализируем receiver'у
    SetEvent(hEventReady);

    // Ждём подтверждения
    WaitForSingleObject(hEventRead, INFINITE);
    WriteConsoleW(hConsole, L"Получено подтверждение от receiver'а.\n", 40, NULL, NULL);

    // Очистка
    UnmapViewOfFile(data);
    CloseHandle(hMapFile);
    CloseHandle(hEventReady);
    CloseHandle(hEventRead);

    WriteConsoleW(hConsole, L"Нажмите любую клавишу для выхода...\n", 40, NULL, NULL);
    _getwch();
    return 0;
}