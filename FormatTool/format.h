#pragma once

#include <windows.h>
#include <ntddscsi.h>

#include "SCSI_IO.h"

void setupMBR(BYTE* buffer, DWORD total_num, DWORD hidden_num);

void setupDBR(BYTE* buffer, DWORD total_num, DWORD hidden_num, DWORD rsvd_num, DWORD clu_size);

ULONGLONG computeFATSz(DWORD total_sec, DWORD hid_sec, DWORD rsvd_sec, DWORD clu_size, DWORD byte_per_sec, DWORD FAT_num);

HANDLE getHandle(char device_name);

DWORD getCapacity(HANDLE hDevice);

BOOL format(HANDLE hDevice, DWORD totalSec_num, bool ifMBR, DWORD hidSec_num, DWORD rsvdSec_num, DWORD clu_size);