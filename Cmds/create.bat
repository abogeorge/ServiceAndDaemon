echo off
cls
:-------------------------------------
REM  --> Check for permissions
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params = %*:"=""
    echo UAC.ShellExecute "cmd.exe", "/c %~s0 %params%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    pushd "%CD%"
    CD /D "%~dp0"
:--------------------------------------

echo --- Logger Service Installer ---
set /p servicename=Service name :
echo Options:
echo 1. Install
echo 2. Start
echo 3. Stop 
echo 4. Remove
echo 5. Exit
:CASE_init
SET /P option="Option: "
2>NUL CALL :CASE_%option% # jump to :CASE_1, :CASE_2, CASE_3, CASE_4, CASE_5
IF ERRORLEVEL 1 CALL :DEFAULT_CASE # if label doesn't exist

ECHO Done.
EXIT /B

:CASE_1

sc create %servicename% binPath= "D:\Projects\ServiceAndDaemon\Runs\WLServiceDaemon.exe"
GOTO CASE_init
:CASE_2
sc start %servicename%
GOTO CASE_init
:CASE_3
sc stop %servicename%
GOTO CASE_init
:CASE_4
sc delete %servicename%
GOTO CASE_init
:CASE_5
GOTO END_CASE
:DEFAULT_CASE
ECHO Unknown option %option%
GOTO CASE_init
:END_CASE
pause