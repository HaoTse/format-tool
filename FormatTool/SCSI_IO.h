/*++
AccessMBR

This module sends a SCSI Passthrough command to send a read or write command directly to the disk.
Bypassing an MJ_READ/WRITE filter.

Written by Andrea Allievi, Cisco Talos
Copyright (C) 2016 Cisco Systems Inc
--*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinIoCtl.h>
#include <Ntddscsi.h>

#define SCSIOP_WRITE 0x2A
#define SCSIOP_READ  0x28
#define SCSI_TIMEOUT 0x100
// The physical sector size in bytes
#define PHYSICAL_SECTOR_SIZE 512

// The scsi capability of USB2.0
#define SCSI_CAPABILITY_USB2 65536

// Command to get capacity of volume
BOOL SCSIReadCapacity(HANDLE hDevice, BYTE* capacityBuf);

// Obtain maximum transfer length
DWORD getMaxTransfLen(HANDLE hDrive);

// SCSI Read/Write Sector
BOOL SCSISectorIO(HANDLE hDrive, DWORD maxTransfLen, ULONGLONG offset, LPBYTE buffer, UINT buffSize, BOOLEAN write);

// Compila il Command Descriptor Block della richiesta di lettura o scrittura a 10 Bytes
BOOL SCSIBuild10CDB(PSCSI_PASS_THROUGH_DIRECT srb, ULONGLONG offset, ULONG length, BOOLEAN Write);

