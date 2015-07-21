
#include "stdafx.h"

/// structure contains status information for a service
SERVICE_STATUS        g_ServiceStatus = { 0 };
/// handle to the status information structure for the current service.
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
/// handle for a stop event
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
VOID WINAPI ServiceCtrlHandler(DWORD);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);

#define SERVICE_NAME  _T("LoggerWindowsService")  

int _tmain(int argc, TCHAR *argv[])
{
	/// Structure that specifies the ServiceMain function for a service that can run in the calling process
	/// It is used by the StartServiceCtrlDispatcher function
	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};

	/// Connects the main thread of a service process to the service control manager, 
	/// which causes the thread to be the service control dispatcher thread for the calling process
	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
	{
		return GetLastError();
	}

	return 0;
}

/// The entry point for a service.
VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{
	DWORD Status = E_FAIL;
	/// Registers a function to handle service control requests.
	g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

	if (g_StatusHandle == NULL)
	{
		return;
	}

	/// Informing the smc that the service is starting
	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	/// Creating the stop event
	g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	/// if SetServiceStatus returned error
	if (g_ServiceStopEvent == NULL)
	{
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = GetLastError();
		g_ServiceStatus.dwCheckPoint = 1;
		SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
		return;
	}

	/// Informing the smc that the service started
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;
	SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

	/// Starting the  worker thread
	HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	// Waiting for worker thread exit signal
	WaitForSingleObject(hThread, INFINITE);

	/// Cleanup
	CloseHandle(g_ServiceStopEvent);

	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 3;
	SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

}

/// Controller for the service, managing the stop operation
VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
	switch (CtrlCode)
	{
	case SERVICE_CONTROL_STOP:
	{
		if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
			break;

		/// Stopping the service
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 4;
		SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

		/// Signalling the worker thread to shut down
		SetEvent(g_ServiceStopEvent);

		break;
	}

	default:
	{
		break;
	}
	}

}

/// ServiceWorkerThread function starts writing the timestamp to a file every 3 seconds
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
	FileWorker file;
	file.writeInit();

	///  Checking if the services has issued a stop request
	while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
	{
		Sleep(3000);
		file.writeContent();
	}

	file.wirteEnd();
	return ERROR_SUCCESS;
}