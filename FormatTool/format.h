#pragma once

#include <windows.h>
#include <ntddscsi.h>

#include "SCSI_IO.h"

void setupMBR(BYTE* buffer, DWORD total_num, DWORD hidden_num);

ULONGLONG computeFATSz(DWORD total_sec, DWORD hid_sec, DWORD rsvd_sec, DWORD clu_size, DWORD byte_per_sec, DWORD FAT_num);

void setupDBR(BYTE* buffer, DWORD total_num, DWORD hidden_num, DWORD rsvd_num, DWORD clu_size);

HANDLE getHandle(char device_name);

DWORD getCapacity(HANDLE hDevice);

BOOL formatRegion(HANDLE hDevice, ULONGLONG offset, ULONGLONG size);

BOOL format(HANDLE hDevice, DWORD sectorsNum, bool ifMBR, DWORD hid_sec_num, DWORD rsvd_sec_num, DWORD clu_size);