#include "libsalesshared.h"
#include "../parms_p.h"
#include "../execsup_p.h"
#include "../autoadjust/gdispense_p.h"

_LIBSALESSHARED_API_ bool add_gtest_to_task_list(int test_number, int mapped_module_number, int slot_number, int * module_slot_list)
{
  if (save_lists_parm)
    wr_test_to_exec_file(test_number, mapped_module_number, slot_number, module_slot_list);

  return false;
}
