#include "pch.h"

#include "format.h"

void setupMBR(BYTE* buffer, DWORD total_num, DWORD hidden_num) {
	DWORD boot_offset = 0x0, partition_tab1_offset = 0x1BE, partition_tab2_offset = 0x1CE,
		partition_tab3_offset = 0x1DE, partition_tab4_offset = 0x1EE, sign_offset = 0x1FE;
	BYTE boot[446] = { 0 }, partition_tab1[16] = { 0 }, partition_tab2[16] = { 0 }, partition_tab3[16] = { 0 },
		partition_tab4[16] = { 0 }, sign[2] = { 0x55, 0xAA };

	// initial boot
	boot[0] = 0x40; //@HaoTse
	boot[1] = 0x48;
	boot[2] = 0x61;
	boot[3] = 0x6F;
	boot[4] = 0x54;
	boot[5] = 0x73;
	boot[6] = 0x65;
	memcpy(buffer + boot_offset, boot, 446);

	// initial partition table
	partition_tab1[0x4] = 0x0C;
	DWORD offset = hidden_num, size_bytes = total_num - hidden_num;
	partition_tab1[0x8] = (BYTE)(offset);
	partition_tab1[0x9] = (BYTE)(offset >> 8);
	partition_tab1[0xa] = (BYTE)(offset >> 16);
	partition_tab1[0xb] = (BYTE)(offset >> 24);
	partition_tab1[0xc] = (BYTE)(size_bytes);
	partition_tab1[0xd] = (BYTE)(size_bytes >> 8);
	partition_tab1[0xe] = (BYTE)(size_bytes >> 16);
	partition_tab1[0xf] = (BYTE)(size_bytes >> 24);
	memcpy(buffer + partition_tab1_offset, partition_tab1, 16);
	memcpy(buffer + partition_tab2_offset, partition_tab2, 16);
	memcpy(buffer + partition_tab3_offset, partition_tab3, 16);
	memcpy(buffer + partition_tab4_offset, partition_tab4, 16);

	// initial signature
	memcpy(buffer + sign_offset, sign, 2);
}

ULONGLONG computeFATSz(DWORD total_sec, DWORD hid_sec, DWORD rsvd_sec, DWORD clu_size, DWORD byte_per_sec, DWORD FAT_num) {
	ULONGLONG free_sec = total_sec - hid_sec - rsvd_sec;
	ULONGLONG denominator = (((ULONGLONG)free_sec * 4) + 8 * ((ULONGLONG)clu_size / byte_per_sec)),
			decimal = (ULONGLONG)clu_size + 8;
	return (ULONGLONG)ceil((double)denominator / decimal);
}

void setupDBR(BYTE* buffer, DWORD total_num, DWORD hidden_num, DWORD rsvd_num, DWORD clu_size) {
	DWORD jumpBoot_offset = 0x0, OEMName_offset = 0x3, bytePerSec_offset = 0xB, secPerClu_offset = 0xD,
			rsvdSecCnt_offset = 0xE, numFATs_offset = 0x10, rootEntCnt_offset = 0x11, totSec16_offset = 0x13,
			media_offset = 0x15, FATSz16_offset = 0x16, secPerTrk_offset = 0x18, numHeads_offset = 0x1A, 
			hidSec_offset = 0x1C, totSec32_offset = 0x20, FATSz32_offset = 0x24, ExtFlags_offset = 0x28, 
			FSVer_offset = 0x2A, rootClus_offset = 0x2C, FSInfo_offset = 0x30, bkBootSec_offset = 0x32, 
			reserved_offset = 0x34, drvNum_offset = 0x40, reserved1_offset = 0x41, bootSig_offset = 0x42, 
			volID_offset = 0x43, volLab_offset = 0x47, FilSysType_offset = 0x52, unused_offset = 0x5A, sig_offset = 0x1FE;
	BYTE jumpBoot[3] = { 0xEB, 0x58, 0x90 }, OEMName[8] = { 0x48, 0x61, 0x6F, 0x30, 0x2E, 0x31, 0x20, 0x20 },
			bytePerSec[2] = { 0 }, secPerClu[1] = { 0 }, rsvdSecCnt[2] = { 0 }, numFATs[1] = { 0x2 },
			rootEntCnt[2] = { 0 }, totSec16[2] = { 0 },	media[1] = { 0xF8 }, FATSz16[2] = { 0 },
			secPerTrk[2] = { 0 }, numHeads[2] = { 0 }, hidSec[4] = { 0 }, totSec32[4] = { 0 },
			FATSz32[4] = { 0 }, ExtFlags[2] = { 0 }, FSVer[2] = { 0 }, rootClus[4] = { 0 },
			FSInfo[2] = { 0 }, bkBootSec[2] = { 0 }, reserved[12] = { 0 }, drvNum[1] = { 0x80 },
			reserved1[1] = { 0 }, bootSig[1] = { 0x29 }, volID[4] = { 0 },
			volLab[11] = { 0x48, 0x61, 0x6F, 0x20, 0x43, 0x68, 0x65, 0x6E, 0x67, 0x20, 0x20 },
			FilSysType[8] = { 0x46, 0x41, 0x54, 0x33, 0x32, 0x20, 0x20, 0x20 }, unused[420] = {0}, sig[2] = { 0x55, 0xAA };

	// initial bytePerSec (512Bytes)
	bytePerSec[0] = (BYTE)(PHYSICAL_SECTOR_SIZE);
	bytePerSec[1] = (BYTE)(PHYSICAL_SECTOR_SIZE >> 8);

	// initial secPerClu
	DWORD sec_cnt = clu_size / PHYSICAL_SECTOR_SIZE;
	secPerClu[0] = (BYTE)(sec_cnt);

	// initial rsvdSecCnt
	rsvdSecCnt[0] = (BYTE)(rsvd_num);
	rsvdSecCnt[1] = (BYTE)(rsvd_num >> 8);

	// initial hidSec
	hidSec[0] = (BYTE)(hidden_num);
	hidSec[1] = (BYTE)(hidden_num >> 8);
	hidSec[2] = (BYTE)(hidden_num >> 16);
	hidSec[3] = (BYTE)(hidden_num >> 24);

	// initial totSec32
	DWORD FS_sec = total_num - hidden_num;
	totSec32[0] = (BYTE)(FS_sec);
	totSec32[1] = (BYTE)(FS_sec >> 8);
	totSec32[2] = (BYTE)(FS_sec >> 16);
	totSec32[3] = (BYTE)(FS_sec >> 24);

	// initial FATSz32
	ULONGLONG FATSz = computeFATSz(total_num, hidden_num, rsvd_num, clu_size, PHYSICAL_SECTOR_SIZE, 2);
	FATSz32[0] = (BYTE)(FATSz);
	FATSz32[1] = (BYTE)(FATSz >> 8);
	FATSz32[2] = (BYTE)(FATSz >> 16);
	FATSz32[3] = (BYTE)(FATSz >> 24);

	// initial rootClus, FSInfo, and bkBootSec
	rootClus[0] = 0x2;
	FSInfo[0] = 0x0;
	bkBootSec[0] = 0x0;

	memcpy(buffer + jumpBoot_offset, jumpBoot, sizeof(jumpBoot));
	memcpy(buffer + OEMName_offset, OEMName, sizeof(OEMName));
	memcpy(buffer + bytePerSec_offset, bytePerSec, sizeof(bytePerSec));
	memcpy(buffer + secPerClu_offset, secPerClu, sizeof(secPerClu));
	memcpy(buffer + rsvdSecCnt_offset, rsvdSecCnt, sizeof(rsvdSecCnt));
	memcpy(buffer + numFATs_offset, numFATs, sizeof(numFATs));
	memcpy(buffer + rootEntCnt_offset, rootEntCnt, sizeof(rootEntCnt));
	memcpy(buffer + totSec16_offset, totSec16, sizeof(totSec16));
	memcpy(buffer + media_offset, media, sizeof(media));
	memcpy(buffer + FATSz16_offset, FATSz16, sizeof(FATSz16));
	memcpy(buffer + secPerTrk_offset, secPerTrk, sizeof(secPerTrk));
	memcpy(buffer + numHeads_offset, numHeads, sizeof(numHeads));
	memcpy(buffer + hidSec_offset, hidSec, sizeof(hidSec));
	memcpy(buffer + totSec32_offset, totSec32, sizeof(totSec32));
	memcpy(buffer + FATSz32_offset, FATSz32, sizeof(FATSz32));
	memcpy(buffer + ExtFlags_offset, ExtFlags, sizeof(ExtFlags));
	memcpy(buffer + FSVer_offset, FSVer, sizeof(FSVer));
	memcpy(buffer + rootClus_offset, rootClus, sizeof(rootClus));
	memcpy(buffer + FSInfo_offset, FSInfo, sizeof(FSInfo));
	memcpy(buffer + bkBootSec_offset, bkBootSec, sizeof(bkBootSec));
	memcpy(buffer + reserved_offset, reserved, sizeof(reserved));
	memcpy(buffer + drvNum_offset, drvNum, sizeof(drvNum));
	memcpy(buffer + reserved1_offset, reserved1, sizeof(reserved1));
	memcpy(buffer + bootSig_offset, bootSig, sizeof(bootSig));
	memcpy(buffer + volID_offset, volID, sizeof(volID));
	memcpy(buffer + volLab_offset, volLab, sizeof(volLab));
	memcpy(buffer + FilSysType_offset, FilSysType, sizeof(FilSysType));
	memcpy(buffer + unused_offset, unused, sizeof(unused));
	memcpy(buffer + sig_offset, sig, sizeof(sig));
}

HANDLE getHandle(char device_name) {
	char device_path[10];
	HANDLE hDevice;

	// initial handle of USB
	sprintf_s(device_path, "\\\\.\\%c:", device_name);
	hDevice = CreateFileA(device_path,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE) {
		TRACE("\n[Error] Open fail. Error Code = %u\n", GetLastError());
	}
	else {
		TRACE("\n[Info] Open success.\n");
	}

	return hDevice;
}

DWORD getCapacity(HANDLE hDevice) {
	DWORD sectorsNum, bytesPerSecotr; // volume information
	BYTE capacityBuf[8];
	int retVal;

	// get capacity
	retVal = SCSIReadCapacity(hDevice, capacityBuf);
	if (!retVal) {
		TRACE("\n[Error] Read capacity fail. Error Code = % u\n", GetLastError());
		return 0;
	}

	// RETURNED LOGICAL BLOCK ADDRESS
	sectorsNum = capacityBuf[0] * (1 << 24) + capacityBuf[1] * (1 << 16)
		+ capacityBuf[2] * (1 << 8) + capacityBuf[3] + 1;
	// BLOCK LENGTH IN BYTES
	bytesPerSecotr = capacityBuf[4] * (1 << 24) + capacityBuf[5] * (1 << 16)
		+ capacityBuf[6] * (1 << 8) + capacityBuf[7];
	TRACE("\n[Info] Read capcaity success.\n");
	TRACE("\n[Msg] Sectors number: % d\n", sectorsNum);
	TRACE("\n[Msg] Bytes per sectors: % d\n", bytesPerSecotr);

	if (bytesPerSecotr != PHYSICAL_SECTOR_SIZE) {
		TRACE("\n[Warn] PHYSICAL_SECTOR_SIZE is not equal to block length!\n");
	}

	return sectorsNum;
}

BOOL formatRegion(HANDLE hDevice, ULONGLONG offset, ULONGLONG size) {
	/*
	 * Format the entire continues region to 0.
	 * Parameters: offset is in bytes, and size are in sectors.
	 */

	// format needed bytes to 0
	DWORD max_buf_size = getMaxTransfLen(hDevice) * 100;
	BYTE* data_buf = new BYTE[max_buf_size];
	memset(data_buf, 0, max_buf_size);
	DWORD turns = (DWORD)(size / (max_buf_size / PHYSICAL_SECTOR_SIZE));
	DWORD remainder = (DWORD)(size % (max_buf_size / PHYSICAL_SECTOR_SIZE));
	for (DWORD i = 0; i < turns; i++) {
		if (!SCSISectorIO(hDevice, offset + (ULONGLONG)max_buf_size * i, data_buf, max_buf_size, TRUE)) {
			return FALSE;
		}
	}
	if (remainder > 0) {
		if (!SCSISectorIO(hDevice, offset + (ULONGLONG)max_buf_size * turns, data_buf, remainder * PHYSICAL_SECTOR_SIZE, TRUE)) {
			return FALSE;
		}
	}

	delete[] data_buf;

	return TRUE;
}

BOOL format(HANDLE hDevice, DWORD sectorsNum, bool ifMBR, DWORD hid_sec_num, DWORD rsvd_sec_num, DWORD clu_size) {
	DWORD bytesReturned = 0;

	// Lock the volume
	if (!DeviceIoControl(hDevice, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &bytesReturned, NULL)) {
		TRACE("\n[Error] FSCTL_LOCK_VOLUME failed. Error code = %u\n", GetLastError());
		return FALSE;
	}

	// format hidden
	if (hid_sec_num > 0) {
		DWORD tmp = hid_sec_num > 10 ? 10 : hid_sec_num;
		if (!formatRegion(hDevice, 0, tmp)) {
			TRACE("\n[Error] Format hidden failed.\n");
			return FALSE;
		}
	}

	// format MBR
	if (ifMBR) {
		BYTE MBR_buf[PHYSICAL_SECTOR_SIZE];
		memset(MBR_buf, 0, PHYSICAL_SECTOR_SIZE);
		setupMBR(MBR_buf, sectorsNum, hid_sec_num);
		if (!SCSISectorIO(hDevice, 0, MBR_buf, PHYSICAL_SECTOR_SIZE, TRUE)) {
			TRACE("\n[Error] Format MBR failed.\n");
			return FALSE;
		}
	}

	// format FSINFO and BK (reserved)
	ULONGLONG hid_offset = (ULONGLONG)hid_sec_num * PHYSICAL_SECTOR_SIZE;
	if (!formatRegion(hDevice, hid_offset, rsvd_sec_num)) {
		TRACE("\n[Error] Format reserved failed.\n");
		return FALSE;
	}

	// format DBR
	BYTE DBR_buf[PHYSICAL_SECTOR_SIZE];
	ULONGLONG DBR_offset = (ULONGLONG)hid_sec_num * PHYSICAL_SECTOR_SIZE;
	memset(DBR_buf, 0, PHYSICAL_SECTOR_SIZE);
	setupDBR(DBR_buf, sectorsNum, hid_sec_num, rsvd_sec_num, clu_size);
	if (!SCSISectorIO(hDevice, DBR_offset, DBR_buf, PHYSICAL_SECTOR_SIZE, TRUE)) {
		TRACE("\n[Error] Format DBR failed.\n");
		return FALSE;
	}

	// format FAT
	ULONGLONG FAT_offset = ((ULONGLONG)hid_sec_num + rsvd_sec_num) * PHYSICAL_SECTOR_SIZE;
	ULONGLONG FATSz = computeFATSz(sectorsNum, hid_sec_num, rsvd_sec_num, clu_size, PHYSICAL_SECTOR_SIZE, 2);
	BYTE FAT_init[PHYSICAL_SECTOR_SIZE] = { 0 };
	FAT_init[0] = 0xF8;
	FAT_init[1] = 0xFF;
	FAT_init[2] = 0xFF;
	FAT_init[3] = 0x0F;
	FAT_init[4] = 0xFF;
	FAT_init[5] = 0xFF;
	FAT_init[6] = 0xFF;
	FAT_init[7] = 0xFF;
	FAT_init[8] = 0xFF;
	FAT_init[9] = 0xFF;
	FAT_init[10] = 0xFF;
	FAT_init[11] = 0x0F;
	if (!formatRegion(hDevice, FAT_offset, FATSz * 2)) {
		TRACE("\n[Error] Format FAT failed.\n");
		return FALSE;
	}
	if (!SCSISectorIO(hDevice, FAT_offset, FAT_init, 12, TRUE)) {
		TRACE("\n[Error] Format FAT1 failed.\n");
		return FALSE;
	}
	if (!SCSISectorIO(hDevice, FAT_offset + ((ULONGLONG)FATSz * PHYSICAL_SECTOR_SIZE), FAT_init, 12, TRUE)) {
		TRACE("\n[Error] Format FAT2 failed.\n");
		return FALSE;
	}

	// format 2nd cluster (Root)
	ULONGLONG rsvd_offset = (ULONGLONG)rsvd_sec_num * PHYSICAL_SECTOR_SIZE;
	ULONGLONG heap_offset = hid_offset + rsvd_offset + FATSz * PHYSICAL_SECTOR_SIZE * 2;
	DWORD clu_size_in_secotr = clu_size / PHYSICAL_SECTOR_SIZE;
	if (!formatRegion(hDevice, heap_offset, clu_size_in_secotr)) {
		TRACE("\n[Error] Format Root cluster failed.\n");
		return FALSE;
	}

	if (!DeviceIoControl(hDevice, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &bytesReturned, NULL)) {
		TRACE("\n[Error] FSCTL_DISMOUNT_VOLUME failed. Error code = %u\n", GetLastError());
		return FALSE;
	}
	if (!DeviceIoControl(hDevice, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &bytesReturned, NULL)) {
		TRACE("\n[Error] FSCTL_UNLOCK_VOLUME failed. Error code = %u\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}