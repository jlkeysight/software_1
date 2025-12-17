#include <stdlib.h>
#include <vector>
#include <sstream>
#include <cstring>
#include <stdio.h>
#include <string>
#include <time.h>
#include <p2c/p2c.h>
#include <assert.h>
#include "mt9250_p.h"
#include "../../../u/src/num_p.h"
#include "../../../u/src/umath.h"
#include "../../../u/src/OSLayer.h"

#include "../../../vth/src/vth_aim.h"
#include "../../../vth/src/vth_mac.h"
#include "../../../vth/src/aimasm.h"
#include "../../../vth/src/midasm.h"
#include "../../../vth/src/vth_utility.h"
//#include "../../../vth/src/utilityasm.h"
//#include "../tests/mt8000c.h" //added sri, on the 5th Dec 2007 to support utility card load test


static const int GND_BOTH_       = 4;
static const int LONG_           = 1;
static const int R39_            = 1;

int nasru_p10_id=0;


void t9250(int mod_id, int /*slot*/)
{
  /* FIELD SERVICE CAL MINERVA SETUP */
  open_mux(mod_id);
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
}  /*t9250*/

void t9252(int mod_id, int /*slot*/)
{
  /* FIELD SERVICE CAL MINERVA POST CLEANUP */
  close_mux(mod_id);
}  /*t9252*/

#define NUM_POINTS_TO_TEST  9
typedef double voltage_array_[NUM_POINTS_TO_TEST];
typedef int range_array[NUM_POINTS_TO_TEST];
#define RESOLUTION      1.0e-03
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03
/* seconds */
#define ENABLE_WAIT_TIME  1.0e-03
const voltage_array_ TEST = {-10e00, -5e00, -0.2e00, -0.1e00, 0.0e00, 0.1e00, 0.2e00, 5e00, 10e00};
#define DMM_FUNCTION    DCV
void t9255(int mod_id, int /*slot*/)
{
  /*  DC SOURCE VOLTAGE ACCURACY AND RANGE */
  int voltage;

  range_array range;

  a_word_type this_word;
  test_result_type result;

  open_dmm(mod_id);
  open_mux(mod_id);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K837);   /* SRCOUT to RCAL2 relay */
  vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  vth_afc_en_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  for (voltage = 0; voltage < NUM_POINTS_TO_TEST; voltage++)
  {  /* for voltage */
    setup_dmm(mod_id, DMM_FUNCTION, TEST[voltage], RESOLUTION);
    vth_asrc_set_dc_ampl(mod_id, TEST[voltage]);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    measure_dmm(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected dmm reading)", true);

    /* READ RANGE BIT TO VERIFY THE DC SOURCE OPERATES IN THE CORRECT RANGE */
    vth_ard_cell(mod_id, ARLY_2A);
    if (get_result_nolog(mod_id, &result))
    {  /* check the result */
      this_word.data = result.UU.Integer_result;
      /* THE 14th bit of this_word corresponds to binary 2^1 = range bit */
//mjs -- changed bit number from 14 to 1 to reflect changes from pascal
      range[voltage] = P_getbits_US(this_word.bits, 1, 0);
    }  /* check the result */
    else
      display_prompt("No result in result log (Expected asrc range)", true);

  }  /* for voltage */

  for (voltage = 0; voltage < NUM_POINTS_TO_TEST; voltage++)
  {  /* for voltage */
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = range[voltage];
    log_result(mod_id, result);
  }  /* for voltage */

  vth_afc_dis_src_out(mod_id);

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9255 */
#undef NUM_POINTS_TO_TEST
#undef RESOLUTION
#undef SOURCE_WAIT_TIME
#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION

#define HI_END          0
#define LO_END          1
typedef double voltage_array__[2];
typedef int headroom_array[2];
#define RESOLUTION      1.0e-03
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03
/* seconds */
#define ENABLE_WAIT_TIME  1.0e-03
const voltage_array__ TEST_ = {10e00, -10e00};
const headroom_array HEADROOM = {4095, 0};
#define DMM_FUNCTION    DCV
void t9256(int mod_id, int /*slot*/)
{
  /* DC SOURCE VOLTAGE HEADROOM TEST */
  test_result_type result;

  open_dmm(mod_id);
  open_mux(mod_id);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K837);   /* SRCOUT to RCAL2 relay */
  vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  vth_afc_en_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  for (int src_30ma_ilim = 0; src_30ma_ilim <= 1; ++src_30ma_ilim)
  {
    if (src_30ma_ilim == 1)
      vth_asrc_en_30ma_ilim(mod_id);
    else
      vth_asrc_dis_30ma_ilim(mod_id);

    for (int voltage = HI_END; voltage <= LO_END; ++voltage)
    {  /* for voltage */
      setup_dmm(mod_id, DMM_FUNCTION, TEST_[voltage], RESOLUTION);
      /* SETUP THE DC SOURCE TO OUTPUT MAXIMUM PROGRAMMABLE VALUE */
      vth_asrc_set_dc_ampl(mod_id, TEST_[voltage]);
      /* SETUP THE DC SOURCE TO OUTPUT THE MAXIMUM VOLTAGE (FULL SCALE) */
      vth_awr_cell(mod_id, ADAC_SRC_OFST, HEADROOM[voltage]);
      vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
      measure_dmm(mod_id);
      if (!get_result_log(mod_id, &result))
        display_prompt("No result in result log (Expected dmm vth_reading)", true);
    }  /* for voltage */
  }


  vth_afc_dis_src_out(mod_id);

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9256 */
#undef HI_END
#undef LO_END
#undef RESOLUTION
#undef SOURCE_WAIT_TIME
#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION

/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* unenhanced */
#define MEAS_TYPE       0
#define DEFAULT_SOURCE_RESISTANCE  0
#define POSITIVE_TEST_VOLTAGE  100e-03
#define NEGATIVE_TEST_VOLTAGE  (-100e-03)
#define RESOLUTION      1.0e-03
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03
/* seconds */
#define ENABLE_WAIT_TIME  1.0e-03
#define DMM_FUNCTION    DCV
void t9260(int mod_id, int /*slot*/)
{
  /* DC SOURCE OUTPUT RESISTANCE */
  double r10;
  double dmm_no_load_voltage;
  double dmm_loaded_voltage;
  double source_resistance;

  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  setup_dmm(mod_id, DMM_FUNCTION, POSITIVE_TEST_VOLTAGE, RESOLUTION);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K837);   /* SRCOUT to RCAL2 relay */
  vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  vth_afc_en_src_out(mod_id);
  vth_asrc_set_dc_ampl(mod_id, POSITIVE_TEST_VOLTAGE);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
    dmm_no_load_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);
  vth_afc_dis_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  vth_aclose_rly(mod_id, K826);
  vth_aclose_rly(mod_id, K845);
  vth_aclose_rly(mod_id, K855);

  vth_afc_en_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*measure the loaded voltage, calculate resistance and log*/
    dmm_loaded_voltage = result.UU.Real_result;
    if (dmm_loaded_voltage != 0)
      source_resistance =
        (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
    else
      source_resistance = DEFAULT_SOURCE_RESISTANCE;
    result.UU.Real_result = source_resistance;
    log_result(mod_id, result);
  }  /*measure the loaded voltage, calculate resistance and log*/
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_asrc_set_dc_ampl(mod_id, NEGATIVE_TEST_VOLTAGE);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
    dmm_loaded_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);
  vth_afc_dis_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  vth_aopen_rly(mod_id, K826);
  vth_aopen_rly(mod_id, K845);
  vth_aopen_rly(mod_id, K855);

  vth_afc_en_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*measure the unloaded voltage, calculate resistance and log*/
    dmm_no_load_voltage = result.UU.Real_result;
    if (dmm_loaded_voltage != 0)
      source_resistance =
        (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
    else
      source_resistance = DEFAULT_SOURCE_RESISTANCE;
    result.UU.Real_result = source_resistance;
    log_result(mod_id, result);
  }  /*measure the loaded voltage, calculate resistance and log*/
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_afc_dis_src_out(mod_id);

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9260 */
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef DEFAULT_SOURCE_RESISTANCE
#undef POSITIVE_TEST_VOLTAGE
#undef NEGATIVE_TEST_VOLTAGE
#undef RESOLUTION
#undef SOURCE_WAIT_TIME
#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION

#define MIN_TEST_VOLTAGE		10.0e+00
#define MAX_TEST_VOLTAGE		20.0e+00
#define STARTING_INCREMENT		1024e-03
#define OLYM_FINEST_INCREMENT   10e-03	//added by chuanho, 14jun2012
#define FINEST_INCREMENT		8e-03
#define SYN_WAIT_TIME			10e-03
#define OLYM_WAIT_TIME			150e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_CURR			500e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_OCP			1e+00	//added by chuanho, 14jun2012
#define RESOLUTION				1e-03

/* static variables for t9265: */
struct LOC_t9265
{
  int mod_id;
};

static void close_the_relays(struct LOC_t9265 *LINK)
{
  /* CLOSE THE SRCOUT TO SYN. POSITIVE  PATH RELAYS */
  vth_aclose_rly(LINK->mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(LINK->mod_id, K733);   /* X1 bus to S bus relay */
  vth_aclose_rly(LINK->mod_id, K806);   /* S bus to SRCOUT relay */

  /* CLOSE THE P.S. NEGATIVE  TO GROUND PATH RELAYS */
  vth_aclose_rly(LINK->mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(LINK->mod_id, K726);   /* X2 bus to I bus relay */
  vth_aclose_rly(LINK->mod_id, K855);   /* I bus to CALIB2 relay */
  vth_aclose_rly(LINK->mod_id, K854);   /* CALIB2 to GROUND */
}

void t9265(int mod_id_, int /*slot*/)
{
  /* SOURCE POSITIVE OVER VOLTAGE LIMIT ACCURACY */
  struct LOC_t9265 V;

  boolean v_limit;
  /* ADDED ever_had_a_vlimit TO KEEP FROM RETRYING THE SAME VALUES*/
  /*01.07.91*/
  boolean ever_had_a_vlimit;

  double syn_voltage; 
  double olym_ch1_voltage; //Added by chuanho, 14jun2012
  double voltage_increment;

  

  int over_voltage_bit;
  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id_;

  OpenResourceManager();

  open_mux(V.mod_id);
  open_dmm(V.mod_id);
  open_33250A(V.mod_id);

  /*New codes for Olympus*/
  //Added by chuanho, 14jun2012
  if(check_syn_model())
  {
	  open_olym(V.mod_id);
	  connect_olym_ch1(V.mod_id);
	  aconnect_olym_ch1_abus1_xmint(V.mod_id); 
     

  /* SETUP TO DO POSITIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE DC SOURCE */
  vth_afc_dis_src_out(V.mod_id);

  olym_ch1_voltage = MIN_TEST_VOLTAGE;

  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*01.07.91*/
  close_the_relays(&V);

  setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
  setup_olym_ch1_curr(V.mod_id, OLYM_CH1_CURR); 
  setup_olym_ch1_ocp(V.mod_id, OLYM_CH1_OCP); 
  enable_olym_ch1_out(V.mod_id);
  vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME);
  
  vth_astat_clr(V.mod_id);

  while (!v_limit && olym_ch1_voltage <= MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^8  =  BIT NUMBER 7 */
//mjs - the above comment is no longer true.  the fucked up pascal code numbering is
//no more.  now 2^8 IS the 8th bit


    over_voltage_bit = P_getbits_US(this_word.data, 8, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= OLYM_FINEST_INCREMENT ||
          olym_ch1_voltage == MIN_TEST_VOLTAGE) 
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*01.07.91*/
      v_limit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*01.07.91*/
      disable_olym_ch1_out(V.mod_id); 
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays(&V);
      voltage_increment /= 2;
      olym_ch1_voltage -= voltage_increment; 

      setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
	  enable_olym_ch1_out(V.mod_id);
      /* ADDED SYN WAIT TIME */
      /*01.07.90*/
      vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME);
      continue;
    }  /* Go back to the last voltage and reduce the increment */

    if ( ever_had_a_vlimit && !number_equal(voltage_increment, OLYM_FINEST_INCREMENT) )
      voltage_increment /= 2;
    olym_ch1_voltage += voltage_increment; 
    setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage);
	enable_olym_ch1_out(V.mod_id); 
    vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME);
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, olym_ch1_voltage, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_olym_ch1_out_dmm(V.mod_id, true);
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_ainit(V.mod_id);
  disable_olym_ch1_out(V.mod_id);

  close_olym(V.mod_id);
  close_33250A(V.mod_id);
  }
  /*End of new codes for Olympus*/

  else
  {
  open_syn(V.mod_id);
  setup_syn_hv(V.mod_id, true);
  aconnect_syn_out_xmint(V.mod_id);

  /* SETUP TO DO POSITIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE DC SOURCE */
  vth_afc_dis_src_out(V.mod_id);

  syn_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*01.07.91*/
  close_the_relays(&V);
  setup_syn_off(V.mod_id, syn_voltage);
  vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  vth_astat_clr(V.mod_id);

  while (!v_limit && syn_voltage <= MAX_TEST_VOLTAGE)
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^8  =  BIT NUMBER 7 */
//mjs - the above comment is no longer true.  the fucked up pascal code numbering is
//no more.  now 2^8 IS the 8th bit


    over_voltage_bit = P_getbits_US(this_word.data, 8, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT ||
          syn_voltage == MIN_TEST_VOLTAGE)
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*01.07.91*/
      v_limit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*01.07.91*/
      setup_syn_off(V.mod_id, 0.0);
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays(&V);
      voltage_increment /= 2;
      syn_voltage -= voltage_increment;

      setup_syn_off(V.mod_id, syn_voltage);
      /* ADDED SYN WAIT TIME */
      /*01.07.90*/
      vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
      continue;
    }  /* Go back to the last voltage and reduce the increment */
//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    syn_voltage += voltage_increment;
    setup_syn_off(V.mod_id, syn_voltage);
    vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, syn_voltage, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_syn_out_dmm(V.mod_id, true);
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_ainit(V.mod_id);
  close_syn(V.mod_id);
  }

  close_dmm(V.mod_id);
  close_mux(V.mod_id);
  CloseResourceManager();

  

  /* Go to the next increment */
  /* Go to the next increment */
}  /* t9265 */
#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef SYN_WAIT_TIME
#undef OLYM_WAIT_TIME //added by chuanho, 14jun2012
#undef OLYM_CH1_CURR // added by chuanho, 14jun2012
#undef OLYM_CH1_OCP // added by chuanho, 14jun2012
#undef RESOLUTION

#define OLYM_MIN_TEST_VOLTAGE    10.0e+00 //added by chuanho, 14jun2012
#define OLYM_MAX_TEST_VOLTAGE    20.0e+00 //added by chuanho, 14jun2012
#define MIN_TEST_VOLTAGE    -10.0e+00 
#define MAX_TEST_VOLTAGE    -20.0e+00 
#define STARTING_INCREMENT  1024e-03
#define OLYM_FINEST_INCREMENT    10e-03	//added by chuanho, 14jun2012
#define FINEST_INCREMENT    8e-03
#define SYN_WAIT_TIME       10e-03 
#define OLYM_WAIT_TIME      150e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_CURR       500e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_OCP        1e+00   //added by chuanho, 14jun2012
#define RESOLUTION          1e-03

/* static variables for t9266: */
struct LOC_t9266
{
  int mod_id;
};

static void close_the_relays_(struct LOC_t9266 *LINK)
{
  /* CLOSE THE SRCOUT TO SYN. POSITIVE  PATH RELAYS */
  vth_aclose_rly(LINK->mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(LINK->mod_id, K733);   /* X1 bus to S bus relay */
  vth_aclose_rly(LINK->mod_id, K806);   /* S bus to SRCOUT relay */

  /* CLOSE THE P.S. NEGATIVE  TO GROUND PATH RELAYS */
  vth_aclose_rly(LINK->mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(LINK->mod_id, K726);   /* X2 bus to I bus relay */
  vth_aclose_rly(LINK->mod_id, K855);   /* I bus to CALIB2 relay */
  vth_aclose_rly(LINK->mod_id, K854);   /* CALIB2 to GROUND */
}

void t9266(int mod_id_, int /*slot*/)
{
  /* SOURCE NEGATIVE OVER VOLTAGE LIMIT ACCURACY */
  struct LOC_t9266 V;

  boolean v_limit;
  /* ADDED ever_had_a_vlimit TO KEEP FROM RETRYING THE SAME VALUES*/
  /*01.07.91*/
  boolean ever_had_a_vlimit;

  double syn_voltage; 
  double olym_ch1_voltage; //Added by chuanho, 14jun2012
  double voltage_increment;

  int over_voltage_bit;
  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id_;

  OpenResourceManager(); 
  open_dmm(V.mod_id);
  open_mux(V.mod_id);
  open_33250A(V.mod_id);

  /*New codes for Olympus*/
  //added by chuanho, 14jun2012
  if(check_syn_model())
  {
  open_olym(V.mod_id);
  connect_olym_ch1(V.mod_id); 
  aconnect_olym_ch1_abus1_xmint(V.mod_id); 

  /* SETUP TO DO POSITIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE DC SOURCE */
  vth_afc_dis_src_out(V.mod_id);

  olym_ch1_voltage = OLYM_MIN_TEST_VOLTAGE; 
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*01.07.91*/
  close_the_relays_(&V);

  setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
  setup_olym_ch1_curr(V.mod_id, OLYM_CH1_CURR); 
  setup_olym_ch1_ocp(V.mod_id, OLYM_CH1_OCP);
  enable_olym_ch1_out(V.mod_id); 
  reverse_olym_ch1_out(V.mod_id); 
  vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
  vth_astat_clr(V.mod_id);

  while (!v_limit && olym_ch1_voltage <= OLYM_MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^8  =  BIT NUMBER 7 */
//see comment for test 9265 -- mjs
    over_voltage_bit = P_getbits_US(this_word.bits, 8, 0);
	/* Go back to the last voltage and reduce the increment */
    if (over_voltage_bit == 1)
    {  
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= OLYM_FINEST_INCREMENT || olym_ch1_voltage == OLYM_MIN_TEST_VOLTAGE)
			continue;
		/* ADDED 'AND (syn_voltage <> min_test_voltage' */
		/*01.07.91*/
			v_limit = false;
		/* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
		/*01.07.91*/
		disable_olym_ch1_out(V.mod_id); 
		vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
		close_the_relays_(&V);
		voltage_increment /= 2;
		olym_ch1_voltage -= voltage_increment;
		setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage);
		enable_olym_ch1_out(V.mod_id); 
		/* ADDED SYN WAIT TIME */
		/*01.07.90*/
		vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME);
		continue;
    }  
	/* Go back to the last voltage and reduce the increment */

//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, OLYM_FINEST_INCREMENT) )
		voltage_increment /= 2;
		olym_ch1_voltage += voltage_increment; 
		setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
		vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, olym_ch1_voltage, RESOLUTION); 
  connect_dmm(V.mod_id);
  aconnect_olym_ch1_out_dmm(V.mod_id, true); 
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
	  display_prompt("No result in result log (Expected dmm vth_reading)", true);
  vth_ainit(V.mod_id);
  disable_olym_ch1_out(V.mod_id); 
  close_olym(V.mod_id);
  close_33250A(V.mod_id);
  }

  else
  {
  open_syn(V.mod_id); 
  setup_syn_hv(V.mod_id, true); 
  aconnect_syn_out_xmint(V.mod_id); 

  /* SETUP TO DO POSITIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE DC SOURCE */
  vth_afc_dis_src_out(V.mod_id);

  syn_voltage = MIN_TEST_VOLTAGE; 
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*01.07.91*/
  close_the_relays_(&V);
  setup_syn_off(V.mod_id, syn_voltage);
  vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
  vth_astat_clr(V.mod_id);

  while (!v_limit && syn_voltage >= MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^8  =  BIT NUMBER 7 */
//see comment for test 9265 -- mjs
    over_voltage_bit = P_getbits_US(this_word.bits, 8, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT ||
          syn_voltage == MIN_TEST_VOLTAGE)
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*01.07.91*/
      v_limit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*01.07.91*/
      setup_syn_off(V.mod_id, 0.0); 
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays_(&V);
      voltage_increment /= 2;
      syn_voltage += voltage_increment; 

      setup_syn_off(V.mod_id, syn_voltage); 

      /* ADDED SYN WAIT TIME */
      /*01.07.90*/
      vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
      continue;
    }  /* Go back to the last voltage and reduce the increment */
//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    syn_voltage -= voltage_increment;
    setup_syn_off(V.mod_id, syn_voltage); 
	vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, syn_voltage, RESOLUTION); 
  connect_dmm(V.mod_id);
  aconnect_syn_out_dmm(V.mod_id, true); 
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_ainit(V.mod_id);
  close_syn(V.mod_id); 
  }

  close_dmm(V.mod_id);
  close_mux(V.mod_id);
  CloseResourceManager(); //added by chuanho, 13jul2011

  /* Go to the next increment */
  /* Go to the next increment */
}  /* t9266 */
#undef MIN_TEST_VOLTAGE	
#undef MAX_TEST_VOLTAGE
#undef OLYM_MIN_TEST_VOLTAGE	//added by chuanho, 14jun2012
#undef OLYM_MAX_TEST_VOLTAGE	//added by chuanho, 14jun2012
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef OLYM_FINEST_INCREMENT	//added by chuanho, 14jun2012
#undef SYN_WAIT_TIME 
#undef OLYM_WAIT_TIME //added by chuanho, 14jun2012
#undef OLYM_CH1_CURR //added by chuanho, 14jun2012
#undef OLYM_CH1_OCP  //added by chuanho, 14jun2012
#undef RESOLUTION

/* Where to Measure Voltage */
/* Integer -1 = LEFT */
#define INTERNAL_SRC_VOLTAGE  10e+00
#define EXTERNAL_SRC_VOLTAGE  20e+00
#define OLYM_WAIT_TIME	      150e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_CURR		  500e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_OCP		  1e+00 //added by chuanho, 14jun2012

void t9267(int mod_id, int /*slot*/)
{
  /* DC SOURCE POSITIVE OVER VOLTAGE PROTECTION */

  int over_voltage_bit;
  int fatal_error_bit;
  a_word_type this_word;
  test_result_type result;
  int i;
  int high;
  int num_samples;
  int num_samples2;
  double max_volts;
  double average;
  double interval;
  double trig_level;
  arrform waveform;
  arrform waveform2;

  OpenResourceManager();
  open_dmm50(mod_id);
  open_mux(mod_id);
  open_33250A(mod_id);

  /*New codes for Olympus*/
  //Added by chuanho, 14jun2012
  if(check_syn_model())
  {
  open_olym(mod_id); 
  connect_olym_ch1(mod_id); 
  aconnect_olym_ch1_abus1_xmint(mod_id); 
  connect_dmm50_out_af2(mod_id);
  vth_afc_dis_src_out(mod_id);

  /* SETUP TO DO OVERVOLTAGE MEASUREMENT */

  /* CLOSE THE AF2 MEASUREMENT PATH RELAYS */
  vth_aclose_rly(mod_id, K818);   /* SRCOUT to L bus relay */
  vth_aclose_rly(mod_id, K754);   /* L bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* XL bus to AF2 relay */

  /* CLOSE THE SRCOUT TO SYNTHESIZER PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K733);   /* X1 bus to S bus relay */
  vth_aclose_rly(mod_id, K829);   /* S bus to SRCOUT relay */

  /* CLOSE THE SYNTHESIZER TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K726);   /* X2 bus to I bus relay */
  vth_aclose_rly(mod_id, K813);   /* I bus to DETLO relay */
  vth_aclose_rly(mod_id, K2305);   /* DETLO path relay */
  vth_aclose_rly(mod_id, K2303);   /* DETLO to GROUND path relay */

  flush_commands(mod_id);
  
  /* Since the Olympus cannot be tri-stated, set it to 10v before setting */
  /* up the DC source on the ASRU card.                                       */

  setup_olym_ch1_volt(mod_id, INTERNAL_SRC_VOLTAGE);
  setup_olym_ch1_curr(mod_id, OLYM_CH1_CURR); 
  setup_olym_ch1_ocp(mod_id, OLYM_CH1_OCP); 
  enable_olym_ch1_out(mod_id); 
 
  vth_asrc_en_30ma_ilim(mod_id);
  vth_asrc_set_dc_ampl(mod_id, INTERNAL_SRC_VOLTAGE);
  vth_afc_en_src_out(mod_id);
  flush_commands(mod_id);

  //setup trigger for dmm
  num_samples = 2300;
  interval = 20E-6;
  trig_level = 13;
  setup_dmm_digitizing(mod_id, DMM50_ADDRESS, 100, interval, num_samples, 1.4E-6, 1E-3, trig_level, POS);
  //trigger dmm
  flush_commands(mod_id);

  setup_olym_ch1_volt(mod_id, EXTERNAL_SRC_VOLTAGE);
  //vth_mtmr_wait(mod_id, OLYM_WAIT_TIME); //added by chuanho
  /* No wait should be required for the synthesizer to ramp up from 10 to 20v */

  wait_for_dmm50_measurement(mod_id);


  /* MAKE SURE THE VOLTAGE LIMIT HAS OCCURRED */ /*subtest 0*/
  vth_astat_rd(mod_id);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 8, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = over_voltage_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MEASURE THE PEAK VOLTAGE CAUSED BY TRI-STATE */ /*subtest 1*/

  //take dmm array and find max voltage
  get_dmm_volt(mod_id, DMM50_ADDRESS, num_samples, waveform, 0);

      max_volts = waveform[0];
      high = 0;
	for(i= 1; i < num_samples; i++) 
	{
	  if (max_volts < waveform[i])
	  {
	  max_volts = waveform[i];
	  high = i;
	  }
    }
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = max_volts;
      log_result(mod_id, result);

  /* MAKE SURE THAT THE RELAYS OPENED - FATAL ERROR */ /*subtest 2*/
  /* NEED TO REPLACE THIS ROUTINE WITH A HIGHER LEVEL ROUTINE WHEN AVAILABLE */
  vth_mrd_cell(mod_id, MMFP_GPIO);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    fatal_error_bit = P_getbits_US(this_word.bits, 6, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = fatal_error_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MAKE SURE THAT THE RELAYS OPENED - CHECK MINIMUM VOLTAGE AT SCOPE */ /*subtest 3*/
  //find the voltage at 3.6ms after the relays close (this is the dmm test)
  // READ and make sure that it reaches desired voltage

  /* MAKE SURE THAT THE SOURCE COMES BACK UP - CHECK VOLTAGE with dmm50 */
  //setup trigger for dmm measurement  and a dc measurement
  //just take average of last 5 samples to check the voltage  at which
  // it is settling...

  vth_mhpib_wr(mod_id, 8, "PRESET NORM");

  // average the last 5 samples to find out what voltage the card settled at
  num_samples2 = 5;
  average = 0;
  for (i=num_samples-5; i<num_samples; i++)
    average += waveform[i];

  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_astat_clr(mod_id);
  /* close the AF2 measurement path relays */
  vth_aclose_rly(mod_id, K2702);   /* Source internal relay */
  vth_aclose_rly(mod_id, K818);   /* SRCOUT to L bus relay */
  vth_aclose_rly(mod_id, K754);   /* L bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* XL bus to AF2 relay */


  vth_mtmr_wait(mod_id, .5);
  num_samples2 = 5;
  average = 0;


  // measure and average the return voltage /*subtest 4*/
  for (i=0; i<num_samples2; i++)
  {
    measure_dmm50(mod_id);
    if (get_result_nolog(mod_id, &result))
      waveform2[i] = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected DMM v)", true);
    average += waveform2[i];
  }
  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_afc_dis_src_out(mod_id);
  disable_olym_ch1_out(mod_id); 
  close_olym(mod_id); 
  close_33250A(mod_id);
  }
  /*End of new olympus codes*/

  else
  {
  open_syn(mod_id);
  setup_syn_hv(mod_id, true); 
  aconnect_syn_out_xmint(mod_id); 
  connect_dmm50_out_af2(mod_id);
  vth_afc_dis_src_out(mod_id);

  /* SETUP TO DO OVERVOLTAGE MEASUREMENT */

  /* CLOSE THE AF2 MEASUREMENT PATH RELAYS */
  vth_aclose_rly(mod_id, K818);   /* SRCOUT to L bus relay */
  vth_aclose_rly(mod_id, K754);   /* L bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* XL bus to AF2 relay */

  /* CLOSE THE SRCOUT TO SYNTHESIZER PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K733);   /* X1 bus to S bus relay */
  vth_aclose_rly(mod_id, K829);   /* S bus to SRCOUT relay */

  /* CLOSE THE SYNTHESIZER TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K726);   /* X2 bus to I bus relay */
  vth_aclose_rly(mod_id, K813);   /* I bus to DETLO relay */
  vth_aclose_rly(mod_id, K2305);   /* DETLO path relay */
  vth_aclose_rly(mod_id, K2303);   /* DETLO to GROUND path relay */
  
  /* Since the synthesizer cannot be tri-stated, set it to 10v before setting */
  /* up the DC source on the ASRU card.                                       */
  setup_syn(mod_id, DC, 0, 0.0, INTERNAL_SRC_VOLTAGE);
  vth_asrc_en_30ma_ilim(mod_id);
  vth_asrc_set_dc_ampl(mod_id, INTERNAL_SRC_VOLTAGE);
  vth_afc_en_src_out(mod_id);

  //setup trigger for dmm
  num_samples = 2300;
  interval = 20E-6;
  trig_level = 13;
  setup_dmm_digitizing(mod_id, DMM50_ADDRESS, 100, interval, num_samples, 1.4E-6, 1E-3, trig_level, POS);

  setup_syn_off(mod_id, EXTERNAL_SRC_VOLTAGE); 
  /* No wait should be required for the synthesizer to ramp up from 10 to 20v */

  //trigger dmm
  flush_commands(mod_id);
  wait_for_dmm50_measurement(mod_id);


  /* MAKE SURE THE VOLTAGE LIMIT HAS OCCURRED */ /*subtest 0*/
  vth_astat_rd(mod_id);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 8, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = over_voltage_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MEASURE THE PEAK VOLTAGE CAUSED BY TRI-STATE */ /*subtest 1*/

  //take dmm array and find max voltage
  get_dmm_volt(mod_id, DMM50_ADDRESS, num_samples, waveform, 0);

      max_volts = waveform[0];
      high = 0;
	for(i= 1; i < num_samples; i++) 
	{
	  if (max_volts < waveform[i])
	  {
	  max_volts = waveform[i];
	  high = i;
	  }
    }
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = max_volts;
      log_result(mod_id, result);

  /* MAKE SURE THAT THE RELAYS OPENED - FATAL ERROR */ /*subtest 2*/
  /* NEED TO REPLACE THIS ROUTINE WITH A HIGHER LEVEL ROUTINE WHEN AVAILABLE */
  vth_mrd_cell(mod_id, MMFP_GPIO);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    fatal_error_bit = P_getbits_US(this_word.bits, 6, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = fatal_error_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MAKE SURE THAT THE RELAYS OPENED - CHECK MINIMUM VOLTAGE AT SCOPE */ /*subtest 3*/
  //find the voltage at 3.6ms after the relays close (this is the dmm test)
  // READ and make sure that it reaches desired voltage

  /* MAKE SURE THAT THE SOURCE COMES BACK UP - CHECK VOLTAGE with dmm50 */
  //setup trigger for dmm measurement  and a dc measurement
  //just take average of last 5 samples to check the voltage  at which
  // it is settling...

  vth_mhpib_wr(mod_id, 8, "PRESET NORM");

  // average the last 5 samples to find out what voltage the card settled at
  num_samples2 = 5;
  average = 0;
  for (i=num_samples-5; i<num_samples; i++)
    average += waveform[i];

  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_astat_clr(mod_id);
  /* close the AF2 measurement path relays */
  vth_aclose_rly(mod_id, K2702);   /* Source internal relay */
  vth_aclose_rly(mod_id, K818);   /* SRCOUT to L bus relay */
  vth_aclose_rly(mod_id, K754);   /* L bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* XL bus to AF2 relay */


  vth_mtmr_wait(mod_id, .5);
  num_samples2 = 5;
  average = 0;


  // measure and average the return voltage /*subtest 4*/
  for (i=0; i<num_samples2; i++)
  {
    measure_dmm50(mod_id);
    if (get_result_nolog(mod_id, &result))
      waveform2[i] = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected DMM v)", true);
    average += waveform2[i];
  }
  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_afc_dis_src_out(mod_id);
  close_syn(mod_id); 
  }

  close_mux(mod_id);
  close_dmm50(mod_id);
  CloseResourceManager();

}  /* t9267 */
#undef INTERNAL_SRC_VOLTAGE
#undef EXTERNAL_SRC_VOLTAGE
#undef OLYM_WAIT_TIME //added by chuanho, 8Feb2012
#undef OLYM_CH1_CURR //added by chuanho, 13jul2011
#undef OLYM_CH1_OCP //added by chuanho, 13jul2011

/* Where to Measure voltage */
/* Integer -1 = LEFT */
#define INTERNAL_SRC_VOLTAGE			-10e+00
#define EXTERNAL_SRC_VOLTAGE            -20e+00
#define EXTERNAL_SRC_VOLTAGE_NEG10V		10e+00 //added by chuanho, 14jun2012
#define EXTERNAL_SRC_VOLTAGE_NEG20V		20e+00 //added by chuanho, 14jun2012
#define OLYM_WAIT_TIME					150e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_CURR					500e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_OCP					1e+00 //added by chuanho, 14jun2012
void t9268(int mod_id, int /*slot*/)
{
  /* DC SOURCE NEGATIVE OVER VOLTAGE PROTECTION */
  int over_voltage_bit;
  int fatal_error_bit;
  a_word_type this_word;
  test_result_type result;
  arrform waveform;
  arrform waveform2;
  int i;
  double min_volt;
  double average;
  int num_samples;
  int num_samples2;

  OpenResourceManager(); 
  open_mux(mod_id);
  open_dmm50(mod_id);
  open_33250A(mod_id);

  /*New olympus codes*/
  //Added by chuanho, 14jun2012
  if(check_syn_model())
  {
	  open_olym(mod_id);
	  connect_olym_ch1(mod_id);
	  aconnect_olym_ch1_abus1_xmint(mod_id);
	  vth_afc_dis_src_out(mod_id);
	  connect_dmm50_out_af2(mod_id);

  /* SETUP TO DO OVERVOLTAGE MEASUREMENT */

  /* CLOSE THE AF2 MEASUREMENT PATH RELAYS */
  vth_aclose_rly(mod_id, K818);   /* SRCOUT to L bus relay */
  vth_aclose_rly(mod_id, K754);   /* L bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* XL bus to AF2 relay */

  /* CLOSE THE SRCOUT TO SYNTHESIZER PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K733);   /* X1 bus to S bus relay */
  vth_aclose_rly(mod_id, K829);   /* S bus to SRCOUT relay */

  /* CLOSE THE SYNTHESIZER TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K726);   /* X2 bus to I bus relay */
  vth_aclose_rly(mod_id, K813);   /* I bus to DETLO relay */
  vth_aclose_rly(mod_id, K2305);   /* DETLO path relay */
  vth_aclose_rly(mod_id, K2303);   /* DETLO to GROUND path relay */

  flush_commands(mod_id);

  /* Since the synthesizer cannot be tri-stated, set it to 10v before setting  up the DC source on the ASRU card.      */
  setup_olym_ch1_volt(mod_id, EXTERNAL_SRC_VOLTAGE_NEG10V);
  setup_olym_ch1_curr(mod_id, OLYM_CH1_CURR); 
  setup_olym_ch1_ocp(mod_id, OLYM_CH1_OCP);
  enable_olym_ch1_out(mod_id);
  reverse_olym_ch1_out(mod_id); 
  vth_mtmr_wait(mod_id, OLYM_WAIT_TIME);

  vth_asrc_en_30ma_ilim(mod_id);
  vth_asrc_set_dc_ampl(mod_id, INTERNAL_SRC_VOLTAGE);
  vth_afc_en_src_out(mod_id);

  flush_commands(mod_id);

  num_samples = 2300;  // number of samples to make with the dmm
  // get the dmm ready to catch the voltage on its way from 10v to 20v
  setup_dmm_digitizing(mod_id, DMM50_ADDRESS, 100, 20E-6, num_samples, 1.4E-6, 1E-3, -13, NEG);
  flush_commands(mod_id);
  
  /* No wait should be required for the synthesizer to ramp up from 10 to 20v */
  setup_olym_ch1_volt(mod_id, EXTERNAL_SRC_VOLTAGE_NEG20V);
  
  wait_for_dmm50_measurement(mod_id);  // wait for the dmm to see the trigger

  /* MAKE SURE THE VOLTAGE LIMIT HAS OCCURRED */
  vth_astat_rd(mod_id);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 8, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = over_voltage_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MEASURE THE PEAK VOLTAGE CAUSED BY TRI-STATE */
  get_dmm_volt(mod_id, DMM50_ADDRESS, num_samples, waveform, 0);

  // find the abs value max voltage reached
  min_volt = waveform[0];
  for(i= 1; i < num_samples; i++)
  {
    if (min_volt > waveform[i])
	min_volt = waveform[i];
  }

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = min_volt;
  log_result(mod_id, result);


  /* MAKE SURE THAT THE RELAYS OPENED - FATAL ERROR */
  /* NEED TO REPLACE THIS ROUTINE WITH A HIGHER LEVEL ROUTINE WHEN AVAILABLE */
  vth_mrd_cell(mod_id, MMFP_GPIO);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    fatal_error_bit = P_getbits_US(this_word.bits, 6, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = fatal_error_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MAKE SURE THAT THE RELAYS OPENED - CHECK MAXIMUM VOLTAGE AT SCOPE */

  vth_mhpib_wr(mod_id, 8, "PRESET NORM");

  // average the last 5 samples to find out what voltage the card settled at
  num_samples2 = 5;
  average = 0;
  for (i=num_samples-5; i<num_samples; i++)
    average += waveform[i];

  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);


  vth_astat_clr(mod_id);
  /* close the AF2 measurement path relays */
  vth_aclose_rly(mod_id, K2702);   /* Source internal relay */
  vth_aclose_rly(mod_id, K818);   /* SRCOUT to L bus relay */
  vth_aclose_rly(mod_id, K754);   /* L bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* XL bus to AF2 relay */

  vth_mtmr_wait(mod_id, .5);
  num_samples2 = 5;
  average = 0;
  // measure and average the return voltage
  for (i=0; i<num_samples2; i++)
  {
    measure_dmm50(mod_id);
    if (get_result_nolog(mod_id, &result))
      waveform2[i] = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected DMM v)", true);
    average += waveform2[i];
  }
  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_afc_dis_src_out(mod_id);
  disable_olym_ch1_out(mod_id);
  close_olym(mod_id);
  close_33250A(mod_id);
  }

  else
  {
  open_syn(mod_id); 
  setup_syn_hv(mod_id, true); 
  aconnect_syn_out_xmint(mod_id); 
  connect_dmm50_out_af2(mod_id);
  vth_afc_dis_src_out(mod_id);

  /* SETUP TO DO OVERVOLTAGE MEASUREMENT */

  /* CLOSE THE AF2 MEASUREMENT PATH RELAYS */
  vth_aclose_rly(mod_id, K818);   /* SRCOUT to L bus relay */
  vth_aclose_rly(mod_id, K754);   /* L bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* XL bus to AF2 relay */

  /* CLOSE THE SRCOUT TO SYNTHESIZER PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K733);   /* X1 bus to S bus relay */
  vth_aclose_rly(mod_id, K829);   /* S bus to SRCOUT relay */

  /* CLOSE THE SYNTHESIZER TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K726);   /* X2 bus to I bus relay */
  vth_aclose_rly(mod_id, K813);   /* I bus to DETLO relay */
  vth_aclose_rly(mod_id, K2305);   /* DETLO path relay */
  vth_aclose_rly(mod_id, K2303);   /* DETLO to GROUND path relay */

  /* Since the synthesizer cannot be tri-stated, set it to 10v before setting  up the DC source on the ASRU card.      */
  setup_syn(mod_id, DC, 0, 0.0, INTERNAL_SRC_VOLTAGE); 

  vth_asrc_en_30ma_ilim(mod_id);
  vth_asrc_set_dc_ampl(mod_id, INTERNAL_SRC_VOLTAGE);
  vth_afc_en_src_out(mod_id);

  num_samples = 2300;  // number of samples to make with the dmm
  // get the dmm ready to catch the voltage on its way from 10v to 20v
  setup_dmm_digitizing(mod_id, DMM50_ADDRESS, 100, 20E-6, num_samples, 1.4E-6, 1E-3, -13, NEG);

  setup_syn_off(mod_id, EXTERNAL_SRC_VOLTAGE); 
  /* No wait should be required for the synthesizer to ramp up from 10 to 20v */

  flush_commands(mod_id);
  wait_for_dmm50_measurement(mod_id);  // wait for the dmm to see the trigger

  /* MAKE SURE THE VOLTAGE LIMIT HAS OCCURRED */
  vth_astat_rd(mod_id);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 8, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = over_voltage_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MEASURE THE PEAK VOLTAGE CAUSED BY TRI-STATE */
  get_dmm_volt(mod_id, DMM50_ADDRESS, num_samples, waveform, 0);

  // find the abs value max voltage reached
  min_volt = waveform[0];
  for(i= 1; i < num_samples; i++)
  {
    if (min_volt > waveform[i])
	min_volt = waveform[i];
  }

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = min_volt;
  log_result(mod_id, result);


  /* MAKE SURE THAT THE RELAYS OPENED - FATAL ERROR */
  /* NEED TO REPLACE THIS ROUTINE WITH A HIGHER LEVEL ROUTINE WHEN AVAILABLE */
  vth_mrd_cell(mod_id, MMFP_GPIO);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    fatal_error_bit = P_getbits_US(this_word.bits, 6, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = fatal_error_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MAKE SURE THAT THE RELAYS OPENED - CHECK MAXIMUM VOLTAGE AT SCOPE */

  vth_mhpib_wr(mod_id, 8, "PRESET NORM");

  // average the last 5 samples to find out what voltage the card settled at
  num_samples2 = 5;
  average = 0;
  for (i=num_samples-5; i<num_samples; i++)
    average += waveform[i];

  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);


  vth_astat_clr(mod_id);
  /* close the AF2 measurement path relays */
  vth_aclose_rly(mod_id, K2702);   /* Source internal relay */
  vth_aclose_rly(mod_id, K818);   /* SRCOUT to L bus relay */
  vth_aclose_rly(mod_id, K754);   /* L bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* XL bus to AF2 relay */

  vth_mtmr_wait(mod_id, .5);
  num_samples2 = 5;
  average = 0;
  // measure and average the return voltage
  for (i=0; i<num_samples2; i++)
  {
    measure_dmm50(mod_id);
    if (get_result_nolog(mod_id, &result))
      waveform2[i] = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected DMM v)", true);
    average += waveform2[i];
  }
  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);


  vth_afc_dis_src_out(mod_id);
  close_syn(mod_id); 
  }

  close_mux(mod_id);
  close_dmm50(mod_id);
  CloseResourceManager();

}  /* t9268 */
#undef INTERNAL_SRC_VOLTAGE
#undef EXTERNAL_SRC_VOLTAGE
#undef EXTERNAL_SRC_VOLTAGE_NEG10V	//added by chuanho, 14jun2012
#undef EXTERNAL_SRC_VOLTAGE_NEG20V	//added by chuanho, 14jun2012
#undef OLYM_WAIT_TIME //added by chuanho, 14jun2012
#undef OLYM_CH1_CURR //added by chuanho, 14jun2012
#undef OLYM_CH1_OCP //added by chuanho, 14jun2012

#define POSITIVE_SRC_VOLTAGE  5.0e+00
#define NEGATIVE_SRC_VOLTAGE  (-5.0e+00)
#define MEASURE_WAIT_TIME  2.0e+00
void t9270(int mod_id, int /*slot*/)
{
  /* Modified 6-30-99-  ABR ----------------------------------------------- */
  /* The scope was eliminated from the test and was replaced with a counter */
  /* The same measurements are taken and the test works basically the same. */
  /* however, the voltage measurements would be better if the resolution    */
  /* was better.  We tried to substitute the counter with the dmm but the   */
  /* dmm's best case sub-sampling rate is 10ns and the signal needed        */
  /* to be digitized is about 20ns.  So the sampling rate is too slow.      */
  /* Check notes for a more detailed explanation.                           */
  /* ---------------------------------------------------------------------- */

  /* DC SOURCE TRI-STATE SPIKE TEST */
  double min_volt;
  double max_volt;

  test_result_type result;

  open_cntr(mod_id);
  open_mux(mod_id);

  setup_cntr_defaults(mod_id);
/******************************************************************
abr 07/98 - The counter is set to 50 ohms and DC coupling
I think the attenuation does not matter in this case
because the voltage is around 5V but it could
potentially be  a problem.
*******************************************************************/
  vth_mhpib_wr(mod_id, 4, ":INP:ATT 10");
  vth_mhpib_wr(mod_id, 4, ":INP:COUP DC");
  vth_mhpib_wr(mod_id, 4, ":INP:IMP 50");

  aconnect_osc_ch1_af2(mod_id);

  /* SETUP TO DO TRI-STATE SPIKE MEASUREMENT */

  /* CLOSE THE AF2 MEASUREMENT PATH RELAYS */
  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* XL bus to AF2 relay */

  vth_asrc_dis_30ma_ilim(mod_id);
  vth_asrc_set_dc_ampl(mod_id, POSITIVE_SRC_VOLTAGE);

/*******************************************************************
This section will setup the trigger for the counter
********************************************************************/
  vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'VOLT:MIN'");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:START:SOUR IMM");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:STOP:SOUR DIG");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:STOP:DIG 13");

  vth_afc_en_src_out(mod_id);   /* TAKES THE SRC OUT OF TRI-STATE */

  vth_mtmr_wait(mod_id, MEASURE_WAIT_TIME);

  /* MEASURE THE PEAK VOLTAGE CAUSED BY TRI-STATE */

/******************************************************************
abr 07/98 - Measure the peak voltage caused by TRI-STATE

The scope used a -5V offset for better measurement,
for that reason, I substracted 5V offset to the min_volts
variable
*******************************************************************/
  trigger_cntr(mod_id);
  min_volt = read_cntr(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = (min_volt - 5);
  log_result(mod_id, result);

  vth_afc_dis_src_out(mod_id);

  vth_asrc_set_dc_ampl(mod_id, NEGATIVE_SRC_VOLTAGE);

/*******************************************************************
This section will setup the trigger for the counter
********************************************************************/
  vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'VOLT:MAX'");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:START:SOUR IMM");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:STOP:SOUR DIG");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:STOP:DIG 13");

  vth_afc_en_src_out(mod_id);   /* TAKES THE SRC OUT OF TRI-STATE */

  vth_mtmr_wait(mod_id, MEASURE_WAIT_TIME);

  /* MEASURE THE PEAK VOLTAGE CAUSED BY TRI-STATE */
/******************************************************************
abr 07/98 - Measure the peak voltage caused by TRI-STATE

The scope used a 5V offset for better measurement,
for that reason, I added 5V offset to the max_volts
variable
*******************************************************************/
  trigger_cntr(mod_id);
  max_volt = read_cntr(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = max_volt + 5;
  log_result(mod_id, result);

  vth_afc_dis_src_out(mod_id);

  vth_ainit(mod_id);

  close_cntr(mod_id);
  close_mux(mod_id);

}  /* t9270 */
#undef POSITIVE_SRC_VOLTAGE
#undef NEGATIVE_SRC_VOLTAGE
#undef MEASURE_WAIT_TIME
#undef LOW
#undef HIGH
#undef R625
#undef R2500
#undef R10000
#undef DIFF

#define LOW             0
#define HIGH            1
#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1
typedef double source_voltage_array[2];
/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        10
/* FOR M.A.D. */
#define DIGITAL         0
/* 625mv detector range */
#define R625            4
/* 2.5v detector range */
#define R2500           6
/* 10v detector range */
#define R10000          8
/* initial requiv guess */
#define RINIT           4.0e+00
/* initial rseries guess */
#define RSINIT          0.6e+00
/* initial vatten guess */
#define VINIT           1.01e+00
#define EXPECTED_DET_VOLTS  2.0e+00
#define TIMEOUT         10.0e+00
#define PS_CURRENT      800e-03
#define DIFF            5e-03
#define RAMP_TIME       10e+00
#define LOW_CURRENT_VALUE  27.5e-03
#define HIGH_CURRENT_VALUE  165e-03
const source_voltage_array SOURCE_VOLTAGE = {7e00, -7e00}; 
void t9275(int mod_id, int /*slot*/)
{
  /* DC SOURCE OUTPUT CURRENT TEST */
  int source_i_compliance;
  double r10;
  double detector_voltage;
  double ps_sense_voltage;
  double ps_voltage;
  double source_current;
  double meas_source_current;
  double vload;
  double vnoload;
  double vatten;
  double requiv;
  double rseries;

  test_result_type result;
  a_word_type this_word;


  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_ps(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, EXPECTED_DET_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);   /* DETLO to L bus relay */
  vth_aopen_rly(mod_id, K812);   /* DETHI to I bus relay */

  /* CLOSE THE SRCOUT TO P.S. POSITIVE  PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K826);   /* 10 ohm resistor to SRCOUT */

  /* CLOSE THE P.S. NEGATIVE  TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAYS */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */
  vth_aclose_rly(mod_id, K828);   /* DETHI input */

  /* MEASURE THE POWER SUPPLY ATTENUATION FACTOR AND THE EQUIVALENT RESISTANCE */
  requiv = RINIT;
  vatten = VINIT;
  rseries = RSINIT;
  vth_asrc_dis_30ma_ilim(mod_id);
  source_current = -1 * LOW_CURRENT_VALUE;   /* AMPS */
  ps_voltage = vatten * (SOURCE_VOLTAGE[POSITIVE] +
                         source_current * (r10 + requiv + rseries));
  vth_aclose_rly(mod_id, K725);
  vth_aclose_rly(mod_id, K718);
  vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE[POSITIVE]);
  vth_afc_en_src_out(mod_id);
  setup_ps(mod_id, ps_voltage, PS_CURRENT);
  verify_ps_output(mod_id, DIFF, RAMP_TIME, fabs(ps_voltage));
  vth_astat_clr(mod_id);

  vth_afunc_meas(mod_id);
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    detector_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in result log (Expected DET stat)", true);

  meas_source_current = fabs(detector_voltage / r10);
  if (meas_source_current > 1.5 * fabs(source_current) ||
      meas_source_current < 0.5 * fabs(source_current))
    meas_source_current = fabs(source_current);

  ps_sense_voltage = measure_ps_sense_voltage(mod_id);

  vth_adet_sel_rng(mod_id, R10000);
  vth_aopen_rly(mod_id, K2306);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2304);   /* reconfigure for grounded meas */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_afunc_meas(mod_id);   /* measure the loaded ps voltage */
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    vload = fabs(result.UU.Real_result);
  else
    display_prompt("No result in log(Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in log (Expected DET stat)", true);

  vth_afc_dis_src_out(mod_id);   /* no hot switch of relays */
  vth_aopen_rly(mod_id, K845);   /* remove all load from ps */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_afunc_meas(mod_id);   /* measure the unloaded ps voltage */
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    vnoload = fabs(result.UU.Real_result);
  else
    display_prompt("No result in log(Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in log (Expected DET stat)", true);

  if (vnoload > 10.0 || vnoload < 4.0)
    vnoload = 7.0;
  vatten = ps_sense_voltage / vnoload;
  requiv = fabs(vload - vnoload) / meas_source_current;
  rseries = (SOURCE_VOLTAGE[POSITIVE] - detector_voltage - vload) /
            meas_source_current;

  vth_aclose_rly(mod_id, K845);   /* reconfigure load and differential */
  vth_aopen_rly(mod_id, K2304);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2306);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  for (int current = LOW; current <= HIGH; ++current)
  {  /* SETUP AND MEASURE FOR EACH CASE */
    if (current == LOW)
    {  /* low current */
      vth_asrc_en_30ma_ilim(mod_id);
      vth_adet_sel_rng(mod_id, R625);
    }  /* low current */
    else
    {  /* high current */
      vth_asrc_dis_30ma_ilim(mod_id);
      vth_adet_sel_rng(mod_id, R2500);
    }  /* high current */

    for (int source_polarity = POSITIVE; source_polarity <= NEGATIVE; ++source_polarity)
    {
      if (source_polarity == POSITIVE)
      {  /* CONNECT PS + TO X1 AND PS - TO X2 */
        vth_aopen_rly(mod_id, K726);
        vth_aopen_rly(mod_id, K717);
        vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K725);
        vth_aclose_rly(mod_id, K718);
      }  /* CONNECT PS + TO X1 AND PS - TO X2 */
      else
      {  /* CONNECT PS + TO X2 AND PS - TO X1 */
        vth_aopen_rly(mod_id, K725);
        vth_aopen_rly(mod_id, K718);
        vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K726);
        vth_aclose_rly(mod_id, K717);
      }  /* CONNECT PS + TO X2 AND PS - TO X1 */

      for (int direction = SOURCING; direction <= SINKING; ++direction)
      {  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */
        if (direction == SOURCING)
        {
          if (current == LOW)
            source_current = -1 * LOW_CURRENT_VALUE;   /* AMPS */
          else
            source_current = -1 * HIGH_CURRENT_VALUE;   /* AMPS */
        }
        else
        {  /* direction = sinking */
          if (current == LOW)
            source_current = LOW_CURRENT_VALUE;   /* AMPS */
          else
            source_current = HIGH_CURRENT_VALUE;   /* AMPS */
        }

        vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE[source_polarity]);

        ps_voltage = vatten * (SOURCE_VOLTAGE[source_polarity] +
                               source_current * (r10 + requiv + rseries));
        setup_ps(mod_id, ps_voltage, PS_CURRENT);
        verify_ps_output(mod_id, DIFF, RAMP_TIME, fabs(ps_voltage));
        vth_afc_en_src_out(mod_id);
        vth_mtmr_wait(mod_id, 0.001);
        vth_astat_clr(mod_id);
        vth_mtmr_wait(mod_id, 0.01);

        /* NOW CHECK THE SOURCE COMPLIANCE CURRENT LIMIT FOR NO LIMIT */
        vth_astat_rd(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          this_word.data = result.UU.Integer_result;
          /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  ... 15 */
          /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  ... 0  */
          /* THEREFORE 2^0  =  BIT NUMBER 15 */
//mjs -- another messed up bits problem
          source_i_compliance = P_getbits_US(this_word.bits, 0, 0);
          result.Measurement_class = CLASS_INTEGER;
          result.UU.Integer_result = source_i_compliance;
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected aim status)",
                         true);

        vth_afunc_meas(mod_id);
        vth_get_afunc_result(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          detector_voltage = result.UU.Real_result;
          meas_source_current = detector_voltage / r10;
          result.UU.Real_result = fabs(meas_source_current);
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected DET volts)", true);
        if (!get_result_nolog(mod_id, &result))
          display_prompt("No result in result log (Expected DET stat)", true);

        vth_afc_dis_src_out(mod_id);

      }

    }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */

  }  /* for CURRENT */

  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9275 */
#undef LOW
#undef HIGH
#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef R625
#undef R2500
#undef R10000
#undef RINIT
#undef RSINIT
#undef VINIT
#undef EXPECTED_DET_VOLTS
#undef TIMEOUT
#undef PS_CURRENT
#undef DIFF
#undef RAMP_TIME
#undef LOW_CURRENT_VALUE
#undef HIGH_CURRENT_VALUE

///////////////////added new test for NASRU T9274 equal to T9275 with looser test limit////////////

#define LOW             0
#define HIGH            1
#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1
typedef double source_voltage_array[2];
/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        10
/* FOR M.A.D. */
#define DIGITAL         0
/* 625mv detector range */
#define R625            4
/* 2.5v detector range */
#define R2500           6
/* 10v detector range */
#define R10000          8
/* initial requiv guess */
#define RINIT           4.0e+00
/* initial rseries guess */
#define RSINIT          0.5//NASRU Changed 0.6e+00
/* initial vatten guess */
#define VINIT           1.01e+00
#define EXPECTED_DET_VOLTS  2.0e+00
#define TIMEOUT         10.0e+00
#define PS_CURRENT      800e-03
#define DIFF            5e-03
#define RAMP_TIME       10e+00
#define LOW_CURRENT_VALUE  27.5e-03
#define HIGH_CURRENT_VALUE  165e-03
const source_voltage_array SOURCE_VOLTAGE_NASRU = {7e00, -7e00};
void t9274(int mod_id, int /*slot*/)
{
  /* DC SOURCE OUTPUT CURRENT TEST */
  int source_i_compliance;
  double r10;
  double detector_voltage;
  double ps_sense_voltage;
  double ps_voltage;
  double source_current;
  double meas_source_current;
  double vload;
  double vnoload;
  double vatten;
  double requiv;
  double rseries;

  test_result_type result;
  a_word_type this_word;


  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_ps(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, EXPECTED_DET_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);   /* DETLO to L bus relay */
  vth_aopen_rly(mod_id, K812);   /* DETHI to I bus relay */

  /* CLOSE THE SRCOUT TO P.S. POSITIVE  PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K826);   /* 10 ohm resistor to SRCOUT */

  /* CLOSE THE P.S. NEGATIVE  TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAYS */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */
  vth_aclose_rly(mod_id, K828);   /* DETHI input */

  /* MEASURE THE POWER SUPPLY ATTENUATION FACTOR AND THE EQUIVALENT RESISTANCE */
  requiv = RINIT;
  vatten = VINIT;
  rseries = RSINIT;
  vth_asrc_dis_30ma_ilim(mod_id);
  source_current = -1 * LOW_CURRENT_VALUE;   /* AMPS */
  ps_voltage = vatten * (SOURCE_VOLTAGE_NASRU[POSITIVE] +
                         source_current * (r10 + requiv + rseries));
  vth_aclose_rly(mod_id, K725);
  vth_aclose_rly(mod_id, K718);
  vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE_NASRU[POSITIVE]);
  vth_afc_en_src_out(mod_id);
  setup_ps(mod_id, ps_voltage, PS_CURRENT);
  verify_ps_output(mod_id, DIFF, RAMP_TIME, fabs(ps_voltage));
  vth_astat_clr(mod_id);

  vth_afunc_meas(mod_id);
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    detector_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in result log (Expected DET stat)", true);

  meas_source_current = fabs(detector_voltage / r10);
  if (meas_source_current > 1.5 * fabs(source_current) ||
      meas_source_current < 0.5 * fabs(source_current))
    meas_source_current = fabs(source_current);

  ps_sense_voltage = measure_ps_sense_voltage(mod_id);

  vth_adet_sel_rng(mod_id, R10000);
  vth_aopen_rly(mod_id, K2306);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2304);   /* reconfigure for grounded meas */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_afunc_meas(mod_id);   /* measure the loaded ps voltage */
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    vload = fabs(result.UU.Real_result);
  else
    display_prompt("No result in log(Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in log (Expected DET stat)", true);

  vth_afc_dis_src_out(mod_id);   /* no hot switch of relays */
  vth_aopen_rly(mod_id, K845);   /* remove all load from ps */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_afunc_meas(mod_id);   /* measure the unloaded ps voltage */
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    vnoload = fabs(result.UU.Real_result);
  else
    display_prompt("No result in log(Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in log (Expected DET stat)", true);

  if (vnoload > 10.0 || vnoload < 4.0)
    vnoload = 7.0;
  vatten = ps_sense_voltage / vnoload;
  requiv = fabs(vload - vnoload) / meas_source_current;
  rseries = (SOURCE_VOLTAGE_NASRU[POSITIVE] - detector_voltage - vload) /
            meas_source_current;

  vth_aclose_rly(mod_id, K845);   /* reconfigure load and differential */
  vth_aopen_rly(mod_id, K2304);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2306);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  for (int current = LOW; current <= HIGH; ++current)
  {  /* SETUP AND MEASURE FOR EACH CASE */
    if (current == LOW)
    {  /* low current */
      vth_asrc_en_30ma_ilim(mod_id);
      vth_adet_sel_rng(mod_id, R625);
    }  /* low current */
    else
    {  /* high current */
      vth_asrc_dis_30ma_ilim(mod_id);
      vth_adet_sel_rng(mod_id, R2500);
    }  /* high current */

    for (int source_polarity = POSITIVE; source_polarity <= NEGATIVE; ++source_polarity)
    {
      if (source_polarity == POSITIVE)
      {  /* CONNECT PS + TO X1 AND PS - TO X2 */
        vth_aopen_rly(mod_id, K726);
        vth_aopen_rly(mod_id, K717);
        vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K725);
        vth_aclose_rly(mod_id, K718);
      }  /* CONNECT PS + TO X1 AND PS - TO X2 */
      else
      {  /* CONNECT PS + TO X2 AND PS - TO X1 */
        vth_aopen_rly(mod_id, K725);
        vth_aopen_rly(mod_id, K718);
        vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K726);
        vth_aclose_rly(mod_id, K717);
      }  /* CONNECT PS + TO X2 AND PS - TO X1 */

      for (int direction = SOURCING; direction <= SINKING; ++direction)
      {  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */
        if (direction == SOURCING)
        {
          if (current == LOW)
            source_current = -1 * LOW_CURRENT_VALUE;   /* AMPS */
          else
            source_current = -1 * HIGH_CURRENT_VALUE;   /* AMPS */
        }
        else
        {  /* direction = sinking */
          if (current == LOW)
            source_current = LOW_CURRENT_VALUE;   /* AMPS */
          else
            source_current = HIGH_CURRENT_VALUE;   /* AMPS */
        }

        vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE_NASRU[source_polarity]);

        ps_voltage = vatten * (SOURCE_VOLTAGE_NASRU[source_polarity] +
                               source_current * (r10 + requiv + rseries));
        setup_ps(mod_id, ps_voltage, PS_CURRENT);
        verify_ps_output(mod_id, DIFF, RAMP_TIME, fabs(ps_voltage));
        vth_afc_en_src_out(mod_id);
        vth_mtmr_wait(mod_id, 0.001);
        vth_astat_clr(mod_id);
        vth_mtmr_wait(mod_id, 0.01);

        /* NOW CHECK THE SOURCE COMPLIANCE CURRENT LIMIT FOR NO LIMIT */
        vth_astat_rd(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          this_word.data = result.UU.Integer_result;
          /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  ... 15 */
          /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  ... 0  */
          /* THEREFORE 2^0  =  BIT NUMBER 15 */
//mjs -- another messed up bits problem
          source_i_compliance = P_getbits_US(this_word.bits, 0, 0);
          result.Measurement_class = CLASS_INTEGER;
          result.UU.Integer_result = source_i_compliance;
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected aim status)",
                         true);

        vth_afunc_meas(mod_id);
        vth_get_afunc_result(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          detector_voltage = result.UU.Real_result;
          meas_source_current = detector_voltage / r10;
          result.UU.Real_result = fabs(meas_source_current);
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected DET volts)", true);
        if (!get_result_nolog(mod_id, &result))
          display_prompt("No result in result log (Expected DET stat)", true);

        vth_afc_dis_src_out(mod_id);

      }

    }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */

  }  /* for CURRENT */

  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9274 */
#undef LOW
#undef HIGH
#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef R625
#undef R2500
#undef R10000
#undef RINIT
#undef RSINIT
#undef VINIT
#undef EXPECTED_DET_VOLTS
#undef TIMEOUT
#undef PS_CURRENT
#undef DIFF
#undef RAMP_TIME
#undef LOW_CURRENT_VALUE
#undef HIGH_CURRENT_VALUE


//////////////////////////////////////////// END OF TEST T9274/////////////////////////////////////


#define LOW             0
#define HIGH            1
#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1
typedef double i_type_array[2];
typedef i_type_array i_direction_array[2];
typedef i_direction_array voltage_array___[2];
/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        1
/* NOT IMPLEMENTED (FOR M.A.D.) */
#define DIGITAL         0
#define TIMEOUT         10.0e+00
#define POWER_SUPPLY_CURRENT  800e-03
#define POWER_SUPPLY_RAMP_TIME  1.0e+00
#define MAX_EXPECTED_VOLTS  2.1e+00

const voltage_array___ SOURCE_VOLTAGE_ =
{
  {
    {10.0e+00, 10.0e+00},
    { 9.0e+00,  6.0e+00}
  },
  {
    { -9.0e+00,  -6.0e+00},
    {-10.0e+00, -10.0e+00}
  }
};

const voltage_array___ PS_VOLTAGE =
{
  {
    { 9.0e+00,  6.0e+00},
    {10.0e+00, 10.0e+00}
  },
  {
    {-10.0e+00, -10.0e+00},
    { -9.0e+00,  -6.0e+00}
  }
};

void t9276(int mod_id, int /*slot*/)
{
  /* DC SOURCE MAXIMUM CURRENT */
  double r10;
  double max_current;

  test_result_type result;


  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_ps(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);   /* DETLO to L bus relay */
  vth_aopen_rly(mod_id, K812);   /* DETHI to I bus relay */

  /* CLOSE THE SRCOUT TO P.S. PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K826);   /* 10 ohm resistor to SRCOUT */

  /* CLOSE THE P.S. TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAYS */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */
  vth_aclose_rly(mod_id, K828);   /* DETHI input */

  for (int current = LOW; current <= HIGH; ++current)
  {  /* SETUP FOR LOW AND HIGH CURRENT MAXIMUMS */
    if (current == LOW)
      vth_asrc_en_30ma_ilim(mod_id);
    else
      vth_asrc_dis_30ma_ilim(mod_id);

    for (int src_polarity = POSITIVE; src_polarity <= NEGATIVE; ++src_polarity)
    {  /* SETUP FOR POSITIVE AND NEGATIVE SOURCE VOLTAGES */
      if (src_polarity == POSITIVE)
      {  /* CONNECT PS + TO X1 AND PS - TO X2 */
        vth_aopen_rly(mod_id, K726);
        vth_aopen_rly(mod_id, K717);
        vth_aclose_rly(mod_id, K725);
        vth_aclose_rly(mod_id, K718);
      }  /* CONNECT PS + TO X1 AND PS - TO X2 */
      else
      {  /* CONNECT PS + TO X2 AND PS - TO X1 */
        vth_aopen_rly(mod_id, K725);
        vth_aopen_rly(mod_id, K718);
        vth_aclose_rly(mod_id, K726);
        vth_aclose_rly(mod_id, K717);
      }  /* CONNECT PS + TO X2 AND PS - TO X1 */

      for (int direction = SOURCING; direction <= SINKING; ++direction)
      {  /* SETUP FOR SOURCING AND SINKING CONDITIONS */
        vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE_[src_polarity][direction]
                         [current]);
        vth_afc_en_src_out(mod_id);

        setup_ps(mod_id, PS_VOLTAGE[src_polarity][direction]
                 [current], POWER_SUPPLY_CURRENT);
        vth_mtmr_wait(mod_id, POWER_SUPPLY_RAMP_TIME);

        vth_afunc_meas(mod_id);
        vth_get_afunc_result(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          max_current = result.UU.Real_result / r10;
          result.UU.Real_result = fabs(max_current);
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected DET volts)", true);

        if (!get_result_nolog(mod_id, &result))
          display_prompt("No result in result log (Expected DET stat)", true);

        vth_afc_dis_src_out(mod_id);

      }


    }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */

  }  /* for CURRENT */

  vth_ainit(mod_id);

  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9276 */
#undef LOW
#undef HIGH
#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef TIMEOUT
#undef POWER_SUPPLY_CURRENT
#undef POWER_SUPPLY_RAMP_TIME
#undef MAX_EXPECTED_VOLTS

#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1

typedef double i_direction_array_[2];
typedef i_direction_array_ voltage_array____[2];
/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        1
/* NOT IMPLEMENTED (FOR M.A.D.) */
#define DIGITAL         0
#define TIMEOUT         10.0e+00
#define POWER_SUPPLY_CURRENT  800e-03
#define POWER_SUPPLY_RAMP_TIME  1.0e+00
#define MAX_EXPECTED_VOLTS  2.1e+00

const voltage_array____ SOURCE_VOLTAGE__ =
{
  {10.0e+00,   6.0e+00},
  {-6.0e+00, -10.0e+00}
};

const voltage_array____ PS_VOLTAGE_ =
{
  { 6.0e+00,  10.0e+00},
  {-10.0e+00, -6.0e+00}
};

void t9277(int mod_id, int /*slot*/)
{
  /* DC SOURCE MAXIMUM CURRENT (HIGH LEVEL ONLY) */
  double r10;
  double max_current;

  test_result_type result;


  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_ps(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);   /* DETLO to L bus relay */
  vth_aopen_rly(mod_id, K812);   /* DETHI to I bus relay */

  /* CLOSE THE SRCOUT TO P.S. PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K826);   /* 10 ohm resistor to SRCOUT */

  /* CLOSE THE P.S. TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAYS */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */
  vth_aclose_rly(mod_id, K828);   /* DETHI input */

  vth_asrc_dis_30ma_ilim(mod_id);

  for (int src_polarity = POSITIVE; src_polarity <= NEGATIVE; ++src_polarity)
  {  /* SETUP FOR POSITIVE AND NEGATIVE SOURCE VOLTAGES */
    if (src_polarity == POSITIVE)
    {  /* CONNECT PS + TO X1 AND PS - TO X2 */
      vth_aopen_rly(mod_id, K726);
      vth_aopen_rly(mod_id, K717);
      vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K725);
      vth_aclose_rly(mod_id, K718);
    }  /* CONNECT PS + TO X1 AND PS - TO X2 */
    else
    {  /* CONNECT PS + TO X2 AND PS - TO X1 */
      vth_aopen_rly(mod_id, K725);
      vth_aopen_rly(mod_id, K718);
      vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K726);
      vth_aclose_rly(mod_id, K717);
    }  /* CONNECT PS + TO X2 AND PS - TO X1 */

    for (int direction = SOURCING; direction <= SINKING; ++direction)
    {  /* SETUP FOR SOURCING AND SINKING CONDITIONS */
      vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE__[src_polarity][direction]);
      vth_afc_en_src_out(mod_id);

      setup_ps(mod_id, PS_VOLTAGE_[src_polarity]
               [direction], POWER_SUPPLY_CURRENT);
      vth_mtmr_wait(mod_id, POWER_SUPPLY_RAMP_TIME);

      vth_afunc_meas(mod_id);
      vth_get_afunc_result(mod_id);
      if (get_result_nolog(mod_id, &result))
      {
        max_current = result.UU.Real_result / r10;
        result.UU.Real_result = fabs(max_current);
        log_result(mod_id, result);
      }
      else
        display_prompt("No result in result log (Expected DET volts)", true);

      if (!get_result_nolog(mod_id, &result))
        display_prompt("No result in result log (Expected DET stat)", true);

      vth_afc_dis_src_out(mod_id);

    }  /* SETUP FOR SOURCING AND SINKING CONDITIONS */


  }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */


  vth_ainit(mod_id);

  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9277 */
#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE
#undef WHAT_STRING_
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef TIMEOUT
#undef POWER_SUPPLY_CURRENT
#undef POWER_SUPPLY_RAMP_TIME
#undef MAX_EXPECTED_VOLTS


//################# added test 9300(Read) and 9301(Write) for DH-SUB SN operations ############################


/*
********************************************************************************
*
* File:         EXISTING
* Author:       Sri Tharan
* Description:  SALES tests for the SUB
* Exceptions:
* Notes:        This file contain tests for Utility card (SUB) At the point of writing,
*               relays definition are declared in this file due to "mod" linking problem.
*
*Last Update:   29TH FEB 2008
*  
*
* (c) Copyright 2007, Agilent Technologies, all rights reserved.
*
********************************************************************************
*/




/*
***************************************************************************
* Procedure:   9300
* Card:        SUB
* Requires:    None
* Author:      Sri Tharan
* Last update: 29th Feb 2008
* Notes:       Retrieve card serial number
*
* (c) Copyright 2007, Agilent Technologies, all rights reserved.
***************************************************************************
*/

void t9300(int mod_id, int slot)
{
  test_result_type result;
  char cSerial[NUM_UTIL_SID+1];
  s80 msg;  

  vth_read_utility_sid(mod_id, slot);
  for (int nI=1; nI<NUM_UTIL_SID; nI++)
  {
    get_result_nolog(mod_id, &result);
    itoa(result.UU.Integer_result, &cSerial[nI], 10);
  }
  get_result_nolog(mod_id, &result);
  cSerial[0] = (char)result.UU.Integer_result;
  cSerial[NUM_UTIL_SID] = NULL;
  sprintf(msg,"Utility serial number is %s",cSerial);
  display_prompt(msg, true);
  display_prompt("", true);
}


/*
***************************************************************************
* Procedure:   T9301
* Card:        SUB
* Requires:    None
* Author:      Sri Tharan
* Created on : 29th Feb 2008
* Last Update: 29th Feb 2008
* Notes:       Write card serial number
*
* (c) Copyright 2007, Agilent Technologies, all rights reserved.
***************************************************************************
*/


void t9301(int mod_id, int slot)
{
  //test_result_type result;
  //char cSerial[NUM_UTIL_SID+1];
  //char cMsg[MSG_SIZE]; 
  
 
  s80 buffer = "";
  for (int i = 0; i <= 5; ++i)
    display_prompt("", true);

  display_prompt("SALES TEST 9301--STORE UTILITY CARD SERIAL # (AND REV CODE).", true);
  display_prompt("", true);

  display_prompt("THIS TEST WILL MODIFY THE NON-VOLATILE MEMORY", true);
  sprintf(buffer, "OF THE UTILITY CARD IN MODULE %d.", mod_id);
  display_prompt(buffer, true);
  display_prompt("", true);
  display_prompt("  Do you want to continue? (yes or no): ", false);
  *buffer = '\0';
  get_user_answer(buffer, 80);
  display_prompt("", true);

  if (! strcmp(buffer, "yes"))
  {
    s80 sn1 = "";
    s80 sn2 = "";
    bool serial_good = false;

    display_prompt("  This routine stores the card serial number(and revision", true);
    display_prompt("    code) of the Utility card in its non-volatile memory. ", true);
    display_prompt("", true);

    for (int l = 0; l < 3; ++l)
    {
      display_prompt("  Enter the serial number (1 character + 26 numbers max):  ", false);
      get_user_answer(sn1, 80);
      display_prompt("  Re-enter the serial number:  ", false);
      get_user_answer(sn2, 80);

      serial_good = (strcmp(sn1, sn2) == 0);

      if (serial_good)  break;

      display_prompt("  The numbers don't match, try again.", true);
    }

    if (serial_good)
    {
      display_prompt("", true);
      sprintf(buffer, "  THE SERIAL NUMBER IS %11s.", sn1);
      display_prompt(buffer, true);
      int sn_length = mtd_strlen(sn1);

      if (sn_length > 27)
      {
        display_prompt("  The serial number has been truncated to 27 characters.", true);
        sn_length = 27;
      }

	  vth_program_utility_sid(mod_id, slot,(int)sn1[1],(int)sn1[2],(int)sn1[3],(int)sn1[4],(int)sn1[5],
										(int)sn1[6],(int)sn1[7],(int)sn1[8],(int)sn1[9],(int)sn1[10],
										(int)sn1[11],(int)sn1[12],(int)sn1[13],(int)sn1[14],(int)sn1[15],
										(int)sn1[16],(int)sn1[17],(int)sn1[18],sn1[19],(int)sn1[20],
										(int)sn1[21],(int)sn1[22],(int)sn1[23],(int)sn1[24],(int)sn1[25],
										(int)sn1[26],(int)sn1[0]);
      
      display_prompt("  The serial number has been stored.", true);
    }
    else
      display_prompt("  The serial numbers never matched.  No serial number was stored.", true);

    display_prompt("", true);

	}
  else
    display_prompt("TEST TERMINATED AT USER'S REQUEST.", true);
}



//##############################################################################################################


#define NUM_POINTS_TO_TEST  5
#define CHANNEL         'A'
typedef double voltage_array_____[NUM_POINTS_TO_TEST];
#define RESOLUTION      1.0e-03
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03
/* seconds */
#define ENABLE_WAIT_TIME  1.0e-03
const voltage_array_____ TEST__ = {-10e00, -5e00, 0.0e00, 5e00, 10e00};
#define DMM_FUNCTION    DCV
void t9305(int mod_id, int /*slot*/)
{
  /* AUX SOURCE VOLTAGE ACCURACY AND RANGE */
  int voltage;

  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  connect_dmm(mod_id);
  connect_plus_xn(mod_id, CHANNEL, true);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K823);   /* AUXOUT to A bus relay */
  vth_aclose_rly(mod_id, K709);   /* A bus to X1 bus relay */
  vth_aclose_rly(mod_id, K741);   /* X1 bus to mint pin relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  vth_aaux_en(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  for (voltage = 0; voltage < NUM_POINTS_TO_TEST; voltage++)
  {  /* for voltage */
    setup_dmm(mod_id, DMM_FUNCTION, TEST__[voltage], RESOLUTION);
    vth_aaux_set_ampl(mod_id, TEST__[voltage]);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    measure_dmm(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected dmm vth_reading)", true);

  }  /* for voltage */

  vth_aaux_dis(mod_id);

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9305 */
#undef NUM_POINTS_TO_TEST
#undef CHANNEL
#undef RESOLUTION
#undef SOURCE_WAIT_TIME
#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION

#define HI_END          0
#define LO_END          1
typedef double voltage_array______[2];
typedef int headroom_array_[2];
#define RESOLUTION      1.0e-03
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03
/* seconds */
#define ENABLE_WAIT_TIME  1.0e-03
const voltage_array______ TEST___ = {10e00, -10e00};
const headroom_array_ HEADROOM_ = {4095, 0};
#define CHANNEL         'A'
#define DMM_FUNCTION    DCV
void t9306(int mod_id, int /*slot*/)
{
  /* AUX SOURCE VOLTAGE HEADROOM TEST */
  test_result_type result;

  open_dmm(mod_id);
  open_mux(mod_id);

  connect_dmm(mod_id);
  connect_plus_xn(mod_id, CHANNEL, true);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K823);   /* AUXOUT to A bus relay */
  vth_aclose_rly(mod_id, K709);   /* A bus to X1 bus relay */
  vth_aclose_rly(mod_id, K741);   /* X1 bus to mint pin relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  vth_aaux_en(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  for (int src_30ma_ilim = 0; src_30ma_ilim <= 1; ++src_30ma_ilim)
  {
    if (src_30ma_ilim == 1)
      vth_aaux_en_30ma_ilim(mod_id);
    else
      vth_aaux_dis_30ma_ilim(mod_id);

    for (int voltage = HI_END; voltage <= LO_END; ++voltage)
    {  /* for voltage */
      setup_dmm(mod_id, DMM_FUNCTION, TEST___[voltage], RESOLUTION);
      /* SETUP THE AUX SOURCE TO OUTPUT MAXIMUM PROGRAMMABLE VALUE */
      vth_aaux_set_ampl(mod_id, TEST___[voltage]);
      /* SETUP THE AUX SOURCE TO OUTPUT THE MAXIMUM VOLTAGE (FULL SCALE) */
      vth_awr_cell(mod_id, ADAC_AUX, HEADROOM_[voltage]);
      vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
      measure_dmm(mod_id);
      if (!get_result_log(mod_id, &result))
        display_prompt("No result in result log (Expected dmm vth_reading)", true);
    }  /* for voltage */
  }

  vth_aaux_dis(mod_id);

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9306 */
#undef HI_END
#undef LO_END
#undef RESOLUTION
#undef SOURCE_WAIT_TIME
#undef ENABLE_WAIT_TIME
#undef CHANNEL
#undef DMM_FUNCTION

#define CHANNEL         'A'
#define AUX_DEFAULT_RESISTANCE  0
/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* unenhanced */
#define MEAS_TYPE       0
#define POSITIVE_TEST_VOLTAGE  100e-03
#define NEGATIVE_TEST_VOLTAGE  (-100e-03)
#define RESOLUTION      1.0e-03
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03
/* seconds */
#define ENABLE_WAIT_TIME  1.0e-03
#define DMM_FUNCTION    DCV
void t9310(int mod_id, int /*slot*/)
{
  /* AUX SOURCE OUTPUT RESISTANCE */
  double r10;
  double dmm_no_load_voltage;
  double dmm_loaded_voltage;
  double source_resistance;

  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  setup_dmm(mod_id, DMM_FUNCTION, POSITIVE_TEST_VOLTAGE, RESOLUTION);

  connect_dmm(mod_id);
  connect_plus_xn(mod_id, CHANNEL, true);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K823);   /* AUXOUT to A bus relay */
  vth_aclose_rly(mod_id, K709);   /* A bus to X1 bus relay */
  vth_aclose_rly(mod_id, K741);   /* X1 bus to mint pin relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  vth_aaux_en(mod_id);
  vth_aaux_set_ampl(mod_id, POSITIVE_TEST_VOLTAGE);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
    dmm_no_load_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);
  vth_aaux_dis(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  vth_aclose_rly(mod_id, K822);
  vth_aclose_rly(mod_id, K828);
  vth_aclose_rly(mod_id, K845);
  vth_aclose_rly(mod_id, K855);

  vth_aaux_en(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*measure the loaded voltage, calculate resistance and log*/
    dmm_loaded_voltage = result.UU.Real_result;
    if (dmm_loaded_voltage != 0)
      source_resistance =
        (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
    else
      source_resistance = AUX_DEFAULT_RESISTANCE;
    result.UU.Real_result = source_resistance;
    log_result(mod_id, result);
  }  /*measure the loaded voltage, calculate resistance and log*/
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_aaux_set_ampl(mod_id, NEGATIVE_TEST_VOLTAGE);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
    dmm_loaded_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);
  vth_aaux_dis(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  vth_aopen_rly(mod_id, K822);
  vth_aopen_rly(mod_id, K828);
  vth_aopen_rly(mod_id, K845);
  vth_aopen_rly(mod_id, K855);

  vth_aaux_en(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*measure the unloaded voltage, calculate resistance and log*/
    dmm_no_load_voltage = result.UU.Real_result;
    if (dmm_loaded_voltage != 0)
      source_resistance =
        (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
    else
      source_resistance = AUX_DEFAULT_RESISTANCE;
    result.UU.Real_result = source_resistance;
    log_result(mod_id, result);
  }  /*measure the loaded voltage, calculate resistance and log*/
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_aaux_dis(mod_id);

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9310 */
#undef CHANNEL
#undef AUX_DEFAULT_RESISTANCE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef POSITIVE_TEST_VOLTAGE
#undef NEGATIVE_TEST_VOLTAGE
#undef RESOLUTION
#undef SOURCE_WAIT_TIME
#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION

#define MIN_TEST_VOLTAGE  10.0e+00
#define MAX_TEST_VOLTAGE  20.0e+00
#define STARTING_INCREMENT  1024e-03
#define OLYM_FINEST_INCREMENT  10e-03	//added by chuanho, 14jun2012
#define FINEST_INCREMENT  8e-03
#define SYN_WAIT_TIME   10e-03 
#define OLYM_WAIT_TIME   150e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_CURR  500e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_OCP   1e+00 //added by chuanho, 14jun2012
#define RESOLUTION      1e-03

/* static variables for t9315: */
struct LOC_t9315
{
  int mod_id;
};

static void close_the_relays__(struct LOC_t9315 *LINK)
{
  /* CLOSE THE AUXOUT TO SYN. POSITIVE  PATH RELAYS */
  vth_aclose_rly(LINK->mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(LINK->mod_id, K709);   /* X1 bus to A bus relay */
  vth_aclose_rly(LINK->mod_id, K823);   /* A bus to AUXOUT relay */

  /* CLOSE THE P.S. NEGATIVE  TO GROUND PATH RELAYS */
  /* ------------------------------------------------------------------ */
  /* Modified: 01.21.91 JRM ------------------------------------------- */
  /* Changed ground path from K742, K726, K813, K2305, K2303 ---------- */
  /* ------------------------------------------------------------------ */
  vth_aclose_rly(LINK->mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(LINK->mod_id, K718);   /* X2 bus to L bus relay */
  vth_aclose_rly(LINK->mod_id, K857);   /* L bus to GROUND relay */
}

void t9315(int mod_id_, int /*slot*/)
{
  /* AUX SOURCE POSITIVE OVER VOLTAGE LIMIT ACCURACY */
  struct LOC_t9315 V;

  boolean v_limit;
  /* ADDED ever_had_a_vlimit TO KEEP FROM RETRYING THE SAME VALUES*/
  /*01.07.91*/
  boolean ever_had_a_vlimit;

  double syn_voltage; 
  double olym_ch1_voltage; //added by chuanho, 14jun2012
  double voltage_increment;

  int over_voltage_bit;
  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id_;
  OpenResourceManager();
  open_mux(V.mod_id);
  open_dmm(V.mod_id);
  open_33250A(V.mod_id);

  /*New Olympus codes*/
  //Added by chuanho, 14jun2012
  if(check_syn_model())
  {
  open_olym(V.mod_id); 
  connect_olym_ch1(V.mod_id); 
  aconnect_olym_ch1_abus1_xmint(V.mod_id);

  /* SETUP TO DO POSITIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE DC SOURCE */
  vth_aaux_dis(V.mod_id);

  olym_ch1_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*01.07.91*/
  close_the_relays__(&V);
  setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
  setup_olym_ch1_curr(V.mod_id, OLYM_CH1_CURR); 
  setup_olym_ch1_ocp(V.mod_id, OLYM_CH1_OCP);
  enable_olym_ch1_out(V.mod_id);
  vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME);
  vth_astat_clr(V.mod_id);

  while (!v_limit && olym_ch1_voltage <= MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^9  =  BIT NUMBER 6 */
//mjs -- another messed up bit problem
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= OLYM_FINEST_INCREMENT || olym_ch1_voltage == MIN_TEST_VOLTAGE)
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*01.07.91*/
      v_limit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*01.07.91*/
      disable_olym_ch1_out(V.mod_id); 
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays__(&V);
      voltage_increment /= 2;
      olym_ch1_voltage -= voltage_increment; 

      setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
      enable_olym_ch1_out(V.mod_id);
      /* ADDED SYN WAIT TIME */
      /*01.07.90*/
      vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME);
      continue;
    }  /* Go back to the last voltage and reduce the increment */
//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, OLYM_FINEST_INCREMENT) )
      voltage_increment /= 2;
    olym_ch1_voltage += voltage_increment;
    setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
	vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
  }  /*search for limit level*/



  /* MEASURE THE OLYMPUS VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, olym_ch1_voltage, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_olym_ch1_out_dmm(V.mod_id, true);
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);
   
  vth_ainit(V.mod_id);
  disable_olym_ch1_out(V.mod_id);
  close_olym(V.mod_id);
  close_33250A(V.mod_id);
  }
  /*End of new Olympus codes*/

  else
  {
  open_syn(V.mod_id); 
  setup_syn_hv(V.mod_id, true); 

  aconnect_syn_out_xmint(V.mod_id); 

  /* SETUP TO DO POSITIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE DC SOURCE */
  vth_aaux_dis(V.mod_id);

  syn_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*01.07.91*/
  close_the_relays__(&V);
  setup_syn_off(V.mod_id, syn_voltage); 
  vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
  vth_astat_clr(V.mod_id);

  while (!v_limit && syn_voltage <= MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^9  =  BIT NUMBER 6 */
//mjs -- another messed up bit problem
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT ||
          syn_voltage == MIN_TEST_VOLTAGE) 
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*01.07.91*/
      v_limit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*01.07.91*/
      setup_syn_off(V.mod_id, 0.0); 
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays__(&V);
      voltage_increment /= 2;
      syn_voltage -= voltage_increment; 

      setup_syn_off(V.mod_id, syn_voltage);
      /* ADDED SYN WAIT TIME */
      /*01.07.90*/
      vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
      continue;
    }  /* Go back to the last voltage and reduce the increment */
//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    syn_voltage += voltage_increment; 
    setup_syn_off(V.mod_id, syn_voltage); 
    vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, syn_voltage, RESOLUTION); 
  connect_dmm(V.mod_id);
  aconnect_syn_out_dmm(V.mod_id, true); 
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_ainit(V.mod_id);
  close_syn(V.mod_id);
  }

  close_dmm(V.mod_id);
  close_mux(V.mod_id);
  CloseResourceManager();


  /* Go to the next increment */
  /* Go to the next increment */
}  /* t9315 */
#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef SYN_WAIT_TIME 
#undef OLYM_FINEST_INCREMENT	//added by chuanho, 14jun2012
#undef OLYM_WAIT_TIME			//added by chuanho, 14jun2012
#undef OLYM_CH1_CURR			//added by chuanho, 14jun2012
#undef OLYM_CH1_OCP			//added by chuanho, 14jun2012
#undef RESOLUTION

#define MIN_TEST_VOLTAGE  (-10.0e+00)
#define MAX_TEST_VOLTAGE  (-20.0e+00)
#define STARTING_INCREMENT  1024e-03
#define OLYM_MIN_TEST_VOLTAGE  (10.0e+00) //added by chuanho, 14jun2012
#define OLYM_MAX_TEST_VOLTAGE  (20.0e+00) //added by chuanho, 14jun2012
#define FINEST_INCREMENT  8e-03
#define OLYM_FINEST_INCREMENT  10e-03//added by chuanho, 14jun2012
#define SYN_WAIT_TIME   10e-03 
#define OLYM_WAIT_TIME 150e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_CURR  500e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_OCP  1e+00 //added by chuanho, 14jun2012
#define RESOLUTION      1e-03

/* static variables for t9316: */
struct LOC_t9316
{
  int mod_id;
} ;

static void close_the_relays___(struct LOC_t9316 *LINK)
{
  /* CLOSE THE AUXOUT TO SYN. POSITIVE  PATH RELAYS */
  vth_aclose_rly(LINK->mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(LINK->mod_id, K709);   /* X1 bus to A bus relay */
  vth_aclose_rly(LINK->mod_id, K823);   /* A bus to AUXOUT relay */

  /* CLOSE THE P.S. NEGATIVE  TO GROUND PATH RELAYS */
  /* ------------------------------------------------------------------ */
  /* Modified: 01.21.91 JRM ------------------------------------------- */
  /* Changed ground path from K742, K726, K813, K2305, K2303 ---------- */
  /* ------------------------------------------------------------------ */
  vth_aclose_rly(LINK->mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(LINK->mod_id, K718);   /* X2 bus to L bus relay */
  vth_aclose_rly(LINK->mod_id, K857);   /* L bus to GROUND relay */
}

void t9316(int mod_id_, int /*slot*/)
{
  /* AUX SOURCE NEGATIVE OVER VOLTAGE LIMIT ACCURACY */
  struct LOC_t9316 V;

  boolean v_limit;
  /* ADDED ever_had_a_vlimit TO KEEP FROM RETRYING THE SAME VALUES*/
  /*01.07.91*/
  boolean ever_had_a_vlimit;

  double syn_voltage; 
  double olym_ch1_voltage; //added by chuanho, 13jul2011
  double voltage_increment;

  int over_voltage_bit;
  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id_;

  OpenResourceManager();
  open_dmm(V.mod_id);
  open_mux(V.mod_id);
  open_33250A(V.mod_id);

  /*New Olympus codes*/
  //Added by chuanho, 14jun2012
  if(check_syn_model())
  {
  open_olym(V.mod_id);
  connect_olym_ch1(V.mod_id); 
  aconnect_olym_ch1_abus1_xmint(V.mod_id);

  /* SETUP TO DO NEGATIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE DC SOURCE */
  vth_aaux_dis(V.mod_id);

  olym_ch1_voltage = OLYM_MIN_TEST_VOLTAGE; 
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*01.07.91*/
  close_the_relays___(&V);
  setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
  setup_olym_ch1_curr(V.mod_id, OLYM_CH1_CURR); 
  setup_olym_ch1_ocp(V.mod_id, OLYM_CH1_OCP); 
  enable_olym_ch1_out(V.mod_id); 
  reverse_olym_ch1_out(V.mod_id); 
  vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME);
  vth_astat_clr(V.mod_id);

  while (!v_limit && olym_ch1_voltage <= OLYM_MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^9  =  BIT NUMBER 6 */
//mjs -- another messed up bit problem
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= OLYM_FINEST_INCREMENT || olym_ch1_voltage == OLYM_MIN_TEST_VOLTAGE)
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*01.07.91*/
      v_limit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*01.07.91*/
      
	  disable_olym_ch1_out(V.mod_id); 
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays___(&V);
      voltage_increment /= 2;
      olym_ch1_voltage -= voltage_increment;

      setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
      enable_olym_ch1_out(V.mod_id);
      

      /* ADDED SYN WAIT TIME */
      /*01.07.90*/
      vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
      continue;
    }  /* Go back to the last voltage and reduce the increment */
//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, OLYM_FINEST_INCREMENT) )
      voltage_increment /= 2;
    olym_ch1_voltage += voltage_increment; 
	setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
    vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  //setup_dmm(V.mod_id, DCV, syn_voltage, RESOLUTION); remarked by chuanho, 14jul2011
  setup_dmm(V.mod_id, DCV, olym_ch1_voltage, RESOLUTION); //added by chuanho, 14jul2011
  connect_dmm(V.mod_id);
  aconnect_olym_ch1_out_dmm(V.mod_id, true); 
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  disable_olym_ch1_out(V.mod_id);
  vth_ainit(V.mod_id);
  close_olym(V.mod_id);
  close_33250A(V.mod_id);
  }
  /* Go to the next increment */
  /* Go to the next increment */

  else
  {
  open_syn(V.mod_id); 
  setup_syn_hv(V.mod_id, true); 
  aconnect_syn_out_xmint(V.mod_id); 

  /* SETUP TO DO NEGATIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE DC SOURCE */
  vth_aaux_dis(V.mod_id);

  syn_voltage = MIN_TEST_VOLTAGE; 
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*01.07.91*/
  close_the_relays___(&V);
  setup_syn_off(V.mod_id, syn_voltage); 
  vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
  vth_astat_clr(V.mod_id);

  while (!v_limit && syn_voltage >= MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^9  =  BIT NUMBER 6 */
//mjs -- another messed up bit problem
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT ||
          syn_voltage == MIN_TEST_VOLTAGE) 
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*01.07.91*/
      v_limit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*01.07.91*/
      setup_syn_off(V.mod_id, 0.0); 
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays___(&V);
      voltage_increment /= 2;
      syn_voltage += voltage_increment; 
      setup_syn_off(V.mod_id, syn_voltage); 

      /* ADDED SYN WAIT TIME */
      /*01.07.90*/
      vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
      continue;
    }  /* Go back to the last voltage and reduce the increment */
//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    syn_voltage -= voltage_increment;
	setup_syn_off(V.mod_id, syn_voltage);
    vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, syn_voltage, RESOLUTION); 
  connect_dmm(V.mod_id);
  aconnect_syn_out_dmm(V.mod_id, true); 
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_ainit(V.mod_id);
  close_syn(V.mod_id);
  }

  close_dmm(V.mod_id);
  close_mux(V.mod_id);
  CloseResourceManager();

}  /* t9316 */
#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef OLYM_MIN_TEST_VOLTAGE	//added by chuanho, 14jun2012
#undef OLYM_MAX_TEST_VOLTAGE	//added by chuanho, 14jun2012
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef OLYM_FINEST_INCREMENT	//added by chuanho, 14jun2012
#undef SYN_WAIT_TIME 
#undef OLYM_WAIT_TIME //added by chuanho, 14jun2012
#undef OLYM_CH1_CURR //added by chuanho, 14jun2012
#undef OLYM_CH1_OCP //added by chuanho, 14jun2012
#undef RESOLUTION

/* Where to Measure voltage */
/* Integer -1 = LEFT */
#define INTERNAL_SRC_VOLTAGE  10e+00
#define EXTERNAL_SRC_VOLTAGE  20e+00
#define OLYM_CH1_CURR		  500e-03	//added by chuanho, 14jun2012
#define OLYM_CH1_OCP		  1e+00  	//added by chuanho, 14jun2012
#define OLYM_WAIT_TIME		  150e-03	//added by chuanho, 14jun2012

void t9317(int mod_id, int /*slot*/)
{
  /* AUX SOURCE POSITIVE OVER VOLTAGE PROTECTION */
  int over_voltage_bit;
  int fatal_error_bit;
  a_word_type this_word;
  test_result_type result;
  arrform waveform;
  arrform waveform2;
  int i;
  double max_volt;
  double average;
  int num_samples;
  int num_samples2;

  OpenResourceManager();
  open_dmm50(mod_id);
  open_mux(mod_id);
  open_33250A(mod_id);
  
  /*New Olympus codes*/
  //Added by chuanho, 14jun2012
  if(check_syn_model())
  {
  open_olym(mod_id);
  vth_aaux_dis(mod_id);
  connect_olym_ch1(mod_id);
  aconnect_olym_ch1_abus1_xmint(mod_id);
  connect_dmm50_out_af2(mod_id);

  // SETUP TO DO OVERVOLTAGE MEASUREMENT

  //close the auxout to synthesizer path relays
  vth_aclose_rly(mod_id, K741);   // X1MINT to X1 bus relay
  vth_aclose_rly(mod_id, K725);   // X1 bus to I bus relay
  vth_aclose_rly(mod_id, K846);   // 100 ohm load relay to REFR2
  vth_aclose_rly(mod_id, K828);   // REFR2 to DETHI relay
  vth_aclose_rly(mod_id, K822);   // DETHI to A bus relay
  vth_aclose_rly(mod_id, K823);   // A bus to AUXOUT relay

  // close the af2 measurement path relays
  vth_aclose_rly(mod_id, K808);   // DETHI to S bus relay
  vth_aclose_rly(mod_id, K751);   // S bus to A_XL bus relay
  vth_aclose_rly(mod_id, K764);   // A_XL bus to AF2 relay

  // close the synthesizer to ground path relays
  vth_aclose_rly(mod_id, K742);   // X2MINT to X2 bus relay
  vth_aclose_rly(mod_id, K718);   // X2 bus to L bus relay
  vth_aclose_rly(mod_id, K857);   // L bus to GROUND path relay

  /* Since the OLYMPUS cannot be tri-stated, set it to 10v before setting
     up the AUX source on the ASRU card.  */
  
  flush_commands(mod_id);
  setup_olym_ch1_volt(mod_id, INTERNAL_SRC_VOLTAGE);
  setup_olym_ch1_curr(mod_id, OLYM_CH1_CURR); 
  setup_olym_ch1_ocp(mod_id, OLYM_CH1_OCP);
  enable_olym_ch1_out(mod_id); 
  vth_mtmr_wait(mod_id, OLYM_WAIT_TIME); 

  vth_aaux_en_30ma_ilim(mod_id);
  vth_aaux_set_ampl(mod_id, INTERNAL_SRC_VOLTAGE);
  vth_aaux_en(mod_id);

  flush_commands(mod_id);

  num_samples = 2300;  // number of samples to make with the dmm
  // get the dmm ready to catch the voltage on its way from 10v to 20v
  setup_dmm_digitizing(mod_id, DMM50_ADDRESS, 100, 20E-6, num_samples, 1.4E-6, 1E-3, 13, POS);

  flush_commands(mod_id);

  setup_olym_ch1_volt(mod_id, EXTERNAL_SRC_VOLTAGE);
  wait_for_dmm50_measurement(mod_id);  // wait for the dmm to see the trigger

  vth_astat_rd(mod_id); // check aim status to make sure the voltage limit has occurred
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0); // bit 9 of the word is OV flag bit
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = over_voltage_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  // collect the voltage samples measured by the dmm and put into waveform array
  get_dmm_volt(mod_id, DMM50_ADDRESS, num_samples, waveform, 0);

  // find the abs value max voltage reached
  max_volt = waveform[0];
  for(i= 1; i < num_samples; i++)
  {
    if (max_volt < waveform[i])
      max_volt = waveform[i];
  }

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = max_volt;
  log_result(mod_id, result);

  // make sure that the aim card set the fatal error status bit
  vth_mrd_cell(mod_id, MMFP_GPIO);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    fatal_error_bit = P_getbits_US(this_word.bits, 6, 0); // bit 6 indicates if fatal error set
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = fatal_error_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MAKE SURE THAT THE RELAYS OPENED - CHECK MINIMUM VOLTAGE AT SCOPE */

  /* MAKE SURE THAT THE SOURCE COMES BACK UP - CHECK VOLTAGE AT SCOPE */

  vth_mhpib_wr(mod_id, 8, "PRESET NORM");

  // average the last 5 samples to find out what voltage the card settled at
  num_samples2 = 5;
  average = 0;
  for (i=num_samples-5; i<num_samples; i++)
    average += waveform[i];

  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_astat_clr(mod_id);
  /* close the AF2 measurement path relays */
  vth_aclose_rly(mod_id, K823);   /* AUXOUT to A bus relay */
  vth_aclose_rly(mod_id, K822);   /* A bus to DETHI relay */
  vth_aclose_rly(mod_id, K808);   /* DETHI to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL bus to AF2 relay */

  vth_mtmr_wait(mod_id, .5);
  num_samples2 = 5;
  average = 0;


  // measure and average the return voltage
  for (i=0; i<num_samples2; i++)
  {
    measure_dmm50(mod_id);
    if (get_result_nolog(mod_id, &result))
      waveform2[i] = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected DMM v)", true);
    average += waveform2[i];
  }
  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_aaux_dis(mod_id);
  close_olym(mod_id); 
  close_33250A(mod_id);
  }

  else
  {
  open_syn(mod_id); 
  setup_syn_hv(mod_id, true); 
  vth_aaux_dis(mod_id);
  aconnect_syn_out_xmint(mod_id); 
  connect_dmm50_out_af2(mod_id);

  // SETUP TO DO OVERVOLTAGE MEASUREMENT

  //close the auxout to synthesizer path relays
  vth_aclose_rly(mod_id, K741);   // X1MINT to X1 bus relay
  vth_aclose_rly(mod_id, K725);   // X1 bus to I bus relay
  vth_aclose_rly(mod_id, K846);   // 100 ohm load relay to REFR2
  vth_aclose_rly(mod_id, K828);   // REFR2 to DETHI relay
  vth_aclose_rly(mod_id, K822);   // DETHI to A bus relay
  vth_aclose_rly(mod_id, K823);   // A bus to AUXOUT relay

  // close the af2 measurement path relays
  vth_aclose_rly(mod_id, K808);   // DETHI to S bus relay
  vth_aclose_rly(mod_id, K751);   // S bus to A_XL bus relay
  vth_aclose_rly(mod_id, K764);   // A_XL bus to AF2 relay

  // close the synthesizer to ground path relays
  vth_aclose_rly(mod_id, K742);   // X2MINT to X2 bus relay
  vth_aclose_rly(mod_id, K718);   // X2 bus to L bus relay
  vth_aclose_rly(mod_id, K857);   // L bus to GROUND path relay

  /* Since the synthesizer cannot be tri-stated, set it to 10v before setting
     up the AUX source on the ASRU card.  */
  setup_syn(mod_id, DC, 0, 0.0, INTERNAL_SRC_VOLTAGE);
  vth_aaux_en_30ma_ilim(mod_id);
  vth_aaux_set_ampl(mod_id, INTERNAL_SRC_VOLTAGE);
  vth_aaux_en(mod_id);

  num_samples = 2300;  // number of samples to make with the dmm
  // get the dmm ready to catch the voltage on its way from 10v to 20v
  setup_dmm_digitizing(mod_id, DMM50_ADDRESS, 100, 20E-6, num_samples, 1.4E-6, 1E-3, 13, POS);

  flush_commands(mod_id);

  setup_syn_off(mod_id, EXTERNAL_SRC_VOLTAGE);  // send the Vapplied to 20v 
  wait_for_dmm50_measurement(mod_id);  // wait for the dmm to see the trigger

  vth_astat_rd(mod_id); // check aim status to make sure the voltage limit has occurred
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0); // bit 9 of the word is OV flag bit
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = over_voltage_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  // collect the voltage samples measured by the dmm and put into waveform array
  get_dmm_volt(mod_id, DMM50_ADDRESS, num_samples, waveform, 0);

  // find the abs value max voltage reached
  max_volt = waveform[0];
  for(i= 1; i < num_samples; i++)
  {
    if (max_volt < waveform[i])
      max_volt = waveform[i];
  }

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = max_volt;
  log_result(mod_id, result);

  // make sure that the aim card set the fatal error status bit
  vth_mrd_cell(mod_id, MMFP_GPIO);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    fatal_error_bit = P_getbits_US(this_word.bits, 6, 0); // bit 6 indicates if fatal error set
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = fatal_error_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MAKE SURE THAT THE RELAYS OPENED - CHECK MINIMUM VOLTAGE AT SCOPE */

  /* MAKE SURE THAT THE SOURCE COMES BACK UP - CHECK VOLTAGE AT SCOPE */

  vth_mhpib_wr(mod_id, 8, "PRESET NORM");

  // average the last 5 samples to find out what voltage the card settled at
  num_samples2 = 5;
  average = 0;
  for (i=num_samples-5; i<num_samples; i++)
    average += waveform[i];

  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_astat_clr(mod_id);
  /* close the AF2 measurement path relays */
  vth_aclose_rly(mod_id, K823);   /* AUXOUT to A bus relay */
  vth_aclose_rly(mod_id, K822);   /* A bus to DETHI relay */
  vth_aclose_rly(mod_id, K808);   /* DETHI to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL bus to AF2 relay */

  vth_mtmr_wait(mod_id, .5);
  num_samples2 = 5;
  average = 0;


  // measure and average the return voltage
  for (i=0; i<num_samples2; i++)
  {
    measure_dmm50(mod_id);
    if (get_result_nolog(mod_id, &result))
      waveform2[i] = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected DMM v)", true);
    average += waveform2[i];
  }
  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_aaux_dis(mod_id);
  close_syn(mod_id); 
  }

  close_mux(mod_id);
  close_dmm50(mod_id);
  CloseResourceManager();


}  /* t9317 */
#undef INTERNAL_SRC_VOLTAGE
#undef EXTERNAL_SRC_VOLTAGE
#undef OLYM_CH1_CURR	//added by chuanho, 14jun2012
#undef OLYM_CH1_OCP	//added by chuanho, 14jun2012
#undef OLYM_WAIT_TIME	//added by chuanho, 14jun2012

/* Where to Measure voltage */
/* Integer -1 = LEFT */
#define INTERNAL_SRC_VOLTAGE		-10e+00
#define EXTERNAL_SRC_VOLTAGE		-20e+00
#define EXTERNAL_SRC_VOLTAGE_NEG10V 10e+00	//added by chuanho, 14jun2012
#define EXTERNAL_SRC_VOLTAGE_NEG20V 20e+00	//added by chuanho, 14jun2012
#define OLYM_CH1_CURR				500e-03 //added by chuanho, 14jun2012
#define OLYM_CH1_OCP				1e+00   //added by chuanho, 14jun2012
#define OLYM_WAIT_TIME				150e-03	//added by chuanho, 14jun2012

/*Anita for setup of dmm*/
void t9318(int mod_id, int /*slot*/)
{
  /* AUX SOURCE NEGATIVE OVER VOLTAGE PROTECTION */

  int over_voltage_bit;
  int fatal_error_bit;
  a_word_type this_word;
  test_result_type result;
  arrform waveform;
  arrform waveform2;
  int i;
  double min_volt;
  double average;
  int num_samples;
  int num_samples2;

  OpenResourceManager();
  open_dmm50(mod_id);
  open_mux(mod_id);
  open_33250A(mod_id);
  
  /*New Olympus codes*/
  //Added by chuanho. 14jun2012
  if(check_syn_model())
  {
  open_olym(mod_id);
  vth_aaux_dis(mod_id);
  connect_olym_ch1(mod_id); 
  aconnect_olym_ch1_abus1_xmint(mod_id); 
  connect_dmm50_out_af2(mod_id);

  // SETUP TO DO OVERVOLTAGE MEASUREMENT

  // close the auxout to synthesizer path relays
  vth_aclose_rly(mod_id, K741);   // X1MINT to X1 bus relay
  vth_aclose_rly(mod_id, K725);   // X1 bus to I bus relay
  vth_aclose_rly(mod_id, K846);   // 100 ohm load relay to REFR2
  vth_aclose_rly(mod_id, K828);   // REFR2 to DETHI relay
  vth_aclose_rly(mod_id, K822);   // DETHI to A bus relay
  vth_aclose_rly(mod_id, K823);   // A bus to AUXOUT relay

  // close the af2 measurement path relays
  vth_aclose_rly(mod_id, K808);   // DETHI to S bus relay
  vth_aclose_rly(mod_id, K751);   // S bus to A_XL bus relay
  vth_aclose_rly(mod_id, K764);   // A_XL bus to AF2 relay

  // close the synthesize to ground path relays
  vth_aclose_rly(mod_id, K742);   // X2MINT to X2 bus relay
  vth_aclose_rly(mod_id, K718);   // X2 bus to L bus relay
  vth_aclose_rly(mod_id, K857);   // L bus to GROUND path relay

  /* Since the Olympus cannot be tri-stated, set it to 10v before setting
     up the AUX source on the ASRU card. */
  flush_commands(mod_id);
  setup_olym_ch1_volt(mod_id, EXTERNAL_SRC_VOLTAGE_NEG10V); 
  setup_olym_ch1_curr(mod_id, OLYM_CH1_CURR); 
  setup_olym_ch1_ocp(mod_id, OLYM_CH1_OCP); 
  enable_olym_ch1_out(mod_id);
  reverse_olym_ch1_out(mod_id);
  vth_mtmr_wait(mod_id, OLYM_WAIT_TIME);

  vth_aaux_en_30ma_ilim(mod_id);
  vth_aaux_set_ampl(mod_id, INTERNAL_SRC_VOLTAGE);
  vth_aaux_en(mod_id);

  flush_commands(mod_id);  
  num_samples = 2300;  // number of samples to make with the dmm
  // prepare the dmm to catch the transition from -10v to 20v
  setup_dmm_digitizing(mod_id, DMM50_ADDRESS, 100, 20E-6, num_samples, 1.4E-6, 1E-3, -13, NEG );
  flush_commands(mod_id);

  setup_olym_ch1_volt(mod_id, EXTERNAL_SRC_VOLTAGE_NEG20V);
  wait_for_dmm50_measurement(mod_id);

  //make sure the voltage limit has occurred
  vth_astat_rd(mod_id);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = over_voltage_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MEASURE THE PEAK VOLTAGE CAUSED BY TRI-STATE */

  // collect the data from the dmm into the waveform array
  get_dmm_volt(mod_id, DMM50_ADDRESS, num_samples, waveform, 0);

  min_volt = waveform[0];
  for(i= 1; i < num_samples; i++) {
    if (min_volt > waveform[i])
      min_volt = waveform[i];
  }

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = min_volt;
  log_result(mod_id, result);

  /* MAKE SURE THAT THE RELAYS OPENED - FATAL ERROR */
  vth_mrd_cell(mod_id, MMFP_GPIO);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    fatal_error_bit = P_getbits_US(this_word.bits, 6, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = fatal_error_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MAKE SURE THAT THE RELAYS OPENED - CHECK MINIMUM VOLTAGE AT SCOPE */


  vth_mhpib_wr(mod_id, 8, "PRESET NORM");

  vth_mtmr_wait(mod_id, t9120_res);
  /*anita - take 5 samples and average to get a more accurate value*/
  /*anita - taking average*/
  num_samples2 = 5;
  average = 0;

  for (i = num_samples-5; i < num_samples; i++)
    average += waveform[i];

  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);


  /* MAKE SURE THAT THE SOURCE COMES BACK UP - CHECK VOLTAGE AT SCOPE */

  vth_astat_clr(mod_id);
  /* close the AF2 measurement path relays */
  vth_aclose_rly(mod_id, K823);   /* AUXOUT to A bus relay */
  vth_aclose_rly(mod_id, K822);   /* A bus to DETHI relay */
  vth_aclose_rly(mod_id, K808);   /* DETHI to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL bus to AF2 relay */


  num_samples2 = 5;
  average = 0;
  for (i=0; i<num_samples2; i++){
    measure_dmm50(mod_id);
    if (get_result_nolog(mod_id, &result))
      waveform2[i] = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected DMM v)", true);
    average += waveform2[i];
  }
  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_aaux_dis(mod_id);
  close_olym(mod_id);
  close_33250A(mod_id);
  }

  else
  {
  open_syn(mod_id); 
  setup_syn_hv(mod_id, true); 
  vth_aaux_dis(mod_id);
  aconnect_syn_out_xmint(mod_id); 
  connect_dmm50_out_af2(mod_id);

  // SETUP TO DO OVERVOLTAGE MEASUREMENT

  // close the auxout to synthesizer path relays
  vth_aclose_rly(mod_id, K741);   // X1MINT to X1 bus relay
  vth_aclose_rly(mod_id, K725);   // X1 bus to I bus relay
  vth_aclose_rly(mod_id, K846);   // 100 ohm load relay to REFR2
  vth_aclose_rly(mod_id, K828);   // REFR2 to DETHI relay
  vth_aclose_rly(mod_id, K822);   // DETHI to A bus relay
  vth_aclose_rly(mod_id, K823);   // A bus to AUXOUT relay

  // close the af2 measurement path relays
  vth_aclose_rly(mod_id, K808);   // DETHI to S bus relay
  vth_aclose_rly(mod_id, K751);   // S bus to A_XL bus relay
  vth_aclose_rly(mod_id, K764);   // A_XL bus to AF2 relay

  // close the synthesize to ground path relays
  vth_aclose_rly(mod_id, K742);   // X2MINT to X2 bus relay
  vth_aclose_rly(mod_id, K718);   // X2 bus to L bus relay
  vth_aclose_rly(mod_id, K857);   // L bus to GROUND path relay

  /* Since the synthesizer cannot be tri-stated, set it to 10v before setting
     up the AUX source on the ASRU card. */
  setup_syn(mod_id, DC, 0, 0.0, INTERNAL_SRC_VOLTAGE); 
  vth_aaux_en_30ma_ilim(mod_id);
  vth_aaux_set_ampl(mod_id, INTERNAL_SRC_VOLTAGE);
  vth_aaux_en(mod_id);

  num_samples = 2300;  // number of samples to make with the dmm
  // prepare the dmm to catch the transition from -10v to 20v
  setup_dmm_digitizing(mod_id, DMM50_ADDRESS, 100, 20E-6, num_samples, 1.4E-6, 1E-3, -13, NEG );

  setup_syn_off(mod_id, EXTERNAL_SRC_VOLTAGE);  // send to -20v 

  flush_commands(mod_id);
  wait_for_dmm50_measurement(mod_id);

  //make sure the voltage limit has occurred
  vth_astat_rd(mod_id);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = over_voltage_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MEASURE THE PEAK VOLTAGE CAUSED BY TRI-STATE */

  // collect the data from the dmm into the waveform array
  get_dmm_volt(mod_id, DMM50_ADDRESS, num_samples, waveform, 0);

  min_volt = waveform[0];
  for(i= 1; i < num_samples; i++) {
    if (min_volt > waveform[i])
      min_volt = waveform[i];
  }

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = min_volt;
  log_result(mod_id, result);

  /* MAKE SURE THAT THE RELAYS OPENED - FATAL ERROR */
  vth_mrd_cell(mod_id, MMFP_GPIO);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    fatal_error_bit = P_getbits_US(this_word.bits, 6, 0);
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = fatal_error_bit;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);

  /* MAKE SURE THAT THE RELAYS OPENED - CHECK MINIMUM VOLTAGE AT SCOPE */


  vth_mhpib_wr(mod_id, 8, "PRESET NORM");

  vth_mtmr_wait(mod_id, t9120_res);
  /*anita - take 5 samples and average to get a more accurate value*/
  /*anita - taking average*/
  num_samples2 = 5;
  average = 0;

  for (i = num_samples-5; i < num_samples; i++)
    average += waveform[i];

  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);


  /* MAKE SURE THAT THE SOURCE COMES BACK UP - CHECK VOLTAGE AT SCOPE */

  vth_astat_clr(mod_id);
  /* close the AF2 measurement path relays */
  vth_aclose_rly(mod_id, K823);   /* AUXOUT to A bus relay */
  vth_aclose_rly(mod_id, K822);   /* A bus to DETHI relay */
  vth_aclose_rly(mod_id, K808);   /* DETHI to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL bus to AF2 relay */


  num_samples2 = 5;
  average = 0;
  for (i=0; i<num_samples2; i++){
    measure_dmm50(mod_id);
    if (get_result_nolog(mod_id, &result))
      waveform2[i] = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected DMM v)", true);
    average += waveform2[i];
  }
  average = average/num_samples2;

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);

  vth_aaux_dis(mod_id);
  close_syn(mod_id); 
  }

  close_mux(mod_id);
  close_dmm50(mod_id);
  CloseResourceManager();

}  /* t9318 */
#undef INTERNAL_SRC_VOLTAGE
#undef EXTERNAL_SRC_VOLTAGE
#undef EXTERNAL_SRC_VOLTAGE_NEG10V	//added by chuanho, 14jun2012
#undef EXTERNAL_SRC_VOLTAGE_NEG20V	//added by chuanho, 14jun2012
#undef OLYM_CH1_CURR				//added by chuanho, 14jun2012
#undef OLYM_CH1_OCP				//added by chuanho, 14jun2012
#undef OLYM_WAIT_TIME				//added by chuanho, 14jun2012

#define POSITIVE_SRC_VOLTAGE  5.0e+00
#define NEGATIVE_SRC_VOLTAGE  (-5.0e+00)
#define MEASURE_WAIT_TIME  2.0e+00
void t9320(int mod_id, int /*slot*/)
{
  /* Modified 6-30-99-  ABR ----------------------------------------------- */
  /* The scope was eliminated from the test and was replaced with a counter */
  /* The same measurements are taken and the test works basically the same. */
  /* however, the voltage measurements would be better if the resolution    */
  /* was better.  We tried to substitute the counter with the dmm but the   */
  /* dmm's best case sub-sampling sample rate is 10ns and the signal needed */
  /* to be digitized is about 20ns.  So the sampling rate is too slow.      */
  /* Check notes for a more detailed explaination.                          */
  /* ---------------------------------------------------------------------- */

 /* AUX SOURCE TRI-STATE SPIKE TEST */
  double min_volt;
  double max_volt;

  test_result_type result;

  open_cntr(mod_id);
  open_mux(mod_id);

  setup_cntr_defaults(mod_id);
/******************************************************************
the counter is set to 50 omhs and DC coupling
I think the attenuation does not matter in this case
because the voltage is around 5V but it could
potentially be  a problem.
*******************************************************************/
  vth_mhpib_wr(mod_id, 4, ":INP:ATT 10");
  vth_mhpib_wr(mod_id, 4, ":INP:COUP DC");
  vth_mhpib_wr(mod_id, 4, ":INP:IMP 50");


  aconnect_osc_ch1_af2(mod_id);

  /* SETUP TO DO TRI-STATE SPIKE MEASUREMENT */

  /* CLOSE THE AF2 MEASUREMENT PATH RELAYS */
  vth_aclose_rly(mod_id, K823);   /* AUXOUT to A bus relay */
  vth_aclose_rly(mod_id, K709);   /* A bus to X1 bus relay */
  vth_aclose_rly(mod_id, K717);   /* X1 bus to L bus relay */
  vth_aclose_rly(mod_id, K754);   /* L bus to A_XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL bus to AF2 relay */

  vth_aaux_dis_30ma_ilim(mod_id);
  vth_aaux_set_ampl(mod_id, POSITIVE_SRC_VOLTAGE);

/*******************************************************************
This section will setup the trigger for the counter
********************************************************************/
  vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'VOLT:MAX'");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:START:SOUR IMM");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:STOP:SOUR DIG");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:STOP:DIG 13");
  vth_aaux_en(mod_id);   /* TAKES THE SRC OUT OF TRI-STATE */

  vth_mtmr_wait(mod_id, MEASURE_WAIT_TIME);

  /* MEASURE THE PEAK VOLTAGE CAUSED BY TRI-STATE */
/******************************************************************
abr 07/98 - Measure the peak voltage caused by TRI-STATE

The scope used a -5V offset for better measurement,
for that reason, I substracted 5V offset to the min_volts
variable
*******************************************************************/
  trigger_cntr(mod_id);
  min_volt = read_cntr(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = min_volt-5;
  log_result(mod_id, result);


  vth_aaux_dis(mod_id);

  vth_aaux_set_ampl(mod_id, NEGATIVE_SRC_VOLTAGE);

/*******************************************************************
This section will setup the trigger for the counter
********************************************************************/
  vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'VOLT:MIN'");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:START:SOUR IMM");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:STOP:SOUR DIG");
  vth_mhpib_wr(mod_id, 4, ":SENSE:FREQ:ARM:STOP:DIG 13");


  vth_aaux_en(mod_id);   /* TAKES THE SRC OUT OF TRI-STATE */

  vth_mtmr_wait(mod_id, MEASURE_WAIT_TIME);

  /* MEASURE THE PEAK VOLTAGE CAUSED BY TRI-STATE */
/******************************************************************
abr 07/98 - Measure the peak voltage caused by TRI-STATE

The scope used a 5V offset for better measurement,
for that reason, I added 5V offset to the max_volts
variable
*******************************************************************/
  trigger_cntr(mod_id);
  max_volt = read_cntr(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = max_volt+5;
  log_result(mod_id, result);


  vth_aaux_dis(mod_id);

  vth_ainit(mod_id);

  close_cntr(mod_id);
  close_mux(mod_id);

}  /* t9320 */

#undef POSITIVE_SRC_VOLTAGE
#undef NEGATIVE_SRC_VOLTAGE
#undef MEASURE_WAIT_TIME

/////////////////////////////////////////////ADDED TEST T9324 for NASRU, EQUAL TO T9325 except for the loosen test limit////
#define LOW             0
#define HIGH            1
#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1
typedef double source_voltage_array_[2];
/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        10
/* FOR M.A.D. */
#define DIGITAL         0
/* 625mv detector range */
#define R625            4
/* 2.5v detector range */
#define R2500           6
/* 10v detector range */
#define R10000          8
/* initial requiv guess */
#define RINIT           4.0e+00
/* initial rseries guess */
#define RSINIT          0.5 //NASRU CHNAGED 0.6e+00
/* initial vatten guess */
#define VINIT           1.01e+00
#define EXPECTED_DET_VOLTS  2.0e+00
#define TIMEOUT         10.0e+00
#define PS_CURRENT      800e-03
#define DIFF            5e-03
#define RAMP_TIME       10e+00
#define LOW_CURRENT_VALUE  27.7e-03 //27.5e-03 //sri
#define HIGH_CURRENT_VALUE  165e-03
#define HIGH_CURRENT_VALUE_SINKING 160E-03 //see comment below with code
const source_voltage_array_ SOURCE_VOLTAGE___NASRU = {7e00, -7e00};
void t9324(int mod_id, int /*slot*/)
{
  /* AUX SOURCE OUTPUT CURRENT TEST */
  int source_i_compliance;

  double r10;
  double detector_voltage;
  double ps_sense_voltage;
  double ps_voltage;
  double source_current;
  double meas_source_current;
  double vload;
  double vnoload;
  double vatten;
  double requiv;
  double rseries;

  test_result_type result;
  a_word_type this_word;

//sri added delay 5 sec//
  for (time_t t = time(0) + 5; time(0) < t; ) {}//added Sri

  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_mux(mod_id);
  open_ps(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, EXPECTED_DET_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);   /* DETLO to L bus relay */
  vth_aopen_rly(mod_id, K812);   /* DETHI to I bus relay */

  /* CLOSE THE AUXOUT TO P.S. POSITIVE  PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K828);   /* 10 ohm resistor to DETHI */
  vth_aclose_rly(mod_id, K822);   /* DETHI to A bus relay */
  vth_aclose_rly(mod_id, K823);   /* A bus to AUXOUT relay */

  /* CLOSE THE P.S. NEGATIVE  TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAY */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */

  /* MEASURE THE POWER SUPPLY ATTENUATION FACTOR AND THE EQUIVALENT RESISTANCE */
  requiv = RINIT;
  vatten = VINIT;
  rseries = RSINIT;
  vth_aaux_dis_30ma_ilim(mod_id);
  source_current = -1 * LOW_CURRENT_VALUE;   /* AMPS */
  ps_voltage = vatten * (SOURCE_VOLTAGE___NASRU[POSITIVE] +
                         source_current * (r10 + requiv + rseries));
  vth_aclose_rly(mod_id, K725);
  vth_aclose_rly(mod_id, K718);
  vth_aaux_set_ampl(mod_id, SOURCE_VOLTAGE___NASRU[POSITIVE]);
  vth_aaux_en(mod_id);
  setup_ps(mod_id, ps_voltage, PS_CURRENT);
  verify_ps_output(mod_id, DIFF, RAMP_TIME, fabs(ps_voltage));
  vth_astat_clr(mod_id);

  vth_afunc_meas(mod_id);
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    detector_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in result log (Expected DET stat)", true);

  meas_source_current = fabs(detector_voltage / r10);
  if (meas_source_current > 1.5 * fabs(source_current) ||
      meas_source_current < 0.5 * fabs(source_current))
    meas_source_current = fabs(source_current);

  ps_sense_voltage = measure_ps_sense_voltage(mod_id);

  vth_adet_sel_rng(mod_id, R10000);
  vth_aopen_rly(mod_id, K2306);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2304);   /* reconfigure for grounded meas */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_afunc_meas(mod_id);   /* measure the loaded ps voltage */
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    vload = fabs(result.UU.Real_result);
  else
    display_prompt("No result in log(Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in log (Expected DET stat)", true);

  vth_aaux_dis(mod_id);   /* no hot switch of relays */
  vth_aopen_rly(mod_id, K845);   /* remove all load from ps */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_afunc_meas(mod_id);   /* measure the unloaded ps voltage */
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    vnoload = fabs(result.UU.Real_result);
  else
    display_prompt("No result in log(Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in log (Expected DET stat)", true);

  if (vnoload > 10.0 || vnoload < 4.0)
    vnoload = 7.0;
  vatten = ps_sense_voltage / vnoload;
  requiv = fabs(vload - vnoload) / meas_source_current;
  rseries = (SOURCE_VOLTAGE___NASRU[POSITIVE] - detector_voltage - vload) /
            meas_source_current;

  vth_aclose_rly(mod_id, K845);   /* reconfigure load and differential */
  vth_aopen_rly(mod_id, K2304);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2306);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  for (int current = LOW; current <= HIGH; ++current)
  {  /* SETUP AND MEASURE FOR EACH CASE */
    if (current == LOW)
    {  /* low current */
      vth_aaux_en_30ma_ilim(mod_id);
      vth_adet_sel_rng(mod_id, R625);
    }  /* low current */
    else
    {  /* high current */
      vth_aaux_dis_30ma_ilim(mod_id);
      vth_adet_sel_rng(mod_id, R2500);
    }  /* high current */

    for (int source_polarity = POSITIVE; source_polarity <= NEGATIVE; ++source_polarity)
    {
      if (source_polarity == POSITIVE)
      {  /* CONNECT PS + TO X1 AND PS - TO X2 */
        vth_aopen_rly(mod_id, K726);
        vth_aopen_rly(mod_id, K717);
        vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K725);
        vth_aclose_rly(mod_id, K718);
      }  /* CONNECT PS + TO X1 AND PS - TO X2 */
      else
      {  /* CONNECT PS + TO X2 AND PS - TO X1 */
        vth_aopen_rly(mod_id, K725);
        vth_aopen_rly(mod_id, K718);
        vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K726);
        vth_aclose_rly(mod_id, K717);
      }  /* CONNECT PS + TO X2 AND PS - TO X1 */

      for (int direction = SOURCING; direction <= SINKING; ++direction)
      {  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */
        if (direction == SOURCING)
        {
          if (current == LOW)
            source_current = -1 * LOW_CURRENT_VALUE;   /* AMPS */
          else
            source_current = -1 * HIGH_CURRENT_VALUE;   /* AMPS */
        }
        else
        {  /* direction = sinking */
          if (current == LOW)
            source_current = LOW_CURRENT_VALUE;   /* AMPS */
          else
	  {
            /* the new HIGH_CURRENT_VALUE has been changed to a specific
               value for sinking (160ma) to fix a marginal problem with
               a resistor close to tolerance on the aux source hybrid.
               this should be ok since the customer spec = 150ma.
	       --- mjs per wally austin 10/98 */
            source_current = HIGH_CURRENT_VALUE_SINKING;   /* AMPS */
	  }
        }

        vth_aaux_set_ampl(mod_id, SOURCE_VOLTAGE___NASRU[source_polarity]);

        ps_voltage = vatten * (SOURCE_VOLTAGE___NASRU[source_polarity] +
                               source_current * (r10 + requiv + rseries));
        setup_ps(mod_id, ps_voltage, PS_CURRENT);
        verify_ps_output(mod_id, DIFF, RAMP_TIME, fabs(ps_voltage));
        vth_aaux_en(mod_id);
        vth_mtmr_wait(mod_id, 0.001);
        vth_astat_clr(mod_id);
        vth_mtmr_wait(mod_id, 0.01);

        /* NOW CHECK THE SOURCE COMPLIANCE CURRENT LIMIT FOR NO LIMIT */
        vth_astat_rd(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          this_word.data = result.UU.Integer_result;
          /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  ... 15 */
          /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  ... 0  */
          /* THEREFORE 2^1  =  BIT NUMBER 14 */
//mjs -- another messed up bit problem
          source_i_compliance = P_getbits_US(this_word.bits, 1, 0);
          result.Measurement_class = CLASS_INTEGER;
          result.UU.Integer_result = source_i_compliance;
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected aim status)",
                         true);

        vth_afunc_meas(mod_id);
        vth_get_afunc_result(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          detector_voltage = result.UU.Real_result;
          meas_source_current = detector_voltage / r10;
          result.UU.Real_result = fabs(meas_source_current);
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected DET volts)", true);
        if (!get_result_nolog(mod_id, &result))
          display_prompt("No result in result log (Expected DET stat)", true);

        vth_aaux_dis(mod_id);

      }


    }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */

  }  /* for CURRENT */


  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9324 */
#undef LOW
#undef HIGH
#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef R625
#undef R2500
#undef R10000
#undef RINIT
#undef RSINIT
#undef VINIT
#undef EXPECTED_DET_VOLTS
#undef TIMEOUT
#undef PS_CURRENT
#undef DIFF
#undef RAMP_TIME
#undef LOW_CURRENT_VALUE
#undef HIGH_CURRENT_VALUE

/////////////////////////////////////////////END OF TEST T9324//////////////////////////////////////////////////////////////

#define LOW             0
#define HIGH            1
#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1
typedef double source_voltage_array_[2];
/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        10
/* FOR M.A.D. */
#define DIGITAL         0
/* 625mv detector range */
#define R625            4
/* 2.5v detector range */
#define R2500           6
/* 10v detector range */
#define R10000          8
/* initial requiv guess */
#define RINIT           4.0e+00
/* initial rseries guess */
#define RSINIT          0.6e+00
/* initial vatten guess */
#define VINIT           1.01e+00
#define EXPECTED_DET_VOLTS  2.0e+00
#define TIMEOUT         10.0e+00
#define PS_CURRENT      800e-03
#define DIFF            5e-03
#define RAMP_TIME       10e+00
#define LOW_CURRENT_VALUE  27.7e-03 //27.5e-03 //sri
#define HIGH_CURRENT_VALUE  165e-03
#define HIGH_CURRENT_VALUE_SINKING 160E-03 //see comment below with code
const source_voltage_array_ SOURCE_VOLTAGE___ = {7e00, -7e00};
void t9325(int mod_id, int /*slot*/)
{
  /* AUX SOURCE OUTPUT CURRENT TEST */
  int source_i_compliance;

  double r10;
  double detector_voltage;
  double ps_sense_voltage;
  double ps_voltage;
  double source_current;
  double meas_source_current;
  double vload;
  double vnoload;
  double vatten;
  double requiv;
  double rseries;

  test_result_type result;
  a_word_type this_word;

//sri added delay 5 sec//
  for (time_t t = time(0) + 5; time(0) < t; ) {}//added Sri

  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_mux(mod_id);
  open_ps(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, EXPECTED_DET_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);   /* DETLO to L bus relay */
  vth_aopen_rly(mod_id, K812);   /* DETHI to I bus relay */

  /* CLOSE THE AUXOUT TO P.S. POSITIVE  PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K828);   /* 10 ohm resistor to DETHI */
  vth_aclose_rly(mod_id, K822);   /* DETHI to A bus relay */
  vth_aclose_rly(mod_id, K823);   /* A bus to AUXOUT relay */

  /* CLOSE THE P.S. NEGATIVE  TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAY */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */

  /* MEASURE THE POWER SUPPLY ATTENUATION FACTOR AND THE EQUIVALENT RESISTANCE */
  requiv = RINIT;
  vatten = VINIT;
  rseries = RSINIT;
  vth_aaux_dis_30ma_ilim(mod_id);
  source_current = -1 * LOW_CURRENT_VALUE;   /* AMPS */
  ps_voltage = vatten * (SOURCE_VOLTAGE___[POSITIVE] +
                         source_current * (r10 + requiv + rseries));
  vth_aclose_rly(mod_id, K725);
  vth_aclose_rly(mod_id, K718);
  vth_aaux_set_ampl(mod_id, SOURCE_VOLTAGE___[POSITIVE]);
  vth_aaux_en(mod_id);
  setup_ps(mod_id, ps_voltage, PS_CURRENT);
  verify_ps_output(mod_id, DIFF, RAMP_TIME, fabs(ps_voltage));
  vth_astat_clr(mod_id);

  vth_afunc_meas(mod_id);
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    detector_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in result log (Expected DET stat)", true);

  meas_source_current = fabs(detector_voltage / r10);
  if (meas_source_current > 1.5 * fabs(source_current) ||
      meas_source_current < 0.5 * fabs(source_current))
    meas_source_current = fabs(source_current);

  ps_sense_voltage = measure_ps_sense_voltage(mod_id);

  vth_adet_sel_rng(mod_id, R10000);
  vth_aopen_rly(mod_id, K2306);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2304);   /* reconfigure for grounded meas */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_afunc_meas(mod_id);   /* measure the loaded ps voltage */
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    vload = fabs(result.UU.Real_result);
  else
    display_prompt("No result in log(Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in log (Expected DET stat)", true);

  vth_aaux_dis(mod_id);   /* no hot switch of relays */
  vth_aopen_rly(mod_id, K845);   /* remove all load from ps */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_afunc_meas(mod_id);   /* measure the unloaded ps voltage */
  vth_get_afunc_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    vnoload = fabs(result.UU.Real_result);
  else
    display_prompt("No result in log(Expected DET volts)", true);
  if (!get_result_nolog(mod_id, &result))
    display_prompt("No result in log (Expected DET stat)", true);

  if (vnoload > 10.0 || vnoload < 4.0)
    vnoload = 7.0;
  vatten = ps_sense_voltage / vnoload;
  requiv = fabs(vload - vnoload) / meas_source_current;
  rseries = (SOURCE_VOLTAGE___[POSITIVE] - detector_voltage - vload) /
            meas_source_current;

  vth_aclose_rly(mod_id, K845);   /* reconfigure load and differential */
  vth_aopen_rly(mod_id, K2304);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2306);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  for (int current = LOW; current <= HIGH; ++current)
  {  /* SETUP AND MEASURE FOR EACH CASE */
    if (current == LOW)
    {  /* low current */
      vth_aaux_en_30ma_ilim(mod_id);
      vth_adet_sel_rng(mod_id, R625);
    }  /* low current */
    else
    {  /* high current */
      vth_aaux_dis_30ma_ilim(mod_id);
      vth_adet_sel_rng(mod_id, R2500);
    }  /* high current */

    for (int source_polarity = POSITIVE; source_polarity <= NEGATIVE; ++source_polarity)
    {
      if (source_polarity == POSITIVE)
      {  /* CONNECT PS + TO X1 AND PS - TO X2 */
        vth_aopen_rly(mod_id, K726);
        vth_aopen_rly(mod_id, K717);
        vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K725);
        vth_aclose_rly(mod_id, K718);
      }  /* CONNECT PS + TO X1 AND PS - TO X2 */
      else
      {  /* CONNECT PS + TO X2 AND PS - TO X1 */
        vth_aopen_rly(mod_id, K725);
        vth_aopen_rly(mod_id, K718);
        vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K726);
        vth_aclose_rly(mod_id, K717);
      }  /* CONNECT PS + TO X2 AND PS - TO X1 */

      for (int direction = SOURCING; direction <= SINKING; ++direction)
      {  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */
        if (direction == SOURCING)
        {
          if (current == LOW)
            source_current = -1 * LOW_CURRENT_VALUE;   /* AMPS */
          else
            source_current = -1 * HIGH_CURRENT_VALUE;   /* AMPS */
        }
        else
        {  /* direction = sinking */
          if (current == LOW)
            source_current = LOW_CURRENT_VALUE;   /* AMPS */
          else
	  {
            /* the new HIGH_CURRENT_VALUE has been changed to a specific
               value for sinking (160ma) to fix a marginal problem with
               a resistor close to tolerance on the aux source hybrid.
               this should be ok since the customer spec = 150ma.
	       --- mjs per wally austin 10/98 */
            source_current = HIGH_CURRENT_VALUE_SINKING;   /* AMPS */
	  }
        }

        vth_aaux_set_ampl(mod_id, SOURCE_VOLTAGE___[source_polarity]);

        ps_voltage = vatten * (SOURCE_VOLTAGE___[source_polarity] +
                               source_current * (r10 + requiv + rseries));
        setup_ps(mod_id, ps_voltage, PS_CURRENT);
        verify_ps_output(mod_id, DIFF, RAMP_TIME, fabs(ps_voltage));
        vth_aaux_en(mod_id);
        vth_mtmr_wait(mod_id, 0.001);
        vth_astat_clr(mod_id);
        vth_mtmr_wait(mod_id, 0.01);

        /* NOW CHECK THE SOURCE COMPLIANCE CURRENT LIMIT FOR NO LIMIT */
        vth_astat_rd(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          this_word.data = result.UU.Integer_result;
          /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  ... 15 */
          /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  ... 0  */
          /* THEREFORE 2^1  =  BIT NUMBER 14 */
//mjs -- another messed up bit problem
          source_i_compliance = P_getbits_US(this_word.bits, 1, 0);
          result.Measurement_class = CLASS_INTEGER;
          result.UU.Integer_result = source_i_compliance;
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected aim status)",
                         true);

        vth_afunc_meas(mod_id);
        vth_get_afunc_result(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          detector_voltage = result.UU.Real_result;
          meas_source_current = detector_voltage / r10;
          result.UU.Real_result = fabs(meas_source_current);
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected DET volts)", true);
        if (!get_result_nolog(mod_id, &result))
          display_prompt("No result in result log (Expected DET stat)", true);

        vth_aaux_dis(mod_id);

      }


    }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */

  }  /* for CURRENT */


  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9325 */
#undef LOW
#undef HIGH
#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef R625
#undef R2500
#undef R10000
#undef RINIT
#undef RSINIT
#undef VINIT
#undef EXPECTED_DET_VOLTS
#undef TIMEOUT
#undef PS_CURRENT
#undef DIFF
#undef RAMP_TIME
#undef LOW_CURRENT_VALUE
#undef HIGH_CURRENT_VALUE

#define LOW             0
#define HIGH            1
#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1
typedef double i_type_array_[2];
typedef i_type_array_ i_direction_array__[2];
typedef i_direction_array__ voltage_array_______[2];
/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        50
/* NOT IMPLEMENTED (FOR M.A.D.) */
#define DIGITAL         0
#define TIMEOUT         10.0e+00
#define POWER_SUPPLY_CURRENT  800e-03
#define POWER_SUPPLY_RAMP_TIME  1.0e+00
#define MAX_EXPECTED_VOLTS  2.1e+00

const voltage_array_______ SOURCE_VOLTAGE____ =
{
  {
    {10.0e+00, 10.0e+00},
    { 9.0e+00,  6.0e+00}
  },
  {
    { -9.0e+00,  -6.0e+00},
    {-10.0e+00, -10.0e+00}
  }
};

const voltage_array_______ PS_VOLTAGE__ =
{
  {
    { 9.0e+00,  6.0e+00},
    {10.0e+00, 10.0e+00}
  },
  {
    {-10.0e+00, -10.0e+00},
    { -9.0e+00,  -6.0e+00}
  }
};

void t9326(int mod_id, int /*slot*/)
{
  /* AUX SOURCE MAXIMUM CURRENT */
  double r10;
  double max_current;

  test_result_type result;


  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_ps(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);
  vth_aopen_rly(mod_id, K812);

  /* CLOSE THE AUXOUT TO P.S. PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K828);   /* 10 ohm resistor to DETHI */
  vth_aclose_rly(mod_id, K822);   /* DETHI to A bus relay */
  vth_aclose_rly(mod_id, K823);   /* A bus to AUXOUT relay */

  /* CLOSE THE P.S. TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAY */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */

  for (int current = LOW; current <= HIGH; ++current)
  {  /* SETUP FOR LOW AND HIGH CURRENT MAXIMUMS */
    if (current == LOW)
      vth_aaux_en_30ma_ilim(mod_id);
    else
      vth_aaux_dis_30ma_ilim(mod_id);

    for (int aux_polarity = POSITIVE; aux_polarity <= NEGATIVE; ++aux_polarity)
    {  /* SETUP FOR POSITIVE AND NEGATIVE SOURCE VOLTAGES */
      if (aux_polarity == POSITIVE)
      {  /* CONNECT PS + TO X1 AND PS - TO X2 */
        vth_aopen_rly(mod_id, K726);
        vth_aopen_rly(mod_id, K717);
        vth_aclose_rly(mod_id, K725);
        vth_aclose_rly(mod_id, K718);
      }  /* CONNECT PS + TO X1 AND PS - TO X2 */
      else
      {  /* CONNECT PS + TO X2 AND PS - TO X1 */
        vth_aopen_rly(mod_id, K725);
        vth_aopen_rly(mod_id, K718);
        vth_aclose_rly(mod_id, K726);
        vth_aclose_rly(mod_id, K717);
      }  /* CONNECT PS + TO X2 AND PS - TO X1 */

      for (int direction = SOURCING; direction <= SINKING; ++direction)
      {  /* SETUP FOR SOURCING AND SINKING CONDITIONS */
        vth_aaux_set_ampl(mod_id, SOURCE_VOLTAGE____[aux_polarity][direction]
                      [current]);
        vth_aaux_en(mod_id);

        setup_ps(mod_id, PS_VOLTAGE__[aux_polarity][direction]
                 [current], POWER_SUPPLY_CURRENT);
        vth_mtmr_wait(mod_id, POWER_SUPPLY_RAMP_TIME);

        vth_afunc_meas(mod_id);
        vth_get_afunc_result(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          max_current = result.UU.Real_result / r10;
          result.UU.Real_result = fabs(max_current);
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected DET volts)", true);

        if (!get_result_nolog(mod_id, &result))
          display_prompt("No result in result log (Expected DET stat)", true);

        vth_aaux_dis(mod_id);

      }


    }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */


  }  /* for CURRENT */


  vth_ainit(mod_id);

  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9326 */
#undef LOW
#undef HIGH
#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef TIMEOUT
#undef POWER_SUPPLY_CURRENT
#undef POWER_SUPPLY_RAMP_TIME
#undef MAX_EXPECTED_VOLTS

#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1
typedef double i_direction_array___[2];
typedef i_direction_array___ voltage_array________[2];
/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        50
/* NOT IMPLEMENTED (FOR M.A.D.) */
#define DIGITAL         0
#define TIMEOUT         10.0e+00
#define POWER_SUPPLY_CURRENT  800e-03
#define POWER_SUPPLY_RAMP_TIME  1.0e+00
#define MAX_EXPECTED_VOLTS  2.1e+00

const voltage_array________ SOURCE_VOLTAGE_____ =
{
  {10.0e+00,   6.0e+00},
  {-6.0e+00, -10.0e+00}
};

const voltage_array________ PS_VOLTAGE___ =
{
  {  6.0e+00, 10.0e+00},
  {-10.0e+00, -6.0e+00}
};

void t9327(int mod_id, int /*slot*/)
{
  /* AUX SOURCE MAXIMUM CURRENT (HIGH LEVEL ONLY) */
  double r10;
  double max_current;
  test_result_type result;

  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_ps(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);
  vth_aopen_rly(mod_id, K812);

  /* CLOSE THE AUXOUT TO P.S. PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K828);   /* 10 ohm resistor to DETHI */
  vth_aclose_rly(mod_id, K822);   /* DETHI to A bus relay */
  vth_aclose_rly(mod_id, K823);   /* A bus to AUXOUT relay */

  /* CLOSE THE P.S. TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAY */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */

  vth_aaux_dis_30ma_ilim(mod_id);

  for (int aux_polarity = POSITIVE; aux_polarity <= NEGATIVE; ++aux_polarity)
  {  /* SETUP FOR POSITIVE AND NEGATIVE SOURCE VOLTAGES */
    if (aux_polarity == POSITIVE)
    {  /* CONNECT PS + TO X1 AND PS - TO X2 */
      vth_aopen_rly(mod_id, K726);
      vth_aopen_rly(mod_id, K717);
      vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K725);
      vth_aclose_rly(mod_id, K718);
    }  /* CONNECT PS + TO X1 AND PS - TO X2 */
    else
    {  /* CONNECT PS + TO X2 AND PS - TO X1 */
      vth_aopen_rly(mod_id, K725);
      vth_aopen_rly(mod_id, K718);
      vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K726);
      vth_aclose_rly(mod_id, K717);
    }  /* CONNECT PS + TO X2 AND PS - TO X1 */

    for (int direction = SOURCING; direction <= SINKING; ++direction)
    {  /* SETUP FOR SOURCING AND SINKING CONDITIONS */
      vth_aaux_set_ampl(mod_id, SOURCE_VOLTAGE_____[aux_polarity][direction]);
      vth_aaux_en(mod_id);

      setup_ps(mod_id, PS_VOLTAGE___[aux_polarity]
               [direction], POWER_SUPPLY_CURRENT);
      vth_mtmr_wait(mod_id, POWER_SUPPLY_RAMP_TIME);

      vth_afunc_meas(mod_id);
      vth_get_afunc_result(mod_id);
      if (get_result_nolog(mod_id, &result))
      {
        max_current = result.UU.Real_result / r10;
        result.UU.Real_result = fabs(max_current);
        log_result(mod_id, result);
      }
      else
        display_prompt("No result in result log (Expected DET volts)", true);

      if (!get_result_nolog(mod_id, &result))
        display_prompt("No result in result log (Expected DET stat)", true);

      vth_aaux_dis(mod_id);

    }  /* SETUP FOR SOURCING AND SINKING CONDITIONS */


  }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */


  vth_ainit(mod_id);

  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9327 */
#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef TIMEOUT
#undef POWER_SUPPLY_CURRENT
#undef POWER_SUPPLY_RAMP_TIME
#undef MAX_EXPECTED_VOLTS
#undef F1024
#undef F8192
#undef F128

#define NUM_VOLTAGES_TO_TEST  3
#define NUM_FREQUENCIES_TO_TEST  3
#define STOP_FREQ       20000
typedef double voltage_array_________[NUM_VOLTAGES_TO_TEST];
typedef int range_array__[NUM_VOLTAGES_TO_TEST];
typedef int frequency_array[NUM_FREQUENCIES_TO_TEST];
/*filter_no*/
#define F128            5
/*filter_no*/
#define F1024           6
/*filter_no*/
#define F8192           7
/* dmm range headroom */
#define FUDGE_FACTOR    1.2e+00
#define DC_OFFSET       0.0e+00
#define RESOLUTION      10e-04
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03
#define LOW_BAND_RATIO  0.8e+00

const voltage_array_________ TEST____ = {0.1e00, 0.2e00, 7.07071e00};
const frequency_array FREQ_A = {128, 1024, 8192};
const frequency_array FILTER = {F128, F1024, F8192};
#define DMM_FUNCTION    ACV
void t9355(int mod_id, int /*slot*/)
{
  /* AC SOURCE VOLTAGE ACCURACY AND RANGE */
  int start_freq;
  double dmm_voltage_range;

  range_array__ range;
  a_word_type this_word;
  test_result_type result;

  open_dmm(mod_id);
  open_mux(mod_id);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K837);   /* SRCOUT to RCAL2 relay */
  vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  setup_dmm_level_filter(mod_id, true);

  for (int voltage = 0; voltage < NUM_VOLTAGES_TO_TEST; ++voltage)
  {  /*for voltage*/
    dmm_voltage_range = TEST____[voltage] * sqrt(2.0) * FUDGE_FACTOR;
    setup_dmm(mod_id, DMM_FUNCTION, dmm_voltage_range, RESOLUTION);

    for (int frequency = 0; frequency < NUM_FREQUENCIES_TO_TEST; ++frequency)
    {  /* for frequency */
      start_freq = static_cast<int>(FREQ_A[frequency] * LOW_BAND_RATIO);
      setup_dmm_ac_band(mod_id, start_freq, STOP_FREQ);

      vth_asrc_sel_fltr(mod_id, FILTER[frequency]);
      vth_ascat_cpu_init(mod_id);
      vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
      vth_ascat_set_a_freq(mod_id, FREQ_A[frequency]);
      vth_ascat_en_cont_src(mod_id);
      vth_ascat_en_frun_src(mod_id);
      vth_ascat_run_en(mod_id);
      vth_asrc_en_30ma_ilim(mod_id);
      vth_asrc_set_ampl(mod_id, TEST____[voltage], DC_OFFSET);
      vth_afc_en_src_out(mod_id);

      vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
      measure_dmm(mod_id);
      if (!get_result_log(mod_id, &result))
        display_prompt("No result in result log (Expected dmm vth_reading)", true);
    }  /* for frequency */

    /* READ RANGE BIT TO VERIFY THE AC SOURCE OPERATES IN THE CORRECT RANGE */
    vth_ard_cell(mod_id, ARLY_2A);
    if (get_result_nolog(mod_id, &result))
    {  /* check the result */
      this_word.data = result.UU.Integer_result;
      /* The 15th bit of this_word corresponds to binary 2^0 = range bit */
//mjs -- another messed up bit problem
      range[voltage] = P_getbits_US(this_word.bits, 0, 0);
    }  /* check the result */
    else
      display_prompt("No result in result log (Expected asrc range)", true);

  }  /*for voltage*/


  for (voltage = 0; voltage < NUM_VOLTAGES_TO_TEST; voltage++)
  {  /*for voltage*/
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = range[voltage];
    log_result(mod_id, result);
  }  /*for voltage*/

  vth_afc_dis_src_out(mod_id);

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9355 */
#undef NUM_VOLTAGES_TO_TEST
#undef NUM_FREQUENCIES_TO_TEST
#undef STOP_FREQ
#undef F128
#undef F1024
#undef F8192
#undef FUDGE_FACTOR
#undef DC_OFFSET
#undef RESOLUTION
#undef SOURCE_WAIT_TIME
#undef LOW_BAND_RATIO
#undef DMM_FUNCTION


#define NUM_FREQUENCIES_TO_TEST  3
/* 0 selects frequency A */
/* 30mA current compliance    */
/* No additional ouput resistance*/
/* Source enabled */
/* Source controlled by processor*/
typedef int frequency_array_[NUM_FREQUENCIES_TO_TEST];
/*filter_no*/
#define F128            5
/*filter_no*/
#define F1024           6
/*filter_no*/
#define F8192           7
#define RMS_VOLTS       0.1e+00
#define FUDGE_FACTOR    1.2e+00
#define DC_OFFSET       0.0e+00
#define AC_OF_0V        0.0e+00
#define RESOLUTION      10e-04
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03
const frequency_array_ FREQ_A_ = {128, 1024, 8192};
const frequency_array_ FILTER_ = {F128, F1024, F8192};
#define DMM_FUNCTION    DCV
void t9360(int mod_id, int /*slot*/)
{
  /* AC SOURCE DC OFFSET VOLTAGE ACCURACY */
int frequency;

  double dmm_volts;
  double aperature;

  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K837);   /* SRCOUT to RCAL2 relay */
  vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  dmm_volts = RMS_VOLTS * sqrt(2.0) * FUDGE_FACTOR;
  setup_dmm(mod_id, DMM_FUNCTION, dmm_volts, RESOLUTION);

  for (frequency = 1; frequency <= NUM_FREQUENCIES_TO_TEST; frequency++)
  {  /* for frequency */
    aperature = 1.0 / frequency;
    if (frequency == 1024)
      aperature *= 10;
    if (frequency == 8192)
      aperature *= 100;
    setup_dmm_aperature(mod_id, aperature);

    vth_asrc_sel_fltr(mod_id, FILTER_[frequency - 1]);
    vth_ascat_cpu_init(mod_id);
    vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
    vth_ascat_set_a_freq(mod_id, FREQ_A_[frequency - 1]);
    vth_ascat_en_cont_src(mod_id);
    vth_ascat_en_frun_src(mod_id);
    vth_ascat_run_en(mod_id);
    vth_asrc_en_30ma_ilim(mod_id);
    vth_asrc_set_ampl(mod_id, AC_OF_0V, DC_OFFSET);
    vth_afc_en_src_out(mod_id);

    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    measure_dmm(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected dmm vth_reading)", true);


    vth_afc_dis_src_out(mod_id);
    vth_asrc_set_ampl(mod_id, RMS_VOLTS, DC_OFFSET);
    vth_afc_en_src_out(mod_id);

    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    measure_dmm(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected dmm vth_reading)", true);
  }  /* for frequency */

  vth_afc_dis_src_out(mod_id);

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9360 */
#undef NUM_FREQUENCIES_TO_TEST
#undef F128
#undef F1024
#undef F8192
#undef RMS_VOLTS
#undef FUDGE_FACTOR
#undef DC_OFFSET
#undef AC_OF_0V
#undef RESOLUTION
#undef SOURCE_WAIT_TIME
#undef DMM_FUNCTION

#define NUM_FREQUENCIES_TO_TEST  3
/* 0 selects frequency A */
/* No 30mA current compliance    */
/* No additional ouput resistance*/
/* Source enabled */
/* Source controlled by processor*/
#define SAMPLE_SIZE     100
typedef int frequency_array__[NUM_FREQUENCIES_TO_TEST];
/*filter_no*/
#define F128            5
/*filter_no*/
#define F1024           6
/*filter_no*/
#define F8192           7
#define RMS_VOLTS       3.5e+00
#define DC_OFFSET       0.0e+00
#define TRIGGER_LEVEL   0.0e+00
/*seconds*/
#define SOURCE_WAIT_TIME  1e-03
const frequency_array__ FREQ_A__ = {128, 1024, 8192};
const frequency_array__ FILTER__ = {F128, F1024, F8192};
void t9365(int mod_id, int /*slot*/)
{
  /* AC SOURCE FREQUENCY ACCURACY */
  int this_frequency;
  double cntr_actual_frequency;
  test_result_type result;

  open_cntr(mod_id);
  open_mux(mod_id);

  aconnect_cntr_stop_af2(mod_id);

  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL to AF2 relay */

  setup_cntr_trg_levels(mod_id, TRIGGER_LEVEL, TRIGGER_LEVEL);
  setup_cntr_function(mod_id, FREQUENCY);
  setup_cntr_sample_size(mod_id, SAMPLE_SIZE);
  setup_cntr_md4(mod_id);

  for (this_frequency = 0;
       this_frequency < NUM_FREQUENCIES_TO_TEST;
       this_frequency++)
  {  /* for frequency */
    vth_asrc_sel_fltr(mod_id, FILTER__[this_frequency]);
    vth_ascat_cpu_init(mod_id);
    vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
    vth_ascat_set_a_freq(mod_id, FREQ_A__[this_frequency]);
    vth_ascat_en_cont_src(mod_id);
    vth_ascat_en_frun_src(mod_id);
    vth_ascat_run_en(mod_id);
    vth_asrc_dis_30ma_ilim(mod_id);
    vth_asrc_set_ampl(mod_id, RMS_VOLTS, DC_OFFSET);
    vth_afc_en_src_out(mod_id);

    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    trigger_md4(mod_id, FREQUENCY);
    flush_commands(mod_id);
    wait_for_counter_measurement(mod_id);
    cntr_actual_frequency = read_cntr_mean(mod_id);
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = cntr_actual_frequency;
    log_result(mod_id, result);

  }  /* for frequency */


  vth_afc_dis_src_out(mod_id);

  vth_ainit(mod_id);

  close_cntr(mod_id);
  close_mux(mod_id);

}  /* t9365 */
#undef NUM_FREQUENCIES_TO_TEST
#undef SAMPLE_SIZE
#undef F128
#undef F1024
#undef F8192
#undef RMS_VOLTS
#undef DC_OFFSET
#undef TRIGGER_LEVEL
#undef SOURCE_WAIT_TIME

#define NUM_FREQUENCIES_TO_TEST  3
/* 0 selects frequency A */
/* No 30mA current compliance    */
/* No additional ouput resistance*/
/* Source enabled */
/* Source controlled by processor*/
#define SAMPLE_SIZE     1000
#define SIGMA_MULTIPLIER  6
#define F128            5
#define F1024           6
#define F8192           7
typedef int frequency_array___[NUM_FREQUENCIES_TO_TEST];
#define RMS_VOLTS       3.5e+00
#define DC_OFFSET       0.0e+00
#define TRIGGER_LEVEL   0.0e+00
/*seconds*/
#define SOURCE_WAIT_TIME  1.0e-03
const frequency_array___ FREQ_A___ = {128, 1024, 8192};
const frequency_array___ FILTER___ = {F128, F1024, F8192};
void t9366(int mod_id, int /*slot*/)
{
  /* SOURCE JITTER MEASUREMENT USING SINE WAVE */
int this_frequency;

  double actual_sigma;
  double cntr_actual_min;
  double cntr_actual_max;
  double cntr_max_minus_min;

  test_result_type result;


  open_cntr(mod_id);
  open_mux(mod_id);

  aconnect_cntr_stop_af2(mod_id);

  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL to AF2 relay */

  setup_cntr_trg_levels(mod_id, TRIGGER_LEVEL, TRIGGER_LEVEL);
  setup_cntr_function(mod_id, PERIOD);
  setup_cntr_sample_size(mod_id, SAMPLE_SIZE);
  setup_cntr_md4(mod_id);

  for (this_frequency = 0;
       this_frequency < NUM_FREQUENCIES_TO_TEST;
       this_frequency++)
  {  /* for frequency */
    vth_asrc_sel_fltr(mod_id, FILTER___[this_frequency]);
    vth_ascat_cpu_init(mod_id);
    vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
    vth_ascat_set_a_freq(mod_id, FREQ_A___[this_frequency]);
    vth_ascat_en_cont_src(mod_id);
    vth_ascat_en_frun_src(mod_id);
    vth_ascat_run_en(mod_id);
    vth_asrc_dis_30ma_ilim(mod_id);
    vth_asrc_set_ampl(mod_id, RMS_VOLTS, DC_OFFSET);
    vth_afc_en_src_out(mod_id);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

    break_test_for_function_step_compatability(mod_id);
    trigger_md4(mod_id, PERIOD);
    flush_commands(mod_id);
    wait_for_counter_measurement(mod_id);
    actual_sigma = read_cntr_sigma(mod_id) * SIGMA_MULTIPLIER;

    cntr_actual_min = read_cntr_min(mod_id);
    cntr_actual_max = read_cntr_max(mod_id);
    cntr_max_minus_min = cntr_actual_max - cntr_actual_min;

    result.Measurement_class = CLASS_REAL;
    if (cntr_max_minus_min > actual_sigma)
      result.UU.Real_result = cntr_max_minus_min;
    else
      result.UU.Real_result = actual_sigma;
    log_result(mod_id, result);

  }  /* for frequency */

  vth_afc_dis_src_out(mod_id);

  close_cntr(mod_id);
  close_mux(mod_id);

}  /* t9366 */
#undef NUM_FREQUENCIES_TO_TEST
#undef SAMPLE_SIZE
#undef SIGMA_MULTIPLIER
#undef F128
#undef F1024
#undef F8192
#undef RMS_VOLTS
#undef DC_OFFSET
#undef TRIGGER_LEVEL
#undef SOURCE_WAIT_TIME


#define NUM_FREQUENCIES_TO_TEST  3
/* No 30mA current compliance    */
/* No additional ouput resistance*/
/* Source enabled */
/* Source controlled by processor*/
#define SAMPLE_SIZE     100
#define SIGMA_MULTIPLIER  6
#define F128            5
#define F1024           6
#define F8192           7
#define SALES_FUNC            PER
#define MODE            DCV
#define LOW             0.8
#define HIGH            1.2
typedef int frequency_array____[NUM_FREQUENCIES_TO_TEST];
typedef double res_array[NUM_FREQUENCIES_TO_TEST];
#define RMS_VOLTS       7.071
#define DC_OFFSET       0.0e+00
/*seconds*/
#define SOURCE_WAIT_TIME  1.0e-03
const frequency_array____ FREQ_A____ = {128, 1024, 8192};
const frequency_array____ FILTER____ = {F128, F1024, F8192};
const res_array RES = {0.0001, 0.001, 0.001};
void t9367(int mod_id, int /*slot*/)
{
  /* PRECISION AC SOURCE FREQUENCY ACCURACY AND JITTER
                       MEASUREMENT  */
  /* the res for 128 should be 0.001, but the 3458 has a bug that errors   */
  /* if you use that gate time.  once this is fixed it should be converted */
  /* since the 128 Hz takes nearly 2 minutes to complete. The same is true */
  /* for 8192 Hz.  Eventually gate time for it should be 0.001 => 0.01     */

  int this_frequency;
  double sigma;
  double largest;
  double smallest;
  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K837);   /* SRCOUT to RCAL2 relay */
  vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  setup_dmm_fsource(mod_id, MODE);
  setup_dmm_level(mod_id, 0, DMM_DC);
  setup_dmm_level_filter(mod_id, true);
  hold_dmm(mod_id);
  setup_dmm_readings(mod_id, SAMPLE_SIZE);
  enable_dmm_buffer(mod_id);

  for (this_frequency = 0;
       this_frequency < NUM_FREQUENCIES_TO_TEST;
       this_frequency++)
  {  /* for frequency */
    setup_dmm(mod_id, SALES_FUNC, RMS_VOLTS, RES[this_frequency]);
    setup_dmm_memory(mod_id, DREAL, FIFO);
    setup_dmm_ac_band(mod_id, round(LOW * FREQ_A____[this_frequency]),
                      round(HIGH * FREQ_A____[this_frequency]));
    vth_asrc_sel_fltr(mod_id, FILTER____[this_frequency]);
    vth_ascat_cpu_init(mod_id);
    vth_ascat_sel_freq(mod_id, FREQA);
    vth_ascat_set_a_freq(mod_id, FREQ_A____[this_frequency]);
    vth_ascat_en_cont_src(mod_id);
    vth_ascat_en_frun_src(mod_id);
    vth_ascat_run_en(mod_id);
    vth_asrc_dis_30ma_ilim(mod_id);
    vth_asrc_set_ampl(mod_id, RMS_VOLTS, DC_OFFSET);
    vth_afc_en_src_out(mod_id);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

    break_test_for_function_step_compatability(mod_id);
    trigger_dmm(mod_id);
    flush_commands(mod_id);
    wait_for_dmm_measurement(mod_id);
    calculate_dmm_stats(mod_id);

    /* Read and log the mean */
    read_dmm_stats(mod_id, MEAN);
    if (!get_result_nolog(mod_id, &result))
    {
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = 0.0;
      display_prompt("No result in result log (Expected DMM mean)", true);
    }
    /* convert period to frequency - 01.30.91 JRM */
    /* added check for divide by 0 - 12.11.91 JRM */
    if (result.UU.Real_result != 0.0)
      result.UU.Real_result = 1 / result.UU.Real_result;
    else
      result.UU.Real_result = 0.0;
    log_result(mod_id, result);

    /* Calculate and log the jitter */
    read_dmm_stats(mod_id, SDEV);
    if (get_result_nolog(mod_id, &result))
      sigma = result.UU.Real_result;
    else
    {
      sigma = 9.999e9;
      display_prompt("No result in result log (Expected DMM sdev)", true);
    }

    read_dmm_stats(mod_id, SALES_MIN);
    if (get_result_nolog(mod_id, &result))
      smallest = result.UU.Real_result;
    else
    {
      smallest = -9.999e9;
      display_prompt("No result in result log (Expected DMM min)", true);
    }

    read_dmm_stats(mod_id, SALES_MAX);
    if (get_result_nolog(mod_id, &result))
      largest = result.UU.Real_result;
    else
    {
      largest = 9.999e9;
      display_prompt("No result in result log (Expected DMM max)", true);
    }

    sigma *= SIGMA_MULTIPLIER;

    result.Measurement_class = CLASS_REAL;
    if (largest - smallest > sigma)
      result.UU.Real_result = largest - smallest;
    else
      result.UU.Real_result = sigma;
    log_result(mod_id, result);

  }  /* for frequency */

  vth_afc_dis_src_out(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9367 */
#undef NUM_FREQUENCIES_TO_TEST
#undef SAMPLE_SIZE
#undef SIGMA_MULTIPLIER
#undef F128
#undef F1024
#undef F8192
#undef SALES_FUNC
#undef MODE
#undef LOW
#undef HIGH
#undef RMS_VOLTS
#undef DC_OFFSET
#undef SOURCE_WAIT_TIME


/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* unenhanced */
#define MEAS_TYPE       0
#define DEFAULT_SOURCE_RESISTANCE  0
#define NUM_FREQUENCIES_TO_TEST  3
#define STOP_FREQ       20000
/* dmm range headroom */
#define FUDGE_FACTOR    1.2e+00
#define RMS_TEST_VOLTAGE  100e-03
#define DC_OFFSET       0e+00
#define LOW_BAND_RATIO  0.8e+00
#define RESOLUTION      3.0e-06
#define DMM_FUNCTION    ACV
typedef int frequency_array_____[NUM_FREQUENCIES_TO_TEST];
#define F128            5
#define F1024           6
#define F8192           7
const frequency_array_____ FILTER_____ = {F128, F1024, F8192};
const frequency_array_____ FREQ_A_____ = {128, 1024, 8192};
void t9370(int mod_id, int /*slot*/)
{
  /* AC SOURCE OUTPUT RESISTANCE */
  int start_freq;
  double r10;
  double five_tau;
  double dmm_voltage_range;
  double dmm_no_load_voltage;
  double dmm_loaded_voltage;
  double source_resistance;

  test_result_type result;



  open_dmm(mod_id);
  /* -------------------------------------------------------------------- */
  /* Modified: 12/7/90 JRM BDB ------------------------------------------ */
  /* Added hold_dmm because of changes from measure_dmm to wait for meas  */
  /* complete (because of LID firmware changes).                          */
  /* -------------------------------------------------------------------- */
  hold_dmm(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  dmm_voltage_range = RMS_TEST_VOLTAGE * sqrt(2.0) * FUDGE_FACTOR;
  setup_dmm(mod_id, DMM_FUNCTION, dmm_voltage_range, RESOLUTION);
  setup_dmm_level_filter(mod_id, true);
  enable_dmm_buffer(mod_id);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K837);   /* SRCOUT to RCAL2 relay */
  vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  for (int frequency = 0; frequency < NUM_FREQUENCIES_TO_TEST; ++frequency)
  {  /*for frequency*/
    five_tau = 5.0 / FREQ_A_____[frequency];
    start_freq = static_cast<int>(FREQ_A_____[frequency] * LOW_BAND_RATIO);
    setup_dmm_ac_band(mod_id, start_freq, STOP_FREQ);

    vth_asrc_sel_fltr(mod_id, FILTER_____[frequency]);
    vth_ascat_cpu_init(mod_id);
    vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
    vth_ascat_set_a_freq(mod_id, FREQ_A_____[frequency]);
    vth_ascat_en_cont_src(mod_id);
    vth_ascat_en_frun_src(mod_id);
    vth_ascat_run_en(mod_id);
    vth_asrc_en_30ma_ilim(mod_id);
    vth_asrc_set_ampl(mod_id, RMS_TEST_VOLTAGE, DC_OFFSET);
    vth_afc_en_src_out(mod_id);

    vth_mtmr_wait(mod_id, five_tau);

    /* -------------------------------------------------------------------- */
    /* Modified: 12/7/90 JRM BDB ------------------------------------------ */
    /* Changed from measure_dmm to wait for dmm complete because of LID     */
    /* changes in firmware.                                                 */
    /* -------------------------------------------------------------------- */
    trigger_dmm(mod_id);
    flush_commands(mod_id);
    wait_for_dmm_measurement(mod_id);
    read_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
      dmm_no_load_voltage = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected dmm vth_reading)", true);
    if (dmm_no_load_voltage >= 1.0e37)
      display_prompt("DMM No Load Voltage Reading Error", true);
    vth_afc_dis_src_out(mod_id);
    vth_mtmr_wait(mod_id, five_tau);

    vth_aclose_rly(mod_id, K826);   /*SRCOUT to REFR2*/
    vth_aclose_rly(mod_id, K845);   /*REFR2 to 10 ohm to I bus*/
    vth_aclose_rly(mod_id, K855);   /*I bus to CALIB2*/

    vth_afc_en_src_out(mod_id);
    vth_mtmr_wait(mod_id, five_tau);

    /* -------------------------------------------------------------------- */
    /* Modified: 12/7/90 JRM BDB ------------------------------------------ */
    /* Changed from measure_dmm to wait for dmm complete because of LID     */
    /* changes in firmware.                                                 */
    /* -------------------------------------------------------------------- */
    trigger_dmm(mod_id);
    flush_commands(mod_id);
    wait_for_dmm_measurement(mod_id);
    read_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
    {  /*measure the loaded voltage, calculate resistance and log*/
      dmm_loaded_voltage = result.UU.Real_result;
      if (dmm_loaded_voltage >= 1.0e37)
        display_prompt("DMM Loaded Voltage Reading Error", true);
      if (dmm_loaded_voltage != 0)
        source_resistance = (dmm_no_load_voltage - dmm_loaded_voltage) /
                            dmm_loaded_voltage * r10;
      else
        source_resistance = DEFAULT_SOURCE_RESISTANCE;
      result.UU.Real_result = source_resistance;
      log_result(mod_id, result);
    }  /*measure the loaded voltage, calculate resistance and log*/
    else
      display_prompt("No result in result log (Expected dmm vth_reading)", true);

    vth_afc_dis_src_out(mod_id);

    vth_aopen_rly(mod_id, K826);
    vth_aopen_rly(mod_id, K845);
    vth_aopen_rly(mod_id, K855);

  }  /*for frequency*/

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9370 */
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef SRC_ON
#undef SRC_CONTROL
#undef DEFAULT_SOURCE_RESISTANCE
#undef NUM_FREQUENCIES_TO_TEST
#undef STOP_FREQ
#undef FUDGE_FACTOR
#undef RMS_TEST_VOLTAGE
#undef DC_OFFSET
#undef LOW_BAND_RATIO
#undef RESOLUTION
#undef DMM_FUNCTION
#undef F128
#undef F1024
#undef F8192

#define NUM_FREQS_TO_TEST  3
#define NUM_VOLTS_TO_TEST  2
typedef int freq_array[NUM_FREQS_TO_TEST];
typedef double volts_array[NUM_VOLTS_TO_TEST];
typedef double time_array[NUM_FREQS_TO_TEST];
#define DCSETOFF        0.0e+00
const freq_array TEST_FREQ = {128, 1024, 8192};
const freq_array FILTER______ = {FILTER_128, FILTER_1024, FILTER_8192};
const volts_array TEST_VOLT = {0.1e00, 1.0e00};
const time_array TEST_INTERVAL = {381.5e-6, 47.7e-6, 10.5e-6};
const time_array TEST_APERATURE = {100e-06, 12.0e-6, 1.4e-6};
const freq_array SAMPLE_SIZE = {2048, 2048, 1024};
#define XDB_PEAK        (-90.0)
#define XDB_HARMONIC    (-1.0)
#define MEAS_MODE       1   /* CALIB1/2 output */
void t9371(int mod_id, int /*slot*/)
{
  arrform waveform;
  arrform y;
  double thd;
  double sig_noise_ratio;
  double tsignal;
  double tharmonic;
//  FILE *ofile;
  int freq;
  int volt;
  int range;
  test_result_type result;
  int freq_elements;
  int dc_index;
  int peak_index;
  int peak_lo;
  int peak_hi;
  int lo_index;
  int hi_index;


//  ofile = NULL;
  /* connect DMM to aim source */
  open_dmm(mod_id);
  open_mux(mod_id);

  switch (MEAS_MODE)
  {

  case 1:
    connect_dmm(mod_id);
    connect_plus_calib1(mod_id);
    connect_minus_calib2(mod_id);
    vth_aclose_rly(mod_id, K837);   /* connect SRCOUT to RCAL2 */
    vth_aclose_rly(mod_id, K853);   /* connect CALIB1 to RCAL2 */
    vth_aclose_rly(mod_id, K854);   /* connect CALIB2 to GND */
    break;

  case 2:
    vth_aclose_rly(mod_id, K806);   /* connect SRCOUT to S */
    vth_aclose_rly(mod_id, K751);   /* connect A_XL to S */
    vth_aclose_rly(mod_id, K764);   /* connect AF2 to A_XL */
    break;

  case 3:
    vth_aclose_rly(mod_id, K837);   /* connect SRCOUT to RCAL2 */
    vth_aclose_rly(mod_id, K854);   /* connect CALIB2 to GND */
    vth_aclose_rly(mod_id, K853);   /* connect CALIB1 to RCAL2 */
    break;
  default:
    assert(false);
    break;

  }


  for (volt = 0; volt < NUM_VOLTS_TO_TEST; volt++)
  {
    for (freq = 0; freq < NUM_FREQS_TO_TEST; freq++)
    {  /* for frequency */
      freq_elements = SAMPLE_SIZE[freq] / 2;

      /* setup the source */
      vth_asrc_sel_fltr(mod_id, FILTER______[freq]);
      vth_ascat_cpu_init(mod_id);
      vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
      vth_ascat_set_a_freq(mod_id, TEST_FREQ[freq]);
      vth_ascat_en_cont_src(mod_id);
      vth_ascat_en_frun_src(mod_id);
      vth_ascat_run_en(mod_id);
      vth_asrc_en_30ma_ilim(mod_id);
      vth_asrc_set_ampl(mod_id, TEST_VOLT[volt], DCSETOFF);
      vth_afc_en_src_out(mod_id);

      /* choose appropriate range for DMM and noise_level for spectrum*/
      if (TEST_VOLT[volt] >= 0.707)
        range = 10;
      else
        range = 1;

      setup_dcv_dig(mod_id, range, TEST_INTERVAL[freq], SAMPLE_SIZE[freq],
                    TEST_APERATURE[freq]);
//      display_prompt("starting capture_ddm", true);
      capture_ddm(mod_id, SAMPLE_SIZE[freq], waveform, LEVEL);

/*
      int i;
      display_prompt("starting to write raw.y", true);
      ofile = fopen("raw.y", "w");
      for (i=0; i<SAMPLE_SIZE[freq]; i++)
        fprintf(ofile, "%f\n", waveform[i]);
      fclose(ofile);
      display_prompt("finished writing raw.y", true);
      display_prompt("staring waveform_to_magnitude", true);
      */

      waveform_to_magnitude(waveform, y, SAMPLE_SIZE[freq]);

/*     int i;
      display_prompt("starting to write mag.y", true);
      ofile = fopen("mag.y", "w");
      for (i=0; i<freq_elements; i++)
        fprintf(ofile, "%f\n", y[i]);
      fclose(ofile);
      display_prompt("finished writing mag.y", true);
      */

//      ofile = fopen("values.y", "w");
      dc_index = find_dc_index(freq_elements, y);
//      fprintf(ofile, "dc_index = %d\n", dc_index);

      peak_index = find_peak_index(dc_index, freq_elements, y);
//      fprintf(ofile, "peak_index = %d\n", peak_index);

      find_local_minima(dc_index, freq_elements, peak_index, y, XDB_PEAK,
                        &lo_index, &hi_index);
//      fprintf(ofile, "lo_index = %d\n", lo_index);
//      fprintf(ofile, "hi_index = %d\n", hi_index);

      tsignal = sum_power(lo_index, hi_index - 1, y);


      peak_lo = find_peak_index(dc_index, lo_index, y);
//      fprintf(ofile, "peak_lo = %d\n", peak_lo);
      peak_hi = find_peak_index(hi_index, freq_elements, y);
//      fprintf(ofile, "peak_hi = %d\n", peak_hi);
//      fclose(ofile);
      if (y[peak_lo] > y[peak_hi])
        sig_noise_ratio = 20.0 * log10(y[peak_index] / y[peak_lo]);
      else
        sig_noise_ratio = 20.0 * log10(y[peak_index] / y[peak_hi]);
/*
      if (y[peak_lo - 1] > y[peak_hi - 1])
        sig_noise_ratio = 20.0 * log10(y[peak_index - 1] / y[peak_lo - 1]);
      else
        sig_noise_ratio = 20.0 * log10(y[peak_index - 1] / y[peak_hi - 1]);
        */
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = sig_noise_ratio;
      log_result(mod_id, result);

      tharmonic = find_harmonic_power(peak_index, freq_elements, y,
                                      XDB_HARMONIC);

      thd = 10.0 * log10(tsignal / tharmonic);
      result.UU.Real_result = thd;
      log_result(mod_id, result);

      /* display_prompt('about to rewite res.y',true);*/
      /****/
      /* flush_commands(mod_id);*/
      /****/
      /* rewrite(ofile,'res.y');*/
      /****/
      /* vth_writeln(ofile,'DC Index ', dc_index:6);*/
      /****/
      /* vth_writeln(ofile,'Fundamental Frequency ',((peak_index-1)/test_interval[freq]/
              ( sample_size[freq]):18:8);*/
      /****/
      /* vth_writeln(ofile,'Minimum Signal to Noise Ratio dB ',sig_noise_ratio:18:8);*/
      /****/
      /* vth_writeln(ofile,'Total Signal Power ',Tsignal:18:8);*/
      /****/
      /* vth_writeln(ofile,'Total Harmonic Noise Power ',Tharmonic:18:8);*/
      /****/
      /* vth_writeln(ofile,'Total Harmonic Distortion in dB ', Thd:18:8);*/
      /****/
      /* close(ofile,'SAVE');*/
      /****/
      /* display_prompt('res.y closed',true);*/
      /****/

      vth_afc_dis_src_out(mod_id);

    }  /* for voltage and frequency loops */
  }

  close_dmm(mod_id);
  close_mux(mod_id);

//  if (ofile != NULL)
//    fclose(ofile);
}
#undef NUM_FREQS_TO_TEST
#undef NUM_VOLTS_TO_TEST
#undef DCSETOFF
#undef XDB_PEAK
#undef XDB_HARMONIC
#undef MEAS_MODE

/////////////////////////////////// Test T9372 added for NASRU - EQUAL TO T9371////////////////// 
#define NUM_FREQS_TO_TEST  3
#define NUM_VOLTS_TO_TEST  2
typedef int freq_array[NUM_FREQS_TO_TEST];
typedef double volts_array[NUM_VOLTS_TO_TEST];
typedef double time_array[NUM_FREQS_TO_TEST];
#define DCSETOFF        0.0e+00
const freq_array TEST_FREQ_NASRU = {128, 1024, 8192};
const freq_array FILTER______NASRU = {FILTER_128, FILTER_1024, FILTER_8192};
const volts_array TEST_VOLT_NASRU = {0.1e00, 1.0e00};
const time_array TEST_INTERVAL_NASRU = {381.5e-6, 47.7e-6, 10.5e-6};
const time_array TEST_APERATURE_NASRU = {100e-06, 12.0e-6, 1.4e-6};
const freq_array SAMPLE_SIZE_NASRU = {2048, 2048, 1024};
#define XDB_PEAK        (-90.0)
#define XDB_HARMONIC    (-1.0)
#define MEAS_MODE       1   /* CALIB1/2 output */
void t9372(int mod_id, int /*slot*/)
{
  arrform waveform;
  arrform y;
  double thd;
  double sig_noise_ratio;
  double tsignal;
  double tharmonic;
//  FILE *ofile;
  int freq;
  int volt;
  int range;
  test_result_type result;
  int freq_elements;
  int dc_index;
  int peak_index;
  int peak_lo;
  int peak_hi;
  int lo_index;
  int hi_index;


//  ofile = NULL;
  /* connect DMM to aim source */
  open_dmm(mod_id);
  open_mux(mod_id);

  switch (MEAS_MODE)
  {

  case 1:
    connect_dmm(mod_id);
    connect_plus_calib1(mod_id);
    connect_minus_calib2(mod_id);
    vth_aclose_rly(mod_id, K837);   /* connect SRCOUT to RCAL2 */
    vth_aclose_rly(mod_id, K853);   /* connect CALIB1 to RCAL2 */
    vth_aclose_rly(mod_id, K854);   /* connect CALIB2 to GND */
    break;

  case 2:
    vth_aclose_rly(mod_id, K806);   /* connect SRCOUT to S */
    vth_aclose_rly(mod_id, K751);   /* connect A_XL to S */
    vth_aclose_rly(mod_id, K764);   /* connect AF2 to A_XL */
    break;

  case 3:
    vth_aclose_rly(mod_id, K837);   /* connect SRCOUT to RCAL2 */
    vth_aclose_rly(mod_id, K854);   /* connect CALIB2 to GND */
    vth_aclose_rly(mod_id, K853);   /* connect CALIB1 to RCAL2 */
    break;
  default:
    assert(false);
    break;

  }


  for (volt = 0; volt < NUM_VOLTS_TO_TEST; volt++)
  {
    for (freq = 0; freq < NUM_FREQS_TO_TEST; freq++)
    {  /* for frequency */
      freq_elements = SAMPLE_SIZE_NASRU[freq] / 2;

      /* setup the source */
      vth_asrc_sel_fltr(mod_id, FILTER______NASRU[freq]);
      vth_ascat_cpu_init(mod_id);
      vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
      vth_ascat_set_a_freq(mod_id, TEST_FREQ_NASRU[freq]);
      vth_ascat_en_cont_src(mod_id);
      vth_ascat_en_frun_src(mod_id);
      vth_ascat_run_en(mod_id);
      vth_asrc_en_30ma_ilim(mod_id);
      vth_asrc_set_ampl(mod_id, TEST_VOLT_NASRU[volt], DCSETOFF);
      vth_afc_en_src_out(mod_id);

      /* choose appropriate range for DMM and noise_level for spectrum*/
      if (TEST_VOLT_NASRU[volt] >= 0.707)
        range = 10;
      else
        range = 1;

      setup_dcv_dig(mod_id, range, TEST_INTERVAL_NASRU[freq], SAMPLE_SIZE_NASRU[freq],
                    TEST_APERATURE_NASRU[freq]);
//      display_prompt("starting capture_ddm", true);
      capture_ddm(mod_id, SAMPLE_SIZE_NASRU[freq], waveform, LEVEL);

/*
      int i;
      display_prompt("starting to write raw.y", true);
      ofile = fopen("raw.y", "w");
      for (i=0; i<SAMPLE_SIZE[freq]; i++)
        fprintf(ofile, "%f\n", waveform[i]);
      fclose(ofile);
      display_prompt("finished writing raw.y", true);
      display_prompt("staring waveform_to_magnitude", true);
      */

      waveform_to_magnitude(waveform, y, SAMPLE_SIZE_NASRU[freq]);

/*     int i;
      display_prompt("starting to write mag.y", true);
      ofile = fopen("mag.y", "w");
      for (i=0; i<freq_elements; i++)
        fprintf(ofile, "%f\n", y[i]);
      fclose(ofile);
      display_prompt("finished writing mag.y", true);
      */

//      ofile = fopen("values.y", "w");
      dc_index = find_dc_index(freq_elements, y);
//      fprintf(ofile, "dc_index = %d\n", dc_index);

      peak_index = find_peak_index(dc_index, freq_elements, y);
//      fprintf(ofile, "peak_index = %d\n", peak_index);

      find_local_minima(dc_index, freq_elements, peak_index, y, XDB_PEAK,
                        &lo_index, &hi_index);
//      fprintf(ofile, "lo_index = %d\n", lo_index);
//      fprintf(ofile, "hi_index = %d\n", hi_index);

      tsignal = sum_power(lo_index, hi_index - 1, y);


      peak_lo = find_peak_index(dc_index, lo_index, y);
//      fprintf(ofile, "peak_lo = %d\n", peak_lo);
      peak_hi = find_peak_index(hi_index, freq_elements, y);
//      fprintf(ofile, "peak_hi = %d\n", peak_hi);
//      fclose(ofile);
      if (y[peak_lo] > y[peak_hi])
        sig_noise_ratio = 20.0 * log10(y[peak_index] / y[peak_lo]);
      else
        sig_noise_ratio = 20.0 * log10(y[peak_index] / y[peak_hi]);
/*
      if (y[peak_lo - 1] > y[peak_hi - 1])
        sig_noise_ratio = 20.0 * log10(y[peak_index - 1] / y[peak_lo - 1]);
      else
        sig_noise_ratio = 20.0 * log10(y[peak_index - 1] / y[peak_hi - 1]);
        */
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = sig_noise_ratio;
      log_result(mod_id, result);

      tharmonic = find_harmonic_power(peak_index, freq_elements, y,
                                      XDB_HARMONIC);

      thd = 10.0 * log10(tsignal / tharmonic);
      result.UU.Real_result = thd;
      log_result(mod_id, result);

      /* display_prompt('about to rewite res.y',true);*/
      /****/
      /* flush_commands(mod_id);*/
      /****/
      /* rewrite(ofile,'res.y');*/
      /****/
      /* vth_writeln(ofile,'DC Index ', dc_index:6);*/
      /****/
      /* vth_writeln(ofile,'Fundamental Frequency ',((peak_index-1)/test_interval[freq]/
              ( sample_size[freq]):18:8);*/
      /****/
      /* vth_writeln(ofile,'Minimum Signal to Noise Ratio dB ',sig_noise_ratio:18:8);*/
      /****/
      /* vth_writeln(ofile,'Total Signal Power ',Tsignal:18:8);*/
      /****/
      /* vth_writeln(ofile,'Total Harmonic Noise Power ',Tharmonic:18:8);*/
      /****/
      /* vth_writeln(ofile,'Total Harmonic Distortion in dB ', Thd:18:8);*/
      /****/
      /* close(ofile,'SAVE');*/
      /****/
      /* display_prompt('res.y closed',true);*/
      /****/

      vth_afc_dis_src_out(mod_id);

    }  /* for voltage and frequency loops */
  }

  close_dmm(mod_id);
  close_mux(mod_id);

//  if (ofile != NULL)
//    fclose(ofile);
}
#undef NUM_FREQS_TO_TEST
#undef NUM_VOLTS_TO_TEST
#undef DCSETOFF
#undef XDB_PEAK
#undef XDB_HARMONIC
#undef MEAS_MODE


////////////////////////////////////END OF T9372////////////////////////////////////////////////

#define NUM_F1S_TO_TEST  4
#define NUM_F2S_TO_TEST  3
#define TOLERANCE       0.05   /* acceptable tolerance from nominal   */
#define BOGUS_VALUE     (-99.9)
    /* bogus value logged for invalid responses */
typedef int freq1_array[NUM_F1S_TO_TEST];
typedef int freq2_array[NUM_F2S_TO_TEST];
typedef double f2_real[NUM_F2S_TO_TEST];
typedef f2_real time_array_[NUM_F1S_TO_TEST];
typedef freq2_array sample_array[NUM_F1S_TO_TEST];
#define DCSETOFF        0.0e+00
const freq1_array TESTF1 = {697, 770, 852, 941};
const freq2_array TESTF2 = {1209, 1336, 1477};
#define RANGE           1
const time_array_ SAMPLE_RATE =
{
  {82.0e-6, 76.4e-6, 64.8e-6},
  {72.3e-6, 74.2e-6, 69.1e-6},
  {53.3e-6, 63.6e-6, 64.8e-6},
  {49.3e-6, 51.9e-6, 51.9e-6}
};

#define APERATURE       12.0e-6

const sample_array SAMPLES =
{
  {2048, 2048, 4096},
  {2048, 2048, 2048},
  {2048, 2048, 2048},
  {2048, 2048, 2048}
};

#define XDB             (-75.0)
#define VOLTS_          0.3e+00   /* AC voltage output*/
#define MEAS_MODE       1   /* output through minerva */
/*a*/
/****/
/* lines marked by this are temporary diagnostic lines which can be */
/****/
/* removed and not effect the operation of the test                 */
void t9380(int mod_id, int /*slot*/)
{
  double f1_freq;
  double f2_freq;
  boolean bogus;
  int dc_index;
  int mid_index;
  int peak1;
  int peak2;
  int lo1_index;
  int lo2_index;
  int hi1_index;
  int hi2_index;
  int i;
  int j;
  arrform waveform;
  arrform y;
  double tsignal;
  double tnoise;
  double t1signal;
  double t2signal;
  int freq_elements;
  double tid;
  double freq_span;
  double signal_noise_ratio;
  double lower_mag;
  int lf_peak;
  int mf_peak;
  int hf_peak;
  test_result_type result;
//  FILE *ofile;

  open_dmm(mod_id);
  open_mux(mod_id);

  switch (MEAS_MODE)
  {

  case 1:
    connect_dmm(mod_id);
    connect_plus_calib1(mod_id);
    connect_minus_calib2(mod_id);
    vth_aclose_rly(mod_id, K837);   /* connect SRCOUT to RCAL2 */
    vth_aclose_rly(mod_id, K853);   /* connect RCAL2 to CALIB1 */
    vth_aclose_rly(mod_id, K854);   /* connect CALIB2 to GND */
    break;

  case 2:
    vth_aclose_rly(mod_id, K806);   /* connect SRCOUT to S */
    vth_aclose_rly(mod_id, K751);   /* connect S to A_XL   */
    vth_aclose_rly(mod_id, K764);   /* connect AF2 to A_XL */
    break;

  case 3:
    vth_aclose_rly(mod_id, K837);   /* connect SRCOUT to RCAL2 */
    vth_aclose_rly(mod_id, K854);   /* connect CALIB2 to GND */
    vth_aclose_rly(mod_id, K853);   /* connect CALIB1 to RCAL2 */
    break;
  default:
    assert(false);
    break;

  }/* case */

  for (i = 0; i < NUM_F1S_TO_TEST; i++)
  {
    for (j = 0; j < NUM_F2S_TO_TEST; j++)
    {
      bogus = false;
      freq_span = 1.0 / SAMPLES[i][j] / SAMPLE_RATE[i][j];
      freq_elements = SAMPLES[i][j] / 2;
      mid_index = round((TESTF1[i] + TESTF2[j]) / 2.0 / freq_span + 1);

      /* choose and enable 20khz filter for output */
      vth_asrc_sel_fltr(mod_id, FILTER_20K);
      vth_ascat_cpu_init(mod_id);
      vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
      vth_ascat_set_a_freq(mod_id, TESTF1[i]);
      vth_ascat_en_cont_src(mod_id);
      vth_ascat_en_frun_src(mod_id);
      vth_ascat_run_en(mod_id);
      vth_asrc_en_30ma_ilim(mod_id);
      vth_asrc_set_ampl(mod_id, VOLTS_, DCSETOFF);
      vth_afc_en_src_out(mod_id);

      /* setup aim source for dual frequency mode */
      vth_ascat_set_a_freq(mod_id, TESTF1[i]);
      vth_ascat_set_b_freq(mod_id, TESTF2[j]);
      vth_asrc_set_ac_ampl(mod_id, VOLTS_);
      vth_ascat_en_dual_freq(mod_id);

      setup_dcv_dig(mod_id, RANGE, SAMPLE_RATE[i][j], SAMPLES[i]
                    [j], APERATURE);

      capture_ddm(mod_id, SAMPLES[i][j], waveform, LEVEL);

/*      int k;
      display_prompt("starting to write raw.y", true);
      ofile = fopen("raw.y", "w");
      for (k=0; k<SAMPLES[i][j]; k++)
        fprintf(ofile, "%d %f\n", k, waveform[k]);
      fclose(ofile);
      display_prompt("finished writing raw.y", true);

      */


      waveform_to_magnitude(waveform, y, SAMPLES[i][j]);
/*
      display_prompt("starting to write mag.y", true);
      ofile = fopen("mag.y", "w");
      for (k=0; k<freq_elements; k++)
        fprintf(ofile, "%d %f\n", k, y[k]);
      fclose(ofile);
      display_prompt("finished writing mag.y", true);
      */

//      ofile = fopen("values.y", "w");
      dc_index = find_dc_index(freq_elements, y);
//      fprintf(ofile, "dc_index = %d\n", dc_index);
      /* analyze the spectra as two spectra about the mid point between f1 and f2 */
      peak1 = find_peak_index(dc_index, mid_index, y);
//      fprintf(ofile, "peak1 = %d\n", peak1);
      peak2 = find_peak_index(mid_index, freq_elements, y);
//      fprintf(ofile, "peak2 = %d\n", peak2);
      find_local_minima(dc_index, mid_index, peak1, y, XDB, &lo1_index,
                        &hi1_index);
      find_local_minima(mid_index, freq_elements, peak2, y, XDB, &lo2_index,
                        &hi2_index);

/*      fprintf(ofile, "mid_index = %d\n", mid_index);
      fprintf(ofile, "lo1_index = %d\n", lo1_index);
      fprintf(ofile, "hi1_index = %d\n", hi1_index);
      fprintf(ofile, "lo2_index = %d\n", lo2_index);
      fprintf(ofile, "hi2_index = %d\n", hi2_index);
      fclose(ofile);
      */
      /* verify the proper frequencies were found and that their magnitudes match  */
      /* otherwise log bogus values for the minimium signal to noise ratio and Tid */

      f1_freq = freq_span * (peak1);
      f2_freq = freq_span * (peak2);
//      f1_freq = freq_span * (peak1 - 1);
//      f2_freq = freq_span * (peak2 - 1);


      if (f1_freq < (1 - TOLERANCE) * TESTF1[i] ||
          f1_freq > (1 + TOLERANCE) * TESTF1[i])
      {
        display_error(mod_id, "Frequency 1 out of tolerance");
        bogus = true;
      }

      if (f2_freq < (1 - TOLERANCE) * TESTF2[j] ||
          f2_freq > (1 + TOLERANCE) * TESTF2[j])
      {
        display_error(mod_id, "Frequency 2 out of tolerance");
        bogus = true;
      }

//mjs 0,1,4,5,18,19 passes
      if (y[peak1] / y[peak2] < 1 - TOLERANCE ||
          y[peak1] / y[peak2] > 1 + TOLERANCE)

//original
//      if (y[peak1 - 1] / y[peak2 - 1] < 1 - TOLERANCE ||
//          y[peak1 - 1] / y[peak2 - 1] > 1 + TOLERANCE)
      {
        display_error(mod_id,
                      "Amplitude matching of Freq 1 & 2 out of tolerance");
        bogus = true;
      }

      if (bogus)
      {
        result.Measurement_class = CLASS_REAL;
        result.UU.Real_result = BOGUS_VALUE;
        log_result(mod_id, result);
        log_result(mod_id, result);
      }

      else
      {  // valid signal
        lf_peak = find_peak_index(dc_index, lo1_index, y);
        mf_peak = find_peak_index(hi1_index, lo2_index, y);
        hf_peak = find_peak_index(hi2_index, freq_elements, y);
        if (y[peak1] > y[peak2])
          lower_mag = y[peak2 ];
        else
          lower_mag = y[peak1 ];
        if (y[lf_peak ] > y[mf_peak ])
        {
          if (y[lf_peak ] > y[hf_peak ])
            signal_noise_ratio = 20.0 * log10(lower_mag / y[lf_peak ]);
          else
            signal_noise_ratio = 20.0 * log10(lower_mag / y[hf_peak ]);
        }
        else
        {
          if (y[mf_peak ] > y[hf_peak ])
            signal_noise_ratio = 20.0 * log10(lower_mag / y[mf_peak ]);
          else
            signal_noise_ratio = 20.0 * log10(lower_mag / y[hf_peak ]);
        }
/*      else
        {  // valid signal
          lf_peak = find_peak_index(dc_index, lo1_index, y);
          mf_peak = find_peak_index(hi1_index+1, lo2_index, y);
          hf_peak = find_peak_index(hi2_index+1, freq_elements, y);
          fprintf(ofile, "lf_peak = %d\n", lf_peak);
          fprintf(ofile, "mf_peak = %d\n", mf_peak);
          fprintf(ofile, "hf_peak = %d\n", hf_peak);
          fclose(ofile);

          if (y[peak1 - 1] > y[peak2 - 1])
            lower_mag = y[peak2 - 1];
          else
            lower_mag = y[peak1 - 1];

          if (y[lf_peak - 1] > y[mf_peak - 1])
          {
            if (y[lf_peak - 1] > y[hf_peak - 1])
              signal_noise_ratio = 20.0 * log10(lower_mag / y[lf_peak - 1]);
            else
              signal_noise_ratio = 20.0 * log10(lower_mag / y[hf_peak - 1]);
          }
          else
          {
            if (y[mf_peak - 1] > y[hf_peak - 1])
              signal_noise_ratio = 20.0 * log10(lower_mag / y[mf_peak - 1]);
            else
              signal_noise_ratio = 20.0 * log10(lower_mag / y[hf_peak - 1]);
          }
        }*/
        result.Measurement_class = CLASS_REAL;
        result.UU.Real_result = signal_noise_ratio;
        log_result(mod_id, result);

        t1signal = sum_power(lo1_index+1, hi1_index+1, y);
        t2signal = sum_power(lo2_index+1, hi2_index+1, y);
        tsignal = t1signal + t2signal;
        tnoise = sum_power(dc_index+1, lo1_index+1, y);
        tnoise += sum_power(hi1_index+1, lo2_index+1, y);
        tnoise += sum_power(hi2_index+1, freq_elements+1, y);

/*        t1signal = sum_power(lo1_index, hi1_index - 1, y);
        t2signal = sum_power(lo2_index, hi2_index - 1, y);
        tsignal = t1signal + t2signal;
        tnoise = sum_power(dc_index, lo1_index - 1, y);
        tnoise += sum_power(hi1_index, lo2_index - 1, y);
        tnoise += sum_power(hi2_index, freq_elements, y);
        */
        tid = 10.0 * log10(tsignal / tnoise);
        result.Measurement_class = CLASS_REAL;
        result.UU.Real_result = tid;
        log_result(mod_id, result);

        vth_afc_dis_src_out(mod_id);

      }  /* valid signal */
    }  /* for frequency loops */
  }

  close_dmm(mod_id);
  close_mux(mod_id);
  /* valid signals, now find the minimum signal/noise ratio over the entire range */
}
#undef NUM_F1S_TO_TEST
#undef NUM_F2S_TO_TEST
#undef TOLERANCE
#undef BOGUS_VALUE
#undef DCSETOFF
#undef RANGE
#undef APERATURE
#undef XDB
#undef VOLTS_
#undef MEAS_MODE

#define NUM_FREQS_TO_TEST  5
typedef int freq_array_[NUM_FREQS_TO_TEST];
typedef double time_array__[NUM_FREQS_TO_TEST];
#define DCSETOFF        0.0e+00
const freq_array_ TEST_FREQ_ = {10, 100, 1000, 10000, 20000};
#define TEST_VOLT_      7.071   /* Vrms 10V peak */
#define DCV_RANGE       10
const time_array__ TEST_INTERVAL_ = {2.4414e-3, 483.4e-6, 45.9e-6, 4.98e-6, 2.49e-6};
const time_array__ TEST_APERATURE_ = {1.0e-3, 100e-06, 12.0e-6, 2e-9, 2e-9};
const freq_array_ SAMPLE_SIZE_ = {1024, 2048, 2048, 2048, 2048};
#define XDB_PEAK        (-80.0)
#define XDB_HARMONIC    (-1.0)
#define MEAS_MODE       1   /* CALIB1/2 output */
/*a*/
void t9383(int mod_id, int /*slot*/)
{
  arrform waveform;
  arrform y;
  double thd;
  double sec_har_diff;
  double tsignal;
  double tharmonic;
//  FILE *ofile;
  int freq;
  test_result_type result;
  int freq_elements;
  int dc_index;
  int peak_index;
  int peak_lo;
  int peak_hi;
  int lo_index;
  int hi_index;


  /* connect DMM to aim source */
  open_dmm(mod_id);
  open_mux(mod_id);

  switch (MEAS_MODE)
  {

  case 1:
    connect_dmm(mod_id);
    connect_plus_calib1(mod_id);
    connect_minus_calib2(mod_id);
    vth_aclose_rly(mod_id, K837);   /* connect SRCOUT to RCAL2 */
    vth_aclose_rly(mod_id, K853);   /* connect CALIB1 to RCAL2 */
    vth_aclose_rly(mod_id, K854);   /* connect CALIB2 to GND */
    break;

  case 2:
    vth_aclose_rly(mod_id, K806);   /* connect SRCOUT to S */
    vth_aclose_rly(mod_id, K751);   /* connect A_XL to S */
    vth_aclose_rly(mod_id, K764);   /* connect AF2 to A_XL */
    break;

  case 3:
    vth_aclose_rly(mod_id, K837);   /* connect SRCOUT to RCAL2 */
    vth_aclose_rly(mod_id, K854);   /* connect CALIB2 to GND */
    vth_aclose_rly(mod_id, K853);   /* connect CALIB1 to RCAL2 */
    break;
  default:
    assert(false);
    break;

  }


  for (freq = 0; freq < NUM_FREQS_TO_TEST; freq++)
  {  /* for frequency */
    freq_elements = SAMPLE_SIZE_[freq] / 2;

    /* setup the source */
    vth_asrc_sel_fltr(mod_id, FILTER_20K);
    vth_ascat_cpu_init(mod_id);
    vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
    vth_ascat_set_a_freq(mod_id, TEST_FREQ_[freq]);
    vth_ascat_en_cont_src(mod_id);
    vth_ascat_en_frun_src(mod_id);
    vth_ascat_run_en(mod_id);
    vth_asrc_en_30ma_ilim(mod_id);
    vth_asrc_set_ampl(mod_id, TEST_VOLT_, DCSETOFF);
    vth_afc_en_src_out(mod_id);

    if (TEST_INTERVAL_[freq] >= 10e-6)
      setup_dcv_dig(mod_id, DCV_RANGE, TEST_INTERVAL_[freq],
                    SAMPLE_SIZE_[freq], TEST_APERATURE_[freq]);
    else
      setup_ssdc_dig(mod_id, DCV_RANGE, TEST_INTERVAL_[freq],
                     SAMPLE_SIZE_[freq]);

    capture_ddm(mod_id, SAMPLE_SIZE_[freq], waveform, LEVEL);

/*      int i;
      display_prompt("starting to write raw.y", true);
      ofile = fopen("raw.y", "w");
      for (i=0; i<SAMPLE_SIZE[freq]; i++)
        fprintf(ofile, "%d %f\n", i, waveform[i]);
      fclose(ofile);
      display_prompt("finished writing raw.y", true);
      display_prompt("staring waveform_to_magnitude", true);
      */


    waveform_to_magnitude(waveform, y, SAMPLE_SIZE_[freq]);
/*    int i;
      display_prompt("starting to write mag.y", true);
      ofile = fopen("mag.y", "w");
      for (i=0; i<freq_elements; i++)
        fprintf(ofile, "%d %f\n", i, y[i]);
      fclose(ofile);
      display_prompt("finished writing mag.y", true);
      */



//    ofile = fopen("values.y", "w");
    dc_index = find_dc_index(freq_elements, y);
//    fprintf(ofile, "dc index = %d\n", dc_index);
    peak_index = find_peak_index(dc_index, freq_elements, y);
//    fprintf(ofile, "peak_index = %d\n", peak_index);
    find_local_minima(dc_index, freq_elements, peak_index, y, XDB_PEAK,
                      &lo_index, &hi_index);
//    fprintf(ofile, "lo index = %d\n", lo_index);
//    fprintf(ofile, "hi index = %d\n", hi_index);
    tsignal = sum_power(lo_index, hi_index - 1, y);
//    fprintf(ofile, "tsignal = %f\n", tsignal);
    peak_lo = find_peak_index(dc_index, lo_index, y);
//    fprintf(ofile, "peak_lo = %d\n", peak_lo);
    peak_hi = find_peak_index(hi_index, freq_elements, y);
//    fprintf(ofile, "peak_hi = %d\n", peak_hi);
    if (y[peak_lo] > y[peak_hi])
      sec_har_diff = 20.0 * log10(y[peak_index] / y[peak_lo]);
    else
      sec_har_diff = 20.0 * log10(y[peak_index] / y[peak_hi]);
//mjs changed took out the above (-1) correction factors for p2c
//    fprintf(ofile, "sec_har_diff = %f\n", sec_har_diff);
//    fclose(ofile);
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = sec_har_diff;
    log_result(mod_id, result);

    tharmonic = find_harmonic_power(peak_index, freq_elements, y,
                                    XDB_HARMONIC);

    thd = 10.0 * log10(tsignal / tharmonic);
    result.UU.Real_result = thd;
    log_result(mod_id, result);

    /*display_prompt('about to rewite res.y',true); */
    /*flush_commands(mod_id); */
    /*rewrite(ofile,'res.y'); */
    /*vth_writeln(ofile,'DC Index ', dc_index:6); */
    /*vth_writeln(ofile,'Fundamental Frequency ',((peak_index-1)/test_interval[freq]/
            sample_size[freq]):18:8); */
    /*vth_writeln(ofile,'Second Harmonic Difference in dB ',sec_har_diff:18:8); */
    /*vth_writeln(ofile,'Total Signal Power ',Tsignal:18:8); */
    /*vth_writeln(ofile,'Total Harmonic Noise Power ',Tharmonic:18:8); */
    /*vth_writeln(ofile,'Total Harmonic Distortion in dB ', Thd:18:8); */
    /*close(ofile,'SAVE'); */
    /*display_prompt('res.y closed',true); */

    vth_afc_dis_src_out(mod_id);

  }  /* for voltage and frequency loops */

  close_dmm(mod_id);
  close_mux(mod_id);

}
#undef NUM_FREQS_TO_TEST
#undef DCSETOFF
#undef TEST_VOLT_
#undef DCV_RANGE
#undef XDB_PEAK
#undef XDB_HARMONIC
#undef MEAS_MODE

#define OSC_CHANNEL     1
#define COUNT           5
#define COUPLING        1000000
#define ARB_FREQ        1024
/* No 30mA current compliance */
#define SRC_I_COMPLIANCE  1
/* No additional ouput resistance*/
#define SRC_R_OUT       0
/* Source enabled */
#define SRC_ON          1
/* Source controlled by processor*/
#define SRC_CONTROL     0
#define CHANNEL_RANGE   24e+00
#define CHANNEL_OFFSET  0e+00
#define TRIGGER_LEVEL   0e+00
#define PEAK_VOLTS      20e+00
#define POS_OFFSET      10e+00
#define NEG_OFFSET      (-10e+00)
#define DC_OFFSET       0e+00
#define NEG_VOLTS       (-8e+00)
#define POS_VOLTS       8e+00
#define TIMEBASE_RANGE  20e-06
#define TIMEBASE_OFFSET  0.0e+00
/* seconds */
/*DC = 20KHz filter*/
/* seconds */
#define AVERAGING       true
#define TIMEBASE_MODE   AUTOMATIC
/*a*/
void t9385(int mod_id, int /*slot*/)
{
  /*ARB SOURCE SQUARE WAVE RISE/FALL TIME*/
  double offset;
  double gain;
  double max_volts;
  double min_volts;
  double slew_time;
  double slew_voltage;
  double slew_rate;
  test_result_type result;

  open_osc(mod_id);
  open_mux(mod_id);

  setup_osc_channel(mod_id, OSC_CHANNEL, COUPLING, CHANNEL_RANGE,
                    CHANNEL_OFFSET);
  setup_osc_timebase(mod_id, AVERAGING, COUNT, TIMEBASE_RANGE,
                     TIMEBASE_OFFSET, TIMEBASE_MODE);
  offset = aosc_cal(mod_id, POS_OFFSET, NEG_OFFSET, &gain);

  aconnect_osc_ch1_af2(mod_id);


  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL to AF2 relay */

  vth_src_sqr_setup(mod_id, ARB_FREQ, PEAK_VOLTS, DC_OFFSET, SRC_I_COMPLIANCE,
                SRC_R_OUT, SRC_ON, SRC_CONTROL);

  break_test_for_function_step_compatability(mod_id);

  for (int edge = POS; edge >= NEG; --edge)
  {  /* for edge */
    setup_osc_trigger(mod_id, OSC_CHANNEL, edge, TRIGGER_LEVEL, AVERAGING);
    slew_rate = 0.0;
    slew_voltage = 0.0;
    slew_time = 1.0;

    trigger_osc(mod_id, OSC_CHANNEL, AVERAGING);

    if (edge == NEG)
      setup_osc_arb_markers(mod_id, edge, NEG_VOLTS, OSC_CHANNEL, edge,
                            POS_VOLTS, OSC_CHANNEL);
    else
    {
      measure_osc(mod_id, VMAX, POS);
      if (get_result_nolog(mod_id, &result))
      {  /*process result*/
        max_volts = result.UU.Real_result * gain + offset;
        result.UU.Real_result = max_volts;
        log_result(mod_id, result);
      }  /*process result*/
      else
        display_prompt("Unable to vth_read max volts from the oscilloscope.",
                       true);
      measure_osc(mod_id, VMAX, NEG);
      if (get_result_nolog(mod_id, &result))
      {  /*process result*/
        min_volts = result.UU.Real_result * gain + offset;
        result.UU.Real_result = min_volts;
        log_result(mod_id, result);
      }  /*process result*/
      else
        display_prompt("Unable to vth_read min volts from the oscilloscope.",
                       true);
      setup_osc_arb_markers(mod_id, edge, POS_VOLTS, OSC_CHANNEL, edge,
                            NEG_VOLTS, OSC_CHANNEL);
    }

    measure_osc(mod_id, DELTA_T, edge);
    if (get_result_nolog(mod_id, &result))
      slew_time = result.UU.Real_result;
    else
      display_prompt("Unable to vth_read rise time from the oscilloscope.", true);

    measure_osc(mod_id, DELTA_V, edge);
    if (get_result_nolog(mod_id, &result))
      slew_voltage = result.UU.Real_result;
    else
      display_prompt("Unable to vth_read delta v from the oscilloscope.", true);

    if (slew_time != 0)   /* To avoid divide by 0 */
      slew_time = fabs(slew_time);   /* Convert seconds to uS */
    else
      slew_time = INT_MAX;
    slew_rate = fabs(slew_voltage / slew_time);
    result.UU.Real_result = slew_rate;
    log_result(mod_id, result);

  }  /* for edge */


  vth_ainit(mod_id);

  close_osc(mod_id);
  close_mux(mod_id);

}  /*t9385*/
#undef OSC_CHANNEL
#undef COUNT
#undef COUPLING
#undef ARB_FREQ
#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef CHANNEL_RANGE
#undef CHANNEL_OFFSET
#undef TRIGGER_LEVEL
#undef PEAK_VOLTS
#undef POS_OFFSET
#undef NEG_OFFSET
#undef DC_OFFSET
#undef NEG_VOLTS
#undef POS_VOLTS
#undef TIMEBASE_RANGE
#undef TIMEBASE_OFFSET
#undef AVERAGING
#undef TIMEBASE_MODE

#define ARB_FREQ        1024
/* No 30mA current compliance */
#define SRC_I_COMPLIANCE  1
/* No additional ouput resistance*/
#define SRC_R_OUT       0
/* Source enabled */
#define SRC_ON          1
/* Source controlled by processor*/
#define SRC_CONTROL     0
/* Sqr wave peak to peak volts */
#define PEAK_VOLTS      20e+00//10V is for bugs fix 9.00ph ICT release, original 20V
#define DC_OFFSET       0e+00
void t9386(int mod_id, int /*slot*/)
{
  /* ARB SOURCE SQUARE WAVE VOLTAGE ACCURACY, OVERSHOOT, and SLEW RATE      */
  /* ---------------------------------------------------------------------- */
  /* Originally Test Number 9385 ------------------------------------------ */
  /* ---------------------------------------------------------------------- */
  /* Modified 11-19-90 as defined by BDB; changed voltage accuracy measure- */
  /* ment to occur at 15uS after passing through (abs)9v.  Also measure     */
  /* the max voltage and log it as a percent overshoot.  Slew rate measure- */
  /* ment remains the same.  Setups changed to include both a positve and a */
  /* negative 'calibration' at (abs)9.5v and (abs)10.5v with the cal made   */
  /* on the same scope setup as the accy and overshoot measurements.        */
  /* ---------------------------------------------------------------------- */


  double max_volts;
  double v_at_15us;
  double percent_overshoot;
  double slew_time;
  double slew_voltage;
  double slew_rate;
  int slope;
  test_result_type result;
  arrform waveform;
  int check;
  int counter;
  int first;
  int i;
  int last;
  int num_samples;
  int range;
  int samples;
  int trig_level;
  double first_num;
  double last_num;
  double sampling_rate;
  double volt_average;

  open_dmm50(mod_id);
  open_mux(mod_id);

  connect_dmm50_out_af2(mod_id);

  for (int edge = RISING; edge <= FALLING; ++edge)
  {  /* for edge */
    if (edge == RISING)
      slope = POS;
    else
      slope = NEG;

    vth_src_sqr_setup(mod_id, ARB_FREQ, PEAK_VOLTS, DC_OFFSET, SRC_I_COMPLIANCE,
		      SRC_R_OUT, SRC_ON, SRC_CONTROL);

    vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
    vth_aclose_rly(mod_id, K751);   /* S bus to A_XL bus relay */
    vth_aclose_rly(mod_id, K764);   /* A_XL to AF2 relay */

    slew_rate = 0.0;
    slew_time = 1.0;

    sampling_rate = 10E-9;
    range = 10;      //make sure range is 10 because if this changes so does the level
    num_samples = 5800;
/*******************************************************************
abr 07/98 -  This will take all the measurements needed after 9V to
calculate the voltage that the signal evens out to.
********************************************************************/

/********************************************************************
abr 07/98 - The time needed to measure with the dmm was less because
the measurement in the dmm show that the signal rises after the
overshoot.  So instead of waiting for the signal to go high, I took
the measurement at a shorter time interval, after the signals settles
from the overshoot but before it starts rising too much.
********************************************************************/
  volt_average = 7E-6;

  if (slope == POS){
    trig_level = -115;
    setup_dmm_subsampling_AC_and_DC(mod_id, DMM50_ADDRESS, num_samples, sampling_rate,
				trig_level, POS, range);
      }
  else{
    trig_level = 115;
    setup_dmm_subsampling_AC_and_DC(mod_id, DMM50_ADDRESS, num_samples, sampling_rate,
				trig_level, NEG, range);
      }
  get_dmm_volt(mod_id, DMM50_ADDRESS, num_samples, waveform, 0);
  max_volts = waveform[0];
  check = 0;

  //The measurement needs to be taken after 15us
  //  samples = 15E-6/ sampling_rate;
  samples = volt_average/ sampling_rate;

  for(i= 1; i < num_samples; i++) {
    if (slope == POS)
    {
      if (max_volts < waveform[i]) {
	  max_volts = waveform[i];
      }
      if ((waveform[i] > 9) && (check == 0)){
	  v_at_15us = waveform[i+samples];
	  check=1;
      }
    }
    else
    {
      if (max_volts > waveform[i]) {
	max_volts = waveform[i];
      }
      if ((waveform[i] < -9) && (check == 0)){
	v_at_15us = waveform[i+samples];
	check = 1;
      }
    }
  }
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = v_at_15us;
  log_result(mod_id, result);


  percent_overshoot = (max_volts - v_at_15us) / v_at_15us * 100;
  result.UU.Real_result = percent_overshoot;
  log_result(mod_id, result);


    /* ----------------------------------------------------------- */
    /* Added flush_commands to try to eliminate unexpected result pending error */
    /* 12-18-90 JRM ---------------------------------------------- */
    flush_commands(mod_id);

  counter = 0;
  first = 1;
  last = 1;
  if (slope == POS) {
  //measure slew time with mmeter
    for(i = 0; i < num_samples; i++){
      if(waveform[i]>-8 && first==1){
	first = 0;
	first_num = waveform[i];
      }
      if (waveform[i]>8 && last==1){
	last = 0;
	last_num = waveform[i];
	counter++;
      }
      if (waveform[i]>-8 && last==1){
	counter++;
      }
    }
  }
  else
  {
    for(i = 0; i < num_samples; i++){
      if(waveform[i]<8 && first==1){
	first = 0;
	first_num = waveform[i];
      }
      if (waveform[i]<-8 && last==1){
	last = 0;
	last_num = waveform[i];
	counter++;
      }
      if (waveform[i]<8 && last==1){
	counter++;
      }
    }
  }
  slew_time = counter * sampling_rate;
  //end of meausre slew time with mmeter


    /* Measure the slew time */
    slew_voltage = fabs(last_num - first_num);

    /* Calculate the slew rate (log the value as volts/uS) */
    if (slew_time != 0)   /* To avoid divide by 0 */

      slew_time = fabs(slew_time) * 1e06;   /* Convert seconds TO uS */
    else
      slew_time = INT_MAX;
    slew_rate = fabs(slew_voltage / slew_time);
    result.UU.Real_result = slew_rate;
    log_result(mod_id, result);

    vth_aopen_rly(mod_id, K806);   /* SRCOUT to S bus relay */
    vth_aopen_rly(mod_id, K751);   /* S bus to A_XL bus relay */
    vth_aopen_rly(mod_id, K764);   /* A_XL to AF2 relay */

  }  /* for edge */

  vth_ainit(mod_id);

  close_dmm50(mod_id);
  close_mux(mod_id);

}  /*t9386*/
#undef ARB_FREQ
#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef PEAK_VOLTS
#undef DC_OFFSET

#define ARB_FREQ        1024
/* No 30mA current compliance */
#define SRC_I_COMPLIANCE  1
/* No additional ouput resistance*/
#define SRC_R_OUT       0
/* Source enabled */
#define SRC_ON          1
/* Source controlled by processor*/
#define SRC_CONTROL     0
#define PEAK_VOLTS      20e+00//10V is for bugs fix 9.00ph ICT release,original 20V
#define DC_OFFSET       0e+00
#define NEG_VOLTS       (-8e+00)
#define POS_VOLTS       8e+00
/*a*/
void t9387(int mod_id, int /*slot*/)
{
  /*ARB SOURCE TRIANGLE WAVE RISE/FALL TIME AND LINEARITY*/
  double max_volts;
  double min_volts;
  test_result_type result;
  arrform waveform;
  int counter;
  int counter2;
  int num_samples;
  int i;
  int first;
  double linearity_voltage;
  double trig_level;
  double interval;
  double slope;
  double voltage;

  open_dmm50(mod_id);
  open_mux(mod_id);

  connect_dmm50_out_af2(mod_id);

  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL to AF2 relay */

  vth_src_tri_setup(mod_id, ARB_FREQ, PEAK_VOLTS, DC_OFFSET, SRC_I_COMPLIANCE,
                SRC_R_OUT, SRC_ON, SRC_CONTROL);

  break_test_for_function_step_compatability(mod_id);

  //the period is about 974us
  // The smallest sampling rate is 10us
  interval = 20E-6;    //check sampling rate...
  num_samples = 55;
  for (int edge = POS; edge >= NEG; --edge)
  {  /* for edge */
    //changed trigger values from 8-9.5 in order to get a more complete waveform...
    if (edge == NEG)
      trig_level = -9;  // was +/- 9.5 but i changed to 9...1%steps
    else
      trig_level = 9;
    counter = 0;
    //setup of dmm
    setup_dmm_digitizing(mod_id, DMM50_ADDRESS, 100, interval, num_samples, 1.4E-6,
			 1E-3, trig_level, edge);
    flush_commands(mod_id);
    wait_for_dmm50_measurement(mod_id);
   //get_dmm_volt creates a file called capture.dmm
   //where the signal can be displayed
    get_dmm_volt(mod_id, DMM50_ADDRESS, num_samples, waveform, 0);


    if (edge == NEG)
    {
      // the following piece of code returns the number of samples that are between 0 and -8
      // Volts and that has a negative slope this will be used in the linearity test*/
      counter = 0;
      counter2 = 0;
      for(i= 0; i < num_samples; i++) {
	if (waveform[i] > waveform[i+1] && waveform[i] > NEG_VOLTS
	    && waveform [i] < 0) {
	  counter2 += 1;  //number of samples from 0-(-8V)
	}
      }
    }
    else
    {
      max_volts = waveform[0];
      for(i= 1; i < num_samples; i++) {
	if (max_volts < waveform[i])
	  max_volts = waveform[i];
      }
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = max_volts;
      log_result(mod_id, result);

      min_volts = waveform[0];
      for(i= 1; i < num_samples; i++) {
	if (min_volts > waveform[i])
	  min_volts = waveform[i];
      }
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = min_volts;
      log_result(mod_id, result);

      // The following piece of code returns the number of samples it takes the signal
      // to reach a desired level in this case get time it takes to go from -8V to 0V*/
      counter = 0;
      counter2 = 0;
      for(i= 0; i < num_samples; i++) {
	if (waveform[i] < waveform[i+1]  && waveform[i] < POS_VOLTS
	    && waveform[i] > 0) {
	  counter2 += 1;  //number of samples from (8)-0V
	}
      }
    }

    //get the time from NEG_VOLTS to POS_VOLTS ( from 8V t0 -8V )
    // The variable first has to be a number that will never occur (it sould be 0
    // if the first sample is actually the sample wantes so by making it bigger than num_samples we avoid that problem.*/
    first = num_samples+1;
    if (edge == NEG) {
      for(i= 0; i < num_samples; i++)
	{
	  if ((waveform[i] > waveform[i+1]) && (waveform[i] < POS_VOLTS) &&
	    (waveform[i] > NEG_VOLTS)) {
	    if (first == (num_samples+1)) {
	      counter = 0;
	      first = i;
	    }
	    counter += 1;  //number of samples from 8V to -8V
	  }
	}
    }
    else
    {
      for(i = 0; i < num_samples; i++){
	if ((waveform[i] < waveform[i+1]) && (waveform[i] < POS_VOLTS) &&
	    (waveform[i] > NEG_VOLTS)) {
	  if (first == (num_samples+1)){
	    counter = 0;
	    first = i;
	  }
	  counter += 1;  //number of samples from -8-8V
	}
      }
    }
    if (counter2 != 0) {
      slope = ((waveform[first+counter2] - waveform[first])/(counter2));
    }
    voltage = ((slope * counter) + waveform[first]);
    linearity_voltage = waveform[first+counter] - voltage;

    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = linearity_voltage;
    log_result(mod_id, result);

  }  /* for edge */

 vth_ainit(mod_id);

 close_dmm50(mod_id);
 close_mux(mod_id);

}  /*t9387*/
#undef ARB_FREQ
#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef PEAK_VOLTS
#undef DC_OFFSET
#undef NEG_VOLTS
#undef POS_VOLTS

/* Revision Control */
/* No 30mA current compliance */
#define SRC_I_COMPLIANCE  1
/* No additional ouput resistance*/
#define SRC_R_OUT       0
/* Source enabled */
#define SRC_ON          1
/* Source controlled by processor*/
#define SRC_CONTROL     0
#define NUMBER_OF_SAMPLES  100
#define STEP_SIZE       10
#define START_FREQ      10
#define STOP_FREQ       20000
#define NUMBER_OF_AVERAGES  10
#define TRIGGER_LEVEL   0e+00
#define RMS_VOLTS       1e+00
#define DC_OFFSET       0e+00
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03
/*a*/
void t9390(int mod_id, int /*slot*/)
{
  /*ARB SINE FREQUENCY ACCURACY*/
  int freq;
  int reading;

  double cntr_actual_frequency;

  test_result_type result;



  open_cntr(mod_id);
  open_mux(mod_id);

  aconnect_cntr_stop_af2(mod_id);

  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL to AF2 relay */

  setup_cntr_trg_levels(mod_id, TRIGGER_LEVEL, TRIGGER_LEVEL);

  /* SETUP TO MEASURE 1 Hz */
  freq = 1;
  vth_src_sine_setup(mod_id, freq, RMS_VOLTS, DC_OFFSET, SRC_I_COMPLIANCE,
                 SRC_R_OUT, SRC_ON, SRC_CONTROL);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

  break_test_for_function_step_compatability(mod_id);

  setup_cntr_function(mod_id, FREQUENCY);
  setup_cntr_sample_size(mod_id, 1);
  setup_cntr_md4(mod_id);
  cntr_actual_frequency = 0.0;

  for (reading = 1; reading <= NUMBER_OF_AVERAGES; reading++)
  {  /* Take a number of samples */
    trigger_md4(mod_id, FREQUENCY);
    flush_commands(mod_id);
    wait_for_counter_measurement(mod_id);
    cntr_actual_frequency += read_cntr_mean(mod_id);
  }  /* Take a number of readings */

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = cntr_actual_frequency / NUMBER_OF_AVERAGES;
  log_result(mod_id, result);

  freq = START_FREQ;

  while (freq <= STOP_FREQ)
  {
    vth_src_sine_setup(mod_id, freq, RMS_VOLTS, DC_OFFSET, SRC_I_COMPLIANCE,
                   SRC_R_OUT, SRC_ON, SRC_CONTROL);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

    break_test_for_function_step_compatability(mod_id);

    setup_cntr_function(mod_id, FREQUENCY);
    setup_cntr_sample_size(mod_id, NUMBER_OF_SAMPLES);
    setup_cntr_md4(mod_id);
    trigger_md4(mod_id, FREQUENCY);
    flush_commands(mod_id);
    wait_for_counter_measurement(mod_id);
    cntr_actual_frequency = read_cntr_mean(mod_id);
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = cntr_actual_frequency;
    log_result(mod_id, result);

    if (freq == STOP_FREQ / 2)
      freq = STOP_FREQ;
    else
      freq *= STEP_SIZE;

  }

  vth_ainit(mod_id);

  close_cntr(mod_id);
  close_mux(mod_id);

}  /*t9390*/


#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef NUMBER_OF_SAMPLES
#undef STEP_SIZE
#undef START_FREQ
#undef STOP_FREQ
#undef NUMBER_OF_AVERAGES
#undef TRIGGER_LEVEL
#undef RMS_VOLTS
#undef DC_OFFSET

#undef SOURCE_WAIT_TIME


/* Revision Control */



/* No 30mA current compliance */

#define SRC_I_COMPLIANCE  1
/* No additional ouput resistance*/
#define SRC_R_OUT       0
/* Source enabled */
#define SRC_ON          1
/* Source controlled by processor*/
#define SRC_CONTROL     0
#define STEP_SIZE       10

#define LO_BAND         0
/* Of dmm full scale */
#define TRIGGER_PERCENT  0
/* 1=dc, 2=ac (1Hz requires dc) */
#define COUPLING        1
#define HI_BAND         25000
#define START_FREQ      1
#define STOP_FREQ       20000

#define RESOLUTION      10e-06
/* -------------------------------------------------------------------- */
/* Modified: 01.30.91 JRM --------------------------------------------- */
/* Changed rms_volts from 1v to 7.071v -------------------------------- */
/* -------------------------------------------------------------------- */
#define RMS_VOLTS       7.071e+00
#define FUDGE_FACTOR    1.2e+00
#define DC_OFFSET       0e+00
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03

#define DMM_FSOURCE     ACV
#define DMM_FUNCTION    FR


/*a*/

void t9391(int mod_id, int /*slot*/)
{
  /*ARB SINE FREQUENCY ACCURACY*/
  int freq;

  double dmm_voltage_range;

  test_result_type result;



  open_dmm(mod_id);
  open_mux(mod_id);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K837);   /* SRCOUT to RCAL2 bus relay */
  vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 bus relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to GND relay */

  setup_dmm_ac_band(mod_id, LO_BAND, HI_BAND);
  setup_dmm_level_filter(mod_id, true);
  setup_dmm_fsource(mod_id, DMM_FSOURCE);
  setup_dmm_level(mod_id, TRIGGER_PERCENT, COUPLING);
  dmm_voltage_range = RMS_VOLTS * sqrt(2.0) * FUDGE_FACTOR;
  setup_dmm(mod_id, DMM_FUNCTION, dmm_voltage_range, RESOLUTION);

  freq = START_FREQ;

  while (freq <= STOP_FREQ)
  {
    vth_src_sine_setup(mod_id, freq, RMS_VOLTS, DC_OFFSET, SRC_I_COMPLIANCE,
                   SRC_R_OUT, SRC_ON, SRC_CONTROL);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

    break_test_for_function_step_compatability(mod_id);

    measure_dmm(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected dmm vth_reading)", true);

    if (freq == STOP_FREQ / 2)
      freq = STOP_FREQ;
    else
      freq *= STEP_SIZE;

  }

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t9391*/


#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef STEP_SIZE

#undef LO_BAND
#undef TRIGGER_PERCENT
#undef COUPLING
#undef HI_BAND
#undef START_FREQ
#undef STOP_FREQ
#undef RESOLUTION
#undef RMS_VOLTS
#undef FUDGE_FACTOR
#undef DC_OFFSET
#undef SOURCE_WAIT_TIME
#undef DMM_FSOURCE
#undef DMM_FUNCTION


/* Revision Control */



#define NUM_FREQUENCIES_TO_TEST  5
/* 0 selects frequency A */

/* No 30mA current compliance    */
#define SRC_I_COMPLIANCE  1
/* No additional ouput resistance*/
#define SRC_R_OUT       0
/* Source enabled */
#define SRC_ON          1
/* Source controlled by processor*/
#define SRC_CONTROL     0
#define SAMPLE_SIZE__   100

#define SALES_FUNC            PER
#define MODE            DCV

#define LOW             0.8
#define HIGH            1.2
#define RES_            0.001


typedef int frequency_array______[NUM_FREQUENCIES_TO_TEST];


#define RMS_VOLTS       7.071e+00
#define DC_OFFSET       0.0e+00
/*seconds*/
#define SOURCE_WAIT_TIME  1.0e-03

const frequency_array______ FREQ_A______ = {
  10, 100, 1000, 10000, 20000
};


/*a*/

void t9392(int mod_id, int /*slot*/)
{
  /* ARB SOURCE SINE WAVE JITTER MEASUREMENT */
  /* ---------------------------------------------------------------------- */
  /* Uses dmm ------------------------------------------------------------- */
  /* ---------------------------------------------------------------------- */
int this_frequency;

  double smallest;
  double largest;

  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K837);   /* SRCOUT to RCAL2 relay */
  vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  setup_dmm_fsource(mod_id, MODE);
  setup_dmm_level(mod_id, 0, DMM_DC);
  setup_dmm_level_filter(mod_id, true);
  hold_dmm(mod_id);
  setup_dmm_readings(mod_id, SAMPLE_SIZE__);
  enable_dmm_buffer(mod_id);
  setup_dmm(mod_id, SALES_FUNC, RMS_VOLTS, RES_);
  vth_src_sine_setup(mod_id, FREQ_A______[0], RMS_VOLTS, DC_OFFSET,
                 SRC_I_COMPLIANCE, SRC_R_OUT, SRC_ON, SRC_CONTROL);

  for (this_frequency = 0;
       this_frequency < NUM_FREQUENCIES_TO_TEST;
       this_frequency++)
  {  /* for frequency */
    setup_dmm_memory(mod_id, DREAL, FIFO);
    setup_dmm_ac_band(mod_id, round(LOW * FREQ_A______[this_frequency]),
                      round(HIGH * FREQ_A______[this_frequency]));
    vth_ascat_set_a_freq(mod_id, FREQ_A______[this_frequency]);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

    break_test_for_function_step_compatability(mod_id);
    trigger_dmm(mod_id);
    flush_commands(mod_id);
    wait_for_dmm_measurement(mod_id);
    calculate_dmm_stats(mod_id);

    /* Calculate and log the standard deviation */
    read_dmm_stats(mod_id, SALES_MIN);
    if (get_result_nolog(mod_id, &result))
      smallest = result.UU.Real_result;
    else
    {
      smallest = -9.999e9;
      display_prompt("No result in result log (Expected DMM min)", true);
    }

    read_dmm_stats(mod_id, SALES_MAX);
    if (get_result_nolog(mod_id, &result))
      largest = result.UU.Real_result;
    else
    {
      largest = 9.999e9;
      display_prompt("No result in result log (Expected DMM max)", true);
    }

    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = largest - smallest;
    log_result(mod_id, result);

  }  /* for frequency */

  vth_afc_dis_src_out(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9392 */


#undef NUM_FREQUENCIES_TO_TEST

#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef SAMPLE_SIZE__
#undef SALES_FUNC
#undef MODE
#undef LOW
#undef HIGH
#undef RES_
#undef RMS_VOLTS
#undef DC_OFFSET
#undef SOURCE_WAIT_TIME


/* Revision Control */



#define NUM_FREQUENCIES_TO_TEST  5
/* 0 selects frequency A */

/* No 30mA current compliance    */
#define SRC_I_COMPLIANCE  1
/* No additional ouput resistance*/
#define SRC_R_OUT       0
/* Source enabled */
#define SRC_ON          1
/* Source controlled by processor*/
#define SRC_CONTROL     0


typedef int frequency_array_______[NUM_FREQUENCIES_TO_TEST];


#define PEAK_VOLTS      4.0e+00
#define DC_OFFSET       0.0e+00
#define TRIGGER_LEVEL   0.0e+00
/*seconds*/
#define SOURCE_WAIT_TIME  1.0e-03

const frequency_array_______ FREQ_A_______ = {
  10, 100, 1000, 10000, 20000
};

const frequency_array_______ SAMPLE_SIZE__ = {
  10, 10, 10, 10, 10
};


void t9393(int mod_id, int /*slot*/)
{
  /* ARB SOURCE JITTER MEASUREMENT USING SQUARE WAVE */
int this_frequency;

  double cntr_actual_min;
  double cntr_actual_max;
  double cntr_max_minus_min;

  test_result_type result;


  open_cntr(mod_id);
  open_mux(mod_id);

  aconnect_cntr_stop_af2(mod_id);

  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL to AF2 relay */

  setup_cntr_trg_levels(mod_id, TRIGGER_LEVEL, TRIGGER_LEVEL);
  setup_cntr_function(mod_id, PERIOD);

  for (this_frequency = 0;
       this_frequency < NUM_FREQUENCIES_TO_TEST;
       this_frequency++)
  {  /* for frequency */
    setup_cntr_sample_size(mod_id, SAMPLE_SIZE__[this_frequency]);
    vth_src_sqr_setup(mod_id, FREQ_A_______[this_frequency], PEAK_VOLTS,
                  DC_OFFSET, SRC_I_COMPLIANCE, SRC_R_OUT, SRC_ON,
                  SRC_CONTROL);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

    break_test_for_function_step_compatability(mod_id);

    trigger_cntr(mod_id);
    cntr_actual_min = read_cntr_min(mod_id);
    cntr_actual_max = read_cntr_max(mod_id);
    cntr_max_minus_min = cntr_actual_max - cntr_actual_min;

    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = cntr_max_minus_min;
    log_result(mod_id, result);

  }  /* for frequency */

  vth_afc_dis_src_out(mod_id);

  close_cntr(mod_id);
  close_mux(mod_id);

}  /* t9393 */


#undef NUM_FREQUENCIES_TO_TEST

#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef PEAK_VOLTS
#undef DC_OFFSET
#undef TRIGGER_LEVEL
#undef SOURCE_WAIT_TIME


/* Revision Control */



#define NUM_FREQUENCIES_TO_TEST  6
/* 0 selects frequency A */

/* No 30mA current compliance    */
#define SRC_I_COMPLIANCE  1
/* No additional ouput resistance*/
#define SRC_R_OUT       0
/* Source enabled */
#define SRC_ON          1
/* Source controlled by processor*/
#define SRC_CONTROL     0


typedef int frequency_array________[NUM_FREQUENCIES_TO_TEST];
typedef double gate_array[NUM_FREQUENCIES_TO_TEST];


#define PEAK_VOLTS      4.0e+00
#define DC_OFFSET       0.0e+00
#define TRIGGER_LEVEL   0.0e+00
/*seconds*/
#define SOURCE_WAIT_TIME  1.0e-03

const frequency_array________ FREQ_A________ = {
  1, 10, 100, 1000, 10000, 20000
};

const gate_array GATE_TIME = {
  1.0e+00, 1.0e+00, 1.0e+00, 0.1e+00, 0.1e+00, 0.1e+00
};


void t9394(int mod_id, int /*slot*/)
{
  /* ARB SOURCE SQUARE WAVE FREQUENCY ACCURACY */
int this_frequency;

  double freq;

  test_result_type result;


  open_cntr(mod_id);
  open_mux(mod_id);

  vth_mhpib_wr(mod_id, 4, ":INP2:FILT:LPAS:STAT ON");
  aconnect_cntr_stop_af2(mod_id);

  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to A_XL relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL to AF2 relay */

  setup_cntr_trg_levels(mod_id, TRIGGER_LEVEL, TRIGGER_LEVEL);
  setup_cntr_function(mod_id, FREQUENCY);
  setup_cntr_gate_time(mod_id, GATE_TIME[0]);
  vth_src_sqr_setup(mod_id, FREQ_A________[0], PEAK_VOLTS, DC_OFFSET,
                SRC_I_COMPLIANCE, SRC_R_OUT, SRC_ON, SRC_CONTROL);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

  for (this_frequency = 0;
       this_frequency < NUM_FREQUENCIES_TO_TEST;
       this_frequency++)
  {  /* for frequency */
    vth_ascat_set_a_freq(mod_id, FREQ_A________[this_frequency]);
    setup_cntr_gate_time(mod_id, GATE_TIME[this_frequency]);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    flush_commands(mod_id);

    vth_mhpib_wr(mod_id, 4, ":INIT:IMM");
    vth_mtmr_wait(mod_id, 10.0 * GATE_TIME[this_frequency]);
    flush_commands(mod_id);
    freq = read_cntr(mod_id);
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = freq;
    log_result(mod_id, result);

  }  /* for frequency */

  vth_afc_dis_src_out(mod_id);

  vth_mhpib_wr(mod_id, 4, ":INP2:FILT:LPAS:STAT OFF");

  close_cntr(mod_id);
  close_mux(mod_id);

}  /* t9394 */


#undef NUM_FREQUENCIES_TO_TEST

#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef PEAK_VOLTS
#undef DC_OFFSET
#undef TRIGGER_LEVEL
#undef SOURCE_WAIT_TIME


#define LOW             0
#define HIGH            1

#define SOURCING        0
#define SINKING         1

#define POSITIVE        0
#define NEGATIVE        1

typedef int load_relay_array[2];
typedef int x_bus_relay_array[2];
typedef double v_array_[2];
typedef double i_range_array[2];
typedef i_range_array i_array[2];


/* Revision Control */



#define R10_OHM_FB      0
#define R100_OHM_FB     1
#define MEAS_TYPE       1
#define PIN_SLOT        2

#define R10K            10e+03
#define FUDGE_FACTOR    1.2e+00
#define RESOLUTION      1.0e-03
#define PS_RAMP_TIME    1.0e+00
#define DMM_FUNCTION    DCV
#define IPS             800e-03

const load_relay_array LOAD_RELAY = {
  K846, K845
};

const x_bus_relay_array X1_RELAY = {
  K725, K726
};

const x_bus_relay_array X2_RELAY = {
  K718, K717
};

const v_array_ SOURCE_VOLTAGE______ = {
  -5.25e00, 5.25e00
};

/* TARGET MOA LOAD CURRENT */

const i_array LOAD_CURRENT = {
  { -27.5e-03, -165.0e-03 },
  { 27.5e-03, 165.0e-03 }
};

/* SLOPE/OFFSET LOWER CURRENT POINT */

const i_array LOW_I_CAL_VALUE = {
  { -20e-03, -100.0e-03 },
  { 20e-03, 100e-03 }
};

/* SLOPE/OFFSET HIGHER CURRENT POINT */

const i_array HIGH_I_CAL_VALUE = {
  { -35e-03, -150.0e-03 },
  { 35e-03, 150.0e-03 }
};

void t9410(int mod_id, int /*slot*/)
{
  /* MOA OUTPUT VOLTAGE RANGE */
  int current_position;
  int next_position;

  double expected_dmm_v;
  double dmm_no_load_voltage;
  double low_v;
  double high_v;
  double low_ir;
  double high_ir;
  double low_i;
  double high_i;
  /*slope*/
  double m;
  /*y intersect*/
  double b;
  double vps;
  double load_i;
  double load_ir;
  i_range_array r;
  test_result_type result;
  char s[81];
  int valid_cards;
  int p2c_temp;


  valid_cards = (1 << PIN) | (1 << PIN6) | (1 << PIN6JB) | (1 << PIN12JB) |
                (1 << PINTERM) | (1 << PIN20) | (1 << CP) | (1 << CP20) |
                (1 << ANALOG) | (1 << DD6) | (1 << DDJB) | (1 << DDP) |
                (1 << FDD20) | (1 << FDD6) | (1 << FDDP) | (1 << FDD20_H5) | (1 << FDD6_H5) | (1 << FDDP_H5) |
	        (1 << DD20) | (1 << SD6) | (1 << SDP) | (1 << DDA) | (1 << FDDU);
  p2c_temp = get_testhead_actual_card(mod_id, PIN_SLOT);
  if (!(((1 << p2c_temp) & valid_cards) != 0))
  {  /*Display no slot 2 message*/
    *s = '\0';
    strcat(s, "Test ");
    current_position = mtd_strlen(s) + 1;
    sprintf(s + current_position - 1, "%d", 9410);
    next_position = mtd_strlen(s) + 1;
    strcat(s, ", Module ");
    current_position = mtd_strlen(s) + 1;
    sprintf(s + current_position - 1, "%d", mod_id);
    next_position = mtd_strlen(s) + 1;
    strcat(s, " DID NOT RUN.");
    display_prompt(s, true);
    *s = '\0';
    strcat(s, "MOA Voltage Accuracy Test requires a pin");
    display_prompt(s, true);
    *s = '\0';
    strcat(s, "or channel type card in Slot ");
    current_position = mtd_strlen(s) + 1;
    sprintf(s + current_position - 1, "%d", PIN_SLOT);
    next_position = mtd_strlen(s) + 1;
    strcat(s, ".");
    display_prompt(s, true);
  }  /*Display no slot 2 message*/
  else
  {  /* After check for slot 2 pin card */
    vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
    open_ps(mod_id);
    open_mux(mod_id);
    open_dmm(mod_id);

    r[LOW] = get_cal_resistor_value(mod_id, R100_OHM_FB, MEAS_TYPE);
    r[HIGH] = get_cal_resistor_value(mod_id, R10_OHM_FB, MEAS_TYPE);
    r[LOW] = 1 / (1 / R10K + 1 / r[LOW]);
    r[HIGH] = 1 / (1 / R10K + 1 / r[HIGH]);

    vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
    connect_plus_calib1(mod_id);
    connect_minus_calib2(mod_id);
    connect_power_supply_xmint(mod_id);
    connect_dmm(mod_id);

    expected_dmm_v = fabs(SOURCE_VOLTAGE______[POSITIVE] * 2 * FUDGE_FACTOR);

    setup_dmm(mod_id, DMM_FUNCTION, expected_dmm_v, RESOLUTION);

    /* CONNECT THE SOURCE OUT TO THE MOA IN THE INVERTING MODE */
    vth_aclose_rly(mod_id, K843);   /* MOANON to GROUND relay */
    vth_aclose_rly(mod_id, K831);   /* SRCOUT to S bus relay */
    vth_aclose_rly(mod_id, K737);   /* S bus to X5 bus */
    vth_aclose_rly(mod_id, K705);   /* X5 bus to B bus relay */
    vth_aclose_rly(mod_id, K824);   /* B bus to MOAINV relay */
    vth_aclose_rly(mod_id, K821);   /* MOAOUT to A bus relay */
    vth_aclose_rly(mod_id, K716);   /* A bus to X8 bus relay */

    /* CLOSE THE PATH FOR THE MOA FEEDBACK USING STANDARD RESISTORS */
    vth_pclose_rly(mod_id, PIN_SLOT, KDC8);
    vth_pclose_rly(mod_id, PIN_SLOT, KDC5);
    /* Both mint pins 66 and 45 are on page 0 */
    virt_card_select(mod_id, PIN_SLOT, 0);
    vth_pclose_rly(mod_id, PIN_SLOT, KX8);
    vth_pclose_rly(mod_id, PIN_SLOT, KX5);
    vth_pclose_rly(mod_id, PIN_SLOT, K12H);
    vth_pclose_rly(mod_id, PIN_SLOT, K12E);

    /* CONNECT THE LOAD TO THE MOA OUT FOR AND THE POWER SUPPLY */
    vth_aclose_rly(mod_id, K827);   /* MOAOUT to REFR2 relay */
    vth_aclose_rly(mod_id, K838);   /* MOAOUT to RCAL2 relay */
    vth_aclose_rly(mod_id, K741);   /* X1 MINT pin connect relay */
    vth_aclose_rly(mod_id, K742);   /* X2 MINT pin connect relay */
    vth_aclose_rly(mod_id, K857);   /* L bus to GROUND relay */

    vth_aclose_rly(mod_id, K853);   /* CALIB1 to RCAL2 relay */

    /* SET THE MOA COMPLIANCE DAC TO FULL SCALE */
    vth_amoa_wr_v_cmpl_dac(mod_id, 4095);

    for (int range = LOW; range <= HIGH; ++range)
    {  /* FOR CURRENT - SETUP AND MEASURE EACH CASE */
      vth_aclose_rly(mod_id, LOAD_RELAY[range]);

      for (int source_polarity = POSITIVE; source_polarity <= NEGATIVE; ++source_polarity)
      {  /* FOR SOURCE POLARITY */
        vth_aclose_rly(mod_id, K854);   /* CALIB2 to GROUND relay */

        vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE______[source_polarity]);
        vth_afc_en_src_out(mod_id);
        vth_aopen_rly(mod_id, K2605);   /* MOA happy relay */
        vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

        measure_dmm(mod_id); //subtest 0
        if (get_result_log(mod_id, &result))
          dmm_no_load_voltage = result.UU.Real_result;
        else
          display_prompt("No result in result log (Expected DMM v)", true);

        /* Set source to 0v to avoid hot switch of relays */
        vth_asrc_set_dc_ampl(mod_id, 0.0);
        vth_aopen_rly(mod_id, K854);   /* CALIB2 to GROUND relay */
        vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
        vth_aclose_rly(mod_id, K855);   /* CALIB2 to I bus relay */
        vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

        vth_aclose_rly(mod_id, X1_RELAY[source_polarity]);
        vth_aclose_rly(mod_id, X2_RELAY[source_polarity]);

        for (int direction = SOURCING; direction <= SINKING; ++direction)
        {  /* FOR DIRECTION := SOURCING TO SINKING */
          /* Always disable the 30mA ilim for the calibration */
          vth_amoa_dis_30ma_ilim(mod_id);

          /* CALCULATE THE SLOPE AND OFFSET OF THE Vps vs Imoa */
          aconnect_ps_20_ohm_load(mod_id, true);
          vth_aopen_rly(mod_id, K2605);
          vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
          /* Restore the source voltage previously set to 0v */
          vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE______[source_polarity]);

          /* SETUP FOR THE FIRST MOA I*R MEASUREMENT */
          low_v = LOW_I_CAL_VALUE[direction][range] * r[range] -
                  2 * SOURCE_VOLTAGE______[source_polarity];
          setup_ps(mod_id, low_v, IPS);
          vth_mtmr_wait(mod_id, PS_RAMP_TIME);

          measure_dmm(mod_id);
          if (get_result_nolog(mod_id, &result))
            low_ir = result.UU.Real_result;
          else
            display_prompt("No result in result log (Expected DMM v)", true);

          /* SETUP FOR THE SECOND MOA I*R MEASUREMENT */
          high_v = HIGH_I_CAL_VALUE[direction][range] * r[range] -
                   2 * SOURCE_VOLTAGE______[source_polarity];
          setup_ps(mod_id, high_v, IPS);
          vth_mtmr_wait(mod_id, PS_RAMP_TIME);

          measure_dmm(mod_id);
          if (get_result_nolog(mod_id, &result))
            high_ir = result.UU.Real_result;
          else
            display_prompt("No result in result log (Expected DMM v)", true);

          low_i = low_ir / r[range];
          high_i = high_ir / r[range];
          m = (high_i - low_i) / (high_v - low_v);
          b = high_i - m * high_v;

          /* RESET THE POWER SUPPLY VOLTAGE TO PRODUCE THE DESIRED CURRENT */
          if (range == LOW)
            vth_amoa_en_30ma_ilim(mod_id);
          vps = (-1 * LOAD_CURRENT[direction][range] - b) / m;
          setup_ps(mod_id, vps, IPS);
          vth_mtmr_wait(mod_id, PS_RAMP_TIME);

          /* MEASURE THE OUTPUT CURRENT AND LOG THE RESULT */
          measure_dmm(mod_id); //subtest 1
          if (get_result_nolog(mod_id, &result))
          {  /* get result */
            load_ir = result.UU.Real_result;
            load_i = load_ir / r[range];
            result.UU.Real_result = fabs(load_i);
            log_result(mod_id, result);
          }  /* get result */
          else
            display_prompt("No result in result log (Expected DMM v)", true);

          /* NOW OPEN K855, CLOSE K854, AND MEASURE THE OUTPUT VOLTAGE */
          /* Set source to 0v to avoid hot switch of relays */
          vth_asrc_set_dc_ampl(mod_id, 0.0);
          /* and disable the power supply output voltage */
          disable_ps_out(mod_id);
          vth_aopen_rly(mod_id, K855);   /* CALIB2 to I bus relay */
          vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
          vth_aclose_rly(mod_id, K854);   /* CALIB2 to GROUND relay */
          vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
          /* Restore the source voltage previously set to 0v */
          vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE______[source_polarity]);
          /* and enable the power supply output voltage */
          enable_ps_out(mod_id);
          vth_mtmr_wait(mod_id, PS_RAMP_TIME);
          measure_dmm(mod_id);
          if (!get_result_log(mod_id, &result))
            display_prompt("No result in result log (Expected DMM v)", true);

          /* NOW CLOSE K855, OPEN K854, TO SETUP FOR THE NEXT CURRENT MEAS */
          /* Set source to 0v to avoid hot switch of relays */
          vth_asrc_set_dc_ampl(mod_id, 0.0);
          /* and disable the power supply output voltage */
          disable_ps_out(mod_id);
          vth_aopen_rly(mod_id, K854);   /* CALIB2 to GROUND relay */
          vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
          vth_aclose_rly(mod_id, K855);   /* CALIB2 to I bus relay */
          vth_aclose_rly(mod_id, K2605);   /* MOA happy relay */
          vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

        }  /* FOR DIRECTION := SOURCING TO SINKING */


        vth_aopen_rly(mod_id, K855);   /* CALIB2 to I bus relay */
        vth_aopen_rly(mod_id, X1_RELAY[source_polarity]);
        vth_aopen_rly(mod_id, X2_RELAY[source_polarity]);

      }  /* FOR SOURCE POLARITY */
      /* MEASURE THE NO LOAD OUTPUT VOLTAGE OF THE MOA */


      vth_aopen_rly(mod_id, LOAD_RELAY[range]);

    }  /* FOR CURRENT - SETUP AND MEASURE EACH CASE */


    close_ps(mod_id);
    close_dmm(mod_id);
    close_mux(mod_id);

  }  /* After check for slot 2 pin card */


}  /* t9410 */

#undef LOW
#undef HIGH
#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE

#undef R10_OHM_FB
#undef R100_OHM_FB
#undef MEAS_TYPE
#undef PIN_SLOT
#undef R10K
#undef FUDGE_FACTOR
#undef RESOLUTION
#undef PS_RAMP_TIME
#undef IPS
#undef DMM_FUNCTION


/* Revision Control */



#define MIN_TEST_VOLTAGE  10.0e+00
#define MAX_TEST_VOLTAGE  20.0e+00
#define STARTING_INCREMENT  1024e-03
#define OLYM_FINEST_INCREMENT  8e-03	//added by chuanho, 14jun2012
#define FINEST_INCREMENT  8e-03
#define SYN_WAIT_TIME   10e-03 
#define OLYM_WAIT_TIME   150e-03		//added by chuanho, 14jun2012
#define OLYM_CH1_CURR  500e-03			//added by chuanho, 14jun2012
#define OLYM_CH1_OCP  1e+00			//added by chuanho, 14jun2012

/* ADDED moa_comp */
/*08.09.90*/
#define MOA_COMP        5.0e+00
#define RESOLUTION      1e-03
#define FUDGE_FACTOR    1.2e+00


/* static variables for t9415: */
struct LOC_t9415 {
  int mod_id;
} ;

static void close_the_relays____(struct LOC_t9415 *LINK)
{
  vth_aclose_rly(LINK->mod_id, K2605);   /* MOA happy relay */
  /*08.01.89*/
  /* ADDED 'close 843' to put moa in a known state */
  /*08.09.90*/
  vth_aclose_rly(LINK->mod_id, K843);   /* MOA NON to GND */
  /* ADDED STABILIZATION WAIT */
  /*08.09.90*/
  vth_mtmr_wait(LINK->mod_id, WAIT_TIME_FOR_RELAYS);

  /* CLOSE THE P.S. NEGATIVE  TO GROUND PATH RELAYS */
  vth_aclose_rly(LINK->mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(LINK->mod_id, K718);   /* X2 bus to L bus relay */
  vth_aclose_rly(LINK->mod_id, K857);   /* L bus to GROUND relay */

  /* CLOSE THE MOAOUT TO SYN. POSITIVE  PATH RELAYS */
  vth_aclose_rly(LINK->mod_id, K807);   /* MOAOUT to S bus relay */
  vth_aclose_rly(LINK->mod_id, K733);   /* X1 bus to S bus relay */
  vth_aclose_rly(LINK->mod_id, K741);   /* X1 bus to mint pin relay */

}


void t9415(int mod_id_, int /*slot*/)
{
  /* MOA POSITIVE OVER VOLTAGE LIMIT ACCURACY */
  struct LOC_t9415 V;

  boolean vlimit;
  /* ADDED ever_had_a_vlimit TO KEEP FROM RETRYING THE SAME VALUES*/
  /*08.20.90*/
  boolean ever_had_a_vlimit;

  double syn_voltage; 
  double olym_ch1_voltage; //added by chuanho, 14jun2012
  double voltage_increment;

  int over_voltage_bit;
  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id_;

  OpenResourceManager(); 
  open_dmm(V.mod_id);
  open_mux(V.mod_id);
  open_33250A(V.mod_id);

  /*New Olympus codes*/
  //Added by chuanho, 14jun2012
  if(check_syn_model())
  {
  open_olym(V.mod_id);
  connect_olym_ch1(V.mod_id); 
  aconnect_olym_ch1_abus1_xmint(V.mod_id);


  /* SETUP TO DO POSITIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE MOA SOURCE */
  /* !! THERE IS NO WAY TO EXPLICITLY TRI-STATE THE MOA !! */

  vth_amoa_en_30ma_ilim(V.mod_id);
  /* ADDED 'vth_amoa_set_v_cmpl(mod_id,moa_comp)' */
  /*08.09.90*/
  vth_amoa_set_v_cmpl(V.mod_id, MOA_COMP);

  olym_ch1_voltage = MIN_TEST_VOLTAGE; 
  voltage_increment = STARTING_INCREMENT;
  vlimit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*08.09.90*/
  close_the_relays____(&V);
  setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage);
  setup_olym_ch1_curr(V.mod_id, OLYM_CH1_CURR); 
  setup_olym_ch1_ocp(V.mod_id, OLYM_CH1_OCP); 
  enable_olym_ch1_out(V.mod_id); 
  vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
  vth_astat_clr(V.mod_id);

  while (!vlimit && olym_ch1_voltage <= MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      /* Set the condition such that the while loop will terminate */
      olym_ch1_voltage = MAX_TEST_VOLTAGE + 1; 
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^11  =  BIT NUMBER 4 */
//mjs -- another messed up bit problem
    over_voltage_bit = P_getbits_US(this_word.bits, 11, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      vlimit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= OLYM_FINEST_INCREMENT ||
          olym_ch1_voltage == MIN_TEST_VOLTAGE) 
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*08.09.90*/
      vlimit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*08.09.90*/
      disable_olym_ch1_out(V.mod_id); 
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays____(&V);

      /* DIVIDE THE INCREMENT IN HALF TO GO BACK TO A "HALF" STEP FROM */
      /* WHERE THE LIMIT OCCURRED, YOU DON'T HAVE TO GO BACK ALL THE */
      /* WAY BECAUSE YOU KNOW NO LIMIT OCCURRED AT THE LAST "FULL" STEP */
      voltage_increment /= 2;
      olym_ch1_voltage -= voltage_increment;
      setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
	  enable_olym_ch1_out(V.mod_id);
      /* ADDED SYN WAIT TIME */
      /*08.09.90*/
      vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
      continue;
    }  /* Go back to the last voltage and reduce the increment */
//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, OLYM_FINEST_INCREMENT) )
      voltage_increment /= 2;
    olym_ch1_voltage += voltage_increment; 
    setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
    vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, olym_ch1_voltage * FUDGE_FACTOR, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_olym_ch1_out_dmm(V.mod_id, true);
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  disable_olym_ch1_out(V.mod_id);
  vth_ainit(V.mod_id);
  close_olym(V.mod_id);
  close_33250A(V.mod_id);
  
  /* Go to the next increment */
  /* Go to the next increment */
  /* No result to read */
  /* No result to read */
  }

  else
  {
  open_syn(V.mod_id);
  setup_syn(V.mod_id, DC, 0, 0.0, 0.0); 
  setup_syn_hv(V.mod_id, true); 
  aconnect_syn_out_xmint(V.mod_id);

  /* SETUP TO DO POSITIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE MOA SOURCE */
  /* !! THERE IS NO WAY TO EXPLICITLY TRI-STATE THE MOA !! */

  vth_amoa_en_30ma_ilim(V.mod_id);
  /* ADDED 'vth_amoa_set_v_cmpl(mod_id,moa_comp)' */
  /*08.09.90*/
  vth_amoa_set_v_cmpl(V.mod_id, MOA_COMP);

  syn_voltage = MIN_TEST_VOLTAGE; 
  voltage_increment = STARTING_INCREMENT;
  vlimit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*08.09.90*/
  close_the_relays____(&V);
  setup_syn_off(V.mod_id, syn_voltage); 
  vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
  vth_astat_clr(V.mod_id);

  while (!vlimit && syn_voltage <= MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      /* Set the condition such that the while loop will terminate */
      syn_voltage = MAX_TEST_VOLTAGE + 1; 
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^11  =  BIT NUMBER 4 */
//mjs -- another messed up bit problem
    over_voltage_bit = P_getbits_US(this_word.bits, 11, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      vlimit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT ||
          syn_voltage == MIN_TEST_VOLTAGE) 
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*08.09.90*/
      vlimit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*08.09.90*/
      setup_syn_off(V.mod_id, 0.0);
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays____(&V);

      /* DIVIDE THE INCREMENT IN HALF TO GO BACK TO A "HALF" STEP FROM */
      /* WHERE THE LIMIT OCCURRED, YOU DON'T HAVE TO GO BACK ALL THE */
      /* WAY BECAUSE YOU KNOW NO LIMIT OCCURRED AT THE LAST "FULL" STEP */
      voltage_increment /= 2;
      syn_voltage -= voltage_increment;
      setup_syn_off(V.mod_id, syn_voltage); 
      /* ADDED SYN WAIT TIME */
      /*08.09.90*/
      vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
      continue;
    }  /* Go back to the last voltage and reduce the increment */
//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    syn_voltage += voltage_increment;
    setup_syn_off(V.mod_id, syn_voltage); 
    vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, syn_voltage * FUDGE_FACTOR, RESOLUTION); 
  connect_dmm(V.mod_id);
  aconnect_syn_out_dmm(V.mod_id, true); 
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  setup_syn_off(V.mod_id, 0.0); 
  setup_syn_hv(V.mod_id, false); 

  vth_ainit(V.mod_id);
  close_syn(V.mod_id); 

  /* Go to the next increment */
  /* Go to the next increment */
  /* No result to read */
  /* No result to read */
  }

  close_mux(V.mod_id);
  close_dmm(V.mod_id);
  CloseResourceManager();

}  /* t9415 */


#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef OLYM_FINEST_INCREMENT	//added by chuanho, 14jun2012
#undef SYN_WAIT_TIME 
#undef OLYM_WAIT_TIME			//added by chuanho, 14jun2012
#undef OLYM_CH1_CURR			//added by chuanho, 14jun2012
#undef OLYM_CH1_OCP			//added by chuanho, 14jun2012
#undef MOA_COMP
#undef RESOLUTION
#undef FUDGE_FACTOR


/* Revision Control */



#define MIN_TEST_VOLTAGE  (-10.0e+00)
#define MAX_TEST_VOLTAGE  (-20.0e+00)
#define OLYM_MIN_TEST_VOLTAGE  (10.0e+00)	//added by chuanho, 14jun2012
#define OLYM_MAX_TEST_VOLTAGE  (20.0e+00)	//added by chuanho, 14jun2012
#define OLYM_FINEST_INCREMENT  10e-03		//added by chuanho, 14jun2012
#define STARTING_INCREMENT  1024e-03
#define FINEST_INCREMENT  8e-03
#define SYN_WAIT_TIME   10e-03 
#define OLYM_WAIT_TIME	150e-03				//added by chuanho, 14jun2012
#define OLYM_CH1_CURR  500e-03				//added by chuanho, 14jun2012
#define OLYM_CH1_OCP  1e+00					//added by chuanho, 14jun2012

/* ADDED moa_comp */
/*08.09.90*/
#define MOA_COMP        5.0e+00
#define RESOLUTION      1e-03
#define FUDGE_FACTOR    1.2e+00


/* static variables for t9416: */
struct LOC_t9416 {
  int mod_id;
} ;

static void close_the_relays_____(struct LOC_t9416 *LINK)
{
  vth_aclose_rly(LINK->mod_id, K2605);   /* MOA happy relay */
  /*08.01.89*/
  /* ADDED 'close 843' to put moa in a known state */
  /*08.09.90*/
  vth_aclose_rly(LINK->mod_id, K843);   /* MOA NON to GND */
  /* ADDED STABILIZATION WAIT */
  /*08.09.90*/
  vth_mtmr_wait(LINK->mod_id, WAIT_TIME_FOR_RELAYS);

  /* CLOSE THE P.S. NEGATIVE  TO GROUND PATH RELAYS */
  vth_aclose_rly(LINK->mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(LINK->mod_id, K718);   /* X2 bus to L bus relay */
  vth_aclose_rly(LINK->mod_id, K857);   /* L bus to GROUND relay */

  /* CLOSE THE MOAOUT TO SYN. POSITIVE  PATH RELAYS */
  vth_aclose_rly(LINK->mod_id, K807);   /* MOAOUT to S bus relay */
  vth_aclose_rly(LINK->mod_id, K733);   /* X1 bus to S bus relay */
  vth_aclose_rly(LINK->mod_id, K741);   /* X1 bus to mint pin relay */

}


void t9416(int mod_id_, int /*slot*/)
{
  /* MOA NEGATIVE OVER VOLTAGE LIMIT ACCURACY */
  struct LOC_t9416 V;

  boolean vlimit;
  /* ADDED ever_had_a_vlimit TO KEEP FROM RETRYING THE SAME VALUES*/
  /*08.20.90*/
  boolean ever_had_a_vlimit;

  double syn_voltage; 
  double olym_ch1_voltage;	//added by chuanho, 14jun2012
  double voltage_increment;

  int over_voltage_bit;
  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id_;

  OpenResourceManager();
  open_mux(V.mod_id);
  open_dmm(V.mod_id);
  open_33250A(V.mod_id);

  /*New Olympus codes*/
  //Added by chuanho, 14jun2012
  if(check_syn_model())
  {
  open_olym(V.mod_id);
  connect_olym_ch1(V.mod_id); 
  aconnect_olym_ch1_abus1_xmint(V.mod_id);

  /* SETUP TO DO POSITIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE MOA SOURCE */
  /* !! THERE IS NO WAY TO EXPLICITLY TRI-STATE THE MOA !! */

  vth_amoa_en_30ma_ilim(V.mod_id);
  /* ADDED 'vth_amoa_set_v_cmpl(mod_id,moa_comp)' */
  /*08.09.90*/
  vth_amoa_set_v_cmpl(V.mod_id, MOA_COMP);

  olym_ch1_voltage = OLYM_MIN_TEST_VOLTAGE; 
  voltage_increment = STARTING_INCREMENT;
  vlimit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*08.09.90*/
  close_the_relays_____(&V);

  setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
  setup_olym_ch1_curr(V.mod_id, OLYM_CH1_CURR); 
  setup_olym_ch1_ocp(V.mod_id, OLYM_CH1_OCP); 
  enable_olym_ch1_out(V.mod_id); 
  reverse_olym_ch1_out(V.mod_id);
  vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
  vth_astat_clr(V.mod_id);

  while (!vlimit && olym_ch1_voltage <= OLYM_MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      /* Set the condition such that the while loop will terminate */
      olym_ch1_voltage = MAX_TEST_VOLTAGE - 1; 
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^11  =  BIT NUMBER 4 */
//mjs -- another messed up bit problem
    over_voltage_bit = P_getbits_US(this_word.bits, 11, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      vlimit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= OLYM_FINEST_INCREMENT ||
          olym_ch1_voltage == OLYM_MIN_TEST_VOLTAGE) 
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*08.09.90*/
      vlimit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*08.09.90*/
      disable_olym_ch1_out(V.mod_id); 
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays_____(&V);

      /* DIVIDE THE INCREMENT IN HALF TO GO BACK TO A "HALF" STEP FROM */
      /* WHERE THE LIMIT OCCURRED, YOU DON'T HAVE TO GO BACK ALL THE */
      /* WAY BECAUSE YOU KNOW NO LIMIT OCCURRED AT THE LAST "FULL" STEP */
      voltage_increment /= 2;
      olym_ch1_voltage -= voltage_increment;
      setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage); 
	  enable_olym_ch1_out(V.mod_id);
      /* ADDED SYN WAIT TIME */
      /*08.09.90*/
      vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
      continue;
    }  /* Go back to the last voltage and reduce the increment */
//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, OLYM_FINEST_INCREMENT) )
      voltage_increment /= 2;
    olym_ch1_voltage += voltage_increment; 
	setup_olym_ch1_volt(V.mod_id, olym_ch1_voltage);
    vth_mtmr_wait(V.mod_id, OLYM_WAIT_TIME); 
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, olym_ch1_voltage * FUDGE_FACTOR, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_olym_ch1_out_dmm(V.mod_id, true); 
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  disable_olym_ch1_out(V.mod_id);
  vth_ainit(V.mod_id);
  close_olym(V.mod_id);
  close_33250A(V.mod_id);
  
  /* Go to the next increment */
  /* Go to the next increment */
  /* No result to read */
  /* No result to read */
  }

  else
  {
  open_syn(V.mod_id); 
  setup_syn(V.mod_id, DC, 0, 0.0, 0.0); 
  setup_syn_hv(V.mod_id, true); 

  aconnect_syn_out_xmint(V.mod_id); 

  /* SETUP TO DO POSITIVE OVERVOLTAGE MEASUREMENT */
  /* EXPLICITLY TRI-STATE THE MOA SOURCE */
  /* !! THERE IS NO WAY TO EXPLICITLY TRI-STATE THE MOA !! */

  vth_amoa_en_30ma_ilim(V.mod_id);
  /* ADDED 'vth_amoa_set_v_cmpl(mod_id,moa_comp)' */
  /*08.09.90*/
  vth_amoa_set_v_cmpl(V.mod_id, MOA_COMP);

  syn_voltage = MIN_TEST_VOLTAGE; 
  voltage_increment = STARTING_INCREMENT;
  vlimit = false;
  ever_had_a_vlimit = false;
  /* PULLED THE FOLLOWING FOUR LINES OUT OF THE LOOP */
  /*08.09.90*/
  close_the_relays_____(&V);

  setup_syn_off(V.mod_id, syn_voltage); 
  vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  vth_astat_clr(V.mod_id);

  while (!vlimit && syn_voltage >= MAX_TEST_VOLTAGE) 
  {  /*search for limit level*/
    /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      /* Set the condition such that the while loop will terminate */
      syn_voltage = MAX_TEST_VOLTAGE - 1; 
      continue;
    }

    this_word.data = result.UU.Integer_result;
    /* BITS IN THISWORD ARE NUMBERED:   0  1  2  3  4  5  6  7  8  9  ,  etc */
    /* REPRESENTING 2 ^ THESE NUMBERS: 15 14 13 12 11 10  9  8  7  6  ,  etc */
    /* THEREFORE 2^11  =  BIT NUMBER 4 */
//mjs -- another messed up bit problem
    over_voltage_bit = P_getbits_US(this_word.bits, 11, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      vlimit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT ||
        syn_voltage == MIN_TEST_VOLTAGE) 
        continue;
      /* ADDED 'AND (syn_voltage <> min_test_voltage' */
      /*08.09.90*/
      vlimit = false;
      /* ADDED THE NEXT 3 LINES AFTER TAKING IT OUT OF THE LOOP */
      /*08.09.90*/
      setup_syn_off(V.mod_id, 0.0); 
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays_____(&V);

      /* DIVIDE THE INCREMENT IN HALF TO GO BACK TO A "HALF" STEP FROM */
      /* WHERE THE LIMIT OCCURRED, YOU DON'T HAVE TO GO BACK ALL THE */
      /* WAY BECAUSE YOU KNOW NO LIMIT OCCURRED AT THE LAST "FULL" STEP */
      voltage_increment /= 2;
      syn_voltage += voltage_increment; 
      setup_syn_off(V.mod_id, syn_voltage); 
      /* ADDED SYN WAIT TIME */
      /*08.09.90*/
      vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
      continue;
    }  /* Go back to the last voltage and reduce the increment */
//    if ( !number_equal(ever_had_a_vlimit && voltage_increment, FINEST_INCREMENT) )
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    syn_voltage -= voltage_increment; 
	setup_syn_off(V.mod_id, syn_voltage); 
    vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME); 
  }  /*search for limit level*/



  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, syn_voltage * FUDGE_FACTOR, RESOLUTION); 
  connect_dmm(V.mod_id);
  aconnect_syn_out_dmm(V.mod_id, true); 
  measure_dmm(V.mod_id);
  if (!get_result_log(V.mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  setup_syn_off(V.mod_id, 0.0); 
  setup_syn_hv(V.mod_id, false); 
  vth_ainit(V.mod_id);
  close_syn(V.mod_id);

  /* Go to the next increment */
  /* Go to the next increment */
  /* No result to read */
  /* No result to read */
  }

  close_mux(V.mod_id);
  close_dmm(V.mod_id);
  CloseResourceManager();
  
}  /* t9416 */


#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef SYN_WAIT_TIME 
#undef OLYM_MIN_TEST_VOLTAGE	//added by chuanho, 14jun2012
#undef OLYM_MAX_TEST_VOLTAGE	//added by chuanho, 14jun2012
#undef OLYM_FINEST_INCREMENT	//added by chuanho, 14jun2012
#undef OLYM_WAIT_TIME			//added by chuanho, 14jun2012
#undef OLYM_CH1_CURR			//added by chuanho, 14jun2012
#undef OLYM_CH1_OCP				//added by chuanho, 14jun2012
#undef MOA_COMP
#undef RESOLUTION
#undef FUDGE_FACTOR


/* Revision Control */



#define CHANNEL         'A'

#define MOA_DEFAULT_RESISTANCE  0
/* 10 ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* unenhanced */
#define MEAS_TYPE       0

#define POSITIVE_TEST_VOLTAGE  100e-03
#define NEGATIVE_TEST_VOLTAGE  (-100e-03)
#define RESOLUTION      1.0e-03
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  1.0e-03
/* seconds */
#define ENABLE_WAIT_TIME  1.0e-03

#define DMM_FUNCTION    DCV


void t9420(int mod_id, int /*slot*/)
{
  /* MOA DC OUTPUT RESISTANCE */
  double r10;
  double dmm_no_load_voltage;
  double dmm_loaded_voltage;
  double moa_resistance;

  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  setup_dmm(mod_id, DMM_FUNCTION, POSITIVE_TEST_VOLTAGE, RESOLUTION);

  connect_dmm(mod_id);
  connect_plus_xn(mod_id, CHANNEL, true);
  connect_minus_calib2(mod_id);

  vth_aopen_rly(mod_id, K843);   /* MOANON to GROUND relay */
  vth_aclose_rly(mod_id, K842);   /* AUXOUT to MOANON relay */
  vth_aclose_rly(mod_id, K2605);   /* MOA happy relay */
  vth_aclose_rly(mod_id, K821);   /* MOAOUT to A bus relay */
  vth_aclose_rly(mod_id, K709);   /* A bus to X1 bus relay */
  vth_aclose_rly(mod_id, K741);   /* X1 bus to mint pin relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

  vth_aaux_en(mod_id);
  vth_aaux_set_ampl(mod_id, POSITIVE_TEST_VOLTAGE);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
    dmm_no_load_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);
  vth_aaux_dis(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  vth_aclose_rly(mod_id, K822);
  vth_aclose_rly(mod_id, K828);
  vth_aclose_rly(mod_id, K845);
  vth_aclose_rly(mod_id, K855);

  vth_aaux_en(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*measure the loaded voltage, calculate resistance and log*/
    dmm_loaded_voltage = result.UU.Real_result;
    if (dmm_loaded_voltage != 0)
      moa_resistance =
        (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
    else
      moa_resistance = MOA_DEFAULT_RESISTANCE;
    result.UU.Real_result = moa_resistance;
    log_result(mod_id, result);
  }  /*measure the loaded voltage, calculate resistance and log*/
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_aaux_set_ampl(mod_id, NEGATIVE_TEST_VOLTAGE);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
    dmm_loaded_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);
  vth_aaux_dis(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  vth_aopen_rly(mod_id, K822);
  vth_aopen_rly(mod_id, K828);
  vth_aopen_rly(mod_id, K845);
  vth_aopen_rly(mod_id, K855);

  vth_aaux_en(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*measure the unloaded voltage, calculate resistance and log*/
    dmm_no_load_voltage = result.UU.Real_result;
    if (dmm_loaded_voltage != 0)
      moa_resistance =
        (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
    else
      moa_resistance = MOA_DEFAULT_RESISTANCE;
    result.UU.Real_result = moa_resistance;
    log_result(mod_id, result);
  }  /*measure the loaded voltage, calculate resistance and log*/
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_aaux_dis(mod_id);

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9420 */


#undef CHANNEL
#undef MOA_DEFAULT_RESISTANCE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef POSITIVE_TEST_VOLTAGE
#undef NEGATIVE_TEST_VOLTAGE
#undef RESOLUTION
#undef SOURCE_WAIT_TIME
#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION


/* Revision Control */



#define PIN_SLOT        2
/* 10  ohm feedback */
#define FEEDBACK_RESISTOR_NO  0
/* 10  ohm feedback */

#define FEEDBACK_RESISTOR_RELAY  K845

#define MOA_GAIN        10
/* unenhanced */
#define MEAS_TYPE       0
#define DEFAULT_MOA_RESISTANCE  0
#define STOP_FREQ       20000

#define RMS_TEST_VOLTAGE  100e-03
#define DC_OFFSET       0e+00
#define V_COMP          5e+00
#define LOW_BAND_RATIO  0.8e+00
#define FUDGE_FACTOR    1.2e+00
#define RESOLUTION      85e-06

#define DMM_FUNCTION    ACV


#define FILTER_8192_    7

#define FREQ_A_________  8192


void t9421(int mod_id, int /*slot*/)
{
  /* MOA AC OUTPUT RESISTANCE */
  int start_freq;

  double rload;
  double five_tau;
  double dmm_no_load_voltage;
  double dmm_loaded_voltage;
  double moa_ac_resistance;

  test_result_type result;
  int valid_cards;
int p2c_temp;


  valid_cards = (1 << PIN) | (1 << PIN6) | (1 << PINTERM) | (1 << PIN20) |
                (1 << CP) | (1 << CP20);
  p2c_temp = get_testhead_actual_card(mod_id, PIN_SLOT);
  if (!(((1 << p2c_temp) & valid_cards) != 0))
  {  /*Display no slot 2 message*/
    display_prompt("MOA AC OUTPUT RESISTANCE requires a pin or channel", true);
    display_prompt("type in slot 2.  Test 9421 will not be run!", true);
  }  /*Display no slot 2 message*/
  else
  {  /* After check for slot 2 pin card */
    open_dmm(mod_id);
    open_mux(mod_id);

    rload = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

    setup_dmm(mod_id, DMM_FUNCTION,
              RMS_TEST_VOLTAGE * sqrt(2.0) * MOA_GAIN * FUDGE_FACTOR,
              RESOLUTION);
    setup_dmm_level_filter(mod_id, true);

    connect_dmm(mod_id);
    connect_plus_calib1(mod_id);
    connect_minus_calib2(mod_id);

    /* CONNECT SOURCE OUT TO MOAINV WITH SLOT 2 RELAYS TO COUPLE 1k SERIES R */
    vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
    vth_aclose_rly(mod_id, K733);   /* S bus to X1 bus relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KDC1);   /* X1 bus to X1BUS relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KX1);   /* X1BUS to R0 relay */
    vth_pclose_rly(mod_id, PIN_SLOT, K16A);   /* R0 to mint pin (1k) relay */

    vth_pclose_rly(mod_id, PIN_SLOT, K16D);   /* mint pin (1k) relay to R3 */
    vth_pclose_rly(mod_id, PIN_SLOT, KX4);   /* R3 to X4BUS relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KDC4);   /* X4BUS to X4 bus relay */
    vth_aclose_rly(mod_id, K704);   /* X4 bus to B bus relay */
    vth_aclose_rly(mod_id, K824);   /* B bus to MOAINV relay */

    /* CONNECT MOAINV TO MOAOUT WITH pin_slot 2 RELAYS TO COUPLE 10k SERIES R */
    vth_aclose_rly(mod_id, K843);   /* MOANON to GROUND relay */

    /* CONNECT MOAINV TO MOAOUT WITH pin_slot 2 RELAYS TO COUPLE 10k SERIES R */
    vth_aclose_rly(mod_id, K705);   /* B bus to X5 bus relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KDC5);   /* X5 bus to X5BUS relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KX5);   /* X5BUS to R4 relay */
    vth_pclose_rly(mod_id, PIN_SLOT, K12E);   /* R4 to mint pin (10k) relay */

    vth_pclose_rly(mod_id, PIN_SLOT, K12H);   /* mint pin (10k) relay to R7 */
    vth_pclose_rly(mod_id, PIN_SLOT, KX8);   /* R7 to X8BUS relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KDC8);   /* X8BUS to X8 bus relay */
    vth_aclose_rly(mod_id, K716);   /* X8 bus to A bus relay */
    vth_aclose_rly(mod_id, K821);   /* A bus to MOAOUT relay */

    /* CONNECT THE DMM WITH NO LOAD TO MEASURE THE MOA NO LOAD VOLTAGE */
    vth_aclose_rly(mod_id, K838);   /* MOAOUT to RCAL2 relay */
    vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 relay */
    vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

    /* SETUP THE AC BAND FILTERING ON THE DMM */
    five_tau = 5.0 / FREQ_A_________;
    start_freq = static_cast<int>(FREQ_A_________ * LOW_BAND_RATIO);
    setup_dmm_ac_band(mod_id, start_freq, STOP_FREQ);

    /* SETUP THE AC SOURCE TO DRIVE THE MOA */

    vth_asrc_sel_fltr(mod_id, FILTER_8192_);
    vth_ascat_cpu_init(mod_id);
    vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
    vth_ascat_set_a_freq(mod_id, FREQ_A_________);
    vth_ascat_en_cont_src(mod_id);
    vth_ascat_en_frun_src(mod_id);
    vth_ascat_run_en(mod_id);
    vth_asrc_en_30ma_ilim(mod_id);
    vth_asrc_set_ampl(mod_id, RMS_TEST_VOLTAGE, DC_OFFSET);
    vth_afc_en_src_out(mod_id);

    /* SETUP THE MOA */
    vth_amoa_set_v_cmpl(mod_id, V_COMP);
    vth_amoa_sel_wb(mod_id);   /* Needs wb for open loop gain at 8192 */
    vth_amoa_dis_30ma_ilim(mod_id);

    /* OPEN THE MOA HAPPY RELAY */
    vth_aopen_rly(mod_id, K2605);
    vth_mtmr_wait(mod_id, five_tau);

    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
      dmm_no_load_voltage = result.UU.Real_result;
    else
      display_prompt("No result in result log (Expected dmm vth_reading)", true);
    vth_afc_dis_src_out(mod_id);
    vth_mtmr_wait(mod_id, five_tau);

    /* NOW CLOSE THE LOAD INTO THE OUTPUT PATH */
    vth_aclose_rly(mod_id, K827);   /* MOAOUT to REFR2 relay */
    vth_aclose_rly(mod_id, FEEDBACK_RESISTOR_RELAY);
        /* REFR2 to load to I bus */
    vth_aclose_rly(mod_id, K855);   /* I bus to CALIB2 */

    vth_afc_en_src_out(mod_id);
    vth_mtmr_wait(mod_id, five_tau);

    measure_dmm(mod_id);
    if (get_result_nolog(mod_id, &result))
    {  /*measure the loaded voltage, calculate resistance and log*/
      dmm_loaded_voltage = result.UU.Real_result;
      if (dmm_loaded_voltage != 0)
        moa_ac_resistance = (dmm_no_load_voltage - dmm_loaded_voltage) /
                            dmm_loaded_voltage * rload;
      else
        moa_ac_resistance = DEFAULT_MOA_RESISTANCE;
      result.UU.Real_result = moa_ac_resistance;
      log_result(mod_id, result);
    }  /*measure the loaded voltage, calculate resistance and log*/
    else
      display_prompt("No result in result log (Expected dmm vth_reading)", true);

    vth_afc_dis_src_out(mod_id);

    close_dmm(mod_id);
    close_mux(mod_id);

  }  /* After check for slot 2 pin card */



}  /* t9421 */


#undef PIN_SLOT
#undef FEEDBACK_RESISTOR_NO
#undef FEEDBACK_RESISTOR_RELAY
#undef MOA_GAIN
#undef MEAS_TYPE
#undef DEFAULT_MOA_RESISTANCE
#undef STOP_FREQ
#undef RMS_TEST_VOLTAGE
#undef DC_OFFSET
#undef V_COMP
#undef LOW_BAND_RATIO
#undef FUDGE_FACTOR
#undef RESOLUTION
#undef DMM_FUNCTION
#undef FILTER_8192_
#undef FREQ_A_________


#define LOW             0
#define HIGH            1

#define SOURCING        0
#define SINKING         1

#define POSITIVE        0
#define NEGATIVE        1

typedef double i_type_array__[2];
typedef i_type_array__ i_direction_array____[2];
typedef i_direction_array____ voltage_array__________[2];


/* Revision Control */



/* 10 ohm feedback */

#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        50
/* NOT IMPLEMENTED (FOR M.A.D.) */
#define DIGITAL         0

#define TIMEOUT         10.0e+00
#define POWER_SUPPLY_CURRENT  800e-03
#define POWER_SUPPLY_RAMP_TIME  1.0e+00
#define MAX_EXPECTED_VOLTS  2.1e+00

const voltage_array__________ SOURCE_VOLTAGE_______ = {
  { { -1.0e+00, -1.0e+00 },
    { 1.0e+00, 1.0e+00 } },
  { { -1.0e+00, -1.0e+00 },
    { 1.0e+00, 1.0e+00 } }
};

const voltage_array__________ PS_VOLTAGE____ = {
  { { 7.0e+00, 7.0e+00 },
    { 9.0e+00, 9.0e+00 } },
  { { -9.0e+00, -9.0e+00 },
    { -7.0e+00, -7.0e+00 } }
};


void t9426(int mod_id, int /*slot*/)
{
  /* MOA MAXIMUM OUTPUT CURRENT */
  double r10;
  double max_current;

  test_result_type result;


  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_ps(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);
  vth_aopen_rly(mod_id, K812);

  /* CLOSE THE MOAOUT TO P.S. PATH RELAYS */
  vth_aclose_rly(mod_id, K843);   /* MOANON to GROUND relay */
  vth_aclose_rly(mod_id, K801);   /* SRCOUT to MOAINV relay */
  vth_aclose_rly(mod_id, K802);   /* SRCOUT to MOAINV relay */
  vth_aclose_rly(mod_id, K2605);   /* MOA happy relay */
  vth_aclose_rly(mod_id, K821);   /* MOAOUT to A bus relay */
  vth_aclose_rly(mod_id, K822);   /* DETHI to A bus relay */
  vth_aclose_rly(mod_id, K828);   /* 10 ohm resistor to DETHI */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */

  /* CLOSE THE P.S. TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAY */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */

  for (int current = LOW; current <= HIGH; ++current)
  {  /* SETUP FOR LOW AND HIGH CURRENT MAXIMUMS */
    if (current == LOW)
      vth_amoa_en_30ma_ilim(mod_id);
    else
      vth_amoa_dis_30ma_ilim(mod_id);

    for (int src_polarity = POSITIVE; src_polarity <= NEGATIVE; ++src_polarity)
    {  /* SETUP FOR POSITIVE AND NEGATIVE SOURCE VOLTAGES */
      if (src_polarity == POSITIVE)
      {  /* CONNECT PS + TO X1 AND PS - TO X2 */
        vth_aopen_rly(mod_id, K726);
        vth_aopen_rly(mod_id, K717);
        vth_aclose_rly(mod_id, K725);
        vth_aclose_rly(mod_id, K718);
      }  /* CONNECT PS + TO X1 AND PS - TO X2 */
      else
      {  /* CONNECT PS + TO X2 AND PS - TO X1 */
        vth_aopen_rly(mod_id, K725);
        vth_aopen_rly(mod_id, K718);
        vth_aclose_rly(mod_id, K726);
        vth_aclose_rly(mod_id, K717);
      }  /* CONNECT PS + TO X2 AND PS - TO X1 */

      for (int direction = SOURCING; direction <= SINKING; ++direction)
      {  /* SETUP FOR SOURCING AND SINKING CONDITIONS */
        vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE_______[src_polarity]
                         [direction][current]);
        vth_afc_en_src_out(mod_id);

        setup_ps(mod_id, PS_VOLTAGE____[src_polarity][direction]
                 [current], POWER_SUPPLY_CURRENT);

        vth_aopen_rly(mod_id, K2605);   /* MOA happy relay */
        vth_mtmr_wait(mod_id, POWER_SUPPLY_RAMP_TIME);

        vth_afunc_meas(mod_id);
        vth_get_afunc_result(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          max_current = result.UU.Real_result / r10;
          result.UU.Real_result = fabs(max_current);
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected DET volts)", true);

        if (!get_result_nolog(mod_id, &result))
          display_prompt("No result in result log (Expected DET stat)", true);

        vth_aclose_rly(mod_id, K2605);   /* MOA happy relay */
        vth_afc_dis_src_out(mod_id);

      }


    }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */


  }  /* for CURRENT */


  vth_ainit(mod_id);

  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9426 */


///////////////////////////////T9425 ADDED FOR NASRU TESTING, EQUAL TO T9426 WITH LOOSER SPECS///////////

void t9425(int mod_id, int /*slot*/)
{
  /* MOA MAXIMUM OUTPUT CURRENT */
  double r10;
  double max_current;

  test_result_type result;


  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_ps(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);
  vth_aopen_rly(mod_id, K812);

  /* CLOSE THE MOAOUT TO P.S. PATH RELAYS */
  vth_aclose_rly(mod_id, K843);   /* MOANON to GROUND relay */
  vth_aclose_rly(mod_id, K801);   /* SRCOUT to MOAINV relay */
  vth_aclose_rly(mod_id, K802);   /* SRCOUT to MOAINV relay */
  vth_aclose_rly(mod_id, K2605);   /* MOA happy relay */
  vth_aclose_rly(mod_id, K821);   /* MOAOUT to A bus relay */
  vth_aclose_rly(mod_id, K822);   /* DETHI to A bus relay */
  vth_aclose_rly(mod_id, K828);   /* 10 ohm resistor to DETHI */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */

  /* CLOSE THE P.S. TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAY */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */

  for (int current = LOW; current <= HIGH; ++current)
  {  /* SETUP FOR LOW AND HIGH CURRENT MAXIMUMS */
    if (current == LOW)
      vth_amoa_en_30ma_ilim(mod_id);
    else
      vth_amoa_dis_30ma_ilim(mod_id);

    for (int src_polarity = POSITIVE; src_polarity <= NEGATIVE; ++src_polarity)
    {  /* SETUP FOR POSITIVE AND NEGATIVE SOURCE VOLTAGES */
      if (src_polarity == POSITIVE)
      {  /* CONNECT PS + TO X1 AND PS - TO X2 */
        vth_aopen_rly(mod_id, K726);
        vth_aopen_rly(mod_id, K717);
        vth_aclose_rly(mod_id, K725);
        vth_aclose_rly(mod_id, K718);
      }  /* CONNECT PS + TO X1 AND PS - TO X2 */
      else
      {  /* CONNECT PS + TO X2 AND PS - TO X1 */
        vth_aopen_rly(mod_id, K725);
        vth_aopen_rly(mod_id, K718);
        vth_aclose_rly(mod_id, K726);
        vth_aclose_rly(mod_id, K717);
      }  /* CONNECT PS + TO X2 AND PS - TO X1 */

      for (int direction = SOURCING; direction <= SINKING; ++direction)
      {  /* SETUP FOR SOURCING AND SINKING CONDITIONS */
        vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE_______[src_polarity]
                         [direction][current]);
        vth_afc_en_src_out(mod_id);

        setup_ps(mod_id, PS_VOLTAGE____[src_polarity][direction]
                 [current], POWER_SUPPLY_CURRENT);

        vth_aopen_rly(mod_id, K2605);   /* MOA happy relay */
        vth_mtmr_wait(mod_id, POWER_SUPPLY_RAMP_TIME);

        vth_afunc_meas(mod_id);
        vth_get_afunc_result(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          max_current = result.UU.Real_result / r10;
          result.UU.Real_result = fabs(max_current);
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected DET volts)", true);

        if (!get_result_nolog(mod_id, &result))
          display_prompt("No result in result log (Expected DET stat)", true);

        vth_aclose_rly(mod_id, K2605);   /* MOA happy relay */
        vth_afc_dis_src_out(mod_id);

      }


    }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */


  }  /* for CURRENT */


  vth_ainit(mod_id);

  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9425 */


///////////////////////////////SHARED BETWEEN THE T9246 AND T9245/////////////////////


#undef LOW
#undef HIGH
#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE

#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef TIMEOUT
#undef POWER_SUPPLY_CURRENT
#undef POWER_SUPPLY_RAMP_TIME
#undef MAX_EXPECTED_VOLTS

////////////////////////////////END OF SHARING///////////////////////////////////////


#define SOURCING        0
#define SINKING         1

#define POSITIVE        0
#define NEGATIVE        1

typedef double i_direction_array_____[2];
typedef i_direction_array_____ voltage_array___________[2];


/* Revision Control */



/* 10 ohm feedback */

#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        50
/* NOT IMPLEMENTED (FOR M.A.D.) */
#define DIGITAL         0

#define TIMEOUT         10.0e+00
#define POWER_SUPPLY_CURRENT  800e-03
#define POWER_SUPPLY_RAMP_TIME  1.0e+00
#define MAX_EXPECTED_VOLTS  2.1e+00

const voltage_array___________ SOURCE_VOLTAGE________ = {
  { -1.0e+00, 1.0e+00 },
  { -1.0e+00, 1.0e+00 }
};

const voltage_array___________ PS_VOLTAGE_____ = {
  { 7.0e+00, 9.0e+00 },
  { -9.0e+00, -7.0e+00 }
};


void t9427(int mod_id, int /*slot*/)
{
  /* MOA MAXIMUM OUTPUT CURRENT (HIGH LEVEL ONLY) */
  double r10;
  double max_current;

  test_result_type result;


  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_ps(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);
  vth_aopen_rly(mod_id, K812);

  /* CLOSE THE MOAOUT TO P.S. PATH RELAYS */
  vth_aclose_rly(mod_id, K843);   /* MOANON to GROUND relay */
  vth_aclose_rly(mod_id, K801);   /* SRCOUT to MOAINV relay */
  vth_aclose_rly(mod_id, K802);   /* SRCOUT to MOAINV relay */
  vth_aclose_rly(mod_id, K2605);   /* MOA happy relay */
  vth_aclose_rly(mod_id, K821);   /* MOAOUT to A bus relay */
  vth_aclose_rly(mod_id, K822);   /* DETHI to A bus relay */
  vth_aclose_rly(mod_id, K828);   /* 10 ohm resistor to DETHI */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */

  /* CLOSE THE P.S. TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAY */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */

  vth_amoa_dis_30ma_ilim(mod_id);

  for (int src_polarity = POSITIVE; src_polarity <= NEGATIVE; ++src_polarity)
  {  /* SETUP FOR POSITIVE AND NEGATIVE SOURCE VOLTAGES */
    if (src_polarity == POSITIVE)
    {  /* CONNECT PS + TO X1 AND PS - TO X2 */
      vth_aopen_rly(mod_id, K726);
      vth_aopen_rly(mod_id, K717);
      vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K725);
      vth_aclose_rly(mod_id, K718);
    }  /* CONNECT PS + TO X1 AND PS - TO X2 */
    else
    {  /* CONNECT PS + TO X2 AND PS - TO X1 */
      vth_aopen_rly(mod_id, K725);
      vth_aopen_rly(mod_id, K718);
      vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K726);
      vth_aclose_rly(mod_id, K717);
    }  /* CONNECT PS + TO X2 AND PS - TO X1 */

    for (int direction = SOURCING; direction <= SINKING; ++direction)
    {  /* SETUP FOR SOURCING AND SINKING CONDITIONS */
      vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE________[src_polarity]
                       [direction]);
      vth_afc_en_src_out(mod_id);

      setup_ps(mod_id, PS_VOLTAGE_____[src_polarity]
               [direction], POWER_SUPPLY_CURRENT);

      vth_aopen_rly(mod_id, K2605);   /* MOA happy relay */
      vth_mtmr_wait(mod_id, POWER_SUPPLY_RAMP_TIME);

      vth_afunc_meas(mod_id);
      vth_get_afunc_result(mod_id);
      if (get_result_nolog(mod_id, &result))
      {
        max_current = result.UU.Real_result / r10;
        result.UU.Real_result = fabs(max_current);
        log_result(mod_id, result);
      }
      else
        display_prompt("No result in result log (Expected DET volts)", true);

      if (!get_result_nolog(mod_id, &result))
        display_prompt("No result in result log (Expected DET stat)", true);

      vth_aclose_rly(mod_id, K2605);   /* MOA happy relay */
      vth_afc_dis_src_out(mod_id);

    }  /* SETUP FOR SOURCING AND SINKING CONDITIONS */


  }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */


  vth_ainit(mod_id);

  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9427 */


#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE

#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef TIMEOUT
#undef POWER_SUPPLY_CURRENT
#undef POWER_SUPPLY_RAMP_TIME
#undef MAX_EXPECTED_VOLTS


///////////////////////////////////T9428 equal to T9427 with looser test tolerance//////////

#define SOURCING        0
#define SINKING         1

#define POSITIVE        0
#define NEGATIVE        1

typedef double i_direction_array_____[2];
typedef i_direction_array_____ voltage_array___________[2];


/* Revision Control */



/* 10 ohm feedback */

#define FEEDBACK_RESISTOR_NO  0
/* enhanced */
#define MEAS_TYPE       1
/* LONG INTEGRATION TIME */
#define LENGTH          1
/* FOR THE DETECTOR */
#define AVERAGES        50
/* NOT IMPLEMENTED (FOR M.A.D.) */
#define DIGITAL         0

#define TIMEOUT         10.0e+00
#define POWER_SUPPLY_CURRENT  800e-03
#define POWER_SUPPLY_RAMP_TIME  1.0e+00
#define MAX_EXPECTED_VOLTS  2.1e+00

const voltage_array___________ SOURCE_VOLTAGE________NASRU = {
  { -1.0e+00, 1.0e+00 },
  { -1.0e+00, 1.0e+00 }
};

const voltage_array___________ PS_VOLTAGE_____NASRU = {
  { 7.0e+00, 9.0e+00 },
  { -9.0e+00, -7.0e+00 }
};


void t9428(int mod_id, int /*slot*/)
{
  /* MOA MAXIMUM OUTPUT CURRENT (HIGH LEVEL ONLY) */
  double r10;
  double max_current;

  test_result_type result;


  vth_apowered_init(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
  open_ps(mod_id);
  open_mux(mod_id);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_aopen_rly(mod_id, K820);
  vth_aopen_rly(mod_id, K812);

  /* CLOSE THE MOAOUT TO P.S. PATH RELAYS */
  vth_aclose_rly(mod_id, K843);   /* MOANON to GROUND relay */
  vth_aclose_rly(mod_id, K801);   /* SRCOUT to MOAINV relay */
  vth_aclose_rly(mod_id, K802);   /* SRCOUT to MOAINV relay */
  vth_aclose_rly(mod_id, K2605);   /* MOA happy relay */
  vth_aclose_rly(mod_id, K821);   /* MOAOUT to A bus relay */
  vth_aclose_rly(mod_id, K822);   /* DETHI to A bus relay */
  vth_aclose_rly(mod_id, K828);   /* 10 ohm resistor to DETHI */
  vth_aclose_rly(mod_id, K845);   /* I bus to 10 ohm resistor relay */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */

  /* CLOSE THE P.S. TO GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to ground */

  /* CLOSE THE DETECTOR MEASUREMENT RELAY */
  vth_aclose_rly(mod_id, K813);   /* DETLO input */

  vth_amoa_dis_30ma_ilim(mod_id);

  for (int src_polarity = POSITIVE; src_polarity <= NEGATIVE; ++src_polarity)
  {  /* SETUP FOR POSITIVE AND NEGATIVE SOURCE VOLTAGES */
    if (src_polarity == POSITIVE)
    {  /* CONNECT PS + TO X1 AND PS - TO X2 */
      vth_aopen_rly(mod_id, K726);
      vth_aopen_rly(mod_id, K717);
      vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K725);
      vth_aclose_rly(mod_id, K718);
    }  /* CONNECT PS + TO X1 AND PS - TO X2 */
    else
    {  /* CONNECT PS + TO X2 AND PS - TO X1 */
      vth_aopen_rly(mod_id, K725);
      vth_aopen_rly(mod_id, K718);
      vth_mtmr_wait(mod_id, 0.0005);   /* Required for Coto relays so that   */
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K726);
      vth_aclose_rly(mod_id, K717);
    }  /* CONNECT PS + TO X2 AND PS - TO X1 */

    for (int direction = SOURCING; direction <= SINKING; ++direction)
    {  /* SETUP FOR SOURCING AND SINKING CONDITIONS */
      vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE________NASRU[src_polarity]
                       [direction]);
      vth_afc_en_src_out(mod_id);

      setup_ps(mod_id, PS_VOLTAGE_____NASRU[src_polarity]
               [direction], POWER_SUPPLY_CURRENT);

      vth_aopen_rly(mod_id, K2605);   /* MOA happy relay */
      vth_mtmr_wait(mod_id, POWER_SUPPLY_RAMP_TIME);

      vth_afunc_meas(mod_id);
      vth_get_afunc_result(mod_id);
      if (get_result_nolog(mod_id, &result))
      {
        max_current = result.UU.Real_result / r10;
        result.UU.Real_result = fabs(max_current);
        log_result(mod_id, result);
      }
      else
        display_prompt("No result in result log (Expected DET volts)", true);

      if (!get_result_nolog(mod_id, &result))
        display_prompt("No result in result log (Expected DET stat)", true);

      vth_aclose_rly(mod_id, K2605);   /* MOA happy relay */
      vth_afc_dis_src_out(mod_id);

    }  /* SETUP FOR SOURCING AND SINKING CONDITIONS */


  }  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */


  vth_ainit(mod_id);

  close_ps(mod_id);
  close_mux(mod_id);

}  /* t9428 */


#undef SOURCING
#undef SINKING
#undef POSITIVE
#undef NEGATIVE

#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef TIMEOUT
#undef POWER_SUPPLY_CURRENT
#undef POWER_SUPPLY_RAMP_TIME
#undef MAX_EXPECTED_VOLTS


//////////////////////////////////////end of T9428/////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////

/* Revision Control */



#define NUM_FREQS_TO_TEST  3


#define NB              0
#define WB              1

typedef int v_type_;

typedef double varray_type[2];
typedef int frequency_array_________[NUM_FREQS_TO_TEST];


#define PIN_SLOT        2
#define F128            5
#define F1024           6
#define F8192           7
#define R_RATIO         227

#define FUDGE_FACTOR    1.2e+00
#define RESOLUTION      500e-06
#define SOURCE_WAIT_TIME  1.0e-03
#define RMS_VOLTS       3.536e+00
#define DC_OFFSET       0.0e+00

const varray_type VDC_SET = {
  5.0e00, -5.0e00
};

const frequency_array_________ FILTER_______ = {
  F128, F1024, F8192
};

const frequency_array_________ FREQ_A_________ = {
  128, 1024, 8192
};


void t9440(int mod_id, int /*slot*/)
{
  /* MOA CMRR */
  int current_position;
  int next_position;
  v_type_ voltage;
  int this_frequency;

  double cmrr;
  double peak_volts;

  varray_type vs;
  varray_type vo;

  test_result_type result;
  char s[81];
  int valid_cards;
  int p2c_temp;



  valid_cards = (1 << PIN) | (1 << PIN6) | (1 << PIN6JB) | (1 << PIN12JB) |
                (1 << PINTERM) | (1 << PIN20) | (1 << CP) | (1 << CP20) |
                (1 << ANALOG) | (1 << DD6) | (1 << DDJB) | (1 << DDP) |
                (1 << FDD20) | (1 << FDD6) | (1 << FDDP) | (1 << FDD20_H5) | (1 << FDD6_H5) | (1 << FDDP_H5) |
                (1 << DD20) | (1 << SD6) | (1 << SDP) | (1 << DDA) | (1 << FDDU);
  p2c_temp = get_testhead_actual_card(mod_id, PIN_SLOT);
  if (!(((1 << p2c_temp) & valid_cards) != 0))
  {  /*Display no slot 2 message*/
    *s = '\0';
    strcat(s, "Test ");
    current_position = mtd_strlen(s) + 1;
    sprintf(s + current_position - 1, "%d", 9440);
    next_position = mtd_strlen(s) + 1;
    strcat(s, ", Module ");
    current_position = mtd_strlen(s) + 1;
    sprintf(s + current_position - 1, "%d", mod_id);
    next_position = mtd_strlen(s) + 1;
    strcat(s, " DID NOT RUN.");
    display_prompt(s, true);
    *s = '\0';
    strcat(s, "MOA CMRR Test requires a pin or");
    display_prompt(s, true);
    *s = '\0';
    strcat(s, "channel type card in Slot ");
    current_position = mtd_strlen(s) + 1;
    sprintf(s + current_position - 1, "%d", PIN_SLOT);
    next_position = mtd_strlen(s) + 1;
    strcat(s, ".");
    display_prompt(s, true);
  }  /*Display no slot 2 message*/
  else
  {  /* After check for slot 2 pin card */
    open_dmm(mod_id);
    open_mux(mod_id);

    connect_dmm(mod_id);

    vth_aopen_rly(mod_id, K843);   /* MOANON to GND relay */
    vth_aopen_rly(mod_id, K2605);   /* MOA happy relay */

    vth_aclose_rly(mod_id, K806);   /* SRCOUT to S relay */
    vth_aclose_rly(mod_id, K738);   /* S to X6 relay */
    vth_aclose_rly(mod_id, K746);   /* X6 to X6MINT relay */
    vth_aclose_rly(mod_id, K819);   /* MOANON to L relay */
    vth_aclose_rly(mod_id, K721);   /* L to X5 relay */
    vth_aclose_rly(mod_id, K814);   /* Analog GND to G relay */
    vth_aclose_rly(mod_id, K752);   /* G to XG relay */
    vth_aclose_rly(mod_id, K811);   /* MOAINV to I relay */
    vth_aclose_rly(mod_id, K728);   /* I to X4 relay */
    vth_aclose_rly(mod_id, K821);   /* MOAOUT to A relay */
    vth_aclose_rly(mod_id, K711);   /* A to X3 relay */
    vth_aclose_rly(mod_id, K743);   /* X3 to X3MINT relay */

    /* GROUND REFERENCES FOR DMM PATH */
    vth_aclose_rly(mod_id, K747);   /* X7MINT to X7 relay */
    vth_aclose_rly(mod_id, K707);   /* X7 to B relay */
    vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 relay */
    vth_aclose_rly(mod_id, K702);   /* X2 to B relay */
    vth_aclose_rly(mod_id, K856);   /* B to DETLO relay */
    vth_aclose_rly(mod_id, K2305);   /* DETLO to analog GND relay */
    vth_aclose_rly(mod_id, K2303);   /* DETLO to analog GND relay */

    /* PIN CARD RELAYS */
    /* ---------------------------------------------------------------------- */
    /* Modified: 01.24.91 JRM ----------------------------------------------- */
    /* Added virt_card_select for SS cards, used mode 3 for compatibility.    */
    /* ---------------------------------------------------------------------- */
    virt_card_select(mod_id, PIN_SLOT, 3);   /* Select compatibility mode */
    vth_pclose_rly(mod_id, PIN_SLOT, KDC6);   /* X6 to X6BUS relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KX6);   /* X6BUS to R5 relay */
    vth_pclose_rly(mod_id, PIN_SLOT, K12F);   /* R5 to mint pin relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KDC5);   /* X5 to X5BUS relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KX5);   /* X5BUS to R4 relay */
    vth_pclose_rly(mod_id, PIN_SLOT, K13E);   /* R4 to mint pin relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KDCG);   /* XG to XGL relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KGLE);   /* XGL to DR4 relay */
    vth_pclose_rly(mod_id, PIN_SLOT, K5E);   /* DR4 to mint pin relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KDC4);   /* X4 to X4BUS relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KX4);   /* X4BUS to R3 relay */
    vth_pclose_rly(mod_id, PIN_SLOT, K16D);   /* R3 to mint pin relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KDC3);   /* X3 to X3BUS relay */
    vth_pclose_rly(mod_id, PIN_SLOT, KX3);   /* X3BUS to R2 relay */
    vth_pclose_rly(mod_id, PIN_SLOT, K15C);   /* R2 to mint pin relay */

    for (int band = NB; band <= WB; ++band)
    {  /* for band */
      if (band == NB)
        vth_amoa_sel_nb(mod_id);
      else
        vth_amoa_sel_wb(mod_id);

      /* SETUP FOR THE DC CMRR PART */
      setup_dmm(mod_id, DCV, VDC_SET[0] * FUDGE_FACTOR, RESOLUTION);
      for (voltage = 0; voltage <= 1; voltage++)
      {  /* for dc voltage */
        vth_asrc_set_dc_ampl(mod_id, VDC_SET[voltage]);
        vth_afc_en_src_out(mod_id);
        vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

        /* SETUP TO MEASURE VS */
        aconnect_vs_dmm(mod_id, true);
        measure_dmm(mod_id);
        if (get_result_nolog(mod_id, &result))
          vs[voltage] = result.UU.Real_result;
        else
          display_prompt("No result in result log (Expected DMM v)", true);
        aconnect_vs_dmm(mod_id, false);

        /* SETUP TO MEASURE V0 */
        aconnect_vo_dmm(mod_id, true);
        measure_dmm(mod_id);
        if (get_result_nolog(mod_id, &result))
          vo[voltage] = result.UU.Real_result;
        else
          display_prompt("No result in result log (Expected DMM v)", true);
        aconnect_vo_dmm(mod_id, false);

      }  /* for dc voltage */



      /* CALCULATE CMRR */
      if ( !number_equal(vo[0], vo[1]))
        cmrr = 20 * log10(R_RATIO * fabs((vs[0] - vs[1]) / (vo[0] - vo[1])));
      else
        cmrr = 0.0;

      /* LOG THE RESULT */
      result.UU.Real_result = cmrr;
      log_result(mod_id, result);

      /* SETUP FOR AC CMRR */
      peak_volts = sqrt(2.0) * RMS_VOLTS;
      setup_dmm(mod_id, ACV, peak_volts * FUDGE_FACTOR, RESOLUTION);

      for (this_frequency = 0; this_frequency < NUM_FREQS_TO_TEST; this_frequency++)
      {  /* for this_frequency */
        vth_asrc_sel_fltr(mod_id, FILTER_______[this_frequency]);
        vth_ascat_cpu_init(mod_id);
        vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
        vth_ascat_set_a_freq(mod_id, FREQ_A_________[this_frequency]);
        vth_ascat_en_cont_src(mod_id);
        vth_ascat_en_frun_src(mod_id);
        vth_ascat_run_en(mod_id);
        vth_asrc_dis_30ma_ilim(mod_id);
        vth_asrc_set_ampl(mod_id, RMS_VOLTS, DC_OFFSET);
        vth_afc_en_src_out(mod_id);
        vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

        /* SETUP TO MEASURE VS */
        aconnect_vs_dmm(mod_id, true);
        measure_dmm(mod_id);
        if (get_result_nolog(mod_id, &result))
          vs[0] = result.UU.Real_result;
        else
          display_prompt("No result in result log (Expected DMM v)", true);
        aconnect_vs_dmm(mod_id, false);

        /* SETUP TO MEASURE V0 */
        aconnect_vo_dmm(mod_id, true);
        measure_dmm(mod_id);
        if (get_result_nolog(mod_id, &result))
          vo[0] = result.UU.Real_result;
        else
          display_prompt("No result in result log (Expected DMM v)", true);
        aconnect_vo_dmm(mod_id, false);

        /* CALCULATE CMRR */
        if (vo[0] != 0)
          cmrr = 20 * log10(R_RATIO * fabs(vs[0] / vo[0]));
        else
          cmrr = 0.0;

        /* LOG THE RESULT */
        result.UU.Real_result = cmrr;
        log_result(mod_id, result);

      }  /* for this_frequency */


    }  /* for band */



    close_dmm(mod_id);
    close_mux(mod_id);

  }  /* After check for slot 2 pin card */



}  /* t9440 */


#undef NUM_FREQS_TO_TEST
#undef NB
#undef WB
#undef PIN_SLOT
#undef F128
#undef F1024
#undef F8192
#undef R_RATIO
#undef FUDGE_FACTOR
#undef RESOLUTION
#undef SOURCE_WAIT_TIME
#undef RMS_VOLTS
#undef DC_OFFSET


/* Revision Control */



#define CAP_AUX_RELAY_NUMBER  1
#define AVERAGES        10
#define DIGITAL         0

#define POWER_SUPPLY_RAMP_TIME  1.0e+00
#define RESOLUTION      1.0e-03
#define DETECTOR_VOLTS  90.0e+00
#define DMM_VOLT_RANGE  160.0e+00
#define TIMEOUT         0.0e+00

#define DMM_MEASUREMENT_CHANNEL  'A'

#define DMM_FUNCTION    DCV


void t9450(int mod_id, int /*slot*/)
{
  /*DC DETECTOR ATTENUATOR ACCURACY AND RANGE*/
  int pin_slot;
  int integrate_length;

  test_result_type result;
  char buffer[81];
int channel;


  if (get_result_nolog(mod_id, &result))
    display_prompt("Unexpected result encountered", true);
  else
  {  /*display*/
    display_prompt("\007", true);   /* '#7' should cause a beep */
    display_prompt("WARNING! WARNING! WARNING! WARNING! WARNING!", true);
    display_prompt("This test puts 160V on many areas of the system.", true);
    display_prompt("Do not attempt physical contact with the system", true);
    display_prompt("until the test is complete.  Make sure the area", true);
    display_prompt("is free from non-essential personnel before", true);
    display_prompt("continuing this test.  Be careful.", true);
    display_prompt("WARNING! WARNING! WARNING! WARNING! WARNING!", true);
    display_prompt(" ", true);
    get_user_answer(buffer, 80);
  }  /*display*/

  open_dmm(mod_id);
  open_mux(mod_id);

  /* SETUP THE DMM TO MEASURE THE ACTUAL OUTPUT FROM THE SUPPLY */
  connect_dmm(mod_id);
  connect_plus_xn(mod_id, DMM_MEASUREMENT_CHANNEL, true);
  connect_minus_swd_gnd(mod_id);
  setup_dmm(mod_id, DMM_FUNCTION, DMM_VOLT_RANGE, RESOLUTION);

  /* CLOSE THE RELAYS THAT ARE 'GLOBAL' TO THIS TEST */
  vth_aclose_rly(mod_id, K759);   /* LOUT to L mint pin connect relay */
  vth_aclose_rly(mod_id, K820);   /* L to DETLO connect relay */

  vth_aclose_rly(mod_id, K760);   /* GOUT to G mint pin connect relay */
  vth_aclose_rly(mod_id, K816);   /* G to I connect relay */
  vth_aclose_rly(mod_id, K812);   /* I to DETHI connect relay */

  vth_aclose_rly(mod_id, K741);   /* X1 mint pin connect relay (to DMM) */
  vth_aclose_rly(mod_id, K761);   /* SWDGND mint pin connect relay (to DMM) */

  /* CLOSE ADDITIONAL RELAYS REQUIRED TO GET VOLTAGE TO DMM AND DETECTOR */
  vth_aclose_rly(mod_id, K717);   /* L TO X1 RELAY */
  vth_aclose_rly(mod_id, K2309);   /* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */
  vth_aclose_rly(mod_id, K2315);
      /* DETLO DIVIDED SIGNAL TO RANGING AND DETECTOR */
  vth_aclose_rly(mod_id, K2306);   /* DETHI TO GROUND SIGNAL PATH */
  vth_aclose_rly(mod_id, K2304);   /* DETHI TO GROUND SIGNAL PATH */
  vth_aopen_rly(mod_id, K2303);   /* DETLO TO GROUND SIGNAL PATH */

  /* ENABLE THE 160V SUPPLY AC POWER USING CAP CARD RELAY */
  vth_cclose_aux_rly(CAP_AUX_RELAY_NUMBER);
  vth_mtmr_wait(mod_id, POWER_SUPPLY_RAMP_TIME);
  measure_dmm(mod_id);
  if (!get_result_log(mod_id, &result))
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  for (pin_slot = 2; pin_slot <= 11; pin_slot++)
  {
    if (pin_slot != 6)
    {  /* ATTEMPT TO CLOSE RELAYS ON EACH PIN CARD */
      for (channel = 'A'; channel <= 'H'; channel++)
      {
        vth_pclose_rly(mod_id, pin_slot, PIN_KDC_RLYS[channel - 'A']);
        vth_pclose_rly(mod_id, pin_slot, PIN_KX_RLYS[channel - 'A']);
        vth_pclose_rly(mod_id, pin_slot, PIN_KGL_RLYS[channel - 'A']);
      }
    }  /* ATTEMPT TO CLOSE RELAYS ON EACH PIN CARD */
  }

  for (integrate_length = 0; integrate_length <= 1; integrate_length++)
  {  /*for integrate length*/
    /* CAN USE THE DCV_SETUP ROUTINE BECAUSE THE VOLTAGE IS ACROSS I AND L */
    vth_dcv_setup(mod_id, integrate_length, AVERAGES, DIGITAL, DETECTOR_VOLTS,
              TIMEOUT);

    /* CLOSE THE APPROPRIATE PATH RELAYS TO MAKE THE DETHI GROUNDED MEASUREMENT */
    /* 'SOME' OF THESE RELAYS MUST BE CLOSED AFTER THE DCV_SETUP ROUTINE */
    for (channel = 'A'; channel <= 'H'; channel++)
      vth_aclose_rly(mod_id, AIM_X_L_RLYS[channel - 'A']);
    vth_aclose_rly(mod_id, K2309);   /* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */
    vth_aclose_rly(mod_id, K2315);
        /* DETLO DIVIDED SIGNAL TO RANGING AND DETECTOR */
    vth_aclose_rly(mod_id, K2306);   /* DETHI TO GROUND SIGNAL PATH */
    vth_aclose_rly(mod_id, K2304);   /* DETHI TO GROUND SIGNAL PATH */
    vth_aclose_rly(mod_id, K752);   /* G TO XG GROUND REFERENCE FOR PIN CARDS */
    for (pin_slot = 2; pin_slot <= 11; pin_slot++)
    {   /* XG GND REFERENCE FOR PIN CARDS */
      if (pin_slot != 6)
        vth_pclose_rly(mod_id, pin_slot, KDCG);
    }
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

    /* GET_AFUNC_RESULT RETURNS TWO VALUES, THE VOLTAGE AND THE STATUS */
    vth_afunc_meas(mod_id);
    vth_get_afunc_result(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected DET voltage)", true);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected DET STATUS)", true);

    /* OPEN THE RELAYS USED FOR THE DETHI GROUNDED MEASUREMENT */
    for (channel = 'A'; channel <= 'H'; channel++)
      vth_aopen_rly(mod_id, AIM_X_L_RLYS[channel - 'A']);
    vth_aopen_rly(mod_id, K2309);   /* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */
    vth_aopen_rly(mod_id, K2315);
        /* DETLO DIVIDED SIGNAL TO RANGING AND DETECTOR */
    vth_aopen_rly(mod_id, K2306);   /* DETHI TO GROUND SIGNAL PATH */
    vth_aopen_rly(mod_id, K2304);   /* DETHI TO GROUND SIGNAL PATH */
    vth_aopen_rly(mod_id, K752);   /* G TO XG GROUND REFERENCE FOR PIN CARDS */
    for (pin_slot = 2; pin_slot <= 11; pin_slot++)
    {   /* XG GND REFERENCE FOR PIN CARDS */
      if (pin_slot != 6)
        vth_popen_rly(mod_id, pin_slot, KDCG);
    }
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

    /* VERSION 8.2 REQUIRES THAT THE SETUP BE REPEATED BEFORE ANOTHER MEAS */
    vth_dcv_setup(mod_id, integrate_length, AVERAGES, DIGITAL, DETECTOR_VOLTS,
              TIMEOUT);

    /* CLOSE THE APPROPRIATE PATH RELAYS TO MAKE THE DETLO GROUNDED MEASUREMENT */
    /* 'SOME' OF THESE RELAYS MUST BE CLOSED AFTER THE DCV_SETUP ROUTINE */
    for (channel = 'A'; channel <= 'H'; channel++)
      vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
    vth_aclose_rly(mod_id, K2310);   /* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */
    vth_aclose_rly(mod_id, K2316);
        /* DETHI DIVIDED SIGNAL TO RANGING AND DETECTOR */
    vth_aclose_rly(mod_id, K2305);   /* DETLO TO GROUND SIGNAL PATH */
    vth_aclose_rly(mod_id, K2303);   /* DETLO TO GROUND SIGNAL PATH */
    vth_aclose_rly(mod_id, K753);   /* L TO XL GROUND REFERENCE FOR PIN CARDS */
    vth_aclose_rly(mod_id, K754);   /* L TO XL GROUND REFERENCE FOR PIN CARDS */
    for (pin_slot = 2; pin_slot <= 11; pin_slot++)
    {   /* XL GND REFERENCE FOR PIN CARDS */
      if (pin_slot != 6)
        vth_pclose_rly(mod_id, pin_slot, KDCL);
    }
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

    /* GET_AFUNC_RESULT RETURNS TWO VALUES, THE VOLTAGE AND THE STATUS */
    vth_afunc_meas(mod_id);
    vth_get_afunc_result(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected DET voltage)", true);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected DET STATUS)", true);

    /* OPEN THE APPROPRIATE PATH RELAYS TO MAKE THE DETLO GROUNDED MEASUREMENT */
    for (channel = 'A'; channel <= 'H'; channel++)
      vth_aopen_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);
    vth_aopen_rly(mod_id, K2310);   /* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */
    vth_aopen_rly(mod_id, K2316);
        /* DETHI DIVIDED SIGNAL TO RANGING AND DETECTOR */
    vth_aopen_rly(mod_id, K2305);   /* DETLO TO GROUND SIGNAL PATH */
    vth_aopen_rly(mod_id, K2303);   /* DETLO TO GROUND SIGNAL PATH */
    vth_aopen_rly(mod_id, K753);   /* L TO XL GROUND REFERENCE FOR PIN CARDS */
    vth_aopen_rly(mod_id, K754);   /* L TO XL GROUND REFERENCE FOR PIN CARDS */
    for (pin_slot = 2; pin_slot <= 11; pin_slot++)
    {   /* XL GND REFERENCE FOR PIN CARDS */
      if (pin_slot != 6)
        vth_popen_rly(mod_id, pin_slot, KDCL);
    }
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

    /* VERSION 8.2 REQUIRES THAT THE SETUP BE REPEATED BEFORE ANOTHER MEAS */
    vth_dcv_setup(mod_id, integrate_length, AVERAGES, DIGITAL, DETECTOR_VOLTS,
              TIMEOUT);

    /* CLOSE THE APPROPRIATE PATH RELAYS TO MAKE THE DIFFERENTIAL MEASUREMENT */
    /* 'SOME' OF THESE RELAYS MUST BE CLOSED AFTER THE DCV_SETUP ROUTINE */
    vth_aclose_rly(mod_id, K2310);   /* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */
    vth_aclose_rly(mod_id, K2316);
        /* DETHI DIVIDED SIGNAL TO RANGING AND DETECTOR */
    vth_aclose_rly(mod_id, K2309);   /* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */
    vth_aclose_rly(mod_id, K2315);
        /* DETLO DIVIDED SIGNAL TO RANGING AND DETECTOR */
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

    /* GET_AFUNC_RESULT RETURNS TWO VALUES, THE VOLTAGE AND THE STATUS */
    vth_afunc_meas(mod_id);
    vth_get_afunc_result(mod_id);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected DET voltage)", true);
    if (!get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected DET STATUS)", true);

    /* OPEN THE APPROPRIATE PATH RELAYS TO MAKE THE DIFFERENTIAL MEASUREMENT */
    vth_aopen_rly(mod_id, K2310);   /* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */
    vth_aopen_rly(mod_id, K2316);
        /* DETHI DIVIDED SIGNAL TO RANGING AND DETECTOR */
    vth_aopen_rly(mod_id, K2309);   /* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */
    vth_aopen_rly(mod_id, K2315);
        /* DETLO DIVIDED SIGNAL TO RANGING AND DETECTOR */
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  }  /*for integrate length*/
  /* READ THE VOLTAGE WITH THE DETECTOR */


  /* DISABLE THE 160V SUPPLY AC POWER USING CAP CARD RELAY */
  vth_copen_aux_rly(CAP_AUX_RELAY_NUMBER);

  if (get_result_nolog(mod_id, &result))
    display_prompt("Unexpected result encountered", true);
  else
  {  /*display*/
    display_prompt("\007", true);   /* '#7' should cause a beep */
    display_prompt("WARNING! WARNING! WARNING! WARNING! WARNING!", true);
    display_prompt("Make sure that this the 160V power supply has   ", true);
    display_prompt("been disabled.  The AC power indicator light   ", true);
    display_prompt("should be off.                                 ", true);
    display_prompt("WARNING! WARNING! WARNING! WARNING! WARNING!", true);
    display_prompt(" ", true);
    get_user_answer(buffer, 80);
  }  /*display*/

  vth_mod_init(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /*t9450*/


#undef CAP_AUX_RELAY_NUMBER
#undef AVERAGES
#undef DIGITAL
#undef POWER_SUPPLY_RAMP_TIME
#undef RESOLUTION
#undef DETECTOR_VOLTS
#undef DMM_VOLT_RANGE
#undef TIMEOUT
#undef DMM_MEASUREMENT_CHANNEL
#undef DMM_FUNCTION


/* Revision Control */


//////////////////////nasru SN_check, checking for NASRU P10/////////////////////////////////////////////

int t9430(int mod_id)
{


  int nasru_p10_id=0;//default set to none NASRU P10
  char nasru_p10_part_no[10] = { 'N', '1', '8', '0', '7', '6', '6', '8', '1', '0'};	
  static int const SN_SIZE = 20;
  static int const REV_SIZE = 2;
  char serial_number [SN_SIZE + 1] = "";
  char rev_code [REV_SIZE + 1] = "";
  s80 buffer = "";
  test_result_type result;

  for (int i = 0; i <= 3; ++i)
    display_prompt("", true);

  //display_prompt("TEST READ ASRU CARD ID, SERIAL #, AND REV CODE.", true);
  //display_prompt("", true);
  //display_prompt("  This test returns the card id, serial number, and revision", true);
  //sprintf(buffer, "    code for the asru card in MODULE %d.", mod_id);
  //display_prompt(buffer, true);
  //display_prompt("", true);
  vth_ard_cal_rom(mod_id, EEPROM_STRT);

  for (i = 1; i <= SN_SIZE; ++i)
    vth_ard_cal_rom(mod_id, EEPROM_STRT + SERIAL_OFST + i);

  for (i = 0; i < REV_SIZE; ++i)
    vth_ard_cal_rom(mod_id, EEPROM_STRT + REV_OFST + i);

  *buffer = '\0';

  //if (get_result_nolog(mod_id, &result))
  //  sprintf(buffer, "  The card id is %2d.", result.UU.Integer_result);
  //else
  //  sprintf(buffer, "  Unable to read card id.");

  get_result_nolog(mod_id, &result);

  display_prompt(buffer, true);

  for (int c = 0; c < SN_SIZE; ++c)
    if (get_result_nolog(mod_id, &result))
      serial_number[c] = result.UU.Integer_result;
    else
      serial_number[c] = '\0';

  serial_number[c] = '\0';
  *buffer = '\0';

  if (mtd_strlen(serial_number) == SN_SIZE)
    sprintf(buffer, "Slot 1 - NASRU serial number : %*s.", SN_SIZE, serial_number);
  else
    sprintf(buffer, "  Unable to read serial number.");

  display_prompt(buffer, true);

  for (c = 0; c < REV_SIZE; ++c)
    if (get_result_nolog(mod_id, &result))
      rev_code[c] = result.UU.Integer_result;
    else
      rev_code[c] = '\0';

  rev_code[c] = '\0';
  *buffer = '\0';

 // if (mtd_strlen(rev_code) == REV_SIZE)
 //   sprintf(buffer, "  The card revision code is %*s.", REV_SIZE, rev_code);
 // else
 //   sprintf(buffer, "  Unable to read revision code.");

  display_prompt(buffer, true);

  nasru_p10_id=1;//force into loop

  for (int counter = 0; counter < 10 && nasru_p10_id==1; counter++)
    if ((nasru_p10_part_no[counter]==serial_number[counter]))
      nasru_p10_id=1;
    else
      nasru_p10_id=0;

	return nasru_p10_id;

}


////////////////////////////end of NASRU SN# red out test///////////////////






////////////////////////////////added new///////////////////////99V test////////////////
#define NUM_POINTS_TO_CHECK  1

const voltage_array TEST_VOLTAGE_99V = {
99.0e+00, 0e+00, 0e+00, 0e+00};//NASRU 99V test improvement
/* Sets the det to range 8 */

#define RANGE_VOLTAGE   160e+00


void t9431(int mod_id, int /*slot*/)
{
	
  /*DC DETECTOR ATTENUATOR ACCURACY OVER RANGE (to 60v) */
///////////////////////////////

s80 buffer = "";  
nasru_p10_id=t9430(mod_id);	

///////////////////////////////

if(nasru_p10_id==1)
{
		
	    sprintf(buffer, " NASRU P10 detected : 99V Detector Att Test - T9431 will run!");
		display_prompt(buffer, true);
		test_det_atten_range(mod_id, RANGE_VOLTAGE, NUM_POINTS_TO_CHECK, TEST_VOLTAGE_99V);
}

else if(nasru_p10_id!=1)
{
		sprintf(buffer, "NASRU P10 not detected!");
		display_prompt(buffer, true);
		
}

/////////////////////////////////
}  /*t9431*/


#undef NUM_POINTS_TO_CHECK
#undef RANGE_VOLTAGE

///////////////////////////////////////////////////////////////////




#define NUM_POINTS_TO_CHECK  1

const voltage_array TEST_VOLTAGE = {
60.0e+00, 0e+00, 0e+00, 0e+00};//NASRU 99V test improvement
/* Sets the det to range 8 */

#define RANGE_VOLTAGE   160e+00


void t9451(int mod_id, int /*slot*/)
{
  /*DC DETECTOR ATTENUATOR ACCURACY OVER RANGE (to 60v) */
  test_det_atten_range(mod_id, RANGE_VOLTAGE, NUM_POINTS_TO_CHECK, TEST_VOLTAGE);

}  /*t9451*/


#undef NUM_POINTS_TO_CHECK
#undef RANGE_VOLTAGE


/* Revision Control */



#define NUM_POINTS_TO_CHECK  2

const voltage_array TEST_VOLTAGE_ = {
  60.0e+00, 40.0e+00, 0e+00, 0e+00//experimental purpose
  //  10.0e+00, 10e+00, 0e+00, 0e+00
};

/* Sets the det to range 7 */

#define RANGE_VOLTAGE   80e+00


void t9452(int mod_id, int /*slot*/)
{
  /*DC DETECTOR ATTENUATOR ACCURACY OVER RANGE (to 60v) */
  test_det_atten_range(mod_id, RANGE_VOLTAGE, NUM_POINTS_TO_CHECK,
                       
TEST_VOLTAGE_);
}  /*t9452*/


#undef NUM_POINTS_TO_CHECK
#undef RANGE_VOLTAGE


/* Revision Control */



#define NUM_POINTS_TO_CHECK  3

const voltage_array TEST_VOLTAGE__ = {
  40.0e+00, 20.0e+00, 10.0e+00, 0e+00//experimental purpose
//	10.0e+00, 10e+00, 10e+00, 0e+00
};

/* Sets the det to range 6 */

#define RANGE_VOLTAGE   40e+00


void t9453(int mod_id, int /*slot*/)
{
  /*DC DETECTOR ATTENUATOR ACCURACY OVER RANGE (to 40v) */
  test_det_atten_range(mod_id, RANGE_VOLTAGE, NUM_POINTS_TO_CHECK,
                       TEST_VOLTAGE__);

}  /*t9453*/


#undef NUM_POINTS_TO_CHECK
#undef RANGE_VOLTAGE


/* Revision Control */



#define NUM_POINTS_TO_CHECK  2

const voltage_array TEST_VOLTAGE___ = {
  20.0e+00, 10.0e+00, 0e+00, 0e+00//experimental purpose
//	00.0e+00, 0e+00, 0e+00, 0e+00
};

/* Sets the det to range 5 */

#define RANGE_VOLTAGE   20e+00


void t9454(int mod_id, int /*slot*/)
{
  /*DC DETECTOR ATTENUATOR ACCURACY OVER RANGE (to 20v) */
  test_det_atten_range(mod_id, RANGE_VOLTAGE, NUM_POINTS_TO_CHECK,
                       TEST_VOLTAGE___);

}  /*t9454*/


#undef NUM_POINTS_TO_CHECK
#undef RANGE_VOLTAGE


/* Revision Control */

////////////////////////////////////////////DC Detector test starting/////////////////////////////////////////////////

#define RANGE_NUMBER    8

#define RANGE_VOLTAGE   10.0e+00

const voltage_array TEST_____ = {
  10.5e00, 7.5e00, 5.0e00, 2.5e00
};


void t9455(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST_____);

}  /* t9455 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    7

#define RANGE_VOLTAGE   5.0e+00

const voltage_array TEST______ = {
  5.25e00, 3.75e00, 2.5e00, 1.25e00
};


void t9456(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST______);

}  /* t9456 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    6

#define RANGE_VOLTAGE   2.5e+00

const voltage_array TEST_______ = {
  2.625e00, 1.875e00, 1.25e00, 0.625e00
};


void t9457(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST_______);

}  /* t9457 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    5

#define RANGE_VOLTAGE   1.25e+00

const voltage_array TEST________ = {
  1.3125e00, 0.9375e00, 0.625e00, 0.3125e00
};


void t9458(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST________);

}  /* t9458 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    4

#define RANGE_VOLTAGE   0.625e+00

const voltage_array TEST_________ = {
  0.6563e00, 0.4688e00, 0.3125e00, 0.15625e00
};


void t9459(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST_________);

}  /* t9459 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    3

#define RANGE_VOLTAGE   0.3125e+00

const voltage_array TEST__________ = {
  0.3281e00, 0.2344e00, 0.15625e00, 78.125e-03
};


void t9460(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST__________);

}  /* t9460 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    2

#define RANGE_VOLTAGE   0.15625e+00

const voltage_array TEST___________ = {
  0.1641e00, 0.1172e00, 78.125e-03, 39.06e-03
};


void t9461(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST___________);

}  /* t9461 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    1

#define RANGE_VOLTAGE   78.125e-03

const voltage_array TEST____________ = {
  82.03e-03, 58.59e-03, 39.06e-03, 19.53e-03
};


void t9462(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST____________);

}  /* t9462 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    0

#define RANGE_VOLTAGE   39.0625e-03

const voltage_array TEST_____________ = {
  41.02e-03, 29.30e-03, 19.53e-03, 9.77e-03
};


void t9463(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST_____________);

}  /* t9463 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */

////////////////////////////////////////////DC Detector test ending/////////////////////////////////////////////////





/////////////////////////////////////////////DC Detector test start for NASRU with loosen test limit///////////////

#define RANGE_NUMBER    8

#define RANGE_VOLTAGE   10.0e+00

const voltage_array TEST_____NASRU = {
  10.5e00, 7.5e00, 5.0e00, 2.5e00
};


void t9441(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST_____NASRU);

}  /* t9441 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    7

#define RANGE_VOLTAGE   5.0e+00

const voltage_array TEST______NASRU = {
  5.25e00, 3.75e00, 2.5e00, 1.25e00
};


void t9442(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST______NASRU);

}  /* t9442 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    6

#define RANGE_VOLTAGE   2.5e+00

const voltage_array TEST_______NASRU = {
  2.625e00, 1.875e00, 1.25e00, 0.625e00
};


void t9443(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST_______NASRU);

}  /* t9443 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    5

#define RANGE_VOLTAGE   1.25e+00

const voltage_array TEST________NASRU = {
  1.3125e00, 0.9375e00, 0.625e00, 0.3125e00
};


void t9444(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST________NASRU);

}  /* t9444 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    4

#define RANGE_VOLTAGE   0.625e+00

const voltage_array TEST_________NASRU = {
  0.6563e00, 0.4688e00, 0.3125e00, 0.15625e00
};


void t9445(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST_________NASRU);

}  /* t9445 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    3

#define RANGE_VOLTAGE   0.3125e+00

const voltage_array TEST__________NASRU = {
  0.3281e00, 0.2344e00, 0.15625e00, 78.125e-03
};


void t9446(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST__________NASRU);

}  /* t9446 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    2

#define RANGE_VOLTAGE   0.15625e+00

const voltage_array TEST___________NASRU = {
  0.1641e00, 0.1172e00, 78.125e-03, 39.06e-03
};


void t9447(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST___________NASRU);

}  /* t9447 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    1

#define RANGE_VOLTAGE   78.125e-03

const voltage_array TEST____________NASRU = {
  82.03e-03, 58.59e-03, 39.06e-03, 19.53e-03
};


void t9448(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST____________NASRU);

}  /* t9448 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */



#define RANGE_NUMBER    0

#define RANGE_VOLTAGE   39.0625e-03

const voltage_array TEST_____________NASRU = {
  41.02e-03, 29.30e-03, 19.53e-03, 9.77e-03
};


void t9449(int mod_id, int /*slot*/)
{
  /* DC DETECTOR VOLTAGE ACCURACY */
  test_detector_range(mod_id, RANGE_VOLTAGE, RANGE_NUMBER, TEST_____________NASRU);

}  /* t9449 */


#undef RANGE_NUMBER
#undef RANGE_VOLTAGE


/* Revision Control */

/////////////////////////////////////////////DC Detector test ending for NASRU with loosen test limit///////////////








#define NUM_POINTS_TO_CHECK  6


typedef int freq_array__[NUM_POINTS_TO_CHECK];

#define FAST            0
#define NORMAL_         1

typedef freq_array__ test_array[2];


/* FOR THE DETECTOR */

#define AVERAGES        10
/* NOT IMPLEMENTED (FOR M.A.D.) */
#define DIGITAL         0

#define FUDGE_FACTOR    1.2e+00
#define LOW_BAND_RATIO  0.8e+00
#define HI_BAND_RATIO   1.2e+00
#define TIMEOUT         10.0e+00
#define PEAK_AMPLITUDE  10.0e+00
#define OFFSET          0.0e+00
#define RESOLUTION      1e-03
#define WAIT_TIME       60e-3

/* Fast Mode */
/* Normal Mode */

const test_array TEST_FREQ__ = {
  { 50, 200, 400, 5000, 10000, 19000 },
  { 10, 50, 300, 3000, 10000, 19000 }
};


void t9465(int mod_id, int /*slot*/)
{
  /* AC DETECTOR (TRUE RMS) VOLTAGE ACCURACY */
  int start_frequency;
  int stop_frequency;

  double rms_amplitude;
  double peak_to_peak_amplitude;
  double dmm_actual_voltage;
  double detector_voltage;
  double v_delta;

  test_result_type result;
  break_test_for_function_step_compatability(mod_id);

  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  open_33250A(mod_id);
  /*New 33250A codes*/
  //added by chuanho, 14jun2012
  if(check_syn_model())
  {
  setup_33250A_defaults(mod_id);
  setup_33250A_hv(mod_id, true);

  /*Connect the synthesizer output to the X1 and X2 and the CALIB lines to dmm*/
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_syn_out_xmint(mod_id);

  /* CLOSE THE XMINT TO DET PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K725);   /* X1 bus to I bus relay */
  vth_aclose_rly(mod_id, K718);   /* X2 bus to L bus relay */

  /* CLOSE THE I to CALIB2 and L to CALIB1 relays */
  vth_aclose_rly(mod_id, K855);   /* I to CALIB2 bus relay */
  vth_aclose_rly(mod_id, K759);   /* L to LOUT/CALIB1 relay */

  rms_amplitude = PEAK_AMPLITUDE / sqrt(2.0);
  peak_to_peak_amplitude = PEAK_AMPLITUDE * 2;
  setup_dmm(mod_id, ACV, rms_amplitude * FUDGE_FACTOR, RESOLUTION);
  for (int mode = FAST; mode <= NORMAL_; ++mode)
  {  /* for mode */
    for (int frequency = 0; frequency < NUM_POINTS_TO_CHECK; ++frequency)
    {  /*for frequency*/
      setup_33250A(mod_id, SINE, TEST_FREQ__[mode]
                [frequency], peak_to_peak_amplitude, OFFSET);
	  enable_33250A_out(mod_id); 
      vth_acv_setup(mod_id, rms_amplitude, mode, AVERAGES, DIGITAL, TIMEOUT);
      vth_aclose_rly(mod_id, K857);   /* close L to ACOM */
      vth_mtmr_wait(mod_id, WAIT_TIME);   /* wait for synthesizer */
      start_frequency = static_cast<int>(TEST_FREQ__[mode][frequency] * LOW_BAND_RATIO);
      stop_frequency = static_cast<int>(TEST_FREQ__[mode][frequency] * HI_BAND_RATIO);
      setup_dmm_ac_band(mod_id, start_frequency, stop_frequency);
      measure_dmm(mod_id);
      if (get_result_nolog(mod_id, &result))
	  {
        dmm_actual_voltage = result.UU.Real_result;
		//display_it(mod_id, "DMM MEA: ", 3, dmm_actual_voltage);
	  }
      else
        display_prompt("No result in result log (Expected dmm vth_reading)", true);

      vth_afunc_meas(mod_id);
      vth_get_afunc_result(mod_id);

      if (get_result_nolog(mod_id, &result))
      {  /* process result */
        detector_voltage = result.UU.Real_result;
		//display_it(mod_id, "ASR MEA: ", 3, detector_voltage);
        v_delta = detector_voltage - dmm_actual_voltage;
        result.UU.Real_result = v_delta;
        log_result(mod_id, result);
      }  /* process result */
      else
        display_prompt("No result in result log (Expected DET volts)", true);

      if (!get_result_nolog(mod_id, &result))
        display_prompt("No result in result log (Expected DET stat)", true);

    }  /*for frequency*/

  }  /* for mode */


  vth_ainit(mod_id);
  setup_33250A_hv(mod_id, false);
  close_33250A(mod_id);
  }

  else
  {
  open_syn(mod_id);
  setup_syn_hv(mod_id, true);

  /*Connect the synthesizer output to the X1 and X2 and the CALIB lines to dmm*/
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_syn_out_xmint(mod_id);

  /* CLOSE THE XMINT TO DET PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K725);   /* X1 bus to I bus relay */
  vth_aclose_rly(mod_id, K718);   /* X2 bus to L bus relay */

  /* CLOSE THE I to CALIB2 and L to CALIB1 relays */
  vth_aclose_rly(mod_id, K855);   /* I to CALIB2 bus relay */
  vth_aclose_rly(mod_id, K759);   /* L to LOUT/CALIB1 relay */

  rms_amplitude = PEAK_AMPLITUDE / sqrt(2.0);
  peak_to_peak_amplitude = PEAK_AMPLITUDE * 2;
  setup_dmm(mod_id, ACV, rms_amplitude * FUDGE_FACTOR, RESOLUTION);
  for (int mode = FAST; mode <= NORMAL_; ++mode)
  {  /* for mode */
    for (int frequency = 0; frequency < NUM_POINTS_TO_CHECK; ++frequency)
    {  /*for frequency*/
      setup_syn(mod_id, SINE, TEST_FREQ__[mode]
                [frequency], peak_to_peak_amplitude, OFFSET);
      vth_acv_setup(mod_id, rms_amplitude, mode, AVERAGES, DIGITAL, TIMEOUT);
      vth_aclose_rly(mod_id, K857);   /* close L to ACOM */
      vth_mtmr_wait(mod_id, WAIT_TIME);   /* wait for synthesizer */
      start_frequency = static_cast<int>(TEST_FREQ__[mode][frequency] * LOW_BAND_RATIO);
      stop_frequency = static_cast<int>(TEST_FREQ__[mode][frequency] * HI_BAND_RATIO);
      setup_dmm_ac_band(mod_id, start_frequency, stop_frequency);
      measure_dmm(mod_id);
      if (get_result_nolog(mod_id, &result))
	  {
        dmm_actual_voltage = result.UU.Real_result;
		//display_it(mod_id, "DMM MEA: ", 3, dmm_actual_voltage);
	  }
      else
        display_prompt("No result in result log (Expected dmm vth_reading)", true);

      vth_afunc_meas(mod_id);
      vth_get_afunc_result(mod_id);

      if (get_result_nolog(mod_id, &result))
      {  /* process result */
        detector_voltage = result.UU.Real_result;
		//display_it(mod_id, "ASR MEA: ", 3, detector_voltage);
        v_delta = detector_voltage - dmm_actual_voltage;
        result.UU.Real_result = v_delta;
        log_result(mod_id, result);
      }  /* process result */
      else
        display_prompt("No result in result log (Expected DET volts)", true);

      if (!get_result_nolog(mod_id, &result))
        display_prompt("No result in result log (Expected DET stat)", true);

    }  /*for frequency*/

  }  /* for mode */


  vth_ainit(mod_id);
  setup_syn_hv(mod_id, false);
  close_syn(mod_id);
  }

  close_dmm(mod_id);
  CloseResourceManager();

}  /* t9465 */


#undef NUM_POINTS_TO_CHECK
#undef FAST
#undef NORMAL_
#undef AVERAGES
#undef DIGITAL
#undef FUDGE_FACTOR
#undef LOW_BAND_RATIO
#undef HI_BAND_RATIO
#undef TIMEOUT
#undef PEAK_AMPLITUDE
#undef OFFSET
#undef RESOLUTION
#undef WAIT_TIME


/* Revision Control */



/* FOR THE DETECTOR */

#define AVERAGES        10
/* NOT IMPLEMENTED (FOR M.A.D.) */
#define DIGITAL         0
/* Fast mode */
#define MEAS_MODE       0
#define FREQUENCY_      16384

#define FUDGE_FACTOR    1.2e+00
#define LOW_BAND_RATIO  0.8e+00
#define HI_BAND_RATIO   1.2e+00
#define TIMEOUT         10.0e+00
#define MAX_RMS_AMPLITUDE  7.0710678
#define OFFSET          0.0e+00

#define DIGITS          10000


/* No 30mA current compliance    */

#define SRC_I_COMPLIANCE  1
/* No additional ouput resistance*/
#define SRC_R_OUT       0
/* Source enabled */
#define SRC_ON          1
/* Source controlled by processor*/
#define SRC_CONTROL     0


#define FULL            0
#define HALF            1


void t9466(int mod_id, int /*slot*/)
{
  /* AC DETECTOR (TRUE RMS) VOLTAGE ACCURACY OVER VOLTAGE */
  int start_frequency;
  int stop_frequency;
  double rms_voltage;
  double dmm_rms_range;
  double det_rms_range;
  double dmm_actual_voltage;
  double detector_voltage;
  double v_delta;
  double resolution;
  test_result_type result;


  break_test_for_function_step_compatability(mod_id);

  open_dmm(mod_id);
  open_mux(mod_id);

  /*Connect the CALIB lines to dmm*/
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  /*Connect SRCOUT to DETHI and CALIB1*/
  vth_aclose_rly(mod_id, K837);
  vth_aclose_rly(mod_id, K836);
  vth_aclose_rly(mod_id, K853);

  /*Connect CALIB2 to ACOM*/
  vth_aclose_rly(mod_id, K854);

  start_frequency = static_cast<int>(FREQUENCY_ * LOW_BAND_RATIO);
  stop_frequency = static_cast<int>(FREQUENCY_ * HI_BAND_RATIO);
  setup_dmm_ac_band(mod_id, start_frequency, stop_frequency);
  setup_dmm_level(mod_id, 0, DMM_DC);
  setup_dmm_level_filter(mod_id, true);
  hold_dmm(mod_id);

  for (int scale = FULL; scale <= HALF; ++scale)
  {  /* for scale */
    for (int range = 0; range <= 8; ++range)
    {  /*for range*/
      rms_voltage = MAX_RMS_AMPLITUDE / pow(2.0, static_cast<double>(range));
      dmm_rms_range = rms_voltage * FUDGE_FACTOR;
      det_rms_range = rms_voltage;
      if (scale == HALF)
      {
        rms_voltage /= 2;
        dmm_rms_range /= 2;
      }

      /* setup the source */
      vth_src_sine_setup(mod_id, FREQUENCY_, rms_voltage, OFFSET,
                     SRC_I_COMPLIANCE, SRC_R_OUT, SRC_ON, SRC_CONTROL);
      vth_aopen_rly(mod_id, K806);   /* remove SRCOUT from S */

      resolution = dmm_rms_range / DIGITS;
      setup_dmm(mod_id, ACV, dmm_rms_range, resolution);

      /* setup the detector */
      vth_acv_setup(mod_id, det_rms_range, MEAS_MODE, AVERAGES, DIGITAL, TIMEOUT);
      vth_aopen_rly(mod_id, K812);   /* remove DETHI from I */
      vth_aclose_rly(mod_id, K857);   /* L to ACOM */
      vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

      measure_dmm(mod_id);
      if (get_result_nolog(mod_id, &result))
        dmm_actual_voltage = result.UU.Real_result;
      else
        display_prompt("No result in result log (Expected dmm vth_reading)", true);

      vth_afunc_meas(mod_id);
      vth_get_afunc_result(mod_id);

      if (get_result_nolog(mod_id, &result))
      {  /* process result */
        detector_voltage = result.UU.Real_result;
        v_delta = detector_voltage - dmm_actual_voltage;
        if (!get_result_nolog(mod_id, &result))
          display_prompt("No result in result log (Expected DET stat)", true);
        check_uprange(mod_id, &v_delta);
        result.Measurement_class = CLASS_REAL;
        result.UU.Real_result = v_delta;
        log_result(mod_id, result);
      }  /* process result */
      else
        display_prompt("No result in result log (Expected DET volts)", true);

    }  /*for range*/

  }  /* for scale */


  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9466 */


#undef AVERAGES
#undef DIGITAL
#undef MEAS_MODE
#undef FREQUENCY_
#undef FUDGE_FACTOR
#undef LOW_BAND_RATIO
#undef HI_BAND_RATIO
#undef TIMEOUT
#undef MAX_RMS_AMPLITUDE
#undef OFFSET
#undef DIGITS

#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef FULL
#undef HALF


/* Revision Control */



/* 1Mohm feedback */

#define FEEDBACK_RESISTOR_NO  5
/* unenhanced */
#define MEAS_TYPE       0
#define DEFAULT_DET_RESISTANCE  0
#define NPLC            10

#define TEST_VOLTAGE____  10e+00
#define RESOLUTION      1.0e-03
#define RDMM            10e+10
/* seconds */
/*DC = 20KHz filter*/
#define SOURCE_WAIT_TIME  0.5e+00
/* seconds */
#define ENABLE_WAIT_TIME  1.0e-03

#define CHANNEL         'A'

#define DMM_FUNCTION    DCV


void t9470(int mod_id, int /*slot*/)
{
  /* DC DETECTOR INPUT RESISTANCE */
  double r1m;
  double dmm_1m_voltage;
  double dmm_1m_plus_det_voltage;
  double detector_input_resistance;

  test_result_type result;


  open_dmm(mod_id);
  open_mux(mod_id);

  r1m = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);

  setup_dmm(mod_id, DMM_FUNCTION, TEST_VOLTAGE____, RESOLUTION);
  setup_dmm_nplc(mod_id, NPLC);

  connect_dmm(mod_id);
  connect_plus_xn(mod_id, CHANNEL, true);
  connect_minus_calib2(mod_id);

  vth_aclose_rly(mod_id, K826);   /* SRCOUT to REFR2 relay */
  vth_aclose_rly(mod_id, K850);   /* REFR2 to I bus relay (through 1Mohm)*/
  vth_aclose_rly(mod_id, K725);   /* I bus to X1 bus relay */
  vth_aclose_rly(mod_id, K741);   /* X1 bus to X1MINT relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to GROUND relay */

  vth_afc_en_src_out(mod_id);
  vth_asrc_set_dc_ampl(mod_id, TEST_VOLTAGE____);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
    dmm_1m_voltage = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);
  vth_afc_dis_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  vth_aclose_rly(mod_id, K812);   /*DETHI connect relay*/
  vth_aopen_rly(mod_id, K2304);   /*DETHI input ground relay*/
  vth_aclose_rly(mod_id, K2306);   /*DETHI input relay*/
  vth_aclose_rly(mod_id, K2303);   /*DETLO input ground relay*/

  vth_afc_en_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*measure the loaded voltage, calculate resistance and log*/
    dmm_1m_plus_det_voltage = result.UU.Real_result;
    if (dmm_1m_plus_det_voltage != 0)
      detector_input_resistance = dmm_1m_plus_det_voltage * RDMM * r1m /
          (dmm_1m_voltage * (RDMM + r1m) - dmm_1m_plus_det_voltage * RDMM -
           dmm_1m_plus_det_voltage * r1m);
    else
      detector_input_resistance = DEFAULT_DET_RESISTANCE;
    result.UU.Real_result = fabs(detector_input_resistance);
    log_result(mod_id, result);
  }  /*measure the loaded voltage, calculate resistance and log*/
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_afc_dis_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  vth_aopen_rly(mod_id, K812);   /*DETHI connect relay*/
  vth_aclose_rly(mod_id, K813);   /*DETLO connect relay*/
  vth_aopen_rly(mod_id, K2303);   /*DETLO input ground relay*/
  vth_aclose_rly(mod_id, K2305);   /*DETLO input relay*/
  vth_aclose_rly(mod_id, K2304);   /*DETHI input ground relay*/

  vth_afc_en_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  measure_dmm(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*measure the loaded voltage, calculate resistance and log*/
    dmm_1m_plus_det_voltage = result.UU.Real_result;
    if (dmm_1m_plus_det_voltage != 0)
      detector_input_resistance = dmm_1m_plus_det_voltage * RDMM * r1m /
          (dmm_1m_voltage * (RDMM + r1m) - dmm_1m_plus_det_voltage * RDMM -
           dmm_1m_plus_det_voltage * r1m);
    else
      detector_input_resistance = DEFAULT_DET_RESISTANCE;
    result.UU.Real_result = fabs(detector_input_resistance);
    log_result(mod_id, result);
  }  /*measure the loaded voltage, calculate resistance and log*/
  else
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9470 */


#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef DEFAULT_DET_RESISTANCE
#undef NPLC
#undef TEST_VOLTAGE____
#undef RESOLUTION
#undef RDMM
#undef SOURCE_WAIT_TIME
#undef ENABLE_WAIT_TIME
#undef CHANNEL
#undef DMM_FUNCTION


/* Revision Control */



#define LOW_RANGE       0
#define HIGH_RANGE      8
#define INT_TIME        1   /* LONG INTEGRATE TIME */
#define AVERAGES        10   /* AVERAGE 10 READINGS */
#define DIGITAL         0   /* NOT IMPLEMENTED */

#define EXP_COM_MODE_OUT  10.0e+00   /* MAX COMMON MODE OUTPUT VOLTAGE */
#define TIMEOUT         10.0000e+00   /* SECONDS */
#define TEST_VOLTAGE____  10.0000e+00
    /* ABSOLUTE COMMON MODE INPUT VOLTAGE */
#define MIN_VOLTAGE_RANGE  39.0625e-03   /* MINIMUM DETECTOR VOLTAGE RANGE */
#define DETECTOR_RESOLUTION  0.00005e+00   /* PER THE SPEC SHEET */
#define VDET_OUT_STD_DELTA  20.0000e+00   /* INPUT VOLTAGE DELTA */
#define CMRR_MAX_VALUE  160.000e+00   /* MAXIMUM CMRR NUMBER */
#define SOURCE_WAIT_TIME  1.00000e-03   /* WAIT FOR THE SOURCE TO COME UP */


void t9471(int mod_id, int /*slot*/)
{
  /* DC DETECTOR CMRR WITH 1kOHM LEAD IMBALANCE */
  int range;

  double vdet_out_plus_cmode;
  double vdet_out_minus_cmode;
  double vdet_out_cmode_delta;
  double min_difference;
  double range_voltage;
  double cmrr;

  test_result_type result;



  vth_dcv_setup(mod_id, INT_TIME, AVERAGES, DIGITAL, EXP_COM_MODE_OUT, TIMEOUT);
  vth_aopen_rly(mod_id, K812);   /* DETHI TO I BUS */
  vth_aopen_rly(mod_id, K820);   /* DETLO TO L BUS */

  /* CONNECT SOURCE TO 1 KOHM SHORTED DETECTOR INPUTS */
  vth_aclose_rly(mod_id, K826);   /* SRCOUT TO REF2 */
  vth_aclose_rly(mod_id, K828);   /* REF2 TO DETHI */
  vth_aclose_rly(mod_id, K813);   /* I BUS TO DETLO */
  vth_aclose_rly(mod_id, K847);   /* SHORT DETHI AND DETLO WITH 1 KOHM */

  vth_afc_en_src_out(mod_id);

  /* FOR EACH DETECTOR RANGE MEASURE DETECTOR OUTPUT FOR -10 V AND +10 V
      COMMON MODE AND CALCULATE CMRR */
  for (range = LOW_RANGE; range <= HIGH_RANGE; range++)
  {
    vth_adet_sel_rng(mod_id, range);

    /* SET INITIAL VALUE FOR vdet_out_minus_cmode TO -99 (no result) */
    vdet_out_minus_cmode = -99.0;
    vth_asrc_set_dc_ampl(mod_id, -1 * TEST_VOLTAGE____);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    vth_afunc_meas(mod_id);   /* TAKE - COM MODE MEAS */
    vth_get_afunc_result(mod_id);   /* PUT RESULT IN RESULT LOG */
    if (get_result_nolog(mod_id, &result))
      vdet_out_minus_cmode = result.UU.Real_result;
    else
      display_error(mod_id, "Expected CMRR minus common mode detector volts");
    if (!get_result_nolog(mod_id, &result))
      display_error(mod_id, "Expected CMRR detector status (minus common)");

    /* IF THE DETECTOR UPRANGED, THEN SET RESULT TO -99 */
    check_uprange(mod_id, &vdet_out_minus_cmode);

    /* SET INITIAL VALUE FOR vdet_out_plus_cmode TO -99 (no result) */
    vdet_out_plus_cmode = -99.0;
    vth_asrc_set_dc_ampl(mod_id, TEST_VOLTAGE____);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    vth_afunc_meas(mod_id);   /* TAKE + COM MODE MEAS */
    vth_get_afunc_result(mod_id);   /* PUT RESULT IN RESULT LOG */
    if (get_result_nolog(mod_id, &result))
      vdet_out_plus_cmode = result.UU.Real_result;
    else
      display_error(mod_id, "Expected CMRR plus common mode detector volts");
    if (!get_result_nolog(mod_id, &result))
      display_error(mod_id, "Expected CMRR detector status (plus common)");

    /* IF THE DETECTOR UPRANGED, THEN SET RESULT TO -99 */
    check_uprange(mod_id, &vdet_out_plus_cmode);

    vdet_out_cmode_delta = vdet_out_plus_cmode - vdet_out_minus_cmode;

    range_voltage = MIN_VOLTAGE_RANGE * pow(2.0, static_cast<double>(range));
    min_difference = range_voltage * DETECTOR_RESOLUTION;
    if (vdet_out_cmode_delta < min_difference)
      cmrr = CMRR_MAX_VALUE;
    else
      cmrr = 20.0 * log10(VDET_OUT_STD_DELTA / fabs(vdet_out_cmode_delta));

    /* IF NO RESULT OR DETECTOR UPRANGED, SET CMRR TO 0 ; */
    if (vdet_out_plus_cmode < -98 || vdet_out_plus_cmode < -98)
      cmrr = 0.0;
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = cmrr;
    log_result(mod_id, result);

  }

}  /* t9471 */


#undef LOW_RANGE
#undef HIGH_RANGE
#undef INT_TIME
#undef AVERAGES
#undef DIGITAL
#undef EXP_COM_MODE_OUT
#undef TIMEOUT
#undef TEST_VOLTAGE____
#undef MIN_VOLTAGE_RANGE
#undef DETECTOR_RESOLUTION
#undef VDET_OUT_STD_DELTA
#undef CMRR_MAX_VALUE
#undef SOURCE_WAIT_TIME


/* Revision Control */



#define LOW_RANGE       6
#define HIGH_RANGE      8
#define INT_TIME        1   /* LONG INTEGRATE TIME */
#define AVERAGES        10   /* AVERAGE 10 READINGS */
#define DIGITAL         0   /* NOT IMPLEMENTED */

#define EXP_COM_MODE_OUT  160.0e+00   /* MAX COMMON MODE OUTPUT VOLTAGE */
#define TIMEOUT         10.0000e+00   /* SECONDS */
#define TEST_VOLTAGE____  10.0000e+00
    /* ABSOLUTE COMMON MODE INPUT VOLTAGE */
#define MIN_VOLTAGE_RANGE  39.0625e-03   /* MINIMUM DETECTOR VOLTAGE RANGE */
#define DETECTOR_RESOLUTION  0.00005e+00   /* PER THE SPEC SHEET */
#define VDET_OUT_STD_DELTA  20.0000e+00   /* INPUT VOLTAGE DELTA */
#define CMRR_MAX_VALUE  160.000e+00   /* MAXIMUM CMRR NUMBER */
#define SOURCE_WAIT_TIME  1.00000e-03   /* WAIT FOR THE SOURCE TO COME UP */
#define ATTEN_FACTOR    16.0000e+00   /* ATTENUATION MULTIPLIER */


void t9472(int mod_id, int /*slot*/)
{
  /* ATTENUATED DC DETECTOR CMRR WITH 1kOHM LEAD IMBALANCE */
  int range;

  double vdet_out_plus_cmode;
  double vdet_out_minus_cmode;
  double vdet_out_cmode_delta;
  double min_difference;
  double range_voltage;
  double cmrr;

  test_result_type result;



  vth_dcv_setup(mod_id, INT_TIME, AVERAGES, DIGITAL, EXP_COM_MODE_OUT, TIMEOUT);
  vth_aopen_rly(mod_id, K812);   /* DETHI TO I BUS */
  vth_aopen_rly(mod_id, K820);   /* DETLO TO L BUS */

  /* CONNECT SOURCE TO 1 KOHM SHORTED DETECTOR INPUTS */
  vth_aclose_rly(mod_id, K826);   /* SRCOUT TO REF2 */
  vth_aclose_rly(mod_id, K828);   /* REF2 TO DETHI */
  vth_aclose_rly(mod_id, K813);   /* I BUS TO DETLO */
  vth_aclose_rly(mod_id, K847);   /* SHORT DETHI AND DETLO WITH 1 KOHM */

  vth_afc_en_src_out(mod_id);

  /* FOR EACH DETECTOR RANGE MEASURE DETECTOR OUTPUT FOR -10 V AND +10 V
      COMMON MODE AND CALCULATE CMRR */

  for (range = HIGH_RANGE; range >= LOW_RANGE; range--)
  {
    vth_adet_sel_rng(mod_id, range);

    /* SET INITIAL VALUE FOR vdet_out_minus_cmode TO -99 (no result) */
    vdet_out_minus_cmode = -99.0;
    vth_asrc_set_dc_ampl(mod_id, -1 * TEST_VOLTAGE____);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    vth_afunc_meas(mod_id);   /* TAKE - COM MODE MEAS */
    vth_get_afunc_result(mod_id);   /* PUT RESULT IN RESULT LOG */
    if (get_result_nolog(mod_id, &result))
      vdet_out_minus_cmode = result.UU.Real_result;
    else
      display_error(mod_id, "Expected CMRR minus common mode detector volts");
    if (!get_result_nolog(mod_id, &result))
      display_error(mod_id, "Expected CMRR detector status (minus common)");

    /* IF THE DETECTOR UPRANGED, THEN SET RESULT TO -99 */
    check_uprange(mod_id, &vdet_out_minus_cmode);

    /* SET INITIAL VALUE FOR vdet_out_plus_cmode TO -99 (no result) */
    vdet_out_plus_cmode = -99.0;
    vth_asrc_set_dc_ampl(mod_id, TEST_VOLTAGE____);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    vth_afunc_meas(mod_id);   /* TAKE + COM MODE MEAS */
    vth_get_afunc_result(mod_id);   /* PUT RESULT IN RESULT LOG */
    if (get_result_nolog(mod_id, &result))
      vdet_out_plus_cmode = result.UU.Real_result;
    else
      display_error(mod_id, "Expected CMRR plus common mode detector volts");
    if (!get_result_nolog(mod_id, &result))
      display_error(mod_id, "Expected CMRR detector status (plus common)");

    /* IF THE DETECTOR UPRANGED, THEN SET RESULT TO -99 */
    check_uprange(mod_id, &vdet_out_plus_cmode);

    vdet_out_cmode_delta = vdet_out_plus_cmode - vdet_out_minus_cmode;

    range_voltage = MIN_VOLTAGE_RANGE * pow(2.0, static_cast<double>(range)) * ATTEN_FACTOR;
    min_difference = range_voltage * DETECTOR_RESOLUTION;
    if (fabs(vdet_out_cmode_delta) < min_difference)
      cmrr = CMRR_MAX_VALUE;
    else
      cmrr = 20.0 * log10(VDET_OUT_STD_DELTA / fabs(vdet_out_cmode_delta));

    /* IF NO RESULT OR DETECTOR UPRANGED, SET CMRR TO 0 ; */
    if (vdet_out_plus_cmode < -98 || vdet_out_plus_cmode < -98)
      cmrr = 0.0;
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = cmrr;
    log_result(mod_id, result);

  }

}  /* t9472 */


#undef LOW_RANGE
#undef HIGH_RANGE
#undef INT_TIME
#undef AVERAGES
#undef DIGITAL
#undef EXP_COM_MODE_OUT
#undef TIMEOUT
#undef TEST_VOLTAGE____
#undef MIN_VOLTAGE_RANGE
#undef DETECTOR_RESOLUTION
#undef VDET_OUT_STD_DELTA
#undef CMRR_MAX_VALUE
#undef SOURCE_WAIT_TIME
#undef ATTEN_FACTOR
#undef IM_NEG

#define LOW             0
#define HIGH            1

typedef double det_volts[9];
typedef int det_relay[9];
typedef det_volts src_volts[2];
typedef det_relay src_load[2];


/* Revision Control */



#define IM_POS          1   /* imaginary integrate */
#define IM_NEG          2   /* other half cycle */
#define EXTRA_DIGIT     1   /* use long integrate */

#define FREQ_TO_5TAU    (5.0 / 2.0 / 3.14159)

const src_volts V_SOURCE = {
  { 0.028, 0.055, 0.110, 0.177, 0.177, 0.884, 1.768, 1.768, 7.071 },
  { 0.028, 0.055, 0.110, 0.221, 0.442, 0.884, 1.061, 3.536, 7.071 }
};

const src_load LOAD_RLY = {
  { K845, K845, K845, K845, K845, K846, K846, K846, K847 },
  { K845, K845, K845, K845, K845, K845, K845, K846, K846 }
};


void t9475(int mod_id, int /*slot*/)
{
  /* Quadrature Rejection Loaded Source */
  test_result_type result;
  double quad_pos;
  double quad_neg;
  double quad_voltage;
  double ratio;
  double db;
  int freq_idx;
  int freq;

  vth_aclose_rly(mod_id, K826);   /* SRC -> REFR2 */
  vth_aclose_rly(mod_id, K828);   /* REFR2 -> DETHI */
  vth_aclose_rly(mod_id, K816);   /* I to G */
  vth_aclose_rly(mod_id, K814);   /* G to ACOM */
  vth_aopen_rly(mod_id, K2304);   /* DET -> GND */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2306);   /* DETHI -> DET */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  for (freq_idx = 1; freq_idx <= 3; freq_idx++)   /* 128, 1024, 8192 */
  {  /* FOR freq_idx */
    switch (freq_idx)
    {

    case 1:
      freq = 128;
      break;

    case 2:
      freq = 1024;
      break;

    case 3:
      freq = 8192;
      break;
    default:
      assert(false);
      break;

    }/*CASE*/
    vth_asrc_sel_fltr(mod_id, freq_idx + 4);
    vth_aphsync_en_setup(mod_id);
    vth_ascat_set_a_freq(mod_id, freq);
    /*redundant but necessary to setup DET before giving start trigger*/
    vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_POS);
    vth_ascat_ass_psrctrg(mod_id);
    for (int load = LOW; load <= HIGH; ++load)
    {  /* for load */
      if (load == LOW)
        vth_asrc_en_30ma_ilim(mod_id);
      else
        vth_asrc_dis_30ma_ilim(mod_id);
      for (int range = 0; range <= 8; ++range)
      {
        vth_adet_sel_rng(mod_id, range);
        vth_asrc_set_ac_ampl(mod_id, V_SOURCE[load][range]);
        vth_aclose_rly(mod_id, LOAD_RLY[load][range]);
        vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
        vth_afc_en_src_out(mod_id);
        vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_POS);
        vth_mtmr_wait(mod_id, FREQ_TO_5TAU / freq);   /* 5 time const */
        vth_arng_aincrct_meas(mod_id, freq_idx);
        vth_adet_rd_incrct_volt(mod_id);
        vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_NEG);
        vth_arng_aincrct_meas(mod_id, freq_idx);
        vth_adet_rd_incrct_volt(mod_id);
        vth_afc_dis_src_out(mod_id);
        vth_aopen_rly(mod_id, LOAD_RLY[load][range]);
      }  /*FOR range*/
    }  /* for load */
  }  /*FOR freq_idx*/

  /*collect readings, calc dB*/
  for (freq_idx = 1; freq_idx <= 3; freq_idx++)
  {
    for (int load = LOW; load <= HIGH; ++load)
    {
      for (int range = 0; range <= 8; ++range)
      {
        if (get_result_nolog(mod_id, &result))
          quad_pos = result.UU.Real_result;
        if (get_result_nolog(mod_id, &result))
          quad_neg = result.UU.Real_result;
        quad_voltage = (quad_pos - quad_neg) / 2.0;
        /*ratio to source voltage*/
        ratio = fabs(quad_voltage / V_SOURCE[load][range]);
        if (ratio < 1.0e-6)   /*getting too small for ln routine*/
          db = -100.0;
        else  /*convert to dB*/
          db = 20 * log(ratio) / log(10.0);
        result.UU.Real_result = db;
        log_result(mod_id, result);
      }
    }  /* for load */
  }  /*FOR freq_idx*/
}  /*T9475*/

#undef LOW
#undef HIGH

#undef IM_POS
#undef IM_NEG
#undef EXTRA_DIGIT
#undef FREQ_TO_5TAU
#undef R156
#undef R312


//////////////////////////////////////////NEW TEST FOR NASRU T9474, EQUAL TO T9475////////////////////

#define LOW             0
#define HIGH            1

typedef double det_volts[9];
typedef int det_relay[9];
typedef det_volts src_volts[2];
typedef det_relay src_load[2];


/* Revision Control */



#define IM_POS          1   /* imaginary integrate */
#define IM_NEG          2   /* other half cycle */
#define EXTRA_DIGIT     1   /* use long integrate */

#define FREQ_TO_5TAU    (5.0 / 2.0 / 3.14159)

const src_volts V_SOURCE_NASRU = {
  { 0.028, 0.055, 0.110, 0.177, 0.177, 0.884, 1.768, 1.768, 7.071 },
  { 0.028, 0.055, 0.110, 0.221, 0.442, 0.884, 1.061, 3.536, 7.071 }
};

const src_load LOAD_RLY_NASRU = {
  { K845, K845, K845, K845, K845, K846, K846, K846, K847 },
  { K845, K845, K845, K845, K845, K845, K845, K846, K846 }
};


void t9474(int mod_id, int /*slot*/)
{
  /* Quadrature Rejection Loaded Source */
  test_result_type result;
  double quad_pos;
  double quad_neg;
  double quad_voltage;
  double ratio;
  double db;
  int freq_idx;
  int freq;

  vth_aclose_rly(mod_id, K826);   /* SRC -> REFR2 */
  vth_aclose_rly(mod_id, K828);   /* REFR2 -> DETHI */
  vth_aclose_rly(mod_id, K816);   /* I to G */
  vth_aclose_rly(mod_id, K814);   /* G to ACOM */
  vth_aopen_rly(mod_id, K2304);   /* DET -> GND */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2306);   /* DETHI -> DET */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  for (freq_idx = 1; freq_idx <= 3; freq_idx++)   /* 128, 1024, 8192 */
  {  /* FOR freq_idx */
    switch (freq_idx)
    {

    case 1:
      freq = 128;
      break;

    case 2:
      freq = 1024;
      break;

    case 3:
      freq = 8192;
      break;
    default:
      assert(false);
      break;

    }/*CASE*/
    vth_asrc_sel_fltr(mod_id, freq_idx + 4);
    vth_aphsync_en_setup(mod_id);
    vth_ascat_set_a_freq(mod_id, freq);
    /*redundant but necessary to setup DET before giving start trigger*/
    vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_POS);
    vth_ascat_ass_psrctrg(mod_id);
    for (int load = LOW; load <= HIGH; ++load)
    {  /* for load */
      if (load == LOW)
        vth_asrc_en_30ma_ilim(mod_id);
      else
        vth_asrc_dis_30ma_ilim(mod_id);
      for (int range = 0; range <= 8; ++range)
      {
        vth_adet_sel_rng(mod_id, range);
        vth_asrc_set_ac_ampl(mod_id, V_SOURCE_NASRU[load][range]);
        vth_aclose_rly(mod_id, LOAD_RLY_NASRU[load][range]);
        vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
        vth_afc_en_src_out(mod_id);
        vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_POS);
        vth_mtmr_wait(mod_id, FREQ_TO_5TAU / freq);   /* 5 time const */
        vth_arng_aincrct_meas(mod_id, freq_idx);
        vth_adet_rd_incrct_volt(mod_id);
        vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_NEG);
        vth_arng_aincrct_meas(mod_id, freq_idx);
        vth_adet_rd_incrct_volt(mod_id);
        vth_afc_dis_src_out(mod_id);
        vth_aopen_rly(mod_id, LOAD_RLY_NASRU[load][range]);
      }  /*FOR range*/
    }  /* for load */
  }  /*FOR freq_idx*/

  /*collect readings, calc dB*/
  for (freq_idx = 1; freq_idx <= 3; freq_idx++)
  {
    for (int load = LOW; load <= HIGH; ++load)
    {
      for (int range = 0; range <= 8; ++range)
      {
        if (get_result_nolog(mod_id, &result))
          quad_pos = result.UU.Real_result;
        if (get_result_nolog(mod_id, &result))
          quad_neg = result.UU.Real_result;
        quad_voltage = (quad_pos - quad_neg) / 2.0;
        /*ratio to source voltage*/
        ratio = fabs(quad_voltage / V_SOURCE_NASRU[load][range]);
        if (ratio < 1.0e-6)   /*getting too small for ln routine*/
          db = -100.0;
        else  /*convert to dB*/
          db = 20 * log(ratio) / log(10.0);
        result.UU.Real_result = db;
        log_result(mod_id, result);
      }
    }  /* for load */
  }  /*FOR freq_idx*/
}  /*T9474*/

#undef LOW
#undef HIGH

#undef IM_POS
#undef IM_NEG
#undef EXTRA_DIGIT
#undef FREQ_TO_5TAU
#undef R156
#undef R312

//////////////////////////////////////////END OF TEST T9474///////////////////////////////////////////

#define NO_             0

#define HIGH            2

typedef double src_volts_[4];


/* Revision Control */



#define R156            2
#define R312            3
#define IM_POS          1   /* imaginary integrate */
#define IM_NEG          2   /* other half cycle */
#define EXTRA_DIGIT     1   /* use long integrate */

#define FREQ_TO_5TAU    (5.0 / 2.0 / 3.14159)

const src_volts_ V_SOURCE_ = {
  0.110, 0.100, 0.100, 0.110
};


/*a*/
void t9476(int mod_id, int /*slot*/)
{
  /* Source Crossover Quadrature Rejection */
  test_result_type result;
  double quad_pos;
  double quad_neg;
  double quad_voltage;
  double ratio;
  double db;
  int freq_idx;
  int condition;
  int freq;

  vth_aclose_rly(mod_id, K826);   /* SRC -> REFR2 */
  vth_aclose_rly(mod_id, K828);   /* REFR2 -> DETHI */
  vth_aclose_rly(mod_id, K816);   /* I to G */
  vth_aclose_rly(mod_id, K814);   /* G to ACOM */
  vth_aopen_rly(mod_id, K2304);   /* DET -> GND */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2306);   /* DETHI -> DET */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  for (freq_idx = 1; freq_idx <= 3; freq_idx++)   /* 128, 1024, 8192 */
  {  /* FOR freq_idx */
    switch (freq_idx)
    {

    case 1:
      freq = 128;
      break;

    case 2:
      freq = 1024;
      break;

    case 3:
      freq = 8192;
      break;
    default:
      assert(false);
      break;

    }/*CASE*/
    vth_asrc_sel_fltr(mod_id, freq_idx + 4);
    vth_aphsync_en_setup(mod_id);
    vth_ascat_set_a_freq(mod_id, freq);
    /*redundant but necessary to setup DET before giving start trigger*/
    vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_POS);
    vth_ascat_ass_psrctrg(mod_id);
    for (int load = NO_; load <= HIGH; ++load)
    {  /* for load */
      if (load == HIGH)
        vth_asrc_dis_30ma_ilim(mod_id);
      else
        vth_asrc_en_30ma_ilim(mod_id);
      for (condition = 0; condition <= 3; condition++)
      {
        if (condition == 0 || condition == 1)
          vth_adet_sel_rng(mod_id, R156);
        else
          vth_adet_sel_rng(mod_id, R312);
        vth_asrc_set_ac_ampl(mod_id, V_SOURCE_[condition]);
        if (load != NO_)
        {
          vth_aclose_rly(mod_id, K845);   /* 10 ohm load */
          vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
        }
        vth_afc_en_src_out(mod_id);
        vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_POS);
        vth_mtmr_wait(mod_id, FREQ_TO_5TAU / freq);   /* 5 time const */
        vth_arng_aincrct_meas(mod_id, freq_idx);
        vth_adet_rd_incrct_volt(mod_id);
        vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_NEG);
        vth_arng_aincrct_meas(mod_id, freq_idx);
        vth_adet_rd_incrct_volt(mod_id);
        vth_afc_dis_src_out(mod_id);
        if (load != NO_)
        {
          vth_aopen_rly(mod_id, K845);   /* 10 ohm load */
          vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
        }
      }  /*FOR condition*/
    }  /* for load */
  }  /*FOR freq_idx*/

  /*collect readings, calc dB*/
  for (freq_idx = 1; freq_idx <= 3; freq_idx++)
  {
    for (int load = NO_; load <= HIGH; ++load)
    {
      for (condition = 0; condition <= 3; condition++)
      {
        if (get_result_nolog(mod_id, &result))
          quad_pos = result.UU.Real_result;
        if (get_result_nolog(mod_id, &result))
          quad_neg = result.UU.Real_result;
        quad_voltage = (quad_pos - quad_neg) / 2.0;
        /*ratio to source voltage*/
        ratio = fabs(quad_voltage / V_SOURCE_[condition]);
        if (ratio < 1.0e-6)   /*getting too small for ln routine*/
          db = -100.0;
        else  /*convert to dB*/
          db = 20 * log(ratio) / log(10.0);
        result.UU.Real_result = db;
        log_result(mod_id, result);
      }
    }  /* for load */
  }  /*FOR freq_idx*/
}  /*T9476*/

#undef NO_

#undef HIGH

#undef R156
#undef R312
#undef IM_POS
#undef IM_NEG
#undef EXTRA_DIGIT
#undef FREQ_TO_5TAU

/////////////////////////////////////////////NEW TEST T9477 for NASRU - Equal to T9476///////////

#define NO_             0

#define HIGH            2

typedef double src_volts_[4];


/* Revision Control */



#define R156            2
#define R312            3
#define IM_POS          1   /* imaginary integrate */
#define IM_NEG          2   /* other half cycle */
#define EXTRA_DIGIT     1   /* use long integrate */

#define FREQ_TO_5TAU    (5.0 / 2.0 / 3.14159)

const src_volts_ V_SOURCE__NASRU = {
  0.110, 0.100, 0.100, 0.110
};


/*a*/
void t9477(int mod_id, int /*slot*/)
{
  /* Source Crossover Quadrature Rejection */
  test_result_type result;
  double quad_pos;
  double quad_neg;
  double quad_voltage;
  double ratio;
  double db;
  int freq_idx;
  int condition;
  int freq;

  vth_aclose_rly(mod_id, K826);   /* SRC -> REFR2 */
  vth_aclose_rly(mod_id, K828);   /* REFR2 -> DETHI */
  vth_aclose_rly(mod_id, K816);   /* I to G */
  vth_aclose_rly(mod_id, K814);   /* G to ACOM */
  vth_aopen_rly(mod_id, K2304);   /* DET -> GND */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_aclose_rly(mod_id, K2306);   /* DETHI -> DET */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  for (freq_idx = 1; freq_idx <= 3; freq_idx++)   /* 128, 1024, 8192 */
  {  /* FOR freq_idx */
    switch (freq_idx)
    {

    case 1:
      freq = 128;
      break;

    case 2:
      freq = 1024;
      break;

    case 3:
      freq = 8192;
      break;
    default:
      assert(false);
      break;

    }/*CASE*/
    vth_asrc_sel_fltr(mod_id, freq_idx + 4);
    vth_aphsync_en_setup(mod_id);
    vth_ascat_set_a_freq(mod_id, freq);
    /*redundant but necessary to setup DET before giving start trigger*/
    vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_POS);
    vth_ascat_ass_psrctrg(mod_id);
    for (int load = NO_; load <= HIGH; ++load)
    {  /* for load */
      if (load == HIGH)
        vth_asrc_dis_30ma_ilim(mod_id);
      else
        vth_asrc_en_30ma_ilim(mod_id);
      for (condition = 0; condition <= 3; condition++)
      {
        if (condition == 0 || condition == 1)
          vth_adet_sel_rng(mod_id, R156);
        else
          vth_adet_sel_rng(mod_id, R312);
        vth_asrc_set_ac_ampl(mod_id, V_SOURCE__NASRU[condition]);
        if (load != NO_)
        {
          vth_aclose_rly(mod_id, K845);   /* 10 ohm load */
          vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
        }
        vth_afc_en_src_out(mod_id);
        vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_POS);
        vth_mtmr_wait(mod_id, FREQ_TO_5TAU / freq);   /* 5 time const */
        vth_arng_aincrct_meas(mod_id, freq_idx);
        vth_adet_rd_incrct_volt(mod_id);
        vth_ascat_ld_incrct_time(mod_id, EXTRA_DIGIT, freq_idx - 1, IM_NEG);
        vth_arng_aincrct_meas(mod_id, freq_idx);
        vth_adet_rd_incrct_volt(mod_id);
        vth_afc_dis_src_out(mod_id);
        if (load != NO_)
        {
          vth_aopen_rly(mod_id, K845);   /* 10 ohm load */
          vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
        }
      }  /*FOR condition*/
    }  /* for load */
  }  /*FOR freq_idx*/

  /*collect readings, calc dB*/
  for (freq_idx = 1; freq_idx <= 3; freq_idx++)
  {
    for (int load = NO_; load <= HIGH; ++load)
    {
      for (condition = 0; condition <= 3; condition++)
      {
        if (get_result_nolog(mod_id, &result))
          quad_pos = result.UU.Real_result;
        if (get_result_nolog(mod_id, &result))
          quad_neg = result.UU.Real_result;
        quad_voltage = (quad_pos - quad_neg) / 2.0;
        /*ratio to source voltage*/
        ratio = fabs(quad_voltage / V_SOURCE__NASRU[condition]);
        if (ratio < 1.0e-6)   /*getting too small for ln routine*/
          db = -100.0;
        else  /*convert to dB*/
          db = 20 * log(ratio) / log(10.0);
        result.UU.Real_result = db;
        log_result(mod_id, result);
      }
    }  /* for load */
  }  /*FOR freq_idx*/
}  /*T9477*/

#undef NO_

#undef HIGH

#undef R156
#undef R312
#undef IM_POS
#undef IM_NEG
#undef EXTRA_DIGIT
#undef FREQ_TO_5TAU




/////////////////////////////////////////////END OF TEST T9477/////////////////////////////////



/* Revision Control */



#define NUM_SAMPLES     100


typedef int sign_array[2];
typedef double range_array___[9];


/* dc */


/* CPU trigger */
#define TRIG_MODE       0
/* Free Run */
#define MODE            0
/* Immediate */
#define TRIG_TYPE       0
/* Not used */

#define TRIG_THRESHOLD  0.0
#define TIMEOUT        10.0
#define FUDGE_FACTOR    1.1e+00

#define PERIOD_         100e-06

const range_array___ VOLTAGE = {
  39.0625e-03, 78.125e-03, 156.25e-03, 312.5e-03, 625e-03, 1.25e00, 2.5e00,
  5e00, 10e00
};

const sign_array DIRECTION = {
  -1, 1
};


/*a*/

void t9480(int mod_id, int /*slot*/)
{
  /* DC DIGITIZER VOLTAGE ACCURACY */
int range;
  /* 1=FullScale, 2=HalfScale */
int scale;
  /* 1=Negative, 2=Postive */
int sign;
int i;

  double dc_amplitude;
  double dc_src_voltage;
  double dmm_actual_voltage;
  double sum;
  double mean;
  double delta;

  test_result_type result;


  break_test_for_function_step_compatability(mod_id);

  open_dmm(mod_id);
  open_mux(mod_id);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  /* CLOSE THE XMINT TO DET PATH RELAYS */
  vth_aclose_rly(mod_id, K857);   /* L to ACOM */
  vth_aclose_rly(mod_id, K836);   /* SRCOUT to DETHI */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ACOM */
  vth_aclose_rly(mod_id, K853);   /* CALIB1 to RCAL2 */

  vth_asrc_sel_fltr(mod_id, FILTER_128);
  vth_asrc_en_30ma_ilim(mod_id);
  vth_asrc_set_dc_ampl(mod_id, 0.0);
  vth_afc_en_src_out(mod_id);

  for (range = 0; range <= 8; range++)
  {  /*for range*/
    for (scale = 1; scale <= 2; scale++)
    {  /*for scale; scale of 1 is full scale, scale of 2 is half scale*/
      dc_amplitude = VOLTAGE[range] / scale;
      setup_dmm(mod_id, DCV, dc_amplitude * FUDGE_FACTOR, dc_amplitude);
      for (sign = 0; sign <= 1; sign++)
      {  /* positve to negative */
        dc_src_voltage = dc_amplitude * DIRECTION[sign];
        vth_asrc_set_dc_ampl(mod_id, dc_src_voltage);
        vth_aclose_rly(mod_id, K837);   /*SRCOUT to RCAL2*/
        vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
        measure_dmm(mod_id);
        if (get_result_nolog(mod_id, &result))
          dmm_actual_voltage = result.UU.Real_result;
        else
          display_prompt("No result in result log (Expected dmm volts)", true);
        vth_aopen_rly(mod_id, K837);
        vth_dgtz_setup(mod_id, VOLTAGE[range], PERIOD_, NUM_SAMPLES, TRIG_MODE,
                   MODE, TRIG_TYPE, TRIG_THRESHOLD, TIMEOUT);
        vth_aopen_rly(mod_id, K812);   /* DETHI to I */
        vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
        vth_afunc_meas(mod_id);
        vth_adgtz_rtn_volts(mod_id, NUM_SAMPLES);
        sum = 0.0;
        for (i = 1; i <= NUM_SAMPLES; i++)
        {
          if (get_result_nolog(mod_id, &result))
            sum += result.UU.Real_result;
          else
            display_prompt("No result in log (Expected DIG volts)", true);
        }

        mean = sum / NUM_SAMPLES;
        delta = mean - dmm_actual_voltage;
        vth_adet_get_uprng(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          if (result.UU.Integer_result != 0)
          {
            display_prompt("DETECTOR uprange unexpectedly", true);
            delta = -99.9;
          }
        }
        else
          display_prompt("No result in log (Expected DET uprange)", true);
        result.Measurement_class = CLASS_REAL;
        result.UU.Real_result = delta;

        log_result(mod_id, result);
      }  /* postive to negative */


    }  /*for scale; scale of 1 is full scale, scale of 2 is half scale*/

  }  /*for range*/


  close_dmm(mod_id);
  close_mux(mod_id);

}  /* t9480 */


#undef NUM_SAMPLES

#undef TRIG_MODE
#undef MODE
#undef TRIG_TYPE
#undef TRIG_THRESHOLD
#undef TIMEOUT
#undef FUDGE_FACTOR

#undef PERIOD_


/* Revision Control */




typedef double range_array____[9];


#define NUM_SAMPLES     100
/* dc */

/* CPU trigger */
#define TRIG_MODE       0
/* Free Run */
#define MODE            0
/* Immediate */
#define TRIG_TYPE       0
/* Not used */

#define TRIG_THRESHOLD  0.0
#define TIMEOUT        10.0
/* sigma multiplier */

#define X               6

#define PERIOD_         100e-06

const range_array____ VOLTAGE_ = {
  39.0625e-03, 78.125e-03, 156.25e-03, 312.5e-03, 625e-03, 1.25e00, 2.5e00,
  5e00, 10e00
};


/*
********************************************************************************
*
* elw 8/8/88
* Description:  Triggers the detector and reads the integer result. Always
*               sleeps the backplane. Copied and modified version of vth.p
*               routine "trg_rd_inct_det".
*
********************************************************************************
*/
static void trg_rd_inct_det_bits(int mod_id)
{
  vth_mmbd_dis_all(mod_id);   /* module goes to sleep */

  vth_mmbd_arm_ris_aimin_edge(mod_id);
  vth_mmbd_ass_atrg(mod_id);   /* trigger the dettrg pin on the SCAT */
  vth_mmbd_wait_on_aimin(mod_id);   /* Wait for AIMIN/ (EOC) to be asserted */

  vth_mmbd_en_all(mod_id);   /* wake up the module */

  /* Do the reading */
  vth_adet_rd_adc(mod_id);

  /* rst_det_input(mod_id); reset input relays */
  vth_aopen_rly(mod_id, K2305);
  vth_aopen_rly(mod_id, K2306);
  vth_aopen_rly(mod_id, K2309);
  vth_aopen_rly(mod_id, K2310);
  vth_aopen_rly(mod_id, K2315);
  vth_aopen_rly(mod_id, K2316);
  vth_mtmr_wait(mod_id, REED_OPEN);
  vth_aclose_rly(mod_id, K2303);
  vth_aclose_rly(mod_id, K2304);
  vth_mtmr_wait(mod_id, REED_CLOSE);
}  /*  trg_rd_inct_det_bits  */




void t9481(int mod_id, int /*slot*/)
{
  /* DC DIGITIZER OFFSET AND NOISE OVER RANGES */
int range;
int i;
  double sample;
  double sum;
  double sum_of_squares;
  double max_value;
  double min_value;
  double mean;
  double noise;
  test_result_type result;

  //open_mux(mod_id);
  
  /*Dummy loop for exercising & warm up the ADC*/
  /*added by jys Nov2010*/
  for (int loop = 0; loop <3; loop++)
  {
	vth_setup_dc_inct_det(mod_id, GND_BOTH_, R39_, LONG_);
	trg_rd_inct_det_bits(mod_id);
	get_result_nolog(mod_id, &result);
  }


  for (range = 0; range <= 8; range++)
  {  /*for range*/
    vth_dgtz_setup(mod_id, VOLTAGE_[range], PERIOD_, NUM_SAMPLES, TRIG_MODE, MODE,
               TRIG_TYPE, TRIG_THRESHOLD, TIMEOUT);
    vth_aclose_rly(mod_id, K2303);
    vth_aclose_rly(mod_id, K2304);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    vth_afunc_meas(mod_id);
    vth_adgtz_rtn_volts(mod_id, NUM_SAMPLES);

    for (i = 1; i <= NUM_SAMPLES; i++)
    {
      if (get_result_nolog(mod_id, &result))
      {  /* accumulate sum, sigma, and min and max */
        sample = result.UU.Real_result;

        if (i == 1)
        {  /* special case for first sample */
          sum = sample;
          sum_of_squares = sample * sample;
          max_value = sample;
          min_value = sample;
        }  /* special case for first sample */
        else
        {  /* standard case for I = 2 to number of samples */
          sum += sample;
          sum_of_squares += sample * sample;
          max_value = larger(max_value, sample);
          min_value = smaller(min_value, sample);
        }  /* standard case for I = 2 to number of samples */


      }  /* accumulate sum, sigma, and min and max */
      else
        display_prompt("No result in result log (Expected DIG volts)", true);


    }


    stats(&mean, &noise, sum, sum_of_squares, NUM_SAMPLES, min_value,
          max_value, X);

    /* LOG THE MEAN AND THE NOISE */
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = mean;
    log_result(mod_id, result);
    result.UU.Real_result = noise;
    log_result(mod_id, result);
  }  /*for range*/


  //close_mux(mod_id);




}  /* t9481 */


#undef NUM_SAMPLES
#undef FREQUENCY_
#undef TRIG_MODE
#undef MODE
#undef TRIG_TYPE
#undef TRIG_THRESHOLD
#undef TIMEOUT
#undef X
#undef PERIOD_


/* Revision Control */



#define NUM_SAMPLES     100
/* dc */

/* CPU trigger */
#define TRIG_MODE       0
/* Free Run */
#define MODE            0
/* Immediate */
#define TRIG_TYPE       0
/* Not used */

#define TRIG_THRESHOLD  0.0
#define TIMEOUT        10.0
#define ZERO_INPUT      0.0
/* sigma multiplier */

#define X               6

#define NUM_MEAS        8


typedef double period_array[NUM_MEAS];


const period_array PERIOD_ = {
  25e-06, 250e-06, 1e-03, 2.5e-03, 10e-3, 16.6667e-03, 20e-03, 31.2e-03
};


void t9482(int mod_id, int /*slot*/)
{
  /* DC DIGITIZER OFFSET AND NOISE OVER SAMPLING RATE */
int j;
int i;

  double sample;
  double sum;
  double sum_of_squares;
  double max_value;
  double min_value;
  double mean;
  double noise;

  test_result_type result;

  open_mux(mod_id);

  for (j = 0; j < NUM_MEAS; j++)
  {  /*for period*/
    vth_dgtz_setup(mod_id, ZERO_INPUT, PERIOD_[j], NUM_SAMPLES, TRIG_MODE, MODE,
               TRIG_TYPE, TRIG_THRESHOLD, TIMEOUT);
    vth_aclose_rly(mod_id, K2303);
    vth_aclose_rly(mod_id, K2304);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
    vth_afunc_meas(mod_id);
    vth_adgtz_rtn_volts(mod_id, NUM_SAMPLES);

    for (i = 1; i <= NUM_SAMPLES; i++)
    {
      if (get_result_nolog(mod_id, &result))
      {  /* accumulate sum, sigma, and min and max */
        sample = result.UU.Real_result;

        if (i == 1)
        {  /* special case for first sample */
          sum = sample;
          sum_of_squares = sample * sample;
          max_value = sample;
          min_value = sample;
        }  /* special case for first sample */
        else
        {  /* standard case for I = 2 to number of samples */
          sum += sample;
          sum_of_squares += sample * sample;
          max_value = larger(max_value, sample);
          min_value = smaller(min_value, sample);
        }  /* standard case for I = 2 to number of samples */


      }  /* accumulate sum, sigma, and min and max */
      else
        display_prompt("No result in result log (Expected DIG volts)", true);


    }


    stats(&mean, &noise, sum, sum_of_squares, NUM_SAMPLES, min_value,
          max_value, X);

    /* LOG THE MEAN AND THE NOISE */
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = mean;
    log_result(mod_id, result);
    result.UU.Real_result = noise;
    log_result(mod_id, result);
  }  /*for period*/


  close_mux(mod_id);




}  /* t9482 */


#undef NUM_SAMPLES

#undef TRIG_MODE
#undef MODE
#undef TRIG_TYPE
#undef TRIG_THRESHOLD
#undef TIMEOUT
#undef ZERO_INPUT
#undef X
#undef NUM_MEAS


/* mt9250.p */



/* End. */
