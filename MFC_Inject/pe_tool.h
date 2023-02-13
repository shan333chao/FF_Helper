#include <Windows.h>
#pragma once


/// <summary>
/// ���ImageDosHeader �� Dos sub�����õ�����
/// </summary>
/// <param name="pFileBuffer"></param>
void clearPE(char* pFileBuffer, PDWORD exportTableRav);

/// <summary>
/// ��ȡ�ļ��ֽ�����
/// </summary>
/// <param name="filePath">�ļ�·��</param>
/// <param name="fsize">out �ļ��ֽڴ�С</param>
/// <param name="is64">out �Ƿ�������64λ��</param>
/// <returns></returns>
char* getpFileBuffer(char* filePath, int* fsize, int* is64);

/// <summary>
/// �ڴ�ת���ļ�
/// </summary>
/// <param name="outFilePath">ת�����ļ�·��</param>
/// <param name="pFileBuffer">ת�����ڴ��ֽ�</param>
/// <param name="fsize">�ֽڳ���</param>
void Mem2File(char* outFilePath, char* pFileBuffer, int* fsize);







void chFree(char* ptr);
void shFree(short* ptr);
void nFree(int* ptr);
BOOL RepairImportTable(PCHAR pImageBuffer);

PCHAR fileBuffer2ImageBuffer(PCHAR pfileBuffer);
PCHAR pImageBuffer2fileBuffer(PCHAR pfileBuffer);

PCHAR pImageBuffer2fileBuffer_II(PCHAR pImageBuffer, int* fileSize);



DWORD rav2Fov(char* pFileBuffer, DWORD dwRav);

DWORD fov2Rav(char* pFileBuffer, DWORD dwFov);



PDWORD  getProcAddressByName(PCHAR pFileBuffer, PCHAR funcName, DWORD exportTableRav);


PDWORD  getProcAddressByOrdinal(PCHAR pFileBuffer, DWORD index, DWORD exportTableRav);

void relocationRepair(PCHAR pFileBuffer);
typedef struct _ExportFunctions
{
	WORD Ordinal;
	char Name[8];
	DWORD Addr;


}ExportFunctions, * PExportFunctions;


/// <summary>
/// ����ģ�����
/// </summary>
/// <param name="pDllPath">dll·��</param>
/// <param name="exportTableRav">������rav</param>
/// <returns>�Ƿ�ɹ�</returns>
PCHAR LoadMyLibrary(PCHAR pDllPath,  PDWORD exportTableRav);