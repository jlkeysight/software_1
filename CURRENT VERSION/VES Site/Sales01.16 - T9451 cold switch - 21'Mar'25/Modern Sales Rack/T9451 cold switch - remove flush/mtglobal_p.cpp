// testing....
#pragma warning( disable:4786)

#include <cstring>

#include <cassert>

#include <p2c/p2c.h>

#include <stdio.h>

#include <visa.h>

#include <stdlib.h>

#define MTGLOBAL_P_G

#include "mtglobal_p.h"

#include "../../../uCC/src/future.h"

#include "../../../u/src/num_p.h"

#include "../../../u/src/umath.h"

#include "../../../u/src/OSLayer.h"

#include "../parms_p.h"

#include <sicl.h>

#include <time.h>
#include <string>//chun23112021
#include <iostream>//chun23112021

using namespace std;

ViSession rsm;
ViSession viDMM;//3458A
ViSession viDMM50;//3458A
ViSession viPS;//old-6634B  new-N6746B(channel2)
ViSession viPS_N6754A;//N6754A;
ViSession viCNTR;//old-53132A/new-53230A
ViSession viSYNTH;//old-33250A/new-33611A
ViSession viLCR;//old-4263B/new-E4980AL
ViSession viMUX;//switchbox old-HP3235/new-34980A
ViSession viGEN;
ViSession viOLYM;//old-N6734B/new-N6776A(channel1)
ViStatus  err;
ViStatus  err1;//used in N6746B
char buf[256] = {0};

const aim_x_relays AIM_X_PLUS_1_MINT_RLYS = {K742, K743, K744, K745, K746, K747, K748, K741};

const aim_x_relays AIM_X_MINT_RLYS = {K741, K742, K743, K744, K745, K746, K747, K748};

const aim_x_relays AIM_X_S_RLYS = {K733, K734, K735, K736, K737, K738, K739, K740};

const aim_x_relays AIM_X_I_RLYS = {K725, K726, K727, K728, K729, K730, K731, K732};

const aim_x_relays AIM_X_L_RLYS = {K717, K718, K719, K720, K721, K722, K723, K724};

const aim_x_relays AIM_X_A_RLYS = {K709, K710, K711, K712, K713, K714, K715, K716};

const aim_x_relays AIM_X_B_RLYS = {K701, K702, K703, K704, K705, K706, K707, K708};

const chan_relays PIN_KR_RLYS = {KRA, KRB, KRC, KRD, KRE, KRF, KRG, KRH};

const chan_relays PIN_KD_RLYS = {KDA, KDB, KDC, KDD, KDE, KDF, KDG, KDH};

const chan_relays PIN_KX_RLYS = {KX1, KX2, KX3, KX4, KX5, KX6, KX7, KX8};

const chan_relays PIN_KDC_RLYS = {KDC1, KDC2, KDC3, KDC4, KDC5, KDC6, KDC7, KDC8};

const chan_relays PIN_KGL_RLYS = {KGLA, KGLB, KGLC, KGLD, KGLE, KGLF, KGLG, KGLH};

const mux_relays PIN_DRVR_RLYS =

{

  {K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H},

  {K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H},

  {K3A, K3B, K3C, K3D, K3E, K3F, K3G, K3H},

  {K4A, K4B, K4C, K4D, K4E, K4F, K4G, K4H},

  {K5A, K5B, K5C, K5D, K5E, K5F, K5G, K5H},

  {K6A, K6B, K6C, K6D, K6E, K6F, K6G, K6H},

  {K7A, K7B, K7C, K7D, K7E, K7F, K7G, K7H},

  {K8A, K8B, K8C, K8D, K8E, K8F, K8G, K8H},

  {K9A, K9B, K9C, K9D, K9E, K9F, K9G, K9H}

};



const mux_relays PIN_RCVR_RLYS =

{

  {K10A, K10B, K10C, K10D, K10E, K10F, K10G, K10H},

  {K11A, K11B, K11C, K11D, K11E, K11F, K11G, K11H},

  {K12A, K12B, K12C, K12D, K12E, K12F, K12G, K12H},

  {K13A, K13B, K13C, K13D, K13E, K13F, K13G, K13H},

  {K14A, K14B, K14C, K14D, K14E, K14F, K14G, K14H},

  {K15A, K15B, K15C, K15D, K15E, K15F, K15G, K15H},

  {K16A, K16B, K16C, K16D, K16E, K16F, K16G, K16H},

  {K17A, K17B, K17C, K17D, K17E, K17F, K17G, K17H},

  {K18A, K18B, K18C, K18D, K18E, K18F, K18G, K18H}

};



int gen_cal_counter=0;

double gen_cal_delay;



#include "digital_const_p.h"



//-------------------------------------------------------------------------

// flush_commands

void flush_commands(int mod_id)

{

  /* This is a stub that allows these tests to compile on the 700 Series */

  /* when used in conjunction with the all_modules or all_modules_but_me */

  /* procedures.                                                         */

  test_result_type result;



  if (get_result_nolog(mod_id, &result))

    display_prompt("Unexpected result pending.", true);

}  /*flush_commands*/

//-------------------------------------------------------------------------



void display_it(int mod_id, char *s_, int i, double r)

{

  char s[81];



  int current_position;

  int next;



  test_result_type result;



 strcpy(s, s_);

  /* SYNC UP THE PIPE */

  if (get_result_nolog(mod_id, &result))

    display_prompt("Unexpected result pending.", true);

  current_position = mtd_strlen(s) + 1;

  if (current_position < 80)

  {  /* attempt to write the integer */

    sprintf(s + current_position - 1, "%d", i);

    next = mtd_strlen(s) + 1;

  }  /* attempt to write the integer */

  current_position = mtd_strlen(s) + 1;

  if (current_position < 80)

  {  /* attemp to write the real */

    strcat(s, " ");

    sprintf(s + current_position, "%1.12f", r);

    next = mtd_strlen(s) + 1;

  }  /* attemp to write the real */

  display_prompt(s, true);



}



/* wpd - include begins here */

void show_present_test_info(int present_test_num, int mod_id, int present_slot_num, char * s)

{

  int start;

  int next;

  *s = '\0';

  start = 1;

  strcat(s, "Test ");

  start = mtd_strlen(s) + 1;

  sprintf(s + start - 1, "%d", present_test_num);

  next = mtd_strlen(s) + 1;

  strcat(s, "  Module ");

  start = mtd_strlen(s) + 1;

  sprintf(s + start - 1, "%d", mod_id);

  next = mtd_strlen(s) + 1;

  strcat(s, "  Slot ");

  start = mtd_strlen(s) + 1;
    vth_mtmr_wait(mod_id, 0.3); //added fikri 29/5/2024

  sprintf(s + start - 1, "%d", present_slot_num);

  next = mtd_strlen(s) + 1;

}



/* ------------------------------------------------------------------------ */

void sales_reset_sequencer(int mod_id, int slot)

{

  vth_mrst_seq(mod_id);

  vth_pfc_rst_soft(mod_id, slot);

}  /* sales_reset_sequencer */



/* ------------------------------------------------------------------------ */

void sales_reset_sequencer_aim(int mod_id)

{

  vth_mrst_seq(mod_id);

  vth_afc_rst_soft(mod_id);

}  /* sales_reset_sequencer_aim */



/* ------------------------------------------------------------------------ */

void sales_start_sequencer(int mod_id, int slot)

{

  vth_pdrv_clr_err(mod_id, slot);

  vth_mseq_dbg_setup(mod_id, 1, 0xffffffff, 0xffffffff, 0);

  vth_mseq_run(mod_id);

}  /* sales_start_sequencer */



/* ------------------------------------------------------------------------ */

void sales_start_sequencer_mac(int mod_id)

{

  vth_mseq_dbg_setup(mod_id, 1, 0xffffffff, 0xffffff, 0);

  vth_mseq_run(mod_id);

}  /* sales_start_sequencer_mac */



/* ------------------------------------------------------------------------ */

void convert_seconds_nanoseconds(double *value)

{

  (*value) = (*value) * 1e+09;

}  /* convert_seconds_nanoseconds */



/* ------------------------------------------------------------------------ */

/* This procedure takes a real value and rounds it to a whole number of nS  */

/* ------------------------------------------------------------------------ */

double nanotrunc(double value)

{

//  return static_cast<int>((value*1e9) / 1e9);

	return ((int)(value*1e9) / 1e9);

}



/* ------------------------------------------------------------------------ */

double convert_logged_string_to_numeric(int mod_id)

{

  test_result_type result;

  char *s_ptr;

  int start;

  int next;

  s_type s;

  double numeric_value;

  char *str1;

  result.Measurement_class = CLASS_PSTRING_PTR;



  if (get_result_nolog(mod_id, &result))

  {  /*there is a result*/

    s_ptr = static_cast(char*, result.UU.Pstring_ptr);

    strcpy(s, s_ptr);

    start = strpos2("=", s, 0) + 1;   /* READING CNTR OR LCR METER */



    if (start == 1)   /* READING THE POWER SUPPLY SENSE format: VSET XX.XX */

      start = strpos2(" ", s, 0) + 1;



    numeric_value = strtod(s + start - 1, &str1);

    next = str1 - s + 1;

    return numeric_value;

  }  /*there is a result*/

  else

  {  /*there isn't a result*/

    display_prompt("No result in result log - Expected string value.", true);

    return 0.0;

  }  /*there isn't a result*/

}  /*convert_logged_string_to_numeric*/



/* ------------------------------------------------------------------------*/

void close_pin_drvr_load_rlys(int mod_id, int slot, int driver)

{

  switch (driver)

  {



  case 'A':

  case 'B':

  case 'E':

  case 'F':

    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[7][driver - 'A']);

    break;



  case 'C':

  case 'D':

  case 'G':

  case 'H':

    vth_pclose_rly(mod_id, slot, PIN_DRVR_RLYS[1][driver - 'A']);

    break;



  default:

    assert(false);

    break;



  }/*case*/

}  /*close_pin_drvr_load_rlys*/



/* ------------------------------------------------------------------------ */

void close_pin_gnd_relays(int mod_id, int slot)

{

  vth_pclose_rly(mod_id, slot, KGD1);

  vth_pclose_rly(mod_id, slot, KGD2);

  vth_pclose_rly(mod_id, slot, KGD3);

  vth_pclose_rly(mod_id, slot, KGD4);

  vth_pclose_rly(mod_id, slot, KGD5);

  vth_pclose_rly(mod_id, slot, KGD6);

}  /*close_pin_GND_relays*/



#define MAX_LOOPS       3

/* ------------------------------------------------------------------------ */

boolean counter_measurement_pending(int mod_id)

{



  /* This function determines whether or not a measurement is pending on

     the SALES rack 53131 counter.  The return is a corresponding boolean */

  int i;

  double stat;





  vth_mhpib_wr(mod_id, 4, "*RST;*CLS;*SRE 0;*ESE 0");

  vth_mhpib_wr(mod_id, 4, ":STAT:PRES");

  vth_mhpib_wr(mod_id, 4, ":STAT:OPER:PTR 0;NTR 16");

  vth_mhpib_wr(mod_id, 4, ":STAT:OPER:ENABLE 16");

  vth_mhpib_wr(mod_id, 4, "*SRE 128");

  vth_mhpib_wr(mod_id, 4, ":FUNC 'FREQ 2'");

  vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:STAR:SOUR IMM");

  vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:STOP:SOUR TIM");

  vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:STOP:TIM .01");

  vth_mhpib_wr(mod_id, 4, ":INP2:COUP DC");

  vth_mhpib_wr(mod_id, 4, ":INP2:IMP 50");

  setup_cntr_trg_levels(mod_id, 1.3, 1.3);

  vth_mhpib_wr(mod_id, 4, ":INIT");



  i = 1;

  do

  {

    i++;

    vth_mhpib_wr(mod_id, 4, "*STB?");

    vth_mhpib_rd(mod_id, 4);

    stat = convert_logged_string_to_numeric(mod_id);

    /*

    s:='stat result = ';

    strwrite(s,15,next_position,stat);

    display_prompt(s,true);

    */

  } while (stat != 192 && i <= MAX_LOOPS);



  if (stat == 192)

    return true;

  else

    return false;



}  /* counter_measurement_pending */

#undef MAX_LOOPS



/* ------------------------------------------------------------------------ */

void display_error(int mod_id, char *specific)

{

  int current_position;

  int next_position;



  char s[81];



  *s = '\0';

  display_prompt(s, true);

  display_prompt("*****************************************************",

                 true);



  strcat(s, "*** Test ");

  current_position = mtd_strlen(s) + 1;

  sprintf(s + current_position - 1,

          "%d", present_test[mod_id - NO_TESTHEAD_NUMBER]);

  next_position = mtd_strlen(s) + 1;

  strcat(s, ", Module ");

  current_position = mtd_strlen(s) + 1;

  sprintf(s + current_position - 1, "%d", mod_id);

  next_position = mtd_strlen(s) + 1;

  strcat(s, " did not find a result in the result log.");

  display_prompt(s, true);

  display_prompt(specific, true);



  *s = '\0';

  display_prompt("*****************************************************", true);

  display_prompt("", true);



}  /*display_error*/



/* ------------------------------------------------------------------------ */

void measure_47_ohm_aim_delta(int mod_id, double *resistance)

{

  double resistance_w_47_ohms;

  double resistance_wo_47_ohms;



  test_result_type result;



  vth_aclose_rly(mod_id, K733);

  vth_aclose_rly(mod_id, K725);



  /*MEASURE RESISTANCE WITH 47 OHMS IN SOURCE PATH*/

  vth_rlc_setup(mod_id, 0, 0.1, 0.0, 0.0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0.2, 0.0);

  vth_aclose_rly(mod_id, K829);

  vth_aopen_rly(mod_id, K806);

  vth_mtmr_wait(mod_id, 0.005);// modified by sri

  vth_resistor_meas(mod_id, 1, 1);

  vth_get_resistor_result(mod_id);

  if (get_result_nolog(mod_id, &result))

    resistance_w_47_ohms = result.UU.Real_result;

  else

    display_prompt("No result in result log (Expected resistance)", true);



  vth_aclose_rly(mod_id, K806);



  /*MEASURE RESISTANCE WITHOUT 47 OHMS IN SOURCE PATH*/

  vth_rlc_setup(mod_id, 0, 0.05, 0.0, 0.0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1.5, 0.0);

  vth_aopen_rly(mod_id, K829);

  vth_aclose_rly(mod_id, K806);

  vth_mtmr_wait(mod_id, 0.005);//modified by sri

  vth_resistor_meas(mod_id, 1, 1);

  vth_get_resistor_result(mod_id);

  if (get_result_nolog(mod_id, &result))

    resistance_wo_47_ohms = result.UU.Real_result;

  else

    display_prompt("No result in result log (Expected resistance)", true);



  ///added sri for debugging purposes////////////

  //printf("\n Resistance measured including 47 ohm: %e \n", resistance_w_47_ohms);

  //printf("\n Resistance measured excluding 47 ohm: %e \n", resistance_wo_47_ohms);

  //printf("\nActual RSOURCE is :  %e\n", rsource);

  ////////////////////////////



  *resistance = resistance_w_47_ohms - resistance_wo_47_ohms;

  vth_ainit(mod_id);

}



/*-------------------------------------------------------------------------*/

/*       | 2 | 0 |  Module Configuration from the front.  High speed links */

/*       ---------  are driven in a clockwise direction from the front.    */

/*       | 3 | 1 |                                                         */

/*-------------------------------------------------------------------------*/

int slave_links_to_master(int master_mod_id, int slave_mod_id)

{

  int module_number;

  int where_master;

  int where_slave;

  int links;

  int loop;



  boolean hold;

  boolean slave_found;



  boolean list[4];



  int set_of_modules;



  set_of_modules = get_testhead_modules_present();

  for (module_number = 0; module_number <= 3; module_number++)

  {

    /*SET UP TRUE AND FALSE BASED ON PRESENCE OF MODULES*/

    if (module_number >= 0 && module_number < 32 &&

        ((1 << module_number) & set_of_modules) != 0)

      list[module_number] = true;

    else

      list[module_number] = false;

  }

  /*SET UP TRUE AND FALSE BASED ON PRESENCE OF MODULES*/



  /*MAKE ARRAY SUBSCRIPTS 0-3 REPRESENT MODULES 0,1,3,2 RESPECTIVELY BECAUSE*/

  /*THAT IS THE ORDER OF THE HIGH SPEED LINK CONFIGURATION - CLOCKWISE*/

  hold = list[3];

  list[3] = list[2];

  list[2] = hold;



  /*DETERMINE THE ARRAY SUBSCRIPT THAT REPRESENTS THE MASTER MODULE ID*/

  switch (master_mod_id)

  {



  case 0:

    where_master = 0;

    break;



  case 1:

    where_master = 1;

    break;



  case 2:

    where_master = 3;

    break;



  case 3:

    where_master = 2;

    break;



  default:

    assert(false);

    break;



  }/*case*/



  /*DETERMINE THE ARRAY SUBSCRIPT THAT REPRESENTS THE SLAVE MODULE ID*/

  switch (slave_mod_id)

  {



  case 0:

    where_slave = 0;

    break;



  case 1:

    where_slave = 1;

    break;



  case 2:

    where_slave = 3;

    break;



  case 3:

    where_slave = 2;

    break;



  default:

    assert(false);

    break;



  }/*case*/



  /*COUNT THE NUMBER OF LINKS (MODULES PRESENT) BETWEEN MASTER AND SLAVE*/

  links = 0;

  slave_found = false;

  for (loop = 1; loop <= 3; loop++)

  {

    if (!slave_found)

    {  /*IF MODULE IS PRESENT, INCREMENT LINKS BY 1 COUNT*/

      if (list[(where_master + loop) & 3])

        links++;

      if (((where_master + loop) & 3) == where_slave)

        slave_found = true;

    }  /*IF MODULE IS PRESENT, INCREMENT LINKS BY 1 COUNT*/

  }



  return links;



}  /*slave_links_to_master*/



/* ------------------------------------------------------------------------ */

int any_module_present(void)

{

  int result;

  int module_number;

  int set_of_modules;



  set_of_modules = get_testhead_modules_present();

  result = -1;

  for (module_number = 0; module_number <= 3; module_number++)

  {

    if (module_number >= 0 && module_number < 32 &&

        ((1 << module_number) & set_of_modules) != 0)

      result = module_number;

  }



  return result;

}  /*any_module_present*/



/* ------------------------------------------------------------------------ */

void break_test_for_function_step_compatability(int mod_id)

{

  test_result_type result;



  if (get_result_nolog(mod_id, &result))

    display_prompt("Unexpected result pending.", true);

}



/* ------------------------------------------------------------------------ */

int exclusive_or(int a, int b)

{

  int result;



  result = 0;



  if (a + b == 1)

    return 1;



  return result;

}



/* ------------------------------------------------------------------------ */

double get_cal_resistor_value(int mod_id, int feedback_resistor_no, int meas_type)

{

  test_result_type result;





  vth_acal_rtn_fb(mod_id, feedback_resistor_no, meas_type);



  if (get_result_nolog(mod_id, &result))

    return (result.UU.Real_result);

  else

  {  /*else*/

    display_prompt("No result in result log (Expected resistor cal)", true);

    return 0.000001;

  }  /*else*/



}  /*get_cal_resistor_value*/



/*-----------------------------------------------------------------------*/

void adet_manual_meas(int mod_id)

{

  vth_astat_clr(mod_id);

  vth_mtmr_wait(mod_id, 250e-6);

  vth_mmbd_arm_ris_aimin_edge(mod_id);

  vth_mmbd_ass_atrg(mod_id);

  vth_mmbd_wait_on_aimin(mod_id);

  vth_mmbd_neg_atrg(mod_id);

  vth_adet_rd_volt(mod_id);

  vth_astat_rd(mod_id);

}  /*adet_manual_meas*/



/*-----------------------------------------------------------------------*/

void read_database_line(FILE *f, database_module *data)

{

  char *p2c_temp;



  fscanf(f, "%lg%lg%lg%lg%d", &data->mean, &data->std, &data->sum_x, &data->sum_xx, &data->n);

  fgets(data->comment, 81, f);

  p2c_temp = strchr(data->comment, '\n');



  if (p2c_temp != NULL)

    *p2c_temp = 0;

}



#define DOUBLE_SPACE    "  "

/*-----------------------------------------------------------------------*/

void write_database_line(FILE *f, database_module *data)

{

  fprintf(f, "% .5E%s% .5E%s% .5E%s% .5E%s%d%s%55s\n",

          data->mean, DOUBLE_SPACE, data->std, DOUBLE_SPACE, data->sum_x,

          DOUBLE_SPACE, data->sum_xx, DOUBLE_SPACE, data->n, DOUBLE_SPACE,

          data->comment);

}

#undef DOUBLE_SPACE



/*-----------------------------------------------------------------------*/

void recalculate_database_line(double d, database_module *data)

{

  double rad;



  data->sum_x += d;

  data->sum_xx += d * d;

  data->n++;

  if (data->n > 0)

    data->mean = data->sum_x / data->n;

  else

    data->mean = 0.0;

  rad = (data->n * data->sum_xx - data->sum_x * data->sum_x) /

        (data->n * (data->n - 1));

  if (data->n > 1 && rad > 0)

    data->std = sqrt(rad);

  else

    data->std = 0.0;

}

/*-----------------------------------------------------------------------*/

boolean rack_and_fixture_compatible(char *rack_serial_number, int fixture_serial_number)

{

  boolean compatible;



  compatible = false;

  switch (fixture_serial_number)

  {



  /* ------------------------------------------------------------------ */

  /* Note: sales rack PROD0 (2725A00436) supports both a single density */

  /* fixture (3795) and the Monet double density fixture (3786)         */

  /* ------------------------------------------------------------------ */

  case 3795:

  //case 3786: Fixture 3786 sent to Singapore now associated with PROD11

    if (!strcmp(rack_serial_number, "2725A00436"))

      compatible = true;

    break;



  /* ------------------------------------------------------------------ */

  /* Note: sales rack PROD11 (2725A00436) supports both a single density*/

  /* fixture Monet double density fixture (3786)                        */

  /* ------------------------------------------------------------------ */

  case 3786:

    if (!strcmp(rack_serial_number, "2843A01709"))

      compatible = true;

    break;



  /* ------------------------------------------------------------------ */

  /* Note: sales rack PROD1 (2829A01044) supports both a single density */

  /* fixture (3794) and the Monet double density fixture (3790)         */

  /* ------------------------------------------------------------------ */

  case 3790:

  //case 3794:

    if (!strcmp(rack_serial_number, "2829A01044"))

      compatible = true;

    break;



  /* ------------------------------------------------------------------ */

  /* Note: sales rack PROD2 (2829A01045) supports both a single density */

  /* fixture (3793) and the Monet double density fixture (3789)         */

  /* ------------------------------------------------------------------ */

  case 3793:

  case 3789:

    if (!strcmp(rack_serial_number, "2829A01045"))

      compatible = true;

    break;



  /* ------------------------------------------------------------------ */

  /* Note: sales rack PROD3 (2843A01316) supports both a single density */

  /* fixture (3792) and the Monet double density fixture (3788)         */

  /* ------------------------------------------------------------------ */

  case 3792:

  case 3788:

    if (!strncmp(rack_serial_number, "2843A01316", 9))

      compatible = true;

    break;



  /* ------------------------------------------------------------------ */

  /* Note: sales rack PROD4 (2843A01751) supports both a single density */

  /* fixture (3791) and the Monet double density fixture (3787)         */

  /* ------------------------------------------------------------------ */

  case 3791:

  case 3787:

    if (!strcmp(rack_serial_number, "2843A01751"))

      compatible = true;

    break;



  /* ------------------------------------------------------------------ */

  /* Note: sales rack PROD5 (2843A03827) supports only a double density */

  /* fixture (3785).                                                    */

  /* ------------------------------------------------------------------ */

  case 3785:

    if (!strcmp(rack_serial_number, "2843A03827"))

      compatible = true;

    break;



  /* ------------------------------------------------------------------ */

  /* Note: sales rack PROD6 (2843A03924) supports only a double density */

  /* fixture (3784).                                                    */

  /* ------------------------------------------------------------------ */

  case 3784:

    if (!strcmp(rack_serial_number, "2843A03924"))

      compatible = true;

    break;



  /* ------------------------------------------------------------------ */

  /* Note: sales rack PROD6 (2843A03924) supports only a double density */

  /* fixture (3784).                                                    */

  /* ------------------------------------------------------------------ */

  case 3779:

  case 3794:

    if (!strcmp(rack_serial_number, "2650A00323"))

      compatible = true;

    break;



  default:

    compatible = false;

    break;

  }/*case*/

  return compatible;

}  /*rack_and_fixture_compatible*/



/*-----------------------------------------------------------------------*/

int get_fixture_id(int mod_id)

{

  int lower_fixture_id;

  int upper_fixture_id;



  test_result_type result;



  vth_mrd_cell(mod_id, MIO_FXT_ID_0);

  if (get_result_nolog(mod_id, &result))

    lower_fixture_id = result.UU.Integer_result;

  else

    display_prompt("No result in result log (Expected fxt_id)", true);



  vth_mrd_cell(mod_id, MIO_FXT_ID_1);



  if (get_result_nolog(mod_id, &result))

    upper_fixture_id = result.UU.Integer_result;

  else

    display_prompt("No result in result log (Expected fxt_id)", true);



  return (lower_fixture_id + upper_fixture_id * 256);

}  /*get_fixture_id*/





/*-----------------------------------------------------------------------*/

boolean four_mod_fixture(int mod_id)

{

  boolean result;

  int fixture_id;



  fixture_id = get_fixture_id(mod_id);

  switch (fixture_id)

  {



  case 3794:

  case 3795:

  case 3790:

  case 3786:

    result = false;

    break;



  default:

    result = true;

    break;

  }/* case */



  return result;

}  /* four_mod_fixture */



/*-----------------------------------------------------------------------*/

void load_fxt_specific_actual_values(int mod_id, int number_of_components, double *actual_value)

{

  int dummy;

  int component;

  char buffer[81];

  FILE *infile = NULL;

  int fixture_number;

  fixture_number = get_fixture_id(mod_id);



  switch (fixture_number)

  {

    case 3801:

	  sprintf(buffer, "%sstd.nom_3ind_03", testwhere_path);

      infile = fopen(buffer, "r");



      if (infile == NULL)

      {

        display_prompt("The 'std.nom_3ind_03' file was not found.  Please have sys admin replace.", true);

        _EscIO(FileNotFound);

      }



      break;



    case 3800:

	  sprintf(buffer, "%sstd.nom_3rcd_12", testwhere_path);

      infile = fopen(buffer, "r");



      if (infile == NULL)

      {

        display_prompt("The 'std.nom_3rcd_12' file was not found.  Please have sys admin replace.", true);

        _EscIO(FileNotFound);

      }



      break;



    case 3785:

	  sprintf(buffer, "%sstd.nom_0_12", testwhere_path);

      infile = fopen(buffer, "r");



      if (infile == NULL)

      {

        display_prompt("The 'std.nom_0_12' file was not found.  Please have sys admin replace.", true);

        _EscIO(FileNotFound);

      }



      break;



    case 3784:

	case 3779: // making use of the analog fixture #3

		sprintf(buffer, "%sstd.nom_0_03", testwhere_path);

      infile = fopen(buffer, "r");

      if (infile == NULL)

      {

        display_prompt("The 'std.nom_0_03' file was not found.  Please have sys admin replace.", true);

        _EscIO(FileNotFound);

      }



      break;



    case 3783:

	  sprintf(buffer, "%sstd.nom_1_03", testwhere_path);

      infile = fopen(buffer, "r");



      if (infile == NULL)

      {

        display_prompt("The 'std.nom_1_03' file was not found.  Please have sys admin replace.", true);

        _EscIO(FileNotFound);

      }



      break;



    case 3782:

	  sprintf(buffer, "%sstd.nom_1_12", testwhere_path);

      infile = fopen(buffer, "r");



      if (infile == NULL)

      {

        display_prompt("The 'std.nom_1_12' file was not found.  Please have sys admin replace.", true);

        _EscIO(FileNotFound);

      }



      break;



    case 3781:

	  sprintf(buffer, "%sstd.nom_2_03", testwhere_path);

      infile = fopen(buffer, "r");



      if (infile == NULL)

      {

        display_prompt("The 'std.nom_2_03' file was not found.  Please have sys admin replace.", true);

        _EscIO(FileNotFound);

      }



      break;



    case 3780:   /* otherwise */

	  sprintf(buffer, "%sstd.nom_2_12", testwhere_path);

      infile = fopen(buffer, "r");



      if (infile == NULL)

      {

        display_prompt("The 'std.nom_2_12' file was not found.  Please have sys admin replace.", true);

        _EscIO(FileNotFound);

      }



      break;



    default:

	  sprintf(buffer, "%sstd.nom_zero", testwhere_path);

      infile = fopen(buffer, "r");



      if (infile == NULL)

      {

        display_prompt("The 'std.nom_zero' file was not found.  Please have sys admin replace.", true);

        _EscIO(FileNotFound);

      }



      display_prompt("", true);

      display_prompt("Warning: Using zero actual value file for Standard Components", true);

      display_prompt("", true);   /* otherwise */

      break;



  }/*fixture_id*/



  do

  {

    fscanf(infile, "%d%*[^\n]", &dummy);

    getc(infile);

  } while (dummy != present_test[mod_id - NO_TESTHEAD_NUMBER]);



  for (component = 0; component < number_of_components; component++)

  {

    fscanf(infile, "%lg%*[^\n]", &actual_value[component]);

    getc(infile);

  }



  if (infile != NULL)

    fclose(infile);



  infile = NULL;

}  /*load_fxt_specific_actual_values*/



#define MARK_NUMBER     1

#define MAX_TIME        60.0e+00

#define READY_BIT_POS   3

/*-----------------------------------------------------------------------*/

boolean poll_for_dig_complete(int mod_id, hfs_fd_type* id)

{

  boolean          result;

  boolean          exit;

  int              stat;

  int              error;

  double           time;

  test_result_type test_result;

  a_byte_type      stat_byte;



  vth_mtmr_set_mark(mod_id, MARK_NUMBER);

  time = 0.0;

  result = false;

  exit = false;

  flush_commands(mod_id);



  do

  {

    c_hfs_hpib_spoll(id, &stat, &error);



    if (error == HFS_HPIB_SPOLL_FAILED)

    {

      display_prompt("WARNING: unable to SPOLL 54111 oscilloscope", true);

      time = MAX_TIME;

    }

    else

    {  /* valid stat */

      stat_byte.data = stat;



//changed stat_byte.bool to stat_byte.data to account for Union not working

//also changed ordering of byte by using  7 -                 mjs 6/97



      if ((stat_byte.data & (1 << (7-READY_BIT_POS))) != 0)

      {

        result = true;

        exit = true;

      }

      else

      {  /* busy */

        test_result.Measurement_class = CLASS_REAL;

        vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUMBER);



        if (get_result_nolog(mod_id, &test_result))

          time = test_result.UU.Real_result;

        else

        {

          time = MAX_TIME;

          display_prompt("No result in result log (Expected mark time)", true);

        }

      }  /* busy */

    }  /* valid stat */

  } while (!(exit || time >= MAX_TIME));

  return result;

}  /* poll_for_dig_complete */

#undef MARK_NUMBER

#undef MAX_TIME

#undef READY_BIT_POS



#define MARK_NUMBER     1

#define MAX_TIME        60.0e+00

#define DATA_AVAILABLE_BIT_POS  0

/*-----------------------------------------------------------------------*/

boolean poll_for_dmm_complete(int mod_id, hfs_fd_type* id)

{

  boolean result;



  boolean exit;

  int stat;

  int error;

  double time;

  test_result_type test_result;

  a_byte_type stat_byte;



  vth_mtmr_set_mark(mod_id, MARK_NUMBER);

  time = 0.0;

  result = false;

  exit = false;

  flush_commands(mod_id);

  do

  {

    c_hfs_hpib_spoll(id, &stat, &error);



    if (error == HFS_HPIB_SPOLL_FAILED)

    {

      display_prompt("WARNING: unable to SPOLL 3458 DMM", true);

      time = MAX_TIME;

    }

    else

    {  /* valid stat */

      stat_byte.data = stat;



//potentially a problem -- mjs

//changed stat_byte.bool to stat_byte.data to account for Union not working

//also changed ordering of byte by using  7 -                 mjs 6/97

      if ((stat_byte.data & (1 << (7-DATA_AVAILABLE_BIT_POS))) != 0)

      {

        result = true;

        exit = true;

      }

      else

      {  /* busy */

        test_result.Measurement_class = CLASS_REAL;

        vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUMBER);

        if (get_result_nolog(mod_id, &test_result))

          time = test_result.UU.Real_result;

        else

        {

          time = MAX_TIME;

          display_prompt("No result in result log (Expected mark time)", true);

        }

      }  /* busy */

    }  /* valid stat */

  } while (!(exit || time >= MAX_TIME));

  return result;

}  /* poll_for_dmm_complete */

#undef MARK_NUMBER

#undef MAX_TIME

#undef DATA_AVAILABLE_BIT_POS



#define SAMPLE_DELAY    60.0e-9

#define HOLD_MODE_SETTLING_TIME  2.6e-6

#define MAX_CONV_TIME   17.18e-6

/*---------------------------------------------------------------------------*/

void set_up_digitizer(int mod_id, int input_mode, int range, int n, double period)

{

  double stop_comparator;

  double start_comparator;



  /* code common to all options goes here  */

  vth_ascat_cpu_init(mod_id);

  vth_ascat_run_en(mod_id);

  vth_ascat_en_sandh(mod_id);

  vth_aopen_rly(mod_id, K2303);

  vth_aopen_rly(mod_id, K2304);



  switch (input_mode)

  {



  case 1:

    vth_aclose_rly(mod_id, K2305);

    vth_aclose_rly(mod_id, K2306);

    break;



  case 2:

    vth_aclose_rly(mod_id, K2305);

    vth_aclose_rly(mod_id, K2306);

    vth_aclose_rly(mod_id, K2303);

    break;



  case 3:

    vth_aclose_rly(mod_id, K2305);

    vth_aclose_rly(mod_id, K2306);

    vth_aclose_rly(mod_id, K2304);

    break;



  case 4:

    vth_aclose_rly(mod_id, K2305);

    vth_aclose_rly(mod_id, K2306);

    vth_aclose_rly(mod_id, K2303);

    vth_aclose_rly(mod_id, K2304);

    break;



  default:

    display_prompt("INPUT MODE DOES NOT EXIST!", true);

    break;

  }/* input_mode case */



  /* start relay timer */



  vth_adet_sel_rng(mod_id, range);



  /* check n for limits */

  if (n < 1)

    n = 1;

  if (n > 65536)

    n = 65536;

  vth_ascat_set_cycles(mod_id, n);



  /* check period for limits */

  if (period < 25.0e-6)

    period = 25.0e-6;

  if (period > 31.2e-3)

    period = 31.2e-3;

  if (period > 3.9e-3)

  {

    vth_ascat_en_prscl(mod_id);

    period /= 8;

  }

  vth_ascat_set_prd_cntr(mod_id, period);



  /*  The following 4 lines were used for early telecom use  */

  /*   vth_ascat_set_strt_cmpr(mod_id, period-60e-9);   */

  /*   vth_ascat_set_sampl_dly(mod_id, 60e-9);  */

  /*   vth_ascat_set_stop_cmpr(mod_id, (period - 6.12e-6) );*/

  /*s/h aquisition time*/

  /*   vth_ascat_set_conv_dly(mod_id, 3.76e-6); */

  /*s/h hold delay*/



  /* The following 6 lines fix a timing problem in the SCAT for 25us periods */

  start_comparator = period - SAMPLE_DELAY;

  stop_comparator = MAX_CONV_TIME + HOLD_MODE_SETTLING_TIME;

  vth_ascat_set_sampl_dly(mod_id, SAMPLE_DELAY);

  vth_ascat_set_strt_cmpr(mod_id, start_comparator);

  vth_ascat_set_stop_cmpr(mod_id, stop_comparator);

  vth_ascat_set_conv_dly(mod_id, HOLD_MODE_SETTLING_TIME);



  /* set aimin to EOC */

  vth_aaimin_sel(mod_id, 1);





  /* relay timeout */

  vth_mtmr_wait(mod_id, 0.001);



}  /* set_up_digitizer */

#undef SAMPLE_DELAY

#undef HOLD_MODE_SETTLING_TIME

#undef MAX_CONV_TIME



/*---------------------------------------------------------------------------*/

double smaller(double first_value, double second_value)

{

  if (first_value > second_value)

    return second_value;

  else

    return first_value;

}  /* smaller */



/*---------------------------------------------------------------------------*/

double larger(double first_value, double second_value)

{

  if (first_value > second_value)

    return first_value;

  else

    return second_value;

}  /* larger */



/*---------------------------------------------------------------------------*/

double standard_deviation(int n, double sum, double sum_of_squares)

{

  return sqrt((n * sum_of_squares - sum * sum) / (n * (n - 1)));

}  /* standard_deviation */



/*---------------------------------------------------------------------------*/

void stats(double *mean, double *noise, double sum, double sum_of_squares, int number, double minimum, double maximum, int multiplier)

{

  double std_dev;

  double max_minus_min;

  double std_dev_times_multiplier;



  *mean = sum / number;

  std_dev = standard_deviation(number, sum, sum_of_squares);

  std_dev_times_multiplier = std_dev * multiplier;

  max_minus_min = maximum - minimum;

  *noise = max_minus_min;



  if (std_dev_times_multiplier > max_minus_min)

    *noise = std_dev_times_multiplier;



}  /* stats */



#define READY_BIT_POSITION  4   /* 0-7; 7 msb */

/* ------------------------------------------------------------------------ */

boolean dmm_measurement_complete(void)

{



  /* This function determines whether or not a the 3458 DMM is ready for

     instructions.  This indicates that it has completed the measurements

     it wasw programmed for and is ready to continue testing.  The return

     is a boolean.                                                            */

  boolean result;



  hfs_fd_type* id;

  char         s[81];

  int          stat;

  int          current_position;

  int          next_position;

  int          error;

  path_type    path;

  int          ready;

  a_byte_type  status_byte;



  strcpy(path,"/dev/sales.dmm");

  c_hfs_open(&id, path, HACCESS_READ_WRITE, &error);

  if (error != HFS_NO_ERROR)

  {

    display_prompt("WARNING: Error opening dmm file.", true);

    *s = '\0';

    strcat(s, "Error number = ");

    current_position = mtd_strlen(s) + 1;

    sprintf(s + current_position - 1, "%12d", error);

    next_position = mtd_strlen(s) + 1;

    display_prompt(s, true);

    return false;

  }

  c_hfs_hpib_spoll(id, &stat, &error);

  if (error == HFS_HPIB_SPOLL_FAILED)

  {

    display_prompt("WARNING: unable to SPOLL 3458 DMM", true);

    result = false;

  }

  else

  {

    status_byte.data = stat;

    ready = P_getbits_US(stat, READY_BIT_POSITION, 0);

    if (ready == 1)   /* measurement complete */

      result = true;

    else

      result = false;

  }

  c_hfs_close(id, &error);

  if (error == HFS_CLOSE_ERROR)

    display_prompt("WARNING: unable to close GPIB file", true);

  return result;

}  /* dmm_measurement_complete */

#undef READY_BIT_POSITION



/* ------------------------------------------------------------------------ */

det_range_type sel_det_range(double dcv_input)

{

  int range;



  dcv_input = fabs(dcv_input);

  range = 8 - static_cast<int>(log10(10.0 / dcv_input) / log10(2.0));

  if (range < 0)

    range = 0;

  if (range > 8)

    range = 8;

  return range;

}



#define FIRST           1

#define BLANK_LINE      ' '

/* ------------------------------------------------------------------------ */

double input_real(char *prompt)

{

  char buffer[81];

  int next;

  double value;

  char str1[256];

  char *str2;



  display_prompt(prompt, true);

  sprintf(str1, "%c", BLANK_LINE);

  display_prompt(str1, true);

  get_user_answer(buffer, 80);

  value = strtod(buffer, &str2);

  next = str2 - buffer + FIRST;

  return value;

}

#undef FIRST

#undef BLANK_LINE



#define DIG_SETTLE      250e-6

#define SMALLEST_RANGE  0

#define WAIT_TIME_FOR_SETTLE  0.5

/* ------------------------------------------------------------------------ */

double measure_open(int mod_id, double vplus, double vminus, double rfb)

{

  double result;



  test_result_type test_result;

  double vs1;

  double vs2;

  double vo1;

  double vo2;

  det_range_type range;



  vth_asrc_set_dc_ampl(mod_id, vminus);

  range = sel_det_range(vminus);

  vth_adet_sel_rng(mod_id, range);

  vth_aclose_rly(mod_id, K808);   /* DETHI to S */

  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_mmbd_dis_all(mod_id);

  vth_mtmr_wait(mod_id, DIG_SETTLE);

  vth_afunc_meas(mod_id);

  vth_mmbd_en_all(mod_id);

  vth_get_afunc_result(mod_id);

  if (get_result_nolog(mod_id, &test_result))

    vs1 = test_result.UU.Real_result;

  else

  {

    display_prompt("No result in result log, expected DETECTOR DCV", true);

    vs1 = vminus;

  }

  if (!get_result_nolog(mod_id, &test_result))

    display_prompt("No result in result log (Expected DET stat)", true);

  vth_aopen_rly(mod_id, K808);   /* disconnect DETHI from S */

  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);

  vth_aclose_rly(mod_id, K828);   /* DETHI to REFR2 */

  vth_adet_sel_rng(mod_id, SMALLEST_RANGE);

  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_SETTLE);

  vth_mmbd_dis_all(mod_id);

  vth_mtmr_wait(mod_id, DIG_SETTLE);

  vth_afunc_meas(mod_id);

  vth_mmbd_en_all(mod_id);

  vth_get_afunc_result(mod_id);

  if (get_result_nolog(mod_id, &test_result))

    vo1 = test_result.UU.Real_result;

  else

  {

    display_prompt("No result in result log, expected DETECTOR DCV", true);

    vo1 = vminus;

  }

  if (!get_result_nolog(mod_id, &test_result))

    display_prompt("No result in result log (Expected DET stat)", true);

  vth_aopen_rly(mod_id, K828);   /* disconnect DETHI from REFR2 */

  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_asrc_set_dc_ampl(mod_id, vplus);

  range = sel_det_range(vplus);

  vth_adet_sel_rng(mod_id, range);

  vth_aclose_rly(mod_id, K808);   /* DETHI to S */

  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_mmbd_dis_all(mod_id);

  vth_mtmr_wait(mod_id, DIG_SETTLE);

  vth_afunc_meas(mod_id);

  vth_mmbd_en_all(mod_id);

  vth_get_afunc_result(mod_id);

  if (get_result_nolog(mod_id, &test_result))

    vs2 = test_result.UU.Real_result;

  else

  {

    display_prompt("No result in result log, expected DETECTOR DCV", true);

    vs2 = vplus;

  }

  if (!get_result_nolog(mod_id, &test_result))

    display_prompt("No result in result log (Expected DET stat)", true);

  vth_aopen_rly(mod_id, K808);   /* disconnect DETHI from S */

  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_aclose_rly(mod_id, K828);   /* DETHI to REFR2 */

  vth_adet_sel_rng(mod_id, SMALLEST_RANGE);

  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_SETTLE);

  vth_mmbd_dis_all(mod_id);

  vth_mtmr_wait(mod_id, DIG_SETTLE);

  vth_afunc_meas(mod_id);

  vth_mmbd_en_all(mod_id);

  vth_get_afunc_result(mod_id);

  if (get_result_nolog(mod_id, &test_result))

    vo2 = test_result.UU.Real_result;

  else

  {

    display_prompt("No result in result log, expected DETECTOR DCV", true);

    vo2 = vplus;

  }

  if (!get_result_nolog(mod_id, &test_result))

    display_prompt("No result in result log (Expected DET stat)", true);

  if ( !number_equal(vo1, vo2) )

    result = -1 * (vs2 - vs1) / (vo2 - vo1) * rfb;

  else

    result = R_MAX_PARALLEL;

  vth_aopen_rly(mod_id, K828);   /* disconnect DETHI from REFR2 */

//Debug

//display_it(0, "vs1 = ", mod_id, vs1);

//display_it(0, "vo1 = ", mod_id, vo1);

//display_it(0, "vs2 = ", mod_id, vs2);

//display_it(0, "vo2 = ", mod_id, vo2);



  return result;

}  /* measure_open */

#undef DIG_SETTLE

#undef SMALLEST_RANGE

#undef WAIT_TIME_FOR_SETTLE



#define DIG_SETTLE      250e-6

#define SMALLEST_RANGE  0

#define WAIT_TIME_FOR_SETTLE  0.5

/* ------------------------------------------------------------------------ */

double measure_open_non(int mod_id, double vplus, double vminus, double rfb)

{

  test_result_type result;

  double vs1;

  double vs2;

  double vo1;

  double vo2;

  det_range_type range;

  boolean bogus;



  bogus = false;

  vth_aaux_set_ampl(mod_id, vminus);

  range = sel_det_range(vminus);

  vth_aopen_rly(mod_id, K2306);

  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);

  vth_aclose_rly(mod_id, K2304);   /* Ground DETHI */

  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_adet_sel_rng(mod_id, range);

  vth_mmbd_dis_all(mod_id);

  vth_mtmr_wait(mod_id, DIG_SETTLE);

  vth_afunc_meas(mod_id);

  vth_mmbd_en_all(mod_id);

  vth_get_afunc_result(mod_id);



  if (get_result_nolog(mod_id, &result))

    vs1 = -1.0 * result.UU.Real_result;

  else

  {

    display_prompt("No result in result log, expected DETECTOR DCV", true);

    vs1 = vminus;

  }



  if (!get_result_nolog(mod_id, &result))

    display_prompt("No result in result log (Expected DET stat)", true);



  vth_aopen_rly(mod_id, K2304);   /* disconnect DETHI from ground */

  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);

  vth_aclose_rly(mod_id, K2306);   /* DETHI to REFR2 */

  vth_adet_sel_rng(mod_id, SMALLEST_RANGE);

  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_SETTLE);

  vth_mmbd_dis_all(mod_id);

  vth_mtmr_wait(mod_id, DIG_SETTLE);

  vth_afunc_meas(mod_id);

  vth_mmbd_en_all(mod_id);

  vth_get_afunc_result(mod_id);



  if (get_result_nolog(mod_id, &result))

    vo1 = result.UU.Real_result;

  else

  {

    display_prompt("No result in result log, expected DETECTOR DCV", true);

    bogus = true;

  }



  if (!get_result_nolog(mod_id, &result))

    display_prompt("No result in result log (Expected DET stat)", true);



  vth_aopen_rly(mod_id, K2306);   /* disconnect DETHI from REFR2 */

  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);

  vth_aaux_set_ampl(mod_id, vplus);

  range = sel_det_range(vplus);

  vth_adet_sel_rng(mod_id, range);

  vth_aclose_rly(mod_id, K2304);   /* ground DETHI */

  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

  vth_mmbd_dis_all(mod_id);

  vth_mtmr_wait(mod_id, DIG_SETTLE);

  vth_afunc_meas(mod_id);

  vth_mmbd_en_all(mod_id);

  vth_get_afunc_result(mod_id);



  if (get_result_nolog(mod_id, &result))

    vs2 = -1.0 * result.UU.Real_result;

  else

  {

    display_prompt("No result in result log, expected DETECTOR DCV", true);

    vs2 = vplus;

  }



  if (!get_result_nolog(mod_id, &result))

    display_prompt("No result in result log (Expected DET stat)", true);



  vth_aopen_rly(mod_id, K2304);   /* disconnect DETHI from ground */

  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);

  vth_aclose_rly(mod_id, K2306);   /* DETHI to REFR2 */

  vth_adet_sel_rng(mod_id, SMALLEST_RANGE);

  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_SETTLE);

  vth_mmbd_dis_all(mod_id);

  vth_mtmr_wait(mod_id, DIG_SETTLE);

  vth_afunc_meas(mod_id);

  vth_mmbd_en_all(mod_id);

  vth_get_afunc_result(mod_id);



  if (get_result_nolog(mod_id, &result))

    vo2 = result.UU.Real_result;

  else

  {

    display_prompt("No result in result log, expected DETECTOR DCV", true);

    bogus = true;

  }



  if (!get_result_nolog(mod_id, &result))

    display_prompt("No result in result log (Expected DET stat)", true);



//Debug

//display_it(0, "vs1 = ", mod_id, vs1);

//display_it(0, "vo1 = ", mod_id, vo1);

//display_it(0, "vs2 = ", mod_id, vs2);

//display_it(0, "vo2 = ", mod_id, vo2);



  if (!bogus)

  {

    if ( !number_equal(vo1, vo2) )

      return ((vs2 - vs1) / (vo2 - vo1) * rfb);

    else

      return R_MAX_PARALLEL;

  }

  else



  return rfb;



}  /* measure_open_non */

#undef DIG_SETTLE

#undef SMALLEST_RANGE

#undef WAIT_TIME_FOR_SETTLE



/* ------------------------------------------------------------------------ */

/* This procedure checks to see if the detector upranged during the reading, */

/* if it did, then it sets the delta to -99.9.                               */

void check_uprange(int mod_id, double *delta)

{

  test_result_type result;



  vth_adet_get_uprng(mod_id);

  if (get_result_nolog(mod_id, &result))

  {

    if (result.UU.Integer_result != 0)

    {

      display_prompt("DETECTOR uprange unexpectedly", true);

      *delta = -99.9;

    }

  }

  else

    display_prompt("No result in log (Expected DET uprange)", true);



}  /* check_uprange */



#define POSITIVE        0

#define NEGATIVE        1

/* 10 ohm feedback */

/* enhanced */

/* FOR THE DETECTOR */

#define AVERAGES        1

/* NOT IMPLEMENTED (FOR M.A.D.) */

#define DIGITAL         0

#define TIMEOUT         10.0e+00

#define RESOLUTION      1e-03

/* ------------------------------------------------------------------------ */

void test_detector_range(int mod_id, double range_voltage, int range_number, double const * test)

{

  /* DC DETECTOR VOLTAGE ACCURACY - this procedure is called once for each */

  /* DC detecor range test.                                                */

  int voltage;

  int length;



  double dmm_actual_voltage;

  double detector_voltage;

  double v_delta;



  test_result_type result;

  OpenResourceManager();//chun added 07032022

  open_dmm(mod_id);

  //open_mux(mod_id);
  open_34980A(mod_id);



  connect_dmm(mod_id);

  connect_plus_calib1(mod_id);

  connect_minus_calib2(mod_id);

  flush_commands(mod_id);


  break_test_for_function_step_compatability(mod_id);

  /* CLOSE THE SRCOUT TO DETHI/CALIB2 RELAYS */

  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */

  vth_aclose_rly(mod_id, K733);   /* S bus to X1 bus relay */

  vth_aclose_rly(mod_id, K725);   /* X1 bus to I bus relay */

  vth_aclose_rly(mod_id, K855);   /* I bus to CALIB2 relay */

  /* CLOSE THE GROUND PATH RELAYS */

  vth_aclose_rly(mod_id, K857);   /* L bus to GROUND relay */

  vth_aclose_rly(mod_id, K759);   /* L bus to LOUT relay */



  break_test_for_function_step_compatability(mod_id);

  vth_dcv_setup(mod_id, 0, AVERAGES, DIGITAL, range_voltage, TIMEOUT);

  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);

  vth_afc_en_src_out(mod_id);



  for (voltage = 0; voltage < NUM_DET_POINTS_TO_CHECK; voltage++)

  {

    for (int polarity = POSITIVE; polarity <= NEGATIVE; ++polarity)

    {  /* for polarity */

      if (polarity == NEGATIVE)

        vth_asrc_set_dc_ampl(mod_id, -1 * test[voltage]);

      else

        vth_asrc_set_dc_ampl(mod_id, test[voltage]);



      break_test_for_function_step_compatability(mod_id);

      setup_dmm(mod_id, DCV, test[voltage], RESOLUTION);

      measure_dmm(mod_id);



      if (get_result_nolog(mod_id, &result))

        dmm_actual_voltage = -1 * result.UU.Real_result;

      else

        display_prompt("No result in result log (Expected dmm vth_reading)", true);



      for (length = 0; length <= 1; length++)

      {  /* for length */

        vth_adet_dc_setup(mod_id, length);

        vth_adet_sel_rng(mod_id, range_number);

        adet_manual_meas(mod_id);



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



      }  /* for length */

    }  /* for polarity */

  }



  close_dmm(mod_id);

  close_34980A(mod_id);//close_mux(mod_id);
  CloseResourceManager();//chun addded 07032022
}  /* test_detector_range */

#undef POSITIVE

#undef NEGATIVE

#undef AVERAGES

#undef DIGITAL

#undef TIMEOUT

#undef RESOLUTION



#define AVERAGES        10

#define DIGITAL         0

/* max ps ramp time */

#define RAMP_TIME_ALLOWED  10e+00

/* diff in 2 ps readings */

#define DIFF_V          2e-03

#define RESOLUTION      1.0e-03

#define POWER_SUPPLY_CURRENT  100e-03

#define TIMEOUT         10.0e+00

#define DMM_FUNCTION    DCV



/* static variables for test_det_atten_range: */

struct LOC_test_det_atten_range

{

  int mod_id;

  double zero_v_correction;

  double v_delta;

  test_result_type result;

} ;



/*------------------------------------------------------------------------*/

static void massage_and_log_results(struct LOC_test_det_atten_range * LINK)

{

  /* GET_AFUNC_RESULT RETURNS TWO VALUES, THE VOLTAGE AND THE STATUS */

  vth_afunc_meas(LINK->mod_id);

  vth_get_afunc_result(LINK->mod_id);



  if (get_result_nolog(LINK->mod_id, &LINK->result))

  {  /*adjust result per dmm reading*/

    LINK->v_delta = LINK->result.UU.Real_result - LINK->zero_v_correction;



    if (!get_result_nolog(LINK->mod_id, &LINK->result))

      display_prompt("No result in result log (Expected DET stat)", true);



    check_uprange(LINK->mod_id, &LINK->v_delta);

    LINK->result.Measurement_class = CLASS_REAL;

    LINK->result.UU.Real_result = LINK->v_delta;

    log_result(LINK->mod_id, LINK->result);

    /* BUMP THE SUBTEST_ID BY 1 SO THAT OLD DATA AND NEW DATA ARE */

    /* COMPATIBLE.  OLD DATA ALSO LOGS STATUS OF MEASUREMENT. NEW */

    /* DATA DOES NOT, BUMPING THE SUB_ID ALIGNS THE DATA PROPERLY */

    present_subtest[LINK->mod_id - NO_TESTHEAD_NUMBER]++;

  }  /*adjust result per dmm reading*/

  else

    display_prompt("No result in result log (Expected DET volt)", true);



}  /* massage_and_log_results */


/* ------------------------------------------------------------------------ */
int check_nasru_p10(int mod_id)
{

  /*return aim card id, serial number, and rev code*/
  int nasru_p10_id=0;//default set to none NASRU P10
  char nasru_p10_part_no[10] = { 'N', '1', '8', '0', '7', '6', '6', '8', '1', '0'};  //red relay
//  char nasru_p10_part_no[10] = { 'N', '1', '8', '0', '7', '6', '6', '8', '2', '0'};  //black relay
  static int const SN_SIZE = 20;

  char serial_number [SN_SIZE + 1] = "";

  s80 buffer = "";
  test_result_type result;


  vth_ard_cal_rom(mod_id, EEPROM_STRT);

  for (int i = 0; i < 12; i++)
    vth_ard_cal_rom(mod_id, EEPROM_STRT + SERIAL_OFST + i);


  //*buffer = '\0';


  for (int c = 0; c < 12; c++)
    if (get_result_nolog(mod_id, &result))
      serial_number[c] = result.UU.Integer_result;
    else
      serial_number[c] = '\0';

  //serial_number[c] = '\0';
  //*buffer = '\0';

  if (mtd_strlen(serial_number) == 13)
    sprintf(buffer, "  The PCA Part Number is %*s.", SN_SIZE, serial_number);
  else
    sprintf(buffer, "  Unable to read serial number.");

  display_prompt(buffer, true);

  nasru_p10_id=1;//force to enter loop


   for (int j = 0; j < 12; j++)//re-arrange shifting back since the 1st character is not valid
    serial_number[j]=serial_number[j+1];


  for (int counter = 0; counter < 10 && nasru_p10_id==1; counter++)
    if ((nasru_p10_part_no[counter]==serial_number[counter+1]))
      nasru_p10_id=1;
    else
      nasru_p10_id=0;

	return  nasru_p10_id;
}


/* ------------------------------------------------------------------------ */

void test_det_atten_range(int mod_id, double range_voltage, int num_points, double const * test)

{

  /* DC DETECTOR ATTENUATOR VOLTAGE ACCURACY - this procedure is called */

  /* once for each DC detecor range(supported in attenuated mode) test. */

  struct LOC_test_det_atten_range V;



  int point;

  int length;



  double dmm_actual_voltage;



  /*------------------------------------------------------------------------*/



  V.mod_id = mod_id;

  vth_ainit(mod_id);//jack change it same as i5000

  vth_mtmr_wait(V.mod_id, 0.100);   /* Required for DMM */

/* commenting out these lines as communication between hpib is not needed - Dom*/
  OpenResourceManager();//chun added 07032022
//  open_ps(V.mod_id);
  open_PS_N6746B(V.mod_id);

  open_dmm(V.mod_id);

  //open_mux(V.mod_id); //old switchbox
  open_34980A(V.mod_id); //new swtchbox



  /* SETUP THE DMM TO MEASURE THE ACTUAL OUTPUT FROM THE SUPPLY */

  /* commented these too, I believe are for the Minerva - Dom*/

  connect_dmm(V.mod_id);

  connect_plus_calib1(V.mod_id); //31

  connect_minus_calib2(V.mod_id);

  connect_power_supply_xmint(V.mod_id); //X1 & X2
//  connect_power_supply_xmint_PS6634B(V.mod_id); //X1 & X2 //fikri 31/01/2023


  //flush_commands(mod_id); //remove flush trial quah 21/3/25


  /* CLOSE THE RELAYS THAT ARE 'GLOBAL' TO THIS TEST */

  vth_aclose_rly(V.mod_id, K820);   /* L to DETLO connect relay */

  vth_aclose_rly(V.mod_id, K812);   /* I to DETHI connect relay */

  vth_aclose_rly(V.mod_id, K741);   /* X1 mint pin connect relay  */

  vth_aclose_rly(V.mod_id, K742);   /* X2 mint pin connect relay  */


  //flush_commands(mod_id); //remove flush trial quah 21/3/25

  vth_mtmr_wait(mod_id, 1); 


  /* CLOSE THE DMM READBACK PATH RELAYS CALIB1/LOUT and CALIB2 */

  /* Although I don't need them, I leaving them here so I can connect a multimeter manually - Dom */

  vth_aclose_rly(V.mod_id, K759);   /* LOUT mint pin connect relay */

  vth_aclose_rly(V.mod_id, K855);   /* I bus to CALIB2 relay */


  //flush_commands(mod_id);//remove flush trial quah 21/3/25

  vth_mtmr_wait(mod_id, 1);

  /* SETUP THE 60V SUPPLY DC POWER SUPPLY */



  for (point = 0; point < num_points; point++)

  {  /* for point */

    vth_aopen_rly(V.mod_id, K725);

    //flush_commands(mod_id);//remove flush trial quah 21/3/25

    vth_mtmr_wait(mod_id, 1);

    vth_aopen_rly(V.mod_id, K718);

    //flush_commands(mod_id);//remove flush trial quah 21/3/25

    vth_mtmr_wait(mod_id, 1);

    vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);   /* Required for Coto relays so that */

    /* the supply doesn't get shorted out */

    vth_aclose_rly(V.mod_id, K726);

    //flush_commands(mod_id);//remove flush trial quah 21/3/25

    vth_mtmr_wait(mod_id, 1);

    vth_aclose_rly(V.mod_id, K717);

    //flush_commands(mod_id);//remove flush trial quah 21/3/25

    vth_mtmr_wait(mod_id, 1);

    vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);//jack

/* leaving the power supply setup line, so that I know when to setup the PS when I single step thru the program - dom*/

		    setup_dmm(V.mod_id, DMM_FUNCTION, test[point], RESOLUTION);
// 		    setup_ps_detector_test(V.mod_id, test[point], 0.10); //for 99V detector test
			setup_PS_N6746B_detector_test(V.mod_id, test[point], 0.025); //for 99V detector test //reduce 100mA to 50mA
	vth_mtmr_wait(V.mod_id, 5.0);

    dmm_actual_voltage = fabs(test[point]);

    //flush_commands(mod_id);//remove flush trial quah 21/3/25

    vth_mtmr_wait(mod_id, 1);

    wait_for_ps_ramp(V.mod_id, DIFF_V, RAMP_TIME_ALLOWED, &dmm_actual_voltage);

//removed	setup_PS_N6746B_detector_test_off(V.mod_id, test[point], 0.10);  //turn off PS N6746B Fikri 15-Dec-2022


    //flush_commands(mod_id);//remove flush trial quah 21/3/25

    vth_mtmr_wait(mod_id, 1);

    for (length = 0; length <= 1; length++)//Sri

    {  /*for integrate length*/

      for (int polarity = NEG; polarity <= POS; ++polarity)//Sri

      {  /*for polarity*/

        if (polarity == NEG)

        {  /*polarity = neg*/

          V.zero_v_correction = -1 * dmm_actual_voltage; //this line is not needed - Dom

          //V.zero_v_correction = 0; //make it zero for the time being so that the "massage_and_log_results" function runs ok - dom

          vth_aopen_rly(V.mod_id, K725);

	  //flush_commands(mod_id);//remove flush trial quah 21/3/25

    	  vth_mtmr_wait(mod_id, 1);

          vth_aopen_rly(V.mod_id, K718);

	  //flush_commands(mod_id);//remove flush trial quah 21/3/25

          vth_mtmr_wait(mod_id, 1);

          //vth_mtmr_wait(V.mod_id, 0.0005);

          vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);//jack

              /* Required for Coto relays so that */

          /* the supply doesn't get shorted out */

          vth_aclose_rly(V.mod_id, K726);

          //flush_commands(mod_id);//remove flush trial quah 21/3/25

          vth_mtmr_wait(mod_id, 1);

          vth_aclose_rly(V.mod_id, K717);

         // flush_commands(mod_id);//remove flush trial quah 21/3/25

          vth_mtmr_wait(mod_id, 1);

        }  /*polarity = neg*/

        else

        {  /*polarity = pos*/

          V.zero_v_correction = dmm_actual_voltage; //this line is not needed - dom

          //V.zero_v_correction = 0; //make it zero for the time being so that the "massage_and_log_results" function runs ok - dom

          vth_aopen_rly(V.mod_id, K726);

          vth_aopen_rly(V.mod_id, K717);

          //vth_mtmr_wait(V.mod_id, 0.0005);

		  vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);//jack

              /* Required for Coto relays so that */

          /* the supply doesn't get shorted out */

          vth_aclose_rly(V.mod_id, K725);

          vth_aclose_rly(V.mod_id, K718);

        }  /*polarity = pos*/

		vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);//jack

		setup_PS_N6746B_detector_test_off(V.mod_id, test[point], 0.025); // quah cold switching - off PS 4/2/25
		vth_mtmr_wait(mod_id, 1);
	
        /* VERSION 8.2 REQUIRES THAT THE SETUP BE REPEATED BEFORE ANOTHER MEAS */

        vth_dcv_setup_sales(V.mod_id, length, AVERAGES, DIGITAL, range_voltage, TIMEOUT);

        //flush_commands(mod_id);//remove flush trial quah 21/3/25

        vth_mtmr_wait(mod_id, 1);

        /* CLOSE THE APPROPRIATE PATH RELAYS FOR THE DETLO GNDED MEASUREMENT */

        /* 'SOME' OF THESE RELAYS MUST BE CLOSED AFTER THE DCV_SETUP ROUTINE */



	vth_aclose_rly(V.mod_id, K2310);

        //flush_commands(mod_id);//remove flush trial quah 21/3/25

        vth_mtmr_wait(mod_id, 1);

            /* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */

        vth_aclose_rly(V.mod_id, K2316);

       // flush_commands(mod_id);//remove flush trial quah 21/3/25

        vth_mtmr_wait(mod_id, 1);

            /* DETHI DIVIDED SIGNAL TO RANGING/DETECTOR */

        vth_aclose_rly(V.mod_id, K2303);   /* DETLO TO GROUND SIGNAL PATH */

        //flush_commands(mod_id);//remove flush trial quah 21/3/25

        vth_mtmr_wait(mod_id, 1);

        vth_aclose_rly(V.mod_id, K2305);   /* DETLO TO GROUND SIGNAL PATH */

        //flush_commands(mod_id);//remove flush trial quah 21/3/25

        vth_mtmr_wait(mod_id, 1);

        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);

//removed        setup_PS_N6746B_detector_test(V.mod_id, test[point], 0.10);  //turn on PS N6746B Fikri 15-Dec-2022
//removed       	vth_mtmr_wait(V.mod_id, 3.0);
		setup_PS_N6746B_detector_test(V.mod_id, test[point], 0.025); // quah ON PS - 4/2/25
		vth_mtmr_wait(mod_id, 10);
        massage_and_log_results(&V);

        vth_astat_clr(V.mod_id);

		setup_PS_N6746B_detector_test_off(V.mod_id, test[point], 0.025); // quah cold switching - off PS 4/2/25
		vth_mtmr_wait(mod_id, 1);
		
		
//removed        setup_PS_N6746B_detector_test_off(V.mod_id, test[point], 0.10);  //turn off PS N6746B Fikri 15-Dec-2022
//removed    	vth_mtmr_wait(V.mod_id, 3.0);


            /* DETHI DIVIDED SIGNAL TO RANGING/DETECTOR */



        /* OPEN THE PATH RELAYS TO MAKE THE DETLO GROUNDED MEASUREMENT */

        vth_aopen_rly(V.mod_id, K2310);

            /* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */

        vth_aopen_rly(V.mod_id, K2316);

		vth_aopen_rly(V.mod_id, K2309);//jack

		vth_aopen_rly(V.mod_id, K2315);//jack

        vth_aopen_rly(V.mod_id, K2305);   /* DETLO TO GROUND SIGNAL PATH */

        vth_aopen_rly(V.mod_id, K2303);   /* DETLO TO GROUND SIGNAL PATH */

        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);



		/* CAN USE THE DCV_SETUP ROUTINE BECAUSE THE VOLTAGE IS ACROSS I AND L */

        vth_dcv_setup_sales(V.mod_id, length, AVERAGES, DIGITAL, range_voltage, TIMEOUT);



            /* DETLO DIVIDED SIGNAL TO RANGING/DETECTOR */



        /* CLOSE THE PATH RELAYS TO MAKE THE DETHI GROUNDED MEASUREMENT */

        /* 'SOME' OF THESE RELAYS MUST BE CLOSED AFTER THE DCV_SETUP ROUTINE */

        vth_aclose_rly(V.mod_id, K2309);

            /* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */

		vth_aclose_rly(V.mod_id, K2315);

		vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN); //added for 99V	test

        vth_aclose_rly(V.mod_id, K2306);   /* DETHI TO GROUND SIGNAL PATH */

		vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);

		vth_aclose_rly(V.mod_id, K2304);   /* DETHI TO GROUND SIGNAL PATH */

        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);

		//vth_mtmr_wait(mod_id, 0.01);

//removed        setup_PS_N6746B_detector_test(V.mod_id, test[point], 0.10);  //turn on PS N6746B Fikri 15-Dec-2022
//removed     	vth_mtmr_wait(V.mod_id, 3.0);
		setup_PS_N6746B_detector_test(V.mod_id, test[point], 0.025); // quah ON PS - 4/2/25
		vth_mtmr_wait(mod_id, 10);

        massage_and_log_results(&V); //should step into this function to read variable

        vth_astat_clr(V.mod_id);

//removed       setup_PS_N6746B_detector_test_off(V.mod_id, test[point], 0.10);  //turn off PS N6746B Fikri 15-Dec-2022
//removed      	vth_mtmr_wait(V.mod_id, 3.0);
		setup_PS_N6746B_detector_test_off(V.mod_id, test[point], 0.025); // quah cold switching - off PS 4/2/25
		vth_mtmr_wait(mod_id, 1);


        /* OPEN THE RELAYS USED FOR THE DETHI GROUNDED MEASUREMENT */

        vth_aopen_rly(V.mod_id, K2309);

            /* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */

        vth_aopen_rly(V.mod_id, K2315);

		vth_aopen_rly(V.mod_id, K2310);//jack

		vth_aopen_rly(V.mod_id, K2316);//jack

            /* DETLO DIVIDED SIGNAL TO RANGING/DETECTOR */

        vth_aopen_rly(V.mod_id, K2306);   /* DETHI TO GROUND SIGNAL PATH */

        vth_aopen_rly(V.mod_id, K2304);   /* DETHI TO GROUND SIGNAL PATH */

        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);





        /* VERSION 8.2 REQUIRES THAT THE SETUP BE REPEATED BEFORE ANOTHER MEAS */

        vth_dcv_setup_sales(V.mod_id, length, AVERAGES, DIGITAL, range_voltage, TIMEOUT);



        /* CLOSE THE APPROPRIATE PATH RELAYS TO MAKE THE DIFF MEASUREMENT */

        /* 'SOME' OF THESE RELAYS MUST BE CLOSED AFTER THE DCV_SETUP ROUTINE */

        vth_aclose_rly(V.mod_id, K2310);

            /* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */

        vth_aclose_rly(V.mod_id, K2316);

            /* DETHI DIVIDED SIGNAL TO RANGING/DETECTOR */

        vth_aclose_rly(V.mod_id, K2309);

            /* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */

        vth_aclose_rly(V.mod_id, K2315);

            /* DETLO DIVIDED SIGNAL TO RANGING/DETECTOR */

        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);

 //removed       setup_PS_N6746B_detector_test(V.mod_id, test[point], 0.10);  //turn on PS N6746B Fikri 15-Dec-2022
 //removed   	vth_mtmr_wait(V.mod_id, 3.0);
		
		setup_PS_N6746B_detector_test(V.mod_id, test[point], 0.025); // quah ON PS - 4/2/25
		vth_mtmr_wait(mod_id, 10);
		
        massage_and_log_results(&V);

        vth_astat_clr(V.mod_id);

  //removed      setup_PS_N6746B_detector_test_off(V.mod_id, test[point], 0.10);  //turn off PS N6746B Fikri 15-Dec-2022
 //removed   	vth_mtmr_wait(V.mod_id, 3.0);

		/* OPEN THE APPROPRIATE PATH RELAYS TO MAKE THE DIFF MEASUREMENT */

        vth_aopen_rly(V.mod_id, K2310);

            /* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */

        vth_aopen_rly(V.mod_id, K2316);

            /* DETHI DIVIDED SIGNAL TO RANGING/DETECTOR */

        vth_aopen_rly(V.mod_id, K2309);

            /* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */

        vth_aopen_rly(V.mod_id, K2315);

            /* DETLO DIVIDED SIGNAL TO RANGING/DETECTOR */

        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS_OPEN);



      }  /*for polarity*/





    }  /*for integrate length*/



  }  /*for point*/


  vth_mod_init(V.mod_id);

//  close_ps(V.mod_id);
  close_PS_N6746B(mod_id);
  vth_mtmr_wait(V.mod_id, 3.0);

  close_dmm(V.mod_id);
  close_34980A(V.mod_id);//close_mux(V.mod_id);
  CloseResourceManager();//chun addded 07032022
  vth_mtmr_wait(mod_id, 1);

  //flush_commands(mod_id);//remove flush trial quah 21/3/25
  vth_mtmr_wait(mod_id, 1);


}  /*test_det_atten_range*/

#undef AVERAGES

#undef DIGITAL

#undef RAMP_TIME_ALLOWED

#undef DIFF_V

#undef RESOLUTION

#undef POWER_SUPPLY_CURRENT

#undef TIMEOUT

#undef DMM_FUNCTION



#define BUZZ_WAIT       2.0e-3

/* ------------------------------------------------------------------------ */

/* Procedure buzz_prelays from ~/sccs/dgn/src/tests/hkmttools.p */

void buzz_prelays(int mod_id, int slot)

{

  int i;   /*index for repeat time*/



  for (i = 1; i <= 20; i++)

  {  /*of for I*/

    vth_pwr_cell(mod_id, slot, PRLY_1A, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_2A, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_3A, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_4A, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_5A, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_6A, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_1B, 0);

    vth_pwr_cell(mod_id, slot, PRLY_2B, 0);

    vth_pwr_cell(mod_id, slot, PRLY_3B, 0);

    vth_pwr_cell(mod_id, slot, PRLY_4B, 0);

    vth_pwr_cell(mod_id, slot, PRLY_5B, 0);

    vth_pwr_cell(mod_id, slot, PRLY_6B, 0);



    vth_mtmr_wait(mod_id, BUZZ_WAIT);



    vth_pwr_cell(mod_id, slot, PRLY_1A, 0);

    vth_pwr_cell(mod_id, slot, PRLY_2A, 0);

    vth_pwr_cell(mod_id, slot, PRLY_3A, 0);

    vth_pwr_cell(mod_id, slot, PRLY_4A, 0);

    vth_pwr_cell(mod_id, slot, PRLY_5A, 0);

    vth_pwr_cell(mod_id, slot, PRLY_6A, 0);

    vth_pwr_cell(mod_id, slot, PRLY_1B, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_2B, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_3B, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_4B, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_5B, 0xffff);

    vth_pwr_cell(mod_id, slot, PRLY_6B, 0xffff);



    vth_mtmr_wait(mod_id, BUZZ_WAIT);

  }  /*of for I*/



  /*initialize all PIN relays*/

  vth_popen_all_rlys(mod_id, slot);

}  /*of buzz_prelays*/

#undef BUZZ_WAIT



/* ----------------------------------------------------------------------- */

#ifdef SHOW_DEBUG

void sales_show_state(int  mod_id, char* toggle_label, int conditional)

#else

void sales_show_state(int, char*, int)

#endif

{

#ifdef SHOW_DEBUG

  flush_commands(mod_id);

  s:=toggle_label;

  strappend(s,' = ');

  current_position:= mtd_strlen(s) + 1;

  strappend(s,(conditional ? 'true' : 'false'))

  display_prompt(s,true);

#endif

}



/* ----------------------------------------------------------------------- */

//DOWD 6/22/04: updated show_value to eliminate pascal conversion dependancies

/*#ifdef SHOW_DEBUG

void show_value(int mod_id, char *value_label, double value)

#else

void show_value(int, char*, double)

#endif

{

#ifdef SHOW_DEBUG

  flush_commands(mod_id);

  s:=value_label;

  strappend(s,' = ');

  current_position:= mtd_strlen(s) + 1;

  strwrite(s,current_position,next_position,value);

  display_prompt(s,true);

#endif

}*/



void show_value(int mod_id, char *label, double value)

{

  char s[81];

  int current_position;

  int next;

  test_result_type result;



  strcpy(s, label);

  /* SYNC UP THE PIPE */

  if (get_result_nolog(mod_id, &result))

    display_prompt("Unexpected result pending.", true);

  current_position = strlen(s) + 1;

  if (current_position < 80)

  {

    strcat(s, " ");

    sprintf(s + current_position, "%1.12f", value);

    next = strlen(s) + 1;

  }

  display_prompt(s, true);

}

/****************************************************************************
*
*Start of Sales rack calls
*
****************************************************************************/
void OpenResourceManager()
{
	err = viOpenDefaultRM(&rsm);
	if( err != 0 )
    {
       display_prompt("Can't open resource manager.", true);
       exit( 1 );
    }
}

/*------------------------------------------------------------------------*/

void CloseResourceManager()
{
	err = viClose(rsm);
	if( err != 0 )
    {
       display_prompt("Can't close resource manager.", true);
       exit( 1 );
    }
}

/*------------------------------------------------------------------------*/

void send_msg(char *command, int instrument)
{
// This function will send a  command string to the GPIB
// port.

	unsigned long	actual;
	char			message[1024];

	strcpy(message, command);
	strcat(message, "\n");
	err = viWrite(viDMM, (ViBuf)message, strlen(message), &actual);

	if (err < VI_SUCCESS)
	{
		printf("I/O ERROR = %s\n", err);
	}

}

/*------------------------------------------------------------------------*/

void err_handler()
{
	char err_msg[1024] = {0};
	viStatusDesc(rsm, err, err_msg);
	printf("ERROR = %s\n", err_msg);
}



/* ----------------------------------------------------------------------- */

/*s*/

/* wpd - include begins here */

/* Use tag 5 for the Digital Multimeter */



/*------------------------------------------------------------------------*/

void open_dmm(int mod_id)

{

  test_result_type result;



  // SYNC UP THE PIPE   //mjs added this

  if (get_result_nolog(mod_id, &result))

    display_prompt("Failed to sync up pipe when opening dmm", true);



  vth_mhpib_open(mod_id, 5, "/dev/sales.dmm");

  vth_mhpib_wr(mod_id, 5, "RESET");

}  /*open_dmm*/



/*------------------------------------------------------------------------*/

void setup_dmm(int mod_id, int dmm_function, double range, double resolution)

{

  char setup_string[81];



  int current_position;

  int next_position;



  double percent_res;



  *setup_string = '\0';

  strcpy(setup_string, "FUNC ");

  if (dmm_function == FOUR_WIRE_OHMS)

    strcat(setup_string, "OHMF ");//4 wire measurement

  else if (dmm_function == TWO_WIRE_OHMS)

    strcat(setup_string, "OHM ");//2 wire measurement

  else if (dmm_function == DCV)

    strcat(setup_string, "DCV ");//select DC voltage

  else if (dmm_function == ACV)

    strcat(setup_string, "ACV ");

  else if (dmm_function == FR)

    strcat(setup_string, "FREQ ");

  else if (dmm_function == PER)

    strcat(setup_string, "PER ");

  else if (dmm_function == FIXED_Z)

    strcat(setup_string, "FIXEDZ ON");



  /* 11-26-90 JRM Check to see if the range is less than the allowable range */

  /* Also add abs(range) to set the range correctly. */

  range = fabs(range);

  if (range == 0 && dmm_function == DCV)

    range = 0.100;

  if (range == 0 && dmm_function == ACV)

    range = 0.010;



  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.2f", range);

  next_position = mtd_strlen(setup_string) + 1;



  strcat(setup_string, ",");

  current_position = next_position + 1;

  percent_res = resolution / range * 100;



  if (dmm_function != FR && dmm_function != PER)

  {

    sprintf(setup_string + current_position - 1, "%1.4f", percent_res);

    next_position = mtd_strlen(setup_string) + 1;

  }

  else

  {

    sprintf(setup_string + current_position - 1, "%1.5f", resolution);

    next_position = mtd_strlen(setup_string) + 1;

  }



  vth_mhpib_wr(mod_id, 5, "NPLC 1");

  vth_mhpib_wr(mod_id, 5, setup_string);

  if (dmm_function == ACV)

    vth_mhpib_wr(mod_id, 5, "SETACV SYNC");



}  /*setup_dmm*/



/*------------------------------------------------------------------------*/

void setup_dmm_ac_band(int mod_id, int start_freq, int stop_freq)

{

  char setup_string[81];



  int current_position;

  int next_position;





  *setup_string = '\0';

  strcpy(setup_string, "ACBAND ");

  current_position = mtd_strlen(setup_string) + 1;

  if (start_freq != 0)

  {

    sprintf(setup_string + current_position - 1, "%d", start_freq);

    next_position = mtd_strlen(setup_string) + 1;

  }

  else

    strcat(setup_string, "0.5");

  strcat(setup_string, ",");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", stop_freq);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);

}  /*setup_dmm_AC_Band*/



/*------------------------------------------------------------------------*/

void setup_dmm_aperature(int mod_id, double aperature)

{

  char setup_string[81];



  int current_position;

  int next_position;





  *setup_string = '\0';

  strcpy(setup_string, "APER ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%10.8f", aperature);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);



}  /*setup_dmm_aperature*/



/*------------------------------------------------------------------------*/

void setup_dmm_fixed_z(int mod_id, int on)

{

//  if (on == true) //fikri edit 03032025
  if (on == 1)

    vth_mhpib_wr(mod_id, 5, "FIXEDZ ON");

  else

    vth_mhpib_wr(mod_id, 5, "FIXEDZ OFF");

}  /*setup_dmm_fixed_z*/



/*------------------------------------------------------------------------*/

void setup_dmm_fsource(int mod_id, int fsource)

{

  char setup_string[81];

  int current_position;



  *setup_string = '\0';

  strcpy(setup_string, "FSOURCE ");

  current_position = mtd_strlen(setup_string) + 1;

  if (fsource == ACV)

    strcat(setup_string, "ACV");

  else if (fsource == DCV)

    strcat(setup_string, "ACDCV");

  vth_mhpib_wr(mod_id, 5, setup_string);



}  /*setup_dmm_fsource*/



/*------------------------------------------------------------------------*/

void setup_dmm_level(int mod_id, int trigger_percent, int coupling)

{

  char setup_string[81];



  int current_position;

  int next_position;



  *setup_string = '\0';

  strcpy(setup_string, "LEVEL ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", trigger_percent);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", coupling);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);



}  /*setup_dmm_level*/



/*------------------------------------------------------------------------*/

void setup_dmm_level_filter(int mod_id, int filter_control)

{

  if (filter_control == true)

    vth_mhpib_wr(mod_id, 5, "LFILTER ON");

  else

    vth_mhpib_wr(mod_id, 5, "LFILTER OFF");



}  /*setup_dmm_level_filter*/



/*------------------------------------------------------------------------*/

void setup_dmm_nplc(int mod_id, int nplc)

{

  char setup_string[81];



  int current_position;

  int next_position;



  *setup_string = '\0';

  strcpy(setup_string, "NPLC ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", nplc);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);

}  /*setup_dmm_NPLC*/



/*------------------------------------------------------------------------*/

void setup_dmm_memory(int mod_id, int fmat, int mem)

{

  switch (fmat)

  {



  case ASCII:

    vth_mhpib_wr(mod_id, 5, "MFORMAT ASCII");

    break;



  case SINT:

    vth_mhpib_wr(mod_id, 5, "MFORMAT SINT");

    break;



  case DINT:

    vth_mhpib_wr(mod_id, 5, "MFORMAT DINT");

    break;



  case SREAL:

    vth_mhpib_wr(mod_id, 5, "MFORMAT SREAL");

    break;



  case DREAL:

    vth_mhpib_wr(mod_id, 5, "MFORMAT DREAL");

    break;



  default:

    assert(false);

    break;



  }

  switch (mem)

  {



  case MOFF:

    vth_mhpib_wr(mod_id, 5, "MEM OFF");

    break;



  case LIFO:

    vth_mhpib_wr(mod_id, 5, "MEM LIFO");

    break;



  case FIFO:

    vth_mhpib_wr(mod_id, 5, "MEM FIFO");

    break;



  case CONT:

    vth_mhpib_wr(mod_id, 5, "MEM CONT");

    break;



  default:

    assert(false);

    break;



  }

}



/*------------------------------------------------------------------------*/

void setup_dmm_ocomp_on(int mod_id)

{

  vth_mhpib_wr(mod_id, 5, "OCOMP ON");

}  /*setup_dmm_ocomp_on*/



/*------------------------------------------------------------------------*/

void measure_dmm(int mod_id)

{

  vth_mhpib_wr(mod_id, 5, "TRIG SGL");

  vth_mhpib_num_rd(mod_id, 5);

}  /*measure_dmm*/



/*------------------------------------------------------------------------*/

void close_dmm(int mod_id)

{

  vth_mhpib_wr(mod_id, 5, "RESET");

  vth_mhpib_close(mod_id, 5);

}  /*close_dmm*/



/*------------------------------------------------------------------------*/

void read_dmm_stats(int mod_id, int dmm_stat)

{

  switch (dmm_stat)

  {



  case SDEV:

    vth_mhpib_wr(mod_id, 5, "RMATH SDEV");

    vth_mhpib_num_rd(mod_id, 5);

    break;



  case MEAN:

    vth_mhpib_wr(mod_id, 5, "RMATH MEAN");

    vth_mhpib_num_rd(mod_id, 5);

    break;



  case N:

    vth_mhpib_wr(mod_id, 5, "RMATH NSAMP");

    vth_mhpib_num_rd(mod_id, 5);

    break;



  case SALES_MAX:

    vth_mhpib_wr(mod_id, 5, "RMATH UPPER");

    vth_mhpib_num_rd(mod_id, 5);

    break;



  case SALES_MIN:

    vth_mhpib_wr(mod_id, 5, "RMATH LOWER");

    vth_mhpib_num_rd(mod_id, 5);

    break;



  case ALL_STATS:

    vth_mhpib_wr(mod_id, 5, "RMATH MEAN");

    vth_mhpib_num_rd(mod_id, 5);

    vth_mhpib_wr(mod_id, 5, "RMATH SDEV");

    vth_mhpib_num_rd(mod_id, 5);

    vth_mhpib_wr(mod_id, 5, "RMATH NSAMP");

    vth_mhpib_num_rd(mod_id, 5);

    vth_mhpib_wr(mod_id, 5, "RMATH LOWER");

    vth_mhpib_num_rd(mod_id, 5);

    vth_mhpib_wr(mod_id, 5, "RMATH UPPER");

    vth_mhpib_num_rd(mod_id, 5);

    break;



  default:

    assert(false);

    break;



  }/* case */

}



/*------------------------------------------------------------------------*/

void setup_dmm_readings(int mod_id, int n)

{

  char setup_string[81];

  int c_pos;

  int n_pos;



  strcpy(setup_string, "NRDGS ");

  c_pos = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + c_pos - 1, "%d", n);

  n_pos = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);

}



/*------------------------------------------------------------------------*/

void calculate_dmm_stats(int mod_id)

{

  vth_mhpib_wr(mod_id, 5, "MMATH STAT");

}



/*------------------------------------------------------------------------*/

void hold_dmm(int mod_id)

{

  vth_mhpib_wr(mod_id, 5, "TRIG HOLD");

}



/*------------------------------------------------------------------------*/

void trigger_dmm(int mod_id)

{

  vth_mhpib_wr(mod_id, 5, "TRIG SGL");

}



/*------------------------------------------------------------------------*/

void read_dmm(int mod_id)

{

  vth_mhpib_num_rd(mod_id, 5);

}



/*------------------------------------------------------------------------*/

void enable_dmm_buffer(int mod_id)

{

  vth_mhpib_wr(mod_id, 5, "INBUF ON");

}



/*------------------------------------------------------------------------*/

void disable_dmm_buffer(int mod_id)

{

  vth_mhpib_wr(mod_id, 5, "INBUF OFF");

}



#define MARK_NUMBER     0

#define MAX_TIME        190.0e+00

/* ---------------------------------------------------------------------- */

void wait_for_dmm_measurement(int mod_id)

{

  double time;

  boolean ready;

  test_result_type result;





  ready = 0; // false

  vth_mtmr_set_mark(mod_id, MARK_NUMBER);

  do

  {

    ready = dmm_measurement_complete();

    vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUMBER);

    if (get_result_nolog(mod_id, &result))

      time = result.UU.Real_result;

    else

    {

      time = MAX_TIME;

      display_prompt("No result in result log (Expected mark time)", true);

    }

  }  while (!(ready || time >= MAX_TIME));

  if (!ready)

    display_prompt("GPIB Timeout of DMM waiting for measurement complete",

                   true);

}  /*wait_for_dmm_measurement*/

#undef MARK_NUMBER

#undef MAX_TIME



/*------------------------------------------------------------------------*/

void autocal_dmm(int mod_id)

{

  enable_dmm_buffer(mod_id);

  vth_mhpib_wr(mod_id, 5, "ACAL DCV,3458");

  flush_commands(mod_id);

  wait_for_dmm_measurement(mod_id);

  flush_commands(mod_id);

  vth_mhpib_wr(mod_id, 5, "ACAL AC,3458");

  flush_commands(mod_id);

  wait_for_dmm_measurement(mod_id);

  flush_commands(mod_id);

}  /*autocal_dmm*/



void setup_dcv_dig(int mod_id, int range, double interval, int num_samples, double aperature)

{

  char setup_string[81];

  int current_position;

  int next_position;



  vth_mhpib_wr(mod_id, 5, "PRESET DIG");   /* PRESET DIG command */



  strcpy(setup_string, "APER ");   /* APER command */

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .3E", aperature);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);



  vth_mhpib_wr(mod_id, 5, "MEM FIFO");   /* MEM FIFO */

  vth_mhpib_wr(mod_id, 5, "OFORMAT SREAL");

  if (aperature > 1.4e-6)

    vth_mhpib_wr(mod_id, 5, "MFORMAT DINT");

  else

    vth_mhpib_wr(mod_id, 5, "MFORMAT SINT");



  strcpy(setup_string, "DCV ");   /* DCV range */

  sprintf(setup_string + 4, "%d", range);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);



  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */

  sprintf(setup_string + 6, "% .4E", interval);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",");

  sprintf(setup_string + next_position, "%d", num_samples);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);



  vth_mhpib_wr(mod_id, 5, "LEVEL 0, DC");

  //vth_mhpib_wr(mod_id, 5, "LFILTER ON");// commented by anita for scope elim

}  /* setup_dcv_dig */



/* ----------------------------------------------------------------------- */

void setup_ssdc_dig(int mod_id, int range, double interval, int num_samples)

{

  char setup_string[81];

  int next_position;



  vth_mhpib_wr(mod_id, 5, "PRESET DIG");   /* PRESET DIG command */

  vth_mhpib_wr(mod_id, 5, "MEM FIFO");   /* MEM FIFO */

  vth_mhpib_wr(mod_id, 5, "OFORMAT SREAL");   /* OFORMAT SREAL */



  strcpy(setup_string, "SSDC ");

  sprintf(setup_string + 5, "%d", range);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);



  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */

  sprintf(setup_string + 6, "% .3E", interval);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",");

  sprintf(setup_string + next_position, "%d", num_samples);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);



  vth_mhpib_wr(mod_id, 5, "LEVEL 0, DC");

  vth_mhpib_wr(mod_id, 5, "LFILTER ON");

  vth_mhpib_wr(mod_id, 5, "SSRC LEVEL");

}  /* setup_ssdc_dig */



/* ----------------------------------------------------------------------- */

void setup_ssdc_fast(int mod_id, int range, int level, int slope, double interval, double sample_delay, int num_samples)

{

  char setup_string[81];

  int next_position;

  vth_mhpib_wr(mod_id, 5, "PRESET FAST");   /* PRESET FAST command */

  vth_mhpib_wr(mod_id, 5, "MEM FIFO");   /* MEM FIFO */

  vth_mhpib_wr(mod_id, 5, "OFORMAT SREAL");   /* OFORMAT SREAL */

  vth_mhpib_wr(mod_id, 5, "MFORMAT SINT");   /* MFORMAT SINT */



  strcpy(setup_string, "SSDC ");

  sprintf(setup_string + 5, "%d", range);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);



  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */

  sprintf(setup_string + 6, "% .3E", interval);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",");

  sprintf(setup_string + next_position, "%d", num_samples);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);



  strcpy(setup_string, "LEVEL ");

  sprintf(setup_string + 6, "%d", level);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ", DC");

  vth_mhpib_wr(mod_id, 5, setup_string);



  if (slope == NEG)

    vth_mhpib_wr(mod_id, 5, "SLOPE NEG");



  strcpy(setup_string, "DELAY ");

  sprintf(setup_string + 6, "% .5E", sample_delay);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 5, setup_string);



  vth_mhpib_wr(mod_id, 5, "SSRC LEVEL");

}  /* setup_ssdc_fast */



/* ----------------------------------------------------------------------- */



//=======================================================================

//-------------------------------------------------------------------------

/* Floating point conversion program for SREAL format given out by the 3458 DMM

   mjs 02/27/97

   */



/* floating point SREAL format of the dmm:



   byte 0    byte 1    byte2     byte3

   SEEEEEEE  EMMMMMMM  MMMMMMMM  MMMMMMMM

   76543210  76543210  76543210  76543210

   */

double floating_point(int byte0, int byte1, int byte2, int byte3)

{



  int sign = 1;   //sign of the floating point number (either +1 or -1)

  int exponent = 0;   //exponent for floating point number

  double expmultiply = 1;   //exponent multiplier for final fp number calc

  double weight = 1;     //weight of bits used to calculate mantissa

  double sum = 0;     //mantissa sum

  double temp = 0;

  int i;



//the sign bit is the MSB of byte0

  if(P_getbits_US(byte0, 7, 0)) (sign = -1);



//the exponent is the last seven bits of byte0 and the first bit of byte 1

  exponent = 2*byte0;  //shift bits to the left by one

  P_getbits_US(byte1, 7, 0) ? (exponent += 1) : (exponent += 0);



//  display_it(2, "raw exponent = ", exponent, 999);



  for (i=0; i<8; i++)

  {

    if (P_getbits_US(exponent, i, 0)) temp += weight;

    weight *=2;

  }



//  display_it(2, "exponent before biasing (after weights)  = ", 999, temp);



  exponent = static_cast<int>(temp - 127.0);



//  display_it(2, "exponent after biasing = ", exponent, 999);



  if (exponent < 0)

     for (i = 0; i < (exponent *(-1)); i++)   expmultiply *= .5;



  if (exponent == 0) expmultiply = 1;



  if (exponent > 0)

     for (i=0; i<exponent; i++)           expmultiply *= 2;



//  display_it(2, "exponent multiplier = ", 999, expmultiply);



//calculate mantissa from the last three bytes

  weight = .5;

  for (i=6; i>-1; i--) //mantissa for byte 1

  {

    P_getbits_US(byte1, i, 0) ? (sum += weight) : (sum += 0);

    weight /= 2; //weights of bits

  }

  for (i=7; i>-1; i--) //mantissa for byte 2

  {

    P_getbits_US(byte2, i, 0) ? (sum += weight) : (sum += 0);

    weight /= 2; //weights of bits

  }

  for (i=7; i>-1; i--) //mantissa for byte 3

  {

    P_getbits_US(byte3, i, 0) ? (sum += weight) : (sum += 0);

    weight /= 2; //weights of bits

  }



  return (sign * expmultiply * (1+sum));

}



//=======================================================================

/* ----------------------------------------------------------------------- */

void capture_ddm(int mod_id, int num_samples, double *waveform, int mode)

{

  temp_array t;

  int j;

  int error;

  int srq;

  int bytes_read;

  hfs_fd_type* id;

  cmd_type command;

  path_type path;

  int mla;

  int byte0 = 0; //byte 0 of sample

  int byte1 = 0; //byte 1 of sample

  int byte2 = 0; //byte 2 of sample

  int byte3 = 0; //byte 3 of sample



  /*trigger sampler*/

  enable_dmm_buffer(mod_id);

  if (mode == LEVEL)

    vth_mhpib_wr(mod_id, 5, "TARM SGL");

  else

  {

    vth_mhpib_wr(mod_id, 5, "TRIG HOLD");

    vth_mhpib_wr(mod_id, 5, "TARM AUTO");

    vth_mhpib_wr(mod_id, 5, "TRIG SGL");

  }



  flush_commands(mod_id);

  wait_for_dmm_measurement(mod_id);



  /* get data from Digital Sampler */

  strcpy(path, "/dev/sales.dmm");

  c_hfs_open(&id, path,HACCESS_READ_WRITE, &error);

  if (error == HFS_OPEN_ERROR)

    display_prompt("WARNING: unable to open GPIB file", true);



  c_hfs_hpib_srq(id, &srq, &error);

  mla = srq + 32;



  command[0] = UNTALK;

  command[1] = UNLISTEN;

  command[2] = TALK + DMM_3458;   /* talk address for dmm */

  command[3] = mla;   /* my listen address */

  command[4] = '\0';

  c_hfs_hpib_send_command(id, command, 4, &error);

//  display_it(mod_id, command, 7, 7);



  nhfs_read_bytes(id, 1, MAX_SAMPLES * 4, t, num_samples * 4, &bytes_read,

                  &error);

  if (error == HFS_HPIB_READ_FAILED)

    display_prompt("Cannot vth_read data from dmm in CAPTURE", true);



  command[2] = '\0';

  c_hfs_hpib_send_command(id, command, 2, &error);



  c_hfs_close(id, &error);

  if (error == HFS_CLOSE_ERROR)

    display_prompt("WARNING: unable to close dmm file", true);



  flush_commands(mod_id);



  for (j = 0; j < num_samples; j++)

  {

    byte0 = t[0 + ((j)*4)];

    byte1 = t[1 + ((j)*4)];

    byte2 = t[2 + ((j)*4)];

    byte3 = t[3 + ((j)*4)];

    waveform[j] = floating_point(byte0, byte1, byte2, byte3);

  } //for j loop



  disable_dmm_buffer(mod_id);

}  /*capture_ddm*/



//-----------------------------------------------------------------------

/* Use tag 9 for the E1406A command module */

void open_com(int mod_id)

{

  vth_mhpib_open(mod_id, 9, "/dev/e1406a");

} /*open_com*/



void close_com(int mod_id)

{

  vth_mhpib_close(mod_id, 9);

}  /*close_com*/



//-----------------------------------------------------------------------

/* Use tag 8 for the Digital Multimeter with 50 ohm load */

void open_dmm50(int mod_id)

{

  vth_mhpib_open(mod_id, 8, "/dev/sales.dmm50");

  vth_mhpib_wr(mod_id, 8, "RESET");

}  /*open_dmm50*/



/*------------------------------------------------------------------------*/

void setup_dmm50(int mod_id, int dmm_function, double range, double resolution)

{

  char setup_string[81];



  int current_position;

  int next_position;



  double percent_res;



  *setup_string = '\0';

  strcpy(setup_string, "FUNC ");

  if (dmm_function == FOUR_WIRE_OHMS)

    strcat(setup_string, "OHMF ");

  else if (dmm_function == TWO_WIRE_OHMS)

    strcat(setup_string, "OHM ");

  else if (dmm_function == DCV)

    strcat(setup_string, "DCV ");

  else if (dmm_function == ACV)

    strcat(setup_string, "ACV ");

  else if (dmm_function == FR)

    strcat(setup_string, "FREQ ");

  else if (dmm_function == PER)

    strcat(setup_string, "PER ");

  else if (dmm_function == FIXED_Z)

    strcat(setup_string, "FIXEDZ ON");



  /* 11-26-90 JRM Check to see if the range is less than the allowable range */

  /* Also add abs(range) to set the range correctly. */

  range = fabs(range);

  if (range == 0 && dmm_function == DCV)

    range = 0.100;

  if (range == 0 && dmm_function == ACV)

    range = 0.010;



  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.2f", range);

  next_position = mtd_strlen(setup_string) + 1;



  strcat(setup_string, ",");

  current_position = next_position + 1;

  percent_res = resolution / range * 100;



  if (dmm_function != FR && dmm_function != PER)

  {

    sprintf(setup_string + current_position - 1, "%1.4f", percent_res);

    next_position = mtd_strlen(setup_string) + 1;

  }

  else

  {

    sprintf(setup_string + current_position - 1, "%1.5f", resolution);

    next_position = mtd_strlen(setup_string) + 1;

  }



  vth_mhpib_wr(mod_id, 8, "NPLC 1");

  vth_mhpib_wr(mod_id, 8, setup_string);

  if (dmm_function == ACV)

    vth_mhpib_wr(mod_id, 8, "SETACV SYNC");



}  /*setup_dmm50*/



/*------------------------------------------------------------------------*/

void setup_dmm50_ac_band(int mod_id, int start_freq, int stop_freq)

{

  char setup_string[81];



  int current_position;

  int next_position;



  *setup_string = '\0';

  strcpy(setup_string, "ACBAND ");

  current_position = mtd_strlen(setup_string) + 1;

  if (start_freq != 0)

  {

    sprintf(setup_string + current_position - 1, "%d", start_freq);

    next_position = mtd_strlen(setup_string) + 1;

  }

  else

    strcat(setup_string, "0.5");

  strcat(setup_string, ",");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", stop_freq);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);



}  /*setup_dmm50_AC_Band*/



/*------------------------------------------------------------------------*/

void setup_dmm50_aperature(int mod_id, double aperature)

{

  char setup_string[81];



  int current_position;

  int next_position;



  *setup_string = '\0';

  strcpy(setup_string, "APER ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%10.8f", aperature);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);



}  /*setup_dmm50_aperature*/



/*------------------------------------------------------------------------*/

void setup_dmm50_fixed_z(int mod_id, int on)

{  /*setup_dmm_fixed_z*/

  if (on == true)

    vth_mhpib_wr(mod_id, 8, "FIXEDZ ON");

  else

    vth_mhpib_wr(mod_id, 8, "FIXEDZ OFF");

}  /*setup_dmm50_fixed_z*/



/*------------------------------------------------------------------------*/

void setup_dmm50_fsource(int mod_id, int fsource)

{

  char setup_string[81];

  int current_position;



  *setup_string = '\0';

  strcpy(setup_string, "FSOURCE ");

  current_position = mtd_strlen(setup_string) + 1;

  if (fsource == ACV)

    strcat(setup_string, "ACV");

  else if (fsource == DCV)

    strcat(setup_string, "ACDCV");

  vth_mhpib_wr(mod_id, 8, setup_string);



}  /*setup_dmm50_fsource*/



/*------------------------------------------------------------------------*/

void setup_dmm50_level(int mod_id, int trigger_percent, int coupling)

{

  char setup_string[81];



  int current_position;

  int next_position;



  *setup_string = '\0';

  strcpy(setup_string, "LEVEL ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", trigger_percent);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", coupling);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);

}  /*setup_dmm50_level*/



/*------------------------------------------------------------------------*/

void setup_dmm50_level_filter(int mod_id, int filter_control)

{

  if (filter_control == true)

    vth_mhpib_wr(mod_id, 8, "LFILTER ON");

  else

    vth_mhpib_wr(mod_id, 8, "LFILTER OFF");

}  /*setup_dmm50_level_filter*/



/*------------------------------------------------------------------------*/

void setup_dmm50_nplc(int mod_id, int nplc)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcpy(setup_string, "NPLC ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", nplc);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);

}  /*setup_dmm50_NPLC*/



/*------------------------------------------------------------------------*/

void setup_dmm50_memory(int mod_id, int fmat, int mem)

{

  switch (fmat)

  {



  case ASCII:

    vth_mhpib_wr(mod_id, 8, "MFORMAT ASCII");

    break;



  case SINT:

    vth_mhpib_wr(mod_id, 8, "MFORMAT SINT");

    break;



  case DINT:

    vth_mhpib_wr(mod_id, 8, "MFORMAT DINT");

    break;



  case SREAL:

    vth_mhpib_wr(mod_id, 8, "MFORMAT SREAL");

    break;



  case DREAL:

    vth_mhpib_wr(mod_id, 8, "MFORMAT DREAL");

    break;



  default:

    assert(false);

    break;



  }

  switch (mem)

  {



  case MOFF:

    vth_mhpib_wr(mod_id, 8, "MEM OFF");

    break;



  case LIFO:

    vth_mhpib_wr(mod_id, 8, "MEM LIFO");

    break;



  case FIFO:

    vth_mhpib_wr(mod_id, 8, "MEM FIFO");

    break;



  case CONT:

    vth_mhpib_wr(mod_id, 8, "MEM CONT");

    break;



  default:

    assert(false);

    break;



  }

}



/*------------------------------------------------------------------------*/

void setup_dmm50_ocomp_on(int mod_id)

{

  vth_mhpib_wr(mod_id, 8, "OCOMP ON");

}  /*setup_dmm50_ocomp_on*/



/*------------------------------------------------------------------------*/

void measure_dmm50(int mod_id)
{

		vth_mhpib_wr(mod_id, 8, "TRIG SGL");

		vth_mhpib_num_rd(mod_id, 8);


}  /*measure_dmm50*/



/*------------------------------------------------------------------------*/

void close_dmm50(int mod_id)

{

  vth_mhpib_wr(mod_id, 8, "RESET");

  vth_mhpib_close(mod_id, 8);

}  /*close_dmm50*/



/*------------------------------------------------------------------------*/

void read_dmm50_stats(int mod_id, int dmm_stat)

{

  switch (dmm_stat)

  {



  case SDEV:

    vth_mhpib_wr(mod_id, 8, "RMATH SDEV");

    vth_mhpib_num_rd(mod_id, 8);

    break;



  case MEAN:

    vth_mhpib_wr(mod_id, 8, "RMATH MEAN");

    vth_mhpib_num_rd(mod_id, 8);

    break;



  case N:

    vth_mhpib_wr(mod_id, 8, "RMATH NSAMP");

    vth_mhpib_num_rd(mod_id, 8);

    break;



  case SALES_MAX:

    vth_mhpib_wr(mod_id, 8, "RMATH UPPER");

    vth_mhpib_num_rd(mod_id, 8);

    break;



  case SALES_MIN:

    vth_mhpib_wr(mod_id, 8, "RMATH LOWER");

    vth_mhpib_num_rd(mod_id, 8);

    break;



  case ALL_STATS:

    vth_mhpib_wr(mod_id, 8, "RMATH MEAN");

    vth_mhpib_num_rd(mod_id, 8);

    vth_mhpib_wr(mod_id, 8, "RMATH SDEV");

    vth_mhpib_num_rd(mod_id, 8);

    vth_mhpib_wr(mod_id, 8, "RMATH NSAMP");

    vth_mhpib_num_rd(mod_id, 8);

    vth_mhpib_wr(mod_id, 8, "RMATH LOWER");

    vth_mhpib_num_rd(mod_id, 8);

    vth_mhpib_wr(mod_id, 8, "RMATH UPPER");

    vth_mhpib_num_rd(mod_id, 8);

    break;



  default:

    assert(false);

    break;



  }/* case */

}



/*------------------------------------------------------------------------*/

void setup_dmm50_readings(int mod_id, int n)

{

  char setup_string[81];

  int c_pos;

  int n_pos;



  strcpy(setup_string, "NRDGS ");

  c_pos = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + c_pos - 1, "%d", n);

  n_pos = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);

}



/*------------------------------------------------------------------------*/

void calculate_dmm50_stats(int mod_id)

{

    vth_mhpib_wr(mod_id, 8, "MMATH STAT");

}



/*------------------------------------------------------------------------*/

void hold_dmm50(int mod_id)

{

  vth_mhpib_wr(mod_id, 8, "TRIG HOLD");

}



/*------------------------------------------------------------------------*/

void trigger_dmm50(int mod_id)

{

  vth_mhpib_wr(mod_id, 8, "TRIG SGL");

}



/*------------------------------------------------------------------------*/

void vth_read_dmm50(int mod_id)

{

  vth_mhpib_num_rd(mod_id, 8);

}



/*------------------------------------------------------------------------*/

void enable_dmm50_buffer(int mod_id)

{

  vth_mhpib_wr(mod_id, 8, "INBUF ON");

}



/*------------------------------------------------------------------------*/

void disable_dmm50_buffer(int mod_id)

{

  vth_mhpib_wr(mod_id, 8, "INBUF OFF");

}



#define MARK_NUMBER     0

#define MAX_TIME        190.0e+00

/* ---------------------------------------------------------------------- */

void wait_for_dmm50_measurement(int mod_id)

{

  double time;

  boolean ready;

  test_result_type result;



  vth_mtmr_set_mark(mod_id, MARK_NUMBER);

  do

  {

    ready = dmm_measurement_complete();

    vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUMBER);

    if (get_result_nolog(mod_id, &result))

      time = result.UU.Real_result;

    else

    {

      time = MAX_TIME;

      display_prompt("No result in result log (Expected mark time)", true);

    }



  } while (!(ready || time >= MAX_TIME));

  if (!ready)

    display_prompt("GPIB Timeout of DMM waiting for measurement complete",

                   true);

}  /*wait_for_dmm50_measurement*/

#undef MARK_NUMBER

#undef MAX_TIME



/*------------------------------------------------------------------------*/

void autocal_dmm50(int mod_id)

{

  enable_dmm50_buffer(mod_id);

  vth_mhpib_wr(mod_id, 8, "ACAL DCV,3458");

  flush_commands(mod_id);

  wait_for_dmm50_measurement(mod_id);

  flush_commands(mod_id);

  vth_mhpib_wr(mod_id, 8, "ACAL AC,3458");

  flush_commands(mod_id);

  wait_for_dmm50_measurement(mod_id);

  flush_commands(mod_id);

}  /*autocal_dmm50*/





/*------------------------------------------------------------------------*/

void setup_dcv50_dig(int mod_id, int range, double interval, int num_samples, double aperature)

{

  char setup_string[81];

  int current_position;

  int next_position;



  vth_mhpib_wr(mod_id, 8, "PRESET DIG");   /* PRESET DIG command */



  strcpy(setup_string, "APER ");   /* APER command */

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .3E", aperature);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);



  vth_mhpib_wr(mod_id, 8, "MEM FIFO");   /* MEM FIFO */

  vth_mhpib_wr(mod_id, 8, "OFORMAT SREAL");



  if (aperature > 1.4e-6)

    vth_mhpib_wr(mod_id, 8, "MFORMAT DINT");

  else

    vth_mhpib_wr(mod_id, 8, "MFORMAT SINT");



  strcpy(setup_string, "DCV ");   /* DCV range */

  sprintf(setup_string + 4, "%d", range);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);



  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */

  sprintf(setup_string + 6, "% .4E", interval);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",");

  sprintf(setup_string + next_position, "%d", num_samples);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);



  vth_mhpib_wr(mod_id, 8, "LEVEL 0, DC");

  vth_mhpib_wr(mod_id, 8, "LFILTER ON");

}  /* setup_dcv50_dig */



/* ----------------------------------------------------------------------- */

void setup_ssdc50_dig(int mod_id, int range, double interval, int num_samples)

{

  char setup_string[81];

  int next_position;



  vth_mhpib_wr(mod_id, 8, "PRESET DIG");   /* PRESET DIG command */

  vth_mhpib_wr(mod_id, 8, "MEM FIFO");   /* MEM FIFO */

  vth_mhpib_wr(mod_id, 8, "OFORMAT SREAL");   /* OFORMAT SREAL */



  strcpy(setup_string, "SSDC ");

  sprintf(setup_string + 5, "%d", range);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);



  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */

  sprintf(setup_string + 6, "% .3E", interval);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",");

  sprintf(setup_string + next_position, "%d", num_samples);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);



  vth_mhpib_wr(mod_id, 8, "LEVEL 0, DC");

  vth_mhpib_wr(mod_id, 8, "LFILTER ON");

  vth_mhpib_wr(mod_id, 8, "SSRC LEVEL");

}  /* setup_ssdc50_dig */



/* ----------------------------------------------------------------------- */

void setup_ssdc50_fast(int mod_id, int range, int level, int slope, double interval, double sample_delay, int num_samples)

{

  char setup_string[81];

  int next_position;

  vth_mhpib_wr(mod_id, 8, "PRESET FAST");   /* PRESET FAST command */

  vth_mhpib_wr(mod_id, 8, "MEM FIFO");   /* MEM FIFO */

  vth_mhpib_wr(mod_id, 8, "OFORMAT SREAL");   /* OFORMAT SREAL */

  vth_mhpib_wr(mod_id, 8, "MFORMAT SINT");   /* MFORMAT SINT */



  strcpy(setup_string, "SSDC ");

  sprintf(setup_string + 5, "%d", range);

  next_position = strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);



  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */

  sprintf(setup_string + 6, "% .3E", interval);

  next_position = strlen(setup_string) + 1;

  strcat(setup_string, ",");

  sprintf(setup_string + next_position, "%d", num_samples);

  next_position = strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);



  strcpy(setup_string, "LEVEL ");

  sprintf(setup_string + 6, "%d", level);

  next_position = strlen(setup_string) + 1;

  strcat(setup_string, ", DC");

  vth_mhpib_wr(mod_id, 8, setup_string);



  if (slope == NEG)

    vth_mhpib_wr(mod_id, 8, "SLOPE NEG");



  strcpy(setup_string, "DELAY ");

  sprintf(setup_string + 6, "% .5E", sample_delay);

  next_position = strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 8, setup_string);



  vth_mhpib_wr(mod_id, 8, "SSRC LEVEL");

}  /* setup_ssdc50_fast */



/* ----------------------------------------------------------------------- */

void clear_dmm50_memory(int mod_id)

{  /* clear_ddm50_memory */

  vth_mhpib_wr(mod_id, 8, "MEM FIFO");   /* MEM FIFO */

}  /* clear_ddm50_memory */



/* ----------------------------------------------------------------------- */

void capture_ddm50(int mod_id, int num_samples, double *waveform, int mode)

{

  temp_array t;

  int j;

  int error;

  int srq;

  int bytes_read;

  hfs_fd_type* id;

  cmd_type command;

  path_type path;

  int mla;

  int byte0 = 0; //byte 0 of sample

  int byte1 = 0; //byte 1 of sample

  int byte2 = 0;  //byte 2 of sample

  int byte3 = 0;//byte 3 of sample



  /*trigger sampler*/

  enable_dmm50_buffer(mod_id);

  if (mode == LEVEL)

    vth_mhpib_wr(mod_id, 8, "TARM SGL");

  else

  {

    vth_mhpib_wr(mod_id, 8, "TRIG HOLD");

    vth_mhpib_wr(mod_id, 8, "TARM AUTO");

    vth_mhpib_wr(mod_id, 8, "TRIG SGL");

  }



  flush_commands(mod_id);

  wait_for_dmm_measurement(mod_id);



  /* get data from Digital Sampler */

  strcpy(path, "/dev/sales.dmm50");

  c_hfs_open(&id, path, HACCESS_READ_WRITE, &error);

  if (error == HFS_OPEN_ERROR)

    display_prompt("WARNING: unable to open GPIB file", true);



  c_hfs_hpib_srq(id, &srq, &error);

  mla = srq + 32;



  command[0] = UNTALK;

  command[1] = UNLISTEN;

  command[2] = TALK + 20;   /* talk address for #22 */

  command[3] = mla;   /* my listen address */

  command[4] = '\0';

  c_hfs_hpib_send_command(id, command, 4, &error);



  nhfs_read_bytes(id, 1, MAX_SAMPLES * 4, t, num_samples * 4, &bytes_read,

                  &error);

  if (error == HFS_HPIB_READ_FAILED)

    display_prompt("Cannot vth_read data from dmm in CAPTURE", true);



  command[2] = '\0';

  c_hfs_hpib_send_command(id, command, 2, &error);



  c_hfs_close(id, &error);

  if (error == HFS_CLOSE_ERROR)

    display_prompt("WARNING: unable to close dmm file", true);



  flush_commands(mod_id);



  for (j = 0; j < num_samples; j++)

  {

    byte0 = t[0 + ((j)*4)];

    byte1 = t[1 + ((j)*4)];

    byte2 = t[2 + ((j)*4)];

    byte3 = t[3 + ((j)*4)];

    waveform[j] = floating_point(byte0, byte1, byte2, byte3);

  }



  disable_dmm50_buffer(mod_id);

}  /*capture_ddm50*/



/*---------------------------------------------------------------------------*/

/* wpd - include begins here */

/*----------------------------------------------------------------------*/

void fft(double *xr, double *xi, int npts)

{

  int i;

  int j;

  int k;

  int ip;

  int l;

  int le;

  int le1;

  int m;

  int nv2;

  int nm1;

  double tr;

  double ti;

  double ur;

  double ui;

  double wr;

  double wi;

  double tmr;

  double tmi;

  double ln2;



  ln2 = log(2.0);

  m = round(log(static_cast<double>(npts)) / ln2);

  for (l = 0; l < m; l++)

  {

    le = round(exp((m - l) * ln2));

    le1 = round(le / 2.0);

    ur = 1.0;

    ui = 0.0;

    wr = cos(LONGPI / le1);

    wi = -sin(LONGPI / le1);

    for (j = 1; j <= le1; j++)

    {

      i = j;

      while (i <= npts)

      {

        ip = i + le1;

        tr = xr[i - 1] + xr[ip - 1];

        ti = xi[i - 1] + xi[ip - 1];

        tmr = xr[i - 1] - xr[ip - 1];

        tmi = xi[i - 1] - xi[ip - 1];

        xr[ip - 1] = tmr * ur - tmi * ui;

        xi[ip - 1] = tmr * ui + tmi * ur;

        xr[i - 1] = tr;

        xi[i - 1] = ti;

        i += le;

      }

      tr = ur * wr - ui * wi;

      ui = ur * wi + ui * wr;

      ur = tr;

    }

  }

  /* now for data reversal part */

  nv2 = round(npts / 2.0);

  nm1 = npts - 1;

  j = 1;

  for (i = 0; i < nm1; i++)

  {

    if (i + 1 < j)

    {

      tr = xr[j - 1];

      ti = xi[j - 1];

      xr[j - 1] = xr[i];

      xi[j - 1] = xi[i];

      xr[i] = tr;

      xi[i] = ti;

    }

    k = nv2;

    while (k < j)

    {

      j -= k;

      k = round(k / 2.0);

    }

    j += k;

  }

}



/* procedure FBT */

/*----------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/

/*   procedure copy(x, y, npts) :                                              */

/*                                                                             */

/*      The procedure copy copies the first npts elements of one array (x)     */

/*      into another array (y).                                                */

/*                                                                             */

/*         y[n] = x[n]                                                         */

/*                                                                             */

/*      where                                                                  */

/*         x    - source array from which to copy                              */

/*         y    - destination array to copy to                                 */

/*         npts - number of points                                             */

/*                                                                             */

/*     @(#)     Revision 1.1     86/06/26     11:23:36     Gary O'Brien        */

/*-----------------------------------------------------------------------------*/

void copy_(double *x, double *y, int npts)

{  /* start of procedure copy              */

  int n;   /* counter variable                     */



  for (n = 0; n < npts; n++)   /* loop through all points and assign   */

    y[n] = x[n];   /* the values in x to y                 */

}  /* end of copy                          */



/*-----------------------------------------------------------------------------*/

/*   procedure fill(x, cnst, npts) :                                           */

/*                                                                             */

/*      The procedure fill sets elements 1 through npts in the array x equal   */

/*      to the constant value cnst.                                            */

/*                                                                             */

/*         x[n] = cnst  where  n = 1, 2, ..., npts                             */

/*                                                                             */

/*      where                                                                  */

/*         x     - array to be acted upon                                      */

/*         cnst  - constant to which the array is to be set                    */

/*         npts  - number of elements in array                                 */

/*                                                                             */

/*     @(#)     Revision 1.1     86/06/30     16:38:21     Gary O'Brien        */

/*-----------------------------------------------------------------------------*/

void fill(double * x, double cnst, int npts)

{  /* start of procedure fill              */

  int n;   /* counter variable                     */



  for (n = 0; n < npts; n++)   /* loop through all elements of array   */

    x[n] = cnst;   /* and multiply each element by cnst    */

}  /* end of fill                          */



/*-----------------------------------------------------------------------------*/

/*   procedure mag(xr, xi, y, npts) :                                          */

/*                                                                             */

/*      The procedure mag computes the magnitude of npts individual complex    */

/*      vectors (xr(n), xi(n)) and stores the result in the return y(n).       */

/*                                                                             */

/*         y(n) = square root(xr[n]^2 + xi[n]^2)                               */

/*                                                                             */

/*      where                                                                  */

/*         xr(n) - real element of complex vector                              */

/*         xi(n) - complex element of complex vector                           */

/*         y(n)  - returned magnitudes                                         */

/*         npts  - number of vectors or points                                 */

/*                                                                             */

/*     @(#)     Revision 1.1     86/06/26     07:03:04     Gary O'Brien        */

/*-----------------------------------------------------------------------------*/

void mag(double * xr, double * xi, double * y, int npts)

{  /* start of procedure mag               */

  int n;   /* counter variable                     */



  for (n = 0; n < npts; n++)   /* loop through all points and compute  */

    y[n] = sqrt(xr[n] * xr[n] + xi[n] * xi[n]);   /* the magnitudes     */

}  /* end of mag                           */



/*-----------------------------------------------------------------------------*/

/*   procedure scalar_mult(x, cnst, npts) :                                    */

/*                                                                             */

/*      The procedure scalar_mult multiplies the elements 1 through npts in    */

/*      the array x by cnst and stores the value back into the array.          */

/*                                                                             */

/*         x[n] = x[n] * cnst                                                  */

/*                                                                             */

/*      where                                                                  */

/*         x     - array to be acted upon                                      */

/*         cnst  - constant by which the array is to be multiplied             */

/*         npts  - number of elements in array                                 */

/*                                                                             */

/*     @(#)     Revision 1.1     86/06/30     15:45:47     Gary O'Brien        */

/*-----------------------------------------------------------------------------*/

void scalar_mult(double *x, double cnst, int npts)

{  /* start of procedure scalar_mult       */

  int n;   /* counter variable                     */



  for (n = 0; n < npts; n++)   /* loop through all elements of array   */

    x[n] *= cnst;   /* and multiply each element by cnst    */

}  /* end of scalar_mult                   */



/*-----------------------------------------------------------------------------*/

static double find_dc_offset(double *x, int npts)

{

  int n;   /* counter variable                     */

  double sum;   /* running total */



  sum = 0.0;

  for (n = 0; n < npts; n++)

    sum += x[n];

  return (sum / npts);

}  /* find_dc_offset */



/*-----------------------------------------------------------------------------*/

static void blackman_window(double *data, int n)

{

  /* see Proakis p. 549 */

  double ang;

  double factor;

  int i;

  int j;



  ang = LONGTWO_PI / (n - 1);

  for (i = 0; i < n; i++)

  {

    j = i;

    factor = 0.42 - 0.5 * cos(ang * j) + 0.08 * cos(2 * ang * j);

    data[i] *= factor;

  }

}  /* blackman_window */



/*-----------------------------------------------------------------------------*/

void waveform_to_magnitude(double *x_, double *y, int npts)

{

  arrform x;

  arrform xi;

//dest, src for memcpy

  memcpy(x, x_, sizeof(arrform));

  blackman_window(x, npts);

  fill(xi, 0.0, npts);

  fft(x, xi, npts);

  mag(x, xi, y, npts);

}  /* waveform_to_magnitude */



/*-----------------------------------------------------------------------------*/

int find_dc_index(int n, double *y)

{



  /* This function finds the index of the first local minima of the magnitude

     array y of length n.  If the array is continously decreasing, n-1 is

     returned.                                                                 */

  int i;

//mjs -- changed this

  i = 0;

  while (y[i] > y[i+1] && i + 1 < n)

    i++;

  return i;

}  /* find_dc_index */



/*-----------------------------------------------------------------------------*/

int find_peak_index(int imin, int imax, double *y)

{

// mjs changed several of these correction factors-- may hurt other tests??



  /* This function finds the index of the peak magnitude of array y between

     the indices imin and imax inclusive.                                    */

  int result;

  int i;

  double ypeak;



  result = imin;

  ypeak = y[imin];

  for (i = imin + 1; i <= imax; i++)

  {

    if (y[i] > ypeak)

    {

      result = i;

      ypeak = y[i];

    }

  }

  return (result);

}  /* find_peak_index */



/*-----------------------------------------------------------------------------*/

void find_local_minima(int imin, int imax, int ipeak, double *y, double db_down, int *ilo, int *ihi)

{

  /* This procedure finds the local minima indices, ilo and ihi, of the

     magnitude array, y, around around the peak index, ipeak, that is atleast

     dB_down dB below the peak magnitude. If either of these conditions can not

     be met, the appropriate array end point, imin and/or imax, is returned.    */

  int i;

  double peak_limit;

//mjs changed some of these correction factors from the translation

  peak_limit = y[ipeak] * pow(10.0, db_down / 20.0);

  if (ipeak > imin)

  {

    i = ipeak - 1;

    while (y[i] > peak_limit && i > imin)

      i--;

    while (y[i] < y[i+1] && i > imin)

      i--;

    if (i == imin && y[i] < y[i+1])

      *ilo = i;

    else

      *ilo = i + 1;

  }

  else

    *ilo = imin;

  if (ipeak < imax)

  {

    i = ipeak + 1;

    while (y[i] > peak_limit && i < imax)

      i++;

    while (y[i] < y[i-1] && i < imax)

      i++;

    if (i == imax && y[i] < y[i - 1])

      *ihi = i;

    else

      *ihi = i - 1;

  }

  else

    *ihi = imax;

}  /* find_local_minima */



/*-----------------------------------------------------------------------------*/

double sum_power(int imin, int imax, double * y)

{

  /* This function finds the power of the magnitude array y from indices imin

     to imax.  Power is defined as the square of the magnitude.               */

  int i;

  double sum;



  sum = 0.0;

  for (i = imin - 1; i < imax; i++)

    sum += y[i] * y[i];



  return sum;

}  /* sum_power */



/*-----------------------------------------------------------------------------*/

int find_local_max(int imin, int imax, int iexp, double *y)

{

  /* This function finds local maximum of the magnitude array y about the

     expected value of iexp within the limits of imin to imax.  If iexp

     is a local minimum iexp = 0 is returned.                             */

  int result;

  int i;

  boolean up;

  boolean down;



  down = false;

  up = false;

  result = iexp;

  if (iexp > imin)

  {

    //if (y[iexp - 1] < y[iexp - 2])

    if (y[iexp] < y[iexp - 1])

      down = true;

  }

  if (iexp < imax)

  {

    //if (y[iexp - 1] < y[iexp])

    if (y[iexp] < y[iexp + 1])

      up = true;

  }

  if (down && !up)

  {  /* search down */

    i = iexp - 1;

    while (i > imin && down)

    {

      if (y[i - 1] < y[i - 2])

        i--;

      else

        down = false;

    }

    result = i;

  }  /* search down */

  else

  {

    if (up && !down)

    {  /* search up */

      i = iexp + 1;

      while (i < imax && up)

      {

        if (y[i - 1] < y[i])

          i++;

        else

          up = false;

      }

      result = i;

    }  /* search up */

  }

  if (up && down)   /* local minimum */

    //return 0;

    if (y[iexp - 1] < y[iexp + 1])

	return iexp + 1;

    else

	return iexp - 1;

  return result;

}  /* find_local_max */



/*-----------------------------------------------------------------------------*/

double find_harmonic_power(int ipeak, int imax, double *y, double db_down)

{



  /* This function finds the total harmonic power of the magnitude array y

     whose peak index is ipeak and length is imax.  At each harmonic, first

     the local maximum peak is found.  Next the surrounding local minima, which

     are atleast dB_down dB from the peak, are found.  Finally, the power,

     defined as the square of the magnitude, is summed over the range. If

     either of these conditions can not be met, the appropriate end point is

     used.                                                                      */

  int i;

  int offset;

  int last_min;

  int local_max;

  int ilo;

  int ihi;

  int next_peak;

  double sum;



  offset = ipeak - 1;

  i = ipeak + offset;

  sum = 0.0;

  last_min = ipeak;

  while (i <= imax)

  {  /* sum each harmonic */

    next_peak = i + offset;

    if (next_peak > imax)

      next_peak = imax;

    local_max = find_local_max(last_min, next_peak, i, y);

    if (local_max != 0)

    {  /* real harmonic peak */

      find_local_minima(last_min, next_peak, local_max, y, db_down, &ilo,

                        &ihi);

      if (ihi < next_peak)

      {  /* valid peak found */

        sum += sum_power(ilo, ihi - 1, y);

        last_min = ihi;

      }  /* valid peak found */

    }  /* real harmonic peak */

    else

    {  /* local minimum */

      sum += sum_power(i, i, y);

      last_min = i;

    }  /* local minimum */

    i += offset;

  }  /* sum each harmonic */

  return sum;

}  /* find_harmonic_power */



/*-----------------------------------------------------------------------------*/

/* wpd - include begins here */

/*+XOSC.P ............... use tag 0 for the 54111D digitizing oscilloscope*/



/*+15-NOV-89 (slc) added "sys_write" and "xvth_mhpib_wr" procedures to "osc.p"*/

/*+15-NOV-89 (slc) replaced all "vth_mhpib_wr" calls with "xvth_mhpib_wr"*/

/*------------------------------------------------------------------------*/

/*+xvth_mhpib_wr ................... write to 54111D scope in "raw" GPIB mode*/



void xvth_mhpib_wr(int mod_id, int tag, char *s_)

{

  char s[MAX_STRING + 1];



  cmd_type command;

  path_type path;

  p200 data;   /*packed array of chars*/

  hfs_fd_type* id;

  int bytes_written;

  int error;

  int srq;



  strcpy(s, s_);

  /*+MAKE SURE ALL PENDING TESTHEAD COMMANDS ARE FLUSHED*/

  flush_commands(mod_id);



  /*+OPEN THE RAW GPIB DEVICE*/

  switch (tag)

  {



  case 0:   /*sales scope*/

    memcpy(path, "/dev/sales.raw_hpib\0                              ",

           sizeof(path_type));

    break;



  case 8:   /*delay scope*/

    memcpy(path, "/dev/delay.raw_hpib\0                              ",

           sizeof(path_type));

    break;

  default:

    assert(false);

    break;



  }

  c_hfs_open(&id, path, HACCESS_READ_WRITE, &error);



  /*+BUILD AND SEND THE "ATTENTION BYTES"*/

  command[0] = UNLISTEN;

  c_hfs_hpib_srq(id, &srq, &error);

  command[1] = TALK + srq;   /*controller's talk address*/

  command[2] = LISTEN + 7;   /*scope's listen address*/

  c_hfs_hpib_send_command(id, command, 3, &error);



  /*+APPEND CR-LF TO MARK THE END OF THE INSTRUMENT'S COMMAND STREAM*/

  strcat(s, CRLF);



  /*+COPY THE DATA BYTES TO A PACKED ARRAY OF CHARACTERS*/

  memcpy(data,

    "                                                                                                                                                                                                        ",

    sizeof(p200));

  strmove(mtd_strlen(s), s, 1, data, 1);



  /*+SEND THE DATA BYTES AND THEN CLOSE THE RAW GPIB DEVICE*/

  c_hfs_write_bytes(id,data, mtd_strlen(s), &bytes_written, &error);

  c_hfs_close(id, &error);

  if (error == HFS_CLOSE_ERROR)

    display_prompt("WARNING: unable to close osc file", true);



  /*+ISSUE ANOTHER FLUSH COMMAND FOR GOOD MEASURE*/

  flush_commands(mod_id);

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_DEFAULTS ....................................................*/

void setup_osc_defaults(int mod_id)

{

  /*----------------------------------------------------------------------*/

  /* Modified: 11-30-90 JRM ----------------------------------------------*/

  /* Deleted the 'RES' command in the setup defaults (open) procedure.----*/

  /*----------------------------------------------------------------------*/

  /* vth_mhpib_wr(mod_id,0,'RES') ; */

  vth_mhpib_wr(mod_id, 0, "DISPLAY FORMAT SINGLE");

  vth_mhpib_wr(mod_id, 0, "BLANK CHANNEL1");

  vth_mhpib_wr(mod_id, 0, "BLANK CHANNEL2");

  vth_mhpib_wr(mod_id, 0, "TRIGGER MODE EDGE;SOURCE CHANNEL2");

  vth_mhpib_wr(mod_id, 0, "PROBE 1");

  vth_mhpib_wr(mod_id, 0, "SOURCE CHANNEL1");

  vth_mhpib_wr(mod_id, 0, "PROBE 1");

}



/*------------------------------------------------------------------------*/

/*+OPEN_OSC ..............................................................*/

void open_osc(int mod_id)

{

  vth_mhpib_open(mod_id, 0, "/dev/sales.osc");

  setup_osc_defaults(mod_id);

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_CHANNEL .....................................................*/



void setup_osc_channel(int mod_id, int osc_channel, int load,

                       double channel_range, double channel_offset)

{

  char setup_string[81];

  int current_position;

  int next_position;



  vth_mhpib_wr(mod_id, 0, "TIMEBASE MODE AUTOMATIC");

  /*------------------------------------------------------------------------*/

  /* Modified: 11/29/90 JRM ------------------------------------------------*/

  /* Deleted VIEW CHANNEL section from here and added it to the trigger_osc */

  /* procedure for all cases.  The purpose is to eliminate the waveform     */

  /* from begin displayed when it is not setup yet.                         */

  /*------------------------------------------------------------------------*/

  /* Deleted Section: ------------------------------------------------------*/

  /*        if osc_channel = 1 then                                         */

  /*          vth_mhpib_wr(mod_id,0,'VIEW CHANNEL1')                            */

  /*        else                                                            */

  /*          vth_mhpib_wr(mod_id,0,'VIEW CHANNEL2') ;                          */

  /*------------------------------------------------------------------------*/



  *setup_string = '\0';

  strcat(setup_string, "CHANNEL");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", osc_channel);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, " COUPLING DC");

  if (load < 51)

    strcat(setup_string, "FIFTY");

  vth_mhpib_wr(mod_id, 0, setup_string);



  *setup_string = '\0';

  strcat(setup_string, "RANGE ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .1E", channel_range);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 0, setup_string);



  *setup_string = '\0';

  strcat(setup_string, "OFFSET ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .1E", channel_offset);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 0, setup_string);

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_TRIGGER .....................................................*/



void setup_osc_trigger(int mod_id, int osc_channel, int trigger_slope,

                       double trigger_level, int averaging)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, "TRIGGER MODE EDGE;SOURCE CHANNEL");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", osc_channel);

  next_position = mtd_strlen(setup_string) + 1;



  if (trigger_slope == NEG)

    strcat(setup_string, ";SLOPE NEGATIVE;LEVEL ");

  else

    strcat(setup_string, ";SLOPE POSITIVE;LEVEL ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .1E", trigger_level);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 0, setup_string);



  if (averaging)

  {

    vth_mhpib_wr(mod_id, 0, "ACQUIRE TYPE AVERAGE");

    *setup_string = '\0';

    strcat(setup_string, "VIEW MEMORY");

    current_position = mtd_strlen(setup_string) + 1;

    sprintf(setup_string + current_position - 1, "%d", osc_channel + 4);

    next_position = mtd_strlen(setup_string) + 1;

    vth_mhpib_wr(mod_id, 0, setup_string);

    *setup_string = '\0';

    strcat(setup_string, "MEASURE SOURCE MEMORY");

    current_position = mtd_strlen(setup_string) + 1;

    sprintf(setup_string + current_position - 1, "%d", osc_channel + 4);

    next_position = mtd_strlen(setup_string) + 1;

  }

  else

  {

    *setup_string = '\0';

    strcat(setup_string, "VIEW CHANNEL");

    current_position = mtd_strlen(setup_string) + 1;

    sprintf(setup_string + current_position - 1, "%d", osc_channel);

    next_position = mtd_strlen(setup_string) + 1;

    vth_mhpib_wr(mod_id, 0, setup_string);

    *setup_string = '\0';

    strcat(setup_string, "MEASURE SOURCE CHANNEL");

    current_position = mtd_strlen(setup_string) + 1;

    sprintf(setup_string + current_position - 1, "%d", osc_channel);

    next_position = mtd_strlen(setup_string) + 1;

  }

  vth_mhpib_wr(mod_id, 0, setup_string);

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_TIMEBASE ....................................................*/



void setup_osc_timebase(int mod_id, int averaging, int count,

                        double timebase_range, double timebase_offset,

                        int timebase_mode)

{

  char setup_string[81];

  int current_position;

  int next_position;



  if (averaging)

  {

    *setup_string = '\0';

    strcat(setup_string, "ACQUIRE TYPE AVERAGE COUNT ");

    current_position = mtd_strlen(setup_string) + 1;

    sprintf(setup_string + current_position - 1, "%d", count);

    next_position = mtd_strlen(setup_string) + 1;

    strcat(setup_string, " COMPLETE 90");

    vth_mhpib_wr(mod_id, 0, setup_string);

  }

  else

    vth_mhpib_wr(mod_id, 0, "ACQUIRE TYPE NORMAL");



  if (fabs(timebase_offset) >= 1.0)

  {

    if (timebase_offset >= 1.0)

    {

      vth_mhpib_wr(mod_id, 0, "TIMEBASE REFERENCE RIGHT");

      timebase_offset--;

    }

    else

    {

      vth_mhpib_wr(mod_id, 0, "TIMEBASE REFERENCE LEFT");

      timebase_offset++;

    }

  }

  else

    vth_mhpib_wr(mod_id, 0, "TIMEBASE REFERENCE CENTER");



  *setup_string = '\0';

  strcat(setup_string, "RANGE ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .1E", timebase_range);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 0, setup_string);



  *setup_string = '\0';

  strcat(setup_string, "OFFSET ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .1E", timebase_offset);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 0, setup_string);



  /* The mode is setup last because of how things are set on the scope  */

  /* when the mode is single.  Setups will occur only after a trigger   */

  /* has been received if the mode is set to single.                    */



  if (timebase_mode == SINGLE)

  {

    vth_mtmr_wait(mod_id, 100e-03);

    vth_mhpib_wr(mod_id, 0, "MODE SINGLE");

  }

  else

    vth_mhpib_wr(mod_id, 0, "MODE AUTOMATIC");

}



/*------------------------------------------------------------------------*/

/*+RESET_OSC_TIMEBASE_RANGE ..............................................*/

void reset_osc_timebase_range(int mod_id, double timebase_range)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, "TIMEBASE RANGE ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .1E", timebase_range);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 0, setup_string);

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_LOAD ........................................................*/

void setup_osc_load(int mod_id, int osc_channel, int load)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, "CHANNEL ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", osc_channel);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, " COUPLING DC");

  if (load < 51)

    strcat(setup_string, "FIFTY");

  vth_mhpib_wr(mod_id, 0, setup_string);

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_PERSISTENCE .................................................*/

void setup_osc_persistence(int mod_id)

{

  vth_mhpib_wr(mod_id, 0, "ACQUIRE TYPE VPERSISTENCE");

  vth_mhpib_wr(mod_id, 0, "DISPLAY PERSISTENCE INFINITE");

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_VMARKERS ....................................................*/

void setup_osc_vmarkers_jitter(int mod_id)

{

  vth_mhpib_wr(mod_id, 0, "MEASURE SOURCE CHANNEL 1");

  vth_mhpib_wr(mod_id, 0, "VFIFTY");

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_TMARKERS ....................................................*/

void setup_osc_tmarkers_jitter(int mod_id)

{

  vth_mhpib_wr(mod_id, 0, "DISPLAY TMARKER ON");

  vth_mhpib_wr(mod_id, 0, "DISPLAY VMARKER ON");

  vth_mhpib_wr(mod_id, 0, "MEASURE TSTART 0.0");

  vth_mhpib_wr(mod_id, 0, "ESTOP 1");

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_TI_MARKERS ..................................................*/

void setup_osc_ti_markers(int mod_id, int slope_start, int slope_stop)

{

  vth_mhpib_wr(mod_id, 0, "DISPLAY TMARKER ON");

  vth_mhpib_wr(mod_id, 0, "DISPLAY VMARKER ON");

  vth_mhpib_wr(mod_id, 0, "MEASURE SOURCE MEMORY 6, MEMORY 5");

  vth_mhpib_wr(mod_id, 0, "VSTART 1.5");

  vth_mhpib_wr(mod_id, 0, "VSTOP 1.5");



  if (slope_start == NEG)

    vth_mhpib_wr(mod_id, 0, "ESTART -1");

  else

    vth_mhpib_wr(mod_id, 0, "ESTART 1");



  if (slope_stop == NEG)

    vth_mhpib_wr(mod_id, 0, "ESTOP -1");

  else

    vth_mhpib_wr(mod_id, 0, "ESTOP 1");

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_ARB_MARKERS .................................................*/

void setup_osc_arb_markers(int mod_id, int slope_start, double start_volts,

                           int start_channel, int slope_stop,

                           double stop_volts, int stop_channel)

{

  char setup_string[81];

  int current_position;

  int next_position;



  vth_mhpib_wr(mod_id, 0, "DISPLAY TMARKER ON");

  vth_mhpib_wr(mod_id, 0, "DISPLAY VMARKER ON");

  *setup_string = '\0';

  strcat(setup_string, "MEASURE SOURCE MEMORY ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", start_channel + 4);

  next_position = mtd_strlen(setup_string) + 1;

  if (start_channel != stop_channel)

  {

    strcat(setup_string, ", MEMORY ");

    current_position = mtd_strlen(setup_string) + 1;

    sprintf(setup_string + current_position - 1, "%d", stop_channel + 4);

    next_position = mtd_strlen(setup_string) + 1;

  }

  vth_mhpib_wr(mod_id, 0, setup_string);



  *setup_string = '\0';

  strcat(setup_string, "VSTART ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.2f", start_volts);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 0, setup_string);

  *setup_string = '\0';

  strcat(setup_string, "VSTOP ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.2f", stop_volts);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 0, setup_string);



  if (slope_start == NEG)

    vth_mhpib_wr(mod_id, 0, "ESTART -1");

  else

    vth_mhpib_wr(mod_id, 0, "ESTART 1");



  if (slope_stop == NEG)

    vth_mhpib_wr(mod_id, 0, "ESTOP -1");

  else

    vth_mhpib_wr(mod_id, 0, "ESTOP 1");

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_PULSE_WIDTH_MARKERS .........................................*/

void setup_osc_pulse_width_markers(int mod_id, int trigger_slope)

{

  vth_mhpib_wr(mod_id, 0, "DISPLAY TMARKER ON");

  vth_mhpib_wr(mod_id, 0, "DISPLAY VMARKER ON");

  vth_mhpib_wr(mod_id, 0, "MEASURE VSTART 1.5");

  vth_mhpib_wr(mod_id, 0, "VSTOP 1.5");



  if (trigger_slope == NEG)

    vth_mhpib_wr(mod_id, 0, "ESTART -1 ESTOP 1");

  else

    vth_mhpib_wr(mod_id, 0, "ESTART 1 ESTOP -1");

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_VMARKERS_SLEW_RATE ..........................................*/

void setup_osc_vmarkers_slew_rate(int mod_id, int trigger_slope)

{

  vth_mhpib_wr(mod_id, 0, "DISPLAY TMARKER ON");

  vth_mhpib_wr(mod_id, 0, "DISPLAY VMARKER ON");

  vth_mhpib_wr(mod_id, 0, "MEASURE TOPBASE?");

  vth_mhpib_wr(mod_id, 0, "PTOP 80 PBASE 20");



  if (trigger_slope == NEG)

    vth_mhpib_wr(mod_id, 0, "ESTART -1 ESTOP -1");

  else

    vth_mhpib_wr(mod_id, 0, "ESTART 1 ESTOP 1");

}



/*------------------------------------------------------------------------*/

/*+TRIGGER_OSC ...........................................................*/

void trigger_osc(int mod_id, int osc_channel, int averaging)

{

  char setup_string[81];

  int current_position;

  int next_position;



  /*-------------------------------------------------------------------*/

  /* Modified: 11/29/90 JRM -------------------------------------------*/

  /* Moved VIEW CHANNEL section up from inside the 'if averaging' loop */

  /* so that the view channel would be executed either way.  This is   */

  /* in conjunction with removing the view from setup_osc_channel.     */

  /*-------------------------------------------------------------------*/

  *setup_string = '\0';

  strcat(setup_string, "VIEW CHANNEL");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", osc_channel);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 0, setup_string);



  if (averaging)

  {

    /* Above VIEW CHANNEL section was here */

    *setup_string = '\0';

    strcat(setup_string, "DIGITIZE ");

    current_position = mtd_strlen(setup_string) + 1;

    sprintf(setup_string + current_position - 1, "%d", osc_channel);

    next_position = mtd_strlen(setup_string) + 1;

    vth_mhpib_wr(mod_id, 0, setup_string);



    *setup_string = '\0';

    strcat(setup_string, "VIEW MEMORY");

    current_position = mtd_strlen(setup_string) + 1;

    sprintf(setup_string + current_position - 1, "%d", osc_channel + 4);

    next_position = mtd_strlen(setup_string) + 1;

    vth_mhpib_wr(mod_id, 0, setup_string);



    *setup_string = '\0';

    strcat(setup_string, "BLANK CHANNEL");

    current_position = mtd_strlen(setup_string) + 1;

    sprintf(setup_string + current_position - 1, "%d", osc_channel);

    next_position = mtd_strlen(setup_string) + 1;

    vth_mhpib_wr(mod_id, 0, setup_string);

  }

  else

  {

    vth_mhpib_wr(mod_id, 0, "ERASE DISPLAY");

    vth_mhpib_wr(mod_id, 0, "TRG");

    vth_mtmr_wait(mod_id, 0.5);

  }

}



/*------------------------------------------------------------------------*/

/*+TRIGGER_OSC_DIG ....................................... not implemented*/

void trigger_osc_dig(int /* mod_id */, int /* averaging */)

{

}



/*------------------------------------------------------------------------*/

/*+MEASURE_OSC ...........................................................*/

void measure_osc(int mod_id, int osc_function, int trigger_slope)

{

  char setup_string[81];



  *setup_string = '\0';

  if (osc_function == RISE_TIME)

    strcat(setup_string, "MEASURE RISE?");

  if (osc_function == FALL_TIME)

    strcat(setup_string, "MEASURE FALL?");

  if (osc_function == VMAX && trigger_slope == POS)

    strcat(setup_string, "MEASURE VMAX?");

  if (osc_function == VMAX && trigger_slope == NEG)

    strcat(setup_string, "MEASURE VMIN?");

  if (osc_function == DELTA_V)

    strcat(setup_string, "MEASURE VDEL?");

  if (osc_function == DELTA_T)

    strcat(setup_string, "MEASURE TDEL?");

  if (osc_function == WIDTH && trigger_slope == NEG)

    strcat(setup_string, "MEASURE NWID?");

  if (osc_function == WIDTH && trigger_slope == POS)

    strcat(setup_string, "MEASURE PWID?");

  if (osc_function == DUTY_CYCLE)

    strcat(setup_string, "MEASURE DUTY?");

  if (osc_function == FREQ)

    strcat(setup_string, "MEASURE FREQ?");

  if (osc_function == VAVERAGE)

    strcat(setup_string, "MEASURE VAV?");



  vth_mhpib_wr(mod_id, 0, setup_string);

  vth_mhpib_num_rd(mod_id, 0);

}



/*------------------------------------------------------------------------*/

/*+MEASURE_OSC_VOLTS .....................................................*/

void measure_osc_tvolts(int mod_id, double tvolts, int slope)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, "MEASURE TVOLT ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .1E", tvolts);

  next_position = mtd_strlen(setup_string) + 1;

  if (slope == NEG)

    strcat(setup_string, ",-1?");

  else

    strcat(setup_string, ",+1?");



  vth_mhpib_wr(mod_id, 0, setup_string);

  vth_mhpib_num_rd(mod_id, 0);

}



/*------------------------------------------------------------------------*/

/*+MEASURE_OSC_VTIME .....................................................*/

void measure_osc_vtime(int mod_id, double vtime)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, "MEASURE VTIME ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .1E", vtime);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, "?");



  vth_mhpib_wr(mod_id, 0, setup_string);

  vth_mhpib_num_rd(mod_id, 0);

}



/*------------------------------------------------------------------------*/

void mask_osc_error_display(int mod_id, int status)

{

  if (status)

    vth_mhpib_wr(mod_id, 0, "DISPLAY MASK 254");

  else

    vth_mhpib_wr(mod_id, 0, "DISPLAY MASK 255");



}  /*mask_osc_error_display*/



/*------------------------------------------------------------------------*/

/*+CLOSE_OSC .............................................................*/

void close_osc(int mod_id)

{

  vth_mhpib_close(mod_id, 0);

}



/*------------------------------------------------------------------------*/

/*+SETUP_OSC_SRQ_MASK ....................................................*/

void setup_osc_srq_mask(int mod_id, int mask)

{

  int next_position;

  int current_position;

  char s[81];



  *s = '\0';

  strcat(s, "REQ ");

  current_position = mtd_strlen(s) + 1;

  sprintf(s + current_position - 1, "%3d", mask);

  next_position = mtd_strlen(s) + 1;

  vth_mhpib_wr(mod_id, 0, s);

  vth_mhpib_wr(mod_id, 0, "CLEAR");

}



/*------------------------------------------------------------------------*/

/*end of xosc.p*/

/* wpd - include begins here */

/*+XDELAY_OSC.P .......... use tag 8 for the 54111D for delay measurements*/

/*+20-NOV-89 (slc) replaced all "vth_mhpib_wr" calls with "vth_mhpib_wr"*/

/*------------------------------------------------------------------------*/

/*+SETUP_DELAY_OSC_DEFAULTS ..............................................*/

void setup_delay_osc_defaults(int mod_id)

{

  test_result_type result;



  result.Measurement_class = CLASS_REAL;



/*  vth_mhpib_wr(mod_id, 8, "RESET");

  vth_mhpib_wr(mod_id, 8, "DISPLAY FORMAT SINGLE");

  vth_mhpib_wr(mod_id, 8, "VMARKER ON");

  vth_mhpib_wr(mod_id, 8, "TMARKER ON");

  vth_mhpib_wr(mod_id, 8, "TIM MODE TRIG");

  vth_mhpib_wr(mod_id, 8, "TIM SENS 5E-10");

  vth_mhpib_wr(mod_id, 8, "CH 1 SENS 1.5 OFFS 1.5");

  vth_mhpib_wr(mod_id, 8, "CH 2 SENS 1.5 OFFS 1.5");

  vth_mhpib_wr(mod_id, 8, "MEAS SOUR CHAN1, CHAN2");

  vth_mhpib_wr(mod_id, 8, "VSTART 1.5");

  vth_mhpib_wr(mod_id, 8, "VSTOP 1.5");

  vth_mhpib_wr(mod_id, 8, "ESTART 1");

  vth_mhpib_wr(mod_id, 8, "ESTOP 1");

  vth_mhpib_wr(mod_id, 8, "SAVE 0");

  vth_mhpib_wr(mod_id, 8, "ESTART -1");

  vth_mhpib_wr(mod_id, 8, "ESTOP -1");

  vth_mhpib_wr(mod_id, 8, "TRIG SLOP NEG");

  vth_mhpib_wr(mod_id, 8, "SAVE 1");

  */



  vth_mhpib_wr(mod_id, 8, "RESET");

  vth_mhpib_wr(mod_id, 8, "DISPLAY FORMAT SINGLE");

  vth_mhpib_wr(mod_id, 8, "VMARKER ON");

  vth_mhpib_wr(mod_id, 8, "TMARKER ON");

  vth_mhpib_wr(mod_id, 8, "TIM MODE TRIG");

  vth_mhpib_wr(mod_id, 8, "CH 1 SENS 1 OFFS 1.5");

  vth_mhpib_wr(mod_id, 8, "CH 2 SENS 1 OFFS 1.5");

  vth_mhpib_wr(mod_id, 8, "BLANK MEMORY 5");

  vth_mhpib_wr(mod_id, 8, "BLANK MEMORY 6");

  vth_mhpib_wr(mod_id, 8, "CH 1 PROBE?");

  vth_mhpib_num_rd(mod_id, 8);



  if (get_result_nolog(mod_id, &result))

    chan1_probe_atten_delay_osc = result.UU.Real_result;

  else

    display_prompt("No result in result log (Expected CHAN 1 atten)", true);



  vth_mhpib_wr(mod_id, 8, "CH 2 PROBE?");

  vth_mhpib_num_rd(mod_id, 8);



  if (get_result_nolog(mod_id, &result))

    chan2_probe_atten_delay_osc = result.UU.Real_result;

  else

    display_prompt("No result in result log (Expected CHAN 2 atten)", true);



  vth_mhpib_wr(mod_id, 8, "ACQ COUNT 8 COMP 90");

  vth_mhpib_wr(mod_id, 8, "TIM SENS 5E-9");

}



/*------------------------------------------------------------------------*/

/*+OPEN_DELAY_OSC ........................................................*/

void open_delay_osc(int mod_id)

{

  vth_mhpib_open(mod_id, 8, "/dev/delay.osc");

  setup_delay_osc_defaults(mod_id);

}



#define TRIG_LEVEL      1.5   /* trigger voltage threshold */

#define TRIG_LEVEL1      1.0   /* trigger voltage threshold */

#define ACQ_MASK        1024   /* acquistion ready enable */

#define TRIG_BIT_POS    6   /* 0 = MSB */

#define WAIT_TIME       1.0

#define BLANK_LINE      ' '

#define ERR_BIT_POS     2   /* bit position of error */

#define WAIT_TIME_      0.3



/* static variables for measure_td: */

struct LOC_measure_td

{

  int mod_id;

  int error;

  s_type setup_string;

  int current_position;

  int next_position;

  test_result_type result;

} ;



static void verify_valid_signals_present(struct LOC_measure_td * LINK)

{

  a_byte_type ready_byte;

  char buffer[81];

  char str1[256];



  *LINK->setup_string = '\0';

  strcat(LINK->setup_string, "TRIG MODE EDGE; SOURCE CHANNEL1; LEV ");

  LINK->current_position = mtd_strlen(LINK->setup_string) + 1;

  sprintf(LINK->setup_string + LINK->current_position - 1,

          "% .1E", TRIG_LEVEL1);

  LINK->next_position = mtd_strlen(LINK->setup_string) + 1;

  vth_mhpib_wr(LINK->mod_id, 8, LINK->setup_string);

  vth_mtmr_wait(LINK->mod_id, WAIT_TIME);

  vth_mhpib_wr(LINK->mod_id, 8, "RDY?");

  vth_mhpib_num_rd(LINK->mod_id, 8);



  if (get_result_nolog(LINK->mod_id, &LINK->result))

  {

    ready_byte.data = round(LINK->result.UU.Real_result);



//changed this to reorder the bits in the byte -- mjs 6/97

//also change the ready_byte.bool to ready_byte.data to account for Union not working

    if ((ready_byte.data & (1 << (7-TRIG_BIT_POS))) == 0)

    {

      update_status(WAITING);

      display_prompt("No signal is present on Channel 1", true);

      display_prompt("Check your connections", true);

      display_prompt("Press <Return> or <Enter> when vth_ready to continue", true);

      sprintf(str1, "%c", BLANK_LINE);

      display_prompt(str1, false);

      get_user_answer(buffer, 80);

      update_status(MEASURING);

    }

  }

  else

    display_prompt("No result in result log (Expected READY byte)", true);



  *LINK->setup_string = '\0';

  strcat(LINK->setup_string, "TRIG MODE EDGE; SOURCE CHANNEL2; LEV ");

  LINK->current_position = mtd_strlen(LINK->setup_string) + 1;

  sprintf(LINK->setup_string + LINK->current_position - 1,

          "% .1E", TRIG_LEVEL);

  LINK->next_position = mtd_strlen(LINK->setup_string) + 1;

  vth_mhpib_wr(LINK->mod_id, 8, LINK->setup_string);

  vth_mtmr_wait(LINK->mod_id, WAIT_TIME);

  vth_mhpib_wr(LINK->mod_id, 8, "RDY?");

  vth_mhpib_num_rd(LINK->mod_id, 8);



  if (get_result_nolog(LINK->mod_id, &LINK->result))

  {

    ready_byte.data = round(LINK->result.UU.Real_result);

//changed this to reorder the bits in the byte -- mjs 6/97

//also change the ready_byte.bool to ready_byte.data to account for Union not working

    if ((ready_byte.data & (1 << (7-TRIG_BIT_POS))) == 0)

    {

      update_status(WAITING);

      display_prompt("No signal is present on Channel 2", true);

      display_prompt("Check your connections", true);

      display_prompt("Press <Return> or <Enter> when vth_ready to continue", true);

      sprintf(str1, "%c", BLANK_LINE);

      display_prompt(str1, false);

      get_user_answer(buffer, 80);

      update_status(MEASURING);

    }

  }

  else

    display_prompt("No result in result log (Expected READY byte)", true);

}

#undef TRIG_BIT_POS

#undef WAIT_TIME

#undef BLANK_LINE



static boolean osc_hpib_write(hfs_fd_type* id, char* s, struct LOC_measure_td* LINK)

{

  boolean result;

  s_type disp_string;

  int current_position;

  int next_position;

  int error_num;

  test_result_type test_result;

  int stat;

  a_byte_type stat_byte;



  vth_mhpib_wr(LINK->mod_id, 8, s);

  flush_commands(LINK->mod_id);

  vth_mtmr_wait(LINK->mod_id, WAIT_TIME_);

  flush_commands(LINK->mod_id);

  c_hfs_hpib_spoll(id, &stat, &LINK->error);

  flush_commands(LINK->mod_id);



  if (LINK->error == HFS_HPIB_SPOLL_FAILED)

  {

    display_prompt("WARNING: unable to SPOLL 54111 oscilloscope", true);

    return false;

  }



  stat_byte.data = stat;

//changed stat_byte.bool to stat_byte.data to account for Union not working

//also changed ordering of byte by using  7 -                 mjs 6/97

  if ((stat_byte.data & (1 << (7-ERR_BIT_POS))) == 0)

    return true;

  result = false;

  vth_mhpib_wr(LINK->mod_id, 8, "ERR?");

  vth_mhpib_num_rd(LINK->mod_id, 8);

  if (get_result_nolog(LINK->mod_id, &test_result))

  {

    error_num = round(test_result.UU.Real_result);

    *disp_string = '\0';

    strcat(disp_string, "54111 ERROR ");

    current_position = mtd_strlen(disp_string) + 1;

    sprintf(disp_string + current_position - 1, "%5d", error_num);

    next_position = mtd_strlen(disp_string) + 1;

    display_prompt(disp_string, true);

  }

  else

    display_prompt("No result in result log (Expected ERR num)", true);

  return result;



  /*valid stat*/

}

#undef ERR_BIT_POS

#undef WAIT_TIME_



/*------------------------------------------------------------------------*/

/*+MEASURE_TD ............................................................*/

double measure_td(int mod_id, double t_nominal, int trig_source,

                  int trig_edge, int other_direction,

                  int other_edge, double timebase_sensitivity)

{

  struct LOC_measure_td V;

  double result;

  hfs_fd_type* id;

  osc_chan_type other_channel;

  int direction;

  int edge;

  double time_offset;

  double trigger_level;

  int request_mask;

  boolean error_detected;

  path_type path;



  /*----------------------------------------------------- body of measure_td*/

  V.mod_id = mod_id;

  error_detected = false;

  request_mask = ACQ_MASK;   /* enable acq RDYs only */

  *V.setup_string = '\0';

  strcat(V.setup_string, "REQ ");

  V.current_position = mtd_strlen(V.setup_string) + 1;

  sprintf(V.setup_string + V.current_position - 1, "%5d", request_mask);

  V.next_position = mtd_strlen(V.setup_string) + 1;

  vth_mhpib_wr(V.mod_id, 8, V.setup_string);

  vth_mhpib_wr(V.mod_id, 8, "CLEAR");   /* clear the status byte */

  flush_commands(V.mod_id);

  strcpy(path, "/dev/delay.osc");

  c_hfs_open(&id, path, HACCESS_READ_WRITE, &V.error);

  if (V.error == HFS_OPEN_ERROR)

  {

    display_prompt("WARNING: unable to open GPIB file", true);

    error_detected = true;

  }

  else

  {

    *V.setup_string = '\0';

    strcat(V.setup_string, "TIM SENS ");

    V.current_position = mtd_strlen(V.setup_string) + 1;

    sprintf(V.setup_string + V.current_position - 1,

            "% .2E", timebase_sensitivity);

    V.next_position = mtd_strlen(V.setup_string) + 1;

    if (!osc_hpib_write(id, V.setup_string, &V))

      error_detected = true;

    verify_valid_signals_present(&V);



    if (trig_source == 1)

    {

      *V.setup_string = '\0';

      strcat(V.setup_string, "TRIG MODE EDGE; SOURCE CHANNEL1; LEV ");

      V.current_position = mtd_strlen(V.setup_string) + 1;

      sprintf(V.setup_string + V.current_position - 1, "% .1E", TRIG_LEVEL);

      V.next_position = mtd_strlen(V.setup_string) + 1;

      vth_mhpib_wr(V.mod_id, 8, V.setup_string);

      other_channel = 2;

    }

    else

    {

      *V.setup_string = '\0';

      strcat(V.setup_string, "TRIG MODE EDGE; SOURCE CHANNEL2; LEV ");

      V.current_position = mtd_strlen(V.setup_string) + 1;

      sprintf(V.setup_string + V.current_position - 1, "% .1E", TRIG_LEVEL);

      V.next_position = mtd_strlen(V.setup_string) + 1;

      vth_mhpib_wr(V.mod_id, 8, V.setup_string);

      other_channel = 1;

    }



    if (trig_edge == RISING)

      vth_mhpib_wr(V.mod_id, 8, "TRIG MODE EDGE; SLOP POS");

    else

      vth_mhpib_wr(V.mod_id, 8, "TRIG MODE EDGE; SLOP NEG");



    vth_mhpib_wr(V.mod_id, 8, "TIM OFFS 0");



    *V.setup_string = '\0';

    strcat(V.setup_string, "DIG ");

    V.current_position = mtd_strlen(V.setup_string) + 1;

    sprintf(V.setup_string + V.current_position - 1, "%d", trig_source);

    V.next_position = mtd_strlen(V.setup_string) + 1;

    vth_mhpib_wr(V.mod_id, 8, V.setup_string);



    if (!poll_for_dig_complete(V.mod_id, id))

      error_detected = true;

    if (other_direction == POST)

      direction = 1;

    else

      direction = -1;



    time_offset = direction * t_nominal;

    *V.setup_string = '\0';

    strcat(V.setup_string, "TIM OFFS ");

    V.current_position = mtd_strlen(V.setup_string) + 1;

    sprintf(V.setup_string + V.current_position - 1, "% .3E", time_offset);

    V.next_position = mtd_strlen(V.setup_string) + 1;

    if (!osc_hpib_write(id, V.setup_string, &V))

      error_detected = true;



    *V.setup_string = '\0';

    strcat(V.setup_string, "DIG ");

    V.current_position = mtd_strlen(V.setup_string) + 1;

    sprintf(V.setup_string + V.current_position - 1, "%d", other_channel);

    V.next_position = mtd_strlen(V.setup_string) + 1;

    vth_mhpib_wr(V.mod_id, 8, V.setup_string);



    if (!poll_for_dig_complete(V.mod_id, id))

      error_detected = true;



    if (other_direction == POST && trig_source == 1 ||

        other_direction == PRE && trig_source == 2)

    {

      vth_mhpib_wr(V.mod_id, 8, "MEAS SOUR MEMORY5, MEMORY6");



      trigger_level = TRIG_LEVEL * chan1_probe_atten_delay_osc;

      *V.setup_string = '\0';

      strcat(V.setup_string, "VSTART ");

      V.current_position = mtd_strlen(V.setup_string) + 1;

      sprintf(V.setup_string + V.current_position - 1, "% .3E", trigger_level);

      V.next_position = mtd_strlen(V.setup_string) + 1;

      vth_mhpib_wr(V.mod_id, 8, V.setup_string);



      trigger_level = TRIG_LEVEL * chan2_probe_atten_delay_osc;

      *V.setup_string = '\0';

      strcat(V.setup_string, "VSTOP ");

      V.current_position = mtd_strlen(V.setup_string) + 1;

      sprintf(V.setup_string + V.current_position - 1, "% .3E", trigger_level);

      V.next_position = mtd_strlen(V.setup_string) + 1;

      vth_mhpib_wr(V.mod_id, 8, V.setup_string);

    }

    else

    {

      vth_mhpib_wr(V.mod_id, 8, "MEAS SOUR MEMORY6, MEMORY5");



      trigger_level = TRIG_LEVEL * chan2_probe_atten_delay_osc;

      *V.setup_string = '\0';

      strcat(V.setup_string, "VSTART ");

      V.current_position = mtd_strlen(V.setup_string) + 1;

      sprintf(V.setup_string + V.current_position - 1, "% .3E", trigger_level);

      V.next_position = mtd_strlen(V.setup_string) + 1;

      vth_mhpib_wr(V.mod_id, 8, V.setup_string);



      trigger_level = TRIG_LEVEL * chan1_probe_atten_delay_osc;

      *V.setup_string = '\0';

      strcat(V.setup_string, "VSTOP ");

      V.current_position = mtd_strlen(V.setup_string) + 1;

      sprintf(V.setup_string + V.current_position - 1, "% .3E", trigger_level);

      V.next_position = mtd_strlen(V.setup_string) + 1;

      vth_mhpib_wr(V.mod_id, 8, V.setup_string);

    }



    if (other_direction == POST)

    {

      if (trig_edge == RISING)

        edge = 1;

      else

        edge = -1;

      *V.setup_string = '\0';

      strcat(V.setup_string, "MEAS ESTART ");

      V.current_position = mtd_strlen(V.setup_string) + 1;

      sprintf(V.setup_string + V.current_position - 1, "%2d", edge);

      V.next_position = mtd_strlen(V.setup_string) + 1;

      strcat(V.setup_string, " ESTOP ");

      V.current_position = mtd_strlen(V.setup_string) + 1;

      if (other_edge == RISING)

        edge = 1;

      else

        edge = -1;

      sprintf(V.setup_string + V.current_position - 1, "%2d", edge);

      V.next_position = mtd_strlen(V.setup_string) + 1;

      if (!osc_hpib_write(id, V.setup_string, &V))

        error_detected = true;

    }

    else

    {

      if (other_edge == RISING)

        edge = 1;

      else

        edge = -1;

      *V.setup_string = '\0';

      strcat(V.setup_string, "MEAS ESTART ");

      V.current_position = mtd_strlen(V.setup_string) + 1;

      sprintf(V.setup_string + V.current_position - 1, "%2d", edge);

      V.next_position = mtd_strlen(V.setup_string) + 1;

      strcat(V.setup_string, " ESTOP ");

      V.current_position = mtd_strlen(V.setup_string) + 1;

      if (trig_edge == RISING)

        edge = 1;

      else

        edge = -1;

      sprintf(V.setup_string + V.current_position - 1, "%2d", edge);

      V.next_position = mtd_strlen(V.setup_string) + 1;

      if (!osc_hpib_write(id, V.setup_string, &V))

        error_detected = true;

    }



    V.result.Measurement_class = CLASS_REAL;

    vth_mhpib_wr(V.mod_id, 8, "TDELTA?");

    vth_mhpib_num_rd(V.mod_id, 8);



    if (get_result_nolog(V.mod_id, &V.result))

      result = V.result.UU.Real_result;

    else

    {

      display_prompt("No result in result log (Expected TDELTA)", true);

      error_detected = true;

    }

  }



  if (error_detected)

    result = 9999.0;

  flush_commands(V.mod_id);

  c_hfs_close(id, &V.error);

  if (V.error == HFS_CLOSE_ERROR)

    display_prompt("WARNING: unable to close osc file", true);

  return result;

}

#undef TRIG_LEVEL

#undef TRIG_LEVEL1

#undef ACQ_MASK



/*------------------------------------------------------------------------*/

/*+CLOSE_DELAY_OSC .......................................................*/

void close_delay_osc(int mod_id)

{

  vth_mhpib_close(mod_id, 8);

}



/*------------------------------------------------------------------------*/

/*END OF DELAY_OSC.P*/

/* wpd - include begins here */

/* Use tag 1 for the 3235 Switch Unit */

/* ******************************************************************** */

void connect(int mod_id, int connection, int status)

{

  /* USED BY BOTH DIGITAL AND ANALOG FIXTURE */

  s_type s;

  int next;

  int start;



  *s = '\0';

  if (status)

    strcat(s, "CLOSE ");

  else

    strcat(s, "OPEN ");

  start = mtd_strlen(s) + 1;

  sprintf(s + start - 1, "%d", connection);

  next = mtd_strlen(s) + 1;

  vth_mhpib_wr(mod_id, 1, s);

}  /*connect*/



/* ******************************************************************** */

void connect_cntr_start_af2(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  //connect(mod_id, 502, true);  //fikri added old switchbox 28/06/202
  connect_34980A_relay(6402); //fikri added new switchbox 28/06/2024

  //connect(mod_id, 913, true);  //fikri added old switchbox 28/06/202
  connect_34980A_relay(7203); //fikri added new switchbox 28/06/2024

  //connect(mod_id, mod_id + 901, true);  //fikri added old switchbox 28/06/202
  connect_34980A_relay(mod_id + 8301); //fikri added new switchbox 28/06/2024

}  /*connect_cntr_start_AF2*/



/* ******************************************************************** */

static void connect_cntr_start_probe(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  connect(mod_id, 504, true);

  connect(mod_id, 712, true);

}  /*connect_cntr_start_PROBE*/



/* ******************************************************************** */

void connect_cntr_stop_db1(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  connect(mod_id, 923, true);  //fikri added old switchbox 28/06/2024
  connect_34980A_relay(6303);  //fikri added new switchbox 28/06/2024

}  /*connect_cntr_stop_DB1*/



/* ******************************************************************** */

void connect_cntr_stop_uclk(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  // connect(mod_id, 921, true);  //fikri added old switchbox 28/06/2024
  connect_34980A_relay(6301);    //fikri added new switchbox 28/06/2024

  // connect(mod_id, 611, true);  //fikri added old switchbox 28/06/2024
  connect_34980A_relay(7101);    //fikri added new switchbox 28/06/2024

  if (four_mod_fixture(mod_id))

    // connect(mod_id, mod_id + 621, true);  //fikri added old switchbox 28/06/2024
    connect_34980A_relay(mod_id + 8201);    //fikri added new switchbox 28/06/2024

  else

  {

    switch (mod_id)

    {



    case 0:

    case 3:

      // connect(mod_id, 624, true);  //fikri added old switchbox 28/06/2024
      connect_34980A_relay(8204);  //fikri added new switchbox 28/06/2024

      break;



    case 1:

    case 2:

     // connect(mod_id, 623, true);  //fikri added old switchbox 28/06/2024
      connect_34980A_relay(8203);  //fikri added new switchbox 28/06/2024

      break;

    default:

      assert(false);

      break;



    }

  }

}  /*connect_cntr_stop_UCLK*/



/* ******************************************************************** */

void connect_dg(int mod_id, int status)

{

  /* USED BY ANALOG FIXTURE ONLY */

  connect(mod_id, 206, status);

}  /*connect_DG*/



/* ******************************************************************** */

void connect_dmm(int mod_id)

{

  /* USED BY ANALOG FIXTURE ONLY */



  /* connecting the dmm is accomplished by opening the relays since the   */

  /* dmm contact is made at the normally closed connections               */
	if (get_34980A_IDN() == false)//chun12112021
	{
		connect(mod_id, 201, false);

		connect(mod_id, 202, false);

		connect(mod_id, 204, false);

		connect(mod_id, 205, false);
	}
	else if (get_34980A_IDN() == true)
	{
		/*connect(mod_id, 201, false);

		connect(mod_id, 202, false);

		connect(mod_id, 204, false);

		connect(mod_id, 205, false);*/

		connect_34980A_relay(1001);//chun23112021

		connect_34980A_relay(1002);//chun23112021

		connect_34980A_relay(1004);//chun23112021

		connect_34980A_relay(1005);//chun23112021
	}

}  /*connect_dmm*/



/* ******************************************************************** */

void connect_dmm_pin(int mod_id, int slot)

{

  /* USED BY DIGITAL FIXTURE ONLY */



  /* This connection is made for the digital test fixture.  The connection  */

  /* is through minerva to the dmm mux.  The dmm is in front terminals for  */

  /* this measurement.                                                      */

  int mod_contribution;

  int slot_contribution;

  int mod_slot_determined_connection;





  if (four_mod_fixture(mod_id))

  {

    switch (mod_id)

    {
    case 0:
      //mod_contribution = 400;  //fikri 23/4/2024
      mod_contribution = 4036;  //fikri 23/4/2024
      break;
    case 1:
      //mod_contribution = 420;  //fikri 23/4/2024
      mod_contribution = 4052;  //fikri 23/4/2024
      break;
    case 2:
      //mod_contribution = 320;  //fikri 23/4/2024
      mod_contribution = 4052;  //fikri 23/4/2024
      break;

    case 3:
      //mod_contribution = 300;  //fikri 23/4/2024
      mod_contribution = 4036;  //fikri 23/4/2024
      break;
    default:

      assert(false);
      break;
    }/* case */

  }
  else
  {
    switch (mod_id)
    {
    case 0:
    case 3:
      //mod_contribution = 300;  //fikri 23/4/2024
      mod_contribution = 4036;
      break;
    case 1:
    case 2:
      //mod_contribution = 320;  //fikri 23/4/2024
      mod_contribution = 4052;
      break;
    default:
      assert(false);
      break;
    }
  }

  switch (slot)
  {

  case 2:

  case 13:

    slot_contribution = 1;

    break;



  case 3:

  case 14:

    slot_contribution = 2;

    break;



  case 4:

  case 15:

    slot_contribution = 3;

    break;



  case 5:

  case 16:

    slot_contribution = 4;

    break;



  case 7:

  case 18:

    slot_contribution = 5;

    break;



  case 8:

  case 19:

    slot_contribution = 6;

    break;



  case 9:

  case 20:

    slot_contribution = 7;

    break;



  case 10:

  case 21:

    slot_contribution = 8;

    break;



  case 11:

  case 22:

    //slot_contribution = 11; //fikri 16/05/2024
    slot_contribution = 9; //added fikri 16/05/2024

    break;

  default:

    assert(false);

    break;



  }/* case */





  mod_slot_determined_connection = mod_contribution + slot_contribution;



 /* connect(mod_id, mod_contribution / 100 * 100 + 70, true); //370

  connect(mod_id, mod_contribution / 100 * 100 + 71, true);  //371

  connect(mod_id, mod_contribution / 100 * 100 + 72, true);  //372

  connect(mod_id, mod_contribution / 100 * 100 + 90, true);  //390

  connect(mod_id, mod_slot_determined_connection, true); //301
  */

		//connect_34980A_relay(8204);//chun23112021

		//connect_34980A_relay(7201);//chun23112021

		//connect_34980A_relay(7302);//chun23112021

		connect_34980A_relay(1001);//chun23112021

		connect_34980A_relay(1002);//chun23112021

		connect_34980A_relay(1004);//chun23112021

		connect_34980A_relay(1005);//chun23112021
		//connect_34980A_relay(4038);//fikri
        connect_34980A_relay(mod_slot_determined_connection);

}  /*connect_dmm_pin*/



/* ******************************************************************** */

void connect_gen_trg_cntr_start(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  //connect(mod_id, 512, true); //fikri 19/4/2024 old rack
  connect_34980A_relay(5102);  //fikri 19/4/2024 new rack

  //connect(mod_id, 503, true //fikri 19/4/2024 old rack);
  connect_34980A_relay(6403);  //fikri 19/4/2024 new rack
}  /*connect_gen_trg_cntr_start*/



/* ******************************************************************** */

void connect_gen_trg_clkrb(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

//  connect(mod_id, 511, true); //fikri 19/4/2024 old rack
  connect_34980A_relay(5101);  //fikri 19/4/2024 new rack

  if (four_mod_fixture(mod_id))

    // connect(mod_id, mod_id + 601, true); //fikri 19/4/2024 old rack
    connect_34980A_relay(mod_id + 8101);  //fikri 19/4/2024 new rack

  else

  {

    switch (mod_id)

    {



    case 0:

    case 3:

      //connect(mod_id, 604, true); fikri 19/4/2024 old rack
      connect_34980A_relay(8104); //fikri 19/4/2024 new rack
      break;


        vth_mtmr_wait(mod_id, 0.1); //added fikri 29/5/2024

    case 1:

    case 2:

      // connect(mod_id, 603, true); //fikri 19/4/2024 old rack
      connect_34980A_relay(8103);   //fikri 19/4/2024 new rack

      break;



    default:

      assert(false);

      break;



    }

  }

}  /*connect_gen_trg_clkrb*/



/* ******************************************************************** */

void connect_gen_trg_osc_ch2(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  connect(mod_id, 513, true);

  connect(mod_id, 723, true);

}  /*connect_gen_trg_osc_ch2*/



/* ******************************************************************** */

void connect_gen_out_cntr_stop(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  //connect(mod_id, 523, true); //fikri 19/4/2024 old rack
  connect_34980A_relay(6203);   //fikri 19/4/2024 new rack

  //connect(mod_id, 924, true); //fikri 19/4/2024 old rack
  connect_34980A_relay(6304);   //fikri 19/4/2024 new rack

}  /*connect_gen_out_cntr_stop*/



/* ******************************************************************** */

void connect_gen_out_osc_ch1(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  connect(mod_id, 522, true);

  connect(mod_id, 703, true);

}  /*connect_gen_out_osc_ch1*/



/* ******************************************************************** */

void connect_gen_out_af2(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  /* connect(mod_id, 521, true);

  connect(mod_id, 912, true);

  connect(mod_id, mod_id + 901, true);
  */ //fikri 19/4/2024 old rack

  /* Fikri 19/4/204 new -rack */
  connect_34980A_relay(6201);

  connect_34980A_relay(7202);

  connect_34980A_relay(mod_id + 8301);


}  /*connect_gen_out_AF2*/



/* ******************************************************************** */

void connect_gen_out_probe(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  connect(mod_id, 524, true);

  connect(mod_id, 713, true);

}  /*connect_gen_out_PROBE*/



/* ******************************************************************** */

void connect_lcr(int mod_id)

{

	/* USED BY ANALOG FIXTURE ONLY */



	/* connecting the lcr is accomplished by closing the relays since the   */

	/* lcr contact is made at the normally open connections                 */
	if (get_34980A_IDN() == false)
	{
		connect(mod_id, 201, true);

		connect(mod_id, 202, true);

		connect(mod_id, 204, true);

		connect(mod_id, 205, true);
	}
	else if (get_34980A_IDN() == true)
	{
		connect_34980A_relay(1008);//chun09032022
		connect_34980A_relay(1009);//chun09032022
		connect_34980A_relay(1012);//chun09032022
		connect_34980A_relay(1013);//chun09032022
	}


}  /*connect_lcr*/



/* ******************************************************************** */

void connect_minus_pin(int mod_id, int slot, int channel, int status)

{  /*connect_dmm_minus_pin*/

  /* USED BY ANALOG FIXTURE ONLY */
  int mod_contribution;
  int slot_contribution;
  int channel_contribution;
  int pin_connection;

if (get_34980A_IDN() == false)
{
  switch (mod_id)
  {
  case 0:
  case 3:
    mod_contribution = 0; //module 3
    break;
  case 1:
  case 2:
    mod_contribution = 20; //module 2
    break;

  default:
    assert(false);
    break;
  }/*case*/

  if (slot < 7 || slot > 12 && slot < 17)
    slot_contribution = 0;
  else
    slot_contribution = 10;
  channel_contribution = channel + 1 - 'A';
  pin_connection = mod_contribution + slot_contribution +
                   channel_contribution + 400;

  connect(mod_id, 333, status);
  connect(mod_id, 470, status);
  connect(mod_id, 471, status);
  connect(mod_id, 472, status);
  connect(mod_id, pin_connection, status);
}

else if (get_34980A_IDN() == true) //fikri04042022
{
  switch (mod_id)
  {
  case 0:
  case 3:
    mod_contribution = 0; //module 3
    break;
  case 1:
  case 2:
    mod_contribution = 16; //module 2
    break;

  default:
    assert(false);
    break;
  }/*case*/

  if (slot < 7 || slot > 12 && slot < 17)
    slot_contribution = 0;
  else
    slot_contribution = 8;//10;
  channel_contribution = channel + 1 - 'A';
  pin_connection = mod_contribution + slot_contribution + channel_contribution + 4035;

 /* connect(mod_id, 333, status);
  connect(mod_id, 470, status);
  connect(mod_id, 471, status);
  connect(mod_id, 472, status);*/
  //connect_34980A(mod_id, pin_connection, status);

  if (status == true)
			  {
				  connect_34980A_relay(pin_connection);
				 // connect_34980A_relay(3005);//disconnect X1 module 3 for -,-S p25,p27  //ori is 3041
			  }
			  else if (status == false)
			  {
				  disconnect_34980A_relay(pin_connection);
				 // disconnect_34980A_relay(3005);//disconnect X1 module 3 for -,-S p25,p27  //ori is 3041
			  }
}
}  /*connect_dmm_minus_pin*/



/* ******************************************************************** */

void connect_minus_calib2(int mod_id)

{

  /* USED BY ANALOG FIXTURE ONLY */
	if (get_34980A_IDN() == false)
	{
		switch (mod_id)

		{
		case 0:
			connect(mod_id, 337, true);
			break;
		case 1:
			connect(mod_id, 338, true);
			break;
		case 2:
			connect(mod_id, 338, true);
			break;
		case 3:
			connect(mod_id, 337, true);
			break;
		default:
			assert(false);
			break;
		}/*case*/
	}

	else if (get_34980A_IDN() == true)//chun 24112021

	{
		switch (mod_id)
		{
		case 0:
			connect_34980A_relay(3043); //connect_34980A_relay(3042);//(3042);//connect(mod_id, 337, true);
			break;
		case 1:
			connect_34980A_relay(3042); //connect_34980A_relay(3043);//(3043);//connect(mod_id, 338, true);
			break;
		case 2:
			connect_34980A_relay(3042); //connect_34980A_relay(3043);//(3043);//connect(mod_id, 338, true);
			break;
		case 3:
			connect_34980A_relay(3043); //connect_34980A_relay(3042);//(3042);//connect(mod_id, 337, true);
			break;
		default:

			assert(false);
			break;
		}/*case*/
	}

}  /* connect_minus_CALIB2 */



/* ******************************************************************** */

void connect_minus_swd_gnd(int mod_id)

{

  /* USED BY ANALOG FIXTURE ONLY */
	if (get_34980A_IDN()==true)
	{
		switch (mod_id)
		{
			case 0:
				connect_34980A_relay(3039);
				break;

			case 1:
				connect_34980A_relay(3040);
				break;

			case 2:
				connect_34980A_relay(3040);
				break;

			case 3:
				connect_34980A_relay(3039);
				break;
		}
	}
	else if (get_34980A_IDN() == false)
	{
		switch (mod_id)

		{



		case 0:

			connect(mod_id, 334, true);

			break;



		case 1:

			connect(mod_id, 335, true);

			break;



		case 2:

			connect(mod_id, 335, true);

			break;



		case 3:

			connect(mod_id, 334, true);

			break;



		default:

			assert(false);

			break;



		}/*case*/
	}



}  /* connect_minus_SWD_GND */



/* ******************************************************************** */

void dconnect_minus_swd_gnd(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  if (four_mod_fixture(mod_id))

  {

    switch (mod_id)

    {



    case 0:

      //connect(mod_id, 414, true);
      connect_34980A_relay(4001);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4911);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4921);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4912);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4922);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4913);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4923);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4914);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4924);      //fikri new switcbox 19/06/2024

      break;



    case 1:

      //connect(mod_id, 434, true);
      connect_34980A_relay(4001);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4911);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4921);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4912);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4922);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4913);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4923);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4914);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4924);      //fikri new switcbox 19/06/2024

      break;



    case 2:

      //connect(mod_id, 334, true);
      connect_34980A_relay(4001);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4911);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4921);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4912);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4922);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4913);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4923);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4914);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4924);      //fikri new switcbox 19/06/2024

      break;



    case 3:

   //   connect(mod_id, 314, true);
      connect_34980A_relay(4001);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4911);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4921);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4912);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4922);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4913);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4923);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4914);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4924);      //fikri new switcbox 19/06/2024

      break;



    default:

      assert(false);

      break;



    }/*case*/

  }

  else

  {

    switch (mod_id)

    {



    case 0:

    case 3:

      // connect(mod_id, 314, true);   //fikri -old switchbox
      connect_34980A_relay(4001);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4911);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4921);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4912);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4922);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4913);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4923);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4914);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4924);      //fikri new switcbox 19/06/2024

      break;



    case 1:

    case 2:

      //connect(mod_id, 334, true);  //fikri -old switchbox
      connect_34980A_relay(4001);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4911);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4921);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4912);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4922);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4913);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4923);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4914);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4924);      //fikri new switcbox 19/06/2024

      break;



    default:

      assert(false);

      break;



    }

  }



}  /* dconnect_minus_SWD_GND */



void connect_minus_xn_plus_2(int mod_id, int channel, int status)//chun 16032022 -,-S pin 71

{

	/* USED BY ANALOG FIXTURE ONLY */

	int channel_contribution;

	channel_contribution = channel + 1 - 'A';


	if (get_34980A_IDN() == false)
	{
		switch (mod_id)

		{

		case 0:  /*0*/

		case 3:  /*3*/

			connect(mod_id, 331, status);

			connect(mod_id, channel_contribution + 300, status);

			break;

			/*0*/

		case 1:  /*1*/

		case 2:  /*2*/

			connect(mod_id, 332, status);

			connect(mod_id, channel_contribution + 310, status);

			break;

			/*1*/

		default:

			assert(false);

			break;

		}/*case*/
	}
	else if (get_34980A_IDN() == true)//chun 09032022
	{
		switch (mod_id)
		{
		case 1: /*0*/
		case 2:
			//module_contribution = 10;
			//connect_34980A_relay(2067);//(2044);//connect X1 module 2 for +,+S p72

			if (status == true)
			  {
				  connect_34980A_relay(2028); // + channel_contribution); //connect X1-X8 bus to -, -S //fikri16062022
				  connect_34980A_relay(3037);//connect X2 slot 2(mux) to slot 3(mux)  //Fikri25042022
			  }
			  else if (status == false)
			  {
				  disconnect_34980A_relay(2028);// + channel_contribution); //disconnect X1-X8 bus to -, -S //fikri16062022
				  disconnect_34980A_relay(3037);//disconnect X2 slot 2(mux) to slot 3(mux)  //Fikri25042022
			  }
			break;
		case 0: /*0*/
		case 3:
			//module_contribution = 0;
			connect_34980A_relay(1022);//(2059);//(2036);//connect X1 module 3 for +,+S p72
 				if (status == true)
			  {

				  connect_34980A_relay(2000 + channel_contribution); //connect X1-X8 bus to -, -S //fikri16062022
			      connect_34980A_relay(3037);//connect X2 slot 2(mux) to slot 3(mux)
			  }
			  else if (status == false)
			  {
				  disconnect_34980A_relay(2000 + channel_contribution); //connect X1-X8 bus to -, -S //fikri16062022
			      disconnect_34980A_relay(3037);//disconnect X2 slot 2(mux) to slot 3(mux)
			   }
			break;
		default:
			assert(false);
			break;
		}/*case*/
	}

}  /* connect_minus_Xn_plus_2 */

/* ******************************************************************** */

void connect_minus_xn_plus_1(int mod_id, int channel, int status)//-,-s pin 72

{

  /* USED BY ANALOG FIXTURE ONLY */

  int channel_contribution;

  channel_contribution = channel + 1 - 'A';


	  if (get_34980A_IDN() == false)
	  {
		  switch (mod_id)

		  {

		  case 0:  /*0*/

		  case 3:  /*3*/

			  connect(mod_id, 331, status);

			  connect(mod_id, channel_contribution + 300, status);

			  break;

			  /*0*/

		  case 1:  /*1*/

		  case 2:  /*2*/

			  connect(mod_id, 332, status);

			  connect(mod_id, channel_contribution + 310, status);

			  break;

			  /*1*/

		  default:

			  assert(false);

			  break;

		  }/*case*/
	  }
	  else if (get_34980A_IDN() == true)//chun 09032022
	  {
		  switch (mod_id)
		  {
		  case 0: /*0*/
			  //module_contribution = 0;
			  connect_34980A_relay(1022);//disconnect X2 from from PS 6634B
			  if (status == true)
			  {
				  //connect_34980A_relay(2036 + channel_contribution);
			      //connect_34980A_relay(3036);//connect X2 slot 2(mux) to slot 3(mux)

				  connect_34980A_relay(2000 + channel_contribution); //connect X1-X8 bus to -, -S //fikri16062022
			      connect_34980A_relay(3036);//connect X2 slot 2(mux) to slot 3(mux)
			  }
			  else if (status == false)
			  {
			     //disconnect_34980A_relay(2036 + channel_contribution);
			     //disconnect_34980A_relay(3036);//disconnect X2 slot 2(mux) to slot 3(mux)
			     // disconnect_34980A_relay(3005);//disconnect X1 module 3 for -,-S p25,p27  //ori is 3041

				 disconnect_34980A_relay(2000 + channel_contribution); //connect X1-X8 bus to -, -S //fikri16062022
			     disconnect_34980A_relay(3036);//disconnect X2 slot 2(mux) to slot 3(mux)

			  }
			  break;
		  case 1: /*0*/
			  //module_contribution = 10;
			  //connect_34980A_relay(2044);//connect X1 module 2 for +,+S p72
			  if (status == true)
			  {
				  //connect_34980A_relay(2051 + channel_contribution);
				  //connect_34980A_relay(3036);//connect X2 slot 2(mux) to slot 3(mux)  //Fikri25042022

				  connect_34980A_relay(2020 + channel_contribution); //connect X1-X8 bus to -, -S //fikri16062022
				  connect_34980A_relay(3036);//connect X2 slot 2(mux) to slot 3(mux)  //Fikri25042022
			  }
			  else if (status == false)
			  {
				  //disconnect_34980A_relay(2051 + channel_contribution);
				  //disconnect_34980A_relay(3036);//disconnect X2 slot 2(mux) to slot 3(mux)  //Fikri25042022

				  disconnect_34980A_relay(2020 + channel_contribution); //disconnect X1-X8 bus to -, -S //fikri16062022
				  disconnect_34980A_relay(3036);//disconnect X2 slot 2(mux) to slot 3(mux)  //Fikri25042022
			  }
			  break;
		  case 2:
			  //module_contribution = 10;
			  //connect_34980A_relay(2044);//connect X1 module 2 for +,+S p72
			  if (status == true)
			  {
				  //connect_34980A_relay(2051 + channel_contribution);
				  //connect_34980A_relay(3036);//connect X2 slot 2(mux) to slot 3(mux)  //Fikri25042022

				  connect_34980A_relay(2020 + channel_contribution); //connect X1-X8 bus to -, -S //fikri16062022
				  connect_34980A_relay(3036);//connect X2 slot 2(mux) to slot 3(mux)  //Fikri25042022
			  }
			  else if (status == false)
			  {
				  //disconnect_34980A_relay(2051 + channel_contribution);
				  //disconnect_34980A_relay(3036);//disconnect X2 slot 2(mux) to slot 3(mux)  //Fikri25042022

				  disconnect_34980A_relay(2020 + channel_contribution); //disconnect X1-X8 bus to -, -S //fikri16062022
				  disconnect_34980A_relay(3036);//disconnect X2 slot 2(mux) to slot 3(mux)  //Fikri25042022
			  }
			  break;
		  case 3:
			  //module_contribution = 0;
			  connect_34980A_relay(1022);//disconnect X2 from from PS 6634B
			  if (status == true)
			  {
				  //connect_34980A_relay(2036 + channel_contribution);
			      //connect_34980A_relay(3036);//connect X2 slot 2(mux) to slot 3(mux)

				  connect_34980A_relay(2000 + channel_contribution); //connect X1-X8 bus to -, -S //fikri16062022
			      connect_34980A_relay(3036);//connect X2 slot 2(mux) to slot 3(mux)
			  }
			  else if (status == false)
			  {
			     //disconnect_34980A_relay(2036 + channel_contribution);
			     //disconnect_34980A_relay(3036);//disconnect X2 slot 2(mux) to slot 3(mux)
			     // disconnect_34980A_relay(3005);//disconnect X1 module 3 for -,-S p25,p27  //ori is 3041

				 disconnect_34980A_relay(2000 + channel_contribution); //connect X1-X8 bus to -, -S //fikri16062022
			     disconnect_34980A_relay(3036);//disconnect X2 slot 2(mux) to slot 3(mux)

			  }
			  break;
		  default:
			  assert(false);
			  break;
		  }/*case*/
	  }

}  /* connect_minus_Xn_plus_1 */

/* ******************************************************************** */

void connect_osc_ch1_af2(int mod_id)

{  /*connect_osc_AF2*/

  /* USED BY DIGITAL FIXTURE ONLY */

  /* New Sales Rack configuration for Monet - JRM 2/22/93 */

  connect(mod_id, 702, true);

  connect(mod_id, 911, true);

  connect(mod_id, 823, true);

  connect(mod_id, mod_id + 901, true);

}  /*connect_osc_AF2*/



/* ******************************************************************** */

void connect_dmm50_af2(int mod_id)

{

   /* USED BY DIGITAL FIXTURE ONLY */
	if (get_34980A_IDN() == false)
	{
	  connect(mod_id, 911, true);

	  connect(mod_id, 822, true);

 	  connect(mod_id, mod_id + 901, true);
	}
	else if (get_34980A_IDN() == true)
	{
		//connect_34980A_relay(5103);
		connect_34980A_relay(7201);//chun added on 08032022  counter CH1
		connect_34980A_relay(7302);//chun added on 08032022  counter CH1
		//connect_34980A_relay(5102);//chun added on 08032022  counter CH2
		if (mod_id == 0)
		{
			 connect_34980A_relay(8301);
	 	}
		if (mod_id == 1)
		{
			connect_34980A_relay(8302);
		}
		if (mod_id == 2)
		{
			connect_34980A_relay(8303);
		}
		if (mod_id == 3)
		{
			connect_34980A_relay(8304);
		}

	}



}  /*connect_osc_AF2*/



/* ******************************************************************** */

void aconnect_osc_ch1_af2(int mod_id)

{  /*connect_osc_AF2_analog*/

  /* USED BY ANALOG FIXTURE ONLY */
	if (get_34980A_IDN() == false)
	{
		connect(mod_id, 913, true);

		connect(mod_id, mod_id + 901, true);
	}
	else if (get_34980A_IDN() == true)
	{
		//connect_34980A_relay(5103);
		connect_34980A_relay(5101);//chun added on 08032022  counter CH1
		//connect_34980A_relay(5102);//chun added on 08032022  counter CH2
		if (mod_id == 0)
		{
			connect_34980A_relay(5401);
		}
		if (mod_id == 1)
		{
			connect_34980A_relay(5402);
		}
		if (mod_id == 2)
		{
			connect_34980A_relay(5403);
		}
		if (mod_id == 3)
		{
			connect_34980A_relay(5404);
		}

	}

}  /*connect_osc_AF2_analog*/



/* ******************************************************************** */

void aconnect_cntr_stop_af2(int mod_id)

{  /*connect_cntr_stop_AF2_analog*/

  /* USED BY ANALOG FIXTURE ONLY */
	if (get_34980A_IDN() == true)
	{
		connect_34980A_relay(5101);//connect 53230A counter channel 1 to RF mux bank 1 101
		connect_34980A_relay(5102);//connect 53230A counter channel 2 to RF mux bank 1 102

		if (mod_id == 0)
		{
			connect_34980A_relay(5401);//connect RF mux com to bank 4 401 if module 0 //AF2
		}
		if (mod_id == 1)
		{
			connect_34980A_relay(5402);//connect RF mux com to bank 4 402 if module 1 //AF2
		}
		if (mod_id == 2)
		{
			connect_34980A_relay(5403);//connect RF mux com to bank 4 403 if module 2 //AF2
		}
		if (mod_id == 3)
		{
			connect_34980A_relay(5404);//connect RF mux com to bank 4 404 if module 3 //AF2
		}
	}
	else
	{
		connect(mod_id, 911, true);

		connect(mod_id, mod_id + 901, true);
	}

}  /*connect_cntr_stop_AF2_analog*/



/* ******************************************************************** */

void connect_osc_ch1_cable_measure(int mod_id)

{  /*connect_osc_cable_measure*/

  /* USED BY DIGITAL FIXTURE ONLY */

  connect(mod_id, 704, true);

}  /*connect_osc_cable_measure*/



/* ******************************************************************** */

void connect_osc_ch2_uclk(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  connect(mod_id, 722, true);

  connect(mod_id, 613, true);

  if (four_mod_fixture(mod_id))

    connect(mod_id, mod_id + 621, true);

  else

  {

    switch (mod_id)

    {



    case 0:

    case 3:

      connect(mod_id, 624, true);

      break;



    case 1:

    case 2:

      connect(mod_id, 623, true);

      break;



    default:

      assert(false);

      break;



    }

  }

}  /*connect_osc_ch2_UCLK*/



/* ******************************************************************** */

void connect_cntr_ch1_uclk(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  // connect(mod_id, 504, true);  //FIKRI added old switchbox 28/06/2024
  connect_34980A_relay(6404);   //FIKRI added new switchbox 28/06/2024

    // connect(mod_id, 700, true);  // commented for scope elim

  // connect(mod_id, 702, true);   // minerva change for scope elim   //FIKRI added old switchbox 28/06/2024
   connect_34980A_relay(5402);   // minerva change for scope elim   //FIKRI added new switchbox 28/06/2024

  // connect(mod_id, 612, true);  //FIKRI added old switchbox 28/06/2024
  connect_34980A_relay(7102);  //FIKRI added new switchbox 28/06/2024

  if (four_mod_fixture(mod_id))

    //connect(mod_id, mod_id + 621, true);  //FIKRI added old switchbox 28/06/2024
    connect_34980A_relay(mod_id + 8201);  //FIKRI added new switchbox 28/06/2024

  else

  {

    switch (mod_id)

    {



    case 0:

    case 3:

      //connect(mod_id, 624, true);  //FIKRI added old switchbox 28/06/2024
      connect_34980A_relay(8204);  //FIKRI added NEW switchbox 28/06/2024

      break;



    case 1:

    case 2:

      //connect(mod_id, 623, true);  //FIKRI added old switchbox 28/06/2024
      connect_34980A_relay(8203);  //FIKRI added new switchbox 28/06/2024

      break;



    default:

      assert(false);

      break;



    }

  }

}  /*connect_cntr_ch1_UCLK*/



/* ******************************************************************** */

void connect_plus_calib1(int mod_id)

{  /*connect_dmm_plus_CALIB1*/

  /* USED BY ANALOG FIXTURE ONLY */

	if (get_34980A_IDN() == false)//chun23112021
	{
		switch (mod_id)

		{



		case 0:

			connect(mod_id, 327, true);

			break;



		case 1:

			connect(mod_id, 328, true);

			break;



		case 2:

			connect(mod_id, 328, true);

			break;



		case 3:

			connect(mod_id, 327, true);

			break;



		default:

			assert(false);

			break;



		}/*case*/
	}
	else if (get_34980A_IDN() == true)//chun23112021
	{
		switch (mod_id)
		{
		case 0:
			connect_34980A_relay(3008); //connect_34980A_relay(3007);//(3007);//chun23112021
			break;
		case 1:
			connect_34980A_relay(3007); //connect_34980A_relay(3008); //(3008);//chun23112021
			break;
		case 2:
			connect_34980A_relay(3007); //connect_34980A_relay(3008); //(3008);//chun23112021
			break;
		case 3:
			connect_34980A_relay(3008); //connect_34980A_relay(3007); //(3007);//chun23112021
			break;
		default:

			assert(false);
			break;
		}/*case*/
	}

}  /*connect_dmm_plus_CALIB1*/



/* ******************************************************************** */

void connect_plus_gout(int mod_id)

{  /*connect_dmm_plus_GOUT*/

  /* USED BY ANALOG FIXTURE ONLY */
	if (get_34980A_IDN == false)
	{
		switch (mod_id)

		{



		case 0:

		case 3:

			connect(mod_id, 325, true);

			break;



		case 1:

		case 2:

			connect(mod_id, 326, true);

			break;



		default:

			assert(false);

			break;



		}/*case*/
	}
	else if (get_34980A_IDN() == true)
	{
		switch (mod_id)
		{
		case 0: /*0*/
			//module_contribution = 0;
			connect_34980A_relay(3004);//connect X1 module 3 for +,+S p25,p27  //ori is 3041
			break;
		case 1: /*0*/
			//module_contribution = 10;
			connect_34980A_relay(3005);//connect X1 module 2 for +,+S p25,p27  //ori is 3005
			break;
		case 2:
			//module_contribution = 10;
			connect_34980A_relay(3005);//connect X1 module 2 for +,+S p25,p27  //ori is 3005
			break;
		case 3:
			//module_contribution = 0;
			connect_34980A_relay(3004);//connect X1 module 3 for +,+S p25,p27  //ori is 3041
			break;
		default:
			assert(false);
			break;
		}/*case*/
	}



}  /*connect_dmm_plus_GOUT*/



/* ******************************************************************** */

void connect_plus_xn(int mod_id, int channel, int status)

{

  /* This routine provides source connections to the fixture. */

  /* SOURCE AND SENSE ARE SHORTED INSIDE THE MINERVA FIXURE. */



  /* USED BY ANALOG FIXTURE ONLY */

  int channel_contribution;

  channel_contribution = channel + 1 - 'A';

  if (get_34980A_IDN() == true)//fikri28012022
  {
	  switch (mod_id)
	  {
     	  case 0: /*0*/
		  //module_contribution = 0;
		  //connect_34980A_relay(1023);//disconnect X1 from from PS 6634B
		  if (status == true)
		  {
			  //connect_34980A_relay(2000 + channel_contribution);
		      //connect_34980A_relay(3001);//connect X1 from from slot 2(mux) to slot 3(mux)  //Fikri25042022

			  connect_34980A_relay(2000 + channel_contribution);
		      connect_34980A_relay(3002);//connect X1 from from slot 2(mux) to slot 3(mux)  //Fikri25042022
		  }
		  else if (status == false)
		  {
			 //disconnect_34980A_relay(2000 + channel_contribution);
		     //disconnect_34980A_relay(3001);//disconnect X1 from from slot 2(mux) to slot 3(mux) //Fikri25042022
		     //disconnect_34980A_relay(3005);//disconnect X1 module 3 for -,-S p25,p27  //ori is 3041

			  disconnect_34980A_relay(2000 + channel_contribution);
		      disconnect_34980A_relay(3002);//disconnect X1 from from slot 2(mux) to slot 3(mux) //Fikri25042022
		  }
		  break;
 	  	  case 1: /*0*/
		  //module_contribution = 10;
		  //connect_34980A_relay(2009);//connect X1 module 2 for +,+S p71
		  if (status == true)
		  {
			  //connect_34980A_relay(2008 + channel_contribution);
			  //connect_34980A_relay(3001);//connect X1 from from slot 2(mux) to slot 3(mux) //Fikri25042022

			  connect_34980A_relay(2020 + channel_contribution);
			  connect_34980A_relay(3002);//connect X1 from from slot 2(mux) to slot 3(mux) //Fikri25042022
		  }
		  else if (status == false)
		  {
			  //disconnect_34980A_relay(2008 + channel_contribution);
			  //disconnect_34980A_relay(3001);//disconnect X1 from from slot 2(mux) to slot 3(mux)  //Fikri25042022

			  disconnect_34980A_relay(2020 + channel_contribution);
			  disconnect_34980A_relay(3002);//disconnect X1 from from slot 2(mux) to slot 3(mux)  //Fikri25042022
		  }
		  break;
	      case 2:
		  //module_contribution = 10;
		  //connect_34980A_relay(2009);//connect X1 module 2 for +,+S p71
		  if (status == true)
		  {
			  //connect_34980A_relay(2008 + channel_contribution);
			  //connect_34980A_relay(3001);//connect X1 from from slot 2(mux) to slot 3(mux) //Fikri25042022

			  connect_34980A_relay(2020 + channel_contribution);
			  connect_34980A_relay(3002);//connect X1 from from slot 2(mux) to slot 3(mux) //Fikri25042022
		  }
		  else if (status == false)
		  {
			  //disconnect_34980A_relay(2008 + channel_contribution);
			  //disconnect_34980A_relay(3001);//disconnect X1 from from slot 2(mux) to slot 3(mux)  //Fikri25042022

			  disconnect_34980A_relay(2020 + channel_contribution);
			  disconnect_34980A_relay(3002);//disconnect X1 from from slot 2(mux) to slot 3(mux)  //Fikri25042022
		  }
		  break;
       case 3:
		  //module_contribution = 0;
		  //connect_34980A_relay(1023);//disconnect X1 from from PS 6634B
		  if (status == true)
		  {
			  //connect_34980A_relay(2000 + channel_contribution);
		      //connect_34980A_relay(3001);//connect X1 from from slot 2(mux) to slot 3(mux)  //Fikri25042022

			  connect_34980A_relay(2000 + channel_contribution);
		      connect_34980A_relay(3002);//connect X1 from from slot 2(mux) to slot 3(mux)  //Fikri25042022
		  }
		  else if (status == false)
		  {
			 //disconnect_34980A_relay(2000 + channel_contribution);
		     //disconnect_34980A_relay(3001);//disconnect X1 from from slot 2(mux) to slot 3(mux) //Fikri25042022
		     //disconnect_34980A_relay(3005);//disconnect X1 module 3 for -,-S p25,p27  //ori is 3041

			  disconnect_34980A_relay(2000 + channel_contribution);
		      disconnect_34980A_relay(3002);//disconnect X1 from from slot 2(mux) to slot 3(mux) //Fikri25042022
		  }
		  break;
	  default:
		  assert(false);
		  break;
	  }/*case*/

  }
  else if(get_34980A_IDN() == false)
  {

	  switch (mod_id)

	  {



	  case 0:  /*0*/
	  case 3:  /*3*/

		  connect(mod_id, 321, status);

		  connect(mod_id, channel_contribution + 300, status);

		  break;



	  case 1:  /*1*/
	  case 2:  /*2*/

		  connect(mod_id, 322, status);

		  connect(mod_id, channel_contribution + 310, status);

		  break;



	  default:

		  assert(false);

		  break;
	  }



  }/*case*/



}  /*connect_plus_Xn*/




/* ******************************************************************** */

void connect_4wire_plus_xn(int mod_id, int channel, int status)

{

  /* This routine provides source and sense connections all the way to the */

/* testhead fixture. The test is expected to provide the int between */

  /* source and sense on the board under test.                             */

  /* USED BY ANALOG FIXTURE ONLY */

  int channel_contribution;



  channel_contribution = channel + 1 - 'A';


  if (get_34980A_IDN() == false)
  {
	  switch (mod_id)

  {



  case 0:  /*0*/

  case 3:  /*3*/

    connect(mod_id, 323, status);

    connect(mod_id, channel_contribution + 300, status);

    break;

    /*mod 0/3*/



  case 1:  /*1*/

  case 2:  /*2*/

    connect(mod_id, 324, status);

    connect(mod_id, channel_contribution + 310, status);

    break;

    /*mod 1/2*/



  default:

    assert(false);

    break;



	  }/*case*/
  }
  else if (get_34980A_IDN() == true)
  {
	  switch (mod_id)
	  {
	  case 0:  /*0*/
	  case 3:  /*3*/
		  //connect(mod_id, 323, status);
		  //connect(mod_id, channel_contribution + 300, status);
		  //connect_34980A_relay(2000+channel_contribution);
		  //connect_34980A_relay(3001);
		  //connect_34980A_relay(2036 + channel_contribution);
		  //connect_34980A_relay(3036);

		  //module_contribution = 10;  //ori
		  if (status == true)
		  {
			  connect_34980A_relay(2000+channel_contribution);
			  connect_34980A_relay(3001);//connect X1 from from slot 2(mux) to slot 3(mux) //Fikri16062022
		  }
		  else if (status == false)
		  {
			  disconnect_34980A_relay(2000 + channel_contribution);
			  disconnect_34980A_relay(3001);//disconnect X1 from from slot 2(mux) to slot 3(mux)  //Fikri16062022
		  }
		  break;
		  /*mod 0/3*/

	  case 1:  /*1*/
	  case 2:  /*2*/
		  //connect(mod_id, 324, status);
		  //connect(mod_id, channel_contribution + 310, status);
		  //connect_34980A_relay(2008+channel_contribution);
		  //connect_34980A_relay(2044+channel_contribution);
		  //module_contribution = 10; //ori
		  if (status == true)
		  {
			  connect_34980A_relay(2020+channel_contribution);
			  connect_34980A_relay(3001);//connect X1 from from slot 2(mux) to slot 3(mux) //Fikri16062022
		  }
		  else if (status == false)
		  {
			  disconnect_34980A_relay(2020 + channel_contribution);
			  disconnect_34980A_relay(3001);//disconnect X1 from from slot 2(mux) to slot 3(mux)  //Fikri16062022
		  }
		  break;

		  /*mod 1/2*/

	  default:

		  assert(false);

		  break;

	  }/*case*/
  }

}  /*connect_4wire_plus_Xn*/



/* ******************************************************************** */

void connect_power_supply_xmint(int mod_id)

{

  /* Connects X1 and X2 to the 6038A power supply + and - respectively */

  /* USED BY ANALOG FIXTURE ONLY */

  int module_contribution;

  if (get_34980A_IDN() == true)
  {
      //ori code
	  //connect_34980A_relay(2911);


	  connect_34980A_relay(1024);  //fikri25012022 //connect com2lo to N6700
	  connect_34980A_relay(1028);  //fikri25012022 //connect com2hi to N6700

	  switch (mod_id)
	  {
	  case 0:
		  //module_contribution = 0;
		  //connect_34980A_relay(2001);//connect X1 module 0 for +,+S
		  //connect_34980A_relay(2037);//connect X2 module 0 for -,-S

		  connect_34980A_relay(2001);//connect X1 module 0 for +,+S
		  //connect_34980A_relay(2037);//connect X2 module 0 for -,-S
		  break;
	  case 1:
		  //module_contribution = 10;
		  //connect_34980A_relay(2009);//connect X1 module 1 for +,+S
		  //connect_34980A_relay(2052);//connect X2 module 1 for -,-S
		  //connect_34980A_relay(2021);

		  //connect_34980A_relay(2009);//connect X1 module 1 for +,+S
		  //connect_34980A_relay(2052);//connect X2 module 1 for -,-S

		  connect_34980A_relay(2021);//connect X1,X2 module 1 for +,+S(x1), - ,-S(x2) //fikri16062022


		  break;
	  case 2:
		  //module_contribution = 10;
		  //ori code
		  //connect_34980A_relay(2009);//connect X1 module 2 for +,+S
		  //connect_34980A_relay(2052);//connect X2 module 2 for -,-S

		  //connect_34980A_relay(2001);
		  //connect_34980A_relay(2009);
		  //connect_34980A_relay(2021);

		  //connect_34980A_relay(2009);//connect X1 module 2 for +,+S
		  //connect_34980A_relay(2052);//connect X2 module 2 for -,-S

		  connect_34980A_relay(2021);//connect X1,X2 module 1 for +,+S(x1), - ,-S(x2) //fikri16062022

		  break;
	  case 3:
		  //module_contribution = 0;
		  //connect_34980A_relay(2001);//connect X1 module 3 for +,+S
		  //connect_34980A_relay(2037);//connect X2 module 3 for -,-S

		  connect_34980A_relay(2001);//connect X1 module 3 for +,+S
		  //connect_34980A_relay(2037);//connect X2 module 3 for -,-S
		  break;
	  default:

		  assert(false);

		  break;
	  }/*case*/
  }

  else
  {

	  connect(mod_id, 390, true);

	  connect(mod_id, 370, true);

	  switch (mod_id)

	  {



	  case 0:

		  module_contribution = 0;

		  break;



	  case 1:

		  module_contribution = 10;

		  break;



	  case 2:

		  module_contribution = 10;

		  break;



	  case 3:

		  module_contribution = 0;

		  break;



	  default:

		  assert(false);

		  break;



	  }/*case*/

		connect(mod_id, module_contribution + 301, true);

  }

}  /*connect_power_supply_XMint*/

/* ******************************************************************** */

void connect_power_supply_xmint_PS6634B(int mod_id)

{

  /* Connects X1 and X2 to the 6038A power supply + and - respectively */

  /* USED BY ANALOG FIXTURE ONLY */

  int module_contribution;

  if (get_34980A_IDN() == true)
  {
      //ori code
	  //connect_34980A_relay(2911);
	  connect_34980A_relay(1020);  //fikri25012022 //connect com2lo to PS6634B
	  connect_34980A_relay(1019);  //fikri25012022 //connect com2hi to PS6634B

	  switch (mod_id)
	  {
	  case 0:
		  //module_contribution = 0;
		  //connect_34980A_relay(2001);//connect X1 module 0 for +,+S
		  //connect_34980A_relay(2037);//connect X2 module 0 for -,-S

		  connect_34980A_relay(2001);//connect X1 module 0 for +,+S
		  //connect_34980A_relay(2037);//connect X2 module 0 for -,-S
		  break;
	  case 1:
		  //module_contribution = 10;
		  //connect_34980A_relay(2009);//connect X1 module 1 for +,+S
		  //connect_34980A_relay(2052);//connect X2 module 1 for -,-S
		  //connect_34980A_relay(2021);

		  //connect_34980A_relay(2009);//connect X1 module 1 for +,+S
		  //connect_34980A_relay(2052);//connect X2 module 1 for -,-S

		  connect_34980A_relay(2021);//connect X1,X2 module 1 for +,+S(x1), - ,-S(x2) //fikri16062022


		  break;
	  case 2:
		  //module_contribution = 10;
		  //ori code
		  //connect_34980A_relay(2009);//connect X1 module 2 for +,+S
		  //connect_34980A_relay(2052);//connect X2 module 2 for -,-S

		  //connect_34980A_relay(2001);
		  //connect_34980A_relay(2009);
		  //connect_34980A_relay(2021);

		  //connect_34980A_relay(2009);//connect X1 module 2 for +,+S
		  //connect_34980A_relay(2052);//connect X2 module 2 for -,-S

		  connect_34980A_relay(2021);//connect X1,X2 module 1 for +,+S(x1), - ,-S(x2) //fikri16062022

		  break;
	  case 3:
		  //module_contribution = 0;
		  //connect_34980A_relay(2001);//connect X1 module 3 for +,+S
		  //connect_34980A_relay(2037);//connect X2 module 3 for -,-S

		  connect_34980A_relay(2001);//connect X1 module 3 for +,+S
		  //connect_34980A_relay(2037);//connect X2 module 3 for -,-S
		  break;
	  default:

		  assert(false);

		  break;
	  }/*case*/
  }

  else
  {

	  connect(mod_id, 390, true);

	  connect(mod_id, 370, true);

	  switch (mod_id)

	  {



	  case 0:

		  module_contribution = 0;

		  break;



	  case 1:

		  module_contribution = 10;

		  break;



	  case 2:

		  module_contribution = 10;

		  break;



	  case 3:

		  module_contribution = 0;

		  break;



	  default:

		  assert(false);

		  break;



	  }/*case*/

		connect(mod_id, module_contribution + 301, true);

  }

}  /*connect_power_supply_XMint_PS6634B*/


/* ******************************************************************** */

void connect_syn_aux_gen_ext(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  // connect(mod_id, 804, false);  fikri 19/4/2024 old rack
  connect_34980A_relay(5204);    //fikri 19/4/2024 new-rack

}  /*connect_syn_aux_gen_ext*/



/* ******************************************************************** */

void connect_syn_out_gen_ext(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  // connect(mod_id, 804, false);  fikri 19/4/2024 old rack
  connect_34980A_relay(5204);    //fikri 19/4/2024 new-rack

}  /*connect_syn_out_gen_ext*/



/* ******************************************************************** */

void dconnect_syn_out_dmm(int mod_id)

{

  connect(mod_id, 802, true);

  /* The dmm connection is made through the auxiliary 700 connection.  This */

  /* requires relay 701 to be in the open state.                            */

  connect(mod_id, 701, false);



  connect(mod_id, 312, true);   /* Digital DC mux connection */

  connect(mod_id, 390, true);   /* Digital DC mux connection */

  connect(mod_id, 370, true);   /* Digital DC mux connection */



}  /*dconnect_syn_out_dmm*/



/* ******************************************************************** */

void aconnect_syn_out_dmm(int mod_id, int status)

{
	if (get_34980A_IDN() == true)
	{
		connect_34980A_relay(2912);
		//connect_34980A_relay(2922);

		//connect(mod_id, 371, status);//no need to connect as 371 connect bank relay

		//connect(mod_id, 331, status);//no need to connect as 331 connect common LO

		//connect(mod_id, 323, status);//no need to connect as 331 connect common HI
	}
	else
	{
		connect(mod_id, 391, status);

		connect(mod_id, 371, status);

		connect(mod_id, 331, status);

		connect(mod_id, 323, status);
	}


}  /*aconnect_syn_out_dmm*/



/* ******************************************************************** */

void connect_syn_out_osc_ch1(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  connect(mod_id, 802, true);

  connect(mod_id, 701, true);

}  /*connect_syn_out_osc_ch1*/



/* ******************************************************************** */

void connect_syn_out_af2(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  /* New Sales Rack configuration for Monet - JRM 2/22/93 */

  /* connect(mod_id, 801, true);

  connect(mod_id, 821, true);

  connect(mod_id, 911, true);

  connect(mod_id, mod_id + 901, true); */


 /* New Code for new switchbox Fikri added 26/06/2024 */
  connect_34980A_relay(5201);

  connect_34980A_relay(7301);

  connect_34980A_relay(7201);

  if (mod_id == 0)
  {
  	 connect_34980A_relay(8301);
  }
  if (mod_id == 1)
  {
  	connect_34980A_relay(8302);
  }
  if (mod_id == 2)
  {
  	connect_34980A_relay(8303);
  }
  if (mod_id == 3)
  {
  	connect_34980A_relay(8304);
	}
/* End New Code for new switchbox Fikri added 26/06/2024 */

}  /*connect_syn_out_AF2*/



/* ******************************************************************** */

void aconnect_syn_out_xmint(int mod_id)

{

  /* Connects X1 and X2 to the synthesizer output */
  int module_contribution;

  if (get_34980A_IDN() == true)
  {
	  connect_34980A_relay(2912);
	  connect_34980A_relay(3001);   //connect COM1 slot 2 to COM1 slot 3&4 //Fikri25042022
	  connect_34980A_relay(3036);   //connect COM2 slot 2 to COM2 slot 3&4 //Fikri25042022
	  //connect_34980A_relay(2922);
	  switch (mod_id)	  {
	  case 0:
		  //module_contribution = 0;
		  connect_34980A_relay(2001);//connect X1 module 0 for +,+S
		  //connect_34980A_relay(2037);//connect X1 module 0 for -,-S
		  break;
	  case 1:
		  //module_contribution = 10;
		  //connect_34980A_relay(2009);//connect X1 module 1 for +,+S
		  //connect_34980A_relay(2052);//connect X1 module 1 for -,-S

		   connect_34980A_relay(2021);//connect X1 module 1 for +,+S
		  break;
	  case 2:
		  //module_contribution = 10;
		  //connect_34980A_relay(2009);//connect X1 module 2 for +,+S
		  //connect_34980A_relay(2052);//connect X1 module 2 for -,-S

		  connect_34980A_relay(2021);//connect X1 module 2 for +,+S
		  break;
	  case 3:
		  //module_contribution = 0;
		  connect_34980A_relay(2001);//connect X1 module 3 for +,+S
		  //connect_34980A_relay(2037);//connect X1 module 3 for -,-S
		  break;
	  default:

		  assert(false);

		  break;
	  }/*case*/
  }
  else
  {


	  //int module_contribution;



	  connect(mod_id, 391, true);

	  connect(mod_id, 370, true);

	  switch (mod_id)

	  {



	  case 0:

		  module_contribution = 0;

		  break;



	  case 1:

		  module_contribution = 10;

		  break;



	  case 2:

		  module_contribution = 10;

		  break;



	  case 3:

		  module_contribution = 0;

		  break;



	  default:

		  assert(false);

		  break;



	  }/*case*/


	  connect(mod_id, module_contribution + 301, true);

  }


}  /*aconnect_syn_out_XMint*/



/* ******************************************************************** */

void aconnect_vs_dmm(int mod_id, int status)

{

	/* Connects X6 to the dmm input */

	int module_contribution;

	if(get_34980A_IDN() == false)
	{
	connect(mod_id, 321, status);

	connect(mod_id, 331, status);

	switch (mod_id)

	{



	case 0:

	case 3:

		module_contribution = 0;

		break;



	case 1:

	case 2:

		module_contribution = 10;

		connect(mod_id, 370, status);

		break;

	default:

		assert(false);

		break;



	}/*case*/

	connect(mod_id, module_contribution + 306, status);
}
	else if (get_34980A_IDN() == true)
	{
		switch (mod_id)
		{
		case 0:
			//connect_34980A_relay(3006);
			//connect_34980A_relay(3041);
			if (status == true)
			{
			connect_34980A_relay(2006);  //connect x6 to hi,x7 to low module 0
		    connect_34980A_relay(3036);
			connect_34980A_relay(3002);
			}

			else if (status == false)
			{
			disconnect_34980A_relay(2006);  //disconnect x6 from hi,x7 from lo module 0
		    disconnect_34980A_relay(3036);
			disconnect_34980A_relay(3002);
			}
			break;
		case 1:
			//connect_34980A_relay(3014);
			//connect_34980A_relay(3049);
			if (status == true)
			{
			connect_34980A_relay(2026);  //connect x6 to hi,x7 to low module 1
		    connect_34980A_relay(3036);
			connect_34980A_relay(3002);
			}

			else if (status == false)
			{
			disconnect_34980A_relay(2026);  //disconnect x6 from hi,x7 from lo module 1
		    disconnect_34980A_relay(3036);
			disconnect_34980A_relay(3002);
			}
			break;
		case 2:
			//connect_34980A_relay(3014);
			//connect_34980A_relay(3049);
			if (status == true)
			{
			connect_34980A_relay(2026);  //connect x6 to hi,x7 to low module 2
		    connect_34980A_relay(3036);
			connect_34980A_relay(3002);
			}

			else if (status == false)
			{
			disconnect_34980A_relay(2026);  //disconnect x6 from hi,x7 from lo module 2
		    disconnect_34980A_relay(3036);
			disconnect_34980A_relay(3002);
			}
			break;
		case 3:
			//connect_34980A_relay(2006);
		    //connect_34980A_relay(2042);
		    //connect_34980A_relay(3036);
			//connect_34980A_relay(3001);
			if (status == true)
			{
			connect_34980A_relay(2006);  //connect x6 to hi,x7 to low module 2
		    connect_34980A_relay(3036);
			connect_34980A_relay(3002);
			}

			else if (status == false)
			{
			disconnect_34980A_relay(2006);  //disconnect x6 from hi,x7 from lo module 2
		    disconnect_34980A_relay(3036);
			disconnect_34980A_relay(3002);
			}
			break;
		default:
			assert(false);
			break;
		}

	}

}  /*aconnect_Vs_dmm*/



/* ******************************************************************** */

void aconnect_vo_dmm(int mod_id, int status)

{

  /* Connects X3 to the dmm input */

  int module_contribution;

  if (get_34980A_IDN() == false)
  {

	  connect(mod_id, 321, status);

	  connect(mod_id, 331, status);

	  switch (mod_id)

	  {



	  case 0:

	  case 3:

		  module_contribution = 0;

		  break;



	  case 1:

	  case 2:

		  module_contribution = 10;

		  connect(mod_id, 370, status);

		  break;



	  default:

		  assert(false);

		  break;



	  }/*case*/

	  connect(mod_id, module_contribution + 302, status);
  }
  else if (get_34980A_IDN() == true)
  {
	  switch (mod_id)
	  {
	  case 0:
		  //connect_34980A_relay(3002);
	  if (status == true){
		connect_34980A_relay(2003); //x3 hi
		connect_34980A_relay(3036);
		connect_34980A_relay(3003);
	  }
	  else if (status == false){
		disconnect_34980A_relay(2003); //x3 hi
		disconnect_34980A_relay(3036);
		disconnect_34980A_relay(3003);
		}
		  break;
	  case 1:
		  //connect_34980A_relay(3010);
     if (status == true)
	 {
		connect_34980A_relay(2023); //x3 hi
		connect_34980A_relay(3036); //x2 lo
		connect_34980A_relay(3003);
	  }
	  else if (status == false){
		disconnect_34980A_relay(2023); //x3 hi
		disconnect_34980A_relay(3036);
		disconnect_34980A_relay(3003);
		}
		  break;
	  case 2:
		  //connect_34980A_relay(3010);
	  if (status == true)
	  {
		connect_34980A_relay(2023); //x3 hi
		connect_34980A_relay(3036); //x2 lo
		connect_34980A_relay(3003);
	  }
	  else if (status == false)
	  {
		disconnect_34980A_relay(2023); //x3 hi
		disconnect_34980A_relay(3036);
		disconnect_34980A_relay(3003);
		}
		  break;
	  case 3:
	  //connect_34980A_relay(2002);
		  //connect_34980A_relay(2043);
		  //onnect_34980A_relay(3036);
		  //connect_34980A_relay(3001);
    	if (status == true)
		{
		connect_34980A_relay(2003); //x3 hi
		connect_34980A_relay(3036);
		connect_34980A_relay(3003);
		}
		  else if (status == false)
		  {
			disconnect_34980A_relay(2003); //x3 hi
			disconnect_34980A_relay(3036);
			disconnect_34980A_relay(3003);
		}
		  break;
	  }
  }

}  /*aconnect_Vo_dmm*/



/* ******************************************************************** */

void connect_dmm_probe(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

  /*connect(mod_id, 711, true);

  connect(mod_id, 313, true);

  connect(mod_id, 370, true);

  connect(mod_id, 390, true); */ //fikri 21/06/2024 old switchbox


	  connect_34980A_relay(4002); //terminal slot 4 to dmm
      connect_34980A_relay(8001); //probe mode 3
      connect_34980A_relay(4911);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4921);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4912);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4922);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4913);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4923);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4914);      //fikri new switcbox 19/06/2024
      connect_34980A_relay(4924);      //fikri new switcbox 19/06/2024

}  /*connect_dmm_PROBE*/



/* ******************************************************************** */

void aconnect_ps_20_ohm_load(int mod_id, int status)

{

	/*connect(mod_id, 208, status);*/ //original

	if (get_34980A_IDN() == false)
	{
		connect(mod_id, 208, status);
	}
	else if (get_34980A_IDN() == true)
	{
		//ori code
		//connect_34980A_relay(1021);  //fikri25012022

		//connect_34980A_relay(1021);  //fikri25012022
		//connect_34980A_relay(1024);  //fikri25012022 connect com2lo to 6634b
		//connect_34980A_relay(1028);  //fikri25012022 //connect com2hi to 6634b

	//connect_34980A_relay(1021);  //fikri25012022
	//connect_34980A_relay(1024);  //fikri25012022 //connect com2lo to 6634b
	//connect_34980A_relay(1028);  //fikri25012022 //connect com2hi to 6634b
	//fikri14042022	connect_34980A_relay(1022);  //fikri25012022
	//fikri14042022	connect_34980A_relay(1023);  //fikri25012022
	//fikri14042022	connect_34980A_relay(1024);  //fikri25012022

	      if (status == true)
		  {
		//		connect_34980A_relay(1021);  //fikri17052023 //turn on 20ohm resistor - original
				connect_34980A_relay(1025);  //fikri25012022 //turn on 20ohm resistor
				connect_34980A_relay(1019);  //fikri17052023 //turn on 20ohm resistor - new
				connect_34980A_relay(1020);  //fikri25012022 //turn on 20ohm resistor - new
		  }
		  else if (status == false)
		  {
		//	disconnect_34980A_relay(1021);  //fikri17052023 //turn off 20ohm resistor
			disconnect_34980A_relay(1025);  //fikri25012022 //turn off 20ohm resistor
	 	 	connect_34980A_relay(1024);  //fikri17052023 //turn on 20ohm resistor - new
     		connect_34980A_relay(1028);  //fikri17052023 //turn on 20ohm resistor - new
		  }
	}



}  /*aconnect_ps_20_ohm_load*/

/* ********************************ADDED BY SRI THARAN FOR LOAD TEST DH - UTILITY************* */

void connect_load_test_relay(int mod_id, int status)
{
	if (get_34980A_IDN() == true)
	{
		connect(mod_id, 209, status);

		connect(mod_id, 210, status);
	}
	else if (get_34980A_IDN() == false)
	{
		connect(mod_id, 209, status);

		connect(mod_id, 210, status);
	}

}

/* ******************************************************************** */



void connect_1g_ohm_resistor(int mod_id, int status)

{

  /* USED BY ANALOG FIXTURE ONLY */
	if (get_34980A_IDN() == false)
	{
		connect(mod_id, 207, status);
	}
	else if (get_34980A_IDN() == true)
	{
		connect_34980A_relay(1007);
	}


}  /*connect_1G_ohm_resistor*/



/* ******************************************************************** */

void connect_500_ohm_load(int mod_id)

{

  /* USED BY DIGITAL FIXTURE ONLY */

 /* connect(mod_id, 201, true);

  connect(mod_id, 202, true); */

  connect_34980A_relay(1003);   //fikri added new switchbox 5/06/2024

  connect_34980A_relay(1007);   //fikri added new switchbox 5/06/2024

}  /*connect_500_ohm_load*/



/* ******************************************************************** */

void connect_10k_load(int mod_id)

{

  /* USED BY ANALOG FIXTURE ONLY */
	if (get_34980A_IDN() == false)
	{
		connect(mod_id, 203, true);
	}
	else if (get_34980A_IDN() == true)
	{
		connect_34980A_relay(1003);
	}

}  /*connect_10k_load*/



/* ******************************************************************** */

void disconnect_10k_load(int mod_id)

{  /*connect_10k_load*/

  /* USED BY ANALOG FIXTURE ONLY */
	if (get_34980A_IDN() == false)
	{
		connect(mod_id, 203, false);
	}
	else if (get_34980A_IDN() == true)
	{
		disconnect_34980A_relay(1003);
	}

}  /*connect_10k_load*/

/*N6700b connections*/

//Added by chuanho, 30jun2011
/* ******************************************************************** */

void connect_olym_ch1(int mod_id)
{
  /* USED BY ANALOG FIXTURE ONLY */



  /* connecting the n6700b is accomplished by closing the relays since the   */

  /* n6700b channel 1 contacts are made at the normally open connections                 */
	if (get_34980A_IDN() == false)
	{
		connect(mod_id, 213, true); // - of n6700b (white wire)  //chun24112021 -s
		connect(mod_id, 214, true); // -S of n6700b (green wire) //chun24112021 -
		connect(mod_id, 215, true); // +S of n6700b (red wire)   //chun24112021 +s
		connect(mod_id, 216, true); // + of n6700b (black wire)  //chun24112021 +
	}
	else if (get_34980A_IDN() == true)//chun24112021
	{
		connect_34980A_relay(1024); //connect com2lo to relay 2022 //fikri16062022
		connect_34980A_relay(1028); //connect com2hi to relay 2023 //fikri16062022
		connect_34980A_relay(1022); // connect COM2 to -, -S //fikri16062022
		connect_34980A_relay(1023); // connect COM1 to +, +S //fikri16062022
		connect_34980A_relay(1026); // +, +s from N6700C     //fikri16062022
		connect_34980A_relay(1027); // -, -s from N6700C     //fikri16062022
//		connect_34980A_relay(2912); //analog bus2(COM1)
		//connect_34980A_relay(2922);//analog bus2(COM2)
		/*connect_34980A_relay();
		connect_34980A_relay();*/
	}
}

/*End of n6700b connections*/

/* ******************************************************************** */


//Added by chuanho, 30jun2011
/* ******************************************************************** */
/*Analog connection N6700B channel 1 to X Mint*/

void aconnect_olym_ch1_abus1_xmint(int mod_id)

{

  /* Connects X1 and X2 to the N6700B channel 1 output */

  int module_contribution;

  if (get_34980A_IDN()== true)//chun24112021
  {
	  /* Connect N6700B channel 1 to Analog bus 1*/
	  connect_34980A_relay(1024); //connect com2lo to relay 2022 //fikri16062022
	  connect_34980A_relay(1028); //connect com2hi to relay 2023 //fikri16062022
	  connect_34980A_relay(1022); // connect COM2 to -, -S //fikri16062022
	  connect_34980A_relay(1023); // connect COM1 to +, +S //fikri16062022
	  connect_34980A_relay(1026); // +, +s from N6700C     //fikri16062022
	  connect_34980A_relay(1027); // -, -s from N6700C     //fikri16062022
	  /* Connect COM1,2 slot 2 to slot 3 for X1 & X2*/               //Fikri25042022
	  connect_34980A_relay(3001); //COM1 slot 2(COM1) to COM1 slot 3 //Fikri25042022
	  connect_34980A_relay(3036); //COM2 slot 2(COM2) to COM1 slot 3 //Fikri25042022

  }
  else
  {
	  /* Connect N6700B channel 1 to Analog bus 1*/
	  connect(mod_id, 211, true);
	  connect(mod_id, 212, true);

	  /* Connect Analog bus 1 to X1 & X2*/
	  connect(mod_id, 391, true);
	  connect(mod_id, 370, true);
  }

  if (get_34980A_IDN() == true)//chun29112021
  {
	  switch (mod_id)
	  {
	  case 0:
		  //module_contribution = 0;
		  connect_34980A_relay(2001);//connect X1 & X2 module 0 for +,+S(x1), -, -S(x2)
		  //connect_34980A_relay(2037);//connect X2 module 0 for -,-S
		  break;
	  case 1:
		  //module_contribution = 10;
		  //connect_34980A_relay(2009);//connect X1 module 1 for +,+S
		  //connect_34980A_relay(2052);//connect X2 module 1 for -,-S

		  connect_34980A_relay(2021);//connect X1 & X2 module 1 for +,+S(x1), -, -S(x2) //fikri16062022

		  break;
	  case 2:
		  //module_contribution = 10;
		  //connect_34980A_relay(2009);//connect X1 module 2 for +,+S
		  //connect_34980A_relay(2052);//connect X2 module 2 for -,-S

		  connect_34980A_relay(2021);//connect X1 & X2 module 2 for +,+S(x1), -, -S(x2) //fikri16062022

		  break;
	  case 3:
		  //module_contribution = 0;
		  connect_34980A_relay(2001);//connect X1 module 3 for +,+S
		  //connect_34980A_relay(2037);//connect X1 module 3 for -,-S
		  break;

	  default:

		  assert(false);

		  break;

	  }/*case*/
  }
  else
  {
	  switch (mod_id)

	  {



	  case 0:

		  module_contribution = 0;

		  break;



	  case 1:

		  module_contribution = 10;

		  break;



	  case 2:

		  module_contribution = 10;

		  break;



	  case 3:

		  module_contribution = 0;

		  break;



	  default:

		  assert(false);

		  break;



	  }/*case*/

		connect(mod_id, module_contribution + 301, true);

  }

}
/*End of Analog connection N6700B channel 1 to X Mint*/

/* ******************************************************************** */

//Added by chuanho, 30jun2011
/* ******************************************************************** */
/*Connect N6700B output to dmm*/
void aconnect_olym_ch1_out_dmm (int mod_id, int status)
{
	/*Used by Analog Fixture only*/

	if (get_34980A_IDN() == false)
	{
		connect(mod_id, 390, status);

		connect(mod_id, 331, status);

		connect(mod_id, 321, status);
	}
	else if (get_34980A_IDN()== true)
	{
		connect_34980A_relay(2912);//connect analog bus 2 for N6700C(COM1) //chun29112021
		//connect_34980A_relay(2922);//connect analog bus 2 for N6700C(COM2)
		//connect(mod_id, 331, status);//no need to connect as 331 connect common LO //not sure still need this connection, need to confirm
		//connect(mod_id, 321, status);//no need to connect as 331 connect common HI //not sure still need this connection, need to confirm
	}
}
	/*End of Connect N6700B output to dmm*/

//Added by chuanho, 18jul2011
/* ******************************************************************** */


/*Digital connection N6700B channel 1 output to af2*/
void dconnect_olym_ch1_af2(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */

  /*connect(mod_id, 724, true);

  connect(mod_id, 824, true);

  connect(mod_id, 911, true);

  connect(mod_id, mod_id + 901, true); */

 /* New Code for new switchbox Fikri added 26/06/2024 */
  connect_34980A_relay(5201);

  connect_34980A_relay(7301);

  connect_34980A_relay(7201);

  if (mod_id == 0)
  {
  	 connect_34980A_relay(8301);
  }
  if (mod_id == 1)
  {
  	connect_34980A_relay(8302);
  }
  if (mod_id == 2)
  {
  	connect_34980A_relay(8303);
  }
  if (mod_id == 3)
  {
  	connect_34980A_relay(8304);
	}
/* End New Code for new switchbox Fikri added 26/06/2024 */


}
	/*End of Digital connection N6700B channel 1 output to af2*/

/* ******************************************************************** */


/* ******************************************************************** */

void disconnect_all(int mod_id)

{

  /* USED BY BOTH DIGITAL AND ANALOG FIXTURE */

  vth_mhpib_wr(mod_id, 1, "RESET");

}  /*disconnect_all*/



/* ******************************************************************** */

void get_mux_serial_number(int mod_id)

{



  /* USED BY BOTH DIGITAL FIXTURE */

  vth_mhpib_wr(mod_id, 1, "DEFKEY? 'U'");

  vth_mhpib_rd(mod_id, 1);

}  /*get_mux_serial_number*/



/* ******************************************************************** */

char *get_sales_rack_serial_number(char *result, int mod_id)

{

  /* USED BY DIGITAL FIXTURE */



  /* This procedure takes as input the module number and returns the */

  /* minerva serial number as type s80.                              */

  test_result_type test_result;

  char rack_serial_number[81];

  char *s_ptr;

  char str1[81];



  vth_mhpib_wr(mod_id, 1, "DEFKEY? 'U'");

  vth_mhpib_rd(mod_id, 1);

  flush_commands(mod_id); //fikri added 19/06/2024
  if (get_result_nolog(mod_id, &test_result))

  {

    test_result.Measurement_class = CLASS_PSTRING_PTR;

    s_ptr = static_cast(char*, test_result.UU.Pstring_ptr);

    strcpy(rack_serial_number, s_ptr);

    /* The following line strips the " marks from either side of the S/N */

    if (mtd_strlen(rack_serial_number) > 11)

    {

      sprintf(str1, "%.10s", rack_serial_number + 1);

      strcpy(rack_serial_number, str1);

    }

    error = false;

  }

  else

  {

    display_prompt("No result in result log (Expected mux vth_reading)", true);

    strcpy(rack_serial_number, "UNDEFINED");

    error = true;

  }

  return strcpy(result, rack_serial_number);

}  /*get_sales_rack_serial_number*/



/* ******************************************************************** */

void store_mux_serial_number(int mod_id, char *serial_number)

{

  /* USED BY BOTH DIGITAL FIXTURE */

  char setup_string[81];



  *setup_string = '\0';

  strcat(setup_string, "DEFKEY 'U','");

  strcat(setup_string, serial_number);

  strcat(setup_string, "'");

  vth_mhpib_wr(mod_id, 1, setup_string);

}  /*store_mux_serial_number*/





/* ******************************************************************** */

void close_mux(int mod_id)

{

  /* USED BY BOTH DIGITAL AND ANALOG FIXTURE */

  disconnect_all(mod_id);

  vth_mhpib_close(mod_id, 1);

}  /*close_mux*/



/* ******************************************************************** */

void open_mux(int mod_id)

{

  /* USED BY BOTH DIGITAL AND ANALOG FIXTURE */

  s_type s;

  char str1[92];



  test_result_type result;



  // SYNC UP THE PIPE   //mjs added this

  if (get_result_nolog(mod_id, &result))

    display_prompt("Failed to sync up pipe when opening mux", true);



  vth_mhpib_open(mod_id, 1, "/dev/sales.mux");

  disconnect_all(mod_id);

  *s = '\0';

  show_present_test_info(present_test[mod_id - NO_TESTHEAD_NUMBER], mod_id,

                         present_slot[mod_id - NO_TESTHEAD_NUMBER], s);

  sprintf(s, "DISP MSG,\"%s", strcpy(str1, s));

  strcat(s, "\"");

  vth_mhpib_wr(mod_id, 1, s);

}  /*open_mux*/



/*hp*/

/* wpd - include begins here */

/* Modified 4/9/97 for 8110A Pulse Generator*/

/* Use tag 3 for the 8110A Pulse Generator */

/* *********************************************************************** */

void setup_gen_defaults(int mod_id)

{

  vth_mhpib_wr(mod_id, 3, "*RST");      /*Reset the 8110A*/

  vth_mhpib_wr(mod_id, 3, "*SRE 0");

  vth_mhpib_wr(mod_id, 3, "*ESE 0");

  vth_mhpib_wr(mod_id, 3, "*CLS");

  vth_mhpib_wr(mod_id, 3, ":ARM:SOUR EXT1");  /*Setup for the triggered mode*/

  vth_mhpib_wr(mod_id, 3, ":ARM:LEV 0V");   /*Trigger Level*/

  vth_mhpib_wr(mod_id, 3, ":PULS:TRAN1 4.5NS");   /*Pulse Transition time 4.5nS*/

              /* to match 8112 */

  vth_mhpib_wr(mod_id, 3, ":VOLT:LOW 0.0V");  /*Low level of output = 0.0v*/

  vth_mhpib_wr(mod_id, 3, ":VOLT:HIGH 1.5V");   /*High level of output = 1.5v*/

  vth_mhpib_wr(mod_id, 3, ":ARM:SOUR EXT1");  /*Pulse width = 500nS*/



//old code for 8112 generator used the 65ns

//  vth_mhpib_wr(mod_id, 3, ":PULS:DEL1 65NS");   /*Trigger to output delay = 65nS*/



//8110 generator requires this...



//mjs changed this next line for experimental purposes

//  vth_mhpib_wr(mod_id, 3, ":PULS:DEL1 45NS");   /*Trigger to output delay = 45nS*/

  vth_mhpib_wr(mod_id, 3, ":PULS:DEL1 38NS");   /*Trigger to output delay = 38nS*/



}  /*setup_gen_defaults*/



/* *********************************************************************** */

void open_gen(int mod_id)
{
	//if (get_34980A_IDN() == false)
	//{
		vth_mhpib_open(mod_id, 3, "/dev/sales.gen");

		setup_gen_defaults(mod_id);
	//}
	//else if (get_34980A_IDN() == true)
	//{
	/*
		ViStatus  hw_err;
		char buf1[256] = { 0 };
		char str1[256] = { 0 };

		err = viQueryf(viGEN, "*IDN?", "%t", &buf);  //buf for IDN
		hw_err = viQueryf(viGEN, "SYST:ERR?", "%t", &buf1);  //buf1 for error
		if (err != 0 && hw_err != 0)
		{
			display_prompt("81160A not responding to GPIB queries", true);
		}
		else
		{
			sprintf(str1, "%.38s", buf);
			//display_prompt("   34980A switchbox Okay", true);
			if (!strcmp(str1, "81160A"))
			{
				display_prompt(str1, true);
				display_prompt("  81160A pulse generator Okay", true);
				//return true;
			}
			else
			{
				display_prompt("   81160A pulse generator not connected", true);
				//return false;
			}
		}
		*/
	//}

}  /*open_gen*/



/* *********************************************************************** */

void disable_gen_out(int mod_id)

{

  vth_mhpib_wr(mod_id, 3, ":OUTP1 OFF");

}  /*disable_gen_out*/



/* *********************************************************************** */

void enable_gen_out(int mod_id)

{

  vth_mhpib_wr(mod_id, 3, ":OUTP1 ON");

}  /*enable_gen_out*/



/* *********************************************************************** */

void complement_gen_out(int mod_id)

{

  vth_mhpib_wr(mod_id, 3, ":OUTP1:POL INV");

}  /*complement_gen_out*/



/* *********************************************************************** */

void no_complement_gen_out(int mod_id)

{

  vth_mhpib_wr(mod_id, 3, ":OUTP1:POL NORM");

}  /*no_complement_gen_out*/



/* *********************************************************************** */

void setup_gen_mode(int mod_id, int mode, int slope)

{

  if (mode == NORM)

    vth_mhpib_wr(mod_id, 3, ":ARM:SOUR IMM");

  else  /*mode = triggered*/

  {  /* if */

    vth_mhpib_wr(mod_id, 3, ":ARM:SOUR EXT1");

    vth_mhpib_wr(mod_id, 3, ":ARM:LEV 0V");

   if (slope == POS)

      vth_mhpib_wr(mod_id, 3, ":ARM:SLOP POS");

  }  /* if */

}  /*setup_gen_mode*/



/* *********************************************************************** */

void setup_gen_levels(int mod_id, double low_level, double high_level)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, ":VOLT:LOW ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.2f", low_level);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, "V");

  vth_mhpib_wr(mod_id, 3, setup_string);



  *setup_string = '\0';

  strcat(setup_string, ":VOLT:HIGH ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.2f", high_level);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, "V");

  vth_mhpib_wr(mod_id, 3, setup_string);

}  /*setup_gen_levels*/



/* *********************************************************************** */

void setup_gen_pwidth(int mod_id, double pulse_width)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, ":PULS:WIDT1 ");

  current_position = mtd_strlen(setup_string) + 1;

  convert_seconds_nanoseconds(&pulse_width);

  sprintf(setup_string + current_position - 1, "%1.1f", pulse_width);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, "NS");

  vth_mhpib_wr(mod_id, 3, setup_string);

}  /*setup_gen_pwidth*/



/* *********************************************************************** */

void setup_gen_period(int mod_id, double gen_period)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, ":PULS:PER ");

  current_position = mtd_strlen(setup_string) + 1;

  convert_seconds_nanoseconds(&gen_period);

  sprintf(setup_string + current_position - 1, "%1.1f", gen_period);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, "NS");

  vth_mhpib_wr(mod_id, 3, setup_string);

}  /*setup_gen_period*/



/* *********************************************************************** */

void setup_gen_fixed_transition(int mod_id)

{

  vth_mhpib_wr(mod_id, 3, ":PULS:TRAN1 4.5NS");

}  /*setup_gen_fixed_transition*/



/* *********************************************************************** */

void setup_gen_transition_times(int mod_id, double tt)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, ":PULS:TRAN1 ");

  current_position = mtd_strlen(setup_string) + 1;

  convert_seconds_nanoseconds(&tt);

  sprintf(setup_string + current_position - 1, "%1.1f", tt);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, "NS");

  vth_mhpib_wr(mod_id, 3, setup_string);

}  /*setup_gen_transition_times*/



/* *********************************************************************** */

void setup_gen_trg_out_delay(int mod_id, double delay)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, ":PULS:DEL1 ");

  current_position = mtd_strlen(setup_string) + 1;

  convert_seconds_nanoseconds(&delay);



//mjs -- next line in for experimental purposes

//  delay -= 27;  //orig

//  delay -= 31;

  delay -= (get_gen_cal_delay(mod_id) * 1e9);

  sprintf(setup_string + current_position - 1, "%1.1f", delay);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, "NS");

  vth_mhpib_wr(mod_id, 3, setup_string);

}  /*setup_gen_trg_out_delay*/





/* *********************************************************************** */

void close_gen(int mod_id)

{

  vth_mhpib_close(mod_id, 3);

}  /*close_gen*/



/* wpd - include begins here */

/* Use tag 2 for the 3325A Synthesizer */

/* *********************************************************************** */

double get_gen_cal_delay(int mod_id)

{

  if (!gen_cal_counter)

  {

//  display_prompt("######### experimental ############", true);

  gen_cal_counter++;



/*  open_mux(mod_id);

  open_gen(mod_id);

  vth_mhpib_open(mod_id, 4, "/dev/sales.cntr");

  open_syn(mod_id);

  */

  connect_syn_out_gen_ext(mod_id);

  connect_gen_trg_cntr_start(mod_id);

  connect_gen_out_cntr_stop(mod_id);

  vth_mhpib_wr(mod_id, 3, ":PULS:DEL1 100NS");

  enable_gen_out(mod_id);

  trigger_cntr(mod_id);

  gen_cal_delay = read_cntr_mean(mod_id) - 100e-09;

/*close_mux(mod_id);

  close_gen(mod_id);

  close_cntr(mod_id);

  close_syn(mod_id);*/

  }



  return(gen_cal_delay);

}


/*Old codes for 3325B*/
///* ------------------------------------------------------------------------ */
//
void setup_syn(int mod_id, int wave, int frequency, double amplitude, double offset)

{
  char setup_string[81];

  int current_position;

  int next_position;

  if (wave == SINE)
  {
    /* SETUP THE SINE WAVE FUNCTION */
    vth_mhpib_wr(mod_id, 2, "FU1");
  }

  if (wave == SQUARE)
  {
    /* SETUP THE SQUARE WAVE FUNCTION */
    vth_mhpib_wr(mod_id, 2, "FU2");
  }

  if (wave == DC)
  {
    /* SETUP THE NO WAVEFORM FUNCTION */
    vth_mhpib_wr(mod_id, 2, "FU0");
  }

  /* ------------------------------------------------------------------- */
  /* Modified: 01.16.91 JRM -------------------------------------------- */

  /* Moved setup offset voltage to before p-p amplitude for our testing. */

  /* ------------------------------------------------------------------- */

  /* SETUP THE OFFSET VOLTAGE */

  *setup_string = '\0';
  strcat(setup_string, "OF");
  current_position = mtd_strlen(setup_string) + 1;
  if (offset >= 10.0)
  {
    sprintf(setup_string + current_position - 1, "%1.2f", offset);
    next_position = mtd_strlen(setup_string) + 1;
    strcat(setup_string, "VO");
  }

  else
  {
    if (offset >= 1.0)
    {
      sprintf(setup_string + current_position - 1, "%1.3f", offset);
      next_position = mtd_strlen(setup_string) + 1;
      strcat(setup_string, "VO");
    }

    else
    {
      if (offset >= 0.1)
      {
        sprintf(setup_string + current_position - 1, "%1.1f", offset * 1000);
        next_position = mtd_strlen(setup_string) + 1;
      }

	  else
      {
        sprintf(setup_string + current_position - 1, "%1.2f", offset * 1000);
        next_position = mtd_strlen(setup_string) + 1;
      }

      strcat(setup_string, "MV");
    }

  }

  vth_mhpib_wr(mod_id, 2, setup_string);

  /* ------------------------------------------------------------------- */

  /* Modified 01.16.91 JRM --------------------------------------------- */

  /* setup the minimum amplitude of 0.01 vac if anything less is entered */

  /* ------------------------------------------------------------------- */

  if (amplitude < 0.01)
    amplitude = 0.01;

  if (wave != DC)
  {  /*setup the peak to peak amplitude*/

    /* SETUP THE FREQUENCY */

    *setup_string = '\0';
    strcat(setup_string, "FR");
    current_position = mtd_strlen(setup_string) + 1;
    sprintf(setup_string + current_position - 1, "%d", frequency);
    next_position = mtd_strlen(setup_string) + 1;
    strcat(setup_string, "HZ");
    vth_mhpib_wr(mod_id, 2, setup_string);

    /* PEAK TO PEAK AMPLITUDE */
    *setup_string = '\0';
    strcat(setup_string, "AM");
    current_position = mtd_strlen(setup_string) + 1;
    if (amplitude >= 10.0)
    {
      sprintf(setup_string + current_position - 1, "%1.2f", amplitude);
      next_position = mtd_strlen(setup_string) + 1;
      strcat(setup_string, "VO");
    }
    else
    {
      if (amplitude >= 1.0)
      {
        sprintf(setup_string + current_position - 1, "%1.3f", amplitude);
        next_position = mtd_strlen(setup_string) + 1;
        strcat(setup_string, "VO");
      }
      else
      {
        if (amplitude >= 0.1)
        {
          sprintf(setup_string + current_position - 1,
                  "%1.1f", amplitude * 1000);
          next_position = mtd_strlen(setup_string) + 1;
        }
        else
        {
          sprintf(setup_string + current_position - 1,
                  "%1.2f", amplitude * 1000);
          next_position = mtd_strlen(setup_string) + 1;
        }
        strcat(setup_string, "MV");
      }
    }
    vth_mhpib_wr(mod_id, 2, setup_string);

  }  /*setup the peak to peak amplitude*/

}  /*setup_syn*/

/* ------------------------------------------------------------------------ */

void setup_syn_hv(int mod_id, int status)
{
  if (status == true)
    vth_mhpib_wr(mod_id, 2, "HV 1");
  else
    vth_mhpib_wr(mod_id, 2, "HV 0");

  vth_mtmr_wait(mod_id, .050); //mjs added to be safe

}  /*setup_syn_hv*/

/* ------------------------------------------------------------------------ */
void setup_syn_defaults(int mod_id)
{
  setup_syn_hv(mod_id, false);
  setup_syn(mod_id, SINE, 5000000, 1.0, 0.0);
}  /*setup_syn_defaults*/

/* ------------------------------------------------------------------------ */
void open_syn(int mod_id)
{
  test_result_type result;
  // SYNC UP THE PIPE   //mjs added this
  if (get_result_nolog(mod_id, &result))
    display_prompt("Failed to sync up pipe when opening syn", true);
    vth_mhpib_open(mod_id, 2, "/dev/sales.syn");
    setup_syn_defaults(mod_id);
}  /*open_syn*/

/* ------------------------------------------------------------------------ */

void setup_syn_wave(int mod_id, int wave)
{
  if (wave == SINE)
  {
    /* SETUP THE SINE WAVE FUNCTION */
    vth_mhpib_wr(mod_id, 2, "FU1");
  }
  else if (wave == DC)
  {
    /* SETUP THE NO WAVEFORM FUNCTION */
    vth_mhpib_wr(mod_id, 2, "FU0");
  }
}  /*setup_syn_wave*/

/* ------------------------------------------------------------------------ */
void setup_syn_freq(int mod_id, int frequency)
{
  char setup_string[81];
  int current_position;
  int next_position;

  /* SETUP THE FREQUENCY */
  *setup_string = '\0';
  strcat(setup_string, "FR");
  current_position = mtd_strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d", frequency);
  next_position = mtd_strlen(setup_string) + 1;
  strcat(setup_string, "HZ");
  vth_mhpib_wr(mod_id, 2, setup_string);
}  /*setup_syn_freq*/

/* ------------------------------------------------------------------------ */

void setup_syn_amp(int mod_id, double amplitude)
{
  char setup_string[81];
  int current_position;
  int next_position;

  /* PEAK TO PEAK AMPLITUDE */
  *setup_string = '\0';
  strcat(setup_string, "AM");
  current_position = mtd_strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.1f", amplitude);
  next_position = mtd_strlen(setup_string) + 1;
  strcat(setup_string, "VO");
  vth_mhpib_wr(mod_id, 2, setup_string);
}  /*setup_syn_amp*/

/* ------------------------------------------------------------------------ */
void setup_syn_off(int mod_id, double offset)
{
  char setup_string[81];
  int current_position;
  int next_position;

  /* SETUP THE OFFSET VOLTAGE */
  *setup_string = '\0';
  strcat(setup_string, "OF");
  current_position = mtd_strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.3f", offset);
  next_position = mtd_strlen(setup_string) + 1;
  strcat(setup_string, "VO");
  vth_mhpib_wr(mod_id, 2, setup_string);
}  /*setup_syn_off*/
/* ------------------------------------------------------------------------ */

void close_syn(int mod_id)
{
  setup_syn(mod_id, SINE, 0, 0.0, 0.0);
  vth_mhpib_close(mod_id, 2);
}  /*close_syn*/
/*------------------------------------------------------------------------*/
/*End of old codes for FG 3325B*/


/*New code for FG 33250A*/
/*------------------------------------------------------------------------*/
/* ------------------------------------------------------------------------ */
void setup_33250A(int mod_id, int wave, int frequency, double amplitude, double offset)
{
	setup_33250A_wave(mod_id, wave);
	setup_33250A_off(mod_id, offset);
	setup_33250A_freq(mod_id, frequency);
	setup_33250A_amp(mod_id, amplitude);

}  /*setup_33250A*/


/* ------------------------------------------------------------------------ */
void setup_33250A_hv(int mod_id, int status)
{
  char setup_string[81];
  *setup_string = '\0';

  if (status == true)
  {
    sprintf(setup_string,"OUTP:LOAD INF\n");
  }
  else
  {
    sprintf(setup_string,"OUTP:LOAD 50\n");
  }

  viPrintf(viSYNTH, setup_string);

  vth_mtmr_wait(mod_id, MEASUREMENT_WAIT); //added to wait for syn stable

}  /*setup_33250A_hv*/

/* ------------------------------------------------------------------------ */
void setup_33250A_defaults(int mod_id)
{
  setup_33250A(mod_id, SINE, 5000000, 1.0, 0.0);
}  /*setup_33250A_defaults*/

/* ------------------------------------------------------------------------ */
void open_33250A(int mod_id)

{
	err = viOpen(rsm, SYNTH, VI_NULL, VI_NULL, &viSYNTH );
	if( err != 0 )
	{
		display_prompt("Can't connect Function Generator!!!.", true);
	}
	viSetAttribute(viSYNTH, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viSYNTH, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viSYNTH, "*RST\n");
	//setup_33250A_defaults(mod_id);
}
 /*open_33250A*/

/* ------------------------------------------------------------------------ */
void setup_33250A_wave(int mod_id, int wave)
{
  char setup_string[81];
  *setup_string = '\0';

  if (wave == SINE)
  {
    /* SETUP THE SINE WAVE FUNCTION */
    sprintf(setup_string, "%s\n", "FUNC SIN ");
  }
  else if (wave == DC)
  {
    /* SETUP THE NO WAVEFORM FUNCTION */
    sprintf(setup_string, "%s\n", "FUNC DC ");
  }

  viPrintf(viSYNTH, setup_string);

}  /*setup_33250A_wave*/

/* ------------------------------------------------------------------------ */
void setup_33250A_freq(int mod_id, int frequency)
{
  /* SETUP THE FREQUENCY */
  char setup_string[81];
  *setup_string = '\0';

  sprintf(setup_string, "%s %d\n", "FREQ ", frequency);
  viPrintf(viSYNTH, setup_string);

}  /*setup_33250A_freq*/

/* ------------------------------------------------------------------------ */
void setup_33250A_amp(int mod_id, double amplitude)
{
  /* PEAK TO PEAK AMPLITUDE */
  char setup_string[81];
  *setup_string = '\0';

    if (amplitude >= 10.0)
      sprintf(setup_string, "%s %1.2f\n", "VOLT ", amplitude);
    else
    {
      if (amplitude >= 1.0)
        sprintf(setup_string, "%s %1.3f\n", "VOLT ", amplitude);
      else
      {
        if (amplitude >= 0.1)

          sprintf(setup_string, "%s %1.1f\n", "VOLT ", amplitude * 1000);
         else
          sprintf(setup_string,
                  "%s %1.2f\n", "VOLT ", amplitude * 1000);
	  }
	}
   viPrintf(viSYNTH, setup_string);
}
  /*setup_33250A_amp*/

/* ------------------------------------------------------------------------ */
void setup_33250A_off(int mod_id, double offset)
{
	char setup_string[81];
    *setup_string = '\0';

    if (offset >= 10.0)
      sprintf(setup_string, "%s %1.2f\n", "VOLT:OFFS ", offset);
    else
	{
	  if (offset >= 1.0)
        sprintf(setup_string, "%s %1.3f\n", "VOLT:OFFS ", offset);
    else
    {
      if (offset >= 0.1)
        sprintf(setup_string, "%s %1.1f\n", "VOLT:OFFS ", offset * 1000);
      else
        sprintf(setup_string, "%s %1.2f\n", "VOLT:OFFS ", offset * 1000);
    }
  }
    viPrintf(viSYNTH, setup_string);
}
  /*setup_33250A_off*/


/*Enable 33250A output*/
/* ------------------------------------------------------------------------ */
void enable_33250A_out(int mod_id)
{
	viPrintf(viSYNTH, "OUTP ON\n");
}

/*End of enable 33250A output*/
/* ------------------------------------------------------------------------ */


/*Disable 33250A output*/
/* ------------------------------------------------------------------------ */
void disable_33250A_out(int mod_id)
{
	viPrintf(viSYNTH, "OUTP OFF\n");
}

/*End of disable 33250A output*/
/* ------------------------------------------------------------------------ */

/*Close 33250A*/
/* ------------------------------------------------------------------------ */
void close_33250A(int mod_id)
{

	char buf1[256] = {0};
	err = viQueryf(viSYNTH, "*OPC?\n", "%t", &buf1);
	if (err != 0)
	{
		printf("GPIB command pending on 33250A!!!");

	}
	else
		viPrintf(viSYNTH, "*RST\n");
		viClose(viSYNTH);

}
/* ------------------------------------------------------------------------ */
/*End of close 33250A*/

/*------------------------------------------------------------------------*/
/*End of new codes for FG 33250A*/

/*Verify Synthesizer*/
/* ------------------------------------------------------------------------ */
void verify_syn(int mod_id)
{
	ViStatus  hw_err;
	char buf1[256] = {0};
	char str1[256] = {0};

	if (get_34980A_IDN() == false)
	{
		err = viQueryf(viSYNTH, "*IDN?", "%t", &buf);
		if (check_syn_model())
			hw_err = viQueryf(viSYNTH, "SYST:ERR?", "%t", &buf1);
		else
			hw_err = viQueryf(viSYNTH, "ERR?", "%t", &buf1);

		if (err != 0 && hw_err != 0)
		{
			display_prompt("Function Generator not responding to HPIB queries\n", true);
			exit(1);
		}

		else
		{
			sprintf(str1, "%.50s", buf);
			display_prompt(str1, true);
			display_prompt("   Function Generator Okay\n", true);
		}
	}
	else if (get_34980A_IDN() == true)
	{
		err = viQueryf(viSYNTH, "*IDN?", "%t", &buf);
		if (check_syn_model())
			hw_err = viQueryf(viSYNTH, "SYST:ERR?", "%t", &buf1);
		else
			hw_err = viQueryf(viSYNTH, "ERR?", "%t", &buf1);

		if (err != 0 && hw_err != 0)
		{
			display_prompt("Function Generator not responding to GPIB queries\n", true);
			exit(1);
		}

		else
		{
			sprintf(str1, "%.50s", buf);
			display_prompt(str1, true);
			display_prompt("33611A Function Generator Okay\n", true);
		}
	}

}
/*End of verify 33250A*/
/* ------------------------------------------------------------------------ */

/*Check Synthesizer Model*/
/* ------------------------------------------------------------------------ */

boolean check_syn_model(void)
{
	char str1[256] = {0};
	char syn_model[256] = {0};

	err = viQueryf(viSYNTH, "*IDN?", "%t", &syn_model);

    if(err != 0)
	{
		display_prompt("Error!!!",true);
		exit(1);
	}

	else
	{
		sprintf(str1, "%.07s", syn_model);
		if (!strcmp(str1, "HEWLETT"))
		{
			return false;
		}
		else if (!strcmp(str1, "Agilent"))
		{
			return true;
		}
		else
		{
			display_prompt("Function Generator model mismatch!!!",true);
			exit(1);
		}
	}
}

/*End Of Check Synthesizer Model*/
/* ------------------------------------------------------------------------ */


/* wpd - include begins here */

/* Use tag 6 for the LCR Meter */

/*------------------------------------------------------------------------*/
/*open_lcr*/ //4263B
void open_lcr(int mod_id)
{

  vth_mhpib_open(mod_id, 6, "/dev/sales.lcr");

}/*open_lcr*/ //4263B

/*open_E4980AL_LCR*/
void open_E4980AL_LCR(int mod_id)
{
	err = viOpen(rsm, LCR, VI_NULL, VI_NULL, &viLCR);
	if (err != 0)
	{
		display_prompt("Can't connect E4980AL LCR meter !!!.", true);
	}
	viSetAttribute(viLCR, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viLCR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viLCR, "*RST\n");
}
/*open_E4980AL_LCR*/

/*Verify E4980AL*/
boolean verify_E4980AL_LCR(int mod_id)
{
	ViStatus  hw_err;
	char buf1[256] = { 0 };
	char buf[256] = { 0 }; //create new buf quah 4/3/25
	char str1[256] = { 0 }; // increase str1 length l quah 4/3/25
	
	err = viQueryf(viLCR, "*IDN?", "%t", &buf);  //buf for IDN
	
	hw_err = viQueryf(viLCR, "SYST:ERR?", "%t", &buf1);  //buf1 for error
	
	if (err != 0 && hw_err != 0)
	{
		display_prompt("E4980AL LCR not responding to GPIB queries", true);
		return false; //fikri added 03032025
	}
	else
	{
	
		sprintf(str1, "%.48s", buf); //increase limit 48 character ; quah 4/3/25
		//display_prompt(str1, true);
		//if (!strcmp(str1, "Keysight Technologies,E4980AL,MY56485976"))//Keysight Technologies,E4980AL,MY54412306,B.07.05
		//if (!strcmp(str1, "Keysight Technologies,E4980AL,MY54412306,B.07.05"))//Keysight Technologies,E4980AL,MY54412306,B.07.05
		if (!strcmp(str1, "Keysight Technologies,E4980AL,MY54412306,B.07.05"))//Keysight Technologies,E4980AL,MY54412306,B.07.05, remove /n quah 4/3/25
		{
	
			display_prompt(str1, true);
			
			display_prompt("   E4980AL LCR Okay", true); 
			return true;
		}

		else if (!strcmp(str1, "Keysight Technologies,E4980AL,MY54412993,B.07.05"))//Keysight Technologies,E4980AL,MY54412306,B.07.05, remove /n quah 4/3/25
		{
			display_prompt(str1, true);
			display_prompt("   E4980AL LCR Okay", true);
			return true;
		}

		else if (!strcmp(str1, "Keysight Technologies,E4980AL,MY54413996,B.07.05"))//Keysight Technologies,E4980AL,MY54412306,B.07.05, remove /n quah 4/3/25
		{
			display_prompt(str1, true);
			display_prompt("   E4980AL LCR Okay", true);
			return true;
		}

		else if (!strcmp(str1, "Keysight Technologies,E4980AL,MY54415718,B.07.05"))//Keysight Technologies,E4980AL,MY54412306,B.07.05, remove /n quah 4/3/25
		{
			display_prompt(str1, true);
			display_prompt("   E4980AL LCR Okay", true);
			return true;
		}


		else
		{
			display_prompt("   E4980AL LCR not connected", true);
			return false;
		}
	}

}
/*End of verify E4980AL*/

/*------------------------------------------------------------------------*/

void setup_lcr(int mod_id, int lcr_function, int lcr_display,

               int lcr_ckt_mode, double lcr_freq, int lcr_mult)

{

  char setup_string[81];



  *setup_string = '\0';

/************************************************************************/



/*if (lcr_function == INDUCTANCE)

    strcat(setup_string, "A1");

  else  // must be capacitance

    strcat(setup_string, "A2");



  if (lcr_display == Q)

    strcat(setup_string, "B2");



  if (lcr_ckt_mode == SERIES)

    strcat(setup_string, "C2");

  else  // must be parallel

    strcat(setup_string, "C3");



  if (lcr_freq == 100000)

    strcat(setup_string, "F21");*/



  if (lcr_function == INDUCTANCE)

  if (lcr_ckt_mode == SERIES)

  {
	//SELECT LS-D MODE
     //ori vth_mhpib_wr(mod_id, 6,":SENS:FUNC 'FIMP';");
     //ori vth_mhpib_wr(mod_id, 6,":CALC1:FORM LS;");
     vth_mhpib_wr(mod_id, 6,"::FUNCtion:IMPedance:TYPE LSQ");
 	if (lcr_display == Q)

    //ori vth_mhpib_wr(mod_id, 6,":CALC2:FORM Q;");
    vth_mhpib_wr(mod_id, 6,":FUNCtion:IMPedance:TYPE LSQ");
 }
  else

  {
	//SELECT LP-D MODE
    //ori vth_mhpib_wr(mod_id, 6,":SENS:FUNC 'FADM';");
    //ori vth_mhpib_wr(mod_id, 6,":CALC1:FORM LP;");
	vth_mhpib_wr(mod_id, 6,":FUNCtion:IMPedance:TYPE LPQ");


  }

  else  /* must be capacitance */

  if (lcr_ckt_mode == SERIES)

  {
    //SELECT CS-D MODE
    //ori vth_mhpib_wr(mod_id, 6,":SENS:FUNC 'FIMP';");
    //ori vth_mhpib_wr(mod_id, 6,":CALC1:FORM CS;");
	vth_mhpib_wr(mod_id, 6,":FUNCtion:IMPedance:TYPE CSQ");

  }

  else

  {
    //SELECT CP-D MODE
    //ori vth_mhpib_wr(mod_id, 6,":SENS:FUNC 'FADM';");
    //ori vth_mhpib_wr(mod_id, 6,":CALC1:FORM CP;");
	vth_mhpib_wr(mod_id, 6,":FUNCtion:IMPedance:TYPE CPQ");

	if (lcr_display == Q)

    //ori vth_mhpib_wr(mod_id, 6,":CALC2:FORM Q;");
    vth_mhpib_wr(mod_id, 6,":FUNCtion:IMPedance:TYPE CPQ");


  }


  if (lcr_freq == 100000)

    //ori vth_mhpib_wr(mod_id, 6,":SOUR:FREQ:CW 100000 HZ;");
    vth_mhpib_wr(mod_id, 6,":FREQuency:CW 100000 HZ");
/************************************************************************/



/*  no high resolution function on new lcr.       */

//strcat(setup_string, "H1");    High resolution



  if (lcr_mult == 5)

/*  strcat(setup_string, "M4");*/

/* Set test signal level to the maximum.*/

    //ori vth_mhpib_wr(mod_id, 6,":SOUR:VOLT 1;");
    vth_mhpib_wr(mod_id, 6,":VOLTage:LEVel 1 V");


//strcat(setup_string, "R31");   /* Auto range */

  //ori vth_mhpib_wr(mod_id, 6,":SENS:FIMP:RANG:AUTO ON;");   /* Auto range */
    vth_mhpib_wr(mod_id, 6,":FUNCtion:IMPedance:RANGe:AUTO 1");   /* Auto range */



//strcat(setup_string, "T3");   /* Manual trigger / HOLD */

//ori vth_mhpib_wr(mod_id, 6,":TRIG:SOUR MAN;");   /* Manual trigger / HOLD */
 vth_mhpib_wr(mod_id, 6,":TRIGger:SOURce HOLD;");   /* Manual trigger / HOLD */



}  /*setup_lcr*/



/*setup_E4980AL_LCR*/
void setup_E4980AL_LCR(int mod_id, int lcr_function, int lcr_display,
	                   int lcr_ckt_mode, double lcr_freq, int lcr_mult)
{

	char setup_string[81];



	*setup_string = '\0';

	/************************************************************************/



	/*if (lcr_function == INDUCTANCE)

		strcat(setup_string, "A1");

	  else  // must be capacitance

		strcat(setup_string, "A2");



	  if (lcr_display == Q)

		strcat(setup_string, "B2");



	  if (lcr_ckt_mode == SERIES)

		strcat(setup_string, "C2");

	  else  // must be parallel

		strcat(setup_string, "C3");



	  if (lcr_freq == 100000)

		strcat(setup_string, "F21");*/



	if (lcr_function == INDUCTANCE)
	{
		if (lcr_ckt_mode == SERIES)
		{

			//vth_mhpib_wr(mod_id, 6, ":SENS:FUNC 'FIMP';");
			//vth_mhpib_wr(mod_id, 6, ":CALC1:FORM LS;");

			sprintf(setup_string, "%s\n", ":FUNCtion:IMPedance:TYPE LSQ");//:FUNC:IMP:TYPE LSQ
			viPrintf(viLCR, setup_string);

		}

		else //parallel
		{

			//vth_mhpib_wr(mod_id, 6, ":SENS:FUNC 'FADM';");

			//vth_mhpib_wr(mod_id, 6, ":CALC1:FORM LP;");

			sprintf(setup_string, "%s\n", ":FUNCtion:IMPedance:TYPE LPQ");//:FUNC:IMP:TYPE LSD
			viPrintf(viLCR, setup_string);

		}
	}
	else  /* must be capacitance */
	{
		if (lcr_ckt_mode == SERIES)
		{

			//vth_mhpib_wr(mod_id, 6, ":SENS:FUNC 'FIMP';");

			//vth_mhpib_wr(mod_id, 6, ":CALC1:FORM CS;");

			sprintf(setup_string, "%s\n", ":FUNCtion:IMPedance:TYPE CSQ");//:FUNC:IMP:TYPE LSD
			viPrintf(viLCR, setup_string);

		}

		else
		{

			//vth_mhpib_wr(mod_id, 6, ":SENS:FUNC 'FADM';");

			//vth_mhpib_wr(mod_id, 6, ":CALC1:FORM CP;");
			//sprintf(setup_string, "%s\n", "*RST");//:FUNC:IMP:TYPE LSD
			//viPrintf(viLCR, setup_string);
			sprintf(setup_string, "%s\n", ":FUNCtion:IMPedance:TYPE CPQ");//:FUNC:IMP:TYPE LSD
			viPrintf(viLCR, setup_string);
		}

	}

	if (lcr_display == Q)
	{
		//vth_mhpib_wr(mod_id, 6, ":CALC2:FORM Q;");
		sprintf(setup_string, "%s\n", ":FUNCtion:IMPedance:TYPE CPQ");//:FUNC:IMP:TYPE LSD
		viPrintf(viLCR, setup_string);
	}


	if (lcr_freq == 100000)
	{
		//vth_mhpib_wr(mod_id, 6, ":SOUR:FREQ:CW 100000 HZ;");
		sprintf(setup_string,"%s\n",":FREQuency:CW 100000 HZ");
		viPrintf(viLCR, setup_string);

	}
	/************************************************************************/



	/*  no high resolution function on new lcr.       */

	//strcat(setup_string, "H1");    High resolution



	if (lcr_mult == 5)
	{
		/*  strcat(setup_string, "M4");*/

		/* Set test signal level to the maximum.*/

		//vth_mhpib_wr(mod_id, 6, ":SOUR:VOLT 1;");

		sprintf(setup_string,"%s\n",":VOLTage:LEVel 1 V");
		viPrintf(viLCR,setup_string);

	}


	//strcat(setup_string, "R31");   /* Auto range */

	//vth_mhpib_wr(mod_id, 6, ":SENS:FIMP:RANG:AUTO ON;");   /* Auto range */

	sprintf(setup_string, "%s\n", ":FUNCtion:IMPedance:RANGe:AUTO 1");
	viPrintf(viLCR, setup_string);



  //strcat(setup_string, "T3");   /* Manual trigger / HOLD */

	//vth_mhpib_wr(mod_id, 6, ":TRIG:SOUR MAN;");   /* Manual trigger / HOLD */

	sprintf(setup_string,"%s\n",":TRIGger:SOURce HOLD;");
	viPrintf(viLCR,setup_string);



}  /*setup_lcr*/

/*------------------------------------------------------------------------*/

void measure_lcr(int mod_id)

{

//vth_mhpib_wr(mod_id, 6, "E");   /* E is for execute */

  vth_mhpib_wr(mod_id, 6, ":TRIG");   /* Trigger */

  vth_mhpib_wr(mod_id, 6, ":FETC?");   /* FETC? is for execute */

  vth_mhpib_rd(mod_id, 6);

}  /*measure_lcr*/

double measure_E4980AL_LCR(int mod_id)
{
	char setup_string[81];
	double E4980AL_LCR_fetch_result;
	char str1[256] = { 0 };
	char str2[256] =  { 0 };
	char str3[256]  = { 0 };
	char str4[256] =  { 0 };

	*setup_string = '\0';

	sprintf(setup_string, "%s\n", "*OPC?");//(setup_string, "%s %1.3f %s\n", "VOLT", amplitude, ", (@2)");
    err = viQueryf(viLCR, setup_string, "%t",&buf);

	if (err != 0)
	{
		printf("GPIB command pending on E4980AL LCR!!!");
	}
	else
	{
		*setup_string = '\0';

		//sprintf(setup_string,"%s\n",":TRIGger:SOURce HOLD");
	    //viPrintf(viLCR,setup_string);

		//sprintf(setup_string, "%s\n", ":FORMat:BORDer SWAP");
		//viPrintf(viLCR, setup_string);

		//sprintf(setup_string, "%s\n", ":FORMat:DATA REAL");
		//viPrintf(viLCR, setup_string);

		//sprintf(setup_string, "%s\n", ":FETCh:IMPedance:FORMatted?");
		sprintf(setup_string,"%s\n",":FETC?");
		err = viQueryf(viLCR, setup_string, "%t", &buf);
		if (err != 0)
		{
			printf("GPIB command pending on E4980AL LCR!!!");
		}
		else if(err==0)
		{
			sprintf(str1, "%.50s", buf);
			str1[28] = '\0';
			int x = 0;
			char *ptr;
			ptr = strtok(str1,",");
			while(ptr != NULL)
			{
				if (x==0)
				{
					strcat(str2,ptr);
					strcat(str2,",");
				}
				if (x==1)
				{
					strcat(str3,ptr);
				}
				if (x==2)
				{
					strcat(str4,ptr);
					strcat(str4,",");
					strcat(str4,str2);
        			strcat(str4,str3);
				}
				ptr = strtok(NULL,",");
     			x++;
			}

			E4980AL_LCR_fetch_result = atof(str2);//(str4);

			//E4980AL_LCR_fetch_result = atof(str1);
			return E4980AL_LCR_fetch_result;
		}

	}

}

/*------------------------------------------------------------------------*/

void close_lcr(int mod_id)

{

  /* ------------------------------------------------------------------------ */

  /* Modified 01.15.91 JRM -------------------------------------------------- */

  /* Added M1 setup in close to set the source to the smallest possible value */

  /* and to keep it from screwing up the minerva backplane.                   */

  /* Modified 01.18.91 JRM -------------------------------------------------- */

  /* Added T3 to keep it from sitting there triggering (Trigger HOLD) ------- */

  /* ------------------------------------------------------------------------ */

  /* Modified 01.29.91 JRM -------------------------------------------------- */

  /* Added F11 to set the frequency to 100Hz -------------------------------- */

  /* ------------------------------------------------------------------------ */

/*vth_mhpib_wr(mod_id, 6, "A2B1C2H0I0S0M1F11R11");*/

  vth_mhpib_wr(mod_id, 6, "SENS:FUNC 'FIMP'");

  vth_mhpib_wr(mod_id, 6, "CALC1:FORM CS");

  vth_mhpib_wr(mod_id, 6, "CALC2:FORM Q");

  vth_mhpib_wr(mod_id, 6, "SOUR:VOLT .005");

  vth_mhpib_wr(mod_id, 6, "SOUR:FREQ:CW 100000 HZ");

  vth_mhpib_wr(mod_id, 6, "SENS:FIMP:RANG 1E-3");

  vth_mhpib_wr(mod_id, 6, "TRIG:SOUR MAN");

/*vth_mhpib_wr(mod_id, 6, "T3");*/

  vth_mhpib_close(mod_id, 6);

}  /*close_lcr*/

/*close_E4980AL_LCR*/
void close_E4980AL_LCR(int mod_id)
{
	char buf1[256] = { 0 };
	err = viQueryf(viLCR, "*OPC?\n", "%t", &buf1);
	if (err != 0)
	{
		printf("GPIB command pending on E4980AL!!!");

	}
	else
	{
		viPrintf(viLCR, "*RST\n");
	}
	viClose(viLCR);
}
/*close_E4980AL_LCR*/

/* wpd - include begins here */

/* Use tag 4 for the Time Interval Counter */

/* ---------------------------------------------------------------------- */

void setup_cntr_defaults(int mod_id)

{  /*setup_cntr_power_up_state*/

  /* preset 1 defined & stored in t9000.p */

  vth_mhpib_wr(mod_id, 4, ":ABOR");

  vth_mhpib_wr(mod_id, 4, "*RST");

  vth_mhpib_wr(mod_id, 4, "*CLS;*SRE 0;*ESE 0;:STAT:PRES");

  vth_mhpib_wr(mod_id, 4, ":INP1:COUP DC");   // input coupling = DC

  vth_mhpib_wr(mod_id, 4, ":INP2:COUP DC");   // input coupling = DC

  vth_mhpib_wr(mod_id, 4, ":INP1:IMP 50");   // input impedance 50 ohms

  vth_mhpib_wr(mod_id, 4, ":INP2:IMP 50");   // input impedance 50 ohms

  setup_cntr_function(mod_id, TIME_INTERVAL);   // to match old 5370 cntr

  setup_cntr_trg_levels(mod_id, 1.3, 1.3);

  setup_cntr_gate_time(mod_id, 0.0);

  vth_mhpib_wr(mod_id, 4, "*SAV 1");   // save preset 1



  vth_mhpib_wr(mod_id, 4, "*RST");

  vth_mhpib_wr(mod_id, 4, "*CLS");

  vth_mhpib_wr(mod_id, 4, "*RCL 1");



}  /*setup_cntr_power_up_state*/



/* ---------------------------------------------------------------------- */

void open_cntr(int mod_id)// used in old counter 53132A

{

  vth_mhpib_open(mod_id, 4, "/dev/sales.cntr");

  setup_cntr_defaults(mod_id);

}  /*open_cntr*/

void open_53230A(int mod_id)// used in new counter 53230A
{
	err = viOpen(rsm, CNTR, VI_NULL, VI_NULL, &viCNTR);
	if (err != 0)
	{
		display_prompt("Can't connect universal counter 53230A!!!.", true);
	}
	viSetAttribute(viCNTR, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viCNTR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viCNTR, "*RST\n");
}

void close_53230A(int mod_id)
{

	char buf1[256] = { 0 };
	err = viQueryf(viCNTR, "*OPC?\n", "%t", &buf1);
	if (err != 0)
	{
		printf("GPIB command pending on 53230A!!!");

	}
	else
	{
		viPrintf(viCNTR, "*RST\n");
	}
	viClose(viCNTR);

}
/* ---------------------------------------------------------------------- */

/*Verify 53230A counter*/
boolean verify_53230A_counter(int mod_id)
{
	ViStatus  hw_err;
	char buf1[256] = { 0 };
	char str1[256] = { 0 };

	err = viQueryf(viCNTR, "*IDN?", "%t", &buf);  //buf for IDN
	hw_err = viQueryf(viCNTR, "SYST:ERR?", "%t", &buf1);  //buf1 for error
	if (err != 0 && hw_err != 0)
	{
		display_prompt("53230A counter not responding to GPIB queries", true);
		return false;  //fikri added 03032025
	}
	else
	{
		sprintf(str1, "%.38s", buf);
		if (!strcmp(str1, "Keysight Technologies,53230A,MY56485976"))
		{
			display_prompt(str1, true);
			display_prompt("   53230A counter Okay", true);
			return true;
		}

		else if (!strcmp(str1, "Keysight Technologies,53230A,MY61400282"))
		{
			display_prompt(str1, true);
			display_prompt("   53230A counter Okay", true);
			return true;
		}

		else if (!strcmp(str1, "Keysight Technologies,53230A,MY61410357"))
		{
			display_prompt(str1, true);
			display_prompt("   53230A counter Okay", true);
			return true;
		}

		else if (!strcmp(str1, "Keysight Technologies,53230A,MY62310297"))
		{
			display_prompt(str1, true);
			display_prompt("   53230A counter Okay", true);
			return true;
		}

		else if (!strcmp(str1, "Keysight Technologies,53230A,MY63260275"))
		{
			display_prompt(str1, true);
			display_prompt("   53230A counter Okay", true);
			return true;
		}
		else
		{
			display_prompt("   53230A counter not connected", true);
			return false;
		}
	}

}
/*End of verify 53230A*/

void setup_cntr_trg_levels(int mod_id, double start_level, double stop_level)

{

  char setup_string[81];

  int current_position;

  int next_position;



  *setup_string = '\0';

  strcat(setup_string, ":SENSE:EVENT1:LEVEL:ABS ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.2f", start_level);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 4, setup_string);



  *setup_string = '\0';

  strcat(setup_string, ":SENSE:EVENT2:LEVEL:ABS ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.2f", stop_level);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 4, setup_string);



}  /*setup_cntr_trg_levels*/



/* ---------------------------------------------------------------------- */

void setup_cntr_trigger_slopes(int mod_id, int start_slope, int stop_slope)

{

  if (start_slope == POS)   /*Start on positive slope*/

    vth_mhpib_wr(mod_id, 4, ":SENSE:EVENT1:SLOPE POS");

  else  /*slope = neg*/

    vth_mhpib_wr(mod_id, 4, ":SENSE:EVENT1:SLOPE NEG");

  /*Start on negative slope*/

  if (stop_slope == POS)   /*Stop on positive slope*/

    vth_mhpib_wr(mod_id, 4, ":SENSE:EVENT2:SLOPE POS");

  else  /*slope = neg*/

    vth_mhpib_wr(mod_id, 4, ":SENSE:EVENT2:SLOPE NEG");

  /*Stop on negative slope*/

}  /*setup_cntr_trigger_slopes*/



/* ---------------------------------------------------------------------- */

void setup_cntr_function(int mod_id, int cntr_function)

{

  if (cntr_function == TIME_INTERVAL)

    vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'TINT'");

  else

  {

    if (cntr_function == FREQUENCY)

      vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'FREQ 2'");

    else

    {

      if (cntr_function == PERIOD)

        vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'PER 2'");

      else

      {

        if (cntr_function == PWID)

          vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'Pwidth'");

        else

        {

          if (cntr_function == NWID)

            vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'Nwidth'");

          else

          {

            if (cntr_function == DCYCLE)

              vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'Dcycle'");

          }

        }

      }

    }

  }





  setup_cntr_sample_size(mod_id, 10);   /* enable math/averaging */

}  /*setup_cntr_function*/



/* ---------------------------------------------------------------------- */

void setup_cntr_gate_time(int mod_id, double gate_time)

{

  vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:START:SOUR IMM");

  if (gate_time == 0.0)

    vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:STOP:SOUR IMM");

  else

  {

    if ( number_equal(gate_time, 0.01) )

    {

      vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:STOP:SOUR TIM");

      vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:STOP:TIM 0.01");

    }

    else

    {

      if ( number_equal(gate_time, 0.1) )

      {

        vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:STOP:SOUR TIM");

        vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:STOP:TIM 0.1");

      }

      else

      {

        vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:STOP:SOUR TIM");

        vth_mhpib_wr(mod_id, 4, ":FREQ:ARM:STOP:TIM 1.0");

      }

    }

  }

}  /*setup_cntr_gate_time*/



/* ---------------------------------------------------------------------- */

void setup_cntr_sample_size(int mod_id, int samples)

{

  char setup_string[81];

  int current_position;

  int next_position;



  vth_mhpib_wr(mod_id, 4, ":CALC3:AVERAGE:TYPE MEAN");

  vth_mhpib_wr(mod_id, 4, ":CALC3:AVERAGE ON");   /*init takes block of vth_readings*/

  if (samples == 1)

    vth_mhpib_wr(mod_id, 4, ":CALC3:AVERAGE OFF");

  else

  {

    *setup_string = '\0';

    strcat(setup_string, ":CALC3:AVERAGE:COUNT ");

    current_position = mtd_strlen(setup_string) + 1;

    sprintf(setup_string + current_position - 1, "%12d", samples);

    next_position = mtd_strlen(setup_string) + 1;

    vth_mhpib_wr(mod_id, 4, setup_string);

  }

  vth_mhpib_wr(mod_id, 4, ":TRIG:COUNT:AUTO ON");

      /*init takes block of readings*/

  vth_mhpib_wr(mod_id, 4, "*ESE 1");

}  /*setup_cntr_sample_size*/



/* ---------------------------------------------------------------------- */

void setup_cntr_md4(int mod_id)

{

  vth_mhpib_wr(mod_id, 4, "DISP:ENABLE OFF");

}



/* ---------------------------------------------------------------------- */

void setup_cntr_md1(int mod_id)

{

  vth_mhpib_wr(mod_id, 4, "DISP:ENABLE ON");

}



/* ---------------------------------------------------------------------- */

void trigger_cntr(int mod_id)

{

  double cntr_ready;



  vth_mtmr_wait(mod_id, 0.050);

  vth_mhpib_wr(mod_id, 4, ":INIT:IMM");

  vth_mhpib_wr(mod_id, 4, "*OPC?");

  vth_mhpib_rd(mod_id, 4);
//	return (convert_logged_string_to_numeric(mod_id));

  cntr_ready = convert_logged_string_to_numeric(mod_id);
}  /*trigger_cntr*/



/* ---------------------------------------------------------------------- */

void trigger_md4(int mod_id, int cntr_function)

{



  /* ---------------------------------------------------------------------- */

  /* In the MD4 mode of operation, a trigger for the counter is initiated   */

  /* by the function command.  Therefore, issuing a FNx initiates a measure.*/

  /* ---------------------------------------------------------------------- */

  setup_cntr_function(mod_id, cntr_function);

  trigger_cntr(mod_id);

}  /*trigger_MD4*/



/* ---------------------------------------------------------------------- */

double read_cntr(int mod_id)
{
	//vth_mhpib_wr(mod_id, 4, ":FETCH?");
    vth_mhpib_wr(mod_id, 4, ":FETCh?");  //new fikri 23/07/2024
	vth_mhpib_rd(mod_id, 4);

	return (convert_logged_string_to_numeric(mod_id));

}  /*read_cntr*/



/* ---------------------------------------------------------------------- */

double read_cntr_mean(int mod_id)

{

  vth_mhpib_wr(mod_id, 4, ":CALC3:AVERAGE:TYPE MEAN;:CALC3:DATA?");

  vth_mhpib_rd(mod_id, 4);

  return (convert_logged_string_to_numeric(mod_id));

}  /*read_cntr_mean*/



/* ---------------------------------------------------------------------- */

double read_cntr_sigma(int mod_id)

{

  vth_mhpib_wr(mod_id, 4, ":CALC3:AVERAGE:TYPE SDEV;:CALC3:DATA?");

  vth_mhpib_rd(mod_id, 4);

  return (convert_logged_string_to_numeric(mod_id));

}  /*read_cntr_sigma*/



/* ---------------------------------------------------------------------- */

double read_cntr_min(int mod_id)

{

  vth_mhpib_wr(mod_id, 4, ":CALC3:AVERAGE:TYPE MIN;:CALC3:DATA?");

  vth_mhpib_rd(mod_id, 4);

  return (convert_logged_string_to_numeric(mod_id));

}  /*read_cntr_min*/



/* ---------------------------------------------------------------------- */

double read_cntr_max(int mod_id)

{

  vth_mhpib_wr(mod_id, 4, ":CALC3:AVERAGE:TYPE MAX;:CALC3:DATA?");

  vth_mhpib_rd(mod_id, 4);

  return (convert_logged_string_to_numeric(mod_id));

}  /*read_cntr_max*/





/* ---------------------------------------------------------------------- */



#define TIMEOUT	1.0e-03



void setup_cntr_max_volt(int mod_id)

{

	vth_mhpib_wr(mod_id, 4, ":CONF:MAX");

	vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'VOLT:MAX'");

	vth_mhpib_wr(mod_id, 4, ":INIT:IMM");

	vth_mtmr_wait(mod_id, TIMEOUT);

}



#undef TIMEOUT



/* ---------------------------------------------------------------------- */



#define TIMEOUT	1.0e-03



void setup_cntr_min_volt(int mod_id)

{

	vth_mhpib_wr(mod_id, 4, ":CONF:MIN");

	vth_mhpib_wr(mod_id, 4, ":SENSE:FUNC 'VOLT:MIN'");

	vth_mhpib_wr(mod_id, 4, ":INIT:IMM");

	vth_mtmr_wait(mod_id, TIMEOUT);

}



#undef TIMEOUT



/* ---------------------------------------------------------------------- */



#define MARK_NUMBER     0

#define MAX_TIME        30.0e+00



void wait_for_counter_measurement(int mod_id)

{

  double time;

  boolean ready;

  test_result_type result;



  vth_mtmr_set_mark(mod_id, MARK_NUMBER);

  do

  {

    ready = counter_measurement_pending(mod_id);

    vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUMBER);

    if (get_result_nolog(mod_id, &result))

      time = result.UU.Real_result;

    else

    {

      time = MAX_TIME;

      display_prompt("No result in result log (Expected mark time)", true);

    }

  } while (!(ready || time >= MAX_TIME));

}  /*wait_for_counter_measurement*/



#undef MARK_NUMBER

#undef MAX_TIME



/* ---------------------------------------------------------------------- */

void close_cntr(int mod_id)

{

  vth_mhpib_wr(mod_id, 4, "*CLS");

  vth_mhpib_wr(mod_id, 4, "*RST");

  vth_mhpib_close(mod_id, 4);

}  /*close_cntr*/



/* ---------------------------------------------------------------------- */

/* wpd - include begins here */

/* Use tag 7 for the 6038A(NOW CHANGED TO 6634B) Power Supply */

/* ------------------------------------------------------------------------ */

void open_ps(int mod_id)//6634B or N6746B
{

		vth_mhpib_open(mod_id, 7, "/dev/sales.ps");

		vth_mhpib_wr(mod_id, 7, "CLR");  //old_ps
    //    vth_mhpib_wr(mod_id, 7, "*RST"); //new_ps  fikri 28-dec-22

		//vth_mhpib_wr(mod_id, 7, "OUT OFF");//sri

		vth_mhpib_wr(mod_id, 7, "OUT 0"); //old_ps
    //    vth_mhpib_wr(mod_id, 7, "OUTP OFF,(@2)"); // New_PS


}  /*open_ps*/

void open_PS_N6746B(int mod_id)
{
	char setup_string[81];
	*setup_string = '\0';

	err = viOpen(rsm, PS_N6746B, VI_NULL, VI_NULL, &viPS);

	if (err != 0)
	{
		display_prompt("Can't connect N6746B", true);
	}

	viSetAttribute(viPS, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viPS, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	//viPrintf(viPS, "*RST\n");

	sprintf(setup_string, "%s\n","*RST");
	viPrintf(viPS, setup_string);

}

void close_PS_N6746B(int mod_id)
{
//fikri03032025	char setup_string[81];
	char buf1[256]={0};

	//*setup_string = '\0';

	//sprintf(setup_string, "%s\n", "*OPC?");
	//err = viQueryf(viPS, setup_string, &buf);

	err = viQueryf(viPS, "*OPC?\n", "%t", &buf1);
	if (err != 0)
	{
		printf("GPIB command pending on N6746B!!!");

	}
	else
	{
		//*setup_string = '\0';
		//sprintf(setup_string, "%s\n", "*RST");
		//viPrintf(viPS, setup_string);

		viPrintf(viPS, "*RST\n");
	}
	viClose(viPS);
}

boolean verify_PS_N6746B(int mod_id)
{
	ViStatus  hw_err;
	char buf1[256] = { 0 };
	char str1[256] = { 0 };

	err = viQueryf(viPS, "SYST:CHAN:MOD? (@2)", "%t", &buf); //"*IDN?", "%t", &buf);  //buf for IDN
	hw_err = viQueryf(viPS, "SYST:ERR?", "%t", &buf1);  //buf1 for error
	if (err != 0 && hw_err != 0)
	{
		display_prompt("N6746B not responding to GPIB queries", true);
	}
	else
	{
		sprintf(str1, "%.38s", buf);
		if (!strcmp(str1, "N6746B\n"))//"Keysight Technologies"))
		{
			display_prompt(str1, true);
			display_prompt("   N6746B Okay\n", true);
			return true;
		}
		else
		{
			display_prompt("   N6746B not connected\n", true);
			return false;
		}
	}

}

#define MAX_VOLTS_WITH_PS_LOADED  10.0e+00

/* ------------------------------------------------------------------------ */

void setup_ps(int mod_id, double volts, double amps)

{  /*setup_ps_voltage*/

		char setup_string[81];

		int current_position;

		int next_position;

		//aconnect_ps_20_ohm_load(mod_id, true);

		if (fabs(volts) > MAX_VOLTS_WITH_PS_LOADED)
		{

			//vth_mhpib_wr(mod_id, 7, "OUT OFF");//

			vth_mhpib_wr(mod_id, 7, "OUT 0");

			aconnect_ps_20_ohm_load(mod_id, false);

		}

		//vth_mhpib_wr(mod_id, 7, "DLY 0S");//

		vth_mhpib_wr(mod_id, 7, "DLY 0.00");

		*setup_string = '\0';

		strcat(setup_string, "VSET");

		current_position = mtd_strlen(setup_string) + 1;

		sprintf(setup_string + current_position - 1, "%1.3f", fabs(volts));

		next_position = mtd_strlen(setup_string) + 1;

		//strcat(setup_string, "V");//sri

		vth_mhpib_wr(mod_id, 7, setup_string);

		*setup_string = '\0';

		strcat(setup_string, "ISET");

		current_position = mtd_strlen(setup_string) + 1;

		sprintf(setup_string + current_position - 1, "%1.2f", amps);

		next_position = mtd_strlen(setup_string) + 1;

		//strcat(setup_string, "A");//sri

		vth_mhpib_wr(mod_id, 7, setup_string);

		////Sri add the OCP August 20th 2009////

		//vth_mhpib_wr(mod_id, 7, "OCP 1");

		///////////////////////////////////////

	   // vth_mhpib_wr(mod_id, 7, "OUT ON");//sri

		vth_mhpib_wr(mod_id, 7, "OUT 1");
	//}

}  /*setup_ps_voltage*/

void setup_PS_N6746B(int mod_id, double volts, double amps)
{
	/*else if (get_34980A_IDN() == true)
	{*/
	char setup_string[81];
	int current_position;
	int next_position;
	//aconnect_ps_20_ohm_load(mod_id, true);

	if (fabs(volts) > MAX_VOLTS_WITH_PS_LOADED)
	{
		//vth_mhpib_wr(mod_id, 7, "OUT OFF");//
		setup_N6746B_ch2_output(0);//set N6746B output off //chun 20052022
		//vth_mhpib_wr(mod_id, 7, "OUT 0");
		aconnect_ps_20_ohm_load(mod_id, false);
	}

	//vth_mhpib_wr(mod_id, 7, "DLY 0S");//
	setup_N6746B_ch2_delay(0.00);
	//vth_mhpib_wr(mod_id, 7, "DLY 0.08");

	*setup_string = '\0';
	//strcat(setup_string, "VSET");
	//current_position = mtd_strlen(setup_string) + 1;
	//sprintf(setup_string + current_position - 1, "%1.3f", fabs(volts));
	//next_position = mtd_strlen(setup_string) + 1;
	////strcat(setup_string, "V");//sri
	//vth_mhpib_wr(mod_id, 7, setup_string);

	sprintf(setup_string, "%s %1.3f %s\n", "VOLT ", fabs(volts), ",(@2)");
	viPrintf(viPS, setup_string);

	*setup_string = '\0';
	//strcat(setup_string, "ISET");
	//current_position = mtd_strlen(setup_string) + 1;
	//sprintf(setup_string + current_position - 1, "%1.2f", amps);
	//next_position = mtd_strlen(setup_string) + 1;
	////strcat(setup_string, "A");//sri
	//vth_mhpib_wr(mod_id, 7, setup_string);

	sprintf(setup_string, "%s %1.3f %s\n", "CURR ", fabs(amps), ",(@2)");
	viPrintf(viPS, setup_string);

	////Sri add the OCP August 20th 2009////
	//vth_mhpib_wr(mod_id, 7, "OCP 1");
	///////////////////////////////////////
   // vth_mhpib_wr(mod_id, 7, "OUT ON");//sri
	//vth_mhpib_wr(mod_id, 7, "OUT 1");
	setup_N6746B_ch2_output(1);
	//}
}

#undef MAX_VOLTS_WITH_PS_LOADED





//////////////////////////////ADDED FOR 99V NASRU//////////////////////////

#define MAX_VOLTS_WITH_PS_LOADED  10.0e+00

/* ------------------------------------------------------------------------ */

void setup_ps_detector_test(int mod_id, double volts, double amps)

{  /*setup_ps_voltage*/

  char setup_string[81];

  int current_position;

  int next_position;


  if (fabs(volts) > MAX_VOLTS_WITH_PS_LOADED)

  {

    //vth_mhpib_wr(mod_id, 7, "OUT OFF");//

    vth_mhpib_wr(mod_id, 7, "OUT 0");  //old_ps
    //   vth_mhpib_wr(mod_id, 7, "OUTP OFF,(@2)"); //New_ps

	aconnect_ps_20_ohm_load(mod_id, false);

  }



  //vth_mhpib_wr(mod_id, 7, "DLY 0S");//

    vth_mhpib_wr(mod_id, 7, "DLY 0.00");  //old_ps
    // vth_mhpib_wr(mod_id, 7, "OUTP:PROT:DEL 0.08,(@2)");  //new_PS



  *setup_string = '\0';

  strcat(setup_string, "VSET");  //old_ps
  //strcat(setup_string, "VOLT");  //new_ps

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.3f", fabs(volts)); //old_pS
  //sprintf(setup_string + current_position - 1, "%1.3f, %s\n", fabs(volts), ",(@2)"); //new ps
 //	sprintf(setup_string, "%s %1.3f %s\n", "VOLT ", fabs(volts),",(@2)");
 //	viPrintf(viPS, setup_string);


	next_position = mtd_strlen(setup_string) + 1;


  //strcat(setup_string, "V");//sri

  vth_mhpib_wr(mod_id, 7, setup_string);


  *setup_string = '\0';

  strcat(setup_string, "ISET"); //old_ps
  // strcat(setup_string, "CURR"); //New_ps

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.2f", amps);  //old_ps
  //sprintf(setup_string + current_position - 1, "%1.2f", amps,(@2));  //_new_ps
  //	sprintf(setup_string, "%s %1.2f %s\n", "CURR ", fabs(amps),",(@2)");
	//viPrintf(viPS, setup_string);

  next_position = mtd_strlen(setup_string) + 1;

  //strcat(setup_string, "A");//sri
  vth_mhpib_wr(mod_id, 7, setup_string);
  //vth_mhpib_wr(mod_id, 7, "CURR %1.2f,(@2)");



  ////added for 99V NASRU test*

  vth_mhpib_wr(mod_id, 7, "OCP 1"); //old_ps
 // vth_mhpib_wr(mod_id, 7, "CURR:PROT:STAT ON,(@2)");   //new_ps



  ///////////////////////////////////////



 // vth_mhpib_wr(mod_id, 7, "OUT ON");//sri

  vth_mhpib_wr(mod_id, 7, "OUT 1");  //old_ps
  //vth_mhpib_wr(mod_id, 7, "OUTP ON,(@2)");  //New_ps



}  /*setup_ps_voltage*/


#define MAX_VOLTS_WITH_PS_LOADED  10.0e+00

void setup_PS_N6746B_detector_test(int mod_id, double volts, double amps)

{  /*setup_ps_voltage*/

	char setup_string[81];

	int current_position;

	int next_position;


	if (fabs(volts) > MAX_VOLTS_WITH_PS_LOADED)

	{

		//vth_mhpib_wr(mod_id, 7, "OUT OFF");//

		//vth_mhpib_wr(mod_id, 7, "OUT 0");
		setup_N6746B_ch2_output(0);
		aconnect_ps_20_ohm_load(mod_id, false);

	}



	//vth_mhpib_wr(mod_id, 7, "DLY 0S");//

	//vth_mhpib_wr(mod_id, 7, "DLY 0.08");
	setup_N6746B_ch2_delay(0.00);


	*setup_string = '\0';

	//strcat(setup_string, "VSET");

	//current_position = mtd_strlen(setup_string) + 1;

	//sprintf(setup_string + current_position - 1, "%1.3f", fabs(volts));

	//next_position = mtd_strlen(setup_string) + 1;

	////strcat(setup_string, "V");//sri

	//vth_mhpib_wr(mod_id, 7, setup_string);

	sprintf(setup_string, "%s %1.3f %s\n", "VOLT ", fabs(volts), ",(@2)");
	viPrintf(viPS, setup_string);

	*setup_string = '\0';

	//strcat(setup_string, "ISET");

	//current_position = mtd_strlen(setup_string) + 1;

	//sprintf(setup_string + current_position - 1, "%1.2f", amps);

	//next_position = mtd_strlen(setup_string) + 1;

	////strcat(setup_string, "A");//sri

	//vth_mhpib_wr(mod_id, 7, setup_string);

	sprintf(setup_string, "%s %1.3f %s\n", "CURR ", fabs(amps), ",(@2)");
	viPrintf(viPS, setup_string);

	////added for 99V NASRU test*

	*setup_string = '\0';
	//vth_mhpib_wr(mod_id, 7, "OCP 1");
	sprintf(setup_string, "%s\n", "CURR:PROT:STAT ON, (@2)");
	viPrintf(viPS, setup_string);


	///////////////////////////////////////



   // vth_mhpib_wr(mod_id, 7, "OUT ON");//sri

	//vth_mhpib_wr(mod_id, 7, "OUT 1");
	setup_N6746B_ch2_output(1);



}  /*setup_ps_voltage*/


void setup_PS_N6746B_detector_test_off(int mod_id, double volts, double amps)

{  /*setup_ps_voltage*/

	char setup_string[81];

	int current_position;

	int next_position;


	if (fabs(volts) > MAX_VOLTS_WITH_PS_LOADED)

	{

		//vth_mhpib_wr(mod_id, 7, "OUT OFF");//

		//vth_mhpib_wr(mod_id, 7, "OUT 0");
		setup_N6746B_ch2_output(0);
		aconnect_ps_20_ohm_load(mod_id, false);

	}



	//vth_mhpib_wr(mod_id, 7, "DLY 0S");//

	//vth_mhpib_wr(mod_id, 7, "DLY 0.08");
	setup_N6746B_ch2_delay(0.00);


	*setup_string = '\0';

	//strcat(setup_string, "VSET");

	//current_position = mtd_strlen(setup_string) + 1;

	//sprintf(setup_string + current_position - 1, "%1.3f", fabs(volts));

	//next_position = mtd_strlen(setup_string) + 1;

	////strcat(setup_string, "V");//sri

	//vth_mhpib_wr(mod_id, 7, setup_string);

	sprintf(setup_string, "%s %1.3f %s\n", "VOLT ", fabs(volts), ",(@2)");
	viPrintf(viPS, setup_string);

	*setup_string = '\0';

	//strcat(setup_string, "ISET");

	//current_position = mtd_strlen(setup_string) + 1;

	//sprintf(setup_string + current_position - 1, "%1.2f", amps);

	//next_position = mtd_strlen(setup_string) + 1;

	////strcat(setup_string, "A");//sri

	//vth_mhpib_wr(mod_id, 7, setup_string);

	sprintf(setup_string, "%s %1.3f %s\n", "CURR ", fabs(amps), ",(@2)");
	viPrintf(viPS, setup_string);

	////added for 99V NASRU test*

	*setup_string = '\0';
	//vth_mhpib_wr(mod_id, 7, "OCP 1");
	sprintf(setup_string, "%s\n", "CURR:PROT:STAT ON, (@2)");
	viPrintf(viPS, setup_string);


	///////////////////////////////////////



   // vth_mhpib_wr(mod_id, 7, "OUT ON");//sri

	//vth_mhpib_wr(mod_id, 7, "OUT 1");
	setup_N6746B_ch2_output(0);



}  /*setup_ps_voltage*/

#undef MAX_VOLTS_WITH_PS_LOADED





//////////////////////////////////////////////////////////////////////////

/* ------------------------------------------------------------------------ */

double measure_ps_sense_voltage(int mod_id)

{

  vth_mhpib_wr(mod_id, 7, "VOUT?");

  vth_mhpib_rd(mod_id, 7);

  return (convert_logged_string_to_numeric(mod_id));

}  /*measure_ps_sense_voltage*/

double measure_PS_N6746B_sense_voltage(int mod_id)
{
	//*setup_string = '\0';
	//sprintf(setup_string, "%s", "MEAS:VOLT? (@2)");
	//viPrintf(viPS,setup_string);

	double PS_N6746B_result;
	char str_N6746B[256] = { 0 };
	//char buf1[256] = { 0 };// for OPC
	char setup_string[81];

	*setup_string = '\0';

	sprintf(setup_string, "%s\n", "*OPC?");
	err = viQueryf(viPS, setup_string, "%t", &buf);
	//err = viQueryf(viPS, "*OPC?", "%t", &buf);
	//vth_mtmr_wait(mod_id, 1.0);
	if (err != 0)
	{
		printf("GPIB command pending on N6746B!!!");
	}
	else
	{
		*setup_string = '\0';
		sprintf(setup_string, "%s\n", "MEAS:VOLT? (@2)");
		err = viQueryf(viPS, setup_string, "%t", &buf);

		//err = viQueryf(viPS, "MEAS:VOLT? (@2)", "%t", &buf);
		/*err1 = viQueryf(viPS, "*OPC?", "%t", &buf1);
		if (err1 != 0 || err != 0)
		{
			printf("GPIB command pending on N6746B!!!");
		}*/
	    //else if (err1 == 0 && err == 0)
		if (err != 0)
		{
			printf("GPIB command pending on N6746B!!!");
		}
		else if(err == 0)
		{
			sprintf(str_N6746B, "%.50s", buf);
			PS_N6746B_result = atof(str_N6746B);
			//display_prompt(str1, true);
			return PS_N6746B_result;
		}
	}

	//double counter_fetch_result;
	//char str1[256] = { 0 };
	//err = viQueryf(viCNTR, "FETC?\n", "%t", &buf);
	//if (err != 0)
	//{
	//	printf("GPIB command pending on 53230A!!!");
	//}
	//else
	//{
	//	sprintf(str1, "%.50s", buf);
	//	counter_fetch_result = atof(str1);
	//	//display_prompt(str1, true);
	//	return counter_fetch_result;
	//}

	/*vth_mhpib_wr(mod_id, 7, "VOUT?");

	vth_mhpib_rd(mod_id, 7);

	return (convert_logged_string_to_numeric(mod_id));*/

}  /*measure_ps_sense_voltage*/

/*non-priv mark*/

#define MARK_NUM        1

/*wait time between readings*/

#define WAIT_INCREMENT  50e-3

/* ------------------------------------------------------------------------ */

void verify_ps_output(int mod_id, double diff, double ramp_time,

                      double expected_volts)

{

  double ps_actual_voltage;

  double last_voltage;

  double elapsed_time;

  double current_diff;



  boolean within_10_percent;



  test_result_type result;



  within_10_percent = false;

  vth_mtmr_set_mark(mod_id, MARK_NUM);

  ps_actual_voltage = 0.0;

  do

  {

    last_voltage = ps_actual_voltage;

    ps_actual_voltage = measure_ps_sense_voltage(mod_id);

    if (fabs((ps_actual_voltage - expected_volts) / expected_volts) <= 0.1)

      within_10_percent = true;

    vth_mtmr_wait(mod_id, WAIT_INCREMENT);

    vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUM);

    if (get_result_nolog(mod_id, &result))

      elapsed_time = result.UU.Real_result;

    else

      display_prompt("No result in result log (Expected mark time)", true);

    current_diff = fabs(ps_actual_voltage - last_voltage);

  } while (!(current_diff <= diff && within_10_percent ||

             elapsed_time >= ramp_time));

  if (elapsed_time >= ramp_time)

  {  /* ps error message */

    display_prompt("Power supply did not ramp within allotted time-sri.", true);

    display_prompt("Test results are not valid.  Check Sales Rack PS.", true);

  }  /* ps error message */



}  /*verify_ps_output*/

void verify_PS_N6746B_output(int mod_id, double diff, double ramp_time, double expected_volts)
{

	double ps_actual_voltage;

	double last_voltage;

	double elapsed_time;

	double current_diff;



	boolean within_10_percent;



	test_result_type result;



	within_10_percent = false;

	vth_mtmr_set_mark(mod_id, MARK_NUM);

	ps_actual_voltage = 0.0;

	do

	{

		last_voltage = ps_actual_voltage;

		ps_actual_voltage = measure_PS_N6746B_sense_voltage(mod_id);//chun new measure function

		if (fabs((ps_actual_voltage - expected_volts) / expected_volts) <= 0.1)

			within_10_percent = true;

		vth_mtmr_wait(mod_id, WAIT_INCREMENT);

		vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUM);

		if (get_result_nolog(mod_id, &result))

			elapsed_time = result.UU.Real_result;

		else

			display_prompt("No result in result log (Expected mark time)", true);

		current_diff = fabs(ps_actual_voltage - last_voltage);

	} while (!(current_diff <= diff && within_10_percent ||

		elapsed_time >= ramp_time));

	if (elapsed_time >= ramp_time)

	{  /* ps error message */

		display_prompt("Power supply did not ramp within allotted time-sri.", true);

		display_prompt("Test results are not valid.  Check Sales Rack PS.", true);

	}  /* ps error message */



}  /*verify_ps_output*/

#undef MARK_NUM

#undef WAIT_INCREMENT



/*non-priv mark*/

#define MARK_NUM        1

/*wait time between readings*/

#define WAIT_INCREMENT  50e-3

/* ------------------------------------------------------------------------ */

void wait_for_ps_ramp(int mod_id, double diff, double ramp_time,

                      double *dmm_actual_voltage)

{

  double expected_volts;

  double last_voltage;

  double elapsed_time;

  double current_diff;



  boolean within_10_percent;



  test_result_type result;



  expected_volts = *dmm_actual_voltage;

  within_10_percent = false;

  vth_mtmr_set_mark(mod_id, MARK_NUM);

  *dmm_actual_voltage = 0.0;

  do

  {

    last_voltage = *dmm_actual_voltage;

    measure_dmm(mod_id);

    if (get_result_nolog(mod_id, &result))

      *dmm_actual_voltage = result.UU.Real_result;

    else

      display_prompt("No result in result log (Expected dmm vth_reading)", true);

    if (fabs((*dmm_actual_voltage - expected_volts) / expected_volts) <= 0.1)

      within_10_percent = true;

    vth_mtmr_wait(mod_id, WAIT_INCREMENT);

    vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUM);

    if (get_result_nolog(mod_id, &result))

      elapsed_time = result.UU.Real_result;

    else

      display_prompt("No result in result log (Expected mark time)", true);

    current_diff = fabs(*dmm_actual_voltage - last_voltage);

  } while (!(current_diff <= diff && within_10_percent ||

             elapsed_time >= ramp_time));

  if (elapsed_time >= ramp_time)

  {  /* ps error message */

    display_prompt("Power supply did not ramp within allotted time-Sri1.", true);

    display_prompt("Test results are not valid.  Check Sales Rack PS.", true);

  }  /* ps error message */



}  /* wait_for_ps_ramp */

#undef MARK_NUM

#undef WAIT_INCREMENT



/* ------------------------------------------------------------------------ */

void disable_ps_out(int mod_id)

{

  //vth_mhpib_wr(mod_id, 7, "OUT OFF");//

  vth_mhpib_wr(mod_id, 7, "OUT 0");

}  /*disable_ps_out*/

void disable_ps_N6746B_out(int mod_id)
{

	//vth_mhpib_wr(mod_id, 7, "OUT OFF");//

	//vth_mhpib_wr(mod_id, 7, "OUT 0");

	char command[256]; //= "CONF:TOT:CONT (@1)";//":CONFigure:TOTalize:CONTinuous";
	*command = '\0';
	sprintf(command, "%s\n", "OUTP OFF, (@1)");
	viPrintf(viPS, command);

}  /*disable_ps_out*/

/* ------------------------------------------------------------------------ */

void enable_ps_out(int mod_id)
{

 // vth_mhpib_wr(mod_id, 7, "OUT ON");//

  vth_mhpib_wr(mod_id, 7, "OUT 1");

}  /*enable_ps_out*/

void enable_ps_N6746B_out(int mod_id)

{

	// vth_mhpib_wr(mod_id, 7, "OUT ON");//

	//vth_mhpib_wr(mod_id, 7, "OUT 1");

	char command[256]; //= "CONF:TOT:CONT (@1)";//":CONFigure:TOTalize:CONTinuous";
	*command = '\0';
	sprintf(command, "%s\n", "OUTP ON, (@1)");
	viPrintf(viPS, command);

}  /*enable_ps_out*/
/* ------------------------------------------------------------------------ */

void close_ps(int mod_id)
{

 // vth_mhpib_wr(mod_id, 7, "OUT OFF");//

  vth_mhpib_wr(mod_id, 7, "OUT 0"); //old_ps
//  vth_mhpib_wr(mod_id, 7, "OUTP OFF, (@2)"); //old_ps

  vth_mhpib_close(mod_id, 7);

}  /*close_ps*/

//void close_ps_N6746B(int mod_id)
//{
//
//	// vth_mhpib_wr(mod_id, 7, "OUT OFF");//
//
//	//vth_mhpib_wr(mod_id, 7, "OUT 0");
//
//	//vth_mhpib_close(mod_id, 7);
//
//	/*char command[256];
//	*command = '\0';
//	sprintf(command, "%s\n", "OUTP OFF, (@1)");
//	viPrintf(viPS, command);*/
//
//	char buf1[256] = { 0 };
//	err = viQueryf(viPS, "*OPC?\n", "%t", &buf1);
//	if (err != 0)
//	{
//		printf("GPIB command pending on N6746B!!!");
//
//	}
//	else
//	{
//		viPrintf(viPS, "*RST\n"); //temp comment out chun on 09122021
//	}
//
//	viClose(viPS);
//
//}  /*close_ps*/











/* Sri- added on 28August 2007, for the Load PS- 6038A.*/



/* ---------------------------------------------------------------------- */

/* wpd - include begins here */

/* Use tag 0 for the 6038A Load Power Supply */

/* Address is 5  */

/* ------------------------------------------------------------------------ */

void open_ps_load(int mod_id)
{

  printf("Testing-using the new Load PS\n");

	  vth_mhpib_open(mod_id, 0, "/dev/sales.ps_load"); //changed file name, sri, the tag is defined here"0"

	  vth_mhpib_wr(mod_id, 0, "CLR");

	  vth_mhpib_wr(mod_id, 0, "OUT OFF");

	  //vth_mhpib_wr(mod_id, 7, "OUT 0");

}  /*open_ps*/

void open_PS_N6754A_load(int mod_id)
{
	char setup_string[81];
	*setup_string = '\0';

	err = viOpen(rsm, PS_N6754A, VI_NULL, VI_NULL, &viPS_N6754A);

	if (err != 0)
	{
		display_prompt("Can't connect N6754A", true);
	}

	viSetAttribute(viPS_N6754A, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viPS_N6754A, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	//viPrintf(viPS, "*RST\n");

	sprintf(setup_string, "%s\n", "*RST");
	viPrintf(viPS_N6754A, setup_string);
}

boolean verify_PS_N6754A_load(int mod_id)
{
	ViStatus  hw_err;
	char buf1[256] = { 0 };
	char str1[256] = { 0 };

	err = viQueryf(viPS_N6754A, "SYST:CHAN:MOD? (@1)", "%t", &buf);  //buf for IDN
	hw_err = viQueryf(viPS_N6754A, "SYST:ERR?", "%t", &buf1);  //buf1 for error
	if (err != 0 && hw_err != 0)
	{
		display_prompt("N6754A not responding to GPIB queries", true);
	}
	else
	{
		sprintf(str1, "%.38s", buf);
		if (!strcmp(str1, "N6754A"))
		{
			display_prompt(str1, true);
			display_prompt("   N6754A Okay", true);
			return true;
		}
		else
		{
			display_prompt("   N6754A not connected", true);
			return false;
		}
	}
}

#define MAX_VOLTS_WITH_PS_LOADED  31.0e+00

/* ------------------------------------------------------------------------ */

void setup_ps_load(int mod_id, double volts, double amps)

{  /*setup_ps_voltage*/

  char setup_string[81];

  int current_position;

  int next_position;



  if (fabs(volts) > MAX_VOLTS_WITH_PS_LOADED)

  {

    vth_mhpib_wr(mod_id, 0, "OUT OFF");

    //vth_mhpib_wr(mod_id, 7, "OUT 0");

	aconnect_ps_20_ohm_load(mod_id, false);

  }



  //vth_mhpib_wr(mod_id, 7, "DLY 0S");//

  //vth_mhpib_wr(mod_id, 0, "DLY 0.08");



  *setup_string = '\0';

  strcat(setup_string, "VSET");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.3f", fabs(volts));

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, "V");//sri

  vth_mhpib_wr(mod_id, 0, setup_string);



  *setup_string = '\0';

  strcat(setup_string, "ISET");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.2f", amps);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, "A");//sri

  vth_mhpib_wr(mod_id, 0, setup_string);



  vth_mhpib_wr(mod_id, 0, "OUT ON");//sri

 // vth_mhpib_wr(mod_id, 7, "OUT 1");



}  /*setup_ps_voltage*/

void setup_PS_N6754A_load(int mod_id, double volts, double amps)

{  /*setup_ps_voltage*/

	char setup_string[81];

	int current_position;

	int next_position;



	if (fabs(volts) > MAX_VOLTS_WITH_PS_LOADED)

	{

		//vth_mhpib_wr(mod_id, 0, "OUT OFF");
		setup_N6754A_ch1_output(0);

		//vth_mhpib_wr(mod_id, 7, "OUT 0");

		aconnect_ps_20_ohm_load(mod_id, false);

	}



	//vth_mhpib_wr(mod_id, 7, "DLY 0S");//

	//vth_mhpib_wr(mod_id, 0, "DLY 0.08");



	*setup_string = '\0';

	//strcat(setup_string, "VSET");

	//current_position = mtd_strlen(setup_string) + 1;

	//sprintf(setup_string + current_position - 1, "%1.3f", fabs(volts));

	//next_position = mtd_strlen(setup_string) + 1;

	//strcat(setup_string, "V");//sri

	//vth_mhpib_wr(mod_id, 0, setup_string);
	sprintf(setup_string, "%s %1.3f %s\n", "VOLT ", fabs(volts), ",(@1)");
	viPrintf(viPS_N6754A, setup_string);


	*setup_string = '\0';

	//strcat(setup_string, "ISET");

	//current_position = mtd_strlen(setup_string) + 1;

	//sprintf(setup_string + current_position - 1, "%1.2f", amps);

	//next_position = mtd_strlen(setup_string) + 1;

	//strcat(setup_string, "A");//sri

	//vth_mhpib_wr(mod_id, 0, setup_string);
	sprintf(setup_string, "%s %1.3f %s\n", "CURR ", fabs(amps), ",(@1)");
	viPrintf(viPS_N6754A, setup_string);


	//vth_mhpib_wr(mod_id, 0, "OUT ON");//sri
	setup_N6754A_ch1_output(1);
   // vth_mhpib_wr(mod_id, 7, "OUT 1");



}  /*setup_ps_N6754A_voltage*/

#undef MAX_VOLTS_WITH_PS_LOADED



/* ------------------------------------------------------------------------ */

double measure_ps_sense_voltage_load(int mod_id)

{

  vth_mhpib_wr(mod_id, 0, "VOUT?");

  vth_mhpib_rd(mod_id, 0);

  return (convert_logged_string_to_numeric(mod_id));

}  /*measure_ps_sense_voltage*/


double measure_PS_N6754A_sense_voltage_load(int mod_id)
{

	//*setup_string = '\0';
	//sprintf(setup_string, "%s", "MEAS:VOLT? (@2)");
	//viPrintf(viPS,setup_string);

	double PS_N6754A_result;
	char str_N6754A[256] = { 0 };
	char buf1[256] = { 0 };// for OPC
	char setup_string[81];

	*setup_string = '\0';

	sprintf(setup_string, "%s\n", "*OPC?");
	err = viQueryf(viPS_N6754A, setup_string, "%t", &buf);
	//err = viQueryf(viPS, "*OPC?", "%t", &buf);

	if (err != 0)
	{
		printf("GPIB command pending on N6754A!!!");
	}
	else
	{
		*setup_string = '\0';
		sprintf(setup_string, "%s\n", "MEAS:VOLT? (@1)");
		err = viQueryf(viPS_N6754A, setup_string, "%t", &buf);

		if (err != 0)
		{
			printf("GPIB command pending on N6754A!!!");
		}
		else if (err == 0)
		{
			sprintf(str_N6754A, "%.50s", buf);
			PS_N6754A_result = atof(str_N6754A);
			return PS_N6754A_result;
		}
	}

}  /*measure_ps_sense_voltage*/



double measure_ps_load_current(int mod_id)

{

  vth_mhpib_wr(mod_id, 0, "IOUT?");

  vth_mhpib_rd(mod_id, 0);

  return (convert_logged_string_to_numeric(mod_id));

}

double measure_PS_N6754A_load_current(int mod_id)
{

	double PS_N6754A_result;
	char str_N6754A[256] = { 0 };
	char buf1[256] = { 0 };// for OPC
	char setup_string[81];

	*setup_string = '\0';

	sprintf(setup_string, "%s\n", "*OPC?");
	err = viQueryf(viPS_N6754A, setup_string, "%t", &buf);
	//err = viQueryf(viPS, "*OPC?", "%t", &buf);

	if (err != 0)
	{
		printf("GPIB command pending on N6754A!!!");
	}
	else
	{
		*setup_string = '\0';
		sprintf(setup_string, "%s\n", "MEAS:CURR? (@1)");
		err = viQueryf(viPS_N6754A, setup_string, "%t", &buf);

		if (err != 0)
		{
			printf("GPIB command pending on N6754A!!!");
		}
		else if (err == 0)
		{
			sprintf(str_N6754A, "%.50s", buf);
			PS_N6754A_result = atof(str_N6754A);
			//display_prompt(str1, true);
			return PS_N6754A_result;
		}
	}

}





/*non-priv mark*/

#define MARK_NUM        1

/*wait time between readings*/

#define WAIT_INCREMENT  50e-3

/* ------------------------------------------------------------------------ */



void verify_ps_output_load(int mod_id, double diff, double ramp_time,

                      double expected_volts)

{

  double ps_actual_voltage;

  double last_voltage;

  double elapsed_time;

  double current_diff;



  boolean within_10_percent;



  test_result_type result;



  within_10_percent = false;

  vth_mtmr_set_mark(mod_id, MARK_NUM);

  ps_actual_voltage = 0.0;

  do

  {

    last_voltage = ps_actual_voltage;

    ps_actual_voltage = measure_ps_sense_voltage_load(mod_id);

    if (fabs((ps_actual_voltage - expected_volts) / expected_volts) <= 0.1)

      within_10_percent = true;

    vth_mtmr_wait(mod_id, WAIT_INCREMENT);

    vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUM);

    if (get_result_nolog(mod_id, &result))

      elapsed_time = result.UU.Real_result;

    else

      display_prompt("No result in result log (Expected mark time)", true);

    current_diff = fabs(ps_actual_voltage - last_voltage);

  } while (!(current_diff <= diff && within_10_percent ||

             elapsed_time >= ramp_time));

  if (elapsed_time >= ramp_time)

  {  /* ps error message */

    display_prompt("Power supply did not ramp within allotted time-sri.", true);

    display_prompt("Test results are not valid.  Check Sales Rack PS.", true);

  }  /* ps error message */



}  /*verify_ps_output*/

void verify_PS_N6754A_output_load(int mod_id, double diff, double ramp_time,

	double expected_volts)

{

	double ps_actual_voltage;

	double last_voltage;

	double elapsed_time;

	double current_diff;



	boolean within_10_percent;



	test_result_type result;



	within_10_percent = false;

	vth_mtmr_set_mark(mod_id, MARK_NUM);

	ps_actual_voltage = 0.0;

	do

	{

		last_voltage = ps_actual_voltage;

		ps_actual_voltage = measure_PS_N6754A_sense_voltage_load(mod_id);

		if (fabs((ps_actual_voltage - expected_volts) / expected_volts) <= 0.1)

			within_10_percent = true;

		vth_mtmr_wait(mod_id, WAIT_INCREMENT);

		vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUM);

		if (get_result_nolog(mod_id, &result))

			elapsed_time = result.UU.Real_result;

		else

			display_prompt("No result in result log (Expected mark time)", true);

		current_diff = fabs(ps_actual_voltage - last_voltage);

	} while (!(current_diff <= diff && within_10_percent ||

		elapsed_time >= ramp_time));

	if (elapsed_time >= ramp_time)

	{  /* ps error message */

		display_prompt("Power supply did not ramp within allotted time-sri.", true);

		display_prompt("Test results are not valid.  Check Sales Rack PS.", true);

	}  /* ps error message */



}  /*verify_ps_output*/

#undef MARK_NUM

#undef WAIT_INCREMENT



/*non-priv mark*/

#define MARK_NUM        1

/*wait time between readings*/

#define WAIT_INCREMENT  50e-3

/* ------------------------------------------------------------------------ */

void wait_for_ps_ramp_load(int mod_id, double diff, double ramp_time,

                      double *dmm_actual_voltage)

{

  double expected_volts;

  double last_voltage;

  double elapsed_time;

  double current_diff;



  boolean within_10_percent;



  test_result_type result;



  expected_volts = *dmm_actual_voltage;

  within_10_percent = false;

  vth_mtmr_set_mark(mod_id, MARK_NUM);

  *dmm_actual_voltage = 0.0;

  do

  {

    last_voltage = *dmm_actual_voltage;

    measure_dmm(mod_id);

    if (get_result_nolog(mod_id, &result))

      *dmm_actual_voltage = result.UU.Real_result;

    else

      display_prompt("No result in result log (Expected dmm vth_reading)", true);

    if (fabs((*dmm_actual_voltage - expected_volts) / expected_volts) <= 0.1)

      within_10_percent = true;

    vth_mtmr_wait(mod_id, WAIT_INCREMENT);

    vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUM);

    if (get_result_nolog(mod_id, &result))

      elapsed_time = result.UU.Real_result;

    else

      display_prompt("No result in result log (Expected mark time)", true);

    current_diff = fabs(*dmm_actual_voltage - last_voltage);

  } while (!(current_diff <= diff && within_10_percent ||

             elapsed_time >= ramp_time));

  if (elapsed_time >= ramp_time)

  {  /* ps error message */

    display_prompt("Power supply did not ramp within allotted time-Sri1.", true);

    display_prompt("Test results are not valid.  Check Sales Rack PS.", true);

  }  /* ps error message */



}  /* wait_for_ps_ramp */

#undef MARK_NUM

#undef WAIT_INCREMENT



/* ------------------------------------------------------------------------ */

void disable_ps_out_load(int mod_id)

{

  vth_mhpib_wr(mod_id, 0, "OUT OFF");//

  //vth_mhpib_wr(mod_id, 7, "OUT 0");

}  /*disable_ps_out*/

void disable_ps_N6754A_out_load(int mod_id)
{

	//vth_mhpib_wr(mod_id, 0, "OUT OFF");//

	//vth_mhpib_wr(mod_id, 7, "OUT 0");

	char command[256];
	*command = '\0';

	sprintf(command,"%s\n", "OUTP OFF, (@1)");
	viPrintf(viPS_N6754A, command);

}  /*disable_ps_out*/

/* ------------------------------------------------------------------------ */

void enable_ps_out_load(int mod_id)
{

  vth_mhpib_wr(mod_id, 0, "OUT ON");//

  //vth_mhpib_wr(mod_id, 7, "OUT 1");

}  /*enable_ps_out*/

/* ------------------------------------------------------------------------ */

void enable_ps_N6754A_out_load(int mod_id)
{

	//vth_mhpib_wr(mod_id, 0, "OUT ON");//

	//vth_mhpib_wr(mod_id, 7, "OUT 1");

	char command[256];
	*command = '\0';

	sprintf(command, "%s\n", "OUTP ON, (@1)");
	viPrintf(viPS_N6754A, command);

}  /*enable_ps_out*/

void close_ps_load(int mod_id)
{

  vth_mhpib_wr(mod_id, 0, "OUT OFF");//

  //vth_mhpib_wr(mod_id, 7, "OUT 0");

  vth_mhpib_close(mod_id, 0);

}  /*close_ps*/

void close_PS_N6754A_load(int mod_id)
{
	char setup_string[81];
	char buf1[256] = { 0 };

	//*setup_string = '\0';

	//sprintf(setup_string, "%s\n", "*OPC?");
	//err = viQueryf(viPS, setup_string, &buf);

	err = viQueryf(viPS_N6754A, "*OPC?\n", "%t", &buf1);
	if (err != 0)
	{
		printf("GPIB command pending on N6754A!!!");

	}
	else
	{
		//*setup_string = '\0';
		//sprintf(setup_string, "%s\n", "*RST");
		//viPrintf(viPS, setup_string);

		viPrintf(viPS_N6754A, "*RST\n");
	}
	viClose(viPS_N6754A);
}

///////////////////////////END HERE! SRI ADDING NEW LOAD PS//////////////////////











/* wpd - include begins here */

/* Use tag 9 for the DUT power supply for the module */

/* Written for Orion Pin Card Ringing Test (T9155) 3/26/91 Jim Myers */

/* ------------------------------------------------------------------------ */

void open_dut_ps(int mod_id)

{

  switch (mod_id)

  {   /*open the power supply associated with this module*/



  case 0:

    vth_mhpib_open(mod_id, 9, "/dev/ps0");

    break;



  case 1:

    vth_mhpib_open(mod_id, 9, "/dev/ps1");

    break;



  case 2:

    vth_mhpib_open(mod_id, 9, "/dev/ps2");

    break;



  case 3:

    vth_mhpib_open(mod_id, 9, "/dev/ps3");

    break;



  default:

    assert(false);

    break;



  }/*case*/



  vth_mhpib_wr(mod_id, 9, "CLR");



  /* Disable all four outputs of the quad supply */

  vth_mhpib_wr(mod_id, 9, "OUT 1,0");   /* Disable quad output #1*/

  vth_mhpib_wr(mod_id, 9, "OUT 2,0");   /* Disable quad output #2*/

  vth_mhpib_wr(mod_id, 9, "OUT 3,0");   /* Disable quad output #3*/

  vth_mhpib_wr(mod_id, 9, "OUT 4,0");   /* Disable quad output #4*/



}  /*open_dut_ps*/



/* ------------------------------------------------------------------------ */

void setup_dut_ps(int mod_id, int ps_channel, double volts, double ov, double amps)

{

  char setup_string[81];

  int current_position;

  int next_position;



  /* Reset the over voltage protection for the power supply */

  *setup_string = '\0';

  strcat(setup_string, "OVRST ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", ps_channel);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 9, setup_string);



  /* Setup the over voltage protection for the power supply */

  *setup_string = '\0';

  strcat(setup_string, "OVSET ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", ps_channel);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.3f", ov);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 9, setup_string);



  /* Setup the voltage for the power supply */

  *setup_string = '\0';

  strcat(setup_string, "VSET ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", ps_channel);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.3f", volts);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 9, setup_string);



  /* Setup the current limit for the power supply */

  *setup_string = '\0';

  strcat(setup_string, "ISET ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", ps_channel);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%1.3f", amps);

  next_position = mtd_strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, 9, setup_string);



  /* Enable the dut supply out to the aim mint pins */

  vth_aclose_dut_pwr_rly(mod_id, ps_channel);

  vth_aclose_dut_sense_rlys(mod_id, ps_channel);



  /* Turn the output of the power supply on */

  *setup_string = '\0';

  strcat(setup_string, "OUT ");

  current_position = mtd_strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "%d", ps_channel);

  next_position = mtd_strlen(setup_string) + 1;

  strcat(setup_string, ",1");

  vth_mhpib_wr(mod_id, 9, setup_string);



}  /*setup_dut_ps*/



/* ------------------------------------------------------------------------ */

void close_dut_ps(int mod_id)

{

  /* Disable all four outputs of the quad supply */

  vth_mhpib_wr(mod_id, 9, "OUT 1,0");   /* Disable quad output #1*/

  vth_mhpib_wr(mod_id, 9, "OUT 2,0");   /* Disable quad output #2*/

  vth_mhpib_wr(mod_id, 9, "OUT 3,0");   /* Disable quad output #3*/

  vth_mhpib_wr(mod_id, 9, "OUT 4,0");   /* Disable quad output #4*/



  vth_mhpib_close(mod_id, 9);



}  /*close_dut_ps*/



#define OSC_CHANNEL     1

#define SYN_FREQ        0

#define SYN_AMP         0.0

#define RESOLUTION      0.001

#define SYN_WAVE        DC

#define DMM_FUNCTION    DCV

#define AVERAGING       true

/* wpd - include begins here */

double offset_calibration(int mod_id, double calibration_voltage)

{

  /* This function assumes that the oscilloscope is set up before this routine */

  /* is called.  The dmm and synthesizer are set up inside this routine.       */

  double result;





  double dmm_actual_voltage;

  double osc_actual_cal;

  test_result_type test_result;

  open_33250A(mod_id);

  /*Run with new FG*/
  if(check_syn_model())
  {

  setup_33250A_defaults(mod_id);

  setup_33250A(mod_id, SYN_WAVE, SYN_FREQ, SYN_AMP, calibration_voltage);

  setup_dmm(mod_id, DMM_FUNCTION, calibration_voltage, RESOLUTION);



  dconnect_syn_out_dmm(mod_id);

  measure_dmm(mod_id);

  if (get_result_nolog(mod_id, &test_result))

  {  /*process result*/

    test_result.Measurement_class = CLASS_REAL;

    dmm_actual_voltage = test_result.UU.Real_result;

  }  /*process result*/

  else

    display_prompt("No result in result log (Expected syn out volt)", true);



  connect_syn_out_osc_ch1(mod_id);

  trigger_osc(mod_id, OSC_CHANNEL, AVERAGING);

  measure_osc(mod_id, VAVERAGE, POS);

  if (get_result_nolog(mod_id, &test_result))

  {  /*process result*/

    test_result.Measurement_class = CLASS_REAL;

    osc_actual_cal = test_result.UU.Real_result;

  }  /*process result*/

  else

    display_prompt("No result in result log (Expected syn out volt)", true);



  result = osc_actual_cal - dmm_actual_voltage;

  close_33250A(mod_id);

  /* DO NOT CLOSE THE DMM IN CASE IT IS USED IN THE PARENT TEST */

  return result;
  }
  /*End of run with new FG*/

  /* Run with old synthesizer*/
  else
  {
  open_syn(mod_id);

  setup_syn(mod_id, SYN_WAVE, SYN_FREQ, SYN_AMP, calibration_voltage);

  setup_dmm(mod_id, DMM_FUNCTION, calibration_voltage, RESOLUTION);



  dconnect_syn_out_dmm(mod_id);

  measure_dmm(mod_id);

  if (get_result_nolog(mod_id, &test_result))

  {  /*process result*/

    test_result.Measurement_class = CLASS_REAL;

    dmm_actual_voltage = test_result.UU.Real_result;

  }  /*process result*/

  else

    display_prompt("No result in result log (Expected syn out volt)", true);



  connect_syn_out_osc_ch1(mod_id);

  trigger_osc(mod_id, OSC_CHANNEL, AVERAGING);

  measure_osc(mod_id, VAVERAGE, POS);

  if (get_result_nolog(mod_id, &test_result))

  {  /*process result*/

    test_result.Measurement_class = CLASS_REAL;

    osc_actual_cal = test_result.UU.Real_result;

  }  /*process result*/

  else

    display_prompt("No result in result log (Expected syn out volt)", true);



  result = osc_actual_cal - dmm_actual_voltage;

  close_syn(mod_id);

  /* DO NOT CLOSE THE DMM IN CASE IT IS USED IN THE PARENT TEST */

  return result;
  }
  /*End of run with old synthesizer*/

}

#undef OSC_CHANNEL

#undef SYN_FREQ

#undef SYN_AMP

#undef RESOLUTION

#undef SYN_WAVE

#undef DMM_FUNCTION

#undef AVERAGING



#define RESOLUTION      1.0e-03

#define SOURCE_WAIT_TIME  1e-03

/* ------------------------------------------------------------------------ */

double aosc_cal(int mod_id, double voltage1, double voltage2, double *gain)

{

  double result;



  int loop;



  double voltage[2];

  double vdmm[2];

  double vosc[2];



  test_result_type test_result;



  open_dmm(mod_id);



  connect_dmm(mod_id);

  connect_plus_calib1(mod_id);

  connect_minus_calib2(mod_id);

  aconnect_osc_ch1_af2(mod_id);



  voltage[0] = voltage1;

  voltage[1] = voltage2;



  for (loop = 0; loop <= 1; loop++)

  {  /*for voltage*/

    setup_dmm(mod_id, DCV, voltage[loop], RESOLUTION);

    vth_asrc_set_dc_ampl(mod_id, voltage[loop]);



    vth_aclose_rly(mod_id, K837);

    vth_aclose_rly(mod_id, K853);

    vth_aclose_rly(mod_id, K854);



    vth_afc_en_src_out(mod_id);

    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);



    measure_dmm(mod_id);

    if (get_result_nolog(mod_id, &test_result))

    {  /*process the result*/

      vdmm[loop] = test_result.UU.Real_result;

    }  /*process the result*/

    else

      display_prompt("No result in result log - (Expected dmm volts)", true);



    vth_afc_dis_src_out(mod_id);



    vth_aopen_rly(mod_id, K837);

    vth_aopen_rly(mod_id, K853);

    vth_aopen_rly(mod_id, K854);



    vth_aclose_rly(mod_id, K806);

    vth_aclose_rly(mod_id, K751);

    vth_aclose_rly(mod_id, K764);



    vth_afc_en_src_out(mod_id);

    vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);



    trigger_osc(mod_id, 1, true);

    measure_osc(mod_id, VAVERAGE, POS);

    if (get_result_nolog(mod_id, &test_result))

    {  /*process the result*/

      vosc[loop] = test_result.UU.Real_result;

    }  /*process the result*/

    else

      display_prompt("No result in result log - (Expected dmm volts)", true);



    vth_afc_dis_src_out(mod_id);



    vth_aopen_rly(mod_id, K806);

    vth_aopen_rly(mod_id, K751);

    vth_aopen_rly(mod_id, K764);

  }  /*for voltage*/



  *gain = (vdmm[0] - vdmm[1]) / (vosc[0] - vosc[1]);

  result = vdmm[1] - *gain * vosc[1];



  close_dmm(mod_id);



  return result;

}  /*aosc_cal*/

#undef RESOLUTION

#undef SOURCE_WAIT_TIME



/* ------------------------------------------------------------------------- */

void setup_inst_defaults(int mod_id)

{

  /* Opens and closes the instruments that are may cause noise problems */

  /* in their power up states.                                          */

  open_gen(mod_id);

  open_lcr(mod_id);

  open_ps(mod_id);

  open_33250A(mod_id);

  open_olym(mod_id);

  vth_mtmr_wait(mod_id, 0.01);   /* Just in case */



  close_gen(mod_id);

  close_lcr(mod_id);

  close_ps(mod_id);

  if(check_syn_model())
	  close_33250A(mod_id);
  else
      close_syn(mod_id);

  close_olym(mod_id);



}  /*setup_inst_defaults*/



/* wpd - include begins here */

void relay_map(int mod_id, int slot, int relay_page)

{  /* RELAY MAP */

  switch (relay_page)

  {



  case 0:

    vth_pwr_cell(mod_id, slot, PAGE_SEL, 0xd600);

    break;



  case 1:

    vth_pwr_cell(mod_id, slot, PAGE_SEL, 0xcd00);

    break;



  case 2:

    vth_pwr_cell(mod_id, slot, PAGE_SEL, 0x9b00);

    break;

   default:

    assert(false);

    break;



  }/*case*/



}  /* RELAY MAP */



void virt_card_select(int mod_id, int slot, int virt_card)

{

  switch (virt_card)

  {



  case 0:

    vth_pwr_cell(mod_id, slot, PAGE_SEL, 0xd600);

    break;



  case 1:

    vth_pwr_cell(mod_id, slot, PAGE_SEL, 0xcd00);

    break;



  case 2:

    vth_pwr_cell(mod_id, slot, PAGE_SEL, 0x9b00);

    break;



  case 3:

    vth_pwr_cell(mod_id, slot, PAGE_SEL, 0);

    break;



  default:

    assert(false);

    break;



  }/*case*/

}  /*virt_card_select*/



//----------------------------------------------------------------------------

/*    CALC_MOD_CORRECTION                   mjs 6/97



PURPOSE:  This function reads the config of a module and determines how

          many cards (not counting the Mac and Aim) exist and how many of

          those are double density.  It then takes the correction factor

          and applies it once per card and then again for each double density.

          This is used for tests which have results that vary with the number

          of cards in the module.



INPUTS:   mod_num - the module that you need to determine a correction for

          sd_factor - the factor for a single density card



OUTPUT:   correction - a module configuration dependent correction factor



*/

double calc_mod_correction(int mod_num, double sd_factor)

{//begin



  int slot_num=0;

  int card_type=0;

  int num_cards=0;    //number of cards in module

  int num_dd_cards=0; //number of double density cards

  double correction;



    for(slot_num=1; slot_num <= 11; slot_num++)

    {//for each slot



      //get card type

      card_type = get_testhead_actual_card(mod_num, slot_num);



      //count total number of cards

      if(card_type != NO_CARD) num_cards++;



      //also keep track of double density cards

/* mls - rem; unexpected */

      if((card_type == DD6) || (card_type == DD20) || (card_type == DDP) ||

         (card_type == DDA) || (card_type == DDJB) || (card_type == SERIAL) ||

         (card_type == FDDU) || (card_type == FDDP) || (card_type == FDD6) ||

         (card_type == FDD20) || (card_type == FDDA) || (card_type == FDDJB) ||

         (card_type == CDDU) || (card_type == CDDP) || (card_type == CDD6) ||

         (card_type == CDD20) || (card_type == CDDJB) ||

         (card_type == STC_B_REV))

        num_dd_cards++;

/*

      if(card_type == NO_CARD) display_prompt("no card", true);

      else if(card_type == AIM) display_prompt("aim", true);

      else if(card_type == AIM20) display_prompt("aim20", true);

      else if(card_type == CONTROL) display_prompt("control", true);

      else if(card_type == CONTROLPLUS) display_prompt("controlplus", true);

      else if(card_type == PIN) display_prompt("pin", true);

      else if(card_type == PIN6) display_prompt("pin6", true);

      else if(card_type == PIN20) display_prompt("pin20", true);

      else if(card_type == CP) display_prompt("cp", true);

      else if(card_type == ACCESS) display_prompt("access", true);

      else if(card_type == SP) display_prompt("sp", true);

      else if(card_type == PINTERM) display_prompt("pinterm", true);

      else if(card_type == CP20) display_prompt("cp20", true);

      else if(card_type == SERIAL) display_prompt("serial", true);

* mls - rem; unexpected *

      else if(card_type == STC_B_REV) display_prompt("stcbrev", true);

      else if(card_type == ANALOG) display_prompt("analog", true);

      else if(card_type == DD6) display_prompt("fdd6", true);

      else if(card_type == FDD6) display_prompt("dd6", true);

      else if(card_type == FDDP) display_prompt("fddp", true);

      else if(card_type == DDP) display_prompt("ddp", true);

      else if(card_type == FDD20) display_prompt("fdd20", true);

      else if(card_type == DD20) display_prompt("dd20", true);

      else if(card_type == FDDA) display_prompt("fdda", true);

      else if(card_type == DDA) display_prompt("dda", true);

      else if(card_type == SD6) display_prompt("sd6", true);

      else if(card_type == SDP) display_prompt("sdp", true);

      else if(card_type == AIMC) display_prompt("aimc", true);

      else if(card_type == DDJB) display_prompt("ddjb", true);

      else if(card_type == PIN6JB) display_prompt("pin6jb", true);

      else if(card_type == FDDU) display_prompt("unprog", true); // unprogrammed Fezziwig pincards

      else if(card_type == FDDU) display_prompt("unprog", true);        //unprogrammed Fezziwig card

      else if(card_type == PIN12JB) display_prompt("pin12jb", true);

      else display_it(3, "card type number = ", card_type, card_type);

*/

   }



    //the -2 is to account for the MAC and AIM

    correction = (num_cards - 2 + num_dd_cards) * sd_factor;



    //if there are only 3 cards in the module including the MAC and AIM,

    //then only 1 other card is present.  don't correct in this situation

    if (num_cards==3) correction =0;

    return correction;

}



// ******************************************************************************************

// scope elimination -- abr

void setup_dmm_digitizing(int mod_id, int dmm_addr, int range, double interval,

       int num_samples, double aperature, double resolution, int trig_level, int slope)

{

// an gpib ID of 5 is for the dmm that doesn't have a terminating resistor

// an gpib ID of 8 is for the dmm50 that has the 50ohm termination resistor



  char setup_string[81];

  int current_position;

  int next_position;

  double percent_resolution;



  vth_mhpib_wr(mod_id, dmm_addr, "PRESET DIG");   /* PRESET DIG command */



  strcpy(setup_string, "APER ");   /* APER command */

  current_position = strlen(setup_string) + 1;

  sprintf(setup_string + current_position - 1, "% .3E", aperature);

  next_position = strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, dmm_addr, setup_string);



  vth_mhpib_wr(mod_id, dmm_addr, "MEM FIFO");   /* MEM FIFO */

  vth_mhpib_wr(mod_id, dmm_addr, "OFORMAT SREAL");

  vth_mhpib_wr(mod_id, dmm_addr, "MFORMAT DINT");



  //The precent_resolution is the actual resolution divided by the range(or max_imput) times 100...

  percent_resolution = (resolution/range)*100;



  strcpy(setup_string, "DCV ");   /* DCV range */

  sprintf(setup_string + 4, "%d", range);

  next_position = strlen(setup_string) + 1;

  strcat(setup_string, ",");

  sprintf(setup_string + next_position, "%7.3E", percent_resolution);

  next_position = strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, dmm_addr, setup_string);



  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */

  sprintf(setup_string + 6, "% .4E", interval);

  next_position = strlen(setup_string) + 1;

  strcat(setup_string, ",");

  sprintf(setup_string + next_position, "%d", num_samples);

  next_position = strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, dmm_addr, setup_string);



  strcpy(setup_string, "LEVEL ");   /* DCV range */

  sprintf(setup_string + 6, "%d", trig_level);

  next_position = strlen(setup_string) + 1;

  strcat(setup_string, ",");

  sprintf(setup_string + next_position, "%d", 1);  //The 1 represents DC coupling

  next_position = strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, dmm_addr, setup_string);



  if (slope == POS)

    vth_mhpib_wr(mod_id, dmm_addr, "SLOPE POS");

  else

    vth_mhpib_wr(mod_id, dmm_addr, "SLOPE NEG");



  if (dmm_addr == DMM_ADDRESS)  enable_dmm_buffer(mod_id);

  if (dmm_addr == DMM50_ADDRESS)  enable_dmm50_buffer(mod_id);



  vth_mhpib_wr(mod_id, dmm_addr, "TARM SGL");



}



//------------------------------------------------------------------

/* scope elimination project functions */

void get_dmm_volt(int mod_id, int dmm_addr, int num_samples, double *waveform, int mode)

{

  temp_array t;

  int j;

  int error;

  int srq;

  int bytes_read;

  hfs_fd_type* id;

  cmd_type command;

  path_type path;

  int mla;



  int byte0 = 0; //byte 0 of sample

  int byte1 = 0; //byte 1 of sample

  int byte2 = 0; //byte 2 of sample

  int byte3 = 0; //byte 3 of sample



  //FILE *ofile;



  flush_commands(mod_id);



  /* get data from Digital Sampler */



  if (dmm_addr == DMM_ADDRESS)  strcpy(path, "/dev/sales.dmm");

  if (dmm_addr == DMM50_ADDRESS)  strcpy(path, "/dev/sales.dmm50");



  c_hfs_open(&id, path,HACCESS_READ_WRITE, &error);

  if (error == HFS_OPEN_ERROR)

    display_prompt("WARNING: unable to open GPIB file", true);



  c_hfs_hpib_srq(id, &srq, &error);

  mla = srq + 32;



  command[0] = UNTALK;

  command[1] = UNLISTEN;

  command[2] = TALK + DMM_3458;   /* talk address for dmm */

  command[3] = mla;   /* my listen address */

  command[4] = '\0';

  c_hfs_hpib_send_command(id, command, 4, &error);



  nhfs_read_bytes(id, 1, MAX_SAMPLES * 4, t, num_samples * 4, &bytes_read,

                  &error);

  if (error == HFS_HPIB_READ_FAILED)

    display_prompt("Cannot read data from dmm in get_dmm_volt", true);



  command[2] = '\0';

  c_hfs_hpib_send_command(id, command, 2, &error);



  c_hfs_close(id, &error);

  if (error == HFS_CLOSE_ERROR)

    display_prompt("WARNING: unable to close dmm file", true);



  flush_commands(mod_id);



  //  display_prompt("starting to write capture.dmm file", true);

  //ofile = fopen("capture.dmm", "w");



  for (j = 0; j < num_samples; j++)

  {

    byte0 = t[0 + ((j)*4)];

    byte1 = t[1 + ((j)*4)];

    byte2 = t[2 + ((j)*4)];

    byte3 = t[3 + ((j)*4)];

    waveform[j] = floating_point(byte0, byte1, byte2, byte3);



    //fprintf(ofile, "%d %f\n", j, waveform[j]);

  } //for j loop



  //fclose(ofile);

  // display_prompt("finished writing capture.dmm", true);



  if (dmm_addr == DMM_ADDRESS) disable_dmm_buffer(mod_id);

  if (dmm_addr == DMM50_ADDRESS) disable_dmm50_buffer(mod_id);

}  /*get_dmm_volt*/



//------------------------------------------------------------------

//----------------------------------------------------------------------------

/* log_array_data:        mjs 6/98



  This is a generic routine to write information contained in an array to a file.  It

  is quicker to write the data after it has been collected instead of during.  Also, the

  code to open the file, print the data, etc doesn't have to be rewritten.



  Include these declarations at the beginning of the test you want to log the data for:



    double hm_array[30][2];  // n x 2 array where each row is a different data point

    double *ap;

    ap = &hm_array[0][0];

    int index=0;



  Write the data to the array as normal, remembering to increase the array index:

        hm_array[index][0] = low_bounds_ov;

        hm_array[index][1] = LOW_BOUNDS;

        index++;



  After all of the data has been collected, you can decide whether you want to print it

  to the file or not:



      if ((actual_voltage >= high_lim_9847) || (actual_voltage <= low_lim_9847))

          log_array_data(9847, V.driver-'A', V.mod_id, V.slot, ap, index, result.UU.Real_result);



 */



void log_array_data(int test, int subtest, int mod_id, int slot, double *ap, int index, double data_to_log)

{

  /*  test                   -> test number from which data comes

      subtest, mod_id, slot  -> self-explanatory

      ap                     -> array pointer

      index                  -> maximum index of the array

      data_to_log            -> self-explanatory

  */



  FILE *ofile; // file pointer

  int i;       // loop variable

  int dim=2;   // array is of dimension 2

  time_t now;



  ofile = fopen("data.out", "a");  // data.out file in dgn directory is appended with the latest data

  assert(ofile);



  for (i=0; i<index; i++)

  {

    fprintf(ofile, "t%d | sub %d | mod %d | slot %d |   %1.0f   %-+8.4f\n", test, subtest, mod_id, slot, (*((ap+0)+(dim*i))), (*((ap+1)+(dim*i))));

  }



  now = time(NULL);

  fprintf(ofile, "    data = %-+8.4f     time stamp:  %s\n", data_to_log, ctime(&now));

  fprintf(ofile, "-------------------------------------------------------------------------------\n");

  fclose(ofile);

}



// ********************************************************************************************************************

/* scope elimination project functions */

void setup_dmm_subsampling_AC_and_DC(int mod_id, int dmm_addr, int num_samples, double sampling_rate,

                                        int trig_level, int slope, int range)

{

  char setup_string[81];

  int next_position;



  vth_mhpib_wr(mod_id, dmm_addr, "PRESET FAST");

  vth_mhpib_wr(mod_id, dmm_addr, "MFORMAT SINT");

  vth_mhpib_wr(mod_id, dmm_addr, "OFORMAT SREAL");

  vth_mhpib_wr(mod_id, dmm_addr, "MEM FIFO");



//SSDC was chosen to make sure that the whole waveform was captured

  strcpy(setup_string, "SSDC ");   /* SSDC range */

  sprintf(setup_string + dmm_addr, "%d", range);

  next_position = strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, dmm_addr, setup_string);



  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */

  sprintf(setup_string + 6, "% .4E", sampling_rate);

  next_position = strlen(setup_string) + 1;

  strcat(setup_string, ",");

  sprintf(setup_string + next_position, "%d", num_samples);

  next_position = strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, dmm_addr, setup_string);



  strcpy(setup_string, "LEVEL ");   /* DCV range */

  sprintf(setup_string + 6, "%d", trig_level);

  next_position = strlen(setup_string) + 1;

  vth_mhpib_wr(mod_id, dmm_addr, setup_string);



if (slope == POS)

  vth_mhpib_wr(mod_id, dmm_addr, "SLOPE POS");

else

  vth_mhpib_wr(mod_id, dmm_addr, "SLOPE NEG");



  vth_mhpib_wr(mod_id, dmm_addr, "LFILTER OFF");

  vth_mhpib_wr(mod_id, dmm_addr, "TARM SGL");



  flush_commands(mod_id);

}/*setup_dmm_subsampling_AC_and_DC*/



/*******************************************************************/

void connect_cntr_ch1_af2(int mod_id)

{  /*connect_cntr_AF2*/

  /* USED BY DIGITAL FIXTURE ONLY */

  /* New Sales Rack configuration for Monet - JRM 2/22/93 */

  /* New counter implementation - ABR 7/6/99 */

 /* connect(mod_id, 502, true);

  connect(mod_id, 913, true);

  connect(mod_id, mod_id + 901, true);*/

  /* New Switchbox implementation - ABR 3/07/2024 */

  connect_34980A_relay(6402);

  connect_34980A_relay(7203);

  connect_34980A_relay(mod_id + 8301);

}  /*connect_cntr_AF2*/



/********************************************************************/

void connect_gen_out_cntr_ch1(int mod_id){

/* scope elimination project functions */

// new for scope elimination project

  /* USED BY DIGITAL FIXTURE ONLY */

/* ******************** for oldf switchbox ******************* */
  /*connect(mod_id, 522, true);

  connect(mod_id, 703, true);

  connect(mod_id, 504, true);*/
/* ******************************************************************** */

   connect_34980A_relay(6202);

   connect_34980A_relay(5403);

   connect_34980A_relay(6404);

}



/* ******************************************************************** */

void connect_dmm50_out_af2(int mod_id)

{

/* scope elimination project functions */

  /* USED BY ANALOG  FIXTURE ONLY */
	if (get_34980A_IDN() == true)
	{
		connect_34980A_relay(5103);//connect 53230A counter channel 1 to RF mux bank 1 101

		if (mod_id == 0)
		{
			connect_34980A_relay(5401);//connect RF mux com to bank 4 401 if module 0 //AF2
		}
		if (mod_id == 1)
		{
			connect_34980A_relay(5402);//connect RF mux com to bank 4 402 if module 1 //AF2
		}
		if (mod_id == 2)
		{
			connect_34980A_relay(5403);//connect RF mux com to bank 4 403 if module 2 //AF2
		}
		if (mod_id == 3)
		{
			connect_34980A_relay(5404);//connect RF mux com to bank 4 404 if module 3 //AF2
		}
	}
	else
	{
		connect(mod_id, 912, true);

		connect(mod_id, mod_id + 901, true);
	}
}  /*connect_dmm_out_AF2*/



/* ******************************************************************** */

void check_dmm50_button(int mod_id, int usage)

{

  test_result_type result;

  double dmm50_impedance;

  char buffer[80];

  int not_ok = 1;

  int fail = 0;



  // connect(mod_id, 822, false);  // for digital fikri 22/4/2024
  connect(mod_id, 7302, false);  // for digital fikri 22/4/2024

  // connect(mod_id, 912, false);  // for analog
  connect(mod_id, 7202, false);  // for analog



  open_dmm50(mod_id);

  setup_dmm50(mod_id, TWO_WIRE_OHMS, 100, 1);



  while ( (not_ok) && (fail <5))

  { // while not ok

    measure_dmm50(mod_id);



    if (get_result_nolog(mod_id, &result))

    {

      dmm50_impedance = result.UU.Real_result;



      if ( ((usage == DMM50_DIGITAL_USAGE) && (dmm50_impedance <= 100)) ||

           ((usage == DMM50_ANALOG_USAGE) && (dmm50_impedance >100)) )

      {

        not_ok=0;

        continue;

      }

      else

      {

        display_prompt("Incorrect usage of dmm50 for this rack!", true);

        display_prompt("   Use REAR terminals for DIGITAL", true);

        display_prompt("   Use FRONT terminals for ANALOG", true);

        display_prompt("  Please correct now and hit <Return> or <Enter>", true);

        get_user_answer(buffer, 80);

        fail++;

      }

    }

    else

    {

      fail++;

      display_prompt("Trouble reading impedance from dmm50...reset", true);

    }

  }//while not ok



  close_dmm50(mod_id);

}

/*----------------------------Test code for N6700B------------------------- */
//Added by chuanho, 28Jun2011

/*Open N6776A/N6734B*/
void open_olym(int mod_id)
{
	/*if (get_34980A_IDN() == false)
	{*/
		err = viOpen(rsm, OLYM, VI_NULL, VI_NULL, &viOLYM);
		if (err != 0)
		{
			display_prompt("Can't connect N6776A/N6734B.", true);
		}
		viSetAttribute(viOLYM, VI_ATTR_TMO_VALUE, HPIB_TO);
		viSetAttribute(viOLYM, VI_ATTR_TERMCHAR_EN, VI_TRUE);
		viPrintf(viOLYM, "*RST\n");
	//}
	/*else if(get_34980A_IDN() == true)
	{
		err = viOpen(rsm, OLYM, VI_NULL, VI_NULL, &viOLYM);
		if (err != 0)
		{
			display_prompt("Can't connect N6776A.", true);
		}
		viSetAttribute(viOLYM, VI_ATTR_TMO_VALUE, HPIB_TO);
		viSetAttribute(viOLYM, VI_ATTR_TERMCHAR_EN, VI_TRUE);
		viPrintf(viOLYM, "*RST\n");
	}*/
}
/*End of open N6746B or N6746B*/

/*Verify N6700B or N6700C*/
void verify_olym(int mod_id)
{
	/*if (get_34980A_IDN() == false)
	{
		ViStatus  hw_err;
		char buf1[256] = { 0 };
		char str1[256] = { 0 };

		err = viQueryf(viOLYM, "*IDN?", "%t", &buf);
		hw_err = viQueryf(viOLYM, "SYST:ERR?", "%t", &buf1);
		if (err != 0 && hw_err != 0)
		{
			display_prompt("N6734B/N6776A not responding to HPIB queries", true);
		}
		else
		{
			sprintf(str1, "%.38s", buf);
			if ( (!strcmp(str1, "N6734B")) || (!strcmp(str1, "N6776A")) )
			{
				display_prompt(str1, true);
				display_prompt("   N6734B/N6776A Power Supply Okay", true);
			}
			else
			{
				display_prompt("   N6734B/N6776A are not connected", true);
			}

		}
	}
	else if (get_34980A_IDN() == true)
	{*/
		ViStatus  hw_err;
		char buf1[256] = { 0 };
		char str1[256] = { 0 };

		err = viQueryf(viOLYM, "SYST:CHAN:MOD? (@1)", "%t", &buf); //"*IDN?", "%t", &buf);
		hw_err = viQueryf(viOLYM, "SYST:ERR?", "%t", &buf1);
		if (err != 0 && hw_err != 0)
		{
			display_prompt("N6734B/N6776A not responding to GPIB queries\n", true);
		}
		else
		{

			sprintf(str1, "%.38s", buf);
			if (!strcmp(str1, "N6734B\n"))
			{
				display_prompt(str1, true);
				display_prompt("   N6734B Power Supply Okay\n", true);
			}

		    else if (!strcmp(str1, "N6776A\n"))
			{
				display_prompt(str1, true);
				display_prompt("   N6776A Power Supply Okay\n", true);
			}
			else
			{
				display_prompt("   N6734B/N6776A are not connected\n", true);
			}
		}
	//}
}
/*End of Verify N6700B or N6700C*/

/*Open 34980A*/
void open_34980A(int mod_id)//chun12112021
{
	err = viOpen(rsm, MUX, VI_NULL, VI_NULL, &viMUX);
	if (err != 0)
	{
		display_prompt("Can't connect 34980A.\n", true);
		//return false;
	}
	viSetAttribute(viMUX, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viMUX, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viMUX, "*RST\n"); //temp comment out on 09122021 chun
	//return true;
}
/*End of open 34980A*/

/*Verify 34980A*/
boolean verify_34980A(int mod_id)//chun12112021
{
	ViStatus  hw_err;
	char buf[256] = { 0 };
	char str1[256] = { 0 };

	err = viQueryf(viMUX, "*IDN?", "%t", &buf);  //buf for IDN
	hw_err = viQueryf(viMUX, "SYST:ERR?", "%t", &buf);  //buf1 for error
	if (err != 0 && hw_err != 0)
		{
		display_prompt("34980A not responding to GPIB queries\n", true);
		return false;
		}
	else
	{
		//sprintf(str1, "%.38s", buf);
		sprintf(str1, "%s", buf);
		//display_prompt("   34980A switchbox Okay", true);
		//Agilent Technologies,34980A,MY56485976,2.51-2.43-2.07-0.00
		if (!strcmp(str1, "Agilent Technologies,34980A,MY56485976,2.51-2.43-2.07-0.00\n")) //1st switch
		{
			//display_prompt(str1, true);
			display_prompt("   34980A switchbox Okay\n", true);
			return true;
		}
		else if (!strcmp(str1, "Agilent Technologies,34980A,MY44006382,2.43-2.25-2.07-1.05\n")) //2nd switch
		{
			//display_prompt(str1, true);
			display_prompt("   34980A switchbox Okay\n", true);
			return true;
		}
		else if (!strcmp(str1, "Agilent Technologies,34980A,MY62360001,2.51-2.43-2.07-0.00\n"))//3rd switch
		{
			//display_prompt(str1, true);
			display_prompt("   34980A switchbox Okay\n", true);
			return true;
		}
		else if (!strcmp(str1, "Agilent Technologies,34980A,MY62360007,2.51-2.43-2.07-0.00\n")) //4th switch
		{
			//display_prompt(str1, true);
			display_prompt("   34980A switchbox Okay\n", true);
			return true;
		}
	    else
		{
			display_prompt("   34980A switchbox not connected\n", true);
			return false;
		}
	}

}
/*End of verify 34980A*/

/* ******************************************************************** */

/*Get 34980A IDN*/
boolean get_34980A_IDN(void)//chun12112021
{
	ViStatus  hw_err;
	char buf[256] = { 0 };
	char buf1[256] = { 0 };
	char str1[256] = { 0 };

	err = viQueryf(viMUX, "*IDN?", "%t", &buf);  //buf for IDN
	hw_err = viQueryf(viMUX, "SYST:ERR?", "%t", &buf1);  //buf1 for error
	//display_prompt("ERR : " + err , true);  // quah 3/3/25 trace error
	//display_prompt("HW_ERR: "+ hw_err, true); // quah 3/3/25 trace error
	if (err != 0 && hw_err != 0)
	{
		//display_prompt("Inside ERR & HW_ERR" , true);
		display_prompt("34980A not responding to GPIB queries", true);
		return false;
	}
	else
	{	
		//display_prompt("HERE2 "  , true);// quah 3/3/25 trace error
		sprintf(str1, "%.38s", buf);
		//display_prompt("   34980A switchbox Okay", true);
		if (!strcmp(str1, "Agilent Technologies,34980A,MY56485976"))//,2.51-2.43-2.07-0.00\n")) //1st switch
		{
			return true;
		}
		else if (!strcmp(str1, "Agilent Technologies,34980A,MY44006382"))//,2.43-2.25-2.07-1.05\n")) //2nd switch
		{
			return true;
		}
		else if (!strcmp(str1, "Agilent Technologies,34980A,MY62360001"))//,2.51-2.43-2.07-0.00\n"))//3rd switch
		{
			return true;
		}
		else if (!strcmp(str1, "Agilent Technologies,34980A,MY62360007"))//,2.51-2.43-2.07-0.00\n")) //4th switch
		{
			return true;
		}
		else
		{
			//display_prompt("   34980A switchbox not connected", true);
			return false;
		}
	}

}
/*End of Get 34980A IDN*/

/* ******************************************************************** */

void connect_34980A_relay(int connection)//(int mod_id, int connection, int status)//chun 19112021
{

	/* USED BY BOTH DIGITAL AND ANALOG FIXTURE */
	//char buf1[256] = { 0 };
	//char close_relay_command[256] = { 0 };

	char relay[256] = { 0 };
	char routeclose_start[256] = "ROUTE:CLOSE (@";
	sprintf(relay, "%d", connection);
	char routeclose_end[256] = ")\n";

	strcat(routeclose_start, relay);
	strcat(routeclose_start, routeclose_end);

	err = viOpen(rsm, MUX, VI_NULL, VI_NULL, &viMUX);
	viSetAttribute(viMUX, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viMUX, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viMUX, routeclose_start);
	/*strcat(s, "OPEN ");

	start = mtd_strlen(s) + 1;

	sprintf(s + start - 1, "%d", connection);*/

	//close_relay_command = "ROUTE:CLOSE (" + "@" + connection + "\n" + ")";
	//if (verify_34980A() == true)
	//{
		//viPrintf(viMUX, routeclose_start);
		//viPrintf(viMUX, "ROUTE:CLOSE (@1001)\n");
		//err = viQueryf(viMUX, "ROUTE:CLOSE (@" + connection + ")" , "%t", &buf1);
	//}



}  /*connect*/

void disconnect_34980A_relay(int connection)//(int mod_id, int connection, int status)//chun 19112021
{

	/* USED BY BOTH DIGITAL AND ANALOG FIXTURE */
	//char buf1[256] = { 0 };
	//char close_relay_command[256] = { 0 };
	char relay[256] = { 0 };
	char routeopen_start[256] = "ROUTE:OPEN (@";
	sprintf(relay, "%d", connection);
	char routeopen_end[256] = ")\n";

	strcat(routeopen_start, relay);
	strcat(routeopen_start, routeopen_end);

	err = viOpen(rsm, MUX, VI_NULL, VI_NULL, &viMUX);
	viSetAttribute(viMUX, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viMUX, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viMUX, routeopen_start);


	/*err = viOpen(rsm, MUX, VI_NULL, VI_NULL, &viMUX);
	viSetAttribute(viMUX, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viMUX, VI_ATTR_TERMCHAR_EN, VI_TRUE);*/

	//close_relay_command = "ROUTE:CLOSE (" + "@" + connection + "\n" + ")";
	//if (verify_34980A() == true)
	//{
	//viPrintf(viMUX, "ROUTE:OPEN (@1001)\n");
	//err = viQueryf(viMUX, "ROUTE:CLOSE (@" + connection + ")" , "%t", &buf1);
//}

}  /*disconnect*/

boolean get_53132A_IDN(void)//chun17052022
{
	ViStatus  hw_err;
	char buf[256] = { 0 };
	char buf1[256] = { 0 };
	char str1[256] = { 0 };

	err = viQueryf(viCNTR, "*IDN?", "%t", &buf);  //buf for IDN
	hw_err = viQueryf(viCNTR, "SYST:ERR?", "%t", &buf1);  //buf1 for error
	if (err != 0 && hw_err != 0)
	{
		return false;
		display_prompt("53132A not responding to GPIB queries", true);
	}
	else
	{
		sprintf(str1, "%.38s", buf);
		//display_prompt("   34980A switchbox Okay", true);

		//if (!strcmp(str1, Agilent Technologies, 53230A) //MY61400282, 03.02 - 1924.2831 - 3.15 - 4.16 - 127 - 159 - 35
		if (!strcmp(str1,"HEWLETT-PACKARD,53132A,0,4806"))//(!strcmp(str1, "Keysight Technologies,E4980AL,MY54412993"))
		{
			//display_prompt(str1, true);
			//display_prompt("   34980A switchbox Okay", true);
			return true;
		}
		else
		{
			//display_prompt("   34980A switchbox not connected", true);
			return false;
		}
	}

}


/*Get 53230A IDN*/
boolean get_53230A_IDN(void)//chun17052022
{
	ViStatus  hw_err;
	char buf[256] = { 0 };
	char buf1[256] = { 0 };
	char str1[256] = { 0 };

	err = viQueryf(viCNTR, "*IDN?", "%t", &buf);  //buf for IDN
	hw_err = viQueryf(viCNTR, "SYST:ERR?", "%t", &buf1);  //buf1 for error
	if (err != 0 && hw_err != 0)
	{
		return false;
		display_prompt("53230A not responding to GPIB queries", true);
	}
	else
	{
		sprintf(str1, "%.38s", buf);
		//display_prompt("   34980A switchbox Okay", true);

		if(!strcmp(str1,"Agilent Technologies, 53230A"))
		//MY61400282, 03.02 - 1924.2831 - 3.15 - 4.16 - 127 - 159 - 35
		//if (!strcmp(str1,"HEWLETT-PACKARD,53132A,0,4806"))
		//(!strcmp(str1, "Keysight Technologies,E4980AL,MY54412993"))
		{
			//display_prompt(str1, true);
			//display_prompt("   34980A switchbox Okay", true);
			return true;
		}
		else
		{
			//display_prompt("   34980A switchbox not connected", true);
			return false;
		}
	}

}

void setup_counter_53230A_conf_total_cont(void)//chun 17052022
{
	char command[256]; //= "CONF:TOT:CONT (@1)";//":CONFigure:TOTalize:CONTinuous";
	*command = '\0';
	sprintf(command, "%s\n", ":CONFigure:TOTalize:TIMed 5s");//"CONF:TOT:CONT (@1)");
	viPrintf(viCNTR, command);

	//char command[256] = "CONF:TOT:CONT (@1)";//":CONFigure:TOTalize:CONTinuous";

	//err = viOpen(rsm, CNTR, VI_NULL, VI_NULL, &viCNTR);
	//viSetAttribute(viCNTR, VI_ATTR_TMO_VALUE, HPIB_TO);
	//viSetAttribute(viCNTR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	//viPrintf(viCNTR, command);

}  /*send_SCPI_command*/

void setup_counter_53230A_inp_lev_0p5(void)
{
	char command[256];
	*command = '\0';
	sprintf(command, "%s\n", ":EVENT1:LEVEL 0.5");//"INP:LEV .5");
	viPrintf(viCNTR, command);

}

void setup_counter_53230A_inp_slop_neg(void)
{
	char command[256];
	*command = '\0';
	sprintf(command, "%s\n", "INP:SLOP NEG");
	viPrintf(viCNTR, command);

}

void setup_counter_53230A_init(void)//chun 17052022
{
	char command[256];
	*command = '\0';
	sprintf(command, "%s\n", ":init:continuous on");//"INIT");
	viPrintf(viCNTR, command);

	//char command[256] = "INIT";

	//err = viOpen(rsm, CNTR, VI_NULL, VI_NULL, &viCNTR);
	//viSetAttribute(viCNTR, VI_ATTR_TMO_VALUE, HPIB_TO);
	//viSetAttribute(viCNTR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	//viPrintf(viCNTR, command);

}  /*send_SCPI_command*/

void setup_counter_53230A_abort(void)//chun 17052022
{
	char command[256];
	*command = '\0';
	sprintf(command, "%s\n", "ABOR");
	viPrintf(viCNTR, command);

	//char command[256] = "ABOR";

	//err = viOpen(rsm, CNTR, VI_NULL, VI_NULL, &viCNTR);
	//viSetAttribute(viCNTR, VI_ATTR_TMO_VALUE, HPIB_TO);
	//viSetAttribute(viCNTR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	//viPrintf(viCNTR, command);

}  /*send_SCPI_command*/

double read_counter_53230A_fetch(void)
{
	double counter_fetch_result;
	char str1[256] = { 0 };
	//err = viQueryf(viCNTR, "MEASure:TOTalize:TIMed?", "%t", &buf);//"FETC?\n", "%t", &buf);
	err = viQueryf(viCNTR, "FETCh?", "%t", &buf);//"FETC?\n", "%t", &buf);

	if (err != 0)
	{
		printf("GPIB command pending on 53230A!!!");
	}
	else
	{
		sprintf(str1, "%.50s", buf);
		counter_fetch_result = atof(str1);
		//display_prompt(str1, true);
		return counter_fetch_result;
	}
}

void setup_counter_53230A_sense_event1_level_Absolute_auto_0(void)//chun 17052022
{
	char command[256];
	*command = '\0';
	sprintf(command, "%s\n", ":SENSe:EVENt1:LEVel:ABSolute:AUTO OFF");
	viPrintf(viCNTR, command);

	/*char command[256] = ":SENSe:EVENt1:LEVel:ABSolute:AUTO OFF";

	err = viOpen(rsm, CNTR, VI_NULL, VI_NULL, &viCNTR);
	viSetAttribute(viCNTR, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viCNTR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viCNTR, command);*/

}  /*send_SCPI_command*/

void setup_counter_53230A_event1_level_0d35(void)//chun 17052022
{
	char command[256];
	*command = '\0';
	sprintf(command, "%s\n", ":EVENt1:LEVel 0.35");
	viPrintf(viCNTR, command);

	/*char command[256] = ":EVENt1:LEVel 0.35";

	err = viOpen(rsm, CNTR, VI_NULL, VI_NULL, &viCNTR);
	viSetAttribute(viCNTR, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viCNTR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viCNTR, command);*/

}  /*send_SCPI_command*/

void setup_counter_53230A_init_continuous_on(void)//chun 17052022
{
	char command[256];
	*command = '\0';
	sprintf(command, "%s\n", ":INIT:CONTinuous ON");
	viPrintf(viCNTR, command);

	/*char command[256] = ":INIT:CONTinuous ON";

	err = viOpen(rsm, CNTR, VI_NULL, VI_NULL, &viCNTR);
	viSetAttribute(viCNTR, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viCNTR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viLCR, command);*/

}  /*send_SCPI_command*/

void setup_counter_53230A_init_continuous_off(void)//chun 17052022
{
	char command[256];
	*command = '\0';
	sprintf(command, "%s\n", ":INIT:CONTinuous OFF");
	viPrintf(viCNTR, command);

	/*char command[256] = ":INIT:CONTinuous OFF";

	err = viOpen(rsm, CNTR, VI_NULL, VI_NULL, &viCNTR);
	viSetAttribute(viCNTR, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viCNTR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viLCR, command);*/

}  /*send_SCPI_command*/

void setup_N6746B_ch2_output(int x)//set N6746B module 2 output //chun 20052022
{
	char setup_string[81];
	*setup_string = '\0';

	if (x == 1)//output on
	{
		//*setup_string = '\0';
		sprintf(setup_string, "%s\n", "OUTP ON,(@2)");//(setup_string, "%s %1.3f %s\n", "VOLT", amplitude, ", (@2)");
		viPrintf(viPS, setup_string);
	}
	else if (x == 0)//output off
	{
		//*setup_string = '\0';
		sprintf(setup_string, "%s\n", "OUTP OFF,(@2)");//(setup_string, "%s %1.3f %s\n", "VOLT", amplitude, ", (@2)");
		viPrintf(viPS, setup_string);
	}
}

void setup_N6754A_ch1_output(int x)//set N6754A module 1 output //chun 20052022
{
	char setup_string[81];
	*setup_string = '\0';

	if (x == 1)//output on
	{
		//*setup_string = '\0';
		sprintf(setup_string, "%s\n", "OUTP ON,(@1)");//(setup_string, "%s %1.3f %s\n", "VOLT", amplitude, ", (@2)");
		viPrintf(viPS_N6754A, setup_string);
	}
	else if (x == 0)//output off
	{
		//*setup_string = '\0';
		sprintf(setup_string, "%s\n", "OUTP OFF,(@1)");//(setup_string, "%s %1.3f %s\n", "VOLT", amplitude, ", (@2)");
		viPrintf(viPS_N6754A, setup_string);
	}
}

/*Setup N6746B Voltage - 6634B replacement)*/ //chun 20052022
void setup_N6746B_ch2_volt(int mod_id, double amplitude)
{
	char setup_string[81];

	/* SETUP THE AMPLITUDE */
	*setup_string = '\0';
	sprintf(setup_string, "%s %1.3f %s\n", "VOLT", amplitude, ", (@2)");
	viPrintf(viOLYM, setup_string);
}

/*Setup N6746B channel 2 Current - 6634B replacement*/ //20052022
void setup_N6746B_ch2_curr(int mod_id, double current)
{
	char setup_string[81];

	/* SETUP THE CURRENT */
	*setup_string = '\0';
	sprintf(setup_string, "%s %1.3f %s\n", "CURR", current, ", (@2)");
	viPrintf(viOLYM, setup_string);
}

/*Setup N6746B channel 2 delay - 6634B replacement*/
void setup_N6746B_ch2_delay(double x)
{
	char setup_string[81];

	/* SETUP THE CURRENT */
	*setup_string = '\0';
	sprintf(setup_string, "%s %1.3f %s\n", "OUTP:PROT:DEL ", x, ", (@2)");
	//sprintf(setup_string, "%s %1.3f %s\n", "CURR:PROT:DEL ", x , ", (@2)");
	viPrintf(viPS, setup_string);

}

/*Setup N6776A or N6734B Voltage*/
void setup_olym_ch1_volt(int mod_id, double amplitude)
{
	char setup_string[81];

	/* SETUP THE AMPLITUDE */
    *setup_string = '\0';
    sprintf(setup_string, "%s %1.3f %s\n", "VOLT", amplitude, ", (@1)");
	viPrintf(viOLYM, setup_string);
}

/*Setup N6776A or N6734B channel 1 Current*/
void setup_olym_ch1_curr(int mod_id, double current)
{
	char setup_string[81];

	/* SETUP THE CURRENT */
	*setup_string = '\0';
	sprintf(setup_string, "%s %1.3f %s\n", "CURR", current, ", (@1)");
	viPrintf(viOLYM, setup_string);
}
/*End of setup N6700B channel 1 Current*/

/*Setup N6700B channel 1 OCP*/
void setup_olym_ch1_ocp(int mod_id, double current)
{
	char setup_string[81];

	/* SETUP THE OCP*/
	*setup_string = '\0';
	sprintf(setup_string, "%s %1.3f %s\n", "CURR", current, ", (@1)");
	viPrintf(viOLYM, setup_string);
}
/*end of setup N6700B channel 1 OCP*/

/*Reverse N6700B channel 1 output*/
void reverse_olym_ch1_out(int mod_id)
{
	viPrintf(viOLYM, "OUTP:REL:POL REV, (@1)\n");
}
/*End of reverse N6700B channel 1 output*/

/*Enable N6700B channel 1 output*/
void enable_olym_ch1_out(int mod_id)
{
	viPrintf(viOLYM, "OUTP ON,(@1)\n");
}
/*End of enable N6700B channel 1 output*/

/*Disable N6700B channel 1 output*/
void disable_olym_ch1_out(int mod_id)
{
	viPrintf(viOLYM, "OUTP OFF,(@1)\n");
}
/*End of disable N6700B channel 1 output*/

/*Close N6700B*/
void close_olym(int mod_id)
{
	if (get_34980A_IDN() == false)
	{

		char buf1[256] = { 0 };
		err = viQueryf(viOLYM, "*OPC?\n", "%t", &buf1);
		if (err != 0)
		{
			printf("GPIB command pending on N6776A/N66734B!!!");

		}
		else
		{
			viPrintf(viOLYM, "*RST\n");
		}
		viClose(viOLYM);
	}
	else if (get_34980A_IDN() == true)
	{

		char buf1[256] = { 0 };
		err = viQueryf(viOLYM, "*OPC?\n", "%t", &buf1);
		if (err != 0)
		{
			printf("GPIB command pending on N6776A!!!");

		}
		else
			viPrintf(viOLYM, "*RST\n");
		viClose(viOLYM);
	}
}
/*End of close N6700B*/

/* ---------------------End Of test code for N6700B------------------------- */

/*Close 34980A*/
void close_34980A(int mod_id)//chun 12112021
{
	char buf1[256] = { 0 };
	err = viQueryf(viMUX, "*OPC?\n", "%t", &buf1);
	if (err != 0)
	{
		printf("GPIB command pending on 34980A!!!");

	}
	else
	{
		viPrintf(viMUX, "*RST\n"); //temp comment out chun on 09122021
	}

	viClose(viMUX);
}
/*End of close 34980A*/
