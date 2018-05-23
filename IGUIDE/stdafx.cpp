
// stdafx.cpp : source file that includes just the standard includes
// IGUIDE.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

//void CustomTrace(const wchar_t* format, ...)
//{
//	const int TraceBufferSize = 1024;
//	wchar_t buffer[TraceBufferSize];
//
//	va_list argptr; va_start(argptr, format);
//	vswprintf_s(buffer, format, argptr);
//	va_end(argptr);
//
//	::OutputDebugString(buffer);
//}
//
//void CustomTrace(int dwCategory, int line, const wchar_t* format, ...)
//{
//	va_list argptr; va_start(argptr, format);
//	CustomTrace(format, argptr);
//	va_end(argptr);
//}