#pragma once

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <time.h>

using namespace std;

#include "FileWorker.h"

#ifdef _WIN32
#include "targetver.h"
#include <tchar.h>
#include <string>
#include <Windows.h>
#include "WindowsService.h"
#elif linux
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include "LinuxDaemon.h"
#endif
