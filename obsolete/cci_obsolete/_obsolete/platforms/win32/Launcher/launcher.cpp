#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "nsWindowRestart.cpp"

/*
 * launcher.cpp
 * just a application launcher that execute a file
 * in a specified directory
 * 
 * usage :
 * launcher <path> <file> <followings arguments are passed to the child process>
 */


#define __BREAK__ _asm { int 3h } // break on x86 plateforms

#define PATH_SEPARATOR_CHAR '\\'
#define MAXPATHLEN MAX_PATH

// ======================================================================= 
// Main
// ======================================================================= 
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
  // Get arguments
  int    argc = __argc;
  char** argv = __argv;

  BOOL ok = WinLaunchChild(argv[2], argv[1], argc-2, argv+2);
  return !ok;
};
