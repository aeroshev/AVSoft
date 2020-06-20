// Created by Ерошев Артём
//
// Так в условии не сказано какая разрядность системы, то буду считать, что это x64 программа,
// а значит размер указателя равен 64 битам и использовать ссылку для DWORD64 не имеет смысла, так как она (ссылка)
// имеет размер 64 бита как и DWORD64


#include <Windows.h>
#include <iostream>
#include <memory>
#include <random>
#include <array>

#define QTY_READERS 15
#define QTY_WRITERS 10
#define QTY_ROUND_TRIP 10
#define LATENCY 100 // ms
#define MEM_SIZE 1024

using namespace std;

using sharedDWORD64 = shared_ptr<array<DWORD64, MEM_SIZE>>;

HANDLE hMutex, outMutex;
HANDLE activeEvent;

class Writer
{
private:
    DWORD64 ID;
    DWORD64 key;
    DWORD64 num;
    sharedDWORD64 sharedMemory;
public:
    Writer() = default;

    Writer(DWORD64 ID_, DWORD64 key_, DWORD64 num_, sharedDWORD64 ptrMem):
        ID(ID_),
        key(key_),
        num(num_),
        sharedMemory(ptrMem)
    {}

    VOID set(DWORD64 ID_, DWORD64 key_, DWORD64 num_, sharedDWORD64 ptrMem)
    {
        ID = ID_;
        key = key_;
        num = num_;
        sharedMemory = ptrMem;
    }

    static DWORD WINAPI WriteInMemory(LPVOID lpParam)
    {
        auto writer = (Writer*)lpParam;
        Sleep(LATENCY);

        if (writer->key < MEM_SIZE && writer->key >= 0)
        {
            ResetEvent(activeEvent);

            WaitForSingleObject(hMutex, INFINITE);
            writer->sharedMemory->at(writer->key) = writer->num;
            auto writed_value = writer->sharedMemory->at(writer->key);
            ReleaseMutex(hMutex);

            WaitForSingleObject(outMutex, INFINITE);
            wcout << L"Writer ID - " << writer->ID << L", Write in mem[" << writer->key << L"] = " << writed_value << '\n';
            ReleaseMutex(outMutex);

            SetEvent(activeEvent);
        }

        ExitThread(0);
    }

    HANDLE CreateThreadWriter()
    {
        return CreateThread(NULL, 0, WriteInMemory, this, NULL, 0);
    }
};

class Reader
{
private:
    DWORD64 ID;
    DWORD64 key;
    sharedDWORD64 sharedMemory;
public:
    Reader() = default;

    Reader(DWORD64 ID_, DWORD64 key_, sharedDWORD64 ptrMem):
        ID(ID_),
        key(key_),
        sharedMemory(ptrMem)
    {}

    VOID set(DWORD64 ID_, DWORD64 key_, sharedDWORD64 ptrMem)
    {
        ID = ID_;
        key = key_;
        sharedMemory = ptrMem;
    }

    static DWORD WINAPI ReadFromMemory(LPVOID lpParam)
    {
        auto reader = (Reader*)lpParam;
        Sleep(LATENCY);

        if (reader->key < MEM_SIZE && reader->key >= 0)
        {
            WaitForSingleObject(activeEvent, INFINITE);
            auto num_ = reader->sharedMemory->at(reader->key);

            WaitForSingleObject(outMutex, INFINITE);
            wcout << L"Reader ID - " << reader->ID << L", read from mem[" << reader->key << L"] number -> " << num_ << '\n';
            ReleaseMutex(outMutex);
        }

        ExitThread(0);
    }

    HANDLE CreateThreadReader()
    {
        return CreateThread(NULL, 0, ReadFromMemory, this, NULL, 0);
    }
};


int main()
{
    random_device device;
    mt19937 rng(device());
    uniform_int_distribution<mt19937::result_type> dist_key(0, MEM_SIZE - 1);
    uniform_int_distribution<mt19937::result_type> dist_value(0 - MAXDWORD64 - 1);

    array<Writer, QTY_WRITERS> oWriters;
    array<HANDLE, QTY_WRITERS> hWriters;

    array<Reader, QTY_READERS> oReaders;
    array<HANDLE, QTY_READERS> hReaders;

    auto SharedMemory = make_shared<array<DWORD64, MEM_SIZE>>();
   
    hMutex = CreateMutex(NULL, FALSE, L"Mutex");
    outMutex = CreateMutex(NULL, FALSE, L"OutputMutex");
    activeEvent = CreateEvent(NULL, TRUE, FALSE, L"ActiveEvent");

    DWORD flag;
    wcout << L"Clear after every round? 1 - Yes, 0 - No" << '\n';
    wcin >> flag;

    for (DWORD loop = 0; loop != QTY_ROUND_TRIP; loop++)
    {
        wcout << L"Number trip: " << loop << '\n';
        if (flag)
        {
            SharedMemory->fill(0);
        }

        for (DWORD i = 0; i != QTY_WRITERS; i++)
        {
            oWriters[i].set(i, dist_key(rng), dist_value(rng), SharedMemory);
            hWriters[i] = oWriters[i].CreateThreadWriter();
        }

        for (DWORD i = 0; i != QTY_READERS; i++)
        {
            oReaders[i].set(i, dist_key(rng), SharedMemory);
            hReaders[i] = oReaders[i].CreateThreadReader();
        }

        WaitForMultipleObjects(QTY_WRITERS, hWriters.data(), TRUE, INFINITE);
        WaitForMultipleObjects(QTY_READERS, hReaders.data(), TRUE, INFINITE);
    }

    return 0;
}
