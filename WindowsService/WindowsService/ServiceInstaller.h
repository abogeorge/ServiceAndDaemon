#pragma once
class ServiceInstaller
{
public:
	ServiceInstaller();
	~ServiceInstaller();
	void installService(PWSTR serviceName,
		PWSTR displayName,
		DWORD startType,
		PWSTR dependencies,
		PWSTR account,
		PWSTR password);
	void uninstallService(PWSTR serviceName);
};