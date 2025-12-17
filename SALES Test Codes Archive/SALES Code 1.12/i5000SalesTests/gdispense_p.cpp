#include <p2c/p2c.h>

#include "../../../u/src/DebugWriter.h"
#include "../../../vth/src/GeneralException.h"
#include "../../../vth/src/agi5000ftp/FtpException.h"
#include "../termio_p.h"
#include "../parms_p.h"
#include "../execsup_p.h"
#include "../autoadjust/libautoadjshared.h"
#include "../autoadjust/gdispense_p.h"
#include "../autoadjust/mt4250_p.h"
#include "../autoadjust/st4250_p.h"
#include "../autoadjust/mt4500.h"
#include "../autoadjust/st4500.h"
#include "../autoadjust/mt4750.h"
#include "../autoadjust/st4750.h"

_LIBAUTOADJSHARED_API_ bool add_gtest_to_task_list(int test_number, int mapped_module_number, int slot_number, int *module_slot_list)
{
  dbgout << "Scheduling test number " << test_number;
  dbgout << " on module " << mapped_module_number;
  dbgout << " slot " << slot_number << endl;
#ifdef _DEBUG
  if (has_mslist(test_number))
  {
    dbgout << " module_slot_list";

    for (unsigned int mlsi = 0; mlsi < 96; ++mlsi)
    {
      dbgout << " [" << mlsi << "] " << module_slot_list[mlsi];

      if (mlsi < 95)
      {
        dbgout << ',';

        if ((mlsi % 15) == 0)
        {
          dbgout << endl;
          dbgout << "                 ";
        }
      }
    }

    dbgout << endl;
  }
  else if ((test_number == 1159) ||
           (test_number == 1162) ||
           (test_number == 1165) ||
           (test_number == 1182) ||
           (test_number == 1192) ||
           (test_number == 14539))
  {
    dbgout << " slot_list";

    for (unsigned int mlsi = 0; mlsi < 22; ++mlsi)
    {
      dbgout << " [" << mlsi << "] " << module_slot_list[mlsi];

      if (mlsi < 21)
      {
        dbgout << ',';

        if ((mlsi % 15) == 0)
        {
          dbgout << endl;
          dbgout << "          ";
        }
      }
    }

    dbgout << endl;
  }
#endif

  try
  {
    if (save_lists_parm)
      wr_test_to_exec_file(test_number, mapped_module_number, slot_number, module_slot_list);

    switch (test_number)
    {
      case 4250: t4250(mapped_module_number); return true;
      case 4251: t4251(mapped_module_number); return true;
      case 4255: t4255(mapped_module_number); return true;
      case 4260: t4260(mapped_module_number); return true;
      case 4275: t4275(mapped_module_number); return true;
      case 4280: t4280(mapped_module_number); return true;
      case 4285: t4285(mapped_module_number); return true;
      case 4290: t4290(mapped_module_number); return true;
      case 4302: t4302(mapped_module_number); return true;
      case 4305: t4305(mapped_module_number); return true;
      case 4319: t4319(mapped_module_number, slot_number); return true;
      case 4329: t4329(mapped_module_number, slot_number); return true;
      case 4339: t4319(mapped_module_number, slot_number); return true;
      case 4348: t4348(mapped_module_number); return true;
      case 4349: t4349(mapped_module_number); return true;
      case 4350: t4350(mapped_module_number, slot_number); return true;
      case 4352: t4352(mapped_module_number, slot_number); return true;
      case 4353: t4353(mapped_module_number, slot_number); return true;
      case 4355: t4355(mapped_module_number, slot_number); return true;
      case 4360: t4360(mapped_module_number, slot_number); return true;
      case 4362: t4362(mapped_module_number, slot_number); return true;
      case 4363: t4363(mapped_module_number, slot_number); return true;
      case 4364: t4364(mapped_module_number); return true;
      case 4365: t4365(mapped_module_number, slot_number); return true;
      case 4370: t4370(mapped_module_number, slot_number); return true;
      case 4372: t4372(mapped_module_number, slot_number); return true;
      case 4373: t4373(mapped_module_number, slot_number); return true;
      case 4375: t4375(mapped_module_number, slot_number); return true;
      case 4390: t4390(mapped_module_number); return true;
      case 4391: t4391(mapped_module_number); return true;
      case 4395: t4395(mapped_module_number); return true;
      case 4396: t4396(mapped_module_number); return true;
      case 4397: t4397(mapped_module_number); return true;
      case 4398: t4398(mapped_module_number); return true;
      case 4419: t4419(mapped_module_number, slot_number); return true;
      case 4429: t4429(mapped_module_number, slot_number); return true;
      case 4495: t4495(); return true;
      case 4496: t4496(); return true;
      case 4499: t4499(mapped_module_number); return true;
      case 4500: t4500(mapped_module_number); return true;
      case 4501: t4501(mapped_module_number); return true;
      case 4502: t4502(mapped_module_number); return true;
      case 4503: t4503(mapped_module_number); return true;
      case 4504: t4504(mapped_module_number); return true;
      case 4505: t4505(mapped_module_number); return true;
      case 4506: t4506(mapped_module_number); return true;
      case 4508: t4508(mapped_module_number); return true;
      case 4520: t4520(mapped_module_number); return true;
      case 4530: t4530(mapped_module_number); return true;
      case 4536: t4536(mapped_module_number); return true;
      case 4537: t4537(mapped_module_number); return true;
      case 4750: t4750(mapped_module_number); return true;
      case 4751: t4751(); return true;
      case 4753: t4753(mapped_module_number); return true;
      case 4762: t4762(module_slot_list); return true;
      case 4763: t4763(module_slot_list); return true;
      case 4764: t4764(module_slot_list); return true;
      case 4800: t4800(); return true;
      case 4801: t4801(); return true;
      case 4802: t4802(); return true;

      default: return false;
    }
  }
  catch (FtpException & e)
  {
    print_line(EXEC_FILE_NUMBER, e.getErrorMsg().c_str());
    print_line(EXEC_FILE_NUMBER, "Test aborted.");
    complete_tests("Error", false);
    _Escape(ABORT_NUMBER);
  }
  catch (GeneralException & e)
  {
    print_line(EXEC_FILE_NUMBER, e.msg().c_str());
    print_line(EXEC_FILE_NUMBER, "Test aborted.");
    complete_tests("Error", false);
    _Escape(ABORT_NUMBER);
  }
  catch (...)
  {
    print_line(EXEC_FILE_NUMBER, "Unknown exception encountered, test aborted.");
    dbgout << __FILE__ << ": " << __LINE__ << ": Unknown exception encountered." << endl;
    complete_tests("Error", false);
    _Escape(ABORT_NUMBER);
  }

  return false;
}
