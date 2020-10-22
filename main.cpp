#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <thread>
const DWORD dwLocalPlayer = 0xCBD6A4;
const DWORD dwEntityList = 0x4CCDCBC;
const DWORD m_iTeamNum = 0xF4;
const DWORD m_iGlowIndex = 0xA3F8;
const DWORD dwGlowObjectManager = 0x520DAE0;
HANDLE process; // непосредственно сам процесс CSGO
DWORD clientBase; // этo для работы с client_panorama.dll
DWORD engineBase; // этo для работы с engine.dll
bool wh = false;
DWORD getModuleBaseAddress(DWORD pid, const char* name)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    MODULEENTRY32 mEntry;
    mEntry.dwSize = sizeof(MODULEENTRY32);
    do
    {
        if (!strcmp(mEntry.szModule, name))
        {
            CloseHandle(snapshot);
            return (DWORD)mEntry.modBaseAddr;
        }
    } while (Module32Next(snapshot, &mEntry));
}

template <typename T>
T readMem(DWORD address)
{
    T buffer;
    ReadProcessMemory(process, (LPVOID)address, &buffer, sizeof(buffer), 0);
    return buffer;
}

template <typename T>
void writeMem(DWORD address, T value)
{
    WriteProcessMemory(process, (LPVOID)address, &value, sizeof(value), 0);
}
int main() {

    SetConsoleTitle("Top non-pasted shit 1337"); // устанавливаем заголовок нашей консоли

    std::cout << "Open CS:GO\n"; // выводим в консоль сообщение о том, что надо открыть ксго

    HWND hwnd;

    do {
        hwnd = FindWindowA(0, "Counter-Strike: Global Offensive"); // ищем ксго, если находим - выходим из цикла
        Sleep(50); // таймаут (чтобы не грузить процессор)
    } while (!hwnd);

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid); // получаем id приложения
    process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid); // заходим в кс го его id

    std::cout << "Csgo started, pid " << pid << ".\n"; // выводим сообщение о том, что ксго запущена

    do {
        clientBase = getModuleBaseAddress(pid, "client_panorama.dll"); // ищем клиент кс го
        Sleep(50);
    } while (!clientBase);

    do {
        engineBase = getModuleBaseAddress(pid, "engine.dll"); // ищем движок кс го
        Sleep(50);
    } while (!engineBase);

}
void wallhack()
{
    while (true) // создаем бесконечный цикл
    {
        Sleep(10); // таймаут 10 мс, чтобы не грузить процессор под 100
        if (!wallhack && !readMem<DWORD>(readMem<DWORD>(clientBase + dwLocalPlayer) + 0xED)) // если вх выключено или не удается прочитать память - выходим из цикла
            continue;

        DWORD glowObj = readMem<DWORD>(clientBase + dwGlowObjectManager); // создаем объект glowObj из модельки игрока
        DWORD myTeam = readMem<DWORD>(readMem<DWORD>(clientBase + dwLocalPlayer) + m_iTeamNum); // создаем объект тиммейтов

        for (int x = 0; x < 32; x++) // сам вх
        {
            DWORD player = readMem<DWORD>(clientBase + dwEntityList + x * 0x10); // обычный игрок
            if (player == 0)
                continue;

            bool dormant = readMem<bool>(player + 0xED); // спектатор
            if (dormant)
                continue;

            DWORD team = readMem<DWORD>(player + m_iTeamNum); // тиммейт
            if (team != 2 && team != 3)
                continue;

            DWORD currentGlowIndex = readMem<DWORD>(player + m_iGlowIndex); // текущий индекс игрока

            if (team != myTeam) // если игрок не тиммейт
            {
                // делаем его обводку красным
                writeMem<float>(glowObj + currentGlowIndex * 0x38 + 0x4, 255); // red
                writeMem<float>(glowObj + currentGlowIndex * 0x38 + 0x8, 0); // green
                writeMem<float>(glowObj + currentGlowIndex * 0x38 + 0xC, 0); // blue
                writeMem<float>(glowObj + currentGlowIndex * 0x38 + 0x10, 255);
                writeMem<bool>(glowObj + currentGlowIndex * 0x38 + 0x24, true);
                writeMem<bool>(glowObj + currentGlowIndex * 0x38 + 0x25, false);
            }
            else // если игрок тиммейт
            {
                // делаем его обводку синим
                writeMem<float>(glowObj + currentGlowIndex * 0x38 + 0x4, 0); // red
                writeMem<float>(glowObj + currentGlowIndex * 0x38 + 0x8, 0); // green
                writeMem<float>(glowObj + currentGlowIndex * 0x38 + 0xC, 255); // blue
                writeMem<float>(glowObj + currentGlowIndex * 0x38 + 0x10, 255);
                writeMem<bool>(glowObj + currentGlowIndex * 0x38 + 0x24, true);
                writeMem<bool>(glowObj + currentGlowIndex * 0x38 + 0x25, false);
            }
        }
    }
}
std::thread whThread(wallhack);
while; (true)
{

    if (GetAsyncKeyState(VK_F9)) // если нажали f9
    {

        wh = !wh; // заменяем значение переменной на противоположное
        if (wh)
            std::cout("wh: on\n"); // если wallhack - true, то пишем, что вх включен
        else
            std::cout("wh: off\n"); // иначе пишем, что вх выключен

        Sleep(100); // таймаут, чтобы сбросить нагрузку

    }

}
