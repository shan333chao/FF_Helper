#include <Windows.h>
#pragma once


/// <summary>
/// 清除ImageDosHeader 和 Dos sub中无用的区域
/// </summary>
/// <param name="pFileBuffer"></param>
void clearPE(char* pFileBuffer, PDWORD exportTableRav);

/// <summary>
/// 获取文件字节内容
/// </summary>
/// <param name="filePath">文件路径</param>
/// <param name="fsize">out 文件字节大小</param>
/// <param name="is64">out 是否运行在64位上</param>
/// <returns></returns>
char* getpFileBuffer(char* filePath, int* fsize, int* is64);

/// <summary>
/// 内存转储文件
/// </summary>
/// <param name="outFilePath">转储的文件路径</param>
/// <param name="pFileBuffer">转储的内存字节</param>
/// <param name="fsize">字节长度</param>
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
/// 隐藏模块加载
/// </summary>
/// <param name="pDllPath">dll路径</param>
/// <param name="exportTableRav">导出表rav</param>
/// <returns>是否成功</returns>
PCHAR LoadMyLibrary(PCHAR pDllPath,  PDWORD exportTableRav);