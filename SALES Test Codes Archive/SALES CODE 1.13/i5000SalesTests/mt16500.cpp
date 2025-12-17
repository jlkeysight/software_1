#pragma warning(disable:4786)

#include <math.h>
#include <cassert>
#include <vector>
#include <set>
#include <algorithm>
#include <p2c/p2c.h>
#include <visa.h>
#include "../../../u/src/DebugWriter.h"
#include "../../../vth/src/vth_aim.h"
#include "../../../vth/src/vth_mac.h"
#include "../../../vth/src/aimasm.h"
#include "../../../vth/src/vth_cap.h"
#include "../../../vth/src/macasm.h"
#include "../../../vth/src/MetaTesthead.h"
#include "../../../vth/src/ConnectBRCtoBus.h"
#include "../../../vth/src/ConnectDigitalResource.h"
#include "../../../dgn/src/dgnconfig_p.h"
#include "../../../dgn/src/parms_p.h"
#include "../../../dgn/src/termio_p.h"
#include "../Feature.h"
#include "../i5000Tests/TestRelays.h"
#include "../i5000Tests/TestTalon.h" 
#include "../i5000Tests/TestPin.h" 
#include "RelayPath.h"
#include "mt16500.h"

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
using HTalon::KAS02;
using HTalon::KAI22;

 /*****************************************************************************************
 * The following tables and equations will allow you to map between a subtest
 * number and the measurement.
 *---------------------------------------------------------------------------------
 * channel (c) to i map:
 *  001   0 | 002   2 | 003   4 | 004   6 | 005   8 | 006  10 | 007  12 | 008  14 | 009  16
 *  010  18 | 011  20 | 012  22 | 013  24 | 014  26 | 015  28 | 016  30 | 017  32 | 018  34
 *  021  36 | 022  38 | 023  40 | 024  42 | 025  44 | 026  46 | 027  48 | 028  50 | 029  52
 *  030  54 | 031  56 | 032  58 | 033  60 | 034  62 | 035  64 | 036  66 | 037  68 | 038  70
 *  041  72 | 042  74 | 043  76 | 044  78 | 045  80 | 046  82 | 047  84 | 048  86 | 049  88
 *  050  90 | 051  92 | 052  94 | 053  96 | 054  98 | 055 100 | 056 102 | 057 104 | 058 106
 *  061 108 | 062 110 | 063 112 | 064 114 | 065 116 | 066 118 | 067 120 | 068 122 | 069 124
 *  070 126 | 071 128 | 072 130 | 073 132 | 074 134 | 075 136 | 076 138 | 077 140 | 078 142
 *  101 144 | 102 146 | 103 148 | 104 150 | 105 152 | 106 154 | 107 156 | 108 158 | 109 160
 *  110 162 | 111 164 | 112 166 | 113 168 | 114 170 | 115 172 | 116 174 | 117 176 | 118 178
 *  121 180 | 122 182 | 123 184 | 124 186 | 125 188 | 126 190 | 127 192 | 128 194 | 129 196
 *  130 198 | 131 200 | 132 202 | 133 204 | 134 206 | 135 208 | 136 210 | 137 212 | 138 214
 *  141 216 | 142 218 | 143 220 | 144 222 | 145 224 | 146 226 | 147 228 | 148 230 | 149 232
 *  150 234 | 151 236 | 152 238 | 153 240 | 154 242 | 155 244 | 156 246 | 157 248 | 158 250
 *  161 252 | 162 254 | 163 256 | 164 258 | 165 260 | 166 262 | 167 264 | 168 266 | 169 268
 *  170 270 | 171 272 | 172 274 | 173 276 | 174 278 | 175 280 | 176 282 | 177 284 | 178 286
 *******************************************************************************************/

/*
*--------------------------------------------------------------------------------
* Test: 16500
* Description: Standard components - fixture ID 
* 
* Fixture: Standard Components	
* Resource(s): 
*--------------------------------------------------------------------------------
*/

void t16500(int mod_id, int slot)
{
  static int const ID_MIN     = 3780;
  static int const ID_MAX     = 3783;
  static int const ID_MIN_EXP = 3798;
  static int const ID_MAX_EXP = 3803;
  int fixture_id;

  char buffer[81];
  test_result_type result;

//  if (!vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_lock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  vth_mrd_cell(mod_id, MIO_FXT_EN);
  if (get_result_nolog(mod_id, &result))
  {
    if (result.UU.Integer_result != FIXTURE_ENABLED)
    {
//      if (!vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//        display_prompt("Press footswitch to (un)lock fixture",true);

      vth_cfxt_unlock();
      /* FLUSH THE COMMANDS */
      if (get_result_nolog(mod_id, &result))
        display_prompt("Unexpected result encountered", true);
      display_prompt("Please install the Standard Components Sales fixture.", true);
      display_prompt("Then press <Return> or <Enter>.  ", true);
      get_user_answer(buffer, 80);
    }
  }

//  if (!vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//    display_prompt("Press footswitch to (un)lock fixture",true);

  vth_cfxt_lock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  vth_mrd_cell(mod_id, MIO_FXT_EN);
  if (get_result_nolog(mod_id, &result))
  {
    if (result.UU.Integer_result == FIXTURE_ENABLED)
    {  /*fixture found - check type*/
      fixture_id = get_fixture_id(mod_id);
     // if (fixture_id < ID_MIN || fixture_id > ID_MAX)
      if (!(((ID_MIN <= fixture_id) && (fixture_id <= ID_MAX)) || ((ID_MIN_EXP <= fixture_id) && (fixture_id <= ID_MAX_EXP) )))
	  {
        display_prompt("Incorrect Fixture Found", true);
        /* Next routine causes the non-module failure to be set also */
        add_test_to_gfs(present_test[mod_id - NO_TESTHEAD_NUMBER]);
      }
      //result.UU.Integer_result = fixture_id;
      //log_result(mod_id, result);
    }  /*fixture found - check type*/
    else
    {  /*no fixture found*/
      display_prompt("No fixture found.", true);
      /* Next routine causes the non-module failure to be set also */
      add_test_to_gfs(present_test[mod_id - NO_TESTHEAD_NUMBER]);

//      if (!vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
//        display_prompt("Press footswitch to (un)lock fixture",true);

      vth_cfxt_unlock();
    }  /*no fixture found*/

  }

}  /*t16500*/

/*
*--------------------------------------------------------------------------------
* Test: 16540
* Description: Talon standard components: standard resistors 
* Notes:       Dedicated to Slot 2, Fixture only utilizes Page A
* Fixture: Standard Components	
* Resource(s): None
*
* Test 16540, subtests 0, 1, 2, 3, 4 and 5 were replaced by 16548 subtests 0, 1, 2, 3, 4,  and 5.   
* The new 16548 performs a true 4 wire measurement up through the mint pin to the Resistor under test.  
* Test 16540 does a 4 wire sense measurement only up to the relay field and does NOT account for the 
* trace resistance from the relay, through the mint pint up to the DUT - as is apparent from the data, 
* this additional path resistance adds about 150 milliohms to the component value on test 16540.  
* Test 15548, being a true 4 wire measurement, compensates for the path resistance.  
* You do NOT want to remove the subtests in 16540 - this would corrupt existing data analysis tools 
* for the remaining 16540 subtests.
*
* Test limits for 16540 are as follows so as to never fail a good PCA:
* Subtest 0:  for the .1 ohm --> limit +/- 2 ohms 
* Subtest 1:  for the  1 ohm --> limit +/- 2 ohms
* Subtest 2:  for the 10 ohm --> limit +/- 20 ohms
* Subtest 3:  for the 10 ohm --> limit +/- 20 ohms
* Subtest 4:  for the 100 ohm --> limit +/- 110 ohms
* Subtest 5:  for the 100 ohm --> limit +/- 110 ohms
*--------------------------------------------------------------------------------
*/
static int const INDUCTOR_FXT = 3801;

struct rlylist
{
  HTalon::Relays rut;
  HTalon::Relays ref;
  rlylist(RLYLIST rut_b, RLYLIST rut_e, RLYLIST ref_b, RLYLIST ref_e)
	  : rut(rut_b, rut_e), ref(ref_b, ref_e) {}
}; // end struct rlylist

void t16540(int mod_id, int slot)
{
  static int const FILTER           = 10;
  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  2;
  static int const NUM_MEASUREMENTS = 22;  // 22 (0-21 subtests)// different from number resistors - can have more than
                                           // one measurement for a given resistor
	double nominal_value[NUM_MEASUREMENTS];
	double src_amp[NUM_MEASUREMENTS];
	double src_off[NUM_MEASUREMENTS];
	double src_wait[NUM_MEASUREMENTS];
	double moa_out[NUM_MEASUREMENTS];
	double det_wait[NUM_MEASUREMENTS];
	int src_freq[NUM_MEASUREMENTS];
	int src_comp[NUM_MEASUREMENTS];
	int en[NUM_MEASUREMENTS];
	int ed[NUM_MEASUREMENTS];
	int rf[NUM_MEASUREMENTS];
	int sa[NUM_MEASUREMENTS];
	int sb[NUM_MEASUREMENTS];
	int sl[NUM_MEASUREMENTS];
	int wb[NUM_MEASUREMENTS];
	int hfs_cap[NUM_MEASUREMENTS];
	int diff_det[NUM_MEASUREMENTS];
	int src_type[NUM_MEASUREMENTS];
	int ser_mode[NUM_MEASUREMENTS];
	int comp_type[NUM_MEASUREMENTS];
	int dc_rej[NUM_MEASUREMENTS];
	int reading;
	int card_type;
	char buffer[81];
	
	test_result_type result;  
	std_component_array actual_value;
	FILE *infile;
	infile = NULL;
	
	static HTalon::RelayName const SI_01[4]    = {HTalon::KAS05, HTalon::KAS06, HTalon::KAI25, HTalon::KAI26};
	static HTalon::RelayName const SI_1[4]     = {HTalon::KAS07, HTalon::KAS08, HTalon::KAI27, HTalon::KAI28};
	static HTalon::RelayName const SI_10[4]    = {HTalon::KAS09, HTalon::KAS10, HTalon::KAI29, HTalon::KAI30};
	static HTalon::RelayName const SI_100_4[4] = {HTalon::KAS11, HTalon::KAS12, HTalon::KAI31, HTalon::KAI32};
	static HTalon::RelayName const SI_100_2[2] = {HTalon::KAS11, HTalon::KAI32};
	static HTalon::RelayName const SI_1K_4[4]  = {HTalon::KAS13, HTalon::KAS14, HTalon::KAI33, HTalon::KAI34};
	static HTalon::RelayName const SI_1K_2[2]  = {HTalon::KAS13, HTalon::KAI33};
	static HTalon::RelayName const SI_10K_4[4] = {HTalon::KAS45, HTalon::KAS46, HTalon::KAI66, HTalon::KAI65};
	static HTalon::RelayName const SI_10K_2[2] = {HTalon::KAS45, HTalon::KAI66};
	static HTalon::RelayName const SI_100K[2]  = {HTalon::KAS47, HTalon::KAI68};
	static HTalon::RelayName const SI_1M[2]    = {HTalon::KAS49, HTalon::KAI70};
	static HTalon::RelayName const SI_10M[2]   = {HTalon::KAS51, HTalon::KAI72};
	static HTalon::RelayName const SI_SUB_4[6] = {HTalon::KSA, HTalon::KSAA, HTalon::KAA, HTalon::KIA, HTalon::KIBA, HTalon::KBA};
	static HTalon::RelayName const SI_SUB_2[2] = {HTalon::KSA, HTalon::KIA};

static unsigned int const n=22;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SI_01),    R(SI_SUB_4)),  //4W subtest 0 
	rlylist(R(SI_1),     R(SI_SUB_4)),	//4W subtest 1
	rlylist(R(SI_10),    R(SI_SUB_4)),  //4W subtest 2
	rlylist(R(SI_10),    R(SI_SUB_4)),  //4W subtest 3
	rlylist(R(SI_100_4), R(SI_SUB_4)),  //4W subtest 4
	rlylist(R(SI_100_4), R(SI_SUB_4)),  //4W subtest 5
	rlylist(R(SI_100_2), R(SI_SUB_2)),  //2W subtest 6
	rlylist(R(SI_1K_4),  R(SI_SUB_4)),  //4W subtest 7
    rlylist(R(SI_1K_2),  R(SI_SUB_2)),  //2W subtest 8
	rlylist(R(SI_1K_4),  R(SI_SUB_4)),  //4W subtest 9
    rlylist(R(SI_1K_2),  R(SI_SUB_2)),  //2W subtest 10
	rlylist(R(SI_10K_4), R(SI_SUB_4)),  //4W subtest 11
    rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 12
	rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 13
	rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 14
	rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 15
	rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 16
	rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 17
	rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 18
	rlylist(R(SI_1M),    R(SI_SUB_2)),  //2W subtest 19
	rlylist(R(SI_1M),    R(SI_SUB_2)),  //2W subtest 20
	rlylist(R(SI_10M),   R(SI_SUB_2)),  //2W subtest 21

#undef R
}; // end static rlylist const

  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
	
	card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, PIN_SLOT);
	talonCard->openAllRelays(false,true);  //Initialize Talon
	vth_aopen_all_rlys(mod_id);
	if (card_type != TDD10) // Verify Talon Card
	{  
		display_prompt("Standard Resistors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 2.  Test 16540 will not be run!", true);
		_EscIO(FileNotFound);
	}  // end if card_type
	//present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
	load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);
	sprintf(buffer, "%sstd.resistor_talon", testwhere_path);  // loads CAL Lab standard values
	infile = fopen(buffer, "r");
	if (infile == NULL)
	{
		display_prompt("The 'std.resistor_talon' file was not found.  Please have sys admin replace.", true);
		_EscIO(FileNotFound);
	} // end if infile
	
	// Read ASRU measurement setup parameters for each R Measurement
	for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
	{
		fscanf(infile, "%lg", &nominal_value[reading]);
		fscanf(infile, "%d", &src_freq[reading]);
		fscanf(infile, "%lg", &src_amp[reading]);
		fscanf(infile, "%lg", &src_off[reading]);
		fscanf(infile, "%lg", &src_wait[reading]);
		fscanf(infile, "%d", &src_comp[reading]);
		fscanf(infile, "%d", &en[reading]);
		fscanf(infile, "%d", &ed[reading]);
		fscanf(infile, "%d", &rf[reading]);
		fscanf(infile, "%d", &sa[reading]);
		fscanf(infile, "%d", &sb[reading]);
		fscanf(infile, "%d", &sl[reading]);
		fscanf(infile, "%d", &wb[reading]);
		fscanf(infile, "%d", &hfs_cap[reading]);
		fscanf(infile, "%d", &diff_det[reading]);
		fscanf(infile, "%lg", &moa_out[reading]);
		fscanf(infile, "%lg", &det_wait[reading]);
		fscanf(infile, "%d", &src_type[reading]);
		fscanf(infile, "%d", &ser_mode[reading]);
		fscanf(infile, "%d", &comp_type[reading]);
		fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
		getc(infile);
	}  // end for reading
	if (infile != NULL)
		fclose(infile);
	infile = NULL;

	for (int p = 0; p < n; ++p) 
	{ /*for: subtest*/
		vth_mra_in_mod(mod_id);
	
		
		if (p == 0)   // 0.1 ohm resistor 
		{  
			vth_ameas_set_averaging(mod_id, FILTER);	// TURN FILTERING ON TO DECREASE THE DISTRIBUTION OF THE pS 
		}  
		else
		{  
			vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);	// USE THE DEFAULT FILTER VALUE 
		}  

		close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		if (sa[p] == 1)  vth_aclose_rly(mod_id, K710); // X2 to AIM A Bus
	    if (sb[p] == 1)  vth_aclose_rly(mod_id, K704); // X4 to AIM B Bus
		vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

        vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
			src_wait[p], src_comp[p], en[p],
			ed[p], rf[p], sa[p], sb[p],
			sl[p], wb[p], hfs_cap[p], diff_det[p],
			moa_out[p], det_wait[p]);
		vth_resistor_meas(mod_id, ser_mode[p], dc_rej[p]);
		vth_get_resistor_result(mod_id);
		if (get_result_nolog(mod_id, &result))
		{  
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		}
		else
			display_prompt("No result in result log (Expected component value)", true);
		
		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);
		
	} /*for: subtest*/
  }
	if (infile != NULL)
		fclose(infile);	
	vth_pinit(mod_id, PIN_SLOT);
	vth_ainit(mod_id);
	
}  /*t16540*/


////added test T16561, T16562, T16563 equivalent to T16540, T16542, T16546 for NASRU with new TMS SPEC///
/*--------------------------------------------------------------------------------
* Test: 16561
* Description: Talon standard components: standard resistors 
* Notes:       Dedicated to Slot 2, Fixture only utilizes Page A
* Fixture: Standard Components	
* Resource(s): None
*
* Test 16561, subtests 0, 1, 2, 3, 4 and 5 were replaced by 16548 subtests 0, 1, 2, 3, 4,  and 5.   
* The new 16548 performs a true 4 wire measurement up through the mint pin to the Resistor under test.  
* Test 16540 does a 4 wire sense measurement only up to the relay field and does NOT account for the 
* trace resistance from the relay, through the mint pint up to the DUT - as is apparent from the data, 
* this additional path resistance adds about 150 milliohms to the component value on test 16561.  
* Test 15548, being a true 4 wire measurement, compensates for the path resistance.  
* You do NOT want to remove the subtests in 16561 - this would corrupt existing data analysis tools 
* for the remaining 16561 subtests.
*
* Test limits for 16561 are as follows so as to never fail a good PCA:
* Subtest 0:  for the .1 ohm --> limit +/- 2 ohms 
* Subtest 1:  for the  1 ohm --> limit +/- 2 ohms
* Subtest 2:  for the 10 ohm --> limit +/- 20 ohms
* Subtest 3:  for the 10 ohm --> limit +/- 20 ohms
* Subtest 4:  for the 100 ohm --> limit +/- 110 ohms
* Subtest 5:  for the 100 ohm --> limit +/- 110 ohms
*--------------------------------------------------------------------------------
*/
//static int const INDUCTOR_FXT = 3801;

//struct rlylist
//{
//  HTalon::Relays rut;
//  HTalon::Relays ref;
//  rlylist(RLYLIST rut_b, RLYLIST rut_e, RLYLIST ref_b, RLYLIST ref_e)
//	  : rut(rut_b, rut_e), ref(ref_b, ref_e) {}
//}; // end struct rlylist

void t16561(int mod_id, int slot)
{
  static int const FILTER           = 10;
  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  2;
  static int const NUM_MEASUREMENTS =  6;  // (0-5 subtests)// different from number resistors - can have more than
                                           // one measurement for a given resistor
	double nominal_value[NUM_MEASUREMENTS];
	double src_amp[NUM_MEASUREMENTS];
	double src_off[NUM_MEASUREMENTS];
	double src_wait[NUM_MEASUREMENTS];
	double moa_out[NUM_MEASUREMENTS];
	double det_wait[NUM_MEASUREMENTS];
	int src_freq[NUM_MEASUREMENTS];
	int src_comp[NUM_MEASUREMENTS];
	int en[NUM_MEASUREMENTS];
	int ed[NUM_MEASUREMENTS];
	int rf[NUM_MEASUREMENTS];
	int sa[NUM_MEASUREMENTS];
	int sb[NUM_MEASUREMENTS];
	int sl[NUM_MEASUREMENTS];
	int wb[NUM_MEASUREMENTS];
	int hfs_cap[NUM_MEASUREMENTS];
	int diff_det[NUM_MEASUREMENTS];
	int src_type[NUM_MEASUREMENTS];
	int ser_mode[NUM_MEASUREMENTS];
	int comp_type[NUM_MEASUREMENTS];
	int dc_rej[NUM_MEASUREMENTS];
	int reading;

	int card_type;
	char buffer[81];
	
	test_result_type result;  
	std_component_array actual_value;
	FILE *infile;
	infile = NULL;
	
	static HTalon::RelayName const SI_01[4]    = {HTalon::KAS05, HTalon::KBS06, HTalon::KBI25, HTalon::KAI26};
	static HTalon::RelayName const SI_1[4]     = {HTalon::KAS07, HTalon::KBS08, HTalon::KBI27, HTalon::KAI28};
	static HTalon::RelayName const SI_10[4]    = {HTalon::KAS09, HTalon::KBS10, HTalon::KBI29, HTalon::KAI30};
	static HTalon::RelayName const SI_100_4[4] = {HTalon::KAS11, HTalon::KBS12, HTalon::KBI31, HTalon::KAI32};
	//static HTalon::RelayName const SI_100_2[2] = {HTalon::KAS11, HTalon::KAI32};
	//static HTalon::RelayName const SI_1K_4[4]  = {HTalon::KAS13, HTalon::KBS14, HTalon::KBI33, HTalon::KAI34};
	//static HTalon::RelayName const SI_1K_2[2]  = {HTalon::KAS13, HTalon::KAI33};
	//static HTalon::RelayName const SI_10K_4[4] = {HTalon::KAS45, HTalon::KBS46, HTalon::KBI66, HTalon::KAI65};
	//static HTalon::RelayName const SI_10K_2[2] = {HTalon::KAS45, HTalon::KAI66};
	//static HTalon::RelayName const SI_100K[2]  = {HTalon::KAS47, HTalon::KAI68};
	//static HTalon::RelayName const SI_1M[2]    = {HTalon::KAS49, HTalon::KAI70};
	//static HTalon::RelayName const SI_10M[2]   = {HTalon::KAS51, HTalon::KAI72};
	static HTalon::RelayName const SI_SUB_4[6] = {HTalon::KSA, HTalon::KSAB, HTalon::KAB, HTalon::KIA, HTalon::KIBB, HTalon::KBB};
	//static HTalon::RelayName const SI_SUB_2[2] = {HTalon::KSA, HTalon::KIA};

static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SI_01),    R(SI_SUB_4)),  //4W subtest 0 
	rlylist(R(SI_1),     R(SI_SUB_4)),	//4W subtest 1
	rlylist(R(SI_10),    R(SI_SUB_4)),  //4W subtest 2
	rlylist(R(SI_10),    R(SI_SUB_4)),  //4W subtest 3
	rlylist(R(SI_100_4), R(SI_SUB_4)),  //4W subtest 4
	rlylist(R(SI_100_4), R(SI_SUB_4)),  //4W subtest 5
	//rlylist(R(SI_100_2), R(SI_SUB_2)),  //2W subtest 6
	//rlylist(R(SI_1K_4),  R(SI_SUB_4)),  //4W subtest 7
    //rlylist(R(SI_1K_2),  R(SI_SUB_2)),  //2W subtest 8
	//rlylist(R(SI_1K_4),  R(SI_SUB_4)),  //4W subtest 9
    //rlylist(R(SI_1K_2),  R(SI_SUB_2)),  //2W subtest 10
	//rlylist(R(SI_10K_4), R(SI_SUB_4)),  //4W subtest 11
    //rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 12
	//rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 13
	//rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 14
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 15
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 16
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 17
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 18
	//rlylist(R(SI_1M),    R(SI_SUB_2)),  //2W subtest 19
	//rlylist(R(SI_1M),    R(SI_SUB_2)),  //2W subtest 20
	//rlylist(R(SI_10M),   R(SI_SUB_2)),  //2W subtest 21

#undef R
}; // end static rlylist const
	
  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */

	card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, PIN_SLOT);
	talonCard->openAllRelays(false,true);  //Initialize Talon
	vth_aopen_all_rlys(mod_id);
	if (card_type != TDD10) // Verify Talon Card
	{  
		display_prompt("Standard Resistors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 2.  Test 16561 will not be run!", true);
		_EscIO(FileNotFound);
	}  // end if card_type
	//present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
	load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);
	sprintf(buffer, "%sstd.resistor_talon", testwhere_path);  // loads CAL Lab standard values
	infile = fopen(buffer, "r");
	if (infile == NULL)
	{
		display_prompt("The 'std.resistor_talon' file was not found.  Please have sys admin replace.", true);
		_EscIO(FileNotFound);
	} // end if infile
	
	// Read ASRU measurement setup parameters for each R Measurement
	for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
	{
		fscanf(infile, "%lg", &nominal_value[reading]);
		fscanf(infile, "%d", &src_freq[reading]);
		fscanf(infile, "%lg", &src_amp[reading]);
		fscanf(infile, "%lg", &src_off[reading]);
		fscanf(infile, "%lg", &src_wait[reading]);
		fscanf(infile, "%d", &src_comp[reading]);
		fscanf(infile, "%d", &en[reading]);
		fscanf(infile, "%d", &ed[reading]);
		fscanf(infile, "%d", &rf[reading]);
		fscanf(infile, "%d", &sa[reading]);
		fscanf(infile, "%d", &sb[reading]);
		fscanf(infile, "%d", &sl[reading]);
		fscanf(infile, "%d", &wb[reading]);
		fscanf(infile, "%d", &hfs_cap[reading]);
		fscanf(infile, "%d", &diff_det[reading]);
		fscanf(infile, "%lg", &moa_out[reading]);
		fscanf(infile, "%lg", &det_wait[reading]);
		fscanf(infile, "%d", &src_type[reading]);
		fscanf(infile, "%d", &ser_mode[reading]);
		fscanf(infile, "%d", &comp_type[reading]);
		fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
		getc(infile);
	}  // end for reading
	if (infile != NULL)
		fclose(infile);
	infile = NULL;

	for (int p = 0; p < n; ++p) 
	{ /*for: subtest*/
		vth_mra_in_mod(mod_id);
	
		
		if (p == 0)   // 0.1 ohm resistor 
		{  
			vth_ameas_set_averaging(mod_id, FILTER);	// TURN FILTERING ON TO DECREASE THE DISTRIBUTION OF THE pS 
		}  
		else
		{  
			vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);	// USE THE DEFAULT FILTER VALUE 
		}  

		close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		if (sa[p] == 1)  vth_aclose_rly(mod_id, K714); // X6 to AIM A Bus Page B
	    if (sb[p] == 1)  vth_aclose_rly(mod_id, K708); // X8 to AIM B Bus Page B
		vth_aclose_rly(mod_id, K843);  // MOANON to ACOM 
		vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

		vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
			src_wait[p], src_comp[p], en[p],
			ed[p], rf[p], sa[p], sb[p],
			sl[p], wb[p], hfs_cap[p], diff_det[p],
			moa_out[p], det_wait[p]);
		vth_resistor_meas(mod_id, ser_mode[p], dc_rej[p]);
		vth_get_resistor_result(mod_id);
		if (get_result_nolog(mod_id, &result))
		{  
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		}
		else
			display_prompt("No result in result log (Expected component value)", true);
		
		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);
		
	} /*for: subtest*/
  }

	if (infile != NULL)
		fclose(infile);	
	vth_pinit(mod_id, PIN_SLOT);
	vth_ainit(mod_id);
	
}  /*t16561*/


static int const RC_FXT = 3800;

/* INDUCTOR STANDARD COMPONENTS TESTS */
void t16562(int mod_id, int /*slot*/)
{ //Inductor Standard Components Tests
  //Dedicated to Slot 4, Fixture only utilizes Page A
//  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  4;
  static int const NUM_MEASUREMENTS = 13;

int reading;

// --- 2 & 4 Wire Measurement Paths on Talon
//Inductor 2 uHenry 4 wire (subtest 0)
//static HTalon::RelayName const S_Bus_L2uHenry[2]    = {HTalon::KAS05,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L2uHenry[3]    = {HTalon::KAS06, HTalon::KSAA,  HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L2uHenry[2]    = {HTalon::KAI26,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L2uhHenry[3]   = {HTalon::KAI25, HTalon::KIBA,  HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L2uHenry[5]   = {HTalon::KAS05, HTalon::KAS06, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L2uHenry[5]   = {HTalon::KAI26, HTalon::KAI25, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 20 uHenry 4 wire (subtest 1)
//static HTalon::RelayName const S_Bus_L20uHenry[2]   = {HTalon::KAS53,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L20uHenry[3]   = {HTalon::KAS54, HTalon::KSAA,  HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L20uHenry[2]   = {HTalon::KAI74,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L20uHenry[3]   = {HTalon::KAI73, HTalon::KIBA,  HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L20uHenry[5]  = {HTalon::KAS53, HTalon::KAS54, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L20uHenry[5]  = {HTalon::KAI74, HTalon::KAI73, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 154 uHenry 4 wire (subtest 2)
//static HTalon::RelayName const S_Bus_L154uHenry[2]  = {HTalon::KAS07,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L154uHenry[3]  = {HTalon::KAS08, HTalon::KSAA,  HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L154uHenry[2]  = {HTalon::KAI28,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L154uHenry[3]  = {HTalon::KAI27, HTalon::KIBA,  HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L154uHenry[5] = {HTalon::KAS07, HTalon::KAS08, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L154uHenry[5] = {HTalon::KAI28, HTalon::KAI27, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 1.54 mHenry 4 wire (subtest 3)
//static HTalon::RelayName const S_Bus_L1Pt54mHenry[2] = {HTalon::KAS09,               HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L1Pt54mHenry[3] = {HTalon::KAS10, HTalon::KSAA, HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L1Pt54mHenry[2] = {HTalon::KAI30,               HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L1Pt54mHenry[3] = {HTalon::KAI29, HTalon::KIBA, HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L1Pt54mHenry[5]= {HTalon::KAS09, HTalon::KAS10,HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L1Pt54mHenry[5]= {HTalon::KAI30, HTalon::KAI29,HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 15.4 mHenry 4 wire (subtest 4)
//static HTalon::RelayName const S_Bus_L15Pt4mHenry[2] = {HTalon::KAS11,               HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L15Pt4mHenry[3] = {HTalon::KAS12, HTalon::KSAA, HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L15Pt4mHenry[2] = {HTalon::KAI32,               HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L15Pt4mHenry[3] = {HTalon::KAI31, HTalon::KIBA, HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L15Pt4mHenry[5]= {HTalon::KAS11, HTalon::KAS12,HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L15Pt4mHenry[5]= {HTalon::KAI32, HTalon::KAI31,HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 154 mHenry 2 wire (subtest 5 and 6)
static HTalon::RelayName const S_Bus_L154mHenry[2] = {HTalon::KAS13,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L154mHenry[3] = {HTalon::KAS14, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L154mHenry[2] = {HTalon::KAI34,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L154mHenry[3] = {HTalon::KAI33, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L154mHenry[5]= {HTalon::KAS13, HTalon::KAS14, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L154mHenry[5]= {HTalon::KAI34, HTalon::KAI33, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 1.54 Henry 2 wire (subtest 7, 8, and 9)
static HTalon::RelayName const S_Bus_L1Pt54Henry[2] = {HTalon::KAS45,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L1Pt54Henry[3] = {HTalon::KAS46, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L1Pt54Henry[2] = {HTalon::KAI66,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L1Pt54Henry[3] = {HTalon::KAI65, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L1Pt54Henry[5]= {HTalon::KAS45, HTalon::KAS46, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L1Pt54Henry[5]= {HTalon::KAI66, HTalon::KAI65, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 15.4 Henry 2 wire (subtest 10)
static HTalon::RelayName const S_Bus_L15Pt4Henry[2] = {HTalon::KAS47,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L15Pt4Henry[3] = {HTalon::KAS48, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L15Pt4Henry[2] = {HTalon::KAI68,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L15Pt4Henry[3] = {HTalon::KAI67, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L15Pt4Henry[5]= {HTalon::KAS47, HTalon::KAS48, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L15Pt4Henry[5]= {HTalon::KAI68, HTalon::KAI67, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 100 Henry 2 wire (subtest 11 and 12)
static HTalon::RelayName const S_Bus_L100Henry[2] = {HTalon::KAS49,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L100Henry[3] = {HTalon::KAS50, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L100Henry[2] = {HTalon::KAI70,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L100Henry[3] = {HTalon::KAI69, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L100Henry[5]= {HTalon::KAS49, HTalon::KAS50, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L100Henry[5]= {HTalon::KAI70, HTalon::KAI69, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SA_Bus_L2uHenry),    R(IB_Bus_L2uHenry)),    //4W subtest 0 
	rlylist(R(SA_Bus_L20uHenry),   R(IB_Bus_L20uHenry)),   //4W subtest 1 
	rlylist(R(SA_Bus_L154uHenry),  R(IB_Bus_L154uHenry)),  //4W subtest 2 
	rlylist(R(SA_Bus_L1Pt54mHenry),R(IB_Bus_L1Pt54mHenry)),//4W subtest 3  
	rlylist(R(SA_Bus_L15Pt4mHenry),R(IB_Bus_L15Pt4mHenry)),//4W subtest 4 
	rlylist(R(S_Bus_L154mHenry),   R(I_Bus_L154mHenry)),   //2W subtest 5  
	rlylist(R(S_Bus_L154mHenry),   R(I_Bus_L154mHenry)),   //2W subtest 6  same as 5
	rlylist(R(S_Bus_L1Pt54Henry),  R(I_Bus_L1Pt54Henry)),  //2W subtest 7
	rlylist(R(S_Bus_L1Pt54Henry),  R(I_Bus_L1Pt54Henry)),  //2W subtest 8  same as 7
    rlylist(R(S_Bus_L1Pt54Henry),  R(I_Bus_L1Pt54Henry)),  //2W subtest 9  same as 7
	rlylist(R(S_Bus_L15Pt4Henry),  R(I_Bus_L15Pt4Henry)),  //2W subtest 10
	rlylist(R(S_Bus_L100Henry),    R(I_Bus_L100Henry)),    //2W subtest 11
	rlylist(R(S_Bus_L100Henry),    R(I_Bus_L100Henry)),    //2W subtest 12 same as 11	
#undef R
};

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double src_off[NUM_MEASUREMENTS];
  double src_wait[NUM_MEASUREMENTS];
  int src_comp[NUM_MEASUREMENTS];
  int en[NUM_MEASUREMENTS];
  int ed[NUM_MEASUREMENTS];
  int rf[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int wb[NUM_MEASUREMENTS];
  int hfs_cap[NUM_MEASUREMENTS];
  int diff_det[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  int src_type[NUM_MEASUREMENTS];
  int ser_mode[NUM_MEASUREMENTS];
  int comp_type[NUM_MEASUREMENTS];
  int dc_rej[NUM_MEASUREMENTS];
 
  test_result_type result;
  char buffer[81];
  infile = NULL;
  
  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);

  if (get_fixture_id(mod_id) != RC_FXT)
  { /* Bypass Loop */

	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		display_prompt("Standard Inductores Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 4.  Test 16242 will not be run!", true);
	  }  //End Display no pin card in pin_slot message

	  else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  display_prompt("FOUND TALON CARD",true);// Else After check for slot 2 Talon pin card
		  /* Else After check for slot 2 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.inductor_talon", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'std.inductor_talon' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile


		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		  fscanf(infile, "%lg", &nominal_value[reading]);
		  fscanf(infile, "%d", &src_freq[reading]);
		  fscanf(infile, "%lg", &src_amp[reading]);
		  fscanf(infile, "%lg", &src_off[reading]);
		  fscanf(infile, "%lg", &src_wait[reading]);
		  fscanf(infile, "%d", &src_comp[reading]);
		  fscanf(infile, "%d", &en[reading]);
		  fscanf(infile, "%d", &ed[reading]);
		  fscanf(infile, "%d", &rf[reading]);
		  fscanf(infile, "%d", &sa[reading]);
		  fscanf(infile, "%d", &sb[reading]);
		  fscanf(infile, "%d", &sl[reading]);
		  fscanf(infile, "%d", &wb[reading]);
		  fscanf(infile, "%d", &hfs_cap[reading]);
		  fscanf(infile, "%d", &diff_det[reading]);
		  fscanf(infile, "%lg", &moa_out[reading]);
		  fscanf(infile, "%lg", &det_wait[reading]);
		  fscanf(infile, "%d", &src_type[reading]);
		  fscanf(infile, "%d", &ser_mode[reading]);
		  fscanf(infile, "%d", &comp_type[reading]);
		  fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
		  getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon

		//***********************BEGIN MAIN LOOP******************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		//NUM_MEASUREMENTS =20 but start p at 0
		{ // FOR (each SUBTEST)
      
		  //vth_mra_in_mod(mod_id);

		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  if (sa[p] == 1)  vth_aclose_rly(mod_id, K710); // X2 to AIM A Bus
		  if (sb[p] == 1)  vth_aclose_rly(mod_id, K704); // X4 to AIM B Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

		  vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
				src_wait[p], src_comp[p], en[p],
				ed[p], rf[p], sa[p], sb[p],
				sl[p], wb[p], hfs_cap[p], diff_det[p],
				moa_out[p], det_wait[p]);

		  //vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);      
		  vth_inductor_meas(mod_id, ser_mode[p], dc_rej[p]);
		  vth_get_inductor_result(mod_id);

		  if (get_result_nolog(mod_id, &result))
		  {  // apply the actual to the p to create a delta value 
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		  }  
		  else
			display_prompt("No result in result log (Expected component value)",
						   true);

		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for READING (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	

} /* t16562 */

//-------------------------------------------------------------------------



/*--------------------------------------------------------------------------------
* Test: 16563
* Description: Talon standard components: low value capacitors with compensation
*				
* Fixture: Standard components - Utilizes Page A (single density row of Pin card)	
* Resource(s): 
*--------------------------------------------------------------------------------
*/

void t16563(int mod_id, int /*slot*/)
{   // Test utilizes Pin card slot 3 Page A only
  static int const PIN_SLOT            =  3;
  static int const NUM_MEASUREMENTS    = 11;
  static double const LARGE_FORCE_FAIL = 1.0E20;
  int reading;
  double compensation_reading[11];
  double capacitor_reading;
  char buffer[81];
  test_result_type result;
  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double src_off[NUM_MEASUREMENTS];
  double src_wait[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  int    src_freq[NUM_MEASUREMENTS];
  int    src_comp[NUM_MEASUREMENTS];
  int    en[NUM_MEASUREMENTS];
  int    ed[NUM_MEASUREMENTS];
  int    rf[NUM_MEASUREMENTS];
  int    sa[NUM_MEASUREMENTS];
  int    sb[NUM_MEASUREMENTS];
  int    sl[NUM_MEASUREMENTS];
  int    wb[NUM_MEASUREMENTS];
  int    hfs_cap[NUM_MEASUREMENTS];
  int    diff_det[NUM_MEASUREMENTS];
  int    src_type[NUM_MEASUREMENTS];
  int    ser_mode[NUM_MEASUREMENTS];
  int    comp_type[NUM_MEASUREMENTS];
  int    dc_rej[NUM_MEASUREMENTS];
  
  // --- Compensation Paths on Talon
  static HTalon::RelayName const S_Bus_COMP[2]   = {HTalon::KAS15,  HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_COMP[2]   = {HTalon::KAI36,  HTalon::KIA}; //to X3
  
  // --- 2 Wire Capacitance Paths on Talon
  //NOTE FOR CLEAN UP : A and B Busses NOT USED for 2 WIRE measurements
  //Capacitor 150 nF - 2 wire (subtest 0)  
  static HTalon::RelayName const S_Bus_C150nF[2]   = {HTalon::KAS13, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C150nF[2]   = {HTalon::KAI34, HTalon::KIA}; //to X3
  
  //Capacitor 15 nF  - 2 wire (subtest 1 and 2)
  static HTalon::RelayName const S_Bus_C15nF[2]    = {HTalon::KAS45, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C15nF[2]    = {HTalon::KAI66, HTalon::KIA}; //to X3
  
  //Capacitor 1.5 nF - 2 wire (subtest 3, and 4) 
  static HTalon::RelayName const S_Bus_C1Pt5nF[2]  = {HTalon::KAS47, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C1Pt5nF[2]  = {HTalon::KAI68, HTalon::KIA}; //to X3
  
  //Capacitor 150 pF - 2 wire (subtest 5, 6, and 8)
  static HTalon::RelayName const S_Bus_C150pF[2]   = {HTalon::KAS49, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C150pF[2]   = {HTalon::KAI70, HTalon::KIA}; //to X3
  
  //Capacitor 15 pF - 2 wire (subtest 7 and 9)
  static HTalon::RelayName const S_Bus_C15pF[2]    = {HTalon::KAS51, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C15pF[2]    = {HTalon::KAI72, HTalon::KIA}; //to X3
  
  //Capacitor 10 pF - 2 wire (subtest 10)
  static HTalon::RelayName const S_Bus_C10pF[2]    = {HTalon::KAS53, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C10pF[2]    = {HTalon::KAI74, HTalon::KIA}; //to X3

  static unsigned int const n=NUM_MEASUREMENTS;
  static rlylist const tps[n+1] =  // add 1 to allow for Capacitance COMPENSATION setup
  {
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(S_Bus_C150nF),    R(I_Bus_C150nF)),   //2W subtest 0
	rlylist(R(S_Bus_C15nF),     R(I_Bus_C15nF)),    //2W subtest 1
	rlylist(R(S_Bus_C15nF),     R(I_Bus_C15nF)),    //2W subtest 2 same as 1
	rlylist(R(S_Bus_C1Pt5nF),   R(I_Bus_C1Pt5nF)),  //2W subtest 3
	rlylist(R(S_Bus_C1Pt5nF),   R(I_Bus_C1Pt5nF)),  //2W subtest 4 same as 3
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 5
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 6 same as 5
	rlylist(R(S_Bus_C15pF),     R(I_Bus_C15pF)),    //2W subtest 7
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 8 same as 5
	rlylist(R(S_Bus_C15pF),     R(I_Bus_C15pF)),    //2W subtest 9 same as 7
	rlylist(R(S_Bus_C10pF),     R(I_Bus_C10pF)),    //2W subtest 10
	rlylist(R(S_Bus_COMP),      R(I_Bus_COMP)),     //Path Capacitance COMP array #11
#undef R
}; // end static rlylist const

  FILE *infile;
  infile = NULL;
  std_component_array actual_value;
  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
	  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		 display_prompt("Standard Capacitors Test requires a Talon Pin Card", true);
		 display_prompt("type card in Slot 3.  Test 16563 will not be run!", true);
	  }  //End Display no pin card in pin_slot message
      else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  
		  /* Else After check for slot 3 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.compcap", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'std.compcap' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile

		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		  fscanf(infile, "%lg", &nominal_value[reading]);
		  fscanf(infile, "%d", &src_freq[reading]);
		  fscanf(infile, "%lg", &src_amp[reading]);
		  fscanf(infile, "%lg", &src_off[reading]);
		  fscanf(infile, "%lg", &src_wait[reading]);
		  fscanf(infile, "%d", &src_comp[reading]);
		  fscanf(infile, "%d", &en[reading]);
		  fscanf(infile, "%d", &ed[reading]);
		  fscanf(infile, "%d", &rf[reading]);
		  fscanf(infile, "%d", &sa[reading]);
		  fscanf(infile, "%d", &sb[reading]);
		  fscanf(infile, "%d", &sl[reading]);
		  fscanf(infile, "%d", &wb[reading]);
		  fscanf(infile, "%d", &hfs_cap[reading]);
		  fscanf(infile, "%d", &diff_det[reading]);
		  fscanf(infile, "%lg", &moa_out[reading]);
		  fscanf(infile, "%lg", &det_wait[reading]);
		  fscanf(infile, "%d", &src_type[reading]);
		  fscanf(infile, "%d", &ser_mode[reading]);
		  fscanf(infile, "%d", &comp_type[reading]);
		  fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
		  getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon 

		//***********************Measuring compansation capacitance************************* 
		for (int j = 0; j < NUM_MEASUREMENTS; j++) //outer loop for each subtest
		{ 
		  vth_cfxt_unlock();
		  vth_mtmr_wait(mod_id, WAIT_FOR_DMM_CAPTURE);
		  flush_commands(mod_id);
		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 
//		  talonCard->closeRelay(side == SIDEA ? HTalon::KIA : HTalon::KISB, false, false);
//		  talonCard->closeRelay(side == SIDEA ? HTalon::KSA : HTalon::KSB, true, true);
		  close(mod_id, PIN_SLOT, tps[11].rut, tps[11].ref, talonCard);  // relay array 11 (COMP)
		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
          flush_commands(mod_id);

		  vth_rlc_setup(mod_id, src_freq[j], src_amp[j], src_off[j],
				src_wait[j], src_comp[j], en[j],
				ed[j], rf[j], sa[j], sb[j],
				sl[j], wb[j], hfs_cap[j], diff_det[j],
				moa_out[j], det_wait[j]);

		  vth_capacitor_meas(mod_id, ser_mode[j], dc_rej[j]);
		  vth_get_capacitor_result(mod_id);

		  if (get_result_nolog(mod_id, &result))
		  {   
			  compensation_reading[j] = result.UU.Real_result;	
		  }  
		  else
		  {   // No result present
			  display_prompt("No result in result log (Expected component value)",
				   true);
			  compensation_reading[j] = 0.0;
		  }
		}
		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);
		vth_cfxt_lock();
		vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
		flush_commands(mod_id);
		
		//***********************BEGIN MAIN LOOP of SUBTESTS**************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		{ // FOR READING (each SUBTEST)
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		 
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 	  

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

		  vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
				src_wait[p], src_comp[p], en[p],
				ed[p], rf[p], sa[p], sb[p],
				sl[p], wb[p], hfs_cap[p], diff_det[p],
				moa_out[p], det_wait[p]);

		  //vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);      
		  vth_capacitor_meas(mod_id, ser_mode[p], dc_rej[p]);
		  vth_get_capacitor_result(mod_id);

		  if (get_result_nolog(mod_id, &result))
			  capacitor_reading = result.UU.Real_result;
		  else
		  {
			  display_prompt("No result in result log (Expected component value)",
						   true);
			  capacitor_reading = LARGE_FORCE_FAIL;
		  }

		  result.UU.Real_result = capacitor_reading - compensation_reading[p];
		  result.UU.Real_result -= actual_value[p];
		  log_result(mod_id, result);
      
		  talonCard->openAllRelays(false,true);  //Initialize Talon
		  vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for "p" (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	
  
} /* t16563 */

/////////////////////////////////THE END//////////////////////////

/*
 *-----------------------------------------------------------------------------
 * Test:16541
 * Description: Talon standard components: low value capacitors without
 * compensation
 *
 * Fixture: Standard components
 * Resource(s):
 *
 * The i5000 cannot perform an accurate 4 wire ohm measurement (using only one side of a pin card) at 1024 hertz.   
 * A better test is performed at 128 Hz.   Widen the limits for subtest 2 only, to -8.0 uFarads (-8.0 E-6), 
 * so as to not affect the sequencing of the other subtests.
*--------------------------------------------------------------------------------
*/

/* PIN STANDARD LOW VALUE CAPACITORS WITHOUT COMPENSATION */
void t16541(int mod_id, int /* slot */)
{
//  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  3;
  static int const NUM_MEASUREMENTS = 21;
	int reading;
	

// --- 2 and 4 Wire Capacitance Paths on Talon

//Capacitor 150 uF - 4 wire (subtest 0 and 2)
static HTalon::RelayName const SA_Bus_C150uF[5]  = {HTalon::KAS07, HTalon::KAS08, HTalon::KSA, HTalon::KSAA, HTalon::KAA}; 
static HTalon::RelayName const IB_Bus_C150uF[5]  = {HTalon::KAI28, HTalon::KAI27, HTalon::KIA, HTalon::KIBA, HTalon::KBA}; 

//Capacitor 15 uF - 4 wire (subtest 1, and 3)
static HTalon::RelayName const SA_Bus_C15uF[5]   = {HTalon::KAS09, HTalon::KAS10, HTalon::KSA, HTalon::KSAA, HTalon::KAA}; 
static HTalon::RelayName const IB_Bus_C15uF[5]   = {HTalon::KAI30, HTalon::KAI29, HTalon::KIA, HTalon::KIBA, HTalon::KBA}; 

//Capacitor 1.5 uF - 4 wire (subtest 4, and 5)  2 wire (subtest 6)
static HTalon::RelayName const S_Bus_C1Pt5uF[2]  = {HTalon::KAS11,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C1Pt5uF[2]  = {HTalon::KAI32,                HTalon::KIA}; //to X3
static HTalon::RelayName const SA_Bus_C1Pt5uF[5] = {HTalon::KAS11, HTalon::KAS12, HTalon::KSA, HTalon::KSAA, HTalon::KAA}; 
static HTalon::RelayName const IB_Bus_C1Pt5uF[5] = {HTalon::KAI32, HTalon::KAI31, HTalon::KIA, HTalon::KIBA, HTalon::KBA}; 

//Capacitor 150 nF - 4 wire (subtest 7, and 9)  2 wire (subtest 8 and 10)
static HTalon::RelayName const S_Bus_C150nF[2]   = {HTalon::KAS13,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C150nF[2]   = {HTalon::KAI34,                HTalon::KIA}; //to X3
static HTalon::RelayName const SA_Bus_C150nF[5]  = {HTalon::KAS13, HTalon::KAS14, HTalon::KSA, HTalon::KSAA, HTalon::KAA}; 
static HTalon::RelayName const IB_Bus_C150nF[5]  = {HTalon::KAI34, HTalon::KAI33, HTalon::KIA, HTalon::KIBA, HTalon::KBA}; 

//Capacitor 15 nF  - 2 wire (subtest 11 and 12)
static HTalon::RelayName const S_Bus_C15nF[2]    = {HTalon::KAS45,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C15nF[2]    = {HTalon::KAI66,                HTalon::KIA}; //to X3

//Capacitor 1.5 nF - 2 wire (subtest 13, and 14) 
static HTalon::RelayName const S_Bus_C1Pt5nF[2]  = {HTalon::KAS47,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C1Pt5nF[2]  = {HTalon::KAI68,                HTalon::KIA}; //to X3

//Capacitor 150 pF - 2 wire (subtest 15, 16, and 18)
static HTalon::RelayName const S_Bus_C150pF[2]   = {HTalon::KAS49,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C150pF[2]   = {HTalon::KAI70,                HTalon::KIA}; //to X3

//Capacitor 15 pF - 2 wire (subtest 17 and 19)
static HTalon::RelayName const S_Bus_C15pF[2]    = {HTalon::KAS51,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C15pF[2]    = {HTalon::KAI72,                HTalon::KIA}; //to X3

//Capacitor 10 pF - 2 wire (subtest 20)
static HTalon::RelayName const S_Bus_C10pF[2]    = {HTalon::KAS53,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C10pF[2]    = {HTalon::KAI74,                HTalon::KIA}; //to X3

static unsigned int const n=21;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SA_Bus_C150uF),   R(IB_Bus_C150uF)),  //4W subtest 0  
	rlylist(R(SA_Bus_C15uF),    R(IB_Bus_C15uF)),   //4W subtest 1 
	rlylist(R(SA_Bus_C150uF),   R(IB_Bus_C150uF)),  //4W subtest 2 same as 0
	rlylist(R(SA_Bus_C15uF),    R(IB_Bus_C15uF)),   //4W subtest 3 same as 1
	rlylist(R(SA_Bus_C1Pt5uF),  R(IB_Bus_C1Pt5uF)), //4W subtest 4 
	rlylist(R(SA_Bus_C1Pt5uF),  R(IB_Bus_C1Pt5uF)), //4W subtest 5 same as 4
	rlylist(R(S_Bus_C1Pt5uF),   R(I_Bus_C1Pt5uF)),  //2W subtest 6
	rlylist(R(SA_Bus_C150nF),   R(IB_Bus_C150nF)),  //4W subtest 7  
	rlylist(R(S_Bus_C150nF),    R(I_Bus_C150nF)),   //2W subtest 8
	rlylist(R(SA_Bus_C150nF),   R(IB_Bus_C150nF)),  //4W subtest 9  same as 7
	rlylist(R(S_Bus_C150nF),    R(I_Bus_C150nF)),   //2W subtest 10 same as 8
	rlylist(R(S_Bus_C15nF),     R(I_Bus_C15nF)),    //2W subtest 11
	rlylist(R(S_Bus_C15nF),     R(I_Bus_C15nF)),    //2W subtest 12 same as 11
	rlylist(R(S_Bus_C1Pt5nF),   R(I_Bus_C1Pt5nF)),  //2W subtest 13
	rlylist(R(S_Bus_C1Pt5nF),   R(I_Bus_C1Pt5nF)),  //2W subtest 14 same as 13
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 15
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 16 same as 15
	rlylist(R(S_Bus_C15pF),     R(I_Bus_C15pF)),    //2W subtest 17
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 18 same as 15
	rlylist(R(S_Bus_C15pF),     R(I_Bus_C15pF)),    //2W subtest 19 same as 17
	rlylist(R(S_Bus_C10pF),     R(I_Bus_C10pF)),    //2W subtest 20
#undef R
};

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  int    src_freq[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double src_off[NUM_MEASUREMENTS];
  double src_wait[NUM_MEASUREMENTS];
  int    src_comp[NUM_MEASUREMENTS];
  int    en[NUM_MEASUREMENTS];
  int    ed[NUM_MEASUREMENTS];
  int    rf[NUM_MEASUREMENTS];
  int    sa[NUM_MEASUREMENTS];
  int    sb[NUM_MEASUREMENTS];
  int    sl[NUM_MEASUREMENTS];
  int    wb[NUM_MEASUREMENTS];
  int    hfs_cap[NUM_MEASUREMENTS];
  int    diff_det[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  int    src_type[NUM_MEASUREMENTS];
  int    ser_mode[NUM_MEASUREMENTS];
  int    comp_type[NUM_MEASUREMENTS];
  int    dc_rej[NUM_MEASUREMENTS];

  char buffer[81];

  test_result_type result;

  infile = NULL;

  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
  
	  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		display_prompt("Standard Capacitors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 3.  Test16541 will not be run!", true);
	  }  //End Display no pin card in pin_slot message


	  else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  display_prompt("FOUND TALON CARD",true);// Else After check for slot 2 Talon pin card
		  /* Else After check for slot 2 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.capacitor", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'std.capacitor' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile


		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		  fscanf(infile, "%lg", &nominal_value[reading]);
		  fscanf(infile, "%d", &src_freq[reading]);
		  fscanf(infile, "%lg", &src_amp[reading]);
		  fscanf(infile, "%lg", &src_off[reading]);
		  fscanf(infile, "%lg", &src_wait[reading]);
		  fscanf(infile, "%d", &src_comp[reading]);
		  fscanf(infile, "%d", &en[reading]);
		  fscanf(infile, "%d", &ed[reading]);
		  fscanf(infile, "%d", &rf[reading]);
		  fscanf(infile, "%d", &sa[reading]);
		  fscanf(infile, "%d", &sb[reading]);
		  fscanf(infile, "%d", &sl[reading]);
		  fscanf(infile, "%d", &wb[reading]);
		  fscanf(infile, "%d", &hfs_cap[reading]);
		  fscanf(infile, "%d", &diff_det[reading]);
		  fscanf(infile, "%lg", &moa_out[reading]);
		  fscanf(infile, "%lg", &det_wait[reading]);
		  fscanf(infile, "%d", &src_type[reading]);
		  fscanf(infile, "%d", &ser_mode[reading]);
		  fscanf(infile, "%d", &comp_type[reading]);
		  fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
		  getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon 

		//***********************BEGIN MAIN LOOP******************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		//NUM_MEASUREMENTS =20 but start p at 0
		{ // FOR READING (each SUBTEST)
      
		  //vth_mra_in_mod(mod_id);
		 
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  if (sa[p] == 1)  vth_aclose_rly(mod_id, K710); // X2 to AIM A Bus
		  if (sb[p] == 1)  vth_aclose_rly(mod_id, K704); // X4 to AIM B Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

		  vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
				src_wait[p], src_comp[p], en[p],
				ed[p], rf[p], sa[p], sb[p],
				sl[p], wb[p], hfs_cap[p], diff_det[p],
				moa_out[p], det_wait[p]);

		  //vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);      
		  vth_capacitor_meas(mod_id, ser_mode[p], dc_rej[p]);
		  vth_get_capacitor_result(mod_id);

		  if (get_result_nolog(mod_id, &result))
		  {  // apply the actual to the p to create a delta value 
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		  }  
		  else
			display_prompt("No result in result log (Expected component value)",
						   true);

			talonCard->openAllRelays(false,true);  //Initialize Talon
			vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for READING (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	
  
} /* t16541 */


//static int const RC_FXT = 3800;

/* INDUCTOR STANDARD COMPONENTS TESTS */
void t16542(int mod_id, int /*slot*/)
{ //Inductor Standard Components Tests
  //Dedicated to Slot 4, Fixture only utilizes Page A
//  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  4;
  static int const NUM_MEASUREMENTS = 13;

int reading;

// --- 2 & 4 Wire Measurement Paths on Talon
//Inductor 2 uHenry 4 wire (subtest 0)
//static HTalon::RelayName const S_Bus_L2uHenry[2]    = {HTalon::KAS05,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L2uHenry[3]    = {HTalon::KAS06, HTalon::KSAA,  HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L2uHenry[2]    = {HTalon::KAI26,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L2uhHenry[3]   = {HTalon::KAI25, HTalon::KIBA,  HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L2uHenry[5]   = {HTalon::KAS05, HTalon::KAS06, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L2uHenry[5]   = {HTalon::KAI26, HTalon::KAI25, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 20 uHenry 4 wire (subtest 1)
//static HTalon::RelayName const S_Bus_L20uHenry[2]   = {HTalon::KAS53,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L20uHenry[3]   = {HTalon::KAS54, HTalon::KSAA,  HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L20uHenry[2]   = {HTalon::KAI74,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L20uHenry[3]   = {HTalon::KAI73, HTalon::KIBA,  HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L20uHenry[5]  = {HTalon::KAS53, HTalon::KAS54, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L20uHenry[5]  = {HTalon::KAI74, HTalon::KAI73, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 154 uHenry 4 wire (subtest 2)
//static HTalon::RelayName const S_Bus_L154uHenry[2]  = {HTalon::KAS07,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L154uHenry[3]  = {HTalon::KAS08, HTalon::KSAA,  HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L154uHenry[2]  = {HTalon::KAI28,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L154uHenry[3]  = {HTalon::KAI27, HTalon::KIBA,  HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L154uHenry[5] = {HTalon::KAS07, HTalon::KAS08, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L154uHenry[5] = {HTalon::KAI28, HTalon::KAI27, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 1.54 mHenry 4 wire (subtest 3)
//static HTalon::RelayName const S_Bus_L1Pt54mHenry[2] = {HTalon::KAS09,               HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L1Pt54mHenry[3] = {HTalon::KAS10, HTalon::KSAA, HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L1Pt54mHenry[2] = {HTalon::KAI30,               HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L1Pt54mHenry[3] = {HTalon::KAI29, HTalon::KIBA, HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L1Pt54mHenry[5]= {HTalon::KAS09, HTalon::KAS10,HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L1Pt54mHenry[5]= {HTalon::KAI30, HTalon::KAI29,HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 15.4 mHenry 4 wire (subtest 4)
//static HTalon::RelayName const S_Bus_L15Pt4mHenry[2] = {HTalon::KAS11,               HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L15Pt4mHenry[3] = {HTalon::KAS12, HTalon::KSAA, HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L15Pt4mHenry[2] = {HTalon::KAI32,               HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L15Pt4mHenry[3] = {HTalon::KAI31, HTalon::KIBA, HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L15Pt4mHenry[5]= {HTalon::KAS11, HTalon::KAS12,HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L15Pt4mHenry[5]= {HTalon::KAI32, HTalon::KAI31,HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 154 mHenry 2 wire (subtest 5 and 6)
static HTalon::RelayName const S_Bus_L154mHenry[2] = {HTalon::KAS13,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L154mHenry[3] = {HTalon::KAS14, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L154mHenry[2] = {HTalon::KAI34,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L154mHenry[3] = {HTalon::KAI33, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L154mHenry[5]= {HTalon::KAS13, HTalon::KAS14, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L154mHenry[5]= {HTalon::KAI34, HTalon::KAI33, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 1.54 Henry 2 wire (subtest 7, 8, and 9)
static HTalon::RelayName const S_Bus_L1Pt54Henry[2] = {HTalon::KAS45,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L1Pt54Henry[3] = {HTalon::KAS46, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L1Pt54Henry[2] = {HTalon::KAI66,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L1Pt54Henry[3] = {HTalon::KAI65, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L1Pt54Henry[5]= {HTalon::KAS45, HTalon::KAS46, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L1Pt54Henry[5]= {HTalon::KAI66, HTalon::KAI65, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 15.4 Henry 2 wire (subtest 10)
static HTalon::RelayName const S_Bus_L15Pt4Henry[2] = {HTalon::KAS47,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L15Pt4Henry[3] = {HTalon::KAS48, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L15Pt4Henry[2] = {HTalon::KAI68,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L15Pt4Henry[3] = {HTalon::KAI67, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L15Pt4Henry[5]= {HTalon::KAS47, HTalon::KAS48, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L15Pt4Henry[5]= {HTalon::KAI68, HTalon::KAI67, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 100 Henry 2 wire (subtest 11 and 12)
static HTalon::RelayName const S_Bus_L100Henry[2] = {HTalon::KAS49,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L100Henry[3] = {HTalon::KAS50, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L100Henry[2] = {HTalon::KAI70,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L100Henry[3] = {HTalon::KAI69, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L100Henry[5]= {HTalon::KAS49, HTalon::KAS50, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L100Henry[5]= {HTalon::KAI70, HTalon::KAI69, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SA_Bus_L2uHenry),    R(IB_Bus_L2uHenry)),    //4W subtest 0 
	rlylist(R(SA_Bus_L20uHenry),   R(IB_Bus_L20uHenry)),   //4W subtest 1 
	rlylist(R(SA_Bus_L154uHenry),  R(IB_Bus_L154uHenry)),  //4W subtest 2 
	rlylist(R(SA_Bus_L1Pt54mHenry),R(IB_Bus_L1Pt54mHenry)),//4W subtest 3  
	rlylist(R(SA_Bus_L15Pt4mHenry),R(IB_Bus_L15Pt4mHenry)),//4W subtest 4 
	rlylist(R(S_Bus_L154mHenry),   R(I_Bus_L154mHenry)),   //2W subtest 5  
	rlylist(R(S_Bus_L154mHenry),   R(I_Bus_L154mHenry)),   //2W subtest 6  same as 5
	rlylist(R(S_Bus_L1Pt54Henry),  R(I_Bus_L1Pt54Henry)),  //2W subtest 7
	rlylist(R(S_Bus_L1Pt54Henry),  R(I_Bus_L1Pt54Henry)),  //2W subtest 8  same as 7
    rlylist(R(S_Bus_L1Pt54Henry),  R(I_Bus_L1Pt54Henry)),  //2W subtest 9  same as 7
	rlylist(R(S_Bus_L15Pt4Henry),  R(I_Bus_L15Pt4Henry)),  //2W subtest 10
	rlylist(R(S_Bus_L100Henry),    R(I_Bus_L100Henry)),    //2W subtest 11
	rlylist(R(S_Bus_L100Henry),    R(I_Bus_L100Henry)),    //2W subtest 12 same as 11	
#undef R
};

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double src_off[NUM_MEASUREMENTS];
  double src_wait[NUM_MEASUREMENTS];
  int src_comp[NUM_MEASUREMENTS];
  int en[NUM_MEASUREMENTS];
  int ed[NUM_MEASUREMENTS];
  int rf[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int wb[NUM_MEASUREMENTS];
  int hfs_cap[NUM_MEASUREMENTS];
  int diff_det[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  int src_type[NUM_MEASUREMENTS];
  int ser_mode[NUM_MEASUREMENTS];
  int comp_type[NUM_MEASUREMENTS];
  int dc_rej[NUM_MEASUREMENTS];
 
  test_result_type result;
  char buffer[81];
  infile = NULL;
  
  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);

  if (get_fixture_id(mod_id) != RC_FXT)
  { /* Bypass Loop */

	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		display_prompt("Standard Inductores Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 4.  Test 16242 will not be run!", true);
	  }  //End Display no pin card in pin_slot message

	  else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  display_prompt("FOUND TALON CARD",true);// Else After check for slot 2 Talon pin card
		  /* Else After check for slot 2 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.inductor_talon", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'std.inductor_talon' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile


		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		  fscanf(infile, "%lg", &nominal_value[reading]);
		  fscanf(infile, "%d", &src_freq[reading]);
		  fscanf(infile, "%lg", &src_amp[reading]);
		  fscanf(infile, "%lg", &src_off[reading]);
		  fscanf(infile, "%lg", &src_wait[reading]);
		  fscanf(infile, "%d", &src_comp[reading]);
		  fscanf(infile, "%d", &en[reading]);
		  fscanf(infile, "%d", &ed[reading]);
		  fscanf(infile, "%d", &rf[reading]);
		  fscanf(infile, "%d", &sa[reading]);
		  fscanf(infile, "%d", &sb[reading]);
		  fscanf(infile, "%d", &sl[reading]);
		  fscanf(infile, "%d", &wb[reading]);
		  fscanf(infile, "%d", &hfs_cap[reading]);
		  fscanf(infile, "%d", &diff_det[reading]);
		  fscanf(infile, "%lg", &moa_out[reading]);
		  fscanf(infile, "%lg", &det_wait[reading]);
		  fscanf(infile, "%d", &src_type[reading]);
		  fscanf(infile, "%d", &ser_mode[reading]);
		  fscanf(infile, "%d", &comp_type[reading]);
		  fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
		  getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon

		//***********************BEGIN MAIN LOOP******************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		//NUM_MEASUREMENTS =20 but start p at 0
		{ // FOR (each SUBTEST)
      
		  //vth_mra_in_mod(mod_id);

		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  if (sa[p] == 1)  vth_aclose_rly(mod_id, K710); // X2 to AIM A Bus
		  if (sb[p] == 1)  vth_aclose_rly(mod_id, K704); // X4 to AIM B Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

		  vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
				src_wait[p], src_comp[p], en[p],
				ed[p], rf[p], sa[p], sb[p],
				sl[p], wb[p], hfs_cap[p], diff_det[p],
				moa_out[p], det_wait[p]);

		  //vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);      
		  vth_inductor_meas(mod_id, ser_mode[p], dc_rej[p]);
		  vth_get_inductor_result(mod_id);

		  if (get_result_nolog(mod_id, &result))
		  {  // apply the actual to the p to create a delta value 
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		  }  
		  else
			display_prompt("No result in result log (Expected component value)",
						   true);

		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for READING (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	

} /* t16542 */

//-------------------------------------------------------------------------
/* RESISTOR STANDARD COMPONENTS DELTA NETWORKS */
void t16543(int mod_id, int /*slot*/)
{ //Dedicated to Slot 5, Utilizes PAGE A and PAGE B (second row) of Talon Pin Card
  static int const PIN_SLOT         = 5;
  static int const NUM_MEASUREMENTS = 2;
  int reading;
  
// Paths on Talon
// 10K - 10 - 10 -- 6W EN ED RF = 10K   (subtest 0)
//                                                      <----S BUS  Page A----->   <-----I Bus  Page A----->    <---- G Bus Page A ------>  
static HTalon::RelayName const SI_Bus_10KDelta[6]  = {HTalon::KAS26, HTalon::KSA, HTalon::KAI05, HTalon::KIA,   HTalon::KAG46, HTalon::KGA}; //to X1(S) X3(I) XG(G) on Page A
//                                                      <-----------A BUS  Page B ----------->      <------------B BUS   Page B----------->   <--------- L Bus  Page B ------------>   <-Pin 18 to L Bus->                     
static HTalon::RelayName const AB_Bus_10KDelta[10] = {HTalon::KBS25, HTalon::KSAB,HTalon::KAB,   HTalon::KBI06, HTalon::KIBB,  HTalon::KBB, HTalon::KBG45, HTalon::KGLB, HTalon::KLB, HTalon::KBI16}; //to X6(A) X8(B) XL(L) on Page B 

// 100K - 0.1 - 3K -- 6W EN ED WB RF=1M (subtest 1)
//			                                            <----S BUS  Page A----->   <-----I Bus  Page A----->    <---- G Bus Page A ------>  
static HTalon::RelayName const SI_Bus_100KDelta[6]  ={HTalon::KAS28, HTalon::KSA, HTalon::KAI07, HTalon::KIA,   HTalon::KAG48, HTalon::KGA}; //to X1(S) X3(I) XG(G) on Page A
//                                                      <-----------A BUS  Page B ----------->      <------------B BUS   Page B----------->   <--------- L Bus  Page B ------------>   <-Pin 18 to L Bus->                     
static HTalon::RelayName const AB_Bus_100KDelta[10] ={HTalon::KBS27, HTalon::KSAB,HTalon::KAB,   HTalon::KBI08, HTalon::KIBB,  HTalon::KBB, HTalon::KBG47, HTalon::KGLB, HTalon::KLB, HTalon::KBI18}; //to X6(A) X8(B) XL(L) on Page B 

static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SI_Bus_10KDelta),  R(AB_Bus_10KDelta)),  //6W subtest 0 
	rlylist(R(SI_Bus_100KDelta), R(AB_Bus_100KDelta)), //6W subtest 1 
#undef R
};

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double src_off[NUM_MEASUREMENTS];
  double src_wait[NUM_MEASUREMENTS];
  int src_comp[NUM_MEASUREMENTS];
  int en[NUM_MEASUREMENTS];
  int ed[NUM_MEASUREMENTS];
  int rf[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int wb[NUM_MEASUREMENTS];
  int hfs_cap[NUM_MEASUREMENTS];
  int diff_det[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  int src_type[NUM_MEASUREMENTS];
  int ser_mode[NUM_MEASUREMENTS];
  int comp_type[NUM_MEASUREMENTS];
  int dc_rej[NUM_MEASUREMENTS];

  test_result_type result;

  char buffer[81];

  infile = NULL;
  
  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);

  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */

    if (card_type != TDD10) // Verify Talon Card
    { //Display no pin card in pin_slot message*/
      display_prompt("Standard Resistor Deltas require a pin or channel", true);
      display_prompt("type card in Slot 5.  Test 16543 will not be run!", true);
    }  
    else // BEGIN MAIN TEST LOOP IF TALON CARD!
    { 
      present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
      load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

      sprintf(buffer, "%sstd.rdelta", testwhere_path);
	  infile = fopen(buffer, "r");

      if (infile == NULL)
      {
        display_prompt("The 'std.rdelta' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      // Read ASRU measurement setup parameters for each R Measurement
      for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
      {
        fscanf(infile, "%lg", &nominal_value[reading]);
        fscanf(infile, "%d", &src_freq[reading]);
        fscanf(infile, "%lg", &src_amp[reading]);
        fscanf(infile, "%lg", &src_off[reading]);
        fscanf(infile, "%lg", &src_wait[reading]);
        fscanf(infile, "%d", &src_comp[reading]);
        fscanf(infile, "%d", &en[reading]);
        fscanf(infile, "%d", &ed[reading]);
        fscanf(infile, "%d", &rf[reading]);
        fscanf(infile, "%d", &sa[reading]);
        fscanf(infile, "%d", &sb[reading]);
        fscanf(infile, "%d", &sl[reading]);
        fscanf(infile, "%d", &wb[reading]);
        fscanf(infile, "%d", &hfs_cap[reading]);
        fscanf(infile, "%d", &diff_det[reading]);
        fscanf(infile, "%lg", &moa_out[reading]);
        fscanf(infile, "%lg", &det_wait[reading]);
        fscanf(infile, "%d", &src_type[reading]);
        fscanf(infile, "%d", &ser_mode[reading]);
        fscanf(infile, "%d", &comp_type[reading]);
        fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
        getc(infile);
      }

      if (infile != NULL)
        fclose(infile);
      infile = NULL;

	  HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
	  vth_aopen_all_rlys(mod_id);
	  talonCard->openAllRelays(false,true);  //Initialize Talon

	  for (int p = 0; p < NUM_MEASUREMENTS; p++) //BEGIN MAIN LOOP for each subtest
      //NUM_MEASUREMENTS = 2 but start p at 0
      { 
		close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		vth_aclose_rly(mod_id, K752); // XG to AIM G Bus
		vth_aclose_rly(mod_id, K814); // G bus to ACOM
		vth_aclose_rly(mod_id, K714); // X6 to AIM A Bus
		vth_aclose_rly(mod_id, K708); // X8 to AIM B Bus	
		vth_aclose_rly(mod_id, K754); // XL to AIM L Bus
        vth_aclose_rly(mod_id, K753); // XL to AIM L Bus

        vth_aclose_rly(mod_id, K843); // MOANON to ACOM
		vth_aclose_rly(mod_id, K2303);// DetLO to ACOM
			  
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

        vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
                  src_wait[p], src_comp[p], en[p],
                  ed[p], rf[p], sa[p], sb[p],
                  sl[p], wb[p], hfs_cap[p], diff_det[p],
                  moa_out[p], det_wait[p]);

        vth_resistor_meas(mod_id, ser_mode[p], dc_rej[p]);
        vth_get_resistor_result(mod_id);

        if (get_result_nolog(mod_id, &result))
        {  /* apply the actual to the reading to create a delta value */
          result.UU.Real_result -= actual_value[p];
          log_result(mod_id, result);
        }  
        else
          display_prompt("No result in result log (Expected component value)",
                         true);

        talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);
      }  /* FOR p */
	}
  } /* Bypass Loop */

  if (infile != NULL)
    fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);

} /* t16543 */


/*
 *-----------------------------------------------------------------------------
 * Test:16544
 * Description: Talon standard components: capacitor standard components delta networks
 * compensation
 *
 * Fixture: Standard components
 * Resource(s): Dedicated to Slot 5, Fixture utilizes Page A AND Page B (second row) of Talon Pin card
 *
 * The standard components measurement should (but currently does NOT) subtract off the fixture capacitance.  
 * The test limits have been increased to +90 pf (from +60 pf) to account for a typical +30 pf worst case 
 * fixture capacitance.
 *
 * Future enhancement would be to measure the fixture capacitance in sales test (with the fixture pulled down) 
 * of unused mint pins J5(13), J5(14) to J5(33), J5(33) in slot 5 near the delta network components.  
 * This measured fixture capacitance value must be subtracted from the Delta C measured value.
*--------------------------------------------------------------------------------
*/

void t16544(int mod_id, int /*slot*/)
{ 
  static int const PIN_SLOT         = 5;
  static int const NUM_MEASUREMENTS = 1;
  int reading;

static HTalon::RelayName const SI_Bus_1nFDelta[6]  = {HTalon::KAS30, HTalon::KSA, HTalon::KAI09, HTalon::KIA,   HTalon::KAG50, HTalon::KGA}; //to X1(S) X3(I) XG(G) on Page A
static HTalon::RelayName const AB_Bus_1nFDelta[10] = {HTalon::KBS29, HTalon::KSAB,HTalon::KAB,   HTalon::KBI10, HTalon::KIBB,  HTalon::KBB, HTalon::KBG49, HTalon::KGLB, HTalon::KLB, HTalon::KBI16}; //to X6(A) X8(B) XL(L) on Page B 

static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SI_Bus_1nFDelta),  R(AB_Bus_1nFDelta))  //6W subtest 0	
#undef R
};

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double src_off[NUM_MEASUREMENTS];
  double src_wait[NUM_MEASUREMENTS];
  int src_comp[NUM_MEASUREMENTS];
  int en[NUM_MEASUREMENTS];
  int ed[NUM_MEASUREMENTS];
  int rf[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int wb[NUM_MEASUREMENTS];
  int hfs_cap[NUM_MEASUREMENTS];
  int diff_det[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  int src_type[NUM_MEASUREMENTS];
  int ser_mode[NUM_MEASUREMENTS];
  int comp_type[NUM_MEASUREMENTS];
  int dc_rej[NUM_MEASUREMENTS];

  test_result_type result;

  char buffer[81];

  infile = NULL;
  
  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);

  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */

	  if (card_type != TDD10) // Verify Talon Card
    { //Display no pin card in pin_slot message*/
      display_prompt("Standard Capacitor Deltas require a pin or channel", true);
      display_prompt("type card in Slot 5.  Test 16544 will not be run!", true);
    }  
    else // BEGIN MAIN TEST LOOP IF TALON CARD!
    { display_prompt("FOUND TALON CARD",true);// Else After check for slot 2 Talon pin card
      present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
      load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

      sprintf(buffer, "%sstd.cdelta", testwhere_path);
	  infile = fopen(buffer, "r");

      if (infile == NULL)
      {
        display_prompt("The 'std.cdelta' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      // Read ASRU measurement setup parameters for each R Measurement
      for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
      {
        fscanf(infile, "%lg", &nominal_value[reading]);
        fscanf(infile, "%d", &src_freq[reading]);
        fscanf(infile, "%lg", &src_amp[reading]);
        fscanf(infile, "%lg", &src_off[reading]);
        fscanf(infile, "%lg", &src_wait[reading]);
        fscanf(infile, "%d", &src_comp[reading]);
        fscanf(infile, "%d", &en[reading]);
        fscanf(infile, "%d", &ed[reading]);
        fscanf(infile, "%d", &rf[reading]);
        fscanf(infile, "%d", &sa[reading]);
        fscanf(infile, "%d", &sb[reading]);
        fscanf(infile, "%d", &sl[reading]);
        fscanf(infile, "%d", &wb[reading]);
        fscanf(infile, "%d", &hfs_cap[reading]);
        fscanf(infile, "%d", &diff_det[reading]);
        fscanf(infile, "%lg", &moa_out[reading]);
        fscanf(infile, "%lg", &det_wait[reading]);
        fscanf(infile, "%d", &src_type[reading]);
        fscanf(infile, "%d", &ser_mode[reading]);
        fscanf(infile, "%d", &comp_type[reading]);
        fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
        getc(infile);
      }

      if (infile != NULL)
        fclose(infile);
      infile = NULL;

	  HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
	  vth_aopen_all_rlys(mod_id);
	  talonCard->openAllRelays(false,true);  //Initialize Talon

	  for (int p = 0; p < NUM_MEASUREMENTS; p++) //BEGIN MAIN LOOP for each subtest
      //NUM_MEASUREMENTS = 2 but start p at 0
      { 
		close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		vth_aclose_rly(mod_id, K752); // XG to AIM G Bus
		vth_aclose_rly(mod_id, K814); // G bus to ACOM
		vth_aclose_rly(mod_id, K714); // X6 to AIM A Bus
		vth_aclose_rly(mod_id, K708); // X8 to AIM B Bus	
		vth_aclose_rly(mod_id, K754); // XL to AIM L Bus
        vth_aclose_rly(mod_id, K753); // XL to AIM L Bus

        vth_aclose_rly(mod_id, K843); // MOANON to ACOM
		vth_aclose_rly(mod_id, K2303);// DetLO to ACOM
			  
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

        vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
                  src_wait[p], src_comp[p], en[p],
                  ed[p], rf[p], sa[p], sb[p],
                  sl[p], wb[p], hfs_cap[p], diff_det[p],
                  moa_out[p], det_wait[p]);

        vth_capacitor_meas(mod_id, ser_mode[p], dc_rej[p]);
        vth_get_capacitor_result(mod_id);

        if (get_result_nolog(mod_id, &result))
        {  /* apply the actual to the reading to create a delta value */
          result.UU.Real_result -= actual_value[p];
          log_result(mod_id, result);
        }  
        else
          display_prompt("No result in result log (Expected component value)",
                         true);

       	talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id); 
      }  /* FOR p */
	}
   
  } /* Bypass Loop */

  if (infile != NULL)
    fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);

} /* t16544 */


//-------------------------------------------------------------------------
/* INDUCTOR STANDARD COMPONENTS DELTA NETWORKS */
void t16545(int mod_id, int /*slot*/)
{ //Dedicated to Slot 5, Fixture Utilizes Page A and Page B (second row) of Talon Pin card
  static int const PIN_SLOT         = 5;
  static int const NUM_MEASUREMENTS = 1;
  int reading;
  
// Paths on Talon
// 10mH +320Ohm - 10mH - 10mH -- 6W EN ED RF = 10K   (subtest 0)
static HTalon::RelayName const SI_Bus_10mHDelta[6]  = {HTalon::KAS32, HTalon::KSA, HTalon::KAI11, HTalon::KIA,   HTalon::KAG52, HTalon::KGA}; //to X1(S) X3(I) XG(G) on Page A
static HTalon::RelayName const AB_Bus_10mHDelta[10] = {HTalon::KBS31, HTalon::KSAB,HTalon::KAB,   HTalon::KBI12, HTalon::KIBB,  HTalon::KBB, HTalon::KBG51, HTalon::KGLB, HTalon::KLB, HTalon::KBI18}; //to X6(A) X8(B) XL(L) on Page B 
//                                                      <-----------A BUS  Page B ----------->      <------------B BUS   Page B----------->   <--------- L Bus  Page B ------------>   <-Pin 18 to L Bus->                     
static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SI_Bus_10mHDelta),  R(AB_Bus_10mHDelta)),  //6W subtest 0 
	
#undef R
};

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double src_off[NUM_MEASUREMENTS];
  double src_wait[NUM_MEASUREMENTS];
  int src_comp[NUM_MEASUREMENTS];
  int en[NUM_MEASUREMENTS];
  int ed[NUM_MEASUREMENTS];
  int rf[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int wb[NUM_MEASUREMENTS];
  int hfs_cap[NUM_MEASUREMENTS];
  int diff_det[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  int src_type[NUM_MEASUREMENTS];
  int ser_mode[NUM_MEASUREMENTS];
  int comp_type[NUM_MEASUREMENTS];
  int dc_rej[NUM_MEASUREMENTS];

  test_result_type result;

  char buffer[81];

  infile = NULL;
  
  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);

  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */

	if (card_type != TDD10) // Verify Talon Card
    { //Display no pin card in pin_slot message*/
      display_prompt("Standard Inductor Deltas require a pin or channel", true);
      display_prompt("type card in Slot 5.  Test 16545 will not be run!", true);
    }  
    else // BEGIN MAIN TEST LOOP IF TALON CARD!
    { display_prompt("FOUND TALON CARD",true);// Else After check for slot 2 Talon pin card
      present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
      load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

      sprintf(buffer, "%sstd.ldelta", testwhere_path);
	  infile = fopen(buffer, "r");

      if (infile == NULL)
      {
        display_prompt("The 'std.ldelta' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }

      // Read ASRU measurement setup parameters for each R Measurement
      for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
      {
        fscanf(infile, "%lg", &nominal_value[reading]);
        fscanf(infile, "%d", &src_freq[reading]);
        fscanf(infile, "%lg", &src_amp[reading]);
        fscanf(infile, "%lg", &src_off[reading]);
        fscanf(infile, "%lg", &src_wait[reading]);
        fscanf(infile, "%d", &src_comp[reading]);
        fscanf(infile, "%d", &en[reading]);
        fscanf(infile, "%d", &ed[reading]);
        fscanf(infile, "%d", &rf[reading]);
        fscanf(infile, "%d", &sa[reading]);
        fscanf(infile, "%d", &sb[reading]);
        fscanf(infile, "%d", &sl[reading]);
        fscanf(infile, "%d", &wb[reading]);
        fscanf(infile, "%d", &hfs_cap[reading]);
        fscanf(infile, "%d", &diff_det[reading]);
        fscanf(infile, "%lg", &moa_out[reading]);
        fscanf(infile, "%lg", &det_wait[reading]);
        fscanf(infile, "%d", &src_type[reading]);
        fscanf(infile, "%d", &ser_mode[reading]);
        fscanf(infile, "%d", &comp_type[reading]);
        fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
        getc(infile);
      }

      if (infile != NULL)
        fclose(infile);
      infile = NULL;

	  HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
	  vth_aopen_all_rlys(mod_id);
	  talonCard->openAllRelays(false,true);  //Initialize Talon

	  for (int p = 0; p < NUM_MEASUREMENTS; p++) //BEGIN MAIN LOOP for each subtest
      //NUM_MEASUREMENTS = 2 but start p at 0
      { 
		close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		vth_aclose_rly(mod_id, K752); // XG to AIM G Bus
		vth_aclose_rly(mod_id, K814); // G bus to ACOM
		vth_aclose_rly(mod_id, K714); // X6 to AIM A Bus
		vth_aclose_rly(mod_id, K708); // X8 to AIM B Bus	
		vth_aclose_rly(mod_id, K754); // XL to AIM L Bus
        vth_aclose_rly(mod_id, K753); // XL to AIM L Bus

        vth_aclose_rly(mod_id, K843); // MOANON to ACOM
		vth_aclose_rly(mod_id, K2303);// DetLO to ACOM
			  
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

        vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
                  src_wait[p], src_comp[p], en[p],
                  ed[p], rf[p], sa[p], sb[p],
                  sl[p], wb[p], hfs_cap[p], diff_det[p],
                  moa_out[p], det_wait[p]);

        vth_inductor_meas(mod_id, ser_mode[p], dc_rej[p]);
        vth_get_inductor_result(mod_id);

        if (get_result_nolog(mod_id, &result))
        {  /* apply the actual to the reading to create a delta value */
          result.UU.Real_result -= actual_value[p];
          log_result(mod_id, result);
        }  
        else
          display_prompt("No result in result log (Expected component value)",
                         true);

		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);
        
      }  /* FOR p */
	}
   
  } /* Bypass Loop */

  if (infile != NULL)
    fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);

} /* t16545 */


/*
*--------------------------------------------------------------------------------
* Test: 16546
* Description: Talon standard components: low value capacitors with compensation
*				
* Fixture: Standard components - Utilizes Page A (single density row of Pin card)	
* Resource(s): 
*--------------------------------------------------------------------------------
*/

void t16546(int mod_id, int /*slot*/)
{   // Test utilizes Pin card slot 3 Page A only
  static int const PIN_SLOT            =  3;
  static int const NUM_MEASUREMENTS    = 11;
  static double const LARGE_FORCE_FAIL = 1.0E20;
  int reading;
  double compensation_reading[11];
  double capacitor_reading;
  char buffer[81];
  test_result_type result;
  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double src_off[NUM_MEASUREMENTS];
  double src_wait[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  int    src_freq[NUM_MEASUREMENTS];
  int    src_comp[NUM_MEASUREMENTS];
  int    en[NUM_MEASUREMENTS];
  int    ed[NUM_MEASUREMENTS];
  int    rf[NUM_MEASUREMENTS];
  int    sa[NUM_MEASUREMENTS];
  int    sb[NUM_MEASUREMENTS];
  int    sl[NUM_MEASUREMENTS];
  int    wb[NUM_MEASUREMENTS];
  int    hfs_cap[NUM_MEASUREMENTS];
  int    diff_det[NUM_MEASUREMENTS];
  int    src_type[NUM_MEASUREMENTS];
  int    ser_mode[NUM_MEASUREMENTS];
  int    comp_type[NUM_MEASUREMENTS];
  int    dc_rej[NUM_MEASUREMENTS];
  
  // --- Compensation Paths on Talon
  static HTalon::RelayName const S_Bus_COMP[2]   = {HTalon::KAS15,  HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_COMP[2]   = {HTalon::KAI36,  HTalon::KIA}; //to X3
  
  // --- 2 Wire Capacitance Paths on Talon
  //NOTE FOR CLEAN UP : A and B Busses NOT USED for 2 WIRE measurements
  //Capacitor 150 nF - 2 wire (subtest 0)  
  static HTalon::RelayName const S_Bus_C150nF[2]   = {HTalon::KAS13, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C150nF[2]   = {HTalon::KAI34, HTalon::KIA}; //to X3
  
  //Capacitor 15 nF  - 2 wire (subtest 1 and 2)
  static HTalon::RelayName const S_Bus_C15nF[2]    = {HTalon::KAS45, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C15nF[2]    = {HTalon::KAI66, HTalon::KIA}; //to X3
  
  //Capacitor 1.5 nF - 2 wire (subtest 3, and 4) 
  static HTalon::RelayName const S_Bus_C1Pt5nF[2]  = {HTalon::KAS47, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C1Pt5nF[2]  = {HTalon::KAI68, HTalon::KIA}; //to X3
  
  //Capacitor 150 pF - 2 wire (subtest 5, 6, and 8)
  static HTalon::RelayName const S_Bus_C150pF[2]   = {HTalon::KAS49, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C150pF[2]   = {HTalon::KAI70, HTalon::KIA}; //to X3
  
  //Capacitor 15 pF - 2 wire (subtest 7 and 9)
  static HTalon::RelayName const S_Bus_C15pF[2]    = {HTalon::KAS51, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C15pF[2]    = {HTalon::KAI72, HTalon::KIA}; //to X3
  
  //Capacitor 10 pF - 2 wire (subtest 10)
  static HTalon::RelayName const S_Bus_C10pF[2]    = {HTalon::KAS53, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C10pF[2]    = {HTalon::KAI74, HTalon::KIA}; //to X3

  static unsigned int const n=NUM_MEASUREMENTS;
  static rlylist const tps[n+1] =  // add 1 to allow for Capacitance COMPENSATION setup
  {
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(S_Bus_C150nF),    R(I_Bus_C150nF)),   //2W subtest 0
	rlylist(R(S_Bus_C15nF),     R(I_Bus_C15nF)),    //2W subtest 1
	rlylist(R(S_Bus_C15nF),     R(I_Bus_C15nF)),    //2W subtest 2 same as 1
	rlylist(R(S_Bus_C1Pt5nF),   R(I_Bus_C1Pt5nF)),  //2W subtest 3
	rlylist(R(S_Bus_C1Pt5nF),   R(I_Bus_C1Pt5nF)),  //2W subtest 4 same as 3
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 5
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 6 same as 5
	rlylist(R(S_Bus_C15pF),     R(I_Bus_C15pF)),    //2W subtest 7
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 8 same as 5
	rlylist(R(S_Bus_C15pF),     R(I_Bus_C15pF)),    //2W subtest 9 same as 7
	rlylist(R(S_Bus_C10pF),     R(I_Bus_C10pF)),    //2W subtest 10
	rlylist(R(S_Bus_COMP),      R(I_Bus_COMP)),     //Path Capacitance COMP array #11
#undef R
}; // end static rlylist const

  FILE *infile;
  infile = NULL;
  std_component_array actual_value;
  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
	  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		 display_prompt("Standard Capacitors Test requires a Talon Pin Card", true);
		 display_prompt("type card in Slot 3.  Test 16546 will not be run!", true);
	  }  //End Display no pin card in pin_slot message
      else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  
		  /* Else After check for slot 3 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.compcap", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'std.compcap' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile

		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		  fscanf(infile, "%lg", &nominal_value[reading]);
		  fscanf(infile, "%d", &src_freq[reading]);
		  fscanf(infile, "%lg", &src_amp[reading]);
		  fscanf(infile, "%lg", &src_off[reading]);
		  fscanf(infile, "%lg", &src_wait[reading]);
		  fscanf(infile, "%d", &src_comp[reading]);
		  fscanf(infile, "%d", &en[reading]);
		  fscanf(infile, "%d", &ed[reading]);
		  fscanf(infile, "%d", &rf[reading]);
		  fscanf(infile, "%d", &sa[reading]);
		  fscanf(infile, "%d", &sb[reading]);
		  fscanf(infile, "%d", &sl[reading]);
		  fscanf(infile, "%d", &wb[reading]);
		  fscanf(infile, "%d", &hfs_cap[reading]);
		  fscanf(infile, "%d", &diff_det[reading]);
		  fscanf(infile, "%lg", &moa_out[reading]);
		  fscanf(infile, "%lg", &det_wait[reading]);
		  fscanf(infile, "%d", &src_type[reading]);
		  fscanf(infile, "%d", &ser_mode[reading]);
		  fscanf(infile, "%d", &comp_type[reading]);
		  fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
		  getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon 

		//***********************Measuring compansation capacitance************************* 
		for (int j = 0; j < NUM_MEASUREMENTS; j++) //outer loop for each subtest
		{ 
		  vth_cfxt_unlock();
		  vth_mtmr_wait(mod_id, WAIT_FOR_DMM_CAPTURE);
		  flush_commands(mod_id);
		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 
//		  talonCard->closeRelay(side == SIDEA ? HTalon::KIA : HTalon::KISB, false, false);
//		  talonCard->closeRelay(side == SIDEA ? HTalon::KSA : HTalon::KSB, true, true);
		  close(mod_id, PIN_SLOT, tps[11].rut, tps[11].ref, talonCard);  // relay array 11 (COMP)
		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
          flush_commands(mod_id);

		  vth_rlc_setup(mod_id, src_freq[j], src_amp[j], src_off[j],
				src_wait[j], src_comp[j], en[j],
				ed[j], rf[j], sa[j], sb[j],
				sl[j], wb[j], hfs_cap[j], diff_det[j],
				moa_out[j], det_wait[j]);

		  vth_capacitor_meas(mod_id, ser_mode[j], dc_rej[j]);
		  vth_get_capacitor_result(mod_id);

		  if (get_result_nolog(mod_id, &result))
		  {   
			  compensation_reading[j] = result.UU.Real_result;	
		  }  
		  else
		  {   // No result present
			  display_prompt("No result in result log (Expected component value)",
				   true);
			  compensation_reading[j] = 0.0;
		  }
		}
		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);
		vth_cfxt_lock();
		vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
		flush_commands(mod_id);
		
		//***********************BEGIN MAIN LOOP of SUBTESTS**************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		{ // FOR READING (each SUBTEST)
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		 
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 	  

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

		  vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
				src_wait[p], src_comp[p], en[p],
				ed[p], rf[p], sa[p], sb[p],
				sl[p], wb[p], hfs_cap[p], diff_det[p],
				moa_out[p], det_wait[p]);

		  //vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);      
		  vth_capacitor_meas(mod_id, ser_mode[p], dc_rej[p]);
		  vth_get_capacitor_result(mod_id);

		  if (get_result_nolog(mod_id, &result))
			  capacitor_reading = result.UU.Real_result;
		  else
		  {
			  display_prompt("No result in result log (Expected component value)",
						   true);
			  capacitor_reading = LARGE_FORCE_FAIL;
		  }

		  result.UU.Real_result = capacitor_reading - compensation_reading[p];
		  result.UU.Real_result -= actual_value[p];
		  log_result(mod_id, result);
      
		  talonCard->openAllRelays(false,true);  //Initialize Talon
		  vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for "p" (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	
  
} /* t16546 */

/*
*--------------------------------------------------------------------------------
* Test: 
* Description: Talon standard components: Standard Large Capacitors
*				
* Fixture: Standard components - Utilizes Page A AND Page B (second row) of Talon Pin Card	
* Resource(s): 
*--------------------------------------------------------------------------------
*/

/* PIN STANDARD LARGE CAPACITORS  */
void t16547(int mod_id, int /*slot*/)
{
//  static int const DEFAULT_FILTER = 1;
  static int const PIN_SLOT         = 5;
  static int const NUM_MEASUREMENTS = 3;
	int reading;

	//Capacitor 10 mF - 4 wire (subtest 0)             S                          I
	static HTalon::RelayName const SI_Bus_C10mF[4]  = {HTalon::KAS74, HTalon::KSA, HTalon::KAI53, HTalon::KIA};
	//                                                 A                                           B 
	static HTalon::RelayName const AB_Bus_C10mF[6]  = {HTalon::KBS73, HTalon::KSAB,HTalon::KAB,   HTalon::KBI54, HTalon::KIBB, HTalon::KBB};

	//Capacitor 1 mF  - 4 wire (subtest 1)             S                          I
	static HTalon::RelayName const SI_Bus_C1mF[4]   = {HTalon::KAS68, HTalon::KSA, HTalon::KAI47, HTalon::KIA};
	//                                                 A                                           B
	static HTalon::RelayName const AB_Bus_C1mF[6]   = {HTalon::KBS67, HTalon::KSAB,HTalon::KAB,   HTalon::KBI48, HTalon::KIBB, HTalon::KBB};
	
	//Capacitor 100 uF  - 4 wire (subtest 1)           S                          I
	static HTalon::RelayName const SI_Bus_C100uF[4] = {HTalon::KAS66, HTalon::KSA, HTalon::KAI45, HTalon::KIA};
	//                                                 A                                           B
	static HTalon::RelayName const AB_Bus_C100uF[6] = {HTalon::KBS65, HTalon::KSAB,HTalon::KAB,   HTalon::KBI46, HTalon::KIBB, HTalon::KBB};
	

static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SI_Bus_C10mF),    R(AB_Bus_C10mF)),   //2W subtest 0
	rlylist(R(SI_Bus_C1mF),     R(AB_Bus_C1mF)),    //2W subtest 1
	rlylist(R(SI_Bus_C100uF),   R(AB_Bus_C100uF)),  //2W subtest 2 
#undef R
}; // end static rlylist const

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  int    src_freq[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double src_off[NUM_MEASUREMENTS];
  double src_wait[NUM_MEASUREMENTS];
  int    src_comp[NUM_MEASUREMENTS];
  int    en[NUM_MEASUREMENTS];
  int    ed[NUM_MEASUREMENTS];
  int    rf[NUM_MEASUREMENTS];
  int    sa[NUM_MEASUREMENTS];
  int    sb[NUM_MEASUREMENTS];
  int    sl[NUM_MEASUREMENTS];
  int    wb[NUM_MEASUREMENTS];
  int    hfs_cap[NUM_MEASUREMENTS];
  int    diff_det[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  int    src_type[NUM_MEASUREMENTS];
  int    ser_mode[NUM_MEASUREMENTS];
  int    comp_type[NUM_MEASUREMENTS];
  int    dc_rej[NUM_MEASUREMENTS];
  
  char buffer[81];

  test_result_type result;

  infile = NULL;
   
  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
	  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		display_prompt("Standard Capacitors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 5.  Test 16547 will not be run!", true);
	  }  //End Display no pin card in pin_slot message


	  else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  display_prompt("FOUND TALON CARD",true);// Else After check for slot 3 Talon pin card
		  /* Else After check for slot 5 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.largecap", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'std.largecap' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile

		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		  fscanf(infile, "%lg", &nominal_value[reading]);
		  fscanf(infile, "%d", &src_freq[reading]);
		  fscanf(infile, "%lg", &src_amp[reading]);
		  fscanf(infile, "%lg", &src_off[reading]);
		  fscanf(infile, "%lg", &src_wait[reading]);
		  fscanf(infile, "%d", &src_comp[reading]);
		  fscanf(infile, "%d", &en[reading]);
		  fscanf(infile, "%d", &ed[reading]);
		  fscanf(infile, "%d", &rf[reading]);
		  fscanf(infile, "%d", &sa[reading]);
		  fscanf(infile, "%d", &sb[reading]);
		  fscanf(infile, "%d", &sl[reading]);
		  fscanf(infile, "%d", &wb[reading]);
		  fscanf(infile, "%d", &hfs_cap[reading]);
		  fscanf(infile, "%d", &diff_det[reading]);
		  fscanf(infile, "%lg", &moa_out[reading]);
		  fscanf(infile, "%lg", &det_wait[reading]);
		  fscanf(infile, "%d", &src_type[reading]);
		  fscanf(infile, "%d", &ser_mode[reading]);
		  fscanf(infile, "%d", &comp_type[reading]);
		  fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
		  getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon 

		//***********************BEGIN MAIN LOOP******************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		{ // FOR READING (each SUBTEST)
      
		  //vth_mra_in_mod(mod_id);
		 
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  if (sa[p] == 1)  vth_aclose_rly(mod_id, K714); // X6 to AIM A Bus
		  if (sb[p] == 1)  vth_aclose_rly(mod_id, K708); // X8 to AIM B Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

		  vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
				src_wait[p], src_comp[p], en[p],
				ed[p], rf[p], sa[p], sb[p],
				sl[p], wb[p], hfs_cap[p], diff_det[p],
				moa_out[p], det_wait[p]);

		  //vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);      
		  vth_capacitor_meas(mod_id, ser_mode[p], dc_rej[p]);
		  vth_get_capacitor_result(mod_id);

		  if (get_result_nolog(mod_id, &result))
		  {  // apply the actual to the p to create a delta value 
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		  }  
		  else
			display_prompt("No result in result log (Expected component value)",
						   true);

			talonCard->openAllRelays(false,true);  //Initialize Talon
			vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for "p" (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	
  
} /* t16547 */


/*
*--------------------------------------------------------------------------------
* Test: 16548
* Description: Talon standard components: Low Value Resistors 4 Wire Measurements 
*				
* Fixture: Standard components	- Fixture utilizes Page A and Page B (second row) of Talon Pin Card
* Resource(s): 
*--------------------------------------------------------------------------------
*/
void t16548(int mod_id, int slot)  // Page A and B to get true 4 Wire Ohm for .1, 1 and 10 ohms only
{
  static int const FILTER           = 10;
  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  2;
  static int const NUM_MEASUREMENTS =  6;  // (0-5 subtests)// different from number resistors - can have more than
                                           // one measurement for a given resistor
	double nominal_value[NUM_MEASUREMENTS];
	double src_amp[NUM_MEASUREMENTS];
	double src_off[NUM_MEASUREMENTS];
	double src_wait[NUM_MEASUREMENTS];
	double moa_out[NUM_MEASUREMENTS];
	double det_wait[NUM_MEASUREMENTS];
	int src_freq[NUM_MEASUREMENTS];
	int src_comp[NUM_MEASUREMENTS];
	int en[NUM_MEASUREMENTS];
	int ed[NUM_MEASUREMENTS];
	int rf[NUM_MEASUREMENTS];
	int sa[NUM_MEASUREMENTS];
	int sb[NUM_MEASUREMENTS];
	int sl[NUM_MEASUREMENTS];
	int wb[NUM_MEASUREMENTS];
	int hfs_cap[NUM_MEASUREMENTS];
	int diff_det[NUM_MEASUREMENTS];
	int src_type[NUM_MEASUREMENTS];
	int ser_mode[NUM_MEASUREMENTS];
	int comp_type[NUM_MEASUREMENTS];
	int dc_rej[NUM_MEASUREMENTS];
	int reading;

	int card_type;
	char buffer[81];
	
	test_result_type result;  
	std_component_array actual_value;
	FILE *infile;
	infile = NULL;
	
	static HTalon::RelayName const SI_01[4]    = {HTalon::KAS05, HTalon::KBS06, HTalon::KBI25, HTalon::KAI26};
	static HTalon::RelayName const SI_1[4]     = {HTalon::KAS07, HTalon::KBS08, HTalon::KBI27, HTalon::KAI28};
	static HTalon::RelayName const SI_10[4]    = {HTalon::KAS09, HTalon::KBS10, HTalon::KBI29, HTalon::KAI30};
	static HTalon::RelayName const SI_100_4[4] = {HTalon::KAS11, HTalon::KBS12, HTalon::KBI31, HTalon::KAI32};
	//static HTalon::RelayName const SI_100_2[2] = {HTalon::KAS11, HTalon::KAI32};
	//static HTalon::RelayName const SI_1K_4[4]  = {HTalon::KAS13, HTalon::KBS14, HTalon::KBI33, HTalon::KAI34};
	//static HTalon::RelayName const SI_1K_2[2]  = {HTalon::KAS13, HTalon::KAI33};
	//static HTalon::RelayName const SI_10K_4[4] = {HTalon::KAS45, HTalon::KBS46, HTalon::KBI66, HTalon::KAI65};
	//static HTalon::RelayName const SI_10K_2[2] = {HTalon::KAS45, HTalon::KAI66};
	//static HTalon::RelayName const SI_100K[2]  = {HTalon::KAS47, HTalon::KAI68};
	//static HTalon::RelayName const SI_1M[2]    = {HTalon::KAS49, HTalon::KAI70};
	//static HTalon::RelayName const SI_10M[2]   = {HTalon::KAS51, HTalon::KAI72};
	static HTalon::RelayName const SI_SUB_4[6] = {HTalon::KSA, HTalon::KSAB, HTalon::KAB, HTalon::KIA, HTalon::KIBB, HTalon::KBB};
	//static HTalon::RelayName const SI_SUB_2[2] = {HTalon::KSA, HTalon::KIA};

static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SI_01),    R(SI_SUB_4)),  //4W subtest 0 
	rlylist(R(SI_1),     R(SI_SUB_4)),	//4W subtest 1
	rlylist(R(SI_10),    R(SI_SUB_4)),  //4W subtest 2
	rlylist(R(SI_10),    R(SI_SUB_4)),  //4W subtest 3
	rlylist(R(SI_100_4), R(SI_SUB_4)),  //4W subtest 4
	rlylist(R(SI_100_4), R(SI_SUB_4)),  //4W subtest 5
	//rlylist(R(SI_100_2), R(SI_SUB_2)),  //2W subtest 6
	//rlylist(R(SI_1K_4),  R(SI_SUB_4)),  //4W subtest 7
    //rlylist(R(SI_1K_2),  R(SI_SUB_2)),  //2W subtest 8
	//rlylist(R(SI_1K_4),  R(SI_SUB_4)),  //4W subtest 9
    //rlylist(R(SI_1K_2),  R(SI_SUB_2)),  //2W subtest 10
	//rlylist(R(SI_10K_4), R(SI_SUB_4)),  //4W subtest 11
    //rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 12
	//rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 13
	//rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 14
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 15
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 16
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 17
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 18
	//rlylist(R(SI_1M),    R(SI_SUB_2)),  //2W subtest 19
	//rlylist(R(SI_1M),    R(SI_SUB_2)),  //2W subtest 20
	//rlylist(R(SI_10M),   R(SI_SUB_2)),  //2W subtest 21

#undef R
}; // end static rlylist const
	
  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */

	card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, PIN_SLOT);
	talonCard->openAllRelays(false,true);  //Initialize Talon
	vth_aopen_all_rlys(mod_id);
	if (card_type != TDD10) // Verify Talon Card
	{  
		display_prompt("Standard Resistors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 2.  Test 16548 will not be run!", true);
		_EscIO(FileNotFound);
	}  // end if card_type
	//present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
	load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);
	sprintf(buffer, "%sstd.resistor_talon", testwhere_path);  // loads CAL Lab standard values
	infile = fopen(buffer, "r");
	if (infile == NULL)
	{
		display_prompt("The 'std.resistor_talon' file was not found.  Please have sys admin replace.", true);
		_EscIO(FileNotFound);
	} // end if infile
	
	// Read ASRU measurement setup parameters for each R Measurement
	for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
	{
		fscanf(infile, "%lg", &nominal_value[reading]);
		fscanf(infile, "%d", &src_freq[reading]);
		fscanf(infile, "%lg", &src_amp[reading]);
		fscanf(infile, "%lg", &src_off[reading]);
		fscanf(infile, "%lg", &src_wait[reading]);
		fscanf(infile, "%d", &src_comp[reading]);
		fscanf(infile, "%d", &en[reading]);
		fscanf(infile, "%d", &ed[reading]);
		fscanf(infile, "%d", &rf[reading]);
		fscanf(infile, "%d", &sa[reading]);
		fscanf(infile, "%d", &sb[reading]);
		fscanf(infile, "%d", &sl[reading]);
		fscanf(infile, "%d", &wb[reading]);
		fscanf(infile, "%d", &hfs_cap[reading]);
		fscanf(infile, "%d", &diff_det[reading]);
		fscanf(infile, "%lg", &moa_out[reading]);
		fscanf(infile, "%lg", &det_wait[reading]);
		fscanf(infile, "%d", &src_type[reading]);
		fscanf(infile, "%d", &ser_mode[reading]);
		fscanf(infile, "%d", &comp_type[reading]);
		fscanf(infile, "%d%*[^\n]", &dc_rej[reading]);
		getc(infile);
	}  // end for reading
	if (infile != NULL)
		fclose(infile);
	infile = NULL;

	for (int p = 0; p < n; ++p) 
	{ /*for: subtest*/
		vth_mra_in_mod(mod_id);
	
		
		if (p == 0)   // 0.1 ohm resistor 
		{  
			vth_ameas_set_averaging(mod_id, FILTER);	// TURN FILTERING ON TO DECREASE THE DISTRIBUTION OF THE pS 
		}  
		else
		{  
			vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);	// USE THE DEFAULT FILTER VALUE 
		}  

		close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		if (sa[p] == 1)  vth_aclose_rly(mod_id, K714); // X6 to AIM A Bus Page B
	    if (sb[p] == 1)  vth_aclose_rly(mod_id, K708); // X8 to AIM B Bus Page B
		vth_aclose_rly(mod_id, K843);  // MOANON to ACOM 
		vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

		vth_rlc_setup(mod_id, src_freq[p], src_amp[p], src_off[p],
			src_wait[p], src_comp[p], en[p],
			ed[p], rf[p], sa[p], sb[p],
			sl[p], wb[p], hfs_cap[p], diff_det[p],
			moa_out[p], det_wait[p]);
		vth_resistor_meas(mod_id, ser_mode[p], dc_rej[p]);
		vth_get_resistor_result(mod_id);
		if (get_result_nolog(mod_id, &result))
		{  
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		}
		else
			display_prompt("No result in result log (Expected component value)", true);
		
		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);
		
	} /*for: subtest*/
  }

	if (infile != NULL)
		fclose(infile);	
	vth_pinit(mod_id, PIN_SLOT);
	vth_ainit(mod_id);
	
}  /*t16548*/



////added new test T16553 for NASRU MCU std comp resistor////
/*--------------------------------------------------------------------------------
* Test: 16553
* Description: Talon standard components: Low Value Resistors 4 Wire Measurements 
*				
* Fixture: Standard components	- Fixture utilizes Page A and Page B (second row) of Talon Pin Card
* Resource(s): 
*--------------------------------------------------------------------------------
*/
void t16553(int mod_id, int slot)  // Page A and B to get true 4 Wire Ohm for .1, 1 and 10 ohms only
{
  static int const FILTER           = 10;
  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  2;
  static int const NUM_MEASUREMENTS =  5;//6(0.1 OHM RESISTOR CANT BE TESTED BY MCU-NASRU;  // (0-5 subtests)// different from number resistors - can have more than
                                           // one measurement for a given resistor
  double nominal_value[NUM_MEASUREMENTS];  // The below are all modified to suite the MCU block measurement/sri
  int fb[NUM_MEASUREMENTS];
  int rc[NUM_MEASUREMENTS];
  int avr[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int extra[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double lo[NUM_MEASUREMENTS];
  double hi[NUM_MEASUREMENTS];
  int opp[NUM_MEASUREMENTS];
  int sm[NUM_MEASUREMENTS];
  int ac[NUM_MEASUREMENTS];
  int reading;

	int card_type;
	char buffer[81];
	
	test_result_type result;  
	std_component_array actual_value;
	FILE *infile;
	infile = NULL;
	
	//static HTalon::RelayName const SI_01[4]    = {HTalon::KAS05, HTalon::KBS06, HTalon::KBI25, HTalon::KAI26};
	static HTalon::RelayName const SI_1[4]     = {HTalon::KAS07, HTalon::KBS08, HTalon::KBI27, HTalon::KAI28};
	static HTalon::RelayName const SI_10[4]    = {HTalon::KAS09, HTalon::KBS10, HTalon::KBI29, HTalon::KAI30};
	static HTalon::RelayName const SI_100_4[4] = {HTalon::KAS11, HTalon::KBS12, HTalon::KBI31, HTalon::KAI32};
	//static HTalon::RelayName const SI_100_2[2] = {HTalon::KAS11, HTalon::KAI32};
	//static HTalon::RelayName const SI_1K_4[4]  = {HTalon::KAS13, HTalon::KBS14, HTalon::KBI33, HTalon::KAI34};
	//static HTalon::RelayName const SI_1K_2[2]  = {HTalon::KAS13, HTalon::KAI33};
	//static HTalon::RelayName const SI_10K_4[4] = {HTalon::KAS45, HTalon::KBS46, HTalon::KBI66, HTalon::KAI65};
	//static HTalon::RelayName const SI_10K_2[2] = {HTalon::KAS45, HTalon::KAI66};
	//static HTalon::RelayName const SI_100K[2]  = {HTalon::KAS47, HTalon::KAI68};
	//static HTalon::RelayName const SI_1M[2]    = {HTalon::KAS49, HTalon::KAI70};
	//static HTalon::RelayName const SI_10M[2]   = {HTalon::KAS51, HTalon::KAI72};
	static HTalon::RelayName const SI_SUB_4[6] = {HTalon::KSA, HTalon::KSAB, HTalon::KAB, HTalon::KIA, HTalon::KIBB, HTalon::KBB};
	//static HTalon::RelayName const SI_SUB_2[2] = {HTalon::KSA, HTalon::KIA};

static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	//rlylist(R(SI_01),    R(SI_SUB_4)),  //4W subtest 0 
	rlylist(R(SI_1),     R(SI_SUB_4)),	//4W subtest 1
	rlylist(R(SI_10),    R(SI_SUB_4)),  //4W subtest 2
	rlylist(R(SI_10),    R(SI_SUB_4)),  //4W subtest 3
	rlylist(R(SI_100_4), R(SI_SUB_4)),  //4W subtest 4
	rlylist(R(SI_100_4), R(SI_SUB_4)),  //4W subtest 5
	//rlylist(R(SI_100_2), R(SI_SUB_2)),  //2W subtest 6
	//rlylist(R(SI_1K_4),  R(SI_SUB_4)),  //4W subtest 7
    //rlylist(R(SI_1K_2),  R(SI_SUB_2)),  //2W subtest 8
	//rlylist(R(SI_1K_4),  R(SI_SUB_4)),  //4W subtest 9
    //rlylist(R(SI_1K_2),  R(SI_SUB_2)),  //2W subtest 10
	//rlylist(R(SI_10K_4), R(SI_SUB_4)),  //4W subtest 11
    //rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 12
	//rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 13
	//rlylist(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 14
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 15
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 16
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 17
	//rlylist(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 18
	//rlylist(R(SI_1M),    R(SI_SUB_2)),  //2W subtest 19
	//rlylist(R(SI_1M),    R(SI_SUB_2)),  //2W subtest 20
	//rlylist(R(SI_10M),   R(SI_SUB_2)),  //2W subtest 21

#undef R
}; // end static rlylist const
	
  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */

	card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, PIN_SLOT);
	talonCard->openAllRelays(false,true);  //Initialize Talon
	vth_aopen_all_rlys(mod_id);
	if (card_type != TDD10) // Verify Talon Card
	{  
		display_prompt("Standard Resistors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 2.  Test 16553 will not be run!", true);
		_EscIO(FileNotFound);
	}  // end if card_type
	//present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
	load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);
	sprintf(buffer, "%sstd.resistor_MCU_talon", testwhere_path);  // loads CAL Lab standard values
	infile = fopen(buffer, "r");
	if (infile == NULL)
	{
		display_prompt("The 'std.resistor_MCU_talon' file was not found.  Please have sys admin replace.", true);
		_EscIO(FileNotFound);
	} // end if infile
	
	// Read ASRU measurement setup parameters for each R Measurement
	for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
	{
	fscanf(infile, "%lg", &nominal_value[reading]);
		
	    fscanf(infile, "%d", &fb[reading]);
        fscanf(infile, "%d", &rc[reading]);
        fscanf(infile, "%d", &avr[reading]);
        fscanf(infile, "%d", &src_freq[reading]);
        fscanf(infile, "%d", &sa[reading]);
        fscanf(infile, "%d", &sb[reading]);
        fscanf(infile, "%d", &sl[reading]);
        fscanf(infile, "%d", &extra[reading]);
        fscanf(infile, "%lg", &src_amp[reading]);
        fscanf(infile, "%lg", &det_wait[reading]);
        fscanf(infile, "%lg", &moa_out[reading]);
        fscanf(infile, "%lg", &lo[reading]);
        fscanf(infile, "%lg", &hi[reading]);
        fscanf(infile, "%d", &opp[reading]);
        fscanf(infile, "%d", &sm[reading]);
		fscanf(infile, "%d", &ac[reading]);
		getc(infile);
	}  // end for reading
	if (infile != NULL)
		fclose(infile);
	infile = NULL;

	for (int p = 0; p < n; ++p) 
	{ /*for: subtest*/
		vth_mra_in_mod(mod_id);
	
		
		if (p == 0)   // 0.1 ohm resistor 
		{  
			vth_ameas_set_averaging(mod_id, FILTER);	// TURN FILTERING ON TO DECREASE THE DISTRIBUTION OF THE pS 
		}  
		else
		{  
			vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);	// USE THE DEFAULT FILTER VALUE 
		}  

		close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		if (sa[p] == 1)  vth_aclose_rly(mod_id, K714); // X6 to AIM A Bus Page B
	    if (sb[p] == 1)  vth_aclose_rly(mod_id, K708); // X8 to AIM B Bus Page B
		vth_aclose_rly(mod_id, K843);  // MOANON to ACOM 
		vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

        vth_set_dgn_use_mcu_flag(mod_id);
	    vth_mcu_rlc_measure(mod_id, fb[p], rc[p], avr[p], src_freq[p], sa[p], 
		  sb[p], sl[p],extra[p], src_amp[p], det_wait[p], moa_out[p], 
		  lo[p], hi[p], opp[p], sm[p], ac[p]);
		if (get_result_nolog(mod_id, &result))
		{  
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		}
		else
			display_prompt("No result in result log (Expected component value)", true);
		
		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);
		
	} /*for: subtest*/
  }

	if (infile != NULL)
		fclose(infile);	
	vth_pinit(mod_id, PIN_SLOT);
	vth_ainit(mod_id);
	
}  /*t16553*/


////////////////////////the end for 16553////////////////////

//###### STD. COMPONENET TEST FOR NASRU P3 USING MCU TEST OPTIONS#########
/**--------------------------------------------------------------------------------
* Test: 16550
* Description: Talon standard components: standard resistors 
* Notes:       Dedicated to Slot 2, Fixture only utilizes Page A
* Fixture: Standard Components	
* Resource(s): None
*
* Test 16550, subtests 0, 1, 2, 3, 4 and 5 were replaced by 16548 subtests 0, 1, 2, 3, 4,  and 5.   
* The new 16548 performs a true 4 wire measurement up through the mint pin to the Resistor under test.  
* Test 16550 does a 4 wire sense measurement only up to the relay field and does NOT account for the 
* trace resistance from the relay, through the mint pint up to the DUT - as is apparent from the data, 
* this additional path resistance adds about 150 milliohms to the component value on test 16550.  
* Test 15548, being a true 4 wire measurement, compensates for the path resistance.  
* You do NOT want to remove the subtests in 16550 - this would corrupt existing data analysis tools 
* for the remaining 16550 subtests.
*
* Test limits for 16550 are as follows so as to never fail a good PCA:
* Subtest 0:  for the .1 ohm --> limit +/- 2 ohms 
* Subtest 1:  for the  1 ohm --> limit +/- 2 ohms
* Subtest 2:  for the 10 ohm --> limit +/- 20 ohms
* Subtest 3:  for the 10 ohm --> limit +/- 20 ohms
* Subtest 4:  for the 100 ohm --> limit +/- 110 ohms
* Subtest 5:  for the 100 ohm --> limit +/- 110 ohms
*--------------------------------------------------------------------------------
*/
//static int const INDUCTOR_FXT = 3801;

struct rlylist1
{
  HTalon::Relays rut;
  HTalon::Relays ref;
  rlylist1(RLYLIST rut_b, RLYLIST rut_e, RLYLIST ref_b, RLYLIST ref_e)
	  : rut(rut_b, rut_e), ref(ref_b, ref_e) {}
}; // end struct rlylist

void t16550(int mod_id, int slot)
{
  static int const FILTER           = 10;
  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  2;
  static int const NUM_MEASUREMENTS = 21;//22{nasru MCU cant measure 0.1 ohm};  // 22 (0-21 subtests)// different from number resistors - can have more than
  char buffer[81];                                         // one measurement for a given resistor
  double nominal_value[NUM_MEASUREMENTS];  // The below are all modified to suite the MCU block measurement/sri
  int fb[NUM_MEASUREMENTS];
  int rc[NUM_MEASUREMENTS];
  int avr[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int extra[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double lo[NUM_MEASUREMENTS];
  double hi[NUM_MEASUREMENTS];
  int opp[NUM_MEASUREMENTS];
  int sm[NUM_MEASUREMENTS];
  int ac[NUM_MEASUREMENTS];
  int reading;	
	test_result_type result;  
	std_component_array actual_value;
	FILE *infile;
	infile = NULL;
	
	static HTalon::RelayName const SI_01[4]    = {HTalon::KAS05, HTalon::KAS06, HTalon::KAI25, HTalon::KAI26};
	static HTalon::RelayName const SI_1[4]     = {HTalon::KAS07, HTalon::KAS08, HTalon::KAI27, HTalon::KAI28};
	static HTalon::RelayName const SI_10[4]    = {HTalon::KAS09, HTalon::KAS10, HTalon::KAI29, HTalon::KAI30};
	static HTalon::RelayName const SI_100_4[4] = {HTalon::KAS11, HTalon::KAS12, HTalon::KAI31, HTalon::KAI32};
	static HTalon::RelayName const SI_100_2[2] = {HTalon::KAS11, HTalon::KAI32};
	static HTalon::RelayName const SI_1K_4[4]  = {HTalon::KAS13, HTalon::KAS14, HTalon::KAI33, HTalon::KAI34};
	static HTalon::RelayName const SI_1K_2[2]  = {HTalon::KAS13, HTalon::KAI33};
	static HTalon::RelayName const SI_10K_4[4] = {HTalon::KAS45, HTalon::KAS46, HTalon::KAI66, HTalon::KAI65};
	static HTalon::RelayName const SI_10K_2[2] = {HTalon::KAS45, HTalon::KAI66};
	static HTalon::RelayName const SI_100K[2]  = {HTalon::KAS47, HTalon::KAI68};
	static HTalon::RelayName const SI_1M[2]    = {HTalon::KAS49, HTalon::KAI70};
	static HTalon::RelayName const SI_10M[2]   = {HTalon::KAS51, HTalon::KAI72};
	static HTalon::RelayName const SI_SUB_4[6] = {HTalon::KSA, HTalon::KSAA, HTalon::KAA, HTalon::KIA, HTalon::KIBA, HTalon::KBA};
	static HTalon::RelayName const SI_SUB_2[2] = {HTalon::KSA, HTalon::KIA};

static unsigned int const n=21;//22;
static rlylist1 const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	//rlylist1(R(SI_01),    R(SI_SUB_4)),  //4W subtest 0 {NASRU MCU cant measure 0.1ohm}
	rlylist1(R(SI_1),     R(SI_SUB_4)),	//4W subtest 1
	rlylist1(R(SI_10),    R(SI_SUB_4)),  //4W subtest 2
	rlylist1(R(SI_10),    R(SI_SUB_4)),  //4W subtest 3
	rlylist1(R(SI_100_4), R(SI_SUB_4)),  //4W subtest 4
	rlylist1(R(SI_100_4), R(SI_SUB_4)),  //4W subtest 5
	rlylist1(R(SI_100_2), R(SI_SUB_2)),  //2W subtest 6
	rlylist1(R(SI_1K_4),  R(SI_SUB_4)),  //4W subtest 7
    rlylist1(R(SI_1K_2),  R(SI_SUB_2)),  //2W subtest 8
	rlylist1(R(SI_1K_4),  R(SI_SUB_4)),  //4W subtest 9
    rlylist1(R(SI_1K_2),  R(SI_SUB_2)),  //2W subtest 10
	rlylist1(R(SI_10K_4), R(SI_SUB_4)),  //4W subtest 11
    rlylist1(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 12
	rlylist1(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 13
	rlylist1(R(SI_10K_2), R(SI_SUB_2)),  //2W subtest 14
	rlylist1(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 15
	rlylist1(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 16
	rlylist1(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 17
	rlylist1(R(SI_100K),  R(SI_SUB_2)),  //2W subtest 18
	rlylist1(R(SI_1M),    R(SI_SUB_2)),  //2W subtest 19
	rlylist1(R(SI_1M),    R(SI_SUB_2)),  //2W subtest 20
	rlylist1(R(SI_10M),   R(SI_SUB_2)),  //2W subtest 21

#undef R
}; // end static rlylist1 const

  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
	
	int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, PIN_SLOT);
	talonCard->openAllRelays(false,true);  //Initialize Talon
	vth_aopen_all_rlys(mod_id);
	if (card_type != TDD10) // Verify Talon Card
	{  
		display_prompt("Standard Resistors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 2.  Test 16550 will not be run!", true);
		_EscIO(FileNotFound);
	}  // end if card_type
	//present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
	load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);
	sprintf(buffer, "%sstd.resistor_MCU_Talon_1", testwhere_path);  // loads CAL Lab standard values
	infile = fopen(buffer, "r");
	if (infile == NULL)
	{
		display_prompt("The 'std.resistor_MCU_Talon_1' file was not found.  Please have sys admin replace.", true);
		_EscIO(FileNotFound);
	} // end if infile
	
	// Read ASRU measurement setup parameters for each R Measurement
	for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
	{
		fscanf(infile, "%lg", &nominal_value[reading]);
		
	    fscanf(infile, "%d", &fb[reading]);
        fscanf(infile, "%d", &rc[reading]);
        fscanf(infile, "%d", &avr[reading]);
        fscanf(infile, "%d", &src_freq[reading]);
        fscanf(infile, "%d", &sa[reading]);
        fscanf(infile, "%d", &sb[reading]);
        fscanf(infile, "%d", &sl[reading]);
        fscanf(infile, "%d", &extra[reading]);
        fscanf(infile, "%lg", &src_amp[reading]);
        fscanf(infile, "%lg", &det_wait[reading]);
        fscanf(infile, "%lg", &moa_out[reading]);
        fscanf(infile, "%lg", &lo[reading]);
        fscanf(infile, "%lg", &hi[reading]);
        fscanf(infile, "%d", &opp[reading]);
        fscanf(infile, "%d", &sm[reading]);
		fscanf(infile, "%d", &ac[reading]);
		getc(infile);
	}  // end for reading
	if (infile != NULL)
		fclose(infile);
	infile = NULL;

	for (int p = 0; p < n; ++p) 
	{ /*for: subtest*/
		vth_mra_in_mod(mod_id);
	
		
		if (p == 0)   // 0.1 ohm resistor 
		{  
			vth_ameas_set_averaging(mod_id, FILTER);	// TURN FILTERING ON TO DECREASE THE DISTRIBUTION OF THE pS 
		}  
		else
		{  
			vth_ameas_set_averaging(mod_id, DEFAULT_FILTER);	// USE THE DEFAULT FILTER VALUE 
		}  

		close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		if (sa[p] == 1)  vth_aclose_rly(mod_id, K710); // X2 to AIM A Bus
	    if (sb[p] == 1)  vth_aclose_rly(mod_id, K704); // X4 to AIM B Bus
		vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

        vth_set_dgn_use_mcu_flag(mod_id);
	    vth_mcu_rlc_measure(mod_id, fb[p], rc[p], avr[p], src_freq[p], sa[p], 
		  sb[p], sl[p],extra[p], src_amp[p], det_wait[p], moa_out[p], 
		  lo[p], hi[p], opp[p], sm[p], ac[p]);
		
		if (get_result_nolog(mod_id, &result))
		{  
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		}
		else
			display_prompt("No result in result log (Expected component value)", true);
		
		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);
		
	} /*for: subtest*/
  }
	if (infile != NULL)
		fclose(infile);	
	vth_pinit(mod_id, PIN_SLOT);
	vth_ainit(mod_id);
	
}  /*t16550*/

/*
 *-----------------------------------------------------------------------------
 * Test:16551
 * Description: Talon standard components: low value capacitors without
 * compensation
 *
 * Fixture: Standard components
 * Resource(s):
 *
 * The i5000 cannot perform an accurate 4 wire ohm measurement (using only one side of a pin card) at 1024 hertz.   
 * A better test is performed at 128 Hz.   Widen the limits for subtest 2 only, to -8.0 uFarads (-8.0 E-6), 
 * so as to not affect the sequencing of the other subtests.
*--------------------------------------------------------------------------------
*/

/* PIN STANDARD LOW VALUE CAPACITORS WITHOUT COMPENSATION */
void t16551(int mod_id, int /* slot */)
{
//  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  3;
  static int const NUM_MEASUREMENTS = 21;
	int reading;
	

// --- 2 and 4 Wire Capacitance Paths on Talon

//Capacitor 150 uF - 4 wire (subtest 0 and 2)
static HTalon::RelayName const SA_Bus_C150uF[5]  = {HTalon::KAS07, HTalon::KAS08, HTalon::KSA, HTalon::KSAA, HTalon::KAA}; 
static HTalon::RelayName const IB_Bus_C150uF[5]  = {HTalon::KAI28, HTalon::KAI27, HTalon::KIA, HTalon::KIBA, HTalon::KBA}; 

//Capacitor 15 uF - 4 wire (subtest 1, and 3)
static HTalon::RelayName const SA_Bus_C15uF[5]   = {HTalon::KAS09, HTalon::KAS10, HTalon::KSA, HTalon::KSAA, HTalon::KAA}; 
static HTalon::RelayName const IB_Bus_C15uF[5]   = {HTalon::KAI30, HTalon::KAI29, HTalon::KIA, HTalon::KIBA, HTalon::KBA}; 

//Capacitor 1.5 uF - 4 wire (subtest 4, and 5)  2 wire (subtest 6)
static HTalon::RelayName const S_Bus_C1Pt5uF[2]  = {HTalon::KAS11,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C1Pt5uF[2]  = {HTalon::KAI32,                HTalon::KIA}; //to X3
static HTalon::RelayName const SA_Bus_C1Pt5uF[5] = {HTalon::KAS11, HTalon::KAS12, HTalon::KSA, HTalon::KSAA, HTalon::KAA}; 
static HTalon::RelayName const IB_Bus_C1Pt5uF[5] = {HTalon::KAI32, HTalon::KAI31, HTalon::KIA, HTalon::KIBA, HTalon::KBA}; 

//Capacitor 150 nF - 4 wire (subtest 7, and 9)  2 wire (subtest 8 and 10)
static HTalon::RelayName const S_Bus_C150nF[2]   = {HTalon::KAS13,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C150nF[2]   = {HTalon::KAI34,                HTalon::KIA}; //to X3
static HTalon::RelayName const SA_Bus_C150nF[5]  = {HTalon::KAS13, HTalon::KAS14, HTalon::KSA, HTalon::KSAA, HTalon::KAA}; 
static HTalon::RelayName const IB_Bus_C150nF[5]  = {HTalon::KAI34, HTalon::KAI33, HTalon::KIA, HTalon::KIBA, HTalon::KBA}; 

//Capacitor 15 nF  - 2 wire (subtest 11 and 12)
static HTalon::RelayName const S_Bus_C15nF[2]    = {HTalon::KAS45,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C15nF[2]    = {HTalon::KAI66,                HTalon::KIA}; //to X3

//Capacitor 1.5 nF - 2 wire (subtest 13, and 14) 
static HTalon::RelayName const S_Bus_C1Pt5nF[2]  = {HTalon::KAS47,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C1Pt5nF[2]  = {HTalon::KAI68,                HTalon::KIA}; //to X3

//Capacitor 150 pF - 2 wire (subtest 15, 16, and 18)
static HTalon::RelayName const S_Bus_C150pF[2]   = {HTalon::KAS49,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C150pF[2]   = {HTalon::KAI70,                HTalon::KIA}; //to X3

//Capacitor 15 pF - 2 wire (subtest 17 and 19)
static HTalon::RelayName const S_Bus_C15pF[2]    = {HTalon::KAS51,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C15pF[2]    = {HTalon::KAI72,                HTalon::KIA}; //to X3

//Capacitor 10 pF - 2 wire (subtest 20)
static HTalon::RelayName const S_Bus_C10pF[2]    = {HTalon::KAS53,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C10pF[2]    = {HTalon::KAI74,                HTalon::KIA}; //to X3

static unsigned int const n=21;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SA_Bus_C150uF),   R(IB_Bus_C150uF)),  //4W subtest 0  
	rlylist(R(SA_Bus_C15uF),    R(IB_Bus_C15uF)),   //4W subtest 1 
	rlylist(R(SA_Bus_C150uF),   R(IB_Bus_C150uF)),  //4W subtest 2 same as 0
	rlylist(R(SA_Bus_C15uF),    R(IB_Bus_C15uF)),   //4W subtest 3 same as 1
	rlylist(R(SA_Bus_C1Pt5uF),  R(IB_Bus_C1Pt5uF)), //4W subtest 4 
	rlylist(R(SA_Bus_C1Pt5uF),  R(IB_Bus_C1Pt5uF)), //4W subtest 5 same as 4
	rlylist(R(S_Bus_C1Pt5uF),   R(I_Bus_C1Pt5uF)),  //2W subtest 6
	rlylist(R(SA_Bus_C150nF),   R(IB_Bus_C150nF)),  //4W subtest 7  
	rlylist(R(S_Bus_C150nF),    R(I_Bus_C150nF)),   //2W subtest 8
	rlylist(R(SA_Bus_C150nF),   R(IB_Bus_C150nF)),  //4W subtest 9  same as 7
	rlylist(R(S_Bus_C150nF),    R(I_Bus_C150nF)),   //2W subtest 10 same as 8
	rlylist(R(S_Bus_C15nF),     R(I_Bus_C15nF)),    //2W subtest 11
	rlylist(R(S_Bus_C15nF),     R(I_Bus_C15nF)),    //2W subtest 12 same as 11
	rlylist(R(S_Bus_C1Pt5nF),   R(I_Bus_C1Pt5nF)),  //2W subtest 13
	rlylist(R(S_Bus_C1Pt5nF),   R(I_Bus_C1Pt5nF)),  //2W subtest 14 same as 13
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 15
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 16 same as 15
	rlylist(R(S_Bus_C15pF),     R(I_Bus_C15pF)),    //2W subtest 17
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 18 same as 15
	rlylist(R(S_Bus_C15pF),     R(I_Bus_C15pF)),    //2W subtest 19 same as 17
	rlylist(R(S_Bus_C10pF),     R(I_Bus_C10pF)),    //2W subtest 20
#undef R
};

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  
  int fb[NUM_MEASUREMENTS];
  int rc[NUM_MEASUREMENTS];
  int avr[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int extra[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double lo[NUM_MEASUREMENTS];
  double hi[NUM_MEASUREMENTS];
  int opp[NUM_MEASUREMENTS];
  int sm[NUM_MEASUREMENTS];
  int ac[NUM_MEASUREMENTS];
  //int ssvirt_card[NUM_MEASUREMENTS];

  char buffer[81];

  test_result_type result;

  infile = NULL;

  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
  
	  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		display_prompt("Standard Capacitors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 3.  Test16551 will not be run!", true);
	  }  //End Display no pin card in pin_slot message


	  else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  display_prompt("FOUND TALON CARD",true);// Else After check for slot 2 Talon pin card
		  /* Else After check for slot 2 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.capacitor_MCU_talon", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'std.capacitor_MCU_talon' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile


		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		  fscanf(infile, "%lg", &nominal_value[reading]);
		fscanf(infile, "%d", &fb[reading]);
        fscanf(infile, "%d", &rc[reading]);
        fscanf(infile, "%d", &avr[reading]);
        fscanf(infile, "%d", &src_freq[reading]);
        fscanf(infile, "%d", &sa[reading]);
        fscanf(infile, "%d", &sb[reading]);
        fscanf(infile, "%d", &sl[reading]);
        fscanf(infile, "%d", &extra[reading]);
        fscanf(infile, "%lg", &src_amp[reading]);
        fscanf(infile, "%lg", &det_wait[reading]);
        fscanf(infile, "%lg", &moa_out[reading]);
        fscanf(infile, "%lg", &lo[reading]);
        fscanf(infile, "%lg", &hi[reading]);
        fscanf(infile, "%d", &opp[reading]);
        fscanf(infile, "%d", &sm[reading]);
        fscanf(infile, "%d%*[^\n]", &ac[reading]);
		getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon 

		//***********************BEGIN MAIN LOOP******************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		//NUM_MEASUREMENTS =20 but start p at 0
		{ // FOR READING (each SUBTEST)
      
		  //vth_mra_in_mod(mod_id);
		 
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  if (sa[p] == 1)  vth_aclose_rly(mod_id, K710); // X2 to AIM A Bus
		  if (sb[p] == 1)  vth_aclose_rly(mod_id, K704); // X4 to AIM B Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
          
		  vth_set_dgn_use_mcu_flag(mod_id);
		  vth_mcu_rlc_measure(mod_id, fb[p], rc[p], avr[p], src_freq[p], sa[p], 
		  sb[p], sl[p],extra[p], src_amp[p], det_wait[p], moa_out[p], 
		  lo[p], hi[p], opp[p], sm[p], ac[p]);
		  
		  if (get_result_nolog(mod_id, &result))
		  {  // apply the actual to the p to create a delta value 
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		  }  
		  else
			display_prompt("No result in result log (Expected component value)",
						   true);

			talonCard->openAllRelays(false,true);  //Initialize Talon
			vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for READING (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	
  
} /* t16551 */


//static int const RC_FXT = 3800;

/* INDUCTOR STANDARD COMPONENTS TESTS */
void t16552(int mod_id, int /*slot*/)
{ //Inductor Standard Components Tests
  //Dedicated to Slot 4, Fixture only utilizes Page A
//  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  4;
  static int const NUM_MEASUREMENTS = 12;//13; 2u can't be tested by MCU

int reading;

// --- 2 & 4 Wire Measurement Paths on Talon
//Inductor 2 uHenry 4 wire (subtest 0)
//static HTalon::RelayName const S_Bus_L2uHenry[2]    = {HTalon::KAS05,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L2uHenry[3]    = {HTalon::KAS06, HTalon::KSAA,  HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L2uHenry[2]    = {HTalon::KAI26,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L2uhHenry[3]   = {HTalon::KAI25, HTalon::KIBA,  HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L2uHenry[5]   = {HTalon::KAS05, HTalon::KAS06, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L2uHenry[5]   = {HTalon::KAI26, HTalon::KAI25, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 20 uHenry 4 wire (subtest 1)
//static HTalon::RelayName const S_Bus_L20uHenry[2]   = {HTalon::KAS53,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L20uHenry[3]   = {HTalon::KAS54, HTalon::KSAA,  HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L20uHenry[2]   = {HTalon::KAI74,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L20uHenry[3]   = {HTalon::KAI73, HTalon::KIBA,  HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L20uHenry[5]  = {HTalon::KAS53, HTalon::KAS54, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L20uHenry[5]  = {HTalon::KAI74, HTalon::KAI73, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 154 uHenry 4 wire (subtest 2)
//static HTalon::RelayName const S_Bus_L154uHenry[2]  = {HTalon::KAS07,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L154uHenry[3]  = {HTalon::KAS08, HTalon::KSAA,  HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L154uHenry[2]  = {HTalon::KAI28,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L154uHenry[3]  = {HTalon::KAI27, HTalon::KIBA,  HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L154uHenry[5] = {HTalon::KAS07, HTalon::KAS08, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L154uHenry[5] = {HTalon::KAI28, HTalon::KAI27, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 1.54 mHenry 4 wire (subtest 3)
//static HTalon::RelayName const S_Bus_L1Pt54mHenry[2] = {HTalon::KAS09,               HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L1Pt54mHenry[3] = {HTalon::KAS10, HTalon::KSAA, HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L1Pt54mHenry[2] = {HTalon::KAI30,               HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L1Pt54mHenry[3] = {HTalon::KAI29, HTalon::KIBA, HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L1Pt54mHenry[5]= {HTalon::KAS09, HTalon::KAS10,HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L1Pt54mHenry[5]= {HTalon::KAI30, HTalon::KAI29,HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 15.4 mHenry 4 wire (subtest 4)
//static HTalon::RelayName const S_Bus_L15Pt4mHenry[2] = {HTalon::KAS11,               HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L15Pt4mHenry[3] = {HTalon::KAS12, HTalon::KSAA, HTalon::KAA}; //to X2
//static HTalon::RelayName const I_Bus_L15Pt4mHenry[2] = {HTalon::KAI32,               HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L15Pt4mHenry[3] = {HTalon::KAI31, HTalon::KIBA, HTalon::KBA}; //to X4
static HTalon::RelayName const SA_Bus_L15Pt4mHenry[5]= {HTalon::KAS11, HTalon::KAS12,HTalon::KSAA, HTalon::KSA, HTalon::KAA};
static HTalon::RelayName const IB_Bus_L15Pt4mHenry[5]= {HTalon::KAI32, HTalon::KAI31,HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 154 mHenry 2 wire (subtest 5 and 6)
static HTalon::RelayName const S_Bus_L154mHenry[2] = {HTalon::KAS13,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L154mHenry[3] = {HTalon::KAS14, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L154mHenry[2] = {HTalon::KAI34,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L154mHenry[3] = {HTalon::KAI33, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L154mHenry[5]= {HTalon::KAS13, HTalon::KAS14, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L154mHenry[5]= {HTalon::KAI34, HTalon::KAI33, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 1.54 Henry 2 wire (subtest 7, 8, and 9)
static HTalon::RelayName const S_Bus_L1Pt54Henry[2] = {HTalon::KAS45,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L1Pt54Henry[3] = {HTalon::KAS46, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L1Pt54Henry[2] = {HTalon::KAI66,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L1Pt54Henry[3] = {HTalon::KAI65, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L1Pt54Henry[5]= {HTalon::KAS45, HTalon::KAS46, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L1Pt54Henry[5]= {HTalon::KAI66, HTalon::KAI65, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 15.4 Henry 2 wire (subtest 10)
static HTalon::RelayName const S_Bus_L15Pt4Henry[2] = {HTalon::KAS47,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L15Pt4Henry[3] = {HTalon::KAS48, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L15Pt4Henry[2] = {HTalon::KAI68,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L15Pt4Henry[3] = {HTalon::KAI67, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L15Pt4Henry[5]= {HTalon::KAS47, HTalon::KAS48, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L15Pt4Henry[5]= {HTalon::KAI68, HTalon::KAI67, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
//Inductor 100 Henry 2 wire (subtest 11 and 12)
static HTalon::RelayName const S_Bus_L100Henry[2] = {HTalon::KAS49,                HTalon::KSA}; //to X1
//static HTalon::RelayName const A_Bus_L100Henry[3] = {HTalon::KAS50, HTalon::KSAA,  HTalon::KAA}; //to X2
static HTalon::RelayName const I_Bus_L100Henry[2] = {HTalon::KAI70,                HTalon::KIA}; //to X3
//static HTalon::RelayName const B_Bus_L100Henry[3] = {HTalon::KAI69, HTalon::KIBA,  HTalon::KBA}; //to X4
//static HTalon::RelayName const SA_Bus_L100Henry[5]= {HTalon::KAS49, HTalon::KAS50, HTalon::KSAA, HTalon::KSA, HTalon::KAA};
//static HTalon::RelayName const IB_Bus_L100Henry[5]= {HTalon::KAI70, HTalon::KAI69, HTalon::KIBA, HTalon::KIA, HTalon::KBA};
//
static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	//rlylist(R(SA_Bus_L2uHenry),    R(IB_Bus_L2uHenry)),    //4W subtest 0 //NOT TESTED BY NASRU MCU
	rlylist(R(SA_Bus_L20uHenry),   R(IB_Bus_L20uHenry)),   //4W subtest 1 
	rlylist(R(SA_Bus_L154uHenry),  R(IB_Bus_L154uHenry)),  //4W subtest 2 
	rlylist(R(SA_Bus_L1Pt54mHenry),R(IB_Bus_L1Pt54mHenry)),//4W subtest 3  
	rlylist(R(SA_Bus_L15Pt4mHenry),R(IB_Bus_L15Pt4mHenry)),//4W subtest 4 
	rlylist(R(S_Bus_L154mHenry),   R(I_Bus_L154mHenry)),   //2W subtest 5  
	rlylist(R(S_Bus_L154mHenry),   R(I_Bus_L154mHenry)),   //2W subtest 6  same as 5
	rlylist(R(S_Bus_L1Pt54Henry),  R(I_Bus_L1Pt54Henry)),  //2W subtest 7
	rlylist(R(S_Bus_L1Pt54Henry),  R(I_Bus_L1Pt54Henry)),  //2W subtest 8  same as 7
    rlylist(R(S_Bus_L1Pt54Henry),  R(I_Bus_L1Pt54Henry)),  //2W subtest 9  same as 7
	rlylist(R(S_Bus_L15Pt4Henry),  R(I_Bus_L15Pt4Henry)),  //2W subtest 10
	rlylist(R(S_Bus_L100Henry),    R(I_Bus_L100Henry)),    //2W subtest 11
	rlylist(R(S_Bus_L100Henry),    R(I_Bus_L100Henry)),    //2W subtest 12 same as 11	
#undef R
};

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  int fb[NUM_MEASUREMENTS];
  int rc[NUM_MEASUREMENTS];
  int avr[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int extra[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double lo[NUM_MEASUREMENTS];
  double hi[NUM_MEASUREMENTS];
  int opp[NUM_MEASUREMENTS];
  int sm[NUM_MEASUREMENTS];
  int ac[NUM_MEASUREMENTS];
  //int ssvirt_card[NUM_MEASUREMENTS];
  
  test_result_type result;
  char buffer[81];
  infile = NULL;
  
  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);

  if (get_fixture_id(mod_id) != RC_FXT)
  { /* Bypass Loop */

	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		display_prompt("Standard Inductores Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 4.  Test 16242 will not be run!", true);
	  }  //End Display no pin card in pin_slot message

	  else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  display_prompt("FOUND TALON CARD",true);// Else After check for slot 2 Talon pin card
		  /* Else After check for slot 2 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.inductor_MCU_talon", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'std.inductor_MCU_talon' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile


		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		    fscanf(infile, "%lg", &nominal_value[reading]);
		    fscanf(infile, "%d", &fb[reading]);
			fscanf(infile, "%d", &rc[reading]);
			fscanf(infile, "%d", &avr[reading]);
			fscanf(infile, "%d", &src_freq[reading]);
			fscanf(infile, "%d", &sa[reading]);
			fscanf(infile, "%d", &sb[reading]);
			fscanf(infile, "%d", &sl[reading]);
			fscanf(infile, "%d", &extra[reading]);
			fscanf(infile, "%lg", &src_amp[reading]);
			fscanf(infile, "%lg", &det_wait[reading]);
			fscanf(infile, "%lg", &moa_out[reading]);
			fscanf(infile, "%lg", &lo[reading]);
			fscanf(infile, "%lg", &hi[reading]);
			fscanf(infile, "%d", &opp[reading]);
			fscanf(infile, "%d", &sm[reading]);
			fscanf(infile, "%d%*[^\n]", &ac[reading]);
		
			 
		  getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon

		//***********************BEGIN MAIN LOOP******************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		//NUM_MEASUREMENTS =20 but start p at 0
		{ // FOR (each SUBTEST)
      
		  //vth_mra_in_mod(mod_id);

		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  if (sa[p] == 1)  vth_aclose_rly(mod_id, K710); // X2 to AIM A Bus
		  if (sb[p] == 1)  vth_aclose_rly(mod_id, K704); // X4 to AIM B Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
          vth_set_dgn_use_mcu_flag(mod_id);
		  vth_mcu_rlc_measure(mod_id, fb[p], rc[p], avr[p], src_freq[p], sa[p], 
		  sb[p], sl[p],extra[p], src_amp[p], det_wait[p], moa_out[p], 
		  lo[p], hi[p], opp[p], sm[p], ac[p]);
	

		  if (get_result_nolog(mod_id, &result))
		  {  // apply the actual to the p to create a delta value 
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		  }  
		  else
			display_prompt("No result in result log (Expected component value)",
						   true);

		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for READING (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	

} /* t16552 */




/*
*--------------------------------------------------------------------------------
* Test: 16556
* Description: Talon standard components: low value capacitors with compensation
*				
* Fixture: Standard components - Utilizes Page A (single density row of Pin card)	
* Resource(s): 
*--------------------------------------------------------------------------------
*/

void t16556(int mod_id, int /*slot*/)
{   // Test utilizes Pin card slot 3 Page A only
  static int const PIN_SLOT            =  3;
  static int const NUM_MEASUREMENTS    = 11;
  static double const LARGE_FORCE_FAIL = 1.0E20;
  int reading;
  double compensation_reading[11];
  double capacitor_reading;
  char buffer[81];
  test_result_type result;
  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  int fb[NUM_MEASUREMENTS];
  int rc[NUM_MEASUREMENTS];
  int avr[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int extra[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double lo[NUM_MEASUREMENTS];
  double hi[NUM_MEASUREMENTS];
  int opp[NUM_MEASUREMENTS];
  int sm[NUM_MEASUREMENTS];
  int ac[NUM_MEASUREMENTS];


  // --- Compensation Paths on Talon
  static HTalon::RelayName const S_Bus_COMP[2]   = {HTalon::KAS15,  HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_COMP[2]   = {HTalon::KAI36,  HTalon::KIA}; //to X3
  
  // --- 2 Wire Capacitance Paths on Talon
  //NOTE FOR CLEAN UP : A and B Busses NOT USED for 2 WIRE measurements
  //Capacitor 150 nF - 2 wire (subtest 0)  
  static HTalon::RelayName const S_Bus_C150nF[2]   = {HTalon::KAS13, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C150nF[2]   = {HTalon::KAI34, HTalon::KIA}; //to X3
  
  //Capacitor 15 nF  - 2 wire (subtest 1 and 2)
  static HTalon::RelayName const S_Bus_C15nF[2]    = {HTalon::KAS45, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C15nF[2]    = {HTalon::KAI66, HTalon::KIA}; //to X3
  
  //Capacitor 1.5 nF - 2 wire (subtest 3, and 4) 
  static HTalon::RelayName const S_Bus_C1Pt5nF[2]  = {HTalon::KAS47, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C1Pt5nF[2]  = {HTalon::KAI68, HTalon::KIA}; //to X3
  
  //Capacitor 150 pF - 2 wire (subtest 5, 6, and 8)
  static HTalon::RelayName const S_Bus_C150pF[2]   = {HTalon::KAS49, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C150pF[2]   = {HTalon::KAI70, HTalon::KIA}; //to X3
  
  //Capacitor 15 pF - 2 wire (subtest 7 and 9)
  static HTalon::RelayName const S_Bus_C15pF[2]    = {HTalon::KAS51, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C15pF[2]    = {HTalon::KAI72, HTalon::KIA}; //to X3
  
  //Capacitor 10 pF - 2 wire (subtest 10)
  static HTalon::RelayName const S_Bus_C10pF[2]    = {HTalon::KAS53, HTalon::KSA}; //to X1
  static HTalon::RelayName const I_Bus_C10pF[2]    = {HTalon::KAI74, HTalon::KIA}; //to X3

  static unsigned int const n=NUM_MEASUREMENTS;
  static rlylist const tps[n+1] =  // add 1 to allow for Capacitance COMPENSATION setup
  {
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(S_Bus_C150nF),    R(I_Bus_C150nF)),   //2W subtest 0
	rlylist(R(S_Bus_C15nF),     R(I_Bus_C15nF)),    //2W subtest 1
	rlylist(R(S_Bus_C15nF),     R(I_Bus_C15nF)),    //2W subtest 2 same as 1
	rlylist(R(S_Bus_C1Pt5nF),   R(I_Bus_C1Pt5nF)),  //2W subtest 3
	rlylist(R(S_Bus_C1Pt5nF),   R(I_Bus_C1Pt5nF)),  //2W subtest 4 same as 3
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 5
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 6 same as 5
	rlylist(R(S_Bus_C15pF),     R(I_Bus_C15pF)),    //2W subtest 7
	rlylist(R(S_Bus_C150pF),    R(I_Bus_C150pF)),   //2W subtest 8 same as 5
	rlylist(R(S_Bus_C15pF),     R(I_Bus_C15pF)),    //2W subtest 9 same as 7
	rlylist(R(S_Bus_C10pF),     R(I_Bus_C10pF)),    //2W subtest 10
	rlylist(R(S_Bus_COMP),      R(I_Bus_COMP)),     //Path Capacitance COMP array #11
#undef R
}; // end static rlylist const

  FILE *infile;
  infile = NULL;
  std_component_array actual_value;
  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
	  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		 display_prompt("Standard Capacitors Test requires a Talon Pin Card", true);
		 display_prompt("type card in Slot 3.  Test 16556 will not be run!", true);
	  }  //End Display no pin card in pin_slot message
      else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  
		  /* Else After check for slot 3 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.compcap_MCU_talon", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'std.compcap_MCU_talon' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile

		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		fscanf(infile, "%lg", &nominal_value[reading]);
		fscanf(infile, "%d", &fb[reading]);
        fscanf(infile, "%d", &rc[reading]);
        fscanf(infile, "%d", &avr[reading]);
        fscanf(infile, "%d", &src_freq[reading]);
        fscanf(infile, "%d", &sa[reading]);
        fscanf(infile, "%d", &sb[reading]);
        fscanf(infile, "%d", &sl[reading]);
        fscanf(infile, "%d", &extra[reading]);
        fscanf(infile, "%lg", &src_amp[reading]);
        fscanf(infile, "%lg", &det_wait[reading]);
        fscanf(infile, "%lg", &moa_out[reading]);
        fscanf(infile, "%lg", &lo[reading]);
        fscanf(infile, "%lg", &hi[reading]);
        fscanf(infile, "%d", &opp[reading]);
        fscanf(infile, "%d", &sm[reading]);
		fscanf(infile, "%d%*[^\n]", &ac[reading]);
		getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon 

		//***********************Measuring compansation capacitance************************* 
		for (int j = 0; j < NUM_MEASUREMENTS; j++) //outer loop for each subtest
		{ 
		  vth_cfxt_unlock();
		  vth_mtmr_wait(mod_id, WAIT_FOR_DMM_CAPTURE);
		  flush_commands(mod_id);
		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 
//		  talonCard->closeRelay(side == SIDEA ? HTalon::KIA : HTalon::KISB, false, false);
//		  talonCard->closeRelay(side == SIDEA ? HTalon::KSA : HTalon::KSB, true, true);
		  close(mod_id, PIN_SLOT, tps[11].rut, tps[11].ref, talonCard);  // relay array 11 (COMP)
		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
          flush_commands(mod_id);

		  vth_set_dgn_use_mcu_flag(mod_id);
		  vth_mcu_rlc_measure(mod_id, fb[j], rc[j], avr[j], src_freq[j], sa[j], 
		  sb[j], sl[j],extra[j], src_amp[j], det_wait[j], moa_out[j], 
		  lo[j], hi[j], opp[j], sm[j], ac[j]);

		  if (get_result_nolog(mod_id, &result))
		  {   
			  compensation_reading[j] = result.UU.Real_result;	
		  }  
		  else
		  {   // No result present
			  display_prompt("No result in result log (Expected component value)",
				   true);
			  compensation_reading[j] = 0.0;
		  }
		}
		talonCard->openAllRelays(false,true);  //Initialize Talon
		vth_aopen_all_rlys(mod_id);
		vth_cfxt_lock();
		vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
		flush_commands(mod_id);
		
		//***********************BEGIN MAIN LOOP of SUBTESTS**************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		{ // FOR READING (each SUBTEST)
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		 
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 	  

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

	      vth_set_dgn_use_mcu_flag(mod_id);
		  vth_mcu_rlc_measure(mod_id, fb[p], rc[p], avr[p], src_freq[p], sa[p], 
		  sb[p], sl[p],extra[p], src_amp[p], det_wait[p], moa_out[p], 
		  lo[p], hi[p], opp[p], sm[p], ac[p]);


		  if (get_result_nolog(mod_id, &result))
			  capacitor_reading = result.UU.Real_result;
		  else
		  {
			  display_prompt("No result in result log (Expected component value)",
						   true);
			  capacitor_reading = LARGE_FORCE_FAIL;
		  }

		  result.UU.Real_result = capacitor_reading - compensation_reading[p];
		  result.UU.Real_result -= actual_value[p];
		  log_result(mod_id, result);
      
		  talonCard->openAllRelays(false,true);  //Initialize Talon
		  vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for "p" (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	
  
} /* t16556 */

/*
*--------------------------------------------------------------------------------
* Test: 
* Description: Talon standard components: Standard Large Capacitors
*				
* Fixture: Standard components - Utilizes Page A AND Page B (second row) of Talon Pin Card	
* Resource(s): 
*--------------------------------------------------------------------------------
*/

/* PIN STANDARD LARGE CAPACITORS  */
void t16557(int mod_id, int /*slot*/)
{
//  static int const DEFAULT_FILTER = 1;
  static int const PIN_SLOT         = 5;
  static int const NUM_MEASUREMENTS = 2;//3; NASRU MCU can't measure 10mf
	int reading;

	//Capacitor 10 mF - 4 wire (subtest 0)             S                          I
	static HTalon::RelayName const SI_Bus_C10mF[4]  = {HTalon::KAS74, HTalon::KSA, HTalon::KAI53, HTalon::KIA};
	//                                                 A                                           B 
	static HTalon::RelayName const AB_Bus_C10mF[6]  = {HTalon::KBS73, HTalon::KSAB,HTalon::KAB,   HTalon::KBI54, HTalon::KIBB, HTalon::KBB};

	//Capacitor 1 mF  - 4 wire (subtest 1)             S                          I
	static HTalon::RelayName const SI_Bus_C1mF[4]   = {HTalon::KAS68, HTalon::KSA, HTalon::KAI47, HTalon::KIA};
	//                                                 A                                           B
	static HTalon::RelayName const AB_Bus_C1mF[6]   = {HTalon::KBS67, HTalon::KSAB,HTalon::KAB,   HTalon::KBI48, HTalon::KIBB, HTalon::KBB};
	
	//Capacitor 100 uF  - 4 wire (subtest 1)           S                          I
	static HTalon::RelayName const SI_Bus_C100uF[4] = {HTalon::KAS66, HTalon::KSA, HTalon::KAI45, HTalon::KIA};
	//                                                 A                                           B
	static HTalon::RelayName const AB_Bus_C100uF[6] = {HTalon::KBS65, HTalon::KSAB,HTalon::KAB,   HTalon::KBI46, HTalon::KIBB, HTalon::KBB};
	

static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	//rlylist(R(SI_Bus_C10mF),    R(AB_Bus_C10mF)),   //2W subtest 0
	rlylist(R(SI_Bus_C1mF),     R(AB_Bus_C1mF)),    //2W subtest 1
	rlylist(R(SI_Bus_C100uF),   R(AB_Bus_C100uF)),  //2W subtest 2 
#undef R
}; // end static rlylist const

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  int fb[NUM_MEASUREMENTS];
  int rc[NUM_MEASUREMENTS];
  int avr[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int extra[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double lo[NUM_MEASUREMENTS];
  double hi[NUM_MEASUREMENTS];
  int opp[NUM_MEASUREMENTS];
  int sm[NUM_MEASUREMENTS];
  int ac[NUM_MEASUREMENTS];
  
  char buffer[81];

  test_result_type result;

  infile = NULL;
   
  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
	  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		display_prompt("Standard Capacitors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 5.  Test 16557 will not be run!", true);
	  }  //End Display no pin card in pin_slot message


	  else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  display_prompt("FOUND TALON CARD",true);// Else After check for slot 3 Talon pin card
		  /* Else After check for slot 5 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.largecap_MCU", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'std.largecap_MCU' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile

		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		fscanf(infile, "%lg", &nominal_value[reading]);
		fscanf(infile, "%d", &fb[reading]);
        fscanf(infile, "%d", &rc[reading]);
        fscanf(infile, "%d", &avr[reading]);
        fscanf(infile, "%d", &src_freq[reading]);
        fscanf(infile, "%d", &sa[reading]);
        fscanf(infile, "%d", &sb[reading]);
        fscanf(infile, "%d", &sl[reading]);
        fscanf(infile, "%d", &extra[reading]);
        fscanf(infile, "%lg", &src_amp[reading]);
        fscanf(infile, "%lg", &det_wait[reading]);
        fscanf(infile, "%lg", &moa_out[reading]);
        fscanf(infile, "%lg", &lo[reading]);
        fscanf(infile, "%lg", &hi[reading]);
        fscanf(infile, "%d", &opp[reading]);
        fscanf(infile, "%d", &sm[reading]);
		fscanf(infile, "%d%*[^\n]", &ac[reading]);
		getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon 

		//***********************BEGIN MAIN LOOP******************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		{ // FOR READING (each SUBTEST)
      
		  //vth_mra_in_mod(mod_id);
		 
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  if (sa[p] == 1)  vth_aclose_rly(mod_id, K714); // X6 to AIM A Bus
		  if (sb[p] == 1)  vth_aclose_rly(mod_id, K708); // X8 to AIM B Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

		  vth_set_dgn_use_mcu_flag(mod_id);
		 vth_mcu_rlc_measure(mod_id, fb[p], rc[p], avr[p], src_freq[p], sa[p], 
		  sb[p], sl[p],extra[p], src_amp[p], det_wait[p], moa_out[p], 
		  lo[p], hi[p], opp[p], sm[p], ac[p]);


		  if (get_result_nolog(mod_id, &result))
		  {  // apply the actual to the p to create a delta value 
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		  }  
		  else
			display_prompt("No result in result log (Expected component value)",
						   true);

			talonCard->openAllRelays(false,true);  //Initialize Talon
			vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for "p" (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	
  
} /* t16557 */


/*
*--------------------------------------------------------------------------------
* Test: 16558
* Description: Talon standard components: DC Large Value caps at 1m farad 
*				
* Fixture: Standard components	- 
*--------------------------------------------------------------------------------
*/
void t16558(int mod_id, int /*slot*/)
{
//  static int const DEFAULT_FILTER = 1;
    static int const PIN_SLOT         = 5;
    static int const NUM_MEASUREMENTS = 1;
//	int reading;


	//Capacitor 1 mF  - 4 wire (subtest 1)             S                          I
	static HTalon::RelayName const SI_Bus_C1mF[4]   = {HTalon::KAS68, HTalon::KSA, HTalon::KAI47, HTalon::KIA};
	//                                                 A                                           B
	static HTalon::RelayName const AB_Bus_C1mF[6]   = {HTalon::KBS67, HTalon::KSAB,HTalon::KAB,   HTalon::KBI48, HTalon::KIBB, HTalon::KBB};
	

static unsigned int const n=NUM_MEASUREMENTS;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
	rlylist(R(SI_Bus_C1mF),     R(AB_Bus_C1mF)),    //2W subtest 1
#undef R
}; // end static rlylist const

  FILE *infile;
  std_component_array actual_value;

  test_result_type result;

  infile = NULL;
   
  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
	  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		display_prompt("Standard Capacitors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 5.  Test 16558 will not be run!", true);
	  }  //End Display no pin card in pin_slot message


	  else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  display_prompt("FOUND TALON CARD",true);// Else After check for slot 3 Talon pin card
		  /* Else After check for slot 5 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

	
		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon 

		//***********************BEGIN MAIN LOOP******************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		{ // FOR READING (each SUBTEST)
      
		  //vth_mra_in_mod(mod_id);
		 
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  //if (sa[p] == 1)  vth_aclose_rly(mod_id, K714); // X6 to AIM A Bus- 2 wire measurement DC Caps
		  //if (sb[p] == 1)  vth_aclose_rly(mod_id, K708); // X8 to AIM B Bus- 2 wire measurement DC Caps
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);

		  vth_dc_cap_set_sales_test_flag(mod_id);
		  vth_dc_cap_setup(mod_id, 0.100000, 0.000045, 0, 0, 0, 0, 0, 3.000000, 1.500000);//Interval time set at 600us for 10mF
		  vth_dc_cap_meas(mod_id);
		  vth_ra_dc_cap(mod_id, 0.015, 0.0095, 1, 0.100000, 0.000045);

		  if (get_result_nolog(mod_id, &result))
		  {  // apply the actual to the p to create a delta value 
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		  }  
		  else
			display_prompt("No result in result log (Expected component value)",
						   true);

			talonCard->openAllRelays(false,true);  //Initialize Talon
			vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for "p" (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	
  
} /* t16558 */


/*-----------------------------------------------------------------------------
 * Test:16559
 * Description: Talon standard components: low value capacitors without
 * compensation
 *
 * Fixture: Standard components
 * Resource(s):
 *
 * The i5000 cannot perform an accurate 4 wire ohm measurement (using only one side of a pin card) at 1024 hertz.   
 * A better test is performed at 128 Hz.   Widen the limits for subtest 2 only, to -8.0 uFarads (-8.0 E-6), 
 * so as to not affect the sequencing of the other subtests.
*--------------------------------------------------------------------------------
*/

/* PIN STANDARD LOW VALUE CAPACITORS WITHOUT COMPENSATION */
void t16559(int mod_id, int /* slot */)
{
//  static int const DEFAULT_FILTER   =  1;
  static int const PIN_SLOT         =  3;
  static int const NUM_MEASUREMENTS = 2;
  int reading;
	

// --- 2 and 4 Wire Capacitance Paths on Talon

  
//Capacitor 10 pF - 2 wire (subtest 20)
static HTalon::RelayName const S_Bus_C10pF[2]    = {HTalon::KAS53,                HTalon::KSA}; //to X1
static HTalon::RelayName const I_Bus_C10pF[2]    = {HTalon::KAI74,                HTalon::KIA}; //to X3

static unsigned int const n=2;
static rlylist const tps[n] =
{
#define R(P) P, &P[sizeof(P)/ sizeof(HTalon::RelayName)]
 rlylist(R(S_Bus_C10pF),     R(I_Bus_C10pF)),    //2W subtest 0
 rlylist(R(S_Bus_C10pF),     R(I_Bus_C10pF)),    //2W subtest 1
#undef R
};

  FILE *infile;
  std_component_array actual_value;

  // ARRAYS for ASRU MOA Parameters for each measurement
  double nominal_value[NUM_MEASUREMENTS];
  
  int fb[NUM_MEASUREMENTS];
  int rc[NUM_MEASUREMENTS];
  int avr[NUM_MEASUREMENTS];
  int src_freq[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  int extra[NUM_MEASUREMENTS];
  double src_amp[NUM_MEASUREMENTS];
  double det_wait[NUM_MEASUREMENTS];
  double moa_out[NUM_MEASUREMENTS];
  double lo[NUM_MEASUREMENTS];
  double hi[NUM_MEASUREMENTS];
  int opp[NUM_MEASUREMENTS];
  int sm[NUM_MEASUREMENTS];
  int ac[NUM_MEASUREMENTS];
  
  char buffer[81];

  test_result_type result;

  infile = NULL;

  if (get_fixture_id(mod_id) != INDUCTOR_FXT)
  { /* Bypass Loop */
  
	  int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		display_prompt("Standard Capacitors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 3.  Test16559 will not be run!", true);
	  }  //End Display no pin card in pin_slot message


	  else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  display_prompt("FOUND TALON CARD",true);// Else After check for slot 2 Talon pin card
		  /* Else After check for slot 2 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		  load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.capacitor_MCU_HI_FREQ", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
		  {
			display_prompt("The 'sstd.capacitor_MCU_HI_FREQ' file was not found.  Please have sys admin replace.", true);
			_EscIO(FileNotFound);
		  } // end if infile


		// Read ASRU measurement setup parameters for each R Measurement
		for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
		{
		fscanf(infile, "%lg", &nominal_value[reading]);
		fscanf(infile, "%d", &fb[reading]);
        fscanf(infile, "%d", &rc[reading]);
        fscanf(infile, "%d", &avr[reading]);
        fscanf(infile, "%d", &src_freq[reading]);
        fscanf(infile, "%d", &sa[reading]);
        fscanf(infile, "%d", &sb[reading]);
        fscanf(infile, "%d", &sl[reading]);
        fscanf(infile, "%d", &extra[reading]);
        fscanf(infile, "%lg", &src_amp[reading]);
        fscanf(infile, "%lg", &det_wait[reading]);
        fscanf(infile, "%lg", &moa_out[reading]);
        fscanf(infile, "%lg", &lo[reading]);
        fscanf(infile, "%lg", &hi[reading]);
        fscanf(infile, "%d", &opp[reading]);
        fscanf(infile, "%d", &sm[reading]);
        fscanf(infile, "%d%*[^\n]", &ac[reading]);
		getc(infile);

		}  // end for reading file

		if (infile != NULL)
		  fclose(infile);
		infile = NULL;

		HTalon *talonCard = TestTalon::getTalonCardAt(mod_id,PIN_SLOT);
		vth_aopen_all_rlys(mod_id);
		talonCard->openAllRelays(false,true);  //Initialize Talon 

		//***********************BEGIN MAIN LOOP******************************** 
		for (int p = 0; p < NUM_MEASUREMENTS; p++) //outer loop for each subtest
		//NUM_MEASUREMENTS =20 but start p at 0
		{ // FOR READING (each SUBTEST)
      
		  //vth_mra_in_mod(mod_id);
		 
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);
		  close(mod_id, PIN_SLOT, tps[p].rut, tps[p].ref, talonCard);

		  vth_aclose_rly(mod_id, K733); // X1 to AIM S Bus
		  vth_aclose_rly(mod_id, K727); // X3 to AIM I Bus
		  if (sa[p] == 1)  vth_aclose_rly(mod_id, K710); // X2 to AIM A Bus
		  if (sb[p] == 1)  vth_aclose_rly(mod_id, K704); // X4 to AIM B Bus
		  vth_aclose_rly(mod_id, K843); // MOANON to ACOM 
		  vth_aclose_rly(mod_id, K2303); // DetLO to ACOM 

		  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
          
		  vth_set_dgn_use_mcu_flag(mod_id);
		  vth_mcu_rlc_measure(mod_id, fb[p], rc[p], avr[p], src_freq[p], sa[p], 
		  sb[p], sl[p],extra[p], src_amp[p], det_wait[p], moa_out[p], 
		  lo[p], hi[p], opp[p], sm[p], ac[p]);
		  
		  if (get_result_nolog(mod_id, &result))
		  {  // apply the actual to the p to create a delta value 
			result.UU.Real_result -= actual_value[p];
			log_result(mod_id, result);
		  }  
		  else
			display_prompt("No result in result log (Expected component value)",
						   true);

			talonCard->openAllRelays(false,true);  //Initialize Talon
			vth_aopen_all_rlys(mod_id);

		}  // END MAIN LOOP for READING (each SUBTEST)

	  }  // END Else Do the test :Correct Cardtype = Talon
  }
  
  if (infile != NULL)
	  fclose(infile);
  vth_pinit(mod_id, PIN_SLOT);
  vth_ainit(mod_id);	
  
} /* t16559 */

//############################### END OF NASRU MCU talon testing###############



//* SRI - New test 16560 added for NASRU-DH, using Slot 7 pins involved are Pin 2-S and Pin 22-I*/

//#######################################START-16560 - Zener Diode######################################
#undef RANGE
#undef RESOLUTION
#undef LOAD_RESISTANCE
#undef DMM_FUNCTION

#define NUM_ZENER   1
#define NUM_MEASUREMENTS  2
#define PIN_SLOT        7

#define FILTER          10
#define DEFAULT_FILTER  1
typedef double val_array_zener[NUM_ZENER];
typedef int bus_array_zener[NUM_ZENER];
typedef int mux_array_zener[NUM_ZENER];
const val_array_zener ZENER_VALUES = {56};
const bus_array_zener S_BUSSES_ZENER = {'A'};
const bus_array_zener A_BUSSES_ZENER = {'B'};
const bus_array_zener B_BUSSES_ZENER = {'C'};
const bus_array_zener I_BUSSES_ZENER = {'D'};
const mux_array_zener MUX_NODE_ARRAY_ZENER = {3};
static int const INDUCTOR_FXT_2 = 3801;
//------------------------------------------------------------------------------
/* PIN STANDARD COMPONENTS ZENER DIODE */
void t16560(int mod_id, int /*slot*/)
{
  int reading;
  int i;
  int zener_no;
  FILE *infile;
  std_component_array actual_value;
  double nominal_value[NUM_MEASUREMENTS];
  double current[NUM_MEASUREMENTS];
  double src_delay[NUM_MEASUREMENTS];
  int xtra_dig[NUM_MEASUREMENTS];
  int sa[NUM_MEASUREMENTS];
  int sb[NUM_MEASUREMENTS];
  int sl[NUM_MEASUREMENTS];
  double meas[NUM_MEASUREMENTS];
  double hi[NUM_MEASUREMENTS];
  double lo[NUM_MEASUREMENTS];
  int opp[NUM_MEASUREMENTS];

  test_result_type result;
  char buffer[81];

  infile = NULL;

   
// MD: Due to splitting the standard component fixture into 
//     two fixtures (rc and ind) it was necessary to have an
//     "escape" loop so that the inductor fixture will not 
//     run the RC tests and vice versa. This is NOT the best way
//     to do this however I have no time to do this the proper way

  if (get_fixture_id(mod_id) != INDUCTOR_FXT_2)
  { /* Bypass Loop */
    int card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
	  if (card_type != TDD10) // Verify Talon Card
	  {  //Display no pin card in pin_slot message
		display_prompt("Standard Capacitors Test requires a Talon Pin Card", true);
		display_prompt("type card in Slot 3.  Test16559 will not be run!", true);
	  }  //End Display no pin card in pin_slot message


	  else  // BEGIN MAIN TEST LOOP IF TALON CARD!
	  {  display_prompt("FOUND TALON CARD",true);// Else After check for slot 2 Talon pin card
		  /* Else After check for slot 2 pin card */
		  present_slot[mod_id - NO_TESTHEAD_NUMBER] = PIN_SLOT;
		 // load_fxt_specific_actual_values(mod_id, NUM_MEASUREMENTS, actual_value);

		  sprintf(buffer, "%sstd.zener", testwhere_path);  // loads CAL Lab standard values
		  infile = fopen(buffer, "r");

		  if (infile == NULL)
      {
        display_prompt("The 'std.zener' file was not found.  Please have sys admin replace.", true);
        _EscIO(FileNotFound);
      }



      for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
      {
        fscanf(infile, "%lg", &nominal_value[reading]);
        fscanf(infile, "%lg", &current[reading]);
        fscanf(infile, "%lg", &src_delay[reading]);
        fscanf(infile, "%d", &xtra_dig[reading]);
        fscanf(infile, "%d", &sa[reading]);
        fscanf(infile, "%d", &sb[reading]);
        fscanf(infile, "%d", &sl[reading]);
        fscanf(infile, "%lg", &meas[reading]);
        fscanf(infile, "%lg", &hi[reading]);
		fscanf(infile, "%lg", &lo[reading]);
		fscanf(infile, "%d", &opp[reading]);
		getc(infile);
      }

  	  
	  if (infile != NULL)
        fclose(infile);
      infile = NULL;

      for (reading = 0; reading < NUM_MEASUREMENTS; reading++)
          /* FOR READING */
      {  /* FOR READING */
        for (i = 1; i <= NUM_ZENER; i++)
        {
          if (nominal_value[reading]== ZENER_VALUES[i - 1]) 
            zener_no = i;
        }

        card_type = get_testhead_actual_card(mod_id, PIN_SLOT);
        /* determine if card is caribou and put in compatibility mode */
        /* if not Caribou then select all Channel Virtual Cards */
        if ((card_type == CDDP) || (card_type == CDD20) ||
            (card_type == CDD6) || (card_type == CDDJB) ||
            (card_type == CDDU) || (card_type == VCDD2) ||
            (card_type == VFDD2))
          vth_pdis_32chan(mod_id, PIN_SLOT);
        else
          virt_card_select(mod_id, PIN_SLOT, 3);

        /* ALWAYS CONNECT THE S BUS MEASUREMENT PATH */
        vth_aclose_rly(mod_id, K734);
        vth_pclose_rly(mod_id, PIN_SLOT, KAA);
        vth_pclose_rly(mod_id, PIN_SLOT, KSAA);
        vth_pclose_rly(mod_id, PIN_SLOT, KAS02);


        /* ALWAYS CONNECT THE I BUS MEASUREMENT PATH */
        vth_aclose_rly(mod_id, K728);
        vth_pclose_rly(mod_id, PIN_SLOT, KBA);
        vth_pclose_rly(mod_id, PIN_SLOT, KIBA);
        vth_pclose_rly(mod_id, PIN_SLOT, KAI22);
 
        	   		
		//Setup for the zener  
        vth_high_volt_zener_setup(mod_id, current[reading], src_delay[reading], xtra_dig[reading], sa[reading], 
			sb[reading], sl[reading], meas[reading]);
        
		//Measure for the zener  
		vth_high_volt_zener_meas(mod_id);

		//Process the results of the Zener measurement
		//vth_ra_diode(mod_id, hi[reading], lo[reading], opp[reading]);
		vth_get_diode_result(mod_id); 
     
        if (get_result_nolog(mod_id, &result))
        {  /* apply the actual to the reading to create a delta value */
          //result.UU.Real_result -= actual_value[reading];
          log_result(mod_id, result);
        }  /* apply the actual to the reading to create a delta value */
        else
          display_prompt("No result in result log (Expected component value)",
                         true);

        /* ALWAYS DISCONNECT THE S BUS MEASUREMENT PATH */
        vth_aopen_rly(mod_id, K734);
        vth_popen_rly(mod_id, PIN_SLOT, KAA);
        vth_popen_rly(mod_id, PIN_SLOT, KSAA);
        vth_popen_rly(mod_id, PIN_SLOT, KAS02);

        /* ALWAYS DISCONNECT THE I BUS MEASUREMENT PATH */
        vth_aopen_rly(mod_id, K728);
        vth_popen_rly(mod_id, PIN_SLOT, KBA);
        vth_popen_rly(mod_id, PIN_SLOT, KIBA);
        vth_popen_rly(mod_id, PIN_SLOT, KAI22);

     
      }  /* for READING */
    }  /* Else Do the test */
  } /* Bypass Loop */

  if (infile != NULL)
    fclose(infile);
} /* t16560 */


#undef NUM_ZENER
#undef NUM_MEASUREMENTS
#undef PIN_SLOT
#undef FILTER
#undef DEFAULT_FILTER

//#############################################END-16560-Zener ##########################################











