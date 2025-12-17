#include <p2c/p2c.h>

#include <assert.h>

#include <time.h>

#include <stdio.h>



#include "mt9500_p.h"

#include "digital_const_p.h"

#include "../../../uCC/src/future.h"

#include "../../../vmc/src/vmc.h"

#include "../../../u/src/num_p.h"

#include "../../../u/src/OSLayer.h"



#define OSC_CHANNEL     1

#define COUPLING        1000000   /* 1 Meg ohm */

#define CHANNEL_RANGE   8.0e+00   /* 1 Volt per div */

#define CHANNEL_OFFSET  2.0e+00

#define TRIGGER_LEVEL   2.5e+00

#define TIMEBASE_RANGE  20.0e-09   /* 2 nSec per div */

#define TIMEBASE_OFFSET  0.0e+00

#define TIMEOUT         1.0e-03

#define INPUT_FREQ      20.0e+06

/* divide by 32 */

#define TCLK_FREQ       625e+03

#define AVERAGING       true

#define COUNT           2   /* number averages */

#define TIMEBASE_MODE   AUTOMATIC

static void ss_slew_setup(int mod_id, int edge)

{

  vth_mhpib_wr(mod_id, 0, "DISPLAY TMARKER ON");

  vth_mhpib_wr(mod_id, 0, "DISPLAY VMARKER ON");

  vth_mhpib_wr(mod_id, 0, "MENU 7");   /*select delta T menu*/

  switch (edge)

  {



  case POS:

    vth_mhpib_wr(mod_id, 0, "MEASURE VSTART 0.5 VSTOP 1.5");

    vth_mhpib_wr(mod_id, 0, "ESTA+1 ESTO+1");

    break;



  case NEG:

    vth_mhpib_wr(mod_id, 0, "MEASURE VSTART 4.5 VSTOP 3.5");

    vth_mhpib_wr(mod_id, 0, "ESTA-1 ESTO-1");

    break;

  default:

    assert(false);

    break;



  }

}



static void ss_slew_meas(int mod_id)

{

  vth_mhpib_wr(mod_id, 0, "KEY 10");   /*front panel function "find edge"*/

  vth_mhpib_wr(mod_id, 0, "MEASURE TDEL?");

  vth_mhpib_num_rd(mod_id, 0);

}



void t9500(void)

{

  time_t now;

  struct tm *local,format;

  int mod_id;

  char   id_string[MAX_S_LENGTH + 1];

  char *s_ptr;

  char   str1[256],date_str[256],time_str[256];

  test_result_type bresult;

  test_result_type hpib;

  FILE *infile;

  infile = NULL;



  mod_id = any_module_present();

  bresult.Measurement_class = CLASS_INTEGER;

  bresult.UU.Integer_result = 1;



// getting current date and time.

  now=time(NULL);

  local=localtime(&now);

  format=*local;

  sprintf(time_str,"SYST:TIME %d,%d,%d",format.tm_hour,format.tm_min,format.tm_sec);

  sprintf(date_str,"SYST:DATE %d,%d,%d",format.tm_year+1900,format.tm_mon+1,format.tm_mday);

// end getting current date and time.



  infile = fopen("config.borg", "r");

  if (infile != NULL)

  {

// checking E1406A identification number.

    open_com(mod_id);

    vth_mhpib_wr(mod_id, 9, "*CLS");

    vth_mhpib_wr(mod_id, 9, "*RST");

    vth_mhpib_wr(mod_id, 9, "*IDN?");

    vth_mhpib_rd(mod_id, 9);

    if(get_result_nolog(mod_id, &hpib))

    {

      s_ptr=static_cast(char*, hpib.UU.Pstring_ptr);

      strcpy(id_string,s_ptr);

    }

    else

      display_prompt("No result in result log - Expected string value", true);

    sprintf(str1,"%.22s", id_string);

//    if (!strcmp(str1, "Agilent Technologies,E1406A"))

    if (!strcmp(str1, "HEWLETT-PACKARD,E1406A"))

      bresult.UU.Integer_result = 0;

    else

    {

      display_prompt("   COMMAND MODULE ERROR!!!!!", true);

      display_prompt(id_string, true);

      bresult.UU.Integer_result = 1;

    }



    log_result(mod_id, bresult);

// end checking E1406A identification number.

// running selftest on E1406A.

    vth_mhpib_wr(mod_id, 9, "*CLS");

    vth_mhpib_wr(mod_id, 9, "*RST");

    vth_mhpib_wr(mod_id, 9, "*TST?");

    vth_mhpib_rd(mod_id, 9);

    if(get_result_nolog(mod_id, &hpib))

    {

      s_ptr=static_cast(char*, hpib.UU.Pstring_ptr);

      strcpy(id_string,s_ptr);

    }

    else

      display_prompt("No result in result log - Expected string value", true);

    sprintf(str1,"%.2s", id_string);

    if (!strcmp(str1, "+0"))

      bresult.UU.Integer_result = 0;

    else

    {

      display_prompt("   COMMAND MODULE ERROR!!!!!", true);

      display_prompt(id_string, true);

      bresult.UU.Integer_result = 1;

    }



    log_result(mod_id, bresult);

// end running selftest on E1406A.

// setting date on E1406A.

    vth_mhpib_wr(mod_id, 9, date_str);

    vth_mhpib_wr(mod_id, 9, "SYST:ERR?");

    vth_mhpib_rd(mod_id, 9);

    if(get_result_nolog(mod_id, &hpib))

    {

      s_ptr=static_cast(char*, hpib.UU.Pstring_ptr);

      strcpy(id_string,s_ptr);

    }

    else

      display_prompt("No result in result log - Expected string value", true);

    sprintf(str1,"%.2s", id_string);

    if (!strcmp(str1, "+0"))

      bresult.UU.Integer_result = 0;

    else

    {

      display_prompt("   COMMAND MODULE ERROR!!!!!", true);

      display_prompt(id_string, true);

      bresult.UU.Integer_result = 1;

    }



    log_result(mod_id, bresult);

// end setting date on E1406A.

// setting date on E1406A.

    vth_mhpib_wr(mod_id, 9, time_str);

    vth_mhpib_wr(mod_id, 9, "SYST:ERR?");

    vth_mhpib_rd(mod_id, 9);

    if(get_result_nolog(mod_id, &hpib))

    {

      s_ptr=static_cast(char*, hpib.UU.Pstring_ptr);

      strcpy(id_string,s_ptr);

    }

    else

      display_prompt("No result in result log - Expected string value", true);

    sprintf(str1,"%.2s", id_string);

    if (!strcmp(str1, "+0"))

      bresult.UU.Integer_result = 0;

    else

    {

      display_prompt("   COMMAND MODULE ERROR!!!!!", true);

      display_prompt(id_string, true);

      bresult.UU.Integer_result = 1;

    }



    log_result(mod_id, bresult);

// end setting time on E1406A.

    close_com(mod_id);

  }

  else

    display_prompt("Not a borg system.",true);

  if (infile != NULL)

    fclose(infile);

  infile = NULL;

}  /* t9500 */



void t9605(int mod_id, int slot)

{

  /*DRIVER OUTPUT SLEW RATE (400 V/uSec minimum)*/



  /*2 Aug 89 replace scope delta v vs delta t routines with Bryan's "arb"

           routine.*/

  double slew_time;

  double slew_rate;

  int driver;

  test_result_type result;

  int ssrly_page;



  open_osc(mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);



  connect_osc_ch1_af2(mod_id);



  setup_osc_channel(mod_id, OSC_CHANNEL, COUPLING, CHANNEL_RANGE,

                    CHANNEL_OFFSET);

  setup_osc_timebase(mod_id, AVERAGING, COUNT, TIMEBASE_RANGE,

                     TIMEBASE_OFFSET, TIMEBASE_MODE);

  vth_aclose_rly(mod_id, K753);   /* XL to AF2 relay      */

  vth_aclose_rly(mod_id, K764);   /* XL to AF2 relay      */

  vth_pclose_rly(mod_id, slot, KDCL);   /* XL common connection */

  for (ssrly_page = 0; ssrly_page <= 2; ssrly_page++)

  {  /*ssrly_page*/

    relay_map(mod_id, slot, ssrly_page);   /*selects "relay page"*/

    for (driver = 'A'; driver <= 'H'; driver++)

      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[driver - 'A']);

  }  /*ssrly_page*/



  vth_mtclk_clk_20m_mux(mod_id);

  vth_mtclk_set_freq(mod_id, INPUT_FREQ, TCLK_FREQ);



  sales_reset_sequencer(mod_id, slot);



  break_test_for_function_step_compatability(mod_id);



#include "t9605_vml.cmd"



  for (int edge = POS; edge >= NEG; --edge)

  {  /* for edge */

    setup_osc_trigger(mod_id, OSC_CHANNEL, edge, TRIGGER_LEVEL, AVERAGING);

    ss_slew_setup(mod_id, edge);



    for (ssrly_page = 0; ssrly_page <= 2; ssrly_page++)

    {  /*ssrly_page*/

      relay_map(mod_id, slot, ssrly_page);   /*selects "relay page"*/

      for (driver = 'A'; driver <= 'H'; driver++)

      {  /* for driver */

        slew_rate = 0.0;

        slew_time = 1.0;



        vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);

        sales_start_sequencer(mod_id, slot);



        trigger_osc(mod_id, OSC_CHANNEL, AVERAGING);

        ss_slew_meas(mod_id);

        if (get_result_nolog(mod_id, &result))

          slew_time = result.UU.Real_result;

        else

          display_prompt("Unable to vth_read rise time from oscilloscope.", true);



        vth_mseq_stop(mod_id, TIMEOUT);

        vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[driver - 'A']);



        if (slew_time == 0)   /*prevent div by 0*/

          slew_time = INT_MAX;



        slew_rate = 1 / slew_time / 1e+06;

            /* delta V=1volt .. volts/uSec. */

        result.UU.Real_result = slew_rate;

        log_result(mod_id, result);

      }  /* for driver */



    }  /*for ssrly_page*/

  }  /* for edge */





  vth_pinit(mod_id, slot);

  vth_ainit(mod_id);



  close_osc(mod_id);

  close_34980A(mod_id);//close_mux(mod_id);



}  /*t9605*/

#undef OSC_CHANNEL

#undef COUPLING

#undef CHANNEL_RANGE

#undef CHANNEL_OFFSET

#undef TRIGGER_LEVEL

#undef TIMEBASE_RANGE

#undef TIMEBASE_OFFSET

#undef TIMEOUT

#undef INPUT_FREQ

#undef TCLK_FREQ

#undef AVERAGING

#undef COUNT

#undef TIMEBASE_MODE



typedef int delay_type;

typedef double delay_array[2];

typedef double cntr_trg_level_array[4];

#define RISING_EDGE     1

#define TCLK_FREQ       5.0e+06

#define GEN_LOW         0.0e+00

#define GEN_HIGH_TERMINATED  3.0e+00

#define TIMEOUT         1.0e-03

const delay_array GEN_DELAY = {85e-09, 185e-09};

const delay_array PROG_DELAY = {0e-09, 100e-09};

const cntr_trg_level_array CNTR_TRG_LEVEL = {0.0, 1.4, 1.0, 0.0};

void t9620(int mod_id, int slot)

{  /*tss20*/

  /*SSDRIVER EDGE PLACEMENT ACCURACY AND SKEW*/

  /*2may90 - added cntr trg level prgming for strt ch (dvr UT)*/

  /*31may90- put the seq rst immed b4 seq run to get repeatable performance*/

  /*28jun90- changed to internal array reading for drvr & slot constants;

            added the rising edge correction factor which is the last entry

            in the "const_ssd" file.   Test 9505 is no longer a prereq.    */

  /*4sep90- moved const reading routines to t9005*/

  delay_type offset_cal;

  delay_type programmed_delays;



  double drvr_delay_actual;

  double skew;



  double offset[2];

  double ch_delta[24][4];



  test_result_type result;



  int sschanl;

  int ssdrvr;

  int ssvirt_card;

  int ssrly_page;

  OpenResourceManager();
  open_33250A(mod_id);

  /*Added by chuanho to select syn model, 15jun2012*/
  /*--------------------------------------------*/
  if(check_syn_model())
  {
	enable_33250A_out(mod_id);
	setup_33250A_defaults(mod_id);
  }
  else
      open_syn(mod_id);
  /*--------------------------------------------*/


  open_gen(mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);

  open_cntr(mod_id);

  connect_syn_out_gen_ext(mod_id);

  setup_gen_levels(mod_id, GEN_LOW, GEN_HIGH_TERMINATED);



  connect_gen_trg_cntr_start(mod_id);

  connect_gen_out_cntr_stop(mod_id);



  for (offset_cal = 0; offset_cal <= 1; offset_cal++)

  {  /*offset_cal*/

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



  for (ssrly_page = 0; ssrly_page <= 2; ssrly_page++)

  {  /*ssrly_page*/

    relay_map(mod_id, slot, ssrly_page);   /*selects "relay page"*/



    for (sschanl = 'A'; sschanl <= 'H'; sschanl++)

    {  /*sschanl*/

      vth_pclose_rly(mod_id, slot, PIN_KD_RLYS[sschanl - 'A']);

    }  /*sschanl*/

  }  /*ssrly_page*/



  vth_pclose_rly(mod_id, slot, KDCL);



  vth_mhsl_1_init_primary(mod_id);

  all_modules_but_me(mod_id, &vth_mhsl_1_init_passive);

  vth_mhsl_2_init_primary(mod_id);

  all_modules_but_me(mod_id, &vth_mhsl_2_init_passive);

  vth_mdig_set_clkrb_primary(mod_id, TCLK_FREQ, TCLK_FREQ, RISING_EDGE);



  for (programmed_delays = 0; programmed_delays <= 1; programmed_delays++)

  {  /* for programmed_delays */



    if (programmed_delays == 0)

    {

#include "t9620_nd.cmd"

    }

    else

    {

#include "t9620_d.cmd"

    }



    setup_gen_trg_out_delay(mod_id, GEN_DELAY[programmed_delays]);



    for (int edge = NEG; edge <= POS; ++edge)

    {  /*for edge*/

      setup_cntr_trigger_slopes(mod_id, edge, POS);

      setup_cntr_trg_levels(mod_id, CNTR_TRG_LEVEL[edge], 1.5);



      for (ssvirt_card = 0; ssvirt_card <= 2; ssvirt_card++)

      {  /*ssvirt_card*/

        for (sschanl = 'A'; sschanl <= 'H'; sschanl++)

        {  /*sschanl*/

          ssdrvr = ssvirt_card * 8 + sschanl - 'A';

          enable_gen_out(mod_id);



          relay_map(mod_id, slot, ssvirt_card);

          vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[sschanl - 'A']);



          sales_reset_sequencer(mod_id, slot);

          sales_start_sequencer(mod_id, slot);



          trigger_cntr(mod_id);

          drvr_delay_actual = read_cntr_mean(mod_id);

          if (edge == POS)

            drvr_delay_actual += ssrecf;

/*

          display_it(mod_id, "ssdrvr = ", ssdrvr, 0);

          display_it(mod_id, "edge = ", edge, 0);

          display_it(mod_id, "mod_id = ", mod_id, 0);

          display_it(mod_id, "programmed_delays = ", programmed_delays, 0);

          display_it(mod_id, "slot -1 = ", slot-1, 0);

          display_it(mod_id, "offset[programmed_delays] = ", 0, offset[programmed_delays] );

          display_it(mod_id, "clkrb_delay[mod_id] = ", 0, clkrb_delay[mod_id]);

          display_it(mod_id, "ssdrvr_const[ssdrvr] = ", 0, ssdrvr_const[ssdrvr]);

          display_it(mod_id, "drvr_delay_actual = ", 0, drvr_delay_actual);

          display_it(mod_id, "af2_start_delay[mod_id] = ", 0,af2_start_delay[mod_id] );

          display_it(mod_id, "af2_system_delay[mod_id] = ", 0,af2_system_delay[mod_id] );

          display_it(mod_id, "ss_slot_const[slot-1] = ", 0, ss_slot_const[slot-1]);

          display_it(mod_id, "PROG_DELAY[programmed_delays] = ", 0, PROG_DELAY[programmed_delays]);

          */



         ch_delta[ssdrvr][edge]

                   = offset[programmed_delays] + clkrb_delay[mod_id] -

                     ssdrvr_const[ssdrvr] - drvr_delay_actual -

                     af2_start_delay[mod_id] - af2_system_delay[mod_id] -

                     ss_slot_const[slot-1] - PROG_DELAY[programmed_delays];



          result.Measurement_class = CLASS_REAL;

          result.UU.Real_result = ch_delta[ssdrvr][edge];

          log_result(mod_id, result);

          vth_mseq_stop(mod_id, TIMEOUT);

          disable_gen_out(mod_id);

          vth_popen_rly(mod_id, slot, PIN_KGL_RLYS[sschanl - 'A']);

        }  /*sschanl*/

      }  /*ssvirt_card*/



    }  /* for edge */









    for (ssdrvr = 0; ssdrvr <= 23; ssdrvr++)

    {  /*CALCULATE SKEW*/

      skew = ch_delta[ssdrvr][POS] - ch_delta[ssdrvr][NEG];

      result.UU.Real_result = skew;

      log_result(mod_id, result);

    }  /*CALCULATE SKEW*/



  }  /* for programmed_delays */

#include "t9620_nd.cmd"





  vth_pinit(mod_id, slot);

  vth_minit(mod_id);

  vth_ainit(mod_id);

  /*Added by chuanho to select syn model, 15jun2012*/
  /*--------------------------------------------*/
  if(check_syn_model())
	  close_33250A(mod_id);
  else
      close_syn(mod_id);
  /*--------------------------------------------*/

  close_gen(mod_id);

  close_34980A(mod_id);//close_mux(mod_id);

  close_cntr(mod_id);



}  /*t9620*/

#undef RISING_EDGE

#undef TCLK_FREQ

#undef GEN_LOW

#undef GEN_HIGH_TERMINATED

#undef TIMEOUT



#define TCLK_FREQ       5.0e+06

#define GEN_LOW_50      0.10e+00

#define GEN_HIGH_50     4.0e+00

#define RX_SPEC         10e-09

#define TIMEOUT         1.0e-03

#define STOP_WAIT       100e-03

#define PROG_DELAY_     20e-09

#define TCLK_ADV_TIME   40e-09

#define RES             1.0e-09

#define STARTING_INCREMENT  4.0e-09



/* static variables for t9671: */

struct LOC_t9671

{

  int mod_id;

  int slot;

  boolean rresp;

  boolean werr;

  boolean dummy;

  int receiver_no;

  double gen_delay;

  test_result_type result;

  int relay_page;

};



static void sample(struct LOC_t9671 *LINK)

{

  /* ------------------------------------------------------------------------ */

  /* Procedure sample sets up the generator delay to the value to be tested,  */

  /* enables the RRESP AND WERR signals onto the ARCV lines, starts and stops */

  /* the sequencer, and sets the return values for WERR and RRESP             */

  /* ------------------------------------------------------------------------ */

  setup_gen_trg_out_delay(LINK->mod_id, LINK->gen_delay);

  enable_gen_out(LINK->mod_id);

  sales_reset_sequencer(LINK->mod_id, LINK->slot);



  relay_map(LINK->mod_id, LINK->slot, LINK->relay_page);

  vth_pfc_sel_arcv0(LINK->mod_id, LINK->slot, LINK->receiver_no, 3);

      /* 3 = ARCV0 = RRESP */

  vth_pfc_sel_arcv1(LINK->mod_id, LINK->slot, LINK->receiver_no, 4);

      /* 4 = ARCV1 = WERR */



  vth_prcv_set_hi(LINK->mod_id, LINK->slot, LINK->receiver_no, 1.5);

  vth_prcv_set_lo(LINK->mod_id, LINK->slot, LINK->receiver_no, 1.5);



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



void t9671(int mod_id, int slot)

{  /* main t9671 */

  /*Silversword Receiver Edge Accuracy and Skew*/

  struct LOC_t9671 V;

  boolean expected_response;

  boolean found;

  boolean hit;

  int direction;

  int slice;

  int chnl;

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

  double ch_result['H' + 1 - 'A'][4][2];



  int mux_node_pin;

  int receiver;



  V.mod_id = mod_id;

  V.slot = slot;

  OpenResourceManager();
  open_33250A(V.mod_id);

  /*Added by chuanho to select syn model, 15jun2012*/
  /*--------------------------------------------*/
  if(check_syn_model())
  {
	  enable_33250A_out(V.mod_id);
	  setup_33250A_defaults(V.mod_id);
  }
  else
      open_syn(V.mod_id);
  /*--------------------------------------------*/

  open_gen(V.mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);

  open_cntr(V.mod_id);

  /* t0_actual is different for mac10s and mac11s.  This value is provided by */

  /* AutoCal software.  AutoCal defines this constant as T0_TIME */

  if ((get_testhead_actual_card(V.mod_id, MAC_SLOT) == CONTROLPLUS) || (get_testhead_actual_card(V.mod_id, MAC_SLOT == CONTROLXT)))

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

  delta_delay = af2_system_delay[V.mod_id] + ss_slot_const[V.slot - 1] +

                rx_path_offset - 1.25e-09 - af2_clkrb_delay[V.mod_id];

  /* rx_path_offset is measured result for PIN cards, we are */

  /* using that value ( -1.25 ) for sscards until the delay  */

  /* constants for ss cards can be generated                 */



  /* expected_receive is when the receiver strobe should occur at no error */

  /* prog_delay is the delay value that the rx is programmed to in the VML code */

  /* tclk_adv_time is the amount of time we advance the tclk (from CLKRB).  We */

  /*  set(picked) this value such that our receive will occur within the delay */

  /*  range of the generator. */

  expected_receive = t0_actual + PROG_DELAY_ - TCLK_ADV_TIME;



  /* The following section calculates the gain and offset associated with the */

  /* generator delay settings.  These values are used to increase the accuracy */

  /* of the results.  This 2 point calibration assumes the delay is linear.  */



  /* min/max_gen_delay are the values over which the receiver will be checked */

  min_gen_delay = nanotrunc(expected_receive - delta_delay - rx_range);

  max_gen_delay = nanotrunc(expected_receive - delta_delay + rx_range);



  /* Make the required connections.  The generator is synced to the synthesizer */

  /* to provide a more accurate (less jitter) timebase for the generator. */

  connect_syn_out_gen_ext(V.mod_id);

  connect_gen_trg_cntr_start(V.mod_id);

  connect_gen_out_cntr_stop(V.mod_id);



  setup_gen_levels(V.mod_id, GEN_LOW_50, GEN_HIGH_50);


  int edge = RISING;
  for (; edge <= FALLING; ++edge)

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



  for (V.relay_page = 0; V.relay_page <= 2; V.relay_page++)

  {  /* for relay page */

    relay_map(V.mod_id, V.slot, V.relay_page);

    mux_node_pin = V.relay_page * 3 + 1;   /* algorithm for array subscript*/



    for (receiver = 'A'; receiver <= 'H'; receiver++)

    {  /* for */

      vth_pclose_rly(V.mod_id, V.slot, PIN_KR_RLYS[receiver - 'A']);

      vth_pclose_rly(V.mod_id, V.slot, PIN_DRVR_RLYS[mux_node_pin]

                 [receiver - 'A']);

      vth_pclose_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[mux_node_pin]

                 [receiver - 'A']);

    }  /* for receiver */

  }  /* for relay_page */



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



#include "t9671_vml.cmd"



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



    for (slice = 0; slice <= 3; slice++)

    {

      switch (slice)

      {   /* Select appropriate XGL termination. */



      case 0:

        relay_map(V.mod_id, V.slot, 2);

        vth_pclose_rly(V.mod_id, V.slot, KGLA);

        vth_pclose_rly(V.mod_id, V.slot, KGLB);

        vth_popen_rly(V.mod_id, V.slot, K17A);

        vth_popen_rly(V.mod_id, V.slot, K17B);

        break;



      case 1:

        relay_map(V.mod_id, V.slot, 0);

        vth_pclose_rly(V.mod_id, V.slot, KGLC);

        vth_pclose_rly(V.mod_id, V.slot, KGLD);

        vth_popen_rly(V.mod_id, V.slot, K11C);

        vth_popen_rly(V.mod_id, V.slot, K11D);

        break;



      case 2:

        relay_map(V.mod_id, V.slot, 2);

        vth_pclose_rly(V.mod_id, V.slot, KGLE);

        vth_pclose_rly(V.mod_id, V.slot, KGLF);

        vth_popen_rly(V.mod_id, V.slot, K17E);

        vth_popen_rly(V.mod_id, V.slot, K17F);

        break;



      case 3:

        relay_map(V.mod_id, V.slot, 0);

        vth_pclose_rly(V.mod_id, V.slot, KGLG);

        vth_pclose_rly(V.mod_id, V.slot, KGLH);

        vth_popen_rly(V.mod_id, V.slot, K11G);

        vth_popen_rly(V.mod_id, V.slot, K11H);

        break;

      default:

        assert(false);

        break;



      }/* Select appropriate XGL termination. */



      for (V.relay_page = 0; V.relay_page <= 2; V.relay_page++)

      {  /* relay_page */

        for (chnl = 0; chnl <= 1; chnl++)

        {  /* for chnl */

          relay_map(V.mod_id, V.slot, V.relay_page);

          V.receiver_no = slice * 2 + V.relay_page * 8 + chnl;

          receiver = slice * 2 + chnl + 'A';

          switch (slice & 1)

          {



          case 0:

            if (V.relay_page == 2)

              vth_pclose_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[7][receiver - 'A']);

            else

              vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);

            break;



          case 1:

            if (V.relay_page == 0)

              vth_pclose_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[1][receiver - 'A']);

            else

              vth_pclose_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);

            break;

          default:

            assert(false);

            break;



          }/* case */



          delay_inc = STARTING_INCREMENT;

          direction = 0;

          hit = false;

          found = false;

          V.gen_delay = nanotrunc(expected_receive - delta_delay - 3e-09);



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



          relay_map(V.mod_id, V.slot, V.relay_page);

          switch (slice & 1)

          {



          case 0:

            if (V.relay_page == 2)

              vth_popen_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[7][receiver - 'A']);

            else

              vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);

            break;



          case 1:

            if (V.relay_page == 0)

              vth_popen_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[1][receiver - 'A']);

            else

              vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[receiver - 'A']);

            break;

          default:

            assert(false);

            break;



          }/* case */



          /* calculate the actual receive, correct for gen error and channel delay */

          actual_receive = gain[edge] * (V.gen_delay - min_gen_delay) +

              offset[edge] + ssrec_const[V.receiver_no] + delta_delay;



          ch_result[receiver - 'A'][V.relay_page]

            [edge] = actual_receive - expected_receive;

          V.result.Measurement_class = CLASS_REAL;

          if (!found)

          {   /* it never received */

            display_prompt("**RECEIVE DOES NOT OCCUR WITHIN TEST RANGE**",

                           true);

            ch_result[receiver - 'A'][V.relay_page][edge] = INT_MAX;

          }

          V.result.UU.Real_result = ch_result[receiver - 'A'][V.relay_page]

            [edge];

          log_result(V.mod_id, V.result);

        }  /* for chnl */

      }  /* for relay_page */



      switch (slice)

      {



      case 0:

        relay_map(V.mod_id, V.slot, 2);

        vth_popen_rly(V.mod_id, V.slot, KGLA);

        vth_popen_rly(V.mod_id, V.slot, KGLB);

        break;



      case 1:

        relay_map(V.mod_id, V.slot, 0);

        vth_popen_rly(V.mod_id, V.slot, KGLC);

        vth_popen_rly(V.mod_id, V.slot, KGLD);

        break;



      case 2:

        relay_map(V.mod_id, V.slot, 2);

        vth_popen_rly(V.mod_id, V.slot, KGLE);

        vth_popen_rly(V.mod_id, V.slot, KGLF);

        break;



      case 3:

        relay_map(V.mod_id, V.slot, 0);

        vth_popen_rly(V.mod_id, V.slot, KGLG);

        vth_popen_rly(V.mod_id, V.slot, KGLH);

        break;

      default:

        assert(false);

        break;



      }/*case*/

    }  /* for slice */

  }  /* for edge */



  /* Now log the rising to falling skew for each of the receivers */

  for (V.relay_page = 0; V.relay_page <= 2; V.relay_page++)

  {

    for (receiver = 'A'; receiver <= 'H'; receiver++)

    {  /* for */

      skew = ch_result[receiver - 'A'][V.relay_page]

             [RISING] - ch_result[receiver - 'A'][V.relay_page][FALLING];

      V.result.UU.Real_result = skew;

      log_result(V.mod_id, V.result);

    }  /* for receiver */

  }  /*for relay_page*/

  /*Added by chuanho to select syn model, 15jun2012*/
  /*--------------------------------------------*/
  if(check_syn_model())
	  close_33250A(V.mod_id);
  else
      close_syn(V.mod_id);
  /*--------------------------------------------*/

  close_gen(V.mod_id);

  close_34980A(V.mod_id);//close_mux(V.mod_id);

  close_cntr(V.mod_id);



}  /* main t9671 */

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



#define WHITE           85

/*01010101b*/

/*patterns*/

#define BLACK           170   /*10101010b*/

#define DRIVE_WRITE_MASK  127

/*01111111b*/

/*drive register masks*/

#define DRIVE_READ_MASK  127

#define RECEIVE_WRITE_MASK  127   /*receive register masks*/

#define RECEIVE_READ_MASK  127

typedef char s1[2];

typedef int pattern_type;

typedef char message_type[81];

#define LSB             0

#define MSB             31

typedef union bits

{

  pattern_type word_;

  unsigned bit_;

} bits;



/* static variables for t9685: */

struct LOC_t9685 {

  int mod_id;

  int slot;

  message_type the_pattern;

  test_result_type the_no_result;   /*initialized in main*/

  int subtest;   /*initialized in main*/

} ;



/*!ITOS*/

/*+convert an integer to a string*/

/*-----------------------------------------------------------------------*/

static char *itos(char *result, int i, struct LOC_t9685 * /*LINK*/ )

{

  s80 s;

  int n;



  sprintf(s, "%d", i);

  n = mtd_strlen(s) + 1;

  return strcpy(result, s);

}



/*!CTOS*/

/*+convert a char to a sting*/

/*-----------------------------------------------------------------------*/

static char *ctos(char *result, int c, struct LOC_t9685 * /*LINK*/ )

{

  s1 s;



  s[1] = '\0';

  s[0] = c;

  return strcpy(result, s);

}



/*!THE PATTERN IS*/

/*+record a pattern's description for error messages*/

/*-----------------------------------------------------------------------*/

static void the_pattern_is(char *description, struct LOC_t9685 *LINK)

{

  strcpy(LINK->the_pattern, description);

}



/*!PFC DRV DLY*/

/*+convert a channel A - H  and "drv" register into a pcell*/

/*-----------------------------------------------------------------------*/

static int pfc_drv_dly(int drv, int chan, struct LOC_t9685 * /*LINK*/ )

{

int result;



  switch (drv)

  {



  case 'a':

    switch (chan)

    {



    case 'A':

      result = PFC_DRV_DLY_0_A;

      break;



    case 'B':

      result = PFC_DRV_DLY_1_A;

      break;



    case 'C':

      result = PFC_DRV_DLY_2_A;

      break;



    case 'D':

      result = PFC_DRV_DLY_3_A;

      break;



    case 'E':

      result = PFC_DRV_DLY_4_A;

      break;



    case 'F':

      result = PFC_DRV_DLY_5_A;

      break;



    case 'G':

      result = PFC_DRV_DLY_6_A;

      break;



    case 'H':

      result = PFC_DRV_DLY_7_A;

      break;

    default:

      assert(false);

      break;

    }

    break;



  case 'b':

    switch (chan)

    {



    case 'A':

      result = PFC_DRV_DLY_0_B;

      break;



    case 'B':

      result = PFC_DRV_DLY_1_B;

      break;



    case 'C':

      result = PFC_DRV_DLY_2_B;

      break;



    case 'D':

      result = PFC_DRV_DLY_3_B;

      break;



    case 'E':

      result = PFC_DRV_DLY_4_B;

      break;



    case 'F':

      result = PFC_DRV_DLY_5_B;

      break;



    case 'G':

      result = PFC_DRV_DLY_6_B;

      break;



    case 'H':

      result = PFC_DRV_DLY_7_B;

      break;

    default:

      assert(false);

      break;

    }

    break;

  default:

    assert(false);

    break;

  }

  return result;

}



/*!PFC RCV DLY*/

/*+convert a channel  A - H and "rcv" register into a pcell*/

/*-----------------------------------------------------------------------*/

static int pfc_rcv_dly(int rcv, int chan, struct LOC_t9685 * /*LINK*/ )

{

int result;



  switch (rcv)

  {



  case 'a':

    switch (chan)

    {



    case 'A':

      result = PFC_RCV_DLY_0_A;

      break;



    case 'B':

      result = PFC_RCV_DLY_1_A;

      break;



    case 'C':

      result = PFC_RCV_DLY_2_A;

      break;



    case 'D':

      result = PFC_RCV_DLY_3_A;

      break;



    case 'E':

      result = PFC_RCV_DLY_4_A;

      break;



    case 'F':

      result = PFC_RCV_DLY_5_A;

      break;



    case 'G':

      result = PFC_RCV_DLY_6_A;

      break;



    case 'H':

      result = PFC_RCV_DLY_7_A;

      break;

    default:

      assert(false);

      break;

    }

    break;



  case 'b':

    switch (chan)

    {



    case 'A':

      result = PFC_RCV_DLY_0_B;

      break;



    case 'B':

      result = PFC_RCV_DLY_1_B;

      break;



    case 'C':

      result = PFC_RCV_DLY_2_B;

      break;



    case 'D':

      result = PFC_RCV_DLY_3_B;

      break;



    case 'E':

      result = PFC_RCV_DLY_4_B;

      break;



    case 'F':

      result = PFC_RCV_DLY_5_B;

      break;



    case 'G':

      result = PFC_RCV_DLY_6_B;

      break;



    case 'H':

      result = PFC_RCV_DLY_7_B;

      break;

    default:

      assert(false);

      break;

    }

    break;



  default:

    assert(false);

    break;

  }

  return result;

}



/*!AND MASK*/

/*+return the result of bitwise anding a pattern and a mask*/

/*-----------------------------------------------------------------------*/

static int and_mask(int pattern, int mask, struct LOC_t9685 * /* LINK */ )

{

  bits p;

  bits m;

  bits result;

  int i;



  result.bit_ = ~0; // wpd - check this

  p.word_ = pattern;

  m.word_ = mask;

  for (i = LSB; i <= MSB; i++)

  {

    result.bit_ &= ~(1 << (i - LSB));

    result.bit_ |= ((p.bit_ & (1 << (i - LSB))) != 0 &&

                    (m.bit_ & (1 << (i - LSB))) != 0) << (i - LSB);

  }

  return (result.word_);

}

#undef LSB

#undef MSB



/*!INTERLEAVE WRITE DRIVE*/

/*+issue interleaved writes to all channels of "drv1" and "drv2"*/

/*-----------------------------------------------------------------------*/

static void interleave_write_drive(int drv1, int pattern1, int drv2,

                                  int pattern2, struct LOC_t9685 *LINK)

{

  pattern_type p1;

  pattern_type p2;

int chan;



  /*+MASK THE PATTERNS TO FIT THE REGS.*/

  p1 = and_mask(pattern1, DRIVE_WRITE_MASK, LINK);

  p2 = and_mask(pattern2, DRIVE_WRITE_MASK, LINK);



  /*+FOR EACH DRIVE CHANNEL...*/

  virt_card_select(LINK->mod_id, LINK->slot, 3);

      /*selects all 3 virt. cards*/

  for (chan = 'A'; chan <= 'H'; chan++)

  {

    /*+WRITE MASKED PATTERN1 TO DRV1*/

    vth_pwr_cell(LINK->mod_id, LINK->slot, pfc_drv_dly(drv1, chan, LINK), p1);



    /*+WRITE MASKED PATTERN2 TO DRV2*/

    vth_pwr_cell(LINK->mod_id, LINK->slot, pfc_drv_dly(drv2, chan, LINK), p2);

  }

}



/*!INTERLEAVE WRITE RECEIVE*/

/*+issue interleaved writes to all channels of "rcv1" and "rcv2"*/

/*-----------------------------------------------------------------------*/

static void interleave_write_receive(int rcv1, int pattern1, int rcv2,

                                    int pattern2, struct LOC_t9685 *LINK)

{

  pattern_type p1;

  pattern_type p2;

int chan;



  /*+MASK THE PATTERNS TO FIT THE REGS.*/

  p1 = and_mask(pattern1, RECEIVE_WRITE_MASK, LINK);

  p2 = and_mask(pattern2, RECEIVE_WRITE_MASK, LINK);



  /*+FOR EACH RECEIVE CHANNEL...*/

  virt_card_select(LINK->mod_id, LINK->slot, 3);

      /*selects all 3 virt. cards*/

  for (chan = 'A'; chan <= 'H'; chan++)

  {

    /*+WRITE MASKED PATTERN1 TO RCV1*/

    vth_pwr_cell(LINK->mod_id, LINK->slot, pfc_rcv_dly(rcv1, chan, LINK), p1);



    /*+WRITE MASKED PATTERN2 TO RCV2*/

    vth_pwr_cell(LINK->mod_id, LINK->slot, pfc_rcv_dly(rcv2, chan, LINK), p2);

  }

}



/*!DISPLAY TEST TITLE*/

/*+output test title and pattern description to operator's display*/

/*-----------------------------------------------------------------------*/

static void display_test_title(struct LOC_t9685 *LINK)

{

  s80 str1;

  char str2[196];



  sprintf(str2, "T9685 Formatter Pattern Subtest %s: %s",

          itos(str1, LINK->subtest, LINK), LINK->the_pattern);

  display_prompt(str2, true);

}



/*!NO RESULT*/

/*+alert operator that no result was available*/

/*-----------------------------------------------------------------------*/

static void no_result(char *message, int reg, int chnl, struct LOC_t9685 *LINK)

{

  s80 str1;

  s80 str2;

  char str3[256];

  s1 str4;

  s80 str5;



  display_test_title(LINK);

  sprintf(str3, "Module=%s Slot=%s%s%c Chan=%s No result in result log",

          itos(str1, LINK->mod_id, LINK), itos(str2, LINK->slot, LINK),

          message, ctos(str4, reg, LINK), itos(str5, chnl, LINK));

  display_prompt(str3, true);

  display_prompt("", true);

}



/*!DOESNT MATCH*/

/*+complain to operator about mismatched patterns*/

/*-----------------------------------------------------------------------*/

static void doesnt_match(char *message, int reg, int chnl, int actual,

                        int expected, struct LOC_t9685 *LINK)

{

  s80 str1;

  s80 str2;

  s1 str4;

  s80 str5;

  s80 str6;

  s80 str7;

  char str8[256];



  display_test_title(LINK);

  sprintf(str8, "Module=%s Slot=%s%s%c Chan=%s Actual=%s Expected=%s",

          itos(str1, LINK->mod_id, LINK), itos(str2, LINK->slot, LINK),

          message, ctos(str4, reg, LINK), itos(str5, chnl, LINK),

          itos(str6, actual, LINK), itos(str7, expected, LINK));

  display_prompt(str8, true);

  display_prompt("", true);

}



/*!EXPECT DRIVE*/

/*+check all channels of register "drv" for an expected pattern*/

/*-----------------------------------------------------------------------*/

static void expect_drive(int drv, int pattern, struct LOC_t9685 *LINK)

{

  pattern_type actual;

  pattern_type expected;

int chan;

int chnl;

int virt_card;

  test_result_type the_result;



  /*+APPLY THE DRIVE-READ-MASK TO THE EXPECTED PATTERN*/

  expected = and_mask(pattern, DRIVE_READ_MASK, LINK);



  /*+FOR EACH DRIVE CHANNEL...*/

  for (virt_card = 0; virt_card <= 2; virt_card++)

  {

    virt_card_select(LINK->mod_id, LINK->slot, virt_card);

    for (chan = 'A'; chan <= 'H'; chan++)

    {

      /*+INCREMENT THE GLOBAL SUBTEST COUNTER*/

      LINK->subtest++;



      chnl = chan + virt_card * 8 - 'A';



      /*+READ THE PCELL APPROPRIATE FOR THIS "DRV" CHANNEL*/

      vth_prd_cell(LINK->mod_id, LINK->slot, pfc_drv_dly(drv, chan, LINK));



      /*+IF THERE IS A RESULT...*/

      if (get_result_nolog(LINK->mod_id, &the_result))

      {

        /*+LOG THE RESULT*/

        log_result(LINK->mod_id, the_result);



        /*+APPLY THE READ-MASK*/

        actual = and_mask(the_result.UU.Integer_result, DRIVE_READ_MASK, LINK);



        /*+COMPLAIN IF THE MASKED READ DOESN'T MATCH THE EXPECTED*/

        if (actual != expected)

          doesnt_match(" DRV=", drv, chnl, actual, expected, LINK);

      }

      else

      {

        /*+LOG "NO RESULT" AND COMPLAIN TO THE OPERATOR*/

        no_result(" DRV=", drv, chnl, LINK);

        log_result(LINK->mod_id, LINK->the_no_result);

      }



      /*+OTHERWISE*/

    }

  }

}



/*!EXPECT RECEIVE*/

/*+check all channels of register "rcv" for an expected pattern*/

/*-----------------------------------------------------------------------*/

static void expect_receive(int rcv, int pattern, struct LOC_t9685 *LINK)

{

  pattern_type actual;

  pattern_type expected;

int chan;

int chnl;

int virt_card;

  test_result_type the_result;



  /*+APPLY THE RECEIVE-READ-MASK TO THE EXPECTED PATTERN*/

  expected = and_mask(pattern, RECEIVE_READ_MASK, LINK);



  /*+FOR EACH RECEIVE CHANNEL...*/

  for (virt_card = 0; virt_card <= 2; virt_card++)

  {

    virt_card_select(LINK->mod_id, LINK->slot, virt_card);

    for (chan = 'A'; chan <= 'H'; chan++)

    {

      /*+INCREMENT THE GLOBAL SUBTEST COUNTER*/

      LINK->subtest++;



      chnl = chan + virt_card * 8 - 'A';



      /*+READ THE PCELL APPROPRIATE FOR THIS "RCV" CHANNEL*/

      vth_prd_cell(LINK->mod_id, LINK->slot, pfc_rcv_dly(rcv, chan, LINK));



      /*+IF THERE IS A RESULT...*/

      if (get_result_nolog(LINK->mod_id, &the_result))

      {

        /*+LOG THE RESULT*/

        log_result(LINK->mod_id, the_result);



        /*+APPLY THE READ-MASK*/

        actual = and_mask(the_result.UU.Integer_result, RECEIVE_READ_MASK,

                          LINK);



        /*+COMPLAIN IF THE MASKED READ DOESN'T MATCH THE EXPECTED*/

        if (actual != expected)

          doesnt_match(" RCV=", rcv, chnl, actual, expected, LINK);

      }

      else

      {

        /*+LOG "NO RESULT" AND COMPLAIN TO THE OPERATOR*/

        log_result(LINK->mod_id, LINK->the_no_result);

        no_result(" RCV=", rcv, chnl, LINK);

      }



      /*+OTHERWISE*/

    }

  }

}





/* wpd - include begins here */

/*+T9685.P*/

/*PAGE #*/

/*!T9685*/

/*+17-JAN-90 (slc) Cox wrote original from Devore's notes*/

/*+22-JAN-90 (slc) Cox corrected bugs and got it working for the 1st time*/

/*+05-FEB-90 (slc) Devore checked with real boards and blessed it*/

/*+06-FEB-90 (slc) Cox added error logging*/

/*+11-JUL-90 (mjr) Modified to work w/Silversword & standardized some vars*/

/*----------------------------------------------------------------*/

void t9685(int mod_id_, int slot_)

{

  struct LOC_t9685 V;



  /*+BODY OF T9685*/

  /*-----------------------------------------------------------------------*/

  V.mod_id = mod_id_;

  V.slot = slot_;

  /*+INITIALIZE THE SUBTEST COUNTER*/

  V.subtest = -1;



  /*----------*/



  /*+INITIALIZE "THE NO RESULT" RESULT*/

  V.the_no_result.Measurement_class = CLASS_BOOLEAN;

  V.the_no_result.UU.Boolean_result = false;



  /*----------*/



  /*+WRITE "BLACK ON WHITE" PATTERNS TO REGS. "A" AND "B"*/

  the_pattern_is("Black on white, vth_write A then B", &V);

  interleave_write_drive('a', BLACK, 'b', WHITE, &V);

  interleave_write_receive('a', BLACK, 'b', WHITE, &V);



  /*+READ AND CHECK THE REGS.*/

  expect_drive('a', BLACK, &V);

  expect_drive('b', WHITE, &V);

  expect_receive('a', BLACK, &V);

  expect_receive('b', WHITE, &V);



  /*----------*/



  /*+WRITE "WHITE ON BLACK" PATTERNS TO REGS. "A" AND "B"*/

  the_pattern_is("White on black, vth_write A then B", &V);

  interleave_write_drive('a', WHITE, 'b', BLACK, &V);

  interleave_write_receive('a', WHITE, 'b', BLACK, &V);



  /*+READ AND CHECK THE REGS.*/

  expect_drive('a', WHITE, &V);

  expect_drive('b', BLACK, &V);

  expect_receive('a', WHITE, &V);

  expect_receive('b', BLACK, &V);



  /*----------*/



  /*+WRITE "BLACK ON WHITE" PATTERNS TO REGS. "B" AND "A"*/

  the_pattern_is("Black on white, vth_write B then A", &V);

  interleave_write_drive('b', BLACK, 'a', WHITE, &V);

  interleave_write_receive('b', BLACK, 'a', WHITE, &V);



  /*+READ AND CHECK THE REGS.*/

  expect_drive('a', WHITE, &V);

  expect_drive('b', BLACK, &V);

  expect_receive('a', WHITE, &V);

  expect_receive('b', BLACK, &V);



  /*----------*/



  /*+WRITE "WHITE ON BLACK" PATTERNS TO REGS. "B" AND "A"*/

  the_pattern_is("White on black, vth_write B then A", &V);

  interleave_write_drive('b', WHITE, 'a', BLACK, &V);

  interleave_write_receive('b', WHITE, 'a', BLACK, &V);



  /*+READ AND CHECK THE REGS.*/

  expect_drive('a', BLACK, &V);

  expect_drive('b', WHITE, &V);

  expect_receive('a', BLACK, &V);

  expect_receive('b', WHITE, &V);



  /*----------*/



  /*+RESET THE MODULE TO A KNOWN STATE*/

  vth_mod_init_dgn(V.mod_id);

}

#undef WHITE

#undef BLACK

#undef DRIVE_WRITE_MASK

#undef DRIVE_READ_MASK

#undef RECEIVE_WRITE_MASK

#undef RECEIVE_READ_MASK



#define RANGE           3.0

#define RESOLUTION      1.0e-03

#define DGN_INFINITY    1e+31

#define DMM_FUNCTION    FOUR_WIRE_OHMS

void t9700(int mod_id, int slot)

{  /*t9200*/

  /* Series Xn Bus Resistance */

  /*19Dec89 first  attempt at adapting an analog sales test to Silversword.*/

  int mux_node_pin;

  int slice;

  int chnl;

  int channel;

  test_result_type result;



  //open_mux(mod_id);
  open_34980A(mod_id);

  open_dmm(mod_id);



  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);

  setup_dmm_ocomp_on(mod_id);



  connect_dmm(mod_id);



  for (slice = 0; slice <= 3; slice++)

  {  /*for slice*/

    for (chnl = 0; chnl <= 1; chnl++)

    {  /*for chnl*/

      mux_node_pin = (1 - chnl) * 7;   /*7 or 0*/

      channel = slice * 2 + chnl + 'A';

      relay_map(mod_id, slot, (1 - chnl) * 2);

          /*selects virtual card 0 or 2*/

      vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);

      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mux_node_pin][channel - 'A']);

      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mux_node_pin + 1][channel - 'A']);

      vth_aclose_rly(mod_id, AIM_X_S_RLYS[channel - 'A']);

      vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[channel - 'A']);

    }  /*for chnl*/

    /*0 corresponds to the even numbered channel in

                           a channel pair.*/

  }  /*for slice*/

  /*a slice is two channels wide and 3 virtual cards

                          deep.  The mint pin equivalent of a pin card pair

                          of drivers..18 pins.*/



  for (channel = 'A'; channel <= 'H'; channel++)

  {  /*for*/

    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);

    connect_minus_pin(mod_id, slot, channel, true);

    connect_plus_xn(mod_id, channel, true);



    measure_dmm(mod_id);

    if (get_result_nolog(mod_id, &result))

    {  /*if*/

      if (fabs(result.UU.Real_result) > DGN_INFINITY)

        result.UU.Real_result = DGN_INFINITY - 1;

      log_result(mod_id, result);

    }  /*if*/

    else

      display_prompt("No result in result log (Expected dmm R)", true);



    vth_popen_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);



    connect_minus_pin(mod_id, slot, channel, false);



    connect_plus_xn(mod_id, channel, false);

  }  /*for*/



  vth_ainit(mod_id);

  vth_pinit(mod_id, slot);



  close_34980A(mod_id);//close_mux(mod_id);

  close_dmm(mod_id);



}

#undef RANGE

#undef RESOLUTION

#undef DGN_INFINITY

#undef DMM_FUNCTION



#define CHANNEL         'A'

#define MUX_NODE_PIN    7

#define RANGE           3.0

#define RESOLUTION      1.0e-03

#define DGN_INFINITY    1e+31

#define DMM_FUNCTION    FOUR_WIRE_OHMS

void t9701(int mod_id, int slot)

{

  /* Series XG Bus Resistance */

  test_result_type result;





  //open_mux(mod_id);
  open_34980A(mod_id);

  open_dmm(mod_id);



  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);

  setup_dmm_ocomp_on(mod_id);



  vth_aclose_rly(mod_id, K752);

  vth_aclose_rly(mod_id, K760);

  vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][CHANNEL - 'A']);

  vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[(MUX_NODE_PIN + 1) % 9]

             [CHANNEL - 'A']);

  relay_map(mod_id, slot, 2);   /*select relay page 2*/

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



  close_34980A(mod_id);//close_mux(mod_id);

  close_dmm(mod_id);



}  /*t9701*/

#undef CHANNEL

#undef MUX_NODE_PIN

#undef RANGE

#undef RESOLUTION

#undef DGN_INFINITY

#undef DMM_FUNCTION



#define NPLC            10

#define DMM_VOLTAGE_RANGE  100e-03

#define DMM_RESISTANCE_RANGE  1.0e+09

#define VOLTAGE_RESOLUTION  1e-06

#define OHMS_RESOLUTION  10e+03

#define DISCHARGE_TIME  100e-03

#define WAIT_TIME       500e-03

#define DGN_INFINITY    1e+31

#define ONE_G           1e+09

void t9705(int mod_id, int slot)

{

  /* Silversword Parallel Xn Bus Resistance */

  double resistance_cal;

  double ln_10;

  double resistance_parallel;

  double resistance_xbus;

  double fixed_z_voltage;



  double bus_leakage_current['H' + 1 - 'A'];



int channel;

  test_result_type result;







  ln_10 = log(10.0);



  //open_mux(mod_id);
  open_34980A(mod_id);

  open_dmm(mod_id);



  connect_dmm(mod_id);



  virt_card_select(mod_id, slot, 3);   /*select all virtual cards*/

  for (channel = 'A'; channel <= 'H'; channel++)

  {  /*close the bus paths*/

    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);

    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);

    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);

  }  /*close the bus paths*/



  for (channel = 'A'; channel <= 'H'; channel++)

  {  /*measure the resistance*/

    setup_dmm(mod_id, TWO_WIRE_OHMS, DMM_RESISTANCE_RANGE, OHMS_RESOLUTION);

    setup_dmm_nplc(mod_id, NPLC);



    connect_1g_ohm_resistor(mod_id, true);

    connect_plus_xn(mod_id, channel, true);

    connect_minus_xn_plus_1(mod_id, channel, true);

        /* 'H' of Xn + 1 = 'A' */



    /* Discharge any trapped charge */

    connect_10k_load(mod_id);

    vth_mtmr_wait(mod_id, WAIT_TIME);

    disconnect_10k_load(mod_id);

    vth_mtmr_wait(mod_id, WAIT_TIME);



    measure_dmm(mod_id);

    if (get_result_nolog(mod_id, &result))

      resistance_cal = result.UU.Real_result;

    else

      display_prompt("No result in result log (Expected dmm R)", true);



    vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[channel - 'A']);

    vth_aclose_rly(mod_id, AIM_X_PLUS_1_MINT_RLYS[channel - 'A']);

    vth_mtmr_wait(mod_id, DISCHARGE_TIME);

    vth_aopen_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);

    vth_mtmr_wait(mod_id, WAIT_TIME);



    measure_dmm(mod_id);

    if (get_result_nolog(mod_id, &result))

    {  /*if*/

      resistance_parallel = result.UU.Real_result;

      if ( number_equal(resistance_parallel, resistance_cal) )

        resistance_xbus = DGN_INFINITY - 1;

      else if (resistance_parallel == 0 || resistance_cal == 0)

        resistance_parallel = 0.0;

      else

        resistance_xbus = 1 / (1 / resistance_parallel - 1 / resistance_cal);

      resistance_xbus = fabs(resistance_xbus);

      /* log(x) = ln(x) / ln(10) */

      result.UU.Real_result = log(resistance_xbus) / ln_10;

      log_result(mod_id, result);

    }  /*if*/

    else

      display_prompt("No result in result log (Expected dmm R)", true);



    connect_1g_ohm_resistor(mod_id, false);

    setup_dmm(mod_id, DCV, DMM_VOLTAGE_RANGE, VOLTAGE_RESOLUTION);

    setup_dmm_nplc(mod_id, NPLC);

    setup_dmm_fixed_z(mod_id, true);



    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);

    vth_mtmr_wait(mod_id, DISCHARGE_TIME);

    vth_aopen_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);

    vth_mtmr_wait(mod_id, WAIT_TIME);



    measure_dmm(mod_id);

    if (get_result_nolog(mod_id, &result))

    {  /*if*/

      fixed_z_voltage = result.UU.Real_result;

      bus_leakage_current[channel - 'A'] = fixed_z_voltage / ONE_G;

    }  /*if*/

    else

      display_prompt("No result in result log (Expected dmm R)", true);



    setup_dmm_fixed_z(mod_id, false);

    vth_aopen_rly(mod_id, AIM_X_MINT_RLYS[channel - 'A']);

    vth_aopen_rly(mod_id, AIM_X_PLUS_1_MINT_RLYS[channel - 'A']);

    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);



    connect_plus_xn(mod_id, channel, false);

    connect_minus_xn_plus_1(mod_id, channel, false);

        /* 'H' of Xn + 1 = 'A' */



  }  /*measure the resistance*/



  for (channel = 'A'; channel <= 'H'; channel++)

  {  /*log the leakage*/

    result.UU.Real_result = bus_leakage_current[channel - 'A'];

    log_result(mod_id, result);

  }  /*log the leakage*/





  vth_ainit(mod_id);

  vth_pinit(mod_id, slot);



  close_34980A(mod_id);//close_mux(mod_id);

  close_dmm(mod_id);



}  /*t9705*/

#undef NPLC

#undef DMM_VOLTAGE_RANGE

#undef DMM_RESISTANCE_RANGE

#undef VOLTAGE_RESOLUTION

#undef OHMS_RESOLUTION

#undef DISCHARGE_TIME

#undef WAIT_TIME

#undef DGN_INFINITY

#undef ONE_G



#define MUX_NODE_PIN    7   /* 7 of 0 thru 8 */

#define NPLC            10

#define WAIT_TIME       500e-03

#define DISCHARGE_TIME  100e-03

#define OHMS_RANGE      1e+09

#define VOLTAGE_RANGE   100e-03

#define OHMS_RESOLUTION  100e+03

#define VOLTAGE_RESOLUTION  1e-06

#define DGN_INFINITY    1e+31

#define ONE_G           1e+09

void t9706(int mod_id, int slot)

{

  /* Silversword Parallel Xall to XG Bus Resistance */

  double resistance_cal;

  double ln_10;

  double resistance_parallel;

  double resistance_xgbus;

  double fixed_z_voltage;

  double bus_leakage_current;



int channel;

  test_result_type result;







  ln_10 = log(10.0);



  //open_mux(mod_id);
  open_34980A(mod_id);

  open_dmm(mod_id);



  setup_dmm(mod_id, TWO_WIRE_OHMS, OHMS_RANGE, OHMS_RESOLUTION);

  setup_dmm_nplc(mod_id, NPLC);



  channel = 'H';

  vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][channel - 'A']);

  vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][channel - 'A']);



  connect_dmm(mod_id);

  connect_minus_xn_plus_1(mod_id, channel, true);   /* 'H' of Xn + 1 = 'A' */

  connect_plus_gout(mod_id);

  connect_1g_ohm_resistor(mod_id, true);



  /* Discharge any trapped capacitance */

  connect_10k_load(mod_id);

  vth_mtmr_wait(mod_id, WAIT_TIME);

  disconnect_10k_load(mod_id);

  vth_mtmr_wait(mod_id, WAIT_TIME);



  measure_dmm(mod_id);

  if (get_result_nolog(mod_id, &result))

    resistance_cal = result.UU.Real_result;

  else

    display_prompt("No result in result log (Expected dmm R)", true);



  vth_aclose_rly(mod_id, K752);   /* Gout to XG relay       */

  vth_aclose_rly(mod_id, K760);   /* Gout to XG relay       */

  vth_aclose_rly(mod_id, K741);   /* X1 to mint field relay */

  vth_pclose_rly(mod_id, slot, KDCG);   /* XG to XGL relay        */



  virt_card_select(mod_id, slot, 3);   /*select all 3 virtual cards */

  for (channel = 'A'; channel <= 'H'; channel++)

  {  /*for*/

    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);

    vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);

    vth_pclose_rly(mod_id, slot, PIN_KGL_RLYS[channel - 'A']);

    vth_aclose_rly(mod_id, AIM_X_I_RLYS[channel - 'A']);

  }  /*for*/



  channel = 'H';

  vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][channel - 'A']);

  vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][channel - 'A']);

  vth_mtmr_wait(mod_id, WAIT_TIME);



  measure_dmm(mod_id);

  if (get_result_nolog(mod_id, &result))

  {  /*if*/

    resistance_parallel = result.UU.Real_result;

    if ( number_equal(resistance_parallel, resistance_cal) )

      resistance_xgbus = DGN_INFINITY - 1;

    else if (resistance_parallel == 0 || resistance_cal == 0)

      resistance_xgbus = 0.0;

    else

      resistance_xgbus = 1 / (1 / resistance_parallel - 1 / resistance_cal);

    resistance_xgbus = fabs(resistance_xgbus);

    /* log(x) = ln(x) / ln(10) */

    result.UU.Real_result = log(resistance_xgbus) / ln_10;

    log_result(mod_id, result);

  }  /*if*/

  else

    display_prompt("No result in result log (Expected dmm R)", true);



  connect_1g_ohm_resistor(mod_id, false);

  setup_dmm(mod_id, DCV, VOLTAGE_RANGE, VOLTAGE_RESOLUTION);

  setup_dmm_nplc(mod_id, NPLC);

  setup_dmm_fixed_z(mod_id, true);

  vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][channel - 'A']);

  vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][channel - 'A']);

  vth_mtmr_wait(mod_id, DISCHARGE_TIME);

  vth_popen_rly(mod_id, slot, PIN_RCVR_RLYS[MUX_NODE_PIN][channel - 'A']);

  vth_popen_rly(mod_id, slot, PIN_DRVR_RLYS[MUX_NODE_PIN][channel - 'A']);

  vth_mtmr_wait(mod_id, WAIT_TIME);



  measure_dmm(mod_id);

  if (get_result_nolog(mod_id, &result))

  {  /*if*/

    fixed_z_voltage = result.UU.Real_result;

    bus_leakage_current = fixed_z_voltage / ONE_G;

    result.UU.Real_result = bus_leakage_current;

    log_result(mod_id, result);

  }  /*if*/

  else

    display_prompt("No result in result log (Expected dmm R)", true);

  setup_dmm_fixed_z(mod_id, false);

  vth_aopen_all_rlys(mod_id);

  vth_pinit(mod_id, slot);



  close_34980A(mod_id);//close_mux(mod_id);

  close_dmm(mod_id);



}  /*t9706*/

#undef MUX_NODE_PIN

#undef NPLC

#undef WAIT_TIME

#undef DISCHARGE_TIME

#undef OHMS_RANGE

#undef VOLTAGE_RANGE

#undef OHMS_RESOLUTION

#undef VOLTAGE_RESOLUTION

#undef DGN_INFINITY

#undef ONE_G



#define LCR_MULT        5

#define LCR_FREQ        100e+03

#define LCR_FUNCTION    INDUCTANCE

#define LCR_DISPLAY     Q

#define LCR_CKT_MODE    SERIES

void t9710(int mod_id, int slot)

{

  /* Silversword Series Xn Bus Inductance */

  int mux_node_pin;

  int next;

  int start;

  int chnl;

  int slice;

  double henries;



  int channel;

  test_result_type result;

  char *s_ptr;

  char s[MAX_S_LENGTH + 1];

  char *str1;







  //open_mux(mod_id);
  open_34980A(mod_id);

  open_lcr(mod_id);



  setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,

            LCR_MULT);



  connect_lcr(mod_id);



  for (slice = 0; slice <= 3; slice++)

  {  /*for slice*/

    for (chnl = 0; chnl <= 1; chnl++)

    {  /*for chnl*/

      mux_node_pin = (1 - chnl) * 7;   /*7 or 0*/

      channel = slice * 2 + chnl + 'A';

      virt_card_select(mod_id, slot, (1 - chnl) * 2);

          /*select virt.card 0 or 2*/

      vth_pclose_rly(mod_id, slot, PIN_KX_RLYS[channel - 'A']);

      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mux_node_pin][channel - 'A']);

      vth_pclose_rly(mod_id, slot, PIN_RCVR_RLYS[mux_node_pin + 1][channel - 'A']);

      vth_aclose_rly(mod_id, AIM_X_S_RLYS[channel - 'A']);

      vth_aclose_rly(mod_id, AIM_X_MINT_RLYS[channel - 'A']);

    }  /*for chnl*/

    /*chnl = 0 corresponds to the even numbered

                           channel in a pair of drivers/receivers.*/

  }  /*for slice*/

  /*a slice is two channels wide and 3 virtual cards

                          deep.  The mint pin equivalent of a pin card

                          pair of drivers...18 pins.*/



  for (channel = 'A'; channel <= 'H'; channel++)

  {  /*for*/

    vth_pclose_rly(mod_id, slot, PIN_KDC_RLYS[channel - 'A']);



    connect_minus_pin(mod_id, slot, channel, true);

    connect_plus_xn(mod_id, channel, true);



    measure_lcr(mod_id);

    result.Measurement_class = CLASS_PSTRING_PTR;

    if (get_result_nolog(mod_id, &result))

    {  /* if */

      s_ptr = static_cast(char*, result.UU.Pstring_ptr);

      strcpy(s, s_ptr);

      start = strpos2("L", s, 1) + 1;

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

    connect_plus_xn(mod_id, channel, false);



  }  /*for*/



  vth_ainit(mod_id);

  vth_pinit(mod_id, slot);



  close_34980A(mod_id);//close_mux(mod_id);

  close_lcr(mod_id);



}  /*t9710*/

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

void t9711(int mod_id, int slot)

{

  /* Silversword Series XG/XGL/DRa Bus Inductance */

  int next;

  int start;



  double henries;



  test_result_type result;

  char *s_ptr;

  char s[MAX_S_LENGTH + 1];

  char *str1;





  //open_mux(mod_id);
  open_34980A(mod_id);

  open_lcr(mod_id);



  setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,

            LCR_MULT);



  vth_aclose_rly(mod_id, K752);

  vth_aclose_rly(mod_id, K760);

  virt_card_select(mod_id, slot, 2);

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

    start = strpos2("L", s, 1) + 1;

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



  close_34980A(mod_id);//close_mux(mod_id);

  close_lcr(mod_id);



}  /*t9711*/

#undef CHANNEL

#undef MUX_NODE_PIN

#undef LCR_MULT

#undef LCR_FREQ

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

void t9721(int mod_id, int slot)

{

  /* Silversword Parallel Xall to XG Bus Capacitance */

  int next;

  int start;



  double farads;



  int channel;

  test_result_type result;

  char *s_ptr;

  char s[MAX_S_LENGTH + 1];

  char *str1;







  //open_mux(mod_id);
  open_34980A(mod_id);

  open_lcr(mod_id);



  setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,

            LCR_MULT);



  vth_aopen_all_rlys(mod_id);



  vth_aclose_rly(mod_id, K752);   /* Gout to XG relay       */

  vth_aclose_rly(mod_id, K760);   /* Gout to XG relay       */

  vth_aclose_rly(mod_id, K741);   /* X1 to mint field relay */

  vth_pclose_rly(mod_id, slot, KDCG);   /* XG to XGL relay        */



  if (get_34980A_IDN() == false)
  {
	  connect_dg(mod_id, true);
  }



  virt_card_select(mod_id, slot, 3);

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

    start = strpos2("C", s, 1) + 1;

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



  close_34980A(mod_id);//close_mux(mod_id);

  close_lcr(mod_id);



}  /*t9721*/

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

void t9730(int mod_id, int slot)

{

  /* Silversword Xn BUS LEAKAGE CURRENT */

  double dmm_voltage;

  double leakage_current;



int channel;

  test_result_type result;



  //open_mux(mod_id);
  open_34980A(mod_id);

  open_dmm(mod_id);



  setup_dmm(mod_id, DMM_FUNCTION, RANGE, RESOLUTION);



  vth_aclose_rly(mod_id, K741);   /* X1 to mint field relay */

  vth_aclose_rly(mod_id, K761);   /* SWD GND mint pin relay */



  virt_card_select(mod_id, slot, 3);   /*selects all three virtual cards.*/

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



  close_34980A(mod_id);//close_mux(mod_id);

  close_dmm(mod_id);



}  /*t9730*/

#undef RANGE

#undef RESOLUTION

#undef LOAD_RESISTANCE

#undef DMM_FUNCTION

