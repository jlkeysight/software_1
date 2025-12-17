/*
 *-----------------------------------------------------------------------------
 * (c) Copyright 2003, Agilent Technologies, all rights reserved.
 *-----------------------------------------------------------------------------
 */
#pragma warning( disable:4786)
//
#pragma comment(lib, "visa32.lib")
#pragma comment(lib, "sicl32.lib")
#include <p2c/p2c.h>
#include <cassert>
#include <utility>
#include <time.h>
#include <visa.h>
#include <stdio.h>
#include <stdlib.h>
#include "RelayPath.h"
#include "../../../uCC/src/future.h"
#include "../../../u/src/num_p.h"
#include "../../../vth/src/MetaTesthead.h"
#include "../../../u/src/DebugWriter.h"
#include "../../../vth/src/aimasm.h"
#include "../../../vth/src/vth_aim.h"
#include "../../../vth/src/vth_mac.h"
#include "../../../vth/src/ASRU_C.h"
#include "../../../vth/src/DigitalChannelParameters.h"
#include "../../../vth/src/ConnectDigitalResource.h"
#include "../../../vth/src/HPinDigitalSetup.h"
#include "../../../vth/src/ConnectBRCtoBus.h"
#include "../../../vth/src/IPin.h"
#include "../../../vth/src/vthsup_p.h"
#include "../i5000Tests/TestTalon.h"
#include "../i5000Tests/TestPin.h"
#include "../i5000Tests/TestRelays.h"
/* Mike what headers can we remove from the following list??*/
#include "../../../u/src/umath.h"
#include "../../../u/src/OSLayer.h"
#include "../parms_p.h"
#include "../../../dgn/src/dgnconfig_p.h"
#include "../../../dgn/src/termio_p.h"
#include "../../../dgn/src/testdec_p.h"
#include "../../../dgn/src/execsup_p.h"
#include "../../../dgn/src/strdec.h"
#include "../../../u/src/hdw_p.h"
#include "../../../u/src/newhfs_p.h"
#include "../../../vth/src/capasm_p.h"
#include "../../../vth/src/macasm_p.h"
#include "../../../vth/src/pinasm_p.h"
#include "../../../vth/src/result_p.h"
#include "../../../vth/src/vth_p.h"
#include "../../../vth/src/strings_p.h"

using namespace std;

ViSession rsm; 
ViSession viDMM;
ViSession viDMM50;
ViSession viPS;
ViSession viPS_LOAD;
ViSession viCNTR;
ViSession viSYNTH;
ViSession viLCR;
ViSession viMUX;
ViSession viGEN;
ViSession viOLYM;
ViStatus  err;
char buf[256] = {0};

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
#undef KDG1
#undef KDG2
#undef KDG3
#undef KDG4
#undef KDG5
#undef KDG6
#undef KDG7
#undef KDG8
#undef KDG9
#undef KDG10
#undef KDG11
#undef KDG12

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

const aim_x_relays AIM_X_PLUS_1_MINT_RLYS = {K742, K743, K744, K745, K746, K747, K748, K741};
const aim_x_relays AIM_X_MINT_RLYS = {K741, K742, K743, K744, K745, K746, K747, K748};
const aim_x_relays AIM_X_S_RLYS = {K733, K734, K735, K736, K737, K738, K739, K740};
const aim_x_relays AIM_X_I_RLYS = {K725, K726, K727, K728, K729, K730, K731, K732};
const aim_x_relays AIM_X_L_RLYS = {K717, K718, K719, K720, K721, K722, K723, K724};
const aim_x_relays AIM_X_A_RLYS = {K709, K710, K711, K712, K713, K714, K715, K716};
const aim_x_relays AIM_X_B_RLYS = {K701, K702, K703, K704, K705, K706, K707, K708};
/*const chan_relays PIN_KR_RLYS = {KRA, KRB, KRC, KRD, KRE, KRF, KRG, KRH};
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
};*/


/* ------------------------------------------------------------------------ */

void flush_commands(int mod_id)
{
  test_result_type result;

  if (get_result_nolog(mod_id, &result))
    display_prompt("Unexpected result pending.", true);
}  

/* ------------------------------------------------------------------------ */

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
  current_position = strlen(s) + 1;
  if (current_position < 80)
  {  /* attempt to write the integer */
    sprintf(s + current_position - 1, "%d", i);
    next = strlen(s) + 1;
  }  /* attempt to write the integer */
  current_position = strlen(s) + 1;
  if (current_position < 80)
  {  /* attemp to write the real */
    strcat(s, " ");
    sprintf(s + current_position, "%1.12f", r);
    next = strlen(s) + 1;
  }  /* attemp to write the real */
  display_prompt(s, true);

}

/* ------------------------------------------------------------------------ */

void TALON_LOADS::store_measurements(int mod_id, int slot)
{
	r6_5a[mod_id][slot] = res[mod_id][slot][0];
	r50a[mod_id][slot] = res[mod_id][slot][1];
	r1ka[mod_id][slot] = res[mod_id][slot][2];
	r6_5b[mod_id][slot] = res[mod_id][slot][3];
	r50b[mod_id][slot] = res[mod_id][slot][4];
	r1kb[mod_id][slot] = res[mod_id][slot][5];
}

/* ------------------------------------------------------------------------ */

void RACK_DELAYS::ReadDelays()
{
   int this_module;
   int a_module;
   char rack_serial_number[81];
   char filename[81];
   char buffer[81];

   FILE *rack_delays = NULL;
   FILE *measured_delays = NULL;
   
   this_module = any_module_present();
   get_sales_rack_serial_number(rack_serial_number, this_module);
   strcpy(buffer, testwhere_path);
   strcat(buffer, rack_serial_number);
   if (strpos2("A", rack_serial_number, 1) > 0)
   {
	   if((rack_delays = fopen(buffer, "r")) == NULL)
	   {
		   display_prompt("The sales rack serial number file was not found.  Please have sys admin replace.", true);
		   _EscIO(FileNotFound);
	   }
	   for (a_module = 0; a_module <= 3; a_module++)
	   {
		   fscanf(rack_delays, "%lg%*[^\n]", &clkrb_delay[a_module]);
		   getc(rack_delays);
	   }
	   for (a_module = 0; a_module <= 3; a_module++)
	   {
		   fscanf(rack_delays, "%lg%*[^\n]", &af2_start_delay[a_module]);
		   getc(rack_delays);
	   }
	   for (a_module = 0; a_module <= 3; a_module++)
	   {
		   fscanf(rack_delays, "%lg%*[^\n]", &af2_clkrb_delay[a_module]);
		   getc(rack_delays);
	   }
	   for (a_module = 0; a_module <= 3; a_module++)
	   {
		   fscanf(rack_delays, "%lg%*[^\n]", &probe_clkrb_delay[a_module]);
		   getc(rack_delays);
	   }
	   if (rack_delays != NULL)
		   fclose(rack_delays);
	   
	   rack_delays = NULL;
   }
   else
	   display_prompt("Need to define sales rack serial number (T16005)", true);
   
   sprintf(filename,"%sAF2_delays", testwhere_path); 
   if((measured_delays = fopen(filename, "r")) == NULL)
   {
	   display_prompt("The 'AF2_delays' file was not found.  Please have sys admin replace.", true);
	   _EscIO(FileNotFound);
   }
   for (a_module = 0; a_module <= 3; a_module++)
   {
	   fscanf(measured_delays, "%lg%*[^\n]", &af2_system_delay[a_module]);
	   getc(measured_delays);
   }
   if (measured_delays != NULL)
	   fclose(measured_delays);
   
   measured_delays = NULL;
   if (rack_delays != NULL)
	   fclose(rack_delays);
   rack_delays = NULL;   
}

/* ------------------------------------------------------------------------ */

double RACK_DELAYS::AF2Start(int mod_id)
{
	return af2_start_delay[mod_id];
}

/* ------------------------------------------------------------------------ */

double RACK_DELAYS::AF2Clkrb(int mod_id)
{
	return af2_clkrb_delay[mod_id];
}

/* ------------------------------------------------------------------------ */

double RACK_DELAYS::AF2System(int mod_id)
{
	return af2_system_delay[mod_id];
}

/* ------------------------------------------------------------------------ */

double RACK_DELAYS::ClkrbDelay(int mod_id)
{
	return clkrb_delay[mod_id];
}

/* ------------------------------------------------------------------------ */

double RACK_DELAYS::ProbeDelay(int mod_id)
{
	return probe_clkrb_delay[mod_id];
}

/* ------------------------------------------------------------------------ */

void show_present_test_info(int present_test_num, int mod_id, int present_slot_num, char * s)
{
  int start;
  int next;
  *s = '\0';
  start = 1;
  strcat(s, "Test ");
  start = strlen(s) + 1;
  sprintf(s + start - 1, "%d", present_test_num);
  next = strlen(s) + 1;
  strcat(s, "  Module ");
  start = strlen(s) + 1;
  sprintf(s + start - 1, "%d", mod_id);
  next = strlen(s) + 1;
  strcat(s, "  Slot ");
  start = strlen(s) + 1;
  sprintf(s + start - 1, "%d", present_slot_num);
  next = strlen(s) + 1;
}

/* ------------------------------------------------------------------------ */
void sales_start_sequencer_mac(int mod_id)
{
  vth_mseq_dbg_setup(mod_id, 1, 0xffffffff, 0xffffff, 0);
  vth_mseq_run(mod_id);
}  /* sales_start_sequencer_mac */

/* ------------------------------------------------------------------------ */
void sales_reset_sequencer_aim(int mod_id)
{
  vth_mrst_seq(mod_id);
  vth_afc_rst_soft(mod_id);
}  /* sales_reset_sequencer_aim */

/* ------------------------------------------------------------------------ */
void convert_seconds_nanoseconds(double *value)
{
  (*value) = (*value) * 1e+09;
}  

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
    start = strpos2("=", s, 1) + 1;   /* READING CNTR OR LCR METER */

    if (start == 1)   /* READING THE POWER SUPPLY SENSE format: VSET XX.XX */
      start = strpos2(" ", s, 1) + 1;

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

/* ------------------------------------------------------------------------ */

double edge_placement_calc(int mod_id, int slot, int slope, double offset, 
						   double drvr_delay_actual)
{
	double value = 0.0;
	double edge_delay = 60e-09; 
	RACK_DELAYS Rack;
	Rack.ReadDelays();

    value = offset + Rack.AF2Clkrb(mod_id) - drvr_delay_actual - Rack.AF2Start(mod_id) 
		- SLOT_DELAY[slot - 1][slope - 1] - Rack.AF2System(mod_id) + edge_delay;

	return value;
}


/* ------------------------------------------------------------------------ */

#define MAX_LOOPS       3

bool counter_measurement_pending(int mod_id)
{

  /* This function determines whether or not a measurement is pending on
     the SALES rack 53131 counter.  The return is a corresponding bool */
  int i;
  double stat;


  viPrintf(viCNTR, "*RST;*CLS;*SRE 0;*ESE 0\n");
  viPrintf(viCNTR, ":STAT:PRES\n");
  viPrintf(viCNTR, ":STAT:OPER:PTR 0;NTR 16\n");
  viPrintf(viCNTR, ":STAT:OPER:ENABLE 16\n");
  viPrintf(viCNTR, "*SRE 128\n");
  viPrintf(viCNTR, ":FUNC 'FREQ 2'\n");
  viPrintf(viCNTR, ":FREQ:ARM:STAR:SOUR IMM\n");
  viPrintf(viCNTR, ":FREQ:ARM:STOP:SOUR TIM\n");
  viPrintf(viCNTR, ":FREQ:ARM:STOP:TIM .01\n");
  viPrintf(viCNTR, ":INP2:COUP DC\n");
  viPrintf(viCNTR, ":INP2:IMP 50\n");
  setup_cntr_trg_levels(mod_id, 1.3, 1.3);
  viPrintf(viCNTR, ":INIT\n");

  i = 1;
  do
  {
    i++;
	viQueryf(viCNTR, "*STB?\n", "%Lf", &stat);
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

  strcat(s, "*** Test \n");
  current_position = strlen(s) + 1;
  sprintf(s + current_position - 1,
          "%d", present_test[mod_id - NO_TESTHEAD_NUMBER]);
  next_position = strlen(s) + 1;
  strcat(s, ", Module \n");
  current_position = strlen(s) + 1;
  sprintf(s + current_position - 1, "%d", mod_id);
  next_position = strlen(s) + 1;
  strcat(s, " did not find a result in the result log.");
  display_prompt(s, true);
  display_prompt(specific, true);

  *s = '\0';
  display_prompt("*****************************************************", true);
  display_prompt("", true);

}  /*display_error*/



/*
*--------------------------------------------------------------------------------
* Test: talon_buzz_that_relay
* Description: 
*		(bFull = false) Open and close relay for 5*5 times (5 readings)
*		(bFull = true) Open and close relay for 5*5 times, calculate delta of max and min meas of 5 readings.	
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/

void talon_buzz_that_relay(int mod_id, HTalon::RelayName const rlyName, HTalon * talonCard, bool bFull)
{
	test_result_type resultBuzz;
	double aryResult[5], dblMax, dblMin;
	vth_rlc_setup(mod_id,0,0.1,0.0,0.001,1,0,0,1,0,0,0,0,0,0,-5.0,0.001); 
	vth_resistor_meas(mod_id, 0, 1);

	for (int nLoop=0; nLoop<5; nLoop++)
	{
		vth_rlc_setup(mod_id,
					  0,      /* 0=DC, 128=128Hz, 1024=1024Hz, 8192=8192Hz */
					  0.1,    /* 0.0 to +/-10.0 Volts (DC), 0.0 to 7.0 Volts RMS (AC)*/
					  0.0,    /* 0.0 to +/-10.0 (depends on SRC_AMP) Volts (DC) */
					  0.001,  /* 0.0 to 10.0 seconds */
					  1,      /* 0=30mA, 1=150mA */
					  0,      /* 0=no en, 1=en */
					  0,      /* 0=no ed, 1=ed */
					  1,      /* 1=10ohms, 2=100ohms, 3=1Kohms, 4=10Kohms, 5=100Kohms, 6=1Mohms */
					  0,      /* 0=no sa, 1=sa */
					  0,      /* 0=no sb, 1=sb */
					  0,      /* 0=no sl, 1=sl */
					  0,      /* 0=narrowband, 1=wideband */
					  0,      /* 0=not used */
					  0,      /* 0=single ended, 1=differential NOT IMPLEMENTED */
					  -5.0,   /* expected MOA output voltage */
					  0.001); /* 0.0 to 1.0 seconds */

        vth_aclose_rly(mod_id, K753);
	    vth_aclose_rly(mod_id, K754);
	    vth_aclose_rly(mod_id, K820);
	    vth_aclose_rly(mod_id, K813);
		
		talonCard->openRelay(rlyName,false,true);
		talonCard->closeRelay(rlyName,false,true);

		vth_resistor_meas(mod_id, 0, 1);                          /* Make the resistance measurement. */
		vth_get_abs_resistor_result(mod_id);                      /* Return the measured result. */
		vth_mtmr_wait(mod_id, 10.0e-03);		
	}

	if(bFull)
	{
		get_result_nolog(mod_id, &resultBuzz);
		aryResult[0] = resultBuzz.UU.Real_result;
		dblMax = dblMin = aryResult[0];
		for (int i=1; i<5; i++)
		{
			get_result_nolog(mod_id, &resultBuzz);
			aryResult[i] = resultBuzz.UU.Real_result;
			if (aryResult[i] > dblMax)
				dblMax = aryResult[i];
			else if (aryResult[i] < dblMin)
				dblMin = aryResult[i];
		}
		resultBuzz.UU.Real_result = dblMax - dblMin;
		log_result(mod_id, resultBuzz);
	}
} /* talon_buzz_that_relay */

/* ------------------------------------------------------------------------ */

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
  if(sum == 0)
	  return 1.0;
  else
	  return sum;
}  /* find_harmonic_power */

/*-----------------------------------------------------------------------------*/
void find_local_minima(int imin, int imax, int ipeak, double *y, double db_down, int *ilo, int *ihi)
{
  /* This procedure finds the local minima indices, ilo and ihi, of the
     magnitude array, y, around around the peak index, ipeak, that is atleast
     dB_down dB below the peak magnitude. If either of these conditions can not
     be met, the appropriate array end point, imin and/or imax, is returned.    */
  int i;
  double peak_limit;

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
int find_dc_index(int n, double *y)
{

  /* This function finds the index of the first local minima of the magnitude
     array y of length n.  If the array is continously decreasing, n-1 is
     returned.                                                                 */
  int i = 0;
  while (y[i] > y[i+1] && i + 1 < n)
    i++;
  return i;
}  /* find_dc_index */

/*-----------------------------------------------------------------------------*/
static void black_window(double *data, int n)
{
  /* see Proakis p. 549 */
  double ang;
  double factor;
  int i;
  int j;

  ang = (2 * LONGPI) / (n - 1);
  for (i = 0; i < n; i++)
  {
    j = i;
    factor = 0.42 - 0.5 * cos(ang * j) + 0.08 * cos(2 * ang * j);
	//hack
	if(data[i] == 0)
		data[i] = 0.0015;
    data[i] *= factor;
  }
}  /* black_window */

/*-----------------------------------------------------------------------------*/
void waveform_to_magnitude(double *x_, double *y, int npts)
{
  arrform x;
  arrform xi;

  memcpy(x, x_, sizeof(arrform));
  black_window(x, npts);
  fill(xi, 0.0, npts);
  fft(x, xi, npts);
  mag(x, xi, y, npts);

}  /* waveform_to_magnitude */

/*-----------------------------------------------------------------------------*/
int find_local_max(int imin, int imax, int iexp, double *y)
{
  /* This function finds local maximum of the magnitude array y about the
     expected value of iexp within the limits of imin to imax.  If iexp
     is a local minimum iexp = 0 is returned.                             */
  int result;
  int i;
  bool up;
  bool down;

  down = false;
  up = false;
  result = iexp;
  if (iexp > imin)
  {
    if (y[iexp] < y[iexp - 1])
      down = true;
  }
  if (iexp < imax)
  {
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

  ln2 = log(2.0); //0.30102999566398119521373889472449
  m = round(log((double)npts) / ln2); //11 (npts=2048)
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

/*-----------------------------------------------------------------------------*/

void fill(double * x, double cnst, int npts)
{  /* start of procedure fill              */
  int n;   

  for (n = 0; n < npts; n++)   
    x[n] = cnst;   
}  /* end of fill                          */

/*-----------------------------------------------------------------------------*/

void mag(double * xr, double * xi, double * y, int npts)
{  /* start of procedure mag */
  int n;   

  for (n = 0; n < npts; n++)   
 	  y[n] = sqrt(xr[n] * xr[n] + xi[n] * xi[n]);   /* the magnitudes     */
}  

/*-----------------------------------------------------------------------------*/
int find_peak_index(int imin, int imax, double *y)
{
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
double sum_power(int imin, int imax, double * y)
{
  /* This function finds the power of the magnitude array y from indices imin
     to imax.  Power is defined as the square of the magnitude.               */
  int i;
  double sum;
//FILE *ofile;
//ofile = fopen("371sum.xls", "a");
  sum = 0.0;
  if(imin == 0)
	  imin = 1; 
  for (i = imin - 1; i < imax; i++)
  {
    sum += y[i] * y[i];
//	fprintf(ofile, "%d\t %d\t %f\t %f\n", imin, imax, y[i], sum);
  }
//fclose(ofile);
  return sum;
}  /* sum_power */

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
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_resistor_meas(mod_id, 1, 1);
  vth_get_resistor_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    resistance_w_47_ohms = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected resistance)", true);

  vth_aclose_rly(mod_id, K806);

  /*MEASURE RESISTANCE WITHOUT 47 OHMS IN SOURCE PATH*/
  vth_rlc_setup(mod_id, 0, 0.1, 0.0, 0.0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1.5, 0.0);
  vth_aopen_rly(mod_id, K829);
  vth_aclose_rly(mod_id, K806);
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
  vth_resistor_meas(mod_id, 1, 1);
  vth_get_resistor_result(mod_id);
  if (get_result_nolog(mod_id, &result))
    resistance_wo_47_ohms = result.UU.Real_result;
  else
    display_prompt("No result in result log (Expected resistance)", true);

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

  bool hold;
  bool slave_found;

  bool list[4];

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
bool rack_and_fixture_compatible(char *rack_serial_number, int fixture_serial_number)
{
  bool compatible;

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
bool four_mod_fixture(int mod_id)
{
  bool result;
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
  

  switch (get_fixture_id(mod_id))
  {
    case 3801:
	  sprintf(buffer, "%sstd.nom_3ind_03", testwhere_path);
      infile = fopen(buffer, "r\n");

      if (infile == NULL)
      {
        display_prompt("The 'std.nom_3ind_03' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      break;

    case 3800:
	  sprintf(buffer, "%sstd.nom_3rcd_12", testwhere_path);
      infile = fopen(buffer, "r\n");

      if (infile == NULL)
      {
        display_prompt("The 'std.nom_3rcd_12' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      break;

    case 3785:
	  sprintf(buffer, "%sstd.nom_0_12", testwhere_path);
      infile = fopen(buffer, "r\n");

      if (infile == NULL)
      {
        display_prompt("The 'std.nom_0_12' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      break;

    case 3784:
	  sprintf(buffer, "%sstd.nom_0_03", testwhere_path);
      infile = fopen(buffer, "r\n");
      if (infile == NULL)
      {
        display_prompt("The 'std.nom_0_03' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      break;

    case 3783:
	  sprintf(buffer, "%sstd.nom_1_03", testwhere_path);
      infile = fopen(buffer, "r\n");

      if (infile == NULL)
      {
        display_prompt("The 'std.nom_1_03' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      break;

    case 3782:
	  sprintf(buffer, "%sstd.nom_1_12", testwhere_path);
      infile = fopen(buffer, "r\n");

      if (infile == NULL)
      {
        display_prompt("The 'std.nom_1_12' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      break;

    case 3781:
	  sprintf(buffer, "%sstd.nom_2_03", testwhere_path);
      infile = fopen(buffer, "r\n");

      if (infile == NULL)
      {
        display_prompt("The 'std.nom_2_03' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      break;

    case 3780:   /* otherwise */
	  sprintf(buffer, "%sstd.nom_2_12", testwhere_path);
      infile = fopen(buffer, "r\n");

      if (infile == NULL)
      {
        display_prompt("The 'std.nom_2_12' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      break;

    default:
	  sprintf(buffer, "%sstd.nom_zero", testwhere_path);
      infile = fopen(buffer, "r\n");

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

/*---------------------------------------------------------------------------*/
#define SAMPLE_DELAY    60.0e-9
#define HOLD_MODE_SETTLING_TIME  2.6e-6
#define MAX_CONV_TIME   17.18e-6
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

/* ------------------------------------------------------------------------ */

#define DIG_SETTLE      250e-6
#define SMALLEST_RANGE  0
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
  vth_aclose_rly(mod_id, K808);   // DETHI to S 
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);						//1.0e-03
  vth_mmbd_dis_all(mod_id);
  vth_mtmr_wait(mod_id, DIG_SETTLE);								//250e-6
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
  vth_aopen_rly(mod_id, K808);   // disconnect DETHI from S 
  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);						//100e-06
  vth_aclose_rly(mod_id, K828);   // DETHI to REFR2 
  vth_adet_sel_rng(mod_id, SMALLEST_RANGE);
  vth_mmbd_dis_all(mod_id);
  vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);						//100e-06  => changed to 50e-03
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
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);						//1.0e-03
  vth_asrc_set_dc_ampl(mod_id, vplus);
  range = sel_det_range(vplus);
  vth_adet_sel_rng(mod_id, range);
  vth_aclose_rly(mod_id, K808);   /* DETHI to S */
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);						//1.0e-03
  vth_mmbd_dis_all(mod_id);
  vth_mtmr_wait(mod_id, DIG_SETTLE);								//250e-06
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
  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);						//1.0e-03
  vth_aclose_rly(mod_id, K828);   /* DETHI to REFR2 */
  vth_adet_sel_rng(mod_id, SMALLEST_RANGE);
  vth_mmbd_dis_all(mod_id);
  vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);						//250e-06 => changed to 50e-03
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

/* ------------------------------------------------------------------------ */

#define DIG_SETTLE      250e-6
#define SMALLEST_RANGE  0
#define WAIT_TIME_FOR_SETTLE  0.5
double measure_open_non(int mod_id, double vplus, double vminus, double rfb)
{
  test_result_type result;
  double vs1;
  double vs2;
  double vo1;
  double vo2;
  det_range_type range;
  bool bogus;

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

/* ------------------------------------------------------------------------ */
/* DC DETECTOR VOLTAGE ACCURACY - this procedure is called once for each    */
/* DC detecor range test.                                                   */

#define POSITIVE        0
#define NEGATIVE        1
#define AVERAGES        1
#define DIGITAL         0
#define TIMEOUT         10.0e+00
#define RESOLUTION      1e-03

void test_detector_range(int mod_id, double range_voltage, int range_number, int divisor)
{
  int voltage;
  int length;
  double dmm_actual_voltage;
  double detector_voltage;
  double v_delta;
  double voltage_array[4] = {5.25e00, 3.75e00, 2.5e00, 1.25e00};

  test_result_type result;

  /* CLOSE THE SRCOUT TO DETHI/CALIB2 RELAYS */
  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
  vth_aclose_rly(mod_id, K733);   /* S bus to X1 bus relay */
  vth_aclose_rly(mod_id, K725);   /* X1 bus to I bus relay */
  vth_aclose_rly(mod_id, K855);   /* I bus to CALIB2 relay */
  /* CLOSE THE GROUND PATH RELAYS */
  vth_aclose_rly(mod_id, K857);   /* L bus to GROUND relay */
  vth_aclose_rly(mod_id, K759);   /* L bus to LOUT relay */

  vth_dcv_setup(mod_id, 0, AVERAGES, DIGITAL, range_voltage, TIMEOUT);
  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
  vth_afc_en_src_out(mod_id);
  flush_commands(mod_id);
  while(divisor != 1) // first time do nothing -> max is 8 cycles of halving the static volts
  {
	for(int j = 0; j < 4; j++)
	   voltage_array[j] = voltage_array[j] / 2;

	divisor--;
  }
  for (voltage = 0; voltage < NUM_DET_POINTS_TO_CHECK; voltage++)
  {
    for (int polarity = POSITIVE; polarity <= NEGATIVE; ++polarity)
    {  /* for polarity */
      if (polarity == NEGATIVE)
        vth_asrc_set_dc_ampl(mod_id, -1 * voltage_array[voltage]);
      else
        vth_asrc_set_dc_ampl(mod_id, voltage_array[voltage]);

	  setup_dmm(mod_id, DCV, voltage_array[voltage], RESOLUTION);
	  vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);
	  flush_commands(mod_id);
	  dmm_actual_voltage = (measure_dmm(mod_id) * -1);
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
}  /* test_detector_range */

#undef POSITIVE
#undef NEGATIVE
#undef AVERAGES
#undef DIGITAL
#undef TIMEOUT
#undef RESOLUTION


/*------------------------------------------------------------------------*/

bool fxt_locked(int mod_id)
{
  vth_cfxt_tst_lock();
  test_result_type result;
  get_result_nolog(CAP_NUMBER, &result);
  assert(result.Measurement_class == CLASS_INTEGER);
  flush_commands(mod_id);
  return (result.UU.Integer_result == 1);
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

#define AVERAGES			10
#define DIGITAL				0
#define RAMP_TIME_ALLOWED	10e+00
#define DIFF_V				2e-03
#define RESOLUTION			100e-03
#define POWER_SUPPLY_CURRENT 100e-03
#define TIMEOUT				10.0e+00

/* static variables for test_det_atten_range: */
struct LOC_test_det_atten_range
{
  int mod_id;
  double zero_v_correction;
  double v_delta;
  test_result_type result;
} ;

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
  }  /*adjust result per dmm reading*/
  else
    display_prompt("No result in result log (Expected DET volt)", true);

}  /* massage_and_log_results */

void test_det_atten_range(int mod_id, double range_voltage, int num_points, int range)
{
  /* DC DETECTOR ATTENUATOR VOLTAGE ACCURACY - this procedure is called */
  /* once for each DC detecor range(supported in attenuated mode) test. */
  struct LOC_test_det_atten_range V;
  int length;
  double actual_voltage;
  double test[4] = {60.0e+00, 40.0e+00, 20.0e+00, 10.0e+00};

  vth_ainit(mod_id);
  V.mod_id = mod_id;
  OpenResourceManager();
  open_dmm(V.mod_id);
  open_ps(V.mod_id);
  open_mux(V.mod_id);

  /* SETUP THE DMM TO MEASURE THE ACTUAL OUTPUT FROM THE SUPPLY */
  connect_dmm(V.mod_id);
  connect_plus_calib1(V.mod_id);
  connect_minus_calib2(V.mod_id);
  connect_power_supply_xmint(V.mod_id);

  /* CLOSE THE RELAYS THAT ARE 'GLOBAL' TO THIS TEST */
  vth_aclose_rly(V.mod_id, K820);   /* L to DETLO connect relay */
  vth_aclose_rly(V.mod_id, K812);   /* I to DETHI connect relay */
  vth_aclose_rly(V.mod_id, K741);   /* X1 mint pin connect relay  */
  vth_aclose_rly(V.mod_id, K742);   /* X2 mint pin connect relay  */

  /* CLOSE THE DMM READBACK PATH RELAYS CALIB1/LOUT and CALIB2 */
  vth_aclose_rly(V.mod_id, K759);   /* LOUT mint pin connect relay */
  vth_aclose_rly(V.mod_id, K855);   /* I bus to CALIB2 relay */

  /* SETUP THE 60V SUPPLY DC POWER SUPPLY */
  for (int point = 0; point < num_points; point++)
  {  /* for point */
    setup_dmm(V.mod_id, DCV, test[point], RESOLUTION);
    actual_voltage = fabs(test[range]);
    vth_aopen_rly(V.mod_id, K725);
    vth_aopen_rly(V.mod_id, K718);
    /* the supply doesn't get shorted out */
    vth_aclose_rly(V.mod_id, K726);
    vth_aclose_rly(V.mod_id, K717);
    vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS);  
    setup_ps(V.mod_id, test[point], POWER_SUPPLY_CURRENT);
	flush_commands(mod_id);
    wait_for_ps_ramp(V.mod_id, DIFF_V, RAMP_TIME_ALLOWED, &actual_voltage);

    for (length = 0; length <= 1; length++)
    {  /*for integrate length*/
      for (int polarity = NEG; polarity <= POS; ++polarity)
      {  /*for polarity*/
        if (polarity == NEG)
        {  /*polarity = neg*/
          V.zero_v_correction = -1 * actual_voltage;
          vth_aopen_rly(V.mod_id, K725);
          vth_aopen_rly(V.mod_id, K718);
          vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS);
          vth_aclose_rly(V.mod_id, K726);
          vth_aclose_rly(V.mod_id, K717);
        }  /*polarity = neg*/
        else
        {  /*polarity = pos*/
          V.zero_v_correction = actual_voltage;
          vth_aopen_rly(V.mod_id, K726);
          vth_aopen_rly(V.mod_id, K717);
          vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS);
          vth_aclose_rly(V.mod_id, K725);
          vth_aclose_rly(V.mod_id, K718);
        }  /*polarity = pos*/

        /* CAN USE THE DCV_SETUP ROUTINE BECAUSE THE VOLTAGE IS ACROSS I AND L */
        vth_dcv_setup(V.mod_id, length, AVERAGES, DIGITAL, range_voltage, TIMEOUT);

        /* CLOSE THE PATH RELAYS TO MAKE THE DETHI GROUNDED MEASUREMENT */
        /* 'SOME' OF THESE RELAYS MUST BE CLOSED AFTER THE DCV_SETUP ROUTINE */
        vth_aclose_rly(V.mod_id, K2309);	/* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */
        vth_aclose_rly(V.mod_id, K2315);	/* DETLO DIVIDED SIGNAL TO RANGING/DETECTOR */
        vth_aclose_rly(V.mod_id, K2306);	/* DETHI TO GROUND SIGNAL PATH */
        vth_aclose_rly(V.mod_id, K2304);	/* DETHI TO GROUND SIGNAL PATH */
        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS);
    	vth_mtmr_wait(mod_id, TIME_OUT);
        massage_and_log_results(&V);

        vth_astat_clr(V.mod_id);
        vth_aopen_rly(V.mod_id, K2309);		/* OPEN THE RELAYS USED FOR THE DETHI GROUNDED MEASUREMENT */
        vth_aopen_rly(V.mod_id, K2315);		/* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */
        vth_aopen_rly(V.mod_id, K2306);		/* DETHI TO GROUND SIGNAL PATH */
        vth_aopen_rly(V.mod_id, K2304);		/* DETHI TO GROUND SIGNAL PATH */
        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS);

        /* VERSION 8.2 REQUIRES THAT THE SETUP BE REPEATED BEFORE ANOTHER MEAS */
        vth_dcv_setup(V.mod_id, length, AVERAGES, DIGITAL, range_voltage, TIMEOUT);

        /* CLOSE THE APPROPRIATE PATH RELAYS FOR THE DETLO GNDED MEASUREMENT */
        /* 'SOME' OF THESE RELAYS MUST BE CLOSED AFTER THE DCV_SETUP ROUTINE */
        vth_aclose_rly(V.mod_id, K2310);	/* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */
        vth_aclose_rly(V.mod_id, K2316);	/* DETHI DIVIDED SIGNAL TO RANGING/DETECTOR */
        vth_aclose_rly(V.mod_id, K2305);	/* DETLO TO GROUND SIGNAL PATH */
        vth_aclose_rly(V.mod_id, K2303);	/* DETLO TO GROUND SIGNAL PATH */
        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS);
        massage_and_log_results(&V);

        vth_astat_clr(V.mod_id);
        vth_aopen_rly(V.mod_id, K2310);	/* OPEN THE PATH RELAYS TO MAKE THE DETLO GROUNDED MEASUREMENT */
        vth_aopen_rly(V.mod_id, K2316);	/* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */
        vth_aopen_rly(V.mod_id, K2305);   /* DETLO TO GROUND SIGNAL PATH */
        vth_aopen_rly(V.mod_id, K2303);   /* DETLO TO GROUND SIGNAL PATH */
        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS);

        /* VERSION 8.2 REQUIRES THAT THE SETUP BE REPEATED BEFORE ANOTHER MEAS */
        vth_dcv_setup(V.mod_id, length, AVERAGES, DIGITAL, range_voltage, TIMEOUT);

        /* CLOSE THE APPROPRIATE PATH RELAYS TO MAKE THE DIFF MEASUREMENT */
        /* 'SOME' OF THESE RELAYS MUST BE CLOSED AFTER THE DCV_SETUP ROUTINE */
        vth_aclose_rly(V.mod_id, K2310);	/* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */
        vth_aclose_rly(V.mod_id, K2316);	/* DETHI DIVIDED SIGNAL TO RANGING/DETECTOR */
        vth_aclose_rly(V.mod_id, K2309);	/* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */
        vth_aclose_rly(V.mod_id, K2315);	/* DETLO DIVIDED SIGNAL TO RANGING/DETECTOR */
        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS);
        massage_and_log_results(&V);

        vth_astat_clr(V.mod_id);
        vth_aopen_rly(V.mod_id, K2310);	/* DETHI TO DIVIDE BY 16 RESISTOR NETWORK */
        vth_aopen_rly(V.mod_id, K2316);	/* DETHI DIVIDED SIGNAL TO RANGING/DETECTOR */
        vth_aopen_rly(V.mod_id, K2309);	/* DETLO TO DIVIDE BY 16 RESISTOR NETWORK */
        vth_aopen_rly(V.mod_id, K2315);	/* DETLO DIVIDED SIGNAL TO RANGING/DETECTOR */
        vth_mtmr_wait(V.mod_id, WAIT_TIME_FOR_RELAYS);
		flush_commands(mod_id);
      }  /*for polarity*/
    }  /*for integrate length*/
	range++;
  }  /*for point*/

  vth_mod_init(V.mod_id);
  
  close_dmm(V.mod_id);
  close_ps(V.mod_id);
  close_mux(V.mod_id);
  CloseResourceManager();

}  /*test_det_atten_range*/

#undef AVERAGES
#undef DIGITAL
#undef RAMP_TIME_ALLOWED
#undef DIFF_V
#undef RESOLUTION
#undef POWER_SUPPLY_CURRENT
#undef TIMEOUT

/* ------------------------------------------------------------------------ */
/*------------------------------------------------------------------------*/

#define BUZZ_WAIT       2.0e-3
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
  current_position:= strlen(s) + 1;
  strappend(s,(conditional ? 'true' : 'false'))
  display_prompt(s,true);
#endif
}

/*--------------------------------------------------------------------------------------------------------------------*/

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

/***************************************************************************
* Talon setups
***************************************************************************/

void Start_sequencer(int const &mod_id, int const &slot)
{
	test_result_type result;
	HTalon::writeAllMerlinBusCells(mod_id, slot, HTalon::DriveCheckEnable, HTalon::DriveCheckDisableAll);  /* Turn off all drive check--fatal errors get stuck on with no way to clear.  [REVIEW NEED IN VERSIONS AFTER P1] */
	vth_mseq_dbg_setup(mod_id, 1, 0xffffffff, 0xffffffff, 0);
    vth_mseq_ass_cpu_bra(mod_id);
	vth_mseq_run(mod_id);
	get_result_nolog(mod_id, &result);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Reset_sequencer(int const &mod_id, int const &slot)
{
	test_result_type result;
	vth_mseq_neg_cpu_bra(mod_id);                       /* Set the cpu branch condition to false--allows sequencer program to terminate. */
	vth_mseq_wait_on_stop(mod_id, TIME_OUT);             /* Wait for the sequencer to stop, timeout (in seconds). */
	vth_mseq_ass_prog_mode(mod_id);                     /* Put the sequencer into program mode. */
	vth_mrst_seq(mod_id);
	get_result_nolog(mod_id, &result);					// Flush pipe
}

/*-------------------------------------------------------------------------------------------------------------------*/
void StaticDriveSetup(int const &mod_id, int const &slot, PINDefinition::PinEnum pin, IMetaTesthead *th, IPin *pinCard, int p, int dvh, int dvl)
{
	DigitalChannelParameters dv;
	dv.setTspSource(DigitalDriverTspResourceId::alwaysActive);                   
	dv.setRcvSource(DigitalReceiverResourceId::alwaysDontCare);                  
	dv.setDriveHigh(dvh);                                      
	dv.setDriveLow(dvl);                                                         
	dv.setDrvSource(DigitalDriverResourceId::driveAOne);                         
	dv.setPullUp(DigitalSignalConditionStateId::disabled);                       
	dv.setPullDown(DigitalSignalConditionStateId::disabled);                     
	dv.setReceiverTermination(DigitalSignalConditionStateId::disabled);          
	dv.setDriveCheck(DigitalSignalConditionStateId::disabled);     
	BRC brc(mod_id, slot, pin);                                                
	ConnectBRCtoBus *DSBus = ConnectBRCtoBus::createInstance();
	ConnectDigitalResource *Drvr = ConnectDigitalResource::createInstance(); 
	DSBus->setBusID(MeasurementBus::S);
	Drvr->setBusToDriver();
	DSBus->setBRC(brc); 
	Drvr->setBRC(brc); 
	DSBus->setState(ConnectBRCtoBus::connect);                                                         
	Drvr->setState(ConnectDigitalResource::connect); 
	th->addActionToQueue(Drvr);
	th->addActionToQueue(DSBus); 
	th->processQueuedActions();    
	pinCard->setDigitalChannelParameters(pin, dv);
}

/*-------------------------------------------------------------------------------------------------------------------*/
void DisableStaticDriveOut(int const &mod_id, int const &slot, PINDefinition::PinEnum pin, IMetaTesthead *th, IPin *pinCard)
{
	DigitalChannelParameters off;
	off.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);                       
	off.setRcvSource(DigitalReceiverResourceId::alwaysDontCare); 
	BRC brc(mod_id, slot, pin);     
	pinCard->setDigitalChannelParameters(pin, off); 
	ConnectBRCtoBus *oSDBus = ConnectBRCtoBus::createInstance();              
	ConnectDigitalResource *offDrv = ConnectDigitalResource::createInstance();  
	oSDBus->setBusID(MeasurementBus::S);
	offDrv->setBusToDriver(); 
	oSDBus->setBRC(brc);  
	offDrv->setBRC(brc); 
	oSDBus->setState(ConnectBRCtoBus::disconnect);
	offDrv->setState(ConnectDigitalResource::disconnect);                    
	th->addActionToQueue(oSDBus);
	th->addActionToQueue(offDrv);
	th->processQueuedActions();           
}

/*--------------------------------------------------------------------------------------------------------------------*/

#define MIN_TEST_VOLTAGE	0.0 
	
	double IsolateOVPoint(int const &mod_id, int const &slot, bool high_bounds_ov, bool low_bounds_ov)
	{
		int trip_hmid = 0;
		int trip_lmid = 0;
		double high_pt;
		double low_pt;
		double actual_voltage;
		double mid_high_pt = 8.218;	
		double mid_low_pt = 7.636;
		double window_width = 0.582;
		
		/*high_pt = HIGH_BOUNDS;
        low_pt = LOW_BOUNDS;
        window_width = (high_pt-low_pt)/3;
        mid_high_pt = high_pt - window_width;
        mid_low_pt = mid_high_pt - window_width;*/
		
		a_word_type this_word;
		test_result_type result;
		
        if ( ((high_bounds_ov) && !(low_bounds_ov)) || (!(high_bounds_ov) && (low_bounds_ov)) )
		{/*if: OV exists*/
			for (int i = 0; i < 3; i++) //change iteration number based on the accuracy you want
			{ /*for: zeroing in*/
				trip_hmid = 0;
				trip_lmid = 0;
				vth_asrc_set_dc_ampl(mod_id, mid_high_pt); 
				vth_mtmr_wait(mod_id, 0.010); 
				vth_prd_stat(mod_id, slot);
				if (get_result_nolog(mod_id, &result))
				{
					this_word.data = result.UU.Integer_result;
					if (P_getbits_US(this_word.bits, 5, 0) == 0)
					{ 
						vth_asrc_set_dc_ampl(mod_id, MIN_TEST_VOLTAGE);
						vth_pdrv_clr_err(mod_id, slot);
						trip_hmid = 1;
					}
				}
				
				vth_asrc_set_dc_ampl(mod_id, mid_low_pt); 
				vth_mtmr_wait(mod_id, 0.010); 
				vth_prd_stat(mod_id, slot);
				if (get_result_nolog(mod_id, &result))
				{
					this_word.data = result.UU.Integer_result;
					if (P_getbits_US(this_word.bits, 5, 0) == 0)
					{ //limit_occurred
						vth_asrc_set_dc_ampl(mod_id, MIN_TEST_VOLTAGE);
						vth_pdrv_clr_err(mod_id, slot);
						trip_lmid = 1;
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
			//			display_prompt("Overvoltage window not found.", true);
			//	display_it(mod_id, "high_bounds ov", high_bounds_ov, high_bounds_ov);
			//	display_it(mod_id, "low_bounds ov", low_bounds_ov, low_bounds_ov);
			actual_voltage = 0;
        }// an initial overvoltage window exists
		
		result.Measurement_class = CLASS_REAL;
		return actual_voltage; 
		
	}
#undef MIN_TEST_VOLTAGE
	


/***************************************************************************
*
* Minerva Connect functions
*
***************************************************************************/
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
  start = strlen(s) + 1;
  sprintf(s + start - 1, "%d", connection);
  next = strlen(s) + 1;
  strcat(s, "\n");
  viPrintf(viMUX, s);
}  /*connect*/

/*------------------------------------------------------------------------*/
static void connect_cntr_start_probe(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 504, true);
  connect(mod_id, 712, true);
}  /*connect_cntr_start_PROBE*/

/*------------------------------------------------------------------------*/
void connect_cntr_stop_db1(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 923, true);
}  /*connect_cntr_stop_DB1*/

/*--------------------------------------------------------------------*/
void connect_cntr_stop_uclk(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 921, true);
  connect(mod_id, 611, true);
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
}  /*connect_cntr_stop_UCLK*/

/*------------------------------------------------------------------------*/
void connect_dg(int mod_id, int status)
{
  /* USED BY ANALOG FIXTURE ONLY */
  connect(mod_id, 206, status);
}  /*connect_DG*/

/*------------------------------------------------------------------------*/
void connect_dmm(int mod_id)
{
  /* USED BY ANALOG FIXTURE ONLY */

  /* connecting the dmm is accomplished by opening the relays since the   */
  /* dmm contact is made at the normally closed connections               */
  connect(mod_id, 201, false);
  connect(mod_id, 202, false);
  connect(mod_id, 204, false);
  connect(mod_id, 205, false);
}  /*connect_dmm*/

/*------------------------------------------------------------------------*/
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
      mod_contribution = 400;
      break;

    case 1:
      mod_contribution = 420;
      break;

    case 2:
      mod_contribution = 320;
      break;

    case 3:
      mod_contribution = 300;
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
      mod_contribution = 300;
      break;

    case 1:
    case 2:
      mod_contribution = 320;
      break;
    default:
      assert(false);
      break;

    }
  }

  switch (slot)
  {

  case 2:
    slot_contribution = 1;
    break;

  case 3:
    slot_contribution = 2;
    break;

  case 4:
    slot_contribution = 3;
    break;

  case 5:
    slot_contribution = 4;
    break;

  case 7:
    slot_contribution = 5;
    break;

  case 8:
    slot_contribution = 6;
    break;

  case 9:
    slot_contribution = 7;
    break;

  case 10:
    slot_contribution = 8;
    break;

  case 11:
    slot_contribution = 11;
    break;
  default:
    assert(false);
    break;

  }/* case */

  mod_slot_determined_connection = mod_contribution + slot_contribution;

  connect(mod_id, mod_contribution / 100 * 100 + 70, true);
  connect(mod_id, mod_contribution / 100 * 100 + 71, true);
  connect(mod_id, mod_contribution / 100 * 100 + 72, true);
  connect(mod_id, mod_contribution / 100 * 100 + 90, true);
  connect(mod_id, mod_slot_determined_connection, true);

}  /*connect_dmm_pin*/

/*------------------------------------------------------------------------*/
void connect_gen_trg_cntr_start(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 512, true);
  connect(mod_id, 503, true);
}  /*connect_gen_trg_cntr_start*/

/*------------------------------------------------------------------------*/
void connect_gen_trg_clkrb(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 511, true);
  if (four_mod_fixture(mod_id))
    connect(mod_id, mod_id + 601, true);
  else
  {
    switch (mod_id)
    {

    case 0:
    case 3:
      connect(mod_id, 604, true);
      break;

    case 1:
    case 2:
      connect(mod_id, 603, true);
      break;

    default:
      assert(false);
      break;

    }
  }
}  /*connect_gen_trg_clkrb*/

/*------------------------------------------------------------------------*/
void connect_gen_trg_osc_ch2(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 513, true);
  connect(mod_id, 723, true);
}  /*connect_gen_trg_osc_ch2*/

/*------------------------------------------------------------------------*/
void connect_gen_out_cntr_stop(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 523, true);
  connect(mod_id, 924, true);
}  /*connect_gen_out_cntr_stop*/

/*------------------------------------------------------------------------*/
void connect_gen_out_osc_ch1(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 522, true);
  connect(mod_id, 703, true);
}  /*connect_gen_out_osc_ch1*/

/*------------------------------------------------------------------------*/
void connect_gen_out_af2(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 521, true);
  connect(mod_id, 912, true);
  connect(mod_id, mod_id + 901, true);
}  /*connect_gen_out_AF2*/

/*------------------------------------------------------------------------*/
void connect_gen_out_probe(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 524, true);
  connect(mod_id, 713, true);
}  /*connect_gen_out_PROBE*/

/*------------------------------------------------------------------------*/
void connect_lcr(int mod_id)
{
  /* USED BY ANALOG FIXTURE ONLY */

  /* connecting the lcr is accomplished by closing the relays since the   */
  /* lcr contact is made at the normally open connections                 */
  connect(mod_id, 201, true);
  connect(mod_id, 202, true);
  connect(mod_id, 204, true);
  connect(mod_id, 205, true);
}  /*connect_lcr*/

/*------------------------------------------------------------------------*/
void connect_minus_pin(int mod_id, int slot, int channel, int status)
{  /*connect_dmm_minus_pin*/
  /* USED BY ANALOG FIXTURE ONLY */
  int mod_contribution;
  int slot_contribution;
  int channel_contribution;
  int pin_connection;

  switch (mod_id)
  {

  case 0:
  case 3:
    mod_contribution = 0;
    break;

  case 1:
  case 2:
    mod_contribution = 20;
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
}  /*connect_dmm_minus_pin*/

/*------------------------------------------------------------------------*/
void connect_minus_calib2(int mod_id)
{
  /* USED BY ANALOG FIXTURE ONLY */
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

}  /* connect_minus_CALIB2 */

/*------------------------------------------------------------------------*/
void connect_minus_swd_gnd(int mod_id)
{
  /* USED BY ANALOG FIXTURE ONLY */
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

}  /* connect_minus_SWD_GND */

/*------------------------------------------------------------------------*/
void dconnect_minus_swd_gnd(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  if (four_mod_fixture(mod_id))
  {
    switch (mod_id)
    {

    case 0:
      connect(mod_id, 414, true);
      break;

    case 1:
      connect(mod_id, 434, true);
      break;

    case 2:
      connect(mod_id, 334, true);
      break;

    case 3:
      connect(mod_id, 314, true);
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
      connect(mod_id, 314, true);
      break;

    case 1:
    case 2:
      connect(mod_id, 334, true);
      break;

    default:
      assert(false);
      break;

    }
  }

}  /* dconnect_minus_SWD_GND */


/*------------------------------------------------------------------------*/
void connect_minus_xn_plus_1(int mod_id, int channel, int status)
{
  /* USED BY ANALOG FIXTURE ONLY */
  int channel_contribution;


  channel_contribution = channel + 1 - 'A';

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

}  /* connect_minus_Xn_plus_1 */

/*------------------------------------------------------------------------*/
void connect_osc_ch1_af2(int mod_id)
{  /*connect_osc_AF2*/
  /* USED BY DIGITAL FIXTURE ONLY */
  /* New Sales Rack configuration for Monet - JRM 2/22/93 */
  connect(mod_id, 702, true);
  connect(mod_id, 911, true);
  connect(mod_id, 823, true);
  connect(mod_id, mod_id + 901, true);
}  /*connect_osc_AF2*/

/*------------------------------------------------------------------------*/
void connect_dmm50_af2(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 911, true);
  connect(mod_id, 822, true);
  connect(mod_id, mod_id + 901, true);
}  /*connect_osc_AF2*/

/*------------------------------------------------------------------------*/
void aconnect_cntr_ch1_af2(int mod_id)
{ /* USED BY ANALOG FIXTURE ONLY */
  connect(mod_id, 913, true);
  connect(mod_id, mod_id + 901, true);
}  /*connect_osc_AF2_analog*/

/*------------------------------------------------------------------------*/
void aconnect_cntr_stop_af2(int mod_id)
{  /*connect_cntr_stop_AF2_analog*/
  /* USED BY ANALOG FIXTURE ONLY */
  connect(mod_id, 911, true);
  connect(mod_id, mod_id + 901, true);
}  /*connect_cntr_stop_AF2_analog*/

/*------------------------------------------------------------------------*/
void connect_osc_ch1_cable_measure(int mod_id)
{  /*connect_osc_cable_measure*/
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 704, true);
}  /*connect_osc_cable_measure*/

/*------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------*/
void connect_cntr_ch1_uclk(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 504, true);
    // connect(mod_id, 700, true);  // commented for scope elim
  connect(mod_id, 702, true);   // minerva change for scope elim
  connect(mod_id, 612, true);
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
}  /*connect_cntr_ch1_UCLK*/

/*------------------------------------------------------------------------*/
void connect_plus_calib1(int mod_id)
{  /*connect_dmm_plus_CALIB1*/
  /* USED BY ANALOG FIXTURE ONLY */
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

}  /*connect_dmm_plus_CALIB1*/

/*------------------------------------------------------------------------*/
void connect_plus_gout(int mod_id)
{  /*connect_dmm_plus_GOUT*/
  /* USED BY ANALOG FIXTURE ONLY */
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

}  /*connect_dmm_plus_GOUT*/

/*------------------------------------------------------------------------*/
void connect_plus_xn(int mod_id, int channel, int status)
{
  /* This routine provides source connections to the fixture. */
  /* SOURCE AND SENSE ARE SHORTED INSIDE THE MINERVA FIXURE. */

  /* USED BY ANALOG FIXTURE ONLY */
  int channel_contribution;


  channel_contribution = channel + 1 - 'A';

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

  }/*case*/

}  /*connect_plus_Xn*/

/*------------------------------------------------------------------------*/
void connect_4wire_plus_xn(int mod_id, int channel, int status)
{
  /* This routine provides source and sense connections all the way to the */
/* testhead fixture. The test is expected to provide the int between */
  /* source and sense on the board under test.                             */
  /* USED BY ANALOG FIXTURE ONLY */
  int channel_contribution;

  channel_contribution = channel + 1 - 'A';

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

}  /*connect_4wire_plus_Xn*/

/*------------------------------------------------------------------------*/
void connect_power_supply_xmint(int mod_id)
{
  /* Connects X1 and X2 to the 6038A power supply + and - respectively */
  /* USED BY ANALOG FIXTURE ONLY */
  int module_contribution;


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

}  /*connect_power_supply_XMint*/

/*------------------------------------------------------------------------*/
void connect_syn_aux_gen_ext(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 804, false);
}  /*connect_syn_aux_gen_ext*/

/*------------------------------------------------------------------------*/
void connect_syn_out_gen_ext(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 804, true);
}  /*connect_syn_out_gen_ext*/

/*------------------------------------------------------------------------*/

void aconnect_srcout_AF2(int mod_id)
{
   /* CLOSE THE AF2 MEASUREMENT PATH RELAYS */
  vth_aclose_rly(mod_id, K806);   /* SRCOUT to S bus relay */
  vth_aclose_rly(mod_id, K751);   /* S bus to XL bus relay */
  vth_aclose_rly(mod_id, K764);   /* XL bus to AF2 relay */
}

/*------------------------------------------------------------------------*/

void aconnect_srcout_calib1(int mod_id)
{
  vth_aclose_rly(mod_id, K837);   /* SRCOUT to RCAL2 relay */
  vth_aclose_rly(mod_id, K853);   /* RCAL2 to CALIB1 relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */

}

/*------------------------------------------------------------------------*/

void aconnect_auxsrc_fxt(int mod_id)
{
  vth_aclose_rly(mod_id, K823);   /* AUXOUT to A bus relay */
  vth_aclose_rly(mod_id, K709);   /* A bus to X1 bus relay */
  vth_aclose_rly(mod_id, K741);   /* X1 bus to mint pin relay */
  vth_aclose_rly(mod_id, K854);   /* CALIB2 to ground relay */
}

/*------------------------------------------------------------------------*/

void aconnect_auxout_synth(int mod_id)
{
  /* CLOSE THE AUXOUT TO SYN. POSITIVE  PATH RELAYS */
  vth_aclose_rly(mod_id, K741);   /* X1MINT to X1 bus relay */
  vth_aclose_rly(mod_id, K709);   /* X1 bus to A bus relay */
  vth_aclose_rly(mod_id, K823);   /* A bus to AUXOUT relay */
  vth_aclose_rly(mod_id, K742);   /* X2MINT to X2 bus relay */
  vth_aclose_rly(mod_id, K718);   /* X2 bus to L bus relay */
  vth_aclose_rly(mod_id, K857);   /* L bus to GROUND relay */
}

/*------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------*/
void aconnect_syn_out_dmm(int mod_id, int status)
{
  connect(mod_id, 391, status);
  connect(mod_id, 371, status);
  connect(mod_id, 331, status);
  connect(mod_id, 323, status);

}  /*aconnect_syn_out_dmm*/

/*------------------------------------------------------------------------*/
void connect_syn_out_osc_ch1(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 802, true);
  connect(mod_id, 701, true);
}  /*connect_syn_out_osc_ch1*/

/*------------------------------------------------------------------------*/
void connect_syn_out_af2(int mod_id)
{
  connect(mod_id, 801, true);
  connect(mod_id, 821, true);
  connect(mod_id, 911, true);
  connect(mod_id, mod_id + 901, true);
}  /*connect_syn_out_AF2*/

/*------------------------------------------------------------------------*/
void aconnect_syn_out_xmint(int mod_id)
{
  /* Connects X1 and X2 to the synthesizer output */
  int module_contribution;

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

}  /*aconnect_syn_out_XMint*/

/*------------------------------------------------------------------------*/
void aconnect_vs_dmm(int mod_id, int status)
{
  /* Connects X6 to the dmm input */
  int module_contribution;

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

}  /*aconnect_Vs_dmm*/

/*------------------------------------------------------------------------*/
void aconnect_vo_dmm(int mod_id, int status)
{
  /* Connects X3 to the dmm input */
  int module_contribution;

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

}  /*aconnect_Vo_dmm*/

/*--------------------------------------------------------------------* */
void connect_dmm_probe(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 711, true);
  connect(mod_id, 313, true);
  connect(mod_id, 370, true);
  connect(mod_id, 390, true);
}  /*connect_dmm_PROBE*/

/*--------------------------------------------------------------------* */
void aconnect_ps_20_ohm_load(int mod_id, int status)
{
  connect(mod_id, 208, status);
}  /*aconnect_ps_20_ohm_load*/

/*------------------------------------------------------------------------*/
void connect_1g_ohm_resistor(int mod_id, int status)
{
  /* USED BY ANALOG FIXTURE ONLY */
  connect(mod_id, 207, status);
}  /*connect_1G_ohm_resistor*/

/*------------------------------------------------------------------------*/
void connect_500_ohm_load(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 201, true);
  connect(mod_id, 202, true);
}  /*connect_500_ohm_load*/

/*------------------------------------------------------------------------*/
void connect_10k_load(int mod_id)
{
  /* USED BY ANALOG FIXTURE ONLY */
  connect(mod_id, 203, true);
}  /*connect_10k_load*/

/*------------------------------------------------------------------------*/
void disconnect_10k_load(int mod_id)
{  /*connect_10k_load*/
  /* USED BY ANALOG FIXTURE ONLY */
  connect(mod_id, 203, false);
}  /*connect_10k_load*/

/****************************************************************************
* Talon
****************************************************************************/

double setup_moa_resistance_meas(int mod_id, int rfb, int mode)
{
	  test_result_type result;

	  /* setup the MOA for resistor measurements */
	  vth_aclose_rly(mod_id, K827);   /* MOAOUT to REFR2 */
	  vth_aclose_rly(mod_id, K850);   /* Rfb = 1MEG */
	  vth_aclose_rly(mod_id, K811);   /* I to MOAINV */
	  vth_aclose_rly(mod_id, K843);   /* MOANON to ACOM */
	  vth_aopen_rly(mod_id, K2605);   /* MOA Happy Relay */
	  vth_amoa_sel_wb(mod_id);
	  vth_acal_rtn_fb(mod_id, rfb, mode);
	  if (get_result_nolog(mod_id, &result))
		  rfb = result.UU.Real_result;
	  else
	  {
		  display_prompt("No result in result log, expected Rfb", true);
		  rfb = 1e6;
	  }
	  return rfb;
}


/*-------------------------------------------------------------------------------------------------------------------*/

void sglset_close(int const &mod_id, int const &slot, HTalon::Relays const &ref, HTalon *talonCard)
{
	assert((mod_id >= HDW_MIN_MODULE) && (mod_id <= HDW_MAX_MODULE));
	assert((slot >= HDW_MIN_CARD) && (slot <= HDW_MAX_CARD) && (slot != HDW_MAC_CARD_POSITION) && (slot != HDW_AIM_CARD_POSITION));
	
	//		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot);
	talonCard->closeRelays(ref.begin(), ref.end(), false, true);    // Close relays in c. No wait. Send. 
}

/*-------------------------------------------------------------------------------------------------------------------*/
void close(int const &mod_id, int const &slot, HTalon::Relays const &rut, HTalon::Relays const &ref, HTalon *talonCard)
{
	assert((mod_id >= HDW_MIN_MODULE) && (mod_id <= HDW_MAX_MODULE));
	assert((slot >= HDW_MIN_CARD) && (slot <= HDW_MAX_CARD) && (slot != HDW_MAC_CARD_POSITION) && (slot != HDW_AIM_CARD_POSITION));
	
	//		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot);
	talonCard->closeRelays(rut.begin(), rut.end(), false, false);   // No wait. No Send. 
	talonCard->closeRelays(ref.begin(), ref.end(), false, true);	// No wait. Send. 
}

/*-------------------------------------------------------------------------------------------------------------------*/
void connect_4wire_talon(int mod_id, int status, int p)
{
	/* This routine provides source and sense connections all the way to the */
	/* testhead fixture. The test is expected to provide the int between	 */
	/* source and sense on the board under test.                             */
	/* USED BY ANALOG FIXTURE ONLY	*/
	int mux_connect;

	switch (p)
	{
	case 0:
		vth_aclose_rly(mod_id, K741);
		vth_aclose_rly(mod_id, K742);
		mux_connect = 1;
		break;
		
	case 1:
		vth_aclose_rly(mod_id, K743);
		vth_aclose_rly(mod_id, K744);
		mux_connect = 3;
		break;
		
	case 2:
		vth_aclose_rly(mod_id, K745);
		vth_aclose_rly(mod_id, K746);
		mux_connect = 5;
		break;
		
	case 3:
		vth_aclose_rly(mod_id, K747);
		vth_aclose_rly(mod_id, K748);
		mux_connect = 7;
		break;
		
	default:
		assert(0);
		break;
	}	
	
	switch (mod_id)
	{
		
	case 0:  /*0*/
	case 3:  /*3*/
		connect(mod_id, 323, status);
		connect(mod_id, mux_connect + 300, status);
		break;
		/*mod 0/3*/
		
	case 1:  /*1*/
	case 2:  /*2*/
		connect(mod_id, 324, status);
		connect(mod_id, mux_connect + 310, status);
		break;
		/*mod 1/2*/
		
	default:
		assert(0);
		break;
		
	}/*case*/
	
}  

/*-------------------------------------------------------------------------------------------------------------------*/
void connect_minus_xn_plus_1_talon(int mod_id, int status)
{
	/* USED BY ANALOG FIXTURE ONLY */
	switch (mod_id)
	{
	case 0:  /*0*/
	case 3:  /*3*/
		connect(mod_id, 331, status);		// LCR or DMM low
		break;
		
	case 1:  /*1*/
	case 2:  /*2*/
		connect(mod_id, 332, status);		// LCR or DMM low
		break;
		
	default:
		assert(0);
		break;
		
	}/*case*/
	
}  

/*-------------------------------------------------------------------------------------------------------------------*/
void connect_plus_xn_talon(int mod_id, int status, int p)
{
	int mux_connect;
	/* This routine provides source connections to the fixture. */
	/* SOURCE AND SENSE ARE SHORTED INSIDE THE MINERVA FIXURE. 8/
	
	/* USED BY ANALOG FIXTURE ONLY */
	
	switch (p)
	{
	case 0:
		vth_aclose_rly(mod_id, K741);
		vth_aclose_rly(mod_id, K745);
		mux_connect = 1;
		break;
		
	case 1:
		vth_aclose_rly(mod_id, K743);
		vth_aclose_rly(mod_id, K747);
		mux_connect = 3;
		break;
		
	case 2:
		vth_aclose_rly(mod_id, K742);
		vth_aclose_rly(mod_id, K746);
		mux_connect = 2;
		break;
		
	case 3:
		vth_aclose_rly(mod_id, K744);
		vth_aclose_rly(mod_id, K748);
		mux_connect = 4;
		break;
		
	default:
		assert(0);
		break;
	}	
	
	switch (mod_id)
	{
		
	case 0:  /*0*/
	case 3:  /*3*/
		connect(mod_id, 321, status);				// LCR or DMM high
		connect(mod_id, mux_connect + 300, status);
		connect(mod_id, mux_connect + 300 + 3, status);
		break;
	case 1:  /*1*/
	case 2:  /*2*/
		connect(mod_id, 322, status);				// LCR or DMM high
		connect(mod_id, mux_connect + 310, status);
		connect(mod_id, mux_connect + 310 + 3, status);	
		break;
	default:
		assert(0);
		break;
		
	}		
}  

/*-------------------------------------------------------------------------------------------------------------------*/
void CloseDigitalFixtureGround(int const &mod_id, int const &slot, HTalon *talonCard)
{
	typedef HTalon::RelayName const *const RLYLIST;
	struct DigGnd
	{		
		HTalon::Relays ref;
		DigGnd(RLYLIST ref_b, RLYLIST ref_e) : ref(ref_b, ref_e) {}
	};
	
	static HTalon::RelayName const FXT_GND[12] = {HTalon::KAGD19, HTalon::KAGD20, HTalon::KAGD39, HTalon::KAGD40, HTalon::KAGD59, HTalon::KAGD60,
		HTalon::KBGD19, HTalon::KBGD20, HTalon::KBGD39, HTalon::KBGD40, HTalon::KBGD59, HTalon::KBGD60};
	
	static unsigned int const n = 1;
	static DigGnd const tps[n] =
	{
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
		DigGnd(R(FXT_GND)),
#undef R
	};
	
	sglset_close(mod_id, slot, tps[LOOP].ref, talonCard);
}

/*-------------------------------------------------------------------------------------------------------------------*/
void ConnectDigitalFixture(int const &mod_id, int const &slot, PINDefinition::PinEnum pin, IMetaTesthead *th, IPin *pinCard)
{
	int page = 0;
	static unsigned int const j = 8;
	
	struct meapin
	{
		PINDefinition::PinEnum const MeaPin;
		meapin(PINDefinition::PinEnum const &m) : MeaPin(m) {}
	};
	
	static meapin const mp[j] = 
	{ 
		meapin(PINDefinition::pin5),
		meapin(PINDefinition::pin25),
		meapin(PINDefinition::pin45),
		meapin(PINDefinition::pin65),
		meapin(PINDefinition::pin105),
		meapin(PINDefinition::pin125),
		meapin(PINDefinition::pin145),
		meapin(PINDefinition::pin165),
	};
	page = ( pin < PINDefinition::pin101 ? ((int)pin/20) : 4 + ((int)pin - 100)/20 );
	PINDefinition::PinEnum DigMeasPin = mp[page].MeaPin;
	BRC cnct(mod_id, slot, DigMeasPin);                                                
	ConnectBRCtoBus *FXTtoDrvr = ConnectBRCtoBus::createInstance();
	FXTtoDrvr->setBusID(MeasurementBus::S);      
	FXTtoDrvr->setBRC(cnct);                                                   
	FXTtoDrvr->setState(ConnectBRCtoBus::connect);  
	th->addActionToQueue(FXTtoDrvr);   
	th->processQueuedActions();                                              
	pinCard->spewToVthStream();  
	FXTtoDrvr = 0;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void ConnectLoadtoAF2Shunt(int const &mod_id, int const &slot, TestUtils::ChannelName chnl, HTalon *talonCard, int load)
{
	talonCard->openAllRelays(false, true);	// Initalize Talon
	vth_aopen_all_rlys(mod_id);
	vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
	vth_aclose_rly(mod_id, K751);
	vth_aclose_rly(mod_id, K764);
	vth_aclose_rly(mod_id, TestUtils::onSideA(chnl) ? K733 : K737);
	switch (load)
	{
	case (65):
		talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KT6_5SA : HTalon::KT6_5SB, false, false);
		break;
	case (50):
		talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KT50SA : HTalon::KT50SB, false, false);
		break;
	case (1000):
		talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KT1KSA : HTalon::KT1KSB, false, false);
		break;
	default: 
		assert(false); 
		break; 
	}
	talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KTGNDA : HTalon::KTGNDB, false, false);
	talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KSA : HTalon::KSB, false, false);
	talonCard->closeRelay(TestRelays::mapSChannelRelay(chnl), false, false); 
	talonCard->closeRelay(TestRelays::mapDChannelRelay(chnl), true, true);
}

/*-------------------------------------------------------------------------------------------------------------------*/

void ConnectLoadtoAF2Series(int const &mod_id, int const &slot, TestUtils::ChannelName chnl, HTalon *talonCard, int load)
{
	talonCard->openAllRelays(false, true);	// Initalize Talon
	vth_aopen_all_rlys(mod_id);
	vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
	vth_aclose_rly(mod_id, K754);
	vth_aclose_rly(mod_id, K764);
	vth_aclose_rly(mod_id, TestUtils::onSideA(chnl) ? K718 : K722);
	switch (load)
	{
	case (65):
		talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KT6_5SA : HTalon::KT6_5SB, false, false);
		break;
	case (50):
		talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KT50SA : HTalon::KT50SB, false, false);
		break;
	case (1000):
		talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KT1KSA : HTalon::KT1KSB, false, false);
		break;
	default: 
		assert(false); 
		break; 
	}
	talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KTCOMMA : HTalon::KTCOMMB, false, false);
	talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KADCAA : HTalon::KADCAB, false, false);
	talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KAA : HTalon::KAB, false, false);
	talonCard->closeRelay(TestRelays::mapSChannelRelay(chnl), false, false); 
	talonCard->closeRelay(TestRelays::mapDChannelRelay(chnl), true, true);
}

/*-------------------------------------------------------------------------------------------------------------------*/

void ConnectAF2toADC(int const &mod_id, int const &slot, int side, HTalon *talonCard)
{
	talonCard->openAllRelays(false, true);	
	vth_aopen_all_rlys(mod_id);
	vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
	vth_aclose_rly(mod_id, K754);
	vth_aclose_rly(mod_id, K764);
	vth_aclose_rly(mod_id, side == SIDEA ? K718 : K722);
	talonCard->closeRelay(side == SIDEA ? HTalon::KADCAA : HTalon::KADCAB, false, false);
//Debug: using a 50 shunt to gnd to not blow up ADC
talonCard->closeRelay(side == SIDEA ? HTalon::KT50MA : HTalon::KT50MB, false, false);
talonCard->closeRelay(side == SIDEA ? HTalon::KTGNDA : HTalon::KTGNDB, false, false);
	talonCard->closeRelay(side == SIDEA ? HTalon::KAA : HTalon::KAB, true, true);
	

}

/*-------------------------------------------------------------------------------------------------------------------*/
void ConnectDrivertoAF2(int mod_id, int slot, TestUtils::ChannelName chnl, HTalon *talonCard)
{
	talonCard->openAllRelays(false, true);	// Initalize Talon
	vth_aopen_all_rlys(mod_id);
	vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
	vth_aclose_rly(mod_id, K754);
	vth_aclose_rly(mod_id, K764);
	vth_aclose_rly(mod_id, TestUtils::onSideA(chnl) ? K717 : K721);
	talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KSA : HTalon::KSB, false, false);
	talonCard->closeRelay(TestRelays::mapSChannelRelay(chnl), false, false); 
	talonCard->closeRelay(TestRelays::mapDChannelRelay(chnl), true, true);
}

/*-------------------------------------------------------------------------------------------------------------------*/
void Connect5VtoSBus(int const &mod_id, int const &slot, TestUtils::ChannelName chnl, HTalon *talonCard)
{
	talonCard->openAllRelays(false, true);	// Initalize Talon
	vth_aopen_all_rlys(mod_id);
	vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
	talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KT5VA : HTalon::KT5VB, false, false);
	talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KTCOMMA : HTalon::KTCOMMB, false, false);
	talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KADCAA : HTalon::KADCBA, false, false);
	talonCard->closeRelay(TestUtils::onSideA(chnl) ? HTalon::KSAA : HTalon::KSAB, false, false);
	talonCard->closeRelay(TestRelays::mapSChannelRelay(chnl), true, true); 
}

/*-------------------------------------------------------------------------------------------------------------------*/

void debug_relay_state(int mod_id, int slot)
{
	vth_pmatt4(mod_id, 1, slot, 0, 0, 0);	//Talon card
	vth_pmatt4(mod_id, 1, 1, 0, 0, 0);		//Aim
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

/****************************************************************************
* DMM
****************************************************************************/

void verify_dmm(int mod_id)
{
	viPrintf(viDMM, "RESET\n");
	viClear(viDMM);
	viPrintf(viDMM, "ID?\n");
	err = viScanf(viDMM, "%t", &buf);
	if(err != 0)
		display_prompt("Can't read ID string from DMM", true);
	else
	{
		printf("ID: %s\n", buf);
		display_prompt("DMM okay", true);
	}
    viPrintf(viDMM, "RESET\n");
}

/*------------------------------------------------------------------------*/
void open_dmm(int mod_id)
{
	err = viOpen(rsm, DMM, VI_NULL, VI_NULL, &viDMM );
	if( err != 0 )
    {
		display_prompt("Can't open DMM.", true);
    }
	viSetAttribute(viDMM, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viDMM, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viDMM, "RESET\n");
}  /*open_dmm*/

/*------------------------------------------------------------------------*/
void setup_dmm(int mod_id, int dmm_function, double range, double resolution)
{
	char setup_string[81] = {0};
	int current_position;
	int next_position;
	double percent_res;
	
	*setup_string = '\0';
	strcpy(setup_string, "FUNC ");
	switch (dmm_function)
	{
	case (0):
		strcat(setup_string, "OHMF ");
		break;
	case (1):
		strcat(setup_string, "OHM ");
		break;
	case (2):
		strcat(setup_string, "DCV ");
		break;
	case (3):
		strcat(setup_string, "ACV ");
		break;
	case (4):
		strcat(setup_string, "FREQ ");
		break;
	case (5):
		strcat(setup_string, "PER ");
		break;
	case (6):
		strcat(setup_string, "FIXEDZ ON");
		break;
	default: 
		assert(false);
		break;
	}
	
	/* 11-26-90 JRM Check to see if the range is less than the allowable range */
	/* Also add abs(range) to set the range correctly. */
	range = fabs(range);
	if (range == 0 && dmm_function == 2)	//DCV
		range = 0.100;
	if (range == 0 && dmm_function == 3)	//ACV
		range = 0.010;
	
  current_position = mtd_strlen(setup_string) + 1;
  if(range == DMMAUTO)
	sprintf(setup_string + current_position - 1, "%s", "AUTO");
  else
	sprintf(setup_string + current_position - 1, "%1.2f", range);
  next_position = mtd_strlen(setup_string) + 1;
  strcat(setup_string, ",");
  current_position = next_position + 1;
  percent_res = resolution / range * 100;

	if (dmm_function != DMMFREQ && dmm_function != PER)
		sprintf(setup_string + current_position - 1, "%1.4f\n", percent_res);
	else
		sprintf(setup_string + current_position - 1, "%1.5f\n", resolution);

	viPrintf(viDMM, "NPLC 1\n");
	viPrintf(viDMM, setup_string); 
	if (dmm_function == ACV)
		viPrintf(viDMM, "SETACV SYNC\n");
//	viPrintf(viDMM, "TRIG SGL\n");
	
}  /*setup_dmm*/

/* ------------------------------------------------------------------------ */

void setup_dmm_periodic(int mod_id, double range, double resolution, 
						int start_freq, int stop_freq, int samples)
{
  char setup_string[81];
  int next_position, current_position;
  *setup_string = '\0';

  viPrintf(viDMM, "PRESET FAST\n");		/* PRESET FAST command */
  viPrintf(viDMM, "OFORMAT ASCII\n");
  viPrintf(viDMM, "MFORMAT SREAL\n");
  viPrintf(viDMM, "MEM FIFO\n");				/* MEM FIFO */

  strcpy(setup_string, "PER ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.3f %s %1.4f\n", range, ",", resolution);
  viPrintf(viDMM, setup_string);

  *setup_string = '\0';
  strcpy(setup_string, "ACBAND ");
  current_position = strlen(setup_string) + 1;
  if (start_freq != 0)
  {
    sprintf(setup_string + current_position - 1, "%d %s", start_freq, ",");
    next_position = strlen(setup_string) + 1;
  }
  else
    strcat(setup_string, "0.5");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d\n", stop_freq);
  viPrintf(viDMM, setup_string);

  *setup_string = '\0';
  strcpy(setup_string, "NRDGS ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d %s %s\n", samples, ", ", "AUTO");
  viPrintf(viDMM, setup_string);
  
  viPrintf(viDMM, "TARM SGL\n");

}

/*------------------------------------------------------------------------*/

void setup_dmm_norm(int mod_id)
{
  viPrintf(viDMM,  "PRESET NORM\n");  
}

/*------------------------------------------------------------------------*/

void setup_dmm_ac_band(int mod_id, int start_freq, int stop_freq)
{
   
  char setup_string[81];
  int current_position;
  int next_position;

  *setup_string = '\0';
  strcpy(setup_string, "ACBAND ");
  current_position = strlen(setup_string) + 1;
  if (start_freq != 0)
  {
    sprintf(setup_string + current_position - 1, "%d %s", start_freq, ", ");
    next_position = strlen(setup_string) + 1;
  }
  else
    strcat(setup_string, "0.5");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d\n", stop_freq);
  viPrintf(viDMM, setup_string);
}  /*setup_dmm_AC_Band*/

/* ------------------------------------------------------------------------ */

void setup_dmm_subsampling_AC_and_DC(int mod_id, int dmm_addr, int num_samples, double sampling_rate,
                                        int trig_level, int slope, int range)
{
  ViSession vi;
  char setup_string[81];
  int next_position;

  if (dmm_addr == DMM_ADDRESS)
	vi = viDMM;
  else
	vi = viDMM50;

  viPrintf(vi, "PRESET FAST\n");
  viPrintf(vi, "OFORMAT ASCII\n");
  viPrintf(vi, "MFORMAT SINT\n");
  viPrintf(vi, "MEM FIFO\n");   

  if (dmm_addr == DMM_ADDRESS)
	  enable_dmm_buffer(mod_id);
  else
	  enable_dmm50_buffer(mod_id);

//SSDC was chosen to make sure that the whole waveform was captured
  strcpy(setup_string, "SSDC \n");   
  sprintf(setup_string + 5, "%d\n", range);
  viPrintf(vi, setup_string);

  strcpy(setup_string, "SWEEP \n");  
  sprintf(setup_string + 6, "% .4E", sampling_rate);
  next_position = strlen(setup_string) + 1;
  strcat(setup_string, ",\n");
  sprintf(setup_string + next_position, "%d\n", num_samples);
  viPrintf(vi, setup_string);

  strcpy(setup_string, "LEVEL \n");  
  sprintf(setup_string + 6, "%d\n", trig_level);
  viPrintf(vi, setup_string);

if (slope == POS)
  viPrintf(vi, "SLOPE POS\n");
else
  viPrintf(vi, "SLOPE NEG\n");

  viPrintf(vi, "LFILTER OFF\n");

}/*setup_dmm_subsampling_AC_and_DC*/

/*------------------------------------------------------------------------*/
void setup_dmm_aperature(int mod_id, double aperature)
{
   
  char setup_string[81];
  int current_position;

  *setup_string = '\0';
  strcpy(setup_string, "APER ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%10.8f\n", aperature);
  viPrintf(viDMM, setup_string);

}  /*setup_dmm_aperature*/

/*------------------------------------------------------------------------*/
void setup_dmm_fixed_z(int mod_id, int on)
{
   
  if (on == true)
    viPrintf(viDMM, "FIXEDZ ON\n");
  else
    viPrintf(viDMM, "FIXEDZ OFF\n");
}  /*setup_dmm_fixed_z*/

/*------------------------------------------------------------------------*/
void setup_dmm_fsource(int mod_id, int fsource)
{
   
  char setup_string[81];
  int current_position;

  *setup_string = '\0';
  strcpy(setup_string, "FSOURCE ");
  current_position = strlen(setup_string) + 1;
  if (fsource == ACV)
    strcat(setup_string, "ACV\n");
  else if (fsource == DCV)
    strcat(setup_string, "ACDCV\n");

  viPrintf(viDMM, setup_string);

}  /*setup_dmm_fsource*/

/*------------------------------------------------------------------------*/
void setup_dmm_level(int mod_id, int trigger_percent, int coupling)
{
  char setup_string[81];
  int current_position;

  *setup_string = '\0';
  strcpy(setup_string, "LEVEL ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d %s %d\n", trigger_percent, ",", coupling);
  viPrintf(viDMM, setup_string);

}  /*setup_dmm_level*/

/*------------------------------------------------------------------------*/
void setup_dmm_level_filter(int mod_id, int filter_control)
{
   
  if (filter_control == true)
    viPrintf(viDMM, "LFILTER ON\n");
  else
    viPrintf(viDMM, "LFILTER OFF\n");

}  /*setup_dmm_level_filter*/

/*------------------------------------------------------------------------*/
void setup_dmm_nplc(int mod_id, int nplc)
{
   
  char setup_string[81];
  int current_position;

  *setup_string = '\0';
  strcpy(setup_string, "NPLC ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d\n", nplc);
  viPrintf(viDMM, setup_string);
}  /*setup_dmm_NPLC*/

/*------------------------------------------------------------------------*/
void setup_dmm_memory(int mod_id, int fmat, int mem)
{
   
  switch (fmat)
  {

  case ASCII:
    viPrintf(viDMM, "MFORMAT ASCII\n");
    break;

  case SINT:
    viPrintf(viDMM, "MFORMAT SINT\n");
    break;

  case DINT:
    viPrintf(viDMM, "MFORMAT DINT\n");
    break;

  case SREAL:
    viPrintf(viDMM, "MFORMAT SREAL\n");
    break;

  case DREAL:
    viPrintf(viDMM, "MFORMAT DREAL\n");
    break;

  default:
    assert(false);
    break;

  }
  switch (mem)
  {

  case MOFF:
    viPrintf(viDMM, "MEM OFF\n");
    break;

  case LIFO:
    viPrintf(viDMM, "MEM LIFO\n");
    break;

  case FIFO:
    viPrintf(viDMM, "MEM FIFO\n");
    break;

  case CONT:
    viPrintf(viDMM, "MEM CONT\n");
    break;

  default:
    assert(false);
    break;

  }
}

/*------------------------------------------------------------------------*/
void setup_dmm_ocomp_on(int mod_id)
{
  viPrintf(viDMM, "OCOMP ON\n");
}  /*setup_dmm_ocomp_on*/

/*------------------------------------------------------------------------*/
double measure_dmm(int mod_id)
{
  double res;
  viQueryf(viDMM, "TRIG SGL\n", "%Lf", &res);
  return res;
}  /*measure_dmm*/

/*------------------------------------------------------------------------*/
int check_dmm_measured(int mod_id)
{
  int res;//  double res;
  err = viQueryf(viDMM, "TRIG SGL\n", "%d", &res); //"%Lf", &res);
  return err;
}

/*------------------------------------------------------------------------*/
void dmm_display(int mod_id, int dmm_addr, bool mode)
{
  ViSession vi;

  if (dmm_addr == DMM_ADDRESS)
	vi = viDMM;
  else
	vi = viDMM50;

  if(mode)
	viPrintf(vi, "DISP ON\n");
  else
	viPrintf(vi, "DISP OFF\n");
}

/*------------------------------------------------------------------------*/
void close_dmm(int mod_id)
{
  err = viScanf(viDMM, "%t", &buf); 
  viClear(viDMM);
  viPrintf(viDMM, "RESET\n");
  viClose(viDMM);
}  /*close_dmm*/

/*------------------------------------------------------------------------*/
double read_dmm_stats(int mod_id, int dmm_stat)
{
  double res;

  switch (dmm_stat)
  {

  case SDEV:
	viQueryf(viDMM,  "RMATH SDEV\n", "%Lf", &res);
    break;

  case MEAN:
	viQueryf(viDMM,  "RMATH MEAN\n", "%Lf", &res);
    break;

  case N:
	viQueryf(viDMM,  "RMATH NSAMP\n", "%Lf", &res);
    break;

  case SALES_MAX:
	viQueryf(viDMM,  "RMATH UPPER\n", "%Lf", &res);
    break;

  case SALES_MIN:
	viQueryf(viDMM,  "RMATH LOWER\n", "%Lf", &res);
    break;

  case ALL_STATS:
	viQueryf(viDMM,  "RMATH MEAN\n", "%Lf", &res);
	viQueryf(viDMM,  "RMATH SDEV\n", "%Lf", &res);
	viQueryf(viDMM,  "RMATH NSAMP\n", "%Lf", &res);
	viQueryf(viDMM,  "RMATH LOWER\n", "%Lf", &res);
	viQueryf(viDMM,  "RMATH UPPER\n", "%Lf", &res);
    break;
  default:
    assert(false);
    break;
  
  }/* case */

  return res;
}

/*------------------------------------------------------------------------*/
void setup_dmm_readings(int mod_id, int n)
{
  char setup_string[81];
  int c_pos;

  strcpy(setup_string, "NRDGS ");
  c_pos = strlen(setup_string) + 1;
  sprintf(setup_string + c_pos - 1, "%d\n", n);
  viPrintf(viDMM, setup_string);
}

/*------------------------------------------------------------------------*/
void calculate_dmm_stats(int mod_id)
{
  viPrintf(viDMM, "MMATH STAT\n");
}

/*------------------------------------------------------------------------*/
void hold_dmm(int mod_id)
{
  viPrintf(viDMM, "TRIG HOLD\n");
}

/*------------------------------------------------------------------------*/
void trigger_dmm(int mod_id)
{
  viPrintf(viDMM, "TRIG SGL\n");
}

/* ----------------------------------------------------------------------- */

void arm_dmm(int mod_id)
{
  viPrintf(viDMM, "TARM SGL\n");
}


/*------------------------------------------------------------------------*/
double read_dmm(int mod_id)
{
  double res;
  viScanf(viDMM, "%Lf", &res);
  return res;
}

/*------------------------------------------------------------------------*/
void enable_dmm_buffer(int mod_id)
{
  viPrintf(viDMM, "INBUF ON\n");
}

/*------------------------------------------------------------------------*/
void disable_dmm_buffer(int mod_id)
{
  viPrintf(viDMM, "INBUF OFF\n");
}

/* ---------------------------------------------------------------------- */
#define MARK_NUMBER     0
#define MAX_TIME        120.0e+00

void wait_for_dmm_measurement(int mod_id, int dmm_address)
{
  ViSession vi;
  int status;
  double time = 0.0;
  test_result_type result;

  if (dmm_address == DMM_ADDRESS)
	vi = viDMM;
  else
	vi = viDMM50;

  vth_mtmr_set_mark(mod_id, MARK_NUMBER);
  do
  {
    vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUMBER);
    if (get_result_nolog(mod_id, &result))
      time = result.UU.Real_result;
    else
    {
      time = MAX_TIME;
      display_prompt("No result in result log (Expected mark time)", true);
    }
	viQueryf(vi, "ERRSTR?\n", "%t", &buf);
	viQueryf(vi, "STB?\n", "%3d", &status);
  }  while (!(status == 128 || time >= MAX_TIME));

  if(time >= MAX_TIME)
	  display_prompt("DMM measurement timed out", true);

}  /*wait_for_dmm_measurement*/

#undef MARK_NUMBER
#undef MAX_TIME

/*------------------------------------------------------------------------*/
void autocal_dmm(int mod_id)
{
  enable_dmm_buffer(mod_id);
  viPrintf(viDMM, "ACAL DCV,3458\n");
  flush_commands(mod_id);
  wait_for_dmm_measurement(mod_id, DMM_ADDRESS);
  flush_commands(mod_id);
  viPrintf(viDMM, "ACAL AC,3458\n");
  flush_commands(mod_id);
  wait_for_dmm_measurement(mod_id, DMM_ADDRESS);
  flush_commands(mod_id);
}  /*autocal_dmm*/

/*------------------------------------------------------------------------*/
void setup_dcv_dig(int mod_id, int range, double interval, int num_samples, double aperature, 
				   int level, int mode)
{
  char setup_string[81];
  int current_position;

  viPrintf(viDMM, "PRESET DIG\n");  /* PRESET DIG command */

  strcpy(setup_string, "APER ");	/* APER command */
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "% .3E\n", aperature);
  viPrintf(viDMM, setup_string);

  viPrintf(viDMM, "OFORMAT ASCII\n");
  if (aperature > 1.4e-6)
    viPrintf(viDMM, "MFORMAT DINT\n");//SREAL\n");
  else
    viPrintf(viDMM, "MFORMAT SINT\n");//SREAL\n");
  
  viPrintf(viDMM, "MEM FIFO\n");   /* MEM FIFO */

  strcpy(setup_string, "DCV ");		/* DCV range */
  sprintf(setup_string + 4, "%d\n", range);
  viPrintf(viDMM, setup_string);

  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */
  sprintf(setup_string + 6, "% .4E %s %d\n", interval, ",", num_samples);
  viPrintf(viDMM, setup_string);

  strcpy(setup_string, "LEVEL ");
  sprintf(setup_string + 6, "%d %s %d\n", level, ",", mode);  
  viPrintf(viDMM, setup_string);

}  /* setup_dcv_dig */

/* ----------------------------------------------------------------------- */
void setup_ssdc_dig(int mod_id, int range, double interval, int num_samples)
{
  char setup_string[81];
  int next_position;

  viPrintf(viDMM, "PRESET DIG\n");   /* PRESET DIG command */
  viPrintf(viDMM, "MEM FIFO\n");   /* MEM FIFO */
  viPrintf(viDMM, "OFORMAT SREAL\n");   /* OFORMAT SREAL */

  strcpy(setup_string, "SSDC ");
  sprintf(setup_string + 5, "%d\n", range);
  viPrintf(viDMM, setup_string);

  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */
  sprintf(setup_string + 6, "% .3E", interval);
  next_position = strlen(setup_string) + 1;
  sprintf(setup_string + next_position, "%d\n", num_samples);
  viPrintf(viDMM, setup_string);

  viPrintf(viDMM, "LEVEL 0, DC\n");
  viPrintf(viDMM, "LFILTER ON\n");
  viPrintf(viDMM, "SSRC LEVEL\n");
}  /* setup_ssdc_dig */

/* ----------------------------------------------------------------------- */
void setup_ssdc_fast(int mod_id, int range, int level, int slope, double interval, double sample_delay, int num_samples)
{
  char setup_string[81];
  viPrintf(viDMM, "PRESET FAST\n");   /* PRESET FAST command */
  viPrintf(viDMM, "MEM FIFO\n");   /* MEM FIFO */
  viPrintf(viDMM, "OFORMAT SREAL\n");   /* OFORMAT SREAL */
  viPrintf(viDMM, "MFORMAT SINT\n");   /* MFORMAT SINT */

  sprintf(setup_string + 5, "%s %d\n", "SSDC ", range);
  viPrintf(viDMM, setup_string);

  sprintf(setup_string + 6, "%s % .3E %d\n", "SWEEP ", interval, num_samples); /* SWEEP interval,num_samples */
  viPrintf(viDMM, setup_string);

  sprintf(setup_string + 6, "%s %d %s\n", "LEVEL ", level, ", DC");
  viPrintf(viDMM, setup_string);

  if (slope == NEG)
    viPrintf(viDMM, "SLOPE NEG\n");

  sprintf(setup_string + 6, "%s % .5E\n", "DELAY ", sample_delay);
  viPrintf(viDMM, setup_string);

  viPrintf(viDMM, "SSRC LEVEL\n");
}  /* setup_ssdc_fast */

/* ----------------------------------------------------------------------- */

//=======================================================================
/* ------------------------------------------------------------------------ */
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

/*------------------------------------------------------------------------*/
void setup_dsdc_dig(int mod_id, int range, double interval, int num_samples)
{
  char setup_string[81];

  viPrintf(viDMM, "PRESET FAST\n");   /* PRESET DIG command */

  viPrintf(viDMM, "OFORMAT ASCII\n");
  viPrintf(viDMM, "MFORMAT SREAL\n");
  viPrintf(viDMM, "MEM FIFO\n");   /* MEM FIFO */

  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */
  sprintf(setup_string + 6, "% .4E %s %d\n", interval, ",", num_samples);
  viPrintf(viDMM, setup_string);

  strcpy(setup_string, "DSDC ");   /* DCV range */
  sprintf(setup_string + 5, "%d\n", range);
  viPrintf(viDMM, setup_string);

  viPrintf(viDMM, "LEVEL 0, DC\n");
  viPrintf(viDMM, "TRIG LEVEL\n");

}  /* setup_dsdc_dig */

/* ----------------------------------------------------------------------- */

#define MARK_NUMBER	0
#define MAX_TIME	30.0

void capture_ddm(int mod_id, int num_samples, double *waveform, int dmm_addr)
{
	ViSession	vi;
	ViUInt16	rdcnt = 0;
	ViChar		ReturnedData[100];
	unsigned long	actual; // Number of characters sent/returned
	char		cmd_str[50];
	char		cmd_str1[50];
	int			status;
	double time = 0.0;
    test_result_type result;

//	FILE *ofile;
//    ofile = fopen("error.txt", "a");

	if (dmm_addr == DMM_ADDRESS)
		vi = viDMM;
	else
		vi = viDMM50;
    
	viPrintf(vi, "BEEP OFF\n");
	viPrintf(vi, "TARM SGL\n");
   	vth_mtmr_wait(mod_id, MEASUREMENT_WAIT);
	flush_commands(mod_id);

	vth_mtmr_set_mark(mod_id, MARK_NUMBER);
    do
	{
      vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUMBER);
      if (get_result_nolog(mod_id, &result))
        time = result.UU.Real_result;
      viQueryf(vi, "ERRSTR?\n", "%t", &buf);
	  viQueryf(vi, "STB?\n", "%3d", &status);
	} while (!(status == 128 || time >= MAX_TIME));
	viRead(vi, (ViBuf)ReturnedData, num_samples, &actual);
	sprintf(cmd_str, "%s %d\n", "RMEM 1,", num_samples - 1);
	sprintf(cmd_str1, "%%,%dlf", num_samples);
	err = viQueryf(vi, cmd_str, cmd_str1, waveform);
	viClear(vi);
	viPrintf(vi, "RESET\n");

    if (dmm_addr == DMM_ADDRESS) disable_dmm_buffer(mod_id);
    if (dmm_addr == DMM50_ADDRESS) disable_dmm50_buffer(mod_id);
//    fclose(ofile);

}  /*capture_ddm*/

#undef MARK_NUMBER
#undef MAX_TIME

/*------------------------------------------------------------------------*/

void setup_dmm_digitizing(int mod_id, int dmm_addr, int range, double interval,
       int num_samples, double aperature, double resolution, int level, int slope, int mode)
{
  ViSession vi;
  char setup_string[81];
  int current_position;
  double percent_resolution;

  if (dmm_addr == DMM_ADDRESS)
	vi = viDMM;
  else
	vi = viDMM50;
  
  viPrintf(vi, "PRESET DIG\n");   /* PRESET DIG command */

  strcpy(setup_string, "APER ");   /* APER command */
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "% .3E\n", aperature);
  viPrintf(vi, setup_string);

  viPrintf(vi, "OFORMAT ASCII\n");
  viPrintf(vi, "MFORMAT SREAL\n");
  viPrintf(vi, "MEM FIFO\n");   /* MEM FIFO */

  //The precent_resolution is the actual resolution divided by the range(or max_imput) times 100...
  percent_resolution = (resolution/range)*100;

  strcpy(setup_string, "DCV ");   /* DCV range */
  sprintf(setup_string + 4, "%d %s %7.3E\n", range, ",", percent_resolution);
  viPrintf(vi, setup_string);

  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */
  sprintf(setup_string + 6, "% .4E %s %d\n", interval, ",", num_samples);
  viPrintf(viDMM, setup_string);

  strcpy(setup_string, "LEVEL ");
  sprintf(setup_string + 6, "%d %s %d\n", level, ",", mode);  
  viPrintf(viDMM, setup_string);

  if (slope == POS)
    viPrintf(vi, "SLOPE POS\n");
  else
    viPrintf(vi, "SLOPE NEG\n");

  if (dmm_addr == DMM_ADDRESS)
	  enable_dmm_buffer(mod_id);
  else
	  enable_dmm50_buffer(mod_id);

}

/* ------------------------------------------------------------------------ */

double get_dmm_volt(int mod_id, int dmm_addr, double *waveform, int num_samples, int mode)
{
	ViSession	vi;
	ViUInt16	rdcnt = 0;
	ViChar		ReturnedData[100];
	unsigned long	actual; // Number of characters sent/returned
	int			status;
	int			records;
	double		max = 0.0;
    double		min = 0.0;
	char		cmd_str[50];
	char		cmd_str1[50];

	if (dmm_addr == DMM_ADDRESS)
		vi = viDMM;
	else
		vi = viDMM50;

	do
	{
		viQueryf(vi, "ERRSTR?\n", "%t", &buf);
		viQueryf(vi, "STB?\n", "%3d", &status);
	} while (status != 128);
	viRead(vi, (ViBuf)ReturnedData, num_samples, &actual);
	sprintf(cmd_str, "%s %d\n", "RMEM 1,", num_samples - 1);
	sprintf(cmd_str1, "%%,%dlf", num_samples);
	err = viQueryf(vi, cmd_str, cmd_str1, waveform);
	viQueryf(vi, "MCOUNT?\n", "%4d", &records);
	for(int i = 0; i < records - 1; i++)
	{
		if(max < waveform[i])
		  max = waveform[i];
		if(min > waveform[i])
			min = waveform[i];
	}	
	viClear(vi);
	viPrintf(vi, "RESET\n");
    if (dmm_addr == DMM_ADDRESS) 
		disable_dmm_buffer(mod_id);
    else
		disable_dmm50_buffer(mod_id);
    if(mode == MAX)
	  return max;
    else
	  return min;
}  /*get_dmm_volt*/

/*************************************************************************
* (DMM50) Digital Multimeter with 50 ohm load 
*************************************************************************/

void verify_dmm50(int mod_id)
{
	viPrintf(viDMM50, "RESET\n");
	viClear(viDMM50);
	viPrintf(viDMM50, "ID?\n");
	err = viScanf(viDMM50, "%t", &buf);
	if(err != 0)
		display_prompt("Can't read ID string from DMM50", true);
	else
	{
		printf("ID: %s\n", buf);
		display_prompt("DMM50 okay", true);
	}
	viPrintf(viDMM50, "RESET\n");
}

/*------------------------------------------------------------------------*/

/*Verify DMM50 button*/
/* ------------------------------------------------------------------------ */

boolean is_dmm50_rear_input(int mod_id)
{
  double dmm50_impedance;
  
  connect(mod_id, 822, false);  // for rear input verification
  connect(mod_id, 912, false);  // for front input verification
  flush_commands(mod_id);
  open_dmm50(mod_id);
  setup_dmm50(mod_id, TWO_WIRE_OHMS, 100, 1);
  flush_commands(mod_id);
  dmm50_impedance = measure_dmm(mod_id);

  if(dmm50_impedance > 100) 
	  return false;
  else
	  return true;
}
	
/*End verify DMM50*/
/* ------------------------------------------------------------------------ */

void open_dmm50(int mod_id)
{
  err = viOpen(rsm, DMM50, VI_NULL, VI_NULL, &viDMM50 );
  if( err != 0 )
  {
	display_prompt("Can't open DMM50.", true);
	exit( 1 );
  }
  viSetAttribute(viDMM50, VI_ATTR_TMO_VALUE, HPIB_TO);
  viSetAttribute(viDMM50, VI_ATTR_TERMCHAR_EN, VI_TRUE);
  viPrintf(viDMM50, "RESET\n");
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
	switch (dmm_function)
	{
	case (0):
		strcat(setup_string, "OHMF ");
		break;
	case (1):
		strcat(setup_string, "OHM ");
		break;
	case (2):
		strcat(setup_string, "DCV ");
		break;
	case (3):
		strcat(setup_string, "ACV ");
		break;
	case (4):
		strcat(setup_string, "FREQ ");
		break;
	case (5):
		strcat(setup_string, "PER ");
		break;
	case (6):
		strcat(setup_string, "FIXEDZ ON ");
		break;
	default: 
		assert(false);
		break;
	}
	
	/* 11-26-90 JRM Check to see if the range is less than the allowable range */
	/* Also add abs(range) to set the range correctly. */
	range = fabs(range);
	if (range == 0 && dmm_function == 2)	//DCV
		range = 0.100;
	if (range == 0 && dmm_function == 3)	//ACV
		range = 0.010;

    percent_res = resolution / range * 100;
    current_position = strlen(setup_string) + 1;
    sprintf(setup_string + current_position - 1, "%1.2f", range);
    next_position = strlen(setup_string) + 1;
    strcat(setup_string, ", ");
    current_position = next_position + 1;

	if (dmm_function != DMMFREQ && dmm_function != PER)
		sprintf(setup_string + current_position - 1, "%1.4f\n", percent_res);
	else
		sprintf(setup_string + current_position - 1, "%1.5f\n", resolution);

/*  
    current_position = strlen(setup_string) + 1;

	if (dmm_function != DMMFREQ && dmm_function != PER)
        sprintf(setup_string + current_position - 1, "%1.2f %s %1.4f\n", range, ", ", percent_res);
	else
		sprintf(setup_string + current_position - 1, "%1.2f %s %1.5f\n", range, ", ", resolution);

*/	
	viPrintf(viDMM50,  "NPLC 1\n");
	viPrintf(viDMM50,  setup_string);
	if (dmm_function == ACV)
		viPrintf(viDMM50,  "SETACV SYNC\n");
	
}  /*setup_dmm50*/

/*------------------------------------------------------------------------*/
void setup_dmm50_ac_band(int mod_id, int start_freq, int stop_freq)
{
  char setup_string[81];
  int current_position;

  *setup_string = '\0';
  strcpy(setup_string, "ACBAND ");
  current_position = strlen(setup_string) + 1;
  if (start_freq != 0)
    sprintf(setup_string + current_position - 1, "%d", start_freq);
  else
    strcat(setup_string, "0.5");

  sprintf(setup_string + current_position - 1, "%d\n", stop_freq);
  viPrintf(viDMM50,  setup_string);

}  /*setup_dmm50_AC_Band*/

/*------------------------------------------------------------------------*/
void setup_dmm50_aperature(int mod_id, double aperature)
{
  char setup_string[81];
  int current_position =0;

  *setup_string = '\0';
  sprintf(setup_string + current_position - 1, "%s %10.8f\n", "APER ", aperature);
  viPrintf(viDMM50,  setup_string);

}  /*setup_dmm50_aperature*/

/*------------------------------------------------------------------------*/
void setup_dmm50_fixed_z(int mod_id, int on)
{  /*setup_dmm_fixed_z*/
  
  if (on == true)
    viPrintf(viDMM50,  "FIXEDZ ON\n");
  else
    viPrintf(viDMM50,  "FIXEDZ OFF\n");
}  /*setup_dmm50_fixed_z*/

/*------------------------------------------------------------------------*/
void setup_dmm50_fsource(int mod_id, int fsource)
{
  
  char setup_string[81];
  int current_position;

  *setup_string = '\0';
  strcpy(setup_string, "FSOURCE ");
  current_position = strlen(setup_string) + 1;
  if (fsource == ACV)
    strcat(setup_string, "ACV");
  else if (fsource == DCV)
    strcat(setup_string, "ACDCV");
  strcat(setup_string, "\n");
  viPrintf(viDMM50,  setup_string);

}  /*setup_dmm50_fsource*/

/*------------------------------------------------------------------------*/
void setup_dmm50_level(int mod_id, int trigger_percent, int coupling)
{
  
  char setup_string[81];
  int current_position;
  int next_position;

  *setup_string = '\0';
  strcpy(setup_string, "LEVEL ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d", trigger_percent);
  next_position = strlen(setup_string) + 1;
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d\n", coupling);
  viPrintf(viDMM50,  setup_string);
}  /*setup_dmm50_level*/

/*------------------------------------------------------------------------*/
void setup_dmm50_level_filter(int mod_id, int filter_control)
{
  
  if (filter_control == true)
    viPrintf(viDMM50,  "LFILTER ON\n");
  else
    viPrintf(viDMM50,  "LFILTER OFF\n");
}  /*setup_dmm50_level_filter*/

/*------------------------------------------------------------------------*/
void setup_dmm50_nplc(int mod_id, int nplc)
{
  char setup_string[81];
  int current_position;

  *setup_string = '\0';
  strcpy(setup_string, "NPLC ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d\n", nplc);
  viPrintf(viDMM50,  setup_string);
}  /*setup_dmm50_NPLC*/

/*------------------------------------------------------------------------*/
void setup_dmm50_memory(int mod_id, int fmat, int mem)
{
  switch (fmat)
  {

  case ASCII:
    viPrintf(viDMM50,  "MFORMAT ASCII\n");
    break;

  case SINT:
    viPrintf(viDMM50,  "MFORMAT SINT\n");
    break;

  case DINT:
    viPrintf(viDMM50,  "MFORMAT DINT\n");
    break;

  case SREAL:
    viPrintf(viDMM50,  "MFORMAT SREAL\n");
    break;

  case DREAL:
    viPrintf(viDMM50,  "MFORMAT DREAL\n");
    break;

  default:
    assert(false);
    break;

  }
  switch (mem)
  {

  case MOFF:
    viPrintf(viDMM50,  "MEM OFF\n");
    break;

  case LIFO:
    viPrintf(viDMM50,  "MEM LIFO\n");
    break;

  case FIFO:
    viPrintf(viDMM50,  "MEM FIFO\n");
    break;

  case CONT:
    viPrintf(viDMM50,  "MEM CONT\n");
    break;

  default:
    assert(false);
    break;

  }
}

/*------------------------------------------------------------------------*/
void setup_dmm50_ocomp_on(int mod_id)
{
  viPrintf(viDMM50,  "OCOMP ON\n");
}  /*setup_dmm50_ocomp_on*/

/*------------------------------------------------------------------------*/
double measure_dmm50(int mod_id)
{
  double res;
  viQueryf(viDMM50,  "TRIG SGL\n", "%Lf", &res);
  return res;
}  /*measure_dmm50*/

/*------------------------------------------------------------------------*/
void close_dmm50(int mod_id)
{
  err = viScanf(viDMM50, "%t", &buf);
  viClear(viDMM50);
  viPrintf(viDMM50,  "RESET\n");
  viClose(viDMM50);
}  /*close_dmm50*/

/*------------------------------------------------------------------------*/
double read_dmm50_stats(int mod_id, int dmm_stat)
{
  double res;

  switch (dmm_stat)
  {

  case SDEV:
	viQueryf(viDMM50,  "RMATH SDEV\n", "%Lf", &res);
    break;

  case MEAN:
	viQueryf(viDMM50,  "RMATH MEAN\n", "%Lf", &res);
    break;

  case N:
	viQueryf(viDMM50,  "RMATH NSAMP\n", "%Lf", &res);
    break;

  case SALES_MAX:
	viQueryf(viDMM50,  "RMATH UPPER\n", "%Lf", &res);
    break;

  case SALES_MIN:
	viQueryf(viDMM50,  "RMATH LOWER\n", "%Lf", &res);
    break;

  case ALL_STATS:  //I know this won't work, however, this option is NOT used at the time of writing.
	viQueryf(viDMM50,  "RMATH MEAN\n", "%Lf", &res);
	viQueryf(viDMM50,  "RMATH SDEV\n", "%Lf", &res);
	viQueryf(viDMM50,  "RMATH NSAMP\n", "%Lf", &res);
	viQueryf(viDMM50,  "RMATH LOWER\n", "%Lf", &res);
	viQueryf(viDMM50,  "RMATH UPPER\n", "%Lf", &res);
    break;

  default:
    assert(false);
    break;
  
  }/* case */

  return res;
}

/*------------------------------------------------------------------------*/
double read_dmm50(int mod_id)
{
  double res;
  viScanf(viDMM50, "%Lf", &res);
  return res;
}

/*------------------------------------------------------------------------*/
void setup_dmm50_readings(int mod_id, int n)
{
  char setup_string[81];
  int c_pos;

  strcpy(setup_string, "NRDGS ");
  c_pos = strlen(setup_string) + 1;
  sprintf(setup_string + c_pos - 1, "%d\n", n);
  viPrintf(viDMM50,  setup_string);
}

/*------------------------------------------------------------------------*/
void calculate_dmm50_stats(int mod_id)
{  
  viPrintf(viDMM50, "MMATH STAT\n");
}

/*------------------------------------------------------------------------*/
void hold_dmm50(int mod_id)
{  
  viPrintf(viDMM50,  "TRIG HOLD\n");
}

/*------------------------------------------------------------------------*/
void trigger_dmm50(int mod_id)
{  
  viPrintf(viDMM50,  "TRIG SGL\n");
}

/*------------------------------------------------------------------------*/
void enable_dmm50_buffer(int mod_id)
{
  viPrintf(viDMM50,  "INBUF ON\n");
}

/*------------------------------------------------------------------------*/
void disable_dmm50_buffer(int mod_id)
{
  viPrintf(viDMM50,  "INBUF OFF\n");
}

/*------------------------------------------------------------------------*/
void autocal_dmm50(int mod_id)
{
  
  enable_dmm50_buffer(mod_id);
  viPrintf(viDMM50,  "ACAL DCV,3458\n");
  flush_commands(mod_id);
  wait_for_dmm_measurement(mod_id, DMM50_ADDRESS);
  flush_commands(mod_id);
  viPrintf(viDMM50,  "ACAL AC,3458\n");
  flush_commands(mod_id);
  wait_for_dmm_measurement(mod_id, DMM50_ADDRESS);
  flush_commands(mod_id);
}  /*autocal_dmm50*/

/*------------------------------------------------------------------------*/

void setup_dmm50_norm(int mod_id)
{
  viPrintf(viDMM50,  "PRESET NORM\n");  
}

/*------------------------------------------------------------------------*/
void setup_dcv50_dig(int mod_id, int range, double interval, int num_samples, double aperature, 
				   int level, int mode)
{
  char setup_string[81];
  int current_position;
  
  viPrintf(viDMM50, "PRESET DIG\n");  /* PRESET DIG command */

  strcpy(setup_string, "APER ");	/* APER command */
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "% .3E\n", aperature);
  viPrintf(viDMM50, setup_string);

  viPrintf(viDMM50, "OFORMAT ASCII\n");
  viPrintf(viDMM50, "MFORMAT SREAL\n");
  viPrintf(viDMM50, "MEM FIFO\n");   /* MEM FIFO */

  strcpy(setup_string, "DCV ");		/* DCV range */
  sprintf(setup_string + 4, "%d\n", range);
  viPrintf(viDMM50, setup_string);

  strcpy(setup_string, "SWEEP ");   /* SWEEP interval,num_samples */
  sprintf(setup_string + 6, "% .4E %s %d\n", interval, ",", num_samples);
  viPrintf(viDMM50, setup_string);

  strcpy(setup_string, "LEVEL ");
  sprintf(setup_string + 6, "%d %s %d\n", level, ",", mode);  
  viPrintf(viDMM50, setup_string);

}  /* setup_dcv50_dig */

/* ----------------------------------------------------------------------- */
void setup_ssdc50_dig(int mod_id, int range, double interval, int num_samples)
{
  char setup_string[81];

  viPrintf(viDMM50,  "PRESET DIG\n");   /* PRESET DIG command */
  viPrintf(viDMM50,  "MEM FIFO\n");   /* MEM FIFO */
  viPrintf(viDMM50,  "OFORMAT SREAL\n");   /* OFORMAT SREAL */
  sprintf(setup_string + 5, "%s %d\n", "SSDC ", range);
  viPrintf(viDMM50,  setup_string);

  sprintf(setup_string + 6, "%s % .3E %d\n", "SWEEP ", interval, num_samples);
  viPrintf(viDMM50,  setup_string);

  viPrintf(viDMM50,  "LEVEL 0, DC\n");
  viPrintf(viDMM50,  "LFILTER ON\n");
  viPrintf(viDMM50,  "SSRC LEVEL\n");
}  /* setup_ssdc50_dig */

/* ----------------------------------------------------------------------- */
void setup_ssdc50_fast(int mod_id, int range, int level, int slope, double interval, double sample_delay, int num_samples)
{
  char setup_string[81];

  viPrintf(viDMM50,  "PRESET FAST\n");   /* PRESET FAST command */
  viPrintf(viDMM50,  "MEM FIFO\n");   /* MEM FIFO */
  viPrintf(viDMM50,  "OFORMAT SREAL\n");   /* OFORMAT SREAL */
  viPrintf(viDMM50,  "MFORMAT SINT\n");   /* MFORMAT SINT */

  strcpy(setup_string, "SSDC ");
  sprintf(setup_string + 5, "%d\n", range);
  viPrintf(viDMM50,  setup_string);

  strcpy(setup_string, "SWEEP ");
  sprintf(setup_string + 6, "% .3E %s %d\n", interval, ",", num_samples);
  viPrintf(viDMM50,  setup_string);

  strcpy(setup_string, "LEVEL ");
  sprintf(setup_string + 6, "%d %s\n", level, ", DC");
  viPrintf(viDMM50,  setup_string);

  if (slope == NEG)
    viPrintf(viDMM50, "SLOPE NEG\n");
  else
	viPrintf(viDMM50, "SLOPE POS\n");

  strcpy(setup_string, "DELAY ");
  sprintf(setup_string + 6, "% .5E\n", sample_delay);
  viPrintf(viDMM50,  setup_string);

  viPrintf(viDMM50,  "SSRC LEVEL\n");
}  /* setup_ssdc50_fast */

/* ----------------------------------------------------------------------- */

void clear_dmm50_memory(int mod_id)
{  
  viPrintf(viDMM50,  "MEM FIFO\n");   /* MEM FIFO */
}  

/* ----------------------------------------------------------------------- */

void arm_dmm50(int mod_id)
{
  viPrintf(viDMM50, "TARM SGL\n");
}

/*------------------------------------------------------------------------*/
void connect_dmm50_out_af2(int mod_id)
{
  connect(mod_id, 912, true);
  connect(mod_id, mod_id + 901, true);
}  /*connect_dmm50_out_AF2*/

/*------------------------------------------------------------------------*/


void check_dmm50_button(int mod_id, int usage)
{
  double dmm50_impedance;
  char buffer[80];
  bool fxt_ok = false;
  int fail = 0;

  connect(mod_id, 822, false);  // for digital
  connect(mod_id, 912, false);  // for analog
  OpenResourceManager();
  open_dmm50(mod_id);
  setup_dmm50(mod_id, TWO_WIRE_OHMS, 100, 1);

  while ((!fxt_ok) && (fail < 5))
  { 
    dmm50_impedance = measure_dmm50(mod_id);
	if(((usage == DMM50_DIGITAL_USAGE) && (dmm50_impedance <= 100)) ||
       ((usage == DMM50_ANALOG_USAGE) && (dmm50_impedance >100)) )
	{
		fxt_ok = true;
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
  close_dmm50(mod_id);
}

/*************************************************************************
* Minerva (mux)
*************************************************************************/

void verify_mux(int mod_id)
{
	char   rack_serial_number[81];

	viPrintf(viMUX, "RESET\n");
	viPrintf(viMUX, "ID?\n");
	err = viScanf(viMUX, "%t", &buf);
	if(err != 0)
		display_prompt("Can't read ID string from MUX", true);
	else
	{
		printf("Mux ID: %s\n", buf);
		get_sales_rack_serial_number(rack_serial_number, mod_id);
		display_prompt(rack_serial_number, true);
		display_prompt("Mux okay", true);
	}
	viPrintf(viMUX, "RESET\n");
}

/*------------------------------------------------------------------------*/

void disconnect_all(int mod_id)
{
  /* USED BY BOTH DIGITAL AND ANALOG FIXTURE */
  viPrintf(viMUX, "RESET\n");
}  /*disconnect_all*/

/*------------------------------------------------------------------------*/
void get_mux_serial_number(int mod_id)
{
  /* USED BY BOTH DIGITAL FIXTURE */
  viPrintf(viMUX, "DEFKEY? 'U'\n");
  viScanf(viMUX, "%t", &buf);
}  /*get_mux_serial_number*/

/*------------------------------------------------------------------------*/
char *get_sales_rack_serial_number(char *result, int mod_id)
{
  /* USED BY DIGITAL FIXTURE */

  /* This procedure takes as input the module number and returns the */
  /* minerva serial number as type s80.                              */
//  test_result_type test_result;
  char rack_serial_number[81];
  char str1[81];
  char str[81] = {0};
  bool error;
  err = 1;

  err = viQueryf(viMUX, "DEFKEY? 'U'\n", "%t", &str);
  if(err == 0)
  {
    strcpy(rack_serial_number, str);
    /* The following line strips the " marks from either side of the S/N */
    if (strlen(rack_serial_number) > 11)
    {
      sprintf(str1, "%.10s", rack_serial_number + 1);
      strcpy(rack_serial_number, str1);
    }
    error = false;
  }
  else
  {
    display_prompt("No result in result log (Expected mux reading)", true);
    strcpy(rack_serial_number, "UNDEFINED\n");
    error = true;
  }
  return strcpy(result, rack_serial_number);
}  /*get_sales_rack_serial_number*/

/*------------------------------------------------------------------------*/
void store_mux_serial_number(int mod_id, char *serial_number)
{
  /* USED BY BOTH DIGITAL FIXTURE */
  char setup_string[81];

  *setup_string = '\0';
  strcat(setup_string, "DEFKEY 'U','");
  strcat(setup_string, serial_number);
  strcat(setup_string, "'\n");
  viPrintf(viMUX, setup_string);
}  /*store_mux_serial_number*/


/*------------------------------------------------------------------------*/
void close_mux(int mod_id)
{
  /* USED BY BOTH DIGITAL AND ANALOG FIXTURE */
  viClear(viMUX);
  disconnect_all(mod_id);
  viClose(viMUX);
}  /*close_mux*/

/*------------------------------------------------------------------------*/
void open_mux(int mod_id)
{
  /* USED BY BOTH DIGITAL AND ANALOG FIXTURE */
  s_type s;
  char str1[92];

  test_result_type result;
  if (get_result_nolog(mod_id, &result))
    display_prompt("Failed to sync up pipe when opening mux", true);

  err = viOpen(rsm, MUX, VI_NULL, VI_NULL, &viMUX );
  if( err != 0 )
  {
 	display_prompt("Can't open MUX.", true);
	exit(0);
  }
  viSetAttribute(viMUX, VI_ATTR_TMO_VALUE, HPIB_TO);
  viSetAttribute(viMUX, VI_ATTR_TERMCHAR_EN, VI_TRUE);
  *s = '\0';
  show_present_test_info(present_test[mod_id - NO_TESTHEAD_NUMBER], mod_id,
                         present_slot[mod_id - NO_TESTHEAD_NUMBER], s);
  sprintf(s, "DISP MSG,\"%s", strcpy(str1, s));
  strcat(s, "\n");
  viPrintf(viMUX, s);
}  /*open_mux*/


/*************************************************************************
* Generator
*************************************************************************/

void verify_gen(int mod_id)
{
  viPrintf(viGEN, "*CLS\n");      
  viPrintf(viGEN, "*RST\n");      /*Reset the 8110A*/
  viPrintf(viGEN, "*IDN?\n");
  err = viScanf(viGEN, "%t", &buf);
  if(err != 0)
	  display_prompt("Pulse generator not responding query", true);
  else
  {
	  printf("Generator ID: %s\n", buf);
	  display_prompt("Generator okay", true);
  }
  viPrintf(viGEN, "*CLS\n");      
  viPrintf(viGEN, "*RST\n");      
}	

/*------------------------------------------------------------------------*/

void setup_gen_defaults(int mod_id)
{
  viPrintf(viGEN, "*RST\n");      /*Reset the 8110A*/
  viPrintf(viGEN, "*SRE 0\n");      
  viPrintf(viGEN, "*ESE 0\n");      
  viPrintf(viGEN, "*CLS\n");      
  viPrintf(viGEN, ":ARM:SOUR EXT1\n");  /*Setup for the triggered mode*/
  viPrintf(viGEN, ":ARM:LEV 0V\n");   /*Trigger Level*/
  viPrintf(viGEN, ":PULS:TRAN1 4.5NS\n");   /*Pulse Transition time 4.5nS*/
              /* to match 8112 */
  viPrintf(viGEN, ":VOLT:LOW 0.0V\n");  /*Low level of output = 0.0v*/
  viPrintf(viGEN, ":VOLT:HIGH 1.5V\n");   /*High level of output = 1.5v*/
  viPrintf(viGEN, ":ARM:SOUR EXT1\n");  /*Pulse width = 500nS*/

//old code for 8112 generator used the 65ns
//  viPrintf(viGEN, ":PULS:DEL1 65NS\n");   /*Trigger to output delay = 65nS*/

//8110 generator requires this...

//mjs changed this next line for experimental purposes
//  viPrintf(viGEN, ":PULS:DEL1 45NS\n");   /*Trigger to output delay = 45nS*/
  viPrintf(viGEN, ":PULS:DEL1 38NS\n");   /*Trigger to output delay = 38nS*/

}  /*setup_gen_defaults*/

/*------------------------------------------------------------------------*/
void open_gen(int mod_id)
{
  err = viOpen(rsm, GEN, VI_NULL, VI_NULL, &viGEN );
  if( err != 0 )
  {
	display_prompt("Can't open GEN.", true);
  }
  viSetAttribute(viGEN, VI_ATTR_TMO_VALUE, HPIB_TO);
  viSetAttribute(viGEN, VI_ATTR_TERMCHAR_EN, VI_TRUE);
  setup_gen_defaults(mod_id);
}  /*open_gen*/

/*------------------------------------------------------------------------*/

void arm_gen(int mod_id)
{
	viPrintf(viGEN, ":ARM:EWID ON\n");
}

/*------------------------------------------------------------------------*/

int read_gen_err(int mod_id)
{
  viPrintf(viGEN, ":ARM:EWID OFF\n");
  err = viQueryf(viGEN, ":SYST:ERR?\n", "%t", &buf);
  return err;
}

/*------------------------------------------------------------------------*/

void disable_gen_out(int mod_id)
{
  viPrintf(viGEN, ":OUTP1 OFF\n");
}  /*disable_gen_out*/

/*------------------------------------------------------------------------*/
void enable_gen_out(int mod_id)
{
  viPrintf(viGEN, ":OUTP1 ON\n");
}  /*enable_gen_out*/

/*------------------------------------------------------------------------*/
void complement_gen_out(int mod_id)
{
  viPrintf(viGEN, ":OUTP1:POL INV\n");
}  /*complement_gen_out*/

/*------------------------------------------------------------------------*/
void no_complement_gen_out(int mod_id)
{
  viPrintf(viGEN, ":OUTP1:POL NORM\n");
}  /*no_complement_gen_out*/

/*------------------------------------------------------------------------*/
void setup_gen_mode(int mod_id, int mode, int slope)
{
  if (mode == NORM)
    viPrintf(viGEN, ":ARM:SOUR IMM\n");
  else  /*mode = triggered*/
  {  /* if */
    viPrintf(viGEN, ":ARM:SOUR EXT1\n");
    viPrintf(viGEN, ":ARM:LEV 0V\n");
   if (slope == POS)
      viPrintf(viGEN, ":ARM:SLOP POS\n");
  }  /* if */
}  /*setup_gen_mode*/

/*------------------------------------------------------------------------*/
void setup_gen_levels(int mod_id, double low_level, double high_level)
{
  char setup_string[81];

  *setup_string = '\0';
  sprintf(setup_string, "%s %1.2f %s\n", ":VOLT:LOW ", low_level, "V");
  viPrintf(viGEN, setup_string);

  sprintf(setup_string, "%s %1.2f %s\n", ":VOLT:HIGH ", high_level, "V");
  viPrintf(viGEN, setup_string);
}  /*setup_gen_levels*/

/*------------------------------------------------------------------------*/
void setup_gen_pwidth(int mod_id, double pulse_width)
{
  char setup_string[81];

  *setup_string = '\0';
  convert_seconds_nanoseconds(&pulse_width);
  sprintf(setup_string, "%s %1.1f %s\n", ":PULS:WIDT1 ", pulse_width, "NS");
  viPrintf(viGEN, setup_string);
}  /*setup_gen_pwidth*/

/*------------------------------------------------------------------------*/
void setup_gen_period(int mod_id, double gen_period)
{
  char setup_string[81];

  *setup_string = '\0';
  convert_seconds_nanoseconds(&gen_period);
  sprintf(setup_string, "%s %1.1f %s\n", ":PULS:PER ", gen_period, "NS");
  viPrintf(viGEN, setup_string);
}  /*setup_gen_period*/

/*------------------------------------------------------------------------*/
void setup_gen_fixed_transition(int mod_id)
{
  viPrintf(viGEN, ":PULS:TRAN1 4.5NS\n");
}  /*setup_gen_fixed_transition*/

/*------------------------------------------------------------------------*/
void setup_gen_transition_times(int mod_id, double tt)
{
  char setup_string[81];
 
  *setup_string = '\0';
  convert_seconds_nanoseconds(&tt);
  sprintf(setup_string, "%s %1.1f %s\n", ":PULS:TRAN1 ", tt, "NS");
  viPrintf(viGEN, setup_string);
}  /*setup_gen_transition_times*/

/*------------------------------------------------------------------------*/
void setup_gen_trg_out_delay(int mod_id, double delay, bool gen_cal_counter)
{
  char setup_string[81];
  *setup_string = '\0';

  convert_seconds_nanoseconds(&delay);
  if(gen_cal_counter)
  {
    delay -= (get_gen_cal_delay(mod_id) * 1e9);
	gen_cal_counter = false;
  }

  sprintf(setup_string, "%s %1.1f %s\n", ":PULS:DEL1 ", delay, "NS");
  viPrintf(viGEN, setup_string);
}  /*setup_gen_trg_out_delay*/


/*------------------------------------------------------------------------*/
void close_gen(int mod_id)
{
  viClear(viGEN);
  viPrintf(viGEN, "*RST\n");
  viClose(viGEN);
}  /*close_gen*/

/*------------------------------------------------------------------------*/
double get_gen_cal_delay(int mod_id)
{
  double gen_cal_delay;
  connect_syn_out_gen_ext(mod_id);
  connect_gen_trg_cntr_start(mod_id);
  connect_gen_out_cntr_stop(mod_id);
  viPrintf(viGEN, ":PULS:DEL1 100NS\n");
  enable_gen_out(mod_id);
  trigger_cntr(mod_id);
  gen_cal_delay = read_cntr_mean(mod_id) - 100e-09;
  return(gen_cal_delay);
}

/*------------------------------------------------------------------------*/
#define GEN_LOW				0.0e+00
#define GEN_HIGH_TERMINATED 5.0e+00
	
double MeasureGenTrigOutDelay(int const &mod_id, int const &slot, double gen_delay)
{
	double measured_offset = 0.0;
  
	connect_syn_out_gen_ext(mod_id);
	setup_gen_levels(mod_id, GEN_LOW, GEN_HIGH_TERMINATED);
	connect_gen_trg_cntr_start(mod_id);
	connect_gen_out_cntr_stop(mod_id);
	setup_gen_trg_out_delay(mod_id, gen_delay, true);
	enable_gen_out(mod_id);
	trigger_cntr(mod_id);
	measured_offset = read_cntr_mean(mod_id);
	disable_gen_out(mod_id);
	connect_gen_trg_clkrb(mod_id);
	connect_cntr_ch1_af2(mod_id);
	vth_mclose_rly(mod_id, MK2902);   /* clkrb path relay */
	vth_mclose_rly(mod_id, MK2903);   /* clkrsg path relay */
	return measured_offset;
}

#undef GEN_LOW			
#undef GEN_HIGH_TERMINATED


/*************************************************************************
* Synthesizer
*************************************************************************/

void verify_syn(int mod_id)
{
	/*viPrintf(viSYNTH, "*RST\n");
	viPrintf(viSYNTH, "*IDN?\n");
	err = viScanf(viSYNTH, "%t", &buf);
	if(err != 0)
		display_prompt("Sythesizer not responding to ID query", true);
	else
	{
		printf("Sythesizer ID: %s\n", buf);
		display_prompt("Synthesizer okay", true);
	}
	viPrintf(viSYNTH, "*RST\n");
	*/
	ViStatus  hw_err;
	char buf1[256] = {0};
	char str1[256] = {0};
	
	
	err = viQueryf(viSYNTH, "*IDN?", "%t", &buf);
	if(check_syn_model())
		hw_err = viQueryf(viSYNTH, "SYST:ERR?", "%t", &buf1);
	else
		hw_err = viQueryf(viSYNTH, "ERR?", "%t", &buf1);
	
	if(err != 0 && hw_err != 0)
	{
		display_prompt("Function Generator not responding to HPIB queries", true);
	    exit(1);
	}

	else
	{
		sprintf(str1, "%.50s", buf);
		display_prompt(str1, true);		
		display_prompt("   Function Generator Okay", true); 
	}
}

/* ------------------------------------------------------------------------ */

void setup_syn(int mod_id, int wave, int frequency, double amplitude, double offset)
{
  char setup_string[81];

  switch (wave)
  {
  case (DC):
    viPrintf(viSYNTH, "FU0\n");
	break;
  case (SINE):
    viPrintf(viSYNTH, "FU1\n");
	break;
  case (SQUARE):
    viPrintf(viSYNTH, "FU2\n");
	break;
  case (TRIANGLE):
    viPrintf(viSYNTH, "FU3\n");
	break;
  case (POS_RAMP):
    viPrintf(viSYNTH, "FU4\n");
	break;
  case (NEG_RAMP):
    viPrintf(viSYNTH, "FU5\n");
	break;
  default: assert(false); 
	  break; 
  }

  vth_mtmr_wait(mod_id, MEASUREMENT_WAIT);
  flush_commands(mod_id);
  *setup_string = '\0';
  if (offset >= 10.0)
    sprintf(setup_string, "%s %1.2f %s\n", "OF ", offset, " VO");
  else
  {
	  if (offset >= 1.0)
        sprintf(setup_string, "%s %1.3f %s\n", "OF ", offset, " VO");
    else
    {
      if (offset >= 0.1)
        sprintf(setup_string, "%s %1.1f %s\n", "OF ", offset * 1000, " MV");
      else
        sprintf(setup_string, "%s %1.2f %s\n", "OF ", offset * 1000, " MV");
    }
  }
  viPrintf(viSYNTH, setup_string);

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
    sprintf(setup_string, "%s %d %s\n", "FR ", frequency, " HZ");
    viPrintf(viSYNTH, setup_string);

    /* PEAK TO PEAK AMPLITUDE */
    *setup_string = '\0';

    if (amplitude >= 10.0)
      sprintf(setup_string, "%s %1.2f %s\n", "AM ", amplitude, " VO");
    else
    {
      if (amplitude >= 1.0)
        sprintf(setup_string, "%s %1.3f %s\n", "AM ", amplitude, " VO");
      else
      {
        if (amplitude >= 0.1)
        
          sprintf(setup_string,
                "%s %1.1f %s %s\n", "AM ", amplitude * 1000, " VO", " MV");
         else
          sprintf(setup_string,
                  "%s %1.2f %s %s\n", "AM ", amplitude * 1000, " VO", " MV");
	  }
	}
   viPrintf(viSYNTH, setup_string);
  }  /*setup the peak to peak amplitude*/

}  /*setup_syn*/

/* ------------------------------------------------------------------------ */

void setup_syn_hv(int mod_id, int status)
{
  if (status == true)
    viPrintf(viSYNTH, "HV 1\n");
  else
    viPrintf(viSYNTH, "HV 0\n");

  vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME); 
  flush_commands(mod_id);

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

  if (get_result_nolog(mod_id, &result))
    display_prompt("Failed to sync up pipe when opening syn", true);

  err = viOpen(rsm, SYNTH, VI_NULL, VI_NULL, &viSYNTH );
  if( err != 0 )
  {
	display_prompt("Can't open SYNTH.", true);
	exit( 1 );
  }
  viSetAttribute(viSYNTH, VI_ATTR_TMO_VALUE, HPIB_TO);
  viSetAttribute(viSYNTH, VI_ATTR_TERMCHAR_EN, VI_TRUE);
  if(check_syn_model())
	  setup_33250A_defaults(mod_id);
  else
  setup_syn_defaults(mod_id);
}  /*open_syn*/

/* ------------------------------------------------------------------------ */
void setup_syn_wave(int mod_id, int wave)
{
  if (wave == SINE)
  {
    /* SETUP THE SINE WAVE FUNCTION */
    viPrintf(viSYNTH, "FU1\n");
  }
  else if (wave == DC)
  {
    /* SETUP THE NO WAVEFORM FUNCTION */
    viPrintf(viSYNTH, "FU0\n");
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
  strcat(setup_string, "FR ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d", frequency);
  next_position = strlen(setup_string) + 1;
  strcat(setup_string, "HZ");
  strcat(setup_string, "\n");
  viPrintf(viSYNTH, setup_string);

}  /*setup_syn_freq*/

/* ------------------------------------------------------------------------ */
void setup_syn_amp(int mod_id, double amplitude)
{
  char setup_string[81];
  int current_position;
  int next_position;

  /* PEAK TO PEAK AMPLITUDE */
  *setup_string = '\0';
  strcat(setup_string, "AM ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.1f", amplitude);
  next_position = strlen(setup_string) + 1;
  strcat(setup_string, "VO");
  strcat(setup_string, "\n");
  viPrintf(viSYNTH, setup_string);

}  /*setup_syn_amp*/

/* ------------------------------------------------------------------------ */
void setup_syn_off(int mod_id, double offset)
{
  char setup_string[81];
  int current_position;
  int next_position;

  /* SETUP THE OFFSET VOLTAGE */
  *setup_string = '\0';
  strcat(setup_string, "OF ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.3f", offset);
  next_position = strlen(setup_string) + 1;
  strcat(setup_string, "VO");
  strcat(setup_string, "\n");
  viPrintf(viSYNTH, setup_string);

}  /*setup_syn_off*/

/* ------------------------------------------------------------------------ */
/*
void close_syn(int mod_id)
{
  setup_syn(mod_id, SINE, 0, 0.0, 0.0);
  viClose(viSYNTH);
}
*/  
/*close_syn*/


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

/*Close synthesizer*/
/* ------------------------------------------------------------------------ */
void close_syn(int mod_id)
{
  
	//char buf1[256] = {0};
	//err = viQueryf(viSYNTH, "*OPC?\n", "%t", &buf1); //3325B does not regconize OPC
	//if (err != 0)
	//{
	//	printf("GPIB command pending on 33250A!!!");
		
	//}
	//else
		viPrintf(viSYNTH, "*RST\n");
		viClose(viSYNTH);

}  
/* ------------------------------------------------------------------------ */
/*End of close synthesizer*/

/*------------------------------------------------------------------------*/
/*End of new codes for FG 33250A*/



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
		if(!strcmp(str1, "HEWLETT"))
			return false;
		else if(!strcmp(str1, "Agilent"))
			return true;
		else
			{
				display_prompt("Function Generator model mismatch!!!",true);
				exit(1);
			}
	}
}
	
/*End Of Check Synthesizer Model*/
/* ------------------------------------------------------------------------ */	

/*************************************************************************
* LCR
*************************************************************************/

void verify_lcr(int mod_id)
{
  	viPrintf(viLCR, "*RST\n");
	viQueryf(viLCR, "*IDN?", "%t", &buf);
	err = viPrintf(viLCR, ":SYST:ERR?\n");
	if (err != 0)
		display_prompt("   LCR ERROR !!!!!", true);
	else
	{
		printf("LCR meter ID: %s\n", buf);
		display_prompt("LCR okay", true);
	}
	viPrintf(viLCR, "*RST\n");
	viPrintf(viLCR, ":SYST:PRES\n");
}

/*------------------------------------------------------------------------*/
void open_lcr(int mod_id)
{
  err = viOpen(rsm, LCR, VI_NULL, VI_NULL, &viLCR );
  if( err != 0 )
  {
	display_prompt("Can't open LCR.", true);
	exit( 1 );
  }
  viSetAttribute(viLCR, VI_ATTR_TMO_VALUE, HPIB_TO);
  viSetAttribute(viLCR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
  //viPrintf(viLCR,"*RST\n");
}  /*open_lcr*/

/*------------------------------------------------------------------------*/
void setup_lcr(int mod_id, int lcr_function, int lcr_display,
               int lcr_ckt_mode, double lcr_freq, int lcr_mult)
{
  char setup_string[81];

  *setup_string = '\0';

/*if (lcr_function == INDUCTANCE)
    strcat(setup_string, "A1\n");
  else  // must be capacitance
    strcat(setup_string, "A2\n");

  if (lcr_display == Q)
    strcat(setup_string, "B2\n");

  if (lcr_ckt_mode == SERIES)
    strcat(setup_string, "C2\n");
  else  // must be parallel
    strcat(setup_string, "C3\n");

  if (lcr_freq == 100000)
    strcat(setup_string, "F21\n");*/

  if (lcr_function == INDUCTANCE)
  if (lcr_ckt_mode == SERIES)
  {
    viPrintf(viLCR,":SENS:FUNC 'FIMP';\n");
    viPrintf(viLCR,":CALC1:FORM LS;\n");
  }
  else
  {
    viPrintf(viLCR,":SENS:FUNC 'FADM';\n");
    viPrintf(viLCR,":CALC1:FORM LP;\n");
        }
  else  /* must be capacitance */
  if (lcr_ckt_mode == SERIES)
  {
          viPrintf(viLCR,":SENS:FUNC 'FIMP';\n");
    viPrintf(viLCR,":CALC1:FORM CS;\n");
  }
  else
  {
          viPrintf(viLCR,":SENS:FUNC 'FADM';\n");
    viPrintf(viLCR,":CALC1:FORM CP;\n");
  }

  if (lcr_display == Q)
    viPrintf(viLCR,":CALC2:FORM Q;\n");

  if (lcr_freq == 100000)
    viPrintf(viLCR,":SOUR:FREQ:CW 100000 HZ;\n");

/*  no high resolution function on new lcr.       */
//strcat(setup_string, "H1\n");    High resolution

  if (lcr_mult == 5)
/*  strcat(setup_string, "M4\n");*/
/* Set test signal level to the maximum.*/
    viPrintf(viLCR,":SOUR:VOLT 1;\n");

//strcat(setup_string, "R31\n");   /* Auto range */
  viPrintf(viLCR,":SENS:FIMP:RANG:AUTO ON;\n");   /* Auto range */

//strcat(setup_string, "T3\n");   /* Manual trigger / HOLD */
  viPrintf(viLCR,":TRIG:SOUR MAN;\n");   /* Manual trigger / HOLD */

}  /*setup_lcr*/

/*------------------------------------------------------------------------*/
void trigger_lcr(int mod_id)
{
	viPrintf(viLCR, ":TRIG\n");
}

/*------------------------------------------------------------------------*/
double measure_lcr(int mod_id)
{
  int next;
  int start;
  double res;
  char s[256 + 1];
  char *str1;

  viPrintf(viLCR, ":TRIG\n");   
  err = viQueryf(viLCR, ":FETC?\n", "%t", &buf);
  strcpy(s, buf);
  start = strpos2(",", s, 1) + 1;
  res = strtod(s + start - 1, &str1);
  next = str1 - s + 1;
  return res;
}  /*measure_lcr*/

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
/*viPrintf(viLCR, "A2B1C2H0I0S0M1F11R11\n");*/
  viPrintf(viLCR, "SENS:FUNC 'FIMP'\n");
  viPrintf(viLCR, "CALC1:FORM CS\n");
  viPrintf(viLCR, "CALC2:FORM Q\n");
  viPrintf(viLCR, "SOUR:VOLT .005\n");
  viPrintf(viLCR, "SOUR:FREQ:CW 100000 HZ\n");
  viPrintf(viLCR, "SENS:FIMP:RANG 1E-3\n");
  viPrintf(viLCR, "TRIG:SOUR MAN\n");
/*viPrintf(viLCR, "T3\n");*/
  viClose(viLCR);
}  /*close_lcr*/


/*************************************************************************
* Counter
*************************************************************************/

void verify_counter(int mod_id)
{
  	viPrintf(viCNTR, ":INP1:COUP DC\n");    // input coupling = DC
	viPrintf(viCNTR, ":INP2:COUP DC\n");    // input coupling = DC
	viPrintf(viCNTR, ":INP1:IMP 50\n");     // input impedance 50 ohms
	viPrintf(viCNTR, ":INP2:IMP 50\n");     // input impedance 50 ohms
	setup_cntr_function(mod_id, TIME_INTERVAL);  // to match old 5370 cntr
	setup_cntr_trg_levels(mod_id, 1.3, 1.3);
	setup_cntr_gate_time(mod_id, 0.0);
	viPrintf(viCNTR, "*SAV 1\n");           // save preset 1
	viPrintf(viCNTR, "*IDN?\n");
	err = viScanf(viCNTR, "%t", &buf);
	if(err != 0)
		display_prompt("   COUNTER ERROR !!!!!", true);
	else
	{
		printf("Counter ID: %s\n", buf);
		display_prompt("Counter okay", true);
	}
	viPrintf(viCNTR, "*RST\n");
}

/* ---------------------------------------------------------------------- */
void open_cntr(int mod_id)
{
  err = viOpen(rsm, CNTR, VI_NULL, VI_NULL, &viCNTR);
  if( err != 0 )
  {
     display_prompt("Counter is not responding! If this is not Final", true);
	 display_prompt("(System) test then all data from this test loop", true);
	 display_prompt("is not valid.", true);
  }
  viSetAttribute(viCNTR, VI_ATTR_TMO_VALUE, HPIB_TO);
  viSetAttribute(viCNTR, VI_ATTR_TERMCHAR_EN, VI_TRUE);
  setup_cntr_defaults(mod_id);
}  /*open_cntr*/

/* ---------------------------------------------------------------------- */

void setup_cntr_out2(int mod_id)
{
	viPrintf(viCNTR, ":EVEN2:FEED 'INP1'\n");
}

/* ---------------------------------------------------------------------- */

void setup_cntr_defaults(int mod_id)
{  /*setup_cntr_power_up_state*/

  viPrintf(viCNTR, "*RST\n");
  viPrintf(viCNTR, "*CLS\n");
  viPrintf(viCNTR, "*RCL 1\n");

}  /*setup_cntr_power_up_state*/

/* ---------------------------------------------------------------------- */

void set_cntr_1meg(int mod_id)
{
	viPrintf(viCNTR, ":INP1:IMP 1E6\n");
}

/* ---------------------------------------------------------------------- */

void set_cntr_stop_1meg(int mod_id)
{
	viPrintf(viCNTR, ":INP2:IMP 1E6\n");
}

/* ---------------------------------------------------------------------- */
void setup_cntr_trg_levels(int mod_id, double start_level, double stop_level)
{
  char setup_string[81];
  int current_position;
//  int next_position;

  *setup_string = '\0';
  strcat(setup_string, ":SENSE:EVENT1:LEVEL:ABS ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.2f\n", start_level);
  viPrintf(viCNTR, setup_string);

  *setup_string = '\0';
  strcat(setup_string, ":SENSE:EVENT2:LEVEL:ABS ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.2f\n", stop_level);
  viPrintf(viCNTR, setup_string);

}  /*setup_cntr_trg_levels*/

/* ---------------------------------------------------------------------- */
  	
void setup_cntr_trigger_min_max(int mod_id, int level)
{
  if(level == MIN)
    viPrintf(viCNTR, ":SENSE:FUNC 'VOLT:MIN'\n");
  else
    viPrintf(viCNTR, ":SENSE:FUNC 'VOLT:MAX'\n");

  viPrintf(viCNTR, ":SENSE:FREQ:ARM:START:SOUR IMM\n");
  viPrintf(viCNTR, ":SENSE:FREQ:ARM:STOP:SOUR DIG\n");
  viPrintf(viCNTR, ":SENSE:FREQ:ARM:STOP:DIG 13\n");
}
	
/* ---------------------------------------------------------------------- */
  	
void setup_cntr_trigger_slopes(int mod_id, int start_slope, int stop_slope)
{
  if (start_slope == POS)   
    viPrintf(viCNTR, ":SENSE:EVENT1:SLOPE POS\n");	/*Start on positive slope*/
  else  
    viPrintf(viCNTR, ":SENSE:EVENT1:SLOPE NEG\n");	/*Start on negative slope*/
  
  if (stop_slope == POS)   
    viPrintf(viCNTR, ":SENSE:EVENT2:SLOPE POS\n");	/*Stop on positive slope*/
  else  
    viPrintf(viCNTR, ":SENSE:EVENT2:SLOPE NEG\n");	/*Stop on negative slope*/
  
}  /*setup_cntr_trigger_slopes*/

/* ---------------------------------------------------------------------- */

void setup_cntr_inp2_lowpass(int mod_id)
{
  viPrintf(viCNTR, ":INP2:FILT:LPAS:STAT ON\n");
}

/* ---------------------------------------------------------------------- */
void setup_cntr_function(int mod_id, int cntr_function)
{
  switch(cntr_function)
  {
  case (TIME_INTERVAL):
	  viPrintf(viCNTR, ":SENSE:FUNC 'TINT'\n");
	  break;
  case (FREQUENCY):
	  viPrintf(viCNTR, ":SENSE:FUNC 'FREQ 2'\n");
	  break;
  case (PERIOD):
	  viPrintf(viCNTR, ":SENSE:FUNC 'PER 2'\n");
	  break;
  case (PWID):
	  viPrintf(viCNTR, ":SENSE:FUNC 'Pwidth'\n");
	  break;
  case (NWID):
	  viPrintf(viCNTR, ":SENSE:FUNC 'Nwidth'\n");
	  break;
  case (DCYCLE):
	  viPrintf(viCNTR, ":SENSE:FUNC 'Dcycle'\n");
	  break;
  default:
      assert(false);
      break;
  }

  setup_cntr_sample_size(mod_id, 10);   /* enable math/averaging */
}  /*setup_cntr_function*/

/* ---------------------------------------------------------------------- */
void setup_cntr_volt_meas(int mod_id, int cntr_function, int count)
{
  char setup_string[81];
  int current_position;

  switch(cntr_function)
  {
  case (V_MAX):
	  viPrintf(viCNTR, ":SENSE:FUNC 'VOLT:MAX 2'\n");
      viPrintf(viCNTR, ":CALC3:AVER:TYPE MAX\n");
	  break;
  case (V_MIN):
	  viPrintf(viCNTR, ":SENSE:FUNC 'VOLT:MIN 2'\n");
	  viPrintf(viCNTR, ":CALC3:AVER:TYPE MIN\n");
	  break;
  default:
      assert(false);
      break;
  }

  *setup_string = '\0';
  strcat(setup_string, ":CALC3:AVERAGE:COUNT ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d\n", count);
  viPrintf(viCNTR, setup_string);
  viPrintf(viCNTR, ":TRIG:COUNT:AUTO ON\n");
  viPrintf(viCNTR, "*ESE 1\n");
}

/* ---------------------------------------------------------------------- */
void setup_cntr_gate_time(int mod_id, double gate_time)
{
  viPrintf(viCNTR, ":FREQ:ARM:START:SOUR IMM\n");
  if (gate_time == 0.0)
    viPrintf(viCNTR, ":FREQ:ARM:STOP:SOUR IMM\n");
  else
  {
    if ( number_equal(gate_time, 0.01) )
    {
      viPrintf(viCNTR, ":FREQ:ARM:STOP:SOUR TIM\n");
      viPrintf(viCNTR, ":FREQ:ARM:STOP:TIM 0.01\n");
    }
    else
    {
      if ( number_equal(gate_time, 0.1) )
      {
        viPrintf(viCNTR, ":FREQ:ARM:STOP:SOUR TIM\n");
        viPrintf(viCNTR, ":FREQ:ARM:STOP:TIM 0.1\n");
      }
      else
      {
        viPrintf(viCNTR, ":FREQ:ARM:STOP:SOUR TIM\n");
        viPrintf(viCNTR, ":FREQ:ARM:STOP:TIM 1.0\n");
      }
    }
  }
}  /*setup_cntr_gate_time*/

/* ---------------------------------------------------------------------- */
void setup_cntr_sample_size(int mod_id, int samples)
{
  char setup_string[81];
  int current_position;

  viPrintf(viCNTR, ":CALC3:AVERAGE:TYPE MEAN\n");
  viPrintf(viCNTR, ":CALC3:AVERAGE ON\n");   /*init takes block of vth_readings*/
  if (samples == 1)
    viPrintf(viCNTR, ":CALC3:AVERAGE OFF\n");
  else
  {
    *setup_string = '\0';
    strcat(setup_string, ":CALC3:AVERAGE:COUNT ");
    current_position = strlen(setup_string) + 1;
    sprintf(setup_string + current_position - 1, "%d\n", samples);
    viPrintf(viCNTR, setup_string);
  }
  viPrintf(viCNTR, ":TRIG:COUNT:AUTO ON\n");
      /*init takes block of readings*/
  viPrintf(viCNTR, "*ESE 1\n");
}  /*setup_cntr_sample_size*/

/* ---------------------------------------------------------------------- */
void setup_cntr_md4(int mod_id)
{
  viPrintf(viCNTR, "DISP:ENABLE OFF\n");
}

/* ---------------------------------------------------------------------- */
void setup_cntr_md1(int mod_id)
{
  viPrintf(viCNTR, "DISP:ENABLE ON\n");
}

/* ---------------------------------------------------------------------- */
int TrigCntr(int mod_id)
{
	int count = 1; 
	vth_mtmr_wait(mod_id, 0.050);
	viPrintf(viCNTR, ":INIT:IMM\n");
	err = viQueryf(viCNTR, "*OPC?\n", "%t", &buf);
    if(err != 0)
	{
	  while((count <= 5) && (err != 0))
	  {
		  viClear(viCNTR);
		  viPrintf(viCNTR, ":INIT:IMM\n");
		  vth_mtmr_wait(mod_id, 0.050);
		  err = viQueryf(viCNTR, "*OPC?\n", "%t", &buf);
		  count++;
	  };
	}
	return err;
}
	
/* ---------------------------------------------------------------------- */

void trigger_cntr(int mod_id)
{
  vth_mtmr_wait(mod_id, 0.050);
  viPrintf(viCNTR, ":INIT:IMM\n");
  err = viQueryf(viCNTR, "*OPC?\n", "%t", &buf);
  if(err != 0)
    viClear(viCNTR);
}  

/* ---------------------------------------------------------------------- */

void setup_cntr_duty(int mod_id)
{
	viPrintf(viCNTR, ":SENSE:FUNC 'DCYCLE'\n");
}

/* ---------------------------------------------------------------------- */

void setup_cntr_measurement(int mod_id, char measurement)
{
  viPrintf(viCNTR, ":CONF:FREQ\n");
  viPrintf(viCNTR, ":SENSE:FREQ:ARM:START:SOUR IMM\n");
  viPrintf(viCNTR, ":SENSE:FREQ:ARM:STOP:SOUR DIG\n");
  viPrintf(viCNTR, ":SENSE:FREQ:ARM:STOP:DIG 9\n");
  if(measurement == 'P')
	viPrintf(viCNTR, ":SENSE:FUNC 'PER 1'\n");
  else
    viPrintf(viCNTR, ":INIT:IMM\n");
}

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
  double res;
  viQueryf(viCNTR, ":FETCH?\n", "%Lf", &res);
  return res;
}  

/* ---------------------------------------------------------------------- */

double read_cntr_mean(int mod_id)
{
  vth_mtmr_wait(mod_id, 2 * TIME_OUT);
  double res;
  viQueryf(viCNTR, ":CALC3:AVERAGE:TYPE MEAN;:CALC3:DATA?\n", "%Lf", &res);
  return res;
}  

/* ---------------------------------------------------------------------- */

double read_cntr_sigma(int mod_id)
{
  double res;
  viQueryf(viCNTR, ":CALC3:AVERAGE:TYPE SDEV;:CALC3:DATA?\n", "%Lf", &res);
  return res;
}  

/* ---------------------------------------------------------------------- */
double read_cntr_min(int mod_id)
{
  double res;
  viQueryf(viCNTR, ":CALC3:AVERAGE:TYPE MIN;:CALC3:DATA?\n", "%Lf", &res);
  return res;
}  

/* ---------------------------------------------------------------------- */
double read_cntr_max(int mod_id)
{
  double res;
  viQueryf(viCNTR, ":READ:VOLT:MAX 2\n", "%Lf", &res);
  return res;
}  

/* ---------------------------------------------------------------------- */

void setup_cntr_max_volt(int mod_id)
{
	viPrintf(viCNTR, ":CONF:MAX\n");
	viPrintf(viCNTR, ":SENSE:FUNC 'VOLT:MAX'\n");
	viPrintf(viCNTR, ":INIT:IMM\n");
	vth_mtmr_wait(mod_id, 1.0e-03);
}

/* ---------------------------------------------------------------------- */

#define TIMEOUT	1.0e-03

void setup_cntr_min_volt(int mod_id)
{
	viPrintf(viCNTR, ":CONF:MIN\n");
	viPrintf(viCNTR, ":SENSE:FUNC 'VOLT:MIN'\n");
	viPrintf(viCNTR, ":INIT:IMM\n");
	vth_mtmr_wait(mod_id, TIMEOUT);
}

#undef TIMEOUT
		
/* ---------------------------------------------------------------------- */

#define MARK_NUMBER     0
#define MAX_TIME        30.0e+00

void wait_for_counter_measurement(int mod_id)
{
  double time;
  bool ready;
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
void setup_cntr_input(int mod_id, int coupling, int impedance)
{
  char setup_string[81];
  int current_position;
  *setup_string = '\0';

  viPrintf(viCNTR, ":INP:ATT 10\n");
  strcat(setup_string, ":INP:COUP ");
  current_position = strlen(setup_string) + 1;
  if(coupling == DC)
	//strcat(setup_string + current_position - 1, "DC\n");
    sprintf(setup_string + current_position - 1, "%s\n", "DC"); //changed to clear lint warning
  else
	//strcat(setup_string + current_position - 1, "AC\n");
    sprintf(setup_string + current_position - 1, "%s\n", "AC");
  viPrintf(viCNTR, setup_string);

  *setup_string = '\0';		//clearing the string
  strcat(setup_string, ":INP:IMP ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d\n", impedance);
  viPrintf(viCNTR, setup_string);
}

/* ---------------------------------------------------------------------- */
void setup_cntr_inputz(int mod_id, int impedance)
{
	  char setup_string[81];
  int current_position;
  *setup_string = '\0';

  strcat(setup_string, ":INP:IMP ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d\n", impedance);
  viPrintf(viCNTR, setup_string);
}

/* ---------------------------------------------------------------------- */
void setup_cntr_edge(int mod_id, int edge)
{
  if(edge == 0)
	viPrintf(viCNTR, ":SENSE:FUNC 'RTIME'\n");
  else
	viPrintf(viCNTR, ":SENSE:FUNC 'FTIME'\n");
}


/* ---------------------------------------------------------------------- */
void close_cntr(int mod_id)
{
  viPrintf(viCNTR, "*OPC\n");
  viClear(viCNTR);
  viPrintf(viCNTR, "*CLS\n");
  viPrintf(viCNTR, "*RST\n");
  viClose(viCNTR);
}  /*close_cntr*/


/*************************************************************************
* Power Supply
*************************************************************************/

void verify_ps(int mod_id)
{
  ViStatus  hw_err;
  char buf1[256] = {0};
  viPrintf(viPS, "CLR\n");//chnaged from RST to CLR
  err = viQueryf(viPS, "ID?\n", "%t", &buf);
  hw_err = viQueryf(viPS, "ERR?\n", "%t", &buf1);
  if(err != 0 && hw_err != 0)
  	display_prompt("Power supply not responding to HPIB queries", true);
  else
  {
	printf("Power supply: %s\n", buf);
	display_prompt("Power supply okay", true);
  }
  viPrintf(viPS, "CLR\n");//CHNAGED FROM RST TO CLR
}

/* ------------------------------------------------------------------------ */

void open_ps(int mod_id)
{
  err = viOpen(rsm, PS, VI_NULL, VI_NULL, &viPS );
  if( err != 0 )
  {
	display_prompt("Can't open PS.", true);
	exit( 1 );
  }
  viSetAttribute(viPS, VI_ATTR_TMO_VALUE, HPIB_TO);
  viSetAttribute(viPS, VI_ATTR_TERMCHAR_EN, VI_TRUE);
  viPrintf(viPS, "CLR\n");
  //viPrintf(viPS, "OUT OFF\n");//sri acorn
    viPrintf(viPS, "OUT 0\n");
}  /*open_ps*/

/* ------------------------------------------------------------------------ */

#define MAX_VOLTS_WITH_PS_LOADED  10.0e+00

void setup_ps(int mod_id, double volts, double amps)
{  /*setup_ps_voltage*/
  char setup_string[81];
  int current_position;
  int next_position;

  if (fabs(volts) > MAX_VOLTS_WITH_PS_LOADED)
  {
    viPrintf(viPS, "OUT 0\n");//ACORN changed from OFF to '0'
    
	aconnect_ps_20_ohm_load(mod_id, false);
  }

  viPrintf(viPS, "DLY 0\n");
 

  *setup_string = '\0';
  strcat(setup_string, "VSET ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.3f", fabs(volts));
  next_position = strlen(setup_string) + 1;
  //strcat(setup_string, "V");acorn sri
  strcat(setup_string, "\n");
  viPrintf(viPS, setup_string);

  *setup_string = '\0';
  strcat(setup_string, "ISET ");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.2f", amps);
  next_position = strlen(setup_string) + 1;
  //strcat(setup_string, "A");//acorn sri
  strcat(setup_string, "\n");
  viPrintf(viPS, setup_string);
  viPrintf(viPS, "OUT 1\n");//acorn sri chnaged from ON to 1
 

}  /*setup_ps_voltage*/
#undef MAX_VOLTS_WITH_PS_LOADED

/* ------------------------------------------------------------------------ */
double measure_ps_sense_voltage(int mod_id)
{
  double res;
  int next;
  int start;
  char s[MAX_S_LENGTH + 1];
  char *str1;

  viQueryf(viPS, "VOUT?\n", "%t", &buf);
  //strcpy(s, &buf[6]);//sri 6634B
  strcpy(s,buf);
  start = strpos2(",", s, 1) + 1;
  res = strtod(s + start - 1, &str1);
  next = str1 - s + 1;
  return res;
}  /*measure_ps_sense_voltage*/

/* ------------------------------------------------------------------------ */

#define MARK_NUM        1
#define WAIT_INCREMENT  50e-3

void verify_ps_output(int mod_id, double diff, double ramp_time, double expected_volts)
{
  double ps_actual_voltage = 0.0;
  double last_voltage;
  double elapsed_time;
  double current_diff;
  bool within_10_percent = false;

  test_result_type result;
  vth_mtmr_set_mark(mod_id, MARK_NUM);
  do
  {
    vth_mtmr_wait(mod_id, 1);
	last_voltage = ps_actual_voltage;
    ps_actual_voltage = measure_ps_sense_voltage(mod_id);
	vth_mtmr_wait(mod_id, 1);
    if (fabs((ps_actual_voltage - expected_volts) / expected_volts) <= 0.1)
      within_10_percent = true;
	vth_mtmr_wait(mod_id, 1);
    vth_mtmr_wait(mod_id, WAIT_INCREMENT);
    vth_mtmr_rd_time_fr_mark(mod_id, MARK_NUM);
	vth_mtmr_wait(mod_id, 1);
    if (get_result_nolog(mod_id, &result))
      elapsed_time = result.UU.Real_result;
    else
    display_prompt("No result in result log (Expected mark time)", true);
    current_diff = fabs(ps_actual_voltage - last_voltage);
  } while (!(current_diff <= diff && within_10_percent || elapsed_time >= ramp_time));

  if (elapsed_time >= ramp_time)
  {  /* ps error message */
    display_prompt("Power supply did not ramp within allotted time.", true);
    display_prompt("Test results are not valid.  Check Sales Rack PS.", true);
  }  /* ps error message */

}  /*verify_ps_output*/
#undef MARK_NUM
#undef WAIT_INCREMENT

/* ------------------------------------------------------------------------ */

#define MARK_NUM        1
#define WAIT_INCREMENT  50e-3
void wait_for_ps_ramp(int mod_id, double diff, double ramp_time, double *dmm_actual_voltage)
{
  double expected_volts;
  double last_voltage;
  double elapsed_time;
  double current_diff;
  bool within_10_percent = false;

  test_result_type result;

  expected_volts = *dmm_actual_voltage;
  vth_mtmr_set_mark(mod_id, MARK_NUM);
  *dmm_actual_voltage = 0.0;
  do
  {
    last_voltage = *dmm_actual_voltage;
    *dmm_actual_voltage = measure_dmm(mod_id);
    result.UU.Real_result = *dmm_actual_voltage;
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
    display_prompt("Power supply did not ramp within allotted time.", true);
    display_prompt("Test results are not valid.  Check Sales Rack PS.", true);
  }  /* ps error message */

}  /* wait_for_ps_ramp */
#undef MARK_NUM
#undef WAIT_INCREMENT

/* ------------------------------------------------------------------------ */
void disable_ps_out(int mod_id)
{
  	viPrintf(viPS, "OUT 0\n");//acorn chnaged from OFF to 0
    
}  /*disable_ps_out*/

/* ------------------------------------------------------------------------ */
void enable_ps_out(int mod_id)
{
    viPrintf(viPS, "OUT 1\n");//acorn , chnaged from ON to 1
    
}  /*enable_ps_out*/

/* ------------------------------------------------------------------------ */
void close_ps(int mod_id)
{
    viPrintf(viPS, "OUT 0\n");//acorn chnaged from off to 0
  
  viClose(viPS);
}  /*close_ps*/

//###################################################


/* Sri- added on 28August 2007, for the Load PS- 6038A.*/

/* ---------------------------------------------------------------------- */
/* wpd - include begins here */
/* Use tag 0 for the 6038A Load Power Supply */
/* Address is 5  */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
#define	CHECK(func)	\
do {\
    ViStatus _s = (func);\
    if (_s < 0)\
    {\
    fprintf(stderr, "Error: %s returned %d\n", #func, _s);\
    exit(0);\
    }\
} while (0)

#define MAX_VOLTS_WITH_PS_LOADED  31.0e+00


float setup_ps_load(int mod_id, double volts, double amps)
{
	float res;
  int next;
  int start;
  char s[MAX_S_LENGTH + 1];
  char *str1;
    //float current_large;
    viPrintf(viPS_LOAD, "OUT ON\n");//acorn sri chnaged from ON to 1
	viPrintf(viPS_LOAD, "VSET 30V\n");
	viPrintf(viPS_LOAD, "ISET 3A\n");
	
  viQueryf(viPS_LOAD, "IOUT?\n", "%t", &buf);
  strcpy(s, &buf[6]);//sri 6634B
  //strcpy(s,buf);
  start = strpos2(",", s, 1) + 1;
  res = strtod(s + start - 1, &str1);
  next = str1 - s + 1;
  return res;

}

void open_ps_load()
{
err = viOpen(rsm, PS_LOAD, VI_NULL, VI_NULL, &viPS_LOAD );
  if( err != 0 )
  {
	display_prompt("Can't open PS.", true);
	exit( 1 );
  }
  viSetAttribute(viPS_LOAD, VI_ATTR_TMO_VALUE, HPIB_TO);
  viSetAttribute(viPS_LOAD, VI_ATTR_TERMCHAR_EN, VI_TRUE);
  viPrintf(viPS, "CLR\n");
  viPrintf(viPS, "OUT ON\n");
}  /*open_ps*/


 /*open_ps*/

void close_ps_load()
{
    viPrintf(viPS_LOAD, "VSET 0V\n");
	viPrintf(viPS_LOAD, "ISET 0A\n");
	viPrintf(viPS_LOAD, "OUT OFF\n");//acorn chnaged from off to 0
  
  viClose(viPS_LOAD);
}  /*close_ps*/

double measure_current()
{
    double current;
    #define DEFAULT_ADDRESS_3458A "GPIB0::5::INSTR"
    
    ViSession   viDefaultRM, Inst;
    ViRsrc      TxtAddress = DEFAULT_ADDRESS_3458A;

    CHECK(viOpenDefaultRM(&viDefaultRM));
    CHECK(viOpen(viDefaultRM, TxtAddress, VI_NULL, 20000, &Inst));
    CHECK(viSetAttribute(Inst, VI_ATTR_TMO_VALUE, 20000)); 

	viPrintf(Inst,   "IOUT?\n");
	
	viScanf(Inst,"%Le\n",&current);
	
	CHECK(viClose(viDefaultRM));
    return current;
}


#undef MAX_VOLTS_WITH_PS_LOADED

/* ------------------------------------------------------------------------ */
double measure_ps_sense_voltage_load(int mod_id)
{
  vth_mhpib_wr(mod_id, 0, "VOUT?");
  vth_mhpib_rd(mod_id, 0);
  return (convert_logged_string_to_numeric(mod_id));
}  /*measure_ps_sense_voltage*/






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


//* ********************************ADDED BY SRI THARAN FOR LOAD TEST DH - UTILITY************* */
void connect_load_test_relay(int mod_id, int status)
{
connect(mod_id, 209, status);
connect(mod_id, 210, status);
}
/* ******************************************************************** */


///////////////////////////END HERE! SRI ADDING NEW LOAD PS//////////////////////


/*************************************************************************
* DUT power supply (not used)
*************************************************************************/

/*void open_dut_ps(int mod_id)
{
  switch (mod_id)
  {   //open the power supply associated with this module

  case 0:
    viOpen(rsm,  9, "/dev/ps0\n");
    break;

  case 1:
    viOpen(rsm,  9, "/dev/ps1\n");
    break;

  case 2:
    viOpen(rsm,  9, "/dev/ps2\n");
    break;

  case 3:
    viOpen(rsm,  9, "/dev/ps3\n");
    break;

  default:
    assert(false);
    break;

  }

  viPrintf(, 9, "CLR\n");

  // Disable all four outputs of the quad supply 
  viPrintf(, 9, "OUT 1,0\n");   // Disable quad output #1
  viPrintf(, 9, "OUT 2,0\n");   // Disable quad output #2
  viPrintf(, 9, "OUT 3,0\n");   // Disable quad output #3
  viPrintf(, 9, "OUT 4,0\n");   // Disable quad output #4

}  /*open_dut_ps*/

/* ------------------------------------------------------------------------ */
/*void setup_dut_ps(int mod_id, int ps_channel, double volts, double ov, double amps)
{
  char setup_string[81];
  int current_position;
  int next_position;

  /* Reset the over voltage protection for the power supply //
  *setup_string = '\0';
  strcat(setup_string, "OVRST \n");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d", ps_channel);
  next_position = strlen(setup_string) + 1;
  viPrintf(, 9, setup_string);

  /* Setup the over voltage protection for the power supply //
  *setup_string = '\0';
  strcat(setup_string, "OVSET \n");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d", ps_channel);
  next_position = strlen(setup_string) + 1;
  strcat(setup_string, ",\n");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.3f", ov);
  next_position = strlen(setup_string) + 1;
  viPrintf(, 9, setup_string);

  /* Setup the voltage for the power supply //
  *setup_string = '\0';
  strcat(setup_string, "VSET \n");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d", ps_channel);
  next_position = strlen(setup_string) + 1;
  strcat(setup_string, ",\n");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.3f", volts);
  next_position = strlen(setup_string) + 1;
  viPrintf(, 9, setup_string);

  /* Setup the current limit for the power supply //
  *setup_string = '\0';
  strcat(setup_string, "ISET \n");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d", ps_channel);
  next_position = strlen(setup_string) + 1;
  strcat(setup_string, ",\n");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%1.3f", amps);
  next_position = strlen(setup_string) + 1;
  viPrintf(, 9, setup_string);

  /* Enable the dut supply out to the aim mint pins //
  vth_aclose_dut_pwr_rly(mod_id, ps_channel);
  vth_aclose_dut_sense_rlys(mod_id, ps_channel);

  /* Turn the output of the power supply on //
  *setup_string = '\0';
  strcat(setup_string, "OUT \n");
  current_position = strlen(setup_string) + 1;
  sprintf(setup_string + current_position - 1, "%d", ps_channel);
  next_position = strlen(setup_string) + 1;
  strcat(setup_string, ",1\n");
  viPrintf(, 9, setup_string);

}  /*setup_dut_ps*/

/* ------------------------------------------------------------------------ */
//void close_dut_ps(int mod_id)
//{
  /* Disable all four outputs of the quad supply */
//  viPrintf(, 9, "OUT 1,0\n");   /* Disable quad output #1*/
//  viPrintf(, 9, "OUT 2,0\n");   /* Disable quad output #2*/
//  viPrintf(, 9, "OUT 3,0\n");   /* Disable quad output #3*/
//  viPrintf(, 9, "OUT 4,0\n");   /* Disable quad output #4*/
//
//  viClose(mod_id, 9);
//
//}  /*close_dut_ps*/

/* ------------------------------------------------------------------------ */

void setup_inst_defaults(int mod_id)
{
  /* Opens and closes the instruments that are may cause noise problems */
  /* in their power up states.                                          */
  open_gen(mod_id);
  open_lcr(mod_id);
  open_ps(mod_id);
  open_syn(mod_id);

  vth_mtmr_wait(mod_id, 0.01);   /* Just in case */

  close_gen(mod_id);
  close_lcr(mod_id);
  close_ps(mod_id);
  close_syn(mod_id);

}  /*setup_inst_defaults*/

/* ------------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------------ 
CALC_MOD_LOADING                  mjs 6/97

PURPOSE:  This function reads the config of a module and determines how
          many cards (not counting the Mac and Aim) exist and how many of
          those are double density.  It then takes the correction factor
          and applies it once per card and then again for each double density.
          This is used for tests which have results that vary with the number
          of cards in the module.

	 correction factor because results of this test are dependent
     on the number and type of cards in module
     the .435ns is the correction factor for 1 single density card.  it was observed from
     Report data that indicated the shift in this test.  it was calculated as follows:
   
	 with 2 DD cards, mean for test was 1.12ns
	 with 9 DD cards, mean for test was -4.97ns
	 delta mean = 1.12 - -4.97 = 6.09ns
	 delta cards = 9-2 = 7
	 correction per DD card = 6.09ns / 7 = .87ns
	 correction per SD card is thus .87ns /2 = .435ns

INPUTS:   mod_num - the module that you need to determine a correction for
          sd_factor - the factor for a single density card

OUTPUT:   correction - a module configuration dependent correction factor
---------------------------------------------------------------------------- */

double calc_mod_loading(int mod_num, double sd_factor)
{
	int slot_num = 0;
	int card_type = 0;
	int num_unmux = 0; 
	double correction;
	
	for(slot_num = 2; slot_num <= 11; slot_num++)
	{
		card_type = get_testhead_actual_card(mod_num, slot_num);
		if((card_type == TDD10) || (card_type == TDD2))  
			num_unmux++;
	}
	
	if (num_unmux == 1)		
		correction = 0;
	else
		correction = num_unmux * sd_factor;  
	
	return correction;
}



/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------------ */

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

  ofile = fopen("data.out", "a\n");  // data.out file in dgn directory is appended with the latest data
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

/* ------------------------------------------------------------------------ */

void connect_cntr_ch1_af2(int mod_id) 
{  /*connect_cntr_start -> AF2*/
  /* USED BY DIGITAL FIXTURE ONLY */
  /* New Sales Rack configuration for Monet - JRM 2/22/93 */
  /* New counter implementation - ABR 7/6/99 */
  connect(mod_id, 502, true);
  connect(mod_id, 913, true);
  connect(mod_id, mod_id + 901, true);
}  /*connect_cntr_AF2*/

/* ------------------------------------------------------------------------ */

void connect_gen_out_cntr_ch1(int mod_id)
{
/* scope elimination project functions */
// new for scope elimination project
  /* USED BY DIGITAL FIXTURE ONLY */
  connect(mod_id, 522, true);
  connect(mod_id, 703, true);
  connect(mod_id, 504, true);
}

/* ------------------------------------------------------------------------ */

void connect_cntr_ch2_af2(int mod_id)
{
	/* USED BY ANALOG FIXTURE */
	connect(mod_id, 911, true);
	connect(mod_id, mod_id + 901, true);
}

/*----------------------------Test code for N6700B------------------------- */
//Added by chuanho, 28Jun2011

	/*Open N6700B*/
void open_n6700b(int mod_id)
{
	err = viOpen(rsm, OLYM, VI_NULL, VI_NULL, &viOLYM );
	if( err != 0 )
	{
		display_prompt("Can't connect N6700B.", true);
	}
	viSetAttribute(viOLYM, VI_ATTR_TMO_VALUE, HPIB_TO);
	viSetAttribute(viOLYM, VI_ATTR_TERMCHAR_EN, VI_TRUE);
	viPrintf(viOLYM, "*RST\n");
}  
	/*End of open N6700B*/

	/*Verify N6700B*/
void verify_n6700b(int mod_id)
{
	ViStatus  hw_err;
	char buf1[256] = {0};
	char str1[256] = {0};

	err = viQueryf(viOLYM, "*IDN?", "%t", &buf);
	hw_err = viQueryf(viOLYM, "SYST:ERR?", "%t", &buf1);
	if(err != 0 && hw_err != 0)
	{
  		display_prompt("N6700B not responding to HPIB queries", true);
		return;
	}
	else
	{
		sprintf(str1, "%.38s", buf);
		display_prompt(str1, true);		
		display_prompt("   N6700B Power Supply Okay", true); 
	}
  
}
	/*End of verify N6700B*/

	/*Setup N6700B Voltage*/
void setup_n6700b_ch1_volt(int mod_id, double amplitude)
{
	char setup_string[81];
	
	/* SETUP THE AMPLITUDE */
    *setup_string = '\0';
    sprintf(setup_string, "%s %1.3f %s\n", "VOLT", amplitude, ", (@1)");
	viPrintf(viOLYM, setup_string);
}

/*Setup N6700B channel 1 Current*/

void setup_n6700b_ch1_curr(int mod_id, double current)
{
	char setup_string[81];

	/* SETUP THE CURRENT */
	*setup_string = '\0';
	sprintf(setup_string, "%s %1.3f %s\n", "CURR", current, ", (@1)");
	viPrintf(viOLYM, setup_string);
}
	/*End of setup N6700B channel 1 Current*/


/*Reverse N6700B channel 1 output*/
void reverse_n6700b_ch1_out(int mod_id)
{
	viPrintf(viOLYM, "OUTP:REL:POL REV, (@1)\n");
}
/*End of reverse N6700B channel 1 output*/

/*Enable N6700B channel 1 output*/
void enable_n6700b_ch1_out(int mod_id)
{
	viPrintf(viOLYM, "OUTP ON,(@1)\n");
}
/*End of enable N6700B channel 1 output*/

/*Disable N6700B channel 1 output*/
void disable_n6700b_ch1_out(int mod_id)
{
	viPrintf(viOLYM, "OUTP OFF,(@1)\n");
}
/*End of disable N6700B channel 1 output*/

/*Close N6700B*/
void close_n6700b(int mod_id)
{
	char buf1[256] = {0};
	err = viQueryf(viOLYM, "*OPC?\n", "%t", &buf1);
	if (err != 0)
	{
		printf("GPIB command pending on N6700B!!!");
		
	}
	else
		viPrintf(viOLYM, "*RST\n");
		viClose(viOLYM);		
}
/*End of close N6700B*/
/* ---------------------End Of test code for N6700B------------------------- */

/*N6700b connections*/
//Added by chuanho, 30jun2011
/* ******************************************************************** */

void connect_n6700b_ch1(int mod_id)
{
  /* USED BY ANALOG FIXTURE ONLY */



  /* connecting the n6700b is accomplished by closing the relays since the   */

  /* n6700b channel 1 contacts are made at the normally open connections                 */

  connect(mod_id, 213, true); // - of n6700b (white wire)

  connect(mod_id, 214, true); // -S of n6700b (green wire)

  connect(mod_id, 215, true); // +S of n6700b (red wire)

  connect(mod_id, 216, true); // + of n6700b (black wire)
}

/*End of n6700b connections*/

/* ******************************************************************** */


//Added by chuanho, 30jun2011
/* ******************************************************************** */
/*Analog connection N6700B channel 1 to X Mint*/

void aconnect_n6700b_ch1_abus1_xmint(int mod_id)

{

  /* Connects X1 and X2 to the N6700B channel 1 output */

  int module_contribution;


  /* Connect N6700B channel 1 to Analog bus 1*/
  connect(mod_id, 211, true);

  connect(mod_id, 212, true);

  /* Connect Analog bus 1 to X1 & X2*/

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
/*End of Analog connection N6700B channel 1 to X Mint*/

/* ******************************************************************** */

//Added by chuanho, 30jun2011
/* ******************************************************************** */
/*Connect N6700B output to dmm*/
void aconnect_n6700b_ch1_out_dmm(int mod_id, int status)
{
	/*Used by Analog Fixture only*/

	
	connect(mod_id, 390, status);

	connect(mod_id, 331, status);

	connect(mod_id, 321, status);
}
	/*End of Connect N6700B output to dmm*/

//Added by chuanho, 18jul2011
/* ******************************************************************** */
/*Digital connection N6700B channel 1 output to af2*/
void dconnect_n6700b_ch1_af2(int mod_id)
{
  /* USED BY DIGITAL FIXTURE ONLY */

  connect(mod_id, 724, true);

  connect(mod_id, 824, true);

  connect(mod_id, 911, true);

  connect(mod_id, mod_id + 901, true);	

}
	/*End of Digital connection N6700B channel 1 output to af2*/

/* ******************************************************************** */
/* ------------------------------------------------------------------------ */

/*   *---------------------------------------------------------------------------
   * The following table describes the revision code register content:
   *
   *    bit:  15  14  13  12  11  10  09  08  07  06  05  04  03  02  01  00
   *         <----style---->  <--year-->  <----month--->  <-------day------>
   *
   *  where: <style> 0000 = R&D date code     
   *                 0001 = TDB
   *                 0010 = TDB
   *                 0011 = TDB
   *                 0100 = TDB
   *                 0101 = TDB
   *                 0110 = TDB
   *                 0111 = TDB
   *                 1000 = TDB
   *                 1001 = TDB
   *                 1010 = TDB
   *                 1011 = TDB
   *                 1100 = TDB
   *                 1101 = TDB
   *                 1110 = TDB
   *                 1111 = TDB
   *
   *         R&D date code encoding:
   *
   *         <year> 000 = 2000  <month> 0000 = <ILLEGAL>  <day> 00000 = <ILLEGAL>
   *                001 = 2001          0001 = January          00001 =  1
   *                010 = 2002          0010 = February         00010 =  2
   *                011 = 2003          0011 = March            00011 =  3
   *                100 = 2004          0100 = April            00100 =  4
   *                110 = 2006          0101 = May              00101 =  5
   *                111 = 2007          0110 = June             00110 =  6
   *                                    0111 = July             00111 =  7
   *                                    1000 = August           01000 =  8
   *                                    1001 = September        01001 =  9
   *                                    1010 = October          01010 = 10
   *                                    1011 = November         01011 = 11
   *                                    1100 = December         01100 = 12
   *                                    1101 = <ILLEGAL>        01101 = 13
   *                                    1110 = <ILLEGAL>        01110 = 14
   *                                    1111 = <ILLEGAL>        01111 = 15
   *                                                            10000 = 16
   *                                                            10001 = 17
   *                                                            10010 = 18
   *                                                            10011 = 19
   *                                                            10100 = 20
   *                                                            10101 = 21
   *                                                            10110 = 22
   *                                                            10111 = 23
   *                                                            11000 = 24
   *                                                            11001 = 25
   *                                                            11010 = 26
   *                                                            11011 = 27
   *                                                            11100 = 28
   *         <ILLEGAL (m==2 && y!=2000 && year!=2004)>          11101 = 29
   *         <ILLEGAL (m==2)>                                   11110 = 30
   *         <ILLEGAL (m==2 || m==4 || m==6 || m==9 || m==11)>  11111 = 31
   *
   * i =  2: Kestrel revision code (see above for encoding)
   * i =  3: Peregrine revision code (see above for encoding)
   *---------------------------------------------------------------------------
*/
void convert_byte_buffer_as_words(int mod_id)
{
  test_result_type result;

  if (get_result_nolog(mod_id, &result))
  {
      buffer_record_type const * br = static_cast<buffer_record_type *>(result.UU.Byte_ptr);
      assert(br);
      int l;                                       /* Index of left (high order byte) in buf. */
      int r;                                       /* Index of right (low order byte) in buf. */
      int m = (MAX_RESULT_BUFFER_INDEX / 2) * 2;   /* Maximum size (that is even) of buf. */
      int n = (br->Length < m) ? br->Length : m;   /* Ensure indices (l & r) do not exceed bounds of buf. */
      assert((n % 2) == 0);
      byte_type const * buf = br->Buffer;
      assert(buf);

      for (l = 0, r = 1; r < n; l += 2, r += 2)
      {
        typedef unsigned char u8bit_type;          /* unsigned 8 bit type */
        assert(sizeof(u8bit_type) == 1);           /* check size assumption */
        u8bit_type const byte_l = static_cast<u8bit_type>(buf[l]);
        u8bit_type const byte_r = static_cast<u8bit_type>(buf[r]);
        typedef unsigned short u16bit_type;        /* unsigned 16 bit type */
        assert(sizeof(u16bit_type) == 2);          /* check size assumption */
        u16bit_type const word = (static_cast<u16bit_type>(byte_l) << 8) | static_cast<u16bit_type>(byte_r);
        test_result_type res;                      /* Result created. */
        res.Measurement_class = CLASS_INTEGER;
        res.UU.Integer_result = static_cast<int>(word);
        log_result(mod_id, res);                   /* Send result to standard dgn limits processing. */
      }
    
  }
}


