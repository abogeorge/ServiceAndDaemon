#pragma once
class ServiceBase
{
public:
	ServiceBase(PWSTR serviceName,
		BOOL stoppable = TRUE,
		BOOL closeable = TRUE,
		BOOL pauseable = FALSE);
	~ServiceBase();
	static BOOL Run(ServiceBase &service);
	void Stop();

protected:
	virtual void OnStart(DWORD dwArgc, PWSTR *pszArgv);
	virtual void OnStop();
	virtual void OnPause();
	virtual void OnContinue();
	virtual void OnShutdown();
	void SetServiceStatus(DWORD dwCurrentState,
		DWORD dwWin32ExitCode = NO_ERROR,
		DWORD dwWaitHint = 0);

private:
	static void WINAPI ServiceMain(DWORD dwArgc, LPWSTR *lpszArgv);
	static void WINAPI ServiceCtrlHandler(DWORD ctrl);
	void Start(DWORD dwArgc, PWSTR *pszArgv);
	void Pause();
	void Continue();
	void Shutdown();
	static ServiceBase *singletonService;
	PWSTR name;
	SERVICE_STATUS status;
	SERVICE_STATUS_HANDLE statusHandle;
};

