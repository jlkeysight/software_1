#include <ctime>
#include <stdio.h>
#include <assert.h>
#include <p2c/p2c.h>

#include "../../../u/src/DebugWriter.h"
#include "../../../vth/src/GeneralException.h"
#include "../../../vth/src/agi5000ftp/FtpException.h"
#include "../termio_p.h"
#include "../sales_tests/libsalesshared.h"
#include "../tests/dispense_p.h"
#include "../parms_p.h"
#include "../execsup_p.h"
#include "mt16000.h"
#include "mt16250.h"
#include "mt16500.h"
#include "mt16750.h"

_LIBSALESSHARED_API_ void add_test_to_task_list(int test_number, int mapped_module_number, int slot_number, int * module_slot_list)
{
  FILE * test_time_file = 0;
  int test_time;
  int hours, minutes, seconds;

  try
  {
    if (save_lists_parm)
      wr_test_to_exec_file(test_number, mapped_module_number, slot_number, module_slot_list);

    if (test_time_file)
      test_time_file = freopen("sales.times", "a", test_time_file);
    else
      test_time_file = fopen("sales.times", "a");

    time_t test_start_time = time(0);  //record current time in seconds

    switch (test_number)
    {
      case 16000: t16000();                                  break;
      case 16001: t16001(mapped_module_number, slot_number); break;
      case 16002: t16002(mapped_module_number, slot_number); break;
      case 16003: t16003();                                  break;
      case 16004: t16004();                                  break;
      case 16005: t16005(mapped_module_number, slot_number); break;
      case 16006: t16006(mapped_module_number, slot_number); break;
      case 16010: t16010(mapped_module_number, slot_number); break;
      case 16011: t16011(mapped_module_number, slot_number); break;
	  case 16012: t16012(mapped_module_number, slot_number); break;
	  case 16032: t16032(mapped_module_number);              break;
      case 16188: t16188(mapped_module_number, slot_number); break;
      case 16189: t16189(mapped_module_number, slot_number); break;
      case 16200: t16200(mapped_module_number, slot_number); break;
      case 16201: t16201(mapped_module_number, slot_number); break;
      case 16202: t16202(mapped_module_number, slot_number); break;
      case 16203: t16203(mapped_module_number, slot_number); break;
      case 16204: t16204(mapped_module_number, slot_number); break;
      case 16205: t16205(mapped_module_number, slot_number); break;
      case 16206: t16206(mapped_module_number, slot_number); break;
      case 16207: t16207(mapped_module_number, slot_number); break;
      case 16208: t16208(mapped_module_number, slot_number); break;
      case 16209: t16209(mapped_module_number, slot_number); break;
      case 16216: t16216(mapped_module_number, slot_number); break;
      case 16217: t16217(mapped_module_number, slot_number); break;
      case 16230: t16230(mapped_module_number, slot_number); break;
      case 16231: t16231(mapped_module_number, slot_number); break;
      case 16248: t16248(mapped_module_number, slot_number); break;
      case 16249: t16249(mapped_module_number, slot_number); break;
      case 16250: t16250(mapped_module_number, slot_number); break;
      case 16252: t16252(mapped_module_number, slot_number); break;
      case 16256: t16256(mapped_module_number, slot_number); break;
      case 16260: t16260(mapped_module_number, slot_number); break;
      case 16261: t16261(mapped_module_number, slot_number); break;
      case 16265: t16265(mapped_module_number, slot_number); break;
      case 16275: t16275(mapped_module_number, slot_number); break;
      case 16297: t16297(mapped_module_number);              break;
      case 16298: t16298(mapped_module_number);              break;
      case 16299: t16299(mapped_module_number);              break;
      case 16305: t16305(mapped_module_number, slot_number); break;
      case 16306: t16306(mapped_module_number, slot_number); break;
      case 16310: t16310(mapped_module_number, slot_number); break;
      case 16315: t16315(mapped_module_number, slot_number); break;
      case 16316: t16316(mapped_module_number, slot_number); break;
      case 16317: t16317(mapped_module_number);              break;
      case 16318: t16318(mapped_module_number);              break;
      case 16320: t16320(mapped_module_number, slot_number); break;
      case 16325: t16325(mapped_module_number, slot_number); break;
      case 16327: t16327(mapped_module_number, slot_number); break;
      case 16330: t16330(mapped_module_number, slot_number); break;
      case 16331: t16331(mapped_module_number, slot_number); break;
      case 16332: t16332(mapped_module_number, slot_number); break;
      case 16333: t16333(mapped_module_number, slot_number); break;
      case 16334: t16334(mapped_module_number, slot_number); break;
      case 16335: t16335(mapped_module_number);              break;
      case 16336: t16336(mapped_module_number);              break;
      case 16337: t16337(mapped_module_number, slot_number); break;
      case 16338: t16338(mapped_module_number, slot_number); break;
      case 16340: t16340(mapped_module_number, slot_number); break;
      case 16355: t16355(mapped_module_number, slot_number); break;
      case 16360: t16360(mapped_module_number, slot_number); break;
      case 16367: t16367(mapped_module_number, slot_number); break;
      case 16370: t16370(mapped_module_number, slot_number); break;
      case 16371: t16371(mapped_module_number, slot_number); break;
      case 16380: t16380(mapped_module_number, slot_number); break;
      case 16386: t16386(mapped_module_number, slot_number); break;
      case 16387: t16387(mapped_module_number, slot_number); break;
      case 16391: t16391(mapped_module_number, slot_number); break;
      case 16392: t16392(mapped_module_number, slot_number); break;
      case 16393: t16393(mapped_module_number, slot_number); break;
      case 16394: t16394(mapped_module_number, slot_number); break;
      case 16415: t16415(mapped_module_number, slot_number); break;
      case 16416: t16416(mapped_module_number, slot_number); break;
      case 16420: t16420(mapped_module_number, slot_number); break;
      case 16427: t16427(mapped_module_number, slot_number); break;
      case 16456: t16456(mapped_module_number, slot_number); break;
      case 16465: t16465(mapped_module_number, slot_number); break;
      case 16466: t16466(mapped_module_number, slot_number); break;
      case 16470: t16470(mapped_module_number, slot_number); break;
      case 16471: t16471(mapped_module_number, slot_number); break;
      case 16472: t16472(mapped_module_number, slot_number); break;
      case 16475: t16475(mapped_module_number, slot_number); break;
      case 16476: t16476(mapped_module_number, slot_number); break;
      case 16480: t16480(mapped_module_number, slot_number); break;
      case 16481: t16481(mapped_module_number, slot_number); break;
      case 16482: t16482(mapped_module_number, slot_number); break;
      case 16500: t16500(mapped_module_number, slot_number); break;
      case 16540: t16540(mapped_module_number, slot_number); break;
      case 16541: t16541(mapped_module_number, slot_number); break;
      case 16542: t16542(mapped_module_number, slot_number); break;
      case 16543: t16543(mapped_module_number, slot_number); break;
      case 16544: t16544(mapped_module_number, slot_number); break;
      case 16545: t16545(mapped_module_number, slot_number); break;
      case 16546: t16546(mapped_module_number, slot_number); break;
      case 16547: t16547(mapped_module_number, slot_number); break;
      case 16548: t16548(mapped_module_number, slot_number); break;
      case 16560: t16560(mapped_module_number, slot_number); break;
	  case 16550: t16550(mapped_module_number, slot_number); break;
      case 16551: t16551(mapped_module_number, slot_number); break;
      case 16552: t16552(mapped_module_number, slot_number); break;
      case 16553: t16553(mapped_module_number, slot_number); break;
	  case 16556: t16556(mapped_module_number, slot_number); break;
      case 16557: t16557(mapped_module_number, slot_number); break;
      case 16558: t16558(mapped_module_number, slot_number); break;
      case 16559: t16559(mapped_module_number, slot_number); break;

      case 16561: t16561(mapped_module_number, slot_number); break;
      case 16562: t16562(mapped_module_number, slot_number); break;
      case 16563: t16563(mapped_module_number, slot_number); break;


	  case 16800: t16800(mapped_module_number, slot_number); break;
      case 16801: t16801(mapped_module_number, slot_number); break;
      case 16802: t16802(mapped_module_number, slot_number); break;
      case 16803: t16803(mapped_module_number, slot_number); break;
      case 16804: t16804(mapped_module_number, slot_number); break;
      case 16806: t16806(mapped_module_number, slot_number); break;
      case 16811: t16811(mapped_module_number, slot_number); break;
      case 16820: t16820(mapped_module_number, slot_number); break;
      case 16821: t16821(mapped_module_number, slot_number); break;
      case 16829: t16829(mapped_module_number, slot_number); break;
      case 16832: t16832(mapped_module_number, slot_number); break;
      case 16835: t16835(mapped_module_number, slot_number); break;
      case 16836: t16836(mapped_module_number, slot_number); break;
      case 16840: t16840(mapped_module_number, slot_number); break;
      case 16841: t16841(mapped_module_number, slot_number); break;
      case 16847: t16847(mapped_module_number, slot_number); break;
      case 16850: t16850(mapped_module_number, slot_number); break;
      case 16851: t16851(mapped_module_number, slot_number); break;
      case 16852: t16852(mapped_module_number, slot_number); break;
      case 16853: t16853(mapped_module_number, slot_number); break;
      case 16860: t16860(mapped_module_number, slot_number); break;
      case 16861: t16861(mapped_module_number, slot_number); break;
      case 16871: t16871(mapped_module_number, slot_number); break;
      case 16872: t16872(mapped_module_number, slot_number); break;
      case 16900: t16900(mapped_module_number, slot_number); break;
      case 16904: t16904(mapped_module_number, slot_number); break;
      case 16999: t16999(mapped_module_number, slot_number); break;
      default:
          size_t const l = 80;
          char s[l + 1];
          snprintf(s, l, "Unknown test number (%d)--test skipped.", test_number);
          s[l] = '\0';
          print_line(EXEC_FILE_NUMBER, s);
          break;
    }

/*    reset16000 = 0;
    // while we have an instrument exception and we have reset the
    // sales rack <3 times, attempt to clear the errors.
    while (((exception_value() >= UNABLE_TO_CLOSE_FILE) && (exception_value() <= UNABLE_TO_WRITE_FILE)) && (reset9000 < 3))
    {
      display_it(mapped_module_number, "exception received = ", exception_value(), 0.1);
      set_exception_register(NO_EXCEPTION);
      ++reset16000;
      t16000();
    }

    if (reset16000 == 3)
    {
        display_prompt("HPIB device problem", true);
        display_prompt(" Hit Break to abort testing...", true);
        get_user_answer(buffer, buffer_size);
    }*/

    assert(test_time_file);
    test_time = static_cast<int>(difftime(time(NULL), test_start_time));
    hours = test_time / 3600;
    minutes = (test_time % 3600) / 60;
    seconds = (test_time % 3600) % 60;
    fprintf(test_time_file, "%d\t%d:%d:%d\t m %d sl %d\n", test_number, hours, minutes, seconds, mapped_module_number, slot_number);
//    fprintf(test_time_file, "%d\t%d:%d:%d(hh:mm:ss)\t m %d sl %d\n", test_number, hours, minutes, seconds, mapped_module_number, slot_number);
    fclose(test_time_file);
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
}
