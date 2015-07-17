/// Functions for installing and uninstalling a service

#include "stdafx.h"
#include "ServiceInstaller.h"


ServiceInstaller::ServiceInstaller()
{
}


ServiceInstaller::~ServiceInstaller()
{
}


/// function InstallService installs the application as a service to the locam scm
/// param serviceName: name of the service
/// param displayName: display name of the service
/// param startType: service start option
/// param dependencies: array of services or load ordering groups that must be started before the services
/// param account: name of the account under which the service runs
/// param password: password of the account
void ServiceInstaller::installService(PWSTR serviceName,
	PWSTR displayName,
	DWORD startType,
	PWSTR dependencies,
	PWSTR account,
	PWSTR password)
{
	wchar_t path[MAX_PATH];
	SC_HANDLE sCManager = NULL;
	SC_HANDLE service = NULL;

	if (GetModuleFileName(NULL, path, ARRAYSIZE(path)) == 0)
	{
		wprintf(L"GetModuleFileName failed w/err 0x%08lx\n", GetLastError());
		return;
	}

	/// Open the SCM
	sCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT |
		SC_MANAGER_CREATE_SERVICE);
	if (sCManager == NULL)
	{
		wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
		CloseServiceHandle(sCManager);
		return;
	}

	/// Install the service into SCM
	service = CreateService(
		sCManager,                   // SCManager database
		serviceName,                 // Name of service
		displayName,                 // Name to display
		SERVICE_QUERY_STATUS,           // Desired access
		SERVICE_WIN32_OWN_PROCESS,      // Service type
		startType,                    // Service start type
		SERVICE_ERROR_NORMAL,           // Error control type
		path,                         // Service's binary
		NULL,                           // No load ordering group
		NULL,                           // No tag identifier
		dependencies,                // Dependencies
		account,                     // Service running account
		password                     // Password of the account
		);
	if (service == NULL)
	{
		wprintf(L"CreateService failed w/err 0x%08lx\n", GetLastError());
		CloseServiceHandle(sCManager);
		CloseServiceHandle(service);
		return;
	}

	wprintf(L"%s is installed.\n", serviceName);
	CloseServiceHandle(sCManager);
	CloseServiceHandle(service);
}


/// function UninstallService performs the stop and the removing of the service from scm
void ServiceInstaller::uninstallService(PWSTR serviceName)
{
	SC_HANDLE sCManager = NULL;
	SC_HANDLE service = NULL;
	SERVICE_STATUS svcStatus = {};

	/// Open the SCM
	sCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (sCManager == NULL)
	{
		wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
		CloseServiceHandle(sCManager);
		return;
	}

	/// Open the service
	service = OpenService(sCManager, serviceName, SERVICE_STOP |
		SERVICE_QUERY_STATUS | DELETE);
	if (service == NULL)
	{
		wprintf(L"OpenService failed w/err 0x%08lx\n", GetLastError());
		CloseServiceHandle(sCManager);
		CloseServiceHandle(service);
		return;
	}

	/// Stopping the service
	if (ControlService(service, SERVICE_CONTROL_STOP, &svcStatus))
	{
		wprintf(L"Stopping %s.", serviceName);
		Sleep(1000);

		while (QueryServiceStatus(service, &svcStatus))
		{
			if (svcStatus.dwCurrentState == SERVICE_STOP_PENDING)
			{
				wprintf(L".");
				Sleep(1000);
			}
			else break;
		}

		if (svcStatus.dwCurrentState == SERVICE_STOPPED)
		{
			wprintf(L"\n%s is stopped.\n", serviceName);
		}
		else
		{
			wprintf(L"\n%s failed to stop.\n", serviceName);
		}
	}

	/// Removing the service
	if (!DeleteService(service))
	{
		wprintf(L"DeleteService failed w/err 0x%08lx\n", GetLastError());
		CloseServiceHandle(sCManager);
		CloseServiceHandle(service);
		return;
	}

	wprintf(L"%s is removed.\n", serviceName);

	CloseServiceHandle(sCManager);
	CloseServiceHandle(service);

}