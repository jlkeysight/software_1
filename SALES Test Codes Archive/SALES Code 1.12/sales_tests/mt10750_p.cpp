#include <time.h>
#include <p2c/p2c.h>
#include <assert.h>
#include "mt10750_p.h"
#include "../../../vmc/src/vmc.h"
#include "digital_const_p.h"
#include "../tests/mt0500.h"
#include "../parms_p.h"

// ---  TEST 10800 - CARIBOU DRV OUT V RANGE AND ACCY (5.0V DRIVE HIGH)  ---
// ---  DONE
#define RANGE           5.0   /* for dmm */
#define MUX_NODE_PIN    2   /* 2 of 0 thru 8 */
#define RESOLUTION      1.0e-03   /* for dmm */
#define VOLTAGE_LEVEL   5.0e+00   /* drive hi level */
#define DMM_FUNCTION    DCV
void t10800(int mod_id, int slot)
{
  /* DR OUT V RANGE AND ACCURACY (5v DRIVE HI) */

  /* For path and test descriptions, see the documentation for test: 9100 */
  int channel_no;
  int driver;
  int page;

  test_result_type result;

  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  connect_dmm_pin(mod_id, slot);
  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);
  close_pin_gnd_relays(mod_id, slot);

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for driver */
    channel_no = driver - 'A' + 8 * page;

    // Becoming standard way to open/close compatibility relays
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pdis_32chan(mod_id, slot);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    }

//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    vth_pdrv_set_hi(mod_id, slot, channel_no, VOLTAGE_LEVEL);
    vth_pfc_set_sttc_tsp(mod_id, slot, channel_no, 0);
    vth_pfc_set_sttc_dd(mod_id, slot, channel_no, 1);

//  }  /* for driver */

//  for (driver = 'A'; driver <= 'H'; driver++)
//  {  /* for driver */

    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);
    vth_mseq_neg_cpu_tsp(mod_id);
    measure_dmm(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected DMM reading)", true);
    vth_mseq_ass_cpu_tsp(mod_id);
    vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    vth_pdis_32chan(mod_id, slot);
    vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

  }  /* for driver */

 }  /* for page */

  vth_pdis_32chan(mod_id, slot);
  vth_pinit(mod_id, slot);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t10800*/
#undef RANGE
#undef MUX_NODE_PIN
#undef RESOLUTION
#undef VOLTAGE_LEVEL
#undef DMM_FUNCTION

// ---  TEST 10802 - CARIBOU DRV OUT V RANGE AND ACCY (-3.5V DRIVE LOW)  ---
// ---  DONE
#define RANGE           5.0   /* for dmm */
#define MUX_NODE_PIN    2   /* 2 of 0 thru 8 */
#define RESOLUTION      1.0e-03   /* for dmm */
#define VOLTAGE_LEVEL   (-3.5e+00)   /* drive lo level */
#define DMM_FUNCTION    DCV
void t10802(int mod_id, int slot)
{
  /* DR OUT V RANGE AND ACCURACY (-3.5V DRIVE LO) */

  /* For path and test descriptions, see the documentation for test: 9100 */
  int channel_no;
  int driver;
  int page;

  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  connect_dmm_pin(mod_id, slot);
  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);
  close_pin_gnd_relays(mod_id, slot);

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for driver */
    channel_no = driver - 'A' + 8 * page;

    // Becoming standard way to open/close compatibility relays
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pdis_32chan(mod_id, slot);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    }

//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    vth_pdrv_set_lo(mod_id, slot, channel_no, VOLTAGE_LEVEL);
    vth_pfc_set_sttc_tsp(mod_id, slot, channel_no, 0);
    vth_pfc_set_sttc_dd(mod_id, slot, channel_no, 0);

//  }  /* for driver */

//  for (driver = 'A'; driver <= 'H'; driver++)
//  {  /* for driver */

    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);
    vth_mseq_neg_cpu_tsp(mod_id);
    measure_dmm(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected DMM reading)", true);
    vth_mseq_ass_cpu_tsp(mod_id);
    vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    vth_pdis_32chan(mod_id, slot);
    vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

  }  /* for driver */

 }  /* for page */

  vth_pdis_32chan(mod_id, slot);
  vth_pinit(mod_id, slot);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t10802*/
#undef RANGE
#undef MUX_NODE_PIN
#undef RESOLUTION
#undef VOLTAGE_LEVEL
#undef DMM_FUNCTION

// ---  TEST 10804 - CARIBOU GROUND DIFFERENCE TEST  ---
// ---  DONE
#define RANGE           1.0   /* for dmm */
#define RESOLUTION      1.0e-06   /* for dmm */
#define DMM_FUNCTION    DCV
//---------------------------------------------------------------------------
void t10804(int mod_id, int slot)
{
  /*GROUND DIFFERENCE TEST*/
  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  vth_pdis_32chan(mod_id, slot);

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);

  connect_dmm_pin(mod_id, slot);
  dconnect_minus_swd_gnd(mod_id);
  close_pin_gnd_relays(mod_id, slot);
  vth_aclose_rly(mod_id, K761);   /*SWDGND relay*/
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  measure_dmm(mod_id);
  if (!get_result_log(mod_id, &result))
    display_prompt("No result in result log (Expected DMM reading)", true);

  vth_pinit(mod_id, slot);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t10804*/
#undef RANGE
#undef RESOLUTION
#undef DMM_FUNCTION

// ---  TEST 10806 - CARIBOU DRIVER OUTPUT SLEW RATE (100V/uS)  ---
// ---  UNTESTED
#define TIMEOUT         1.0e-03
#define INPUT_FREQ      20e+06
#define TCLK_FREQ       20e+06
//-------------------------------------------------------------------------
void t10806(int mod_id, int slot)
{
  /* [CARIBOU] DR OUTPUT SLEW RATE (100V/uS) */

  /* For path and test descriptions, see the documentation for test: 9105 */
  double slew_time;
  double slew_voltage;
  double slew_rate;
  int driver;
  int page;

  test_result_type result;

  double voltage_max;
  double voltage_min;
  double vmax;
  double vmin;

  open_cntr(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  connect_cntr_ch1_af2(mod_id);

  vth_aclose_rly(mod_id, K753);   // XL to AF2 relay
  vth_aclose_rly(mod_id, K764);   // XL to AF2 relay
  vth_pclose_rly(mod_id, slot, KDCL);   // XL common connection

//  for (driver = 'A'; driver <= 'H'; driver++)
//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

  vth_mtclk_clk_20m_mux(mod_id);
  vth_mtclk_set_freq(mod_id, INPUT_FREQ, TCLK_FREQ);

  break_test_for_function_step_compatability(mod_id);
#include "t10806_vml.cmd"

  for (int edge = POS; edge >= NEG; edge = --edge)
  {  // for edge

  // setup counter
    vth_mhpib_wr(mod_id, 4, ":INP:ATT 10");    // x10 attenuation????????
    vth_mhpib_wr(mod_id, 4, ":INP:COUP DC");    // DC input coupling
    vth_mhpib_wr(mod_id, 4, ":INP:IMP 50");     // 50 ohm input impedance

   for (page = 0; page <= 1; page++)
   {  /* for page */

    for (driver = 'A'; driver <= 'H'; driver++)
    {  /* for driver */
      slew_rate = 0.0;
      slew_voltage = 0.0;
      slew_time = 1.0;

      sales_reset_sequencer(mod_id, slot);

    // Trying pmap_rly 0 and 1 for simplified relay control
    // If this works this can be simplified further using
    // vth_pmap_rly(mod_id, slot, page) instead of 0 or 1
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 1);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    }
//      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

      if (edge == POS)
	vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'RTIME'"); // rise time
      else
	vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'FTIME'"); // fall time

      vth_mhpib_wr(mod_id, 4, ":INIT:IMM"); // initiate measurement immediately
      sales_start_sequencer(mod_id, slot);
      slew_time = read_cntr(mod_id);
//      display_it(mod_id, "slew time = ", 0, slew_time);

       vth_mhpib_wr(mod_id, 4, ":CONF:MAX");
       vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'VOLT:MAX'");
       vth_mhpib_wr(mod_id, 4, ":INIT:IMM");
       vmax = read_cntr(mod_id);
//       display_it(mod_id, "vmax = ", 0, vmax);

       vth_mhpib_wr(mod_id, 4, ":CONF:MIN");
       vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'VOLT:MIN'");
       vth_mhpib_wr(mod_id, 4, ":INIT:IMM");
       vmin = read_cntr(mod_id);
//       display_it(mod_id, "vmin = ", 0, vmin);


       // we multiply by .1 because we need 10% of the voltage difference
       // since the voltage difference is taken with a 10% limit
       voltage_min = vmin + 0.1 * (vmax - vmin);
       voltage_max = vmax - 0.1 * (vmax - vmin);
       slew_voltage = voltage_max - voltage_min;
       //  display_it(mod_id, "slew_voltage = ", 0, slew_voltage);

      vth_mseq_stop(mod_id, TIMEOUT);

      vth_pdis_32chan(mod_id, slot);
      vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);


      if (slew_time == 0) slew_time = INT_MAX; /* To avoid divide by 0 */
      if (slew_time > 0)  slew_time *=(1000000);  /* Convert seconds to absolute value uS */
      if (slew_time <0)   slew_time *=(-1000000);  /* Convert seconds to absolute value uS */

      slew_rate = slew_voltage / slew_time;
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = slew_rate;
      log_result(mod_id, result);
    }  /* for driver */

   }  /* for page */

  }  /* for edge */


  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);

  close_cntr(mod_id);
  close_mux(mod_id);

}  /* t10806 */
#undef WHAT_STRING_
#undef TIMEOUT
#undef INPUT_FREQ
#undef TCLK_FREQ

// ---  TEST 10820 - CARIBOU DRIVER EDGE PLACEMENT ACCURACY AND SKEW  ---
// ---  DONE
typedef int delay_type_;
typedef double delay_array[2];
#define RISING_EDGE     1
#define TCLK_FREQ       5.0e+06
#define GEN_LOW         0.0e+00
#define GEN_HIGH_TERMINATED  3.0e+00
#define TIMEOUT         1.0e-03
const delay_array GEN_DELAY = {85e-09, 185e-09};
const delay_array PROG_DELAY = {0e-09, 100e-09};
//----------------------------------------------------------------------------
void t10820(int mod_id, int slot)
{
  /*DRIVER EDGE PLACEMENT ACCURACY AND SKEW*/
  int mapped_slot;
  delay_type_ offset_cal;
  delay_type_ programmed_delays;

  double drvr_delay_actual;
  double skew;

  double offset[2];
  double ch_delta['H' + 1 - 'A'][4];

  int edge;
  test_result_type result;
  int driver;
  int page;

  if (slot > 11)
    mapped_slot = slot - 11;
  else
    mapped_slot = slot;


  open_syn(mod_id);
  open_gen(mod_id);
  open_mux(mod_id);
  open_cntr(mod_id);

  // Initialize card to compatibility mode
  // Which of these does what we really want??
  vth_pwr_cell(mod_id, slot, PCOMPAT_DIS, 0x0);
  vth_pdis_32chan(mod_id, slot);

  connect_syn_out_gen_ext(mod_id);
  setup_gen_levels(mod_id, GEN_LOW, GEN_HIGH_TERMINATED);

  connect_gen_trg_cntr_start(mod_id);
  connect_gen_out_cntr_stop(mod_id);

  for (offset_cal = 0; offset_cal <= 1; offset_cal++)
  {  /*offset_cal*/
//    display_it(mod_id, "offset cal is = ", offset_cal, 0);
//    display_it(mod_id, "GEN_DELAY is = ", 0, GEN_DELAY[offset_cal]);
    setup_gen_trg_out_delay(mod_id, GEN_DELAY[offset_cal]);
    enable_gen_out(mod_id);
    trigger_cntr(mod_id);
    offset[offset_cal] = read_cntr_mean(mod_id);
    disable_gen_out(mod_id);
  }  /*offset_cal*/

  connect_gen_trg_clkrb(mod_id);
  connect_cntr_start_af2(mod_id);

  vth_mclose_rly(mod_id, MK2902);   /* clkrb path relay */
  vth_mclose_rly(mod_id, MK2903);   /* clkrsg path relay */
  vth_aclose_rly(mod_id, K753);   /* XL to AF2 path */
  vth_aclose_rly(mod_id, K764);   /* XL to AF2 path */

  // This was not changed since we want to close all of the
  // relays going to drive sense on both pages, A and C....
  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for */
    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
  }  /* for */
  vth_pclose_rly(mod_id, slot, KDCL);

  vth_mhsl_1_init_master(mod_id);
  all_modules_but_me(mod_id, &vth_mhsl_1_init_passive);
  vth_mhsl_2_init_master(mod_id);
  all_modules_but_me(mod_id, &vth_mhsl_2_init_passive);
  vth_mdig_set_clkrb_master(mod_id, TCLK_FREQ, TCLK_FREQ, RISING_EDGE);

 for (page = 0; page <= 1; page++)
 { /* for page */

  for (programmed_delays = 1; programmed_delays <= 2; programmed_delays++)
  {  /* for programmed_delays */
    setup_gen_trg_out_delay(mod_id, GEN_DELAY[(programmed_delays-1)]);
    if (programmed_delays == 1)
    {
#include "t10820_nd.cmd"
    }
    else
    {
#include "t10820_d.cmd"
    }

    for (int slope = NEG; slope <= POS; slope = ++slope)
    {  /*for slope*/
      if (slope == POS)
        edge = RISING;
      else
        edge = FALLING;
      setup_cntr_trigger_slopes(mod_id, slope, POS);
      for (driver = 'A'; driver <= 'H'; driver++)
      {  /* for driver */
        enable_gen_out(mod_id);

        // Becoming standard way to open/close compatibility relays
        if (page == 0)
        {
          vth_pen_32chan(mod_id, slot);
          vth_pmap_rly(mod_id, slot, 0);
          vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
        }
        else  /* page == 1 */
        {
          vth_pdis_32chan(mod_id, slot);
          vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
          vth_pen_32chan(mod_id, slot);
          vth_pmap_rly(mod_id, slot, 0);
          vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
        }

        sales_reset_sequencer(mod_id, slot);
        sales_start_sequencer(mod_id, slot);

        trigger_cntr(mod_id);
        drvr_delay_actual = read_cntr_mean(mod_id);

        ch_delta[driver - 'A']
          [slope] = offset[(programmed_delays-1)] + clkrb_delay[mod_id] -
            drvr_delay_actual - af2_start_delay[mod_id] -
            af2_system_delay[mod_id] - slot_delay[mapped_slot - 1]
            [edge] - channel_delay[driver - 'A'] - PROG_DELAY[(programmed_delays-1)];

/*
        display_it(mod_id, "edge = ", edge, 0);
        display_it(mod_id, "programmed_delays -1= ", programmed_delays-1, 0);
        display_it(mod_id, "mapped_slot -1 = ", slot-1, 0);
        display_it(mod_id, "offset[programmed_delays-1] = ", 0, offset[programmed_delays-1]);
        display_it(mod_id, "clkrb_delay[mod_id] = ", 0, clkrb_delay[mod_id]);
        display_it(mod_id, "drvr_delay_actual = ", 0, drvr_delay_actual);
        display_it(mod_id, "af2_start_delay[mod_id]", 0, af2_start_delay[mod_id]);
        display_it(mod_id, "af2_system_delay[mod_id] = ", 0,  af2_system_delay[mod_id] );
        display_it(mod_id, "slot_delay[mapped_slot - 1][edge] = ", 0, slot_delay[mapped_slot - 1][edge]);
        display_it(mod_id, "channel_delay[driver - 'A'] = ", 0, channel_delay[driver - 'A']);
        display_it(mod_id, "PROG_DELAY[(programmed_delays-1)] = ", 0, PROG_DELAY[(programmed_delays-1)]);
        */

        result.Measurement_class = CLASS_REAL;
        result.UU.Real_result = ch_delta[driver - 'A'][slope];
        log_result(mod_id, result);
        vth_mseq_stop(mod_id, TIMEOUT);
        disable_gen_out(mod_id);

        // Added pdis_32chan so both A and C open!
        vth_pdis_32chan(mod_id, slot);
        vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
      }  /* for driver */

    }  /* for slope */

    for (driver = 'A'; driver <= 'H'; driver++)
    {  /*CALCULATE SKEW*/
      skew = ch_delta[driver - 'A'][POS] - ch_delta[driver - 'A'][NEG];
      result.UU.Real_result = skew;
      log_result(mod_id, result);

    }  /*CALCULATE SKEW*/



  }  /* for programmed_delays */

 }  /* for page */

  vth_pinit(mod_id, slot);
  vth_minit(mod_id);
  vth_ainit(mod_id);
  close_syn(mod_id);
  close_gen(mod_id);
  close_mux(mod_id);
  close_cntr(mod_id);

}  /*t10820*/
#undef RISING_EDGE
#undef TCLK_FREQ
#undef GEN_LOW
#undef GEN_HIGH_TERMINATED
#undef TIMEOUT

// ---  TEST 10821 - CARIBOU DRIVER OUT V RANGE AND ACCY (-2.5V DRIVE HIGH)  ---
// ---  DONE
#define RANGE           5.0   /* for dmm */
#define MUX_NODE_PIN    2   /* 2 of 0 thru 8 */
#define RESOLUTION      1.0e-03   /* for dmm */
#define VOLTAGE_LEVEL   (-2.5e+00)   /* drive hi level */  //cafe
#define OTHER_LEVEL     (-3.5e+00)   /* drive lo level */
#define DMM_FUNCTION    DCV
void t10821(int mod_id, int slot)
{
  /* [CARIBOU] DR OUT V RANGE AND ACCURACY (-2.5v DRIVE HI) */
  /* limits = -2.5v +/- 99mV */

  /* mjs -- this test is exactly the same as t9801 except for one labeled
          voltage definition above.  it has been cloned for CAFE */

  /* This test is a modified version of the PINCARD test 9101.  It has been */
  /* modified for TWINPIN cards because there must be at least 0.5V         */
  /* separation between drive hi and drive low levels.  For path and test   */
  /* descriptions, see the documentation for test: 9100.                    */

  int channel_no;
  int driver;
  int page;
  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  // Which of these does what we really want??
  vth_pdis_32chan(mod_id, slot);

  connect_dmm_pin(mod_id, slot);
  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);
  close_pin_gnd_relays(mod_id, slot);


 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for driver */
    channel_no = driver - 'A' + 8 * page;

    // Becoming standard way to open/close compatibility relays
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pdis_32chan(mod_id, slot);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    }

//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    vth_pdrv_set_lo(mod_id, slot, channel_no, OTHER_LEVEL);
    vth_pdrv_set_hi(mod_id, slot, channel_no, VOLTAGE_LEVEL);
    vth_pfc_set_sttc_tsp(mod_id, slot, channel_no, 0);
    vth_pfc_set_sttc_dd(mod_id, slot, channel_no, 1);

//  }  /* for driver */

//  for (driver = 'A'; driver <= 'H'; driver++)
//  {  /* for driver */

    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);
    vth_mseq_neg_cpu_tsp(mod_id);
    measure_dmm(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected DMM reading)", true);
    vth_mseq_ass_cpu_tsp(mod_id);
    vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    vth_pdis_32chan(mod_id, slot);
    vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

  }  /* for driver */

 } /* for page */

  vth_pdis_32chan(mod_id, slot);
  vth_pinit(mod_id, slot);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t10821*/
#undef RANGE
#undef MUX_NODE_PIN
#undef RESOLUTION
#undef VOLTAGE_LEVEL
#undef OTHER_LEVEL
#undef DMM_FUNCTION

// ---  TEST 10823 - CARIBOU DRIVE OUTPUT RANGE AND ACCURACY (4.0V DRIVE LOW)  ---
// ---  UNTESTED
#define RANGE           5.0   /* for dmm */
#define MUX_NODE_PIN    2   /* 2 of 0 thru 8 */
#define RESOLUTION      1.0e-03   /* for dmm */
#define VOLTAGE_LEVEL   4.0e+00   /* drive lo level */  //cafe
#define OTHER_LEVEL     5.0e+00   /* drive hi level */
#define DMM_FUNCTION    DCV
void t10823(int mod_id, int slot)
{
  /* [CARIBOU] DR OUT V RANGE AND ACCURACY (4.0V DRIVE LO) */
  /* limits = 4.0v +/- 99mV */

  /* mjs -- this test is exactly the same as t9801 except for one labeled
          voltage definition above.  it has been cloned for CAFE */

  /* This test is a modified version of the PINCARD test 9103.  It has been */
  /* modified for TWINPIN cards because there must be at least 0.5V         */
  /* separation between drive hi and drive low levels.  For path and test   */
  /* descriptions, see the documentation for test: 9100.                    */
  int channel_no;
  int driver;
  int page;
  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  connect_dmm_pin(mod_id, slot);
  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);
  close_pin_gnd_relays(mod_id, slot);

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for driver */
    channel_no = driver - 'A' + 8 * page;

    // Becoming standard way to open/close compatibility relays
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pdis_32chan(mod_id, slot);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    }

//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    vth_pdrv_set_hi(mod_id, slot, channel_no, OTHER_LEVEL);
    vth_pdrv_set_lo(mod_id, slot, channel_no, VOLTAGE_LEVEL);
    vth_pfc_set_sttc_tsp(mod_id, slot, channel_no, 0);
    vth_pfc_set_sttc_dd(mod_id, slot, channel_no, 0);

//  }  /* for driver */

//  for (driver = 'A'; driver <= 'H'; driver++)
//  {  /* for driver */

    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);
    vth_mseq_neg_cpu_tsp(mod_id);
    measure_dmm(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected DMM reading)", true);
    vth_mseq_ass_cpu_tsp(mod_id);
    vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    vth_pdis_32chan(mod_id, slot);
    vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

  }  /* for driver */

 }  /* for page */

  vth_pdis_32chan(mod_id, slot);
  vth_pinit(mod_id, slot);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t10823*/
#undef RANGE
#undef MUX_NODE_PIN
#undef RESOLUTION
#undef VOLTAGE_LEVEL
#undef OTHER_LEVEL
#undef DMM_FUNCTION

// ---  TEST 10829 - TRI-STATE HIGH EDGE PLACEMENT ACCY AND SKEW  ---
// ---  UNTESTED
/* pin 4 of 0-8 */
#define MUX_NODE_PIN    4
#define TCLK_FREQ       5.0e+06
#define GEN_LOW         0.0e+00
#define GEN_HIGH_TERMINATED  3.0e+00
#define GEN_DELAY_      100e-09
/* system actual t9 value */
#define T0_ACTUAL       165.9e-9
#define TIMEOUT         1.0e-03
#define DC_SRC_VOLTAGE  7.0e+00
#define PROG_DELAY_     0.0e-09
//-----------------------------------------------------------------------
void t10829(int mod_id, int slot)
{
  /*TRI-STATE HIGH EDGE PLACEMENT ACCURACY AND SKEW*/
  int mapped_slot;

  double drvr_delay_actual;
  double offset;
  double skew;

  double ch_delta['H' + 1 - 'A'][4];

  int edge;
  test_result_type result;
  int driver;
  int page;

  if (slot > 11)
    mapped_slot = slot - 11;
  else
    mapped_slot = slot;

  open_syn(mod_id);
  open_gen(mod_id);
  open_mux(mod_id);
  open_cntr(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  connect_syn_out_gen_ext(mod_id);
  setup_gen_levels(mod_id, GEN_LOW, GEN_HIGH_TERMINATED);

  connect_gen_trg_cntr_start(mod_id);
  connect_gen_out_cntr_stop(mod_id);

  setup_gen_trg_out_delay(mod_id, GEN_DELAY_);

  enable_gen_out(mod_id);
  trigger_cntr(mod_id);
  offset = read_cntr_mean(mod_id);
  disable_gen_out(mod_id);

  connect_gen_trg_clkrb(mod_id);
  connect_cntr_start_af2(mod_id);

  vth_mclose_rly(mod_id, MK2902);   /* clkrb path relay */
  vth_mclose_rly(mod_id, MK2903);   /* clkrsg path relay */
  vth_aclose_rly(mod_id, K753);   /* XL to AF2 path */
  vth_aclose_rly(mod_id, K764);   /* XL to AF2 path */

//  for (driver = 'A'; driver <= 'H'; driver++)
//  {  /* for */
//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
//    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[driver - 'A']);
//    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);
//    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][driver - 'A']);
//  }  /* for */

  vth_pclose_rly(mod_id, slot, KDCL);
  close_pin_gnd_relays(mod_id, slot);

  vth_mtclk_clkrb_mux(mod_id);
  vth_mtclk_sel_ris_edge(mod_id);
  vth_mtclk_set_freq(mod_id, TCLK_FREQ, TCLK_FREQ);
  vth_mtclk_set_adv_time(mod_id, T0_ACTUAL);

  vth_aclose_rly(mod_id, K829);
  vth_asrc_dis_30ma_ilim(mod_id);
  vth_asrc_set_dc_ampl(mod_id, DC_SRC_VOLTAGE);

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (int slope = NEG; slope <= POS; ++slope)
  {  /* for slope */
    if (slope == POS)
      edge = RISING;
    else
      edge = FALLING;

    setup_cntr_trigger_slopes(mod_id, slope, POS);

    for (driver = 'A'; driver <= 'H'; driver++)
    {  /* for driver */

      // Trying pmap_rly 0 and 1 for simplified relay control
      // vth_pmap_rly(mod_id, slot, page) instead of 0 or 1
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, page);

      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][driver - 'A']);

      enable_gen_out(mod_id);

      sales_reset_sequencer(mod_id, slot);

#include "t10829_vml.cmd"
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[driver - 'A']);
      vth_aclose_rly(mod_id, AIM_X_S_RLYS[driver - 'A']);
      vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
      vth_afc_en_src_out(mod_id);

      sales_start_sequencer(mod_id, slot);

      trigger_cntr(mod_id);
      vth_mtmr_wait(mod_id, 0.100);
      drvr_delay_actual = read_cntr_mean(mod_id);

      ch_delta[driver - 'A']
        [slope] = offset + clkrb_delay[mod_id] - drvr_delay_actual -
                  af2_start_delay[mod_id] - af2_system_delay[mod_id] -
                  slot_delay[mapped_slot - 1]
                  [edge] - channel_delay[driver - 'A'] - PROG_DELAY_;

      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = ch_delta[driver - 'A'][slope];
      log_result(mod_id, result);
      vth_mseq_stop(mod_id, TIMEOUT);
      disable_gen_out(mod_id);
      vth_afc_dis_src_out(mod_id);
      vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_KDC_RLYS[driver - 'A']);
      vth_aopen_rly(mod_id, AIM_X_S_RLYS[driver - 'A']);

      vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_KX_RLYS[driver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    }  /* for driver */

  }  /* for slope */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for */
    skew = ch_delta[driver - 'A'][POS] - ch_delta[driver - 'A'][NEG];
    result.UU.Real_result = skew;
    log_result(mod_id, result);
  }  /* for */

 }  /* for page */

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (int slope = NEG; slope <= POS; ++slope)
  {  /* for slope */
    if (slope == POS)
      edge = RISING;
    else
      edge = FALLING;

    setup_cntr_trigger_slopes(mod_id, slope, POS);
    for (driver = 'A'; driver <= 'H'; driver++)
    {  /* for driver */

      // Trying pmap_rly 0 and 1 for simplified relay control
      // vth_pmap_rly(mod_id, slot, page) instead of 0 or 1
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, page);

      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][driver - 'A']);

      enable_gen_out(mod_id);

      sales_reset_sequencer(mod_id, slot);

#include "t10829_5uS.cmd"
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[driver - 'A']);
      vth_aclose_rly(mod_id, AIM_X_S_RLYS[driver - 'A']);
      vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
      vth_afc_en_src_out(mod_id);

      sales_start_sequencer(mod_id, slot);

      trigger_cntr(mod_id);
      vth_mtmr_wait(mod_id, 0.100);
      drvr_delay_actual = read_cntr_mean(mod_id);

      ch_delta[driver - 'A']
        [slope] = offset + clkrb_delay[mod_id] - drvr_delay_actual -
                  af2_start_delay[mod_id] - af2_system_delay[mod_id] -
                  slot_delay[mapped_slot - 1]
                  [edge] - channel_delay[driver - 'A'] - PROG_DELAY_;

      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = ch_delta[driver - 'A'][slope];
      log_result(mod_id, result);
      vth_mseq_stop(mod_id, TIMEOUT);
      disable_gen_out(mod_id);
      vth_afc_dis_src_out(mod_id);
      vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_KDC_RLYS[driver - 'A']);
      vth_aopen_rly(mod_id, AIM_X_S_RLYS[driver - 'A']);

      vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_KX_RLYS[driver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][driver - 'A']);

    }  /* for driver */

  }  /* for slope */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for */
    skew = ch_delta[driver - 'A'][POS] - ch_delta[driver - 'A'][NEG];
    result.UU.Real_result = skew;
    log_result(mod_id, result);
  }  /* for */

 }  /* for page */

  vth_pinit(mod_id, slot);
  vth_minit(mod_id);
  vth_ainit(mod_id);
  close_syn(mod_id);
  close_gen(mod_id);
  close_mux(mod_id);
  close_cntr(mod_id);

}  /*t10829*/
#undef MUX_NODE_PIN
#undef TCLK_FREQ
#undef GEN_LOW
#undef GEN_HIGH_TERMINATED
#undef GEN_DELAY_
#undef T0_ACTUAL
#undef TIMEOUT
#undef DC_SRC_VOLTAGE
#undef PROG_DELAY_

// ---  TEST 10830 - TRI-STATE LOW EDGE PLACEMENT ACCY AND SKEW  ---
// ---  IN PROGRESS
#define TCLK_FREQ       5.0e+06
#define GEN_LOW         0.0e+00
#define GEN_HIGH_TERMINATED  3.0e+00
#define GEN_DELAY_      100e-09
/* system actual t0 value */
#define T0_ACTUAL       165.9e-9
#define TIMEOUT         1.0e-03
#define PROG_DELAY_     0.0e-09
//------------------------------------------------------------------------------
void t10830(int mod_id, int slot)
{
  /*TRI-STATE LOW EDGE PLACEMENT ACCURACY AND SKEW*/
  int mapped_slot;

  double drvr_delay_actual;
  double offset;
  double skew;

  double ch_delta['H' + 1 - 'A'][4];

  int edge;
  test_result_type result;
  int driver;


  if (slot > 11)
    mapped_slot = slot - 11;
  else
    mapped_slot = slot;

  open_syn(mod_id);
  open_gen(mod_id);
  open_mux(mod_id);
  open_cntr(mod_id);

  connect_syn_out_gen_ext(mod_id);
  setup_gen_levels(mod_id, GEN_LOW, GEN_HIGH_TERMINATED);

  connect_gen_trg_cntr_start(mod_id);
  connect_gen_out_cntr_stop(mod_id);

  setup_gen_trg_out_delay(mod_id, GEN_DELAY_);

  enable_gen_out(mod_id);
  trigger_cntr(mod_id);
  offset = read_cntr_mean(mod_id);
  disable_gen_out(mod_id);

  connect_gen_trg_clkrb(mod_id);
  connect_cntr_start_af2(mod_id);

  vth_mclose_rly(mod_id, MK2902);   /* clkrb path relay */
  vth_mclose_rly(mod_id, MK2903);   /* clkrsg path relay */
  vth_aclose_rly(mod_id, K753);   /* XL to AF2 path */
  vth_aclose_rly(mod_id, K764);   /* XL to AF2 path */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for */
    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
  }  /* for */
  vth_pclose_rly(mod_id, slot, KDCL);

  vth_mtclk_clkrb_mux(mod_id);
  vth_mtclk_sel_ris_edge(mod_id);
  vth_mtclk_set_freq(mod_id, TCLK_FREQ, TCLK_FREQ);
  vth_mtclk_set_adv_time(mod_id, T0_ACTUAL);

  for (int slope = NEG; slope <= POS; ++slope)
  {  /* for slope */
    if (slope == POS)
      edge = RISING;
    else
      edge = FALLING;

    setup_cntr_trigger_slopes(mod_id, slope, POS);
    for (driver = 'A'; driver <= 'H'; driver++)
    {  /* for driver */
      enable_gen_out(mod_id);

      sales_reset_sequencer(mod_id, slot);

#include "t10830_vml.cmd"
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

      sales_start_sequencer(mod_id, slot);

      trigger_cntr(mod_id);
      drvr_delay_actual = read_cntr_mean(mod_id);

      ch_delta[driver - 'A']
        [slope] = offset + clkrb_delay[mod_id] - drvr_delay_actual -
                  af2_start_delay[mod_id] - af2_system_delay[mod_id] -
                  slot_delay[mapped_slot - 1]
                  [edge] - channel_delay[driver - 'A'] - PROG_DELAY_;

      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = ch_delta[driver - 'A'][slope];
      log_result(mod_id, result);
      vth_mseq_stop(mod_id, TIMEOUT);
      disable_gen_out(mod_id);
      vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

    }  /* for driver */


  }  /* for slope */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for */
    skew = ch_delta[driver - 'A'][POS] - ch_delta[driver - 'A'][NEG];
    result.UU.Real_result = skew;
    log_result(mod_id, result);
  }  /* for */

  /* NOW EXECUTE TO HARD DRIVING CODE - drive high for 5uS */
  for (slope = NEG; slope <= POS; ++slope)
  {  /* for slope */
    if (slope == POS)
      edge = RISING;
    else
      edge = FALLING;

    setup_cntr_trigger_slopes(mod_id, slope, POS);
    for (driver = 'A'; driver <= 'H'; driver++)
    {  /* for driver */
      enable_gen_out(mod_id);

      sales_reset_sequencer(mod_id, slot);

#include "t10830_5uS.cmd"
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

      sales_start_sequencer(mod_id, slot);

      trigger_cntr(mod_id);
      drvr_delay_actual = read_cntr_mean(mod_id);

      ch_delta[driver - 'A']
        [slope] = offset + clkrb_delay[mod_id] - drvr_delay_actual -
                  af2_start_delay[mod_id] - af2_system_delay[mod_id] -
                  slot_delay[mapped_slot - 1]
                  [edge] - channel_delay[driver - 'A'] - PROG_DELAY_;

      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = ch_delta[driver - 'A'][slope];
      log_result(mod_id, result);
      vth_mseq_stop(mod_id, TIMEOUT);
      disable_gen_out(mod_id);
      vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

    }  /* for driver */


  }  /* for slope */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for */
    skew = ch_delta[driver - 'A'][POS] - ch_delta[driver - 'A'][NEG];
    result.UU.Real_result = skew;
    log_result(mod_id, result);
  }  /* for */

  vth_pinit(mod_id, slot);
  vth_minit(mod_id);
  vth_ainit(mod_id);
  close_syn(mod_id);
  close_gen(mod_id);
  close_mux(mod_id);
  close_cntr(mod_id);

}  /*t10830*/
#undef TCLK_FREQ
#undef GEN_LOW
#undef GEN_HIGH_TERMINATED
#undef GEN_DELAY_
#undef T0_ACTUAL
#undef TIMEOUT
#undef PROG_DELAY_

// ---  TEST 10832 - CARIBOU DRIVER TRI-STATE LEAKAGE CURRENT  ---
// ---  UNTESTED
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
void t10832(int mod_id, int slot)
{
  /* [CARIBOU] DRIVER TRI-STATE LEAKAGE CURRENT */

  /* For path and test descriptions, see the documentation for test: 9132 */
  int driver_no;

  double drive_lo;
  double drive_hi;
  double driver_tri_state_voltage;
  double driver_tri_state_current;

  test_result_type result;
  int driver;
  int page;

  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);

  connect_dmm_pin(mod_id, slot);
  connect_500_ohm_load(mod_id);

  close_pin_gnd_relays(mod_id, slot);

//  for (driver = 'A'; driver <= 'H'; driver++)
//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

  for (int driver_level = ABOVE_; driver_level <= BELOW; ++driver_level)
  {  /* FOR EACH DRIVER LEVEL */
    drive_lo = DRIVE_LO_LEVEL[driver_level];
    drive_hi = DRIVE_HI_LEVEL[driver_level];

   for (page = 0; page <= 1; page++)
   {  /* for page */

    for (driver = 'A'; driver <= 'H'; driver++)
    {  /* for driver */

      // Becoming standard way to open/close compatibility relays
      if (page == 0)
      {
        vth_pen_32chan(mod_id, slot);
        vth_pmap_rly(mod_id, slot, 0);
        vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      }
      else  /* page == 1 */
      {
        vth_pdis_32chan(mod_id, slot);
//        vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
        vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
        vth_pen_32chan(mod_id, slot);
//        vth_pmap_rly(mod_id, slot, 0);
        vth_pmap_rly(mod_id, slot, 1);
//        vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
        vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      }

      driver_no = driver - 'A' + 8 * page;
      vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][driver - 'A']);
      vth_pdrv_set_lo(mod_id, slot, driver_no, drive_lo);
      vth_pdrv_set_hi(mod_id, slot, driver_no, drive_hi);
      vth_pfc_set_sttc_tsp(mod_id, slot, driver_no, 1);
      if (driver_level == ABOVE_ || driver_level == BETWEEN1)
        vth_pfc_set_sttc_dd(mod_id, slot, driver_no, 1);
      else
        vth_pfc_set_sttc_dd(mod_id, slot, driver_no, 0);
      vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);

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

      vth_pdis_32chan(mod_id, slot);
      vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

    }  /* for driver */

   }  /* for page */

  }  /* FOR DRIVER LEVEL */


  vth_pinit(mod_id, slot);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t10832 */
#undef ABOVE_
#undef BETWEEN1
#undef BELOW
#undef MUX_NODE_PIN
#undef RANGE
#undef RESOLUTION
#undef DRIVER_LOAD_RESISTANCE
#undef DMM_FUNCTION

// ---  TEST 10835 - CARIBOU DRIVER OUTPUT IMPEDENCE (SOURCING 700mA) ---
// ---  UNTESTED
#define SAMPLE_RANGE    10
#define SAMPLE_LEVEL    10
#define SAMPLE_SLOPE    POS
#define SAMPLE_INTERVAL  50.0e-9
/* 1 period + 800ns */
#define SAMPLE_DELAY    1.2808e-3
#define SAMPLE_SIZE     5
#define TIMEOUT         10e-03
#define INPUT_FREQ      20e+06
#define COUPLING        50
//------------------------------------------------------------------------
void t10835(int mod_id, int slot)
{
  /* [CARIBOU] DRIVER OUTPUT IMPEDANCE (SOURCING 700mA) */

  /* For path and test descriptions, see the documentation for test: 9135 */
  int i;
  arrform waveform;
  double volts_noload;
  double volts_load;
  /* Calculated from measured V's*/
  double delta_v_cal;
  /* Calculated from measured V's*/
  double delta_i_cal;
  /* Calculated from measured V's*/
  double driver_impedance;
  double tclk_freq;

  test_result_type result;
  int driver;
  int page;


  open_mux(mod_id);
  open_dmm50(mod_id);
//  vth_mtmr_wait(mod_id, 1);
//  vth_mhpib_wr(mod_id, 8, "RESET");

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  connect_dmm50_af2(mod_id);

  vth_pclose_rly(mod_id, slot, KDCL);   /* XL common relay  */
  vth_aclose_rly(mod_id, K753);   /* XL to AF2 relay  */
  vth_aclose_rly(mod_id, K764);   /* XL to AF2 relay  */
  vth_aclose_rly(mod_id, K756);   /* 5 ohm load relay */
  vth_aclose_trp_chg_rly(mod_id);   /* Aim load common  */

//  for (driver = 'A'; driver <= 'H'; driver++)
//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

  tclk_freq = INPUT_FREQ / 64;
  vth_mtclk_clk_20m_mux(mod_id);
  vth_mtclk_set_freq(mod_id, INPUT_FREQ, tclk_freq);

  break_test_for_function_step_compatability(mod_id);

  setup_ssdc50_fast(mod_id, SAMPLE_RANGE, SAMPLE_LEVEL, SAMPLE_SLOPE,
                    SAMPLE_INTERVAL, SAMPLE_DELAY, SAMPLE_SIZE);

 for (page = 0; page <= 1; page ++)
 {  /* for page */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /*for driver*/

    // Trying pmap_rly 0 and 1 for simplified relay control
    // If this works this can be simplified further using
    // vth_pmap_rly(mod_id, slot, page) instead of 0 or 1
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 1);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    }

//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    sales_reset_sequencer(mod_id, slot);
#include "t10835_vml.cmd"
//    vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    sales_start_sequencer(mod_id, slot);

    clear_dmm50_memory(mod_id);
    capture_ddm50(mod_id, SAMPLE_SIZE, waveform, LEVEL);
    volts_noload = 0.0;
    for (i = 0; i < SAMPLE_SIZE; i++)
      volts_noload += waveform[i] / SAMPLE_SIZE;
    show_value(mod_id, "volts_noload", volts_noload);

    vth_mseq_stop(mod_id, TIMEOUT);
    vth_pclose_rly(mod_id, slot, KDCG);   /* XG common relay  */
    sales_reset_sequencer(mod_id, slot);
#include "t10835_vml.cmd"
    sales_start_sequencer(mod_id, slot);

    clear_dmm50_memory(mod_id);
    // could potentially just use 3 individual readings
    // instead of using the capture sampling routine to
    // make 5 readings (overhead)  -mjs
    capture_ddm50(mod_id, SAMPLE_SIZE, waveform, LEVEL);
    volts_load = 0.0;
    for (i = 0; i < SAMPLE_SIZE; i++)
      volts_load += waveform[i] / SAMPLE_SIZE;
    show_value(mod_id, "volts_load", volts_load);

    vth_mseq_stop(mod_id, TIMEOUT);

    vth_pdis_32chan(mod_id, slot);
    vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    vth_popen_rly(mod_id, slot, KDCG);

    vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);

    delta_v_cal = volts_noload - volts_load;

    delta_i_cal = volts_load / load_5_ohm_resistance[mod_id][slot - 1]
                  [driver - 'A'] - volts_noload / COUPLING;


    if (delta_i_cal != 0)
      driver_impedance = delta_v_cal / delta_i_cal;
    else
      driver_impedance = 10.0;

    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = driver_impedance;
    log_result(mod_id, result);

  }  /*for driver*/

 }  /* for page */

  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);

  close_dmm50(mod_id);
  close_mux(mod_id);

}  /*t10835*/
#undef SAMPLE_RANGE
#undef SAMPLE_LEVEL
#undef SAMPLE_SLOPE
#undef SAMPLE_INTERVAL
#undef SAMPLE_DELAY
#undef SAMPLE_SIZE
#undef TIMEOUT
#undef INPUT_FREQ
#undef COUPLING

// ---  TEST 10836 - CARIBOU DRIVER OUTPUT IMPEDENCE (SINKING 500mA) ---
// ---  UNTESTED
#define SAMPLE_RANGE    10
#define SAMPLE_LEVEL    (-10)
#define SAMPLE_SLOPE    NEG
#define SAMPLE_INTERVAL  50.0e-9
/* 1 period + 800ns */
#define SAMPLE_DELAY    1.2808e-3
#define SAMPLE_SIZE     5
#define TIMEOUT         10e-03
#define INPUT_FREQ      20e+06
#define COUPLING        50
//------------------------------------------------------------------------
void t10836(int mod_id, int slot)
{
  /* [CARIBOU] DRIVER OUTPUT IMPEDANCE (SINKING 500mA) */

  /* For path and test descriptions, see the documentation for test: 9135 */
  int i;
  arrform waveform;
  double volts_noload;
  double volts_load;
  /* Calculated from measured V's*/
  double delta_v_cal;
  /* Calculated from measured V's*/
  double delta_i_cal;
  /* Calculated from measured V's*/
  double driver_impedance;
  double tclk_freq;

  test_result_type result;
  int driver;
  int page;


  open_mux(mod_id);
  open_dmm50(mod_id);
//  vth_mtmr_wait(mod_id, 1);
//  vth_mhpib_wr(mod_id, 8, "RESET");

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  connect_dmm50_af2(mod_id);

  vth_pclose_rly(mod_id, slot, KDCL);   /* XL common relay  */
  vth_aclose_rly(mod_id, K753);   /* XL to AF2 relay  */
  vth_aclose_rly(mod_id, K764);   /* XL to AF2 relay  */
  vth_aclose_rly(mod_id, K756);   /* 5 ohm load relay */
  vth_aclose_trp_chg_rly(mod_id);   /* Aim load common  */

//  for (driver = 'A'; driver <= 'H'; driver++)
//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

  tclk_freq = INPUT_FREQ / 64;
  vth_mtclk_clk_20m_mux(mod_id);
  vth_mtclk_set_freq(mod_id, INPUT_FREQ, tclk_freq);

  break_test_for_function_step_compatability(mod_id);

  setup_ssdc50_fast(mod_id, SAMPLE_RANGE, SAMPLE_LEVEL, SAMPLE_SLOPE,
                    SAMPLE_INTERVAL, SAMPLE_DELAY, SAMPLE_SIZE);

 for (page = 0; page <= 1; page ++)
 {  /* for page */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /*for driver*/

    // Trying pmap_rly 0 and 1 for simplified relay control
    // If this works this can be simplified further using
    // vth_pmap_rly(mod_id, slot, page) instead of 0 or 1
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 1);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    }

//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    sales_reset_sequencer(mod_id, slot);
#include "t10836_vml.cmd"
//    vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    sales_start_sequencer(mod_id, slot);

    clear_dmm50_memory(mod_id);
    capture_ddm50(mod_id, SAMPLE_SIZE, waveform, LEVEL);
    volts_noload = 0.0;
    for (i = 0; i < SAMPLE_SIZE; i++)
      volts_noload += waveform[i] / SAMPLE_SIZE;
    show_value(mod_id, "volts_noload", volts_noload);

    vth_mseq_stop(mod_id, TIMEOUT);
    vth_pclose_rly(mod_id, slot, KDCG);   /* XG common relay  */
    sales_reset_sequencer(mod_id, slot);
#include "t10836_vml.cmd"
    sales_start_sequencer(mod_id, slot);

    clear_dmm50_memory(mod_id);
    // could potentially just use 3 individual readings
    // instead of using the capture sampling routine to
    // make 5 readings (overhead)  -mjs
    capture_ddm50(mod_id, SAMPLE_SIZE, waveform, LEVEL);
    volts_load = 0.0;
    for (i = 0; i < SAMPLE_SIZE; i++)
      volts_load += waveform[i] / SAMPLE_SIZE;
    show_value(mod_id, "volts_load", volts_load);

    vth_mseq_stop(mod_id, TIMEOUT);

    vth_pdis_32chan(mod_id, slot);
    vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    vth_popen_rly(mod_id, slot, KDCG);

    vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);

    delta_v_cal = volts_noload - volts_load;

    delta_i_cal = volts_load / load_5_ohm_resistance[mod_id][slot - 1]
                  [driver - 'A'] - volts_noload / COUPLING;

    if (delta_i_cal != 0)
      driver_impedance = delta_v_cal / delta_i_cal;
    else
      driver_impedance = 10.0;

    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = driver_impedance;
    log_result(mod_id, result);

  }  /*for driver*/

 }  /* for page */

  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);

  close_dmm50(mod_id);
  close_mux(mod_id);

}  /*t10836*/
#undef SAMPLE_RANGE
#undef SAMPLE_LEVEL
#undef SAMPLE_SLOPE
#undef SAMPLE_INTERVAL
#undef SAMPLE_DELAY
#undef SAMPLE_SIZE
#undef TIMEOUT
#undef INPUT_FREQ
#undef COUPLING

// ---  TEST 10840 - CARIBOU MAX DRIVE CURRENT (SOURCING > 700mA) ---
// ---  UNTESTED
#define TIMEOUT         1.0e-01
#define INPUT_FREQ      20e+06
#define SAMPLE_RANGE    10
#define SAMPLE_LEVEL    10
#define SAMPLE_SLOPE    POS
#define SAMPLE_INTERVAL  100e-09
#define SAMPLE_DELAY    1.26e-3   /*about 1 period of 1.28ms */
#define SAMPLE_SIZE     256
void t10840(int mod_id, int slot)
{
  /* MAXIMUM DRIVE CURRENT (SOURCING > 700mA) */
  double dmm_actual_voltage;
  double five_ohm_equivalent;
  /* Calculated from measured V  */
  double drive_current;
  double tclk_freq;

  test_result_type result;
  int driver;
  int page;


  open_dmm50(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  /* setup dmm50 */

  connect_dmm50_af2(mod_id);

  vth_pclose_rly(mod_id, slot, KDCL);   /* XL common relay  */
  vth_aclose_rly(mod_id, K753);   /* XL to AF2 relay  */
  vth_aclose_rly(mod_id, K764);   /* XL to AF2 relay  */

  vth_pclose_rly(mod_id, slot, KDCG);   /* XG common relay  */
  vth_aclose_trp_chg_rly(mod_id);   /* Aim load common  */
  vth_aclose_rly(mod_id, K756);   /* 5 ohm load relay */

//  for (driver = 'A'; driver <= 'H'; driver++)
//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

  tclk_freq = INPUT_FREQ / 64;
  vth_mtclk_clk_20m_mux(mod_id);
  vth_mtclk_set_freq(mod_id, INPUT_FREQ, tclk_freq);
  break_test_for_function_step_compatability(mod_id);

  setup_ssdc50_fast(mod_id, SAMPLE_RANGE, SAMPLE_LEVEL, SAMPLE_SLOPE,
                    SAMPLE_INTERVAL, SAMPLE_DELAY, SAMPLE_SIZE);

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /*for driver*/
    sales_reset_sequencer(mod_id, slot);
#include "t10840_vml.cmd"


    // Trying pmap_rly 0 and 1 for simplified relay control
    // If this works this can be simplified further using
    // vth_pmap_rly(mod_id, slot, page) instead of 0 or 1
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 1);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    }

//    vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

    sales_start_sequencer(mod_id, slot);

    dmm_actual_voltage = 0.0;

    clear_dmm50_memory(mod_id);
    /* trigger_dmm50(mod_id);  */
    /* if this doesn't trigger, try using TARM SGL instead */
    vth_mhpib_wr(mod_id, 8, "TARM SGL");

    calculate_dmm50_stats(mod_id);

    read_dmm50_stats(mod_id, SALES_MAX);
    if (get_result_nolog(mod_id, &result))
      dmm_actual_voltage = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected dr out volt)", true);

    show_value(mod_id, "maximum", dmm_actual_voltage);

    vth_mseq_stop(mod_id, TIMEOUT);

    vth_pdis_32chan(mod_id, slot);
    vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

    five_ohm_equivalent = load_5_ohm_resistance[mod_id][slot - 1]
        [driver - 'A'] * 50 / (load_5_ohm_resistance[mod_id][slot - 1]
                               [driver - 'A'] + 50);
    show_value(mod_id, "5 ohm equivalent", five_ohm_equivalent);

    drive_current = dmm_actual_voltage / five_ohm_equivalent;
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = drive_current;
    log_result(mod_id, result);

  }  /*for driver*/

 }  /* for page */

  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);
  close_dmm50(mod_id);
  close_mux(mod_id);

}  /*t10840*/
#undef TIMEOUT
#undef INPUT_FREQ
#undef SAMPLE_RANGE
#undef SAMPLE_LEVEL
#undef SAMPLE_SLOPE
#undef SAMPLE_INTERVAL
#undef SAMPLE_DELAY
#undef SAMPLE_SIZE

// ---  TEST 10841 - CARIBOU MAX DRIVE CURRENT (SINKING > 500mA) ---
// ---  UNTESTED
#define TIMEOUT         5.0e-03
#define INPUT_FREQ      20e+06
#define SAMPLE_RANGE    10
#define SAMPLE_LEVEL    (-10)
#define SAMPLE_SLOPE    NEG
#define SAMPLE_INTERVAL  100e-09
#define SAMPLE_DELAY    1.26e-3
#define SAMPLE_SIZE     256
void t10841(int mod_id, int slot)
{
  /* MAXIMUM DRIVE CURRENT (SINKING >500mA) */
  double dmm_actual_voltage;
  double five_ohm_equivalent;
  /* Calculated from measured V  */
  double drive_current;
  double tclk_freq;
  test_result_type result;
  int driver;
  int page;

  open_dmm50(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  connect_dmm50_af2(mod_id);

  vth_pclose_rly(mod_id, slot, KDCL);   /* XL common relay  */
  vth_aclose_rly(mod_id, K753);   /* XL to AF2 relay  */
  vth_aclose_rly(mod_id, K764);   /* XL to AF2 relay  */

  vth_pclose_rly(mod_id, slot, KDCG);   /* XG common relay  */
  vth_aclose_trp_chg_rly(mod_id);   /* Aim load common  */
  vth_aclose_rly(mod_id, K756);   /* 5 ohm load relay */

//  for (driver = 'A'; driver <= 'H'; driver++)
//    vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

  tclk_freq = INPUT_FREQ / 64;
  vth_mtclk_clk_20m_mux(mod_id);
  vth_mtclk_set_freq(mod_id, INPUT_FREQ, tclk_freq);

  break_test_for_function_step_compatability(mod_id);

  setup_ssdc50_fast(mod_id, SAMPLE_RANGE, SAMPLE_LEVEL, SAMPLE_SLOPE,
                    SAMPLE_INTERVAL, SAMPLE_DELAY, SAMPLE_SIZE);

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (driver = 'A'; driver <= 'H'; driver++)
  {  /* for driver */
    sales_reset_sequencer(mod_id, slot);
#include "t10841_vml.cmd"

    // Trying pmap_rly 0 and 1 for simplified relay control
    // If this works this can be simplified further using
    // vth_pmap_rly(mod_id, slot, page) instead of 0 or 1
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 1);
      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
    }

//    vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

    sales_start_sequencer(mod_id, slot);

    dmm_actual_voltage = 0.0;

    clear_dmm50_memory(mod_id);
    vth_mhpib_wr(mod_id, 8, "TARM SGL");

    calculate_dmm50_stats(mod_id);

    read_dmm50_stats(mod_id, SALES_MIN);
    if (get_result_nolog(mod_id, &result))
      dmm_actual_voltage = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected dr out volt)", true);

    vth_mseq_stop(mod_id, TIMEOUT);


    vth_pdis_32chan(mod_id, slot);
    vth_popen_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);
    vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

    five_ohm_equivalent = load_5_ohm_resistance[mod_id][slot - 1]
        [driver - 'A'] * 50 / (load_5_ohm_resistance[mod_id][slot - 1]
                               [driver - 'A'] + 50);

    drive_current = fabs(dmm_actual_voltage / five_ohm_equivalent);
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = drive_current;
    log_result(mod_id, result);

  }  /* for driver */

 }  /* for page */

  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);
  close_dmm50(mod_id);
  close_mux(mod_id);

}  /*t10841*/
#undef TIMEOUT
#undef INPUT_FREQ
#undef SAMPLE_RANGE
#undef SAMPLE_LEVEL
#undef SAMPLE_SLOPE
#undef SAMPLE_INTERVAL
#undef SAMPLE_DELAY
#undef SAMPLE_SIZE

// ---  TEST 10847 - CARIBOU DRIVER POSITIVE OVERVOLTAGE  ---
// ---  IN PROGRESS
#define MUX_NODE_PIN    2
#define MIN_TEST_VOLTAGE  0.0 // from 5.5 to avoid hot-closing relays -- mjs
#define LOW_BOUNDS      7.054
#define HIGH_BOUNDS     8.800
/* static variables for t10847: */
struct LOC_t10847
{
  int mod_id;
  int slot;
  int driver;
};

static void close_the_relays(struct LOC_t10847 *LINK)
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


void t10847(int mod_id_, int slot_)
{
  /* [TWINPIN/FEZZIWIG] DR POSITIVE OV LIMIT ACCURACY */

  /* mjs - changed the search algorithm to a "thirded search"
     This test works for Monet and Fezziwig pincards.  The old
     test had holes and didn't work for the Fezziwig card */
  /* mjs - also added the driver output tri-state step to avoid
     contention and optimized some of the wait states based upon
     scope measurements made with WA 5/99 */

  struct LOC_t10847 V;
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

}  /* t10847 */
#undef MUX_NODE_PIN
#undef LOW_BOUNDS
#undef HIGH_BOUNDS
#undef MIN_TEST_VOLTAGE

// ---  TEST 10848 - CARIBOU DRIVER NEGATIVE OVERVOLTAGE  ---
// ---  IN PROGRESS
#define MUX_NODE_PIN    2
#define MIN_TEST_VOLTAGE  0.0 //changed to avoid hot-closing relays -- mjs
#define LOW_BOUNDS      (-6.938)
#define HIGH_BOUNDS     (-5.600)

/* static variables for t10848: */
struct LOC_t10848
{
  int mod_id;
  int slot;
  int driver;
} ;

static void close_the_relays_(struct LOC_t10848 * LINK)
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

void t10848(int mod_id_, int slot_)
{
  /* [TWINPIN/FEZZIWIG] DR NEGATIVE OV LIMIT ACCURACY */

  /* mjs - changed the search algorithm to a "thirded search"
     This test works for Monet and Fezziwig pincards.  The old
     test had holes and didn't work for the Fezziwig card */
  /* mjs - also added the driver output tri-state step to avoid
     contention and optimized some of the wait states based upon
     scope measurements made with WA 5/99 */

  struct LOC_t10848 V;
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

}  /*t10848*/
#undef MUX_NODE_PIN
#undef MIN_TEST_VOLTAGE
#undef LOW_BOUNDS
#undef HIGH_BOUNDS

// ---  TEST 10850 - CARIBOU RECEIVER VOLTAGE ACCY  ---
// ---  IN PROGRESS
#define DMM_FUNCTION    DCV
/* pin 2 of 0 through 8 */
#define MUX_NODE_PIN    2
/* where to load waveform */
#define BLOCK           0
/* 8k low pass filter */
#define FILTER          7
/* Maximum sc ram vector */
#define VECTOR_LENGTH   126
/* Total number of vectors */
//#define STEPS           12 //cafe only
#define STEPS           128  //orig
#define RANGE           10.0e+00
#define RESOLUTION      1.0e-03
#define INPUT_FREQ      20e+06
/*  orig*/
#define RECEIVER_RANGE_P_P  0.4e+00
#define DC_OFFSET_VOLTAGE  5.0e+00
/* richard
#define RECEIVER_RANGE_P_P  0.6e+00
#define DC_OFFSET_VOLTAGE  4.9e+00
*/
#define STOP_WAIT       0.7e+00
//------------------------------------------------------------------------------
void t10850(int mod_id, int slot)
{
  /*RECEIVER VOLTAGE ACCURACY - RECEIVE HIGH - +5v*/
  int receiver_no;
  int number_of_bytes;
  int byte_no;
  int vector;
  int gth_first_transition;
  int gtl_first_transition;
  int gth_last_transition;
  int gtl_last_transition;
  int expected_value;
  int page;

  char filename[81];
  double tclk_freq;
  double ac_rms_amplitude;
  double increment;
  double initial_voltage;
  double final_voltage;

  boolean encountered_invalid_gth;
  boolean encountered_invalid_gtl;

  int rresp;
  int werr;
  int gtl;
  int receiver;
  a_byte_type this_byte;
  test_result_type result;
  buffer_record_type *b_ptr;
  double vfirst_gth[4]['H' + 1 - 'A'];
  double vfirst_gtl[4]['H' + 1 - 'A'];
  double vlast_gth[4]['H' + 1 - 'A'];
  double vlast_gtl[4]['H' + 1 - 'A'];

  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  initial_voltage = 0.0;
  final_voltage = 0.0;
  increment = 0.0;

  ac_rms_amplitude = RECEIVER_RANGE_P_P / sqrt(2.0) / 2;

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);
  connect_dmm_pin(mod_id, slot);

  tclk_freq = INPUT_FREQ / 64;

  vth_mtclk_clk_20m_mux(mod_id);
  vth_mtclk_set_freq(mod_id, INPUT_FREQ, tclk_freq);
  sprintf(filename, "%ssales_step", testwhere_path);
  vth_aarb_ram_ld_file(mod_id, BLOCK, filename);
  //vth_aarb_ram_ld_file(mod_id, BLOCK, "step");
  vth_ascat_cpu_init(mod_id);

  vth_asrc_sel_fltr(mod_id, FILTER);
  vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A        */
//  vth_ascat_set_a_freq(mod_id, 1024);   //richard == cafe
    vth_ascat_set_a_freq(mod_id, 512);   /* 512Hz uses every location */
  vth_ascat_en_arb_ss(mod_id);
  vth_ascat_run_en(mod_id);

  vth_asrc_set_ampl(mod_id, ac_rms_amplitude, DC_OFFSET_VOLTAGE);

  vth_afc_en_src_out(mod_id);

  vth_mmbd_en_arcv(mod_id);
  vth_pen_arcv0(mod_id, slot);
  vth_pen_arcv1(mod_id, slot);

  close_pin_gnd_relays(mod_id, slot);
  vth_aclose_rly(mod_id, K806);   /*Aim source to S bus */

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    vth_aclose_rly(mod_id, AIM_X_S_RLYS[receiver - 'A']);
        /*S bus to X busses   */
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[receiver - 'A']);
        /*X busses disconnect */

    vth_pen_32chan(mod_id, slot);
    vth_pmap_rly(mod_id, slot, page);
    vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
        /*receiver connection */
    if (page == 1)
      vth_pdis_32chan(mod_id, slot);
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
        /*X busses to mint    */

    /*Now connect the source voltage to the mint pins for dmm access       */
    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
  }  /*for receiver*/
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);   /*Wait for relays to close*/

  /* Put card in 32 channel mode! */
  vth_pen_32chan(mod_id, slot);

  for (int ramp = NEG; ramp <= POS; ++ramp)
  {  /*for ramp*/
    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
      initial_voltage = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected dmm voltage)", true);

    for (receiver = 'A'; receiver <= 'H'; receiver++)
      vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    sales_reset_sequencer(mod_id, slot);

#include "t10850_vml.cmd"

    vth_pfc_sel_arcv0(mod_id, slot, 0, 3);   /*ARCV0 = RRESP CHANNEL A*/
    vth_pfc_sel_arcv1(mod_id, slot, 0, 4);   /*ARCV1 = WERR  CHANNEL A*/

    sales_start_sequencer(mod_id, slot);

    vth_mseq_wait_on_stop(mod_id, STOP_WAIT);
    vth_pfc_rst_sc_all(mod_id);   /*Reset the sc ram to enable reading */

    for (receiver = 'A'; receiver <= 'H'; receiver++)
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
    {  /*if*/
      final_voltage = result.UU.Real_result;
      increment = (final_voltage - initial_voltage) / STEPS;
    }  /*if*/
    else
      display_prompt("No result in result log (Expected dmm voltage)", true);

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      receiver_no = receiver - 'A' + 8 * page;

      vth_pfc_get_act_rcv_sc_ram(mod_id, slot, receiver_no, 1, VECTOR_LENGTH);
      if (get_result_nolog(mod_id, &result))
      {  /*if*/
        b_ptr = static_cast(buffer_record_type*,result.UU.Byte_ptr);
        number_of_bytes = b_ptr->Length;
        gth_first_transition = 0;
        gtl_first_transition = 0;
        encountered_invalid_gth = false;
        encountered_invalid_gtl = false;
        if (ramp == NEG)
          expected_value = 1;
        else
          expected_value = 0;
/* debug routine for reciever */
/******************************************************************************/
//        unsigned int rcvr_byte_no;
//        FILE *sc_ram_file;
//        sc_ram_file = NULL;
//        sc_ram_file = fopen("sc_ram", "a");
//        assert(sc_ram_file);
//        fprintf(sc_ram_file, "===============================================\n");
//        fprintf(sc_ram_file, "RECEIVER %c\n", receiver);
//
//        for (rcvr_byte_no = 0; rcvr_byte_no < number_of_bytes; rcvr_byte_no++)
//        {
//          this_byte.data = b_ptr->Buffer[rcvr_byte_no];
//          fprintf(sc_ram_file, "%-d\n", this_byte.data);
//        }
//
//        fclose(sc_ram_file);
/******************************************************************************/

        for (byte_no = 0; byte_no < number_of_bytes; byte_no++)
        {  /*for*/
          this_byte.data = b_ptr->Buffer[byte_no];


          for (vector = 0; vector <= 3; vector++)
          {
            if (!encountered_invalid_gth)
            {

              if (P_getbits_US((2*this_byte.data), (vector * 2 + 1), 0) == expected_value)
              {
                gth_first_transition++;
              }
              else
              {
                encountered_invalid_gth = true;
              }

            }

            if (P_getbits_US((2*this_byte.data), (vector * 2), 0) == 0)
            {
              werr = 0;
            }
            else
            {
              werr = 1;
            }

            if (P_getbits_US((2*this_byte.data), (vector * 2 + 1), 0) == 0)
            {
              rresp = 0;
            }
            else
            {
              rresp = 1;
            }

            gtl = exclusive_or(werr, rresp);

            if (!encountered_invalid_gtl)
            {
              if (gtl == expected_value)
                gtl_first_transition++;
              else
              {
                encountered_invalid_gtl = true;

              }
            }

          }  /*if*/
        }  /*for*/

        gth_last_transition = 0;
        gtl_last_transition = 0;
        encountered_invalid_gth = false;
        encountered_invalid_gtl = false;
        if (ramp == NEG)
          expected_value = 0;
        else
          expected_value = 1;
        for (byte_no = number_of_bytes - 1; byte_no >= 0; byte_no--)
        {  /*for*/
          this_byte.data = b_ptr->Buffer[byte_no];
          for (vector = 3; vector >= 0; vector--)
          {
            if (!encountered_invalid_gth)
            {

              if (P_getbits_US((2*this_byte.data), (vector * 2 + 1), 0) == expected_value)
              {
                gth_last_transition++;
              }
              else
              {
                encountered_invalid_gth = true;
              }
            }
            if (P_getbits_US((2*this_byte.data), (vector * 2), 0) == 0)
            {
              werr = 0;
            }
            else
            {
              werr = 1;
            }

            if (P_getbits_US((2*this_byte.data), (vector * 2 + 1), 0) == 0)
            {
              rresp = 0;
            }
            else
            {
              rresp = 1;
            }

            gtl = exclusive_or(werr, rresp);

            if (!encountered_invalid_gtl)
            {
              if (gtl == expected_value)
                gtl_last_transition++;
              else
                encountered_invalid_gtl = true;
            }

          }  /*if*/
        }  /*for*/

        gth_last_transition = 129 - gth_last_transition;
        gtl_last_transition = 129 - gtl_last_transition;

      }  /*if*/
      else
        display_prompt("No result in result log (Excepted SC RAM states)",
                       true);
      vfirst_gth[ramp]
        [receiver - 'A'] = initial_voltage + gth_first_transition * increment;
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = vfirst_gth[ramp][receiver - 'A'];
      log_result(mod_id, result);

      vlast_gth[ramp]
        [receiver - 'A'] = initial_voltage + gth_last_transition * increment;

      vfirst_gtl[ramp]
        [receiver - 'A'] = initial_voltage + gtl_first_transition * increment;

      vlast_gtl[ramp]
        [receiver - 'A'] = initial_voltage + gtl_last_transition * increment;

    }  /*for receiver*/


    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vlast_gth[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vfirst_gtl[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vlast_gtl[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

  }  /*for ramp*/



  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    /* calculte the GTH hysterisis for each receiver */
    result.UU.Real_result = vfirst_gth[POS][receiver - 'A'] - vfirst_gth[NEG]
                            [receiver - 'A'];

    log_result(mod_id, result);
  }  /*for receiver*/


  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    /* calculte the GTL hysterisis for each receiver */
    result.UU.Real_result = vfirst_gtl[POS][receiver - 'A'] - vfirst_gtl[NEG]
                            [receiver - 'A'];

    log_result(mod_id, result);
  }  /*for receiver*/

 } /* for page */

  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t10850*/
#undef DMM_FUNCTION
#undef MUX_NODE_PIN
#undef BLOCK
#undef FILTER
#undef VECTOR_LENGTH
#undef STEPS
#undef RANGE
#undef RESOLUTION
#undef INPUT_FREQ
#undef RECEIVER_RANGE_P_P
#undef DC_OFFSET_VOLTAGE
#undef STOP_WAIT

// ---  TEST 10851 - CARIBOU RECEIVER VOLTAGE ACCY  ---
// ---  IN PROGRESS
#define DMM_FUNCTION    DCV
/* pin 2 of 0 through 8 */
#define MUX_NODE_PIN    2
/* where to load waveform */
#define BLOCK           0
/* 8k low pass filter */
#define FILTER          7
/* Maximum sc ram vector */
#define VECTOR_LENGTH   126
/* Total number of vectors */
#define STEPS           128
#define RANGE           10.0e+00
#define RESOLUTION      1.0e-03
#define INPUT_FREQ      20e+06
#define RECEIVER_RANGE_P_P  0.4e+00
#define DC_OFFSET_VOLTAGE  4.0e+00
#define STOP_WAIT       0.7e+00
//-------------------------------------------------------------------
void t10851(int mod_id, int slot)
{
  /*RECEIVER VOLTAGE ACCURACY w/PULL UP 4.0v*/
  int receiver_no;
  int number_of_bytes;
  int byte_no;
  int vector;
  int gth_first_transition;
  int gtl_first_transition;
  int gth_last_transition;
  int gtl_last_transition;
  int expected_value;
  int page;

  char filename[81];
  double tclk_freq;
  double ac_rms_amplitude;
  double increment;
  double initial_voltage;
  double final_voltage;

  boolean encountered_invalid_gth;
  boolean encountered_invalid_gtl;

  int rresp;
  int werr;
  int gtl;
  int receiver;
  a_byte_type this_byte;
  test_result_type result;
  buffer_record_type *b_ptr;
  double vfirst_gth[4]['H' + 1 - 'A'];
  double vfirst_gtl[4]['H' + 1 - 'A'];
  double vlast_gth[4]['H' + 1 - 'A'];
  double vlast_gtl[4]['H' + 1 - 'A'];

  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  initial_voltage = 0.0;
  final_voltage = 0.0;
  increment = 0.0;

  ac_rms_amplitude = RECEIVER_RANGE_P_P / sqrt(2.0) / 2;

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);
  connect_dmm_pin(mod_id, slot);

  tclk_freq = INPUT_FREQ / 64;
  vth_mtclk_clk_20m_mux(mod_id);
  vth_mtclk_set_freq(mod_id, INPUT_FREQ, tclk_freq);

  sprintf(filename, "%ssales_step", testwhere_path);
  vth_aarb_ram_ld_file(mod_id, BLOCK, filename);
  //vth_aarb_ram_ld_file(mod_id, BLOCK, "step");
  vth_ascat_cpu_init(mod_id);

  vth_asrc_sel_fltr(mod_id, FILTER);
  vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A        */
  vth_ascat_set_a_freq(mod_id, 512);   /* 512Hz uses every location */
  vth_ascat_en_arb_ss(mod_id);
  vth_ascat_run_en(mod_id);

  vth_asrc_set_ampl(mod_id, ac_rms_amplitude, DC_OFFSET_VOLTAGE);

  vth_afc_en_src_out(mod_id);

  vth_mmbd_en_arcv(mod_id);
  vth_pen_arcv0(mod_id, slot);
  vth_pen_arcv1(mod_id, slot);

  close_pin_gnd_relays(mod_id, slot);
  vth_aclose_rly(mod_id, K806);   /*Aim source to S bus */

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    vth_aclose_rly(mod_id, AIM_X_S_RLYS[receiver - 'A']);
        /*S bus to X busses   */
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[receiver - 'A']);
        /*X busses disconnect */

    vth_pen_32chan(mod_id, slot);
    vth_pmap_rly(mod_id, slot, page);
    vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
        /*receiver connection */
    if (page == 1)
      vth_pdis_32chan(mod_id, slot);
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
        /*X busses to mint    */

//    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
//        /*X busses to mint    */
//    vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
//        /*receiver connection */

    /*Now connect the source voltage to the mint pins for dmm access       */
    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
  }  /*for receiver*/
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);   /*Wait for relays to close*/

  /* Put card in 32 channel mode! */
  vth_pen_32chan(mod_id, slot);

  for (int ramp = NEG; ramp <= POS; ++ramp)
  {  /*for ramp*/
    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
      initial_voltage = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected dmm voltage)", true);

    for (receiver = 'A'; receiver <= 'H'; receiver++)
      vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    sales_reset_sequencer(mod_id, slot);

#include "t10851_vml.cmd"

    vth_pfc_sel_arcv0(mod_id, slot, 0, 3);   /*ARCV0 = RRESP CHANNEL A*/
    vth_pfc_sel_arcv1(mod_id, slot, 0, 4);   /*ARCV1 = WERR  CHANNEL A*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {
      receiver_no = receiver - 'A' + 8 * page;
      vth_prcv_dis_pdwn(mod_id, slot, receiver_no);
      vth_prcv_en_pup(mod_id, slot, receiver_no);
    }

    sales_start_sequencer(mod_id, slot);

    vth_mseq_wait_on_stop(mod_id, STOP_WAIT);
    vth_pfc_rst_sc_all(mod_id);   /*Reset the sc ram to enable reading */

    for (receiver = 'A'; receiver <= 'H'; receiver++)
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
    {  /*if*/
      final_voltage = result.UU.Real_result;
      increment = (final_voltage - initial_voltage) / STEPS;
    }  /*if*/
    else
      display_prompt("No result in result log (Expected dmm voltage)", true);

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      receiver_no = receiver - 'A' + 8 * page;

      vth_pfc_get_act_rcv_sc_ram(mod_id, slot, receiver_no, 1, VECTOR_LENGTH);
      if (get_result_nolog(mod_id, &result))
      {  /*if*/
        b_ptr = static_cast(buffer_record_type*,result.UU.Byte_ptr);
        number_of_bytes = b_ptr->Length;
        gth_first_transition = 0;
        gtl_first_transition = 0;
        encountered_invalid_gth = false;
        encountered_invalid_gtl = false;
        if (ramp == NEG)
          expected_value = 1;
        else
          expected_value = 0;

        for (byte_no = 0; byte_no < number_of_bytes; byte_no++)
        {  /*for*/
          this_byte.data = b_ptr->Buffer[byte_no];
          for (vector = 0; vector <= 3; vector++)
          {
            if (!encountered_invalid_gth)
            {
              if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) ==
                  expected_value)
                gth_first_transition++;
              else
                encountered_invalid_gth = true;
            }

            if (P_getbits_US((2*this_byte.data), vector * 2, 0) == 0)
              werr = 0;
            else
              werr = 1;
            if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) == 0)
              rresp = 0;
            else
              rresp = 1;

            gtl = exclusive_or(werr, rresp);

            if (!encountered_invalid_gtl)
            {
              if (gtl == expected_value)
                gtl_first_transition++;
              else
              {
                encountered_invalid_gtl = true;

              }
            }
          }  /*if*/
        }  /*for*/


        gth_last_transition = 0;
        gtl_last_transition = 0;
        encountered_invalid_gth = false;
        encountered_invalid_gtl = false;
        if (ramp == NEG)
          expected_value = 0;
        else
          expected_value = 1;
        for (byte_no = number_of_bytes - 1; byte_no >= 0; byte_no--)
        {  /*for*/
          this_byte.data = b_ptr->Buffer[byte_no];
          for (vector = 3; vector >= 0; vector--)
          {
            if (!encountered_invalid_gth)
            {
              if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) ==
                  expected_value)
                gth_last_transition++;
              else
                encountered_invalid_gth = true;
            }

            if (P_getbits_US((2*this_byte.data), vector * 2, 0) == 0)
              werr = 0;
            else
              werr = 1;
            if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) == 0)
              rresp = 0;
            else
              rresp = 1;

            gtl = exclusive_or(werr, rresp);

            if (!encountered_invalid_gtl)
            {
              if (gtl == expected_value)
                gtl_last_transition++;
              else
                encountered_invalid_gtl = true;
            }

          }  /*if*/
        }  /*for*/

        gth_last_transition = 129 - gth_last_transition;
        gtl_last_transition = 129 - gtl_last_transition;

      }  /*if*/
      else
        display_prompt("No result in result log (Excepted SC RAM states)",
                       true);
      vfirst_gth[ramp]
        [receiver - 'A'] = initial_voltage + gth_first_transition * increment;
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = vfirst_gth[ramp][receiver - 'A'];
      log_result(mod_id, result);
      vlast_gth[ramp]
        [receiver - 'A'] = initial_voltage + gth_last_transition * increment;
      vfirst_gtl[ramp]
        [receiver - 'A'] = initial_voltage + gtl_first_transition * increment;
      vlast_gtl[ramp]
        [receiver - 'A'] = initial_voltage + gtl_last_transition * increment;

    }  /*for receiver*/


    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vlast_gth[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vfirst_gtl[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vlast_gtl[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

  }  /*for ramp*/



  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    /* calculte the GTH hysterisis for each receiver */
    result.UU.Real_result = vfirst_gth[POS][receiver - 'A'] - vfirst_gth[NEG]
                            [receiver - 'A'];

    log_result(mod_id, result);
  }  /*for receiver*/


  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    /* calculte the GTL hysterisis for each receiver */
    result.UU.Real_result = vfirst_gtl[POS][receiver - 'A'] - vfirst_gtl[NEG]
                            [receiver - 'A'];

    log_result(mod_id, result);
  }  /*for receiver*/

 } /* for page */

  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t10851*/
#undef DMM_FUNCTION
#undef MUX_NODE_PIN
#undef BLOCK
#undef FILTER
#undef VECTOR_LENGTH
#undef STEPS
#undef RANGE
#undef RESOLUTION
#undef INPUT_FREQ
#undef RECEIVER_RANGE_P_P
#undef DC_OFFSET_VOLTAGE
#undef STOP_WAIT

// ---  TEST 10852 - CARIBOU RECEIVER VOLTAGE ACCY  ---
// ---  IN PROGRESS
#define DMM_FUNCTION    DCV
/* pin 2 of 0 through 8 */
#define MUX_NODE_PIN    2
/* where to load waveform */
#define BLOCK           0
/* 8k low pass filter */
#define FILTER          7
/* Maximum sc ram vector */
#define VECTOR_LENGTH   126
/* Total number of vectors */
#define STEPS           128
#define RANGE           5.0e+00
#define RESOLUTION      1.0e-03
#define INPUT_FREQ      20e+06
#define RECEIVER_RANGE_P_P  0.4e+00
#define DC_OFFSET_VOLTAGE  (-3.5e+00)
#define STOP_WAIT       0.7e+00
//--------------------------------------------------------------------------
void t10852(int mod_id, int slot)
{
  /*RECEIVER VOLTAGE ACCURACY - RECEIVE HIGH - -3.5v*/
  int receiver_no;
  int number_of_bytes;
  int byte_no;
  int vector;
  int gth_first_transition;
  int gtl_first_transition;
  int gth_last_transition;
  int gtl_last_transition;
  int expected_value;
  int page;

  char filename[81];
  double tclk_freq;
  double ac_rms_amplitude;
  double increment;
  double initial_voltage;
  double final_voltage;

  boolean encountered_invalid_gth;
  boolean encountered_invalid_gtl;

  int rresp;
  int werr;
  int gtl;
  int receiver;
  a_byte_type this_byte;
  test_result_type result;
  buffer_record_type *b_ptr;
  double vfirst_gth[4]['H' + 1 - 'A'];
  double vfirst_gtl[4]['H' + 1 - 'A'];
  double vlast_gth[4]['H' + 1 - 'A'];
  double vlast_gtl[4]['H' + 1 - 'A'];


  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  initial_voltage = 0.0;
  final_voltage = 0.0;
  increment = 0.0;

  ac_rms_amplitude = RECEIVER_RANGE_P_P / sqrt(2.0) / 2;

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);
  connect_dmm_pin(mod_id, slot);

  tclk_freq = INPUT_FREQ / 64;
  vth_mtclk_clk_20m_mux(mod_id);
  vth_mtclk_set_freq(mod_id, INPUT_FREQ, tclk_freq);

  sprintf(filename, "%ssales_step", testwhere_path);
  vth_aarb_ram_ld_file(mod_id, BLOCK, filename);
  //vth_aarb_ram_ld_file(mod_id, BLOCK, "step");
  vth_ascat_cpu_init(mod_id);

  vth_asrc_sel_fltr(mod_id, FILTER);
  vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A        */
  vth_ascat_set_a_freq(mod_id, 512);   /* 512Hz uses every location */
  vth_ascat_en_arb_ss(mod_id);
  vth_ascat_run_en(mod_id);

  vth_asrc_set_ampl(mod_id, ac_rms_amplitude, DC_OFFSET_VOLTAGE);

  vth_afc_en_src_out(mod_id);

  vth_mmbd_en_arcv(mod_id);
  vth_pen_arcv0(mod_id, slot);
  vth_pen_arcv1(mod_id, slot);

  close_pin_gnd_relays(mod_id, slot);
  vth_aclose_rly(mod_id, K806);   /*Aim source to S bus */

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    vth_aclose_rly(mod_id, AIM_X_S_RLYS[receiver - 'A']);
        /*S bus to X busses   */
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[receiver - 'A']);
        /*X busses disconnect */

    vth_pen_32chan(mod_id, slot);
    vth_pmap_rly(mod_id, slot, page);
    vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
        /*receiver connection */
    if (page == 1)
      vth_pdis_32chan(mod_id, slot);
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
        /*X busses to mint    */

//    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
//        /*X busses to mint    */
//    vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
//        /*receiver connection */

    /*Now connect the source voltage to the mint pins for dmm access       */
    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
  }  /*for receiver*/
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);   /*Wait for relays to close*/

  /* Put card in 32 channel mode! */
  vth_pen_32chan(mod_id, slot);

  for (int ramp = NEG; ramp <= POS; ++ramp)
  {  /*for ramp*/
    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
      initial_voltage = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected dmm voltage)", true);

    for (receiver = 'A'; receiver <= 'H'; receiver++)
      vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    sales_reset_sequencer(mod_id, slot);

#include "t10852_vml.cmd"

    vth_pfc_sel_arcv0(mod_id, slot, 0, 3);   /*ARCV0 = RRESP CHANNEL A*/
    vth_pfc_sel_arcv1(mod_id, slot, 0, 4);   /*ARCV1 = WERR  CHANNEL A*/

    sales_start_sequencer(mod_id, slot);

    vth_mseq_wait_on_stop(mod_id, STOP_WAIT);
    vth_pfc_rst_sc_all(mod_id);   /*Reset the sc ram to enable reading */

    for (receiver = 'A'; receiver <= 'H'; receiver++)
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
    {  /*if*/
      final_voltage = result.UU.Real_result;
      increment = (final_voltage - initial_voltage) / STEPS;
    }  /*if*/
    else
      display_prompt("No result in result log (Expected dmm voltage)", true);

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      receiver_no = receiver - 'A' + 8 * page;

      vth_pfc_get_act_rcv_sc_ram(mod_id, slot, receiver_no, 1, VECTOR_LENGTH);
      if (get_result_nolog(mod_id, &result))
      {  /*if*/
        b_ptr = static_cast(buffer_record_type*,result.UU.Byte_ptr);
        number_of_bytes = b_ptr->Length;
        gth_first_transition = 0;
        gtl_first_transition = 0;
        encountered_invalid_gth = false;
        encountered_invalid_gtl = false;
        if (ramp == NEG)
          expected_value = 1;
        else
          expected_value = 0;

        for (byte_no = 0; byte_no < number_of_bytes; byte_no++)
        {  /*for*/
          this_byte.data = b_ptr->Buffer[byte_no];
          for (vector = 0; vector <= 3; vector++)
          {
            if (!encountered_invalid_gth)
            {
              if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) ==
                  expected_value)
                gth_first_transition++;
              else
                encountered_invalid_gth = true;
            }

            if (P_getbits_US((2*this_byte.data), vector * 2, 0) == 0)
              werr = 0;
            else
              werr = 1;
            if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) == 0)
              rresp = 0;
            else
              rresp = 1;

            gtl = exclusive_or(werr, rresp);

            if (!encountered_invalid_gtl)
            {
              if (gtl == expected_value)
                gtl_first_transition++;
              else
              {
                encountered_invalid_gtl = true;

              }
            }
          }  /*if*/
        }  /*for*/


        gth_last_transition = 0;
        gtl_last_transition = 0;
        encountered_invalid_gth = false;
        encountered_invalid_gtl = false;
        if (ramp == NEG)
          expected_value = 0;
        else
          expected_value = 1;
        for (byte_no = number_of_bytes - 1; byte_no >= 0; byte_no--)
        {  /*for*/
          this_byte.data = b_ptr->Buffer[byte_no];
          for (vector = 3; vector >= 0; vector--)
          {
            if (!encountered_invalid_gth)
            {
              if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) ==
                  expected_value)
                gth_last_transition++;
              else
                encountered_invalid_gth = true;
            }

            if (P_getbits_US((2*this_byte.data), vector * 2, 0) == 0)
              werr = 0;
            else
              werr = 1;
            if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) == 0)
              rresp = 0;
            else
              rresp = 1;

            gtl = exclusive_or(werr, rresp);

            if (!encountered_invalid_gtl)
            {
              if (gtl == expected_value)
                gtl_last_transition++;
              else
                encountered_invalid_gtl = true;
            }

          }  /*if*/
        }  /*for*/

        gth_last_transition = 129 - gth_last_transition;
        gtl_last_transition = 129 - gtl_last_transition;

      }  /*if*/
      else
        display_prompt("No result in result log (Excepted SC RAM states)",
                       true);
      vfirst_gth[ramp]
        [receiver - 'A'] = initial_voltage + gth_first_transition * increment;
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = vfirst_gth[ramp][receiver - 'A'];
      log_result(mod_id, result);
      vlast_gth[ramp]
        [receiver - 'A'] = initial_voltage + gth_last_transition * increment;
      vfirst_gtl[ramp]
        [receiver - 'A'] = initial_voltage + gtl_first_transition * increment;
      vlast_gtl[ramp]
        [receiver - 'A'] = initial_voltage + gtl_last_transition * increment;

    }  /*for receiver*/


    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vlast_gth[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vfirst_gtl[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vlast_gtl[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

  }  /*for ramp*/

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    /* calculte the GTH hysterisis for each receiver */
    result.UU.Real_result = vfirst_gth[POS][receiver - 'A'] - vfirst_gth[NEG]
                            [receiver - 'A'];

    log_result(mod_id, result);
  }  /*for receiver*/

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    /* calculte the GTL hysterisis for each receiver */
    result.UU.Real_result = vfirst_gtl[POS][receiver - 'A'] - vfirst_gtl[NEG]
                            [receiver - 'A'];

    log_result(mod_id, result);
  }  /*for receiver*/

 } /* for page */

  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t10852*/
#undef DMM_FUNCTION
#undef MUX_NODE_PIN
#undef BLOCK
#undef FILTER
#undef VECTOR_LENGTH
#undef STEPS
#undef RANGE
#undef RESOLUTION
#undef INPUT_FREQ
#undef RECEIVER_RANGE_P_P
#undef DC_OFFSET_VOLTAGE
#undef STOP_WAIT

// ---  TEST 10853 - CARIBOU RECEIVER VOLTAGE ACCY  ---
// ---  IN PROGRESS
#define DMM_FUNCTION    DCV
/* pin 2 of 0 through 8 */
#define MUX_NODE_PIN    2
/* where to load waveform */
#define BLOCK           0
/* 8k low pass filter */
#define FILTER          7
/* Maximum sc ram vector */
#define VECTOR_LENGTH   126
/* Total number of vectors */
#define STEPS           128
#define RANGE           5.0e+00
#define RESOLUTION      1.0e-03
#define INPUT_FREQ      20e+06
#define RECEIVER_RANGE_P_P  0.4e+00
#define DC_OFFSET_VOLTAGE  (-2.5e+00)
#define STOP_WAIT       0.7e+00
//-------------------------------------------------------------------------
void t10853(int mod_id, int slot)
{
  /*RECEIVER VOLTAGE ACCURACY - RECEIVE HIGH - -3.5v*/
  int receiver_no;
  int number_of_bytes;
  int byte_no;
  int vector;
  int gth_first_transition;
  int gtl_first_transition;
  int gth_last_transition;
  int gtl_last_transition;
  int expected_value;
  int page;

  char filename[81];
  double tclk_freq;
  double ac_rms_amplitude;
  double increment;
  double initial_voltage;
  double final_voltage;

  boolean encountered_invalid_gth;
  boolean encountered_invalid_gtl;

  int rresp;
  int werr;
  int gtl;
  int receiver;
  a_byte_type this_byte;
  test_result_type result;
  buffer_record_type *b_ptr;
  double vfirst_gth[4]['H' + 1 - 'A'];
  double vfirst_gtl[4]['H' + 1 - 'A'];
  double vlast_gth[4]['H' + 1 - 'A'];
  double vlast_gtl[4]['H' + 1 - 'A'];

  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  initial_voltage = 0.0;
  final_voltage = 0.0;
  increment = 0.0;

  ac_rms_amplitude = RECEIVER_RANGE_P_P / sqrt(2.0) / 2;

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);
  connect_dmm_pin(mod_id, slot);

  tclk_freq = INPUT_FREQ / 64;
  vth_mtclk_clk_20m_mux(mod_id);
  vth_mtclk_set_freq(mod_id, INPUT_FREQ, tclk_freq);

  sprintf(filename, "%ssales_step", testwhere_path);
  vth_aarb_ram_ld_file(mod_id, BLOCK, filename);
  //vth_aarb_ram_ld_file(mod_id, BLOCK, "step");
  vth_ascat_cpu_init(mod_id);

  vth_asrc_sel_fltr(mod_id, FILTER);
  vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A        */
  vth_ascat_set_a_freq(mod_id, 512);   /* 512Hz uses every location */
  vth_ascat_en_arb_ss(mod_id);
  vth_ascat_run_en(mod_id);

  vth_asrc_set_ampl(mod_id, ac_rms_amplitude, DC_OFFSET_VOLTAGE);

  vth_afc_en_src_out(mod_id);

  vth_mmbd_en_arcv(mod_id);
  vth_pen_arcv0(mod_id, slot);
  vth_pen_arcv1(mod_id, slot);

  close_pin_gnd_relays(mod_id, slot);
  vth_aclose_rly(mod_id, K806);   /*Aim source to S bus */

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    vth_aclose_rly(mod_id, AIM_X_S_RLYS[receiver - 'A']);
        /*S bus to X busses   */
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[receiver - 'A']);
        /*X busses disconnect */

    vth_pen_32chan(mod_id, slot);
    vth_pmap_rly(mod_id, slot, page);
    vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
        /*receiver connection */
    if (page == 1)
      vth_pdis_32chan(mod_id, slot);
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
        /*X busses to mint    */

//    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
//        /*X busses to mint    */
//    vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
//        /*receiver connection */

    /*Now connect the source voltage to the mint pins for dmm access       */
    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
  }  /*for receiver*/
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);   /*Wait for relays to close*/

  /* Put card in 32 channel mode! */
  vth_pen_32chan(mod_id, slot);

  for (int ramp = NEG; ramp <= POS; ++ramp)
  {  /*for ramp*/
    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
      initial_voltage = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected dmm voltage)", true);

    for (receiver = 'A'; receiver <= 'H'; receiver++)
      vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    sales_reset_sequencer(mod_id, slot);

#include "t10853_vml.cmd"

    vth_pfc_sel_arcv0(mod_id, slot, 0, 3);   /*ARCV0 = RRESP CHANNEL A*/
    vth_pfc_sel_arcv1(mod_id, slot, 0, 4);   /*ARCV1 = WERR  CHANNEL A*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {
      receiver_no = receiver - 'A' + 8 * page;
      vth_prcv_dis_pup(mod_id, slot, receiver_no);
      vth_prcv_en_pdwn(mod_id, slot, receiver_no);
    }
    sales_start_sequencer(mod_id, slot);

    vth_mseq_wait_on_stop(mod_id, STOP_WAIT);
    vth_pfc_rst_sc_all(mod_id);   /*Reset the sc ram to enable reading */

    for (receiver = 'A'; receiver <= 'H'; receiver++)
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
    {  /*if*/
      final_voltage = result.UU.Real_result;
      increment = (final_voltage - initial_voltage) / STEPS;
    }  /*if*/
    else
      display_prompt("No result in result log (Expected dmm voltage)", true);

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      receiver_no = receiver - 'A' + 8 * page;

      vth_pfc_get_act_rcv_sc_ram(mod_id, slot, receiver_no, 1, VECTOR_LENGTH);
      if (get_result_nolog(mod_id, &result))
      {  /*if*/
        b_ptr = static_cast(buffer_record_type*, result.UU.Byte_ptr);
        number_of_bytes = b_ptr->Length;
        gth_first_transition = 0;
        gtl_first_transition = 0;
        encountered_invalid_gth = false;
        encountered_invalid_gtl = false;
        if (ramp == NEG)
          expected_value = 1;
        else
          expected_value = 0;

        for (byte_no = 0; byte_no < number_of_bytes; byte_no++)
        {  /*for*/
          this_byte.data = b_ptr->Buffer[byte_no];
          for (vector = 0; vector <= 3; vector++)
          {
            if (!encountered_invalid_gth)
            {
              if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) ==
                  expected_value)
                gth_first_transition++;
              else
                encountered_invalid_gth = true;
            }

            if (P_getbits_US((2*this_byte.data), vector * 2, 0) == 0)
              werr = 0;
            else
              werr = 1;
            if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) == 0)
              rresp = 0;
            else
              rresp = 1;

            gtl = exclusive_or(werr, rresp);

            if (!encountered_invalid_gtl)
            {
              if (gtl == expected_value)
                gtl_first_transition++;
              else
              {
                encountered_invalid_gtl = true;

              }
            }
          }  /*if*/
        }  /*for*/

        gth_last_transition = 0;
        gtl_last_transition = 0;
        encountered_invalid_gth = false;
        encountered_invalid_gtl = false;
        if (ramp == NEG)
          expected_value = 0;
        else
          expected_value = 1;
        for (byte_no = number_of_bytes - 1; byte_no >= 0; byte_no--)
        {  /*for*/
          this_byte.data = b_ptr->Buffer[byte_no];
          for (vector = 3; vector >= 0; vector--)
          {
            if (!encountered_invalid_gth)
            {
              if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) ==
                  expected_value)
                gth_last_transition++;
              else
                encountered_invalid_gth = true;
            }

            if (P_getbits_US((2*this_byte.data), vector * 2, 0) == 0)
              werr = 0;
            else
              werr = 1;
            if (P_getbits_US((2*this_byte.data), vector * 2 + 1, 0) == 0)
              rresp = 0;
            else
              rresp = 1;

            gtl = exclusive_or(werr, rresp);

            if (!encountered_invalid_gtl)
            {
              if (gtl == expected_value)
                gtl_last_transition++;
              else
                encountered_invalid_gtl = true;
            }

          }  /*if*/
        }  /*for*/

        gth_last_transition = 129 - gth_last_transition;
        gtl_last_transition = 129 - gtl_last_transition;

      }  /*if*/
      else
        display_prompt("No result in result log (Excepted SC RAM states)",
                       true);
      vfirst_gth[ramp]
        [receiver - 'A'] = initial_voltage + gth_first_transition * increment;
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = vfirst_gth[ramp][receiver - 'A'];
      log_result(mod_id, result);
      vlast_gth[ramp]
        [receiver - 'A'] = initial_voltage + gth_last_transition * increment;
      vfirst_gtl[ramp]
        [receiver - 'A'] = initial_voltage + gtl_first_transition * increment;
      vlast_gtl[ramp]
        [receiver - 'A'] = initial_voltage + gtl_last_transition * increment;

    }  /*for receiver*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vlast_gth[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vfirst_gtl[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /*for receiver*/
      result.UU.Real_result = vlast_gtl[ramp][receiver - 'A'];
      log_result(mod_id, result);
    }  /*for receiver*/

  }  /*for ramp*/

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    /* calculte the GTH hysterisis for each receiver */
    result.UU.Real_result = vfirst_gth[POS][receiver - 'A'] - vfirst_gth[NEG]
                            [receiver - 'A'];

    log_result(mod_id, result);
  }  /*for receiver*/

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /*for receiver*/
    /* calculte the GTL hysterisis for each receiver */
    result.UU.Real_result = vfirst_gtl[POS][receiver - 'A'] - vfirst_gtl[NEG]
                            [receiver - 'A'];

    log_result(mod_id, result);
  }  /*for receiver*/

 } /* for page */

  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t10853*/
#undef DMM_FUNCTION
#undef MUX_NODE_PIN
#undef BLOCK
#undef FILTER
#undef VECTOR_LENGTH
#undef STEPS
#undef RANGE
#undef RESOLUTION
#undef INPUT_FREQ
#undef RECEIVER_RANGE_P_P
#undef DC_OFFSET_VOLTAGE
#undef STOP_WAIT

// ---  TEST 10860 - CARIBOU RECEIVER INPUT CURRENT SOURCING  ---
// ---  DONE
#define RANGE           3.0
#define MUX_NODE_PIN    2
#define RESOLUTION      1.0e-03
#define RECEIVE_HI      2.0e+00
#define PULL_UP_LOAD_RESISTANCE  500e+00
#define DMM_FUNCTION    DCV
void t10860(int mod_id, int slot)
{
  /*RECEIVER INPUT CURRENT SOURCING*/
  int receiver_no;

  double pull_up_actual_voltage;
  double pull_up_current;
  int receiver;
  int page;

  test_result_type result;



  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);

  connect_dmm_pin(mod_id, slot);
  connect_500_ohm_load(mod_id);

  close_pin_gnd_relays(mod_id, slot);

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /* for receiver */

    // Becoming standard way to open/close compatibility relays
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pdis_32chan(mod_id, slot);
      vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_popen_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
    }

//  vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);

//  for (receiver = 'A'; receiver <= 'H'; receiver++)
//  {  /* for receiver */

    receiver_no = receiver - 'A' + 8 * page;
    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
    vth_prcv_set_hi(mod_id, slot, receiver_no, RECEIVE_HI);
    vth_prcv_dis_pdwn(mod_id, slot, receiver_no);
    vth_prcv_en_pup(mod_id, slot, receiver_no);
    vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);

    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
    {  /*if*/
      pull_up_actual_voltage = result.UU.Real_result;
      pull_up_current = pull_up_actual_voltage / PULL_UP_LOAD_RESISTANCE;
      result.UU.Real_result = pull_up_current;
      log_result(mod_id, result);
    }  /*if*/
    else
      display_prompt("No result in result log (Expected rcvr pull up voltage)",
                     true);

    vth_prcv_dis_pup(mod_id, slot, receiver_no);

    vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);

    vth_pdis_32chan(mod_id, slot);
    vth_popen_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
    vth_popen_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);

  }  /* for receiver */

 }  /* for page */

  vth_pdis_32chan(mod_id, slot);
  vth_pinit(mod_id, slot);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t10860 */
#undef RANGE
#undef MUX_NODE_PIN
#undef RESOLUTION
#undef RECEIVE_HI
#undef PULL_UP_LOAD_RESISTANCE
#undef DMM_FUNCTION

// ---  TEST 10861 - CARIBOU RECEIVER INPUT CURRENT SINKING  ---
// ---  DONE
#define RANGE           3.0
#define MUX_NODE_PIN    2
#define RESOLUTION      1.0e-03
#define RECEIVE_LO      (-3.0e+00)
#define PULL_DN_LOAD_RESISTANCE  500e+00
#define DMM_FUNCTION    DCV
void t10861(int mod_id, int slot)
{
  /*RECEIVER INPUT CURRENT SINKING*/
  int receiver_no;

  double pull_dn_actual_voltage;
  double pull_dn_current;
  int receiver;
  int page;

  test_result_type result;



  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);

  connect_dmm_pin(mod_id, slot);
  connect_500_ohm_load(mod_id);

  close_pin_gnd_relays(mod_id, slot);

 for (page = 0; page <= 1; page++)
 {  /* for page */

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /* for receiver */

    // Becoming standard way to open/close compatibility relays
    if (page == 0)
    {
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
    }
    else  /* page == 1 */
    {
      vth_pdis_32chan(mod_id, slot);
      vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
      vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
      vth_pen_32chan(mod_id, slot);
      vth_pmap_rly(mod_id, slot, 0);
      vth_popen_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
    }

//    vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);

//  for (receiver = 'A'; receiver <= 'H'; receiver++)
//  {  /* for receiver */

    receiver_no = receiver - 'A' + 8 * page;
    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
    vth_prcv_set_lo(mod_id, slot, receiver_no, RECEIVE_LO);
    vth_prcv_dis_pup(mod_id, slot, receiver_no);
    vth_prcv_en_pdwn(mod_id, slot, receiver_no);
    vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);

    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
    {  /*if*/
      pull_dn_actual_voltage = result.UU.Real_result;
      pull_dn_current = pull_dn_actual_voltage / PULL_DN_LOAD_RESISTANCE;
      result.UU.Real_result = pull_dn_current;
      log_result(mod_id, result);
    }  /*if*/
    else
      display_prompt("No result in result log (Expected rcvr pull dn voltage)",
                     true);

    vth_prcv_dis_pdwn(mod_id, slot, receiver_no);

    vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);

    vth_pdis_32chan(mod_id, slot);
    vth_popen_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
    vth_popen_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);

  }  /* for receiver */

 }  /* for page */

  vth_pdis_32chan(mod_id, slot);
  vth_pinit(mod_id, slot);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t10861 */
#undef RANGE
#undef MUX_NODE_PIN
#undef RESOLUTION
#undef RECEIVE_LO
#undef PULL_DN_LOAD_RESISTANCE
#undef DMM_FUNCTION

// ---  TEST 10862 - CARIBOU RECEIVER INPUT LEAKAGE CURRENT  ---
// ---  UNTESTED!!
#define BELOW           0
#define ABOVE_          2
typedef double rx_level_array[3];
#define MUX_NODE_PIN    2
#define RANGE           300e-03
#define RESOLUTION      1.0e-03
#define RECEIVER_LOAD_RESISTANCE  500e+00
const rx_level_array RECEIVE_LO_LEVEL = {1.0, -2.0, -2.0};
const rx_level_array RECEIVE_HI_LEVEL = {2.0, 2.0, -1.0};
#define DMM_FUNCTION    DCV
//------------------------------------------------------------------------------
void t10862(int mod_id, int slot)
{
  /*RECEIVER INPUT LEAKAGE CURRENT*/
  int receiver_no;

  double receive_lo;
  double receive_hi;
  double receiver_input_voltage;
  double receiver_input_current;

  test_result_type result;
  int receiver;
  int page;

  open_dmm(mod_id);
  open_mux(mod_id);

  // Initialize card to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);

  connect_dmm_pin(mod_id, slot);
  connect_500_ohm_load(mod_id);

  close_pin_gnd_relays(mod_id, slot);

//  for (receiver = 'A'; receiver <= 'H'; receiver++)
//    vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);

  for (int receiver_level = BELOW; receiver_level <= ABOVE_; ++receiver_level)
  {  /* FOR EACH RECEIVER LEVEL */
    receive_lo = RECEIVE_LO_LEVEL[receiver_level];
    receive_hi = RECEIVE_HI_LEVEL[receiver_level];

   for (page = 0; page <= 1; page++)
   {  /* for page */

    for (receiver = 'A'; receiver <= 'H'; receiver++)
    {  /* for receiver */

      // Becoming standard way to open/close compatibility relays
      if (page == 0)
      {
        vth_pen_32chan(mod_id, slot);
        vth_pmap_rly(mod_id, slot, 0);
        vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
      }
      else  /* page == 1 */
      {
        vth_pdis_32chan(mod_id, slot);
        vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
        vth_pen_32chan(mod_id, slot);
        vth_pmap_rly(mod_id, slot, 1);
        vth_pclose_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);
      }

      receiver_no = receiver - 'A' + 8 * page;
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
      vth_prcv_set_lo(mod_id, slot, receiver_no, receive_lo);
      vth_prcv_set_hi(mod_id, slot, receiver_no, receive_hi);
      vth_prcv_dis_pdwn(mod_id, slot, receiver_no);
      vth_prcv_dis_pup(mod_id, slot, receiver_no);
      vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);

      measure_dmm(mod_id);
      if (get_result_nolog(mod_id, &result))
      {  /*if*/
        receiver_input_voltage = result.UU.Real_result;
        receiver_input_current = receiver_input_voltage / RECEIVER_LOAD_RESISTANCE;
        result.UU.Real_result = receiver_input_current;
        log_result(mod_id, result);
      }  /*if*/
      else
        display_prompt("No result in result log (Expected dmm voltage)", true);

      vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][receiver - 'A']);
      vth_pdis_32chan(mod_id, slot);
      vth_popen_rly(mod_id, slot, PIN_KX_RLYS[receiver - 'A']);
      vth_popen_rly(mod_id, slot, PIN_KR_RLYS[receiver - 'A']);

    }  /* for receiver */

   }  /* for page */

  }  /* FOR RECEIVER LEVEL */


  vth_pinit(mod_id, slot);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t10862 */
#undef BELOW
#undef ABOVE_
#undef MUX_NODE_PIN
#undef RANGE
#undef RESOLUTION
#undef RECEIVER_LOAD_RESISTANCE
#undef DMM_FUNCTION

// ---  TEST 10870 - CARIBOU RECEIVER EDGE PLACEMENT ACCURACY AND SKEW  ---
// ---  IN PROGRESS!!  NOT NEEDED BUT KEEP FOR NOW 3-20
#define TCLK_FREQ       5.0e+06
#define GEN_LOW_50      0.20e+00
#define GEN_HIGH_50     3.5e+00
#define RX_SPEC         10e-09
#define TIMEOUT         1.0e-03
#define STOP_WAIT       100e-03
#define PROG_DELAY_     20e-09
#define TCLK_ADV_TIME   40e-09
#define RES             1.0e-09
#define STARTING_INCREMENT  2.0e-09

/* static variables for t10870: */
struct LOC_t10870
{
  int mod_id;
  int slot;
  boolean rresp;
  boolean werr;
  boolean dummy;
  int receiver_no;
  double gen_delay;
  test_result_type result;
} ;

static void sample(struct LOC_t10870 *LINK)
{
  /* ------------------------------------------------------------------------ */
  /* Procedure sample sets up the generator delay to the value to be tested,  */
  /* enables the RRESP AND WERR signals onto the ARCV lines, starts and stops */
  /* the sequencer, and sets the return values for WERR and RRESP             */
  /* ------------------------------------------------------------------------ */
  setup_gen_trg_out_delay(LINK->mod_id, LINK->gen_delay);
  enable_gen_out(LINK->mod_id);
  sales_reset_sequencer(LINK->mod_id, LINK->slot);

  vth_pfc_sel_arcv0(LINK->mod_id, LINK->slot, LINK->receiver_no, 3);
      /* 3 = ARCV0 = RRESP */
  vth_pfc_sel_arcv1(LINK->mod_id, LINK->slot, LINK->receiver_no, 4);
      /* 4 = ARCV1 = WERR */

  sales_start_sequencer(LINK->mod_id, LINK->slot);
  vth_mseq_wait_on_stop(LINK->mod_id, STOP_WAIT);
  vth_mseq_stop(LINK->mod_id, TIMEOUT);

  vth_mmbd_rd_arcv1(LINK->mod_id);   /* vth_read WERR */
  LINK->dummy = get_result_nolog(LINK->mod_id, &LINK->result);
      /* get from result log */
  if (LINK->result.UU.Integer_result == 1)   /* WERR occurred */
    LINK->werr = true;
  else
  {  /* no WERR occurred */
    LINK->werr = false;
    vth_mmbd_rd_arcv0(LINK->mod_id);   /* get RRESP */
    LINK->dummy = get_result_nolog(LINK->mod_id, &LINK->result);
        /* get from result log */
    if (LINK->result.UU.Integer_result == 1)
      LINK->rresp = true;
    else
      LINK->rresp = false;
  }  /* no WERR occurred */
}  /* sample */

//------------------------------------------------------------------------------

void t10870(int mod_id, int slot)
{  /* main t10870 */
  /* [CARIBOU] RX Edge Accuracy and Skew */

  /* For path and test descriptions, see the documentation for test: 9171 */
  struct LOC_t10870 V;

  int mapped_slot;

  boolean expected_response;
  boolean found;
  boolean hit;
  int direction;

  double t0_actual;
  double delay_inc;
  double expected_receive;
  double actual_receive;
  double min_gen_delay;
  double max_gen_delay;
  double min_gen_meas;
  double max_gen_meas;
  double rx_range;
  double skew;
  double delta_delay;

  double offset[2];
  double gain[2];
  double ch_result['H' + 1 - 'A'][2];

  int mux_node_pin;
  int receiver;
  double correction;   //correction factor because results of this test are dependent
                       // on the number and type of cards in module

/* the .435ns is the correction factor for 1 single density card.  it was observed from
   Report data that indicated the shift in this test.  it was calculated as follows:

   with 2 DD cards, mean for test was 1.12ns
   with 9 DD cards, mean for test was -4.97ns
   delta mean = 1.12 - -4.97 = 6.09ns
   delta cards = 9-2 = 7
   correction per DD card = 6.09ns / 7 = .87ns
   correction per SD card is thus .87ns /2 = .435ns
   */


//  correction = calc_mod_correction(mod_id, .435e-09);

    correction = calc_mod_correction(mod_id, .2e-09);

  V.mod_id = mod_id;
  V.slot = slot;
  if (V.slot > 11)
    mapped_slot = V.slot - 11;
  else
    mapped_slot = V.slot;

  open_syn(V.mod_id);
  open_gen(V.mod_id);
  open_mux(V.mod_id);
  open_cntr(V.mod_id);

  /* t0_actual is different for mac10s and mac11s.  This value is provided by */
  /* AutoCal software.  AutoCal defines this constant as T0_TIME */
//assuming that ACME uses the same autocal constant that mac11 uses -- mjs 6/98
  if ((get_testhead_actual_card(V.mod_id, MAC_SLOT) == CONTROLPLUS) || (get_testhead_actual_card(V.mod_id, MAC_SLOT) == CONTROLXT))
    t0_actual = MAC_11_T0_ACTUAL;
  else
    t0_actual = MAC_10_T0_ACTUAL;

  /* rx_range is the +/- max value we will check to see if a receive occurred */
  rx_range = 2 * RX_SPEC;

  /* delta_delay is the difference between when the generator trig will get */
  /* to CLKRB and when the generator output will get to the receiver mint pin */
  /* The values are:                                                      */
  /*  AF2_system_delay = delay of AF2 cable for module (unique for each system) */
  /*  slot_delay = delay associated with slot differences (slot 2 = 0.0 delay */
  /*  rx_path_offset = AF2 to receiver delay less slot delay and channel delay */
  /*  AF2_CLKRB_delay = sales rack specific constant; diff AF2 to CLKRB delays */
  delta_delay = af2_system_delay[V.mod_id] +
                (slot_delay[mapped_slot - 1][RISING]
                 + slot_delay[mapped_slot - 1][FALLING]) / 2
                 + rx_path_offset - af2_clkrb_delay[V.mod_id];

//  display_it(mod_id, "af2_system_delay = ", 0, af2_system_delay[V.mod_id]);
//  display_it(mod_id, "slot_delay[mapped_slot - 1][RISING]", 0, slot_delay[mapped_slot - 1][RISING]);
//  display_it(mod_id, "slot_delay[mapped_slot - 1][FALLING]", 0, slot_delay[mapped_slot - 1][FALLING]);
//  display_it(mod_id, "rx_path_offset = ", 0, rx_path_offset);
//  display_it(mod_id, "af2_clkrb_delay = ", 0, af2_clkrb_delay[V.mod_id]);

  /* expected_receive is when the receiver strobe should occur at no error */
  /* prog_delay is the delay value that the rx is programmed to in the VML code */
  /* tclk_adv_time is the amount of time we advance the tclk (from CLKRB).  We */
  /*  set(picked) this value such that our receive will occur within the delay */
  /*  range of the generator. */
  expected_receive = t0_actual + PROG_DELAY_ - TCLK_ADV_TIME;

//  display_it(mod_id, "t0_actual = ", 0, t0_actual);
  /* The following section calculates the gain and offset associated with the */
  /* generator delay settings.  These values are used to increase the accuracy */
  /* of the results.  This 2 point calibration assumes the delay is linear.  */

  /* min/max_gen_delay are the values over which the receiver will be checked */
//  display_it(mod_id, "expected_receive = ", 0, expected_receive);
//  display_it(mod_id, "delta_delay = ", 0, delta_delay);
//  display_it(mod_id, "rx_range = ", 0, rx_range);

  min_gen_delay = nanotrunc(expected_receive - delta_delay - rx_range);
  max_gen_delay = nanotrunc(expected_receive - delta_delay + rx_range);

//  display_it(mod_id, "min_gen_delay = ", 0, min_gen_delay);
//  display_it(mod_id, "max_gen_delay = ", 0, max_gen_delay);

  /* Make the required connections.  The generator is synced to the synthesizer */
  /* to provide a more accurate (less jitter) timebase for the generator. */
  connect_syn_out_gen_ext(V.mod_id);
  connect_gen_trg_cntr_start(V.mod_id);
  connect_gen_out_cntr_stop(V.mod_id);

  setup_gen_levels(V.mod_id, GEN_LOW_50, GEN_HIGH_50);

  for (int edge = RISING; edge <= FALLING; ++edge)
  {  /* Measure the offset and gain for pos and neg generator output */
    setup_gen_trg_out_delay(V.mod_id, min_gen_delay);
    enable_gen_out(V.mod_id);
    vth_mtmr_wait(V.mod_id, 0.3);
    trigger_cntr(V.mod_id);
    min_gen_meas = read_cntr_mean(V.mod_id);
    setup_gen_trg_out_delay(V.mod_id, max_gen_delay);
    vth_mtmr_wait(V.mod_id, 0.3);
    trigger_cntr(V.mod_id);
    max_gen_meas = read_cntr_mean(V.mod_id);
    disable_gen_out(V.mod_id);
    offset[edge] = min_gen_meas;
    gain[edge] = (max_gen_meas - min_gen_meas) / (max_gen_delay - min_gen_delay);
    complement_gen_out(V.mod_id);
    setup_cntr_trigger_slopes(V.mod_id, POS, NEG);
  }  /* Measure the offset and gain for pos and neg generator output */

  no_complement_gen_out(V.mod_id);
  setup_gen_levels(V.mod_id, GEN_LOW_50, GEN_HIGH_50);

  connect_gen_trg_clkrb(V.mod_id);
  connect_gen_out_af2(V.mod_id);

  vth_mclose_rly(V.mod_id, MK2902);   /* clkrb path relay */
  vth_mclose_rly(V.mod_id, MK2903);   /* clkrsg path relay */
  vth_aclose_rly(V.mod_id, K753);   /* XL to AF2 path relay */
  vth_aclose_rly(V.mod_id, K764);   /* XL to AF2 path relay */

// Added for compat
  // Initialize card to compatibility mode
  // Which of these does what we really want??
  vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x0);
  vth_pdis_32chan(V.mod_id, V.slot);

/* THIS IS where compatibility loop will need to start */

//  for (receiver = 'A'; receiver <= 'H'; receiver++)
  for (receiver = 'A'; receiver <= 'B'; receiver++)
  {  /* for */
//    vth_pclose_rly(V.mod_id, V.slot, PIN_KR_RLYS[receiver - 'A']);
    vth_pen_32chan(V.mod_id, V.slot);
    vth_pmap_rly(V.mod_id, V.slot, 0);
    vth_pclose_rly(V.mod_id, V.slot, PIN_KR_RLYS[receiver - 'A']);
    vth_pdis_32chan(V.mod_id, V.slot);
    vth_pclose_rly(V.mod_id, V.slot, PIN_KX_RLYS[receiver - 'A']);
    switch (receiver)
    {   /*Use mux_node_pins closest to the grounds*/

    case 'A':
    case 'B':
    case 'E':
    case 'F':
      mux_node_pin = 7;
      break;

    case 'C':
    case 'D':
    case 'G':
    case 'H':
      mux_node_pin = 0;
      break;
    default:
      assert(false);
      break;

    }/* case */
    vth_pclose_rly(V.mod_id, V.slot, PIN_DRVR_RLYS[mux_node_pin][receiver - 'A']);
    vth_pclose_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[mux_node_pin + 1][receiver - 'A']);
  }  /* for */


  close_pin_gnd_relays(V.mod_id, V.slot);
      /*ground all termination resistors */
  vth_pclose_rly(V.mod_id, V.slot, KDCL);

  /* Set up the TCLK generator to sync to the external CLKRB (gen trig) signal */
  vth_mtclk_clkrb_mux(V.mod_id);
  vth_mtclk_sel_ris_edge(V.mod_id);
  vth_mtclk_set_freq(V.mod_id, TCLK_FREQ, TCLK_FREQ);
  vth_mtclk_set_adv_time(V.mod_id, TCLK_ADV_TIME);

  /* Enable the ARCV lines from pin to mother card and from mother card to mac */
  vth_pen_arcv0(V.mod_id, V.slot);
  vth_pen_arcv1(V.mod_id, V.slot);
  vth_mmbd_en_arcv(V.mod_id);

#include "t10871_vml.cmd"

  for (edge = RISING; edge <= FALLING; ++edge)
  {  /* for edge */
    if (edge == RISING)
    {  /* then */
      no_complement_gen_out(V.mod_id);   /* Use at the rising edge */
      expected_response = true;
    }  /* then */
    else
    {  /* else */
      complement_gen_out(V.mod_id);   /* Use at the falling edge */
      expected_response = false;
    }  /* else */

    /* Recalculate delta_delay for the particular edge that we are testing */
    delta_delay = af2_system_delay[V.mod_id] + slot_delay[mapped_slot - 1]
                  [edge] + rx_path_offset - af2_clkrb_delay[V.mod_id];

//    for (receiver = 'A'; receiver <= 'H'; receiver++)
    for (receiver = 'A'; receiver <= 'A'; receiver++)
    {  /* for receiver */
      V.receiver_no = receiver - 'A';

//      /* Two kgl relays are closed for each receiver.  This effectively puts */
//      /* two 100 ohm load resistors in parallel to simulate a 50 ohm load. */
//     vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);
     vth_pdis_32chan(V.mod_id, V.slot);
     vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);
      switch (receiver)
      {   /*Use mux_node_pins closest to the grounds*/

      case 'A':
      case 'C':
      case 'E':
      case 'G':
        vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' + 1]);
        break;

      case 'B':
      case 'D':
      case 'F':
      case 'H':
        vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' - 1]);
        break;
      default:
        assert(false);
        break;

      }/* case */

      delay_inc = STARTING_INCREMENT;
      direction = 0;
      hit = false;
      found = false;
      V.gen_delay = nanotrunc(expected_receive - delta_delay + 1e-09);

      vth_pen_32chan(V.mod_id, V.slot);

      do
      {

        sample(&V);   /* Sample sets the value of WERR and RRESP */

        /* If a WERR occurs, the current gen delay is the closest tested */
        /* value of gen delay to rx strobe */
        if (V.werr)
          found = true;
        else
        {  /* Do the non-window error procedure */
          /* Determine if 'hit' and set the direction of delay based on rresp */
          if (V.rresp == expected_response)
          {  /* increase the delay */
            if (direction < 0)   /* can never hit on first point */
              hit = true;
            direction = 1;   /* set direction to increase delay */
          }  /* increase the delay */
          else
          {  /* decrease the delay */
            if (direction > 0)   /* can never hit on first point */
              hit = true;
            direction = -1;   /* set direction to decrease delay */
          }  /* decrease the delay */

          /* Each time a 'hit' occurs, cut the delay increment in half. 'found' */
          /* will only be set true when a 'hit' occurs at the 'res'(resolution) */
          /* value of generator delay. */
          if (hit)
          {
            if (delay_inc <= RES)
              found = true;
            delay_inc /= 2;
          }

          /* On the final 'hit' value of gen delay, the gen delay value will */
          /* be adjusted by 1/2 the 'res' value in the proper direction to */
          /* effectively average the RRESP=0 and RRESP=1 values.  The rx strobe */
          /* occurred somewhere between the RRESP=0 and RRESP=1. */
          V.gen_delay += delay_inc * direction;

        }  /* Do the non-window error procedure */

      } while (!(found || V.gen_delay < min_gen_delay ||
                 V.gen_delay > max_gen_delay));


      disable_gen_out(V.mod_id);

      vth_pdis_32chan(V.mod_id, V.slot);
      vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);

      switch (receiver)
      {   /*Use mux_node_pins closest to the grounds*/

      case 'A':
      case 'C':
      case 'E':
      case 'G':
        vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' + 1]);
        break;

      case 'B':
      case 'D':
      case 'F':
      case 'H':
        vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' - 1]);
        break;
      default:
        assert(false);
        break;

      }/* case */

      /* calculate the actual receive, correct for gen error and channel delay */
      actual_receive = gain[edge] * (V.gen_delay - min_gen_delay) +
          offset[edge] + channel_delay[receiver - 'A'] + delta_delay;


      ch_result[receiver - 'A'][edge] = actual_receive - expected_receive;
      V.result.Measurement_class = CLASS_REAL;


      if (!found)
      {   /* it never received */
        display_prompt("**RECEIVE DOES NOT OCCUR WITHIN TEST RANGE**", true);
        ch_result[receiver - 'A'][edge] = INT_MAX;
      }


      //added the correction factor to test to correct for module configuration
      //see calculations above at top of test
      //mjs - 6/97
      V.result.UU.Real_result = ch_result[receiver - 'A'][edge] + correction;
      log_result(V.mod_id, V.result);

    }  /* for receiver */

  }  /* for edge */

  /* Now log the rising to falling skew for each of the receivers */

//  for (receiver = 'A'; receiver <= 'H'; receiver++)
  for (receiver = 'A'; receiver <= 'A'; receiver++)

  {  /* for */
    skew = ch_result[receiver - 'A'][RISING] - ch_result[receiver - 'A'][FALLING];

    V.result.UU.Real_result = skew;
    log_result(V.mod_id, V.result);
  }  /* for */

  close_syn(V.mod_id);
  close_gen(V.mod_id);
  close_mux(V.mod_id);
  close_cntr(V.mod_id);

}  /* main t10870 */
#undef TCLK_FREQ
#undef GEN_LOW_50
#undef GEN_HIGH_50
#undef RX_SPEC
#undef TIMEOUT
#undef STOP_WAIT
#undef PROG_DELAY_
#undef TCLK_ADV_TIME
#undef RES
#undef STARTING_INCREMENT

// ---  TEST 10871 - CARIBOU RECEIVER RISING EDGE PLACEMENT ACCURACY ---
// ---  DONE!!
#define TCLK_FREQ       5.0e+06
#define GEN_LOW_50      0.20e+00
#define GEN_HIGH_50     3.5e+00
#define RX_SPEC         10e-09
#define TIMEOUT         1.0e-03
#define STOP_WAIT       100e-03
#define PROG_DELAY_     20e-09
#define TCLK_ADV_TIME   40e-09
#define RES             1.0e-09
#define STARTING_INCREMENT  2.0e-09

/* static variables for t10871: */
struct LOC_t10871 {
  int mod_id;
  int slot;
  boolean rresp;
  boolean werr;
  boolean dummy;
  int receiver_no;
  double gen_delay;
  test_result_type result;
} ;

static void sample(struct LOC_t10871 *LINK)
{
  /* ------------------------------------------------------------------------ */
  /* Procedure sample sets up the generator delay to the value to be tested,  */
  /* enables the RRESP AND WERR signals onto the ARCV lines, starts and stops */
  /* the sequencer, and sets the return values for WERR and RRESP             */
  /* ------------------------------------------------------------------------ */
  setup_gen_trg_out_delay(LINK->mod_id, LINK->gen_delay);
  enable_gen_out(LINK->mod_id);
  sales_reset_sequencer(LINK->mod_id, LINK->slot);

  vth_pfc_sel_arcv0(LINK->mod_id, LINK->slot, LINK->receiver_no, 3);
      /* 3 = ARCV0 = RRESP */
  vth_pfc_sel_arcv1(LINK->mod_id, LINK->slot, LINK->receiver_no, 4);
      /* 4 = ARCV1 = WERR */

  sales_start_sequencer(LINK->mod_id, LINK->slot);
  vth_mseq_wait_on_stop(LINK->mod_id, STOP_WAIT);
  vth_mseq_stop(LINK->mod_id, TIMEOUT);

  vth_mmbd_rd_arcv1(LINK->mod_id);   /* vth_read WERR */
  LINK->dummy = get_result_nolog(LINK->mod_id, &LINK->result);
      /* get from result log */
  if (LINK->result.UU.Integer_result == 1)   /* WERR occurred */
    LINK->werr = true;
  else
  {  /* no WERR occurred */
    LINK->werr = false;
    vth_mmbd_rd_arcv0(LINK->mod_id);   /* get RRESP */
    LINK->dummy = get_result_nolog(LINK->mod_id, &LINK->result);
        /* get from result log */
    if (LINK->result.UU.Integer_result == 1)
      LINK->rresp = true;
    else
      LINK->rresp = false;
  }  /* no WERR occurred */
}  /* sample */

//------------------------------------------------------------------------------
/* MAIN t10871 [CARIBOU] RX Edge Rising edge Accuracy */
/* For path and test descriptions, see the documentation for test: 9171 */
//------------------------------------------------------------------------------
void t10871(int mod_id, int slot)
{
  struct LOC_t10871 V;
  int mapped_slot;
  boolean expected_response;
  boolean found;
  boolean hit;
  int direction;
  double t0_actual;
  double delay_inc;
  double expected_receive;
  double actual_receive;
  double min_gen_delay;
  double max_gen_delay;
  double min_gen_meas;
  double max_gen_meas;
  double rx_range;
  double delta_delay;
  double offset[2];
  double gain[2];
  double ch_result['H' + 1 - 'A'][2];
  int mux_node_pin;
  int edge;
  int receiver;
  int page;
  double correction;   //correction factor because results of this test are dependent
                       // on the number and type of cards in module

/* the .435ns is the correction factor for 1 single density card.  it was observed from
   Report data that indicated the shift in this test.  it was calculated as follows:

   with 2 DD cards, mean for test was 1.12ns
   with 9 DD cards, mean for test was -4.97ns
   delta mean = 1.12 - -4.97 = 6.09ns
   delta cards = 9-2 = 7
   correction per DD card = 6.09ns / 7 = .87ns
   correction per SD card is thus .87ns /2 = .435ns
   */

/*  The correction factor was calculated by (RISING):
	mean for 9 Caribou (DD) cards -> -1.45551nS
	Difference between loads -> 8 cards
	Total mean -> (1.45551/8)= .18194nS
*/

  correction = calc_mod_correction(mod_id, 0.6500e-09);

  V.mod_id = mod_id;
  V.slot = slot;
  if (V.slot > 11)
    mapped_slot = V.slot - 11;
  else
    mapped_slot = V.slot;

  open_syn(V.mod_id);
  open_gen(V.mod_id);
  open_mux(V.mod_id);
  open_cntr(V.mod_id);

  /* t0_actual is different for mac10s and mac11s.  This value is provided by */
  /* AutoCal software.  AutoCal defines this constant as T0_TIME */
//assuming that ACME uses the same autocal constant that mac11 uses -- mjs 6/98
  if ((get_testhead_actual_card(V.mod_id, MAC_SLOT) == CONTROLPLUS) || (get_testhead_actual_card(V.mod_id, MAC_SLOT) == CONTROLXT))
    t0_actual = MAC_11_T0_ACTUAL;
  else
    t0_actual = MAC_10_T0_ACTUAL;

  /* rx_range is the +/- max value we will check to see if a receive occurred */
  rx_range = 2 * RX_SPEC;

  /* delta_delay is the difference between when the generator trig will get */
  /* to CLKRB and when the generator output will get to the receiver mint pin */
  /* The values are:                                                      */
  /*  AF2_system_delay = delay of AF2 cable for module (unique for each system) */
  /*  slot_delay = delay associated with slot differences (slot 2 = 0.0 delay */
  /*  rx_path_offset = AF2 to receiver delay less slot delay and channel delay */
  /*  AF2_CLKRB_delay = sales rack specific constant; diff AF2 to CLKRB delays */
  delta_delay = af2_system_delay[V.mod_id] +
                (slot_delay[mapped_slot - 1][RISING]
                 + slot_delay[mapped_slot - 1][FALLING]) / 2
                 + rx_path_offset - af2_clkrb_delay[V.mod_id];

//  display_it(mod_id, "af2_system_delay = ", 0, af2_system_delay[V.mod_id]);
//  display_it(mod_id, "slot_delay[mapped_slot - 1][RISING]", 0, slot_delay[mapped_slot - 1][RISING]);
//  display_it(mod_id, "slot_delay[mapped_slot - 1][FALLING]", 0, slot_delay[mapped_slot - 1][FALLING]);
//  display_it(mod_id, "rx_path_offset = ", 0, rx_path_offset);
//  display_it(mod_id, "af2_clkrb_delay = ", 0, af2_clkrb_delay[V.mod_id]);

  /* expected_receive is when the receiver strobe should occur at no error */
  /* prog_delay is the delay value that the rx is programmed to in the VML code */
  /* tclk_adv_time is the amount of time we advance the tclk (from CLKRB).  We */
  /*  set(picked) this value such that our receive will occur within the delay */
  /*  range of the generator. */
  expected_receive = t0_actual + PROG_DELAY_ - TCLK_ADV_TIME;

//  display_it(mod_id, "t0_actual = ", 0, t0_actual);
  /* The following section calculates the gain and offset associated with the */
  /* generator delay settings.  These values are used to increase the accuracy */
  /* of the results.  This 2 point calibration assumes the delay is linear.  */

  /* min/max_gen_delay are the values over which the receiver will be checked */
//  display_it(mod_id, "expected_receive = ", 0, expected_receive);
//  display_it(mod_id, "delta_delay = ", 0, delta_delay);
//  display_it(mod_id, "rx_range = ", 0, rx_range);

  min_gen_delay = nanotrunc(expected_receive - delta_delay - rx_range);
  max_gen_delay = nanotrunc(expected_receive - delta_delay + rx_range);

//  display_it(mod_id, "min_gen_delay = ", 0, min_gen_delay);
//  display_it(mod_id, "max_gen_delay = ", 0, max_gen_delay);

  /* Make the required connections.  The generator is synced to the synthesizer */
  /* to provide a more accurate (less jitter) timebase for the generator. */
  connect_syn_out_gen_ext(V.mod_id);
  connect_gen_trg_cntr_start(V.mod_id);
  connect_gen_out_cntr_stop(V.mod_id);

  setup_gen_levels(V.mod_id, GEN_LOW_50, GEN_HIGH_50);

//  for (edge = RISING; edge <= FALLING; ++edge)
//  {  /* Measure the offset and gain for pos and neg generator output */
//    setup_gen_trg_out_delay(V.mod_id, min_gen_delay);
//    enable_gen_out(V.mod_id);
//    vth_mtmr_wait(V.mod_id, 0.3);
//    trigger_cntr(V.mod_id);
//    min_gen_meas = read_cntr_mean(V.mod_id);
//    setup_gen_trg_out_delay(V.mod_id, max_gen_delay);
//    vth_mtmr_wait(V.mod_id, 0.3);
//    trigger_cntr(V.mod_id);
//    max_gen_meas = read_cntr_mean(V.mod_id);
//    disable_gen_out(V.mod_id);
//    offset[edge] = min_gen_meas;
//    gain[edge] = (max_gen_meas - min_gen_meas) / (max_gen_delay - min_gen_delay);
//    complement_gen_out(V.mod_id);
//    setup_cntr_trigger_slopes(V.mod_id, POS, NEG);
//  }  /* Measure the offset and gain for pos and neg generator output */

/* Measure the offset and gain for pos and neg generator output */

  edge = RISING;
  setup_gen_trg_out_delay(V.mod_id, min_gen_delay);
  enable_gen_out(V.mod_id);
  vth_mtmr_wait(V.mod_id, 0.3);
  trigger_cntr(V.mod_id);
  min_gen_meas = read_cntr_mean(V.mod_id);
  setup_gen_trg_out_delay(V.mod_id, max_gen_delay);
  vth_mtmr_wait(V.mod_id, 0.3);
  trigger_cntr(V.mod_id);
  max_gen_meas = read_cntr_mean(V.mod_id);
  disable_gen_out(V.mod_id);
  offset[edge] = min_gen_meas;
  gain[edge] = (max_gen_meas - min_gen_meas) / (max_gen_delay - min_gen_delay);
  complement_gen_out(V.mod_id);
  setup_cntr_trigger_slopes(V.mod_id, POS, NEG);

/* Measure the offset and gain for pos and neg generator output */

  no_complement_gen_out(V.mod_id);
  setup_gen_levels(V.mod_id, GEN_LOW_50, GEN_HIGH_50);

  connect_gen_trg_clkrb(V.mod_id);
  connect_gen_out_af2(V.mod_id);

  vth_mclose_rly(V.mod_id, MK2902);   /* clkrb path relay */
  vth_mclose_rly(V.mod_id, MK2903);   /* clkrsg path relay */
  vth_aclose_rly(V.mod_id, K753);   /* XL to AF2 path relay */
  vth_aclose_rly(V.mod_id, K764);   /* XL to AF2 path relay */


// Initialize card to compatibility mode
// Which of these does what we really want??
  vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x0);
  vth_pdis_32chan(V.mod_id, V.slot);

/* THIS IS where compatibility loop will need to start */
 for (page = 0; page <= 1; page++)
 {

//        // Becoming standard way to open/close compatibility relays
//        if (page == 0)
//        {
//          vth_pen_32chan(mod_id, slot);
//          vth_pmap_rly(mod_id, slot, 0);
//          vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
//        }
//        else  /* page == 1 */
//        {
//          vth_pdis_32chan(mod_id, slot);
//          vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
//          vth_pen_32chan(mod_id, slot);
//          vth_pmap_rly(mod_id, slot, 0);
//          vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
//        }

//
  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /* for */
    vth_pdis_32chan(V.mod_id, V.slot);
    vth_pclose_rly(V.mod_id, V.slot, PIN_KR_RLYS[receiver - 'A']);
    vth_pclose_rly(V.mod_id, V.slot, PIN_KX_RLYS[receiver - 'A']);
    switch (receiver)
    {   /*Use mux_node_pins closest to the grounds*/

    case 'A':
    case 'B':
    case 'E':
    case 'F':
      mux_node_pin = 7;
      break;

    case 'C':
    case 'D':
    case 'G':
    case 'H':
      mux_node_pin = 0;
      break;
    default:
      assert(false);
      break;

    }/* case */
    vth_pclose_rly(V.mod_id, V.slot, PIN_DRVR_RLYS[mux_node_pin][receiver - 'A']);
    vth_pclose_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[mux_node_pin + 1][receiver - 'A']);
  }  /* for */


  close_pin_gnd_relays(V.mod_id, V.slot);
      /*ground all termination resistors */
  vth_pclose_rly(V.mod_id, V.slot, KDCL);

  /* Set up the TCLK generator to sync to the external CLKRB (gen trig) signal */
  vth_mtclk_clkrb_mux(V.mod_id);
  vth_mtclk_sel_ris_edge(V.mod_id);
  vth_mtclk_set_freq(V.mod_id, TCLK_FREQ, TCLK_FREQ);
  vth_mtclk_set_adv_time(V.mod_id, TCLK_ADV_TIME);

  /* Enable the ARCV lines from pin to mother card and from mother card to mac */
  vth_pen_arcv0(V.mod_id, V.slot);
  vth_pen_arcv1(V.mod_id, V.slot);
  vth_mmbd_en_arcv(V.mod_id);

#include "t10871_vml.cmd"

//  for (edge = RISING; edge <= FALLING; ++edge)
//  {  /* for edge */
//    if (edge == RISING)
//    {  /* then */
//      no_complement_gen_out(V.mod_id);   /* Use at the rising edge */
//      expected_response = true;
//    }  /* then */
//    else
//    {  /* else */
//      complement_gen_out(V.mod_id);   /* Use at the falling edge */
//      expected_response = false;
//    }  /* else */

  edge = RISING;
  no_complement_gen_out(V.mod_id);   /* Use at the rising edge */
  expected_response = true;

/* Recalculate delta_delay for the particular edge that we are testing */
  delta_delay = af2_system_delay[V.mod_id] + slot_delay[mapped_slot - 1]
                     [edge] + rx_path_offset - af2_clkrb_delay[V.mod_id];

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /* for receiver */
     V.receiver_no = receiver - 'A' + 8 * page;

//      /* Two kgl relays are closed for each receiver.  This effectively puts */
//      /* two 100 ohm load resistors in parallel to simulate a 50 ohm load. */
//     vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);
    vth_pdis_32chan(V.mod_id, V.slot);
    vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);
      switch (receiver)
      {   /*Use mux_node_pins closest to the grounds*/

      case 'A':
      case 'C':
      case 'E':
      case 'G':
        vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' + 1]);
        break;

      case 'B':
      case 'D':
      case 'F':
      case 'H':
        vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' - 1]);
        break;
      default:
        assert(false);
        break;

      }/* case */

      delay_inc = STARTING_INCREMENT;
      direction = 0;
      hit = false;
      found = false;
      V.gen_delay = nanotrunc(expected_receive - delta_delay - 1e-09);
      vth_pen_32chan(V.mod_id, V.slot);

      do
      {

        sample(&V);   /* Sample sets the value of WERR and RRESP */

//display_it(mod_id, "V.rresp  ", 0, V.rresp);
//  display_it(mod_id, "V.gen_delay  ", 0, V.gen_delay);
// MIKE

	/* If a WERR occurs, the current gen delay is the closest tested */
        /* value of gen delay to rx strobe */
        if (V.werr)
          found = true;
        else
        {  /* Do the non-window error procedure */
          /* Determine if 'hit' and set the direction of delay based on rresp */
          if (V.rresp == expected_response)
          {  /* increase the delay */
            if (direction < 0)   /* can never hit on first point */
              hit = true;
            direction = 1;   /* set direction to increase delay */
          }  /* increase the delay */
          else
          {  /* decrease the delay */
            if (direction > 0)   /* can never hit on first point */
              hit = true;
            direction = -1;   /* set direction to decrease delay */
          }  /* decrease the delay */

          /* Each time a 'hit' occurs, cut the delay increment in half. 'found' */
          /* will only be set true when a 'hit' occurs at the 'res'(resolution) */
          /* value of generator delay. */
          if (hit)
          {
            if (delay_inc <= RES)
              found = true;
            delay_inc /= 2;
          }

          /* On the final 'hit' value of gen delay, the gen delay value will */
          /* be adjusted by 1/2 the 'res' value in the proper direction to */
          /* effectively average the RRESP=0 and RRESP=1 values.  The rx strobe */
          /* occurred somewhere between the RRESP=0 and RRESP=1. */
          V.gen_delay += delay_inc * direction;

//display_it(mod_id, "V.gen_delay  ", 0, V.gen_delay);
// MIKE

        }  /* Do the non-window error procedure */

      } while (!(found || V.gen_delay < min_gen_delay ||
                 V.gen_delay > max_gen_delay));


      disable_gen_out(V.mod_id);

      vth_pdis_32chan(V.mod_id, V.slot);
      vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);

      switch (receiver)
      {   /*Use mux_node_pins closest to the grounds*/

      case 'A':
      case 'C':
      case 'E':
      case 'G':
        vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' + 1]);
        break;

      case 'B':
      case 'D':
      case 'F':
      case 'H':
        vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' - 1]);
        break;
      default:
        assert(false);
        break;

      }/* case */

      /* calculate the actual receive, correct for gen error and channel delay */
      actual_receive = gain[edge] * (V.gen_delay - min_gen_delay) +
          offset[edge] + channel_delay[receiver - 'A'] + delta_delay;

//display_it(mod_id, "actual_receive ", 0, actual_receive);
//display_it(mod_id, "expected_receive ", 0, expected_receive);
// MIKE

      ch_result[receiver - 'A'][edge] = actual_receive - expected_receive;
      V.result.Measurement_class = CLASS_REAL;


      if (!found)
      {   /* it never received */
        display_prompt("**RECEIVE DOES NOT OCCUR WITHIN TEST RANGE**", true);
//        ch_result[receiver - 'A'][edge] = INT_MAX;
        ch_result[receiver - 'A'][edge] = 0.00000002;
      }


      //added the correction factor to test to correct for module configuration
      //see calculations above at top of test
      //mjs - 6/97
      V.result.UU.Real_result = ch_result[receiver - 'A'][edge] + correction;
      log_result(V.mod_id, V.result);

    }  /* for receiver */

//  }  /* for edge */

  }  /* for page */

  close_syn(V.mod_id);
  close_gen(V.mod_id);
  close_mux(V.mod_id);
  close_cntr(V.mod_id);

}  /* END MAIN t10871 */
#undef TCLK_FREQ
#undef GEN_LOW_50
#undef GEN_HIGH_50
#undef RX_SPEC
#undef TIMEOUT
#undef STOP_WAIT
#undef PROG_DELAY_
#undef TCLK_ADV_TIME
#undef RES
#undef STARTING_INCREMENT
/* ####### END 10871 ######## */

// --- #### TEST 10872 Receiver Falling EDGE PLACEMENT ACCURACY ####---
#define TCLK_FREQ       5.0e+06
#define GEN_LOW_50      0.20e+00
#define GEN_HIGH_50     3.5e+00
#define RX_SPEC         10e-09
#define TIMEOUT         1.0e-03
#define STOP_WAIT       100e-03
#define PROG_DELAY_     20e-09
#define TCLK_ADV_TIME   40e-09
#define RES             1.0e-09
#define STARTING_INCREMENT  2.0e-09

/* static variables for t10872: */
struct LOC_t10872
{
  int mod_id;
  int slot;
  boolean rresp;
  boolean werr;
  boolean dummy;
  int receiver_no;
  double gen_delay;
  test_result_type result;
};

static void sample(struct LOC_t10872 *LINK)
{
  /* ------------------------------------------------------------------------ */
  /* Procedure sample sets up the generator delay to the value to be tested,  */
  /* enables the RRESP AND WERR signals onto the ARCV lines, starts and stops */
  /* the sequencer, and sets the return values for WERR and RRESP             */
  /* ------------------------------------------------------------------------ */
  setup_gen_trg_out_delay(LINK->mod_id, LINK->gen_delay);
  enable_gen_out(LINK->mod_id);
  sales_reset_sequencer(LINK->mod_id, LINK->slot);

  vth_pfc_sel_arcv0(LINK->mod_id, LINK->slot, LINK->receiver_no, 3);
      /* 3 = ARCV0 = RRESP */
  vth_pfc_sel_arcv1(LINK->mod_id, LINK->slot, LINK->receiver_no, 4);
      /* 4 = ARCV1 = WERR */

  sales_start_sequencer(LINK->mod_id, LINK->slot);
  vth_mseq_wait_on_stop(LINK->mod_id, STOP_WAIT);
  vth_mseq_stop(LINK->mod_id, TIMEOUT);

  vth_mmbd_rd_arcv1(LINK->mod_id);   /* vth_read WERR */
  LINK->dummy = get_result_nolog(LINK->mod_id, &LINK->result);
      /* get from result log */
  if (LINK->result.UU.Integer_result == 1)   /* WERR occurred */
    LINK->werr = true;
  else
  {  /* no WERR occurred */
    LINK->werr = false;
    vth_mmbd_rd_arcv0(LINK->mod_id);   /* get RRESP */
    LINK->dummy = get_result_nolog(LINK->mod_id, &LINK->result);
        /* get from result log */
    if (LINK->result.UU.Integer_result == 1)
      LINK->rresp = true;
    else
      LINK->rresp = false;
  }  /* no WERR occurred */
}  /* sample */

//------------------------------------------------------------------------------
/* MAIN t10872 [CARIBOU] RX Edge Falling edge Accuracy */
/* For path and test descriptions, see the documentation for test: 9171 */
//------------------------------------------------------------------------------
void t10872(int mod_id, int slot)
{
  struct LOC_t10872 V;
  int mapped_slot;
  boolean expected_response;
  boolean found;
  boolean hit;
  int direction;
  double t0_actual;
  double delay_inc;
  double expected_receive;
  double actual_receive;
  double min_gen_delay;
  double max_gen_delay;
  double min_gen_meas;
  double max_gen_meas;
  double rx_range;
  double delta_delay;
  double offset[2];
  double gain[2];
  double ch_result['H' + 1 - 'A'][2];
  int mux_node_pin;
  int edge;
  int receiver;
  int shift;
  int page;

  V.mod_id = mod_id;
  V.slot = slot;
  if (V.slot > 11)
    mapped_slot = V.slot - 11;
  else
    mapped_slot = V.slot;

  open_syn(V.mod_id);
  open_gen(V.mod_id);
  open_mux(V.mod_id);
  open_cntr(V.mod_id);

  /* t0_actual is different for mac10s and mac11s.  This value is provided by */
  /* AutoCal software.  AutoCal defines this constant as T0_TIME */
//assuming that ACME uses the same autocal constant that mac11 uses -- mjs 6/98
  if ((get_testhead_actual_card(V.mod_id, MAC_SLOT) == CONTROLPLUS) || (get_testhead_actual_card(V.mod_id, MAC_SLOT) == CONTROLXT))
    t0_actual = MAC_11_T0_ACTUAL;
  else
    t0_actual = MAC_10_T0_ACTUAL;

  /* rx_range is the +/- max value we will check to see if a receive occurred */
  rx_range = 2 * RX_SPEC;

  /* delta_delay is the difference between when the generator trig will get */
  /* to CLKRB and when the generator output will get to the receiver mint pin */
  /* The values are:                                                      */
  /*  AF2_system_delay = delay of AF2 cable for module (unique for each system) */
  /*  slot_delay = delay associated with slot differences (slot 2 = 0.0 delay */
  /*  rx_path_offset = AF2 to receiver delay less slot delay and channel delay */
  /*  AF2_CLKRB_delay = sales rack specific constant; diff AF2 to CLKRB delays */
  delta_delay = af2_system_delay[V.mod_id] +
                (slot_delay[mapped_slot - 1][RISING]
                 + slot_delay[mapped_slot - 1][FALLING]) / 2
                 + rx_path_offset - af2_clkrb_delay[V.mod_id];

//  display_it(mod_id, "af2_system_delay = ", 0, af2_system_delay[V.mod_id]);
//  display_it(mod_id, "slot_delay[mapped_slot - 1][RISING]", 0, slot_delay[mapped_slot - 1][RISING]);
//  display_it(mod_id, "slot_delay[mapped_slot - 1][FALLING]", 0, slot_delay[mapped_slot - 1][FALLING]);
//  display_it(mod_id, "rx_path_offset = ", 0, rx_path_offset);
//  display_it(mod_id, "af2_clkrb_delay = ", 0, af2_clkrb_delay[V.mod_id]);

  /* expected_receive is when the receiver strobe should occur at no error */
  /* prog_delay is the delay value that the rx is programmed to in the VML code */
  /* tclk_adv_time is the amount of time we advance the tclk (from CLKRB).  We */
  /*  set(picked) this value such that our receive will occur within the delay */
  /*  range of the generator. */
  expected_receive = t0_actual + PROG_DELAY_ - TCLK_ADV_TIME;

//  display_it(mod_id, "t0_actual = ", 0, t0_actual);
  /* The following section calculates the gain and offset associated with the */
  /* generator delay settings.  These values are used to increase the accuracy */
  /* of the results.  This 2 point calibration assumes the delay is linear.  */

  /* min/max_gen_delay are the values over which the receiver will be checked */
//  display_it(mod_id, "expected_receive = ", 0, expected_receive);
//  display_it(mod_id, "delta_delay = ", 0, delta_delay);
//  display_it(mod_id, "rx_range = ", 0, rx_range);

  min_gen_delay = nanotrunc(expected_receive - delta_delay - rx_range);
  max_gen_delay = nanotrunc(expected_receive - delta_delay + rx_range);

//  display_it(mod_id, "min_gen_delay = ", 0, min_gen_delay);
//  display_it(mod_id, "max_gen_delay = ", 0, max_gen_delay);

  /* Make the required connections.  The generator is synced to the synthesizer */
  /* to provide a more accurate (less jitter) timebase for the generator. */
  connect_syn_out_gen_ext(V.mod_id);
  connect_gen_trg_cntr_start(V.mod_id);
  connect_gen_out_cntr_stop(V.mod_id);

  setup_gen_levels(V.mod_id, GEN_LOW_50, GEN_HIGH_50);

//  for (edge = RISING; edge <= FALLING; ++edge)
//  {  /* Measure the offset and gain for pos and neg generator output */
//    setup_gen_trg_out_delay(V.mod_id, min_gen_delay);
//    enable_gen_out(V.mod_id);
//    vth_mtmr_wait(V.mod_id, 0.3);
//    trigger_cntr(V.mod_id);
//    min_gen_meas = read_cntr_mean(V.mod_id);
//    setup_gen_trg_out_delay(V.mod_id, max_gen_delay);
//    vth_mtmr_wait(V.mod_id, 0.3);
//    trigger_cntr(V.mod_id);
//    max_gen_meas = read_cntr_mean(V.mod_id);
//    disable_gen_out(V.mod_id);
//    offset[edge] = min_gen_meas;
//    gain[edge] = (max_gen_meas - min_gen_meas) / (max_gen_delay - min_gen_delay);
//    complement_gen_out(V.mod_id);
//    setup_cntr_trigger_slopes(V.mod_id, POS, NEG);
//  }  /* Measure the offset and gain for pos and neg generator output */

/* Measure the offset and gain for pos and neg generator output */

  edge = FALLING;
  setup_gen_trg_out_delay(V.mod_id, min_gen_delay);
  enable_gen_out(V.mod_id);
  vth_mtmr_wait(V.mod_id, 0.3);
  trigger_cntr(V.mod_id);
  min_gen_meas = read_cntr_mean(V.mod_id);
  setup_gen_trg_out_delay(V.mod_id, max_gen_delay);
  vth_mtmr_wait(V.mod_id, 0.3);
  trigger_cntr(V.mod_id);
  max_gen_meas = read_cntr_mean(V.mod_id);
  disable_gen_out(V.mod_id);
  offset[edge] = min_gen_meas;
  gain[edge] = (max_gen_meas - min_gen_meas) / (max_gen_delay - min_gen_delay);
  complement_gen_out(V.mod_id);
  setup_cntr_trigger_slopes(V.mod_id, POS, NEG);

/* Measure the offset and gain for pos and neg generator output */

  no_complement_gen_out(V.mod_id);
  setup_gen_levels(V.mod_id, GEN_LOW_50, GEN_HIGH_50);

  connect_gen_trg_clkrb(V.mod_id);
  connect_gen_out_af2(V.mod_id);

  vth_mclose_rly(V.mod_id, MK2902);   /* clkrb path relay */
  vth_mclose_rly(V.mod_id, MK2903);   /* clkrsg path relay */
  vth_aclose_rly(V.mod_id, K753);   /* XL to AF2 path relay */
  vth_aclose_rly(V.mod_id, K764);   /* XL to AF2 path relay */


// Initialize card to compatibility mode
// Which of these does what we really want??
  vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x0);
  vth_pdis_32chan(V.mod_id, V.slot);

/* THIS IS where compatibility loop will need to start */
 for (page = 0; page <= 1; page++)
 {

//        // Becoming standard way to open/close compatibility relays
//        if (page == 0)
//        {
//          vth_pen_32chan(mod_id, slot);
//          vth_pmap_rly(mod_id, slot, 0);
//          vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
//        }
//        else  /* page == 1 */
//        {
//          vth_pdis_32chan(mod_id, slot);
//          vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
//          vth_pen_32chan(mod_id, slot);
//          vth_pmap_rly(mod_id, slot, 0);
//          vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);
//        }

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /* for */
    vth_pdis_32chan(V.mod_id, V.slot);
    vth_pclose_rly(V.mod_id, V.slot, PIN_KR_RLYS[receiver - 'A']);
    vth_pclose_rly(V.mod_id, V.slot, PIN_KX_RLYS[receiver - 'A']);
    switch (receiver)
    {   /*Use mux_node_pins closest to the grounds*/

    case 'A':
    case 'B':
    case 'E':
    case 'F':
      mux_node_pin = 7;
      break;

    case 'C':
    case 'D':
    case 'G':
    case 'H':
      mux_node_pin = 0;
      break;
    default:
      assert(false);
      break;

    }/* case */
    vth_pclose_rly(V.mod_id, V.slot, PIN_DRVR_RLYS[mux_node_pin][receiver - 'A']);
    vth_pclose_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[mux_node_pin + 1][receiver - 'A']);
  }  /* for */


  close_pin_gnd_relays(V.mod_id, V.slot);
      /*ground all termination resistors */
  vth_pclose_rly(V.mod_id, V.slot, KDCL);

  /* Set up the TCLK generator to sync to the external CLKRB (gen trig) signal */
  vth_mtclk_clkrb_mux(V.mod_id);
  vth_mtclk_sel_ris_edge(V.mod_id);
  vth_mtclk_set_freq(V.mod_id, TCLK_FREQ, TCLK_FREQ);
  vth_mtclk_set_adv_time(V.mod_id, TCLK_ADV_TIME);

  /* Enable the ARCV lines from pin to mother card and from mother card to mac */
  vth_pen_arcv0(V.mod_id, V.slot);
  vth_pen_arcv1(V.mod_id, V.slot);
  vth_mmbd_en_arcv(V.mod_id);

#include "t10871_vml.cmd"

//  for (edge = RISING; edge <= FALLING; ++edge)
//  {  /* for edge */
//    if (edge == RISING)
//    {  /* then */
//      no_complement_gen_out(V.mod_id);   /* Use at the rising edge */
//      expected_response = true;
//    }  /* then */
//    else
//    {  /* else */
//      complement_gen_out(V.mod_id);   /* Use at the falling edge */
//      expected_response = false;
//    }  /* else */

  edge = FALLING;
  complement_gen_out(V.mod_id);   /* Use at the falling edge */
  expected_response = true;

/* Recalculate delta_delay for the particular edge that we are testing */
  delta_delay = af2_system_delay[V.mod_id] + slot_delay[mapped_slot - 1]
                     [edge] + rx_path_offset - af2_clkrb_delay[V.mod_id];

  for (receiver = 'A'; receiver <= 'H'; receiver++)
  {  /* for receiver */
     V.receiver_no = receiver - 'A' + 8 * page;
     shift = 0;


//      /* Two kgl relays are closed for each receiver.  This effectively puts */
//      /* two 100 ohm load resistors in parallel to simulate a 50 ohm load. */
//     vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);
    vth_pdis_32chan(V.mod_id, V.slot);
    vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);
      switch (receiver)
      {   /*Use mux_node_pins closest to the grounds*/

      case 'A':
      case 'C':
      case 'E':
      case 'G':
        vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' + 1]);
        break;

      case 'B':
      case 'D':
      case 'F':
      case 'H':
	vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' - 1]);
        break;
      default:
        assert(false);
        break;

      }/* case */

      delay_inc = STARTING_INCREMENT;
      direction = 0;
      hit = false;
      found = false;
      V.gen_delay = nanotrunc(expected_receive - delta_delay + 1e-09);

     vth_pen_32chan(V.mod_id, V.slot);

      do
      {

	sample(&V);   /* Sample sets the value of WERR and RRESP */

	/* If a WERR occurs, the current gen delay is the closest tested */
        /* value of gen delay to rx strobe */
        if (V.werr)
          found = true;
        else
        {  /* Do the non-window error procedure */

//  display_it(mod_id, "V.rresp  ", 0, V.rresp);
//  display_it(mod_id, "V.gen_delay  ", 0, V.gen_delay);
// MIKE

	   /* Determine if 'hit' and set the direction of delay based on rresp */
          if (V.rresp == expected_response)
          {  /* increase the delay */
            if (direction < 0)   /* can never hit on first point */
              hit = true;
            direction = 1;   /* set direction to increase delay */
          }  /* increase the delay */
          else
          {  /* decrease the delay */
            if (direction > 0)   /* can never hit on first point */
              hit = true;
            direction = -1;   /* set direction to decrease delay */
          }  /* decrease the delay */

          /* Each time a 'hit' occurs, cut the delay increment in half. 'found' */
          /* will only be set true when a 'hit' occurs at the 'res'(resolution) */
          /* value of generator delay. */
          if (hit)
          {
            if (delay_inc <= RES)
              found = true;
            delay_inc /= 2;
          }

          /* On the final 'hit' value of gen delay, the gen delay value will */
          /* be adjusted by 1/2 the 'res' value in the proper direction to */
          /* effectively average the RRESP=0 and RRESP=1 values.  The rx strobe */
          /* occurred somewhere between the RRESP=0 and RRESP=1. */

	  /* Added this if statement to force V.gen_delay to move left after */
	  /* receiving the first true rresp. This was forced due to overshooting */
	  /*  the edge on even rx responses. MD 12/16/00 */

          if (shift == 0)
	  {
	     direction = -1;
	     shift += 1;
          }

	  V.gen_delay += delay_inc * direction;

        }  /* Do the non-window error procedure */

//  display_it(mod_id, "V.gen_delay  ", 0, V.gen_delay);
// MIKE

      } while (!(found || V.gen_delay < min_gen_delay ||
                 V.gen_delay > max_gen_delay));


      disable_gen_out(V.mod_id);

      vth_pdis_32chan(V.mod_id, V.slot);
      vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);

      switch (receiver)
      {   /*Use mux_node_pins closest to the grounds*/

      case 'A':
      case 'C':
      case 'E':
      case 'G':
        vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' + 1]);
        break;

      case 'B':
      case 'D':
      case 'F':
      case 'H':
        vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' - 1]);
        break;
      default:
        assert(false);
        break;

      }/* case */

      /* calculate the actual receive, correct for gen error and channel delay */
      actual_receive = gain[edge] * (V.gen_delay - min_gen_delay) +
          offset[edge] + channel_delay[receiver - 'A'] + delta_delay;

// display_it(mod_id, "actual_receive ", 0, actual_receive);
// MIKE

      ch_result[receiver - 'A'][edge] = actual_receive - expected_receive;
      V.result.Measurement_class = CLASS_REAL;

    if (!found)
      {   /* it never received */
        display_prompt("**RECEIVE DOES NOT OCCUR WITHIN TEST RANGE**", true);
//        ch_result[receiver - 'A'][edge] = INT_MAX;
        ch_result[receiver - 'A'][edge] = 0.00000002;
      }

      V.result.UU.Real_result = ch_result[receiver - 'A'][edge];
      log_result(V.mod_id, V.result);
    }  /* for receiver */

//  }  /* for edge */

 }  /* for page */

  close_syn(V.mod_id);
  close_gen(V.mod_id);
  close_mux(V.mod_id);
  close_cntr(V.mod_id);

}  /* END MAIN t10872 */
#undef TCLK_FREQ
#undef GEN_LOW_50
#undef GEN_HIGH_50
#undef RX_SPEC
#undef TIMEOUT
#undef STOP_WAIT
#undef PROG_DELAY_
#undef TCLK_ADV_TIME
#undef RES
#undef STARTING_INCREMENT
/* ####### END OF 10872 ######## */

// ---  TEST 10873 - CARIBOU RECEIVER EDGE PLACEMENT ACCURACY AND SKEW  ---
// ---  IN PROGRESS!!  NOT NEEDED BUT KEEP FOR NOW 3-20
#define TCLK_FREQ       5.0e+06
#define GEN_LOW_50      0.20e+00
#define GEN_HIGH_50     3.5e+00
#define RX_SPEC         10e-09
#define TIMEOUT         1.0e-03
#define STOP_WAIT       100e-03
#define PROG_DELAY_     20e-09
#define TCLK_ADV_TIME   40e-09
#define RES             1.0e-09
#define STARTING_INCREMENT  2.0e-09

/* static variables for t10873: */
struct LOC_t10873
{
  int mod_id;
  int slot;
  boolean rresp;
  boolean werr;
  boolean dummy;
  int receiver_no;
  double gen_delay;
  test_result_type result;
} ;

static void sample(struct LOC_t10873 *LINK)
{
  /* ------------------------------------------------------------------------ */
  /* Procedure sample sets up the generator delay to the value to be tested,  */
  /* enables the RRESP AND WERR signals onto the ARCV lines, starts and stops */
  /* the sequencer, and sets the return values for WERR and RRESP             */
  /* ------------------------------------------------------------------------ */
  setup_gen_trg_out_delay(LINK->mod_id, LINK->gen_delay);
  enable_gen_out(LINK->mod_id);
  sales_reset_sequencer(LINK->mod_id, LINK->slot);

  vth_pfc_sel_arcv0(LINK->mod_id, LINK->slot, LINK->receiver_no, 3);
      /* 3 = ARCV0 = RRESP */
  vth_pfc_sel_arcv1(LINK->mod_id, LINK->slot, LINK->receiver_no, 4);
      /* 4 = ARCV1 = WERR */

  sales_start_sequencer(LINK->mod_id, LINK->slot);
  vth_mseq_wait_on_stop(LINK->mod_id, STOP_WAIT);
  vth_mseq_stop(LINK->mod_id, TIMEOUT);

  vth_mmbd_rd_arcv1(LINK->mod_id);   /* vth_read WERR */
  LINK->dummy = get_result_nolog(LINK->mod_id, &LINK->result);
      /* get from result log */
  if (LINK->result.UU.Integer_result == 1)   /* WERR occurred */
    LINK->werr = true;
  else
  {  /* no WERR occurred */
    LINK->werr = false;
    vth_mmbd_rd_arcv0(LINK->mod_id);   /* get RRESP */
    LINK->dummy = get_result_nolog(LINK->mod_id, &LINK->result);
        /* get from result log */
    if (LINK->result.UU.Integer_result == 1)
      LINK->rresp = true;
    else
      LINK->rresp = false;
  }  /* no WERR occurred */
}  /* END SAMPLE struct  */

//------------------------------------------------------------------------------
void t10873(int mod_id, int slot)
{  /* main t10873 */
  /* [CARIBOU] RX Edge Accuracy and Skew */

  /* For path and test descriptions, see the documentation for test: 9171 */
  struct LOC_t10873 V;
  int mapped_slot;
  boolean expected_response;
  boolean found;
  boolean hit;
  int direction;
  double t0_actual;
  double delay_inc;
  double expected_receive;
  double actual_receive;
  double min_gen_delay;
  double max_gen_delay;
  double min_gen_meas;
  double max_gen_meas;
  double rx_range;
  double skew;
  double delta_delay;
  double offset[2];
  double gain[2];
  double ch_result['H' + 1 - 'A'][2];
  int mux_node_pin;
  int receiver;

//  FILE *diagfile;   // diagnostic file to check how well the correction for this test works -- mjs
//  diagfile = NULL;

  //mjs 6/97
  double correction;   //correction factor because results of this test are dependent
                       // on the number and type of cards in module

/* the .435ns is the correction factor for 1 single density card.  it was observed from
   Report data that indicated the shift in this test.  it was calculated as follows:

   with 2 DD cards, mean for test was 1.12ns
   with 9 DD cards, mean for test was -4.97ns
   delta mean = 1.12 - -4.97 = 6.09ns
   delta cards = 9-2 = 7
   correction per DD card = 6.09ns / 7 = .87ns
   correction per SD card is thus .87ns /2 = .435ns
   */

//  correction = calc_mod_correction(mod_id, .435e-09);

    correction = calc_mod_correction(mod_id, .2e-09);
//mjs - changed the correction factor down to keep from going out of high end.
//      changed on 6/30 after seeing results for a module of single density cards
/*  diagfile = fopen("diag.mjs", "a");
  fprintf(diagfile, "===============================================\n", mod_id, slot);
  fprintf(diagfile, "TEST 9171/9871         module %2d   slot %2d \n\n", mod_id, slot);
  fprintf(diagfile, "single density correction constant = .2e-09\n");
  fprintf(diagfile, "calculated correction for Module %d is %1.12f\n", mod_id, correction);
*/

  V.mod_id = mod_id;
  V.slot = slot;
  if (V.slot > 11)
    mapped_slot = V.slot - 11;
  else
    mapped_slot = V.slot;

  open_syn(V.mod_id);
  open_gen(V.mod_id);
  open_mux(V.mod_id);
  open_cntr(V.mod_id);

  /* t0_actual is different for mac10s and mac11s.  This value is provided by */
  /* AutoCal software.  AutoCal defines this constant as T0_TIME */
//assuming that ACME uses the same autocal constant that mac11 uses -- mjs 6/98
  if ((get_testhead_actual_card(V.mod_id, MAC_SLOT) == CONTROLPLUS) || (get_testhead_actual_card(V.mod_id, MAC_SLOT) == CONTROLXT))
    t0_actual = MAC_11_T0_ACTUAL;
  else
    t0_actual = MAC_10_T0_ACTUAL;

  /* rx_range is the +/- max value we will check to see if a receive occurred */
  rx_range = 2 * RX_SPEC;

  /* delta_delay is the difference between when the generator trig will get */
  /* to CLKRB and when the generator output will get to the receiver mint pin */
  /* The values are:                                                      */
  /*  AF2_system_delay = delay of AF2 cable for module (unique for each system) */
  /*  slot_delay = delay associated with slot differences (slot 2 = 0.0 delay */
  /*  rx_path_offset = AF2 to receiver delay less slot delay and channel delay */
  /*  AF2_CLKRB_delay = sales rack specific constant; diff AF2 to CLKRB delays */
  delta_delay = af2_system_delay[V.mod_id] +
                (slot_delay[mapped_slot - 1][RISING]
                 + slot_delay[mapped_slot - 1][FALLING]) / 2
                 + rx_path_offset - af2_clkrb_delay[V.mod_id];

//  display_it(mod_id, "af2_system_delay = ", 0, af2_system_delay[V.mod_id]);
//  display_it(mod_id, "slot_delay[mapped_slot - 1][RISING]", 0, slot_delay[mapped_slot - 1][RISING]);
//  display_it(mod_id, "slot_delay[mapped_slot - 1][FALLING]", 0, slot_delay[mapped_slot - 1][FALLING]);
//  display_it(mod_id, "rx_path_offset = ", 0, rx_path_offset);
//  display_it(mod_id, "af2_clkrb_delay = ", 0, af2_clkrb_delay[V.mod_id]);

  /* expected_receive is when the receiver strobe should occur at no error */
  /* prog_delay is the delay value that the rx is programmed to in the VML code */
  /* tclk_adv_time is the amount of time we advance the tclk (from CLKRB).  We */
  /*  set(picked) this value such that our receive will occur within the delay */
  /*  range of the generator. */
  expected_receive = t0_actual + PROG_DELAY_ - TCLK_ADV_TIME;

//  display_it(mod_id, "t0_actual = ", 0, t0_actual);
  /* The following section calculates the gain and offset associated with the */
  /* generator delay settings.  These values are used to increase the accuracy */
  /* of the results.  This 2 point calibration assumes the delay is linear.  */

  /* min/max_gen_delay are the values over which the receiver will be checked */
//  display_it(mod_id, "expected_receive = ", 0, expected_receive);
//  display_it(mod_id, "delta_delay = ", 0, delta_delay);
//  display_it(mod_id, "rx_range = ", 0, rx_range);

  min_gen_delay = nanotrunc(expected_receive - delta_delay - rx_range);
  max_gen_delay = nanotrunc(expected_receive - delta_delay + rx_range);

//  display_it(mod_id, "min_gen_delay = ", 0, min_gen_delay);
//  display_it(mod_id, "max_gen_delay = ", 0, max_gen_delay);

  /* Make the required connections.  The generator is synced to the synthesizer */
  /* to provide a more accurate (less jitter) timebase for the generator. */
  connect_syn_out_gen_ext(V.mod_id);
  connect_gen_trg_cntr_start(V.mod_id);
  connect_gen_out_cntr_stop(V.mod_id);

  setup_gen_levels(V.mod_id, GEN_LOW_50, GEN_HIGH_50);

  for (int edge = RISING; edge <= FALLING; ++edge)
  {  /* Measure the offset and gain for pos and neg generator output */
    setup_gen_trg_out_delay(V.mod_id, min_gen_delay);
    enable_gen_out(V.mod_id);
    vth_mtmr_wait(V.mod_id, 0.3);
    trigger_cntr(V.mod_id);
    min_gen_meas = read_cntr_mean(V.mod_id);
    setup_gen_trg_out_delay(V.mod_id, max_gen_delay);
    vth_mtmr_wait(V.mod_id, 0.3);
    trigger_cntr(V.mod_id);
    max_gen_meas = read_cntr_mean(V.mod_id);
    disable_gen_out(V.mod_id);
    offset[edge] = min_gen_meas;
    gain[edge] = (max_gen_meas - min_gen_meas) / (max_gen_delay - min_gen_delay);
    complement_gen_out(V.mod_id);
    setup_cntr_trigger_slopes(V.mod_id, POS, NEG);
  }  /* Measure the offset and gain for pos and neg generator output */

  no_complement_gen_out(V.mod_id);
  setup_gen_levels(V.mod_id, GEN_LOW_50, GEN_HIGH_50);

  connect_gen_trg_clkrb(V.mod_id);
  connect_gen_out_af2(V.mod_id);

  vth_mclose_rly(V.mod_id, MK2902);   /* clkrb path relay */
  vth_mclose_rly(V.mod_id, MK2903);   /* clkrsg path relay */
  vth_aclose_rly(V.mod_id, K753);   /* XL to AF2 path relay */
  vth_aclose_rly(V.mod_id, K764);   /* XL to AF2 path relay */

  // Initialize card to compatibility mode
  // Which of these does what we really want??
  vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x0);
  vth_pdis_32chan(V.mod_id, V.slot);

/* THIS IS where compatibility loop will need to start */

//  for (receiver = 'A'; receiver <= 'H'; receiver++)
  for (receiver = 'A'; receiver <= 'B'; receiver++)
  {  /* for */
//    vth_pclose_rly(V.mod_id, V.slot, PIN_KR_RLYS[receiver - 'A']);
    vth_pen_32chan(V.mod_id, V.slot);
    vth_pmap_rly(V.mod_id, V.slot, 0);
    if (receiver == 'A')
      vth_pclose_rly(V.mod_id, V.slot, PIN_KR_RLYS[receiver - 'A']);
    vth_pdis_32chan(V.mod_id, V.slot);
    vth_pclose_rly(V.mod_id, V.slot, PIN_KX_RLYS[receiver - 'A']);
    switch (receiver)
    {   /*Use mux_node_pins closest to the grounds*/

    case 'A':
    case 'B':
    case 'E':
    case 'F':
      mux_node_pin = 7;
      break;

    case 'C':
    case 'D':
    case 'G':
    case 'H':
      mux_node_pin = 0;
      break;
    default:
      assert(false);
      break;

    }/* case */
    vth_pclose_rly(V.mod_id, V.slot, PIN_DRVR_RLYS[mux_node_pin][receiver - 'A']);
    vth_pclose_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[mux_node_pin + 1][receiver - 'A']);
  }  /* for */

  close_pin_gnd_relays(V.mod_id, V.slot);
      /*ground all termination resistors */
  vth_pclose_rly(V.mod_id, V.slot, KDCL);

  /* Set up the TCLK generator to sync to the external CLKRB (gen trig) signal */
  vth_mtclk_clkrb_mux(V.mod_id);
  vth_mtclk_sel_ris_edge(V.mod_id);
  vth_mtclk_set_freq(V.mod_id, TCLK_FREQ, TCLK_FREQ);
  vth_mtclk_set_adv_time(V.mod_id, TCLK_ADV_TIME);

  /* Enable the ARCV lines from pin to mother card and from mother card to mac */
  vth_pen_arcv0(V.mod_id, V.slot);
  vth_pen_arcv1(V.mod_id, V.slot);
  vth_mmbd_en_arcv(V.mod_id);

#include "t10871_vml.cmd"

  for (edge = RISING; edge <= FALLING; ++edge)
  {  /* for edge */
    if (edge == RISING)
    {  /* then */
      no_complement_gen_out(V.mod_id);   /* Use at the rising edge */
      expected_response = true;
    }  /* then */
    else
    {  /* else */
      complement_gen_out(V.mod_id);   /* Use at the falling edge */
      expected_response = false;
    }  /* else */

    /* Recalculate delta_delay for the particular edge that we are testing */
    delta_delay = af2_system_delay[V.mod_id] + slot_delay[mapped_slot - 1]
                  [edge] + rx_path_offset - af2_clkrb_delay[V.mod_id];

//    for (receiver = 'A'; receiver <= 'H'; receiver++)
    for (receiver = 'A'; receiver <= 'A'; receiver++)
    {  /* for receiver */
      V.receiver_no = receiver - 'A';

//      /* Two kgl relays are closed for each receiver.  This effectively puts */
//      /* two 100 ohm load resistors in parallel to simulate a 50 ohm load. */
//     vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);
     vth_pdis_32chan(V.mod_id, V.slot);
     vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);
      switch (receiver)
      {   /*Use mux_node_pins closest to the grounds*/

      case 'A':
      case 'C':
      case 'E':
      case 'G':
        vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' + 1]);
        break;

      case 'B':
      case 'D':
      case 'F':
      case 'H':
        vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' - 1]);
        break;
      default:
        assert(false);
        break;

      }/* case */

      delay_inc = STARTING_INCREMENT;
      direction = 0;
      hit = false;
      found = false;
      V.gen_delay = nanotrunc(expected_receive - delta_delay + 1e-09);

      vth_pen_32chan(V.mod_id, V.slot);

      do
      {

        sample(&V);   /* Sample sets the value of WERR and RRESP */

        /* If a WERR occurs, the current gen delay is the closest tested */
        /* value of gen delay to rx strobe */
        if (V.werr)
          found = true;
        else
        {  /* Do the non-window error procedure */
          /* Determine if 'hit' and set the direction of delay based on rresp */
          if (V.rresp == expected_response)
          {  /* increase the delay */
            if (direction < 0)   /* can never hit on first point */
              hit = true;
            direction = 1;   /* set direction to increase delay */
          }  /* increase the delay */
          else
          {  /* decrease the delay */
            if (direction > 0)   /* can never hit on first point */
              hit = true;
            direction = -1;   /* set direction to decrease delay */
          }  /* decrease the delay */

          /* Each time a 'hit' occurs, cut the delay increment in half. 'found' */
          /* will only be set true when a 'hit' occurs at the 'res'(resolution) */
          /* value of generator delay. */
          if (hit)
          {
            if (delay_inc <= RES)
              found = true;
            delay_inc /= 2;
          }

          /* On the final 'hit' value of gen delay, the gen delay value will */
          /* be adjusted by 1/2 the 'res' value in the proper direction to */
          /* effectively average the RRESP=0 and RRESP=1 values.  The rx strobe */
          /* occurred somewhere between the RRESP=0 and RRESP=1. */
          V.gen_delay += delay_inc * direction;

        }  /* Do the non-window error procedure */

      } while (!(found || V.gen_delay < min_gen_delay ||
                 V.gen_delay > max_gen_delay));


      disable_gen_out(V.mod_id);

      vth_pdis_32chan(V.mod_id, V.slot);
      vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);

      switch (receiver)
      {   /*Use mux_node_pins closest to the grounds*/

      case 'A':
      case 'C':
      case 'E':
      case 'G':
        vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' + 1]);
        break;

      case 'B':
      case 'D':
      case 'F':
      case 'H':
        vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A' - 1]);
        break;
      default:
        assert(false);
        break;

      }/* case */

      /* calculate the actual receive, correct for gen error and channel delay */
      actual_receive = gain[edge] * (V.gen_delay - min_gen_delay) +
          offset[edge] + channel_delay[receiver - 'A'] + delta_delay;


      ch_result[receiver - 'A'][edge] = actual_receive - expected_receive;
      V.result.Measurement_class = CLASS_REAL;


      if (!found)
      {   /* it never received */
        display_prompt("**RECEIVE DOES NOT OCCUR WITHIN TEST RANGE**", true);
        ch_result[receiver - 'A'][edge] = INT_MAX;
      }


      //added the correction factor to test to correct for module configuration
      //see calculations above at top of test
      //mjs - 6/97
      V.result.UU.Real_result = ch_result[receiver - 'A'][edge] + correction;
      log_result(V.mod_id, V.result);

/*      if ((V.result.UU.Real_result - correction) <= -10e-09) //helped these subtests
      fprintf(diagfile, "+  before correction: %1.12f  after correction: %1.12f\n", (V.result.UU.Real_result - correction), V.result.UU.Real_result);
      if ((V.result.UU.Real_result) >= 10e-09) //hurt these subtests
      fprintf(diagfile, "-  before correction: %1.12f  after correction: %1.12f\n", (V.result.UU.Real_result - correction), V.result.UU.Real_result);
*/

    }  /* for receiver */

  }  /* for edge */



  /* Now log the rising to falling skew for each of the receivers */


//  for (receiver = 'A'; receiver <= 'H'; receiver++)
  for (receiver = 'A'; receiver <= 'A'; receiver++)

  {  /* for */
    skew = ch_result[receiver - 'A'][RISING] - ch_result[receiver - 'A'][FALLING];

/*    if ((skew <= -5e-09) || (skew >= 5e-09))  // +/- 5ns are the limits for the skew subtests
    {//had a failure so lets see if any of the results are out of line
      fprintf(diagfile, "=== Skew failure on chan %d =  %-+12.12e\n", receiver, skew);
      fprintf(diagfile, "    ch_result[receiver - 'A'][RISING] = %-+12.12e\n", ch_result[receiver - 'A'][RISING]);
      fprintf(diagfile, "    ch_result[receiver - 'A'][FALLING] = %-+12.12e\n", ch_result[receiver - 'A'][FALLING]);
      }*/

    V.result.UU.Real_result = skew;
    log_result(V.mod_id, V.result);
  }  /* for */

//  fclose(diagfile);
  close_syn(V.mod_id);
  close_gen(V.mod_id);
  close_mux(V.mod_id);
  close_cntr(V.mod_id);

}  /* main t10873 */
#undef TCLK_FREQ
#undef GEN_LOW_50
#undef GEN_HIGH_50
#undef RX_SPEC
#undef TIMEOUT
#undef STOP_WAIT
#undef PROG_DELAY_
#undef TCLK_ADV_TIME
#undef RES
#undef STARTING_INCREMENT

// ---  TEST 10882 - CARIBOU RECEIVER POSITIVE OVERVOLTAGE  ---
// ---  COMPLETED!!
/* #define MUX_NODE_PIN    2 */
#define LOW_BOUNDS      13.00 // this value should NEVER trip overvoltage
#define HIGH_BOUNDS     17.60 // this value should trip overvoltage
#define MIN_TEST_VOLTAGE 10.0  //intermediate value

/* static variables for t10882: */
struct LOC_t10882
{
  int mod_id;
  int slot;
  int receiver;
};

static void close_the_relays__(struct LOC_t10882 *LINK)
{
  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KDC_RLYS[LINK->receiver - 'A']);
      /* Xn connect relay */
  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KX_RLYS[LINK->receiver - 'A']);
      /* Xn to receiver relay */

  /* vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_RCVR_RLYS[MUX_NODE_PIN]
     [LINK->receiver - 'A']);
   commented out as this is residual from 9182 - gpm 1/11/2000 */

  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KR_RLYS[LINK->receiver - 'A']);
      /* receiver connect relay */
  vth_mtmr_wait(LINK->mod_id, 0.001); // mjs - added 1ms for relay close time
}

void t10882(int mod_id_, int slot_)
{
  /* This is a window version of the overvoltage test. The specs for the Edge
     driver say it should trip overvoltage @  +/-14.5v w/ +/-.5v accuracy.
     This test does not find the exact value, only verifies that it will trip
     and clear.  The customer receiver spec is +/-12.0v.
  */

  /* Caribou version of 9882.  Required to isolate receiver under test and
     test additional Receivers.  GregM 1/6/2000. */

  struct LOC_t10882 V;

  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id_;
  V.slot = slot_;

  int page;
  int high_trip;
  int low_trip;

  vth_mod_init(V.mod_id);
  vth_ainit(V.mod_id);

  open_mux(V.mod_id);
  open_syn(V.mod_id);

  setup_syn_hv(V.mod_id, true);         // need high voltage capabilities
  setup_syn(V.mod_id, DC, 0, 0.0, 0.0);
  setup_syn_off(V.mod_id, 0.0);

  connect_syn_out_af2(V.mod_id);
  vth_aclose_rly(V.mod_id, K764);   // connects AF2 to A_XL
  vth_aclose_rly(V.mod_id, K751);   // connects A_XL to S

  //  Initialize card to compatibility mode
  // (known Caribou if this test is running)
  vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x0);

  for (V.receiver = 'A'; V.receiver <= 'H'; V.receiver++)  //S bus to Xn busses
    vth_aclose_rly(V.mod_id, AIM_X_S_RLYS[V.receiver - 'A']);

 for (page = 0 ; page <= 1 ; page++)
 {

  for (V.receiver = 'A'; V.receiver <= 'H'; V.receiver++)
  {  //for receiver

    setup_syn_off(V.mod_id, MIN_TEST_VOLTAGE);
    // close_the_relays__(&V);

   if (page == 0)
   { // close only page A or C compatibilty relays
     // compatibility mode off
//     vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x1);
//     vth_pwr_cell(V.mod_id, V.slot, PLOGICAL_PG_SEL, 0);
     vth_pen_32chan(V.mod_id, V.slot);
     vth_pmap_rly(V.mod_id, V.slot, 0);
     close_the_relays__(&V);
//     vth_pclose_rly(V.mod_id, V.slot, PIN_KDC_RLYS[V.receiver - 'A']);
//     vth_pclose_rly(V.mod_id, V.slot, PIN_KX_RLYS[V.receiver - 'A']);
//     vth_pclose_rly(V.mod_id, V.slot, PIN_KR_RLYS[V.receiver - 'A']);
     // compatibility mode on
//     vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x0);
   }

   if (page == 1)
   { // close A B C and D relays but open page A or B compatibilty relays
     // compatibilty mode on
//     vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x0);
     vth_pdis_32chan(V.mod_id, V.slot);
     close_the_relays__(&V);
     // compatibility mode off
//     vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x1);
//     vth_pwr_cell(V.mod_id, V.slot, PLOGICAL_PG_SEL, 0);
     vth_pen_32chan(V.mod_id, V.slot);
     vth_pmap_rly(V.mod_id, V.slot, 0);
     vth_popen_rly(V.mod_id, V.slot, PIN_KX_RLYS[V.receiver - 'A']);
     vth_popen_rly(V.mod_id, V.slot, PIN_KR_RLYS[V.receiver - 'A']);
     // compatibility mode on
//     vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x0);
   }

    vth_mtmr_wait(V.mod_id, 0.100);  //increased time to 100ms to give syn extra time
    high_trip = 0;
    low_trip = 0;
    //-----------------------------------------------------------------------
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
    //-----------------------------------------------------------------------
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
    //----------------------------------------------------------------------
    result.Measurement_class = CLASS_REAL;
    if ( (high_trip == 1) && (low_trip == 0) )
      result.UU.Real_result = 14.5;  //arbitrary pass value
    else
      result.UU.Real_result = 0;     //arbitrary fail value
    log_result(V.mod_id, result);
    vth_pinit(V.mod_id, V.slot);

  }  //for receiver
 }  // for page

  vth_ainit(V.mod_id);
  close_mux(V.mod_id);
  close_syn(V.mod_id);

}  /*t10882*/
/* #undef MUX_NODE_PIN */
#undef LOW_BOUNDS
#undef HIGH_BOUNDS
#undef MIN_TEST_VOLTAGE

// ---  TEST 10883 - CARIBOU RECEIVER NEGATIVE OVERVOLTAGE  ---
// ---  COMPLETED!!
/* #define MUX_NODE_PIN    2 */
#define LOW_BOUNDS      (-17.60) // this value should easily trip ov
#define HIGH_BOUNDS     (-13.00) // this value should NEVER trip ov
#define MIN_TEST_VOLTAGE (-10.0)  //intermediate value

/* static variables for t10883: */
struct LOC_t10883 {
  int mod_id;
  int slot;
  int receiver;
};

static void close_the_relays___(struct LOC_t10883 *LINK)
{
  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KDC_RLYS[LINK->receiver - 'A']);
      /* Xn connect relay */
  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KX_RLYS[LINK->receiver - 'A']);
      /* Xn to receiver relay */
//  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_RCVR_RLYS[MUX_NODE_PIN]
//             [LINK->receiver - 'A']);
  vth_pclose_rly(LINK->mod_id, LINK->slot, PIN_KR_RLYS[LINK->receiver - 'A']);
      /* receiver connect relay */
  vth_mtmr_wait(LINK->mod_id, 0.001); // mjs - added 1ms for relay close time
}

void t10883(int mod_id_, int slot_)
{
  /* This is a window version of the overvoltage test. The specs for the Edge
     driver say it should trip overvoltage @  +/-14.5v w/ +/-.5v accuracy.
     This test does not find the exact value, only verifies that it will trip
     and clear.  The customer receiver spec is +/-12.0v.

     Caribou version of 9883.  GregM 1/6/2000.
  */

  struct LOC_t10883 V;

  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id_;
  V.slot = slot_;

  int page;
  int high_trip;
  int low_trip;

  vth_mod_init(V.mod_id);
  vth_ainit(V.mod_id);

  open_mux(V.mod_id);
  open_syn(V.mod_id);

  setup_syn_hv(V.mod_id, true);         // need high voltage capabilities
  setup_syn(V.mod_id, DC, 0, 0.0, 0.0);
  setup_syn_off(V.mod_id, 0.0);

  connect_syn_out_af2(V.mod_id);
  vth_aclose_rly(V.mod_id, K764);   // connects AF2 to A_XL
  vth_aclose_rly(V.mod_id, K751);   // connects A_XL to S

  //  Initialize to compatibility mode
  vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x0);

  for (V.receiver = 'A'; V.receiver <= 'H'; V.receiver++)   //S bus to Xn busses
    vth_aclose_rly(V.mod_id, AIM_X_S_RLYS[V.receiver - 'A']);

 for (page = 0; page <= 1; page ++)
 {

  for (V.receiver = 'A'; V.receiver <= 'H'; V.receiver++)
  {  //for receiver

    setup_syn_off(V.mod_id, MIN_TEST_VOLTAGE);
    // close_the_relays___(&V);

   if (page == 0)
   { // close only page A or C relays
     // compatibilty mode off
     vth_pen_32chan(V.mod_id, V.slot);
     vth_pmap_rly(V.mod_id, V.slot, 0);
     close_the_relays___(&V);
   }

   if (page == 1)
   { // close A B C and D relays then open page A or B compatibilty relays
     // compatibility mode on
     vth_pdis_32chan(V.mod_id, V.slot);
     close_the_relays___(&V);
     // compatibility mode off
     vth_pen_32chan(V.mod_id, V.slot);
     vth_pmap_rly(V.mod_id, V.slot, 0);
     vth_popen_rly(V.mod_id, V.slot, PIN_KX_RLYS[V.receiver - 'A']);
     vth_popen_rly(V.mod_id, V.slot, PIN_KR_RLYS[V.receiver - 'A']);
   }

    vth_mtmr_wait(V.mod_id, 0.100);  //increased time to 100ms to give syn extra time
    high_trip = 0;
    low_trip = 0;
    //----------------------------------------------------------------------
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
    //----------------------------------------------------------------------
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
    //----------------------------------------------------------------------
    result.Measurement_class = CLASS_REAL;
    if ( (high_trip == 0) && (low_trip == 1) )
      result.UU.Real_result = -14.5;  //arbitrary pass value
    else
      result.UU.Real_result = 0;     //arbitrary fail value
    log_result(V.mod_id, result);
    vth_pinit(V.mod_id, V.slot);

  }  //for receiver
 }  // for page

  vth_ainit(V.mod_id);
  close_mux(V.mod_id);
  close_syn(V.mod_id);

}  /*t10883*/
/* #undef MUX_NODE_PIN */
#undef LOW_BOUNDS
#undef HIGH_BOUNDS
#undef MIN_TEST_VOLTAGE

#define RANGE           3.0
#define WORST_SLOT      11   /* Highest Xn Bus Resistance */
#define RESOLUTION      1.0e-03
#define DGN_INFINITY    1e+31
#define OFFSET_OHMS_PER_SLOT  39e-03   /* Amount MB R per slot */
#define DMM_FUNCTION    FOUR_WIRE_OHMS
//--------------------------------------------------------------------------
void t10900(int mod_id, int slot)
{
  /* Series Xn Bus Resistance */
  double slot_correction;   /* Added to make worst case */
  int mux_node_pin;

  int channel;
  test_result_type result;


  open_mux(mod_id);
  open_dmm(mod_id);

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);
  setup_dmm_ocomp_on(mod_id);

  connect_dmm(mod_id);

  //gpm -- need to make sure that the fixture is locked for this test
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_lock();
  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*for*/
    if ((channel & 1) == 1)
    {
//      virt_card_select(mod_id, slot, 2);
      mux_node_pin = 7;   /* Channels A,C,E, and G */
    }
    else
    {
//      virt_card_select(mod_id, slot, 0);
      mux_node_pin = 0;   /* Channels B,D,F, and H */
    }
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mux_node_pin][channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mux_node_pin + 1][channel - 'A']);
    vth_aclose_rly(mod_id, AIM_X_S_RLYS[channel - 'A']);
    vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[channel - 'A']);
  }  /*for*/

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*for*/
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);
    connect_minus_pin(mod_id, slot, channel, true);
    connect_4wire_plus_xn(mod_id, channel, true);

    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
    {  /*if*/
      if (fabs(result.UU.Real_result) > DGN_INFINITY)
        result.UU.Real_result = DGN_INFINITY - 1;
      /* Make all slots log vaules as if they were in slot 11 (Worst case) */
      slot_correction = (WORST_SLOT - slot) * OFFSET_OHMS_PER_SLOT;
      result.UU.Real_result += slot_correction;
      log_result(mod_id, result);
    }  /*if*/
    else
      display_prompt("No result in result log (Expected dmm R)", true);

    vth_popen_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);

    connect_minus_pin(mod_id, slot, channel, false);

    connect_4wire_plus_xn(mod_id, channel, false);
  }  /*for*/

  vth_ainit(mod_id);
  vth_pinit(mod_id, slot);

  close_mux(mod_id);
  close_dmm(mod_id);

}  /*t10900*/
#undef RANGE
#undef WORST_SLOT
#undef RESOLUTION
#undef DGN_INFINITY
#undef OFFSET_OHMS_PER_SLOT
#undef DMM_FUNCTION

#define CHANNEL         'A'
#define MUX_NODE_PIN    7
#define RANGE           3.0
#define RESOLUTION      1.0e-03
#define DGN_INFINITY    1e+31
#define DMM_FUNCTION    FOUR_WIRE_OHMS
//---------------------------------------------------------------------------
void t10901(int mod_id, int slot)
{
  /* Series XG Bus Resistance */
  test_result_type result;


  open_mux(mod_id);
  open_dmm(mod_id);

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);
  setup_dmm_ocomp_on(mod_id);

  //gpm -- need to make sure that the fixture is locked for this test
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_lock();
  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  vth_aclose_rly(mod_id, K752);
  vth_aclose_rly(mod_id, K760);

  /* ------------------------------------------------------------------ */
  /* Modified 01.28.91 JRM -------------------------------------------- */
  /* Added the following line for SS compatibility.  This allows all    */
  /* pin, pin6, and cp cards to run exactly the same code.              */
  /* ------------------------------------------------------------------ */
//  virt_card_select(mod_id, slot, 2);   /*selects virtual card 2*/

  vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][CHANNEL - 'A']);
  vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[(MUX_NODE_PIN + 1) % 9]
             [CHANNEL - 'A']);
  vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[CHANNEL - 'A']);
  vth_pclose_rly(mod_id, slot, KDCG);

  connect_dmm(mod_id);
  connect_minus_pin(mod_id, slot, CHANNEL, true);
  connect_plus_gout(mod_id);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*if*/
    if (fabs(result.UU.Real_result) > DGN_INFINITY)
      result.UU.Real_result = DGN_INFINITY - 1;
    log_result(mod_id, result);
  }  /*if*/
  else
    display_prompt("No result in result log (Expected dmm R)", true);

  vth_aopen_all_rlys(mod_id);
  vth_pinit(mod_id, slot);

  close_mux(mod_id);
  close_dmm(mod_id);

}  /*t10901*/
#undef CHANNEL
#undef MUX_NODE_PIN
#undef RANGE
#undef RESOLUTION
#undef DGN_INFINITY
#undef DMM_FUNCTION

#define SALES_LONG            1
#define AVERAGE         10
#define CPU             0
#define VEXP            10.0
#define TOUT            1.0
#define RFB_1MEG        5
#define MODE_2WIRE      0
#define VPLUS           10.0
#define VMINUS          (-10.0)
//--------------------------------------------------------------------------
void t10907(int mod_id, int slot)
{
  /* Parallel Xn Bus Resistance */
  double resistance_xbus;
  int channel;
  test_result_type result;
  double rfb;
  int mint;
  s80 message;
  int len;
  int next;

//mjs -- need to make sure that the fixture is unlocked for this test

//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_unlock();

  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*close the bus paths*/
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);
    vth_aclose_rly(mod_id, AIM_X_S_RLYS[channel - 'A']);

    for (mint = 0; mint <= 8; mint++)
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mint][channel - 'A']);

  }  /*close the bus paths*/

  /* setup the MOA for resistor measurements */
  vth_aclose_rly(mod_id, K827);   /* MOAOUT to REFR2 */
  vth_aclose_rly(mod_id, K850);   /* Rfb = 1MEG */
  vth_aclose_rly(mod_id, K811);   /* I to MOAINV */
  vth_aclose_rly(mod_id, K843);   /* MOANON to ACOM */
  vth_aopen_rly(mod_id, K2605);   /* MOA Happy Relay */
  vth_amoa_sel_wb(mod_id);
  vth_acal_rtn_fb(mod_id, RFB_1MEG, MODE_2WIRE);
  if (get_result_nolog(mod_id, &result))
    rfb = result.UU.Real_result;
  else
  {
    display_prompt("No result in result log, expected Rfb", true);
    rfb = 1e6;
  }

  /* setup the DETECTOR for DCV measurements */
  vth_dcv_setup(mod_id, SALES_LONG, AVERAGE, CPU, VEXP, TOUT);
  vth_aclose_rly(mod_id, K2303);   /* DETLO to ACOM internally */
  vth_aopen_rly(mod_id, K2305);
  vth_aopen_rly(mod_id, K820);   /* disconnect L from DETLO */
  vth_aopen_rly(mod_id, K812);   /* remove DETHI from I */
vth_aclose_rly(mod_id, K857); /* int L to ACOM */

  /* setup the SOURCE for DCV */
  vth_aclose_rly(mod_id, K806);   /* S to SRCOUT */
  vth_asrc_sel_fltr(mod_id, FILTER_128);
  vth_afc_en_src_out(mod_id);

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*measure the resistance*/
    vth_aopen_rly(mod_id, AIM_X_S_RLYS[channel - 'A']);
    vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
    vth_aclose_rly(mod_id, AIM_X_L_RLYS[channel - 'A']);   /* discharge bus */
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    vth_aopen_rly(mod_id, AIM_X_L_RLYS[channel - 'A']);
    vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);

    resistance_xbus = measure_open(mod_id, VPLUS, VMINUS, rfb);
    if (resistance_xbus < -2.0e+11)
    {   /* large negative value */
      resistance_xbus = R_MAX_PARALLEL;
      result.UU.Real_result = log10(resistance_xbus);
    }
    else
    {
      if (resistance_xbus < -1.0e+11)
      {   /* moderate negative < 200uV noise */
        resistance_xbus = R_MAX_PARALLEL;

        /* warn the operator of a potential measurement problem */
        strcpy(message,
               "WARNING: High-Z measurement problem, Absolute value required");
        display_prompt(message, true);
        strcpy(message, "Test 9207 Module ");
        len = strlen(message) + 1;
        sprintf(message + len - 1, "%d", mod_id);
        next = strlen(message) + 1;
        strcat(message, " Slot ");
        len = strlen(message) + 1;
        sprintf(message + len - 1, "%d", slot);
        next = strlen(message) + 1;
        strcat(message, " Channel ");
        len = strlen(message) + 1;
        sprintf(message + len - 1, "%c", channel);
        next = strlen(message) + 1;
        display_prompt(message, true);

        result.UU.Real_result = log10(resistance_xbus);
      }
      else
      {
        if (resistance_xbus < 0)
        {   /* small negative > 200uV noise */
          resistance_xbus = fabs(resistance_xbus);
          result.UU.Real_result = -1.0 * log10(resistance_xbus);
        }
        else
        {  /* positive value */
          if (resistance_xbus > R_MAX_PARALLEL)
            resistance_xbus = R_MAX_PARALLEL;
          result.UU.Real_result = log10(resistance_xbus);
        }
      }
    }
    log_result(mod_id, result);

    vth_aopen_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
    vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
    vth_aclose_rly(mod_id, AIM_X_S_RLYS[channel - 'A']);

  }  /*measure the resistance*/

//mjs -- relock the fixture  ,  but i commented out because of annoyance and relock the fixture in 9210
//  vth_cfxt_lock();

}  /*t10907*/
#undef SALES_LONG
#undef AVERAGE
#undef CPU
#undef VEXP
#undef TOUT
#undef RFB_1MEG
#undef MODE_2WIRE
#undef VPLUS
#undef VMINUS

#define SALES_LONG            1
#define AVERAGE         10
#define CPU             0
#define VEXP            10.0
#define TOUT            1.0
#define RFB_1MEG        5
#define MODE_2WIRE      0
#define VPLUS           10.0
#define VMINUS          (-10.0)
//-------------------------------------------------------------------------
void t10909(int mod_id, int slot)
{
  /* Parallel Xn Bus Resistance */
  double resistance_xbus;
  int channel;
  test_result_type result;
  double rfb;
  int mint;
  s80 message;
  int len;
  int next;

  //gpm -- need to make sure that the fixture is unlocked for this test
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_unlock();
  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  vth_pclose_rly(mod_id, slot, KDCG);
  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*close the bus paths*/
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[channel - 'A']);
    vth_aclose_rly(mod_id, AIM_X_S_RLYS[channel - 'A']);

    for (mint = 0; mint <= 8; mint++)
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mint][channel - 'A']);

  }  /*close the bus paths*/

  /* setup the MOA for resistor measurements */
  vth_aopen_rly(mod_id, K814);   /* disconnect G from ACOM */
  vth_aclose_rly(mod_id, K827);   /* MOAOUT to REFR2 */
  vth_aclose_rly(mod_id, K850);   /* Rfb = 1MEG */
  vth_aclose_rly(mod_id, K811);   /* I to MOAINV */
  vth_aclose_rly(mod_id, K816);   /* I to G */
  vth_aclose_rly(mod_id, K752);   /* G to XG */
  vth_aclose_rly(mod_id, K843);   /* MOANON to ACOM */
  vth_aopen_rly(mod_id, K2605);   /* MOA Happy Relay */
  vth_amoa_sel_wb(mod_id);
  vth_acal_rtn_fb(mod_id, RFB_1MEG, MODE_2WIRE);
  if (get_result_nolog(mod_id, &result))
    rfb = result.UU.Real_result;
  else
  {
    display_prompt("No result in result log, expected Rfb", true);
    rfb = 1e6;
  }

  /* setup the DETECTOR for DCV measurements */
  vth_dcv_setup(mod_id, SALES_LONG, AVERAGE, CPU, VEXP, TOUT);
  vth_aclose_rly(mod_id, K2303);   /* DETLO to ACOM internally */
  vth_aopen_rly(mod_id, K2305);
  vth_aopen_rly(mod_id, K812);   /* remove DETHI from I */

  /* setup the SOURCE for DCV */
  vth_aclose_rly(mod_id, K806);   /* S to SRCOUT */
  vth_asrc_sel_fltr(mod_id, FILTER_128);
  vth_afc_en_src_out(mod_id);

  resistance_xbus = measure_open(mod_id, VPLUS, VMINUS, rfb);
  if (resistance_xbus < -2.0e+11)
  {   /* large negative */
    resistance_xbus = R_MAX_PARALLEL;
    result.UU.Real_result = log10(resistance_xbus);
  }
  else
  {
    if (resistance_xbus < -1.0e+11)
    {   /* moderate negative < 200uV noise */
      resistance_xbus = R_MAX_PARALLEL;

      /* warn the operator of a potential measurement problem */
      strcpy(message,
             "WARNING: High-Z measurement problem, Absolute value required");
      display_prompt(message, true);
      strcpy(message, "Test 9209 Module ");
      len = strlen(message) + 1;
      sprintf(message + len - 1, "%d", mod_id);
      next = strlen(message) + 1;
      strcat(message, " Slot ");
      len = strlen(message) + 1;
      sprintf(message + len - 1, "%d", slot);
      next = strlen(message) + 1;
      strcat(message, " Channel ");
      len = strlen(message) + 1;
      sprintf(message + len - 1, "%c", channel);
      next = strlen(message) + 1;
      display_prompt(message, true);

      result.UU.Real_result = log10(resistance_xbus);
    }
    else
    {
      if (resistance_xbus < 0)
      {   /* small negative > 200uV noise       */
        resistance_xbus = fabs(resistance_xbus);
        result.UU.Real_result = -1.0 * log10(resistance_xbus);
      }
      else
      {  /* positive */
        if (resistance_xbus > R_MAX_PARALLEL)
          resistance_xbus = R_MAX_PARALLEL;
        result.UU.Real_result = log10(resistance_xbus);
      }
    }
  }
  log_result(mod_id, result);

}  /*t10909*/
#undef SALES_LONG
#undef AVERAGE
#undef CPU
#undef VEXP
#undef TOUT
#undef RFB_1MEG
#undef MODE_2WIRE
#undef VPLUS
#undef VMINUS

#define LCR_MULT        5
#define LCR_FREQ        100e+03
#define LCR_FUNCTION    INDUCTANCE
#define LCR_DISPLAY     Q
#define LCR_CKT_MODE    SERIES
//------------------------------------------------------------------------------
void t10910(int mod_id, int slot)
{
  /* Series Xn Bus Inductance */
  int mux_node_pin;
  int next;
  int start;

  double henries;

  int channel;
  test_result_type result;
  char *s_ptr;
  char s[MAX_S_LENGTH + 1];
  char *str1;



  open_mux(mod_id);
  open_lcr(mod_id);

//mjs -- need to make sure that the fixture is locked for this test
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_lock();

  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,
            LCR_MULT);

  connect_lcr(mod_id);

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*for*/
    if ((channel & 1) == 1)
    {
//      virt_card_select(mod_id, slot, 2);
      mux_node_pin = 7;   /* Channels A,C,E, and G */
    }
    else
    {
//      virt_card_select(mod_id, slot, 0);
      mux_node_pin = 0;   /* Channels B,D,F, and H */
    }
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mux_node_pin][channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mux_node_pin + 1][channel - 'A']);
    vth_aclose_rly(mod_id, AIM_X_S_RLYS[channel - 'A']);
    vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[channel - 'A']);
  }  /*for*/

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*for*/
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);

    connect_minus_pin(mod_id, slot, channel, true);
    connect_4wire_plus_xn(mod_id, channel, true);

    measure_lcr(mod_id);
    result.Measurement_class = CLASS_PSTRING_PTR;
    if (get_result_nolog(mod_id, &result))
    {  /* if */
      s_ptr = static_cast(char*, result.UU.Pstring_ptr);
      strcpy(s, s_ptr);
//  Debug
//    start = strpos2("L", s, 1) + 1;
      start = strpos2(",", s, 1) + 1;
      henries = strtod(s + start - 1, &str1);
      next = str1 - s + 1;
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = henries;
      log_result(mod_id, result);
    }  /* if */
    else
      display_prompt("No result in result log (Expected Series L)", true);

    vth_popen_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);

    connect_minus_pin(mod_id, slot, channel, false);
    connect_4wire_plus_xn(mod_id, channel, false);

  }  /*for*/

  vth_ainit(mod_id);
  vth_pinit(mod_id, slot);

  close_mux(mod_id);
  close_lcr(mod_id);

}  /*t10910*/
#undef LCR_MULT
#undef LCR_FREQ
#undef LCR_FUNCTION
#undef LCR_DISPLAY
#undef LCR_CKT_MODE

#define CHANNEL         'A'
#define MUX_NODE_PIN    7
#define LCR_MULT        5
#define LCR_FREQ        100e+03
#define LCR_FUNCTION    INDUCTANCE
#define LCR_DISPLAY     Q
#define LCR_CKT_MODE    SERIES
//------------------------------------------------------------------------------
void t10911(int mod_id, int slot)
{
  /* Series XG/XGL/DRa Bus Inductance */
  int next;
  int start;

  double henries;

  test_result_type result;
  char *s_ptr;
  char s[MAX_S_LENGTH + 1];
  char *str1;


  open_mux(mod_id);
  open_lcr(mod_id);

  //gpm -- need to make sure that the fixture is locked for this test
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_lock();
  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,
            LCR_MULT);

  vth_aclose_rly(mod_id, K752);
  vth_aclose_rly(mod_id, K760);

  /* ------------------------------------------------------------------ */
  /* Modified 01.28.91 JRM -------------------------------------------- */
  /* Added the following line for SS compatibility.  This allows all    */
  /* pin, pin6, and cp cards to run exactly the same code.              */
  /* ------------------------------------------------------------------ */
//  virt_card_select(mod_id, slot, 2);   /*selects virtual card 2*/

  vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][CHANNEL - 'A']);
  vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[(MUX_NODE_PIN + 1) % 9]
             [CHANNEL - 'A']);
  vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[CHANNEL - 'A']);
  vth_pclose_rly(mod_id, slot, KDCG);

  connect_lcr(mod_id);
  connect_minus_pin(mod_id, slot, CHANNEL, true);
  connect_plus_gout(mod_id);

  measure_lcr(mod_id);
  result.Measurement_class = CLASS_PSTRING_PTR;
  if (get_result_nolog(mod_id, &result))
  {  /* if */
    s_ptr = static_cast(char*, result.UU.Pstring_ptr);
    strcpy(s, s_ptr);
//  Debug
//  start = strpos2("L", s, 1) + 1;
    start = strpos2(",", s, 1) + 1;
    henries = strtod(s + start - 1, &str1);
    next = str1 - s + 1;
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = henries;
    log_result(mod_id, result);
  }  /* if */
  else
    display_prompt("No result in result log (Expected Series L)", true);

  vth_ainit(mod_id);
  vth_pinit(mod_id, slot);

  close_mux(mod_id);
  close_lcr(mod_id);

}  /*t10911*/
#undef CHANNEL
#undef MUX_NODE_PIN
#undef LCR_MULT
#undef LCR_FREQ
#undef LCR_FUNCTION
#undef LCR_DISPLAY
#undef LCR_CKT_MODE

#define SALES_LONG            1
#define AVERAGE         10
#define CPU             0
#define VEXP            0.039
#define TOUT            1.0
#define RFB_1MEG        5
#define MODE_2WIRE      0
#define VPLUS           5.0
#define VMINUS          (-5.0)
//---------------------------------------------------------------------------
void t10917(int mod_id, int slot)
{
  /* Parallel Xn Bus Resistance to Ground */
  double resistance_xbus;
  int channel;
  test_result_type result;
  double rfb;
  int mint;
  s80 message;
  int len;
  int next;

//mjs -- need to make sure that the fixture is unlocked for this test
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_unlock();

  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*close the bus paths*/
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[0][channel - 'A']);

    for (mint = 0; mint <= 8; mint++)
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mint][channel - 'A']);

  }  /*close the bus paths*/

  vth_pclose_rly(mod_id, slot, KDCG);

  /* setup the MOA for resistor measurements */
  vth_aclose_rly(mod_id, K827);   /* MOAOUT to REFR2 */
  vth_aclose_rly(mod_id, K850);   /* Rfb = 1MEG */
  vth_aclose_rly(mod_id, K811);   /* I to MOAINV */
  vth_aopen_rly(mod_id, K843);   /* MOANON to AUXOUT */
  vth_aclose_rly(mod_id, K842);
  vth_aopen_rly(mod_id, K2605);   /* MOA Happy Relay */
  vth_aopen_rly(mod_id, K857);   /* disconnect L from ACOM */
  vth_aclose_rly(mod_id, K819);   /* MOANON to L */
  vth_aclose_rly(mod_id, K814);   /* G to ACOM */
  vth_aclose_rly(mod_id, K752);   /* G to XG */
  vth_aclose_rly(mod_id, K828);   /* REFR2 to DETHI */
  vth_amoa_sel_wb(mod_id);
  vth_acal_rtn_fb(mod_id, RFB_1MEG, MODE_2WIRE);
  if (get_result_nolog(mod_id, &result))
    rfb = result.UU.Real_result;
  else
  {
    display_prompt("No result in result log, expected Rfb", true);
    rfb = 1e6;
  }

  /* setup the DETECTOR for DCV measurements */
  vth_dcv_setup(mod_id, SALES_LONG, AVERAGE, CPU, VEXP, TOUT);
  vth_aopen_rly(mod_id, K812);   /* remove DETHI from I */

  /* setup the AUX */
  vth_aaux_en_30ma_ilim(mod_id);
  vth_aaux_set_ampl(mod_id, 0.0);
  vth_aaux_en(mod_id);

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*measure the resistance*/
    vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[0][channel - 'A']);
    vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

    resistance_xbus = measure_open_non(mod_id, VPLUS, VMINUS, rfb);
    if (resistance_xbus < -1.0e+11)
    {   /* large negative value */
      resistance_xbus = R_MAX_PARALLEL;
      result.UU.Real_result = log10(resistance_xbus);
    }
    else
    {
      if (resistance_xbus < -5.0e+10)
      {   /* moderate negative < 200uV noise */
        resistance_xbus = R_MAX_PARALLEL;

        /* warn the operator of a potential measurement problem */
        strcpy(message,
               "WARNING: High-Z measurement problem, Absolute value required");
        display_prompt(message, true);
        strcpy(message, "Test 9217 Module ");
        len = strlen(message) + 1;
        sprintf(message + len - 1, "%d", mod_id);
        next = strlen(message) + 1;
        strcat(message, " Slot ");
        len = strlen(message) + 1;
        sprintf(message + len - 1, "%d", slot);
        next = strlen(message) + 1;
        strcat(message, " Channel ");
        len = strlen(message) + 1;
        sprintf(message + len - 1, "%c", channel);
        next = strlen(message) + 1;
        display_prompt(message, true);

        result.UU.Real_result = log10(resistance_xbus);
      }
      else
      {
        if (resistance_xbus < 0)
        {   /* small negative > 200uV noise */
          resistance_xbus = fabs(resistance_xbus);
          result.UU.Real_result = -1.0 * log10(resistance_xbus);
        }
        else
        {  /* positive value */
          if (resistance_xbus > R_MAX_PARALLEL)
            resistance_xbus = R_MAX_PARALLEL;
          result.UU.Real_result = log10(resistance_xbus);
        }
      }
    }
    log_result(mod_id, result);

    vth_aopen_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
    vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[0][channel - 'A']);

  }  /*measure the resistance*/

//mjs -- relock the fixture , but put the lock at the start of 9920
//  vth_cfxt_lock();

}  /*t10917*/
#undef SALES_LONG
#undef AVERAGE
#undef CPU
#undef VEXP
#undef TOUT
#undef RFB_1MEG
#undef MODE_2WIRE
#undef VPLUS
#undef VMINUS

//-----------
//----------- NEW 10920 to change correction factors and paths
//-----------
#define LCR_MULT        5
#define LCR_FREQ        100e+03
#define WAIT_TIME       500e-03
#define LCR_FUNCTION    CAPACITANCE
#define LCR_DISPLAY     Q
#define LCR_CKT_MODE    PARALLEL
//-----------------------------------------------------------------------
//  This test has been greatly modified from the original version
//  test 10920.  We now measure Xn to Xm bus capacitance for all
//  possible combinations of busses X1 through X8.  The previous
//  test measured the capacitance of Xn to Xall_others_in_parallel.
//  There is a limitation due to the sales rack wiring that does
//  not allow us to wire up the measurement with the negative LCR
//  lead to a bus that is 2 more or 6 less than the bus connected
//  to the positive lead.  This results in shorting the LCR leads
//  together, thereby generating false data.  These cases are
//  therefore measured only once (when the leads are the other
//  way), whereas all other combinations are measured twice.
//  Additionally, the previous version called test 9010 to calculate
//  the capacitance in the rack for each pair of busses Xn to Xn+1.
//  This new version does not call 9010, but instead does the rack
//  capacitance for the same combinations that we intend to test
//  within this test itself.  This functionality may later be
//  made into a seperate routine.    - Sept 29, 2000 Greg Milner
//-----------------------------------------------------------------------
void t10920(int mod_id, int slot)
{
  /* [PINCARD] Parallel Xn Bus Capacitance */

  /* For path and test descriptions, see the documentation for test: 9220 */

  int next;
  int start;

  double farads;
  double rack_compensation[8][8];

  int ch1;
  int ch2;
  int channel;
  test_result_type result;
  char *s_ptr;
  char s[MAX_S_LENGTH + 1];
  char *str1;

  // FILE *diagfile;

  open_mux(mod_id);
  open_lcr(mod_id);

  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

// CHANGE OF PLANS... WE WANT TO MEASURE THE COMPATIBILTY STUB 10-4-2000
//  // ADDING THE PUT CARD IN 32 CHAN SO COMPATIBILITY RELAY DOES NOT CLOSE
//  vth_pen_32chan(mod_id, slot);


  double correction;   //correction factor because results of this test are dependent
                       // on the number and type of cards in module

  correction = calc_mod_correction(mod_id, 0.0e-12);

  //mjs -- make sure the fixture is locked for this test
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_lock();

  setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,
            LCR_MULT);

  vth_aopen_all_rlys(mod_id);

  connect_dg(mod_id, true);
  connect_lcr(mod_id);

// ******************************************************************
// Adding Functionality of Test 9010 modified for Xn to Xm all combos
// ******************************************************************

  for (ch1 = 'A'; ch1 <= 'H'; ch1++)
  {
    for (ch2 = 'A'; ch2 <= 'H'; ch2++)
    {
      if ((ch1 != ch2) && (ch1 + 2 != ch2) && (ch1 - 6 != ch2))
      {
        connect_plus_xn(mod_id, ch1, true);
        if (ch2 != 'A')
          connect_minus_xn_plus_1(mod_id, ch2 - 1, true);   /* 'H'  Xn + 1 = 'A' */
        else
          connect_minus_xn_plus_1(mod_id, 'H', true);   /* 'H'  Xn + 1 = 'A' */

        vth_mtmr_wait(mod_id, WAIT_TIME);
        measure_lcr(mod_id);
        result.Measurement_class = CLASS_PSTRING_PTR;
        if (get_result_nolog(mod_id, &result))
        {  /*if*/
          s_ptr = static_cast(char*,result.UU.Pstring_ptr);
          strcpy(s, s_ptr);
          start = strpos2(",", s, 1) + 1;
          farads = strtod(s + start - 1, &str1);
          next = str1 - s + 1;
          rack_compensation[ch1 - 'A'][ch2 - 'A'] = farads;
//          result.Measurement_class = CLASS_REAL;
//          result.UU.Real_result = farads;
//          log_result(mod_id, result);
        } /*if*/
        else
          rack_compensation[ch1 - 'A'][ch2 - 'A'] = 0.0;

        connect_plus_xn(mod_id, ch1, false);
        if (ch2 != 'A')
          connect_minus_xn_plus_1(mod_id, ch2 - 1, false);   /* 'H'  Xn + 1 = 'A' */
        else
          connect_minus_xn_plus_1(mod_id, 'H', false);   /* 'H'  Xn + 1 = 'A' */
      }
    }
  }

// ******************************************************************
// End of Functionality of Test 9010 modified for Xn to Xm all combos
// ******************************************************************


  /* For Channel card compatibility */
  vth_popen_all_rlys(mod_id, slot);

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*for*/
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);
//    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
  }  /*for*/

  for (ch1 = 'A'; ch1 <= 'H'; ch1++)
  {
    for (ch2 = 'A'; ch2 <= 'H'; ch2++)
    {  /*for*/
      if ((ch1 != ch2) && (ch1 + 2 != ch2) && (ch1 - 6 != ch2))
//      if (ch1 != ch2)
      {
        vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[ch1 - 'A']);
        vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[ch2 - 'A']);
//    vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[channel - 'A']);
//    vth_aclose_rly(mod_id, AIM_X_PLUS_1_MINT_RLYS[channel - 'A']);
        for (channel = 'A'; channel <= 'H'; channel++)
          vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
        for (channel = 'A'; channel <= 'H'; channel++)
          vth_aopen_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);

        connect_plus_xn(mod_id, ch1, true);
        if (ch2 != 'A')
          connect_minus_xn_plus_1(mod_id, ch2 - 1, true);   /* 'H'  Xn + 1 = 'A' */
        else
          connect_minus_xn_plus_1(mod_id, 'H', true);   /* 'H'  Xn + 1 = 'A' */
//    connect_plus_xn(mod_id, channel, true);
//    connect_minus_xn_plus_1(mod_id, channel, true);   /* 'H'  Xn + 1 = 'A' */

        vth_mtmr_wait(mod_id, WAIT_TIME);
        measure_lcr(mod_id);
        result.Measurement_class = CLASS_PSTRING_PTR;
        if (get_result_nolog(mod_id, &result))
        {  /*if*/
          s_ptr = static_cast(char*, result.UU.Pstring_ptr);
          strcpy(s, s_ptr);
          start = strpos2(",", s, 1) + 1;
          farads = strtod(s + start - 1, &str1);
          next = str1 - s + 1;

          farads -= rack_compensation[ch1 - 'A'][ch2 - 'A'];
//          if (mod_id == 0 || mod_id == 3)
//            farads -= sales_rack_offset_cap[0][channel - 'A'];
//          else
//            farads -= sales_rack_offset_cap[1][channel - 'A'];

          result.Measurement_class = CLASS_REAL;
          result.UU.Real_result = farads - correction;
          log_result(mod_id, result);
        }  /*if*/
        else
          display_prompt("No result in result log (Expected Xn to XG C)", true);

        connect_plus_xn(mod_id, ch1, false);
        if (ch2 != 'A')
          connect_minus_xn_plus_1(mod_id, ch2 - 1, false);   /* 'H'  Xn + 1 = 'A' */
        else
          connect_minus_xn_plus_1(mod_id, 'H', false);   /* 'H'  Xn + 1 = 'A' */
//    connect_plus_xn(mod_id, channel, false);
//    connect_minus_xn_plus_1(mod_id, channel, false);   /* 'H'  Xn + 1 = 'A' */
        vth_aopen_rly(mod_id, AIM_X_MINT_RLYS[ch1 - 'A']);
        vth_aopen_rly(mod_id, AIM_X_MINT_RLYS[ch2 - 'A']);
//    vth_aopen_rly(mod_id, AIM_X_MINT_RLYS[channel - 'A']);
//    vth_aopen_rly(mod_id, AIM_X_PLUS_1_MINT_RLYS[channel - 'A']);
//    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);

      } /*if*/
    }  /*for*/
  } /*for*/

  vth_ainit(mod_id);
  vth_pinit(mod_id, slot);
//  fclose(diagfile);
  close_mux(mod_id);
  close_lcr(mod_id);

}  /*t10920*/
#undef LCR_MULT
#undef LCR_FREQ
#undef WAIT_TIME
#undef LCR_FUNCTION
#undef LCR_DISPLAY
#undef LCR_CKT_MODE

//--------------------------------------------------------------------------
void t10919(int mod_id, int slot)
{
  /* [PINCARD] Parallel Xn Bus Capacitance */
  /* For path and test descriptions, see the documentation for test: 9220 */

  test_result_type result;

  int card_type;

  int bus1, bus2;


//  Add code to close ALL PIN_RCVR_RLYS before excuting loops
//  to complete path out to the mint pins
//  Add code to make sure fixture is release during this test

//  for (bus1 = 0; bus1 <= 8; bus1++)
    for (bus2 = 0; bus2 <= 7; bus2++)
//      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[bus1][bus2]);
      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[5][bus2]);

//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_unlock();

  for (bus1 = 1;  bus1 <= 7; bus1++)
  {  // mjs bus1 loop
    for (bus2 = bus1+1;  bus2 <= 8; bus2++)
    {  // mjs bus2 loop

      card_type = get_testhead_actual_card(mod_id, slot);
      /* determine if card is caribou and put in compatibility mode */
      /* if not Caribou then select all Channel Virtual Cards */
      if ((card_type == CDDP) || (card_type == CDD20) ||
          (card_type == CDD6) || (card_type == CDDJB) ||
          (card_type == CDDU))
        vth_pdis_32chan(mod_id, slot);

      /* ALWAYS CONNECT THE S BUS MEASUREMENT PATH */
      vth_aclose_rly(mod_id, AIM_X_S_RLYS[bus1-1]);
      vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[bus1-1]);
      vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[bus1-1]);

      /* ALWAYS CONNECT THE I BUS MEASUREMENT PATH */
      vth_aclose_rly(mod_id, AIM_X_I_RLYS[bus2-1]);
      vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[bus2-1]);
      vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[bus2-1]);


//  Setup the AIM MOA for 2-wire low value capacitor measurement
//  Values stolen from std.capacitor file

//      vth_rlc_setup(mod_id, src_freq[reading], src_amp[reading], src_off[reading],
//              src_wait[reading], src_comp[reading], en[reading],
//              ed[reading], rf[reading], sa[reading], sb[reading],
//              sl[reading], wb[reading], hfs_cap[reading], diff_det[reading],
//              moa_out[reading], det_wait[reading]);

      vth_rlc_setup(mod_id, 1024, 0.1, 0, 0.000, 1, 0, 1, 6, 0, 0, 0, 1, 0, 0, 0.01, 0);

//      vth_capacitor_meas(mod_id, ser_mode[reading], dc_rej[reading]);
      vth_capacitor_meas(mod_id, 0, 1);

      vth_get_capacitor_result(mod_id);

      if (get_result_nolog(mod_id, &result))
        log_result(mod_id, result);
      else
        display_prompt("No result in result log (Expected component value)",
                       true);

      /* ALWAYS DISCONNECT THE S BUS MEASUREMENT PATH */
      vth_aopen_rly(mod_id, AIM_X_S_RLYS[bus1-1]);
      vth_popen_rly(mod_id, slot, PIN_KDC_RLYS[bus1-1]);
      vth_popen_rly(mod_id, slot, PIN_KX_RLYS[bus1-1]);

      /* ALWAYS DISCONNECT THE I BUS MEASUREMENT PATH */
      vth_aopen_rly(mod_id, AIM_X_I_RLYS[bus2-1]);
      vth_popen_rly(mod_id, slot, PIN_KDC_RLYS[bus2-1]);
      vth_popen_rly(mod_id, slot, PIN_KX_RLYS[bus2-1]);

    } // bus2 loop

  } // bus1 loop

//  Add code to open ALL PIN_RCVR_RLYS after excuting loops

//  for (bus1 = 0; bus1 <= 8; bus1++)
    for (bus2 = 0; bus2 <= 7; bus2++)
//      vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[bus1][bus2]);
      vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[5][bus2]);

}

#define LCR_MULT        5
#define LCR_FREQ        100e+03
#define WAIT_TIME       500e-03
#define LCR_FUNCTION    CAPACITANCE
#define LCR_DISPLAY     Q
#define LCR_CKT_MODE    PARALLEL
//-------------------------------------------------------------------------
//  MOCK of new 10920 to change to Xn Page A to Xm Page B measurements
//-------------------------------------------------------------------------
void t10918(int mod_id, int slot)
{
  /* [PINCARD] Parallel Xn Bus Capacitance */

  /* For path and test descriptions, see the documentation for test: 9220 */

  int next;
  int start;

  double farads;
  double rack_compensation[8][8];

  int ch1;
  int ch2;
  int channel;
  test_result_type result;
  char *s_ptr;
  char s[MAX_S_LENGTH + 1];
  char *str1;

  // FILE *diagfile;

  open_mux(mod_id);
  open_lcr(mod_id);

  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

// CHANGE OF PLANS... WE WANT TO MEASURE THE COMPATIBILTY STUB 10-4-2000
//  // ADDING THE PUT CARD IN 32 CHAN SO COMPATIBILITY RELAY DOES NOT CLOSE
//  vth_pen_32chan(mod_id, slot);


  double correction;   //correction factor because results of this test are dependent
                       // on the number and type of cards in module

  correction = calc_mod_correction(mod_id, 0.0e-12);

  //mjs -- make sure the fixture is locked for this test
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_lock();

  setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,
            LCR_MULT);

  vth_aopen_all_rlys(mod_id);

  connect_dg(mod_id, true);
  connect_lcr(mod_id);

// ******************************************************************
// Adding Functionality of Test 9010 modified for Xn to Xm all combos
// ******************************************************************

  for (ch1 = 'A'; ch1 <= 'H'; ch1++)
  {
    for (ch2 = 'A'; ch2 <= 'H'; ch2++)
    {
      if ((ch1 != ch2) && (ch1 + 2 != ch2) && (ch1 - 6 != ch2))
      {
        connect_plus_xn(mod_id, ch1, true);
        if (ch2 != 'A')
          connect_minus_xn_plus_1(mod_id, ch2 - 1, true);   /* 'H'  Xn + 1 = 'A' */
        else
          connect_minus_xn_plus_1(mod_id, 'H', true);   /* 'H'  Xn + 1 = 'A' */

        vth_mtmr_wait(mod_id, WAIT_TIME);
        measure_lcr(mod_id);
        result.Measurement_class = CLASS_PSTRING_PTR;
        if (get_result_nolog(mod_id, &result))
        {  /*if*/
          s_ptr = static_cast(char*,result.UU.Pstring_ptr);
          strcpy(s, s_ptr);
          start = strpos2(",", s, 1) + 1;
          farads = strtod(s + start - 1, &str1);
          next = str1 - s + 1;
          rack_compensation[ch1 - 'A'][ch2 - 'A'] = farads;
//          result.Measurement_class = CLASS_REAL;
//          result.UU.Real_result = farads;
//          log_result(mod_id, result);
        } /*if*/
        else
          rack_compensation[ch1 - 'A'][ch2 - 'A'] = 0.0;

        connect_plus_xn(mod_id, ch1, false);
        if (ch2 != 'A')
          connect_minus_xn_plus_1(mod_id, ch2 - 1, false);   /* 'H'  Xn + 1 = 'A' */
        else
          connect_minus_xn_plus_1(mod_id, 'H', false);   /* 'H'  Xn + 1 = 'A' */
      }
    }
  }

// ******************************************************************
// End of Functionality of Test 9010 modified for Xn to Xm all combos
// ******************************************************************


  /* For Channel card compatibility */
  vth_popen_all_rlys(mod_id, slot);
  if (slot < 12)
    vth_popen_all_rlys(mod_id, slot+11);
  else
    vth_popen_all_rlys(mod_id, slot-11);

//  for (channel = 'A'; channel <= 'H'; channel++)
//  {  /*for*/
//    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);
//    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);
//    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
//  }  /*for*/

  for (ch1 = 'A'; ch1 <= 'H'; ch1++)
  {
    for (ch2 = 'A'; ch2 <= 'H'; ch2++)
    {  /*for*/
      if ((ch1 != ch2) && (ch1 + 2 != ch2) && (ch1 - 6 != ch2))
//      if (ch1 != ch2)
      {
        vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[ch1 - 'A']);
        vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[ch2 - 'A']);
//    vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[channel - 'A']);
//    vth_aclose_rly(mod_id, AIM_X_PLUS_1_MINT_RLYS[channel - 'A']);


//  COMMENT OUT - For just up to disconnect relays data!

//        if (slot < 12)
//        {
//          vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[ch1 - 'A']);
//          vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[ch1 - 'A']);
//          vth_pclose_rly(mod_id, slot+11, PIN_KDC_RLYS[ch2 - 'A']);
//          vth_pclose_rly(mod_id, slot+11, PIN_KX_RLYS[ch2 - 'A']);
//        }
//        else
//        {
//          vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[ch1 - 'A']);
//          vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[ch1 - 'A']);
//          vth_pclose_rly(mod_id, slot-11, PIN_KDC_RLYS[ch2 - 'A']);
//          vth_pclose_rly(mod_id, slot-11, PIN_KX_RLYS[ch2 - 'A']);
//        }

        for (channel = 'A'; channel <= 'H'; channel++)
          vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
        for (channel = 'A'; channel <= 'H'; channel++)
          vth_aopen_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);

        connect_plus_xn(mod_id, ch1, true);
        if (ch2 != 'A')
          connect_minus_xn_plus_1(mod_id, ch2 - 1, true);   /* 'H'  Xn + 1 = 'A' */
        else
          connect_minus_xn_plus_1(mod_id, 'H', true);   /* 'H'  Xn + 1 = 'A' */
//    connect_plus_xn(mod_id, channel, true);
//    connect_minus_xn_plus_1(mod_id, channel, true);   /* 'H'  Xn + 1 = 'A' */

        vth_mtmr_wait(mod_id, WAIT_TIME);
        measure_lcr(mod_id);
        result.Measurement_class = CLASS_PSTRING_PTR;
        if (get_result_nolog(mod_id, &result))
        {  /*if*/
          s_ptr = static_cast(char*, result.UU.Pstring_ptr);
          strcpy(s, s_ptr);
          start = strpos2(",", s, 1) + 1;
          farads = strtod(s + start - 1, &str1);
          next = str1 - s + 1;

          farads -= rack_compensation[ch1 - 'A'][ch2 - 'A'];
//          if (mod_id == 0 || mod_id == 3)
//            farads -= sales_rack_offset_cap[0][channel - 'A'];
//          else
//            farads -= sales_rack_offset_cap[1][channel - 'A'];

          result.Measurement_class = CLASS_REAL;
          result.UU.Real_result = farads - correction;
          log_result(mod_id, result);
        }  /*if*/
        else
          display_prompt("No result in result log (Expected Xn to XG C)", true);

        connect_plus_xn(mod_id, ch1, false);
        if (ch2 != 'A')
          connect_minus_xn_plus_1(mod_id, ch2 - 1, false);   /* 'H'  Xn + 1 = 'A' */
        else
          connect_minus_xn_plus_1(mod_id, 'H', false);   /* 'H'  Xn + 1 = 'A' */
//    connect_plus_xn(mod_id, channel, false);
//    connect_minus_xn_plus_1(mod_id, channel, false);   /* 'H'  Xn + 1 = 'A' */


//  COMMENT OUT - For just up to disconnect relays data!

//        if (slot < 12)
//        {
//          vth_popen_rly(mod_id, slot, PIN_KDC_RLYS[ch1 - 'A']);
//          vth_popen_rly(mod_id, slot, PIN_KX_RLYS[ch1 - 'A']);
//          vth_popen_rly(mod_id, slot+11, PIN_KDC_RLYS[ch2 - 'A']);
//          vth_popen_rly(mod_id, slot+11, PIN_KX_RLYS[ch2 - 'A']);
//        }
//        else
//        {
//          vth_popen_rly(mod_id, slot, PIN_KDC_RLYS[ch1 - 'A']);
//          vth_popen_rly(mod_id, slot, PIN_KX_RLYS[ch1 - 'A']);
//          vth_popen_rly(mod_id, slot-11, PIN_KDC_RLYS[ch2 - 'A']);
//          vth_popen_rly(mod_id, slot-11, PIN_KX_RLYS[ch2 - 'A']);
//        }
        vth_aopen_rly(mod_id, AIM_X_MINT_RLYS[ch1 - 'A']);
        vth_aopen_rly(mod_id, AIM_X_MINT_RLYS[ch2 - 'A']);
//    vth_aopen_rly(mod_id, AIM_X_MINT_RLYS[channel - 'A']);
//    vth_aopen_rly(mod_id, AIM_X_PLUS_1_MINT_RLYS[channel - 'A']);
//    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);

      } /*if*/
    }  /*for*/
  } /*for*/

  vth_ainit(mod_id);
  vth_pinit(mod_id, slot);
//  fclose(diagfile);
  close_mux(mod_id);
  close_lcr(mod_id);

}  /*t10918*/
#undef LCR_MULT
#undef LCR_FREQ
#undef WAIT_TIME
#undef LCR_FUNCTION
#undef LCR_DISPLAY
#undef LCR_CKT_MODE

#define MUX_NODE_PIN    7
#define LCR_MULT        5
#define LCR_FREQ        100e+03
#define WAIT_TIME       500e-03
#define LCR_FUNCTION    CAPACITANCE
#define LCR_DISPLAY     Q
#define LCR_CKT_MODE    PARALLEL
//-------------------------------------------------------------------------
void t10921(int mod_id, int slot)
{
  /* [PINCARD] Parallel Xall to XG Bus Capacitance */

  /* For path and test descriptions, see the documentation for test: 9221 */
  int next;
  int start;

  double farads;

int channel;
  test_result_type result;
  char *s_ptr;
  char s[MAX_S_LENGTH + 1];
  char *str1;



  open_mux(mod_id);
  open_lcr(mod_id);

  //gpm -- need to make sure that the fixture is locked for this test
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_lock();
  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,
            LCR_MULT);

  vth_aopen_all_rlys(mod_id);

  vth_aclose_rly(mod_id, K752);   /* Gout to XG relay       */
  vth_aclose_rly(mod_id, K760);   /* Gout to XG relay       */
  vth_aclose_rly(mod_id, K741);   /* X1 to mint field relay */
  vth_pclose_rly(mod_id, slot, KDCG);   /* XG to XGL relay        */

  connect_dg(mod_id, true);

  /* ------------------------------------------------------------------ */
  /* Modified 01.28.91 JRM -------------------------------------------- */
  /* Added the following line for SS compatibility.  This allows all    */
  /* pin, pin6, and cp cards to run exactly the same code.              */
  /* ------------------------------------------------------------------ */
//  virt_card_select(mod_id, slot, 3);   /*selects all three virtual cards.*/

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*for*/
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[channel - 'A']);
    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
  }  /*for*/

  channel = 'H';

  vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][channel - 'A']);
  vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][channel - 'A']);

  connect_lcr(mod_id);
  connect_minus_xn_plus_1(mod_id, channel, true);   /* 'H' of Xn + 1 = 'A' */
  connect_plus_gout(mod_id);

  vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][channel - 'A']);
  vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][channel - 'A']);

  vth_mtmr_wait(mod_id, WAIT_TIME);
  measure_lcr(mod_id);
  result.Measurement_class = CLASS_PSTRING_PTR;
  if (get_result_nolog(mod_id, &result))
  {  /*if*/
    s_ptr = static_cast(char*, result.UU.Pstring_ptr);
    strcpy(s, s_ptr);
//  Debug
//  start = strpos2("C", s, 1) + 1;
    start = strpos2(",", s, 1) + 1;
    farads = strtod(s + start - 1, &str1);
    next = str1 - s + 1;
    if (mod_id == 0 || mod_id == 3)
      farads -= sales_rack_offset_xg_cap[0];
    else
      farads -= sales_rack_offset_xg_cap[1];
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = farads;
    log_result(mod_id, result);
  }  /*if*/
  else
    display_prompt("No result in result log (Expected Xn to XG C)", true);

  vth_aopen_all_rlys(mod_id);
  vth_pinit(mod_id, slot);

  close_mux(mod_id);
  close_lcr(mod_id);

}  /*t10921*/
#undef MUX_NODE_PIN
#undef LCR_MULT
#undef LCR_FREQ
#undef WAIT_TIME
#undef LCR_FUNCTION
#undef LCR_DISPLAY
#undef LCR_CKT_MODE

#define RANGE           30e-03
#define RESOLUTION      1e-06
#define LOAD_RESISTANCE  10e+03
#define DMM_FUNCTION    DCV
//-------------------------------------------------------------------------
void t10930(int mod_id, int slot)
{
  /* Xn BUS LEAKAGE CURRENT */
  double dmm_voltage;
  double leakage_current;

  int channel;
  test_result_type result;

  open_mux(mod_id);
  open_dmm(mod_id);

  //gpm -- need to make sure that the fixture is locked for this test
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_lock();
  // Initialize to compatibility mode
  vth_pdis_32chan(mod_id, slot);

  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);

  vth_aclose_rly(mod_id, K741);   /* X1 to mint field relay */
  vth_aclose_rly(mod_id, K761);   /* SWD GND mint pin relay */

  /* ------------------------------------------------------------------ */
  /* Modified 01.28.91 JRM -------------------------------------------- */
  /* Added the following line for SS compatibility.  This allows all    */
  /* pin, pin6, and cp cards to run exactly the same code.              */
  /* ------------------------------------------------------------------ */
//  virt_card_select(mod_id, slot, 3);   /*selects all three virtual cards.*/

  for (channel = 'A'; channel <= 'H'; channel++)
  {  /*for*/
    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);
    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);
    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
  }  /*for*/

  connect_10k_load(mod_id);

  channel = 'A';

  connect_dmm(mod_id);
  connect_plus_xn(mod_id, channel, true);
  connect_minus_swd_gnd(mod_id);

  measure_dmm(mod_id);
  result.Measurement_class = CLASS_REAL;
  if (get_result_nolog(mod_id, &result))
  {  /*if*/
    dmm_voltage = result.UU.Real_result;
    leakage_current = dmm_voltage / LOAD_RESISTANCE;
    result.UU.Real_result = leakage_current;
    log_result(mod_id, result);
  }  /*if*/
  else
    display_prompt("No result in result log (Expected volt Xn 10K)", true);

  vth_aopen_all_rlys(mod_id);
  vth_pinit(mod_id, slot);

  close_mux(mod_id);
  close_dmm(mod_id);

}  /*t10930*/
#undef RANGE
#undef RESOLUTION
#undef LOAD_RESISTANCE
#undef DMM_FUNCTION

// ---  TEST 10999 - SALES TIME AND FIXTURE RELEASE  ---
// ---  COMPLETED! MUST BE LAST POSSIBLE SALES TEST  ---
// ---  WHICH USED TO BE TEST 9999 UNTIL CARIBOU...  ---
void t10999(int /*mod_id*/, int /*slot*/)
{
 char *finish_date; /* finish date and time of sales */
 int hours, minutes, seconds;
 int total_time;    /* total elapsed time in seconds */
 time_t now;
 FILE *timefile;

  timefile = fopen("sales_time", "a");
  assert(timefile);

  /* unlock fixture  */
//  if (! vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
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
}  /* t10999 */
