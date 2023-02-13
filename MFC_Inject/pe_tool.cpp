#include "pch.h"
#include "pe_tool.h"
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include "helper.h"
#include <string.h>
using namespace std;



char* getpFileBuffer(char* filePath, int* fsize, int* is64) {
	FILE* pfile = NULL;
	fopen_s(&pfile, filePath, "rb");
	if (pfile == NULL)
	{
		printf(" getpFileBuffer Open file Failed \n");

		return NULL;
	}
	fseek(pfile, 0, SEEK_END);
	int nSize = ftell(pfile);
	fseek(pfile, 0, SEEK_SET);
	char* pFileBuffer = (char*)malloc(nSize);
	if (pFileBuffer == NULL)
	{
		printf("%d \n", nSize);
		return 0;
	}
	memset(pFileBuffer, 0, nSize);
	fread_s(pFileBuffer, nSize, nSize, 1, pfile);
	fclose(pfile);

	if (*(short*)pFileBuffer != IMAGE_DOS_SIGNATURE)
	{
		printf("NO IMAGE_DOS_SIGNATURE ,This is not a PE File \n");
		chFree(pFileBuffer);
		return NULL;
	}

	if ((*(PDWORD)(pFileBuffer + *(PDWORD)(pFileBuffer + 0x3c))) != IMAGE_NT_SIGNATURE)
	{
		printf("NO IMAGE_NT_SIGNATURE, This is not a PE File \n");
		chFree(pFileBuffer);
		return NULL;
	}


	WORD machine = *(PWORD)(pFileBuffer + *(PDWORD)(pFileBuffer + 0x3c) + 0x4);
	*is64 = machine == 0x014c ? 0 : 1;
	*fsize = nSize;
	return pFileBuffer;
}


void Mem2File(char* outFilePath, char* pFileBuffer, int* fsize) {
	FILE* pFile = NULL;
	fopen_s(&pFile, outFilePath, "wb");
	if (!pFile)
	{
		printf("fopen failed \n");
		return;
	}
	fwrite(pFileBuffer, *fsize, 1, pFile);
	fclose(pFile);
}
void chFree(char* ptr) {
	if (ptr != NULL)
	{
		free(ptr);
		ptr = NULL;
	}

}
void shFree(short* ptr) {
	if (ptr != NULL)
	{
		free(ptr);
		ptr = NULL;
	}
}
void nFree(int* ptr) {
	if (ptr != NULL)
	{
		free(ptr);
		ptr = NULL;
	}
}




void clearPE(char* pFileBuffer, PDWORD exportDirAddr) {
	char* imageOptionHeader = ((pFileBuffer + *(PDWORD)(pFileBuffer + 0x3c)) + 0x4) + IMAGE_SIZEOF_FILE_HEADER;
	DWORD  sizeOfHeaders = *(PDWORD)(imageOptionHeader + 0x3c);
	PCHAR pImageDataDirectory = (imageOptionHeader + 0x60);
	*exportDirAddr = *(PDWORD)pImageDataDirectory;
	memset(pFileBuffer, 0xcc, sizeOfHeaders);
}




BOOL RepairImportTable(PCHAR pImageBuffer) {
	char* imageNtHeader = pImageBuffer + *(PDWORD)(pImageBuffer + 0x3c);
	char* imageFileHeader = imageNtHeader + 0x4;
	char* imageOptionHeader = imageFileHeader + IMAGE_SIZEOF_FILE_HEADER;
	PCHAR pImageDataDirectory = (imageOptionHeader + 0x60);
	DWORD exportTableRav = *(PDWORD)pImageDataDirectory;
	DWORD importTableRav = *(PDWORD)(pImageDataDirectory + 0x8);
	if (importTableRav > 0)
	{
		PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)(pImageBuffer + importTableRav);
		while (pImportDesc->FirstThunk + pImportDesc->OriginalFirstThunk)
		{
			PCHAR importDllName = (PCHAR)pImageBuffer + pImportDesc->Name;
			HMODULE hModule = LoadLibraryA(importDllName);
			if (hModule == NULL)
			{
				return FALSE;
			}
			PDWORD pInt = (PDWORD)(pImageBuffer + pImportDesc->OriginalFirstThunk);
			PDWORD pIat = (PDWORD)(pImageBuffer + pImportDesc->FirstThunk);
			do
			{
				DWORD intVal = *pInt;
				DWORD dwFuncAddr = 0;
				if (intVal & IMAGE_ORDINAL_FLAG)
				{
					dwFuncAddr = (DWORD)GetProcAddress(hModule, (LPCSTR)(intVal & 0x7fffffff));
				}
				else {
					PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)(pImageBuffer + intVal);
					dwFuncAddr = (DWORD)GetProcAddress(hModule, pName->Name);

				}
				if (!dwFuncAddr)
				{
					return FALSE;
				}
				*pIat = dwFuncAddr;
				pInt++;
				pIat++;
			} while (*pInt);
			pImportDesc++;
		}
		return  TRUE;
	}


}

void relocationRepair(PCHAR pFileBuffer) {
	char* imageNtHeader = pFileBuffer + *(PDWORD)(pFileBuffer + 0x3c);
	char* imageFileHeader = imageNtHeader + 0x4;
	char* imageOptionHeader = imageFileHeader + IMAGE_SIZEOF_FILE_HEADER;
	DWORD  dwOldImageBase = *(PDWORD)(imageOptionHeader + 0x1c);

	PCHAR pImageDataDirectory = (imageOptionHeader + 0x60);
	DWORD relocationTableRav = *(PDWORD)(pImageDataDirectory + (0x8 * 5));
	if (relocationTableRav == NULL)
	{
		return;
	}
	printf("relocationTable %08x  oldBaseImage: %08x\n", relocationTableRav, dwOldImageBase);
	PCHAR relocationTab = (PCHAR)(pFileBuffer + relocationTableRav);
	PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION)relocationTab;
	int seed = 0;
	while (*(PLONGLONG)relocation) {
		int blockCount = (relocation->SizeOfBlock - 0x8) / 2;
		++seed;

		PWORD block = (PWORD)(relocationTab + 0x8);
		for (size_t i = 0; i < blockCount; i++)
		{
			WORD val = (*block);
			if (val >> 12 == 0x3)
			{
				DWORD dwDataRav = (val & 0xfff) + relocation->VirtualAddress;
				PDWORD pData = (PDWORD)(pFileBuffer + dwDataRav);
				DWORD dwRepair = (DWORD)(*pData - dwOldImageBase + pFileBuffer);
				*pData = dwRepair;
				block++;
			}
		}
		relocationTab += relocation->SizeOfBlock;
		relocation = (PIMAGE_BASE_RELOCATION)relocationTab;
	};
}

PDWORD  getProcAddressByName(PCHAR pFileBuffer, PCHAR funcName, DWORD exportTableRav) {
	if (exportTableRav == 0)
	{
		return NULL;
	}
	PCHAR pImageExportDirectory = pFileBuffer + exportTableRav;
	DWORD pImageExportDirectoryNumberOfFunctions = *(PDWORD)(pImageExportDirectory + 0x14);
	DWORD pImageExportDirectoryNumberOfNames = *(PDWORD)(pImageExportDirectory + 0x18);
	DWORD pImageExportDirectoryAddrOfFunctions = *(PDWORD)(pImageExportDirectory + 0x1c);
	DWORD pImageExportDirectoryAddrOfNames = *(PDWORD)(pImageExportDirectory + 0x20);
	DWORD pImageExportDirectoryAddrOfNameOrdinals = *(PDWORD)(pImageExportDirectory + 0x24);
	LPDWORD pImageExportDirectoryAddrOfFunctionsIndex = (LPDWORD)(pFileBuffer + pImageExportDirectoryAddrOfFunctions);
	LPWORD pImageExportDirectoryAddrOfNameOrdinalsIndex = (LPWORD)(pFileBuffer + pImageExportDirectoryAddrOfNameOrdinals);
	LPDWORD pImageExportDirectoryAddrOfNamesIndex = (LPDWORD)(pFileBuffer + pImageExportDirectoryAddrOfNames);
	PDWORD funcAddr = NULL;
	for (size_t i = 0; i < pImageExportDirectoryNumberOfNames; i++)
	{
		PCHAR localName = pFileBuffer + pImageExportDirectoryAddrOfNamesIndex[i];
		if (strcmp(localName, funcName) == 0)
		{
			funcAddr = (PDWORD)(pImageExportDirectoryAddrOfFunctionsIndex[pImageExportDirectoryAddrOfNameOrdinalsIndex[i]] + pFileBuffer);
			printf("pImageExportDirectoryAddrOfNames->matched functionName:%s   %08x \n", localName, funcAddr);
			break;
		}
	}
	return funcAddr;
}



PDWORD  getProcAddressByOrdinal(PCHAR pFileBuffer, DWORD index, DWORD exportTableRav) {
	if (exportTableRav == 0)
	{
		return NULL;
	}
	PCHAR pImageExportDirectory = pFileBuffer + exportTableRav;
	DWORD pImageExportDirectoryBase = *(PDWORD)(pImageExportDirectory + 0x10);
	DWORD pImageExportDirectoryNumberOfFunctions = *(PDWORD)(pImageExportDirectory + 0x14);
	DWORD pImageExportDirectoryNumberOfNames = *(PDWORD)(pImageExportDirectory + 0x18);
	DWORD pImageExportDirectoryAddrOfFunctions = *(PDWORD)(pImageExportDirectory + 0x1c);
	DWORD pImageExportDirectoryAddrOfNames = *(PDWORD)(pImageExportDirectory + 0x20);
	DWORD pImageExportDirectoryAddrOfNameOrdinals = *(PDWORD)(pImageExportDirectory + 0x24);
	LPDWORD pImageExportDirectoryAddrOfFunctionsIndex = (LPDWORD)(pFileBuffer + pImageExportDirectoryAddrOfFunctions);
	LPWORD pImageExportDirectoryAddrOfNameOrdinalsIndex = (LPWORD)(pFileBuffer + pImageExportDirectoryAddrOfNameOrdinals);
	LPDWORD pImageExportDirectoryAddrOfNamesIndex = (LPDWORD)(pFileBuffer + pImageExportDirectoryAddrOfNames);
	if (index < pImageExportDirectoryBase || index>pImageExportDirectoryNumberOfFunctions + pImageExportDirectoryBase)
	{
		return NULL;
	}
	int readOrder = index - pImageExportDirectoryBase;
	PDWORD funcAddr = (PDWORD)(pImageExportDirectoryAddrOfFunctionsIndex[readOrder] + pFileBuffer);
	PCHAR localName = pFileBuffer + pImageExportDirectoryAddrOfNamesIndex[readOrder];
	printf("pImageExportDirectoryAddrOfNames->matched functionName:%s   %08x   \n", localName, funcAddr);
	return funcAddr;
}

PDWORD  getAllFunction(PCHAR pFileBuffer, DWORD exportTableRav) {

	PCHAR pImageExportDirectory = pFileBuffer + exportTableRav;
	DWORD pImageExportDirectoryBase = *(PDWORD)(pImageExportDirectory + 0x10);
	DWORD pImageExportDirectoryNumberOfFunctions = *(PDWORD)(pImageExportDirectory + 0x14);
	DWORD pImageExportDirectoryNumberOfNames = *(PDWORD)(pImageExportDirectory + 0x18);
	DWORD pImageExportDirectoryAddrOfFunctions = *(PDWORD)(pImageExportDirectory + 0x1c);
	DWORD pImageExportDirectoryAddrOfNames = *(PDWORD)(pImageExportDirectory + 0x20);
	DWORD pImageExportDirectoryAddrOfNameOrdinals = *(PDWORD)(pImageExportDirectory + 0x24);
	LPDWORD pImageExportDirectoryAddrOfFunctionsIndex = (LPDWORD)(pFileBuffer + pImageExportDirectoryAddrOfFunctions);
	LPWORD pImageExportDirectoryAddrOfNameOrdinalsIndex = (LPWORD)(pFileBuffer + pImageExportDirectoryAddrOfNameOrdinals);
	LPDWORD pImageExportDirectoryAddrOfNamesIndex = (LPDWORD)(pFileBuffer + pImageExportDirectoryAddrOfNames);

	for (size_t i = 0; i < pImageExportDirectoryAddrOfFunctions; i++)
	{
		PDWORD funcAddr = (PDWORD)(pImageExportDirectoryAddrOfFunctionsIndex[i + pImageExportDirectoryBase] + pFileBuffer);
		PCHAR localName = pFileBuffer + pImageExportDirectoryAddrOfNamesIndex[i + pImageExportDirectoryBase];

		printf("pImageExportDirectoryAddrOfNames->matched functionName:%s   %08x   \n", localName, funcAddr);
	}



	return NULL;
}


PCHAR fileBuffer2ImageBuffer(PCHAR pFileBuffer) {

	char* imageNtHeader = pFileBuffer + *(PDWORD)(pFileBuffer + 0x3c);
	char* imageFileHeader = imageNtHeader + 0x4;
	char* imageOptionHeader = imageFileHeader + IMAGE_SIZEOF_FILE_HEADER;
	DWORD  sizeOfHeaders = *(PDWORD)(imageOptionHeader + 0x3c);
	DWORD  sizeOfImage = *(PDWORD)(imageOptionHeader + 0x38);

	WORD  numberOfSections = *(PWORD)(imageFileHeader + 0x2);
	WORD  sizeOfOptional = *(PWORD)(imageFileHeader + 0x10);

	PCHAR pImageBuffer = (PCHAR)VirtualAlloc(NULL, sizeOfImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!pImageBuffer)
	{
		return NULL;
	}
	char* imageSectionHeader = imageOptionHeader + sizeOfOptional;
	memset(pImageBuffer, 0, sizeOfImage);

	memcpy(pImageBuffer, pFileBuffer, sizeOfHeaders);
	//拷贝头加节表数据
	for (int i = 0; i < numberOfSections; i++)
	{
		DWORD  pointToRawData = *(PDWORD)(imageSectionHeader + 0x14);
		DWORD  virtualAddress = *(PDWORD)(imageSectionHeader + 0xc);
		DWORD  sizeOfRawData = *(PDWORD)(imageSectionHeader + 0x10);
		memcpy(pImageBuffer + virtualAddress, pFileBuffer + pointToRawData, sizeOfRawData);
		imageSectionHeader += IMAGE_SIZEOF_SECTION_HEADER;
	}
	return pImageBuffer;



}

PCHAR pImageBuffer2fileBuffer(PCHAR pImageBuffer, int* fileSize) {

	char* imageNtHeader = pImageBuffer + *(PDWORD)(pImageBuffer + 0x3c);
	char* imageFileHeader = imageNtHeader + 0x4;
	char* imageOptionHeader = imageFileHeader + IMAGE_SIZEOF_FILE_HEADER;
	DWORD  sizeOfHeaders = *(PDWORD)(imageOptionHeader + 0x3c);
	DWORD  sizeOfImage = *(PDWORD)(imageOptionHeader + 0x38);
	WORD  numberOfSections = *(PWORD)(imageFileHeader + 0x2);
	WORD  sizeOfOptional = *(PWORD)(imageFileHeader + 0x10);
	DWORD totalSizeOfImage = sizeOfHeaders;
	char* imageSectionHeader = imageOptionHeader + sizeOfOptional;
	for (int i = 0; i < numberOfSections; i++)
	{
		DWORD  sizeOfRawData = *(PDWORD)(imageSectionHeader + 0x10);
		totalSizeOfImage += sizeOfRawData;
		imageSectionHeader += IMAGE_SIZEOF_SECTION_HEADER;
	}
	PCHAR pfileBuffer = (PCHAR)malloc(totalSizeOfImage);
	if (!pfileBuffer)
	{
		return 0;
	}
	memset(pfileBuffer, 0, totalSizeOfImage);
	memcpy(pfileBuffer, pImageBuffer, sizeOfHeaders);
	imageSectionHeader = imageOptionHeader + sizeOfOptional;
	for (int i = 0; i < numberOfSections; i++)
	{
		DWORD  pointToRawData = *(PDWORD)(imageSectionHeader + 0x14);
		DWORD  virtualAddress = *(PDWORD)(imageSectionHeader + 0xc);
		DWORD  sizeOfRawData = *(PDWORD)(imageSectionHeader + 0x10);
		memcpy(pfileBuffer + pointToRawData, pImageBuffer + virtualAddress, sizeOfRawData);
		imageSectionHeader += IMAGE_SIZEOF_SECTION_HEADER;
	}
	if (fileSize != NULL)
	{
		*fileSize = totalSizeOfImage;
	}
	return pfileBuffer;

}



PCHAR pImageBuffer2fileBuffer_II(PCHAR pImageBuffer, int* fileSize) {


	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(pImageBuffer + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((PCHAR)pNTHeader + 4);
	PIMAGE_OPTIONAL_HEADER pOptionalHeader = (PIMAGE_OPTIONAL_HEADER)((PCHAR)pFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((PCHAR)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	DWORD dwFileSize = pOptionalHeader->SizeOfHeaders;
	for (size_t i = 0; i < pFileHeader->NumberOfSections; i++)
	{
		dwFileSize += pSectionHeader[i].SizeOfRawData;
	}
	PCHAR pFileBuffer = (PCHAR)malloc(dwFileSize);
	if (!pFileBuffer)
	{
		printf("pFileBuffer malloc fail");
		return NULL;
	}
	memset(pFileBuffer, 0, dwFileSize);
	memcpy(pFileBuffer, pImageBuffer, pOptionalHeader->SizeOfHeaders);
	for (size_t i = 0; i < pFileHeader->NumberOfSections; i++)
	{
		memcpy(pFileBuffer + pSectionHeader[i].PointerToRawData,
			pImageBuffer + pSectionHeader[i].VirtualAddress,
			pSectionHeader[i].SizeOfRawData
		);
	}

	if (fileSize != NULL)
	{
		*fileSize = dwFileSize;
	}
	return pFileBuffer;

}

DWORD rav2Fov(char* pFileBuffer, DWORD dwRav) {
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(pFileBuffer + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((PCHAR)pNTHeader + 4);
	PIMAGE_OPTIONAL_HEADER pOptionalHeader = (PIMAGE_OPTIONAL_HEADER)((PCHAR)pFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((PCHAR)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	if (dwRav == 0x0)
	{
		return 0;
	}
	if (dwRav > pOptionalHeader->ImageBase)
	{
		dwRav = dwRav - pOptionalHeader->ImageBase;
	}
	for (size_t i = 0; i < pFileHeader->NumberOfSections; i++)
	{
		if (dwRav >= pSectionHeader[i].VirtualAddress && (dwRav < pSectionHeader[i].VirtualAddress + pSectionHeader[i].Misc.VirtualSize))
		{
			return dwRav - pSectionHeader[i].VirtualAddress + pSectionHeader[i].PointerToRawData;
		}
	}
	return 0;

}

//【名词解释】：
//·基地址(ImageBase).
//·虚拟地址(Virtual Address, VA).
//·相对虚拟地址(Relative Virtual Address, RVA).
//·文件偏移地址(File Offset) 又称 物理地址(RAW Offset).
//
//【计算公式】：
//·相对虚拟地址(RVA) = 虚拟地址(VA) - 基地址(ImageBase).
//·虚拟地址(VA) = 相对虚拟地址(RVA) + 基地址(ImageBase).
//·文件偏移(File Offset) = 相对虚拟地址(RVA) - 该地址所在节的虚拟偏移(Virtual Offset) + 该地址所在节的物理偏移(Raw Offset).
//·相对虚拟地址(RVA) = 文件偏移(File Offset) - 该地址所在节的物理偏移(Raw Offset) + 该地址所在节的虚拟偏移(Virtual Offset).
DWORD fov2Rav(char* pFileBuffer, DWORD dwFov) {
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)(pFileBuffer + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((PCHAR)pNTHeader + 4);
	PIMAGE_OPTIONAL_HEADER pOptionalHeader = (PIMAGE_OPTIONAL_HEADER)((PCHAR)pFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((PCHAR)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);

	for (size_t i = 0; i < pFileHeader->NumberOfSections; i++)
	{
		if (dwFov >= pSectionHeader[i].PointerToRawData && (dwFov < pSectionHeader[i].PointerToRawData + pSectionHeader[i].SizeOfRawData))
		{
			return dwFov - pSectionHeader[i].PointerToRawData + pSectionHeader[i].VirtualAddress + pOptionalHeader->ImageBase;
		}
	}

}


PCHAR LoadMyLibrary(PCHAR dllPath, PDWORD exportTableRav) {
	int  fileSize = 0;
	int  is64Bit = 0;
	PCHAR  pFileBuffer = getpFileBuffer(dllPath, &fileSize, &is64Bit);
	if (pFileBuffer == NULL)
	{
		cout << "pFileBuffer failed " << get_last_error(0) << endl;
		return 0;
	}
	PCHAR pImageBuffer = fileBuffer2ImageBuffer(pFileBuffer);
	chFree(pFileBuffer);
	if (pImageBuffer == NULL)
	{
		cout << "pImageBuffer failed " << get_last_error(0) << endl;
		return 0;
	}
	relocationRepair(pImageBuffer);
	BOOL isok = RepairImportTable(pImageBuffer);
	if (!isok)
	{
		cout << "RepairImportTable failed " << get_last_error(0) << endl;
		return 0;
	}

	clearPE(pImageBuffer, exportTableRav);


	return pImageBuffer;

}