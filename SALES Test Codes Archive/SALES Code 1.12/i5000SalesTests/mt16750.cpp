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
#include "../../../vth/src/vth_mac.h"
#include "../../../vth/src/aimasm.h"
#include "../../../vth/src/macasm.h"
#include "../../../vth/src/MetaTesthead.h"
#include "../../../vth/src/ConnectBRCtoBus.h"
#include "../../../vth/src/ConnectDigitalResource.h"
#include "../../../vmc/src/vmc_digital.h"
#include "../../../vmc/src/vmc_setup.h"
#include "../../../vmc/src/vmc_channel.h"
#include "../../../vmc/src/vmc_vectors.h"
#include "../../../vmc/src/vmc_seq_ram.h"
#include "../../../vmc/src/vmc_prog_ram.h"
#include "../Feature.h"
#include "../i5000Tests/TestRelays.h"
#include "../i5000Tests/TestTalon.h" 
#include "../i5000Tests/TestPin.h" 
#include "../i5000Tests/TestReadBack.h"
#include "RelayPath.h"
#include "mt16750.h"

//TEMPORARY DEBUG 
#include "../i5000Tests/TestReadBack.h"

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

struct Drvr_setup
{
	TestUtils::ChannelName const begin;
	TestUtils::ChannelName const end;
	double const driveHigh;
	double const driveLow;
	double scale(double s) const { return s * 1000.0; }
	double rangeV(double v) const { assert((v >= 0.0) && (v <= 5.0)); return v; }
	Drvr_setup(TestUtils::ChannelName const &b, TestUtils::ChannelName const &e, double const &dh, double const &dl) 
	 : begin(b), end(e), driveHigh(scale(rangeV(dh))), driveLow(scale(rangeV(dl))) {}
};

struct Drvr
{
	TestUtils::ChannelName const begin;
	TestUtils::ChannelName const end;
	Drvr(TestUtils::ChannelName const &b, TestUtils::ChannelName const &e) 
		 : begin(b), end(e) {}
};

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
* Test: 16800-16804
* Description: Static Driver output 
*				16800: 5.0v out
*				16801: 0.0v out
*				16802: 1.25v out
*				16803: 2.5v out
*				16804: 3.3v out
*
*				These tests are modeled after test 9100-9103.
*
* Fixture: Digital
* Resource(s): DMM
*--------------------------------------------------------------------------------
*/

#define RANGE			5			
#define RESOLUTION		1.0e-03

void static_drive_output(int mod_id, int slot, int test_number)
{
	static unsigned int const n = 5;
	unsigned int page = 0;
	unsigned int channel = 1;
	int dvh;
	int dvl; 
	double meas[144];
	
	test_result_type result;

	OpenResourceManager();
	open_dmm(mod_id);
	open_mux(mod_id);
	connect_dmm_pin(mod_id, slot);
	setup_dmm(mod_id, DCV, (double)RANGE, RESOLUTION);
    HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	vth_pinit(mod_id, slot); 
	vth_cfxt_lock();
	
	static Drvr_setup const tps[n] = 
	{ 
		Drvr_setup(TestUtils::c001, TestUtils::ChannelNamePastEnd, 5.00, 0.0),	//16800
		Drvr_setup(TestUtils::c001, TestUtils::ChannelNamePastEnd, 0.00, 0.2),	//16801
		Drvr_setup(TestUtils::c001, TestUtils::ChannelNamePastEnd, 1.25, 0.0),	//16802
		Drvr_setup(TestUtils::c001, TestUtils::ChannelNamePastEnd, 2.50, 0.0),	//16803
		Drvr_setup(TestUtils::c001, TestUtils::ChannelNamePastEnd, 3.30, 0.0),	//16804
	};
	
	IPin *pinCard = TestPin::getPinCardAt(mod_id, slot);                        
	IMetaTesthead *th = MetaTesthead::getMetaTesthead();                        
    dvh = tps[test_number].driveHigh;
	dvl = tps[test_number].driveLow;
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

	CloseDigitalFixtureGround(mod_id, slot, talonCard);

	for (TestUtils::ChannelName chnl = tps[test_number].begin; chnl < tps[test_number].end; ++chnl)
    {/*for: channel*/
        PINDefinition::PinEnum pin(TestUtils::getPin(TestUtils::mapDrvChannelConnection(chnl)));
		ConnectDigitalFixture(mod_id, slot, pin, th, pinCard);
		BRC brc(mod_id, slot, pin);
		StaticDriveSetup(mod_id, slot, pin, th, pinCard, LOOP, dvh, dvl);
		pinCard->spewToVthStream();  
		vth_mseq_neg_cpu_tsp(mod_id);
        get_result_nolog(mod_id, &result);
		vth_mtmr_wait(mod_id, TIME_OUT);
		meas[chnl] = measure_dmm(mod_id);
		vth_mseq_ass_cpu_tsp(mod_id);
		DisableStaticDriveOut(mod_id, slot, pin, th, pinCard);
	}/*for: channel*/

	for (TestUtils::ChannelName chnl_res = tps[test_number].begin; chnl_res < tps[test_number].end; ++chnl_res)
    {
       result.Measurement_class = CLASS_REAL;
	   result.UU.Real_result = meas[chnl_res];
	   log_result(mod_id, result);
	}
	talonCard->openAllRelays(false, true);	
	vth_pinit(mod_id, slot);       
	close_dmm(mod_id);                                                     
	close_mux(mod_id);
	CloseResourceManager();
	
} /*static_drive_output*/

#undef RANGE			
#undef RESOLUTION	
	

/*
 *--------------------------------------------------------------------------------
 * Test: 16806
 * Description: Driver output slew rate (100v/us)
 *			   This test is modeled after tests 9106
 * Fixture: Sales Digital
 * Resource(s): Counter
 *--------------------------------------------------------------------------------
 */

#define INPUT_FREQ      20e+06
#define TCLK_FREQ       20e+06

void t16806(int mod_id, int slot)
{
	 int err;
	 double slew_time;
	 double slew_voltage;
	 double slew_rate[144][2];
	 double voltage_max;
	 double voltage_min;
	 double vmax;
	 double vmin;
	 static unsigned int const n = 1;
	   	 
	 test_result_type result;

	 vth_mod_init(mod_id);
	 OpenResourceManager();
	 open_cntr(mod_id);
	 open_mux(mod_id);
	 
     static Drvr const tps[n] = { Drvr(TestUtils::c001, TestUtils::ChannelNamePastEnd),};	
//	 static Drvr const tps[n] = { Drvr(TestUtils::c016, TestUtils::c023),};
	 
	 HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	 vth_mtclk_clk_20m_mux(mod_id);
	 vth_mtclk_set_freq(mod_id, INPUT_FREQ, TCLK_FREQ);
	 setup_cntr_input(mod_id, DC, 50);	// x10 attenuation, DC input coupling, 50 ohm input impedance
	 connect_cntr_ch1_af2(mod_id);

     for (int edge = POS; edge >= NEG; --edge)
	 {  /*for: edge*/	 
  	     #include "t16806.cmd"
		 if (edge == POS)
			 setup_cntr_edge(mod_id, RISE_TIME);
		 else
			 setup_cntr_edge(mod_id, FALL_TIME);
		 
		 for (TestUtils::ChannelName chnl = tps[LOOP].begin; chnl < tps[LOOP].end; ++chnl)
		 {/*for: channel*/
			 slew_voltage = 0.0;
			 slew_time = 1.0;
			 ConnectDrivertoAF2(mod_id, slot, chnl, talonCard);
			 vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
			 Start_sequencer(mod_id, slot); 
		     vth_mtmr_wait(mod_id, 2 * TIME_OUT);	
			 err = TrigCntr(mod_id);
		     if(err == 0)
			 {
               slew_time = read_cntr(mod_id);
			   vmax = read_cntr_max(mod_id);
			   vmin = read_cntr_min(mod_id);
			   slew_time *=(1000000);  
			 }
		     else
			 {
			   slew_time = 1.0e+14;	// no result => set measured value to infinity
			   vmax = vmin = 0;
			 }
			 
			 // we multiply by .1 because we need 10% of the voltage difference
			 // since the voltage difference is taken with a 10% limit
			 Reset_sequencer(mod_id, slot); 	
			 voltage_min = vmin + 0.1 * (vmax - vmin);
			 voltage_max = vmax - 0.1 * (vmax - vmin);
			 slew_voltage = voltage_max - voltage_min;
			 //  show_value(mod_id, "slew_voltage = ", slew_voltage);
			 slew_rate[chnl][edge] = slew_voltage / fabs(slew_time);
		 }/*for: channel*/

		 talonCard->openAllRelays(false, true);
		 
	 }/*for: edge*/	

     for(int slope = NEG; slope <= POS; ++slope)
	 {
	    for(TestUtils::ChannelName chnl = tps[LOOP].begin; chnl < tps[LOOP].end; ++chnl)
		{
		   result.Measurement_class = CLASS_REAL;
		   result.UU.Real_result = slew_rate[chnl][slope];
		   log_result(mod_id, result);
		}
	 } 
	 
	 vth_mod_init(mod_id);
	 close_cntr(mod_id);
	 close_mux(mod_id);
	 CloseResourceManager();
	 
} /*16806*/

#undef INPUT_FREQ
#undef TCLK_FREQ


/*
 *--------------------------------------------------------------------------------
 * Test: 16811
 * Description: Drive output minimum pulse width
 *			   This test is modeled after tests 9111
 * Fixture: Sales Digital
 * Resource(s): Counter
 *--------------------------------------------------------------------------------
 */

#define HIGH_TRIGGER_LEVEL  1.3
#define LOW_TRIGGER_LEVEL	1.3
#define INPUT_FREQ			20e+06
#define TCLK_FREQ			20e+06

void t16811(int mod_id, int slot)
{
	static unsigned int const n = 1;
	int impedance = 50;				// counter input impedance
	double readings = 0.0;
	
	test_result_type result;
	OpenResourceManager();
	open_cntr(mod_id);
	open_mux(mod_id);
	connect_cntr_ch1_af2(mod_id);
	setup_cntr_inputz(mod_id, impedance);
    
	static Drvr const tps[n] = { Drvr(TestUtils::c001, TestUtils::ChannelNamePastEnd),};
//	static Drvr const tps[n] = { Drvr(TestUtils::c001, TestUtils::c005),};
	
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	talonCard->openAllRelays(false, true);	// Initalize Talon
	
	for (int pulse = NEG; pulse <= POS; ++pulse)
	{/*for: pulse*/
		vth_mtclk_clk_20m_mux(mod_id);
		vth_mtclk_set_freq(mod_id, INPUT_FREQ, TCLK_FREQ);
		if (pulse == NEG)
		{
			#include "t16811_np.cmd"
			setup_cntr_function(mod_id, NWID);
			setup_cntr_trg_levels(mod_id, LOW_TRIGGER_LEVEL, HIGH_TRIGGER_LEVEL);
		}
		else 
		{
			#include "t16811_pp.cmd"
			setup_cntr_function(mod_id, PWID);
			setup_cntr_trg_levels(mod_id, HIGH_TRIGGER_LEVEL, LOW_TRIGGER_LEVEL);
		}

		for (TestUtils::ChannelName chnl = tps[LOOP].begin; chnl < tps[LOOP].end; ++chnl)
		{/*for: channel*/
			ConnectDrivertoAF2(mod_id, slot, chnl, talonCard);
			Start_sequencer(mod_id, slot); 
			trigger_cntr(mod_id);
			readings = read_cntr(mod_id);
			//show_value(mod_id, "Readings = ", readings);
			Reset_sequencer(mod_id, slot); 
		    result.Measurement_class = CLASS_REAL;
			result.UU.Real_result = readings;
			log_result(mod_id, result);
			readings = 0.0;			

		}/*for: channel*/

	}/*for: pulse*/
	
	vth_pinit(mod_id, slot);
	vth_ainit(mod_id);
	close_cntr(mod_id);
	close_mux(mod_id);
	CloseResourceManager();
	
}  /*t16811*/

#undef HIGH_TRIGGER_LEVEL
#undef LOW_TRIGGER_LEVEL
#undef INPUT_FREQ
#undef TCLK_FREQ


/*
 *--------------------------------------------------------------------------------
 * Test: 16820, 16821, and 16829/16830 
 * Description: Driver edge placement accuracy
 *				 16820: No delay
 *				 16821: 100nS delay
 *				 16829: Out of tri-state 
 *			   
 *			   This test is modeled after tests 9120
 * Fixture: Sales Digital
 * Resource(s): Counter, Synthesizer, Pulse Generator
 *--------------------------------------------------------------------------------
 */
  

#define TCLK_FREQ			5.0e+06
#define T0_ACTUAL			165.9e-9

void driver_edge_placement(int mod_id, int slot, int test_number)
{
	static unsigned int const n = 1;
	int cntr_stat;
	double gen_delay;
	double drvr_delay_actual;
	double offset;
	double ch_delta[144][4];
	test_result_type result;
    result.Measurement_class = CLASS_REAL;
	
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
    vth_mod_init(mod_id);
	OpenResourceManager();
	open_mux(mod_id);
	open_syn(mod_id);
	  if(check_syn_model())
	  {
		  setup_33250A_hv(mod_id, true);
		  enable_33250A_out(mod_id);
	  }
	open_gen(mod_id);
	open_cntr(mod_id);
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);

    static Drvr const tps[n] = { Drvr(TestUtils::c001, TestUtils::ChannelNamePastEnd),};
//       static Drvr const tps[n] = { Drvr(TestUtils::c068, TestUtils::c110),};
   
   switch (test_number)
   {
   case (16820): 
	   gen_delay = 85e-09; 
	   #include "t16820_nd.cmd"
	   break;
   case (16821): 
	   gen_delay = 185e-09;
	   #include "t16820_d.cmd"
	   break;
   case (16829):
	   gen_delay = 100e-09;
	   #include "t16829.cmd"
	   vth_mtclk_set_adv_time(mod_id, T0_ACTUAL);
	   break;
   default: assert(false); break;
   }
   
   offset = MeasureGenTrigOutDelay(mod_id, slot, gen_delay);
  
   for(int slope = NEG; slope <= POS; ++slope)
   {  /*for slope*/
	   if(slope == NEG)
	     gen_delay = 0e-09;
	   else
		 gen_delay = 100e-09;
     
	   setup_gen_trg_out_delay(mod_id, gen_delay, false);
	   setup_cntr_trigger_slopes(mod_id, slope, slope);
       vth_mhsl_1_init_master(mod_id);
       all_modules_but_me(mod_id, &vth_mhsl_1_init_passive);
       vth_mhsl_2_init_master(mod_id);
       all_modules_but_me(mod_id, &vth_mhsl_2_init_passive);
       vth_mdig_set_clkrb_master(mod_id, TCLK_FREQ, TCLK_FREQ, RISING_EDGE);
  	   
	   for(TestUtils::ChannelName chnl = tps[LOOP].begin; chnl < tps[LOOP].end; ++chnl)
	   {/*for: channel*/
		   ConnectDrivertoAF2(mod_id, slot, chnl, talonCard);
		   enable_gen_out(mod_id);
		   vth_mtmr_wait(mod_id, TIME_OUT);
		   Start_sequencer(mod_id, slot); 
		   cntr_stat = TrigCntr(mod_id);
		   if( cntr_stat == 0)
               drvr_delay_actual = read_cntr_mean(mod_id);
		   else
			   drvr_delay_actual = 1.0e+14;	// no result => set result to infinity

		   ch_delta[chnl][slope] = edge_placement_calc(mod_id, slot, slope, offset, drvr_delay_actual);
		   disable_gen_out(mod_id);
		   Reset_sequencer(mod_id, slot); 
	   }/*for: channel*/
	   
	   talonCard->openAllRelays(false, true);
	   vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);	
	   flush_commands(mod_id);
   }/* for slope */

   for(int edge = NEG; edge <= POS; ++edge)
   {
	   for(TestUtils::ChannelName chnl = tps[LOOP].begin; chnl < tps[LOOP].end; ++chnl)
	   {
		   result.UU.Real_result = ch_delta[chnl][edge];
		   log_result(mod_id, result);
	   }
	} 

   vth_mod_init(mod_id);
   close_syn(mod_id);
   close_gen(mod_id);
   close_mux(mod_id);
   close_cntr(mod_id);
   CloseResourceManager();
   
}/*driver_edge_placement*/

#undef TCLK_FREQ
#undef T0_ACTUAL


/*
 *--------------------------------------------------------------------------------
 * Test: 16832
 * Description: Driver tri-state leakage current 
 *			   (high: 5V, low: 0V)
 *			   This test is modeled after tests 9132
 * Fixture: Sales Digital
 * Resource(s): DMM
 *--------------------------------------------------------------------------------
 */
  
#define RANGE           10
#define RESOLUTION      1.0e-03
#define DRIVER_LOAD_RESISTANCE  500e+00

 void t16832(int mod_id, int slot)
 {
	 unsigned int page = 0;
	 unsigned int channel = 1;
	 static unsigned int const n = 1;
	 double driver_tri_state_voltage;
	 double driver_tri_state_current[144];
	 
	 test_result_type result;
	 OpenResourceManager();
	 open_dmm(mod_id);
	 open_mux(mod_id);
	 setup_dmm(mod_id, DCV, RANGE, RESOLUTION);
	 connect_dmm_pin(mod_id, slot);
	 connect_500_ohm_load(mod_id);
     HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	 vth_pinit(mod_id, slot);
	 vth_cfxt_lock();
	 
	 static Drvr_setup const tps[n] = 
	 { 
		 Drvr_setup(TestUtils::c001, TestUtils::ChannelNamePastEnd, 5.00, 0.0),
			 //Drvr(TestUtils::c001, TestUtils::c005, 5.00, 0.0),
	 };
	 
	 IPin *pinCard = TestPin::getPinCardAt(mod_id, slot);                        
	 IMetaTesthead *th = MetaTesthead::getMetaTesthead();   
   	 DigitalChannelParameters dv;
	 dv.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);                   
	 dv.setRcvSource(DigitalReceiverResourceId::alwaysDontCare);                  
	 dv.setDriveHigh(tps[LOOP].driveHigh);                                      
	 dv.setDriveLow(tps[LOOP].driveLow);                                                         
	 dv.setDrvSource(DigitalDriverResourceId::driveAOne);                         
	 dv.setPullUp(DigitalSignalConditionStateId::disabled);                       
	 dv.setPullDown(DigitalSignalConditionStateId::disabled);                     
	 dv.setReceiverTermination(DigitalSignalConditionStateId::disabled);          
	 dv.setDriveCheck(DigitalSignalConditionStateId::disabled);     
	 CloseDigitalFixtureGround(mod_id, slot, talonCard);
	 
	 for (TestUtils::ChannelName chnl = tps[LOOP].begin; chnl < tps[LOOP].end; ++chnl)
	 {/*for: channel*/
		 PINDefinition::PinEnum pin(TestUtils::getPin(TestUtils::mapDrvChannelConnection(chnl)));
		 ConnectDigitalFixture(mod_id, slot, pin, th, pinCard);
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
//		 StaticDriveSetup(mod_id, slot, pin, th, pinCard, LOOP, dvh, dvl);
		 pinCard->spewToVthStream();  
		 get_result_nolog(mod_id, &result);
		 vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);
		 driver_tri_state_voltage = measure_dmm(mod_id);
		 driver_tri_state_current[chnl] = driver_tri_state_voltage / DRIVER_LOAD_RESISTANCE;
		 DisableStaticDriveOut(mod_id, slot, pin, th, pinCard);			  
	 }/*for: channel*/

	 for (TestUtils::ChannelName chnl_res = tps[LOOP].begin; chnl_res < tps[LOOP].end; ++chnl_res)
	 {
		 result.Measurement_class = CLASS_REAL;
		 result.UU.Real_result = driver_tri_state_current[chnl_res];
		 log_result(mod_id, result);
	 }

	 vth_pinit(mod_id, slot);
	 close_dmm(mod_id);                                                     
	 close_mux(mod_id);
	 CloseResourceManager();

 }  /*t16832*/

#undef RANGE
#undef RESOLUTION
#undef DRIVER_LOAD_RESISTANCE


/*
*--------------------------------------------------------------------------------
* Test: 16835, 16836
* Description: Driver output impedance 
*				16835: sourcing 
*				16836: sinking
*			   This test is modeled after tests 9135
* Fixture: Sales Digital
* Resource(s): DMM50.
*--------------------------------------------------------------------------------
*/

   
void driver_output_impedance(int mod_id, int slot, int test_number)
{
   static unsigned int const n = 1;
   int talonload = 65;
   double volts_noload[144];
   double volts_load[144];
   double resistance_loaded;
   double resistance_unloaded;
   double delta_i;
   double delta_v;
   double driver_impedance;
   bool prime = true;
   
   test_result_type result;
   OpenResourceManager();
   open_mux(mod_id);
   open_dmm50(mod_id);
   connect_dmm50_af2(mod_id);

   HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
   talonCard->openAllRelays(false, true);	// Initalize Talon
	
	static Drvr_setup const tps[n] = 
	{ 
		Drvr_setup(TestUtils::c001, TestUtils::ChannelNamePastEnd, 5.00, 0.0),
	 // Drvr(TestUtils::c001, TestUtils::c028, 5.00, 0.0),
	};
	
	IPin *pinCard = TestPin::getPinCardAt(mod_id, slot);                        
	IMetaTesthead *th = MetaTesthead::getMetaTesthead();   
	DigitalChannelParameters source;
	source.setTspSource(DigitalDriverTspResourceId::alwaysActive);                   
	source.setRcvSource(DigitalReceiverResourceId::alwaysDontCare);                  
	source.setDriveHigh(tps[LOOP].driveHigh);                                      
	source.setDriveLow(tps[LOOP].driveLow);                                                         
	source.setDrvSource(DigitalDriverResourceId::driveAOne);                         
	source.setPullUp(DigitalSignalConditionStateId::disabled);                       
	source.setPullDown(DigitalSignalConditionStateId::disabled);                     
	source.setReceiverTermination(DigitalSignalConditionStateId::disabled);          
	source.setDriveCheck(DigitalSignalConditionStateId::disabled); 
	DigitalChannelParameters sink;
	sink.setTspSource(DigitalDriverTspResourceId::alwaysActive);                   
	sink.setRcvSource(DigitalReceiverResourceId::alwaysDontCare);                  
	sink.setDriveHigh(tps[LOOP].driveHigh);                                      
	sink.setDriveLow(tps[LOOP].driveLow);                                                         
	sink.setDrvSource(DigitalDriverResourceId::driveAZero);                         
	sink.setPullUp(DigitalSignalConditionStateId::disabled);                       
	sink.setPullDown(DigitalSignalConditionStateId::disabled);                     
	sink.setReceiverTermination(DigitalSignalConditionStateId::disabled);          
	sink.setDriveCheck(DigitalSignalConditionStateId::disabled); 
    DigitalChannelParameters off;
    off.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);    
    off.setRcvSource(DigitalReceiverResourceId::alwaysDontCare); 

    for(int meas = 0; meas <= 1; meas++)
	{
		for (TestUtils::ChannelName chnl = tps[LOOP].begin; chnl < tps[LOOP].end; ++chnl)
		{/*for: channel*/
		   PINDefinition::PinEnum pin(TestUtils::getPin(TestUtils::mapDrvChannelConnection(chnl)));
		   if(prime)
		   {
			 ConnectDrivertoAF2(mod_id, slot, chnl, talonCard);
			 setup_dmm50(mod_id, TWO_WIRE_OHMS, 100, 1);
			 vth_mtmr_wait(mod_id, 5 * TIME_OUT);
			 flush_commands(mod_id);
			 resistance_unloaded = measure_dmm50(mod_id);
			 talonCard->openAllRelays(false, true);
			 ConnectLoadtoAF2Shunt(mod_id, slot, chnl, talonCard, talonload);
			 vth_mtmr_wait(mod_id, 5 * TIME_OUT);
			 flush_commands(mod_id);
             resistance_loaded = measure_dmm50(mod_id);
			 setup_dmm50(mod_id, DCV, 5, 1.0e-03);
			 talonCard->openAllRelays(false, true);
			 vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
			 prime = false;
		   }
		   if(test_number == 16835)
			   pinCard->setDigitalChannelParameters(pin, source);   
		   else 
			   pinCard->setDigitalChannelParameters(pin, sink);  	
		   vth_mseq_neg_cpu_tsp(mod_id);
		   if(meas == 0)
		   {
		       ConnectDrivertoAF2(mod_id, slot, chnl, talonCard);
     		   pinCard->spewToVthStream();    		   
			   vth_mtmr_wait(mod_id, TIME_OUT);
			   flush_commands(mod_id);
			   volts_noload[chnl] = measure_dmm50(mod_id);
		   }
		   else
		   {
			   ConnectLoadtoAF2Series(mod_id, slot, chnl, talonCard, talonload);
     		   pinCard->spewToVthStream();    	
			   vth_mtmr_wait(mod_id, TIME_OUT);	
			   flush_commands(mod_id);
			   volts_load[chnl] = measure_dmm50(mod_id);
		   }
		   vth_mseq_ass_cpu_tsp(mod_id);
		   pinCard->setDigitalChannelParameters(pin, off);   
		   pinCard->spewToVthStream();  
		   talonCard->openAllRelays(false, true);
		   flush_commands(mod_id);
		}/*for: channel*/
	}
    for (TestUtils::ChannelName chnl_res = tps[LOOP].begin; chnl_res < tps[LOOP].end; ++chnl_res)
	{
	   delta_v = volts_noload[chnl_res] - volts_load[chnl_res];
	   delta_i = volts_load[chnl_res] / resistance_loaded - volts_noload[chnl_res] / resistance_unloaded;
	   if (delta_v != 0)
	     driver_impedance = delta_v / delta_i;
	   else
	     driver_impedance = 1.00e+14;

	   result.Measurement_class = CLASS_REAL;
	   result.UU.Real_result = driver_impedance;
	   log_result(mod_id, result);
	}		   			   		   
   vth_pinit(mod_id, slot);
   close_dmm50(mod_id);
   close_mux(mod_id);
   CloseResourceManager();

}  /*t16835*/


/*
 *--------------------------------------------------------------------------------
 * Test: 16840, 16841
 * Description: Maximum drive current 
 *				16840: sourcing
 *				16841: sinking
 *				
 *			    These tests are modeled after tests 9140
 * Fixture: Sales Digital
 * Resource(s): DMM50.
 *--------------------------------------------------------------------------------
*/

#define INPUT_FREQ      20e+06
   
void drive_current(int mod_id, int slot, int test_number)
{
   static unsigned int const n = 1;
   int talonload = 65;
   double dmm_actual_voltage;
   double drive_current[144];
   double tclk_freq = INPUT_FREQ / 64;
   double dmm50_impedance;
   test_result_type result;
   vth_pinit(mod_id, slot);
   
   HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
   IPin *pinCard = TestPin::getPinCardAt(mod_id, slot);                        
   IMetaTesthead *th = MetaTesthead::getMetaTesthead(); 
   OpenResourceManager();
   open_mux(mod_id);
   open_dmm50(mod_id);
   setup_dmm50(mod_id, TWO_WIRE_OHMS, 100, 1);
   dmm50_impedance = measure_dmm50(mod_id);
   connect_dmm50_af2(mod_id);
   setup_dmm50(mod_id, DCV, 5, 1.0e-03);

	static Drvr_setup const tps[n] = 
	{ 
		Drvr_setup(TestUtils::c001, TestUtils::ChannelNamePastEnd, 5.00, 0.0),
	  //Drvr(TestUtils::c001, TestUtils::c028, 5.00, 0.0),
	};
	
	DigitalChannelParameters source;
	source.setTspSource(DigitalDriverTspResourceId::alwaysActive);                   
	source.setRcvSource(DigitalReceiverResourceId::alwaysDontCare);                  
	source.setDriveHigh(tps[LOOP].driveHigh);                                      
	source.setDriveLow(tps[LOOP].driveLow);                                                         
	source.setDrvSource(DigitalDriverResourceId::driveAOne);                         
	source.setPullUp(DigitalSignalConditionStateId::disabled);                       
	source.setPullDown(DigitalSignalConditionStateId::disabled);                     
	source.setReceiverTermination(DigitalSignalConditionStateId::disabled);          
	source.setDriveCheck(DigitalSignalConditionStateId::disabled); 
	DigitalChannelParameters sink;
	sink.setTspSource(DigitalDriverTspResourceId::alwaysActive);                   
	sink.setRcvSource(DigitalReceiverResourceId::alwaysDontCare);                  
	sink.setDriveHigh(tps[LOOP].driveHigh);                                      
	sink.setDriveLow(tps[LOOP].driveLow);                                                         
	sink.setDrvSource(DigitalDriverResourceId::driveAZero);                         
	sink.setPullUp(DigitalSignalConditionStateId::disabled);                       
	sink.setPullDown(DigitalSignalConditionStateId::disabled);                     
	sink.setReceiverTermination(DigitalSignalConditionStateId::disabled);          
	sink.setDriveCheck(DigitalSignalConditionStateId::disabled); 
    DigitalChannelParameters off;
    off.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);    
    off.setRcvSource(DigitalReceiverResourceId::alwaysDontCare); 

	for (TestUtils::ChannelName chnl = tps[LOOP].begin; chnl < tps[LOOP].end; ++chnl)
	{/*for: channel*/
	   PINDefinition::PinEnum pin(TestUtils::getPin(TestUtils::mapDrvChannelConnection(chnl)));
	   ConnectLoadtoAF2Series(mod_id, slot, chnl, talonCard, talonload);
	   if(test_number == 16840)
		   pinCard->setDigitalChannelParameters(pin, source);   
	   else 
		   pinCard->setDigitalChannelParameters(pin, sink);  	
	   vth_mseq_neg_cpu_tsp(mod_id);
	   pinCard->spewToVthStream();  
	   get_result_nolog(mod_id, &result);
	   dmm_actual_voltage = measure_dmm50(mod_id);
	   drive_current[chnl] = fabs(dmm_actual_voltage / dmm50_impedance);
	   dmm_actual_voltage = 0.0;
   }/*for: channel*/

   for (TestUtils::ChannelName chnl_res = tps[LOOP].begin; chnl_res < tps[LOOP].end; ++chnl_res)
   {
	   result.Measurement_class = CLASS_REAL;
	   result.UU.Real_result = drive_current[chnl_res];
	   log_result(mod_id, result);
   }		   			   		   

   vth_pinit(mod_id, slot);
   close_dmm50(mod_id);
   close_mux(mod_id);
   CloseResourceManager();
   
}  /*drive_current*/

#undef INPUT_FREQ
   

/*
 *--------------------------------------------------------------------------------
 * Test: 16847
 * Description: Driver Check Status
 *			   
 * Fixture: None
 * Resource(s): 
 *--------------------------------------------------------------------------------
*/
   
void t16847(int mod_id, int slot)
{
    static int PIN_MARK = 1;
	test_result_type result;
	vth_mod_init(mod_id);
	vth_pinit(mod_id, slot);
    HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
    vth_pinit(mod_id, slot);                                            /* Initialize pin card. */
    vth_mtmr_set_mark(mod_id, PIN_MARK);                                /* Start timer for pin card initialization settling time. */
    vth_mdclk_0_set_freq(mod_id, 20.0e+6);								/* Set DUTCLK output frequency. */
    vth_mdclk_en_out(mod_id);                                           /* Enable DUTCLK output buffer that goes to TCLK input mux. */
    vth_mtclk_dutclk_mux(mod_id);                                       /* Select DUTCLK input on TCLK mux. */
    vth_mtclk_set_freq(mod_id, 20.0e+6, 20.0e+6);						/* Set TCLK frequency based on input frequency. */
    static int TCLK_MARK = 2;
    vth_mtmr_set_mark(mod_id, TCLK_MARK);                               /* Start timer for TCLK generator settling time. */
#include "t16847.cmd"
	HTalon::writeAllPinCells(mod_id, slot, HTalon::FeatureChannelControl, HTalon::ChannelPullUpTermination); 
	HTalon::writeAllPinCells(mod_id, slot, HTalon::ReceiverVoltageReference, 0x0000);  
	vth_mseq_dbg_setup(mod_id, 1, 0xffffffff, 0xffffffff, 0);
    vth_mseq_ass_cpu_bra(mod_id);
	vth_mtmr_wait_fr_mark(mod_id, TCLK_MARK, 30.0e-3);  
	vth_mtmr_wait_fr_mark(mod_id, PIN_MARK, HTalon::initializeTime());
    vth_mseq_run(mod_id);
	get_result_nolog(mod_id, &result);
	Reset_sequencer(mod_id, slot); 
	HTalon::readPeregrineControlCell(mod_id, slot, HTalon::DriveCheckStatus); 
	TestReadBack::dumpMerlinDriveCheckStatusCells(mod_id, slot); 
	vth_mod_init(mod_id);
	vth_pinit(mod_id, slot);
	
} /*t16847*/


/*
 *--------------------------------------------------------------------------------
 * Test: 16850-16853
 * Description: Receive voltage accuracy 
 *				16850: +1.9v rising edge pulled up (low hystorysis trigger)
 *				16851: +1.9v falling edge pulled down (low hystorysis trigger)
 *				16852: +3.3v rising edge pulled up (high hystorysis trigger)
 *				16853: +3.3v falling edge pulled down (high hystorysis trigger)
 *			   
 * Fixture: Digital	
 * Resource(s): Pulse Generator
 *--------------------------------------------------------------------------------
*/

void reciver_voltage_accuracy(int mod_id, int slot, int test_number)
{
	static unsigned int const n = 2; 
	static int const rlyMark = 1;
	int SETUP;
	test_result_type result;
    result.Measurement_class = CLASS_REAL;
	
	OpenResourceManager();
	open_mux(mod_id);
	open_syn(mod_id);
	  if(check_syn_model())
	  {
		  setup_33250A_hv(mod_id,true);
		  enable_33250A_out(mod_id);
	  }
	open_dmm(mod_id);
	
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	struct rcv
	{
		TestUtils::ChannelName const begin;
		TestUtils::ChannelName const end;
		double const driveHigh;
		double const receiveHigh;
		double const expectLow;
		double const expectHigh;
		double const freq; 
		double scaleDriveV(double n) const { return n * 1000.0; }
		double rangeDriveV(double v) const { assert((v >= 0.0) && (v <= 5.0)); return v; }
		double scaleReceiveV(double n) const { return (n + .15) * 1000.0; } // Compensate for receiver offset of ~ 150mV.
		double rangeReceiveV(double v) const { assert((v >= 0.0) && (v <= 4.85)); return v; }
		rcv(TestUtils::ChannelName const &b, TestUtils::ChannelName const &e, double const &dh, double const &rh, double const &el, double const &eh, double const &fr) 
			: begin(b), end(e), driveHigh(scaleDriveV(rangeDriveV(dh))), receiveHigh(scaleReceiveV(rangeReceiveV(rh))), expectLow(el), expectHigh(eh), freq(fr) {}
	};
	
	static rcv const tps[n] =
	{
		/*         from pin                       to pin+1  DH    RH     ExpL   ExpH   Freq */
		rcv(TestUtils::c001, TestUtils::ChannelNamePastEnd, 0.25, 1.650, 0.470, 0.950, 5e06), 
		rcv(TestUtils::c001, TestUtils::ChannelNamePastEnd, 3.30, 1.650, 1.870, 1.000, 5e06), 
//		rcv(TestUtils::c001, TestUtils::ChannelNamePastEnd, 0.25, 1.650, 0.000, 1.900, 5e06), 
//		rcv(TestUtils::c001, TestUtils::ChannelNamePastEnd, 3.30, 1.650, 1.400, 3.300, 5e06), 
	};

	if((test_number == 16850) || (test_number == 16851))
		SETUP = 0;                           
	else
		SETUP = 1; 

	DigitalChannelParameters off;
	off.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);               
	off.setRcvSource(DigitalReceiverResourceId::alwaysDontCare);                  
	DigitalChannelParameters rising;
	rising.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);       
	rising.setRcvSource(DigitalReceiverResourceId::alwaysEnabled);        
	rising.setDriveHigh(tps[SETUP].driveHigh);                                
	rising.setDriveLow(0000.0);                                           
	rising.setReceiveHigh(tps[SETUP].receiveHigh);                            
	rising.setReceiveLow(0000.0);                                         
	rising.setPullUp(DigitalSignalConditionStateId::enabled);                  
	rising.setPullDown(DigitalSignalConditionStateId::disabled);               
	rising.setReceiverTermination(DigitalSignalConditionStateId::enabled);     
	rising.setDriveCheck(DigitalSignalConditionStateId::disabled);             
	DigitalChannelParameters falling;
	falling.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);       
	falling.setRcvSource(DigitalReceiverResourceId::alwaysEnabled);        
	falling.setDriveHigh(tps[SETUP].driveHigh);                                
	falling.setDriveLow(0000.0);                                           
	falling.setReceiveHigh(tps[SETUP].receiveHigh);                            
	falling.setReceiveLow(0000.0);                                         
	falling.setPullUp(DigitalSignalConditionStateId::disabled);                  
	falling.setPullDown(DigitalSignalConditionStateId::enabled);               
	falling.setReceiverTermination(DigitalSignalConditionStateId::enabled);     
	falling.setDriveCheck(DigitalSignalConditionStateId::disabled);             
	IPin * pinCard = TestPin::getPinCardAt(mod_id, slot);                   
	IMetaTesthead * th = MetaTesthead::getMetaTesthead();  
	CloseDigitalFixtureGround(mod_id, slot, talonCard);
    connect_syn_out_af2(mod_id);
	setup_dmm(mod_id, DCV, 5, 1.0e-03);
		if((test_number == 16850) || (test_number == 16852))
	{
      if(check_syn_model())
		  setup_33250A(mod_id, POS_RAMP, tps[SETUP].freq, tps[SETUP].expectHigh, tps[SETUP].expectLow);
	  else
		  setup_syn(mod_id, POS_RAMP, tps[SETUP].freq, tps[SETUP].expectHigh, tps[SETUP].expectLow);
	}
	else
	{
		if(check_syn_model())
			setup_33250A(mod_id, NEG_RAMP, tps[SETUP].freq, tps[SETUP].expectHigh, tps[SETUP].expectLow);
		else
			setup_syn(mod_id, NEG_RAMP, tps[SETUP].freq, tps[SETUP].expectHigh, tps[SETUP].expectLow);
	}
	for (TestUtils::ChannelName chnl = tps[SETUP].begin; chnl < tps[SETUP].end; ++chnl)
	{/*for: channel*/
		PINDefinition::PinEnum pin(TestUtils::getPin(TestUtils::mapDrvChannelConnection(chnl)));
		ConnectDrivertoAF2(mod_id, slot, chnl, talonCard);
		th->processQueuedActions();                                               
		vth_mtmr_wait(mod_id, TestUtils::talonAsruCloseRelayTime());    
		if((test_number == 16850) || (test_number == 16852))
			pinCard->setDigitalChannelParameters(pin, rising);                           
		else
			pinCard->setDigitalChannelParameters(pin, falling); 
		pinCard->spewToVthStream(); 
		vth_mtmr_wait_fr_mark(mod_id, rlyMark, WAIT_TIME_FOR_RELAYS);           
		enable_gen_out(mod_id);
		vth_mtmr_wait(mod_id, TIME_OUT);  
		flush_commands(mod_id);
//		result.UU.Real_result = measure_dmm(mod_id);
//	    log_result(mod_id, result);
		vth_pfc_rd_rcv_state(mod_id, slot, pin);   
		pinCard->setDigitalChannelParameters(pin, off);                          
		pinCard->spewToVthStream();                                              
		th->processQueuedActions();     
		//get_result_nolog(mod_id, &result);
		get_result_log(mod_id, &result);
	}

	vth_pinit(mod_id, slot);
	close_syn(mod_id);
	close_dmm(mod_id);
	close_mux(mod_id);
	CloseResourceManager();
	
}/*reciver_voltage_accuracy*/


/*
 *--------------------------------------------------------------------------------
 * Test: 16860-16861
 * Description: Receiver input current 
 *				16860: sourcing  
 *				16861: sinking
 *			   This test is modeled after tests 9160/9161
 * Fixture: Digital	
 * Resource(s): DMM
 *--------------------------------------------------------------------------------
*/
   
#define PULL_UP_LOAD_RESISTANCE  389e+00
#define RANGE           3
#define RESOLUTION      1.0e-03

void reciever_input_current(int mod_id, int slot, int test_number)
{
	static unsigned int const n = 1;
	double actual_voltage[144];
//	double pull_up_current;
	test_result_type result;

	vth_pinit(mod_id, slot);
	vth_ainit(mod_id);
	OpenResourceManager();
	open_dmm(mod_id);
	open_mux(mod_id);
	
    HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	IPin *pinCard = TestPin::getPinCardAt(mod_id, slot);                        
	IMetaTesthead *th = MetaTesthead::getMetaTesthead(); 
	
	setup_dmm(mod_id, DCV, (double)RANGE, RESOLUTION);
	connect_dmm_pin(mod_id, slot);
	connect_500_ohm_load(mod_id);
	vth_cfxt_lock();
	
	struct Rcvr
    {
		TestUtils::ChannelName const begin;
		TestUtils::ChannelName const end;
		double const expectedHigh;
		double const expectedLow;
		double scale(double s) const { return s * 1000.0; }
		double rangeV(double v) const { assert((v >= 0.0) && (v <= 5.0)); return v; }
		Rcvr(TestUtils::ChannelName const &b, TestUtils::ChannelName const &e, double const &eh, double const &el) 
			: begin(b), end(e), expectedHigh(scale(rangeV(eh))), expectedLow(scale(rangeV(el))) {}
	};

	static Rcvr const tps[n] = 
	{
		Rcvr(TestUtils::c001, TestUtils::ChannelNamePastEnd, 5.00, 0.00), 
//		Rcvr(TestUtils::c101, TestUtils::c105, 5.00, 0.00),
	};
	
	DigitalChannelParameters off;
	off.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);            /* Disable driver. */
	off.setRcvSource(DigitalReceiverResourceId::alwaysDontCare); 
	DigitalChannelParameters source;
	source.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);                   
	source.setRcvSource(DigitalReceiverResourceId::alwaysEnabled);                  
	source.setDriveHigh(tps[LOOP].expectedLow);                                      
	source.setDriveLow(tps[LOOP].expectedLow); 
	source.setReceiveHigh(tps[LOOP].expectedLow);                                      
	source.setReceiveLow(tps[LOOP].expectedLow);                                                         
	source.setPullUp(DigitalSignalConditionStateId::disabled);                       
	source.setPullDown(DigitalSignalConditionStateId::enabled);                     
	source.setReceiverTermination(DigitalSignalConditionStateId::disabled);          
	source.setDriveCheck(DigitalSignalConditionStateId::disabled); 
	DigitalChannelParameters sink;
	sink.setTspSource(DigitalDriverTspResourceId::alwaysActive);                   
	sink.setRcvSource(DigitalReceiverResourceId::alwaysEnabled);                  
	sink.setDriveHigh(tps[LOOP].expectedHigh);                                      
	sink.setDriveLow(tps[LOOP].expectedLow); 
	sink.setReceiveHigh(tps[LOOP].expectedHigh);                                      
	sink.setReceiveLow(tps[LOOP].expectedLow);                                                         
	sink.setPullUp(DigitalSignalConditionStateId::enabled);                       
	sink.setPullDown(DigitalSignalConditionStateId::disabled);                     
	sink.setReceiverTermination(DigitalSignalConditionStateId::disabled);          
	sink.setDriveCheck(DigitalSignalConditionStateId::disabled); 
    vth_aclose_rly(mod_id, K806);
	vth_aclose_rly(mod_id, K826);
	vth_aclose_rly(mod_id, K847);
	vth_aclose_rly(mod_id, K816);
	vth_aclose_rly(mod_id, K814);

	for (TestUtils::ChannelName chnl = tps[LOOP].begin; chnl < tps[LOOP].end; ++chnl)
	{/*for: channel*/
     	CloseDigitalFixtureGround(mod_id, slot, talonCard);
		PINDefinition::PinEnum pin(TestUtils::getPin(TestUtils::mapDrvChannelConnection(chnl)));
		ConnectDigitalFixture(mod_id, slot, pin, th, pinCard);
		BRC brc(mod_id, slot, pin);                                                
		ConnectBRCtoBus *DSBus = ConnectBRCtoBus::createInstance();
		ConnectDigitalResource *Rcvr = ConnectDigitalResource::createInstance(); 
		DSBus->setBusID(MeasurementBus::S);
		Rcvr->setBusToDriver();
		DSBus->setBRC(brc); 
		Rcvr->setBRC(brc); 
		DSBus->setState(ConnectBRCtoBus::connect);                                                         
		Rcvr->setState(ConnectDigitalResource::connect); 
		th->addActionToQueue(Rcvr);
		th->addActionToQueue(DSBus); 
		th->processQueuedActions();  
		if(test_number == 16860)
		{
		   vth_asrc_set_dc_ampl(mod_id, .5e00);
		   pinCard->setDigitalChannelParameters(pin, source);   
		}
	    else 
		{
		   vth_asrc_set_dc_ampl(mod_id, 4.5e00);
		   pinCard->setDigitalChannelParameters(pin, sink);  
		}
		pinCard->spewToVthStream();  
		vth_asrc_en_30ma_ilim(mod_id);
		vth_afc_en_src_out(mod_id);
		vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);
		get_result_nolog(mod_id, &result);		//clean pipe (flushes message pipe)
		actual_voltage[chnl] = measure_dmm(mod_id);
		pinCard->setDigitalChannelParameters(pin, off);   
		pinCard->spewToVthStream();  
		vth_afc_dis_src_out(mod_id);
		talonCard->openAllRelays(false, true);
		vth_astat_clr(mod_id);
		vth_mtmr_wait(mod_id, TIME_OUT);
		get_result_nolog(mod_id, &result);		//clean pipe (flushes message pipe)
	}/*for: channel*/

	for (TestUtils::ChannelName chnlres = tps[LOOP].begin; chnlres < tps[LOOP].end; ++chnlres)
	{/*for: channel*/
        result.Measurement_class = CLASS_REAL;
		result.UU.Real_result = actual_voltage[chnlres] / PULL_UP_LOAD_RESISTANCE;
		log_result(mod_id, result);
	}
	
    vth_pinit(mod_id, slot);
	vth_ainit(mod_id);
    close_dmm(mod_id);
    close_mux(mod_id);
	CloseResourceManager();
	   
} /*reciever_input_current*/

#undef RANGE
#undef RESOLUTION
#undef PULL_UP_LOAD_RESISTANCE


/*
 *--------------------------------------------------------------------------------
 * Test: 16862
 * Description: Receiver input leakage current
 *			   This test is modeled after tests 9162
 * Fixture: Digital	
 * Resource(s): DMM
 *--------------------------------------------------------------------------------
*/

#define RANGE           300e-03
#define RESOLUTION      1.0e-03
#define RECEIVER_LOAD_RESISTANCE  500e+00

void t16862(int mod_id, int slot)
{
	static unsigned int const n = 3;
	double receiver_input_voltage;
	double receiver_input_current;
	
	test_result_type result;
	OpenResourceManager();
	open_dmm(mod_id);
	open_mux(mod_id);
	setup_dmm(mod_id, DCV, RANGE, RESOLUTION);
	connect_dmm_pin(mod_id, slot);
	connect_500_ohm_load(mod_id);
	vth_cfxt_lock();
	
	struct Rcvr
    {
		PINDefinition::PinEnum const begin;
		PINDefinition::PinEnum const end;
		double const recieveLow;
		double const recieveHigh;
		double scale(double s) const { return s * 1000.0; }
		double rangeV(double v) const { assert((v >= 0.0) && (v <= 5.0)); return v; }
		Rcvr(PINDefinition::PinEnum const &b, PINDefinition::PinEnum const &e, double const &rl, double const &rh) 
			: begin(b), end(e), recieveLow(scale(rangeV(rl))), recieveHigh(scale(rangeV(rh))) {}
	};
	
	static Rcvr const tps[n] = { Rcvr(PINDefinition::pin1, PINDefinition::PinEnd, 1.00, 2.00),
								 Rcvr(PINDefinition::pin1, PINDefinition::PinEnd, -2.00, 2.00),
								 Rcvr(PINDefinition::pin1, PINDefinition::PinEnd, -2.00, -1.00),
								};			
	//	static Rcvr const tps[n] = { Rcvr(PINDefinition::pin16, PINDefinition::pin25, 5.00, 0.00),};
	
    HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	talonCard->openAllRelays(false, true);	// Initalize Talon
	IPin *pinCard = TestPin::getPinCardAt(mod_id, slot);                        
	IMetaTesthead *th = MetaTesthead::getMetaTesthead(); 
	
	for (unsigned int p = 0; p < n; ++p)
	{/*for: passes*/
		DigitalChannelParameters rv;
		rv.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);                   
		rv.setRcvSource(DigitalReceiverResourceId::alwaysEnabled);                  
		rv.setDriveHigh(0.0);                                      
		rv.setDriveLow(0.0); 
		rv.setReceiveHigh(tps[p].recieveHigh);                                      
		rv.setReceiveLow(tps[p].recieveLow);                                                         
		rv.setPullUp(DigitalSignalConditionStateId::disabled);                       
		rv.setPullDown(DigitalSignalConditionStateId::enabled);                     
		rv.setReceiverTermination(DigitalSignalConditionStateId::disabled);          
		rv.setDriveCheck(DigitalSignalConditionStateId::disabled); 
		DigitalChannelParameters off;
		off.setTspSource(DigitalDriverTspResourceId::alwaysHiZ);            /* Disable driver. */
		off.setRcvSource(DigitalReceiverResourceId::alwaysDontCare);
		CloseDigitalFixtureGround(mod_id, slot, talonCard);
		
		for (PINDefinition::PinEnum pin = tps[p].begin; pin < tps[p].end; ++pin)
		{/*for: channel*/
			ConnectDigitalFixture(mod_id, slot, pin, th, pinCard);
			BRC brc(mod_id, slot, pin);                                                
			ConnectBRCtoBus *DSBus = ConnectBRCtoBus::createInstance();
			ConnectDigitalResource *Rcvr = ConnectDigitalResource::createInstance(); 
			DSBus->setBusID(MeasurementBus::S);
			Rcvr->setBusToDriver();
			DSBus->setBRC(brc); 
			Rcvr->setBRC(brc); 
			DSBus->setState(ConnectBRCtoBus::connect);                                                         
			Rcvr->setState(ConnectDigitalResource::connect); 
			th->addActionToQueue(Rcvr);
			th->addActionToQueue(DSBus); 
			th->processQueuedActions();                                              
			pinCard->setDigitalChannelParameters(pin, rv);                         
			pinCard->spewToVthStream();  
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
			
			pinCard->setDigitalChannelParameters(pin, off);   
			pinCard->spewToVthStream();  
			talonCard->openAllRelays(false, true);
			vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);
			
		}/*for: channel*/
		
	   }/*for: passes*/
	
	   talonCard->openAllRelays(false, true);
	   vth_pinit(mod_id, slot);
	   close_dmm(mod_id);
	   close_mux(mod_id);
	   CloseResourceManager();
	   
}  /* t16862 */

#undef RANGE
#undef RESOLUTION
#undef RECEIVER_LOAD_RESISTANCE


/*
 *--------------------------------------------------------------------------------
 * Test: 16871 16872
 * Description: Reciever edge accuracy
 *			16871:	Rising edge
 *			16872:	Falling edge
 *			   
 * Fixture: Digital
 * Resource(s): Synthesizer, Generator, Counter
 *--------------------------------------------------------------------------------
*/

#define TCLK_FREQ       5.0e+06
#define GEN_LOW_50      0.20e+00
#define GEN_HIGH_50     5.0e+00
#define RX_SPEC         25e-09  //HDD spec: 20nS + 1% pattern rate 
#define PROG_DELAY      20e-09
#define TCLK_ADV_TIME   40e-09
#define STARTING_INCREMENT  2.0e-09

void reciever_edge(int mod_id, int slot, int test_number)
{  
	static unsigned int const n = 1;
	int edge;
	int direction = 1;
	int expected_response;
	double actual_receive;
	double expected_receive;
	double gen_delay;
	double min_gen_delay;
	double max_gen_delay;
	double max_gen_meas;
	double rx_range = 2 * RX_SPEC; // rx_range is the +/- max value we will check to see if a receive occurred
	double ch_result[144][2];
	double delay_inc;
	double delta_delay;
   	double offset[2];
    double gain[2];
	double correction; 
	double edge_delay;
	bool rresp = false;
	bool hit = false;
	bool found = false;
	test_result_type result;
//	FILE *ofile;
//	ofile = fopen("rcv_plcmnt.txt", "a");

	RACK_DELAYS Rack;
	OpenResourceManager();
	open_syn(mod_id);
	  if(check_syn_model())
	  {
		  setup_33250A_hv(mod_id, true);
		  enable_33250A_out(mod_id);
	  }
	open_gen(mod_id);
	open_mux(mod_id);
	open_cntr(mod_id);
	vth_cfxt_lock();
	vth_mtmr_wait(mod_id, FIXTURE_LOCK_TIME);
	Rack.ReadDelays();
	
    static Drvr const tps[n] = { Drvr(TestUtils::c001, TestUtils::ChannelNamePastEnd),};
//	   static Drvr const tps[n] = { Drvr(TestUtils::c112, TestUtils::c115),};
   
   vth_mod_init(mod_id);
   HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
   correction = calc_mod_loading(mod_id, 0);
   delta_delay = Rack.AF2System(mod_id) +
	   (SLOT_DELAY[slot - 1][RISING]
	   + SLOT_DELAY[slot - 1][FALLING]) / 2
	   + RX_PATH_OFFSET - Rack.AF2Clkrb(mod_id);
						
   expected_receive = MAC_11_T0_ACTUAL + PROG_DELAY - TCLK_ADV_TIME;
   min_gen_delay = nanotrunc(expected_receive - delta_delay - rx_range);
   max_gen_delay = nanotrunc(expected_receive - delta_delay + rx_range);

   connect_syn_out_gen_ext(mod_id);
   connect_gen_trg_cntr_start(mod_id);
   connect_gen_out_cntr_stop(mod_id);
   setup_gen_levels(mod_id, GEN_LOW_50, GEN_HIGH_50);
   
   for (edge = RISING; edge <= FALLING; ++edge)
   {  // Measure the offset and gain for pos and neg generator output 
	   setup_gen_trg_out_delay(mod_id, min_gen_delay, true);
	   enable_gen_out(mod_id);
	   vth_mtmr_wait(mod_id, 0.3);
	   trigger_cntr(mod_id);
	   offset[edge] = read_cntr_mean(mod_id);
	   setup_gen_trg_out_delay(mod_id, max_gen_delay, true);
	   vth_mtmr_wait(mod_id, 0.3);
	   trigger_cntr(mod_id);
	   max_gen_meas = read_cntr_mean(mod_id);
	   disable_gen_out(mod_id);
	   gain[edge] = (max_gen_meas - offset[edge]) / (max_gen_delay - min_gen_delay);
	   complement_gen_out(mod_id);
	   setup_cntr_trigger_slopes(mod_id, POS, NEG);
   }  
   
   vth_mtmr_wait(mod_id, TIME_OUT);	
   no_complement_gen_out(mod_id);
   setup_gen_levels(mod_id, GEN_LOW_50, GEN_HIGH_50);
   connect_gen_trg_clkrb(mod_id);
   connect_gen_out_af2(mod_id);
   vth_mclose_rly(mod_id, MK2902);   // clkrb path relay 
   vth_mclose_rly(mod_id, MK2903);   // clkrsg path relay 
   
   /* Set up the TCLK generator to sync to the external CLKRB (gen trig) signal */
   vth_mtclk_clkrb_mux(mod_id);
   vth_mtclk_sel_ris_edge(mod_id);
   vth_mtclk_set_freq(mod_id, TCLK_FREQ, TCLK_FREQ);
   vth_mtclk_set_adv_time(mod_id, TCLK_ADV_TIME);
   
   /* Enable the ARC lines from pin to mom board and from mom board to control */
   vth_mtmr_set_mark(mod_id, 1);
   vth_pen_arcv0(mod_id, slot);
   vth_mmbd_en_arcv(mod_id);
   vth_mtmr_wait_fr_mark(mod_id, 1, 30e-3);

#include "t16871.cmd"
   
   if(test_number == 16871) 
   {
	   no_complement_gen_out(mod_id);   
	   expected_response = true;
	   edge = RISING;
	   edge_delay = 0.0;
   }  
   else
   {  
	   complement_gen_out(mod_id);		
	   expected_response = false;
	   edge = FALLING;
//	   edge_delay = 36.0e-09;
   }  
   /* Recalculate delta_delay for the particular edge that we are testing */
   CloseDigitalFixtureGround(mod_id, slot, talonCard);
   delta_delay = Rack.AF2System(mod_id) + SLOT_DELAY[slot - 1][edge] 
	   + RX_PATH_OFFSET - Rack.AF2Clkrb(mod_id);

   for (TestUtils::ChannelName chnl = tps[LOOP].begin; chnl < tps[LOOP].end; ++chnl)
   {/*for: channel*/
	   PINDefinition::PinEnum pin(TestUtils::getPin(TestUtils::mapDrvChannelConnection(chnl)));
	   ConnectDrivertoAF2(mod_id, slot, chnl, talonCard);
	   gen_delay = nanotrunc(expected_receive - delta_delay + 1e-09 - edge_delay);
	   delay_inc = STARTING_INCREMENT;
	
       HTalon::writeAllMerlinBusCells(mod_id, slot, HTalon::DriveCheckEnable, HTalon::DriveCheckDisableAll);  /* Turn off all drive check--fatal errors get stuck on with no way to clear.  [REVIEW NEED IN VERSIONS AFTER P1] */
	   do
	   {
		   setup_gen_trg_out_delay(mod_id, gen_delay, false);
		   enable_gen_out(mod_id);
		   Start_sequencer(mod_id, slot);
		   vth_mmbd_rd_arcv0(mod_id);
		   get_result_nolog(mod_id, &result);
		   if (result.UU.Integer_result == 1)   
			   found = true;
		   else
		   { // Determine if 'hit' and set the direction of delay based on rresp 
		       vth_pfc_sel_arcv0(mod_id, slot, pin, 3); // 3 = Synchronous receive data (formerly RRESP)
		       get_result_nolog(mod_id, &result);
		       if (result.UU.Integer_result == expected_response)
			   {  
				   if (direction < 0)			// can never hit on first point 
					   hit = true;
				   direction = -1;				// set direction to decrease delay 
			   }  
			   else
			   {  
				   if (direction > 0)			// can never hit on first point 
					   hit = true;
				   direction = 1;				// set direction to increase delay 
			   }  
			   gen_delay += delay_inc * direction;
			   Reset_sequencer(mod_id, slot);
		   }  
		   
	   } while (!(found || gen_delay < min_gen_delay || gen_delay > max_gen_delay));
	   disable_gen_out(mod_id);
	   if (!found)
	   {   
		   display_prompt("**RECEIVE DOES NOT OCCUR WITHIN TEST RANGE**", true);
		   ch_result[chnl][edge] = INT_MAX;
	   }
	   else
	   {
		   actual_receive = gain[edge] * (gen_delay - min_gen_delay) +
			   offset[edge] + delta_delay;
		   ch_result[chnl][edge] = actual_receive - expected_receive;
	   }
	   talonCard->openAllRelays(false, true);
	   vth_mtmr_wait(mod_id, RELAY_OPEN_WAIT_TIME);	
	   found = false;
	   hit = false;
	   rresp = false;
	   direction = 0;
   }  /* for: channel */

   for (TestUtils::ChannelName chnl_res = tps[LOOP].begin; chnl_res < tps[LOOP].end; ++chnl_res)
   { 
	   result.Measurement_class = CLASS_REAL;
	   result.UU.Real_result = ch_result[chnl_res][edge] + correction;
	   log_result(mod_id, result);
   }   

   vth_mtic_neg_no_rtn_flg(mod_id);                                        
   vth_pdis_arcv0(mod_id, slot);
   close_gen(mod_id);
   close_syn(mod_id);
   close_cntr(mod_id);
   close_mux(mod_id);
   CloseResourceManager();
   vth_mod_init(mod_id);
// fclose(ofile);
}  /*receiver_edge*/

#undef TCLK_FREQ
#undef GEN_LOW_50
#undef GEN_HIGH_50
#undef RX_SPEC
#undef PROG_DELAY
#undef TCLK_ADV_TIME
#undef STARTING_INCREMENT


/*
 *--------------------------------------------------------------------------------
 * Test: 16900
 * Description: ADC verification
 *			   
 * Fixture: Digital
 * Resource(s): Pulse Generator
 *--------------------------------------------------------------------------------
*/

void t16900(int mod_id, int slot)
{
	int cycle = 5;
	double offset = 0.1;
	double amplitude[5] = {5.0, 10.0, 0.5, -5.0, -10.0};
	test_result_type result;
	OpenResourceManager();
	open_gen(mod_id);
	open_mux(mod_id);
	connect_gen_out_af2(mod_id);
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
    for(int side = SIDEA; side <= SIDEB; side++)
	{
	    ConnectAF2toADC(mod_id, slot, side, talonCard);
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
		for(int iterations = 0; iterations < cycle; iterations++)
		{
			no_complement_gen_out(mod_id);
			setup_gen_levels(mod_id, offset, amplitude[iterations]);
			setup_gen_trg_out_delay(mod_id, offset, true);
            enable_gen_out(mod_id);
			vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME);
			vth_padc_rd_volt(mod_id, slot, side);  /* Make the ADC measurement. */
			get_result_nolog(mod_id, &result);
			disable_gen_out(mod_id);
			vth_mtmr_wait(mod_id, STANDARD_WAIT_TIME); 	
		}
	}
	vth_pinit(mod_id, slot);
	close_gen(mod_id);
	close_mux(mod_id);
	CloseResourceManager();
}


/*
 *--------------------------------------------------------------------------------
 * Test: 16904
 * Description: Ground difference test
 *			   
 * Fixture: Digital
 * Resource(s): DMM
 *--------------------------------------------------------------------------------
*/

#define RANGE           1		  
#define RESOLUTION      1.0e-06   

void t16904(int mod_id, int slot)
{
	static unsigned int const n = 2;
	double res; 
	test_result_type result;
	OpenResourceManager();
	open_dmm(mod_id);
	open_mux(mod_id);
	setup_dmm(mod_id, DCV, (double)RANGE, RESOLUTION);
	connect_dmm_pin(mod_id, slot);
	vth_cfxt_lock();
	HTalon *talonCard = TestTalon::getTalonCardAt(mod_id, slot); 
	for (unsigned int p = 0; p < n; p++)
	{/*for: passes*/
		dconnect_minus_swd_gnd(mod_id);
		CloseDigitalFixtureGround(mod_id, slot, talonCard); 
		vth_aclose_rly(mod_id, K761);   /*SWDGND relay*/
		vth_mtmr_wait(mod_id, WAIT_TIME_FOR_RELAYS);
		get_result_log(mod_id, &result);
		res = measure_dmm(mod_id);
		result.Measurement_class = CLASS_REAL;
		result.UU.Real_result = res;
		log_result(mod_id, result);
	}/*for: passes*/
	
	vth_pinit(mod_id, slot);
	close_dmm(mod_id);
	close_mux(mod_id);
	CloseResourceManager();
	
}  /*t16904*/

#undef RANGE
#undef RESOLUTION


/*
 *--------------------------------------------------------------------------------
 * Test: 16999
 * Description: Firmware/card revision verification
 *			   
 * Fixture: None
 * Resource(s): 
 *--------------------------------------------------------------------------------
*/
   
void t16999(int mod_id, int slot)
{
//	test_result_type result;
	HTalon::readPeregrineControlCell(mod_id, slot, HTalon::KestrelRevision);         /* Read Kestrel revision code. */
//convert_byte_buffer_as_words(mod_id);
/*    result.Measurement_class = CLASS_BOOLEAN;
    result.UU.Boolean_result = false;
    log_result(mod_id, result);*/

    HTalon::readPeregrineControlCell(mod_id, slot, HTalon::CardReset_RevisionCode);  /* Read Peregrine revision code. */
	TestReadBack::dumpMerlinResetRevisionCodeCells(mod_id, slot);                    /* Read Merlin revision codes. */
	HTalon::readPeregrineControlCell(mod_id, slot, HTalon::CardIdStatus);            /* Read Kestrel card ID. */
        
}/*t16999*/
