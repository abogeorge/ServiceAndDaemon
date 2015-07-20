#include "stdafx.h"
#include "ServiceBase.h"

/// Initialize the singleton service instance.
ServiceBase *ServiceBase::singletonService = NULL;

/// function Run registers the exe for a service after calling scmissues a start command
/// param service: refrence to ServiceBase object
/// return True if succeded
BOOL ServiceBase::Run(ServiceBase &service)
{
	singletonService = &service;

	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{ service.name, ServiceMain },
		{ NULL, NULL }
	};

	return StartServiceCtrlDispatcher(serviceTable);
}


/// function ServiceMain entry point for the service, registers the handler function
/// and start the service
/// param dwArgc: number of command line arguments
/// param lpszArgv: array of command line arguments
void WINAPI ServiceBase::ServiceMain(DWORD dwArgc, PWSTR *pszArgv)
{
	assert(singletonService != NULL);

	// Register the handler function for the service
	singletonService->statusHandle = RegisterServiceCtrlHandler(
		singletonService->name, ServiceCtrlHandler);
	if (singletonService->statusHandle == NULL)
	{
		throw GetLastError();
	}

	// Start the service.
	singletonService->Start(dwArgc, pszArgv);
}

/// function ServiceCtrlHandler is called by the scm when a control code is sent to the service
/// param ctrl: control code
void WINAPI ServiceBase::ServiceCtrlHandler(DWORD ctrl)
{
	switch (ctrl)
	{
	case SERVICE_CONTROL_STOP: singletonService->Stop(); break;
	case SERVICE_CONTROL_PAUSE: singletonService->Pause(); break;
	case SERVICE_CONTROL_CONTINUE: singletonService->Continue(); break;
	case SERVICE_CONTROL_SHUTDOWN: singletonService->Shutdown(); break;
	case SERVICE_CONTROL_INTERROGATE: break;
	default: break;
	}
}

ServiceBase::ServiceBase(PWSTR serviceName,
	BOOL stopabble,
	BOOL closeable,
	BOOL pauseable)
{

	// Service name must be a valid string and cannot be NULL.
	if (serviceName == NULL)
	{
		name = L"";
	}
	else
	{
		name = serviceName;
	}

	statusHandle = NULL;
	// The service runs in its own process.
	status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	// The service is starting.
	status.dwCurrentState = SERVICE_START_PENDING;

	// The accepted commands of the service.
	DWORD dwControlsAccepted = 0;
	if (closeable)
		dwControlsAccepted |= SERVICE_ACCEPT_STOP;
	if (closeable)
		dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
	if (pauseable)
		dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
	status.dwControlsAccepted = dwControlsAccepted;

	status.dwWin32ExitCode = NO_ERROR;
	status.dwServiceSpecificExitCode = 0;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;

}


ServiceBase::~ServiceBase()
{
}


/// function Start starts the service
/// param dwArgc: number of command line arguments
/// param lpszArgv: array of command line arguments
void ServiceBase::Start(DWORD dwArgc, PWSTR *pszArgv)
{
	try
	{
		// Tell SCM that the service is starting.
		SetServiceStatus(SERVICE_START_PENDING);

		// Perform service-specific initialization.
		OnStart(dwArgc, pszArgv);

		// Tell SCM that the service is started.
		SetServiceStatus(SERVICE_RUNNING);
	}
	catch (DWORD dwError)
	{
		// Set the service status to be stopped.
		SetServiceStatus(SERVICE_STOPPED, dwError);
	}
	catch (...)
	{
		// Set the service status to be stopped.
		SetServiceStatus(SERVICE_STOPPED);
	}
}


//
//   FUNCTION: CServiceBase::OnStart(DWORD, PWSTR *)
//
//   PURPOSE: When implemented in a derived class, executes when a Start 
//   command is sent to the service by the SCM or when the operating system 
//   starts (for a service that starts automatically). Specifies actions to 
//   take when the service starts. Be sure to periodically call 
//   CServiceBase::SetServiceStatus() with SERVICE_START_PENDING if the 
//   procedure is going to take long time. You may also consider spawning a 
//   new thread in OnStart to perform time-consuming initialization tasks.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//
void ServiceBase::OnStart(DWORD dwArgc, PWSTR *pszArgv)
{

}


//
//   FUNCTION: CServiceBase::Stop()
//
//   PURPOSE: The function stops the service. It calls the OnStop virtual 
//   function in which you can specify the actions to take when the service 
//   stops. If an error occurs, the error will be logged in the Application 
//   event log, and the service will be restored to the original state.
//
void ServiceBase::Stop()
{
	DWORD dwOriginalState = status.dwCurrentState;
	try
	{
		// Tell SCM that the service is stopping.
		SetServiceStatus(SERVICE_STOP_PENDING);

		// Perform service-specific stop operations.
		OnStop();

		// Tell SCM that the service is stopped.
		SetServiceStatus(SERVICE_STOPPED);
	}
	catch (DWORD dwError)
	{
		// Set the orginal service status.
		SetServiceStatus(dwOriginalState);
	}
	catch (...)
	{
		// Set the orginal service status.
		SetServiceStatus(dwOriginalState);
	}
}


//
//   FUNCTION: CServiceBase::OnStop()
//
//   PURPOSE: When implemented in a derived class, executes when a Stop 
//   command is sent to the service by the SCM. Specifies actions to take 
//   when a service stops running. Be sure to periodically call 
//   CServiceBase::SetServiceStatus() with SERVICE_STOP_PENDING if the 
//   procedure is going to take long time. 
//
void ServiceBase::OnStop()
{
}


//
//   FUNCTION: CServiceBase::Pause()
//
//   PURPOSE: The function pauses the service if the service supports pause 
//   and continue. It calls the OnPause virtual function in which you can 
//   specify the actions to take when the service pauses. If an error occurs, 
//   the error will be logged in the Application event log, and the service 
//   will become running.
//
void ServiceBase::Pause()
{
	try
	{
		// Tell SCM that the service is pausing.
		SetServiceStatus(SERVICE_PAUSE_PENDING);

		// Perform service-specific pause operations.
		OnPause();

		// Tell SCM that the service is paused.
		SetServiceStatus(SERVICE_PAUSED);
	}
	catch (DWORD dwError)
	{

		// Tell SCM that the service is still running.
		SetServiceStatus(SERVICE_RUNNING);
	}
	catch (...)
	{
		// Tell SCM that the service is still running.
		SetServiceStatus(SERVICE_RUNNING);
	}
}


//
//   FUNCTION: CServiceBase::OnPause()
//
//   PURPOSE: When implemented in a derived class, executes when a Pause 
//   command is sent to the service by the SCM. Specifies actions to take 
//   when a service pauses.
//
void ServiceBase::OnPause()
{
}


//
//   FUNCTION: CServiceBase::Continue()
//
//   PURPOSE: The function resumes normal functioning after being paused if
//   the service supports pause and continue. It calls the OnContinue virtual 
//   function in which you can specify the actions to take when the service 
//   continues. If an error occurs, the error will be logged in the 
//   Application event log, and the service will still be paused.
//
void ServiceBase::Continue()
{
	try
	{
		// Tell SCM that the service is resuming.
		SetServiceStatus(SERVICE_CONTINUE_PENDING);

		// Perform service-specific continue operations.
		OnContinue();

		// Tell SCM that the service is running.
		SetServiceStatus(SERVICE_RUNNING);
	}
	catch (DWORD dwError)
	{
		// Tell SCM that the service is still paused.
		SetServiceStatus(SERVICE_PAUSED);
	}
	catch (...)
	{
		// Tell SCM that the service is still paused.
		SetServiceStatus(SERVICE_PAUSED);
	}
}


//
//   FUNCTION: CServiceBase::OnContinue()
//
//   PURPOSE: When implemented in a derived class, OnContinue runs when a 
//   Continue command is sent to the service by the SCM. Specifies actions to 
//   take when a service resumes normal functioning after being paused.
//
void ServiceBase::OnContinue()
{
}


//
//   FUNCTION: CServiceBase::Shutdown()
//
//   PURPOSE: The function executes when the system is shutting down. It 
//   calls the OnShutdown virtual function in which you can specify what 
//   should occur immediately prior to the system shutting down. If an error 
//   occurs, the error will be logged in the Application event log.
//
void ServiceBase::Shutdown()
{
	try
	{
		// Perform service-specific shutdown operations.
		OnShutdown();

		// Tell SCM that the service is stopped.
		SetServiceStatus(SERVICE_STOPPED);
	}
	catch (DWORD dwError)
	{
	}
	catch (...)
	{

	}
}


//
//   FUNCTION: CServiceBase::OnShutdown()
//
//   PURPOSE: When implemented in a derived class, executes when the system 
//   is shutting down. Specifies what should occur immediately prior to the 
//   system shutting down.
//
void ServiceBase::OnShutdown()
{
}

#pragma endregion


#pragma region Helper Functions

//
//   FUNCTION: CServiceBase::SetServiceStatus(DWORD, DWORD, DWORD)
//
//   PURPOSE: The function sets the service status and reports the status to 
//   the SCM.
//
//   PARAMETERS:
//   * dwCurrentState - the state of the service
//   * dwWin32ExitCode - error code to report
//   * dwWaitHint - estimated time for pending operation, in milliseconds
//
void ServiceBase::SetServiceStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure of the service.

	status.dwCurrentState = dwCurrentState;
	status.dwWin32ExitCode = dwWin32ExitCode;
	status.dwWaitHint = dwWaitHint;

	status.dwCheckPoint =
		((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED)) ?
		0 : dwCheckPoint++;

	// Report the status of the service to the SCM.
	::SetServiceStatus(statusHandle, &status);
}