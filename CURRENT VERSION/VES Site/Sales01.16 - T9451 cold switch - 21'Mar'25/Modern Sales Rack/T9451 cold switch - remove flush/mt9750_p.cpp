#include <p2c/p2c.h>

#include <assert.h>

#include "mt9750_p.h"

#include "../../../vmc/src/vmc.h"

#include "digital_const_p.h"

#include "../tests/mt0500.h"

#include "../../../u/src/OSLayer.h"


int HeronP5_sales = 0;

void t9800(int mod_id, int slot)

{

  /* [TWINPIN] DR OUT V RANGE AND  ACCURACY (5V DRIVE HI) */



  /* This test uses the identical code as the PINCARD test 9100.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9100   */

  /* Call test 9100, but log the results against t9800 */

  t9100(mod_id, slot);



}  /* t9800 */



#define RANGE           5   /* for dmm */

#define MUX_NODE_PIN    2   /* 2 of 0 thru 8 */

#define RESOLUTION      1.0e-03   /* for dmm */

#define VOLTAGE_LEVEL   (-3.0e+00)   /* drive hi level */

#define OTHER_LEVEL     (-3.5e+00)   /* drive lo level */

#define DMM_FUNCTION    DCV

void t9801(int mod_id, int slot)

{

  /* [TWINPIN] DR OUT V RANGE AND ACCURACY (-3.0v DRIVE HI) */



  /* This test is a modified version of the PINCARD test 9101.  It has been */

  /* modified for TWINPIN cards because there must be at least 0.5V         */

  /* separation between drive hi and drive low levels.  For path and test   */

  /* descriptions, see the documentation for test: 9100.                    */

  int channel_no;

  int driver;

  test_result_type result;





  open_dmm(mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);



  connect_dmm_pin(mod_id, slot);

  setup_dmm(mod_id, DMM_FUNCTION, static_cast<double>(RANGE), RESOLUTION);

  close_pin_gnd_relays(mod_id, slot);

  for (driver = 'A'; driver <= 'H'; driver++)

  {  /* for driver */

    channel_no = driver - 'A';

    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    vth_pdrv_set_lo(mod_id, slot, channel_no, OTHER_LEVEL);

    vth_pdrv_set_hi(mod_id, slot, channel_no, VOLTAGE_LEVEL);

    vth_pfc_set_sttc_tsp(mod_id, slot, channel_no, 0);

    vth_pfc_set_sttc_dd(mod_id, slot, channel_no, 1);

  }  /* for driver */



  for (driver = 'A'; driver <= 'H'; driver++)

  {  /* for driver */

    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    vth_mseq_neg_cpu_tsp(mod_id);

    measure_dmm(mod_id);

    if (!get_result_log(mod_id, &result))

      display_prompt("No result in result log (Expected DMM reading)", true);

    vth_mseq_ass_cpu_tsp(mod_id);

    vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

  }  /* for driver */



  vth_pinit(mod_id, slot);



  close_dmm(mod_id);

  close_34980A(mod_id);//close_mux(mod_id);



}  /*t9801*/

#undef RANGE

#undef MUX_NODE_PIN

#undef RESOLUTION

#undef VOLTAGE_LEVEL

#undef OTHER_LEVEL

#undef DMM_FUNCTION



void t9802(int mod_id, int slot)

{

  /* [TWINPIN] DR OUT V RANGE AND  ACCURACY (-3.5V DRIVE LO) */



  /* This test uses the identical code as the PINCARD test 9102.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9100   */

  /* Call test 9102, but log the results against t9802 */

  t9102(mod_id, slot);



}  /* t9802 */



#define RANGE           5   /* for dmm */

#define MUX_NODE_PIN    2   /* 2 of 0 thru 8 */

#define RESOLUTION      1.0e-03   /* for dmm */

#define VOLTAGE_LEVEL   4.5e+00   /* drive lo level */

#define OTHER_LEVEL     5.0e+00   /* drive hi level */

#define DMM_FUNCTION    DCV

void t9803(int mod_id, int slot)

{

  /* [TWINPIN] DR OUT V RANGE AND ACCURACY (4.5V DRIVE LO) */



  /* This test is a modified version of the PINCARD test 9103.  It has been */

  /* modified for TWINPIN cards because there must be at least 0.5V         */

  /* separation between drive hi and drive low levels.  For path and test   */

  /* descriptions, see the documentation for test: 9100.                    */

  int channel_no;

  int driver;

  test_result_type result;



  open_dmm(mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);



  connect_dmm_pin(mod_id, slot);

  setup_dmm(mod_id, DMM_FUNCTION, static_cast<double>(RANGE), RESOLUTION);

  close_pin_gnd_relays(mod_id, slot);

  for (driver = 'A'; driver <= 'H'; driver++)

  {  /* for driver */

    channel_no = driver - 'A';

    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    vth_pdrv_set_hi(mod_id, slot, channel_no, OTHER_LEVEL);

    vth_pdrv_set_lo(mod_id, slot, channel_no, VOLTAGE_LEVEL);

    vth_pfc_set_sttc_tsp(mod_id, slot, channel_no, 0);

    vth_pfc_set_sttc_dd(mod_id, slot, channel_no, 0);

  }  /* for driver */



  for (driver = 'A'; driver <= 'H'; driver++)

  {  /* for driver */

    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    vth_mseq_neg_cpu_tsp(mod_id);

    measure_dmm(mod_id);

    if (!get_result_log(mod_id, &result))

      display_prompt("No result in result log (Expected DMM reading)", true);

    vth_mseq_ass_cpu_tsp(mod_id);

    vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

  }  /* for driver */



  vth_pinit(mod_id, slot);



  close_dmm(mod_id);

  close_34980A(mod_id);//close_mux(mod_id);



}  /*t9803*/

#undef RANGE

#undef MUX_NODE_PIN

#undef RESOLUTION

#undef VOLTAGE_LEVEL

#undef OTHER_LEVEL

#undef DMM_FUNCTION



void t9804(int mod_id, int slot)

{

  /* [TWINPIN] GROUND DIFFERENCE TEST - TO AIM SWD GND */



  /* This test uses the identical code as the PINCARD test 9104.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9100   */

  /* Call test 9104, but log the results against t9804 */

  t9104(mod_id, slot);



}  /* t9804 */



void t9805(int mod_id, int slot)

{

  /* [TWINPIN] DR OUTPUT SLEW RATE (225V/uS) */



  /* This test uses the identical code as the PINCARD test 9105.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore different limits.            */

  /* For path and test descriptions, see the documentation for test: 9105   */

  /* Call test 9105, but log the results against t9805 */

  t9105(mod_id, slot);



}  /* t9805 */



void t9806(int mod_id, int slot)

{

  /* [TWINPIN] DR OUTPUT SLEW RATE (100V/uS) */



  /* This test uses the identical code as the PINCARD test 9106.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore different limits.            */

  /* For path and test descriptions, see the documentation for test: 9105   */

  /* Call test 9106, but log the results against t9806 */

  t9106(mod_id, slot);



}  /* t9806 */



void t9807(int mod_id, int slot)

{

  /* [TWINPIN] DR OUTPUT SLEW RATE (50V/uS) */



  /* This test uses the identical code as the PINCARD test 9107.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore different limits.            */

  /* For path and test descriptions, see the documentation for test: 9105   */

  /* Call test 9107, but log the results against t9807 */

  t9107(mod_id, slot);



}  /* t9807 */



void t9811(int mod_id, int slot)

{

  /* [TWINPIN] DR OUT MINIMUM PULSE WIDTH */



  /* This test uses the identical code as the PINCARD test 9111.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9110   */

  /* Call test 9111, but log the results against t9811 */

  t9111(mod_id, slot);



}  /* t9811 */



void t9820(int mod_id, int slot)

{

  /* [TWINPIN] DR OUT MINIMUM PULSE WIDTH */



  /* This test uses the identical code as the PINCARD test 9120.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9120   */

  /* Call test 9120, but log the results against t9820 */

  t9120(mod_id, slot);



}  /* t9820 */



#define RANGE           5   /* for dmm */

#define MUX_NODE_PIN    2   /* 2 of 0 thru 8 */

#define RESOLUTION      1.0e-03   /* for dmm */

#define VOLTAGE_LEVEL   (-2.5e+00)   /* drive hi level */  //cafe

#define OTHER_LEVEL     (-3.5e+00)   /* drive lo level */

#define DMM_FUNCTION    DCV

void t9821(int mod_id, int slot)

{

  /* [CAFE] DR OUT V RANGE AND ACCURACY (-2.5v DRIVE HI) */

  /* limits = -2.5v +/- 99mV */



  /* mjs -- this test is exactly the same as t9801 except for one labeled

          voltage definition above.  it has been cloned for CAFE */



  /* This test is a modified version of the PINCARD test 9101.  It has been */

  /* modified for TWINPIN cards because there must be at least 0.5V         */

  /* separation between drive hi and drive low levels.  For path and test   */

  /* descriptions, see the documentation for test: 9100.                    */



  int channel_no;

  int driver;

  test_result_type result;





  open_dmm(mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);



  connect_dmm_pin(mod_id, slot);

  setup_dmm(mod_id, DMM_FUNCTION, static_cast<double>(RANGE), RESOLUTION);

  close_pin_gnd_relays(mod_id, slot);

  for (driver = 'A'; driver <= 'H'; driver++)

  {  /* for driver */

    channel_no = driver - 'A';

    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    vth_pdrv_set_lo(mod_id, slot, channel_no, OTHER_LEVEL);

    vth_pdrv_set_hi(mod_id, slot, channel_no, VOLTAGE_LEVEL);

    vth_pfc_set_sttc_tsp(mod_id, slot, channel_no, 0);

    vth_pfc_set_sttc_dd(mod_id, slot, channel_no, 1);

  }  /* for driver */



  for (driver = 'A'; driver <= 'H'; driver++)

  {  /* for driver */

    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    vth_mseq_neg_cpu_tsp(mod_id);

    measure_dmm(mod_id);

    if (!get_result_log(mod_id, &result))

      display_prompt("No result in result log (Expected DMM reading)", true);

    vth_mseq_ass_cpu_tsp(mod_id);

    vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

  }  /* for driver */



  vth_pinit(mod_id, slot);



  close_dmm(mod_id);

  close_34980A(mod_id);//close_mux(mod_id);



}  /*t9821*/

#undef RANGE

#undef MUX_NODE_PIN

#undef RESOLUTION

#undef VOLTAGE_LEVEL

#undef OTHER_LEVEL

#undef DMM_FUNCTION



#define RANGE           5   /* for dmm */

#define MUX_NODE_PIN    2   /* 2 of 0 thru 8 */

#define RESOLUTION      1.0e-03   /* for dmm */

#define VOLTAGE_LEVEL   4.0e+00   /* drive lo level */  //cafe

#define OTHER_LEVEL     5.0e+00   /* drive hi level */

#define DMM_FUNCTION    DCV

void t9823(int mod_id, int slot)

{

  /* [CAFE] DR OUT V RANGE AND ACCURACY (4.0V DRIVE LO) */

  /* limits = 4.0v +/- 99mV */



  /* mjs -- this test is exactly the same as t9801 except for one labeled

          voltage definition above.  it has been cloned for CAFE */



  /* This test is a modified version of the PINCARD test 9103.  It has been */

  /* modified for TWINPIN cards because there must be at least 0.5V         */

  /* separation between drive hi and drive low levels.  For path and test   */

  /* descriptions, see the documentation for test: 9100.                    */

  int channel_no;

  int driver;

  test_result_type result;



  open_dmm(mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);



  connect_dmm_pin(mod_id, slot);

  setup_dmm(mod_id, DMM_FUNCTION, static_cast<double>(RANGE), RESOLUTION);

  close_pin_gnd_relays(mod_id, slot);

  for (driver = 'A'; driver <= 'H'; driver++)

  {  /* for driver */

    channel_no = driver - 'A';

    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    vth_pdrv_set_hi(mod_id, slot, channel_no, OTHER_LEVEL);

    vth_pdrv_set_lo(mod_id, slot, channel_no, VOLTAGE_LEVEL);

    vth_pfc_set_sttc_tsp(mod_id, slot, channel_no, 0);

    vth_pfc_set_sttc_dd(mod_id, slot, channel_no, 0);

  }  /* for driver */



  for (driver = 'A'; driver <= 'H'; driver++)

  {  /* for driver */

    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    vth_mseq_neg_cpu_tsp(mod_id);

    measure_dmm(mod_id);

    if (!get_result_log(mod_id, &result))

      display_prompt("No result in result log (Expected DMM reading)", true);

    vth_mseq_ass_cpu_tsp(mod_id);

    vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

  }  /* for driver */



  vth_pinit(mod_id, slot);



  close_dmm(mod_id);

  close_34980A(mod_id);//close_mux(mod_id);



}  /*t9823*/

#undef RANGE

#undef MUX_NODE_PIN

#undef RESOLUTION

#undef VOLTAGE_LEVEL

#undef OTHER_LEVEL

#undef DMM_FUNCTION



void t9828(int mod_id, int slot)///ADDED FOR HERON P5 - 19TH APRIL 2016

{

  /* [TWINPIN] DR 3-STATE EDGE ACCY - PULLED HI */



  /* This test uses the identical code as the PINCARD test 9129.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9129   */

  /* Call test 9129, but log the results against t9828 */


	//vth_pin_card_is_heron_p5(mod_id, slot);
	//HeronP5_sales = vt_receive_integer();

	//if (HeronP5_sales == 1)  //if Heron P5 detected then only execute
	//{
		t9129(mod_id, slot);
	//}



}  /* t9828 */


void t9829(int mod_id, int slot)

{

  /* [TWINPIN] DR 3-STATE EDGE ACCY - PULLED HI */



  /* This test uses the identical code as the PINCARD test 9129.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9129   */

  /* Call test 9129, but log the results against t9829 */

 vth_pin_card_is_heron_p5(mod_id, slot);
	HeronP5_sales = vt_receive_integer();

	if (HeronP5_sales != 1)  //if Heron P5 detected then only execute
	{
		t9129(mod_id, slot);
	}


}  /* t9829 */



void t9830(int mod_id, int slot)

{

  /* [TWINPIN] DR 3-STATE EDGE ACCY - PULLED LO */



  /* This test uses the identical code as the PINCARD test 9130.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9130   */

  /* Call test 9130, but log the results against t9830 */

  t9130(mod_id, slot);

   flush_commands(mod_id);  //added fikri 5/6/2024

      vth_mtmr_wait(mod_id, 0.3); //added fikri 5/6/2024


}  /* t9830 */



#define ABOVE_          0

#define BETWEEN1        1

#define BELOW           3

typedef double dr_level_array[4];

#define MUX_NODE_PIN    2

#define RANGE           300e-03

#define RESOLUTION      1.0e-03

#define DRIVER_LOAD_RESISTANCE  500e+00

const dr_level_array DRIVE_LO_LEVEL = {4.5, -3.5, -3.5, -3.5};

const dr_level_array DRIVE_HI_LEVEL = {5.0, 5.0, 5.0, -3.0};

#define DMM_FUNCTION    DCV

void t9832(int mod_id, int slot)

{

  /* [TWINPIN] DRIVER TRI-STATE LEAKAGE CURRENT */



  /* For path and test descriptions, see the documentation for test: 9132 */

  int driver_no;



  double drive_lo;

  double drive_hi;

  double driver_tri_state_voltage;

  double driver_tri_state_current;



  test_result_type result;

  int driver;



  OpenResourceManager();


  open_dmm(mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);



  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);



  connect_dmm_pin(mod_id, slot);

  connect_500_ohm_load(mod_id);



  close_pin_gnd_relays(mod_id, slot);



  for (driver = 'A'; driver <= 'H'; driver++)

    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);



  for (int driver_level = ABOVE_; driver_level <= BELOW; ++driver_level)

  {  /* FOR EACH DRIVER LEVEL */

    drive_lo = DRIVE_LO_LEVEL[driver_level];

    drive_hi = DRIVE_HI_LEVEL[driver_level];



    for (driver = 'A'; driver <= 'H'; driver++)

    {  /* for driver */

      driver_no = driver - 'A';

      vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

      vth_pdrv_set_lo(mod_id, slot, driver_no, drive_lo);

      vth_pdrv_set_hi(mod_id, slot, driver_no, drive_hi);

      vth_pfc_set_sttc_tsp(mod_id, slot, driver_no, 1);

      if (driver_level == ABOVE_ || driver_level == BETWEEN1)

        vth_pfc_set_sttc_dd(mod_id, slot, driver_no, 1);

      else

        vth_pfc_set_sttc_dd(mod_id, slot, driver_no, 0);

      vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);

      flush_commands(mod_id);  //added fikri 5/6/2024

      vth_mtmr_wait(mod_id, 0.3); //added fikri 5/6/2024

      measure_dmm(mod_id);

      if (get_result_nolog(mod_id, &result))

      {  /*if*/

        driver_tri_state_voltage = result.UU.Real_result;

        driver_tri_state_current = driver_tri_state_voltage / DRIVER_LOAD_RESISTANCE;

        result.UU.Real_result = driver_tri_state_current;

        log_result(mod_id, result);

      }  /*if*/

      else

        display_prompt("No result in result log (Expected dmm voltage)", true);



      vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    }  /* for driver */



  }  /* FOR DRIVER LEVEL */





  vth_pinit(mod_id, slot);



  close_dmm(mod_id);

  close_34980A(mod_id);//close_mux(mod_id);
  CloseResourceManager();



}  /* t9832 */

#undef ABOVE_

#undef BETWEEN1

#undef BELOW

#undef MUX_NODE_PIN

#undef RANGE

#undef RESOLUTION

#undef DRIVER_LOAD_RESISTANCE

#undef DMM_FUNCTION



void t9835(int mod_id, int slot)

{

  /* [TWINPIN] DR OUTPUT IMPEDANCE (SOURCING 700mA) */



  /* This test uses the identical code as the PINCARD test 9135.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore different limits.            */

  /* For path and test descriptions, see the documentation for test: 9135   */

  /* Call test 9135, but log the results against t9835 */

  t9135(mod_id, slot);



}  /* t9835 */


void t9837(int mod_id, int slot)///ADDED FOR HERON P5 - 19TH APRIL 2016

{

  /* [TWINPIN] DR OUTPUT IMPEDANCE (SINKING 500mA) */



  /* This test uses the identical code as the PINCARD test 9136.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore different limits.            */

  /* For path and test descriptions, see the documentation for test: 9135   */

  /* Call test 9136, but log the results against t9837 */

    //vth_pin_card_is_heron_p5(mod_id, slot);
	//HeronP5_sales = vt_receive_integer();

	//if (HeronP5_sales == 1)  //if Heron P5 detected then only execute
	//{
		t9136(mod_id, slot);
	//}


}  /* t9837 */


void t9836(int mod_id, int slot)

{

  /* [TWINPIN] DR OUTPUT IMPEDANCE (SINKING 500mA) */



  /* This test uses the identical code as the PINCARD test 9136.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore different limits.            */

  /* For path and test descriptions, see the documentation for test: 9135   */

  /* Call test 9136, but log the results against t9836 */


	vth_pin_card_is_heron_p5(mod_id, slot);
	HeronP5_sales = vt_receive_integer();

	if (HeronP5_sales != 1)  //if Heron P5 detected then only execute
	{
		t9136(mod_id, slot);
	}



}  /* t9836 */



void t9840(int mod_id, int slot)

{

  /* [TWINPIN] MAX DR CURRENT - SOURCING 700mA */



  /* This test uses the identical code as the PINCARD test 9140.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9140   */

  /* Call test 9140, but log the results against t9840 */

  t9140(mod_id, slot);



}  /* t9840 */



void t9841(int mod_id, int slot)

{

  /* [TWINPIN] MAX DR CURRENT - SINKING 500mA */



  /* This test uses the identical code as the PINCARD test 9141.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9140   */

  /* Call test 9141, but log the results against t9841 */

  t9141(mod_id, slot);



}  /* t9841 */



#define OSC_CHANNEL     1

#define COUNT           5

#define COUPLING        50

#define CHANNEL_RANGE   4.0e+00

#define CHANNEL_OFFSET  1.5e+00

#define TRIGGER_LEVEL   1.5e+00

#define TIMEBASE_RANGE  20e-06

#define TIMEBASE_OFFSET  6e-06

#define CALIBRATION_VOLTAGE  2.5e+00

#define LOW_CURRENT_SPEC  450e-03

#define HI_CURRENT_SPEC  550e-03

#define SOURCING_IL     500e-03

#define SOURCING_IL_MAX  700e-03

#define ADDED_CURRENT_RANGE  20e-03

#define CURRENT_INCREMENT  5e-03

#define TIMEOUT         1.0e-03

#define WAIT_TIME       100e-03

#define INPUT_FREQ      20e+06

#define AVERAGING       true

#define TIMEBASE_MODE   AUTOMATIC

#define PEAK            POS   /*   Which peak to measure?    */

void t9844(int mod_id, int slot)

{

  /* HIGH LEVEL CURRENT LIMIT SOURCING */



  /* This is really version 3.2 of 9144, because 3.2 of 9144 still works with

double density cards, but 3.3 of 9144 is the only version that works with

single density cards.  Therefore, 9844 is really 9144 ver. 3.2*/

  int driver_no;



  double voltage_level;

  double osc_actual_voltage;

  double low_current_range;

  double hi_current_range;

  /*   Calibration for o'scope   */

  double offset;

  double current_current;

  double actual_current;

  double tclk_freq;



  boolean i_limit_occurred;

  boolean guessing;



  a_word_type this_word;

  test_result_type result;

  int driver;





  open_osc(mod_id);

  open_dmm(mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);



  low_current_range = LOW_CURRENT_SPEC - ADDED_CURRENT_RANGE;

  hi_current_range = HI_CURRENT_SPEC + ADDED_CURRENT_RANGE;



  setup_osc_channel(mod_id, OSC_CHANNEL, COUPLING, CHANNEL_RANGE,

                    CHANNEL_OFFSET);

  setup_osc_trigger(mod_id, OSC_CHANNEL, PEAK, TRIGGER_LEVEL, AVERAGING);

  setup_osc_timebase(mod_id, AVERAGING, COUNT, TIMEBASE_RANGE,

                     TIMEBASE_OFFSET, TIMEBASE_MODE);



  offset = offset_calibration(mod_id, CALIBRATION_VOLTAGE);



  connect_osc_ch1_af2(mod_id);



  vth_aclose_rly(mod_id, K753);   /* XL to AF2 relay  */

  vth_aclose_rly(mod_id, K764);   /* XL to AF2 relay  */

  vth_aclose_rly(mod_id, K756);   /* Aim load 5 ohms  */

  vth_aclose_trp_chg_rly(mod_id);   /* Aim load common  */





  tclk_freq = INPUT_FREQ / 64;

  vth_mtclk_clk_20m_mux(mod_id);

  vth_mtclk_set_freq(mod_id, INPUT_FREQ, tclk_freq);



  break_test_for_function_step_compatability(mod_id);



  for (driver = 'A'; driver <= 'H'; driver++)

  {  /*for driver*/

    driver_no = driver - 'A';



    /*GUESS THAT THE CURRENT LIMIT WILL HAPPEN AT OR ABOVE 500mA*/

    current_current = SOURCING_IL;

    voltage_level = current_current * load_5_ohm_resistance[mod_id][slot - 1]

                    [driver - 'A'];

    guessing = true;



    vth_pclose_rly(mod_id, slot, KDCL);   /* XL common relay  */

    vth_pclose_rly(mod_id, slot, KDCG);   /* XG common relay  */

    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);



    i_limit_occurred = false;



    break_test_for_function_step_compatability(mod_id);



    while (!i_limit_occurred && current_current < hi_current_range)

    {  /*while*/

      sales_reset_sequencer(mod_id, slot);

#include "t9144_vml.cmd"

      vth_pdrv_set_hi(mod_id, slot, driver_no, voltage_level);

      vth_pdrv_set_il(mod_id, slot, driver_no, SOURCING_IL);

      vth_mtmr_wait(mod_id, WAIT_TIME);

      sales_start_sequencer(mod_id, slot);



      vth_mtmr_wait(mod_id, WAIT_TIME);

      vth_mseq_stop(mod_id, TIMEOUT);

      vth_prd_cell(mod_id, slot, POVR_PWR_AND_CRRNT);

      if (get_result_nolog(mod_id, &result))

      {  /*process_result*/

        this_word.data = result.UU.Integer_result;





        if (P_getbits_US(this_word.bits, driver_no, 0) == 0)

        {

          if (guessing)

          {  /*a current limit occurred on the guessed value*/

            i_limit_occurred = false;

            current_current = low_current_range - CURRENT_INCREMENT;

          }  /*a current limit occurred on the guessed value*/

          else

            i_limit_occurred = true;

        }

      }  /*process_result*/

      else

        display_prompt("No result in result log (Expected pin cell)", true);



      current_current += CURRENT_INCREMENT;

      guessing = false;



      if (!i_limit_occurred)

      {  /*set up for next value of current*/

        voltage_level = current_current * load_5_ohm_resistance[mod_id]

                        [slot - 1][driver - 'A'];

      }  /*set up for next value of current*/



    }  /*while*/





    sales_reset_sequencer(mod_id, slot);

#include "t9144_vml.cmd"

    vth_pdrv_set_il(mod_id, slot, driver_no, SOURCING_IL_MAX);

    vth_pdrv_set_hi(mod_id, slot, driver_no, voltage_level);

    vth_mtmr_wait(mod_id, WAIT_TIME);

    sales_start_sequencer(mod_id, slot);

    osc_actual_voltage = 0.0;

    trigger_osc(mod_id, OSC_CHANNEL, AVERAGING);

    measure_osc(mod_id, VMAX, PEAK);

    if (get_result_nolog(mod_id, &result))

      osc_actual_voltage = result.UU.Real_result;

    else

      display_prompt("No result in result log(Expected osc vth_reading)", true);

    vth_mseq_stop(mod_id, TIMEOUT);

    osc_actual_voltage -= offset;

    osc_actual_voltage *= load_5_ohm_ratio[mod_id][slot - 1][driver - 'A'];

    actual_current = osc_actual_voltage / load_5_ohm_resistance[mod_id][slot - 1][driver - 'A'];

    result.Measurement_class = CLASS_REAL;

    result.UU.Real_result = actual_current;

    log_result(mod_id, result);



    vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);



  }  /*for driver*/







  vth_pinit(mod_id, slot);

  vth_ainit(mod_id);



  close_osc(mod_id);

  close_dmm(mod_id);

  close_34980A(mod_id);//close_mux(mod_id);



}  /*t9844*/

#undef OSC_CHANNEL

#undef COUNT

#undef COUPLING

#undef CHANNEL_RANGE

#undef CHANNEL_OFFSET

#undef TRIGGER_LEVEL

#undef TIMEBASE_RANGE

#undef TIMEBASE_OFFSET

#undef CALIBRATION_VOLTAGE

#undef LOW_CURRENT_SPEC

#undef HI_CURRENT_SPEC

#undef SOURCING_IL

#undef SOURCING_IL_MAX

#undef ADDED_CURRENT_RANGE

#undef CURRENT_INCREMENT

#undef TIMEOUT

#undef WAIT_TIME

#undef INPUT_FREQ

#undef AVERAGING

#undef TIMEBASE_MODE

#undef PEAK



#define MUX_NODE_PIN    2

#define MIN_TEST_VOLTAGE  0.0 //changed from 5.5 to avoid hot-closing relays -- mjs

#define LOW_BOUNDS      7.054

#define HIGH_BOUNDS     8.800


/* static variables for t9847: */

struct LOC_t9847

{

  int mod_id;

  int slot;

  int driver;

};



static void close_the_relays(struct LOC_t9847 * LINK)

{

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KDC_RLYS[LINK->driver - 'A']);

      /* Xn connect relay */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KX_RLYS[LINK->driver - 'A']);

      /* Xn to receiver relay */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_DRVR_RLYS[MUX_NODE_PIN]

             [LINK->driver - 'A']);

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_RCVR_RLYS[MUX_NODE_PIN]

             [LINK->driver - 'A']);

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KD_RLYS[LINK->driver - 'A']);

  /* Driver connect relay */

  vth_mtmr_wait(LINK->mod_id, 0.001); // mjs - added 1ms for relay close time

}



void t9847(int mod_id_, int slot_)

{

  /* [TWINPIN/FEZZIWIG] DR POSITIVE OV LIMIT ACCURACY */



  /* mjs - changed the search algorithm to a "thirded search"

     This test works for Monet and Fezziwig pincards.  The old

     test had holes and didn't work for the Fezziwig card */

  /* mjs - also added the driver output tri-state step to avoid

     contention and optimized some of the wait states based upon

     scope measurements made with WA 5/99 */



  struct LOC_t9847 V;

  double actual_voltage;



  a_word_type this_word;

  test_result_type result;

  V.mod_id = mod_id_;

  V.slot = slot_;



  int high_bounds_ov=0;

  int low_bounds_ov=0;

  double high_pt;

  double mid_high_pt;

  double mid_low_pt;

  double low_pt;

  double window_width;

  int trip_hmid=0;

  int trip_lmid=0;

  int i;





  vth_aclose_rly(V.mod_id, K806);   /* Aim source to S bus */

  for (V.driver = 'A'; V.driver <= 'H'; V.driver++)   /* S bus to Xn busses */

    vth_aclose_rly(V.mod_id, AIM_X_S_RLYS[V.driver - 'A']);

  vth_afc_en_src_out(V.mod_id);



  for (V.driver = 'A'; V.driver <= 'H'; V.driver++)

  {  /*for driver*/

    high_bounds_ov=0;

    low_bounds_ov=0;

    trip_hmid=0;

    trip_lmid=0;

    vth_pinit(V.mod_id, V.slot);

    vth_pdrv_clr_err(V.mod_id, V.slot);

    vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

    vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

    close_the_relays(&V);



      for (i = 0; i < 8; i++)

      {

        //setup driver & receiver

        vth_pdrv_set_hi(V.mod_id, V.slot, i, 3.5);

        vth_pdrv_set_lo(V.mod_id, V.slot, i, 0.2);



        vth_pfc_set_sttc_dd(V.mod_id, V.slot, i, 1);

        vth_pfc_set_sttc_tsp(V.mod_id, V.slot, i, 0);

      }



      // Enable the Driver Backmatch resistor before

      // closing the Driver Disconnect relay

      vth_pdrv_en_bkmtch(V.mod_id, V.slot, V.driver-'A');





//check low bound voltage

      vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

      vth_asrc_set_dc_ampl(V.mod_id, LOW_BOUNDS);

      vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

      vth_prd_stat(V.mod_id, V.slot);

      if (get_result_nolog(V.mod_id, &result))

      {

        this_word.data = result.UU.Integer_result;

        if (P_getbits_US(this_word.bits, 5, 0) == 0)

        { //limit_occurred

           vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

           vth_pinit(V.mod_id, V.slot);

           vth_pdrv_clr_err(V.mod_id, V.slot);

           close_the_relays(&V);

           low_bounds_ov =1;

        }

      }



//check high bound voltage

      vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

      vth_asrc_set_dc_ampl(V.mod_id, HIGH_BOUNDS);

      vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

      vth_prd_stat(V.mod_id, V.slot);

      if (get_result_nolog(V.mod_id, &result))

      {

        this_word.data = result.UU.Integer_result;

        if (P_getbits_US(this_word.bits, 5, 0) == 0)

        { //limit_occurred

           vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

           vth_pinit(V.mod_id, V.slot);

           vth_pdrv_clr_err(V.mod_id, V.slot);

           close_the_relays(&V);

           high_bounds_ov =1;

        }

      }







//initialization of search routine

        high_pt = HIGH_BOUNDS;

        low_pt = LOW_BOUNDS;

        window_width = (high_pt-low_pt)/3;

        mid_high_pt = high_pt - window_width;

        mid_low_pt = mid_high_pt - window_width;



//check to see that ONE AND NOT BOTH of the bounds caused an overvoltage

        if ( ((high_bounds_ov) && !(low_bounds_ov)) || (!(high_bounds_ov) && (low_bounds_ov)) )

        {// an initial overvoltage window exists



          for (i=0; i<3; i++) //change iteration number based on the accuracy you want

          { //windowing loop

           trip_hmid=0;//reinitialize

           trip_lmid=0;

           vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

           vth_asrc_set_dc_ampl(V.mod_id, mid_high_pt); //check mid_high voltage

           vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

           vth_prd_stat(V.mod_id, V.slot);

           if (get_result_nolog(V.mod_id, &result))

           {

             this_word.data = result.UU.Integer_result;

             if (P_getbits_US(this_word.bits, 5, 0) == 0)

             { //limit_occurred

                vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

                vth_pinit(V.mod_id, V.slot);

                vth_pdrv_clr_err(V.mod_id, V.slot);

                close_the_relays(&V);

                trip_hmid=1;

             }

           }



           vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

           vth_asrc_set_dc_ampl(V.mod_id, mid_low_pt); // check mid_low voltage

           vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

           vth_prd_stat(V.mod_id, V.slot);

           if (get_result_nolog(V.mod_id, &result))

           {

             this_word.data = result.UU.Integer_result;

             if (P_getbits_US(this_word.bits, 5, 0) == 0)

             { //limit_occurred

                vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

                vth_pinit(V.mod_id, V.slot);

                vth_pdrv_clr_err(V.mod_id, V.slot);

                close_the_relays(&V);

                trip_lmid=1;

             }

           }



           // recalculate mid high and low points based upon voltage trips

           if ((trip_hmid) && (trip_lmid)) //tripped both midpoints

           {

             high_pt = mid_low_pt;

           }

           else if (!(trip_hmid) && !(trip_lmid)) //tripped neither of the midpoints

           {

             low_pt = mid_high_pt;

           }

           else //tripped one and not the other

           {

             high_pt = mid_high_pt;

             low_pt = mid_low_pt;

           }

           window_width = (high_pt-low_pt)/3;

           mid_high_pt = high_pt - window_width;

           mid_low_pt = mid_high_pt - window_width;



          } //windowing loop



          actual_voltage = (high_pt+low_pt)/2;

        }// an initial overvoltage window exists

        else

        {// no overvoltage window exists

//        display_prompt("Overvoltage window not found.", true);

//	display_it(V.mod_id, "high_bounds ov", high_bounds_ov, high_bounds_ov);

//	display_it(V.mod_id, "low_bounds ov", low_bounds_ov, low_bounds_ov);

        actual_voltage = 0;

        }





        vth_pdrv_dis_bkmtch(V.mod_id, V.slot, V.driver-'A');

        result.Measurement_class = CLASS_REAL;

        result.UU.Real_result = actual_voltage;

        log_result(V.mod_id, result);



  }  /*for driver*/



  vth_asrc_set_dc_ampl(V.mod_id, 0);

  vth_pdrv_clr_err(V.mod_id, V.slot);

  vth_mod_init(V.mod_id);

  vth_ainit(V.mod_id);

  vth_pinit(V.mod_id, V.slot);



}  /* t9847 */

#undef MUX_NODE_PIN

#undef LOW_BOUNDS

#undef HIGH_BOUNDS

#undef MIN_TEST_VOLTAGE






//////////////////////////////////////////////////////////////////////////Start of T9848////////////////////////////////////////////////////

#define MUX_NODE_PIN    2

#define MIN_TEST_VOLTAGE  0.0 //changed to avoid hot-closing relays -- mjs

#define LOW_BOUNDS      (-6.938)

#define HIGH_BOUNDS     (-5.600)


/* static variables for t9848: */

struct LOC_t9848

{

  int mod_id;

  int slot;

  int driver;

};



static void close_the_relays_(struct LOC_t9848 * LINK)

{

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KDC_RLYS[LINK->driver - 'A']);

      /* Xn connect relay */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KX_RLYS[LINK->driver - 'A']);

      /* Xn to receiver relay */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_DRVR_RLYS[MUX_NODE_PIN]

             [LINK->driver - 'A']);

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_RCVR_RLYS[MUX_NODE_PIN]

             [LINK->driver - 'A']);

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KD_RLYS[LINK->driver - 'A']);

      /* Driver connect relay */



  vth_mtmr_wait(LINK->mod_id, 0.001); // mjs - added 1ms for relay close time

}



void t9848(int mod_id_, int slot_)

{

  /* [TWINPIN/FEZZIWIG] DR NEGATIVE OV LIMIT ACCURACY */



  /* mjs - changed the search algorithm to a "thirded search"

     This test works for Monet and Fezziwig pincards.  The old

     test had holes and didn't work for the Fezziwig card */

  /* mjs - also added the driver output tri-state step to avoid

     contention and optimized some of the wait states based upon

     scope measurements made with WA 5/99 */



  struct LOC_t9848 V;

  double actual_voltage;



  a_word_type this_word;

  test_result_type result;

  V.mod_id = mod_id_;

  V.slot = slot_;



  int high_bounds_ov=0;

  int low_bounds_ov=0;

  double high_pt;

  double mid_high_pt;

  double mid_low_pt;

  double low_pt;

  double window_width;

  int trip_hmid=0;

  int trip_lmid=0;

  int i;



  vth_aclose_rly(V.mod_id, K806);   /* Aim source to S bus */

  for (V.driver = 'A'; V.driver <= 'H'; V.driver++)   /* S bus to Xn busses */

    vth_aclose_rly(V.mod_id, AIM_X_S_RLYS[V.driver - 'A']);

  vth_afc_en_src_out(V.mod_id);





  for (V.driver = 'A'; V.driver <= 'H'; V.driver++)

  {  /*for driver*/



    high_bounds_ov=0;

    low_bounds_ov=0;

    trip_hmid=0;

    trip_lmid=0;

    vth_pinit(V.mod_id, V.slot);

    vth_pdrv_clr_err(V.mod_id, V.slot);

    vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

    vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

    close_the_relays_(&V);



      for (i = 0; i < 8; i++)

      {

        //setup driver & receiver

        vth_pdrv_set_hi(V.mod_id, V.slot, i, 3.5);

        vth_pdrv_set_lo(V.mod_id, V.slot, i, 0.2);



        vth_pfc_set_sttc_dd(V.mod_id, V.slot, i, 1);

        vth_pfc_set_sttc_tsp(V.mod_id, V.slot, i, 0);

      }



      // Enable the Driver Backmatch resitor before

      // closing the Driver Disconnect relay

      vth_pdrv_en_bkmtch(V.mod_id, V.slot, V.driver-'A');



//check high bound voltage

      vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

      vth_asrc_set_dc_ampl(V.mod_id, HIGH_BOUNDS);

      vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

      vth_prd_stat(V.mod_id, V.slot);

      if (get_result_nolog(V.mod_id, &result))

      {

        this_word.data = result.UU.Integer_result;

        if (P_getbits_US(this_word.bits, 5, 0) == 0)

        { //limit_occurred

           vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

           vth_pinit(V.mod_id, V.slot);

           vth_pdrv_clr_err(V.mod_id, V.slot);

           close_the_relays_(&V);

           high_bounds_ov =1;

        }



      }



//check low bound voltage

      vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

      vth_asrc_set_dc_ampl(V.mod_id, LOW_BOUNDS);

      vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

      vth_prd_stat(V.mod_id, V.slot);

      if (get_result_nolog(V.mod_id, &result))

      {

        this_word.data = result.UU.Integer_result;

        if (P_getbits_US(this_word.bits, 5, 0) == 0)

        { //limit_occurred

           vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

           vth_pinit(V.mod_id, V.slot);

           vth_pdrv_clr_err(V.mod_id, V.slot);

           close_the_relays_(&V);

           low_bounds_ov =1;

        }



      }



//initialization of search routine

        high_pt = HIGH_BOUNDS;

        low_pt = LOW_BOUNDS;

        window_width = (high_pt-low_pt)/3;

        mid_high_pt = high_pt - window_width;

        mid_low_pt = mid_high_pt - window_width;



//check to see that ONE AND NOT BOTH of the bounds caused an overvoltage

        if ( ((high_bounds_ov) && !(low_bounds_ov)) || (!(high_bounds_ov) && (low_bounds_ov)) )

        {// an initial overvoltage window exists



          for (i=0; i<3; i++)  //change iteration number based on the accuracy you want

          { //windowing loop

           trip_hmid=0;//reinitialize

           trip_lmid=0;

           vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

           vth_asrc_set_dc_ampl(V.mod_id, mid_high_pt); //check mid_high voltage

           vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

           vth_prd_stat(V.mod_id, V.slot);

           if (get_result_nolog(V.mod_id, &result))

           {

             this_word.data = result.UU.Integer_result;

             if (P_getbits_US(this_word.bits, 5, 0) == 0)

             { //limit_occurred

                vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

                vth_pinit(V.mod_id, V.slot);

                vth_pdrv_clr_err(V.mod_id, V.slot);

                close_the_relays_(&V);

                trip_hmid=1;

             }



           }



           vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

           vth_asrc_set_dc_ampl(V.mod_id, mid_low_pt); // check mid_low voltage

           vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

           vth_prd_stat(V.mod_id, V.slot);

           if (get_result_nolog(V.mod_id, &result))

           {

             this_word.data = result.UU.Integer_result;

             if (P_getbits_US(this_word.bits, 5, 0) == 0)

             { //limit_occurred

                vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

                vth_pinit(V.mod_id, V.slot);

                vth_pdrv_clr_err(V.mod_id, V.slot);

                close_the_relays_(&V);

                trip_lmid=1;

             }



           }



           // recalculate mid high and low points based upon voltage trips

           if ((trip_hmid) && (trip_lmid)) //tripped both midpoints

           {

             low_pt = mid_high_pt;

           }

           else if (!(trip_hmid) && !(trip_lmid)) //tripped neither of the midpoints

           {

             high_pt = mid_low_pt;

           }

           else //tripped one and not the other

           {

             high_pt = mid_high_pt;

             low_pt = mid_low_pt;

           }

           window_width = (high_pt-low_pt)/3;

           mid_high_pt = high_pt - window_width;

           mid_low_pt = mid_high_pt - window_width;



          } //windowing loop



          actual_voltage = (high_pt+low_pt)/2;

        }// an initial overvoltage window exists

        else

        {// no overvoltage window exists

//        display_prompt("Overvoltage window not found.", true);

//	display_it(V.mod_id, "high_bounds ov", high_bounds_ov, high_bounds_ov);

//	display_it(V.mod_id, "low_bounds ov", low_bounds_ov, low_bounds_ov);

        actual_voltage = 0;

        }





        vth_pdrv_dis_bkmtch(V.mod_id, V.slot, V.driver-'A');

        result.Measurement_class = CLASS_REAL;

        result.UU.Real_result = actual_voltage;

        log_result(V.mod_id, result);



  }  /*for driver*/



  vth_asrc_set_dc_ampl(V.mod_id, 0);

  vth_pdrv_clr_err(V.mod_id, V.slot);

  vth_mod_init(V.mod_id);

  vth_ainit(V.mod_id);

  vth_pinit(V.mod_id, V.slot);



}  /*t9848*/

#undef MUX_NODE_PIN

#undef MIN_TEST_VOLTAGE

#undef LOW_BOUNDS

#undef HIGH_BOUNDS

////////////////////////////////////////// end of T9848///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////start of newly added T9849 for Fezziwig and Heron support, existing T9848 maintained but disabled in digital test list file///////
#define MUX_NODE_PIN    2

#define MIN_TEST_VOLTAGE  0.0 //changed to avoid hot-closing relays -- mjs

#define LOW_BOUNDS      (-6.938)

//#define HIGH_BOUNDS     (-5.600)
#define HIGH_BOUNDS     (-4.000) // for Heron and Fezziwig


/* static variables for t9849: */

struct LOC_t9849

{

  int mod_id;

  int slot;

  int driver;

};



static void close_the_relays_(struct LOC_t9849 * LINK)

{

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KDC_RLYS[LINK->driver - 'A']);

      /* Xn connect relay */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KX_RLYS[LINK->driver - 'A']);

      /* Xn to receiver relay */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_DRVR_RLYS[MUX_NODE_PIN]

             [LINK->driver - 'A']);

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_RCVR_RLYS[MUX_NODE_PIN]

             [LINK->driver - 'A']);

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KD_RLYS[LINK->driver - 'A']);

      /* Driver connect relay */



  vth_mtmr_wait(LINK->mod_id, 0.001); // mjs - added 1ms for relay close time

}



void t9849(int mod_id_, int slot_)

{

  /* [TWINPIN/FEZZIWIG] DR NEGATIVE OV LIMIT ACCURACY */



  /* mjs - changed the search algorithm to a "thirded search"

     This test works for Monet and Fezziwig pincards.  The old

     test had holes and didn't work for the Fezziwig card */

  /* mjs - also added the driver output tri-state step to avoid

     contention and optimized some of the wait states based upon

     scope measurements made with WA 5/99 */



  struct LOC_t9849 V;

  double actual_voltage;



  a_word_type this_word;

  test_result_type result;

  V.mod_id = mod_id_;

  V.slot = slot_;



  int high_bounds_ov=0;

  int low_bounds_ov=0;

  double high_pt;

  double mid_high_pt;

  double mid_low_pt;

  double low_pt;

  double window_width;

  int trip_hmid=0;

  int trip_lmid=0;

  int i;



  vth_aclose_rly(V.mod_id, K806);   /* Aim source to S bus */

  for (V.driver = 'A'; V.driver <= 'H'; V.driver++)   /* S bus to Xn busses */

    vth_aclose_rly(V.mod_id, AIM_X_S_RLYS[V.driver - 'A']);

  vth_afc_en_src_out(V.mod_id);





  for (V.driver = 'A'; V.driver <= 'H'; V.driver++)

  {  /*for driver*/



    high_bounds_ov=0;

    low_bounds_ov=0;

    trip_hmid=0;

    trip_lmid=0;

    vth_pinit(V.mod_id, V.slot);

    vth_pdrv_clr_err(V.mod_id, V.slot);

    vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

    vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

    close_the_relays_(&V);



      for (i = 0; i < 8; i++)

      {

        //setup driver & receiver

        vth_pdrv_set_hi(V.mod_id, V.slot, i, 3.5);

        vth_pdrv_set_lo(V.mod_id, V.slot, i, 0.2);



        vth_pfc_set_sttc_dd(V.mod_id, V.slot, i, 1);

        vth_pfc_set_sttc_tsp(V.mod_id, V.slot, i, 0);

      }



      // Enable the Driver Backmatch resitor before

      // closing the Driver Disconnect relay

      vth_pdrv_en_bkmtch(V.mod_id, V.slot, V.driver-'A');



//check high bound voltage

      vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

      vth_asrc_set_dc_ampl(V.mod_id, HIGH_BOUNDS);

      vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

      vth_prd_stat(V.mod_id, V.slot);

      if (get_result_nolog(V.mod_id, &result))

      {

        this_word.data = result.UU.Integer_result;

        if (P_getbits_US(this_word.bits, 5, 0) == 0)

        { //limit_occurred

           vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

           vth_pinit(V.mod_id, V.slot);

           vth_pdrv_clr_err(V.mod_id, V.slot);

           close_the_relays_(&V);

           high_bounds_ov =1;

        }



      }



//check low bound voltage

      vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

      vth_asrc_set_dc_ampl(V.mod_id, LOW_BOUNDS);

      vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

      vth_prd_stat(V.mod_id, V.slot);

      if (get_result_nolog(V.mod_id, &result))

      {

        this_word.data = result.UU.Integer_result;

        if (P_getbits_US(this_word.bits, 5, 0) == 0)

        { //limit_occurred

           vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

           vth_pinit(V.mod_id, V.slot);

           vth_pdrv_clr_err(V.mod_id, V.slot);

           close_the_relays_(&V);

           low_bounds_ov =1;

        }



      }



//initialization of search routine

        high_pt = HIGH_BOUNDS;

        low_pt = LOW_BOUNDS;

        window_width = (high_pt-low_pt)/3;

        mid_high_pt = high_pt - window_width;

        mid_low_pt = mid_high_pt - window_width;



//check to see that ONE AND NOT BOTH of the bounds caused an overvoltage

        if ( ((high_bounds_ov) && !(low_bounds_ov)) || (!(high_bounds_ov) && (low_bounds_ov)) )

        {// an initial overvoltage window exists



          for (i=0; i<3; i++)  //change iteration number based on the accuracy you want

          { //windowing loop

           trip_hmid=0;//reinitialize

           trip_lmid=0;

           vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

           vth_asrc_set_dc_ampl(V.mod_id, mid_high_pt); //check mid_high voltage

           vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

           vth_prd_stat(V.mod_id, V.slot);

           if (get_result_nolog(V.mod_id, &result))

           {

             this_word.data = result.UU.Integer_result;

             if (P_getbits_US(this_word.bits, 5, 0) == 0)

             { //limit_occurred

                vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

                vth_pinit(V.mod_id, V.slot);

                vth_pdrv_clr_err(V.mod_id, V.slot);

                close_the_relays_(&V);

                trip_hmid=1;

             }



           }



           vth_pfc_set_sttc_tsp(V.mod_id, V.slot, V.driver-'A', 1); // tri-state driver output

           vth_asrc_set_dc_ampl(V.mod_id, mid_low_pt); // check mid_low voltage

           vth_mtmr_wait(V.mod_id, 0.010); //mjs - allow 10ms for voltage to apply and trip ov

           vth_prd_stat(V.mod_id, V.slot);

           if (get_result_nolog(V.mod_id, &result))

           {

             this_word.data = result.UU.Integer_result;

             if (P_getbits_US(this_word.bits, 5, 0) == 0)

             { //limit_occurred

                vth_asrc_set_dc_ampl(V.mod_id, MIN_TEST_VOLTAGE);

                vth_pinit(V.mod_id, V.slot);

                vth_pdrv_clr_err(V.mod_id, V.slot);

                close_the_relays_(&V);

                trip_lmid=1;

             }



           }



           // recalculate mid high and low points based upon voltage trips

           if ((trip_hmid) && (trip_lmid)) //tripped both midpoints

           {

             low_pt = mid_high_pt;

           }

           else if (!(trip_hmid) && !(trip_lmid)) //tripped neither of the midpoints

           {

             high_pt = mid_low_pt;

           }

           else //tripped one and not the other

           {

             high_pt = mid_high_pt;

             low_pt = mid_low_pt;

           }

           window_width = (high_pt-low_pt)/3;

           mid_high_pt = high_pt - window_width;

           mid_low_pt = mid_high_pt - window_width;



          } //windowing loop



          actual_voltage = (high_pt+low_pt)/2;

        }// an initial overvoltage window exists

        else

        {// no overvoltage window exists

//        display_prompt("Overvoltage window not found.", true);

//	display_it(V.mod_id, "high_bounds ov", high_bounds_ov, high_bounds_ov);

//	display_it(V.mod_id, "low_bounds ov", low_bounds_ov, low_bounds_ov);

        actual_voltage = 0;

        }





        vth_pdrv_dis_bkmtch(V.mod_id, V.slot, V.driver-'A');

        result.Measurement_class = CLASS_REAL;

        result.UU.Real_result = actual_voltage;

        log_result(V.mod_id, result);



  }  /*for driver*/



  vth_asrc_set_dc_ampl(V.mod_id, 0);

  vth_pdrv_clr_err(V.mod_id, V.slot);

  vth_mod_init(V.mod_id);

  vth_ainit(V.mod_id);

  vth_pinit(V.mod_id, V.slot);



}  /*t9849*/

#undef MUX_NODE_PIN

#undef MIN_TEST_VOLTAGE

#undef LOW_BOUNDS

#undef HIGH_BOUNDS


//////////////////////////////////////////////end of T9849///////////////////////////////////////////////////////////////////////////////



void t9850(int mod_id, int slot)

{

  /* [TWINPIN] RX VOLT ACCURACY AT +5V */



  /* This test uses the identical code as the PINCARD test 9150.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9150   */

  /* Call test 9150, but log the results against t9850 */

  t9150(mod_id, slot);



}  /* t9850 */



void t9851(int mod_id, int slot)

{

  /* [TWINPIN] RX VOLT ACCURACY AT +4V - PULLED UP */



  /* This test uses the identical code as the PINCARD test 9151.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9150   */

  /* Call test 9151, but log the results against t9851 */

  t9151(mod_id, slot);



}  /* t9851 */



void t9852(int mod_id, int slot)

{

  /* [TWINPIN] RX VOLT ACCURACY AT -3.5V */



  /* This test uses the identical code as the PINCARD test 9152.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9150   */

  /* Call test 9152, but log the results against t9852 */

  t9152(mod_id, slot);



}  /* t9852 */



void t9853(int mod_id, int slot)

{

  /* [TWINPIN] RX VOLT ACCURACY AT -2.5V - PULLED DOWN */



  /* This test uses the identical code as the PINCARD test 9153.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9150   */

  /* Call test 9153, but log the results against t9853 */

  t9153(mod_id, slot);



}  /* t9853 */



void t9859(int mod_id, int slot)///added 19th April 2016 for Heron P5

{

  /* [TWINPIN] RX INPUT CURRENT SOURCING */



  /* This test uses the identical code as the PINCARD test 9160.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9160   */

  /* Call test 9160, but log the results against t9859 */

//	vth_pin_card_is_heron_p5(mod_id, slot);
//	HeronP5_sales = vt_receive_integer();
//
//	if (HeronP5_sales == 1)  //if Heron P5 detected then only execute
//	{
		t9160(mod_id, slot);
//	}


}  /* t9859 */


void t9860(int mod_id, int slot)

{

  /* [TWINPIN] RX INPUT CURRENT SOURCING */



  /* This test uses the identical code as the PINCARD test 9160.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9160   */

  /* Call test 9160, but log the results against t9860 */

	vth_pin_card_is_heron_p5(mod_id, slot);
	HeronP5_sales = vt_receive_integer();

	if (HeronP5_sales != 1)  //if Heron P5 detected then only execute
	{
		t9160(mod_id, slot);
	}


}  /* t9860 */



void t9861(int mod_id, int slot)

{

  /* [TWINPIN] RX INPUT CURRENT SINKING */



  /* This test uses the identical code as the PINCARD test 9161.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9160   */

  /* Call test 9161, but log the results against t9861 */

  t9161(mod_id, slot);



}  /* t9861 */



void t9862(int mod_id, int slot)

{

  /* [TWINPIN] RX INPUT LEAKAGE CURRENT */



  /* This test uses the identical code as the PINCARD test 9162.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9160   */

  /* Call test 9162, but log the results against t9862 */

  t9162(mod_id, slot);



}  /* t9862 */


void t9870(int mod_id, int slot)///ADDED FOR HERON P5 - 19TH APRIL 2016

{

  /* [TWINPIN] RX EDGE ACCY AND SKEW */



  /* This test uses the identical code as the PINCARD test 9171.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore different limits.            */

  /* For path and test descriptions, see the documentation for test: 9171   */

  /* Call test 9171, but log the results against t9870 */


	//vth_pin_card_is_heron_p5(mod_id, slot);
	//HeronP5_sales = vt_receive_integer();

	//if (HeronP5_sales == 1)  //if Heron P5 detected then only execute
	//{
		t9171(mod_id, slot);
	//}



}  /* t9870 */


void t9871(int mod_id, int slot)

{

  /* [TWINPIN] RX EDGE ACCY AND SKEW */



  /* This test uses the identical code as the PINCARD test 9171.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore different limits.            */

  /* For path and test descriptions, see the documentation for test: 9171   */

  /* Call test 9171, but log the results against t9871 */


	vth_pin_card_is_heron_p5(mod_id, slot);
	HeronP5_sales = vt_receive_integer();

	if (HeronP5_sales != 1)  //if Heron P5 detected then only execute
	{
		t9171(mod_id, slot);
	}



}  /* t9871 */



#define MUX_NODE_PIN    2

#define LOW_BOUNDS      13.00 // this value should NEVER trip overvoltage

#define HIGH_BOUNDS     17.60 // this value should trip overvoltage


#define MIN_TEST_VOLTAGE 10.0  //intermediate value

#define OLYM_CH1_CURR	500e-03 //Added by chuanho, 15jun2012

#define OLYM_CH1_OCP	1.0e+00 //Added by chuanho, 15jun2012


/* static variables for t9882: */

struct LOC_t9882

{

  int mod_id;

  int slot;

  int receiver;

};



static void close_the_relays__(struct LOC_t9882 *LINK)

{

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KDC_RLYS[LINK->receiver - 'A']);

      /* Xn connect relay */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KX_RLYS[LINK->receiver - 'A']);

      /* Xn to receiver relay */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_RCVR_RLYS[MUX_NODE_PIN]

             [LINK->receiver - 'A']);

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KR_RLYS[LINK->receiver - 'A']);

      /* receiver connect relay */

  vth_mtmr_wait(LINK->mod_id, 0.001); // mjs - added 1ms for relay close time

}



void t9882(int mod_id_, int slot_)

{

  /* This is a window version of the overvoltage test. The specs for the Edge

     driver say it should trip overvoltage @  +/-14.5v w/ +/-.5v accuracy.

     This test does not find the exact value, only verifies that it will trip

     and clear.  The customer receiver spec is +/-12.0v.

  */



  struct LOC_t9882 V;



  a_word_type this_word;

  test_result_type result;

  V.mod_id = mod_id_;

  V.slot = slot_;



  int high_trip;

  int low_trip;




  OpenResourceManager(); //added by chuanho, 18jul2011

  open_33250A(V.mod_id);
  //open_mux(V.mod_id);
  open_34980A(V.mod_id);

  /*New Olympus codes*/
  //Added by chuanho, 15jun2012
  if(check_syn_model())
  {
	  open_olym(V.mod_id);
      dconnect_olym_ch1_af2(V.mod_id);
      display_prompt("use olym)", true);
  }
  /*End of new Olympus codes*/
  else
  {

	  open_syn(V.mod_id);
      setup_syn_hv(V.mod_id, true);         // need high voltage capabilities ;
      setup_syn(V.mod_id, DC, 0, 0.0, 0.0);
      setup_syn_off(V.mod_id, 0.0);
	  connect_syn_out_af2(V.mod_id);
	  display_prompt("use sys_out)", true);

  }




  vth_aclose_rly(V.mod_id, K764);   // connects AF2 to A_XL

  vth_aclose_rly(V.mod_id, K751);   // connects A_XL to S


  flush_commands(V.mod_id);  //fikri added 26/06/2024

  vth_mtmr_wait(V.mod_id, 5.0); /* add for stabilize new Fun Gen 33250 Fikri 26/06/2024*/



  for (V.receiver = 'A'; V.receiver <= 'H'; V.receiver++)   //S bus to Xn busses

    vth_aclose_rly(V.mod_id, AIM_X_S_RLYS[V.receiver - 'A']);



  for (V.receiver = 'A'; V.receiver <= 'H'; V.receiver++)

  {  //for receiver

	/*New Olympus codes*/
    //Added by chuanho, 15jun2012
  	if(check_syn_model())
	{

		setup_olym_ch1_volt(V.mod_id, MIN_TEST_VOLTAGE);
		setup_olym_ch1_curr(V.mod_id, OLYM_CH1_CURR);
		setup_olym_ch1_ocp(V.mod_id, OLYM_CH1_OCP);
		enable_olym_ch1_out(V.mod_id);

	}
	/*End of new codes*/
	else
		setup_syn_off(V.mod_id, MIN_TEST_VOLTAGE);

	    close_the_relays__(&V);

	    vth_mtmr_wait(V.mod_id, 0.100);  //increased time to 100ms to give syn extra time

	    high_trip = 0;

	    low_trip = 0;

    //------------------------------------------------------------------------------------

	  /*New Olympus codes*/
      //Added by chuanho, 15jun2012
      if(check_syn_model())
		  setup_olym_ch1_volt(V.mod_id, LOW_BOUNDS);
	  /*End of new codes*/
      else
		  setup_syn_off(V.mod_id, LOW_BOUNDS);

      vth_mtmr_wait(V.mod_id, 0.050);  //increased time to 50ms to give syn extra time

      vth_prd_stat(V.mod_id, V.slot);  // check status of error register



      if (get_result_nolog(V.mod_id, &result))

      { //get_result_nolog

        this_word.data = result.UU.Integer_result;

        if (P_getbits_US(this_word.bits, 5, 0) == 0)

        { //limit_occurred

           low_trip = 1;

           vth_pinit(V.mod_id, V.slot);

           vth_pdrv_clr_err(V.mod_id, V.slot);

        } //limit_occurred

      } //get_result_nolog

    //------------------------------------------------------------------------------------

	  /*New Olympus codes*/
      //Added by chuanho, 15jun2012

      if(check_syn_model())
		  setup_olym_ch1_volt(V.mod_id, HIGH_BOUNDS);
	  /*End of new codes*/
	  else
		  setup_syn_off(V.mod_id, HIGH_BOUNDS);

      vth_mtmr_wait(V.mod_id, 0.050);  //increased time to 50ms to give syn extra time

      vth_prd_stat(V.mod_id, V.slot);  // check status of error register

      if (get_result_nolog(V.mod_id, &result))

      { //get_result_nolog

        this_word.data = result.UU.Integer_result;

        if (P_getbits_US(this_word.bits, 5, 0) == 0)

        { //limit_occurred

           high_trip = 1;

           vth_pinit(V.mod_id, V.slot);

           vth_pdrv_clr_err(V.mod_id, V.slot);

        } //limit_occurred

      } //get_result_nolog

    //------------------------------------------------------------------------------------

    result.Measurement_class = CLASS_REAL;

    if ( (high_trip == 1) && (low_trip == 0) )

      result.UU.Real_result = 14.5;  //arbitrary pass value

    else

      result.UU.Real_result = 0;     //arbitrary fail value

    log_result(V.mod_id, result);



  }  //for receiver


  /*New Olympus codes*/
  //Added by chuanho, 15jun2012

  if(check_syn_model())
  {
	  disable_olym_ch1_out(V.mod_id);
	  close_olym(V.mod_id);

  }
  /*End of new codes*/
  else
	  close_syn(V.mod_id);


 // vth_ainit(V.mod_id);

  vth_mod_init(V.mod_id);

  vth_ainit(V.mod_id);

  vth_mtmr_wait(V.mod_id, 5.0); /* add for stabilize new Fun Gen 3360A Fikri 19-July-2022*/
  close_33250A(V.mod_id);

  close_34980A(V.mod_id);//close_mux(V.mod_id);

  CloseResourceManager();


}  /*t9882*/

#undef MUX_NODE_PIN

#undef LOW_BOUNDS

#undef HIGH_BOUNDS

#undef MIN_TEST_VOLTAGE

#undef OLYM_CH1_CURR //added by chuanho, 15jun2012



#define MUX_NODE_PIN    2

#define LOW_BOUNDS      (-17.60) // this value should easily trip ov;

#define HIGH_BOUNDS     (-13.00) // this value should NEVER trip ov;

#define OLYM_LOW_BOUNDS      (17.60) // added by chuanho, 18jul2011

#define OLYM_HIGH_BOUNDS     (13.00) // added by chuanho, 18jul2011



#define MIN_TEST_VOLTAGE (-10.0)  //intermediate value; remarked by chuanho, 18jul2011

#define OLYM_MIN_TEST_VOLTAGE (10.0)  //added by chuanho, 15jun2012

#define OLYM_CH1_CURR 500e-03 //added by chuanho, 15jun2012

#define OLYM_CH1_OCP  1.0e+00 //added by chuanho, 15jun2012

/* static variables for t9883: */

struct LOC_t9883

{

  int mod_id;

  int slot;

  int receiver;

};



static void close_the_relays___(struct LOC_t9883 * LINK)

{

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KDC_RLYS[LINK->receiver - 'A']);

      /* Xn connect relay */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KX_RLYS[LINK->receiver - 'A']);

      /* Xn to receiver relay */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_RCVR_RLYS[MUX_NODE_PIN]

             [LINK->receiver - 'A']);

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KR_RLYS[LINK->receiver - 'A']);

      /* receiver connect relay */

  vth_mtmr_wait(LINK->mod_id, 0.001); // mjs - added 1ms for relay close time

}



void t9883(int mod_id_, int slot_)

{

  /* This is a window version of the overvoltage test. The specs for the Edge

     driver say it should trip overvoltage @  +/-14.5v w/ +/-.5v accuracy.

     This test does not find the exact value, only verifies that it will trip

     and clear.  The customer receiver spec is +/-12.0v.

  */



  struct LOC_t9883 V;



  a_word_type this_word;

  test_result_type result;

  V.mod_id = mod_id_;

  V.slot = slot_;



  int high_trip;

  int low_trip;



  vth_mod_init(V.mod_id);

  vth_ainit(V.mod_id);

  OpenResourceManager();
  open_33250A(V.mod_id);
  //open_mux(V.mod_id);
  open_34980A(V.mod_id);

  /*New Olympus codes*/
  //Added by chuanho, 15jun2012
  if(check_syn_model())
  {
	  open_olym(V.mod_id);
	  dconnect_olym_ch1_af2(V.mod_id);
  }
  /*End of new codes*/
  else
  {
	  open_syn(V.mod_id);
      setup_syn_hv(V.mod_id, true);         // need high voltage capabilities;
	  setup_syn(V.mod_id, DC, 0, 0.0, 0.0);
	  setup_syn_off(V.mod_id, 0.0);
	  connect_syn_out_af2(V.mod_id);
  }


  vth_aclose_rly(V.mod_id, K764);   // connects AF2 to A_XL

  vth_aclose_rly(V.mod_id, K751);   // connects A_XL to S


  for (V.receiver = 'A'; V.receiver <= 'H'; V.receiver++)   //S bus to Xn busses

    vth_aclose_rly(V.mod_id, AIM_X_S_RLYS[V.receiver - 'A']);





  for (V.receiver = 'A'; V.receiver <= 'H'; V.receiver++)

  {  //for receiver

    /*New Olympus codes*/
	if(check_syn_model())
	{
		setup_olym_ch1_volt(V.mod_id, OLYM_MIN_TEST_VOLTAGE);
		setup_olym_ch1_curr(V.mod_id, OLYM_CH1_CURR);
		setup_olym_ch1_ocp(V.mod_id, OLYM_CH1_OCP);
		enable_olym_ch1_out(V.mod_id);
		reverse_olym_ch1_out(V.mod_id);
	}
	/*End of new codes*/
	else
		setup_syn_off(V.mod_id, MIN_TEST_VOLTAGE);

    close_the_relays___(&V);

    vth_mtmr_wait(V.mod_id, 0.100);  //increased time to 100ms to give syn extra time

    high_trip = 0;

    low_trip = 0;

    //------------------------------------------------------------------------------------

      if(check_syn_model())
		  setup_olym_ch1_volt(V.mod_id, OLYM_HIGH_BOUNDS);
	  else
		  setup_syn_off(V.mod_id, HIGH_BOUNDS);

      vth_mtmr_wait(V.mod_id, 0.050);  //increased time to 50ms to give syn extra time

      vth_prd_stat(V.mod_id, V.slot);  // check status of error register



      if (get_result_nolog(V.mod_id, &result))

      { //get_result_nolog

        this_word.data = result.UU.Integer_result;

        if (P_getbits_US(this_word.bits, 5, 0) == 0)

        { //limit_occurred

           high_trip = 1;

           vth_pinit(V.mod_id, V.slot);

           vth_pdrv_clr_err(V.mod_id, V.slot);

        } //limit_occurred

      } //get_result_nolog

    //------------------------------------------------------------------------------------

      if(check_syn_model())
		  setup_olym_ch1_volt(V.mod_id, OLYM_LOW_BOUNDS);
	  else
		  setup_syn_off(V.mod_id, LOW_BOUNDS);

      vth_mtmr_wait(V.mod_id, 0.050);  //increased time to 50ms to give syn extra time

      vth_prd_stat(V.mod_id, V.slot);  // check status of error register



      if (get_result_nolog(V.mod_id, &result))

      { //get_result_nolog

        this_word.data = result.UU.Integer_result;

        if (P_getbits_US(this_word.bits, 5, 0) == 0)

        { //limit_occurred

           low_trip = 1;

           vth_pinit(V.mod_id, V.slot);

           vth_pdrv_clr_err(V.mod_id, V.slot);

        } //limit_occurred

      } //get_result_nolog

    //------------------------------------------------------------------------------------

    result.Measurement_class = CLASS_REAL;

    if ( (high_trip == 0) && (low_trip == 1) )

      result.UU.Real_result = -14.5;  //arbitrary pass value

    else

      result.UU.Real_result = 0;     //arbitrary fail value

    log_result(V.mod_id, result);



  }  //for receiver



  vth_ainit(V.mod_id);

  close_34980A(V.mod_id);//close_mux(V.mod_id);

  if(check_syn_model())
  {
	  close_olym(V.mod_id);
      disable_olym_ch1_out(V.mod_id);
	  close_33250A(V.mod_id);
  }
  else
	  close_syn(V.mod_id);

  CloseResourceManager();



}  /*t9883*/

#undef MUX_NODE_PIN

#undef LOW_BOUNDS

#undef HIGH_BOUNDS

#undef MIN_TEST_VOLTAGE

#undef OLYM_CH1_CURR //added by chuanho, 18jul2011



void t9900(int mod_id, int slot)

{

  /* [TWINPIN] Series Xn Bus Resistance */



  /* This test uses the identical code as the PINCARD test 9200.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9200   */

  /* Call test 9200, but log the results against t9900 */

  t9200(mod_id, slot);



}  /* t9900 */



void t9901(int mod_id, int slot)

{

  /* [TWINPIN] Series XG Bus Resistance */



  /* This test uses the identical code as the PINCARD test 9201.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9201   */

  /* Call test 9201, but log the results against t9901 */

  t9201(mod_id, slot);



}  /* t9901 */



void t9905(int mod_id, int slot)

{

  /* [TWINPIN]  Diagnostic version of T9909 */



  /* This test uses the identical code as the PINCARD test 9205.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9209   */

  /* Call test 9205, but log the results against t9905 */

  t9205(mod_id, slot);



}  /* t9905 */



void t9906(int mod_id, int slot)

{

  /* [TWINPIN]  Diagnostic version of T9907 */



  /* This test uses the identical code as the PINCARD test 9206.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9207   */

  /* Call test 9206, but log the results against t9906 */

  t9206(mod_id, slot);



}  /* t9906 */



void t9907(int mod_id, int slot)

{

  /* [TWINPIN]  Parallel Xn Bus Resistance */



  /* This test uses the identical code as the PINCARD test 9207.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9207   */

  /* Call test 9207, but log the results against t9907 */

  t9207(mod_id, slot);



}  /* t9907 */



void t9909(int mod_id, int slot)

{

  /* [TWINPIN]  Parallel Xn Bus Resistance */



  /* This test uses the identical code as the PINCARD test 9209.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9209   */

  /* Call test 9209, but log the results against t9909 */

  t9209(mod_id, slot);



}  /* t9909 */





void t9910(int mod_id, int slot)

{

  /* [TWINPIN]  Series Xn Bus Inductance */



  /* This test uses the identical code as the PINCARD test 9210.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9210   */

  /* Call test 9210, but log the results against t9910 */

  t9210(mod_id, slot);



}  /* t9910 */



void t9911(int mod_id, int slot)

{

  /* [TWINPIN]  Series XG/XGL/DRa Bus Inductance */



  /* This test uses the identical code as the PINCARD test 9211.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9211   */

  /* Call test 9211, but log the results against t9911 */

  t9211(mod_id, slot);



}  /* t9911 */



void t9917(int mod_id, int slot)

{

  /* [TWINPIN]  Parallel Xn Bus Resistance to Ground */



  /* This test uses the identical code as the PINCARD test 9217.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9217   */

  /* Call test 9217, but log the results against t9917 */

  t9217(mod_id, slot);



}  /* t9917 */



void t9920(int mod_id, int slot)

{

  /* [TWINPIN] Parallel Xn Bus Capacitance */



  /* This test uses the identical code as the PINCARD test 9220.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore different limits.            */

  /* For path and test descriptions, see the documentation for test: 9220   */

  /* Call test 9220, but log the results against t9920 */

  t9220(mod_id, slot);



}  /* t9920 */



void t9921(int mod_id, int slot)

{

  /* [TWINPIN] Parallel XG Bus Capacitance */



  /* This test uses the identical code as the PINCARD test 9221.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore different limits.            */

  /* For path and test descriptions, see the documentation for test: 9221   */

  /* Call test 9221, but log the results against t9921 */

  t9221(mod_id, slot);



}  /* t9921 */



void t9930(int mod_id, int slot)

{

  /* [TWINPIN] Parallel XG Bus Capacitance */



  /* This test uses the identical code as the PINCARD test 9230.  It is     */

  /* identified as a separate test because the PINCARD and TWINPIN cards    */

  /* will have different results and therefore may have different limits.   */

  /* For path and test descriptions, see the documentation for test: 9230   */

  /* Call test 9230, but log the results against t9930 */

  t9230(mod_id, slot);



}  /* t9930 */



#define NUM_LOOPS       10   /* buzz loops/relay */

typedef int loop_type__;

/* #define PRECLOSURES     5 */



/* static variables for t9995: */

struct LOC_t9189

{

  int mod_id;

  int slot;

};



static void buzz_that_relay__(int relay, struct LOC_t9189 *LINK)

{

  loop_type__ bloop;



  for (bloop = 1; bloop <= NUM_LOOPS; bloop++)

  {  /* for bloop */

    vth_aclose_rly(LINK->mod_id, K820);

    vth_rlc_setup(LINK->mod_id, 0, 0.1, 0.0, 0.001, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,

              -5.0, 0.001);



       //old??vth_rlc_setup(mod_id, 0, 0.1, 0, 0.001, 1, 1, 0, 2, 0, 0, 0, 0, 0, 0,

       //        -5.0, 0.001);



    vth_aopen_rly(LINK->mod_id, K814);

    vth_mtmr_wait(LINK->mod_id, 0.2e-3);



    vth_aclose_rly(LINK->mod_id, K813);   /*connect DETLO with I bus*/

    vth_aclose_rly(LINK->mod_id, K816);   /*connect G bus with I bus*/

    vth_mtmr_wait(LINK->mod_id, 0.2e-3);



/* This pre-close section was commented out 7/1/99 per WA's investigation

   of potentially false failures of buzz relays.  OK'ed with Dave Crook.

   -- mjs

   Fixed this to actually open close relay once each loop instead of not

   opening/closing it at all  10/19/99 --gpm

*/



/*

     for (int extra_loop = 1; extra_loop <= PRECLOSURES; extra_loop++)

    {

*/

      vth_popen_rly(LINK->mod_id, LINK->slot, relay);

      vth_mtmr_wait(LINK->mod_id, 0.08e-3);

      vth_pclose_rly(LINK->mod_id, LINK->slot, relay);

      vth_mtmr_wait(LINK->mod_id, 0.08e-3);

/*    }

*/



    vth_mtmr_wait(LINK->mod_id, 2.0e-3);

    vth_resistor_meas(LINK->mod_id, 0, 1);

    vth_get_abs_resistor_result(LINK->mod_id);

  }  /* for bloop */



}  /* buzz_that_relay */

/* #undef PRECLOSURES */



//------------------------------------------------------------------------------

void t9995(int mod_id, int slot)

{

  /* Buzz test for relays */



  /* Modified version of buzz t9188.  This version adds a pre-buzz loop */

  /* before each measurement is taken.  8-22-94 Wally Austin */



  /*From t3240 in DGN ~/sccs/dgn/src/tests/mt3000.p , modified to replace */

  /*buzz testing in production JRM - 11/18/92*/

  /*This is prepared for the test with/without CD fixture. If this test was failed*/

  /*without CD fixture, the driver relay or kgl will also be suspected.*/



  /*isolates mint relays on the receiver -- 72 relays total*/

  /*assumption = t3210(kxn and kdcn relays test) tested good*/

  struct LOC_t9189 V;

  int mux;

  int channel;

  boolean virgin_card;

  boolean virgin_channel;





  V.mod_id = mod_id;

  V.slot = slot;

  /*PIN relay exercise*/

  display_prompt("extra loop", true);



  buzz_prelays(V.mod_id, V.slot);



  /*initialization*/

  vth_aunpowered_init(V.mod_id);

  vth_ameas_set_averaging(V.mod_id, 1);



  virgin_card = true;



  /*setup XL and XG bus relays*/

  vth_pclose_rly(V.mod_id, V.slot, KDCG);

  vth_pclose_rly(V.mod_id, V.slot, KDCL);

  vth_aclose_rly(V.mod_id, K753);   /*connect L bus with mother board*/

  vth_aclose_rly(V.mod_id, K752);   /*connect G bus with XG bus*/

  vth_aclose_rly(V.mod_id, K754);   /*connect L bus with XL bus*/



  for (channel = 'A'; channel <= 'H'; channel++)

  {  /*of for channel*/

    virgin_channel = true;

    /*close PIN card under test relays*/

    vth_pclose_rly(V.mod_id, V.slot, PIN_KDC_RLYS[channel - 'A']);

    vth_pclose_rly(V.mod_id, V.slot, PIN_KX_RLYS[channel - 'A']);

    vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[channel - 'A']);



    /*setup AIM - SUBMUX S bus connection with Xn bus*/

    vth_aclose_rly(V.mod_id, AIM_X_S_RLYS[channel - 'A']);



    for (mux = 0; mux <= 8; mux++)   /*for each mux node relay pair */

    {  /* for mux */

      /*setup AIM relays*/

      vth_aclose_rly(V.mod_id, K820);   /*connect L bus with DETLO*/



      /*close PIN card under test relays*/

      vth_pclose_rly(V.mod_id, V.slot, PIN_DRVR_RLYS[mux][channel - 'A']);

      vth_pclose_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[mux][channel - 'A']);



      if (virgin_card)

      {  /* buzz once per card relays */

        buzz_that_relay__(KDCG, &V);

        buzz_that_relay__(KDCL, &V);

        virgin_card = false;

      }  /* buzz once per card relays */



      if (virgin_channel)

      {  /* buzz once per channel relays */

        buzz_that_relay__(PIN_KDC_RLYS[channel - 'A'], &V);

        buzz_that_relay__(PIN_KX_RLYS[channel - 'A'], &V);

        buzz_that_relay__(PIN_KGL_RLYS[channel - 'A'], &V);

        virgin_channel = false;

      }  /* buzz once per channel relays */



      buzz_that_relay__(PIN_DRVR_RLYS[mux][channel - 'A'], &V);

      buzz_that_relay__(PIN_RCVR_RLYS[mux][channel - 'A'], &V);



      vth_popen_rly(V.mod_id, V.slot, PIN_DRVR_RLYS[mux][channel - 'A']);

      vth_popen_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[mux][channel - 'A']);



    }  /* for mux */



    /*initialize all PIN and AIM relays*/

    vth_aopen_rly(V.mod_id, AIM_X_S_RLYS[channel - 'A']);

    vth_popen_rly(V.mod_id, V.slot, PIN_KDC_RLYS[channel - 'A']);

    vth_popen_rly(V.mod_id, V.slot, PIN_KX_RLYS[channel - 'A']);

    vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[channel - 'A']);



  }  /*for channel*/



}  /* t9995 */

#undef NUM_LOOPS



#define OSC_CHANNEL     1

#define COUNT           5

/* Parallel scope load        */

#define SCOPE_LOAD_RESISTANCE  50

#define RESOLUTION      1.0e-03

#define CHANNEL_RANGE   4.0e+00

#define CHANNEL_OFFSET  (-1.5e+00)

#define TRIGGER_LEVEL   (-1.5e+00)

#define TIMEBASE_RANGE  20e-06

#define TIMEBASE_OFFSET  6e-06

#define CALIBRATION_VOLTAGE  (-2.85e+00)

#define TIMEOUT         5.0e-03

#define INPUT_FREQ      20e+06

#define AVERAGING       true

#define DMM_FUNCTION    DCV

#define TIMEBASE_MODE   AUTOMATIC

#define PEAK            NEG   /*   Which peak to measure?    */

void t9997(int mod_id, int slot)

{

  /* MAXIMUM DRIVE CURRENT (SINKING >500mA) */

  double osc_actual_voltage;

  /*   Calibration for o'scope   */

  double offset;

  /* Calculated from measured V  */

  double drive_current;

  double tclk_freq;



  test_result_type result;

  int driver;





  open_dmm(mod_id);

  open_osc(mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);



  setup_osc_channel(mod_id, OSC_CHANNEL, SCOPE_LOAD_RESISTANCE, CHANNEL_RANGE,

                    CHANNEL_OFFSET);

  setup_osc_trigger(mod_id, OSC_CHANNEL, PEAK, TRIGGER_LEVEL, AVERAGING);

  setup_osc_timebase(mod_id, AVERAGING, COUNT, TIMEBASE_RANGE,

                     TIMEBASE_OFFSET, TIMEBASE_MODE);



  offset = offset_calibration(mod_id, CALIBRATION_VOLTAGE);



  setup_dmm(mod_id, DMM_FUNCTION, CALIBRATION_VOLTAGE, RESOLUTION);



  connect_osc_ch1_af2(mod_id);



  vth_pclose_rly(mod_id, slot, KDCL);   /* XL common relay  */

  vth_aclose_rly(mod_id, K753);   /* XL to AF2 relay  */

  vth_aclose_rly(mod_id, K764);   /* XL to AF2 relay  */



  vth_pclose_rly(mod_id, slot, KDCG);   /* XG common relay  */

  vth_aclose_trp_chg_rly(mod_id);   /* Aim load common  */

  vth_aclose_rly(mod_id, K756);   /* 5 ohm load relay */



  for (driver = 'A'; driver <= 'H'; driver++)

    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);



  tclk_freq = INPUT_FREQ / 64;

  vth_mtclk_clk_20m_mux(mod_id);

  vth_mtclk_set_freq(mod_id, INPUT_FREQ, tclk_freq);



  break_test_for_function_step_compatability(mod_id);



  for (driver = 'A'; driver <= 'H'; driver++)

  {  /*for driver*/

    sales_reset_sequencer(mod_id, slot);

#include "t9141_vml.cmd"

    vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

    sales_start_sequencer(mod_id, slot);



    osc_actual_voltage = 0.0;

    trigger_osc(mod_id, OSC_CHANNEL, AVERAGING);

    measure_osc(mod_id, VMAX, PEAK);

    if (get_result_nolog(mod_id, &result))

      osc_actual_voltage = result.UU.Real_result;

    else

      display_prompt("No result in result log (Expected osc volt)", true);



    vth_mseq_stop(mod_id, TIMEOUT);

    vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);



    osc_actual_voltage -= offset;

    osc_actual_voltage *= load_5_ohm_ratio[mod_id][slot - 1][driver - 'A'];

    drive_current = fabs(osc_actual_voltage / load_5_ohm_resistance[mod_id]

                         [slot - 1][driver - 'A']);

    result.Measurement_class = CLASS_REAL;

    result.UU.Real_result = drive_current;

    log_result(mod_id, result);



  }  /*for driver*/





  vth_pinit(mod_id, slot);

  vth_ainit(mod_id);



  close_dmm(mod_id);

  close_osc(mod_id);

  close_34980A(mod_id);//close_mux(mod_id);



}  /*t9997*/

#undef OSC_CHANNEL

#undef COUNT

#undef SCOPE_LOAD_RESISTANCE

#undef RESOLUTION

#undef CHANNEL_RANGE

#undef CHANNEL_OFFSET

#undef TRIGGER_LEVEL

#undef TIMEBASE_RANGE

#undef TIMEBASE_OFFSET

#undef CALIBRATION_VOLTAGE

#undef TIMEOUT

#undef INPUT_FREQ

#undef AVERAGING

#undef DMM_FUNCTION

#undef TIMEBASE_MODE

#undef PEAK



#define BLANK_LINE      ' '

#define NUM_LOOPS       150

#define NUM_DATA        100

/* FOR THE DETECTOR */

#define AVERAGES        1

/* NOT IMPLEMENTED */

#define DIGITAL         0

/* DETECTOR TIMEOUT */

#define TIMEOUT         1.0

/* limit multiplier */

#define FACTOR          0.2

/* ground reference */

#define TEST_VOLTAGE_REF  0.0



/* static variables for t9998: */

struct LOC_t9998

{

  int mod_id;

  double test_voltage;

};



static double get_detector_result(struct LOC_t9998 * LINK)

{

  double detector_result;

  test_result_type result;



  detector_result = -999.0;

  vth_afunc_meas(LINK->mod_id);

  vth_get_afunc_result(LINK->mod_id);

  /* get_afunc_result returns 2 values: reading and detector status */

  if (get_result_nolog(LINK->mod_id, &result))

    detector_result = result.UU.Real_result;

  if (!get_result_nolog(LINK->mod_id, &result))

    display_prompt("No result in result log (Expected DET stat)", true);

  return detector_result;

}  /* get_detector_result */



static boolean within_limits(double value, double value_expected, struct LOC_t9998 * LINK)

{

  double spread;

  double low_limit;

  double high_limit;



  spread = LINK->test_voltage - TEST_VOLTAGE_REF;

  low_limit = value_expected - spread * FACTOR;

  high_limit = value_expected + spread * FACTOR;

  if (value >= low_limit && value <= high_limit)

    return true;

  else

    return false;

}  /* within_limits */



void t9998(int mod_id_, int slot)

{

  /* Relay Welding Investigation Test */

  struct LOC_t9998 V;



  boolean exit_status;

  boolean fails_consistently;

  int loop;

  int data_point;

  int current_pos;

  int next;

  int num_failed_loops;

  double disch_volt;

  double chg_volt;

  double sum_disch;

  double sum_chg;

  double cap_value;

  double avg_value;

  char buffer[81];

  char s[81];

  FILE *single_chan;

  FILE *dual_chan;

  char str1[256];

  char *str2;



  V.mod_id = mod_id_;

  dual_chan = NULL;

  single_chan = NULL;

  break_test_for_function_step_compatability(V.mod_id);



  display_prompt("\007", true);   /* '#7' should cause a beep */

  sprintf(str1, "%c", BLANK_LINE);

  display_prompt(str1, true);

  display_prompt("Test voltage for the relay welding test? :", false);

  get_user_answer(buffer, 80);

  V.test_voltage = strtod(buffer, &str2);

  next = str2 - buffer + 1;

  sprintf(str1, "%c", BLANK_LINE);

  display_prompt(str1, true);

  display_prompt("Capacitor value for the relay welding test? :", false);

  get_user_answer(buffer, 80);

  cap_value = strtod(buffer, &str2);

  next = str2 - buffer + 1;

  sprintf(str1, "%c", BLANK_LINE);

  display_prompt(str1, true);



  display_prompt("Hookup capacitor and resistor as shown below.", true);

  sprintf(str1, "%c", BLANK_LINE);

  display_prompt(str1, true);

  display_prompt("  Mint Pin 1 --->---|---------|      Mint Pin 21 --->---|---------|", true);

  display_prompt("                    |         |                         |         |", true);

  display_prompt("   Relay Under      |         |        Relay Under      |         |", true);

  display_prompt("   Test = K1A       |         |        Test = K1C       |         |", true);

  display_prompt("                    |       ____ -                      |       ____ -",true);

  display_prompt("                    |       |  |                        |       |  |", true);

  display_prompt("                    -  C1   |DC|                        -  C2   |DC|", true);

  display_prompt("                    -       |__|                        -       |__|", true);

  display_prompt("                    |         |  +                      |         |  +",true);

  display_prompt("                    |         |                         |         |", true);

  display_prompt("                    |         |                         |         |", true);

  display_prompt("                    |    R1   |                         |    R2   |", true);

  display_prompt("  Mint Pin 3 --->---|---^^^^--|      Mint Pin 22 --->---|---^^^^--|", true);

  sprintf(str1, "%c", BLANK_LINE);

  display_prompt(str1, true);

  display_prompt("Then press <Return> or <Enter>:", false);

  get_user_answer(buffer, 80);

  break_test_for_function_step_compatability(V.mod_id);



  /* Close the "always closed" detector relays in the measurement path */

  vth_aclose_rly(V.mod_id, K820);   /* DETLO to L BUS RELAY */

  vth_aclose_rly(V.mod_id, K857);   /* L to GROUND RELAY    */

  vth_aclose_rly(V.mod_id, K812);   /* DETHI to I BUS RELAY */

  vth_aclose_rly(V.mod_id, K816);   /* I BUS to G BUS RELAY */

  vth_aclose_rly(V.mod_id, K752);   /* G to XG BUS RELAY    */



  /* Close the "always closed" pin card relays in the charge/discharge path */

  vth_pclose_rly(V.mod_id, slot, KDCG);   /* XG to XGL BUS RELAY  */



  /* Single channel case relays: use channel A */

  vth_pclose_rly(V.mod_id, slot, KGLA);   /* XGL to DR0 BUS RELAY */

  vth_pclose_rly(V.mod_id, slot, K2A);   /* DR0 to Mint Pin #3   */



  /* Do setup for detector */

  vth_dcv_setup(V.mod_id, 0, AVERAGES, DIGITAL, V.test_voltage, TIMEOUT);



  /* Verify cap/resistor is connected to single channel case */

  display_prompt("Verifying the single chan external components...", false);

  /* Verify the cap has charged up: ie. cap/resistor/ps are connected properly */

  chg_volt = get_detector_result(&V);

  exit_status = false;



  if (within_limits(chg_volt, V.test_voltage, &V))

    display_prompt("Looking Good...", false);

  else

  {

    display_prompt("\007", true);   /* '#7' should cause a beep */

    display_prompt("Cannot verify connection.", true);

    display_prompt("Recheck connections and re-run the test.", true);

    exit_status = true;

  }



  if (!exit_status)

  {  /* Verify the cap gets discharged (at least once) */

    vth_pclose_rly(V.mod_id, slot, K1A);   /* DR0 to Mint Pin #1   */

    vth_mtmr_wait(V.mod_id, 0.001);   /* Wait for discharge: fast! */

    disch_volt = get_detector_result(&V);



    if (within_limits(disch_volt, TEST_VOLTAGE_REF, &V))

      display_prompt("Verified.", true);

    else

    {

      display_prompt("\007", true);   /* '#7' should cause a beep */

      display_prompt("Cannot verify discharge.", true);

      exit_status = true;

    }

  }  /* Verify the cap gets discharged (at least once) */



  vth_popen_rly(V.mod_id, slot, KGLA);

  vth_popen_rly(V.mod_id, slot, K1A);

  vth_popen_rly(V.mod_id, slot, K2A);



  if (!exit_status)

  {  /* Verify cap/resistor is connected to dual channel case */

    display_prompt("Verifying the dual chan external components...", false);

    /* Verify the dual ch cap has charged up: ie. cap/resistor/ps are connected */

    /* Dual channel case relays: use channels C and D */

    vth_pclose_rly(V.mod_id, slot, KGLC);   /* XGL to DR2 BUS RELAY */

    vth_pclose_rly(V.mod_id, slot, KGLD);   /* XGL to DR3 BUS RELAY */

    vth_pclose_rly(V.mod_id, slot, K1D);   /* DR3 to Mint Pin #22  */



    chg_volt = get_detector_result(&V);



    if (within_limits(chg_volt, V.test_voltage, &V))

      display_prompt("Looking Good...", false);

    else

    {

      display_prompt("\007", true);   /* '#7' should cause a beep */

      display_prompt("Cannot verify connection.", true);

      display_prompt("Recheck connections and re-run the test.", true);

      exit_status = true;

    }



    if (!exit_status)

    {  /* Verify the cap gets discharged (at least once) */

      vth_pclose_rly(V.mod_id, slot, K1C);   /* DR2 to Mint Pin #21   */

      vth_mtmr_wait(V.mod_id, 0.001);   /* Wait for discharge: fast! */

      disch_volt = get_detector_result(&V);



      if (within_limits(disch_volt, TEST_VOLTAGE_REF, &V))

        display_prompt("Verified.", true);

      else

      {

        display_prompt("\007", true);   /* '#7' should cause a beep */

        display_prompt("Cannot verify discharge.", true);

        exit_status = true;

      }

    }  /* Verify the dual ch cap gets discharged (at least once) */

  }  /* Verify cap/resistor is connected to dual channel case */



  vth_popen_rly(V.mod_id, slot, KGLC);

  vth_popen_rly(V.mod_id, slot, KGLD);

  vth_popen_rly(V.mod_id, slot, K1D);

  vth_popen_rly(V.mod_id, slot, K1C);



  if (!exit_status)

  {  /* Do the bulk of the test */

      single_chan = fopen("single_channel", "w");



    if (single_chan == NULL)

    {

      display_prompt("The 'single_channel' file was not found.  Please have sys admin replace.", true);

      _EscIO(FileNotFound);

    }



    fprintf(single_chan, "Single Channel Test Voltage =%7.2f\n", V.test_voltage);

    fprintf(single_chan, "Single Channel Capacitor Value =%11.8f\n", cap_value);



    /* First test single channel for relay welding */

    vth_pclose_rly(V.mod_id, slot, KGLA);

    vth_pclose_rly(V.mod_id, slot, K2A);



    loop = 1;

    num_failed_loops = 0;

    fails_consistently = false;



    while (loop <= NUM_LOOPS && !fails_consistently)

    {  /* loop */

      sum_disch = 0.0;

      sum_chg = 0.0;



      for (data_point = 1; data_point <= NUM_DATA; data_point++)

      /* avg this number of points */

      {  /* for data_point */

        /* K1A completes the discharge path by effectively shorting out the cap */

        vth_pclose_rly(V.mod_id, slot, K1A);   /* DR0 to Mint Pin #1   */

        vth_mtmr_wait(V.mod_id, 0.001);   /* Wait for discharge: fast! */

        /* Measure the detector voltage to verify the relay closed */

        disch_volt = get_detector_result(&V);   /* Expecting ~ 0 volts here */

        /* Opening K1A allows the capacitor to charge (through the resistor) */

        vth_popen_rly(V.mod_id, slot, K1A);

        /* Allow the capacitor to charge. We want at least 3 time    constants */

        /* for charge. With the volt/capacitors values of interest, 100mS  (or */

        /* 10 cycles per second) is the longer of the waits.                   */

        vth_mtmr_wait(V.mod_id, 0.1);

        /* Measure the detector voltage to verify the relay opened */

        chg_volt = get_detector_result(&V);

            /* Expecting ~ test_volts here */



        sum_disch += disch_volt;

        sum_chg += chg_volt;

      }  /* data_points */



      /* put the results in a file */

      avg_value = sum_disch / NUM_DATA;

      fprintf(single_chan, "%5d   DISCHARGE =%7.2f", loop, avg_value);

      avg_value = sum_chg / NUM_DATA;

      fprintf(single_chan, "  CHARGE =%7.2f\n", avg_value);

      /* also display the data point */

      *s = '\0';

      strcat(s, "Single Channel: Loop ");

      current_pos = mtd_strlen(s) + 1;

      sprintf(s + current_pos - 1, "%5d", loop);

      next = mtd_strlen(s) + 1;

      strcat(s, "   K1A Open Result: ");

      current_pos = mtd_strlen(s) + 1;

      sprintf(s + current_pos - 1, "%5.2f", chg_volt);

      next = mtd_strlen(s) + 1;

      strcat(s, "   K1A Closed Result: ");

      current_pos = mtd_strlen(s) + 1;

      sprintf(s + current_pos - 1, "%5.2f", disch_volt);

      next = mtd_strlen(s) + 1;

      display_prompt(s, true);



      if (!within_limits(avg_value, V.test_voltage, &V))

        num_failed_loops++;



      if (num_failed_loops > 10)

        fails_consistently = true;



      loop++;

    }  /* loop */



    vth_popen_rly(V.mod_id, slot, KGLA);

    vth_popen_rly(V.mod_id, slot, K2A);

    vth_popen_rly(V.mod_id, slot, K1A);



    if (single_chan != NULL)

      fclose(single_chan);



    single_chan = NULL;



    dual_chan = fopen("dual_channel", "w");



    if (dual_chan == NULL)

    {

      display_prompt("The 'dual_channel' file was not found.  Please have sys admin replace.", true);

      _EscIO(FileNotFound);

    }



    fprintf(dual_chan, "Test Voltage =%7.2f\n", V.test_voltage);

    fprintf(dual_chan, "Capacitor Value =%11.8f\n", cap_value);



    vth_pclose_rly(V.mod_id, slot, KGLC);

    vth_pclose_rly(V.mod_id, slot, KGLD);

    vth_pclose_rly(V.mod_id, slot, K1D);



    /* Next test dual channel for relay welding */

    loop = 1;

    num_failed_loops = 0;

    fails_consistently = false;



    while (loop <= NUM_LOOPS && !fails_consistently)

    {  /* loop */

      sum_disch = 0.0;

      sum_chg = 0.0;



      for (data_point = 1; data_point <= NUM_DATA; data_point++)

      /* avg this number of points */

      {  /* for data_point */

        /* K1C completes the discharge path by effectively shorting out the cap */

        vth_pclose_rly(V.mod_id, slot, K1C);   /* DR2 to Mint Pin #21  */

        vth_mtmr_wait(V.mod_id, 0.001);   /* Wait for discharge: fast! */

        /* Measure the detector voltage to verify the relay closed */

        disch_volt = get_detector_result(&V);   /* Expecting ~ 0 volts here */

        /* Opening K1C allows the capacitor to charge (through the resistor) */

        vth_popen_rly(V.mod_id, slot, K1C);

        /* Allow the capacitor to charge. We want at least 3 time    constants */

        /* for charge. With the volt/capacitors values of interest, 100mS  (or */

        /* 10 cycles per second) is the longer of the waits.                   */

        vth_mtmr_wait(V.mod_id, 0.1);

        /* Measure the detector voltage to verify the relay opened */

        chg_volt = get_detector_result(&V);

            /* Expecting ~ test_volts here */



        sum_disch += disch_volt;

        sum_chg += chg_volt;

      }  /* data_points */



      /* put the results in a file */

      avg_value = sum_disch / NUM_DATA;

      fprintf(dual_chan, "%5d   DISCHARGE =%7.2f", loop, avg_value);

      avg_value = sum_chg / NUM_DATA;

      fprintf(dual_chan, "   CHARGE =%7.2f\n", avg_value);

      /* also display the data point */

      *s = '\0';

      strcat(s, "Dual Channel: Loop ");

      current_pos = mtd_strlen(s) + 1;

      sprintf(s + current_pos - 1, "%5d", loop);

      next = mtd_strlen(s) + 1;

      strcat(s, "   K1C Open Result: ");

      current_pos = mtd_strlen(s) + 1;

      sprintf(s + current_pos - 1, "%5.2f", chg_volt);

      next = mtd_strlen(s) + 1;

      strcat(s, "   K1C Closed Result: ");

      current_pos = mtd_strlen(s) + 1;

      sprintf(s + current_pos - 1, "%5.2f", disch_volt);

      next = mtd_strlen(s) + 1;

      display_prompt(s, true);



      if (!within_limits(avg_value, V.test_voltage, &V))

        num_failed_loops++;



      if (num_failed_loops > 10)

        fails_consistently = true;



      loop++;

    }  /* loop */



    if (dual_chan != NULL)

      fclose(dual_chan);



    dual_chan = NULL;

    /* Next test 2 channels for relay welding */

    vth_popen_rly(V.mod_id, slot, KGLC);

    vth_popen_rly(V.mod_id, slot, KGLD);

    vth_popen_rly(V.mod_id, slot, K1D);

    vth_popen_rly(V.mod_id, slot, K1C);

  }  /* Do the bulk of the test */



}  /* t9998 */

#undef BLANK_LINE

#undef NUM_LOOPS

#undef NUM_DATA

#undef AVERAGES

#undef DIGITAL

#undef TIMEOUT

#undef FACTOR

#undef TEST_VOLTAGE_REF



void t9999(int /*mod_id*/, int /*slot*/)

{

 char *finish_date; /* finish date and time of sales */

 int hours, minutes, seconds;

 int total_time;    /* total elapsed time in seconds */

 time_t now;

 FILE *timefile;



  timefile = fopen("sales_time", "a");

  assert(timefile);



  /* unlock fixture  */

//  if (!vth_fxt_safe(VTH_READ_FIXTURE_SAFE))

//    display_prompt("Press footswitch to (un)lock fixture",true);



  vth_cfxt_unlock();

  display_prompt("\nSales has Completed.", true);

  total_time = static_cast<int>(difftime(time(NULL), start_time));



  fprintf(timefile, "-----------------------------------------------------------------------\n");

  fprintf(timefile, "START time for Sales:   %s", start_date);

  now = time(NULL);

  finish_date = ctime(&now);

  fprintf(timefile, "FINISH time for Sales:  %s", finish_date);



  hours = total_time / 3600;

  minutes = (total_time % 3600) / 60;

  seconds = (total_time % 3600) % 60;

  fprintf(timefile, "   TOTAL Elapsed time for Sales = %3d hours, %3d minutes, %3d seconds\n", hours, minutes, seconds);

  fprintf(timefile, "-----------------------------------------------------------------------\n");



  display_prompt("* START time for Sales: ", true);

  display_prompt(start_date, true);

  display_prompt("* FINISH time for Sales: ", true);

  display_prompt(finish_date, true);



  fclose(timefile);

}  /* t9999 */

