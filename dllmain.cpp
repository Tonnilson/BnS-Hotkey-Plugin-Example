#include "pch.h"

BOOL APIENTRY DllMain(HMODULE hModule,
  DWORD ul_reason_for_call,
  LPVOID lpReserved)
{
  if ( ul_reason_for_call == DLL_PROCESS_ATTACH )
    (VOID)DisableThreadLibraryCalls(hModule);
  return TRUE;
}