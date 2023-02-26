// AutoLoginConsole.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define LOGURU_WITH_STREAMS 1
#include <iostream>
#include "loguru.hpp"
#include <fstream>
#include "json.hpp"
#include <Windows.h>
#include <Shlwapi.h>
#include <atlconv.h>
using namespace std;
using json = nlohmann::json;


string get_last_error(DWORD errCode)
{
	string err("");
	if (errCode == 0) errCode = GetLastError();
	LPTSTR lpBuffer = NULL;
	if (0 == FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, //标志位，决定如何说明lpSource参数，dwFlags的低位指定如何处理换行功能在输出缓冲区，也决定最大宽度的格式化输出行,可选参数。
		NULL,//根据dwFlags标志而定。
		errCode,//请求的消息的标识符。当dwFlags标志为FORMAT_MESSAGE_FROM_STRING时会被忽略。
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),//请求的消息的语言标识符。
		(LPTSTR)&lpBuffer,//接收错误信息描述的缓冲区指针。
		0,//如果FORMAT_MESSAGE_ALLOCATE_BUFFER标志没有被指定，这个参数必须指定为输出缓冲区的大小，如果指定值为0，这个参数指定为分配给输出缓冲区的最小数。
		NULL//保存格式化信息中的插入值的一个数组。
	))
	{//失败
		char tmp[100] = { 0 };
		sprintf_s(tmp, "{未定义错误描述(%d)}", errCode);
		err = tmp;
	}
	else//成功
	{
		USES_CONVERSION;
		err = W2A(lpBuffer);
		LocalFree(lpBuffer);
	}
	return err;
}

DWORD createMyProcess(LPSTR pePath, LPSTR commandLine) {
	DWORD pid = 0;
	STARTUPINFOA start = { 0 };
	start.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi = { 0 };
	string paramStart(pePath);
	paramStart.append(commandLine);

	::CreateProcessA(NULL, (LPSTR)paramStart.c_str(), NULL, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &start, &pi);
	DWORD last_err = GetLastError();

	if (last_err)
	{
		return pid;
	}
	pid = pi.dwProcessId;
	return pid;
}


/// <summary>
/// char* 转string
/// </summary>
/// <param name="contentChar">char* 内容</param>
/// <returns>string</returns>
string charToStr(char* contentChar)
{
	string tempStr;
	for (int i = 0; contentChar[i] != '\0'; i++)
	{
		tempStr += contentChar[i];
	}
	return tempStr;
}







/// <summary>
/// 修改文件某行内容 输出：文件名 fileName  tip：1,lineNum从第一行开始 2.content需要加上换行符
/// </summary>
/// <param name="fileName">输入：文件名 fileName </param>
/// <param name="lineNum"> 行号   lineNum</param>
/// <param name="content">修改的内容 content</param>
void modifyContentInFile(char* fileName, int lineNum, char* content)
{

	ifstream in;
	char line[1024] = { '\0' };
	in.open(fileName);
	int i = 0;
	string tempStr;
	while (in.getline(line, sizeof(line)))
	{
		i++;
		if (lineNum != i)
		{
			tempStr += charToStr(line);
		}
		else
		{
			tempStr += charToStr(content);
		}
		tempStr += '\n';
	}
	in.close();
	ofstream out;
	out.open(fileName);
	out.flush();
	out << tempStr;
	out.close();
	SetLastError(0);
}
template<class... T>
std::string format(const char* fmt, const T&...t)
{
	const auto len = snprintf(nullptr, 0, fmt, t...);
	std::string r;
	r.resize(static_cast<size_t>(len) + 1);
	snprintf(&r.front(), len + 1, fmt, t...);  // Bad boy
	r.resize(static_cast<size_t>(len));

	return r;
}
int main(int argc, char* argv[])
{// Optional, but useful to time-stamp the start of the log.
// Will also detect verbosity level on command line as -v.
	loguru::init(argc, argv);

	// Put every log message in "everything.log":
	loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);

	// Only log INFO, WARNING, ERROR and FATAL to "latest_readable.log":
	loguru::add_file("latest_readable.log", loguru::Truncate, loguru::Verbosity_INFO);

	// Only show most relevant things on stderr:
	loguru::g_stderr_verbosity = 1;
	loguru::set_fatal_handler([](const loguru::Message& message) {
		throw std::runtime_error(std::string(message.prefix) + message.message);
		});

	LOG_F(INFO, "开始运行");
	system("taskkill /f /t /im Neuz.exe");
	LOG_F(INFO, "清理进程");
	Sleep(5000);
	string currentPath(argv[0]);
	currentPath = currentPath.substr(0, currentPath.find_last_of("\\") + 1);
	string jsonConfig = currentPath;
	string PROGRAM = currentPath;
	string baseINI = currentPath;
	jsonConfig.append("accounts.json");
	PROGRAM.append("Neuz.exe");
	baseINI.append("neuz.ini");
	std::ifstream f(jsonConfig.c_str());
	json data = json::parse(f);
	//USES_CONVERSION;
	for (size_t i = 0; i < data.size(); i++)
	{
		string name = data[i]["name"].get<std::string>();
		string pwd = data[i]["pwd"].get<std::string>();
		int status = data[i]["status"].get<int>();
		if (status==0)
		{
			continue;
		}
		LOG_F(INFO, "修改原始配置");
		string modifyContent;
		modifyContent = format("account \"%s\"", name.c_str());
		modifyContentInFile((PCHAR)baseINI.c_str(), 20, (PCHAR)modifyContent.c_str());
		Sleep(1000);
		DWORD pid = createMyProcess((PCHAR)PROGRAM.c_str(), (PCHAR)" cflyff BXFUATJQAUBJHOFGCD");
		if (pid == NULL)
		{
			string error_txt = get_last_error(0);
			LOG_F(ERROR, error_txt.c_str());
			LOG_F(ERROR, "进程启动失败");
			return 0;
		}
		LOG_F(INFO, name.c_str());
		LOG_F(INFO, "登录账号");
		Sleep(1000);
		// TODO: 在此添加控件通知处理程序代码
		HWND ffWnd = ::FindWindowW(NULL, L"飞飞客户端");
		while (ffWnd == NULL)
		{
			LOG_F(INFO, "等待窗口启动");
			Sleep(1000);
			ffWnd = ::FindWindowW(NULL, L"飞飞客户端");
		}
		Sleep(30000);
		::SetActiveWindow(ffWnd);
		for (size_t i = 0; i < pwd.length(); i++)
		{
			::SendMessage(ffWnd, WM_CHAR, WPARAM(pwd.at(i)), 0);
		}
		Sleep(2000);
		for (size_t i = 0; i < 3; i++)
		{
			::SendMessage(ffWnd, WM_KEYDOWN, VK_RETURN, 0);
			::SendMessage(ffWnd, WM_KEYUP, VK_RETURN, 0);
			Sleep(10000);
		}
		LOG_F(INFO, "登陆完成");
		
	}
}