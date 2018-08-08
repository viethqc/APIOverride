
#include "WriteLog.h"

CWriteLog::CWriteLog(void)
{
	m_hWriteLogMutex = NULL;
	m_listLogWaiting.RemoveAll();
}

CWriteLog::~CWriteLog(void)
{
	m_listLogWaiting.RemoveAll();
}

BOOL CWriteLog::InitAll(__in LPCTSTR pszLogPath)
{
	if (pszLogPath == NULL)
		return FALSE;
	if (!_tcslen(pszLogPath))
		return FALSE;

	m_sLogPath = pszLogPath;	

	// Create mutex to synchronization write log
	m_hWriteLogMutex = CreateMutex(NULL, FALSE, NULL);
	if (m_hWriteLogMutex == NULL)
		return FALSE;

	return TRUE;
}

BOOL CWriteLog::IsHaveWaitingLog()
{
	if (m_listLogWaiting.GetCount() > 0)
		return TRUE;

	// Have not log waiting to write
	return FALSE;
}

CString CWriteLog::GetWaitingLog()
{
	CString sLogWaiting;

	// Get all log is waiting to write file
	while (m_listLogWaiting.GetCount() > 0)
	{
		sLogWaiting += m_listLogWaiting.RemoveHead();
	}

	// Return all log is waiting
	return sLogWaiting;
}

void CWriteLog::SetWaitingLog(__in LPCTSTR pszWaitLog)
{
	// Validate parameter
	if (pszWaitLog == NULL)
		return;
	if (!_tcslen(pszWaitLog))
		return;

	// Add log is tail of list
	m_listLogWaiting.AddTail(pszWaitLog);
}

// Write log
void CWriteLog::WriteLog(__in LPCTSTR pszFormat, ...)
{
	va_list		va;
	CString		sData;
	TCHAR		szMsg[MAX_LOG_LEN + 10];
	CTime		ctCurrentTime = CTime::GetCurrentTime();

	// Format data input
	va_start(va, pszFormat);

#if defined(UNICODE) || defined(_UNICODE)
	// The project is compiled for Unicode
	vswprintf_s(szMsg, MAX_LOG_LEN, pszFormat, va);
#else
	vsprintf_s(szMsg, MAX_LOG_LEN, pszFormat, va);
	// The project is NOT compiled for Unicode
#endif

	//sData.Format(_T("%s\t%s\r\n"), ctCurrentTime.Format(_T("%d/%m/%Y %H:%M:%S")), szMsg);
	sData.Format(_T("%s\r\n"),szMsg);

	// Synchronization process write log operator
	WaitForSingleObject(m_hWriteLogMutex, INFINITE);

	if (IsHaveWaitingLog())
		sData += GetWaitingLog();
	else if (!WriteToFile(sData))
		SetWaitingLog(sData);

	ReleaseMutex(m_hWriteLogMutex);
}

// Write log of program
BOOL CWriteLog::WriteToFile(__in LPCTSTR pszData)
{
	// Validate parameter
	if (pszData == NULL)
		return TRUE;
	if (!_tcslen(pszData))
		return TRUE;

	HANDLE	hFile;
	DWORD	dwSize;
	DWORD	dwSizeHi;
	DWORD	dwByteWrote;
	CString	sLogFilePath;

	hFile = CreateFile(m_sLogPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
		return FALSE;

	// Check file size
	dwSize = GetFileSize(hFile, &dwSizeHi);
	//if (dwSize > MAX_FILE_LOG_SIZE || dwSizeHi > 0)
	//{
	//	CloseHandle(hFile);

	//	hFile = CreateFile(m_sLogPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//	if (hFile == INVALID_HANDLE_VALUE)
	//		return FALSE;

	//	dwSize = GetFileSize(hFile, &dwSizeHi);
	//}

	if (!dwSize)
	{
		// Define unicode file

#if defined(UNICODE) || defined(_UNICODE)
		// The project is compiled for Unicode
		WORD wHeader = 0xFEFF;
		// The 
		if (!WriteFile(hFile, &wHeader, sizeof(WORD), &dwByteWrote, NULL))
		{
			CloseHandle(hFile);
			return FALSE;
		}
#endif
		
	}

	if (SetFilePointer(hFile, 0, 0, FILE_END) == INVALID_SET_FILE_POINTER)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	// Write data in file
	if (!WriteFile(hFile, pszData, _tcslen(pszData) * sizeof(TCHAR), &dwByteWrote, NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);
	return TRUE;
}
