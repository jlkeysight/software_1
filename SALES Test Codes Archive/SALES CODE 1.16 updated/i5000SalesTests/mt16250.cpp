#pragma warning(disable:4786)
#include <time.h>//sri jan 20 07
#include <math.h>
#include <cassert>
#include <vector>
#include <set>
#include <algorithm>
#include <p2c/p2c.h>
#include <visa.h>
#include "../../../u/src/DebugWriter.h"
#include "../../../u/src/num_p.h"
#include "../../../vth/src/vth_aim.h"
#include "../../../vth/src/aimasm.h"
#include "../../../vth/src/vth_mac.h"
#include "../../../vth/src/macasm.h"
#include "../../../vth/src/vth_cap.h"
#include "../../../vth/src/MetaTesthead.h"
#include "../../../vth/src/ConnectBRCtoBus.h"
#include "../../../vth/src/ConnectDigitalResource.h"
#include "../../../dgn/src/termio_p.h"
#include "../../../dgn/src/dgnconfig_p.h"
#include "../i5000Tests/TestUtils.h"
#include "../i5000Tests/TestRelays.h"
#include "../i5000Tests/TestTalon.h" 
#include "../i5000Tests/TestPin.h" 
#include "../parms_p.h"
#include "mt16250.h"
#include "RelayPath.h"

//Dowd 4/15/04: The following relays had to be undef to work with Talon since they were 
//previously defined in /vth/src/pinasm_p.cpp for SIII use.
#undef KAS17
#undef KAS31
#undef KAS42
#undef KAS53
#undef KAS64
#undef KAS75
#undef KAS86
#undef KAS28
#undef KAS99
#undef KGLA
#undef KGLB

using HTalon::KSA;
using HTalon::KAA;
using HTalon::KIA;
using HTalon::KBA;
using HTalon::KGA;
using HTalon::KLA;
using HTalon::KSB;
using HTalon::KAB;
using HTalon::KIB;
using HTalon::KBB;
using HTalon::KGB;
using HTalon::KLB;
using HTalon::KSAB;
using HTalon::KIBB;
using HTalon::KGLA;
using HTalon::KGLB;
using HTalon::KSAA;
using HTalon::KIBA;
double measured;

/******************************************************************************************
* Control card 
*******************************************************************************************/

/*-------------------------------------------------------------------------------
* Test: 16250
* Description: Time interval counter (TIC) verification - frequency mode
*				
* Fixture: 
* Resource(s): Sythesizer, Generator
*--------------------------------------------------------------------------------
*/

#define SYN_FREQ_AF2    20000000
#define SYN_FREQ_CLKRB  60000000
#define GEN_PWIDTH      25e-09

void t16250(int mod_id, int /* slot */)//9050
{
  test_result_type result;
  OpenResourceManager();
  open_syn(mod_id);
  open_gen(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  connect_syn_out_gen_ext(mod_id);
  setup_syn_freq(mod_id, SYN_FREQ_AF2);
  setup_gen_pwidth(mod_id, GEN_PWIDTH);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  // CHECK THE FREQUENCY ACCURACY THROUGH THE AF2 PATH  
  connect_gen_out_af2(mod_id);
  vth_aclose_rly(mod_id, K753);   /* XL to AF2 relay */
  vth_aclose_rly(mod_id, K764);   /* XL to AF2 relay */
  vth_mclose_rly(mod_id, MK1614);   /* XL connect relay */
  vth_mclose_rly(mod_id, MK1604);   /* XL to TIC relay */
  vth_mclose_rly(mod_id, MK1602);   /* TIC A/B common  */
  enable_gen_out(mod_id);
  vth_mtmr_wait(mod_id, TIME_OUT);
  vth_mtic_sel_long_gate(mod_id);
  vth_mtic_freq_a_meas(mod_id);
  if (!get_result_log(mod_id, &result))
    display_prompt("No result in result log (Expected TIC reading)", true);

  vth_mtic_freq_b_meas(mod_id);
  if (!get_result_log(mod_id, &result))
    display_prompt("No result in result log (Expected TIC reading)", true);

  vth_mtic_sel_shrt_gate(mod_id);
  vth_mtic_freq_a_meas(mod_id);
  if (!get_result_log(mod_id, &result))
    display_prompt("No result in result log (Expected TIC reading)", true);

  vth_mtic_freq_b_meas(mod_id);
  if (!get_result_log(mod_id, &result))
    display_prompt("No result in result log (Expected TIC reading)", true);

  disable_gen_out(mod_id);
  vth_mopen_rly(mod_id, MK1614);   /* XL connect relay */
  vth_mopen_rly(mod_id, MK1604);   /* XL to TIC relay */

  //CHECK THE FREQUENCY ACCURACY THROUGH THE CLKRB PATH
  connect_syn_aux_gen_ext(mod_id);
  setup_syn_freq(mod_id, SYN_FREQ_CLKRB);
  connect_gen_trg_clkrb(mod_id);
  vth_mclose_rly(mod_id, MK2902);   /*CLKRB relay*/
  vth_mclose_rly(mod_id, MK2903);   /*CLKRSG relay*/
  vth_mclose_rly(mod_id, MK1601);   /*TIC A MUX to TIC A relay*/
  vth_mtic_a_dclkrb_mux(mod_id);
  vth_mtic_sel_long_gate(mod_id);
  vth_mtmr_wait(mod_id, 0.100);   /* See if this is required? */
  vth_mtic_freq_a_meas(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*if*/
    result.UU.Real_result *= 2;   /*CLKRB input divided by 2*/
    log_result(mod_id, result);
  }  /*if*/
  else
    display_prompt("No result in result log (Expected TIC reading)", true);

  vth_mtic_freq_b_meas(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*if*/
    result.UU.Real_result *= 2;   /*CLKRB input divided by 2*/
    log_result(mod_id, result);
  }  /*if*/
  else
    display_prompt("No result in result log (Expected TIC reading)", true);

  vth_mtic_sel_shrt_gate(mod_id);
  vth_mtic_freq_a_meas(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*if*/
    result.UU.Real_result *= 2;   /*CLKRB input divided by 2*/
    log_result(mod_id, result);
  }  /*if*/
  else
    display_prompt("No result in result log (Expected TIC reading)", true);

  vth_mtic_freq_b_meas(mod_id);
  if (get_result_nolog(mod_id, &result))
  {  /*if*/
    result.UU.Real_result *= 2;   /*CLKRB input divided by 2*/
    log_result(mod_id, result);
  }  /*if*/
  else
    display_prompt("No result in result log (Expected TIC reading)", true);

  vth_minit(mod_id);
  vth_ainit(mod_id);
  close_syn(mod_id);
  close_gen(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /*t16250*/
#undef SYN_FREQ_AF2
#undef SYN_FREQ_CLKRB
#undef GEN_PWIDTH


/*-------------------------------------------------------------------------------
* Test: 16252
* Description: Time interval counter (TIC) verification - TI MODE
*				
* Fixture: 
* Resource(s): Sythesizer, Generator, Counter
*--------------------------------------------------------------------------------
*/

#define SAMPLES         100
#define SYN_FREQ__      20000000
#define GEN_PWIDTH      25e-09
#define GEN_LO          (-1.8e+00)
#define GEN_HI          5.2e+00

void t16252(int mod_id, int /* slot */)//9052
{
  double actual_pos_pulse;
  double actual_neg_pulse;
  double tic_pos_pulse_lg;
  double tic_pos_pulse_sg;
  double tic_neg_pulse_lg;
  double tic_neg_pulse_sg;
  test_result_type result;
  OpenResourceManager();
  open_syn(mod_id);
  open_gen(mod_id);
  open_mux(mod_id);
  open_cntr(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  connect_syn_out_gen_ext(mod_id);
  setup_syn_freq(mod_id, SYN_FREQ__);
  setup_gen_pwidth(mod_id, GEN_PWIDTH);
  setup_gen_levels(mod_id, GEN_LO, GEN_HI);
  connect_gen_out_cntr_ch1(mod_id);
  enable_gen_out(mod_id);
  setup_cntr_out2(mod_id);
  setup_cntr_function(mod_id, PWID);
  trigger_cntr(mod_id);
  actual_pos_pulse = read_cntr(mod_id);
  setup_cntr_function(mod_id, NWID);
  trigger_cntr(mod_id);
  actual_neg_pulse = read_cntr(mod_id);
  disable_gen_out(mod_id);
  connect_gen_out_af2(mod_id);

  vth_aclose_rly(mod_id, K753);   /* XL to AF2 relay */
  vth_aclose_rly(mod_id, K764);   /* XL to AF2 relay */
  vth_mclose_rly(mod_id, MK1614);   /* XL connect relay */
  vth_mclose_rly(mod_id, MK1604);   /* XL to TIC relay */
  vth_mclose_rly(mod_id, MK1602);   /* TIC A/B common  */
  vth_mclose_rly(mod_id, MK1605);   /* TIC 50 ohm load */

  enable_gen_out(mod_id);
  vth_mtic_ti_abrf_fast_meas(mod_id, SAMPLES);
  if (get_result_nolog(mod_id, &result))
  {
    tic_pos_pulse_lg = result.UU.Real_result;
    result.UU.Real_result = tic_pos_pulse_lg - actual_pos_pulse;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected tic reading)", true);

  vth_mtic_ti_abrf_one_meas(mod_id);
  if (get_result_nolog(mod_id, &result))
  {
    tic_pos_pulse_sg = result.UU.Real_result;
    result.UU.Real_result = tic_pos_pulse_sg - actual_pos_pulse;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected tic reading)", true);

  vth_mtic_ti_abfr_fast_meas(mod_id, SAMPLES);
  if (get_result_nolog(mod_id, &result))
  {
    tic_neg_pulse_lg = result.UU.Real_result;
    result.UU.Real_result = tic_neg_pulse_lg - actual_neg_pulse;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected tic reading)", true);

  vth_mtic_ti_abfr_one_meas(mod_id);
  if (get_result_nolog(mod_id, &result))
  {
    tic_neg_pulse_sg = result.UU.Real_result;
    result.UU.Real_result = tic_neg_pulse_sg - actual_neg_pulse;
    log_result(mod_id, result);
  }
  else
    display_prompt("No result in result log (Expected tic reading)", true);

  disable_gen_out(mod_id);
  vth_minit(mod_id);
  vth_ainit(mod_id);
  close_syn(mod_id);
  close_gen(mod_id);
  close_cntr(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /*t16252*/

#undef SAMPLES
#undef SYN_FREQ__
#undef GEN_PWIDTH
#undef GEN_LO
#undef GEN_HI


/*CONTROL PLUS - USER CLOCK RANGE, ACCURACY, & JITTER*/

#define NUM_FREQUENCIES  4
#define CNTR_SAMPLE_SIZE  10
#define TRIGGER_LEVEL   0.6e+00
#define GATE_TIME       0.1e+00
#define TIMEOUT         1.0e-03
typedef double frequency_array___[NUM_FREQUENCIES];
const frequency_array___ FREQUENCIES___ = {625e+03, 20e+06, 40e+06, 50e+06};

void t16256(int mod_id, int /* slot */)//9056
{
  int loop;
  double cntr_actual_frequency;
  double cntr_actual_min;
  double cntr_actual_max;
  double cntr_max_minus_min;
  double cntr_duty_cycle;

  frequency_array___ falling_jitters;
  test_result_type result;
  OpenResourceManager();
  open_mux(mod_id);
  open_cntr(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  setup_cntr_trg_levels(mod_id, TRIGGER_LEVEL, TRIGGER_LEVEL);
  vth_mseq_stop(mod_id, TIMEOUT);
  vth_mclose_rly(mod_id, MK2801);   /* UCLK to Mint pins relay     */
  vth_mclose_rly(mod_id, MK2802);   /* UCLK GND to Mint pins relay */
  vth_mdclk_en_uclk(mod_id);

  for (loop = 0; loop < NUM_FREQUENCIES; loop++)
  {  /*for: frequencies*/
    vth_mdclk_0_set_freq(mod_id, FREQUENCIES___[loop]);
    connect_cntr_stop_uclk(mod_id);
    setup_cntr_function(mod_id, FREQUENCY);
    setup_cntr_gate_time(mod_id, GATE_TIME);
	flush_commands(mod_id);
    trigger_cntr(mod_id);
    cntr_actual_frequency = read_cntr_mean(mod_id);
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = cntr_actual_frequency;
    log_result(mod_id, result);

    setup_cntr_function(mod_id, PERIOD);
    setup_cntr_sample_size(mod_id, CNTR_SAMPLE_SIZE);
    trigger_cntr(mod_id);
    cntr_actual_min = read_cntr_min(mod_id);
    cntr_actual_max = read_cntr_max(mod_id);
    cntr_max_minus_min = cntr_actual_max - cntr_actual_min;
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = cntr_max_minus_min;
    log_result(mod_id, result);

    setup_cntr_trigger_slopes(mod_id, NEG, NEG);
    trigger_cntr(mod_id);
    cntr_actual_min = read_cntr_min(mod_id);
    cntr_actual_max = read_cntr_max(mod_id);
    cntr_max_minus_min = cntr_actual_max - cntr_actual_min;
    falling_jitters[loop] = cntr_max_minus_min;
    setup_cntr_trigger_slopes(mod_id, POS, POS);
    connect_cntr_ch1_uclk(mod_id);
    setup_cntr_function(mod_id, DCYCLE);
    trigger_cntr(mod_id);
    cntr_duty_cycle = read_cntr(mod_id);
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = cntr_duty_cycle * 100;
    log_result(mod_id, result);
  }  /*for: frequencies*/

  for (loop = 0; loop < NUM_FREQUENCIES; loop++)
  {  /* loop to log falling jitter values */
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = falling_jitters[loop];
    log_result(mod_id, result);
  }  

  vth_minit(mod_id);
  close_mux(mod_id);
  close_cntr(mod_id);
  CloseResourceManager();
}  /*t16256*/

#undef NUM_FREQUENCIES
#undef CNTR_SAMPLE_SIZE
#undef TRIGGER_LEVEL
#undef GATE_TIME
#undef TIMEOUT


/*SYNC TO CLOCK RANGE, & JITTER*/

#define NUM_FREQUENCIES  3
#define HIGH            0
#define LOW             1
#define CNTR_SAMPLE_SIZE  10
#define TIMEOUT         1.00e-03
#define GATE_TIME       0.0e+00
#define START_LEVEL     0.6e+00
#define STOP_LEVEL      0.6e+00
#define SIGMA_MULTIPLIER  6.0e+00
typedef int frequency_array____[NUM_FREQUENCIES];
typedef frequency_array____ syn_array__[2];
const frequency_array____ FREQUENCIES____ = {5250000, 19750000, 40000000};
const syn_array__ SYN_FREQ__ =
{
  { 5000000, 10000000, 40000000},
  {10000000, 20000000, 40000000}
};

void t16260(int mod_id, int /* slot */)//9060
{
  int loop;
  int debug_connect_module;
  int temp_debug_connect_module;
  double cntr_actual_frequency;
  double cntr_actual_min;
  double cntr_actual_max;
  double cntr_max_minus_min;
  double actual_sigma;

  test_result_type result;
  void (*p2c_temp)(int mod_id);
  temp_debug_connect_module = get_system_debug_port();
  if (temp_debug_connect_module != -1)
  {  /*has debug ports*/
    debug_connect_module = get_system_debug_port();
    OpenResourceManager();
    open_mux(mod_id);
    open_syn(mod_id);
    open_gen(mod_id);
    open_cntr(mod_id);
    vth_cfxt_lock(); 
    vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

    setup_cntr_trg_levels(mod_id, START_LEVEL, STOP_LEVEL);
    connect_gen_trg_clkrb(mod_id);
    connect_cntr_stop_db1(mod_id);
    vth_mseq_stop(mod_id, TIMEOUT);
    vth_mclose_rly(debug_connect_module, MK2305);
    vth_mclose_rly(mod_id, MK2902);   /* CLKRB to Mint pins relay   */
    vth_mclose_rly(mod_id, MK2903);   /* CLKRSG to Mint pins relay  */
    vth_mtclk_clkrb_mux(mod_id);
    vth_mdbg_1_gtclk_mux(mod_id);
    vth_mtclk_sel_ris_edge(mod_id);
    p2c_temp = flush_commands;
    all_modules(p2c_temp);
    all_modules(&vth_mhsl_dis_hdbg1);   /* Enable HDBG1 through card  */
    all_modules(&flush_commands);
    if (debug_connect_module != mod_id)
      vth_mdbg_1_hdbg1_mux(debug_connect_module);

    all_modules(&flush_commands);
    vth_mhsl_en_hdbg1(mod_id);   /* Enable this mod driving HDBG1 */
    for (loop = 0; loop < NUM_FREQUENCIES; loop++)
    {
      for (int tclk_loop = HIGH; tclk_loop <= LOW; ++tclk_loop)
      {
        if (tclk_loop == HIGH || FREQUENCIES____[loop] < 40e06)
        {  /*if*/
          setup_syn_freq(mod_id, SYN_FREQ__[tclk_loop][loop]);
          enable_gen_out(mod_id);
          if (FREQUENCIES____[loop] <= 20e06)
          {  /*for frequencies <= 20MHz*/
            connect_syn_out_gen_ext(mod_id);
            vth_mtclk_set_freq(mod_id, (double)FREQUENCIES____[loop],
                           (double)FREQUENCIES____[loop]);
          }  
          else
          {  /*Do 40MHz setup*/
            connect_syn_aux_gen_ext(mod_id);
            vth_mtclk_set_freq(mod_id, (double)FREQUENCIES____[loop],
                           FREQUENCIES____[loop] / 2.0);
          }  

          setup_cntr_function(mod_id, FREQUENCY);
          setup_cntr_sample_size(mod_id, CNTR_SAMPLE_SIZE);
          setup_cntr_gate_time(mod_id, GATE_TIME);
		  flush_commands(mod_id);
          trigger_cntr(mod_id);
          cntr_actual_frequency = read_cntr_mean(mod_id);
          result.Measurement_class = CLASS_REAL;
          result.UU.Real_result = cntr_actual_frequency;
          log_result(mod_id, result);

          setup_cntr_function(mod_id, PERIOD);
          setup_cntr_sample_size(mod_id, CNTR_SAMPLE_SIZE);
          trigger_cntr(mod_id);
          actual_sigma = read_cntr_sigma(mod_id) * SIGMA_MULTIPLIER;
          //show_value(mod_id, "std", actual_sigma);

          cntr_actual_min = read_cntr_min(mod_id);
          //show_value(mod_id, "min", cntr_actual_min);

          cntr_actual_max = read_cntr_max(mod_id);
          //show_value(mod_id, "max", cntr_actual_max);

          cntr_max_minus_min = cntr_actual_max - cntr_actual_min;
          result.Measurement_class = CLASS_REAL;
          if (cntr_max_minus_min > actual_sigma)
            result.UU.Real_result = cntr_max_minus_min;
          else
            result.UU.Real_result = actual_sigma;
          log_result(mod_id, result);

          disable_gen_out(mod_id);

        }  /*if*/
      }
    }

    vth_minit(mod_id);
    vth_minit(debug_connect_module);

    close_mux(mod_id);
    close_syn(mod_id);
    close_gen(mod_id);
    close_cntr(mod_id);
    CloseResourceManager();
  }  /*has debug ports*/

}  /*t16260*/

#undef NUM_FREQUENCIES
#undef HIGH
#undef LOW
#undef CNTR_SAMPLE_SIZE
#undef TIMEOUT
#undef GATE_TIME
#undef START_LEVEL
#undef STOP_LEVEL
#undef SIGMA_MULTIPLIER


/*CONTROL PLUS - SYNC TO CLOCK RANGE, & JITTER*/

#define NUM_FREQUENCIES  3
#define HIGH            0
#define LOW             1
#define CNTR_SAMPLE_SIZE  10
#define TIMEOUT         1.00e-03
#define GATE_TIME       0.0e+00
#define START_LEVEL     0.6e+00
#define STOP_LEVEL      0.6e+00
#define SIGMA_MULTIPLIER  6.0e+00
typedef int frequency_array_____[NUM_FREQUENCIES];
typedef frequency_array_____ syn_array___[2];
const frequency_array_____ FREQUENCIES_____ = {5250000, 19750000, 60000000};
const syn_array___ SYN_FREQ___ =
{
  { 5000000, 10000000, 60000000},
  {10000000, 20000000, 60000000}
};

void t16261(int mod_id, int /* slot */)//9061
{
  int loop;
  int debug_connect_module;
  int temp_debug_connect_module;
  double cntr_actual_frequency;
  double cntr_actual_min;
  double cntr_actual_max;
  double cntr_max_minus_min;
  double actual_sigma;

  test_result_type result;
  temp_debug_connect_module = get_system_debug_port();
  if (temp_debug_connect_module != -1)
  {  /*has debug ports*/
    debug_connect_module = get_system_debug_port();
    OpenResourceManager();
    open_mux(mod_id);
    open_syn(mod_id);
    open_gen(mod_id);
    open_cntr(mod_id);
    vth_cfxt_lock(); 
    vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
    setup_cntr_trg_levels(mod_id, START_LEVEL, STOP_LEVEL);
    connect_gen_trg_clkrb(mod_id);
    connect_cntr_stop_db1(mod_id);
    vth_mseq_stop(mod_id, TIMEOUT);
    vth_mclose_rly(debug_connect_module, MK2305);
    vth_mclose_rly(mod_id, MK2902);   /* CLKRB to Mint pins relay   */
    vth_mclose_rly(mod_id, MK2903);   /* CLKRSG to Mint pins relay  */
    vth_mtclk_clkrb_mux(mod_id);
    vth_mdbg_1_gtclk_mux(mod_id);
    vth_mtclk_sel_ris_edge(mod_id);
    all_modules(&flush_commands);
    all_modules(&vth_mhsl_dis_hdbg1);   /* Enable HDBG1 through card  */
    all_modules(&flush_commands);
    if (debug_connect_module != mod_id)
      vth_mdbg_1_hdbg1_mux(debug_connect_module);

    all_modules(&flush_commands);
    vth_mhsl_en_hdbg1(mod_id);   /* Enable this mod driving HDBG1 */
    for (loop = 0; loop < NUM_FREQUENCIES; loop++)
    {
      for (int tclk_loop = HIGH; tclk_loop <= LOW; ++tclk_loop)
      {
        if (tclk_loop == HIGH || FREQUENCIES_____[loop] < 40e06)
        {  /*if*/
          setup_syn_freq(mod_id, SYN_FREQ___[tclk_loop][loop]);
          enable_gen_out(mod_id);

          if (FREQUENCIES_____[loop] <= 20e06)
          {  /*for frequencies <= 20MHz*/
            connect_syn_out_gen_ext(mod_id);
            vth_mtclk_set_freq(mod_id, (double)FREQUENCIES_____[loop],
                           (double)FREQUENCIES_____[loop]);
          } 
          else
          {  /*Do 60MHz setup*/
            connect_syn_aux_gen_ext(mod_id);
            vth_mtclk_set_freq(mod_id, (double)FREQUENCIES_____[loop],
                           FREQUENCIES_____[loop] / 4.0);
          } 
          setup_cntr_function(mod_id, FREQUENCY);
          setup_cntr_sample_size(mod_id, CNTR_SAMPLE_SIZE);
          setup_cntr_gate_time(mod_id, GATE_TIME);
		  flush_commands(mod_id);
          trigger_cntr(mod_id);
          cntr_actual_frequency = read_cntr_mean(mod_id);
          result.Measurement_class = CLASS_REAL;
          result.UU.Real_result = cntr_actual_frequency;
          log_result(mod_id, result);

          setup_cntr_function(mod_id, PERIOD);
          setup_cntr_sample_size(mod_id, CNTR_SAMPLE_SIZE);
          trigger_cntr(mod_id);
          actual_sigma = read_cntr_sigma(mod_id) * SIGMA_MULTIPLIER;
          //show_value(mod_id, "std", actual_sigma);

          cntr_actual_min = read_cntr_min(mod_id);
          //show_value(mod_id, "min", cntr_actual_min);

          cntr_actual_max = read_cntr_max(mod_id);
          //show_value(mod_id, "max", cntr_actual_max);

          cntr_max_minus_min = cntr_actual_max - cntr_actual_min;
          result.Measurement_class = CLASS_REAL;
          if (cntr_max_minus_min > actual_sigma)
            result.UU.Real_result = cntr_max_minus_min;
          else
            result.UU.Real_result = actual_sigma;

          log_result(mod_id, result);
          disable_gen_out(mod_id);

        }  /*if*/
      }
    }

    vth_minit(mod_id);
    vth_minit(debug_connect_module);

    close_mux(mod_id);
    close_syn(mod_id);
    close_gen(mod_id);
    close_cntr(mod_id);
	CloseResourceManager();
  }  /*has debug ports*/

}  /*t16261*/

#undef NUM_FREQUENCIES
#undef HIGH
#undef LOW
#undef CNTR_SAMPLE_SIZE
#undef TIMEOUT
#undef GATE_TIME
#undef START_LEVEL
#undef STOP_LEVEL
#undef SIGMA_MULTIPLIER


/*Sequencer Margin Test*/

#define MAX_VECTORS     32
#define STARTING_INPUT_FREQ  5750000
#define MAXIMUM_INPUT_FREQ  20000000
#define FREQ_INCREMENT  500000
#define EXECUTION_WAIT_TIME  10e-03
#define STOP_WAIT_TIME  1e-03
#define CLEAR_FREQ      5000000

/* static variables for t16265: */
struct LOC_t16265
{
  int    mod_id;
  int    slot;
  double gen_pwidth;
};

static void clear_log_ram(struct LOC_t16265* LINK)
{
  int mod_id = LINK->mod_id;
  int slot   = LINK->slot;
  LINK->gen_pwidth = 1.0 / CLEAR_FREQ / 2;

  setup_syn_freq(LINK->mod_id, CLEAR_FREQ);
  setup_gen_pwidth(LINK->mod_id, LINK->gen_pwidth);
  enable_gen_out(LINK->mod_id);

  /* Adding to give equip time to setup before taking measurement -gpm */
    vth_mtmr_wait(mod_id, 0.100);

  vth_mtclk_set_freq(LINK->mod_id, (double)CLEAR_FREQ, CLEAR_FREQ * 2.0);

//#include "t16265_clr.cmd"

  sales_start_sequencer_mac(LINK->mod_id);
  vth_mtmr_wait(LINK->mod_id, EXECUTION_WAIT_TIME);
  vth_mseq_stop(LINK->mod_id, STOP_WAIT_TIME);
  disable_gen_out(LINK->mod_id);

}  /*clear_log_ram*/

#undef CLEAR_FREQ

void t16265(int mod_id, int slot)//9065
{
  int loop;
  int input_freq;
  int expected_status;
  int expected_vector_count;
  int expected_srp_log_ram_result[MAX_VECTORS];
  int expected_stat_log_ram_result[MAX_VECTORS];
  bool failure_occurred;
  char buffer[81];
  struct LOC_t16265 V;

  FILE *infile;
  test_result_type result;
  V.mod_id = mod_id;
  infile = NULL;
  slot = 2;
  V.slot = slot;

  sprintf(buffer, "%sseq_results", testwhere_path);
  infile = fopen(buffer, "r");
  if (infile == NULL)
    _EscIO(FileNotFound);

  fscanf(infile, "%d%*[^\n]", &expected_status);
  getc(infile);
  fscanf(infile, "%d%*[^\n]", &expected_vector_count);
  getc(infile);
  for (loop = 0; loop < MAX_VECTORS; loop++)
  {
    fscanf(infile, "%d%*[^\n]", &expected_srp_log_ram_result[loop]);
    getc(infile);
  }
  for (loop = 0; loop < MAX_VECTORS; loop++)
  {
    fscanf(infile, "%d%*[^\n]", &expected_stat_log_ram_result[loop]);
    getc(infile);
  }
  OpenResourceManager();
  open_syn(V.mod_id);
  open_gen(V.mod_id);
  open_mux(V.mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  connect_syn_out_gen_ext(V.mod_id);
  connect_gen_trg_clkrb(V.mod_id);
  failure_occurred = false;
  input_freq = STARTING_INPUT_FREQ;
  vth_mclose_rly(V.mod_id, MK2902);   /* CLKRB to Mint pins relay   */
  vth_mclose_rly(V.mod_id, MK2903);   /* CLKRSG to Mint pins relay  */
  vth_mtclk_clkrb_mux(V.mod_id);
  vth_mtclk_sel_ris_edge(V.mod_id);
  while (!failure_occurred && input_freq <= MAXIMUM_INPUT_FREQ)
  {  /*while*/
    clear_log_ram(&V);
    input_freq += FREQ_INCREMENT;
    V.gen_pwidth = 1.0 / input_freq / 2;
    setup_syn_freq(V.mod_id, input_freq);
    setup_gen_pwidth(V.mod_id, V.gen_pwidth);
    enable_gen_out(V.mod_id);
    vth_mtmr_wait(mod_id, TIME_OUT);
    if (input_freq <= 10e06)
      vth_mtclk_set_freq(V.mod_id, (double)input_freq, input_freq * 2.0);
    else
      vth_mtclk_set_freq(V.mod_id, 10e06, 20e06);

//#include "t16265.cmd"

    sales_start_sequencer_mac(V.mod_id);
    vth_mtmr_wait(V.mod_id, EXECUTION_WAIT_TIME);
    vth_mseq_stop(V.mod_id, STOP_WAIT_TIME);
    disable_gen_out(V.mod_id);
    vth_mseq_rd_seq_stat(V.mod_id);
    if (get_result_nolog(V.mod_id, &result))
    {
      if (result.UU.Integer_result != expected_status)
        failure_occurred = true;
    }
    else
      display_prompt("No result in result log (Expected seq status)", true);

    vth_mec_rd_vctr_cntr(V.mod_id);
    if (get_result_nolog(V.mod_id, &result))
    {
      if (result.UU.Integer_result != expected_vector_count)
        failure_occurred = true;
    }
    else
      display_prompt("No result in result log (Expected vector count)", true);

    vth_mseq_rd_srp_log_ram(V.mod_id, 0, expected_vector_count);
    for (loop = 0; loop < expected_vector_count; loop++)
    {
      if (get_result_nolog(V.mod_id, &result))
      {
        if (result.UU.Integer_result != expected_srp_log_ram_result[loop])
          failure_occurred = true;
      }
      else
        display_prompt("No result in result log (Expected srp log RAM)", true);
    }
    vth_mseq_rd_stat_log_ram(V.mod_id, 0, expected_vector_count);
    for (loop = 0; loop < expected_vector_count; loop++)
    {
      if (get_result_nolog(V.mod_id, &result))
      {
        if (result.UU.Integer_result != expected_stat_log_ram_result[loop])
          failure_occurred = true;
      }
      else
        display_prompt("No result in result log (Expected stat log RAM)",
                       true);
    }
  }  /*while*/

  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = input_freq * 2.0;
  log_result(V.mod_id, result);

  close_syn(V.mod_id);
  close_gen(V.mod_id);
  close_mux(V.mod_id);
  CloseResourceManager();
  if (infile != NULL)
    fclose(infile);
}  /*t16265*/

#undef MAX_VECTORS
#undef STARTING_INPUT_FREQ
#undef MAXIMUM_INPUT_FREQ
#undef FREQ_INCREMENT
#undef EXECUTION_WAIT_TIME
#undef STOP_WAIT_TIME



#define RISING_         0
#define FALLING_        1
#define MAX_            0
#define MIN_            1
#define RATIO_NUMBER    0
#define T0_NOM          160e-9
#define SYNC_ADVANCE    5.9e-9
#define LARGE_STEP      2e-9
#define SMALL_STEP      250e-12
#define ET1A_MASK       127
#define SEARCH_ESTIMATE  4e-9
#define SEARCH_LIMIT    20e-9
#define CNTR_MEASUREMENT_TIME  0.10
typedef double control_array_type[2];
const control_array_type TCLK_ADVANCE = {T0_NOM + SYNC_ADVANCE, SYNC_ADVANCE};
const control_array_type ET_DELAY = {100e-9, -30e-9};
const control_array_type NOM_FREQ = {10e6, 7.69e6};
const control_array_type ET_SEARCH_ADVANCE = {0.0, T0_NOM};

void t16275(int mod_id, int slot)//9075
{
  /* Event trigger range/accuracy */
  int event;
  int strg;
  int debug_connect_module;
  int temp_debug_connect_module;
  double first;
  double last;
  double freq;
  bool toggling;
  bool bogus;

  test_result_type result;
  result.Measurement_class = CLASS_REAL;

  temp_debug_connect_module = get_system_debug_port();
  if (temp_debug_connect_module != -1)
  {  /*has debug ports*/
    debug_connect_module = get_system_debug_port();

	OpenResourceManager();
    open_mux(mod_id);
    open_syn(mod_id);
    open_gen(mod_id);
    open_cntr(mod_id);
    vth_cfxt_lock(); 
    vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

    connect_syn_out_gen_ext(mod_id);
    connect_gen_trg_clkrb(mod_id);
    connect_cntr_stop_db1(mod_id);
    arm_gen(mod_id);
    vth_mclose_rly(debug_connect_module, MK2305);
    vth_mclose_rly(mod_id, MK2902);   /* CLKRB to Mint pins relay   */
    vth_mclose_rly(mod_id, MK2903);   /* CLKRSG to Mint pins relay  */
    vth_mclose_rly(mod_id, MK1801);   /* ET1A  to MINT pins */
    vth_mdbg_1_et_strg_mux(mod_id);

    all_modules(&flush_commands);
    all_modules(&vth_mhsl_dis_hdbg1);
    all_modules(&flush_commands);
    if (debug_connect_module != mod_id)
      vth_mdbg_1_hdbg1_mux(debug_connect_module);

    all_modules(&flush_commands);
    vth_mhsl_en_hdbg1(mod_id);
    vth_mtclk_clkrb_mux(mod_id);
    vth_mtclk_sel_ris_edge(mod_id);
    vth_met_init(mod_id);
    vth_met_set_trg_cnd(mod_id, ET1A_MASK);
    vth_met_en(mod_id);
    vth_mseq_run(mod_id);
    vth_mtclk_gtclk_set_ratio(mod_id, RATIO_NUMBER);

    for (int edge = RISING_; edge <= FALLING_; ++edge)
    {  /* for edge */
      if (edge == FALLING_)
      {
        vth_mtclk_sel_fal_edge(mod_id);
        event = 0;   /* Event false */
      }
      else
      {
        vth_mtclk_sel_ris_edge(mod_id);
        event = 1;   /* Event true */
      }

      for (int delay = MAX_; delay <= MIN_; ++delay)
      {  /* for delay */
        bogus = false;
        vth_mseq_ass_prog_mode(mod_id);
        vth_met_set_t0_trg_dly(mod_id, ET_DELAY[delay]);
        vth_mseq_ass_run_mode(mod_id);
        freq = 1.0 / (1.0 / NOM_FREQ[delay] + SEARCH_ESTIMATE);
        //show_value(mod_id, "initial freq", freq);
        setup_syn_freq(mod_id, ceil(freq));
        vth_mtclk_set_freq(mod_id, freq, freq);
        vth_mtclk_set_adv_time(mod_id, TCLK_ADVANCE[delay]);
        vth_mtmr_wait(mod_id, CNTR_MEASUREMENT_TIME);
        toggling = counter_measurement_pending(mod_id);
        sales_show_state(mod_id, "initial toggle", toggling);
        vth_met_tst_et_strg(mod_id);
        if (get_result_nolog(mod_id, &result))
        {
          strg = result.UU.Integer_result;
        }
        else
          display_prompt("No result in result log (Expected STRG)", true);

        if (event == strg || toggling)   /* bad estimate */
        {  /* reprogram for full limit */
          sales_show_state(mod_id, "reprogram for full limit, toggle", toggling);
          freq = 1.0 / (1.0 / NOM_FREQ[delay] + SEARCH_LIMIT);
          //show_value(mod_id, "full limit freq", freq);
          setup_syn_freq(mod_id, ceil(freq));
          vth_mtclk_set_freq(mod_id, freq, freq);
          vth_mtmr_wait(mod_id, CNTR_MEASUREMENT_TIME);
          toggling = counter_measurement_pending(mod_id);
          sales_show_state(mod_id, "full limit toggle", toggling);
          vth_met_tst_et_strg(mod_id);
          if (get_result_nolog(mod_id, &result))
		  {
             strg = result.UU.Integer_result;
		  }
          else
            display_prompt("No result in result log (Expected STRG)", true);

        }  /* reprogram for full limit */

        /* -----  MAIN LOOP ------------------------------------------------------ */

        if (event != strg && !toggling)
        {  /* if valid event */
          while (!toggling &&
                 1.0 / freq >= 1.0 / NOM_FREQ[delay] - SEARCH_LIMIT &&
                 event != strg)
          {  /* while searching */
            freq = 1.0 / (1.0 / freq - LARGE_STEP);
            //show_value(mod_id, "1st search loop freq", freq);
            setup_syn_freq(mod_id, ceil(freq));
            vth_mtclk_set_freq(mod_id, freq, freq);
            vth_mtmr_wait(mod_id, CNTR_MEASUREMENT_TIME);
            toggling = counter_measurement_pending(mod_id);
            sales_show_state(mod_id, "1st search loop toggle", toggling);
            vth_met_tst_et_strg(mod_id);
            if (get_result_nolog(mod_id, &result))
			{
				strg = result.UU.Integer_result;
			}
            else
              display_prompt("No result in result log (Expected STRG)", true);

          }  /* while searching */

          if (toggling || event == strg)
          {  /* if toggling */
            freq = 1.0 / (1.0 / freq + LARGE_STEP - SMALL_STEP);
            //show_value(mod_id, "backed up freq", freq);
            setup_syn_freq(mod_id, ceil(freq));
            vth_mtclk_set_freq(mod_id, freq, freq);
            vth_mtmr_wait(mod_id, CNTR_MEASUREMENT_TIME);
            toggling = counter_measurement_pending(mod_id);
            sales_show_state(mod_id, "backed up toggle", toggling);
            vth_met_tst_et_strg(mod_id);
            if (get_result_nolog(mod_id, &result))
			{
				strg = result.UU.Integer_result;
			}
            else
              display_prompt("No result in result log (Expected STRG)", true);

            while (!toggling && 1.0 / freq >= 1.0 / NOM_FREQ[delay] - SEARCH_LIMIT && event != strg)
            {  /* while searching */
              freq = 1.0 / (1.0 / freq - SMALL_STEP);
              //show_value(mod_id, "2nd search freq", freq);
              setup_syn_freq(mod_id, ceil(freq));
              vth_mtclk_set_freq(mod_id, freq, freq);
              vth_mtmr_wait(mod_id, CNTR_MEASUREMENT_TIME);
              toggling = counter_measurement_pending(mod_id);
              sales_show_state(mod_id, "2nd search toggle", toggling);
              vth_met_tst_et_strg(mod_id);
              if (get_result_nolog(mod_id, &result))
			  {
				strg = result.UU.Integer_result;
			  }
              else
                display_prompt("No result in result log (Expected STRG)", true);

            }  /* while searching */

            /* -----  END MAIN LOOP -------------------------------------------------- */

            first = 1.0 / ceil(freq) - ET_SEARCH_ADVANCE[delay];
            result.Measurement_class = CLASS_REAL;
            result.UU.Real_result = first;
            log_result(mod_id, result);

          }  /* if toggling */
          else
          {  /*value is bogus*/
            bogus = true;
            display_prompt("No Valid Event Trigger Signal Measured", true);
            result.Measurement_class = CLASS_REAL;
            result.UU.Real_result = 0.0;
                /* dummy result assigned and logged */
            log_result(mod_id, result);
            log_result(mod_id, result);
            log_result(mod_id, result);
          }  /*value is bogus*/

          /* -----  SECOND   LOOP -------------------------------------------------- */

          if (!bogus)
          {  /*search for the other value and the delta*/
            while ((toggling || event != strg) &&
                   1.0 / freq >= 1.0 / NOM_FREQ[delay] - SEARCH_LIMIT)
            {  /* while searching */
              freq = 1.0 / (1.0 / freq - SMALL_STEP);
              //show_value(mod_id, "end search freq", freq);
              setup_syn_freq(mod_id, ceil(freq));
              vth_mtclk_set_freq(mod_id, freq, freq);
              vth_mtmr_wait(mod_id, CNTR_MEASUREMENT_TIME);
              toggling = counter_measurement_pending(mod_id);
              sales_show_state(mod_id, "end search toggle", toggling);
              vth_met_tst_et_strg(mod_id);
              if (get_result_nolog(mod_id, &result))
			  {
				strg = result.UU.Integer_result;
			  }
              else
                display_prompt("No result in result log (Expected STRG)", true);

            }  /* while searching */

            /* -----  END SECOND LOOP ------------------------------------------------ */

            last = 1.0 / ceil(freq) - ET_SEARCH_ADVANCE[delay];
            result.Measurement_class = CLASS_REAL;
            result.UU.Real_result = last;
            log_result(mod_id, result);
            result.UU.Real_result = first - last;
            log_result(mod_id, result);
          }  /*search for the other value and the delta*/
        }  /* if valid event */
        else
        {  /* else no valid event */
          display_prompt("No Valid Event Trigger Signal Measured", true);
          result.Measurement_class = CLASS_REAL;
          result.UU.Real_result = 0.0;
              /* dummy result assigned and logged */
          log_result(mod_id, result);
          log_result(mod_id, result);
          log_result(mod_id, result);
        }  /* else no valid event */

      }  /* for delay */
    }  /* for edge */
  }  /*has debug ports*/

  vth_minit(mod_id);
  close_syn(mod_id);
  close_gen(mod_id);
  close_cntr(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /* t16275 */

#undef RISING_
#undef FALLING_
#undef MAX_
#undef MIN_
#undef RATIO_NUMBER
#undef T0_NOM
#undef SYNC_ADVANCE
#undef LARGE_STEP
#undef SMALL_STEP
#undef ET1A_MASK
#undef SEARCH_ESTIMATE
#undef SEARCH_LIMIT
#undef CNTR_MEASUREMENT_TIME


#define NON_INVERTED    0
#define INVERTED        1
#define PROBE_NUMBER    1
#define LINKS_TO_MASTER  0
#define SLEW_RATE       225
#define TCLK_FREQ       5.0e+06
#define GEN_LOW_UNTERMINATED  0.1e+00
#define GEN_LOW_TERMINATED  0.2e+00
#define GEN_HIGH_UNTERMINATED  1.75e+00
#define GEN_HIGH_TERMINATED  3.5e+00
#define PROBE_SPEC      8e-09
#define TIMEOUT         1.0e-03
#define STOP_WAIT       100e-03
#define PROBE_THRESHOLD  1.5e+00
#define RES             1.0e-09
typedef double delay_array__[2];
const delay_array__ PROG_DELAY__ = {100e-09, -29e-09};

struct LOC_t16297
{
  int mod_id;
  int value;
  double gen_delay;
  bool exit_variable;
  test_result_type result;
} ;

/* ----------------------------------------------------------------------- */

static void sample_(struct LOC_t16297 * LINK)
{
  int mod_id = LINK->mod_id;

  setup_gen_trg_out_delay(LINK->mod_id, LINK->gen_delay, true);
  enable_gen_out(LINK->mod_id);
  sales_reset_sequencer_aim(LINK->mod_id);

  if (LINK->value == 1)
    {
//#include "t16297_d100.cmd"
    }
  else
    {
//#include "t16297_dn30.cmd"
    }

  sales_start_sequencer_mac(LINK->mod_id);
  vth_mseq_wait_on_stop(LINK->mod_id, STOP_WAIT);
  vth_mseq_stop(LINK->mod_id, TIMEOUT);

  vth_mmbd_rd_arcv1(LINK->mod_id);   /* check WERR */
  if (get_result_nolog(LINK->mod_id, &LINK->result))
  {
    if (LINK->result.UU.Integer_result != 1)
    {   /* no WERR */
      vth_mmbd_rd_arcv0(LINK->mod_id);
      if (get_result_nolog(LINK->mod_id, &LINK->result))
      {
        if (LINK->result.UU.Integer_result == 1)
          LINK->exit_variable = true;
        else
          LINK->exit_variable = false;
      }
    }
  }
}  /* sample */

/* ----------------------------------------------------------------------- */

void t16297(int mod_id)//9191
{
  /*Probe Edge Accuracy and Skew*/
  int loop;
  int probe_connect_module;
  double delay_inc;
  double direction;
  double transition_time;
  double period;
  double expected_receive;
  double actual_receive;
  double cal_min_set;
  double cal_max_set;
  double cal_min_actual;
  double cal_max_actual;
  double probe_range;
  double skew;
  double offset[2];
  double gain[2];
  double probe_result[2];
  bool expected_response;
  bool found;
  bool hit;
  struct LOC_t16297 V;
  RACK_DELAYS Rack;
  Rack.ReadDelays();

  probe_connect_module = get_system_probe_location();
  V.mod_id = probe_connect_module;
  vth_mod_init_dgn(V.mod_id);
  present_test[V.mod_id - NO_TESTHEAD_NUMBER] = 9191;
  present_test_type[V.mod_id - NO_TESTHEAD_NUMBER] = SINGLE_MODULE_SINGLE_SLOT;
  present_slot[V.mod_id - NO_TESTHEAD_NUMBER] = 1;
  present_subtest[V.mod_id - NO_TESTHEAD_NUMBER] = 0;
  if (V.mod_id < 0 || V.mod_id > 3)
  {  /* Display wrong  configuration error message */
    display_prompt("Configuration in error.  Cannot find a module", true);
    display_prompt("that is connected to the probe.", true);
  }  
  else
  {  /* Do the test */
    open_syn(V.mod_id);
    open_gen(V.mod_id);
    open_mux(V.mod_id);
    open_cntr(V.mod_id);
    period = 1 / TCLK_FREQ;
    probe_range = 2.0 * PROBE_SPEC;
    transition_time = (GEN_HIGH_TERMINATED - GEN_LOW_TERMINATED) * 0.8 * 1e-06/ SLEW_RATE;
    for (V.value = 1; V.value <= 2; V.value++)
    {
      expected_receive = PROG_DELAY__[V.value - 1];
      if (V.value == 2)   /* -30nS receives in the next period */
        expected_receive += period;

      cal_min_set = Rack.ProbeDelay(mod_id) + expected_receive - probe_range;
      cal_max_set = Rack.ProbeDelay(mod_id) + expected_receive +  probe_range;
      //display_it(V.mod_id, "cal_min_set = ", 0, cal_min_set);
      //display_it(V.mod_id, "cal_max_set = ", 0, cal_max_set);
      cal_min_set = nanotrunc(cal_min_set);
      cal_max_set = nanotrunc(cal_max_set);

      //display_it(V.mod_id, "expected_receive = ", 0, expected_receive);
      //display_it(V.mod_id, "probe_clkrb_delay = ", 0, probe_clkrb_delay[V.mod_id]);
      //display_it(V.mod_id, "probe_range = ", 0, probe_range);
      //display_it(V.mod_id, "cal_min_set = ", 0, cal_min_set);
      //display_it(V.mod_id, "cal_max_set = ", 0, cal_max_set);
      connect_syn_out_gen_ext(V.mod_id);
      connect_gen_trg_cntr_start(V.mod_id);
      connect_gen_out_cntr_stop(V.mod_id);
      setup_gen_levels(V.mod_id, GEN_LOW_TERMINATED, GEN_HIGH_TERMINATED);
      setup_gen_transition_times(V.mod_id, transition_time);
      no_complement_gen_out(V.mod_id);
      setup_cntr_trigger_slopes(V.mod_id, POS, POS);
      for (loop = NON_INVERTED; loop <= INVERTED; ++loop)
      {  /* Measure the offset and gain for pos and neg generator output */
        setup_gen_trg_out_delay(V.mod_id, cal_min_set, true);
        enable_gen_out(V.mod_id);
        show_value(V.mod_id, "delay ", cal_min_set);
        //display_it(V.mod_id, "delay = ", 0, cal_min_set);
        trigger_cntr(V.mod_id);
        cal_min_actual = read_cntr_mean(V.mod_id);
        if (cal_min_actual < period / 10)
          cal_min_actual += period;

        //display_it(V.mod_id, "*cal_min_actual = ", 0, cal_min_actual);

        setup_gen_trg_out_delay(V.mod_id, cal_max_set, true);
        //show_value(V.mod_id, "delay ", cal_max_set);
        trigger_cntr(V.mod_id);
        cal_max_actual = read_cntr_mean(V.mod_id);
        if (cal_max_actual < period / 10)
          cal_max_actual += period;
      
		//display_it(V.mod_id, "*cal_max_actual = ", 0, cal_max_actual);
        disable_gen_out(V.mod_id);
        offset[loop] = cal_min_actual;
        gain[loop] = (cal_max_actual - cal_min_actual) / (cal_max_set - cal_min_set);
        complement_gen_out(V.mod_id);
        setup_cntr_trigger_slopes(V.mod_id, POS, NEG);
      }  /* Measure the offset and gain for pos and neg generator output */

      setup_gen_levels(V.mod_id, GEN_LOW_UNTERMINATED, GEN_HIGH_UNTERMINATED);
      connect_gen_trg_clkrb(V.mod_id);
      connect_gen_out_probe(V.mod_id);
      vth_mclose_rly(V.mod_id, MK2902);   /* clkrb path relay */
      vth_mclose_rly(V.mod_id, MK2903);   /* clkrsg path relay */
      vth_aclose_rly(V.mod_id, K2203);   /* probe in relay */
      vth_mtclk_clkrb_mux(V.mod_id);
      vth_mtclk_sel_ris_edge(V.mod_id);
      vth_mtclk_set_freq(V.mod_id, TCLK_FREQ, TCLK_FREQ);
      vth_mtclk_set_cal_adv_time(V.mod_id, LINKS_TO_MASTER);
      vth_mmbd_en_arcv(V.mod_id);
      vth_aarcv0_en(V.mod_id);
      vth_aarcv1_en(V.mod_id);
      vth_afc_gt1_prb(V.mod_id);
      vth_afc_en(V.mod_id);
      vth_aprbhi_set_trg_lvl(V.mod_id, PROBE_THRESHOLD);
      vth_aprblo_set_trg_lvl(V.mod_id, PROBE_THRESHOLD);
      vth_afc_sel_arcv0(V.mod_id, PROBE_NUMBER, 3);   /* ARCV0 = RRESP */
      vth_afc_sel_arcv1(V.mod_id, PROBE_NUMBER, 4);   /* ARCV1 = WERR  */

      for (loop = NON_INVERTED; loop <= INVERTED; ++loop)
      {  /* for loop */
        if (loop == NON_INVERTED)
        {
          no_complement_gen_out(V.mod_id);
          expected_response = true;
        }
        else
        {
          complement_gen_out(V.mod_id);
          expected_response = false;
        }

        delay_inc = 4.0e-9;
        direction = 0.0;
        hit = false;
        found = false;
        V.gen_delay = expected_receive + Rack.ProbeDelay(V.mod_id);
        do
        {
          sample_(&V);
          //show_value(V.mod_id, "gen delay", V.gen_delay);
          if (V.exit_variable == expected_response)
          {  /* delay too small */
            if (direction < 0)
              hit = true;

            direction = 1.0;
          }
          else
          {
            if (direction > 0)
              hit = true;

            direction = -1.0;
          }
          if (hit)
          {
            delay_inc /= 2;
			if (delay_inc <= RES / 2)
              found = true;

          }
          V.gen_delay += delay_inc * direction;
        } while (!(found || V.gen_delay < cal_min_set ||
                   V.gen_delay > cal_max_set));

        actual_receive = gain[loop] * (V.gen_delay - cal_min_set) +
                         offset[loop] - Rack.ProbeDelay(V.mod_id);
       // display_it(V.mod_id, "gain[loop]                  = ", 0, gain[loop]);
       // display_it(V.mod_id, "V.gen_delay                 = ", 0, V.gen_delay);
       // display_it(V.mod_id, "cal_min_set                 = ", 0, cal_min_set);
       // display_it(V.mod_id, "offset[loop]                = ", 0, offset[loop]);
       // display_it(V.mod_id, "probe_clkrb_delay[V.mod_id] = ", 0, probe_clkrb_delay[V.mod_id]);
       // show_value(V.mod_id, "actual rec", actual_receive);
       // show_value(V.mod_id, "expected rec", expected_receive);

        probe_result[loop] = actual_receive - expected_receive;
       // display_it(V.mod_id, "probe_result = ", 0, probe_result[loop]);
       // display_it(V.mod_id, "expected_receive = ", 0, expected_receive);
       // display_it(V.mod_id, "actual_receive              = ", 0, actual_receive);
        V.result.Measurement_class = CLASS_REAL;
        if (!found)
        {   /* it never received */
          probe_result[loop] = INT_MAX;
          display_prompt("*RECEIVE DOES NOT OCCUR WITHIN TEST RANGE*", true);
        }
        V.result.UU.Real_result = probe_result[loop];
        log_result(V.mod_id, V.result);
      }  /* for loop */

      disable_gen_out(V.mod_id);
      skew = probe_result[NON_INVERTED] - probe_result[INVERTED];
      V.result.UU.Real_result = skew;
      log_result(V.mod_id, V.result);

    }  /* for value */

    close_syn(V.mod_id);
    close_gen(V.mod_id);
    close_mux(V.mod_id);
    close_cntr(V.mod_id);
	CloseResourceManager();
  }  /* Do the test */

}  /* t16297 */

#undef NON_INVERTED
#undef INVERTED
#undef PROBE_NUMBER
#undef LINKS_TO_MASTER
#undef SLEW_RATE
#undef TCLK_FREQ
#undef GEN_LOW_UNTERMINATED
#undef GEN_LOW_TERMINATED
#undef GEN_HIGH_UNTERMINATED
#undef GEN_HIGH_TERMINATED
#undef PROBE_SPEC
#undef TIMEOUT
#undef STOP_WAIT
#undef PROBE_THRESHOLD
#undef RES


/*Probe DC Voltage Accuracy and Range*/

#define PROBE_NUMBER		1
#define RANGE				30
#define RESOLUTION			0.001e+00
#define STARTING_STEP_SIZE  0.064e+00
#define FINEST_INCREMENT	0.004e+00
#define DMM_FUNCTION		DCV
#define HI					0
#define LO					1

void probe_voltage_accuracy(int mod_id, int test_number)//9195 & 9196
{
  int search_level;
  int gt;
  int probe_connect_module;
  double probe_voltage;
  double no_trip_voltage;
  double voltage_increment;
  double stop_voltage;
  double skew;
  double probe_result[2][2];
  double max_voltage;
  double min_voltage;
  double expected_rcv_lvl;
  bool search_level_found;

  test_result_type result;
  result.Measurement_class = CLASS_REAL;
  if(test_number == 16298)
  {
	 max_voltage = 5.400e+00;
	 min_voltage = 4.600e+00;
	 expected_rcv_lvl = 5.000e+00;
  }
  else
  {
	 max_voltage = -3.10e+00;
	 min_voltage = -3.90e+00;
	 expected_rcv_lvl = -3.50e+00;
  }

  probe_connect_module = get_system_probe_location();
  mod_id = probe_connect_module;
  vth_mod_init_dgn(mod_id);
  present_test[mod_id - NO_TESTHEAD_NUMBER] = 16298;
  present_test_type[mod_id - NO_TESTHEAD_NUMBER] = SINGLE_MODULE_SINGLE_SLOT;
  present_slot[mod_id - NO_TESTHEAD_NUMBER] = 1;
  present_subtest[mod_id - NO_TESTHEAD_NUMBER] = 0;
  if (mod_id < 0 || mod_id > 3)
  {  /* Display wrong configuration error message */
    display_prompt("Configuration in error.  Cannot find a module", true);
    display_prompt("that is connected to the probe.", true);
  }  
  else
  {  /* Do the test */
	OpenResourceManager();
    open_dmm(mod_id);
    open_mux(mod_id);
    vth_cfxt_lock(); 
    vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

    setup_dmm(mod_id, DMM_FUNCTION, (double)RANGE, RESOLUTION);
    connect_dmm_probe(mod_id);

    vth_aclose_rly(mod_id, K806);   /* aim source resistor connect */
    vth_aclose_rly(mod_id, K2201);   /* S bus to PROBS */
    vth_aclose_rly(mod_id, PROBR);   /* probe tip relay */
    vth_aclose_rly(mod_id, K2203);   /* probe_in relay */

    /* SETUP AIM SOURCE */
    vth_asrc_dis_30ma_ilim(mod_id);
    vth_afc_en_src_out(mod_id);
    vth_afc_gt1_prb(mod_id);
    vth_afc_en(mod_id);
    vth_mmbd_en_arcv(mod_id);		/* Enable ARCV lines onto MAC */
    vth_aarcv0_en(mod_id);			/* Enable ARCV0 line off AIM */
    vth_aarcv1_en(mod_id);			/* Enable ARCV1 line off AIM */
    vth_afc_sel_arcv0(mod_id, PROBE_NUMBER, 0);   /* ARCV0 = GTL */
    vth_afc_sel_arcv1(mod_id, PROBE_NUMBER, 1);   /* ARCV1 = GTH */
    vth_aprbhi_set_trg_lvl(mod_id, expected_rcv_lvl);
    vth_aprblo_set_trg_lvl(mod_id, expected_rcv_lvl);

    for (gt = HI; gt <= LO; ++gt)
    {  /* SEARCH GTH,GTL FOR FIRST LOW WHILE DECREASING THROUGH THRESHOLD */
      search_level = 0;
      search_level_found = false;
      voltage_increment = STARTING_STEP_SIZE;
      probe_voltage = max_voltage;
      no_trip_voltage = max_voltage;
      vth_asrc_set_dc_ampl(mod_id, probe_voltage);
      stop_voltage = min_voltage;

      while (!search_level_found && probe_voltage >= stop_voltage)
      {  /*search for GTH tranistion*/
        if (gt == HI)
          vth_mmbd_rd_arcv1(mod_id);
        else
          vth_mmbd_rd_arcv0(mod_id);

        if (!get_result_nolog(mod_id, &result))
        {  /*process this result*/
          display_prompt("No result in result log (Expected ARCV)", true);
          continue;
        }  /*process this result*/
        if (result.UU.Integer_result != search_level)
        {  /*search_level is found*/
          probe_voltage -= voltage_increment;
          vth_asrc_set_dc_ampl(mod_id, probe_voltage);
          vth_mtmr_wait(mod_id, TIME_OUT);
          continue;
        }  /*search_level is found*/
        search_level_found = true;
        if (voltage_increment <= FINEST_INCREMENT)
        {  /*decrease the increment over a limited range*/
          continue;
        }  
        search_level_found = false;
        vth_asrc_set_dc_ampl(mod_id, no_trip_voltage);
        vth_mtmr_wait(mod_id, TIME_OUT);
        probe_voltage += voltage_increment;
        voltage_increment /= 2;
        vth_asrc_set_dc_ampl(mod_id, probe_voltage);
        vth_mtmr_wait(mod_id, TIME_OUT);
      }  

      if (search_level_found)
      {  /*MEASURE THE VOLTAGE AT THE PROBE TIP*/
        probe_result[gt][search_level] = measure_dmm(mod_id);
        if (get_result_log(mod_id, &result))
          result.UU.Real_result;
        else
          display_prompt("No result in result log. Expected DMM reading", true);

      }  
      else
      {  /*IT NEVER RECEIVED*/
        probe_result[gt][search_level] = min_voltage;
        
        result.UU.Real_result = probe_result[gt][search_level];
        log_result(mod_id, result);
      }  

    }  /* SEARCH GTH,GTL FOR FIRST LOW WHILE DECREASING THROUGH THRESHOLD */

    for (gt = HI; gt <= LO; ++gt)
    {  /* SEARCH GTH,GTL FOR FIRST HIGH WHILE INCREASING THROUGH THRESHOLD */
      search_level = 1;
      search_level_found = false;
      voltage_increment = STARTING_STEP_SIZE;
      probe_voltage = min_voltage;
      no_trip_voltage = min_voltage;
      vth_asrc_set_dc_ampl(mod_id, probe_voltage);
      stop_voltage = max_voltage;

      while (!search_level_found && probe_voltage <= stop_voltage)
      {  /*search for GTH tranistion*/
        if (gt == HI)
          vth_mmbd_rd_arcv1(mod_id);
        else
          vth_mmbd_rd_arcv0(mod_id);
        if (!get_result_nolog(mod_id, &result))
        {  /*process this result*/
          display_prompt("No result in result log (Expected ARCV)", true);
          continue;
        }  /*process this result*/
        if (result.UU.Integer_result != search_level)
        {  /*search_level is found*/
          probe_voltage += voltage_increment;
          vth_asrc_set_dc_ampl(mod_id, probe_voltage);
          vth_mtmr_wait(mod_id, TIME_OUT);
          continue;
        }  /*search_level is found*/
        search_level_found = true;
        if (voltage_increment <= FINEST_INCREMENT)
        {  /*decrease the increment over a limited range*/
          continue;
        }  /*decrease the increment over a limited range*/
        search_level_found = false;
        vth_asrc_set_dc_ampl(mod_id, no_trip_voltage);
        vth_mtmr_wait(mod_id, TIME_OUT);
        probe_voltage -= voltage_increment;
        voltage_increment /= 2;
        vth_asrc_set_dc_ampl(mod_id, probe_voltage);
        vth_mtmr_wait(mod_id, TIME_OUT);
      }  /*search for probe receive*/

      if (search_level_found)
      {  /*MEASURE THE VOLTAGE AT THE PROBE TIP*/
        measure_dmm(mod_id);
        if (get_result_log(mod_id, &result))
          probe_result[gt][search_level] = result.UU.Real_result;
        else
          display_prompt("No result in result log. Expected DMM reading",
                         true);
      }  /*MEASURE THE VOLTAGE AT THE PROBE TIP*/
      else
      {  /*IT NEVER RECEIVED*/
        probe_result[gt][search_level] = max_voltage;
        result.Measurement_class = CLASS_REAL;
        result.UU.Real_result = probe_result[gt][search_level];
        log_result(mod_id, result);
      }  /*IT NEVER RECEIVED*/

    }  /*search for decreasing threshold, then increasing threshold*/

    for (gt = HI; gt <= LO; gt = (int)(gt + 1))
    {  /* calculate skew */
      skew = probe_result[gt][1] - probe_result[gt][0];
      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = skew;
      log_result(mod_id, result);
    }  /* calculate skew */

    vth_ainit(mod_id);
    vth_minit(mod_id);
    close_dmm(mod_id);
    close_mux(mod_id);
    CloseResourceManager();
  }  /* Do the test */

}  /* probe_voltage_accuracy */

#undef PROBE_NUMBER
#undef RANGE
#undef RESOLUTION
#undef STARTING_STEP_SIZE
#undef FINEST_INCREMENT
#undef DMM_FUNCTION
#undef HI
#undef LO


/******************************************************************************************
* Aim card 
*******************************************************************************************/


#define NUM_POINTS_TO_TEST  5
#define CHANNEL         'A'
#define RESOLUTION      1.0e-03

#define ENABLE_WAIT_TIME  1.0e-03
#define DMM_FUNCTION    DCV
typedef double voltage_array_____[NUM_POINTS_TO_TEST];
const voltage_array_____ TEST__ = {-10e00, -5e00, 0.0e00, 5e00, 10e00};

void t16305(int mod_id, int /*slot*/)//9305
{
  /* AUX SOURCE VOLTAGE ACCURACY AND RANGE */
  test_result_type result;
  result.Measurement_class = CLASS_REAL;
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  connect_dmm(mod_id);
  connect_plus_xn(mod_id, CHANNEL, true);
  connect_minus_calib2(mod_id);
  aconnect_auxsrc_fxt(mod_id);
  vth_aaux_en(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  for (int i = 0; i < NUM_POINTS_TO_TEST; i++)
  {  /* for voltage */
    setup_dmm(mod_id, DMM_FUNCTION, TEST__[i], RESOLUTION);
    vth_aaux_set_ampl(mod_id, TEST__[i]);
	flush_commands(mod_id);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    result.UU.Real_result = measure_dmm(mod_id);
    log_result(mod_id, result);
  }  /* for voltage */
  vth_aaux_dis(mod_id);
  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /* t16305 */

#undef NUM_POINTS_TO_TEST
#undef CHANNEL
#undef RESOLUTION

#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION



#define HI_END          0
#define LO_END          1
#define RESOLUTION      1.0e-03

#define ENABLE_WAIT_TIME  1.0e-03
#define CHANNEL         'A'
#define DMM_FUNCTION    DCV
typedef double voltage_array______[2];
typedef int headroom_array_[2];
const voltage_array______ TEST___ = {10e00, -10e00};
const headroom_array_ HEADROOM_ = {4095, 0};

void t16306(int mod_id, int /*slot*/)//9306
{
  /* AUX SOURCE VOLTAGE HEADROOM TEST */
  test_result_type result;
  result.Measurement_class = CLASS_REAL;
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  connect_dmm(mod_id);
  connect_plus_xn(mod_id, CHANNEL, true);
  connect_minus_calib2(mod_id);
  aconnect_auxsrc_fxt(mod_id);
  vth_aaux_en(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  for (int src_30ma_ilim = 0; src_30ma_ilim <= 1; ++src_30ma_ilim)
  {
    if (src_30ma_ilim == 1)
      vth_aaux_en_30ma_ilim(mod_id);
    else
      vth_aaux_dis_30ma_ilim(mod_id);

    for (int i = HI_END; i <= LO_END; ++i)
    {  /* for i */
      setup_dmm(mod_id, DMM_FUNCTION, TEST___[i], RESOLUTION);
      vth_aaux_set_ampl(mod_id, TEST___[i]);
      vth_awr_cell(mod_id, ADAC_AUX, HEADROOM_[i]);
	  flush_commands(mod_id);
      vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
      result.UU.Real_result = measure_dmm(mod_id);
      log_result(mod_id, result);
    }  /* for voltage */
  }
  vth_aaux_dis(mod_id);
  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /* t16306 */

#undef HI_END
#undef LO_END
#undef RESOLUTION

#undef ENABLE_WAIT_TIME
#undef CHANNEL
#undef DMM_FUNCTION



#define CHANNEL         'A'
#define AUX_DEFAULT_RESISTANCE  0
#define FEEDBACK_RESISTOR_NO  0
#define MEAS_TYPE       0
#define POSITIVE_TEST_VOLTAGE  100e-03
#define NEGATIVE_TEST_VOLTAGE  (-100e-03)
#define RESOLUTION      1.0e-03

#define ENABLE_WAIT_TIME  1.0e-03
#define DMM_FUNCTION    DCV

void t16310(int mod_id, int /*slot*/)//9310
{
  /* AUX SOURCE OUTPUT RESISTANCE */
  double r10;
  double dmm_no_load_voltage;
  double dmm_loaded_voltage;
  double source_resistance;
  test_result_type result;
  result.Measurement_class = CLASS_REAL;

  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  setup_dmm(mod_id, DMM_FUNCTION, POSITIVE_TEST_VOLTAGE, RESOLUTION);
  connect_dmm(mod_id);
  connect_plus_xn(mod_id, CHANNEL, true);
  connect_minus_calib2(mod_id);
  aconnect_auxsrc_fxt(mod_id);
  
  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);
  vth_aaux_en(mod_id);
  vth_aaux_set_ampl(mod_id, POSITIVE_TEST_VOLTAGE);
  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
  dmm_no_load_voltage = measure_dmm(mod_id);
  vth_aaux_dis(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  vth_aclose_rly(mod_id, K822);
  vth_aclose_rly(mod_id, K828);
  vth_aclose_rly(mod_id, K845);
  vth_aclose_rly(mod_id, K855);

  vth_aaux_en(mod_id);
  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  dmm_loaded_voltage = measure_dmm(mod_id);
  if (dmm_loaded_voltage != 0)
    source_resistance =
       (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
  else
    source_resistance = AUX_DEFAULT_RESISTANCE;

  result.UU.Real_result = source_resistance;
  log_result(mod_id, result);
  vth_aaux_set_ampl(mod_id, NEGATIVE_TEST_VOLTAGE);
  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
  dmm_loaded_voltage = measure_dmm(mod_id);
  vth_aaux_dis(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  vth_aopen_rly(mod_id, K822);
  vth_aopen_rly(mod_id, K828);
  vth_aopen_rly(mod_id, K845);
  vth_aopen_rly(mod_id, K855);
  vth_aaux_en(mod_id);
  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  dmm_no_load_voltage = measure_dmm(mod_id);
  if (dmm_loaded_voltage != 0)
    source_resistance = (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
  else
    source_resistance = AUX_DEFAULT_RESISTANCE;

  result.UU.Real_result = source_resistance;
  log_result(mod_id, result);
  vth_aaux_dis(mod_id);
  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /* t16310 */

#undef CHANNEL
#undef AUX_DEFAULT_RESISTANCE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef POSITIVE_TEST_VOLTAGE
#undef NEGATIVE_TEST_VOLTAGE
#undef RESOLUTION

#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION



#define MIN_TEST_VOLTAGE  10.0e+00
#define MAX_TEST_VOLTAGE  20.0e+00
#define STARTING_INCREMENT  1024e-03
#define FINEST_INCREMENT  8e-03
#define SYN_WAIT_TIME   10e-03
#define RESOLUTION      1e-03
#define N6700B_CH1_CURR 500e-03

void t16315(int mod_id, int /*slot*/)//9315
{
  /* AUX SOURCE POSITIVE OVER VOLTAGE LIMIT ACCURACY */
  int over_voltage_bit;
  double measurement = 0;
  double syn_voltage;
  double n6700b_ch1_voltage;
  double voltage_increment;
  bool v_limit;
  bool ever_had_a_vlimit;

  a_word_type this_word;
  test_result_type result;

  OpenResourceManager();
  open_syn(mod_id);
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  if(check_syn_model())
  {
  open_n6700b(mod_id); 
  connect_n6700b_ch1(mod_id); 
  aconnect_n6700b_ch1_abus1_xmint(mod_id);
	  
  vth_aaux_dis(mod_id);
  n6700b_ch1_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = N6700B_CH1_STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  aconnect_auxout_synth(mod_id);
  setup_n6700b_ch1_volt(mod_id, n6700b_ch1_voltage); 
  setup_n6700b_ch1_curr(mod_id, N6700B_CH1_CURR); 
  enable_n6700b_ch1_out(mod_id);
  vth_mtmr_wait(mod_id, N6700B_CH1_WAIT_TIME);
  vth_astat_clr(mod_id);

  while (!v_limit && n6700b_ch1_voltage <= MAX_TEST_VOLTAGE)
  {  /*search for limit level*/
    vth_astat_rd(mod_id);
    if (!get_result_nolog(mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0);
    if (over_voltage_bit == 1)
    {  
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= N6700B_CH1_FINEST_INCREMENT || n6700b_ch1_voltage == MIN_TEST_VOLTAGE)
         continue;
      v_limit = false;
      disable_n6700b_ch1_out(mod_id);
      vth_astat_clr(mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      aconnect_auxout_synth(mod_id);
      voltage_increment /= 2;
      n6700b_ch1_voltage -= voltage_increment;
      setup_n6700b_ch1_volt(mod_id, n6700b_ch1_voltage);
	  enable_n6700b_ch1_out(mod_id);
      vth_mtmr_wait(mod_id, N6700B_CH1_WAIT_TIME);
      continue;
    } 
    if( ever_had_a_vlimit && !number_equal(voltage_increment, N6700B_CH1_FINEST_INCREMENT) )
      voltage_increment /= 2;
    n6700b_ch1_voltage += voltage_increment;
    setup_n6700b_ch1_volt(mod_id, n6700b_ch1_voltage);
    vth_mtmr_wait(mod_id, N6700B_CH1_WAIT_TIME);
  }  /*search for limit level*/

  //MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE 
  setup_dmm(mod_id, DCV, n6700b_ch1_voltage, RESOLUTION);
  connect_dmm(mod_id);
  aconnect_n6700b_ch1_out_dmm(mod_id, true);
  measurement = measure_dmm(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measurement;
  log_result(mod_id, result);

  vth_ainit(mod_id);
  close_syn(mod_id);
  close_n6700b(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
  }

  else
  {
  setup_syn_hv(mod_id, true);
  aconnect_syn_out_xmint(mod_id);
  vth_aaux_dis(mod_id);
  syn_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  aconnect_auxout_synth(mod_id);
  setup_syn_off(mod_id, syn_voltage);
  vth_mtmr_wait(mod_id, SYN_WAIT_TIME);
  vth_astat_clr(mod_id);

  while (!v_limit && syn_voltage <= MAX_TEST_VOLTAGE)
  {  /*search for limit level*/
    vth_astat_rd(mod_id);
    if (!get_result_nolog(mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0);
    if (over_voltage_bit == 1)
    {  
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT || syn_voltage == MIN_TEST_VOLTAGE)
         continue;
      v_limit = false;
      setup_syn_off(mod_id, 0.0);
      vth_astat_clr(mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      aconnect_auxout_synth(mod_id);
      voltage_increment /= 2;
      syn_voltage -= voltage_increment;

      setup_syn_off(mod_id, syn_voltage);
      vth_mtmr_wait(mod_id, SYN_WAIT_TIME);
      continue;
    } 
    if( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    syn_voltage += voltage_increment;
    setup_syn_off(mod_id, syn_voltage);
    vth_mtmr_wait(mod_id, SYN_WAIT_TIME);
  }  /*search for limit level*/

  //MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE 
  setup_dmm(mod_id, DCV, syn_voltage, RESOLUTION);
  connect_dmm(mod_id);
  aconnect_syn_out_dmm(mod_id, true);
  measurement = measure_dmm(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measurement;
  log_result(mod_id, result);

  vth_ainit(mod_id);
  close_syn(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
  }
}  /* t16315 */

#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef SYN_WAIT_TIME
#undef RESOLUTION   
#undef N6700B_CH1_CURR 



#define MIN_TEST_VOLTAGE  (-10.0e+00)
#define MAX_TEST_VOLTAGE  (-20.0e+00)
#define STARTING_INCREMENT  1024e-03
#define FINEST_INCREMENT  8e-03
#define SYN_WAIT_TIME   10e-03
#define RESOLUTION      1e-03
#define N6700B_CH1_CURR 500e-3
#define N6700B_CH1_MIN_TEST_VOLTAGE  (10.0e+00) 
#define N6700B_CH1_MAX_TEST_VOLTAGE  (20.0e+00) 

void t16316(int mod_id, int /*slot*/)//9316
{
  /* AUX SOURCE NEGATIVE OVER VOLTAGE LIMIT ACCURACY */
  int over_voltage_bit;
  double measurement;
  double syn_voltage;
  double n6700b_ch1_voltage;
  double voltage_increment;
  bool v_limit;
  bool ever_had_a_vlimit;

  a_word_type this_word;
  test_result_type result;

  OpenResourceManager();
  open_syn(mod_id);
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  if(check_syn_model())
  {
  open_n6700b(mod_id); 
  connect_n6700b_ch1(mod_id); 
  aconnect_n6700b_ch1_abus1_xmint(mod_id);

  vth_aaux_dis(mod_id);
  n6700b_ch1_voltage = N6700B_CH1_MIN_TEST_VOLTAGE;
  voltage_increment = N6700B_CH1_STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  aconnect_auxout_synth(mod_id);
  setup_n6700b_ch1_volt(mod_id, n6700b_ch1_voltage); 
  setup_n6700b_ch1_curr(mod_id, N6700B_CH1_CURR);
  reverse_n6700b_ch1_out(mod_id);
  enable_n6700b_ch1_out(mod_id);
  vth_mtmr_wait(mod_id, N6700B_CH1_WAIT_TIME);
  vth_astat_clr(mod_id);

  while (!v_limit && n6700b_ch1_voltage <= N6700B_CH1_MAX_TEST_VOLTAGE)
  {  /*search for limit level*/
    vth_astat_rd(mod_id);
    if (!get_result_nolog(mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= N6700B_CH1_FINEST_INCREMENT ||
          n6700b_ch1_voltage == N6700B_CH1_MIN_TEST_VOLTAGE)
        continue;
      v_limit = false;
      disable_n6700b_ch1_out(mod_id);
      vth_astat_clr(mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      aconnect_auxout_synth(mod_id);
      voltage_increment /= 2;
      n6700b_ch1_voltage -= voltage_increment;
      setup_n6700b_ch1_volt(mod_id, n6700b_ch1_voltage);
	  enable_n6700b_ch1_out(mod_id);
      vth_mtmr_wait(mod_id, N6700B_CH1_WAIT_TIME);
      continue;
    }
	if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
      n6700b_ch1_voltage += voltage_increment;
      setup_n6700b_ch1_volt(mod_id, n6700b_ch1_voltage);
      vth_mtmr_wait(mod_id, N6700B_CH1_WAIT_TIME);
  }  /*search for limit level*/

  //MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE 
  setup_dmm(mod_id, DCV, n6700b_ch1_voltage, RESOLUTION);
  connect_dmm(mod_id);
  aconnect_n6700b_ch1_out_dmm(mod_id, true);
  measurement = measure_dmm(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measurement;
  log_result(mod_id, result);

  vth_ainit(mod_id);
  close_syn(mod_id);
  close_n6700b(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
  }

  else
  {
  setup_syn_hv(mod_id, true);
  aconnect_syn_out_xmint(mod_id);
  vth_aaux_dis(mod_id);
  syn_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  aconnect_auxout_synth(mod_id);
  setup_syn_off(mod_id, syn_voltage);
  vth_mtmr_wait(mod_id, SYN_WAIT_TIME);
  vth_astat_clr(mod_id);

  while (!v_limit && syn_voltage >= MAX_TEST_VOLTAGE)
  {  /*search for limit level*/
    vth_astat_rd(mod_id);
    if (!get_result_nolog(mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT ||
          syn_voltage == MIN_TEST_VOLTAGE)
        continue;
      v_limit = false;
      setup_syn_off(mod_id, 0.0);
      vth_astat_clr(mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      aconnect_auxout_synth(mod_id);
      voltage_increment /= 2;
      syn_voltage += voltage_increment;
      setup_syn_off(mod_id, syn_voltage);
      vth_mtmr_wait(mod_id, SYN_WAIT_TIME);
      continue;
    }
	if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    syn_voltage -= voltage_increment;
    setup_syn_off(mod_id, syn_voltage);
    vth_mtmr_wait(mod_id, SYN_WAIT_TIME);
  }  /*search for limit level*/

  //MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE 
  setup_dmm(mod_id, DCV, syn_voltage, RESOLUTION);
  connect_dmm(mod_id);
  aconnect_syn_out_dmm(mod_id, true);
  measurement = measure_dmm(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measurement;
  log_result(mod_id, result);

  vth_ainit(mod_id);
  close_syn(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
  }
}  /* t16316 */

#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef SYN_WAIT_TIME
#undef RESOLUTION
#undef N6700B_CH1_CURR 
#undef N6700B_CH1_MIN_TEST_VOLTAGE 
#undef N6700B_CH1_MAX_TEST_VOLTAGE

/* AUX SOURCE POSITIVE OVER VOLTAGE PROTECTION */

void aux_source_ov(int mod_id, int test_number)//9317 & 9318
{
  int over_voltage_bit;
  int i;
  int fatal_error_bit;
  int num_samples = 1000;
  int num_samples2 = 5;
  double range = 10; 
  double average = 0;
  double resolution = 1E-3;
  double internal_src_volts;
  double external_src_volts;
  double n6700b_ch1_init_voltage = 10.0;
  double n6700b_ch1_curr = 500e-03;
  double waveform2[5];
  arrform waveform;
  a_word_type this_word;
  test_result_type result;

  OpenResourceManager();
  open_dmm50(mod_id);
  open_syn(mod_id);
  open_mux(mod_id);
  vth_mod_init(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  if(check_syn_model())
  {
  open_n6700b(mod_id);
  connect_n6700b_ch1(mod_id);
  if(test_number == 16317)
	  internal_src_volts = 10.0;
  else
	  internal_src_volts = -10.0;

  external_src_volts = 20.0;
  vth_aaux_dis(mod_id);
  aconnect_n6700b_ch1_abus1_xmint(mod_id);
  connect_dmm50_out_af2(mod_id);
  //setup_dmm50_norm(mod_id);//not required sri 17th Jan 2007

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
  vth_aopen_rly(mod_id, K2306);	  // Isolate detector

  // close the synthesizer to ground path relays
  vth_aclose_rly(mod_id, K742);   // X2MINT to X2 bus relay
  vth_aclose_rly(mod_id, K718);   // X2 bus to L bus relay
  vth_aclose_rly(mod_id, K857);   // L bus to GROUND path relay

  /* Since the synthesizer cannot be tri-stated, set it to 10v before setting
     up the AUX source on the ASRU card.  */
  setup_n6700b_ch1_volt(mod_id, n6700b_ch1_init_voltage);
  setup_n6700b_ch1_curr(mod_id, n6700b_ch1_curr);
  if(test_number == 16318)
	  reverse_n6700b_ch1_out(mod_id);
  enable_n6700b_ch1_out(mod_id);
  vth_mtmr_wait(mod_id, N6700B_CH1_WAIT_TIME);
  vth_aaux_en_30ma_ilim(mod_id);
  vth_aaux_set_ampl(mod_id, internal_src_volts);
  vth_aaux_en(mod_id);
  setup_n6700b_ch1_volt(mod_id, external_src_volts) ; 
  vth_mtmr_wait(mod_id, N6700B_CH1_WAIT_TIME);
  flush_commands(mod_id);

  vth_astat_rd(mod_id); 
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0); // bit 9 of the word is OV flag bit
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = over_voltage_bit;
    log_result(mod_id, result);	//sub 0
  }
  else
    display_prompt("No result in result log (Expected Aim status)", true);
  vth_mrd_cell(mod_id, MMFP_GPIO);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    fatal_error_bit = P_getbits_US(this_word.bits, 6, 0); // bit 6 indicates if fatal error set
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = fatal_error_bit;
    log_result(mod_id, result);  // sub 1
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);


  setup_dmm50_norm(mod_id);
  setup_dmm50(mod_id,ACV, range, resolution);
  vth_mtmr_wait(mod_id, WAIT_TIME);
  setup_dmm50(mod_id, DCV, range, resolution);
  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, TIME_OUT);
  for (i = num_samples - 5; i < num_samples; i++)
  {
	waveform[i] = measure_dmm50(mod_id);
    average += waveform[i];
  }
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
   
  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, WAIT_TIME);

  for (i = 0; i < num_samples2; i++)
  {
    waveform2[i] = measure_dmm50(mod_id);
    average += waveform2[i];
  }
  average = average/num_samples2;
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result); //sub 3

  vth_aaux_dis(mod_id);
  vth_mod_init(mod_id);
  close_dmm50(mod_id);
  close_syn(mod_id);
  close_n6700b(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
  }

  else
  {
  if(test_number == 16317)
  {
    internal_src_volts = 10.0;
	external_src_volts = 20.0;
	//trig_level = 11;
	//slope = POS;
	//stats = MAX;
  }
  else
  {
    internal_src_volts = -10.0;
	external_src_volts = -20.0;
	//trig_level = -11;
	//slope = NEG;
	//stats = MIN;
  }

  setup_syn_hv(mod_id, true);
  vth_aaux_dis(mod_id);
  aconnect_syn_out_xmint(mod_id);
  connect_dmm50_out_af2(mod_id);
  //setup_dmm50_norm(mod_id);//not required sri 17th Jan 2007

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
  vth_aopen_rly(mod_id, K2306);	  // Isolate detector

  // close the synthesizer to ground path relays
  vth_aclose_rly(mod_id, K742);   // X2MINT to X2 bus relay
  vth_aclose_rly(mod_id, K718);   // X2 bus to L bus relay
  vth_aclose_rly(mod_id, K857);   // L bus to GROUND path relay

  /* Since the synthesizer cannot be tri-stated, set it to 10v before setting
     up the AUX source on the ASRU card.  */
  setup_syn(mod_id, DC, 0, 0.0, internal_src_volts);
  vth_aaux_en_30ma_ilim(mod_id);
  vth_aaux_set_ampl(mod_id, internal_src_volts);
  vth_aaux_en(mod_id);
  setup_syn_off(mod_id, external_src_volts); 
  vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);
  flush_commands(mod_id);

  vth_astat_rd(mod_id); 
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 9, 0); // bit 9 of the word is OV flag bit
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = over_voltage_bit;
    log_result(mod_id, result);	//sub 0
  }
  else
    display_prompt("No result in result log (Expected Aim status)", true);
  vth_mrd_cell(mod_id, MMFP_GPIO);
  if (get_result_nolog(mod_id, &result))
  {
    this_word.data = result.UU.Integer_result;
    fatal_error_bit = P_getbits_US(this_word.bits, 6, 0); // bit 6 indicates if fatal error set
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = fatal_error_bit;
    log_result(mod_id, result);  // sub 1
  }
  else
    display_prompt("No result in result log (Expected aim status)", true);


  setup_dmm50_norm(mod_id);
  setup_dmm50(mod_id,ACV, range, resolution);
  vth_mtmr_wait(mod_id, WAIT_TIME);
  setup_dmm50(mod_id, DCV, range, resolution);
  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, TIME_OUT);
  for (i = num_samples - 5; i < num_samples; i++)
  {
	waveform[i] = measure_dmm50(mod_id);
    average += waveform[i];
  }
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
   
  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, WAIT_TIME);

  for (i = 0; i < num_samples2; i++)
  {
    waveform2[i] = measure_dmm50(mod_id);
    average += waveform2[i];
  }
  average = average/num_samples2;
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result); //sub 3

  vth_aaux_dis(mod_id);
  vth_mod_init(mod_id);
  close_dmm50(mod_id);
  close_syn(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
  }

}  /* t16317 & t16318 */


/* AUX SOURCE TRI-STATE SPIKE TEST */

#define POSITIVE_SRC_VOLTAGE  5.0e+00
#define NEGATIVE_SRC_VOLTAGE  (-5.0e+00)
#define MEASURE_WAIT_TIME  2.0e+00

void t16320(int mod_id, int /*slot*/)//9320
{
  double min_volt;
  double max_volt;
  test_result_type result;

  OpenResourceManager();
  open_cntr(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  setup_cntr_defaults(mod_id);
  setup_cntr_input(mod_id, DC, 50);
  aconnect_cntr_ch1_af2(mod_id); 

  /* CLOSE THE AF2 MEASUREMENT PATH RELAYS */
  vth_aclose_rly(mod_id, K823);   /* AUXOUT to A bus relay */
  vth_aclose_rly(mod_id, K709);   /* A bus to X1 bus relay */
  vth_aclose_rly(mod_id, K717);   /* X1 bus to L bus relay */
  vth_aclose_rly(mod_id, K754);   /* L bus to A_XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* A_XL bus to AF2 relay */

  vth_aaux_dis_30ma_ilim(mod_id);
  vth_aaux_set_ampl(mod_id, POSITIVE_SRC_VOLTAGE);
  setup_cntr_trigger_min_max(mod_id, MAX);
  vth_aaux_en(mod_id);   /* TAKES THE SRC OUT OF TRI-STATE */
  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, MEASURE_WAIT_TIME);
  trigger_cntr(mod_id);
  min_volt = read_cntr(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = min_volt - 5;
  log_result(mod_id, result);
  vth_aaux_dis(mod_id);
  vth_aaux_set_ampl(mod_id, NEGATIVE_SRC_VOLTAGE);

  setup_cntr_trigger_min_max(mod_id, MIN);
  vth_aaux_en(mod_id);   /* TAKES THE SRC OUT OF TRI-STATE */
  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, MEASURE_WAIT_TIME);
  trigger_cntr(mod_id);
  max_volt = read_cntr(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = max_volt + 5;
  log_result(mod_id, result);
  vth_aaux_dis(mod_id);
  vth_ainit(mod_id);
  close_cntr(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /* t16320 */

#undef POSITIVE_SRC_VOLTAGE
#undef NEGATIVE_SRC_VOLTAGE
#undef MEASURE_WAIT_TIME


/* AUX SOURCE OUTPUT CURRENT TEST */

#define LOW             0
#define HIGH            1
#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1
#define FEEDBACK_RESISTOR_NO  0
#define MEAS_TYPE       1
#define LENGTH          1
#define AVERAGES        10
#define DIGITAL         0
#define R625            4
#define R2500           6
#define R10000          8
#define RINIT           4.0e+00
#define RSINIT          0.6e+00
#define VINIT           1.01e+00
#define EXPECTED_DET_VOLTS  2.0e+00
#define TIMEOUT         10.0e+00
#define PS_CURRENT      800e-03
#define DIFF            5e-03
#define RAMP_TIME       30e+00
#define LOW_CURRENT_VALUE  27.1e-03//27.5e-03//6634B Sri 20/1/07
#define HIGH_CURRENT_VALUE  162e-03//165e-03 //6634B Sri 20/1/07
#define HIGH_CURRENT_VALUE_SINKING 160E-03//160E-03 //6634B Sri 20/1/07
typedef double source_voltage_array_[2];
const source_voltage_array_ SOURCE_VOLTAGE___ = {7.2e00, -7.2e00};//{7e00, -7e00};//SRI 6634B ACORN

void t16325(int mod_id, int /*slot*/)//9325
{
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
  OpenResourceManager();
  open_mux(mod_id);
  open_ps(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);
  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
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
  source_current = -1 * LOW_CURRENT_VALUE;   
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
        vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);   
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K725);
        vth_aclose_rly(mod_id, K718);
      }  
      else
      {  /* CONNECT PS + TO X2 AND PS - TO X1 */
        vth_aopen_rly(mod_id, K725);
        vth_aopen_rly(mod_id, K718);
        vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);   
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K726);
        vth_aclose_rly(mod_id, K717);
      }  

      for (int direction = SOURCING; direction <= SINKING; ++direction)
      {  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */
        if (direction == SOURCING)
        {
          if (current == LOW)
            source_current = -1 * LOW_CURRENT_VALUE;   
          else
            source_current = -1 * HIGH_CURRENT_VALUE;   
        }
        else
        {  /* direction = sinking */
          if (current == LOW)
            source_current = LOW_CURRENT_VALUE;   
          else
		  {
            source_current = HIGH_CURRENT_VALUE_SINKING;   
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
  CloseResourceManager();
}  /* t16325 */

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


/* AUX SOURCE MAXIMUM CURRENT (HIGH LEVEL ONLY) */

#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1
#define FEEDBACK_RESISTOR_NO  0
#define MEAS_TYPE       1
#define LENGTH          1
#define AVERAGES        50
#define DIGITAL         0
#define TIMEOUT         10.0e+00
#define POWER_SUPPLY_CURRENT  800e-03
#define POWER_SUPPLY_RAMP_TIME  1.0e+00
#define MAX_EXPECTED_VOLTS  2.1e+00
typedef double i_direction_array___[2];
typedef i_direction_array___ voltage_array________[2];
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

void t16327(int mod_id, int /*slot*/)//9327
{
  double r10;
  double max_current;
  test_result_type result;
  vth_apowered_init(mod_id);
  OpenResourceManager();  
  open_ps(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);
  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);
  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
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
      vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);   
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K725);
      vth_aclose_rly(mod_id, K718);
    }  
    else
    {  /* CONNECT PS + TO X2 AND PS - TO X1 */
      vth_aopen_rly(mod_id, K725);
      vth_aopen_rly(mod_id, K718);
      vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);   
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K726);
      vth_aclose_rly(mod_id, K717);
    }  

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
  CloseResourceManager();
}  /* t16327 */

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


/*--------------------------------------------------------------------------------
* Test: 16330
* Description: DC source voltage accuracy and range
*				
* Fixture: Analog
* Resource(s): DMM
*--------------------------------------------------------------------------------
*/

#define NUM_POINTS_TO_TEST  9 
#define RESOLUTION      1.0e-03

#define ENABLE_WAIT_TIME  1.0e-03
#define DMM_FUNCTION    DCV

void t16330(int mod_id, int slot)//9255
{
  int voltage;
  int range[NUM_POINTS_TO_TEST];
  double TEST[NUM_POINTS_TO_TEST] = {-10e00, -5e00, -0.2e00, -0.1e00, 0.0e00, 0.1e00, 0.2e00, 5e00, 10e00};  
  
  a_word_type this_word;
  test_result_type result;
  result.Measurement_class = CLASS_REAL;
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);
  vth_afc_en_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);

  for (voltage = 0; voltage < NUM_POINTS_TO_TEST; voltage++)
  {  /* for voltage */
    setup_dmm(mod_id, DMM_FUNCTION, TEST[voltage], RESOLUTION);
    vth_asrc_set_dc_ampl(mod_id, TEST[voltage]);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
	flush_commands(mod_id);
    measured = measure_dmm(mod_id);  
	result.Measurement_class = CLASS_REAL;
	result.UU.Real_result = measured;
	log_result(mod_id, result);
    vth_ard_cell(mod_id, ARLY_2A);	// READ RANGE BIT TO VERIFY THE DC SOURCE OPERATES IN THE CORRECT RANGE 
    if (get_result_nolog(mod_id, &result))
    {
      this_word.data = result.UU.Integer_result;
      range[voltage] = P_getbits_US(this_word.bits, 1, 0);
    }  
    else
      display_prompt("No result in result log (Expected asrc range)", true);
  }  /* for voltage */

  for (voltage = 0; voltage < NUM_POINTS_TO_TEST; voltage++)
  {  
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = range[voltage];
    log_result(mod_id, result);
  }  

  vth_afc_dis_src_out(mod_id);
  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();

}  /* t16330 */

#undef NUM_POINTS_TO_TEST
#undef RESOLUTION

#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION


/*--------------------------------------------------------------------------------
* Test: 16331
* Description: DC source voltage headroom test
*				
* Fixture: Analog
* Resource(s): DMM
*--------------------------------------------------------------------------------
*/

#define HI_END          0
#define LO_END          1
#define RESOLUTION      1.0e-03

#define ENABLE_WAIT_TIME  1.0e-03
#define DMM_FUNCTION    DCV

void t16331(int mod_id, int slot)//9256
{
  double TEST_[2] = {10e00, -10e00};
  int HEADROOM[2] = {4095, 0};
  test_result_type result;
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);
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
      vth_asrc_set_dc_ampl(mod_id, TEST_[voltage]);
      vth_awr_cell(mod_id, ADAC_SRC_OFST, HEADROOM[voltage]);
      vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
	  flush_commands(mod_id);
      measured = measure_dmm(mod_id); 
	  result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = measured;
	  log_result(mod_id, result);
    }  /* for voltage */
  }

  vth_afc_dis_src_out(mod_id);
  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /* t16331 */

#undef HI_END
#undef LO_END
#undef RESOLUTION

#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION


/*--------------------------------------------------------------------------------
* Test: 16332
* Description: DC source output resistance
*				
* Fixture: Analog
* Resource(s): DMM
*--------------------------------------------------------------------------------
*/

#define FEEDBACK_RESISTOR_NO  0
#define MEAS_TYPE       0
#define DEFAULT_SOURCE_RESISTANCE  0
#define POSITIVE_TEST_VOLTAGE  100e-03
#define NEGATIVE_TEST_VOLTAGE  (-100e-03)
#define RESOLUTION      1.0e-03

#define ENABLE_WAIT_TIME  1.0e-03
#define DMM_FUNCTION    DCV

void t16332(int mod_id, int slot)//9260
{
  double r10;
  double dmm_no_load_voltage;
  double dmm_loaded_voltage;
  double source_resistance;
  test_result_type result;
  result.Measurement_class = CLASS_REAL;
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);
  setup_dmm(mod_id, DMM_FUNCTION, POSITIVE_TEST_VOLTAGE, RESOLUTION);
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);
  vth_afc_en_src_out(mod_id);
  vth_asrc_set_dc_ampl(mod_id, POSITIVE_TEST_VOLTAGE);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
  flush_commands(mod_id);
  dmm_no_load_voltage = measure_dmm(mod_id);
  vth_afc_dis_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  vth_aclose_rly(mod_id, K826);
  vth_aclose_rly(mod_id, K845);
  vth_aclose_rly(mod_id, K855);
  vth_afc_en_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  flush_commands(mod_id);
  dmm_loaded_voltage = measure_dmm(mod_id); 
  //if (dmm_loaded_voltage != 0)
    source_resistance = (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
  //else
  //{
	//source_resistance = DEFAULT_SOURCE_RESISTANCE;
    result.UU.Real_result = source_resistance;
    log_result(mod_id, result);
  //}

  vth_asrc_set_dc_ampl(mod_id, NEGATIVE_TEST_VOLTAGE);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
  flush_commands(mod_id);
  dmm_loaded_voltage = measure_dmm(mod_id); 
  vth_afc_dis_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  vth_aopen_rly(mod_id, K826);
  vth_aopen_rly(mod_id, K845);
  vth_aopen_rly(mod_id, K855);
  vth_afc_en_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  flush_commands(mod_id);
  dmm_no_load_voltage = measure_dmm(mod_id); 
  //if (dmm_loaded_voltage != 0)
     source_resistance = (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
  //else
    //source_resistance = DEFAULT_SOURCE_RESISTANCE;

    result.UU.Real_result = source_resistance;
    log_result(mod_id, result);

  vth_afc_dis_src_out(mod_id);
  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /* t16332 */

#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef DEFAULT_SOURCE_RESISTANCE
#undef POSITIVE_TEST_VOLTAGE
#undef NEGATIVE_TEST_VOLTAGE
#undef RESOLUTION

#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION

/*--------------------------------------------------------------------------------
* Test: 16333
* Description: Source positive over voltage limit accuracy
*				
* Fixture: Analog
* Resource(s): DMM, Synthesizer
*--------------------------------------------------------------------------------
*/

#define MIN_TEST_VOLTAGE  10.0e+00
#define MAX_TEST_VOLTAGE  20.0e+00
#define STARTING_INCREMENT  1024e-03
#define FINEST_INCREMENT  8e-03
#define SYN_WAIT_TIME   10e-03
#define RESOLUTION      1e-03
#define N6700b_CH1_CURR 500e-03

/* static variables for t16333: */
struct LOC_t16333
{
  int mod_id;
};

static void close_the_relays(struct LOC_t16333 *LINK)
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

void t16333(int mod_id, int slot)//9265
{
  struct LOC_t16333 V;
  int over_voltage_bit;
  double syn_voltage;
  double n6700b_ch1_voltage;
  double voltage_increment;
  bool v_limit;
  bool ever_had_a_vlimit;

  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id;
  OpenResourceManager();
  open_syn(V.mod_id);
  open_dmm(V.mod_id);
  open_mux(V.mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  if(check_syn_model())
  {
  open_n6700b(V.mod_id);
  connect_n6700b_ch1(V.mod_id);
  aconnect_n6700b_ch1_abus1_xmint(V.mod_id);
  flush_commands(mod_id);
  vth_afc_dis_src_out(V.mod_id);
  n6700b_ch1_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = N6700B_CH1_STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  close_the_relays(&V);
  setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
  enable_n6700b_ch1_out(V.mod_id);
  vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
  vth_astat_clr(V.mod_id);

  while (!v_limit && n6700b_ch1_voltage <= MAX_TEST_VOLTAGE)
  {
	vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.data, 8, 0);
    if (over_voltage_bit == 1)
    {  
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= N6700B_CH1_FINEST_INCREMENT || n6700b_ch1_voltage == MIN_TEST_VOLTAGE)
        continue;
      v_limit = false;
      disable_n6700b_ch1_out(V.mod_id);
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays(&V);
      voltage_increment /= 2;
      n6700b_ch1_voltage -= voltage_increment;
      setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
	  enable_n6700b_ch1_out(V.mod_id);
      vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
      continue;
    }
	if ( ever_had_a_vlimit && !number_equal(voltage_increment, N6700B_CH1_FINEST_INCREMENT) )
      voltage_increment /= 2;
    n6700b_ch1_voltage += voltage_increment;
    setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
    vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
  } 
  setup_dmm(V.mod_id, DCV, n6700b_ch1_voltage, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_n6700b_ch1_out_dmm(V.mod_id, true);
  flush_commands(mod_id);
  measured = measure_dmm(V.mod_id); 
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measured;
  log_result(mod_id, result);

  vth_ainit(mod_id);
  close_syn(V.mod_id);
  close_n6700b(V.mod_id);
  close_dmm(V.mod_id);
  close_mux(V.mod_id);
  CloseResourceManager();
  }

  else
  {
  setup_syn_hv(V.mod_id, true);
  aconnect_syn_out_xmint(V.mod_id);
  flush_commands(mod_id);
  vth_afc_dis_src_out(V.mod_id);
  syn_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  close_the_relays(&V);
  setup_syn_off(V.mod_id, syn_voltage);
  vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  vth_astat_clr(V.mod_id);

  while (!v_limit && syn_voltage <= MAX_TEST_VOLTAGE)
  {
	vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.data, 8, 0);
    if (over_voltage_bit == 1)
    {  
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT || syn_voltage == MIN_TEST_VOLTAGE)
        continue;
      v_limit = false;
      setup_syn_off(V.mod_id, 0.0);
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays(&V);
      voltage_increment /= 2;
      syn_voltage -= voltage_increment;
      setup_syn_off(V.mod_id, syn_voltage);
      vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
      continue;
    }
	if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    syn_voltage += voltage_increment;
    setup_syn_off(V.mod_id, syn_voltage);
    vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  } 
  setup_dmm(V.mod_id, DCV, syn_voltage, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_syn_out_dmm(V.mod_id, true);
  flush_commands(mod_id);
  measured = measure_dmm(V.mod_id); 
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measured;
  log_result(mod_id, result);

  vth_ainit(mod_id);
  close_syn(V.mod_id);
  close_dmm(V.mod_id);
  close_mux(V.mod_id);
  CloseResourceManager();
  }
}  /* t16333 */

#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef SYN_WAIT_TIME
#undef RESOLUTION
#undef N6700b_CH1_CURR

/*--------------------------------------------------------------------------------
* Test: 16334
* Description: Source negative over voltage limit accuracy
*				
* Fixture: Analog
* Resource(s): DMM, Synthesizer
*--------------------------------------------------------------------------------
*/

#define MIN_TEST_VOLTAGE  (-10.0e+00)
#define MAX_TEST_VOLTAGE  (-20.0e+00)
#define STARTING_INCREMENT  1024e-03
#define FINEST_INCREMENT  8e-03
#define SYN_WAIT_TIME   10e-03
#define RESOLUTION      1e-03
#define N6700B_CH1_CURR 500e-03
#define N6700B_CH1_MIN_TEST_VOLTAGE  (10.0e+00)
#define N6700B_CH1_MAX_TEST_VOLTAGE  (20.0e+00)

/* static variables for t16334: */
struct LOC_t16334
{
  int mod_id;
};

static void close_the_relays_(struct LOC_t16334 *LINK)
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

void t16334(int mod_id, int slot)//9266
{
  struct LOC_t16334 V;
  int over_voltage_bit;
  double syn_voltage;
  double n6700b_ch1_voltage;
  double voltage_increment;
  bool v_limit;
  bool ever_had_a_vlimit;

  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id;
  OpenResourceManager();
  open_syn(V.mod_id);
  open_dmm(V.mod_id);
  open_mux(V.mod_id);
  connect_dmm(V.mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  flush_commands(mod_id);

  if(check_syn_model())
  {
  open_n6700b(V.mod_id);
  connect_n6700b_ch1(V.mod_id);
  aconnect_n6700b_ch1_abus1_xmint(V.mod_id);
  vth_afc_dis_src_out(V.mod_id);
  n6700b_ch1_voltage = N6700B_CH1_MIN_TEST_VOLTAGE;
  voltage_increment = N6700B_CH1_STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  close_the_relays_(&V);
  setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
  setup_n6700b_ch1_curr(V.mod_id, N6700B_CH1_CURR);
  reverse_n6700b_ch1_out(V.mod_id);
  enable_n6700b_ch1_out(V.mod_id);
  vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
  vth_astat_clr(V.mod_id);
  while (!v_limit && n6700b_ch1_voltage <= N6700B_CH1_MAX_TEST_VOLTAGE)
  {
	vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }

    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 8, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= N6700B_CH1_FINEST_INCREMENT ||
          n6700b_ch1_voltage == N6700B_CH1_MIN_TEST_VOLTAGE)
        continue;
      v_limit = false;
      disable_n6700b_ch1_out(V.mod_id);
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays_(&V);
      voltage_increment /= 2;
      n6700b_ch1_voltage -= voltage_increment;
      setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
	  enable_n6700b_ch1_out(V.mod_id);
      vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
      continue;
    }
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, N6700B_CH1_FINEST_INCREMENT) )
      voltage_increment /= 2;
    n6700b_ch1_voltage += voltage_increment;
    setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
    vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  }  /*search for limit level*/
  setup_dmm(V.mod_id, DCV, n6700b_ch1_voltage, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_n6700b_ch1_out_dmm(V.mod_id, true);
  measured = measure_dmm(V.mod_id); 
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measured;
  log_result(mod_id, result);

  vth_ainit(mod_id);
  close_syn(V.mod_id);
  close_n6700b(V.mod_id);
  close_dmm(V.mod_id);
  close_mux(V.mod_id);
  CloseResourceManager();
  }

  else
  {
  setup_syn_hv(V.mod_id, true);
  aconnect_syn_out_xmint(V.mod_id);
  vth_afc_dis_src_out(V.mod_id);
  syn_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = STARTING_INCREMENT;
  v_limit = false;
  ever_had_a_vlimit = false;
  close_the_relays_(&V);
  setup_syn_off(V.mod_id, syn_voltage);
  vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  vth_astat_clr(V.mod_id);
  while (!v_limit && syn_voltage >= MAX_TEST_VOLTAGE)
  {
	vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      continue;
    }

    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 8, 0);
    if (over_voltage_bit == 1)
    {  /* Go back to the last voltage and reduce the increment */
      v_limit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT ||
          syn_voltage == MIN_TEST_VOLTAGE)
        continue;
      v_limit = false;
      setup_syn_off(V.mod_id, 0.0);
      vth_astat_clr(V.mod_id);   /* MUST BE BEFORE WE CLOSE THE RELAYS */
      close_the_relays_(&V);
      voltage_increment /= 2;
      syn_voltage += voltage_increment;
      setup_syn_off(V.mod_id, syn_voltage);
      vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
      continue;
    }
    if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    syn_voltage -= voltage_increment;
    setup_syn_off(V.mod_id, syn_voltage);
    vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  }  /*search for limit level*/
  setup_dmm(V.mod_id, DCV, syn_voltage, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_syn_out_dmm(V.mod_id, true);
  measured = measure_dmm(V.mod_id); 
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measured;
  log_result(mod_id, result);

  vth_ainit(mod_id);
  close_syn(V.mod_id);
  close_dmm(V.mod_id);
  close_mux(V.mod_id);
  CloseResourceManager();
  }

}  /* t16334 */

#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef SYN_WAIT_TIME
#undef RESOLUTION


/*--------------------------------------------------------------------------------
* Test: 16335 & 16336
* Description: DC source over voltage protection
*				16335: Positive
*				16336: Negative
*				
* Fixture: Analog
* Resource(s): DMM50, Synthesizer
*--------------------------------------------------------------------------------
*/

void dc_src_ov(int mod_id, int test_number)//9267 & 9268
{
  int i;
  int over_voltage_bit;
  int fatal_error_bit;
  int num_samples = 2300;
  int num_samples2 = 5;
  double range = 10; 
  double internal_src_volts;
  double external_src_volts;
  double n6700b_ch1_init_voltage = 10.0;
  double n6700b_ch1_curr = 500e-03;
  double average = 0;
  double resolution = 1E-3;
  double waveform2[5];
  arrform waveform;
  a_word_type this_word;
  test_result_type result;

  vth_mod_init(mod_id);  
  OpenResourceManager();
  open_syn(mod_id);
  open_dmm50(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  
  if(check_syn_model())
  {
  open_n6700b(mod_id);
  connect_n6700b_ch1(mod_id);
  if(test_number == 16335)
	  internal_src_volts = 10.0;
  else
	  internal_src_volts = -10.0;
  external_src_volts = 20.0;
  vth_afc_dis_src_out(mod_id);
  aconnect_n6700b_ch1_abus1_xmint(mod_id);
  connect_dmm50_out_af2(mod_id);
  //setup_dmm50_norm(mod_id);//sri

  /* CLOSE THE AF2 MEASUREMENT PATH RELAYS */
  vth_aclose_rly(mod_id, K818);	  /* SRCOUT to L */
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
  vth_aclose_rly(mod_id, K2305);  /* DETLO path relay */
  vth_aclose_rly(mod_id, K2303);  /* DETLO to GROUND path relay */

  setup_n6700b_ch1_volt(mod_id, n6700b_ch1_init_voltage);
  setup_n6700b_ch1_curr(mod_id, n6700b_ch1_curr);
  if(test_number == 16336)
	  reverse_n6700b_ch1_out(mod_id);
  enable_n6700b_ch1_out(mod_id);
  vth_mtmr_wait(mod_id, N6700B_CH1_WAIT_TIME);
  vth_asrc_en_30ma_ilim(mod_id);
  vth_asrc_set_dc_ampl(mod_id, internal_src_volts);
  vth_afc_en_src_out(mod_id);
  setup_n6700b_ch1_volt(mod_id, external_src_volts);
  vth_mtmr_wait(mod_id, N6700B_CH1_WAIT_TIME);
  flush_commands(mod_id);

  //subtest 0, ensure overvoltage occur
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

  //subtest 1, ensure fatal error detected
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

  //Get dmm50 ready for voltage measurement
  setup_dmm50_norm(mod_id);
  setup_dmm50(mod_id, ACV, range, resolution);
  vth_mtmr_wait(mod_id, WAIT_TIME);
  setup_dmm50(mod_id, DCV, range, resolution);
  vth_mtmr_wait(mod_id, TIME_OUT);
  flush_commands(mod_id);

  //subtest 2, measure voltage after relays open
  for (i = num_samples - 5; i < num_samples; i++)
  {
	waveform[i] = measure_dmm50(mod_id);
    average += waveform[i];
  }
  average = average/num_samples2;
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result); 

  vth_astat_clr(mod_id);
  /* close the AF2 measurement path relays */
  vth_aclose_rly(mod_id, K2702);   /* Source internal relay */
  vth_aclose_rly(mod_id, K818);    /* SRCOUT to L bus relay */
  vth_aclose_rly(mod_id, K754);    /* L bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);    /* XL bus to AF2 relay */

  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, WAIT_TIME);

  //subtest3, measure voltage after re-close relays
  for (i = 0; i < num_samples2; i++)
  {
    waveform2[i] = measure_dmm50(mod_id);
    average += waveform2[i];
  }
  average = average/num_samples2;
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);
  vth_afc_dis_src_out(mod_id);

  vth_ainit(mod_id);
  close_dmm50(mod_id);
  close_syn(mod_id);
  close_n6700b(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
  }

  else
  {
  if(test_number == 16335)
  {
    internal_src_volts = 10.0;
	external_src_volts = 20.0;
	//trig_level = 13;
	//slope = POS;
	//stats = MAX;
  }
  else
  {
    internal_src_volts = -10.0;
	external_src_volts = -20.0;
	//trig_level = -13;
	//slope = NEG;
	//stats = MIN;
  }
  
  setup_syn_hv(mod_id, true);
  vth_afc_dis_src_out(mod_id);
  aconnect_syn_out_xmint(mod_id);
  connect_dmm50_out_af2(mod_id);
  //setup_dmm50_norm(mod_id);//sri

  /* CLOSE THE AF2 MEASUREMENT PATH RELAYS */
  vth_aclose_rly(mod_id, K818);	  /* SRCOUT to L */
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
  vth_aclose_rly(mod_id, K2305);  /* DETLO path relay */
  vth_aclose_rly(mod_id, K2303);  /* DETLO to GROUND path relay */

  setup_syn(mod_id, DC, 0, 0.0, internal_src_volts);
  vth_asrc_en_30ma_ilim(mod_id);
  vth_asrc_set_dc_ampl(mod_id, internal_src_volts);
  vth_afc_en_src_out(mod_id);
  setup_syn_off(mod_id, external_src_volts);
  vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);
  flush_commands(mod_id);

  //subtest 0, ensure overvoltage occur
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

  //subtest 1, ensure fatal error detected
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

  //Get dmm50 ready for voltage measurement
  setup_dmm50_norm(mod_id);
  setup_dmm50(mod_id, ACV, range, resolution);
  vth_mtmr_wait(mod_id, WAIT_TIME);
  setup_dmm50(mod_id, DCV, range, resolution);
  vth_mtmr_wait(mod_id, TIME_OUT);
  flush_commands(mod_id);

  //subtest 2, measure voltage after relays open
  for (i = num_samples - 5; i < num_samples; i++)
  {
	waveform[i] = measure_dmm50(mod_id);
    average += waveform[i];
  }
  average = average/num_samples2;
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result); 

  vth_astat_clr(mod_id);
  /* close the AF2 measurement path relays */
  vth_aclose_rly(mod_id, K2702);   /* Source internal relay */
  vth_aclose_rly(mod_id, K818);    /* SRCOUT to L bus relay */
  vth_aclose_rly(mod_id, K754);    /* L bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);    /* XL bus to AF2 relay */

  flush_commands(mod_id);
  vth_mtmr_wait(mod_id, WAIT_TIME);

  //subtest3, measure voltage after re-close relays
  for (i = 0; i < num_samples2; i++)
  {
    waveform2[i] = measure_dmm50(mod_id);
    average += waveform2[i];
  }
  average = average/num_samples2;
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = average;
  log_result(mod_id, result);
  vth_afc_dis_src_out(mod_id);

  vth_ainit(mod_id);
  close_dmm50(mod_id);
  close_syn(mod_id); 
  close_mux(mod_id);
  CloseResourceManager();
  }

}  /* t16335 */


/*--------------------------------------------------------------------------------
* Test: 16337
* Description: DC source tri-state spike test
*				
* Fixture: Analog
* Resource(s): Counter
*--------------------------------------------------------------------------------
*/

#define POSITIVE_SRC_VOLTAGE  5.0e+00
#define NEGATIVE_SRC_VOLTAGE  (-5.0e+00)
#define MEASURE_WAIT_TIME  2.0e+00

void t16337(int mod_id, int slot)//9270
{
  double min_volt;
  double max_volt;
  test_result_type result;
  OpenResourceManager();
  open_cntr(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  setup_cntr_defaults(mod_id);
  setup_cntr_input(mod_id, DC, 50);
  aconnect_cntr_ch1_af2(mod_id);

  aconnect_srcout_AF2(mod_id);
  vth_asrc_dis_30ma_ilim(mod_id);
  vth_asrc_set_dc_ampl(mod_id, POSITIVE_SRC_VOLTAGE);
  setup_cntr_trigger_min_max(mod_id, MIN);
  vth_afc_en_src_out(mod_id);   /* TAKES THE SRC OUT OF TRI-STATE */
  vth_mtmr_wait(mod_id, MEASURE_WAIT_TIME);
  flush_commands(mod_id);
  trigger_cntr(mod_id);
  min_volt = read_cntr(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = (min_volt - 5);
  log_result(mod_id, result);
  vth_afc_dis_src_out(mod_id);
  vth_asrc_set_dc_ampl(mod_id, NEGATIVE_SRC_VOLTAGE);
  setup_cntr_trigger_min_max(mod_id, MAX);
  vth_afc_en_src_out(mod_id);   /* TAKES THE SRC OUT OF TRI-STATE */
  vth_mtmr_wait(mod_id, MEASURE_WAIT_TIME);
  flush_commands(mod_id);
  trigger_cntr(mod_id); 
  max_volt = read_cntr(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = max_volt + 5;
  log_result(mod_id, result);
  vth_afc_dis_src_out(mod_id);
  vth_ainit(mod_id);

  close_cntr(mod_id);
  close_mux(mod_id);
  CloseResourceManager();

}  /* t16337 */

#undef POSITIVE_SRC_VOLTAGE
#undef NEGATIVE_SRC_VOLTAGE
#undef MEASURE_WAIT_TIME
#undef LOW
#undef HIGH
#undef R625
#undef R2500
#undef R10000
#undef DIFF

/*--------------------------------------------------------------------------------
* Test: 16338
* Description: DC source output current
*				
* Fixture: Analog
* Resource(s): Power supply
*--------------------------------------------------------------------------------
*/

#define LOW						0
#define HIGH					1
#define SOURCING				0
#define SINKING					1
#define POSITIVE				0
#define NEGATIVE				1
#define FEEDBACK_RESISTOR_NO	0
#define MEAS_TYPE				1
#define LENGTH					1
#define AVERAGES				10
#define DIGITAL					0
#define R625					4
#define R2500					6
#define R10000					8
#define EXPECTED_DET_VOLTS		2.0e+00
#define TIMEOUT					10.0e+00
#define PS_CURRENT				800e-03
#define DIFF					5e-03
#define RAMP_TIME				12e+00
#define LOW_CURRENT_VALUE		27.5e-03
#define HIGH_CURRENT_VALUE		165e-03

void t16338(int mod_id, int slot)//9275
{
  int source_i_compliance;
  double SOURCE_VOLTAGE[2] = {7e00, -7e00};
  double r10;
  double detector_voltage;
  double ps_sense_voltage;
  double ps_voltage;
  double source_current;
  double meas_source_current;
  double vload;
  double vnoload;
  double vatten = 1.01e+00;
  double requiv = 4.0e+00;
  double rseries = 0.6e+00;
  test_result_type result;
  a_word_type this_word;

  vth_apowered_init(mod_id);
  OpenResourceManager();
  open_ps(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);
  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);
  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, EXPECTED_DET_VOLTS, TIMEOUT);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
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
  vth_asrc_dis_30ma_ilim(mod_id);
  source_current = -1 * LOW_CURRENT_VALUE;   
  vth_aclose_rly(mod_id, K725);
  vth_aclose_rly(mod_id, K718);
  vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE[POSITIVE]);
  vth_afc_en_src_out(mod_id);
  ps_voltage = vatten * (SOURCE_VOLTAGE[POSITIVE] + source_current * (r10 + requiv + rseries));
  setup_ps(mod_id, ps_voltage, PS_CURRENT);
  verify_ps_output(mod_id, DIFF, RAMP_TIME, fabs(ps_voltage));  //DOWD FIXME!!!
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
  if (meas_source_current > 1.5 * fabs(source_current) || meas_source_current < 0.5 * fabs(source_current))
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
  rseries = (SOURCE_VOLTAGE[POSITIVE] - detector_voltage - vload) / meas_source_current;

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
        vth_mtmr_wait(mod_id, 0.0005);   
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K725);
        vth_aclose_rly(mod_id, K718);
      }  /* CONNECT PS + TO X1 AND PS - TO X2 */
      else
      {  /* CONNECT PS + TO X2 AND PS - TO X1 */
        vth_aopen_rly(mod_id, K725);
        vth_aopen_rly(mod_id, K718);
        vth_mtmr_wait(mod_id, 0.0005);   
        /* the supply doesn't get shorted out */
        vth_aclose_rly(mod_id, K726);
        vth_aclose_rly(mod_id, K717);
      }  /* CONNECT PS + TO X2 AND PS - TO X1 */

      for (int direction = SOURCING; direction <= SINKING; ++direction)
      {  /* CALCULATE SINKING AND SOURCING SETUP VOLTAGES */
        if (direction == SOURCING)
        {
          if (current == LOW)
            source_current = -1 * LOW_CURRENT_VALUE;   
          else
            source_current = -1 * HIGH_CURRENT_VALUE;   
        }
        else
        {  /* direction = sinking */
          if (current == LOW)
            source_current = LOW_CURRENT_VALUE;   
          else
            source_current = HIGH_CURRENT_VALUE;   
        }

        vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE[source_polarity]);

        ps_voltage = vatten * (SOURCE_VOLTAGE[source_polarity] + source_current * (r10 + requiv + rseries));
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
          source_i_compliance = P_getbits_US(this_word.bits, 0, 0);
          result.Measurement_class = CLASS_INTEGER;
          result.UU.Integer_result = source_i_compliance;
          log_result(mod_id, result);
        }
        else
          display_prompt("No result in result log (Expected aim status)", true);

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
  CloseResourceManager();

}  /* t16338 */

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
#undef EXPECTED_DET_VOLTS
#undef TIMEOUT
#undef PS_CURRENT
#undef DIFF
#undef RAMP_TIME
#undef LOW_CURRENT_VALUE
#undef HIGH_CURRENT_VALUE


/*--------------------------------------------------------------------------------
* Test: 16340
* Description: DC source maximum output current (high level only)
*				
* Fixture: Analog
* Resource(s): Power supply
*--------------------------------------------------------------------------------
*/

#define SOURCING        0
#define SINKING         1
#define POSITIVE        0
#define NEGATIVE        1
#define FEEDBACK_RESISTOR_NO  0
#define MEAS_TYPE       1
#define LENGTH          1
#define AVERAGES        1
#define DIGITAL         0
#define TIMEOUT         10.0e+00
#define POWER_SUPPLY_CURRENT  800e-03
#define POWER_SUPPLY_RAMP_TIME  2.0e+00
#define MAX_EXPECTED_VOLTS  2.1e+00

void t16340(int mod_id, int slot)//9277
{
 double SOURCE_VOLTAGE[2][2] = 
 {
	 {10.0e+00,   6.0e+00},
	 {-6.0e+00, -10.0e+00}
 };

 double PS_VOLTAGE[2][2] =
 {
	 { 6.0e+00,  10.0e+00},
	 {-10.0e+00, -6.0e+00}
 };
  double r10;
  double max_current;
  test_result_type result;

  vth_apowered_init(mod_id);
  OpenResourceManager();
  open_ps(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);
  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);

  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
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
      vth_mtmr_wait(mod_id, 0.0005);   
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K725);
      vth_aclose_rly(mod_id, K718);
    }  /* CONNECT PS + TO X1 AND PS - TO X2 */
    else
    {  /* CONNECT PS + TO X2 AND PS - TO X1 */
      vth_aopen_rly(mod_id, K725);
      vth_aopen_rly(mod_id, K718);
      vth_mtmr_wait(mod_id, 0.0005);   
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K726);
      vth_aclose_rly(mod_id, K717);
    }  /* CONNECT PS + TO X2 AND PS - TO X1 */

    for (int direction = SOURCING; direction <= SINKING; ++direction)
    {  /* SETUP FOR SOURCING AND SINKING CONDITIONS */
      vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE[src_polarity][direction]);
      vth_afc_en_src_out(mod_id);
      setup_ps(mod_id, PS_VOLTAGE[src_polarity][direction], POWER_SUPPLY_CURRENT);
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
  CloseResourceManager();

}  /* t16340 */

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


/* AC SOURCE VOLTAGE ACCURACY AND RANGE */

#define NUM_VOLTAGES_TO_TEST	3
#define NUM_FREQUENCIES_TO_TEST 3
#define STOP_FREQ				20000
#define F128					5
#define F1024					6
#define F8192					7
#define DC_OFFSET				0.0e+00
#define LOW_BAND_RATIO			0.8e+00

void t16355(int mod_id, int /*slot*/)//9355
{
  int start_freq;
  int dmm_voltage_range[3] = {1, 5, 10};
  int FREQ_A[3] = {128, 1024, 8192};
  int FILTER[3] = {F128, F1024, F8192};
  int range[3];
  double TEST[3] = {0.1e00, 1.0e00, 7.0707e00};
  double resolution[3] = {1e-03, 10e-03, 100e-03};
  a_word_type this_word;
  test_result_type result;
  vth_ainit(mod_id);

  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);
  setup_dmm_level_filter(mod_id, true);
  for (int voltage = 0; voltage < NUM_VOLTAGES_TO_TEST; ++voltage)
  {  /*for voltage*/
    setup_dmm(mod_id, ACV, dmm_voltage_range[voltage], resolution[voltage]);
    result.Measurement_class = CLASS_REAL;

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
      vth_asrc_set_ampl(mod_id, TEST[voltage], DC_OFFSET);
      vth_afc_en_src_out(mod_id);
	  vth_mtmr_wait(mod_id, TIME_OUT);
      flush_commands(mod_id);
 	  result.UU.Real_result = measure_dmm(mod_id);
	  log_result(mod_id, result);
    }  /* for frequency */
    /* READ RANGE BIT TO VERIFY THE AC SOURCE OPERATES IN THE CORRECT RANGE */
    vth_ard_cell(mod_id, ARLY_2A);
    if (get_result_nolog(mod_id, &result))
    {  
      this_word.data = result.UU.Integer_result;
      range[voltage] = P_getbits_US(this_word.bits, 0, 0);
    }  
    else
      display_prompt("No result in result log (Expected asrc range)", true);
    vth_afc_dis_src_out(mod_id);
	vth_mtmr_wait(mod_id, TIME_OUT);
	flush_commands(mod_id);
  } /*for voltage*/
  for (voltage = 0; voltage < NUM_VOLTAGES_TO_TEST; voltage++)
  {  
    result.Measurement_class = CLASS_INTEGER;
    result.UU.Integer_result = range[voltage];
    log_result(mod_id, result);
  }  

  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();

}  /* t16355 */

#undef NUM_VOLTAGES_TO_TEST
#undef NUM_FREQUENCIES_TO_TEST
#undef STOP_FREQ
#undef F128
#undef F1024
#undef F8192
#undef DC_OFFSET
#undef LOW_BAND_RATIO



#define NUM_FREQUENCIES_TO_TEST  3
#define F128            5
#define F1024           6
#define F8192           7
#define RMS_VOLTS       0.1e+00
#define FUDGE_FACTOR    1.2e+00
#define DC_OFFSET       0.0e+00
#define AC_OF_0V        0.0e+00
#define RESOLUTION      10e-04
#define DMM_FUNCTION    DCV
typedef int frequency_array_[NUM_FREQUENCIES_TO_TEST];
const frequency_array_ FREQ_A_ = {128, 1024, 8192};
const frequency_array_ FILTER_ = {F128, F1024, F8192};

void t16360(int mod_id, int /*slot*/)//9360
{
  /* AC SOURCE DC OFFSET VOLTAGE ACCURACY */
  int frequency;
  //double measurement;
  double dmm_volts;
  double aperature;
  double measurement;

  test_result_type result;
  //result.Measurement_class = CLASS_REAL;

  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);
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
    //if (get_result_nolog(mod_id, &result))
	if (get_result_log(mod_id, &result))
      display_prompt("No result in result log (Expected dmm vth_reading)", true);
	//else
	//{
	//	result.UU.Real_result = measurement;
      //  log_result(mod_id, result);
	//} 

    vth_afc_dis_src_out(mod_id);
    vth_asrc_set_ampl(mod_id, RMS_VOLTS, DC_OFFSET);
    vth_afc_en_src_out(mod_id);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    measurement = measure_dmm(mod_id);
	result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = measurement;
    log_result(mod_id, result);
    //if (get_result_log(mod_id, &result))
      //display_prompt("No result in result log (Expected dmm vth_reading)", true);
	//else
	//{
	//	result.UU.Real_result = measurement;
      //  log_result(mod_id, result);
	//} 

  }  /* for frequency */

  vth_afc_dis_src_out(mod_id);
  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();

}  /* t16360 */

#undef NUM_FREQUENCIES_TO_TEST
#undef F128
#undef F1024
#undef F8192
#undef RMS_VOLTS
#undef FUDGE_FACTOR
#undef DC_OFFSET
#undef AC_OF_0V
#undef RESOLUTION

#undef DMM_FUNCTION


/* PRECISION AC SOURCE FREQUENCY ACCURACY AND JITTER MEASUREMENT  */
/* the res for 128 should be 0.001, but the 3458 has a bug that errors   */
/* if you use that gate time.  once this is fixed it should be converted */
/* since the 128 Hz takes nearly 2 minutes to complete. The same is true */
/* for 8192 Hz.  Eventually gate time for it should be 0.001 => 0.01     */

#define NUM_FREQUENCIES_TO_TEST  3
#define SAMPLE_SIZE     50
#define SIGMA_MULTIPLIER  6
#define F128            5
#define F1024           6
#define F8192           7
#define RMS_VOLTS		7.071
#define LOW             0.8
#define HIGH            1.2
#define DC_OFFSET       0.0e+00


void t16367(int mod_id, int /*slot*/)//9367
{
  int this_frequency;
  int range[3] = {10, 1000, 10000};
  int frequency_array[NUM_FREQUENCIES_TO_TEST] = {128, 1024, 8192};
  int filter_array[NUM_FREQUENCIES_TO_TEST] = {F128, F1024, F8192};
  double res_array[NUM_FREQUENCIES_TO_TEST] = {0.0001, 0.001, 0.001};
  double sigma;
  double largest;
  double smallest;
  double mean;

  test_result_type result;
  result.Measurement_class = CLASS_REAL;
  
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);
  setup_dmm_fsource(mod_id, DCV);
  setup_dmm_level(mod_id, 0, DMM_DC);
  setup_dmm_level_filter(mod_id, true);
  hold_dmm(mod_id);

  for (this_frequency = 0; this_frequency < NUM_FREQUENCIES_TO_TEST; this_frequency++)
  {  /* for frequency */
	if(this_frequency == 0)
		setup_dmm_readings(mod_id, SAMPLE_SIZE / 2);
	else 
		setup_dmm_readings(mod_id, SAMPLE_SIZE);
    enable_dmm_buffer(mod_id);
    setup_dmm(mod_id, PER, DMMAUTO, res_array[this_frequency]);
    setup_dmm_memory(mod_id, SREAL, FIFO);
    setup_dmm_ac_band(mod_id, (LOW * frequency_array[this_frequency]), 
								(HIGH * frequency_array[this_frequency]));
    vth_asrc_sel_fltr(mod_id, filter_array[this_frequency]);
    vth_ascat_cpu_init(mod_id);
    vth_ascat_sel_freq(mod_id, FREQA);
    vth_ascat_set_a_freq(mod_id, frequency_array[this_frequency]);
    vth_ascat_en_cont_src(mod_id);
    vth_ascat_en_frun_src(mod_id);
    vth_ascat_run_en(mod_id);
    vth_asrc_dis_30ma_ilim(mod_id);
    vth_asrc_set_ampl(mod_id, RMS_VOLTS, DC_OFFSET);
    vth_afc_en_src_out(mod_id);
    flush_commands(mod_id);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    trigger_dmm(mod_id);
	wait_for_dmm_measurement(mod_id, DMM_ADDRESS);
    calculate_dmm_stats(mod_id);

    /* Read and log the mean */
    mean = read_dmm_stats(mod_id, MEAN);
    if (mean != 0.0)
      mean = 1 / mean;

	result.UU.Real_result = mean;
    log_result(mod_id, result);

    /* Calculate and log the jitter */
    sigma = read_dmm_stats(mod_id, SDEV);
	smallest = read_dmm_stats(mod_id, SALES_MIN);
	largest = read_dmm_stats(mod_id, SALES_MAX);
    sigma *= SIGMA_MULTIPLIER;
    if (largest - smallest > sigma)
      result.UU.Real_result = largest - smallest;
    else
      result.UU.Real_result = sigma;

    log_result(mod_id, result);
  }  /* for frequency */

  vth_afc_dis_src_out(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();

}  /* t16367 */

#undef NUM_FREQUENCIES_TO_TEST
#undef SAMPLE_SIZE
#undef SIGMA_MULTIPLIER
#undef RMS_VOLTS
#undef F128
#undef F1024
#undef F8192
#undef LOW
#undef HIGH
#undef DC_OFFSET



/* AC SOURCE OUTPUT RESISTANCE */

#define FEEDBACK_RESISTOR_NO		0
#define MEAS_TYPE					0
#define DEFAULT_SOURCE_RESISTANCE	0
#define NUM_FREQUENCIES_TO_TEST		3
#define STOP_FREQ					20000
#define RMS_TEST_VOLTAGE			100e-03
#define DC_OFFSET					0e+00
#define LOW_BAND_RATIO				0.8e+00
#define RESOLUTION					1e-03
#define DMM_FUNCTION				ACV
#define F128						5
#define F1024						6
#define F8192						7

void t16370(int mod_id, int /*slot*/)//9370
{
  int start_freq;
  int frequency_array[NUM_FREQUENCIES_TO_TEST] = {128, 1024, 8192};
  int filter_array[NUM_FREQUENCIES_TO_TEST] = {F128, F1024, F8192};
  double r10;
  double dmm_no_load_voltage;
  double dmm_loaded_voltage;
  double dmm_voltage_range = 1;
  double source_resistance;
  test_result_type result;
  
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  flush_commands(mod_id);
  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);  
  setup_dmm_level_filter(mod_id, true);
  setup_dmm(mod_id, DMM_FUNCTION, dmm_voltage_range, RESOLUTION);
  for (int frequency = 0; frequency < NUM_FREQUENCIES_TO_TEST; ++frequency)
  {  /*for frequency*/
    start_freq = static_cast<int>(frequency_array[frequency] * LOW_BAND_RATIO);
    setup_dmm_ac_band(mod_id, start_freq, STOP_FREQ);
    vth_asrc_sel_fltr(mod_id, filter_array[frequency]);
    vth_ascat_cpu_init(mod_id);
    vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
    vth_ascat_set_a_freq(mod_id, frequency_array[frequency]);
    vth_ascat_en_cont_src(mod_id);
    vth_ascat_en_frun_src(mod_id);
    vth_ascat_run_en(mod_id);
    vth_asrc_en_30ma_ilim(mod_id);
    vth_asrc_set_ampl(mod_id, RMS_TEST_VOLTAGE, DC_OFFSET);
    vth_afc_en_src_out(mod_id);
    vth_mtmr_wait(mod_id, TIME_OUT);
    flush_commands(mod_id);
	dmm_no_load_voltage = measure_dmm(mod_id);
    if (dmm_no_load_voltage <= 1.0e-37)
      display_prompt("DMM No Load Voltage Reading Error", true);

    vth_afc_dis_src_out(mod_id);
    vth_mtmr_wait(mod_id, TIME_OUT);
	flush_commands(mod_id);
    vth_aclose_rly(mod_id, K826);   /*SRCOUT to REFR2*/
    vth_aclose_rly(mod_id, K845);   /*REFR2 to 10 ohm to I bus*/
    vth_aclose_rly(mod_id, K855);   /*I bus to CALIB2*/
    vth_afc_en_src_out(mod_id);
	vth_mtmr_wait(mod_id, TIME_OUT);
	flush_commands(mod_id);
	dmm_loaded_voltage = measure_dmm(mod_id);
    if (dmm_loaded_voltage <= 1.0e-37)
      display_prompt("DMM Loaded Voltage Reading Error", true);

    if (dmm_loaded_voltage != 0)
      source_resistance = (dmm_no_load_voltage - dmm_loaded_voltage) /
                          dmm_loaded_voltage * r10;
    else
      source_resistance = DEFAULT_SOURCE_RESISTANCE;

	result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = source_resistance;
    log_result(mod_id, result);

    vth_afc_dis_src_out(mod_id);
    vth_aopen_rly(mod_id, K826);
    vth_aopen_rly(mod_id, K845);
    vth_aopen_rly(mod_id, K855);
	flush_commands(mod_id);
  }  /*for frequency*/

  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();

}  /* t16370 */

#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef SRC_ON
#undef SRC_CONTROL
#undef DEFAULT_SOURCE_RESISTANCE
#undef NUM_FREQUENCIES_TO_TEST
#undef STOP_FREQ
#undef RMS_TEST_VOLTAGE
#undef DC_OFFSET
#undef LOW_BAND_RATIO
#undef RESOLUTION
#undef DMM_FUNCTION
#undef F128
#undef F1024
#undef F8192


/*--------------------------------------------------------------------------------
* Test: 16371 & 16383
* Description: AC source total harmonic distortion
*				
* Fixture: Analog
* Resource(s): DMM
*--------------------------------------------------------------------------------
*/

#define NUM_FREQS_TO_TEST  4
#define NUM_VOLTS_TO_TEST  3
#define DCSETOFF        0.0e+00
#define XDB_PEAK        (-90.0)
#define XDB_HARMONIC    (-1.0)
typedef int freq_array[NUM_FREQS_TO_TEST];
typedef double volts_array[NUM_VOLTS_TO_TEST];
typedef double time_array[NUM_FREQS_TO_TEST];
const freq_array TEST_FREQ = {128, 1024, 8192, 20000};
const freq_array FILTER = {FILTER_128, FILTER_1024, FILTER_8192, FILTER_20K};
const volts_array TEST_VOLT = {0.1e00, 1.0e00, 7.071};
const time_array TEST_INTERVAL = {381.5e-6, 47.7e-6, 10.5e-6, 2.49e-6};
const time_array TEST_APERATURE = {100e-06, 12.0e-6, 1.4e-6, 2e-9};

void t16371(int mod_id, int slot)
{
  arrform waveform;
  arrform y;
  test_result_type result;
  double thd;
  double sig_noise_ratio;
  double tsignal;
  double tharmonic = 1.0;
  double resolution = 1.0E-03;
  double statavg = 0.0;
  int sample_size[4] = {20, 20, 10, 20};
  int range[3] = {1, 10, 100};
  int trig_level = 0;
  int slope = POS;
  int freq;
  int volt;
  int freq_elements;
  int dc_index;
  int peak_index;
  int peak_lo;
  int peak_hi;
  int lo_index;
  int hi_index;
  int level = 0;
  int mode = DMM_DC;
//FILE *ofile;
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  setup_dmm_norm(mod_id);
  vth_mod_init_dgn(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  flush_commands(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);

  for (volt = 0; volt < NUM_VOLTS_TO_TEST; volt++)
  {
    for (freq = 0; freq < NUM_FREQS_TO_TEST; freq++)
    {  /* for frequency */
      freq_elements = sample_size[volt] / 2;
      vth_asrc_sel_fltr(mod_id, FILTER[freq]);
      vth_ascat_cpu_init(mod_id);
      vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
      vth_ascat_set_a_freq(mod_id, TEST_FREQ[freq]);
      vth_ascat_en_cont_src(mod_id);
      vth_ascat_en_frun_src(mod_id);
      vth_ascat_run_en(mod_id);
      vth_asrc_en_30ma_ilim(mod_id);
      vth_asrc_set_ampl(mod_id, TEST_VOLT[volt], DCSETOFF);
      vth_afc_en_src_out(mod_id);
//	  setup_dmm_digitizing(mod_id, DMM_ADDRESS, range[volt], TEST_INTERVAL[freq], sample_size[volt],
//							TEST_APERATURE[freq], resolution, level, slope, mode);
//    capture_ddm(mod_id, sample_size[volt], waveform, DMM_ADDRESS);
	  vth_mtmr_wait(mod_id, MEASUREMENT_WAIT);
	  flush_commands(mod_id);
	  dmm_display(mod_id, DMM_ADDRESS, false);  
	  for(int i = 0; i < sample_size[volt]; i++)
		waveform[i] = measure_dmm(mod_id);

/*    int i;
      display_prompt("starting to write raw.y", true);
      ofile = fopen("raw.xls", "a");
      for (i =0; i<sample_size[freq]; i++)
        fprintf(ofile, "%f\n", waveform[i]);
	  fprintf(ofile, "before\n");
      fclose(ofile);
      display_prompt("finished writing raw.y", true);
      display_prompt("staring waveform_to_magnitude", true);*/

      waveform_to_magnitude(waveform, y, sample_size[volt]);

/*     display_prompt("starting to write mag.y", true);
      ofile = fopen("mag.xls", "a");
      for (int i =0; i<freq_elements; i++)
        fprintf(ofile, "%f\n", y[i]);
	  fprintf(ofile, "\n\n");
      fclose(ofile);
      display_prompt("finished writing mag.y", true); */

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

//ofile = fopen("16371vars.txt", "a");  
//fprintf(ofile, "%f\t %f\t %f\t %f\n", y[peak_index], y[peak_lo], y[peak_hi], sig_noise_ratio);

      result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = sig_noise_ratio;
	  get_result_nolog(mod_id, &result);
//      tharmonic = find_harmonic_power(peak_index, freq_elements, y, XDB_HARMONIC);
//	  if(volt == 2 && freq == 1)
//		  tharmonic = 1.0;

      thd = 10.0 * log10(tsignal / tharmonic);
      result.UU.Real_result = thd;
	  get_result_nolog(mod_id, &result);

      /* display_prompt('about to rewite res.y',true);*/
      /****/
      /* flush_commands(mod_id);*/
      /****/
      /* rewrite(ofile,'res.y');*/
      /****/
      /* vth_writeln(ofile,'DC Index ', dc_index:6);*/
      /****/
      /* vth_writeln(ofile,'Fundamental Frequency ',((peak_index-1)/test_interval[freq]/
              ( sample_size):18:8);*/
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
	  flush_commands(mod_id);
    }  /* for voltage and frequency loops */
  }
  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
//fclose(ofile);
} /*16371*/

#undef NUM_FREQS_TO_TEST
#undef NUM_VOLTS_TO_TEST
#undef DCSETOFF
#undef XDB_PEAK
#undef XDB_HARMONIC





#define NUM_F1S_TO_TEST  4
#define NUM_F2S_TO_TEST  3
#define DCSETOFF        0.0e+00
#define RANGE           1
#define APERATURE       12.0e-6
#define XDB             (-75.0)
#define VOLTS          0.3e+00   /* AC voltage output*/
typedef int freq1_array[NUM_F1S_TO_TEST];
typedef int freq2_array[NUM_F2S_TO_TEST];
typedef double f2_real[NUM_F2S_TO_TEST];
typedef f2_real time_array_[NUM_F1S_TO_TEST];
typedef freq2_array sample_array[NUM_F1S_TO_TEST];
const freq1_array TESTF1 = {697, 770, 852, 941};
const freq2_array TESTF2 = {1209, 1336, 1477};
const time_array_ SAMPLE_RATE =
{
  {82.0e-6, 76.4e-6, 64.8e-6},
  {72.3e-6, 74.2e-6, 69.1e-6},
  {53.3e-6, 63.6e-6, 64.8e-6},
  {49.3e-6, 51.9e-6, 51.9e-6}
};
const sample_array SAMPLES =
{
  {2048, 2048, 4096},
  {2048, 2048, 2048},
  {2048, 2048, 2048},
  {2048, 2048, 2048}
};

void t16380(int mod_id, int /*slot*/)//9380
{
  bool bogus;
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
  int lf_peak;
  int mf_peak;
  int hf_peak;
  int freq_elements;
  int level = 0;
  int slope = POS;
  int mode = DMM_DC;
  double f1_freq;
  double f2_freq;
  double tsignal;
  double tnoise;
  double t1signal;
  double t2signal;
  double tid;
  double freq_span;
  double signal_noise_ratio;
  double lower_mag;
  double resolution = 1.0E-03;
  arrform waveform;
  arrform y;

  test_result_type result;
  result.Measurement_class = CLASS_REAL;
//FILE *ofile;

  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  flush_commands(mod_id);

  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);
  for (i = 0; i < NUM_F1S_TO_TEST; i++)
  {
    for (j = 0; j < NUM_F2S_TO_TEST; j++)
    {
      bogus = false;
      freq_span = 1.0 / SAMPLES[i][j] / SAMPLE_RATE[i][j];
      freq_elements = SAMPLES[i][j] / 2;
      mid_index = floor((TESTF1[i] + TESTF2[j]) / 2.0 / freq_span + 1);
	  setup_dmm(mod_id, DCV, 1.0, resolution);
      /* choose and enable 20khz filter for output */
      vth_asrc_sel_fltr(mod_id, FILTER_20K);
      vth_ascat_cpu_init(mod_id);
      vth_ascat_sel_freq(mod_id, 0);   /* Select frequency A */
      vth_ascat_set_a_freq(mod_id, TESTF1[i]);
      vth_ascat_en_cont_src(mod_id);
      vth_ascat_en_frun_src(mod_id);
      vth_ascat_run_en(mod_id);
      vth_asrc_en_30ma_ilim(mod_id);
      vth_asrc_set_ampl(mod_id, VOLTS, DCSETOFF);
      vth_afc_en_src_out(mod_id);

      /* setup aim source for dual frequency mode */
      vth_ascat_set_a_freq(mod_id, TESTF1[i]);
      vth_ascat_set_b_freq(mod_id, TESTF2[j]);
      vth_asrc_set_ac_ampl(mod_id, VOLTS);
      vth_ascat_en_dual_freq(mod_id);
      setup_dcv_dig(mod_id, RANGE, SAMPLE_RATE[i][j], SAMPLES[i][j], APERATURE, level, mode);
      capture_ddm(mod_id, SAMPLES[i][j], waveform, DMM_ADDRESS);
      waveform_to_magnitude(waveform, y, SAMPLES[i][j]);
      vth_afc_dis_src_out(mod_id);
	  flush_commands(mod_id);
 
/*    display_prompt("starting to write raw.xls", true);
      ofile = fopen("raw.xls", "w");
      for (int k=0; k<SAMPLES[i][j]; k++)
        fprintf(ofile, "%d %f\n", k, waveform[k]);
      fclose(ofile);
      display_prompt("finished writing raw.xls", true);
      display_prompt("starting to write mag.xls", true);
      ofile = fopen("mag.xls", "w");
      for (k=0; k<SAMPLES[i][j]; k++)
        fprintf(ofile, "%d %f\n", k, y[k]);
      fclose(ofile);
      display_prompt("finished writing mag.xls", true);     
*/
      dc_index = find_dc_index(freq_elements, y);

      /* analyze the spectra as two spectra about the mid point between f1 and f2 */
      peak1 = find_peak_index(dc_index, mid_index, y);
      peak2 = find_peak_index(mid_index, freq_elements, y);
      find_local_minima(dc_index, mid_index, peak1, y, XDB, &lo1_index, &hi1_index);
      find_local_minima(mid_index, freq_elements, peak2, y, XDB, &lo2_index, &hi2_index);

/*      ofile = fopen("values.xls", "w");      
      fprintf(ofile, "dc_index = %d\n", dc_index);      
      fprintf(ofile, "peak1 = %d\n", peak1);      
      fprintf(ofile, "peak2 = %d\n", peak2);      
      fprintf(ofile, "mid_index = %d\n", mid_index);
      fprintf(ofile, "lo1_index = %d\n", lo1_index);
      fprintf(ofile, "hi1_index = %d\n", hi1_index);
	  fprintf(ofile, "lo2_index = %d\n", lo2_index);
	  fprintf(ofile, "hi2_index = %d\n", hi2_index);
	  fclose(ofile);*/
      /* verify the proper frequencies were found and that their magnitudes match  */
      /* otherwise log bogus values for the minimium signal to noise ratio and Tid */

      f1_freq = freq_span * (peak1);
      f2_freq = freq_span * (peak2);
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
	  result.UU.Real_result = signal_noise_ratio;
      log_result(mod_id, result);
	  t1signal = sum_power(lo1_index+1, hi1_index+1, y);
	  t2signal = sum_power(lo2_index+1, hi2_index+1, y);
	  tsignal = t1signal + t2signal;
	  tnoise = sum_power(dc_index+1, lo1_index+1, y);
	  tnoise += sum_power(hi1_index+1, lo2_index+1, y);
	  tnoise += sum_power(hi2_index+1, freq_elements+1, y);
	  tid = 10.0 * log10(tsignal / tnoise);
	  result.UU.Real_result = tid;
      log_result(mod_id, result);
    }  /* for frequency loops */
  }
  vth_ainit(mod_id);
  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}

#undef NUM_F1S_TO_TEST
#undef NUM_F2S_TO_TEST
#undef DCSETOFF
#undef RANGE
#undef APERATURE
#undef XDB
#undef VOLTS



/* ARB SOURCE SQUARE WAVE VOLTAGE ACCURACY, OVERSHOOT, and SLEW RATE      */

#define ARB_FREQ        1024
#define SRC_I_COMPLIANCE  1
#define SRC_R_OUT       0
#define SRC_ON          1
#define SRC_CONTROL     0
#define PEAK_VOLTS      20e+00
#define DC_OFFSET       0e+00

void t16386(int mod_id, int /*slot*/)//9386
{
  bool check;
  int slope;
  int counter;
  int first;
  int i;
  int last;
  int num_samples = 5800;
  int range = 10;
  int samples;
  int trig_level;
  double max_volts = 0.0;
  double min_volts = 0.0;
  double v_at_15us;
  double percent_overshoot;
  double slew_time;
  double slew_voltage;
  double slew_rate;
  double first_num;
  double last_num;
  double sampling_rate = 10E-9;
  double volt_average = 7E-6;
  arrform waveform;

  test_result_type result;

  OpenResourceManager();
  open_dmm50(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  connect_dmm50_out_af2(mod_id);

  for (int edge = RISING; edge <= FALLING; ++edge)
  {  /* for edge */
    if (edge == RISING)
      slope = POS;
    else
      slope = NEG;

    vth_src_sqr_setup(mod_id, ARB_FREQ, PEAK_VOLTS, DC_OFFSET, SRC_I_COMPLIANCE,
		      SRC_R_OUT, SRC_ON, SRC_CONTROL);

    aconnect_srcout_AF2(mod_id);
    check = false;
	i = 1;
    slew_rate = 0.0;
    slew_time = 1.0;
    samples = volt_average/ sampling_rate;
	vth_mtmr_wait(mod_id, TIME_OUT);
	flush_commands(mod_id);
    if (slope == POS)
	{
      trig_level = -115;
      setup_dmm_subsampling_AC_and_DC(mod_id, DMM50_ADDRESS, num_samples, sampling_rate,
				trig_level, POS, range);
	}
    else
	{
      trig_level = 115;
      setup_dmm_subsampling_AC_and_DC(mod_id, DMM50_ADDRESS, num_samples, sampling_rate,
				trig_level, NEG, range);
    }
    capture_ddm(mod_id, num_samples, waveform, DMM50_ADDRESS);
	for(int j = 0; j < num_samples; j++)
	{
		if(max_volts < waveform[j])
		  max_volts = waveform[j];
		if(min_volts > waveform[j])
		  min_volts = waveform[j];
	}	
    if(slope == NEG)
		max_volts = min_volts;

	do
	{
      if (slope == POS)
	  {
        if ((waveform[i] > 9) && (check == false)) 
		{
	      v_at_15us = waveform[i+samples];
	      check = true;
		}
	  }
      else
	  {
        if ((waveform[i] < -9) && (check == false))
		{
	      v_at_15us = waveform[i+samples];
	      check = true;
		}
	  }
	  i++;
	} while (!(check || i >= num_samples));
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = v_at_15us;
    log_result(mod_id, result);
    percent_overshoot = (max_volts - v_at_15us) / v_at_15us * 100;
    result.UU.Real_result = percent_overshoot;
    log_result(mod_id, result);
    counter = 0;
    first = 1;
    last = 1;
    if (slope == POS) 
	{ //measure slew time with mmeter
      for(i = 0; i < num_samples; i++)
	  {
        if(waveform[i] < 8 && first==1)
		{
	      first = 0;
	      first_num = waveform[i];
		}
        if (waveform[i] < 2 && last==1)
		{
	      last = 0;
	      last_num = waveform[i];
	      counter++;
		  i = num_samples;
		}
        if (waveform[i] < 8 && last==1)
	      counter++;
	  }
	}
    else
	{
      for(i = 0; i < num_samples; i++)
	  {
        if(waveform[i] > -8 && first==1)
		{
	      first = 0;
	      first_num = waveform[i];
		}
        if (waveform[i] > -2 && last==1)
		{
	      last = 0;
	      last_num = waveform[i];
	      counter++;
		  i = num_samples;
		}
        if (waveform[i] > -8 && last==1)
	      counter++;
	  }
	}
    slew_time = counter * sampling_rate;
    slew_voltage = fabs(last_num - first_num);
    if (slew_time != 0)   /* To avoid divide by 0 */
      slew_time = fabs(slew_time) * 1e06;   /* Convert uS TO seconds */
    else
      slew_time = INT_MAX;

    slew_rate = fabs(slew_voltage / slew_time);
    result.UU.Real_result = slew_rate;
    log_result(mod_id, result);
  }  /* for edge */

  vth_ainit(mod_id);

  close_dmm50(mod_id);
  close_mux(mod_id);
  CloseResourceManager();

}  /*t16386*/

#undef ARB_FREQ
#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef PEAK_VOLTS
#undef DC_OFFSET


/*ARB SOURCE TRIANGLE WAVE RISE/FALL TIME AND LINEARITY*/

#define ARB_FREQ        1024
#define SRC_I_COMPLIANCE  1
#define SRC_R_OUT       0
#define SRC_ON          1
#define SRC_CONTROL     0
#define PEAK_VOLTS      20e+00
#define DC_OFFSET       0e+00
#define NEG_VOLTS       (-8e+00)
#define POS_VOLTS       8e+00

void t16387(int mod_id, int /*slot*/)//9387
{
  int counter;
  int counter2;
  int num_samples = 5100;
  int range = 10;
  int i;
  int first;
  int mode = DMM_DC;
  double linearity_voltage;
  double trig_level;
  double sampling_rate = 1E-7;
  double interval = 2E-3;
  double slope;
  double voltage;
  double max_volts;
  double min_volts;
  test_result_type result;
  result.Measurement_class = CLASS_REAL;
  arrform waveform;

  OpenResourceManager();
  open_dmm50(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  connect_dmm50_out_af2(mod_id);
  aconnect_srcout_AF2(mod_id);
  vth_src_tri_setup(mod_id, ARB_FREQ, PEAK_VOLTS, DC_OFFSET, SRC_I_COMPLIANCE,
                SRC_R_OUT, SRC_ON, SRC_CONTROL);
  flush_commands(mod_id);
  for (int edge = POS; edge >= NEG; --edge)
  {  /* for edge */
    counter = 0;
    if (edge == POS)
	{
      trig_level = 110;
      setup_dmm_subsampling_AC_and_DC(mod_id, DMM50_ADDRESS, num_samples, sampling_rate,
				trig_level, POS, range);
	}
    else
	{
      trig_level = -110;
      setup_dmm_subsampling_AC_and_DC(mod_id, DMM50_ADDRESS, num_samples, sampling_rate,
				trig_level, NEG, range);
    }
    capture_ddm(mod_id, num_samples, waveform, DMM50_ADDRESS);
    if (edge == NEG)
    {
      counter = 0;
      counter2 = 0;
      for(i = 0; i < num_samples; i++) 
	  {
	    if (waveform[i] > waveform[i+1] && waveform[i] > NEG_VOLTS
	        && waveform [i] < 0) 
		{
	      counter2++;  //number of samples from 0-(-8V)
		}
      }
	}
    else
	{
      for(i = 0; i < num_samples; i++)
	  {
		if(max_volts < waveform[i])
		  max_volts = waveform[i];
		if(min_volts > waveform[i])
			min_volts = waveform[i];
	  }	
	  result.UU.Real_result = max_volts;
      log_result(mod_id, result);
      result.UU.Real_result = min_volts;
      log_result(mod_id, result);
      counter = 0;
      counter2 = 0;
      for(i = 0; i < num_samples; i++) 
	  {
	    if (waveform[i] < waveform[i + 1] && waveform[i] < POS_VOLTS
	        && waveform[i] > 0) 
		{
	      counter2++;  //number of samples from (8)-0V
		}
	  }
	}
    first = num_samples + 1;
    if (edge == NEG) 
	{
      for(i = 0; i < num_samples; i++)
	  {
	    if ((waveform[i] > waveform[i + 1]) && (waveform[i] < POS_VOLTS) &&
	        (waveform[i] > NEG_VOLTS)) 
		{
	      if (first == (num_samples + 1)) 
		  {
	        counter = 0;
	        first = i;
		  }
	      counter++;  //number of samples from 8V to -8V
		}
	  }
    }
    else
    {
      for(i = 0; i < num_samples; i++)
	  {
	    if ((waveform[i] < waveform[i + 1]) && (waveform[i] < POS_VOLTS) &&
	        (waveform[i] > NEG_VOLTS)) 
		{
	      if (first == (num_samples + 1))
		  {
	        counter = 0;
	        first = i;
		  }
	      counter++;  //number of samples from -8 to 8V
		}
      }
    }
    if (counter2 != 0) 
	{
      slope = ((waveform[first + counter2] - waveform[first])/(counter2));
    }
    voltage = ((slope * counter) + waveform[first]);
    linearity_voltage = waveform[first + counter] - voltage;
    result.UU.Real_result = linearity_voltage;
    log_result(mod_id, result);
  }  /* for edge */

 vth_ainit(mod_id);

 close_dmm50(mod_id);
 close_mux(mod_id);
 CloseResourceManager();

}  /*t16387*/

#undef ARB_FREQ
#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef PEAK_VOLTS
#undef DC_OFFSET
#undef NEG_VOLTS
#undef POS_VOLTS


/*ARB SINE FREQUENCY ACCURACY*/

#define SRC_I_COMPLIANCE  1
#define SRC_R_OUT       0
#define SRC_ON          1
#define SRC_CONTROL     0
#define STEP_SIZE       10
#define LO_BAND         0
#define TRIGGER_PERCENT  0
#define COUPLING        1
#define HI_BAND         25000
#define START_FREQ      1
#define STOP_FREQ       20000
#define RESOLUTION      10e-06
#define RMS_VOLTS       7.071e+00
#define FUDGE_FACTOR    1.2e+00
#define DC_OFFSET       0e+00

#define DMM_FSOURCE     ACV

void t16391(int mod_id, int /*slot*/)//9391
{
  int freq = START_FREQ;
  double dmm_result;
  double dmm_voltage_range;
  test_result_type result;
  result.Measurement_class = CLASS_REAL;

  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);
  setup_dmm_ac_band(mod_id, LO_BAND, HI_BAND);
  setup_dmm_level_filter(mod_id, true);
  setup_dmm_fsource(mod_id, DMM_FSOURCE);
  setup_dmm_level(mod_id, TRIGGER_PERCENT, COUPLING);
  dmm_voltage_range = RMS_VOLTS * sqrt(2.0) * FUDGE_FACTOR;
  setup_dmm(mod_id, DMMFREQ, dmm_voltage_range, RESOLUTION);

  while (freq <= STOP_FREQ)
  {
    vth_src_sine_setup(mod_id, freq, RMS_VOLTS, DC_OFFSET, SRC_I_COMPLIANCE,
                   SRC_R_OUT, SRC_ON, SRC_CONTROL);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
	flush_commands(mod_id);
    dmm_result = measure_dmm(mod_id);
    if (dmm_result <= 1e-37)
      display_prompt("No result in result log (Expected dmm vth_reading)", true);

    if (freq == STOP_FREQ / 2)
      freq = STOP_FREQ;
    else
      freq *= STEP_SIZE;
    result.UU.Real_result = dmm_result;
    log_result(mod_id, result);
  }

  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /*t16391*/

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

#undef DMM_FSOURCE


/* ARB SOURCE SINE WAVE JITTER MEASUREMENT */

#define NUM_FREQUENCIES_TO_TEST  5
#define SRC_I_COMPLIANCE  1
#define SRC_R_OUT       0
#define SRC_ON          1
#define SRC_CONTROL     0
#define SAMPLE_SIZE__   100
#define SALES_FUNC            PER
#define MODE            DCV
#define LOW             0.8
#define HIGH            1.2
#define RES_            0.001
#define RMS_VOLTS       7.071e+00
#define DC_OFFSET       0.0e+00


void t16392(int mod_id, int /*slot*/)//9392
{
  double smallest;
  double largest;
  int frequency_array[NUM_FREQUENCIES_TO_TEST] = { 10, 100, 1000, 10000, 20000 };

  test_result_type result;
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);
  aconnect_srcout_calib1(mod_id);
  setup_dmm_fsource(mod_id, MODE);
  setup_dmm_level(mod_id, 0, DMM_DC);
  setup_dmm_level_filter(mod_id, true);
  hold_dmm(mod_id);
  setup_dmm_readings(mod_id, SAMPLE_SIZE__);
  enable_dmm_buffer(mod_id);
  setup_dmm(mod_id, SALES_FUNC, RMS_VOLTS, RES_);
  vth_src_sine_setup(mod_id, frequency_array[0], RMS_VOLTS, DC_OFFSET,
                 SRC_I_COMPLIANCE, SRC_R_OUT, SRC_ON, SRC_CONTROL);

  for (int this_frequency = 0; this_frequency < NUM_FREQUENCIES_TO_TEST; this_frequency++)
  {  /* for frequency */
     setup_dmm_memory(mod_id, DREAL, FIFO);
     setup_dmm_ac_band(mod_id, floor(LOW * frequency_array[this_frequency]),
                      ceil(HIGH * frequency_array[this_frequency]));
     vth_ascat_set_a_freq(mod_id, frequency_array[this_frequency]);
     vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
     trigger_dmm(mod_id);
     flush_commands(mod_id);
     wait_for_dmm_measurement(mod_id, DMM_ADDRESS);
     calculate_dmm_stats(mod_id);
     smallest = read_dmm_stats(mod_id, SALES_MIN);
     largest = read_dmm_stats(mod_id, SALES_MAX);
     result.Measurement_class = CLASS_REAL;
     result.UU.Real_result = largest - smallest;
     log_result(mod_id, result);
   }  /* for frequency */

  vth_afc_dis_src_out(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();

}  /* t16392 */

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



/* ARB SOURCE JITTER MEASUREMENT USING SQUARE WAVE */

#define NUM_FREQUENCIES_TO_TEST  5
#define SRC_I_COMPLIANCE  1
#define SRC_R_OUT       0
#define SRC_ON          1
#define SRC_CONTROL     0
#define PEAK_VOLTS      4.0e+00
#define DC_OFFSET       0.0e+00
#define TRIGGER_LEVEL   0.0e+00


void t16393(int mod_id, int /*slot*/)//9393
{
  double cntr_actual_min;
  double cntr_actual_max;
  double cntr_max_minus_min;
  int frequency_array[NUM_FREQUENCIES_TO_TEST] = { 10, 100, 1000, 10000, 20000 };
  int sample_size = 10;

  test_result_type result;
  OpenResourceManager();
  open_cntr(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  aconnect_cntr_stop_af2(mod_id);
  aconnect_srcout_AF2(mod_id);
  setup_cntr_trg_levels(mod_id, TRIGGER_LEVEL, TRIGGER_LEVEL);
  setup_cntr_function(mod_id, PERIOD);
  for (int this_frequency = 0; this_frequency < NUM_FREQUENCIES_TO_TEST; this_frequency++)
  {  /* for frequency */
    setup_cntr_sample_size(mod_id, sample_size);
    vth_src_sqr_setup(mod_id, frequency_array[this_frequency], PEAK_VOLTS,
                  DC_OFFSET, SRC_I_COMPLIANCE, SRC_R_OUT, SRC_ON,
                  SRC_CONTROL);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
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
  CloseResourceManager();

}  /* t16393 */

#undef NUM_FREQUENCIES_TO_TEST
#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef PEAK_VOLTS
#undef DC_OFFSET
#undef TRIGGER_LEVEL



/* ARB SOURCE SQUARE WAVE FREQUENCY ACCURACY */

#define NUM_FREQUENCIES_TO_TEST  6
#define SRC_I_COMPLIANCE  1
#define SRC_R_OUT       0
#define SRC_ON          1
#define SRC_CONTROL     0
#define PEAK_VOLTS      4.0e+00
#define DC_OFFSET       0.0e+00
#define TRIGGER_LEVEL   0.0e+00


void t16394(int mod_id, int /*slot*/)//9394
{
  int frequency_array[NUM_FREQUENCIES_TO_TEST] = {1, 10, 100, 1000, 10000, 20000};
  double freq;
  double gate_array[NUM_FREQUENCIES_TO_TEST] = {1.0e+00, 1.0e+00, 1.0e+00, 0.1e+00, 0.1e+00, 0.1e+00};
  test_result_type result;
 
  OpenResourceManager();
  open_cntr(mod_id);
  open_mux(mod_id);

  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  setup_cntr_inp2_lowpass(mod_id);
  aconnect_cntr_stop_af2(mod_id);
  aconnect_srcout_AF2(mod_id);
  setup_cntr_trg_levels(mod_id, TRIGGER_LEVEL, TRIGGER_LEVEL);
  setup_cntr_function(mod_id, FREQUENCY);
//  setup_cntr_gate_time(mod_id, gate_array[0]);
  vth_src_sqr_setup(mod_id, frequency_array[0], PEAK_VOLTS, DC_OFFSET,
                SRC_I_COMPLIANCE, SRC_R_OUT, SRC_ON, SRC_CONTROL);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);

  for (int this_frequency = 0; this_frequency < NUM_FREQUENCIES_TO_TEST; this_frequency++)
  {  /* for frequency */
    vth_ascat_set_a_freq(mod_id, frequency_array[this_frequency]);
    setup_cntr_gate_time(mod_id, gate_array[this_frequency]);
    vth_mtmr_wait(mod_id, 10.0 * gate_array[this_frequency]);
    flush_commands(mod_id);
    trigger_cntr(mod_id);
    freq = read_cntr(mod_id);
    result.Measurement_class = CLASS_REAL;
    result.UU.Real_result = freq;
    log_result(mod_id, result);
  }  /* for frequency */
  vth_afc_dis_src_out(mod_id);
  flush_commands(mod_id);

  close_cntr(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /* t16394 */

#undef NUM_FREQUENCIES_TO_TEST
#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef PEAK_VOLTS
#undef DC_OFFSET
#undef TRIGGER_LEVEL



#define MIN_TEST_VOLTAGE  10.0e+00
#define MAX_TEST_VOLTAGE  20.0e+00
#define STARTING_INCREMENT  1024e-03
#define FINEST_INCREMENT  8e-03
#define SYN_WAIT_TIME   10e-03
#define MOA_COMP        5.0e+00
#define RESOLUTION      1e-03
#define FUDGE_FACTOR    1.2e+00
#define N6700B_CH1_CURR 500e-03

struct LOC_t16415
{
  int mod_id;
} ;
static void close_the_relays____(struct LOC_t16415 *LINK)
{
  vth_aclose_rly(LINK->mod_id, K2605);   /* MOA happy relay */
  vth_aclose_rly(LINK->mod_id, K843);   /* MOA NON to GND */
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

void t16415(int mod_id, int /*slot*/)//9415
{
  /* MOA POSITIVE OVER VOLTAGE LIMIT ACCURACY */
  int over_voltage_bit;
  double measurement = 0;
  double syn_voltage;
  double n6700b_ch1_voltage;
  double voltage_increment;
  bool vlimit;
  bool ever_had_a_vlimit;
  struct LOC_t16415 V;
  a_word_type this_word;
  test_result_type result;

  V.mod_id = mod_id;
  OpenResourceManager();
  open_mux(V.mod_id);
  open_dmm(V.mod_id);
  open_syn(V.mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  if(check_syn_model())
  {
  open_n6700b(V.mod_id);
  connect_n6700b_ch1(V.mod_id);
  aconnect_n6700b_ch1_abus1_xmint(V.mod_id);
  vth_amoa_en_30ma_ilim(V.mod_id);
  vth_amoa_set_v_cmpl(V.mod_id, MOA_COMP);
  n6700b_ch1_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = N6700B_CH1_STARTING_INCREMENT;
  vlimit = false;
  ever_had_a_vlimit = false;
  close_the_relays____(&V);
  setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
  setup_n6700b_ch1_curr(V.mod_id, N6700B_CH1_CURR);
  enable_n6700b_ch1_out(V.mod_id);
  vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
  vth_astat_clr(V.mod_id);
  while (!vlimit && n6700b_ch1_voltage <= MAX_TEST_VOLTAGE)
  {  /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      n6700b_ch1_voltage = MAX_TEST_VOLTAGE + 1;
      continue;
    }
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 11, 0);
    if (over_voltage_bit == 1)
    {
      vlimit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= N6700B_CH1_FINEST_INCREMENT ||
          n6700b_ch1_voltage == MIN_TEST_VOLTAGE)
        continue;
      vlimit = false;
      disable_n6700b_ch1_out(V.mod_id);
      vth_astat_clr(V.mod_id); 
      close_the_relays____(&V);
      voltage_increment /= 2;
      n6700b_ch1_voltage -= voltage_increment;
      setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
	  enable_n6700b_ch1_out(V.mod_id);
      vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
      continue;
    }
	if ( ever_had_a_vlimit && !number_equal(voltage_increment, N6700B_CH1_FINEST_INCREMENT) )
      voltage_increment /= 2;
    
	n6700b_ch1_voltage += voltage_increment;
    setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
    vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
  } 
  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, n6700b_ch1_voltage * FUDGE_FACTOR, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_n6700b_ch1_out_dmm(V.mod_id, true);
  measurement = measure_dmm(V.mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measurement;
  log_result(mod_id, result);

  vth_ainit(V.mod_id);
  close_mux(V.mod_id);
  close_dmm(V.mod_id);
  close_syn(V.mod_id);
  close_n6700b(V.mod_id);
  CloseResourceManager();
  }

  else
  {
  setup_syn(V.mod_id, DC, 0, 0.0, 0.0);
  setup_syn_hv(V.mod_id, true);
  aconnect_syn_out_xmint(V.mod_id);
  vth_amoa_en_30ma_ilim(V.mod_id);
  vth_amoa_set_v_cmpl(V.mod_id, MOA_COMP);
  syn_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = STARTING_INCREMENT;
  vlimit = false;
  ever_had_a_vlimit = false;
  close_the_relays____(&V);
  setup_syn_off(V.mod_id, syn_voltage);
  vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  vth_astat_clr(V.mod_id);
  while (!vlimit && syn_voltage <= MAX_TEST_VOLTAGE)
  {  /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      syn_voltage = MAX_TEST_VOLTAGE + 1;
      continue;
    }
    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 11, 0);
    if (over_voltage_bit == 1)
    {
      vlimit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT ||
          syn_voltage == MIN_TEST_VOLTAGE)
        continue;
      vlimit = false;
      setup_syn_off(V.mod_id, 0.0);
      vth_astat_clr(V.mod_id); 
      close_the_relays____(&V);
      voltage_increment /= 2;
      syn_voltage -= voltage_increment;
      setup_syn_off(V.mod_id, syn_voltage);
      vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
      continue;
    }
	if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    
	syn_voltage += voltage_increment;
    setup_syn_off(V.mod_id, syn_voltage);
    vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  } 
  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, syn_voltage * FUDGE_FACTOR, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_syn_out_dmm(V.mod_id, true);
  measurement = measure_dmm(V.mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measurement;
  log_result(mod_id, result);

  setup_syn_off(V.mod_id, 0.0);
  setup_syn_hv(V.mod_id, false);
  vth_ainit(V.mod_id);

  close_mux(V.mod_id);
  close_dmm(V.mod_id);
  close_syn(V.mod_id);
  CloseResourceManager();
  }

}  /* t16415 */

#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef SYN_WAIT_TIME
#undef MOA_COMP
#undef RESOLUTION
#undef FUDGE_FACTOR


/* MOA NEGATIVE OVER VOLTAGE LIMIT ACCURACY */

#define MIN_TEST_VOLTAGE  (-10.0e+00)
#define MAX_TEST_VOLTAGE  (-20.0e+00)
#define STARTING_INCREMENT  1024e-03
#define FINEST_INCREMENT  8e-03
#define SYN_WAIT_TIME   10e-03
#define MOA_COMP        5.0e+00
#define RESOLUTION      1e-03
#define FUDGE_FACTOR    1.2e+00
#define N6700B_CH1_MIN_TEST_VOLTAGE (10.0e+00)
#define N6700B_CH1_MAX_TEST_VOLTAGE (20.0e+00)
#define N6700B_CH1_CURR 500e-03
struct LOC_t9416 {
  int mod_id;
} ;
static void close_the_relays_____(struct LOC_t9416 *LINK)
{
  vth_aclose_rly(LINK->mod_id, K2605);   /* MOA happy relay */
  vth_aclose_rly(LINK->mod_id, K843);   /* MOA NON to GND */
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

void t16416(int mod_id, int /*slot*/)//9416
{
  
  int over_voltage_bit;
  double measurement;
  double syn_voltage;
  double n6700b_ch1_voltage;
  double voltage_increment;
  bool vlimit;
  bool ever_had_a_vlimit;
  struct LOC_t9416 V;

  a_word_type this_word;
  test_result_type result;
  V.mod_id = mod_id;
  OpenResourceManager();
  open_mux(V.mod_id);
  open_dmm(V.mod_id);
  open_syn(V.mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  if(check_syn_model())
  {
  open_n6700b(V.mod_id);
  connect_n6700b_ch1(V.mod_id);
  aconnect_n6700b_ch1_abus1_xmint(V.mod_id);
  vth_amoa_en_30ma_ilim(V.mod_id);
  vth_amoa_set_v_cmpl(V.mod_id, MOA_COMP);
  n6700b_ch1_voltage = N6700B_CH1_MIN_TEST_VOLTAGE;
  voltage_increment = N6700B_CH1_STARTING_INCREMENT;
  vlimit = false;
  ever_had_a_vlimit = false;
  close_the_relays_____(&V);
  setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
  setup_n6700b_ch1_curr(V.mod_id, N6700B_CH1_CURR);
  reverse_n6700b_ch1_out(V.mod_id);
  enable_n6700b_ch1_out(V.mod_id);
  vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
  vth_astat_clr(V.mod_id);

  while (!vlimit && n6700b_ch1_voltage <= N6700B_CH1_MAX_TEST_VOLTAGE)
  { /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      n6700b_ch1_voltage = N6700B_CH1_MAX_TEST_VOLTAGE + 1;
      continue;
    }

    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 11, 0);
    if (over_voltage_bit == 1)
    {  
      vlimit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= N6700B_CH1_FINEST_INCREMENT ||
          n6700b_ch1_voltage == N6700B_CH1_MIN_TEST_VOLTAGE)
        continue;
      vlimit = false;
      disable_n6700b_ch1_out(V.mod_id);
      vth_astat_clr(V.mod_id);   
      close_the_relays_____(&V);
      voltage_increment /= 2;
      n6700b_ch1_voltage -= voltage_increment;
      setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
	  enable_n6700b_ch1_out(V.mod_id);
      vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
      continue;
    }
	if ( ever_had_a_vlimit && !number_equal(voltage_increment, N6700B_CH1_FINEST_INCREMENT) )
      voltage_increment /= 2;
    
	n6700b_ch1_voltage += voltage_increment;
    setup_n6700b_ch1_volt(V.mod_id, n6700b_ch1_voltage);
    vth_mtmr_wait(V.mod_id, N6700B_CH1_WAIT_TIME);
  } 

  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, n6700b_ch1_voltage * FUDGE_FACTOR, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_n6700b_ch1_out_dmm(V.mod_id, true);
  measurement = measure_dmm(V.mod_id); 
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measurement;
  log_result(mod_id, result);

  vth_ainit(V.mod_id);
  close_mux(V.mod_id);
  close_dmm(V.mod_id);
  close_syn(V.mod_id);
  close_n6700b(V.mod_id);
  CloseResourceManager();
  }

  else
  {
  setup_syn(V.mod_id, DC, 0, 0.0, 0.0);
  setup_syn_hv(V.mod_id, true);
  aconnect_syn_out_xmint(V.mod_id);
  vth_amoa_en_30ma_ilim(V.mod_id);
  vth_amoa_set_v_cmpl(V.mod_id, MOA_COMP);
  syn_voltage = MIN_TEST_VOLTAGE;
  voltage_increment = STARTING_INCREMENT;
  vlimit = false;
  ever_had_a_vlimit = false;
  close_the_relays_____(&V);
  setup_syn_off(V.mod_id, syn_voltage);
  vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  vth_astat_clr(V.mod_id);

  while (!vlimit && syn_voltage >= MAX_TEST_VOLTAGE)
  { /* CHECK TO SEE IF A  VOLTAGE LIMIT HAS OCCURRED */
    vth_astat_rd(V.mod_id);
    if (!get_result_nolog(V.mod_id, &result))
    {
      display_prompt("No result in result log (Expected aim status)", true);
      syn_voltage = MAX_TEST_VOLTAGE - 1;
      continue;
    }

    this_word.data = result.UU.Integer_result;
    over_voltage_bit = P_getbits_US(this_word.bits, 11, 0);
    if (over_voltage_bit == 1)
    {  
      vlimit = true;
      ever_had_a_vlimit = true;
      if (voltage_increment <= FINEST_INCREMENT ||
          syn_voltage == MIN_TEST_VOLTAGE)
        continue;
      vlimit = false;
      setup_syn_off(V.mod_id, 0.0);
      vth_astat_clr(V.mod_id);   
      close_the_relays_____(&V);
      voltage_increment /= 2;
      syn_voltage += voltage_increment;
      setup_syn_off(V.mod_id, syn_voltage);
      vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
      continue;
    }
	if ( ever_had_a_vlimit && !number_equal(voltage_increment, FINEST_INCREMENT) )
      voltage_increment /= 2;
    
	syn_voltage -= voltage_increment;
    setup_syn_off(V.mod_id, syn_voltage);
    vth_mtmr_wait(V.mod_id, SYN_WAIT_TIME);
  } 

  /* MEASURE THE SYNTHESIZER VOLTAGE THAT CAUSED OVERVOLTAGE */
  setup_dmm(V.mod_id, DCV, syn_voltage * FUDGE_FACTOR, RESOLUTION);
  connect_dmm(V.mod_id);
  aconnect_syn_out_dmm(V.mod_id, true);
  measurement = measure_dmm(V.mod_id); 
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = measurement;
  log_result(mod_id, result);

  setup_syn_off(V.mod_id, 0.0);
  setup_syn_hv(V.mod_id, false);
  vth_ainit(V.mod_id);
  close_mux(V.mod_id);
  close_dmm(V.mod_id);
  close_syn(V.mod_id);
  CloseResourceManager();
  }

}  /* t16416 */

#undef MIN_TEST_VOLTAGE
#undef MAX_TEST_VOLTAGE
#undef STARTING_INCREMENT
#undef FINEST_INCREMENT
#undef SYN_WAIT_TIME
#undef MOA_COMP
#undef RESOLUTION
#undef FUDGE_FACTOR


/* MOA DC OUTPUT RESISTANCE */

#define CHANNEL         'A'
#define MOA_DEFAULT_RESISTANCE  0
#define FEEDBACK_RESISTOR_NO  0
#define MEAS_TYPE       0
#define POSITIVE_TEST_VOLTAGE  100e-03
#define NEGATIVE_TEST_VOLTAGE  (-100e-03)
#define RESOLUTION      1.0e-03
#define ENABLE_WAIT_TIME  1.0e-03
#define DMM_FUNCTION    DCV

void t16420(int mod_id, int /*slot*/)//9420
{
  
  double r10;
  double dmm_no_load_voltage;
  double dmm_loaded_voltage;
  double moa_resistance;
  test_result_type result;
  result.Measurement_class = CLASS_REAL;
  
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);

  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
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
  flush_commands(mod_id);
  dmm_no_load_voltage = measure_dmm(mod_id);
  vth_aaux_dis(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  vth_aclose_rly(mod_id, K822);
  flush_commands(mod_id);
  vth_aclose_rly(mod_id, K828);
  vth_aclose_rly(mod_id, K845);
  vth_aclose_rly(mod_id, K855);
  vth_aaux_en(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  flush_commands(mod_id);
  dmm_loaded_voltage = measure_dmm(mod_id);
  if (dmm_loaded_voltage != 0)
    moa_resistance = (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
  else
    moa_resistance = MOA_DEFAULT_RESISTANCE;

  result.UU.Real_result = moa_resistance;
  log_result(mod_id, result);
  vth_aaux_set_ampl(mod_id, NEGATIVE_TEST_VOLTAGE);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
  flush_commands(mod_id);
  dmm_loaded_voltage = measure_dmm(mod_id);
  vth_aaux_dis(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  flush_commands(mod_id);
  vth_aopen_rly(mod_id, K822);
  vth_aopen_rly(mod_id, K828);
  vth_aopen_rly(mod_id, K845);
  vth_aopen_rly(mod_id, K855);
  vth_aaux_en(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  flush_commands(mod_id);
  dmm_no_load_voltage = measure_dmm(mod_id);
  if (dmm_loaded_voltage != 0)
    moa_resistance = (dmm_no_load_voltage - dmm_loaded_voltage) / dmm_loaded_voltage * r10;
  else
    moa_resistance = MOA_DEFAULT_RESISTANCE;

  result.UU.Real_result = moa_resistance;
  log_result(mod_id, result);
  vth_aaux_dis(mod_id);
  vth_ainit(mod_id);

  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
}  /* t16420 */

#undef CHANNEL
#undef MOA_DEFAULT_RESISTANCE
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef POSITIVE_TEST_VOLTAGE
#undef NEGATIVE_TEST_VOLTAGE
#undef RESOLUTION

#undef ENABLE_WAIT_TIME
#undef DMM_FUNCTION


/* MOA MAXIMUM OUTPUT CURRENT (HIGH LEVEL ONLY) */

#define SOURCING		0
#define SINKING			1
#define FEEDBACK_RESISTOR_NO  0
#define MEAS_TYPE       1
#define LENGTH          1
#define AVERAGES        50
#define DIGITAL         0
#define TIMEOUT         10.0e+00
#define POWER_SUPPLY_CURRENT  800e-03
#define POWER_SUPPLY_RAMP_TIME  2.0e+00
#define MAX_EXPECTED_VOLTS  2.1e+00
typedef double i_direction_array_____[2];
typedef i_direction_array_____ voltage_array___________[2];
const voltage_array___________ SOURCE_VOLTAGE________ = 
{
  { -1.0e+00, 1.0e+00 },
  { -1.0e+00, 1.0e+00 }
};
const voltage_array___________ PS_VOLTAGE_____ = 
{
  { 7.0e+00, 9.0e+00 },
  { -9.0e+00, -7.0e+00 }
};

void t16427(int mod_id, int /*slot*/)//9427
{
  double r10;
  double max_current;
  test_result_type result;

  OpenResourceManager();
  open_ps(mod_id);
  open_mux(mod_id);

  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  vth_apowered_init(mod_id);
  r10 = get_cal_resistor_value(mod_id, FEEDBACK_RESISTOR_NO, MEAS_TYPE);
  aconnect_ps_20_ohm_load(mod_id, true);
  connect_power_supply_xmint(mod_id);
  vth_dcv_setup(mod_id, LENGTH, AVERAGES, DIGITAL, MAX_EXPECTED_VOLTS, TIMEOUT);
  vth_mtmr_wait(mod_id, SALES_RACK_WAIT);
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

  for (int src_polarity = RISING; src_polarity <= FALLING; ++src_polarity)
  {  /* SETUP FOR POSITIVE AND NEGATIVE SOURCE VOLTAGES */
    if (src_polarity == RISING)
    {  /* CONNECT PS + TO X1 AND PS - TO X2 */
      vth_aopen_rly(mod_id, K726);
      vth_aopen_rly(mod_id, K717);
      vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS); 
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K725);
      vth_aclose_rly(mod_id, K718);
    }  /* CONNECT PS + TO X1 AND PS - TO X2 */
    else
    {  /* CONNECT PS + TO X2 AND PS - TO X1 */
      vth_aopen_rly(mod_id, K725);
      vth_aopen_rly(mod_id, K718);
      vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS); 
      /* the supply doesn't get shorted out */
      vth_aclose_rly(mod_id, K726);
      vth_aclose_rly(mod_id, K717);
    }  /* CONNECT PS + TO X2 AND PS - TO X1 */

    for (int direction = SOURCING; direction <= SINKING; ++direction)
    {  /* SETUP FOR SOURCING AND SINKING CONDITIONS */
      vth_asrc_set_dc_ampl(mod_id, SOURCE_VOLTAGE________[src_polarity][direction]);
      vth_afc_en_src_out(mod_id);
      setup_ps(mod_id, PS_VOLTAGE_____[src_polarity][direction], POWER_SUPPLY_CURRENT);
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
  CloseResourceManager();

}  /* t16427 */

#undef SOURCING
#undef SINKING
#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef LENGTH
#undef AVERAGES
#undef DIGITAL
#undef TIMEOUT
#undef POWER_SUPPLY_CURRENT
#undef POWER_SUPPLY_RAMP_TIME
#undef MAX_EXPECTED_VOLTS


/* DC DETECTOR VOLTAGE ACCURACY */

void t16456(int mod_id, int slot)//9456
{
	int divisor = 1;
	double range_voltage = 5.0e+00;
    
	OpenResourceManager();
	open_dmm(mod_id);
    open_mux(mod_id);
    vth_cfxt_lock(); 
    vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	flush_commands(mod_id);
	connect_dmm(mod_id);
    connect_plus_calib1(mod_id);
    connect_minus_calib2(mod_id);

	for(int range_number = 7; range_number >= 0; range_number--)
	{   
		range_voltage = range_voltage / divisor;
	    test_detector_range(mod_id, range_voltage, range_number, divisor);
		divisor++;
	}

	vth_ainit(mod_id);
	close_dmm(mod_id);
    close_mux(mod_id);
	CloseResourceManager();
} /* t16456 */


/* AC DETECTOR (TRUE RMS) VOLTAGE ACCURACY */

#define NUM_POINTS_TO_CHECK  6
#define AVERAGES			10
#define DIGITAL				0
#define LOW_BAND_RATIO		0.8e+00
#define HI_BAND_RATIO		1.2e+00
#define TIMEOUT				10.0e+00
#define PEAK_AMPLITUDE		10.0e+00
#define OFFSET				0.0e+00
#define RESOLUTION			1e-03
#define RANGE				10
#define FAST				0
#define NORMAL_				1

void t16465(int mod_id, int /*slot*/)//9465
{
  int start_frequency;
  int stop_frequency;
  int freq_array[2][NUM_POINTS_TO_CHECK] = 
  {
	  { 50, 200, 400, 5000, 10000, 19000 },
	  { 10, 50, 300, 3000, 10000, 19000 }
  };
  double rms_amplitude;
  double peak_to_peak_amplitude;
  double dmm_actual_voltage;
  double detector_voltage;
  double v_delta;

  test_result_type result;
  OpenResourceManager();
  open_syn(mod_id);
  open_dmm50(mod_id);
//  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  if(check_syn_model())
  {
  /*Connect the synthesizer output to the X1 and X2 and the CALIB lines to dmm*/
  setup_33250A_defaults(mod_id);
  setup_33250A_hv(mod_id, true);
  connect_dmm50_out_af2(mod_id);
  aconnect_syn_out_xmint(mod_id);

  /* CLOSE THE XMINT TO DET PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K725);   /* X1 bus to I bus relay */
  vth_aclose_rly(mod_id, K733);	  /* X1 to S */
  vth_aclose_rly(mod_id, K718);   /* X2 bus to L bus relay */
  vth_aclose_rly(mod_id, K751);	  /* S to XL */
  vth_aclose_rly(mod_id, K764);	  /* XL to AF2 */

  /* CLOSE THE I to CALIB2 and L to CALIB1 relays 
  vth_aclose_rly(mod_id, K855);   // I to CALIB2 bus relay 
  vth_aclose_rly(mod_id, K759);   // L to LOUT/CALIB1 relay*/

  rms_amplitude = PEAK_AMPLITUDE / sqrt(2.0);
  peak_to_peak_amplitude = PEAK_AMPLITUDE * 2;
  setup_dmm50(mod_id, ACV, RANGE, RESOLUTION);
  for (int mode = FAST; mode <= NORMAL_; ++mode)
  {  /* for mode */
    for (int frequency = 0; frequency < NUM_POINTS_TO_CHECK; ++frequency)
    {  /*for frequency*/
      setup_33250A(mod_id, SINE, freq_array[mode]
                [frequency], peak_to_peak_amplitude, OFFSET);
	  enable_33250A_out(mod_id); 
      vth_acv_setup(mod_id, rms_amplitude, mode, AVERAGES, DIGITAL, TIMEOUT);
      vth_aclose_rly(mod_id, K857);   /* close L to ACOM */
      vth_mtmr_wait(mod_id, WAIT_TIME);   /* wait for synthesizer */
      start_frequency = static_cast<int>(freq_array[mode][frequency] * LOW_BAND_RATIO);
      stop_frequency = static_cast<int>(freq_array[mode][frequency] * HI_BAND_RATIO);
      setup_dmm50_ac_band(mod_id, start_frequency, stop_frequency);
	  get_result_nolog(mod_id, &result);
      dmm_actual_voltage = measure_dmm50(mod_id);
      if (dmm_actual_voltage >= DMM_VALID_RESULT)
        display_prompt("No result in result log (Expected dmm vth_reading)", true);

      vth_afunc_meas(mod_id);
      vth_get_afunc_result(mod_id);
      if (get_result_nolog(mod_id, &result))
      {  /* process result */
        detector_voltage = result.UU.Real_result;
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
  close_syn(mod_id);
  close_dmm50(mod_id);
//  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
  }

  else
  {
  /*Connect the synthesizer output to the X1 and X2 and the CALIB lines to dmm*/
  setup_syn_hv(mod_id, true);
  connect_dmm50_out_af2(mod_id);
  aconnect_syn_out_xmint(mod_id);

  /* CLOSE THE XMINT TO DET PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K725);   /* X1 bus to I bus relay */
  vth_aclose_rly(mod_id, K733);	  /* X1 to S */
  vth_aclose_rly(mod_id, K718);   /* X2 bus to L bus relay */
  vth_aclose_rly(mod_id, K751);	  /* S to XL */
  vth_aclose_rly(mod_id, K764);	  /* XL to AF2 */

  /* CLOSE THE I to CALIB2 and L to CALIB1 relays 
  vth_aclose_rly(mod_id, K855);   // I to CALIB2 bus relay 
  vth_aclose_rly(mod_id, K759);   // L to LOUT/CALIB1 relay*/

  rms_amplitude = PEAK_AMPLITUDE / sqrt(2.0);
  peak_to_peak_amplitude = PEAK_AMPLITUDE * 2;
  setup_dmm50(mod_id, ACV, RANGE, RESOLUTION);
  for (int mode = FAST; mode <= NORMAL_; ++mode)
  {  /* for mode */
    for (int frequency = 0; frequency < NUM_POINTS_TO_CHECK; ++frequency)
    {  /*for frequency*/
      setup_syn(mod_id, SINE, freq_array[mode]
                [frequency], peak_to_peak_amplitude, OFFSET);
      vth_acv_setup(mod_id, rms_amplitude, mode, AVERAGES, DIGITAL, TIMEOUT);
      vth_aclose_rly(mod_id, K857);   /* close L to ACOM */
      vth_mtmr_wait(mod_id, WAIT_TIME);   /* wait for synthesizer */
      start_frequency = static_cast<int>(freq_array[mode][frequency] * LOW_BAND_RATIO);
      stop_frequency = static_cast<int>(freq_array[mode][frequency] * HI_BAND_RATIO);
      setup_dmm50_ac_band(mod_id, start_frequency, stop_frequency);
	  get_result_nolog(mod_id, &result);
      dmm_actual_voltage = measure_dmm50(mod_id);
      if (dmm_actual_voltage >= DMM_VALID_RESULT)
        display_prompt("No result in result log (Expected dmm vth_reading)", true);

      vth_afunc_meas(mod_id);
      vth_get_afunc_result(mod_id);
      if (get_result_nolog(mod_id, &result))
      {  /* process result */
        detector_voltage = result.UU.Real_result;
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
  close_dmm50(mod_id);
//  close_dmm(mod_id);
  close_mux(mod_id);
  CloseResourceManager();
  }
}  /* t16465 */


#undef NUM_POINTS_TO_CHECK
#undef FAST
#undef NORMAL_
#undef AVERAGES
#undef DIGITAL
#undef LOW_BAND_RATIO
#undef HI_BAND_RATIO
#undef TIMEOUT
#undef PEAK_AMPLITUDE
#undef OFFSET
#undef RESOLUTION


/* AC DETECTOR (TRUE RMS) VOLTAGE ACCURACY OVER VOLTAGE */

#define AVERAGES        10
#define DIGITAL         0
#define MEAS_MODE       0
#define FREQUENCY_      16384
#define LOW_BAND_RATIO  0.8e+00
#define HI_BAND_RATIO   1.2e+00
#define TIMEOUT         10.0e+00
#define MAX_RMS_AMPLITUDE  7.0710678
#define OFFSET          0.0e+00
#define SRC_I_COMPLIANCE  1
#define SRC_R_OUT       0
#define SRC_ON          1
#define SRC_CONTROL     0
#define FULL            0
#define HALF            1

void t16466(int mod_id, int /*slot*/)//9466
{
  int start_frequency;
  int stop_frequency;
  double rms_voltage;
  double dmm_rms_range[9] = {10, 5, 5, 1, 1, 1, 1, .1, .1};
  double det_rms_range;
  double dmm_actual_voltage;
  double detector_voltage;
  double v_delta;
  double resolution[9] = {100e-03, 100e-03, 100e-03, 100e-03, 10e-03, 
							10e-03, 10e-03, 1e-03, 1e-03};
  test_result_type result;

  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  connect_dmm(mod_id);
  connect_plus_calib1(mod_id);
  connect_minus_calib2(mod_id);

  /*Connect SRCOUT to DETHI and CALIB1*/
  vth_aclose_rly(mod_id, K836);
  aconnect_srcout_calib1(mod_id);
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
      det_rms_range = rms_voltage;
      if (scale == HALF)
        rms_voltage /= 2;
	
      /* setup the source */
      vth_src_sine_setup(mod_id, FREQUENCY_, rms_voltage, OFFSET,
                     SRC_I_COMPLIANCE, SRC_R_OUT, SRC_ON, SRC_CONTROL);
      vth_aopen_rly(mod_id, K806);   /* remove SRCOUT from S */
      setup_dmm(mod_id, ACV, dmm_rms_range[range], resolution[range]);

      /* setup the detector */
      vth_acv_setup(mod_id, det_rms_range, MEAS_MODE, AVERAGES, DIGITAL, TIMEOUT);
      vth_aopen_rly(mod_id, K812);   /* remove DETHI from I */
      vth_aclose_rly(mod_id, K857);   /* L to ACOM */
      vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
	  flush_commands(mod_id);
      dmm_actual_voltage = measure_dmm(mod_id);
      if (dmm_actual_voltage >= DMM_VALID_RESULT)
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
  CloseResourceManager();

}  /* t16466 */

#undef AVERAGES
#undef DIGITAL
#undef MEAS_MODE
#undef FREQUENCY_
#undef LOW_BAND_RATIO
#undef HI_BAND_RATIO
#undef TIMEOUT
#undef MAX_RMS_AMPLITUDE
#undef OFFSET
#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef FULL
#undef HALF



#define FEEDBACK_RESISTOR_NO  5
#define MEAS_TYPE       0
#define DEFAULT_DET_RESISTANCE  0
#define NPLC            10
#define TEST_VOLTAGE____  10e+00
#define RESOLUTION      1.0e-03
#define RDMM            10e+10
#define ENABLE_WAIT_TIME  1.0e-03
#define CHANNEL         'A'
#define DMM_FUNCTION    DCV

void t16470(int mod_id, int /*slot*/)//9470
{
  /* DC DETECTOR INPUT RESISTANCE */
  double r1m;
  double dmm_1m_voltage;
  double dmm_1m_plus_det_voltage;
  double detector_input_resistance;
  test_result_type result;
  result.Measurement_class = CLASS_REAL;
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
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
  flush_commands(mod_id);
  dmm_1m_voltage = measure_dmm(mod_id);
  if (dmm_1m_voltage > DMM_VALID_RESULT)
    display_prompt("No result in result log (Expected dmm vth_reading)", true);

  vth_afc_dis_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  vth_aclose_rly(mod_id, K812);   /*DETHI connect relay*/
  vth_aopen_rly(mod_id, K2304);   /*DETHI input ground relay*/
  vth_aclose_rly(mod_id, K2306);   /*DETHI input relay*/
  vth_aclose_rly(mod_id, K2303);   /*DETLO input ground relay*/
  vth_afc_en_src_out(mod_id);
  vth_mtmr_wait(mod_id, ENABLE_WAIT_TIME);
  flush_commands(mod_id);
  dmm_1m_plus_det_voltage = measure_dmm(mod_id);
  if (dmm_1m_plus_det_voltage < DMM_VALID_RESULT)
  {  /*measure the loaded voltage, calculate resistance and log*/
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
  flush_commands(mod_id);
  dmm_1m_plus_det_voltage = measure_dmm(mod_id);
  if (dmm_1m_plus_det_voltage < DMM_VALID_RESULT)
  {  /*measure the loaded voltage, calculate resistance and log*/
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
  CloseResourceManager();
}  /* t16470 */

#undef FEEDBACK_RESISTOR_NO
#undef MEAS_TYPE
#undef DEFAULT_DET_RESISTANCE
#undef NPLC
#undef TEST_VOLTAGE____
#undef RESOLUTION
#undef RDMM

#undef ENABLE_WAIT_TIME
#undef CHANNEL
#undef DMM_FUNCTION



#define LOW_RANGE       0
#define HIGH_RANGE      8
#define INT_TIME        1   
#define AVERAGES        10  
#define DIGITAL         0   
#define EXP_COM_MODE_OUT  10.0e+00   /* MAX COMMON MODE OUTPUT VOLTAGE */
#define TIMEOUT         10.0000e+00   /* SECONDS */
#define TEST_VOLTAGE____  10.0000e+00
#define MIN_VOLTAGE_RANGE  39.0625e-03   /* MINIMUM DETECTOR VOLTAGE RANGE */
#define DETECTOR_RESOLUTION  0.00005e+00   /* PER THE SPEC SHEET */
#define VDET_OUT_STD_DELTA  20.0000e+00   /* INPUT VOLTAGE DELTA */
#define CMRR_MAX_VALUE  160.000e+00   /* MAXIMUM CMRR NUMBER */

void t16471(int mod_id, int /*slot*/)//9471
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

  for (range = LOW_RANGE; range <= HIGH_RANGE; range++)
  {
    vth_adet_sel_rng(mod_id, range);
    vdet_out_minus_cmode = -99.0;
    vth_asrc_set_dc_ampl(mod_id, -1 * TEST_VOLTAGE____);
    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
    vth_afunc_meas(mod_id);			/* TAKE - COM MODE MEAS */
    vth_get_afunc_result(mod_id); 
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

}  /* t16471 */

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




#define LOW_RANGE       6
#define HIGH_RANGE      8
#define INT_TIME        1   
#define AVERAGES        10  
#define DIGITAL         0   
#define EXP_COM_MODE_OUT  160.0e+00   /* MAX COMMON MODE OUTPUT VOLTAGE */
#define TIMEOUT         10.0000e+00   /* SECONDS */
#define TEST_VOLTAGE____  10.0000e+00
#define MIN_VOLTAGE_RANGE  39.0625e-03   /* MINIMUM DETECTOR VOLTAGE RANGE */
#define DETECTOR_RESOLUTION  0.00005e+00   /* PER THE SPEC SHEET */
#define VDET_OUT_STD_DELTA  20.0000e+00   /* INPUT VOLTAGE DELTA */
#define CMRR_MAX_VALUE  160.000e+00   /* MAXIMUM CMRR NUMBER */
#define ATTEN_FACTOR    16.0000e+00   /* ATTENUATION MULTIPLIER */


void t16472(int mod_id, int /*slot*/)//9472
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
}  /* t16472 */

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

#undef ATTEN_FACTOR
#undef IM_NEG



#define LOW             0
#define HIGH            1
#define IM_POS          1   /* imaginary integrate */
#define IM_NEG          2   /* other half cycle */
#define EXTRA_DIGIT     1   /* use long integrate */
#define FREQ_TO_5TAU    (5.0 / 2.0 / 3.14159)
typedef double det_volts[9];
typedef int det_relay[9];
typedef det_volts src_volts[2];
typedef det_relay src_load[2];
const src_volts V_SOURCE = 
{
  { 0.028, 0.055, 0.110, 0.177, 0.177, 0.884, 1.768, 1.768, 7.071 },
  { 0.028, 0.055, 0.110, 0.221, 0.442, 0.884, 1.061, 3.536, 7.071 }
};
const src_load LOAD_RLY = 
{
  { K845, K845, K845, K845, K845, K846, K846, K846, K847 },
  { K845, K845, K845, K845, K845, K845, K845, K846, K846 }
};

void t16475(int mod_id, int /*slot*/)//9475
{
  /* Quadrature Rejection Loaded Source */
  int freq_idx;
  int freq;
  double quad_pos;
  double quad_neg;
  double quad_voltage;
  double ratio;
  double db;
  test_result_type result;

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
}  /*t16475*/

#undef LOW
#undef HIGH
#undef IM_POS
#undef IM_NEG
#undef EXTRA_DIGIT
#undef FREQ_TO_5TAU
#undef R156
#undef R312



#define NO_             0
#define HIGH            2
#define R156            2
#define R312            3
#define IM_POS          1   /* imaginary integrate */
#define IM_NEG          2   /* other half cycle */
#define EXTRA_DIGIT     1   /* use long integrate */
#define FREQ_TO_5TAU    (5.0 / 2.0 / 3.14159)
typedef double src_volts_[4];
const src_volts_ V_SOURCE_ = 
{
  0.110, 0.100, 0.100, 0.110
};

void t16476(int mod_id, int /*slot*/)//9476
{
  /* Source Crossover Quadrature Rejection */
  int freq_idx;
  int condition;
  int freq;
  double quad_pos;
  double quad_neg;
  double quad_voltage;
  double ratio;
  double db;
  test_result_type result;

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
}  /*t16476*/

#undef NO_
#undef HIGH
#undef R156
#undef R312
#undef IM_POS
#undef IM_NEG
#undef EXTRA_DIGIT
#undef FREQ_TO_5TAU


/* DC DIGITIZER VOLTAGE ACCURACY */

#define NUM_SAMPLES     100
#define TRIG_MODE       0
#define MODE            0
#define TRIG_TYPE       0
#define TRIG_THRESHOLD  0.0
#define TIMEOUT        10.0
#define PERIOD_         100e-06
typedef int sign_array[2];
typedef double range_array___[9];
const range_array___ VOLTAGE = {39.0625e-03, 78.125e-03, 156.25e-03, 312.5e-03, 
									625e-03, 1.25e00, 2.5e00, 5e00, 10e00};
const sign_array DIRECTION = {-1, 1};

void t16480(int mod_id, int /*slot*/)//9480
{
  int range;
  int scale;
  int sign;
  int i;
  double dmmrange[9] = {.1, .1, 1, 1, 1, 5, 10, 10, 100};
  double dc_amplitude = 1.0e-03;
  double dc_src_voltage;
  double dmm_actual_voltage;
  double sum;
  double mean;
  double delta;

  test_result_type result;
  OpenResourceManager();
  open_dmm(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock(); 
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

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
      setup_dmm(mod_id, DCV, dmmrange[range], dc_amplitude);
      for (sign = 0; sign <= 1; sign++)
      {  /* positve to negative */
        dc_src_voltage = dc_amplitude * DIRECTION[sign];
        vth_asrc_set_dc_ampl(mod_id, dc_src_voltage);
        vth_aclose_rly(mod_id, K837);   /*SRCOUT to RCAL2*/
        vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
		flush_commands(mod_id);
        dmm_actual_voltage = measure_dmm(mod_id);
        if (dmm_actual_voltage > DMM_VALID_RESULT)
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
  CloseResourceManager();
}  /* t16480 */

#undef NUM_SAMPLES
#undef TRIG_MODE
#undef MODE
#undef TRIG_TYPE
#undef TRIG_THRESHOLD
#undef TIMEOUT
#undef PERIOD_


/* DC DIGITIZER OFFSET AND NOISE OVER RANGES */

#define NUM_SAMPLES		100
#define TRIG_MODE       0
#define MODE            0
#define TRIG_TYPE       0
#define TRIG_THRESHOLD  0.0
#define TIMEOUT			10.0
#define X               6
#define PERIOD_         100e-06
typedef double range_array____[9];
const range_array____ VOLTAGE_ = {39.0625e-03, 78.125e-03, 156.25e-03, 312.5e-03, 
									625e-03, 1.25e00, 2.5e00,  5e00, 10e00};

void t16481(int mod_id, int /*slot*/)//9481
{
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

}  /* t16481 */

#undef NUM_SAMPLES
#undef FREQUENCY_
#undef TRIG_MODE
#undef MODE
#undef TRIG_TYPE
#undef TRIG_THRESHOLD
#undef TIMEOUT
#undef X
#undef PERIOD_



#define NUM_SAMPLES     100
#define TRIG_MODE       0
#define MODE            0
#define TRIG_TYPE       0
#define TRIG_THRESHOLD  0.0
#define TIMEOUT        10.0
#define ZERO_INPUT      0.0
#define X               6
#define NUM_MEAS        8
typedef double period_array[NUM_MEAS];
const period_array PERIOD_ = 
{
  25e-06, 250e-06, 1e-03, 2.5e-03, 10e-3, 16.6667e-03, 20e-03, 31.2e-03
};

void t16482(int mod_id, int /*slot*/)//9482
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

}  /* t16482 */

#undef NUM_SAMPLES
#undef TRIG_MODE
#undef MODE
#undef TRIG_TYPE
#undef TRIG_THRESHOLD
#undef TIMEOUT
#undef ZERO_INPUT
#undef X
#undef NUM_MEAS