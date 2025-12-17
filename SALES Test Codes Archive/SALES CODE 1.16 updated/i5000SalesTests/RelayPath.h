/*
 *-----------------------------------------------------------------------------
 * (c) Copyright 2003, Agilent Technologies, all rights reserved.
 *-----------------------------------------------------------------------------
 */

#ifndef RELAYPATH_H
#define RELAYPATH_H

#pragma warning(disable:4786)

/*#include <vector>
#include <utility>
using namespace std;*/

#include "../../../vth/src/HTalon.h"
#include "../../../vth/src/ResourceToken.h"
#include "../../../vth/src/IMetaTesthead.h"
#include "../i5000Tests/TestUtils.h"
#include "../../../vth/src/vthsup_p.h"
#include "../../../vth/src/vth_pin.h"
#include "../../../dgn/src/execsup_p.h"
#include "../../../vth/src/vth_mod.h"
#include "../../../vth/src/pinasm.h"
/*#include "../../../dgn/src/dgnconfig_p.h"
#include "../../../dgn/src/termio_p.h"
#include "../../../dgn/src/testdec_p.h"
#include "../../../dgn/src/strdec.h"
#include "../../../u/src/hdw_p.h"
#include "../../../u/src/newhfs_p.h"
#include "../../../vth/src/capasm_p.h"
#include "../../../vth/src/macasm_p.h"
#include "../../../vth/src/aimasm_p.h"
#include "../../../vth/src/pinasm_p.h"
#include "../../../vth/src/result_p.h"
#include "../../../vth/src/vth_p.h"
#include "../../../vth/src/standard_p.h"
#include "../../../vth/src/strings_p.h"*/

#define MAX_S_LENGTH			80
const int LOOP = 0;
const int SIDEA = 0;
const int SIDEB = 1;
const int MIN = 0;
const int MAX = 1;


typedef char s_type[MAX_S_LENGTH + 1];
typedef HTalon::RelayName const *const RLYLIST;
typedef char what_string_type[81];

#define LEVEL           0
#define DMMAUTO         99
#define ASCII           0
#define SINT            1
#define DINT            2
#define SREAL           3
#define DREAL           4
#define MOFF            0
#define LIFO            1
#define FIFO            2
#define CONT            3
#define MEAN            0
#define SDEV            1
#define N               2
#define SALES_MIN       3
#define SALES_MAX       4
#define ALL_STATS       5

typedef int det_range_type;
typedef int channel_type;
typedef int sschannel_type;
typedef int ssvirt_card_range;   /* 3 indicates all cards (0,1,2) */
typedef int mux_node_type;
typedef int byte_subrange;
typedef int word_subrange;
typedef int mod_type;
typedef int chan_relays['H' + 1 - 'A'];
typedef int aim_x_relays['H' + 1 - 'A'];
typedef chan_relays mux_relays[9];

typedef union a_word_type
{
	unsigned int bits;
	word_subrange data;
} a_word_type;

typedef union a_byte_type
{
	unsigned char bits;
	byte_subrange data;
} a_byte_type;

#define RISING          0
#define FALLING         1
#define RISING_EDGE		1

#define PRE             0
#define POST            1

typedef char path_type[50];
typedef char cmd_type[5];
typedef double realar_type[5];

#define NUM_DET_POINTS_TO_CHECK  4
#define NUM_SLOTS       22
#define MAX_SAMPLES     8192
#define MAX_STD_COMPS   50
#define MAC_SLOT        6  /* Module slot that contains the control card is defined as mac_slot */

#define MUX_NODE_PIN    7
#define NO_RELAYS 0, 0

/*----------------------------------------------------------------------------
* Sales rack constants and measured variables
*-----------------------------------------------------------------------------
*/
#define DMM50	"GPIB0::20::INSTR"	//DMM50
#define DMM		"GPIB0::10::INSTR"	//DMM
#define PS		"GPIB0::8::INSTR"	//Power Supply//sri- 28th Feb 2007-changed to 8 from 5.
#define CNTR	"GPIB0::6::INSTR"	//Counter
#define SYNTH	"GPIB0::17::INSTR"	//Synthesizer-Function Generator
#define LCR		"GPIB0::18::INSTR"	//LCR Meter
#define MUX		"GPIB0::9::INSTR"	//Minerva
#define GEN		"GPIB0::12::INSTR"	//Pulse Generator
#define OLYM	"GPIB0::11::INSTR"	//N6700B System Power Supply
#define PS_LOAD	"GPIB0::5::INSTR"	//6038A Load Power Supply
#define HPIB_TO 5000 				//Time in milliseconds
#define RSM     "ResourceManager"
#define FIXTURE_ENABLED		5
#define FIXTURE_LOCK_TIME	2.0e+00 //1.0e+00//sri xw4300 compliance
#define ANA_ID_MIN			3776
#define ANA_ID_MAX			3779
#define DIG_ID_MIN          3779
#define DIG_ID_MAX          3795
#define RX_PATH_OFFSET		6.2067E-9
#define SSRECF				1.41E-9    
#define SSFDCF				1.00e-9      


class RACK_DELAYS
{
public:
	void ReadDelays();
	double AF2System(int);
	double AF2Start(int);
	double AF2Clkrb(int);
	double ClkrbDelay(int);
	double ProbeDelay(int);
	
private:
	double clkrb_delay[4];
	double af2_system_delay[4];
	double af2_start_delay[4];
	double af2_clkrb_delay[4];
	double probe_clkrb_delay[4];
};

const double SLOT_DELAY[11][2] =
{ //Falling     Rising	     		
	0.0000E-9,	0.0000E-9, 	//  SLOT  1 
	8.1000E-9,  7.5600E-9,	//  SLOT  2 
	8.2938E-9,	7.6937E-9,	//  SLOT  3 
	8.2863E-9,	7.7712E-9,	//  SLOT  4 
	8.3450E-9,	7.7850E-9,	//  SLOT  5 
	0.0000E-9,	0.0000E-9, 	//  SLOT  6 
	8.6013E-9,	7.9625E-9,	//  SLOT  7 
	8.7575E-9,	8.1100E-9,	//  SLOT  8 
	8.7575E-9,	8.1837E-9,	//  SLOT  9 
	9.1413E-9,	8.4025E-9,	//  SLOT 10 
	9.1213E-9,	8.4512E-9	//  SLOT 11 
};

struct OFFSET_CAP
{
	double sales_rack_offset_xg_cap[2];
	double sales_rack_offset_cap[2][8];
};


/*----------------------------------------------------------------------------
* HPIB command values are listed below:
*-----------------------------------------------------------------------------
*/
#define UNLISTEN        63        /* GPIB unlisten command  */
#define TALK            64        /* add-on to talker's bus address */
#define UNTALK          95
#define LISTEN          32        /* add-on to listener's bus address */
#define CRLF            "\015\n"  /* ASCII carriage-return + line-feed */

/*----------------------------------------------------------------------------
* DMM 
*-----------------------------------------------------------------------------
*/
#define FILTER_20K      4
#define FILTER_128      5
#define FILTER_1024     6
#define FILTER_8192     7
#define FREQA           0
#define FREQB           1
#define DMM_DC          1
#define DMM_AC          2
#define FOUR_WIRE_OHMS  0
#define TWO_WIRE_OHMS   1 
#define DCV				2
#define ACV				3 
#define DMMFREQ			4
#define PER				5
#define FIXEDZ			6
#define DMM50_DIGITAL_USAGE   0
#define DMM50_ANALOG_USAGE    1
#define DMM_ADDRESS           5 /* GPIB address of dmm (no 50 ohm termination) */
#define DMM50_ADDRESS         8 /* GPIB address of dmm50 (with 50ohm termination) */
#define DMM_3458			 10 /* bus address of dmm */
#define DMM_VALID_RESULT 100.0

/*----------------------------------------------------------------------------
* Counter 
*-----------------------------------------------------------------------------
*/
#define AC				0
#define CNTR_5370       6  /* bus address of counter */

#define RISE_TIME		0
#define FALL_TIME       1
#define WIDTH			2
#define VMAX			3
#define VMIN			4
#define DELTA_V			5
#define DELTA_T			6
#define DUTY_CYCLE		7
#define FREQ			8
#define VAVERAGE		9

#define INDUCTANCE		0
#define CAPACITANCE		1

#define TIME_INTERVAL	0
#define TRG_LEVEL		1
#define FREQUENCY		2
#define PERIOD			3
#define PWID			4
#define NWID			5
#define DCYCLE			6
#define V_MAX			7
#define	V_MIN			8

/*----------------------------------------------------------------------------
* As of A.05.00, we are using different values for t0, the AutoCal tclk
* advance time, based on whether we are using a mac10 or mac11 control card.
*-----------------------------------------------------------------------------
*/
#define MAC_11_T0_ACTUAL  162.9e-09  // it was found during Talon testing that this value has
									 // shifted 30nS from control+ to XT/XTP cards

#define RELAY_OPEN_WAIT_TIME	350e-06//200e-06//100e-06, sri xw4300 Talon testing compliance, 1/11/06
#define WAIT_TIME_FOR_RELAYS	3.0e-03//1.0e-03
#define SOURCE_WAIT_TIME		1.0e-03
#define TIME_OUT				10.0e-03
#define STANDARD_WAIT_TIME		75e-03 //50e-03, , sri xw4300 Talon testing compliance, 1/11/06
#define WAIT_TIME				950e-03//750e-03 //500e-03, sri xw4300 Talon testing compliance, 1/11/06
#define	MEASUREMENT_WAIT		1.0e+00
#define SALES_RACK_WAIT			5.0e+00
#define WAIT_FOR_DMM_CAPTURE	20.0e+00
#define N6700B_CH1_WAIT_TIME     150e-03 //For n6700b channel1 voltage stabilization - chuanho 29/05/2015
#define N6700B_CH1_FINEST_INCREMENT  5e-03 //For n6700b channel1 finest step voltage - chuanho 29/05/2015
#define N6700B_CH1_STARTING_INCREMENT 1e+00  //For n6700b channel1 initial voltage - chuanho 29/05/2015

#define LONGPI					3.14159265358979323846e+000

#define R_MAX_PARALLEL			1e+13

#define READ_WRITE        2
#define IRQ_BIT_POSITION  1  /* 0-7; 0 msb */
#define DET_OR_BIT       12  /* 0-15; 0 = msb */

typedef int polarity;
typedef int slot_type;

typedef double one_longreal_per_channel['H' + 1 - 'A'];
typedef double std_component_array[MAX_STD_COMPS];
typedef double one_longreal_per_ss_channel[24];
typedef double two_longreals_per_slot[NUM_SLOTS][2];
typedef double one_longreal_per_mod[4];
typedef double mod_slot_channel_table[4][NUM_SLOTS]['H' + 1 - 'A'];
typedef double arrform[MAX_SAMPLES];
typedef char temp_array[MAX_SAMPLES * 4];

typedef struct database_module
{
	double mean;
	double std;
	double sum_x;
	double sum_xx;
	int n;
	char comment[81];
} database_module;

struct TALON_5V
{
	double Vunloaded[4][36][2];
};

class TALON_LOADS
{
public:
	void store_measurements(int mod_id, int slot);
	double res[4][36][6];
	double r6_5a[4][36];
	double r6_5b[4][36];
	double r50a[4][36];
	double r50b[4][36];
	double r1ka[4][36];
	double r1kb[4][36];
};

typedef database_module database_system[4];
typedef database_module database_channel['H' + 1 - 'A'];
typedef database_module database_slot[NUM_SLOTS][2];

extern const aim_x_relays AIM_X_PLUS_1_MINT_RLYS;
extern const aim_x_relays AIM_X_MINT_RLYS;
extern const aim_x_relays AIM_X_S_RLYS;
extern const aim_x_relays AIM_X_I_RLYS;
extern const aim_x_relays AIM_X_L_RLYS;
extern const aim_x_relays AIM_X_A_RLYS;
extern const aim_x_relays AIM_X_B_RLYS;
extern const chan_relays PIN_KR_RLYS;
extern const chan_relays PIN_KD_RLYS;
extern const chan_relays PIN_KX_RLYS;
extern const chan_relays PIN_KDC_RLYS;
extern const chan_relays PIN_KGL_RLYS;
extern const mux_relays PIN_DRVR_RLYS;
extern const mux_relays PIN_RCVR_RLYS;
extern mod_slot_channel_table load_5_ohm_resistance;
extern mod_slot_channel_table load_5_ohm_ratio;
extern mod_slot_channel_table no_load_5_ohm_ratio;
extern mod_slot_channel_table load_24_ohm_resistance;
extern mod_slot_channel_table load_24_50_ohm_resistance;
extern mod_slot_channel_table load_24_ohm_ratio;

extern int dbg_int;
extern double t16820_delay;
extern double t16820_res;
extern double t16820_af2;
extern double t16871_delay;
extern double t16871_af2;
extern double t16891_probe;

//extern char start_date[40];  // start date and time of sales 
//extern time_t start_time;    // start time (seconds) of sales 
//extern int reset9000;        // counter for the number of times we have reset 
// the sales rack following an exception with a test 

typedef double voltage_array[NUM_DET_POINTS_TO_CHECK];

#define NORM          0
#define TRIGGERED     1
#define GATED         2

#define OFF           0
#define NEG           1
#define POS           2
#define BOTH          3

#define DC            0
#define SINE          1
#define SQUARE        2
#define TRIANGLE      3
#define POS_RAMP	  4
#define NEG_RAMP	  5 


#define Q             0
#define D             1

#define SERIES        0
#define PARALLEL      1

#define AUTOMATIC     0

/*----------------------------------------------------------------------------
 * Function declarations
 *----------------------------------------------------------------------------
*/
void flush_commands(int mod_id);
void display_error(int mod_id, char * specific);
void show_present_test_info(int present_test_num, int mod_id, int present_slot_num, char * s);
void sales_reset_sequencer(int mod_id, int slot);
void sales_reset_sequencer_aim(int mod_id);
void sales_start_sequencer(int mod_id, int slot);
void sales_start_sequencer_mac(int mod_id);
void convert_seconds_nanoseconds(double * value);
bool fxt_locked(int mod_id);
double nanotrunc(double value);
double edge_placement_calc(int mod_id, int slot, int slope, double offset, double drvr_delay_actual);
void close_pin_drvr_load_rlys(int mod_id, int slot, int driver);
void close_pin_gnd_relays(int mod_id, int slot);
void measure_47_ohm_aim_delta(int mod_id, double * resistance);
void break_test_for_function_step_compatability(int mod_id);
void adet_manual_meas(int mod_id);
void read_database_line(FILE * f, database_module * data);
void write_database_line(FILE * f, database_module * data);
void recalculate_database_line(double d, database_module * data);
void load_fxt_specific_actual_values(int mod_id, int number_of_components, double * actual_value);
void set_up_digitizer(int mod_id, int input_mode, int range, int n, double period);
void stats(double * mean, double * noise, double sum, double sum_of_squares, int number, double minimum, double maximum, int multiplier);
double convert_logged_string_to_numeric(int mod_id);
bool counter_measurement_pending(int mod_id);
int slave_links_to_master(int master_mod_id, int slave_mod_id);
int any_module_present(void);
int exclusive_or(int a, int b);
double get_cal_resistor_value(int mod_id, int feedback_resistor_no, int meas_type);
bool rack_and_fixture_compatible(char * rack_serial_number, int fixture_serial_number);
int get_fixture_id(int mod_id);
bool four_mod_fixture(int mod_id);
double smaller(double first_value, double second_value);
double larger(double first_value, double second_value);
double standard_deviation(int n, double sum, double sum_of_squares);
void fft(double * xr, double * xi, int npts);
void copy_(double * x, double * y, int npts);
void fill(double * x, double cnst, int npts);
void mag(double * xr, double * xi, double * y, int npts);
void scalar_mult(double * x, double cnst, int npts);
void display_it(int mod_id, char * s, int i, double r);
void relay_map(int mod_id, int slot, int relay_page);
void virt_card_select(int mod_id, int slot, int virt_card);
double offset_calibration(int mod_id, double calibration_voltage);
void setup_inst_defaults(int mod_id);
void setup_dcv_dig(int mod_id, int range, double interval, int num_samples, double aperature, int level, int mode);
void setup_dcv50_dig(int mod_id, int range, double interval, int num_samples, double aperature, int level, int mode);
void setup_ssdc_dig(int mod_id, int range, double interval, int num_samples);
void setup_ssdc_fast(int mod_id, int range, int level, int slope, double interval, double sample_delay, int num_samples);
void setup_ssdc50_dig(int mod_id, int range, double interval, int num_samples);
void setup_ssdc50_fast(int mod_id, int range, int level, int slope, double interval, double sample_delay, int num_samples);
void clear_dmm50_memory(int mod_id);
void dmm_display(int mod_id, int dmm_addr, bool mode);
void capture_ddm(int mod_id, int num_samples, double * waveform, int dmm_addr);
void setup_dsdc_dig(int mod_id, int range, double interval, int num_samples);
void xmhpib_wr(int mod_id, int tag, char * s);
double measure_td(int mod_id, double t_nominal, int trig_source, int trig_edge, int other_direction, int other_edge, double timebase_sensitivity);
void connect(int mod_id, int connection, int status);
void connect_cntr_stop_db1(int mod_id);
void connect_cntr_stop_uclk(int mod_id);
void connect_dg(int mod_id, int status);
void connect_dmm(int mod_id);
void connect_dmm_pin(int mod_id, int slot);
void connect_gen_trg_cntr_start(int mod_id);
void connect_gen_trg_clkrb(int mod_id);
void connect_gen_out_cntr_stop(int mod_id);
void connect_gen_out_af2(int mod_id);
void connect_gen_out_probe(int mod_id);
void connect_lcr(int mod_id);
void connect_minus_pin(int mod_id, int slot, int channel, int status);
void connect_minus_calib2(int mod_id);
void connect_minus_swd_gnd(int mod_id);
void dconnect_minus_swd_gnd(int mod_id);
void connect_minus_xn_plus_1(int mod_id, int channel, int status);
void connect_dmm50_af2(int mod_id);
void aconnect_srcout_AF2(int mod_id);
void aconnect_srcout_calib1(int mod_id);
void aconnect_auxout_synth(int mod_id);
void aconnect_auxsrc_fxt(int mod_id);
void aconnect_cntr_stop_af2(int mod_id);
void connect_cntr_ch1_uclk(int mod_id);
void connect_plus_calib1(int mod_id);
void connect_plus_gout(int mod_id);
void connect_plus_xn(int mod_id, int channel, int status);
void connect_4wire_plus_xn(int mod_id, int channel, int status);
void connect_power_supply_xmint(int mod_id);
void connect_syn_aux_gen_ext(int mod_id);
void connect_syn_out_gen_ext(int mod_id);
void dconnect_syn_out_dmm(int mod_id);
void aconnect_syn_out_dmm(int mod_id, int status);
void connect_syn_out_af2(int mod_id);
void aconnect_syn_out_xmint(int mod_id);
void aconnect_vs_dmm(int mod_id, int status);
void aconnect_vo_dmm(int mod_id, int status);
double setup_moa_resistance_meas(int mod_id, int rfb, int mode);
void connect_dmm_probe(int mod_id);
void aconnect_ps_20_ohm_load(int mod_id, int status);
void connect_1g_ohm_resistor(int mod_id, int status);
void connect_500_ohm_load(int mod_id);
void connect_10k_load(int mod_id);
void disconnect_10k_load(int mod_id);
void disconnect_all(int mod_id);
void get_mux_serial_number(int mod_id);
void store_mux_serial_number(int mod_id, char * serial_number);
void close_mux(int mod_id);
void open_mux(int mod_id);
void open_com(int mod_id);
void close_com(int mod_id);
char * get_sales_rack_serial_number(char * result, int mod_id);
void setup_gen_defaults(int mod_id);
void open_gen(int mod_id);
void disable_gen_out(int mod_id);
void enable_gen_out(int mod_id);
void complement_gen_out(int mod_id);
void arm_gen(int mod_id);
int read_gen_err(int mod_id);
void no_complement_gen_out(int mod_id);
void setup_gen_mode(int mod_id, int mode, int slope);
void setup_gen_levels(int mod_id, double low_level, double high_level);
void setup_gen_pwidth(int mod_id, double pulse_width);
void setup_gen_period(int mod_id, double gen_period);
void setup_gen_fixed_transition(int mod_id);
void setup_gen_transition_times(int mod_id, double tt);
void setup_gen_trg_out_delay(int mod_id, double delay, bool gen_cal_counter);
void close_gen(int mod_id);
//-----------------For Old Synthesizer 3325B--------------------//
void setup_syn(int mod_id, int wave, int frequency, double amplitude, double offset);
void setup_syn_hv(int mod_id, int status);
void setup_syn_defaults(int mod_id);
void open_syn(int mod_id);
void setup_syn_wave(int mod_id, int wave);
void setup_syn_freq(int mod_id, int frequency);
void setup_syn_amp(int mod_id, double amplitude);
void setup_syn_off(int mod_id, double offset);
void close_syn(int mod_id);
boolean check_syn_model(void);
//-----------------For Old Synthesizer 3325B--------------------//

//-----------------   For New FG 33250A     --------------------//
//Added by chuanho, 14jun2012
void setup_33250A(int mod_id, int wave, int frequency, double amplitude, double offset);
void setup_33250A_hv(int mod_id, int status);
void setup_33250A_defaults(int mod_id);
void open_33250A(int mod_id);
void setup_33250A_wave(int mod_id, int wave);
void setup_33250A_freq(int mod_id, int frequency);
void setup_33250A_amp(int mod_id, double amplitude);
void setup_33250A_off(int mod_id, double offset);
void disable_33250A_out(int mod_id);
void enable_33250A_out(int mod_id); //added by chuanho, 13jul2011
void close_33250A(int mod_id);
//-----------------For New FG 33250A--------------------//

void open_dmm(int mod_id);
void open_dmm50(int mod_id);
void setup_dmm(int mod_id, int dmm_function, double range, double resolution);
void setup_dmm_norm(int mod_id);
void setup_dmm50(int mod_id, int dmm_function, double range, double resolution);
void setup_dmm_ac_band(int mod_id, int start_freq, int stop_freq);
void setup_dmm50_ac_band(int mod_id, int start_freq, int stop_freq);
void setup_dmm_aperature(int mod_id, double aperature);
void setup_dmm50_aperature(int mod_id, double aperature);
void setup_dmm_fixed_z(int mod_id, int on);
void setup_dmm50_fixed_z(int mod_id, int on);
void setup_dmm_periodic(int mod_id, double range, double resolution, int start_freq, int stop_freq, int samples);
void setup_dmm_fsource(int mod_id, int fsource);
void setup_dmm50_fsource(int mod_id, int fsource);
void setup_dmm_level(int mod_id, int trigger_percent, int coupling);
void setup_dmm50_level(int mod_id, int trigger_percent, int coupling);
void setup_dmm_level_filter(int mod_id, int filter_control);
void setup_dmm50_level_filter(int mod_id, int filter_control);
void setup_dmm_nplc(int mod_id, int nplc);
void setup_dmm50_nplc(int mod_id, int nplc);
void setup_dmm_ocomp_on(int mod_id);
void setup_dmm50_ocomp_on(int mod_id);
void autocal_dmm(int mod_id);
void autocal_dmm50(int mod_id);
boolean is_dmm50_rear_input(int mod_id);
int check_dmm_measured(int mod_id);
double measure_dmm(int mod_id);
double measure_dmm50(int mod_id);
void close_dmm(int mod_id);
void close_dmm50(int mod_id);
void open_lcr(int mod_id);
void setup_lcr(int mod_id, int lcr_function, int lcr_display, int lcr_ckt_mode, double lcr_freq, int lcr_mult);
void trigger_lcr(int mod_id);
double measure_lcr(int mod_id);
void close_lcr(int mod_id);
void setup_cntr_defaults(int mod_id);
void open_cntr(int mod_id);
void setup_cntr_out2(int mod_id);
void setup_cntr_trg_levels(int mod_id, double start_level, double stop_level);
void setup_cntr_trigger_slopes(int mod_id, int start_slope, int stop_slope);
void setup_cntr_function(int mod_id, int cntr_function);
void setup_cntr_volt_meas(int mod_id, int cntr_function, int count);
void setup_cntr_gate_time(int mod_id, double gate_time);
void setup_cntr_sample_size(int mod_id, int samples);
void setup_cntr_trigger_min_max(int mod_id, int level);
void setup_cntr_inp2_lowpass(int mod_id);
void setup_cntr_md4(int mod_id);
void setup_cntr_md1(int mod_id);
void setup_cntr_max_volt(int mod_id);
void setup_cntr_min_volt(int mod_id);
void setup_cntr_input(int mod_id, int coupling, int impedance);
void setup_cntr_edge(int mod_id, int edge);
void set_cntr_1meg(int mod_id);
void set_cntr_stop_1meg(int mod_id);
void setup_cntr_inputz(int mod_id, int impedance);
void setup_cntr_measurement(int mod_id, char measurement);
void setup_cntr_duty(int mod_id);
void trigger_cntr(int mod_id);
void trigger_md4(int mod_id, int cntr_function);
void wait_for_counter_measurement(int mod_id);
void close_cntr(int mod_id);
double read_cntr_max(int mod_id);
double read_cntr_min(int mod_id);
double read_cntr_sigma(int mod_id);
double read_cntr_mean(int mod_id);
double read_cntr(int mod_id);
void open_ps(int mod_id);
void open_ps_load();
void close_ps_load();
void setup_ps(int mod_id, double volts, double amps);
void wait_for_ps_ramp(int mod_id, double diff, double ramp_time, double * dmm_actual_voltage);
void disable_ps_out(int mod_id);
void enable_ps_out(int mod_id);
void close_ps(int mod_id);
double measure_ps_sense_voltage(int mod_id);

//added by sri on 28th August 2007 for Load Test- Dragon Heart//
void open_ps_load(int mod_id);
float setup_ps_load(int mod_id, double volts, double amps);
void wait_for_ps_ramp_load(int mod_id, double diff, double ramp_time, double * dmm_actual_voltage);
void disable_ps_out_load(int mod_id);
void enable_ps_out_load(int mod_id);
void close_ps_load(int mod_id);
double measure_ps_sense_voltage_load(int mod_id);
double measure_ps_load_current(int mod_id);
void connect_load_test_relay(int mod_id, int status);//added by sri tharan for DH-Utility load testing
void verify_ps_output_load(int mod_id, double diff, double ramp_time, double expected_volts);
//ended here ! sri//

/*-------------------------------N6700B channel 1--------------------------*/
//Added by chuanho, 28Jun2011

void OpenResourceManager();
void CloseResourceManager();
void open_n6700b(int mod_id);
void verify_n6700b(int mod_id);
void setup_n6700b_ch1_volt(int mod_id, double amplitude);
void setup_n6700b_ch1_curr(int mod_id, double current);
void reverse_n6700b_ch1_out(int mod_id);
void enable_n6700b_ch1_out(int mod_id);
void disable_n6700b_ch1_out(int mod_id);
void close_n6700b(int mod_id);
void connect_n6700b_ch1(int mod_id);
void aconnect_n6700b_ch1_abus1_xmint(int mod_id);
void aconnect_n6700b_ch1_out_dmm(int mod_id, int status);
void dconnect_n6700b_ch1_af2(int mod_id); //added by chuanho, 21jul2011

/*--------------------------Enf of N6700B channel 1------------------------*/

void open_dut_ps(int mod_id);
void setup_dut_ps(int mod_id, int ps_channel, double volts, double ov, double amps);
void close_dut_ps(int mod_id);
double read_dmm_stats(int mod_id, int dmm_stat);
double read_dmm50_stats(int mod_id, int dmm_stat);
void setup_dmm_readings(int mod_id, int n);
void setup_dmm50_readings(int mod_id, int n);
void calculate_dmm_stats(int mod_id);
void calculate_dmm50_stats(int mod_id);
void trigger_dmm(int mod_id);
void trigger_dmm50(int mod_id);
void wait_for_dmm_measurement(int mod_id, int dmm_address);
void arm_dmm50(int mod_id);
void arm_dmm(int mod_id);
void hold_dmm(int mod_id);
void hold_dmm50(int mod_id);
void enable_dmm_buffer(int mod_id);
void enable_dmm50_buffer(int mod_id);
void disable_dmm_buffer(int mod_id);
void disable_dmm50_buffer(int mod_id);
void setup_dmm_memory(int mod_id, int fmat, int mem);
void setup_dmm50_memory(int mod_id, int fmat, int mem);
double read_dmm(int mod_id);
double read_dmm50(int mod_id);
void verify_ps_output(int mod_id, double diff, double ramp_time, double expected_volts);
bool det_over_range(int mod_id);
double measure_open(int mod_id, double vplus, double vminus, double rfb);
double measure_open_non(int mod_id, double vplus, double vminus, double rfb);
void check_uprange(int mod_id, double * delta);
double input_real(char * prompt);
det_range_type sel_det_range(double dcv_input);
void test_detector_range(int mod_id, double range_voltage, int range_number, int divisor);
void test_det_atten_range(int mod_id, double range_voltage, int num_points, int range);
void buzz_prelays(int mod_id, int slot);
void waveform_to_magnitude(double * x, double * y, int npts);
int find_dc_index(int n, double * y);
int find_peak_index(int imin, int imax, double * y);
void find_local_minima(int imin, int imax, int ipeak, double * y, double db_down, int * ilo, int * ihi);
double sum_power(int imin, int imax, double * y);
int find_local_max(int imin, int imax, int iexp, double * y);
double find_harmonic_power(int ipeak, int imax, double * y, double db_down);
void sales_show_state(int mod_id, char * toggle_label, int conditional);
void show_value(int mod_id, char *label, double value);
double calc_mod_loading(int mod_num, double sd_factor);
double get_gen_cal_delay(int mod_id);
void log_array_data(int test, int subtest, int mod_id, int slot, double * ap, int index, double data_to_log);
void setup_dmm_digitizing(int mod_id, int dmm_addr, int range, double interval, int num_samples, double aperature, double resolution, int trig_level, int slope, int mode);
void setup_dmm50_norm(int mod_id);
double get_dmm_volt(int mod_id, int dmm_addr, double *waveform, int num_samples, int mode);
void setup_dmm_subsampling_AC_and_DC(int mod_id, int dmm_addr, int num_samples, double sampling_rate, int trig_level, int slope, int range);
void connect_gen_out_cntr_ch1(int mod_id);
void connect_cntr_ch1_af2(int mod_id);
void connect_cntr_ch2_af2(int mod_id);
void aconnect_cntr_ch1_af2(int mod_id);
void connect_dmm50_out_af2(int mod_id);
void check_dmm50_button(int mod_id, int usage);

//Talon specific 
void talon_buzz_that_relay(int, HTalon::RelayName const , HTalon * , bool = false);
void connect_4wire_talon(int mod_id, int status, int p);
void connect_minus_xn_plus_1_talon(int mod_id, int status);
void connect_plus_xn_talon(int mod_id, int status, int p);
void sglset_close(int const &mod_id, int const &slot, HTalon::Relays const &ref, HTalon *talonCard);
void close(int const &mod_id, int const &slot, HTalon::Relays const &rut, HTalon::Relays const &ref, HTalon *talonCard);
void CloseDigitalFixtureGround(int const &mod_id, int const &slot, HTalon *talonCard);
void ConnectAF2toADC(int const &mod_id, int const &slot, int side, HTalon *talonCard);
void ConnectLoadtoAF2Shunt(int const &mod_id, int const &slot, TestUtils::ChannelName chnl, HTalon *talonCard, int load);
void ConnectLoadtoAF2Series(int const &mod_id, int const &slot, TestUtils::ChannelName chnl, HTalon *talonCard, int load);
void ConnectDrivertoAF2(int mod_id, int slot, TestUtils::ChannelName chnl, HTalon *talonCard);
void Connect5VtoSBus(int const &mod_id, int const &slot, TestUtils::ChannelName chnl, HTalon *talonCard);
void ConnectDigitalFixture(int const &mod_id, int const &slot, PINDefinition::PinEnum pin, IMetaTesthead *th, IPin *pinCard);
void debug_relay_state(int mod_id, int slot);
double IsolateOVPoint(int const &mod_id, int const &slot, bool high_bounds_ov, bool low_bounds_ov);
void Start_sequencer(int const &mod_id, int const &slot);
void Reset_sequencer(int const &mod_id, int const &slot);
void StaticDriveSetup(int const &mod_id, int const &slot, PINDefinition::PinEnum pin, IMetaTesthead *th, IPin *pinCard, int p, int dvh, int dvl);		
void DisableStaticDriveOut(int const &mod_id, int const &slot, PINDefinition::PinEnum pin, IMetaTesthead *th, IPin *pinCard);
double MeasureGenTrigOutDelay(int const &mod_id, int const &slot, double gen_delay);
int TrigCntr(int mod_id);
void OpenResourceManager();
void CloseResourceManager();
void verify_syn(int mod_id);
void verify_mux(int mod_id);
void verify_gen(int mod_id);
void verify_lcr(int mod_id);
void verify_counter(int mod_id);
void verify_ps(int mod_id);
void verify_dmm(int mod_id);
void verify_dmm50(int mod_id);
void err_handler();
void send_msg(char *command, int instrument);
void convert_byte_buffer_as_words(int mod_id);
double measure_current();


#endif
