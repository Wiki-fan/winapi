#pragma once
#include <Windows.h>

const int N_PARTS = 4;
const WCHAR* INPUT_FILE_MAPPING_NAME = L"InputFileMapping";
const WCHAR* OUTPUT_FILE_MAPPING_NAME = L"OutputFileMapping";
const WCHAR* TASK_PREPARED = L"Global\TaskPrepared";
const WCHAR* TASK_DONE = L"Global\TaskDone";
const WCHAR* END_OF_WORK = L"Global\EndOfWork";

SECURITY_ATTRIBUTES sa = { sizeof( SECURITY_ATTRIBUTES ), NULL, true };
