#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <cstdlib>
#include <cstdio>

#include "Rad/arg.h"
#include "Rad/RadTextFile.h"
#include "Rad/WinError.h"

#if _DEBUG
void __cdecl CrtDebugEnd()
{
    _CrtMemState state;
    _CrtMemCheckpoint(&state);

    if (state.lCounts[_CLIENT_BLOCK] != 0 ||
        state.lCounts[_NORMAL_BLOCK] != 0 ||
        (_crtDbgFlag & _CRTDBG_CHECK_CRT_DF && state.lCounts[_CRT_BLOCK] != 0))
    {
        _RPT0(_CRT_ERROR, "Detected memory leaks!\n");
    }
}
#endif

int _tmain(const int argc, const TCHAR* const argv[])
{
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#if _DEBUG
    atexit(CrtDebugEnd);
#endif

    arginit(argc, argv, TEXT("convert text file between code pages"));
    int arg = 1;
    argoptional();
    LPCTSTR lpInFileName = argnum(arg++, TEXT("-"), TEXT("ifile"), TEXT("input file name. '-' for stdin (default)."));
    const UINT inCodePage = argvalueint(TEXT("/icp"), GetConsoleCP(), TEXT("icp"), TEXT("\tinput code page. default is console code page."));
    LPCTSTR lpOutFileName = argnum(arg++, TEXT("-"), TEXT("ofile"), TEXT("output file name. '-' for stdout (default)."));
    const UINT outCodePage = argvalueint(TEXT("/ocp"), GetConsoleOutputCP(), TEXT("ocp"), TEXT("\toutput code page. default is console code page."));
    if (!argcleanup())
        return EXIT_FAILURE;
    if (argusage())
    {
        _tprintf(TEXT("\n"));
        _tprintf(TEXT("code page:\n"));
        _tprintf(TEXT("\t0      system default\n"));
        _tprintf(TEXT("\t65001  utf8\n"));
        _tprintf(TEXT("\t1200   utf16 little endian\n"));
        _tprintf(TEXT("\t12001  utf16 big endian\n"));
        return EXIT_SUCCESS;
    }

    if (IsWide32(inCodePage) || IsWide32(outCodePage))
    {
        _ftprintf(stderr, TEXT("ERROR: UTF32 code pages are not supported.\n"));
        return EXIT_FAILURE;
    }

    RadITextFile ifile(lstrcmp(lpInFileName, TEXT("-")) != 0
        ? RadITextFile(lpInFileName, inCodePage)
        : RadITextFile::StdIn(inCodePage));
    if (!ifile.Valid())
    {
        _ftprintf(stderr, TEXT("ERROR: %s\n"), WinError::getMessage(GetLastError(), nullptr, TEXT("Opening input file")).c_str());
        return EXIT_FAILURE;
    }

    if (IsWide32(ifile.GetCodePage()))
    {
        _ftprintf(stderr, TEXT("ERROR: UTF32 code pages are not supported.\n"));
        return EXIT_FAILURE;
    }

    RadOTextFile ofile(lstrcmp(lpOutFileName, TEXT("-")) != 0
        ? RadOTextFile(lpOutFileName, outCodePage, true)
        : RadOTextFile::StdOut(outCodePage));
    if (!ofile.Valid())
    {
        _ftprintf(stderr, TEXT("ERROR: %s\n"), WinError::getMessage(GetLastError(), nullptr, TEXT("Opening output file")).c_str());
        return EXIT_FAILURE;
    }

    if (IsWide16(ofile.GetCodePage()))
    {
        std::wstring line;
        while (ifile.ReadLine(line, ofile.GetCodePage()))
        {
            ofile.Write(line, ofile.GetCodePage());
        }
    }
    else
    {
        std::string line;
        while (ifile.ReadLine(line, ofile.GetCodePage()))
        {
            ofile.Write(line, ofile.GetCodePage());
        }
    }

    return EXIT_SUCCESS;
}
