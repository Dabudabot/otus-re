#include <kefuncs.h>
#include <rtlfuncs.h>
#include <exfuncs.h>
#include <psfuncs.h>
#include <iofuncs.h>
#include <obfuncs.h>

#define DEBUG
#define MBR_SIZE 512

typedef struct _KEYBOARD_INPUT_DATA {
  USHORT UnitId;
  USHORT MakeCode;
  USHORT Flags;
  USHORT Reserved;
  ULONG  ExtraInformation;
} KEYBOARD_INPUT_DATA, *PKEYBOARD_INPUT_DATA;


void Sleep(DWORD mSec)
{
    LARGE_INTEGER interval;
    interval.QuadPart = -1 * (int)(mSec * 10000);
    NtDelayExecution(FALSE, &interval);
}

void WriteLn(LPWSTR Message)
{
    UNICODE_STRING string;
    RtlInitUnicodeString(&string, Message);
    NtDisplayString(&string);
}

BOOL DumpMbr()
{
    NTSTATUS Status;
    HANDLE fMbr, fileHandle;
    char mbr[MBR_SIZE];
    OBJECT_ATTRIBUTES ObjectAttributes;
    IO_STATUS_BLOCK ioStatusBlock;
    UNICODE_STRING string;
    LARGE_INTEGER fileSize;

    fileSize.QuadPart = 0;
    memset(&mbr, '\0',MBR_SIZE);

    RtlInitUnicodeString(&string, L"\\??\\PhysicalDrive0");
    InitializeObjectAttributes(&ObjectAttributes, &string, OBJ_CASE_INSENSITIVE, NULL, NULL);
    Status = NtCreateFile(&fMbr,
                GENERIC_READ | SYNCHRONIZE,
                &ObjectAttributes,
                &ioStatusBlock,
                &fileSize,
                FILE_ATTRIBUTE_NORMAL,
                FILE_SHARE_READ,
                FILE_OPEN,
                FILE_SEQUENTIAL_ONLY | FILE_SYNCHRONOUS_IO_NONALERT,
                NULL, 0);

    if (Status == STATUS_SUCCESS)
    {
        #ifdef DEBUG
        WriteLn(L"+ Open PhysicalDrive0 STATUS_SUCCESS\n");
        #endif

        Status = NtReadFile(fMbr, NULL, NULL, NULL, &ioStatusBlock, mbr, MBR_SIZE, NULL, NULL);
        if (Status == STATUS_SUCCESS)
        {
            #ifdef DEBUG
            WriteLn(L"+ Read MBR OK!\n");
            #endif
            RtlInitUnicodeString(&string, L"\\??\\C:\\mbr.bin");
            InitializeObjectAttributes(&ObjectAttributes, &string, OBJ_CASE_INSENSITIVE, NULL, NULL);
            Status = NtCreateFile(&fileHandle,
                        GENERIC_WRITE | SYNCHRONIZE,
                        &ObjectAttributes,
                        &ioStatusBlock,
                        &fileSize,
                        FILE_ATTRIBUTE_NORMAL,
                        0,
                        FILE_SUPERSEDE,
                        FILE_SEQUENTIAL_ONLY | FILE_SYNCHRONOUS_IO_NONALERT,
                        NULL, 0);

            if (Status == STATUS_SUCCESS)
            {
                #ifdef DEBUG
                WriteLn(L"+ Open C:\\mbr.bin STATUS_SUCCESS!\n");
                #endif
                Status = NtWriteFile(fileHandle, NULL, NULL, NULL, &ioStatusBlock, mbr, MBR_SIZE, NULL, NULL);
                if (Status == STATUS_SUCCESS)
                {
                    #ifdef DEBUG
                    WriteLn(L"+ Dump MBR OK!\n");
                    #endif
                }
                else
                {
                    #ifdef DEBUG
                    WriteLn(L"- Dump MBR ERROR!\n");
                    #endif
                }
            }
            else
            {
                #ifdef DEBUG
                WriteLn(L"- Open C:\\mbr.bin Faled!\n");
                #endif
            }
            NtClose(fileHandle);
        }
        else
        {
            #ifdef DEBUG
            WriteLn(L"- Read MBR ERROR!\n");
            #endif
        }
    }
    else
    {
        #ifdef DEBUG
        WriteLn(L"- Open PhysicalDrive0 Faled\n");
        #endif
    }
    NtClose(fMbr);
    return (Status == STATUS_SUCCESS);
}

void NtProcessStartup(void* StartupArgument)
{
    HANDLE hKeyBoard, hEvent;
    UNICODE_STRING skull, keyboard;
    OBJECT_ATTRIBUTES ObjectAttributes;
    IO_STATUS_BLOCK Iosb;
    LARGE_INTEGER ByteOffset;
    KEYBOARD_INPUT_DATA kbData;

    RtlInitUnicodeString(&keyboard, L"\\Device\\KeyboardClass0");
    InitializeObjectAttributes(&ObjectAttributes, &keyboard, OBJ_CASE_INSENSITIVE, NULL, NULL);

    NtCreateFile(&hKeyBoard,
                SYNCHRONIZE | GENERIC_READ | FILE_READ_ATTRIBUTES,
                &ObjectAttributes,
                &Iosb,
                NULL,
                FILE_ATTRIBUTE_NORMAL,
                0,
                FILE_OPEN,FILE_DIRECTORY_FILE,
                NULL, 0);

    InitializeObjectAttributes(&ObjectAttributes, NULL, 0, NULL, NULL);
    NtCreateEvent(&hEvent, EVENT_ALL_ACCESS, &ObjectAttributes, 1, 0);

    if (DumpMbr())
        WriteLn(L"Dump Created!\n");
    else
        WriteLn(L"Dump Error!\n");
    while (TRUE)
    {
        NtReadFile(hKeyBoard, hEvent, NULL, NULL, &Iosb, &kbData, sizeof(KEYBOARD_INPUT_DATA), &ByteOffset, NULL);
        NtWaitForSingleObject(hEvent, TRUE, NULL);

        if (kbData.MakeCode == 0x01)    //ESC
        {
                break;
        }

    }
    NtTerminateProcess(NtCurrentProcess(), 0);
}
