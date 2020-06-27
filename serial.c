#include <windows.h>
#include <stdio.h>

#include "serial.h"

char serial_comm_buf[256];
const char *device_name = "\\\\.\\COM4";

void serial_communication_test(void)
{
    HANDLE hComm;
    BOOL status;
    DCB dcbSerialParams = {0};                          // Initializing DCB structure
    COMMTIMEOUTS timeouts = {0};
    DWORD dwBytesToWrite;
    DWORD dwBytesWritten;
    int serial_comm_count;

    printf("Serial connect test start ...\n");
    hComm = CreateFile(device_name,                     // Device name
                        GENERIC_READ | GENERIC_WRITE,   // Read & Write
                        0,                              // No sharing
                        NULL,                           // No security
                        OPEN_EXISTING,                  // Open existing port only
                        0,                              // Non overlapped I/O
                        NULL);                          // Null for comm devices

    if (hComm == INVALID_HANDLE_VALUE) {
        printf("Open %s failed\n", device_name);
    } else {
        printf("Open %s passed\n", device_name);
    }

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    status = GetCommState(hComm, &dcbSerialParams);     // retreives  the current settings
    if (status == FALSE) {
        printf("%s get comm state failed (%d)\n", device_name, GetLastError());
    }

    dcbSerialParams.BaudRate = CBR_9600;                // Set BaudRate = 9600
    dcbSerialParams.ByteSize = 8;                       // Set ByteSize = 8
    dcbSerialParams.StopBits = ONE5STOPBITS;            // Set StopBits = 1
    dcbSerialParams.Parity = NOPARITY;                  // Set Parity = None

    status = SetCommState(hComm, &dcbSerialParams);     // Configurate the port according to settings in DCB
    if (status == FALSE) {
        printf("%s set comm state failed (%d)\n", device_name, GetLastError());
        printf("---\n");
        printf("dcbSerialParams.BaudRate: %d\n", dcbSerialParams.BaudRate);
        printf("dcbSerialParams.ByteSize: %d\n", dcbSerialParams.ByteSize);
        printf("dcbSerialParams.StopBits: %d\n", dcbSerialParams.StopBits);
        printf("dcbSerialParams.Parity: %d\n", dcbSerialParams.Parity);
        printf("---\n");
    }

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    status = SetCommTimeouts(hComm, &timeouts);
    if (status == FALSE) {
        printf("%s set comm timeouts failed (%d)\n", device_name, GetLastError());
        printf("---\n");
        printf("timeouts.ReadIntervalTimeout: %d\n", timeouts.ReadIntervalTimeout);
        printf("timeouts.ReadTotalTimeoutConstant: %d\n", timeouts.ReadTotalTimeoutConstant);
        printf("timeouts.ReadTotalTimeoutMultiplier: %d\n", timeouts.ReadTotalTimeoutMultiplier);
        printf("timeouts.WriteTotalTimeoutConstant: %d\n", timeouts.WriteTotalTimeoutConstant);
        printf("timeouts.WriteTotalTimeoutMultiplier: %d\n", timeouts.WriteTotalTimeoutMultiplier);
        printf("---\n");
    }

    serial_comm_count = 0;
    dwBytesWritten = 0;
    while (serial_comm_count < 100) {
        sprintf(serial_comm_buf, "Serial communiction test %d\n", serial_comm_count);
        dwBytesToWrite = strlen(serial_comm_buf);
        serial_comm_count++;
        status = WriteFile(hComm,
                            serial_comm_buf,
                            dwBytesToWrite,
                            &dwBytesWritten,
                            NULL);

        if (status == TRUE) {
            printf("%d bytes - written to %s\n", dwBytesToWrite, device_name);
        } else {
            printf("%s write file failed (%d)\n", device_name, GetLastError());
        }
        Sleep(1000);
    }

    printf("Serial connect test end ...\n");

    CloseHandle(hComm); // Close the serial port
}

void serial_communication_test2(void)
{
    HANDLE hComm;
    BOOL status;
    int serial_comm_count;
    COMMTIMEOUTS timeouts = {0};
    DCB dcbSerialParams = {0};                          // Initializing DCB structure
    DWORD dwBytesToWrite;
    DWORD dwBytesWritten = 0;

    printf("Serial connect test start ...\n");
    serial_comm_count = 0;
    while (serial_comm_count < 100) {
        hComm = CreateFile(device_name,                     // Device name
                            GENERIC_READ | GENERIC_WRITE,   // Read & Write
                            0,                              // No sharing
                            NULL,                           // No security
                            OPEN_EXISTING,                  // Open existing port only
                            0,                              // Non overlapped I/O
                            NULL);                          // Null for comm devices

        if (hComm == INVALID_HANDLE_VALUE) {
            printf("Open %s failed\n", device_name);
        } else {
            printf("Open %s passed\n", device_name);
        }

        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        status = GetCommState(hComm, &dcbSerialParams);     // retreives  the current settings
        if (status == FALSE) {
            printf("%s get comm state failed (%d)\n", device_name, GetLastError());
        }

        memset(&dcbSerialParams, 0, sizeof(dcbSerialParams));
        dcbSerialParams.BaudRate = CBR_9600;                // Set BaudRate = 9600
        dcbSerialParams.ByteSize = 8;                       // Set ByteSize = 8
        dcbSerialParams.StopBits = ONE5STOPBITS;            // Set StopBits = 1
        dcbSerialParams.Parity = NOPARITY;                  // Set Parity = None

        status = SetCommState(hComm, &dcbSerialParams);     // Configurate the port according to settings in DCB
        if (status == FALSE) {
            printf("%s set comm state failed (%d)\n", device_name, GetLastError());
            printf("---\n");
            printf("dcbSerialParams.BaudRate: %d\n", dcbSerialParams.BaudRate);
            printf("dcbSerialParams.ByteSize: %d\n", dcbSerialParams.ByteSize);
            printf("dcbSerialParams.StopBits: %d\n", dcbSerialParams.StopBits);
            printf("dcbSerialParams.Parity: %d\n", dcbSerialParams.Parity);
            printf("---\n");
        }

        memset(&timeouts, 0, sizeof(timeouts));
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;

        status = SetCommTimeouts(hComm, &timeouts);
        if (status == FALSE) {
            printf("%s set comm timeouts failed (%d)\n", device_name, GetLastError());
            printf("---\n");
            printf("timeouts.ReadIntervalTimeout: %d\n", timeouts.ReadIntervalTimeout);
            printf("timeouts.ReadTotalTimeoutConstant: %d\n", timeouts.ReadTotalTimeoutConstant);
            printf("timeouts.ReadTotalTimeoutMultiplier: %d\n", timeouts.ReadTotalTimeoutMultiplier);
            printf("timeouts.WriteTotalTimeoutConstant: %d\n", timeouts.WriteTotalTimeoutConstant);
            printf("timeouts.WriteTotalTimeoutMultiplier: %d\n", timeouts.WriteTotalTimeoutMultiplier);
            printf("---\n");
        }

        sprintf(serial_comm_buf, "Serial communiction test %d\n", serial_comm_count);
        dwBytesToWrite = strlen(serial_comm_buf);
        serial_comm_count++;
        status = WriteFile(hComm,
                            serial_comm_buf,
                            dwBytesToWrite,
                            &dwBytesWritten,
                            NULL);

        if (status == TRUE) {
            printf("%d bytes - written to %s\n", dwBytesToWrite, device_name);
        } else {
            printf("%s write file failed (%d)\n", device_name, GetLastError());
        }
        Sleep(1000);
        CloseHandle(hComm); // Close the serial port
    }

    printf("Serial connect test end ...\n");
}
