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
#include "../../../vth/src/vth_cap.h"
#include "../../../vth/src/aimasm.h"
#include "../../../vth/src/vth_mac.h"
#include "../../../vth/src/macasm.h"
#include "../../../vth/src/vth_cap.h"
#include "../../../vth/src/MetaTesthead.h"
#include "../../../vth/src/ConnectBRCtoBus.h"
#include "../../../vth/src/ConnectDigitalResource.h"
#include "../../../dgn/src/termio_p.h"
#include "../i5000Tests/TestUtils.h"
#include "../i5000Tests/TestRelays.h"
#include "../i5000Tests/TestTalon.h" 
#include "../i5000Tests/TestPin.h" 
#include "../parms_p.h"
#include "mt16000.h"
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
using HTalon::KT5VA;
using HTalon::KT5VB;
using HTalon::KTCOMMA;
using HTalon::KTCOMMB;
using HTalon::KADCLA;
using HTalon::KADCLB;

struct relay_set 
{
	HTalon::Relays rut;   
	HTalon::Relays ref;
	relay_set(RLYLIST rut_b, RLYLIST rut_e, RLYLIST ref_b, RLYLIST ref_e) 
	  : rut(rut_b, rut_e), ref(ref_b, ref_e) {}
};

struct rlylist 
{
	HTalon::Relays ref;
	rlylist(RLYLIST ref_b, RLYLIST ref_e) : ref(ref_b, ref_e) {}
};

static HTalon::RelayName const Ibus_A[72] =
{
  HTalon::KAI01, HTalon::KAI02, HTalon::KAI03, HTalon::KAI04, HTalon::KAI05,
  HTalon::KAI06, HTalon::KAI07, HTalon::KAI08, HTalon::KAI09, HTalon::KAI10, 
  HTalon::KAI11, HTalon::KAI12, HTalon::KAI13, HTalon::KAI14, HTalon::KAI15,
  HTalon::KAI16, HTalon::KAI17, HTalon::KAI18, HTalon::KAI21, HTalon::KAI22, 
  HTalon::KAI23, HTalon::KAI24, HTalon::KAI25, HTalon::KAI26, HTalon::KAI27,
  HTalon::KAI28, HTalon::KAI29, HTalon::KAI30, HTalon::KAI31, HTalon::KAI32, 
  HTalon::KAI33, HTalon::KAI34, HTalon::KAI35, HTalon::KAI36, HTalon::KAI37, 
  HTalon::KAI38, HTalon::KAI41, HTalon::KAI42, HTalon::KAI43, HTalon::KAI44, 
  HTalon::KAI45, HTalon::KAI46, HTalon::KAI47, HTalon::KAI48, HTalon::KAI49,	
  HTalon::KAI50, HTalon::KAI51, HTalon::KAI52, HTalon::KAI53, HTalon::KAI54, 
  HTalon::KAI55, HTalon::KAI56, HTalon::KAI57, HTalon::KAI58, HTalon::KAI61, 
  HTalon::KAI62, HTalon::KAI63, HTalon::KAI64, HTalon::KAI65, HTalon::KAI66, 
  HTalon::KAI67, HTalon::KAI68, HTalon::KAI69, HTalon::KAI70, HTalon::KAI71, 
  HTalon::KAI72, HTalon::KAI73, HTalon::KAI74, HTalon::KAI75, HTalon::KAI76, 
  HTalon::KAI77, HTalon::KAI78
};

static HTalon::RelayName const Ibus_B[72] =
{
  HTalon::KBI01, HTalon::KBI02, HTalon::KBI03, HTalon::KBI04, HTalon::KBI05,
  HTalon::KBI06, HTalon::KBI07, HTalon::KBI08, HTalon::KBI09, HTalon::KBI10, 
  HTalon::KBI11, HTalon::KBI12, HTalon::KBI13, HTalon::KBI14, HTalon::KBI15,
  HTalon::KBI16, HTalon::KBI17, HTalon::KBI18, HTalon::KBI21, HTalon::KBI22, 
  HTalon::KBI23, HTalon::KBI24, HTalon::KBI25, HTalon::KBI26, HTalon::KBI27,
  HTalon::KBI28, HTalon::KBI29, HTalon::KBI30, HTalon::KBI31, HTalon::KBI32, 
  HTalon::KBI33, HTalon::KBI34, HTalon::KBI35, HTalon::KBI36, HTalon::KBI37, 
  HTalon::KBI38, HTalon::KBI41, HTalon::KBI42, HTalon::KBI43, HTalon::KBI44, 
  HTalon::KBI45, HTalon::KBI46, HTalon::KBI47, HTalon::KBI48, HTalon::KBI49,	
  HTalon::KBI50, HTalon::KBI51, HTalon::KBI52, HTalon::KBI53, HTalon::KBI54, 
  HTalon::KBI55, HTalon::KBI56, HTalon::KBI57, HTalon::KBI58, HTalon::KBI61, 
  HTalon::KBI62, HTalon::KBI63, HTalon::KBI64, HTalon::KBI65, HTalon::KBI66, 
  HTalon::KBI67, HTalon::KBI68, HTalon::KBI69, HTalon::KBI70, HTalon::KBI71, 
  HTalon::KBI72, HTalon::KBI73, HTalon::KBI74, HTalon::KBI75, HTalon::KBI76, 
  HTalon::KBI77, HTalon::KBI78
};

static HTalon::RelayName const Sbus_A[72] =
{
  HTalon::KAS01, HTalon::KAS02, HTalon::KAS03, HTalon::KAS04, HTalon::KAS05,
  HTalon::KAS06, HTalon::KAS07, HTalon::KAS08, HTalon::KAS09, HTalon::KAS10, 
  HTalon::KAS11, HTalon::KAS12, HTalon::KAS13, HTalon::KAS14, HTalon::KAS15,
  HTalon::KAS16, HTalon::KAS17, HTalon::KAS18, HTalon::KAS21, HTalon::KAS22, 
  HTalon::KAS23, HTalon::KAS24, HTalon::KAS25, HTalon::KAS26, HTalon::KAS27,
  HTalon::KAS28, HTalon::KAS29, HTalon::KAS30, HTalon::KAS31, HTalon::KAS32, 
  HTalon::KAS33, HTalon::KAS34, HTalon::KAS35, HTalon::KAS36, HTalon::KAS37, 
  HTalon::KAS38, HTalon::KAS41, HTalon::KAS42, HTalon::KAS43, HTalon::KAS44, 
  HTalon::KAS45, HTalon::KAS46, HTalon::KAS47, HTalon::KAS48, HTalon::KAS49,	
  HTalon::KAS50, HTalon::KAS51, HTalon::KAS52, HTalon::KAS53, HTalon::KAS54, 
  HTalon::KAS55, HTalon::KAS56, HTalon::KAS57, HTalon::KAS58, HTalon::KAS61, 
  HTalon::KAS62, HTalon::KAS63, HTalon::KAS64, HTalon::KAS65, HTalon::KAS66, 
  HTalon::KAS67, HTalon::KAS68, HTalon::KAS69, HTalon::KAS70, HTalon::KAS71, 
  HTalon::KAS72, HTalon::KAS73, HTalon::KAS74, HTalon::KAS75, HTalon::KAS76, 
  HTalon::KAS77, HTalon::KAS78
};

static HTalon::RelayName const Sbus_B[72] =
{
  HTalon::KBS01, HTalon::KBS02, HTalon::KBS03, HTalon::KBS04, HTalon::KBS05,
  HTalon::KBS06, HTalon::KBS07, HTalon::KBS08, HTalon::KBS09, HTalon::KBS10, 
  HTalon::KBS11, HTalon::KBS12, HTalon::KBS13, HTalon::KBS14, HTalon::KBS15,
  HTalon::KBS16, HTalon::KBS17, HTalon::KBS18, HTalon::KBS21, HTalon::KBS22, 
  HTalon::KBS23, HTalon::KBS24, HTalon::KBS25, HTalon::KBS26, HTalon::KBS27,
  HTalon::KBS28, HTalon::KBS29, HTalon::KBS30, HTalon::KBS31, HTalon::KBS32, 
  HTalon::KBS33, HTalon::KBS34, HTalon::KBS35, HTalon::KBS36, HTalon::KBS37, 
  HTalon::KBS38, HTalon::KBS41, HTalon::KBS42, HTalon::KBS43, HTalon::KBS44, 
  HTalon::KBS45, HTalon::KBS46, HTalon::KBS47, HTalon::KBS48, HTalon::KBS49,	
  HTalon::KBS50, HTalon::KBS51, HTalon::KBS52, HTalon::KBS53, HTalon::KBS54, 
  HTalon::KBS55, HTalon::KBS56, HTalon::KBS57, HTalon::KBS58, HTalon::KBS61, 
  HTalon::KBS62, HTalon::KBS63, HTalon::KBS64, HTalon::KBS65, HTalon::KBS66, 
  HTalon::KBS67, HTalon::KBS68, HTalon::KBS69, HTalon::KBS70, HTalon::KBS71, 
  HTalon::KBS72, HTalon::KBS73, HTalon::KBS74, HTalon::KBS75, HTalon::KBS76, 
  HTalon::KBS77, HTalon::KBS78
};

static HTalon::RelayName const Gbus_A[72] =
{
  HTalon::KAG01, HTalon::KAG02, HTalon::KAG03, HTalon::KAG04, HTalon::KAG05,
  HTalon::KAG06, HTalon::KAG07, HTalon::KAG08, HTalon::KAG09, HTalon::KAG10, 
  HTalon::KAG11, HTalon::KAG12, HTalon::KAG13, HTalon::KAG14, HTalon::KAG15,
  HTalon::KAG16, HTalon::KAG17, HTalon::KAG18, HTalon::KAG21, HTalon::KAG22, 
  HTalon::KAG23, HTalon::KAG24, HTalon::KAG25, HTalon::KAG26, HTalon::KAG27,
  HTalon::KAG28, HTalon::KAG29, HTalon::KAG30, HTalon::KAG31, HTalon::KAG32, 
  HTalon::KAG33, HTalon::KAG34, HTalon::KAG35, HTalon::KAG36, HTalon::KAG37, 
  HTalon::KAG38, HTalon::KAG41, HTalon::KAG42, HTalon::KAG43, HTalon::KAG44, 
  HTalon::KAG45, HTalon::KAG46, HTalon::KAG47, HTalon::KAG48, HTalon::KAG49,	
  HTalon::KAG50, HTalon::KAG51, HTalon::KAG52, HTalon::KAG53, HTalon::KAG54, 
  HTalon::KAG55, HTalon::KAG56, HTalon::KAG57, HTalon::KAG58, HTalon::KAG61, 
  HTalon::KAG62, HTalon::KAG63, HTalon::KAG64, HTalon::KAG65, HTalon::KAG66, 
  HTalon::KAG67, HTalon::KAG68, HTalon::KAG69, HTalon::KAG70, HTalon::KAG71, 
  HTalon::KAG72, HTalon::KAG73, HTalon::KAG74, HTalon::KAG75, HTalon::KAG76, 
  HTalon::KAG77, HTalon::KAG78
};

static HTalon::RelayName const Gbus_B[72] =
{
  HTalon::KBG01, HTalon::KBG02, HTalon::KBG03, HTalon::KBG04, HTalon::KBG05,
  HTalon::KBG06, HTalon::KBG07, HTalon::KBG08, HTalon::KBG09, HTalon::KBG10, 
  HTalon::KBG11, HTalon::KBG12, HTalon::KBG13, HTalon::KBG14, HTalon::KBG15,
  HTalon::KBG16, HTalon::KBG17, HTalon::KBG18, HTalon::KBG21, HTalon::KBG22, 
  HTalon::KBG23, HTalon::KBG24, HTalon::KBG25, HTalon::KBG26, HTalon::KBG27,
  HTalon::KBG28, HTalon::KBG29, HTalon::KBG30, HTalon::KBG31, HTalon::KBG32, 
  HTalon::KBG33, HTalon::KBG34, HTalon::KBG35, HTalon::KBG36, HTalon::KBG37, 
  HTalon::KBG38, HTalon::KBG41, HTalon::KBG42, HTalon::KBG43, HTalon::KBG44, 
  HTalon::KBG45, HTalon::KBG46, HTalon::KBG47, HTalon::KBG48, HTalon::KBG49,	
  HTalon::KBG50, HTalon::KBG51, HTalon::KBG52, HTalon::KBG53, HTalon::KBG54, 
  HTalon::KBG55, HTalon::KBG56, HTalon::KBG57, HTalon::KBG58, HTalon::KBG61, 
  HTalon::KBG62, HTalon::KBG63, HTalon::KBG64, HTalon::KBG65, HTalon::KBG66, 
  HTalon::KBG67, HTalon::KBG68, HTalon::KBG69, HTalon::KBG70, HTalon::KBG71, 
  HTalon::KBG72, HTalon::KBG73, HTalon::KBG74, HTalon::KBG75, HTalon::KBG76, 
  HTalon::KBG77, HTalon::KBG78
};

s80 message = "WARNING: Potential High-Z measurement problem, THIS IS NOT A FAILURE";


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


/*--------------------------------------------------------------------------------
* Test: 16000
* Description: Verify rack settings & connections
*				
* Fixture: Analog/Digital
* Resource(s): HPIB connection to Sales rack
*--------------------------------------------------------------------------------
*/

void t16000(void)
{
	int    reset16000 = 0;
	int    mod_id = 0;
	int    analog_rack_exists = 9;
	int const buffer_size = 80;
	long int start_time;
	double version = 0.0;
	char   buffer[81];
	char start_date[40];
	char *start_date_ptr;
	char banner[81] = "---------------------------------------------------	";
	char banner2[81] = "            *										"; 
	char banner3[81] = "      *     *     *									";
	char banner4[81] = "        *   *   *									";
	char banner5[81] = "          *   *         Agilent Technologies Co.    ";
	char banner6[81] = "   * * * *     * * * *  In-Circuit Test system		";
	char banner7[81] = "                                                    ";
	char banner8[81] = "          *   *                                     ";
	
	time_t now;
	FILE * infile;
	OpenResourceManager();
	start_date_ptr = start_date;
	now = time(NULL); //record current date and time
	strcpy(start_date_ptr, ctime(&now));  //copy date and time for later
	//use by t16999
	// only start the time if 16000 is run at beginning of sales, not during exception cleanup
	if (reset16000 == 0)
		start_time = time(NULL);  //record current time in seconds
	
	// while we have an instrument exception and we have reset the
	// sales rack <3 times, attempt to clear the errors.
	while (((exception_value() >= UNABLE_TO_CLOSE_FILE) && (exception_value() <= UNABLE_TO_WRITE_FILE)) && (reset16000 < 3))
	{
		show_value(mod_id, "exception received = ", exception_value());
		set_exception_register(NO_EXCEPTION);
		++reset16000;
	}
	
	if (reset16000 == 3)
	{
		display_prompt("HPIB device problem", true);
		display_prompt(" Hit Break to abort testing...", true);
		get_user_answer(buffer, buffer_size);
	}
	
	infile = NULL;
	sprintf(buffer, "%sVerAndRelDate", testwhere_path);
	infile = fopen(buffer, "r");

	display_prompt(banner, true);
	display_prompt(banner7, true);
	display_prompt(banner2, true);
	display_prompt(banner3, true);
	display_prompt(banner4, true);
	display_prompt(banner5, true);
	display_prompt(banner6, true);
	display_prompt(banner8, true);
	display_prompt(banner4, true);
	display_prompt(banner3, true);
	display_prompt(banner2, true);
	display_prompt(banner7, true);
	display_prompt(banner, true);
	
//	display_prompt("Running Sales test version 06.00 ", true);
//	display_it(0,"Built on: , mod_id, version);  
//	display_prompt("---------------------------------------------------	", true);

	if (infile != NULL)
		fclose(infile);
	
	mod_id = any_module_present();
	OpenResourceManager();
	
	/* MUX */
	display_prompt("Verifying MUX ...", true);
	open_mux(mod_id);
	verify_mux(mod_id);
	close_mux(mod_id);
	
	/* SYNTHESIZER */
	display_prompt("Verifying SYNTHESIZER ...", true);
	open_syn(mod_id);
	verify_syn(mod_id);
	close_syn(mod_id);
		
	/* GENERATOR */
	display_prompt("Verifying GENERATOR ...", true);
	open_gen(mod_id);
	verify_gen(mod_id);
	close_gen(mod_id);
		
	/* COUNTER */
	display_prompt("Presetting and verifying COUNTER ...", true);
	open_cntr(mod_id);
	verify_counter(mod_id);
	close_cntr(mod_id);
	
	/* DMM */
	display_prompt("Verifying DMM ...", true);
	open_dmm(mod_id);
	verify_dmm(mod_id);
	close_dmm(mod_id);
	
	/* DMM50 */
	display_prompt("Verifying DMM50 ...", true);
	open_dmm50(mod_id);
	close_dmm50(mod_id);

	/* LCR */
	display_prompt("Verifying LCR ...", true);
	open_lcr(mod_id);
	verify_lcr(mod_id);
	close_lcr(mod_id);
	
	/* PS */
	display_prompt("Verifying PS ...", true);
	open_ps(mod_id);
    /(mod_id);//sri testing 6634B
	close_ps(mod_id);

	CloseResourceManager();
	
}  /* t16000 */


/*--------------------------------------------------------------------------------
* Test: 16001
* Description: ANALOG - Read fixture enable and fixture ID
*
* Fixture: Analog 
* Resource(s): 
*--------------------------------------------------------------------------------
*/

void t16001(int mod_id, int)
{
	int fixture_id;
	char buffer[81];
//	FILE *rack_bypass = NULL;
//	strcpy(buffer, testwhere_path);
//	strcat(buffer, "fixture_id");
	test_result_type result;
	OpenResourceManager();
	open_mux(mod_id);
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	vth_mrd_cell(mod_id, MIO_FXT_EN);
	get_result_nolog(mod_id, &result);
//	if(fopen(buffer, "r") == NULL)
//	{
		if (result.UU.Integer_result != FIXTURE_ENABLED)
		{
			if (get_result_nolog(mod_id, &result))		// FLUSH THE COMMANDS
			{
				vth_cfxt_unlock();
				display_prompt("Unexpected result encountered", true);
				display_prompt("Please install the analog sales fixture.", true);
				display_prompt("Then press <Return> or <Enter>.  ", true);
				get_user_answer(buffer, 80);
			}
		}
		vth_cfxt_lock();
		vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
		vth_mrd_cell(mod_id, MIO_FXT_EN);
		get_result_nolog(mod_id, &result);
		if (result.UU.Integer_result == FIXTURE_ENABLED)
		{  
			fixture_id = get_fixture_id(mod_id);
			//show_value(0, "Fixture ID= ", fixture_id);
			if (fixture_id < ANA_ID_MIN || fixture_id > ANA_ID_MAX)
			{
				display_prompt("Incorrect Fixture Found", true);
				// Next routine causes the non-module failure to be set also
				add_test_to_gfs(present_test[mod_id - NO_TESTHEAD_NUMBER]);
			}
			result.UU.Integer_result = fixture_id;
		}  
		else
		{  
			display_prompt("No fixture found.", true);
			
			vth_cfxt_unlock();
		}  
//		fopen(buffer, "w");

//	}
	check_dmm50_button(mod_id, DMM50_ANALOG_USAGE);
	close_mux(mod_id);
	CloseResourceManager();
	
}  /*t16001*/


/*--------------------------------------------------------------------------------
 * Test: 16002
 * Description: DIGITAL - Read fixture enable and fixture ID
 *
 * Fixture: Digital 
 * Resource(s): 
 *--------------------------------------------------------------------------------
*/

void t16002(int mod_id, int)
{
	int fixture_id;
	int next;
	int current;
	char buffer[81];
	char str1[81];
	test_result_type result;
	boolean correct_fixture_in_place;
	OpenResourceManager();
	open_mux(mod_id);
	correct_fixture_in_place = false;
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	vth_mrd_cell(mod_id, MIO_FXT_EN);
	get_result_nolog(mod_id, &result);
	if (result.UU.Integer_result != FIXTURE_ENABLED)
	{
		if (!vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
			display_prompt("Press footswitch to (un)lock fixture",true);
		
		vth_cfxt_unlock();
		if (get_result_nolog(mod_id, &result))
			display_prompt("Unexpected result encountered", true);
		
		display_prompt("Please install the digital sales fixture.", true);
		display_prompt("Then press <Return> or <Enter>.  ", true);
		get_user_answer(buffer, 80);
	}
	
	if (!vth_fxt_safe(VTH_READ_FIXTURE_SAFE))
		display_prompt("Press footswitch to (un)lock fixture",true);
	
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	vth_mrd_cell(mod_id, MIO_FXT_EN);
	get_result_nolog(mod_id, &result);
	if (result.UU.Integer_result == FIXTURE_ENABLED)
	{
		fixture_id = get_fixture_id(mod_id);
		//show_value(0, "Fixture ID= ", fixture_id);
		if (fixture_id >= DIG_ID_MIN && fixture_id <= DIG_ID_MAX)
		{
			correct_fixture_in_place = rack_and_fixture_compatible(
				get_sales_rack_serial_number(str1, mod_id), fixture_id);
			if (!correct_fixture_in_place)
			{
				strcpy(buffer, "MUX serial number = ");
				current = strlen(buffer) + 1;
				strcpy(buffer + current - 1,
				get_sales_rack_serial_number(str1, mod_id));
				next = strlen(buffer) + 1;
				display_prompt(buffer, true);
				strcpy(buffer, "fixture id = ");
				current = strlen(buffer) + 1;
				sprintf(buffer + current - 1, "%12d", fixture_id);
				next = strlen(buffer) + 1;
				display_prompt(buffer, true);
				display_prompt("Digital Fixture Does NOT match Sales ", true);
				display_prompt("Tests requiring Digital Fixture Will NOT Run", true);
				display_prompt("If MUX serial number seems wrong, reprogram ", true);
				display_prompt("with test 16007", true);
				fixture_id = 9999;   /* Incompatible fixture causes failure */
				/* Next routine causes the non-module failure to also be set */
				add_test_to_gfs(present_test[mod_id - NO_TESTHEAD_NUMBER]);
			}
		}
		else
		{
			display_prompt("Incorrect Fixture Found", true);
			/* Next routine causes the non-module failure to be set also */
			add_test_to_gfs(present_test[mod_id - NO_TESTHEAD_NUMBER]);
		}
		result.UU.Integer_result = fixture_id;
	}
	
	else
	{
		display_prompt("No fixture found.", true);
		vth_cfxt_unlock();
	}
	
	check_dmm50_button(mod_id, DMM50_DIGITAL_USAGE);
	close_mux(mod_id);
	CloseResourceManager();
	
}  /*t16002*/


/*--------------------------------------------------------------------------------
 * Test: 16003
 * Description: Enter and store the Minerva serial number
 *
 * Fixture: 
 * Resource(s): 
 *--------------------------------------------------------------------------------
*/
   
#define BLANK_LINE      ""
   
void t16003(void)
{
	int this_module;
	char buffer[81];
	test_result_type result;
	this_module = any_module_present();
	OpenResourceManager();
	open_mux(this_module);
	
	if (get_result_nolog(this_module, &result))
		display_prompt("Unexpected result encountered!", true);
	else
	{  
		display_prompt(BLANK_LINE, true);
		display_prompt("Enter the serial number of the 3235A (Minerva)", true);
		display_prompt("Then press <Return> or <Enter>.", true);
		display_prompt(BLANK_LINE, true);
		get_user_answer(buffer, 80);
	}  
	store_mux_serial_number(this_module, buffer);
	close_mux(this_module);
	CloseResourceManager();
	   
}  /* t16003 */

#undef BLANK_LINE


/*--------------------------------------------------------------------------------
 * Test: 16004
 * Description: Fixture unlock
 *				
 * Fixture: NONE
 * Resource(s): 
 *--------------------------------------------------------------------------------
*/

void t16004(void) //int mod_id)
{
  vth_cfxt_unlock(); //Yes this only releases the fxt. during proto testing it was found that the fxt
					 // was not being unlocked after a series of tests, thus this simple routine
					 // was implemented to perform this task no mater what the last test ran was.
}

/*--------------------------------------------------------------------------------
 * Test: 16005
 * Description: Analog Fixture Validation
 *				
 * Fixture: Analog	
 * Resource(s): DMM, Power Supply
 *--------------------------------------------------------------------------------
*/

void t16005(int mod_id, int slot) 
{
	test_result_type result;
	OpenResourceManager();
	open_mux(mod_id);
	open_dmm50(mod_id);

    result.Measurement_class = CLASS_REAL;
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	HTalon * talonCard = TestTalon::getTalonCardAt(mod_id, slot);
    talonCard->openAllRelays(true, true);
	
    connect_dmm50_out_af2(mod_id);
	setup_dmm50(mod_id, DCV, 5.0, 1.0E-3);
    vth_mtmr_wait(mod_id, MEASUREMENT_WAIT);
	vth_aclose_rly(mod_id, K753);
	vth_aclose_rly(mod_id, K764);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS); 
	for(int side = SIDEA; side <= SIDEB; side++)
	{
	   talonCard->closeRelay(side == SIDEA ? KT5VA : KT5VB, false, false);
	   talonCard->closeRelay(side == SIDEA ? KTCOMMA : KTCOMMB, false, false);
	   talonCard->closeRelay(side == SIDEA ? KADCLA : KADCLB, false, false);
	   talonCard->closeRelay(side == SIDEA ? KLA : KLB, true, true);
	   flush_commands(mod_id);
        result.UU.Real_result = measure_dmm50(mod_id);
       log_result(mod_id, result);
       talonCard->openAllRelays(true, true);
	}

	vth_mod_init(mod_id);
    close_dmm50(mod_id);
    close_mux(mod_id);
	CloseResourceManager();
}

/*--------------------------------------------------------------------------------
 * Test: 16006
 * Description: Digital Fixture Validation
 *				
 * Fixture: Digital	
 * Resource(s): DMM, Synthsizer
 *--------------------------------------------------------------------------------
*/

void t16006(int mod_id, int slot)
{
	test_result_type result;
    result.Measurement_class = CLASS_REAL;

	OpenResourceManager();
	open_mux(mod_id);
	open_dmm(mod_id);
	connect_dmm_pin(mod_id, slot);
	setup_dmm(mod_id, DCV, 5.0, 1.0E-3);
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

	static HTalon::RelayName const aMINT[4] =
	{	
	  HTalon::KAG05, HTalon::KAG25, HTalon::KAG45, HTalon::KAG65, 
	};

	static HTalon::RelayName const bMINT[4] =
	{	
	  HTalon::KBG05, HTalon::KBG25, HTalon::KBG45, HTalon::KBG65, 
	};

	vth_aclose_rly(mod_id, K753);
	vth_aclose_rly(mod_id, K764);
	HTalon * talonCard = TestTalon::getTalonCardAt(mod_id, slot);
    talonCard->openAllRelays(true, true);
    vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS); 
	flush_commands(mod_id);
	for(int side = SIDEA; side <= SIDEB; side++)
	{
	   talonCard->closeRelay(side == SIDEA ? KT5VA : KT5VB, false, false);
	   talonCard->closeRelay(side == SIDEA ? KTCOMMA : KTCOMMB, false, false);
	   talonCard->closeRelay(side == SIDEA ? KADCLA : KADCLB, false, false);
	   talonCard->closeRelay(side == SIDEA ? KGLA : KGLB, false, false);
	   for(int page = 0; page <= 3; page++)
	   {
		   talonCard->closeRelay(side == SIDEA ? aMINT[page] : bMINT[page], true, true);
		   flush_commands(mod_id);
		   result.UU.Real_result = measure_dmm(mod_id);
		   log_result(mod_id, result);
		   talonCard->openRelay(side == SIDEA ? aMINT[page] : bMINT[page], true, true);
		   flush_commands(mod_id);
	   }
       talonCard->openAllRelays(true, true);
	   flush_commands(mod_id);
	}

	vth_mod_init(mod_id);
    close_dmm(mod_id);
    close_mux(mod_id);
	CloseResourceManager();
}
  

/*--------------------------------------------------------------------------------
 * Test: 16010
 * Description: Parallel Xn Bus Capacitance Offset
 *				
 * Fixture: Analog
 * Resource(s): DMM
 *--------------------------------------------------------------------------------
*/
   
#define LCR_MULT        5
#define LCR_FREQ        100e+03
#define LCR_DISPLAY     Q
#define LCR_FUNCTION    CAPACITANCE
#define LCR_CKT_MODE    PARALLEL
   
static OFFSET_CAP Cap;

void t16010(int mod_id, int /* slot */)
{
 	int bus;
	double farads;
 	test_result_type result;
	result.Measurement_class = CLASS_REAL;
	vth_cfxt_lock(); 
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	OpenResourceManager();
	open_mux(mod_id);
	open_lcr(mod_id);
	   
	setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,
		LCR_MULT);
	
	vth_aopen_all_rlys(mod_id);
	connect_dg(mod_id, true);
	connect_lcr(mod_id);
	for (bus = 'A'; bus <= 'H'; bus++)
	{  /*for*/
		connect_plus_xn(mod_id, bus, true);
		connect_minus_xn_plus_1(mod_id, bus, true);   /* 'H'  Xn + 1 = 'A' */
		vth_mtmr_wait(mod_id, WAIT_TIME);
		if(bus == 'A')  
			measure_lcr(mod_id);
		farads = measure_lcr(mod_id);
		if (mod_id == 0 || mod_id == 3)
			Cap.sales_rack_offset_cap[0][bus - 'A'] = farads;
		else
			Cap.sales_rack_offset_cap[1][bus - 'A'] = farads;

		result.UU.Real_result = farads;
		log_result(mod_id, result);
		connect_plus_xn(mod_id, bus, false);
		connect_minus_xn_plus_1(mod_id, bus, false);   /* 'H'  Xn + 1 = 'A' */
	}  /*for*/
	
	bus = 'H';
	connect_plus_gout(mod_id);
	connect_minus_xn_plus_1(mod_id, bus, true);   /* 'H'  Xn + 1 = 'A' */
	vth_mtmr_wait(mod_id, WAIT_TIME);
	farads = measure_lcr(mod_id);
	if (mod_id == 0 || mod_id == 3)
		Cap.sales_rack_offset_xg_cap[0] = farads;
	else
		Cap.sales_rack_offset_xg_cap[1] = farads;
	result.UU.Real_result = farads;
	log_result(mod_id, result);
	
	close_mux(mod_id);
	close_lcr(mod_id);
	CloseResourceManager();
	
}  /*t16010*/
  
#undef LCR_MULT
#undef LCR_FREQ
#undef LCR_DISPLAY
#undef LCR_FUNCTION
#undef LCR_CKT_MODE


/*--------------------------------------------------------------------------------
 * Test: 16032
 * Description: AF1 port verification
 *				This test is modeled after test 9032
 * Fixture: Analog
 * Resource(s): DMM
 *--------------------------------------------------------------------------------
*/

#define BLANK_LINE      ""
#define SRC_I_COMPLIANCE  1
#define SRC_R_OUT       0
#define SRC_ON          1
#define SRC_CONTROL     0
#define SAMPLE_SIZE     10
#define FREQ_A          20000
#define PEAK_VOLTS      4.0e+00
#define DC_OFFSET       0.0e+00
#define TRIGGER_LEVEL   0.0e+00


void t16032(int mod_id)
{
  /* This test uses the setup for test 9393 to verify the AF1 cable         */
  /* continuity.  The test uses the counter to measure the ARB Source Freq. */
  /* For path and test descriptions, see the documentation for test: 9032   */
  int current;
  int next;
  double cntr_actual_freq;
  char s[81];
  char buffer[81];
  test_result_type result;

  open_cntr(mod_id);
  open_mux(mod_id);
  vth_cfxt_lock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  aconnect_cntr_stop_af2(mod_id);
  if (get_result_nolog(mod_id, &result))
    display_prompt("Unexpected result encountered!", true);
  else
  {  /*else*/
    display_prompt(BLANK_LINE, true);

    /* Build the module number string for display */
    *s = '\0';
    strcat(s, "For Module: ");
    current = strlen(s) + 1;
    sprintf(s + current - 1, "%d", mod_id);
    next = strlen(s) + 1;
    strcat(s, ":");

    display_prompt("\007", true);   /* '#7' should cause a beep */
    display_prompt(s, true);
    display_prompt("Connect the BNC end of the AF1 cable to", true);
    display_prompt("the minerva BNC for that module.", true);
    display_prompt(BLANK_LINE, true);
    display_prompt("Then press <Return> or <Enter>...", true);
    display_prompt(BLANK_LINE, true);
    get_user_answer(buffer, 80);
  }  /*else*/

  aconnect_srcout_AF2(mod_id);
  vth_aclose_rly(mod_id, K757);   /* AF2 to TRIGIN relay */
  vth_aclose_rly(mod_id, K763);   /* TRIGIN AF1 relay */

  setup_cntr_trg_levels(mod_id, TRIGGER_LEVEL, TRIGGER_LEVEL);
  setup_cntr_function(mod_id, FREQUENCY);
  setup_cntr_sample_size(mod_id, SAMPLE_SIZE);

  vth_src_sqr_setup(mod_id, FREQ_A, PEAK_VOLTS, DC_OFFSET, SRC_I_COMPLIANCE,
                SRC_R_OUT, SRC_ON, SRC_CONTROL);
  vth_mtmr_wait(mod_id, SOURCE_WAIT_TIME);
  trigger_cntr(mod_id);
  cntr_actual_freq = read_cntr_mean(mod_id);
  result.Measurement_class = CLASS_REAL;
  result.UU.Real_result = cntr_actual_freq;
  log_result(mod_id, result);

  vth_afc_dis_src_out(mod_id);
  vth_ainit(mod_id);
  if (get_result_nolog(mod_id, &result))
    display_prompt("Unexpected result encountered!", true);
  else
  {  /*else*/
    display_prompt(BLANK_LINE, true);

    *s = '\0';
    strcat(s, "For Module: ");
    current = strlen(s) + 1;
    sprintf(s + current - 1, "%d", mod_id);
    next = strlen(s) + 1;
    strcat(s, ":");

    display_prompt("\007", true);   /* '#7' should cause a beep */
    display_prompt(s, true);
    display_prompt("Re-connect the BNC end of the AF2 cable to", true);
    display_prompt("the minerva BNC for that module.", true);
    display_prompt(BLANK_LINE, true);
    display_prompt("Then press <Return> or <Enter>...", true);
    display_prompt(BLANK_LINE, true);
    get_user_answer(buffer, 80);
  }  /*else*/

  close_cntr(mod_id);
  close_mux(mod_id);

}  /* t16032 */

#undef BLANK_LINE
#undef SRC_I_COMPLIANCE
#undef SRC_R_OUT
#undef SRC_ON
#undef SRC_CONTROL
#undef SAMPLE_SIZE
#undef FREQ_A
#undef PEAK_VOLTS
#undef DC_OFFSET
#undef TRIGGER_LEVEL



/*
*--------------------------------------------------------------------------------
* Test: 16188
* Description: Full Talon Buzz Test in DGN without using analyzebuzz
*				
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/
   
void t16188(int mod_id, int slot)
{
  vth_cfxt_unlock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  static HTalon::RelayName const KLY[2] =
  {
	HTalon::KLA, HTalon::KLB
  };

  static HTalon::RelayName const KSY[2] =
  {
	HTalon::KSA, HTalon::KSB
  };

  static HTalon::RelayName const KIY[2] =
  {
	HTalon::KIA, HTalon::KIB
  };

  static HTalon::RelayName const KGLY[2] =
  {
	HTalon::KGLA, HTalon::KGLB
  };

  static int const K7IY[2] =
  {
	  K735,K739
  };

  static int const K7SY[2] =
  {
	  K733,K737
  };

  static HTalon::RelayName const KYSXX[2][72] =
  {
    HTalon::KAS01, HTalon::KAS02, HTalon::KAS03, HTalon::KAS04, HTalon::KAS05, HTalon::KAS06, HTalon::KAS07, HTalon::KAS08, HTalon::KAS09,
    HTalon::KAS10, HTalon::KAS11, HTalon::KAS12, HTalon::KAS13, HTalon::KAS14, HTalon::KAS15, HTalon::KAS16, HTalon::KAS17, HTalon::KAS18,
    HTalon::KAS21, HTalon::KAS22, HTalon::KAS23, HTalon::KAS24, HTalon::KAS25, HTalon::KAS26, HTalon::KAS27, HTalon::KAS28, HTalon::KAS29,
    HTalon::KAS30, HTalon::KAS31, HTalon::KAS32, HTalon::KAS33, HTalon::KAS34, HTalon::KAS35, HTalon::KAS36, HTalon::KAS37, HTalon::KAS38,
    HTalon::KAS41, HTalon::KAS42, HTalon::KAS43, HTalon::KAS44, HTalon::KAS45, HTalon::KAS46, HTalon::KAS47, HTalon::KAS48, HTalon::KAS49,
    HTalon::KAS50, HTalon::KAS51, HTalon::KAS52, HTalon::KAS53, HTalon::KAS54, HTalon::KAS55, HTalon::KAS56, HTalon::KAS57, HTalon::KAS58,
    HTalon::KAS61, HTalon::KAS62, HTalon::KAS63, HTalon::KAS64, HTalon::KAS65, HTalon::KAS66, HTalon::KAS67, HTalon::KAS68, HTalon::KAS69,
    HTalon::KAS70, HTalon::KAS71, HTalon::KAS72, HTalon::KAS73, HTalon::KAS74, HTalon::KAS75, HTalon::KAS76, HTalon::KAS77, HTalon::KAS78,
    HTalon::KBS01, HTalon::KBS02, HTalon::KBS03, HTalon::KBS04, HTalon::KBS05, HTalon::KBS06, HTalon::KBS07, HTalon::KBS08, HTalon::KBS09,
    HTalon::KBS10, HTalon::KBS11, HTalon::KBS12, HTalon::KBS13, HTalon::KBS14, HTalon::KBS15, HTalon::KBS16, HTalon::KBS17, HTalon::KBS18,
    HTalon::KBS21, HTalon::KBS22, HTalon::KBS23, HTalon::KBS24, HTalon::KBS25, HTalon::KBS26, HTalon::KBS27, HTalon::KBS28, HTalon::KBS29,
    HTalon::KBS30, HTalon::KBS31, HTalon::KBS32, HTalon::KBS33, HTalon::KBS34, HTalon::KBS35, HTalon::KBS36, HTalon::KBS37, HTalon::KBS38,
    HTalon::KBS41, HTalon::KBS42, HTalon::KBS43, HTalon::KBS44, HTalon::KBS45, HTalon::KBS46, HTalon::KBS47, HTalon::KBS48, HTalon::KBS49,
    HTalon::KBS50, HTalon::KBS51, HTalon::KBS52, HTalon::KBS53, HTalon::KBS54, HTalon::KBS55, HTalon::KBS56, HTalon::KBS57, HTalon::KBS58,
    HTalon::KBS61, HTalon::KBS62, HTalon::KBS63, HTalon::KBS64, HTalon::KBS65, HTalon::KBS66, HTalon::KBS67, HTalon::KBS68, HTalon::KBS69,
    HTalon::KBS70, HTalon::KBS71, HTalon::KBS72, HTalon::KBS73, HTalon::KBS74, HTalon::KBS75, HTalon::KBS76, HTalon::KBS77, HTalon::KBS78
  };

  static HTalon::RelayName const KYIXX[2][72] =
  {
    HTalon::KAI01, HTalon::KAI02, HTalon::KAI03, HTalon::KAI04, HTalon::KAI05, HTalon::KAI06, HTalon::KAI07, HTalon::KAI08, HTalon::KAI09,
    HTalon::KAI10, HTalon::KAI11, HTalon::KAI12, HTalon::KAI13, HTalon::KAI14, HTalon::KAI15, HTalon::KAI16, HTalon::KAI17, HTalon::KAI18,
    HTalon::KAI21, HTalon::KAI22, HTalon::KAI23, HTalon::KAI24, HTalon::KAI25, HTalon::KAI26, HTalon::KAI27, HTalon::KAI28, HTalon::KAI29,
    HTalon::KAI30, HTalon::KAI31, HTalon::KAI32, HTalon::KAI33, HTalon::KAI34, HTalon::KAI35, HTalon::KAI36, HTalon::KAI37, HTalon::KAI38,
    HTalon::KAI41, HTalon::KAI42, HTalon::KAI43, HTalon::KAI44, HTalon::KAI45, HTalon::KAI46, HTalon::KAI47, HTalon::KAI48, HTalon::KAI49,
    HTalon::KAI50, HTalon::KAI51, HTalon::KAI52, HTalon::KAI53, HTalon::KAI54, HTalon::KAI55, HTalon::KAI56, HTalon::KAI57, HTalon::KAI58,
    HTalon::KAI61, HTalon::KAI62, HTalon::KAI63, HTalon::KAI64, HTalon::KAI65, HTalon::KAI66, HTalon::KAI67, HTalon::KAI68, HTalon::KAI69,
    HTalon::KAI70, HTalon::KAI71, HTalon::KAI72, HTalon::KAI73, HTalon::KAI74, HTalon::KAI75, HTalon::KAI76, HTalon::KAI77, HTalon::KAI78,
    HTalon::KBI01, HTalon::KBI02, HTalon::KBI03, HTalon::KBI04, HTalon::KBI05, HTalon::KBI06, HTalon::KBI07, HTalon::KBI08, HTalon::KBI09,
    HTalon::KBI10, HTalon::KBI11, HTalon::KBI12, HTalon::KBI13, HTalon::KBI14, HTalon::KBI15, HTalon::KBI16, HTalon::KBI17, HTalon::KBI18,
    HTalon::KBI21, HTalon::KBI22, HTalon::KBI23, HTalon::KBI24, HTalon::KBI25, HTalon::KBI26, HTalon::KBI27, HTalon::KBI28, HTalon::KBI29,
    HTalon::KBI30, HTalon::KBI31, HTalon::KBI32, HTalon::KBI33, HTalon::KBI34, HTalon::KBI35, HTalon::KBI36, HTalon::KBI37, HTalon::KBI38,
    HTalon::KBI41, HTalon::KBI42, HTalon::KBI43, HTalon::KBI44, HTalon::KBI45, HTalon::KBI46, HTalon::KBI47, HTalon::KBI48, HTalon::KBI49,
    HTalon::KBI50, HTalon::KBI51, HTalon::KBI52, HTalon::KBI53, HTalon::KBI54, HTalon::KBI55, HTalon::KBI56, HTalon::KBI57, HTalon::KBI58,
    HTalon::KBI61, HTalon::KBI62, HTalon::KBI63, HTalon::KBI64, HTalon::KBI65, HTalon::KBI66, HTalon::KBI67, HTalon::KBI68, HTalon::KBI69,
    HTalon::KBI70, HTalon::KBI71, HTalon::KBI72, HTalon::KBI73, HTalon::KBI74, HTalon::KBI75, HTalon::KBI76, HTalon::KBI77, HTalon::KBI78
  };

  static HTalon::RelayName const KYGXX[2][72] =
  {
    HTalon::KAG01, HTalon::KAG02, HTalon::KAG03, HTalon::KAG04, HTalon::KAG05, HTalon::KAG06, HTalon::KAG07, HTalon::KAG08, HTalon::KAG09,
    HTalon::KAG10, HTalon::KAG11, HTalon::KAG12, HTalon::KAG13, HTalon::KAG14, HTalon::KAG15, HTalon::KAG16, HTalon::KAG17, HTalon::KAG18,
    HTalon::KAG21, HTalon::KAG22, HTalon::KAG23, HTalon::KAG24, HTalon::KAG25, HTalon::KAG26, HTalon::KAG27, HTalon::KAG28, HTalon::KAG29,
    HTalon::KAG30, HTalon::KAG31, HTalon::KAG32, HTalon::KAG33, HTalon::KAG34, HTalon::KAG35, HTalon::KAG36, HTalon::KAG37, HTalon::KAG38,
    HTalon::KAG41, HTalon::KAG42, HTalon::KAG43, HTalon::KAG44, HTalon::KAG45, HTalon::KAG46, HTalon::KAG47, HTalon::KAG48, HTalon::KAG49,
    HTalon::KAG50, HTalon::KAG51, HTalon::KAG52, HTalon::KAG53, HTalon::KAG54, HTalon::KAG55, HTalon::KAG56, HTalon::KAG57, HTalon::KAG58,
    HTalon::KAG61, HTalon::KAG62, HTalon::KAG63, HTalon::KAG64, HTalon::KAG65, HTalon::KAG66, HTalon::KAG67, HTalon::KAG68, HTalon::KAG69,
    HTalon::KAG70, HTalon::KAG71, HTalon::KAG72, HTalon::KAG73, HTalon::KAG74, HTalon::KAG75, HTalon::KAG76, HTalon::KAG77, HTalon::KAG78,
    HTalon::KBG01, HTalon::KBG02, HTalon::KBG03, HTalon::KBG04, HTalon::KBG05, HTalon::KBG06, HTalon::KBG07, HTalon::KBG08, HTalon::KBG09,
    HTalon::KBG10, HTalon::KBG11, HTalon::KBG12, HTalon::KBG13, HTalon::KBG14, HTalon::KBG15, HTalon::KBG16, HTalon::KBG17, HTalon::KBG18,
    HTalon::KBG21, HTalon::KBG22, HTalon::KBG23, HTalon::KBG24, HTalon::KBG25, HTalon::KBG26, HTalon::KBG27, HTalon::KBG28, HTalon::KBG29,
    HTalon::KBG30, HTalon::KBG31, HTalon::KBG32, HTalon::KBG33, HTalon::KBG34, HTalon::KBG35, HTalon::KBG36, HTalon::KBG37, HTalon::KBG38,
    HTalon::KBG41, HTalon::KBG42, HTalon::KBG43, HTalon::KBG44, HTalon::KBG45, HTalon::KBG46, HTalon::KBG47, HTalon::KBG48, HTalon::KBG49,
    HTalon::KBG50, HTalon::KBG51, HTalon::KBG52, HTalon::KBG53, HTalon::KBG54, HTalon::KBG55, HTalon::KBG56, HTalon::KBG57, HTalon::KBG58,
    HTalon::KBG61, HTalon::KBG62, HTalon::KBG63, HTalon::KBG64, HTalon::KBG65, HTalon::KBG66, HTalon::KBG67, HTalon::KBG68, HTalon::KBG69,
    HTalon::KBG70, HTalon::KBG71, HTalon::KBG72, HTalon::KBG73, HTalon::KBG74, HTalon::KBG75, HTalon::KBG76, HTalon::KBG77, HTalon::KBG78
  };
  
  HTalon * talonCard = TestTalon::getTalonCardAt(mod_id, slot);
  talonCard->openAllRelays(false, true);                      /* Initialize to all relays open.  No wait.  send. */
  
  vth_aunpowered_init(mod_id);                /* Initialization. */
  vth_ameas_set_averaging(mod_id, 1);         /* Make one measurement. */

  vth_aopen_all_rlys(mod_id);                               /* Initialize all AIM relays. */

  bool bNewSide;

  for (int nSide=0; nSide<2; nSide++)
  {
	bNewSide = true;
	for (int nChannel=0; nChannel<72; nChannel++)
	{
      vth_aclose_rly(mod_id, K7SY[nSide]);
	  talonCard->closeRelay(KLY[nSide],false,false);
	  talonCard->closeRelay(KSY[nSide],false,false);
	  talonCard->closeRelay(KGLY[nSide],false,false);
	  talonCard->closeRelay(KYSXX[nSide][nChannel],false,false);
	  talonCard->closeRelay(KYGXX[nSide][nChannel],false,true);

	  if (bNewSide)
	  {
		talon_buzz_that_relay(mod_id, KLY[nSide], talonCard, true);
		talon_buzz_that_relay(mod_id, KSY[nSide], talonCard, true);
		talon_buzz_that_relay(mod_id, KGLY[nSide], talonCard, true);
	  }
	  
	  talon_buzz_that_relay(mod_id, KYSXX[nSide][nChannel], talonCard, true);
	  talon_buzz_that_relay(mod_id, KYGXX[nSide][nChannel], talonCard, true);

	  talonCard->openAllRelays(false, true);                      /* Initialize to all relays open.  No wait.  send. */
      vth_aopen_rly(mod_id,K7SY[nSide]);

      vth_aclose_rly(mod_id, K7IY[nSide]);
	  talonCard->closeRelay(KLY[nSide],false,false);
	  talonCard->closeRelay(KIY[nSide],false,false);
	  talonCard->closeRelay(KGLY[nSide],false,false);
	  talonCard->closeRelay(KYIXX[nSide][nChannel],false,false);
	  talonCard->closeRelay(KYGXX[nSide][nChannel],false,true);
	  
	  if (bNewSide)
	  {
		talon_buzz_that_relay(mod_id, KIY[nSide], talonCard, true);
		bNewSide = false;
	  }
	  talon_buzz_that_relay(mod_id, KYIXX[nSide][nChannel], talonCard, true);

	  talonCard->openAllRelays(false, true);                     /* Initialize to all relays open.  No wait.  send. */
      vth_aopen_rly(mod_id,K7IY[nSide]);
	}
  }

  vth_aopen_all_rlys(mod_id);                               /* Initialize all AIM relays. */
  talonCard->openAllRelays(false, true);                      /* Initialize to all relays open.  No wait.  send. */

}/*end t16188*/

 
/*
*--------------------------------------------------------------------------------
* Test: 16189
* Description: Talon Buzz Test
*				
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/
 
void t16189(int mod_id, int slot)
{
  vth_cfxt_unlock();
  static HTalon::RelayName const KLY[2] =
  {
	HTalon::KLA, HTalon::KLB
  };

  static HTalon::RelayName const KSY[2] =
  {
	HTalon::KSA, HTalon::KSB
  };

  static HTalon::RelayName const KIY[2] =
  {
	HTalon::KIA, HTalon::KIB
  };

  static HTalon::RelayName const KGLY[2] =
  {
	HTalon::KGLA, HTalon::KGLB
  };

  static int const K7IY[2] =
  {
	  K735,K739
  };

  static int const K7SY[2] =
  {
	  K733,K737
  };

  static HTalon::RelayName const KYSXX[2][72] =
  {
    HTalon::KAS01, HTalon::KAS02, HTalon::KAS03, HTalon::KAS04, HTalon::KAS05, HTalon::KAS06, HTalon::KAS07, HTalon::KAS08, HTalon::KAS09,
    HTalon::KAS10, HTalon::KAS11, HTalon::KAS12, HTalon::KAS13, HTalon::KAS14, HTalon::KAS15, HTalon::KAS16, HTalon::KAS17, HTalon::KAS18,
    HTalon::KAS21, HTalon::KAS22, HTalon::KAS23, HTalon::KAS24, HTalon::KAS25, HTalon::KAS26, HTalon::KAS27, HTalon::KAS28, HTalon::KAS29,
    HTalon::KAS30, HTalon::KAS31, HTalon::KAS32, HTalon::KAS33, HTalon::KAS34, HTalon::KAS35, HTalon::KAS36, HTalon::KAS37, HTalon::KAS38,
    HTalon::KAS41, HTalon::KAS42, HTalon::KAS43, HTalon::KAS44, HTalon::KAS45, HTalon::KAS46, HTalon::KAS47, HTalon::KAS48, HTalon::KAS49,
    HTalon::KAS50, HTalon::KAS51, HTalon::KAS52, HTalon::KAS53, HTalon::KAS54, HTalon::KAS55, HTalon::KAS56, HTalon::KAS57, HTalon::KAS58,
    HTalon::KAS61, HTalon::KAS62, HTalon::KAS63, HTalon::KAS64, HTalon::KAS65, HTalon::KAS66, HTalon::KAS67, HTalon::KAS68, HTalon::KAS69,
    HTalon::KAS70, HTalon::KAS71, HTalon::KAS72, HTalon::KAS73, HTalon::KAS74, HTalon::KAS75, HTalon::KAS76, HTalon::KAS77, HTalon::KAS78,
    HTalon::KBS01, HTalon::KBS02, HTalon::KBS03, HTalon::KBS04, HTalon::KBS05, HTalon::KBS06, HTalon::KBS07, HTalon::KBS08, HTalon::KBS09,
    HTalon::KBS10, HTalon::KBS11, HTalon::KBS12, HTalon::KBS13, HTalon::KBS14, HTalon::KBS15, HTalon::KBS16, HTalon::KBS17, HTalon::KBS18,
    HTalon::KBS21, HTalon::KBS22, HTalon::KBS23, HTalon::KBS24, HTalon::KBS25, HTalon::KBS26, HTalon::KBS27, HTalon::KBS28, HTalon::KBS29,
    HTalon::KBS30, HTalon::KBS31, HTalon::KBS32, HTalon::KBS33, HTalon::KBS34, HTalon::KBS35, HTalon::KBS36, HTalon::KBS37, HTalon::KBS38,
    HTalon::KBS41, HTalon::KBS42, HTalon::KBS43, HTalon::KBS44, HTalon::KBS45, HTalon::KBS46, HTalon::KBS47, HTalon::KBS48, HTalon::KBS49,
    HTalon::KBS50, HTalon::KBS51, HTalon::KBS52, HTalon::KBS53, HTalon::KBS54, HTalon::KBS55, HTalon::KBS56, HTalon::KBS57, HTalon::KBS58,
    HTalon::KBS61, HTalon::KBS62, HTalon::KBS63, HTalon::KBS64, HTalon::KBS65, HTalon::KBS66, HTalon::KBS67, HTalon::KBS68, HTalon::KBS69,
    HTalon::KBS70, HTalon::KBS71, HTalon::KBS72, HTalon::KBS73, HTalon::KBS74, HTalon::KBS75, HTalon::KBS76, HTalon::KBS77, HTalon::KBS78
  };

  static HTalon::RelayName const KYIXX[2][72] =
  {
    HTalon::KAI01, HTalon::KAI02, HTalon::KAI03, HTalon::KAI04, HTalon::KAI05, HTalon::KAI06, HTalon::KAI07, HTalon::KAI08, HTalon::KAI09,
    HTalon::KAI10, HTalon::KAI11, HTalon::KAI12, HTalon::KAI13, HTalon::KAI14, HTalon::KAI15, HTalon::KAI16, HTalon::KAI17, HTalon::KAI18,
    HTalon::KAI21, HTalon::KAI22, HTalon::KAI23, HTalon::KAI24, HTalon::KAI25, HTalon::KAI26, HTalon::KAI27, HTalon::KAI28, HTalon::KAI29,
    HTalon::KAI30, HTalon::KAI31, HTalon::KAI32, HTalon::KAI33, HTalon::KAI34, HTalon::KAI35, HTalon::KAI36, HTalon::KAI37, HTalon::KAI38,
    HTalon::KAI41, HTalon::KAI42, HTalon::KAI43, HTalon::KAI44, HTalon::KAI45, HTalon::KAI46, HTalon::KAI47, HTalon::KAI48, HTalon::KAI49,
    HTalon::KAI50, HTalon::KAI51, HTalon::KAI52, HTalon::KAI53, HTalon::KAI54, HTalon::KAI55, HTalon::KAI56, HTalon::KAI57, HTalon::KAI58,
    HTalon::KAI61, HTalon::KAI62, HTalon::KAI63, HTalon::KAI64, HTalon::KAI65, HTalon::KAI66, HTalon::KAI67, HTalon::KAI68, HTalon::KAI69,
    HTalon::KAI70, HTalon::KAI71, HTalon::KAI72, HTalon::KAI73, HTalon::KAI74, HTalon::KAI75, HTalon::KAI76, HTalon::KAI77, HTalon::KAI78,
    HTalon::KBI01, HTalon::KBI02, HTalon::KBI03, HTalon::KBI04, HTalon::KBI05, HTalon::KBI06, HTalon::KBI07, HTalon::KBI08, HTalon::KBI09,
    HTalon::KBI10, HTalon::KBI11, HTalon::KBI12, HTalon::KBI13, HTalon::KBI14, HTalon::KBI15, HTalon::KBI16, HTalon::KBI17, HTalon::KBI18,
    HTalon::KBI21, HTalon::KBI22, HTalon::KBI23, HTalon::KBI24, HTalon::KBI25, HTalon::KBI26, HTalon::KBI27, HTalon::KBI28, HTalon::KBI29,
    HTalon::KBI30, HTalon::KBI31, HTalon::KBI32, HTalon::KBI33, HTalon::KBI34, HTalon::KBI35, HTalon::KBI36, HTalon::KBI37, HTalon::KBI38,
    HTalon::KBI41, HTalon::KBI42, HTalon::KBI43, HTalon::KBI44, HTalon::KBI45, HTalon::KBI46, HTalon::KBI47, HTalon::KBI48, HTalon::KBI49,
    HTalon::KBI50, HTalon::KBI51, HTalon::KBI52, HTalon::KBI53, HTalon::KBI54, HTalon::KBI55, HTalon::KBI56, HTalon::KBI57, HTalon::KBI58,
    HTalon::KBI61, HTalon::KBI62, HTalon::KBI63, HTalon::KBI64, HTalon::KBI65, HTalon::KBI66, HTalon::KBI67, HTalon::KBI68, HTalon::KBI69,
    HTalon::KBI70, HTalon::KBI71, HTalon::KBI72, HTalon::KBI73, HTalon::KBI74, HTalon::KBI75, HTalon::KBI76, HTalon::KBI77, HTalon::KBI78
  };

  static HTalon::RelayName const KYGXX[2][72] =
  {
    HTalon::KAG01, HTalon::KAG02, HTalon::KAG03, HTalon::KAG04, HTalon::KAG05, HTalon::KAG06, HTalon::KAG07, HTalon::KAG08, HTalon::KAG09,
    HTalon::KAG10, HTalon::KAG11, HTalon::KAG12, HTalon::KAG13, HTalon::KAG14, HTalon::KAG15, HTalon::KAG16, HTalon::KAG17, HTalon::KAG18,
    HTalon::KAG21, HTalon::KAG22, HTalon::KAG23, HTalon::KAG24, HTalon::KAG25, HTalon::KAG26, HTalon::KAG27, HTalon::KAG28, HTalon::KAG29,
    HTalon::KAG30, HTalon::KAG31, HTalon::KAG32, HTalon::KAG33, HTalon::KAG34, HTalon::KAG35, HTalon::KAG36, HTalon::KAG37, HTalon::KAG38,
    HTalon::KAG41, HTalon::KAG42, HTalon::KAG43, HTalon::KAG44, HTalon::KAG45, HTalon::KAG46, HTalon::KAG47, HTalon::KAG48, HTalon::KAG49,
    HTalon::KAG50, HTalon::KAG51, HTalon::KAG52, HTalon::KAG53, HTalon::KAG54, HTalon::KAG55, HTalon::KAG56, HTalon::KAG57, HTalon::KAG58,
    HTalon::KAG61, HTalon::KAG62, HTalon::KAG63, HTalon::KAG64, HTalon::KAG65, HTalon::KAG66, HTalon::KAG67, HTalon::KAG68, HTalon::KAG69,
    HTalon::KAG70, HTalon::KAG71, HTalon::KAG72, HTalon::KAG73, HTalon::KAG74, HTalon::KAG75, HTalon::KAG76, HTalon::KAG77, HTalon::KAG78,
    HTalon::KBG01, HTalon::KBG02, HTalon::KBG03, HTalon::KBG04, HTalon::KBG05, HTalon::KBG06, HTalon::KBG07, HTalon::KBG08, HTalon::KBG09,
    HTalon::KBG10, HTalon::KBG11, HTalon::KBG12, HTalon::KBG13, HTalon::KBG14, HTalon::KBG15, HTalon::KBG16, HTalon::KBG17, HTalon::KBG18,
    HTalon::KBG21, HTalon::KBG22, HTalon::KBG23, HTalon::KBG24, HTalon::KBG25, HTalon::KBG26, HTalon::KBG27, HTalon::KBG28, HTalon::KBG29,
    HTalon::KBG30, HTalon::KBG31, HTalon::KBG32, HTalon::KBG33, HTalon::KBG34, HTalon::KBG35, HTalon::KBG36, HTalon::KBG37, HTalon::KBG38,
    HTalon::KBG41, HTalon::KBG42, HTalon::KBG43, HTalon::KBG44, HTalon::KBG45, HTalon::KBG46, HTalon::KBG47, HTalon::KBG48, HTalon::KBG49,
    HTalon::KBG50, HTalon::KBG51, HTalon::KBG52, HTalon::KBG53, HTalon::KBG54, HTalon::KBG55, HTalon::KBG56, HTalon::KBG57, HTalon::KBG58,
    HTalon::KBG61, HTalon::KBG62, HTalon::KBG63, HTalon::KBG64, HTalon::KBG65, HTalon::KBG66, HTalon::KBG67, HTalon::KBG68, HTalon::KBG69,
    HTalon::KBG70, HTalon::KBG71, HTalon::KBG72, HTalon::KBG73, HTalon::KBG74, HTalon::KBG75, HTalon::KBG76, HTalon::KBG77, HTalon::KBG78
  };
  
  HTalon * talonCard = TestTalon::getTalonCardAt(mod_id, slot);
  talonCard->openAllRelays(false, true);                      /* Initialize to all relays open.  No wait.  send. */
  
  vth_aunpowered_init(mod_id);                /* Initialization. */
  vth_ameas_set_averaging(mod_id, 1);         /* Make one measurement. */

  vth_aopen_all_rlys(mod_id);                               /* Initialize all AIM relays. */

  bool bNewSide;

  for (int nSide=0; nSide<2; nSide++)
  {
	bNewSide = true;
	for (int nChannel=0; nChannel<72; nChannel++)
	{
      vth_aclose_rly(mod_id, K7SY[nSide]);
	  talonCard->closeRelay(KLY[nSide],false,false);
	  talonCard->closeRelay(KSY[nSide],false,false);
	  talonCard->closeRelay(KGLY[nSide],false,false);
	  talonCard->closeRelay(KYSXX[nSide][nChannel],false,false);
	  talonCard->closeRelay(KYGXX[nSide][nChannel],false,true);

	  if (bNewSide)
	  {
		talon_buzz_that_relay(mod_id, KLY[nSide], talonCard);
		talon_buzz_that_relay(mod_id, KSY[nSide], talonCard);
		talon_buzz_that_relay(mod_id, KGLY[nSide], talonCard);
	  }
	  
	  talon_buzz_that_relay(mod_id, KYSXX[nSide][nChannel], talonCard);
	  talon_buzz_that_relay(mod_id, KYGXX[nSide][nChannel], talonCard);

	  talonCard->openAllRelays(false, true);                      /* Initialize to all relays open.  No wait.  send. */
      vth_aopen_rly(mod_id,K7SY[nSide]);

      vth_aclose_rly(mod_id, K7IY[nSide]);
	  talonCard->closeRelay(KLY[nSide],false,false);
	  talonCard->closeRelay(KIY[nSide],false,false);
	  talonCard->closeRelay(KGLY[nSide],false,false);
	  talonCard->closeRelay(KYIXX[nSide][nChannel],false,false);
	  talonCard->closeRelay(KYGXX[nSide][nChannel],false,true);
	  
	  if (bNewSide)
	  {
		talon_buzz_that_relay(mod_id, KIY[nSide], talonCard);
		bNewSide = false;
	  }
	  talon_buzz_that_relay(mod_id, KYIXX[nSide][nChannel], talonCard);

	  talonCard->openAllRelays(false, true);                     /* Initialize to all relays open.  No wait.  send. */
      vth_aopen_rly(mod_id,K7IY[nSide]);
	}
  }

  vth_aopen_all_rlys(mod_id);                               /* Initialize all AIM relays. */
  talonCard->openAllRelays(false, true);                      /* Initialize to all relays open.  No wait.  send. */

}/*end t16189*/


   
   
/*--------------------------------------------------------------------------------
* Test: 16200
* Description: Series S/I Bus Resistance
*				This test is modeled after test 9200.
* Fixture: Analog
* Resource(s): DMM
*--------------------------------------------------------------------------------
*/

#define RANGE				  3
#define WORST_SLOT			  11		/* Highest Xn Bus Resistance */
#define RESOLUTION			  1.0e-03
#define DGN_INFINITY		  1e+31
#define OFFSET_OHMS_PER_SLOT  39e-03	/* Amount MB R per slot */
	  
void t16200(int mod_id, int slot)
{
  double slot_correction;   
  double res;
  int channel = 'B';
  test_result_type result;
  vth_cfxt_lock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
 
  static HTalon::RelayName const I_A[2] = {HTalon::KAI02, HTalon::KAI04};
  static HTalon::RelayName const I_B[2] = {HTalon::KBI02, HTalon::KBI04};
  static HTalon::RelayName const S_A[2] = {HTalon::KAS02, HTalon::KAS04};
  static HTalon::RelayName const S_B[2] = {HTalon::KBS02, HTalon::KBS04};
  static HTalon::RelayName const SUB_I_A[3] = {KIA, KBA, KIBA};
  static HTalon::RelayName const SUB_I_B[3] = {KIB, KBB, KIBB};
  static HTalon::RelayName const SUB_S_A[3] = {KSA, KAA, KSAA};
  static HTalon::RelayName const SUB_S_B[3] = {KSB, KAB, KSAB};
  
  static unsigned int const n = 4;
  static relay_set const tps[n] =
  {
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
	  relay_set(R(S_A), R(SUB_S_A)),  // x1:x2
	  relay_set(R(I_A), R(SUB_I_A)),  // x3:x4
	  relay_set(R(S_B), R(SUB_S_B)),  // x5:x6
	  relay_set(R(I_B), R(SUB_I_B)),  // x7:x8
		  
#undef R
  };
  
  HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
  vth_aopen_all_rlys(mod_id);				// Initialize Aim 
  talonCard->openAllRelays(false, true);	// Initalize Talon
  OpenResourceManager();
  open_mux(mod_id);
  open_dmm(mod_id);
  
  setup_dmm(mod_id, FOUR_WIRE_OHMS, (double)RANGE, RESOLUTION);
  setup_dmm_ocomp_on(mod_id);
  connect_dmm(mod_id);
  
  for (unsigned int p = 0; p < n; ++p)
  {/*for: side*/
	  close(mod_id, slot, tps[p].rut, tps[p].ref, talonCard);
	  connect_4wire_talon(mod_id, true, p);
	  connect_minus_pin(mod_id, slot, channel, true);
	  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);		
	  get_result_nolog(mod_id, &result);
	  res = measure_dmm(mod_id);
	  if (fabs(res) > DGN_INFINITY)
		  res = DGN_INFINITY - 1;
	  /* Make all slots log vaules as if they were in slot 11 (Worst case) */
	  slot_correction = (WORST_SLOT - slot) * OFFSET_OHMS_PER_SLOT;
	  res += slot_correction;
	  result.Measurement_class = CLASS_REAL;
      result.UU.Real_result = fabs(res);
	  log_result(mod_id, result);
	  talonCard->openAllRelays(false, true);	
	  vth_aopen_all_rlys(mod_id);				// Re-initialize Aim 
	  connect_4wire_talon(mod_id, false, p);
	  connect_minus_pin(mod_id, slot, channel, false);
	  flush_commands(mod_id);
	  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
  }/*for: side*/
  
  vth_ainit(mod_id);
  vth_pinit(mod_id, slot);
  close_mux(mod_id);
  close_dmm(mod_id);
  CloseResourceManager();
  
}/*t16200*/

#undef RANGE
#undef WORST_SLOT
#undef RESOLUTION
#undef DGN_INFINITY
#undef OFFSET_OHMS_PER_SLOT

  
/*
*--------------------------------------------------------------------------------
* Test: 16201
* Description: G bus resistance
*				This test is modeled after test 9201.
* Fixture: Analog
* Resource(s): DMM
*--------------------------------------------------------------------------------
*/

#define CHANNEL         'A'
#define RANGE           3
#define RESOLUTION      1.0e-03
#define DGN_INFINITY    1e+31

void t16201(int mod_id, int slot)
{
  double res;
  test_result_type result;
  OpenResourceManager();
  open_mux(mod_id);
  open_dmm(mod_id);
  vth_cfxt_lock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  
  static HTalon::RelayName const G_A[3] = {HTalon::KAG15, HTalon::KAG17, HTalon::KGA};
  static HTalon::RelayName const G_B[3] = {HTalon::KBG15, HTalon::KBG17, HTalon::KGB};
  
  static unsigned int const n = 2;
  static rlylist const tps[n] =
  {
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
	  rlylist(R(G_A)),
	  rlylist(R(G_B)), 
		  
#undef R
  };
  
  setup_dmm(mod_id, FOUR_WIRE_OHMS, (double)RANGE, RESOLUTION);
  setup_dmm_ocomp_on(mod_id);
  connect_dmm(mod_id);
  HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
  vth_aopen_all_rlys(mod_id);				// Initialize Aim 
  talonCard->openAllRelays(false, true);	// Initalize Talon
  for (unsigned int p = 0; p < n; ++p)
  {/*for: side*/
	  sglset_close(mod_id, slot, tps[p].ref, talonCard);
	  vth_aclose_rly(mod_id, K752);
	  vth_aclose_rly(mod_id, K760);
	  connect_minus_pin(mod_id, slot, CHANNEL, true);
	  connect_plus_gout(mod_id);
	  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);		
	  get_result_nolog(mod_id, &result);
	  res = measure_dmm(mod_id);
	  if (fabs(res) > DGN_INFINITY)
	  {
		  display_prompt("No result in result log (Expected dmm R)", true);
		  res = DGN_INFINITY - 1;
	  } 
	  result.Measurement_class = CLASS_REAL;
	  result.UU.Real_result = fabs(res);
	  log_result(mod_id, result);
	  vth_aopen_all_rlys(mod_id);
	  talonCard->openAllRelays(false, true);
	  flush_commands(mod_id);		
  }/*for: side*/
  
  close_mux(mod_id);
  close_dmm(mod_id);
  CloseResourceManager();
  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);
  
} /*t16201*/

#undef CHANNEL
#undef RANGE
#undef RESOLUTION
#undef DGN_INFINITY

  
/*
*--------------------------------------------------------------------------------
* Test: 16202
* Description: Series S/I bus inductance
*				This test is modeled after test 9210.
* Fixture: Analog
* Resource(s): LCR
*--------------------------------------------------------------------------------
*/
  
#define LCR_MULT        5
#define LCR_FREQ        100e+03
#define LCR_FUNCTION    INDUCTANCE
#define LCR_DISPLAY     Q
#define LCR_CKT_MODE    SERIES
  
void t16202(int mod_id, int slot)
{
  int channel = 'B';
  double henries;
  test_result_type result;
  OpenResourceManager();
  open_mux(mod_id);
  open_lcr(mod_id);
  vth_cfxt_lock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  static HTalon::RelayName const S_A[2] = {HTalon::KAS02, HTalon::KAS04};
  static HTalon::RelayName const I_A[2] = {HTalon::KAI02, HTalon::KAI04};
  static HTalon::RelayName const S_B[2] = {HTalon::KBS02, HTalon::KBS04};
  static HTalon::RelayName const I_B[2] = {HTalon::KBI02, HTalon::KBI04};
  static HTalon::RelayName const SUB_S_A[3] = {KSA, KAA, KSAA};
  static HTalon::RelayName const SUB_I_A[3] = {KIA, KBA, KIBA};
  static HTalon::RelayName const SUB_S_B[3] = {KSB, KAB, KSAB};
  static HTalon::RelayName const SUB_I_B[3] = {KIB, KBB, KIBB};
  
  static unsigned int const n = 4;
  static relay_set const tps[n] =
  {
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
	  relay_set(R(S_A), R(SUB_S_A)),
	  relay_set(R(I_A), R(SUB_I_A)), 
	  relay_set(R(S_B), R(SUB_S_B)),
	  relay_set(R(I_B), R(SUB_I_B)), 
#undef R
  };
  
  setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,
	  LCR_MULT);
  connect_lcr(mod_id);
  HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
  vth_aopen_all_rlys(mod_id);				// Initialize Aim 
  talonCard->openAllRelays(false, true);	// Initalize Talon

  for (unsigned int p = 0; p < n; ++p)
  {/*for: side*/
	  close(mod_id, slot, tps[p].rut, tps[p].ref, talonCard);
	  get_result_nolog(mod_id, &result);
	  connect_4wire_talon(mod_id, true, p);
	  vth_mtmr_wait(mod_id, WAIT_TIME);		
	  connect_minus_pin(mod_id, slot, channel, true);
	  if(p == 0)								// this was put in to "prime" the lcr meter
		  measure_lcr(mod_id);
	  henries = measure_lcr(mod_id);
	  result.Measurement_class = CLASS_REAL;
	  result.UU.Real_result = fabs(henries);
	  log_result(mod_id, result);
  	  vth_aopen_all_rlys(mod_id);
	  talonCard->openAllRelays(false, true);
	  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
  }  /*for*/
  
  vth_ainit(mod_id);
  vth_pinit(mod_id, slot);
  close_mux(mod_id);
  close_lcr(mod_id);
  CloseResourceManager();
  
}  /*t16202*/

#undef LCR_MULT
#undef LCR_FREQ
#undef LCR_FUNCTION
#undef LCR_DISPLAY
#undef LCR_CKT_MODE


/*
*--------------------------------------------------------------------------------
* Test: 16203
* Description: Series G bus inductance
*				This test is modeled after test 9211.
* Fixture: Analog
* Resource(s): LCR
*--------------------------------------------------------------------------------
*/

#define CHANNEL         'A'
#define LCR_MULT        5
#define LCR_FREQ        100e+03
#define LCR_FUNCTION    INDUCTANCE
#define LCR_DISPLAY     Q
#define LCR_CKT_MODE    SERIES
 
void t16203(int mod_id, int slot)
{
	double henries;
	test_result_type result;
	result.Measurement_class = CLASS_REAL;
	OpenResourceManager();
	open_mux(mod_id);
	open_lcr(mod_id);
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	
	static HTalon::RelayName const G_A[3] = {HTalon::KAG02, HTalon::KAG04, HTalon::KGA};
	static HTalon::RelayName const G_B[3] = {HTalon::KBG02, HTalon::KBG04, HTalon::KGB};
	
	static unsigned int const n = 2;
	static rlylist const tps[n] =
	{
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
		rlylist(R(G_A)),
		rlylist(R(G_B)), 
#undef R
	};
	
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	vth_aopen_all_rlys(mod_id);				// Initialize Aim 
	talonCard->openAllRelays(true, true);	// Initalize Talon
	setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,
		LCR_MULT);
	vth_aclose_rly(mod_id, K752);
	vth_aclose_rly(mod_id, K760);
	connect_lcr(mod_id);
	connect_minus_pin(mod_id, slot, CHANNEL, true);
	connect_plus_gout(mod_id);
	
	for (unsigned int side = SIDEA; side <= SIDEB; ++side)
	{/*for: side*/
		sglset_close(mod_id, slot, tps[side].ref, talonCard);
		vth_mtmr_wait(mod_id, WAIT_TIME);
		flush_commands(mod_id);
		henries = measure_lcr(mod_id);
		result.UU.Real_result = fabs(henries);
		log_result(mod_id, result);
	    talonCard->openAllRelays(true, true);	
		flush_commands(mod_id);
	}/*for: side*/
	
	close_mux(mod_id);
	close_lcr(mod_id);
	CloseResourceManager();
    vth_pinit(mod_id, slot);
    vth_ainit(mod_id);
	
}  /*t16203*/

#undef CHANNEL
#undef LCR_MULT
#undef LCR_FREQ
#undef LCR_FUNCTION
#undef LCR_DISPLAY
#undef LCR_CKT_MODE		


/*
*--------------------------------------------------------------------------------
* Test: 16204
* Description: S/I/B/A parallel bus capacitance
*				This test is modeled after test 9220.
* Fixture: Analog
* Resource(s): LCR
*--------------------------------------------------------------------------------
*/

#define LCR_MULT        5
#define LCR_FREQ        100e+03
#define LCR_FUNCTION    CAPACITANCE
#define LCR_DISPLAY     Q
#define LCR_CKT_MODE    PARALLEL

void t16204(int mod_id, int slot)
{
	double farads;
	double correction;
	test_result_type result;
	result.Measurement_class = CLASS_REAL;
	OpenResourceManager();
	open_mux(mod_id);
	open_lcr(mod_id);
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	
	static HTalon::RelayName const S_AB[2] = {KSA, KSB};
	static HTalon::RelayName const I_AB[2] = {KIA, KIB};
	static HTalon::RelayName const A_AB[4] = {KSAA, KAA, KSAB, KAB};
	static HTalon::RelayName const B_AB[4] = {KIBA, KBA, KIBB, KBB};
	
	static unsigned int const n = 4;
	static rlylist const tps[n] =
	{
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
		rlylist(R(S_AB)),
		rlylist(R(I_AB)),
		rlylist(R(A_AB)),
		rlylist(R(B_AB)),
			
#undef R
	};
	
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	vth_aopen_all_rlys(mod_id);				// Initialize Aim 
	talonCard->openAllRelays(true, true);	// Initalize Talon
	   
	correction = calc_mod_loading(mod_id, .7e-12);
	setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,
		LCR_MULT);
	connect_dg(mod_id, true);
	connect_lcr(mod_id);
	
	for (unsigned int p = 0; p < n; ++p)
	{/*for: side*/
		sglset_close(mod_id, slot, tps[p].ref, talonCard);
		for(unsigned int j = 0; j <= 7; j++)
			vth_aclose_rly(mod_id, AIM_X_L_RLYS[j]);
					
	    vth_aclose_rly(mod_id, K857);	
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);		// Capacitive bleed off	
		vth_aopen_all_rlys(mod_id);							// Re-initialize Aim 
		vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
		connect_plus_xn_talon(mod_id, true, p);
		connect_minus_xn_plus_1_talon(mod_id, true);   
		vth_mtmr_wait(mod_id, WAIT_TIME);
		flush_commands(mod_id);
		farads = measure_lcr(mod_id);
		result.UU.Real_result = farads - correction;
		log_result(mod_id, result);
		vth_aopen_all_rlys(mod_id);					 
		talonCard->openAllRelays(false, true);	
		connect_plus_xn_talon(mod_id, false, p);
		connect_minus_xn_plus_1_talon(mod_id, false);   
		vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
		get_result_nolog(mod_id, &result);
	}  /*for: side*/
	
	vth_ainit(mod_id);
	vth_pinit(mod_id, slot);
	close_mux(mod_id);
	close_lcr(mod_id);
	CloseResourceManager();
	
}/*t16204*/

#undef LCR_MULT
#undef LCR_FREQ
#undef LCR_FUNCTION
#undef LCR_DISPLAY
#undef LCR_CKT_MODE


/*
*--------------------------------------------------------------------------------
* Test: 16205
* Description: G bus parallel capacitance 
*				This test is modeled after test 9221.
* Fixture: Analog
* Resource(s): LCR
*--------------------------------------------------------------------------------
*/

#define LCR_MULT        5
#define LCR_FREQ        100e+03
#define LCR_FUNCTION    CAPACITANCE
#define LCR_DISPLAY     Q
#define LCR_CKT_MODE    PARALLEL

void t16205(int mod_id, int slot)
{
	 int channel = 'H';
	 double farads;
	 test_result_type result;
	 vth_cfxt_lock();
     vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

	 static HTalon::RelayName const G_A[3] = {KGA, KSA, KIA};
	 static HTalon::RelayName const G_B[3] = {KGB, KSB, KIB};
	 static HTalon::RelayName const L_A[4] = {KGA, KSA, KIA, KGLA};
	 static HTalon::RelayName const L_B[4] = {KGB, KSB, KIB, KGLB};
 
	 static unsigned int const n = 4;
	 static rlylist const tps[n] =
	 {
	#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
		 rlylist(R(G_A)),
		 rlylist(R(G_B)),
		 rlylist(R(L_A)),
		 rlylist(R(L_B)),
			 
	#undef R
	 };
 
	 HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	 vth_aopen_all_rlys(mod_id);				// Initialize Aim 
	 talonCard->openAllRelays(false, true);	// Initalize Talon
	 OpenResourceManager();
	 open_mux(mod_id);
	 open_lcr(mod_id);
 
	 setup_lcr(mod_id, LCR_FUNCTION, LCR_DISPLAY, LCR_CKT_MODE, LCR_FREQ,
		 LCR_MULT);
 
	 connect_dg(mod_id, true);
	 connect_lcr(mod_id);
	 connect_minus_xn_plus_1(mod_id, channel, true);   
	 connect_plus_gout(mod_id);
 
	 for (unsigned int p = 0; p < n; ++p)
	 {/*for: side*/
		 sglset_close(mod_id, slot, tps[p].ref, talonCard);
		 vth_aclose_rly(mod_id, K752);		// Gout to XG relay       
		 vth_aclose_rly(mod_id, K760);		// Gout to XG relay       
		 vth_aclose_rly(mod_id, K741);		// X1 to mint field relay 
		 for(unsigned int i = 0; i <= 7; i++)
			 vth_aclose_rly(mod_id, AIM_X_B_RLYS[i]);	// Connect X1-X8
		 
		 vth_mtmr_wait(mod_id, WAIT_TIME);
		 flush_commands(mod_id);
		 farads = measure_lcr(mod_id);
		 if (mod_id == 0 || mod_id == 3)
			 farads -= Cap.sales_rack_offset_xg_cap[0];
		 else
			 farads -= Cap.sales_rack_offset_xg_cap[1];
		 result.Measurement_class = CLASS_REAL;
		 result.UU.Real_result = farads;
		 log_result(mod_id, result);
		 vth_aopen_all_rlys(mod_id);				
		 talonCard->openAllRelays(true, true);	
		 flush_commands(mod_id);
 }/*for: side*/

 close_mux(mod_id);
 close_lcr(mod_id);
 CloseResourceManager();
 vth_pinit(mod_id, slot);
 vth_ainit(mod_id);
 
} /*t16205*/

#undef LCR_MULT
#undef LCR_FREQ
#undef LCR_FUNCTION
#undef LCR_DISPLAY
#undef LCR_CKT_MODE
 
 
/*
*--------------------------------------------------------------------------------
* Test: 16206
* Description: Parallel Xn Bus Resistance 
*				This test is modeled after test 9208.
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/

#define SALES_LONG		1
#define AVERAGE         10
#define CPU             0
#define VEXP            10.0
#define TOUT            1.0
#define RFB_1MEG        5
#define MODE_2WIRE      0
#define VPLUS           10.0
#define VMINUS          -10.0

void t16206(int mod_id, int slot)
{
   int channel = 0;
//   int len;
//   int next;
   double resistance_xbus;
   double rfb;
   test_result_type result;
   vth_cfxt_unlock();
   vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
//   s80 message;
  
   HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
   vth_aopen_all_rlys(mod_id);				 // Initialize Aim 
   talonCard->openAllRelays(false, true);    // Initialize Talon 
   
   /* setup the MOA for resistor measurements */
   rfb = setup_moa_resistance_meas(mod_id, RFB_1MEG, MODE_2WIRE);

   /* setup the DETECTOR for DCV measurements */
   vth_dcv_setup(mod_id, SALES_LONG, AVERAGE, CPU, VEXP, TOUT);
   vth_aclose_rly(mod_id, K2303);	/* DETLO to ACOM internally */
   vth_aopen_rly(mod_id, K2305);  
   vth_aopen_rly(mod_id, K820);		/* disconnect L from DETLO */
   vth_aopen_rly(mod_id, K812);		/* remove DETHI from I */
   vth_aclose_rly(mod_id, K857);	/* connect L to ACOM */
   
   /* setup the SOURCE for DCV */
   vth_aclose_rly(mod_id, K806);   /* S to SRCOUT */
   vth_asrc_sel_fltr(mod_id, FILTER_128);
   vth_afc_en_src_out(mod_id);
   for(int j = 0; j < 8; j++)
   {
	   vth_aclose_rly(mod_id, AIM_X_I_RLYS[j]); // connect Xn + 8 to I
	   vth_aclose_rly(mod_id, AIM_X_L_RLYS[j]);	// inital discharge of X bus
   }
   
   for(int Xn = 0; Xn < 8; Xn++)
   {//measure the resistance
	   for(int i = 0; i < 8; i++)
		   vth_aopen_rly(mod_id, AIM_X_L_RLYS[i]); // disconnect ACOM from X buses

	   vth_aclose_rly(mod_id, AIM_X_S_RLYS[Xn]);
	   vth_aopen_rly(mod_id, AIM_X_I_RLYS[Xn]);
	   vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
	   resistance_xbus = measure_open(mod_id, VPLUS, VMINUS, rfb);
	   if (resistance_xbus < -2.0e+11)
		   resistance_xbus = R_MAX_PARALLEL;
	   else
	   {
		   if (resistance_xbus < -1.0e+11)
		   {   /* moderate negative < 200uV noise */
			   resistance_xbus = R_MAX_PARALLEL; 
			   
			   /* warn the operator of a potential measurement problem */
/*			   strcpy(message,
				   "WARNING: High-Z measurement problem, Absolute value required");
			   display_prompt(message, true);
			   strcpy(message, "Test 16206 Module ");
			   len = strlen(message) + 1;
			   sprintf(message + len - 1, "%d", mod_id);
			   next = strlen(message) + 1;
			   strcat(message, " Slot ");
			   len = strlen(message) + 1;
			   sprintf(message + len - 1, "%d", slot);
			   next = strlen(message) + 1;
			   strcat(message, " X-bus ");
			   len = strlen(message) + 1;
			   sprintf(message + len - 1, "%d", Xn + 1);
			   next = strlen(message) + 1;
			   display_prompt(message, true); */
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
			   }
		   }
	   }
	   result.Measurement_class = CLASS_REAL;
	   result.UU.Real_result = log10(resistance_xbus);
	   log_result(mod_id, result);
   	   for(int var = 0; var < 8; var++)
          vth_aclose_rly(mod_id, AIM_X_L_RLYS[var]); // Discharge X buses

   	   vth_aopen_rly(mod_id, AIM_X_S_RLYS[Xn]);		// Reset S-bus disconnects
	   vth_aclose_rly(mod_id, AIM_X_I_RLYS[Xn]);	// Reset all I-bus disconnects to closed
	   vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
   } /*for: measurement*/
   vth_pinit(mod_id, slot);
   vth_ainit(mod_id);

} /*t16206*/

#undef SALES_LONG
#undef AVERAGE
#undef CPU
#undef VEXP
#undef TOUT
#undef RFB_1MEG
#undef MODE_2WIRE
#undef VPLUS
#undef VMINUS


/*
*--------------------------------------------------------------------------------
* Test: 16207
* Description: I-bus relay resistance 
*				This test is modeled after test 9207.
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/

#define SALES_LONG      1
#define AVERAGE         10
#define CPU             0
#define VEXP            10.0
#define TOUT            1.0
#define RFB_1MEG        5
#define MODE_2WIRE      0
#define VPLUS           10.0
#define VMINUS          -10.0

void t16207(int mod_id, int slot)
{
  int channel = 0;
  int len;
  double resistance_xbus[72][2];
  double measured;
  double rfb;
  test_result_type result;
  vth_cfxt_unlock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  s80 message2;

  HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
  vth_aopen_all_rlys(mod_id);		// Initialize Aim 
  talonCard->openAllRelays(false, true);
  
  for (int side = SIDEA; side <= SIDEB; ++side)
  {/*for: side*/
	  rfb = setup_moa_resistance_meas(mod_id, RFB_1MEG, MODE_2WIRE);	  
	  // setup the DETECTOR for DCV measurements
	  vth_dcv_setup(mod_id, SALES_LONG, AVERAGE, CPU, VEXP, TOUT);
	  vth_aclose_rly(mod_id, K2303);    // DETLO to ACOM internally 
	  vth_aopen_rly(mod_id, K2305);
	  vth_aopen_rly(mod_id, K820);		// disconnect L from DETLO 
	  vth_aopen_rly(mod_id, K812);		// remove I from DETHI 
	  vth_aclose_rly(mod_id, K857);		// L to ACOM 
	  
	  // setup the SOURCE for DCV 
	  vth_aclose_rly(mod_id, K806);		// S to SRCOUT 
	  vth_asrc_sel_fltr(mod_id, FILTER_128);
	  vth_afc_en_src_out(mod_id);  
	  for(int n = 0; n < 8; n++)
		  vth_aclose_rly(mod_id, AIM_X_L_RLYS[n]);	//discharge x-bus
  	  talonCard->closeRelay(KSA, false, false);
	  talonCard->closeRelay(KSB, false, false);
	  talonCard->closeRelay(KIA, false, false);
	  talonCard->closeRelay(KIB, false, true);
	  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);	
	  vth_aopen_rly(mod_id, K719);		
	  vth_aopen_rly(mod_id, K723);		
	  vth_aopen_rly(mod_id, K717);
	  vth_aopen_rly(mod_id, K721);
	  talonCard->openAllRelays(false, true);
	  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);	
	  if (side == SIDEA)	
	  { //submux setup
		  vth_aclose_rly(mod_id, K727);	// connect A side I-bus to MOAINV thru x3
		  vth_aclose_rly(mod_id, K733);	// connect A side S-bus to Source thru x1
 		  vth_aclose_rly(mod_id, K721); // ground x5
		  vth_aclose_rly(mod_id, K723); // ground x7
	  }
	  else
	  {
		  vth_aclose_rly(mod_id, K731); // connect B side I-bus to MOAINV thru x7
		  vth_aclose_rly(mod_id, K737); // connect B side S-bus to Source thru x5
		  vth_aclose_rly(mod_id, K717); // ground x1
		  vth_aclose_rly(mod_id, K719); // ground x3
	  }

	  for(int chnl = 0; chnl < 72; ++chnl) 
	  {/*for: measurement*/
		  talonCard->closeRelay(side == SIDEA ? KSA : KSB, false, false);
		  talonCard->closeRelay(side == SIDEA ? KIA : KIB, false, false);
		  talonCard->closeRelay(side == SIDEA ? Ibus_A[chnl] : Ibus_B[chnl], false, false);
		  switch (chnl)
		  {
		  case (0):
			  talonCard->closeRelay(side == SIDEA ? Sbus_A[chnl + 1] : Sbus_B[chnl + 1], true, true);
			  break;
		  case (71):
			  talonCard->closeRelay(side == SIDEA ? Sbus_A[chnl - 1] : Sbus_B[chnl - 1], true, true);
			  break;
		  default:
  			  talonCard->closeRelay(side == SIDEA ? Sbus_A[chnl + 1] : Sbus_B[chnl + 1], false, false);
			  talonCard->closeRelay(side == SIDEA ? Sbus_A[chnl - 1] : Sbus_B[chnl - 1], true, true);
			  break;
		  }

		  
		  measured = measure_open(mod_id, VPLUS, VMINUS, rfb);
		  if (measured < -2.0e+11)
			  measured = R_MAX_PARALLEL;
		  else
		  {
			  if (measured < -1.0e+11)
			  {   /* moderate negative < 200uV noise */
				  measured = R_MAX_PARALLEL; 
				  
				  /* warn the operator of a potential measurement problem */
				  channel = (side == SIDEA ? (chnl + 1) : (chnl + 101));
//				  display_prompt(message, true);
				  strcpy(message2, "Test 16207 Module ");
				  len = strlen(message2) + 1;
				  sprintf(message2 + len - 1, "%d\t%s%d\t%s%d", mod_id, "Slot ", slot, 
					  "Channel ", channel);
				  display_prompt(message2, true); 
			  }
			  else
			  {
				  if (measured < 0)
				  {   /* small negative > 200uV noise */
					  measured = -1.0 * measured;
				  }
				  else
				  {  /* positive value */
					  if(measured > R_MAX_PARALLEL)
						  measured = R_MAX_PARALLEL;
				  }
			  }
		  }
		  resistance_xbus[chnl][side] = log10(measured);		 
		  talonCard->openAllRelays(true, true);   
	  } /*for: measurement*/
	  vth_aopen_all_rlys(mod_id);

	} /*for: side*/ 

	for(int i = 0; i < 2; ++i)
	{
		for(int j = 0; j < 72; ++j)
		{
		   result.Measurement_class = CLASS_REAL;
		   result.UU.Real_result = resistance_xbus[j][i];
		   log_result(mod_id, result);
		}
	}

    vth_pinit(mod_id, slot);
    vth_ainit(mod_id);

} /*t16207*/

#undef SALES_LONG
#undef AVERAGE
#undef CPU
#undef VEXP
#undef TOUT
#undef RFB_1MEG
#undef MODE_2WIRE
#undef VPLUS
#undef VMINUS


/*
*--------------------------------------------------------------------------------
* Test: 16208
* Description: G-bus relay resistance 
*				This test is modeled after test 9207.
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/

#define SALES_LONG      1
#define AVERAGE         10
#define CPU             0
#define VEXP            10.0
#define TOUT            1.0
#define RFB_1MEG        5
#define MODE_2WIRE      0
#define VPLUS           10.0
#define VMINUS          -10.0

void t16208(int mod_id, int slot)
{
  int channel = 0;
  double resistance_xbus[72][2];
  double measured;
  double rfb;
  test_result_type result;
  vth_cfxt_unlock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);

  HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
  vth_aopen_all_rlys(mod_id); // Initialize Aim 
  talonCard->openAllRelays(false, true);
  
  for (int side = SIDEA; side <= SIDEB; ++side)
  {/*for: side*/
	  rfb = setup_moa_resistance_meas(mod_id, RFB_1MEG, MODE_2WIRE);	  
	  // setup the DETECTOR for DCV measurements
	  vth_dcv_setup(mod_id, SALES_LONG, AVERAGE, CPU, VEXP, TOUT);
	  vth_aclose_rly(mod_id, K2303);    // DETLO to ACOM internally 
	  vth_aopen_rly(mod_id, K2305);
	  vth_aopen_rly(mod_id, K820);		// disconnect L from DETLO 
	  vth_aopen_rly(mod_id, K812);		// remove I from DETHI 
	  vth_aclose_rly(mod_id, K857);		// L to ACOM 
	  
	  // setup the SOURCE for DCV 
	  vth_aclose_rly(mod_id, K806);		// S to SRCOUT 
	  vth_asrc_sel_fltr(mod_id, FILTER_128);
	  vth_afc_en_src_out(mod_id); 
	  for(int n = 0; n < 8; n++)
		  vth_aclose_rly(mod_id, AIM_X_L_RLYS[n]);	//discharge x-bus
  	  talonCard->closeRelay(KSA, false, false);
	  talonCard->closeRelay(KSB, false, false);
	  talonCard->closeRelay(KIA, false, false);
	  talonCard->closeRelay(KIB, false, true);
	  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);	
	  vth_aopen_rly(mod_id, K719);		
	  vth_aopen_rly(mod_id, K723);		
	  vth_aopen_rly(mod_id, K717);
	  vth_aopen_rly(mod_id, K721);
	  talonCard->openAllRelays(false, true);
	  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);	
	  flush_commands(mod_id);
	  if (side == SIDEA)	
	  { //submux setup
		  vth_aclose_rly(mod_id, K727);	// connect A side I-bus to MOAINV thru x3
		  vth_aclose_rly(mod_id, K733);	// connect A side S-bus to Source thru x1
 		  vth_aclose_rly(mod_id, K721); // ground x5
		  vth_aclose_rly(mod_id, K723); // ground x7
	  }
	  else
	  {
		  vth_aclose_rly(mod_id, K731); // connect B side I-bus to MOAINV thru x7
		  vth_aclose_rly(mod_id, K737); // connect B side S-bus to Source thru x5
		  vth_aclose_rly(mod_id, K717); // ground x1
		  vth_aclose_rly(mod_id, K719); // ground x3
	  }

	  for(int chnl = 0; chnl < 72; ++chnl)
	  {/*for: measurement*/
		  talonCard->closeRelay(side == SIDEA ? KSA : KSB, false, false);
		  talonCard->closeRelay(side == SIDEA ? KIA : KIB, false, false);
 		  talonCard->closeRelay(side == SIDEA ? Gbus_A[chnl] : Gbus_B[chnl], false, false);  //no wait, no send
		  talonCard->closeRelay(side == SIDEA ? Ibus_A[chnl] : Ibus_B[chnl], false, false);  //Using I bus to measure G rlys
		  switch (chnl)
		  {
		  case (0):
			  talonCard->closeRelay(side == SIDEA ? Sbus_A[chnl + 1] : Sbus_B[chnl + 1], true, true);
			  break;
		  case (71):
			  talonCard->closeRelay(side == SIDEA ? Sbus_A[chnl - 1] : Sbus_B[chnl - 1], true, true);
			  break;
		  default:
  			  talonCard->closeRelay(side == SIDEA ? Sbus_A[chnl + 1] : Sbus_B[chnl + 1], false, false);
			  talonCard->closeRelay(side == SIDEA ? Sbus_A[chnl - 1] : Sbus_B[chnl - 1], true, true);
			  break;
		  }

		  measured = measure_open(mod_id, VPLUS, VMINUS, rfb);
		  if (measured < -1.0e+11)
			  measured = R_MAX_PARALLEL;
		  else
		  {
			  if (measured < 0)
				  measured = -1.0 * measured;
			  else
			  {  /* positive value */
				  if (measured > R_MAX_PARALLEL)
					  measured = R_MAX_PARALLEL;
			  }
		  }
		  resistance_xbus[chnl][side] = log10(measured);
		  talonCard->openAllRelays(true, true);   
	  } /*for: measurement*/
	  vth_aopen_all_rlys(mod_id);
	}/* for: side */

  	for(int z = 0; z < 2; ++z)
	{
		for(int j = 0; j < 72; ++j)
		{
		   result.Measurement_class = CLASS_REAL;
		   result.UU.Real_result = resistance_xbus[j][z];
		   log_result(mod_id, result);
		}
	}

   vth_pinit(mod_id, slot);
   vth_ainit(mod_id);

} /*t16208*/

#undef SALES_LONG
#undef AVERAGE
#undef CPU
#undef VEXP
#undef TOUT
#undef RFB_1MEG
#undef MODE_2WIRE
#undef VPLUS
#undef VMINUS


/*
*--------------------------------------------------------------------------------
* Test: 16209
* Description: S-bus relay resistance 
*				This test is modeled after test 9207.
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/

#define SALES_LONG      1
#define AVERAGE         10
#define CPU             0
#define VEXP            10.0
#define TOUT            1.0
#define RFB_1MEG        5
#define MODE_2WIRE      0
#define VPLUS           10.0
#define VMINUS          -10.0

void t16209(int mod_id, int slot)
{
  int channel = 0;
  int len;
  double resistance_xbus[72][2];
  double measured;
  double rfb;
  test_result_type result;
  vth_cfxt_unlock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  s80 message2;
  
  HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
  vth_aopen_all_rlys(mod_id);		// Initialize Aim 
  talonCard->openAllRelays(false, true);
  
  for (int side = SIDEA; side <= SIDEB; ++side)
  {/*for: side*/
	  rfb = setup_moa_resistance_meas(mod_id, RFB_1MEG, MODE_2WIRE);	  
	  // setup the DETECTOR for DCV measurements
	  vth_dcv_setup(mod_id, SALES_LONG, AVERAGE, CPU, VEXP, TOUT);
	  vth_aclose_rly(mod_id, K2303);    // DETLO to ACOM internally 
	  vth_aopen_rly(mod_id, K2305);
	  vth_aopen_rly(mod_id, K820);		// disconnect L from DETLO 
	  vth_aopen_rly(mod_id, K812);		// remove I from DETHI 
	  vth_aclose_rly(mod_id, K857);		// L to ACOM 
	  
	  // setup the SOURCE for DCV 
	  vth_aclose_rly(mod_id, K806);		// S to SRCOUT 
	  vth_asrc_sel_fltr(mod_id, FILTER_128);
	  vth_afc_en_src_out(mod_id);
	  for(int n = 0; n < 8; n++)
		  vth_aclose_rly(mod_id, AIM_X_L_RLYS[n]);	//discharge x-bus
  	  talonCard->closeRelay(KSA, false, false);
	  talonCard->closeRelay(KSB, false, false);
	  talonCard->closeRelay(KIA, false, false);
	  talonCard->closeRelay(KIB, false, true);
	  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);	
	  vth_aopen_rly(mod_id, K719);		
	  vth_aopen_rly(mod_id, K723);		
	  vth_aopen_rly(mod_id, K717);
	  vth_aopen_rly(mod_id, K721);
	  talonCard->openAllRelays(false, true);
	  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);	
	  if (side == SIDEA)	
	  { //submux setup
		  vth_aclose_rly(mod_id, K725);	// connect A side S-bus to MOAINV thru x1
		  vth_aclose_rly(mod_id, K735);	// connect A side I-bus to Source thru x3
 		  vth_aclose_rly(mod_id, K721); // ground x5
		  vth_aclose_rly(mod_id, K723); // ground x7
	  }
	  else
	  {
		  vth_aclose_rly(mod_id, K729); // connect B side S-bus to MOAINV thru x5
		  vth_aclose_rly(mod_id, K739); // connect B side I-bus to Source thru x7
		  vth_aclose_rly(mod_id, K717); // ground x1
		  vth_aclose_rly(mod_id, K719); // ground x3
	  }

	  for(int chnl = 0; chnl < 72; ++chnl)
	  {/*for: measurement*/
		  talonCard->closeRelay(side == SIDEA ? KSA : KSB, false, false);
		  talonCard->closeRelay(side == SIDEA ? KIA : KIB, false, false);
		  talonCard->closeRelay(side == SIDEA ? Sbus_A[chnl] : Sbus_B[chnl], false, false);
		  switch (chnl)
		  {
		  case (0):
			  talonCard->closeRelay(side == SIDEA ? Ibus_A[chnl + 1] : Ibus_B[chnl + 1], true, true);
			  break;
		  case (71):
			  talonCard->closeRelay(side == SIDEA ? Ibus_A[chnl - 1] : Ibus_B[chnl - 1], true, true);
			  break;
		  default:
  			  talonCard->closeRelay(side == SIDEA ? Ibus_A[chnl + 1] : Ibus_B[chnl + 1], false, false);
			  talonCard->closeRelay(side == SIDEA ? Ibus_A[chnl - 1] : Ibus_B[chnl - 1], true, true);
			  break;
		  }

		  measured = measure_open(mod_id, VPLUS, VMINUS, rfb);
		  if (measured < -2.0e+11)
			  measured = R_MAX_PARALLEL;
		  else
		  {
			  if (measured < -1.0e+11)
			  {   /* moderate negative < 200uV noise */
				  measured = R_MAX_PARALLEL; 
				  
				  /* warn the operator of a potential measurement problem */
				  channel = (side == SIDEA ? (chnl + 1) : (chnl + 101));
//				  display_prompt(message, true);
				  strcpy(message2, "Test 16209 Module ");
				  len = strlen(message2) + 1;
				  sprintf(message2 + len - 1, "%d\t%s%d\t%s%d", mod_id, "Slot ", slot, 
					  "Channel ", channel);
				  display_prompt(message2, true); 
			  }
			  else
			  {
				  if (measured < 0)
				  {   /* small negative > 200uV noise */
					  measured = -1.0 * measured;
				  }
				  else
				  {  /* positive value */
					  if(measured > R_MAX_PARALLEL)
						  measured = R_MAX_PARALLEL;
				  }
			  }
		  }
		  resistance_xbus[chnl][side] = log10(measured);		 
		  talonCard->openAllRelays(true, true);   
	  } /*for: measurement*/
	  vth_aopen_all_rlys(mod_id);

	} /*for: side*/ 

	for(int i = 0; i < 2; ++i)
	{
		for(int j = 0; j < 72; ++j)
		{
		   result.Measurement_class = CLASS_REAL;
		   result.UU.Real_result = resistance_xbus[j][i];
		   log_result(mod_id, result);
		}
	}

    vth_pinit(mod_id, slot);
    vth_ainit(mod_id);

} /*t16209*/

#undef SALES_LONG
#undef AVERAGE
#undef CPU
#undef VEXP
#undef TOUT
#undef RFB_1MEG
#undef MODE_2WIRE
#undef VPLUS
#undef VMINUS


/*
*--------------------------------------------------------------------------------
* Test: 16216
* Description: Parallel SIGLAB to Ground Resistance
*				This test is modeled after test 9217.
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/

#define SALES_LONG      1
#define AVERAGE         10
#define CPU             0
#define VEXP            0.039
#define TOUT            1.0
#define RFB_1MEG        5
#define MODE_2WIRE      0
#define VPLUS           5.0
#define VMINUS          (-5.0)

void t16216(int mod_id, int slot)
{
  double resistance_xbus;
  double rfb;
  int channel = 0;
  int len;
  vth_cfxt_unlock();
  vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
  s80 message2;
  test_result_type result;
 
  static HTalon::RelayName const G_bus[6] = {KGA, KGB, KGLA, KGLB, KLA, KLB};
  static HTalon::RelayName const S_A[11] = {KSA, KSB, KIA, KIB, KAA, KAB, KBA, KBB, KIBA, KIBB, KSAB};  //KSAA
  static HTalon::RelayName const S_B[11] = {KSAA, KSB, KIA, KIB, KAA, KAB, KBA, KBB, KIBA, KIBB, KSAB}; //KSA
  static HTalon::RelayName const I_A[11] = {KSA, KSB, KIA, KIB, KAA, KAB, KBA, KBB, KSAA, KIBB, KSAB};  //KIBA
  static HTalon::RelayName const I_B[11] = {KSA, KSB, KIBA, KIB, KAA, KAB, KBA, KBB, KSAA, KIBB, KSAB}; //KIA
  static HTalon::RelayName const A_A[11] = {KSA, KSB, KIA, KIB, KAA, KAB, KBA, KBB, KSAA, KIBA, KIBB};  //KSAB
  static HTalon::RelayName const A_B[11] = {KSA, KSAB, KIA, KIB, KAA, KAB, KBA, KBB, KSAA, KIBA, KIBB}; //KSB
  static HTalon::RelayName const B_A[11] = {KIB, KSB, KIA, KSA, KBB, KBA, KAB, KAA, KSAB, KSAA, KIBA};  //KIBB
  static HTalon::RelayName const B_B[11] = {KIBB, KSB, KIA, KSA, KBB, KBA, KAB, KAA, KSAB, KSAA, KIBA}; //KIB
  
  static unsigned int const n = 8;
  static relay_set const tps[n] =
  {
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
	  relay_set(R(S_A), R(G_bus)),
	  relay_set(R(S_B), R(G_bus)),
	  relay_set(R(I_A), R(G_bus)),
	  relay_set(R(I_B), R(G_bus)),
	  relay_set(R(A_A), R(G_bus)),
	  relay_set(R(A_B), R(G_bus)),
	  relay_set(R(B_A), R(G_bus)),
	  relay_set(R(B_B), R(G_bus)),
		  
#undef R
  };
  
  HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
  vth_aopen_all_rlys(mod_id);				// Initialize Aim 
  talonCard->openAllRelays(false, true);	// Initialize Talon 
  
  /* setup the MOA for resistor measurements */
  vth_aclose_rly(mod_id, K827);   /* MOAOUT to REFR2 */
  vth_aclose_rly(mod_id, K850);   /* Rfb = 1MEG */
  vth_aclose_rly(mod_id, K811);   /* I to MOAINV */
  vth_aopen_rly(mod_id, K843);    /* MOANON to AUXOUT */
  vth_aclose_rly(mod_id, K842);
  vth_aopen_rly(mod_id, K2605);   /* MOA Happy Relay */
  vth_aopen_rly(mod_id, K857);    /* disconnect L from ACOM */
  vth_aclose_rly(mod_id, K819);   /* MOANON to L */
  vth_aclose_rly(mod_id, K820);   /* DET-LO to AUX-SRC */
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
  
  for(unsigned int i = 0; i <= 7; i++)
	  vth_aclose_rly(mod_id, AIM_X_L_RLYS[i]); //Connect Xn to Xl
  
  vth_aclose_rly(mod_id, K751);
  
  /* setup the AUX */
  vth_aaux_en_30ma_ilim(mod_id);
  vth_aaux_set_ampl(mod_id, 0.0);
  vth_aaux_en(mod_id);
  
  for (unsigned int p = 0; p < n; ++p)
  {/*for: side*/
	  close(mod_id, slot,tps[p].rut, tps[p].ref, talonCard);
	  vth_aclose_rly(mod_id, AIM_X_I_RLYS[p]);
	  vth_aopen_rly(mod_id, AIM_X_L_RLYS[p]);
	  vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
	  
	  resistance_xbus = measure_open_non(mod_id, VPLUS, VMINUS, rfb);
	  if (resistance_xbus < -1.0e+11)
		  resistance_xbus = R_MAX_PARALLEL;
	  else
	  {
		  if (resistance_xbus < -5.0e+10)
		  {   /* moderate negative < 200uV noise */
			  resistance_xbus = R_MAX_PARALLEL;
			  
			  /* warn the operator of a potential measurement problem */
//			  display_prompt(message, true);
			  strcpy(message2, "Test 16216 Module ");
			  len = strlen(message2) + 1;
			  sprintf(message2 + len - 1, "%d\t%s%d", mod_id, "Slot ", slot);
			  display_prompt(message2, true); 
		  }
		  else
		  {
			  if (resistance_xbus < 0)
				  resistance_xbus = fabs(resistance_xbus);
			  else
			  {  /* positive value */
				  if (resistance_xbus > R_MAX_PARALLEL)
					  resistance_xbus = R_MAX_PARALLEL;
				  
			  }
		  }
	  }
	  result.UU.Real_result = log10(resistance_xbus);
	  log_result(mod_id, result);
	  vth_aopen_rly(mod_id, AIM_X_I_RLYS[p]);
	  vth_aclose_rly(mod_id, AIM_X_L_RLYS[p]);
	  talonCard->openAllRelays(false, true);
	  vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);   
	  
  }/*for: side*/
  vth_pinit(mod_id, slot);
  vth_ainit(mod_id);
  
}  /*t16216*/

#undef SALES_LONG
#undef AVERAGE
#undef CPU
#undef VEXP
#undef TOUT
#undef RFB_1MEG
#undef MODE_2WIRE
#undef VPLUS
#undef VMINUS


/*
*--------------------------------------------------------------------------------
* Test: 16217
* Description: Parallel Xn to Ground Resistance
*				This test is modeled after test 9217.
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/

#define SALES_LONG      1
#define AVERAGE         10
#define CPU             0
#define VEXP            0.039
#define TOUT            1.0
#define RFB_1MEG        5
#define MODE_2WIRE      0
#define VPLUS           5.0
#define VMINUS          (-5.0)

void t16217(int mod_id, int slot)
{
   double resistance_xbus;
   double rfb;
   int channel = 0;
   int len;
   int Xn = 8;
   vth_cfxt_unlock();
   vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
   s80 message2;
   test_result_type result;
   
   static HTalon::RelayName const G_bus[6] = {KGA, KGB, KGLA, KGLB, KLA, KLB};
   static HTalon::RelayName const Dut[8] = {KSA, KAA, KIA, KBA, KSB, KAB, KIB, KBB};
   static unsigned int const n = 1;
   static rlylist const tps[n] =
   {
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]

	   rlylist(R(G_bus)),
		   
#undef R
   };
   
   HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
   vth_aopen_all_rlys(mod_id);				// Initialize Aim 
   talonCard->openAllRelays(false, true);	// Initialize Talon 
   
   /* setup the MOA for resistor measurements */
   vth_aclose_rly(mod_id, K827);	/* MOAOUT to REFR2 */
   vth_aclose_rly(mod_id, K850);	/* Rfb = 1MEG */
   vth_aclose_rly(mod_id, K811);	/* I to MOAINV */
   vth_aopen_rly(mod_id, K843);		
   vth_aclose_rly(mod_id, K842);	/* MOANON to AUXOUT */
   vth_aopen_rly(mod_id, K2605);	/* MOA Happy Relay */
   vth_aopen_rly(mod_id, K857);		/* disconnect L from ACOM */
   vth_aclose_rly(mod_id, K819);	/* MOANON to L */
   vth_aclose_rly(mod_id, K820);	/* DET-LO to AUX-SRC */
   vth_aclose_rly(mod_id, K814);	/* G to ACOM */
   vth_aclose_rly(mod_id, K752);	/* G to XG */
   vth_aclose_rly(mod_id, K828);	/* REFR2 to DETHI */
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
   sglset_close(mod_id, slot, tps[LOOP].ref, talonCard);  //Connect XG/XL to ground	
   for(unsigned int xbus = 0; xbus <= 7; xbus++)
	   vth_aclose_rly(mod_id, AIM_X_L_RLYS[xbus]);	//Connect Xn to XL
   
   vth_aclose_rly(mod_id, K751);
   
   /* setup the AUX */
   vth_aaux_en_30ma_ilim(mod_id);
   vth_aaux_set_ampl(mod_id, 0.0);
   vth_aaux_en(mod_id);
   
   for (unsigned int p = 0; p < Xn; ++p)
   {/*for: measurement*/
	   vth_aclose_rly(mod_id, AIM_X_I_RLYS[p]);
	   vth_aopen_rly(mod_id, AIM_X_L_RLYS[p]);
	   talonCard->closeRelay(Dut[p], false, true);
	   vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
	   
	   resistance_xbus = measure_open_non(mod_id, VPLUS, VMINUS, rfb);
	   if (resistance_xbus < -1.0e+11)
		   resistance_xbus = R_MAX_PARALLEL;
	   else
	   {
		   if (resistance_xbus < -5.0e+10)
		   {   /* moderate negative < 200uV noise */
			   resistance_xbus = R_MAX_PARALLEL;
			   
			  /* warn the operator of a potential measurement problem */
//			  display_prompt(message, true);
			  strcpy(message2, "Test 16217 Module ");
			  len = strlen(message2) + 1;
			  sprintf(message2 + len - 1, "%d\t%s%d", mod_id, "Slot ", slot);
			  display_prompt(message2, true); 
		   }
		   else
		   {
			   if (resistance_xbus < 0)
				   resistance_xbus = fabs(resistance_xbus);
			   else
			   {  /* positive value */
				   if (resistance_xbus > R_MAX_PARALLEL)
					   resistance_xbus = R_MAX_PARALLEL;
			   }
		   }
	   }
	   result.UU.Real_result = log10(resistance_xbus);
	   log_result(mod_id, result);
	   
	   vth_aopen_rly(mod_id, AIM_X_I_RLYS[p]);
	   vth_aclose_rly(mod_id, AIM_X_L_RLYS[p]);
	   vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);   
	   
   }/*for: measurement*/

   vth_pinit(mod_id, slot);
   vth_ainit(mod_id);
   
}  /*t16217*/

#undef SALES_LONG
#undef AVERAGE
#undef CPU
#undef VEXP
#undef TOUT
#undef RFB_1MEG
#undef MODE_2WIRE
#undef VPLUS
#undef VMINUS


/*
*--------------------------------------------------------------------------------
* Test: 16230
* Description: X bus leakage current
*				This test is modeled after test 9230.
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/

#define RANGE           30e-03
#define RESOLUTION      1e-06
#define LOAD_RESISTANCE  10e+03
   
void t16230(int mod_id, int slot)
{
   double dmm_voltage;
   double leakage_current;
   
   int channel = 'A';
   
   test_result_type result;
   OpenResourceManager();
   open_mux(mod_id);
   open_dmm(mod_id);
   vth_cfxt_lock();
   vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
   setup_dmm(mod_id, DCV, RANGE, RESOLUTION);
   
   HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
   vth_aopen_all_rlys(mod_id);				 // Initialize Aim 
   talonCard->openAllRelays(false, true);    // Initialize Talon 
   
   vth_aclose_rly(mod_id, K741);   /* X1 to mint field relay */
   vth_aclose_rly(mod_id, K761);   /* SWD GND mint pin relay */
   
   for(unsigned int i = 0; i <= 7; i++)
	   vth_aclose_rly(mod_id, AIM_X_S_RLYS[i]); 
   
   connect_10k_load(mod_id);
   connect_dmm(mod_id);
   connect_plus_xn(mod_id, channel, true);
   connect_minus_swd_gnd(mod_id);
   dmm_voltage = measure_dmm(mod_id);
   result.Measurement_class = CLASS_REAL;
   leakage_current = dmm_voltage / LOAD_RESISTANCE;
   result.UU.Real_result = leakage_current;
   log_result(mod_id, result);
   
   close_mux(mod_id);
   close_dmm(mod_id);
   CloseResourceManager();
   vth_ainit(mod_id);
   vth_pinit(mod_id, slot);
   
}/*t16230*/

#undef RANGE
#undef RESOLUTION
#undef LOAD_RESISTANCE
   
   
/*
*--------------------------------------------------------------------------------
* Test: 16231
* Description: SI-AB bus leakage current
*				This test is modeled after test 9230.
* Fixture: None
* Resource(s): None
*--------------------------------------------------------------------------------
*/

#define RANGE           30e-03
#define RESOLUTION      1e-06
#define LOAD_RESISTANCE  10e+03

void t16231(int mod_id, int slot)
{
   double dmm_voltage;
   double leakage_current;
   int channel = 'A';
   test_result_type result;
   OpenResourceManager();
   open_mux(mod_id);
   open_dmm(mod_id);
   vth_cfxt_lock();
   vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
   setup_dmm(mod_id, DCV, RANGE, RESOLUTION);
   
   static HTalon::RelayName const Side_A[6] = {KSA, KIA, KAA, KBA, KIBA, KSAA};  
   static HTalon::RelayName const Side_B[6] = {KSB, KIB, KAB, KBB, KIBB, KSAB}; 
   
   static unsigned int const n = 2;
   static rlylist const tps[n] =
   {
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
	   rlylist(R(Side_A)),
	   rlylist(R(Side_B)),
		   
#undef R
   };

   HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
   vth_aopen_all_rlys(mod_id);				 // Initialize Aim 
   talonCard->openAllRelays(false, true);   // Initialize Talon 
   
   for (unsigned int j = 0; j < n; ++j)
   {/*for: side*/
	   sglset_close(mod_id, slot, tps[j].ref, talonCard);  
	   
	   vth_aclose_rly(mod_id, K741);   /* X1 to mint field relay */
	   vth_aclose_rly(mod_id, K761);   /* SWD GND mint pin relay */
	   
	   for(unsigned int i = 0; i <= 7; i++)
		   vth_aclose_rly(mod_id, AIM_X_S_RLYS[i]); 
	   
	   connect_10k_load(mod_id);
	   connect_dmm(mod_id);
	   connect_plus_xn(mod_id, channel, true);
	   connect_minus_swd_gnd(mod_id);
	   dmm_voltage = measure_dmm(mod_id);
	   result.Measurement_class = CLASS_REAL;
	   leakage_current = dmm_voltage / LOAD_RESISTANCE;
	   result.UU.Real_result = leakage_current;
	   log_result(mod_id, result);
	   
	   vth_aopen_all_rlys(mod_id);
	   vth_pinit(mod_id, slot);
   }/*for: side*/
   
   close_mux(mod_id);
   close_dmm(mod_id);
   CloseResourceManager();
   vth_ainit(mod_id);
   vth_pinit(mod_id, slot);

}/*t16231*/
#undef RANGE
#undef RESOLUTION
#undef LOAD_RESISTANCE

/*
 *--------------------------------------------------------------------------------
 * Test: 16248
 * Description: 5 Volt source and fuse
 *			   
 * Fixture: Digital	
 * Resource(s): DMM
 *--------------------------------------------------------------------------------
*/
  
#define RANGE			5			
#define RESOLUTION		1.0e-03

static TALON_5V Talon_5v;
 
void t16248(int mod_id, int slot)
{
	static unsigned int const n = 2;
	int channel = 'B';
    test_result_type result;
	OpenResourceManager();
    open_dmm(mod_id);
	open_mux(mod_id);
    connect_dmm(mod_id);
    setup_dmm(mod_id, DCV, RANGE, RESOLUTION);
    setup_dmm_ocomp_on(mod_id);
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	vth_pinit(mod_id, slot);

//   typedef HTalon::RelayName const *const RLYLIST; Not sure why I had this in here??? see if this kills functionality

   static HTalon::RelayName const A_5V[6] = {HTalon::KT5VA, HTalon::KADCAA, HTalon::KTCOMMA, 
											  HTalon::KAA, HTalon::KAG04, HTalon::KAGD01G};
   static HTalon::RelayName const B_5V[6] = {HTalon::KT5VB, HTalon::KADCAB, HTalon::KTCOMMB, 
											  HTalon::KAB, HTalon::KBG04, HTalon::KBGD01G};
   
   static rlylist const tps[n] =
   {
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
	   rlylist(R(A_5V)),
	   rlylist(R(B_5V)),
#undef R
   };

   vth_aopen_all_rlys(mod_id);				
   talonCard->openAllRelays(false, true);	
   vth_aclose_rly(mod_id, K733);
   vth_aclose_rly(mod_id, K734);
   connect_4wire_talon(mod_id, true, LOOP);
   connect_minus_pin(mod_id, slot, channel, true);
 
   for (int side = SIDEA; side < SIDEB; ++side)
   {/*for: side*/
	   sglset_close(mod_id, slot, tps[side].ref, talonCard);
	   vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);		
	   flush_commands(mod_id);
	   Talon_5v.Vunloaded[mod_id][slot][side] = measure_dmm(mod_id); 
	   result.Measurement_class = CLASS_REAL;
	   result.UU.Real_result = Talon_5v.Vunloaded[mod_id][slot][side];		
	   log_result(mod_id, result);
	   talonCard->openAllRelays(true, true); 
	   get_result_nolog(mod_id, &result);
   }/*for: side*/	
   connect_minus_pin(mod_id, slot, channel, false);
   connect_4wire_talon(mod_id, false, LOOP);
   vth_aopen_all_rlys(mod_id);
   vth_pinit(mod_id, slot);
   close_mux(mod_id);
   close_dmm(mod_id);
   CloseResourceManager();
	
}/*16248*/

#undef RANGE	
#undef RESOLUTION


/*
*--------------------------------------------------------------------------------
* Test: 16249
* Description: Talon load resistor measurment
*				
* Fixture: Analog	
* Resource(s): DMM
*--------------------------------------------------------------------------------
*/
   
#define RESOLUTION	1.0e-03
   
void t16249(int mod_id, int slot)
{
   unsigned const int n = 6;
   int channel = 'B';
   double dmm_resistance = 0;
   double range[6] = {10, 50, 1000, 10, 50, 1000};
   test_result_type result;
   OpenResourceManager();
   open_mux(mod_id);
   open_dmm(mod_id);
   connect_dmm(mod_id);
   vth_cfxt_lock();
   vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
   HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
   vth_ainit(mod_id);
   vth_pinit(mod_id, slot);
   
   static HTalon::RelayName const A_6_5[3] = {HTalon::KAS02, HTalon::KAS04, HTalon::KT6_5SA};
   static HTalon::RelayName const B_6_5[3] = {HTalon::KBS02, HTalon::KBS04, HTalon::KT6_5SB};
   static HTalon::RelayName const A_50[3] = {HTalon::KAS02, HTalon::KAS04, HTalon::KT50SA};
   static HTalon::RelayName const B_50[3] = {HTalon::KBS02, HTalon::KBS04, HTalon::KT50SB};
   static HTalon::RelayName const A_1K[3] = {HTalon::KAS02, HTalon::KAS04, HTalon::KT1KSA};
   static HTalon::RelayName const B_1K[3] = {HTalon::KBS02, HTalon::KBS04, HTalon::KT1KSB};
   static HTalon::RelayName const SUB_A[4] = {KLA, HTalon::KADCAA, HTalon::KTCOMMA, HTalon::KAA};
   static HTalon::RelayName const SUB_B[4] = {KLB, HTalon::KADCAB, HTalon::KTCOMMB, HTalon::KAB};
   
   static relay_set const tps[n] =
   {
#define R( P ) P, &P[sizeof( P ) / sizeof(HTalon::RelayName)]
	   relay_set(R(A_6_5), R(SUB_A)),
	   relay_set(R(A_50), R(SUB_A)),
	   relay_set(R(A_1K), R(SUB_A)),
	   relay_set(R(B_6_5), R(SUB_B)),
	   relay_set(R(B_50), R(SUB_B)),
	   relay_set(R(B_1K), R(SUB_B)),
#undef R
   };

   vth_aclose_rly(mod_id, K733);
   vth_aclose_rly(mod_id, K734);
 
   for (unsigned int p = 0; p < n; ++p)
   {/*for: side*/
	   setup_dmm(mod_id, FOUR_WIRE_OHMS, range[p], RESOLUTION);
	   setup_dmm_ocomp_on(mod_id);
	   if(p < 3)
		   vth_aclose_rly(mod_id, K734);
	   else
		   vth_aclose_rly(mod_id, K738);

	   close(mod_id, slot, tps[p].rut, tps[p].ref, talonCard);
	   connect_4wire_talon(mod_id, true, LOOP);
	   connect_minus_pin(mod_id, slot, channel, true);
	   vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);		
	   get_result_nolog(mod_id, &result);
	   dmm_resistance = measure_dmm(mod_id);
	   result.Measurement_class = CLASS_REAL;
	   result.UU.Real_result = dmm_resistance - 0.75; //Path impedance was measured at .73, buffered for veriability
	   log_result(mod_id, result);
	   connect_minus_pin(mod_id, slot, channel, false);
	   connect_4wire_talon(mod_id, false, LOOP);
	   vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);
	   talonCard->openAllRelays(false, true); 
	   get_result_nolog(mod_id, &result);
   }/*for: side*/
   
   vth_ainit(mod_id);
   vth_pinit(mod_id, slot);
   close_mux(mod_id);
   close_dmm(mod_id);
   CloseResourceManager();
   
}/*end t16249*/

#undef RESOLUTION		


