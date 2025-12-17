#include <windows.h>

#include "../../../vth/src/vth_util.h"
#include "../../../vmc/src/vmcconf.h"

bool APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
      init_vth_for_raptor();
      vmcSetDgnMode(true);
      break;

    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;

    default:
      break;
  }

  return true;
}
