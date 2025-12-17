#include <p2c/p2c.h>
#include <assert.h>
#include "mt10000_p.h"
#include "digital_const_p.h"
#include "mtglobal_p.h"
#include "../../../uCC/src/future.h"
#include "../../../vmc/src/vmc.h"
#include "../../../u/src/num_p.h"

#define NUM_LOOPS       10   /* buzz loops/relay */
typedef int loop_type__;
/* #define PRECLOSURES     5 */

/* static variables for t10188: */
struct LOC_t10188
{
  int mod_id;
  int slot;
} ;

static void buzz_that_relay_10188(int relay, struct LOC_t10188 * LINK)
{
	test_result_type resultBuzz;
	double aryResult[NUM_LOOPS+1], dblMax, dblMin;
	loop_type__ bloop;

	for (bloop = 1; bloop <= NUM_LOOPS; bloop++)
	{  /* for bloop */
		vth_aclose_rly(LINK->mod_id, K820);
		vth_rlc_setup(LINK->mod_id,
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
		vth_aopen_rly(LINK->mod_id, K814);
		vth_mtmr_wait(LINK->mod_id, 0.2e-3);
		vth_aclose_rly(LINK->mod_id, K813);   /*connect DETLO with I bus*/
		vth_aclose_rly(LINK->mod_id, K816);   /*connect G bus with I bus*/
		vth_mtmr_wait(LINK->mod_id, 0.2e-3);

		vth_popen_rly(LINK->mod_id, LINK->slot, relay);
		vth_mtmr_wait(LINK->mod_id, 0.08e-3);
		vth_pclose_rly(LINK->mod_id, LINK->slot, relay);
		vth_mtmr_wait(LINK->mod_id, 0.08e-3);
  		vth_mtmr_wait(LINK->mod_id, 2.0e-3);
		vth_resistor_meas(LINK->mod_id, 0, 1);
		vth_get_abs_resistor_result(LINK->mod_id);
	}  /* for bloop */

	get_result_nolog(LINK->mod_id, &resultBuzz);
	aryResult[1] = resultBuzz.UU.Real_result;
	dblMax = dblMin = aryResult[1];
	for (int i=2; i<=NUM_LOOPS; i++)
	{
		get_result_nolog(LINK->mod_id, &resultBuzz);
		aryResult[i] = resultBuzz.UU.Real_result;
		if (aryResult[i] > dblMax)
			dblMax = aryResult[i];
		else if (aryResult[i] < dblMin)
			dblMin = aryResult[i];
	}
	resultBuzz.UU.Real_result = dblMax - dblMin;
	log_result(LINK->mod_id, resultBuzz);

}  
/* buzz_that_relay */

//------------------------------------------------------------------------------
//t10188
//
//This test combines test9189 (relay close measurement) with AnalyzeBuzz script
//It uses buzz_that_relay_10188 function.
//
//------------------------------------------------------------------------------
void t10188(int mod_id, int slot)
{

	struct LOC_t10188 V;
	int mux;
	int channel;
	boolean virgin_card;
	boolean virgin_channel;
	int card_type=0;

	V.mod_id = mod_id;
	V.slot = slot;

	card_type = get_testhead_actual_card(V.mod_id, V.slot);
	/* determine if card is caribou and put in compatibility mode */
	if ((card_type == CDDP) || (card_type == CDD20) ||
	(card_type == CDD6) || (card_type == CDDJB) ||
	(card_type == CDDU))
		vth_pdis_32chan(V.mod_id, V.slot);

	/*PIN relay exercise*/
	display_prompt("extra loop", true);

	buzz_prelays(V.mod_id, V.slot);

	/*initialization*/
	vth_aunpowered_init(V.mod_id);
	vth_ameas_set_averaging(V.mod_id, 1);
 vth_mtmr_wait(mod_id, 0.3); //added fikri 31/5/2024

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
				buzz_that_relay_10188(KDCG, &V);
				buzz_that_relay_10188(KDCL, &V);
				virgin_card = false;
			}  /* buzz once per card relays */

			if (virgin_channel)
			{  /* buzz once per channel relays */
				buzz_that_relay_10188(PIN_KDC_RLYS[channel - 'A'], &V);
				buzz_that_relay_10188(PIN_KX_RLYS[channel - 'A'], &V);
				buzz_that_relay_10188(PIN_KGL_RLYS[channel - 'A'], &V);
				virgin_channel = false;
			}  /* buzz once per channel relays */

			if (mux == 6)
			{ /* buzz muxing relays in case card is caribou */
				buzz_that_relay_10188(PIN_KX_RLYS[channel - 'A'], &V);
				buzz_that_relay_10188(PIN_KGL_RLYS[channel - 'A'], &V);
			}

			buzz_that_relay_10188(PIN_DRVR_RLYS[mux][channel - 'A'], &V);
			buzz_that_relay_10188(PIN_RCVR_RLYS[mux][channel - 'A'], &V);

			vth_popen_rly(V.mod_id, V.slot, PIN_DRVR_RLYS[mux][channel - 'A']);
			vth_popen_rly(V.mod_id, V.slot, PIN_RCVR_RLYS[mux][channel - 'A']);

		}  /* for mux */

		/*initialize all PIN and AIM relays*/
		vth_aopen_rly(V.mod_id, AIM_X_S_RLYS[channel - 'A']);
		vth_popen_rly(V.mod_id, V.slot, PIN_KDC_RLYS[channel - 'A']);
		vth_popen_rly(V.mod_id, V.slot, PIN_KX_RLYS[channel - 'A']);
		vth_popen_rly(V.mod_id, V.slot, PIN_KGL_RLYS[channel - 'A']);

	}  /*for channel*/

	vth_popen_all_rlys(V.mod_id, V.slot);

}  /* t10188 */
#undef NUM_LOOPS

#define NUM_LOOPS       10   /* buzz loops/relay */
/* #define PRECLOSURES     5 */

/* static variables for t10189: */
struct LOC_t10189
{
  int mod_id;
  int slot;
};

static void buzz_that_relay__(int relay, struct LOC_t10189 * LINK)
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
void t10189(int mod_id, int slot)
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

  /* adding functionality for Caribou -- Greg M 12-22-99 */
  /* duplicate of 9189 which works for Caribou -- Greg M 1-4-99 */

struct LOC_t10189 V;
int mux;
int channel;
boolean virgin_card;
boolean virgin_channel;
int card_type=0;

  V.mod_id = mod_id;
  V.slot = slot;

  card_type = get_testhead_actual_card(V.mod_id, V.slot);
  /* determine if card is caribou and put in compatibility mode */
  if ((card_type == CDDP) || (card_type == CDD20) ||
      (card_type == CDD6) || (card_type == CDDJB) ||
      (card_type == CDDU))
    vth_pdis_32chan(V.mod_id, V.slot);
//    vth_pwr_cell(V.mod_id, V.slot, PCOMPAT_DIS, 0x0);
 vth_mtmr_wait(mod_id, 0.3); //added fikri 31/5/2024

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
		vth_mtmr_wait(mod_id, 0.3); //added fikri 11062024
 		vth_mtmr_wait(mod_id, 0.3); //added fikri 11062024
        flush_commands(mod_id); //fikri 11062024

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

      if (mux == 6)
      { /* buzz muxing relays in case card is caribou */
        buzz_that_relay__(PIN_KX_RLYS[channel - 'A'], &V);
        buzz_that_relay__(PIN_KGL_RLYS[channel - 'A'], &V);
      }

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

  vth_popen_all_rlys(V.mod_id, V.slot);

}  /* t10189 */
#undef NUM_LOOPS
