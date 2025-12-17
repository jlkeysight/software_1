//

#ifndef MTGLOBAL_P_H

#define MTGLOBAL_P_H

#pragma warning (disable:4786)



#include "../../../dgn/src/dgnconfig_p.h"

#include "../../../dgn/src/termio_p.h"

#include "../../../dgn/src/testdec_p.h"

#include "../../../dgn/src/execsup_p.h"

#include "../../../dgn/src/strdec.h"

#include "../../../u/src/hdw_p.h"

#include "../../../u/src/newhfs_p.h"

#include "../../../vth/src/capasm_p.h"

#include "../../../vth/src/macasm_p.h"

#include "../../../vth/src/aimasm_p.h"

#include "../../../vth/src/pinasm_p.h"

#include "../../../vth/src/result_p.h"

#include "../../../vth/src/vth_p.h"

#include "../../../vth/src/vthsup_p.h"

#include "../../../vth/src/standard_p.h"

#include "../../../vth/src/strings_p.h"



typedef char what_string_type[81];



#define LEVEL           0

#define AUTO            1



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



#define PRE             0

#define POST            1



typedef int osc_chan_type;

typedef char path_type[50];

typedef char cmd_type[5];

typedef double realar_type[5];



#define NUM_DET_POINTS_TO_CHECK  4

#define NUM_SLOTS       22

#define MAX_S_LENGTH    80

#define MAX_SAMPLES     8192

#define MAX_STD_COMPS   50

#define MAC_SLOT        6  /* Module slot that contains the control card is defined as mac_slot */


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
#define HPIB_TO 5000 				//Time in milliseconds
#define	MEASUREMENT_WAIT	1.0e+00
/*


/*

 *-----------------------------------------------------------------------------

 * GPIB command values are listed below:

 *-----------------------------------------------------------------------------

 */

#define UNLISTEN        63        /* GPIB unlisten command  */

#define TALK            64        /* add-on to talker's bus address */

#define UNTALK          95

#define LISTEN          32        /* add-on to listener's bus address */

#define CRLF            "\015\n"  /* ASCII carriage-return + line-feed */



/*

 *-----------------------------------------------------------------------------

 * As of A.05.00, we are using different values for t0, the AutoCal tclk

 * advance time, based on whether we are using a mac10 or mac11 control card.

 *-----------------------------------------------------------------------------

 */

#define MAC_10_T0_ACTUAL  165.9e-09

#define MAC_11_T0_ACTUAL  162.9e-09



#define WAIT_TIME_FOR_RELAYS  1.0e-03

#define WAIT_TIME_FOR_RELAYS_OPEN  3.0e-03

#define STANDARD_WAIT_TIME  50e-03

#define RELAY_OPEN_WAIT_TIME  100e-06

#define R_MAX_PARALLEL  1e+13



#define LONGPI          3.14159265358979323846e+000

#define LONGTWO_PI      (2.0 * LONGPI)



#define FILTER_20K      4

#define FILTER_128      5

#define FILTER_1024     6

#define FILTER_8192     7

#define FREQA           0

#define FREQB           1

#define DMM_DC          1

#define DMM_AC          2





#define CNTR_5370         6  /* bus address of counter */

#define OSC_54111         7  /* bus address of oscilloscope */

#define DMM_3458         10  /* bus address of dmm */

#define READ_WRITE        2

#define IRQ_BIT_POSITION  1  /* 0-7; 0 msb */

#define DET_OR_BIT       12  /* 0-15; 0 = msb */



#define MEASURING       "measuring"

#define WAITING         "waiting"



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





typedef double voltage_array[NUM_DET_POINTS_TO_CHECK];



#define NORM            0

#define TRIGGERED       1

#define GATED           2



#define OFF             0

#define NEG             1

#define POS             2

#define BOTH            3



#define DC              0

#define SINE            1

#define SQUARE          2

#define TRIANGLE        3

#define RAMP			4



#define DCV             0

#define ACV             1

#define FOUR_WIRE_OHMS  2

#define TWO_WIRE_OHMS   3

#define FIXED_Z         4

#define FR              5

#define PER             6



/*

 *-----------------------------------------------------------------------------

 * mjs added for scope elimination project

 *-----------------------------------------------------------------------------

 */

#define DMM50_DIGITAL_USAGE   0

#define DMM50_ANALOG_USAGE    1

#define DMM_ADDRESS           5 /* GPIB address of dmm (no 50 ohm termination) */

#define DMM50_ADDRESS         8 /* GPIB address of dmm50 (with 50ohm termination) */



#define RISE_TIME       0

#define FALL_TIME       1

#define WIDTH           2

#define VMAX            3

#define VMIN            4

#define DELTA_V         5

#define DELTA_T         6

#define DUTY_CYCLE      7

#define FREQ            8

#define VAVERAGE        9



#define INDUCTANCE      0

#define CAPACITANCE     1



#define SALES_RACK_WAIT 5.0e+00

#define TIME_INTERVAL   0

#define TRG_LEVEL       1

#define FREQUENCY       2

#define PERIOD          3

#define PWID            4

#define NWID            5

#define DCYCLE          6



#define Q               0

#define D               1



#define SERIES          0

#define PARALLEL        1



#define AUTOMATIC       0



typedef char s_type[MAX_S_LENGTH + 1];



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



typedef union a_temp_type

{

  struct

  {

    char c1data;

    char c2data;

    char c3data;

    char c4data;

  } U1;

  double rdata;

} a_temp_type;



typedef a_temp_type cv_array[MAX_SAMPLES];



typedef struct database_module

{

  double mean;

  double std;

  double sum_x;

  double sum_xx;

  int n;

  char comment[81];

} database_module;



typedef database_module database_system[4];

typedef database_module database_channel['H' + 1 - 'A'];

typedef database_module database_slot[NUM_SLOTS][2];



#ifdef MTGLOBAL_P_G

  #define VEXTERN

#else

  #define VEXTERN extern

#endif



 VEXTERN double                 rx_path_offset;

 VEXTERN mod_slot_channel_table load_5_ohm_resistance;

 VEXTERN mod_slot_channel_table load_5_ohm_ratio;

 VEXTERN mod_slot_channel_table no_load_5_ohm_ratio;

 VEXTERN mod_slot_channel_table load_24_ohm_resistance;

 VEXTERN mod_slot_channel_table load_24_50_ohm_resistance;

 VEXTERN mod_slot_channel_table load_24_ohm_ratio;

 VEXTERN one_longreal_per_channel channel_delay;



 VEXTERN int dbg_int;

 VEXTERN double t9120_delay;

 VEXTERN double t9120_res;

 VEXTERN double t9120_af2;

 VEXTERN double t9171_delay;

 VEXTERN double t9171_af2;

 VEXTERN double t9191_probe;



 VEXTERN char start_date[40];  /* start date and time of sales */

 VEXTERN time_t start_time;    /* start time (seconds) of sales */

 VEXTERN int reset9000;        /* counter for the number of times we have reset */

                               /* the sales rack following an exception with a test */



 VEXTERN double                 ssfdcf;  /* falling data correction factor t9671 */

 VEXTERN double                 ssrecf;  /* rising edge correction factor t9620 */

 VEXTERN double                 ss_slot_const[NUM_SLOTS];

 VEXTERN double                 ssrec_const[24];

 VEXTERN double                 ssdrvr_const[24];

 VEXTERN two_longreals_per_slot slot_delay;

 VEXTERN one_longreal_per_mod   clkrb_delay;

 VEXTERN one_longreal_per_mod   af2_system_delay;

 VEXTERN one_longreal_per_mod   af2_start_delay;

 VEXTERN one_longreal_per_mod   af2_clkrb_delay;

 VEXTERN one_longreal_per_mod   probe_clkrb_delay;

 VEXTERN double                 sales_rack_offset_cap[2]['H' + 1 - 'A'];

 VEXTERN double                 sales_rack_offset_xg_cap[2];

 VEXTERN double                 chan1_probe_atten_delay_osc;

 VEXTERN double                 chan2_probe_atten_delay_osc;

 VEXTERN boolean                error;



#undef VEXTERN



void flush_commands(int mod_id);

void display_error(int mod_id, char * specific);

void show_present_test_info(int present_test_num, int mod_id, int present_slot_num, char * s);

void sales_reset_sequencer(int mod_id, int slot);

void sales_reset_sequencer_aim(int mod_id);

void sales_start_sequencer(int mod_id, int slot);

void sales_start_sequencer_mac(int mod_id);

void convert_seconds_nanoseconds(double * value);

double nanotrunc(double value);

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

boolean counter_measurement_pending(int mod_id);

int secondary_links_to_primary(int primary_mod_id, int secondary_mod_id);

int any_module_present(void);

int exclusive_or(int a, int b);

double get_cal_resistor_value(int mod_id, int feedback_resistor_no, int meas_type);

boolean rack_and_fixture_compatible(char * rack_serial_number, int fixture_serial_number);

int get_fixture_id(int mod_id);

boolean four_mod_fixture(int mod_id);

boolean poll_for_dig_complete(int mod_id, hfs_fd_type * id);

boolean poll_for_dmm_complete(int mod_id, hfs_fd_type * id);

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

double aosc_cal(int mod_id, double voltage1, double voltage2, double * gain);

void setup_inst_defaults(int mod_id);

void setup_dcv_dig(int mod_id, int range, double interval, int num_samples, double aperature);

void setup_dcv50_dig(int mod_id, int range, double interval, int num_samples, double aperature);

void setup_ssdc_dig(int mod_id, int range, double interval, int num_samples);

void setup_ssdc_fast(int mod_id, int range, int level, int slope, double interval, double sample_delay, int num_samples);

void setup_ssdc50_dig(int mod_id, int range, double interval, int num_samples);

void setup_ssdc50_fast(int mod_id, int range, int level, int slope, double interval, double sample_delay, int num_samples);

void clear_dmm50_memory(int mod_id);

void capture_ddm(int mod_id, int num_samples, double * waveform, int mode);

void capture_ddm50(int mod_id, int num_samples, double * waveform, int mode);

void xmhpib_wr(int mod_id, int tag, char * s);

void setup_osc_defaults(int mod_id);

void open_osc(int mod_id);

void setup_osc_load(int mod_id, int osc_channel, int load);

void setup_osc_channel(int mod_id, int osc_channel, int load, double channel_range, double channel_offset);

void setup_osc_trigger(int mod_id, int osc_channel, int trigger_slope, double trigger_level, int averaging);

void setup_osc_timebase(int mod_id, int averaging, int count, double timebase_range, double timebase_offset, int timebase_mode);

void reset_osc_timebase_range(int mod_id, double timebase_range);

void setup_osc_persistence(int mod_id);

void setup_osc_vmarkers_jitter(int mod_id);

void setup_osc_tmarkers_jitter(int mod_id);

void setup_osc_ti_markers(int mod_id, int slope_start, int slope_stop);

void setup_osc_arb_markers(int mod_id, int slope_start, double start_volts, int start_channel, int slope_stop, double stop_volts, int stop_channel);

void setup_osc_pulse_width_markers(int mod_id, int trigger_slope);

void setup_osc_vmarkers_slew_rate(int mod_id, int trigger_slope);

void trigger_osc(int mod_id, int osc_channel, int averaging);

void trigger_osc_dig(int mod_id, int averaging);

void measure_osc(int mod_id, int osc_function, int trigger_slope);

void measure_osc_tvolts(int mod_id, double tvolts, int slope);

void measure_osc_vtime(int mod_id, double vtime);

void mask_osc_error_display(int mod_id, int status);

void close_osc(int mod_id);

void setup_osc_srq_mask(int mod_id, int mask);

void setup_delay_osc_defaults(int mod_id);

void open_delay_osc(int mod_id);

double measure_td(int mod_id, double t_nominal, int trig_source, int trig_edge, int other_direction, int other_edge, double timebase_sensitivity);

void close_delay_osc(int mod_id);

void connect(int mod_id, int connection, int status);

void connect_cntr_start_af2(int mod_id);

void connect_cntr_stop_db1(int mod_id);

void connect_cntr_stop_uclk(int mod_id);

void connect_dg(int mod_id, int status);

void connect_dmm(int mod_id);

void connect_dmm_pin(int mod_id, int slot);

void connect_gen_trg_cntr_start(int mod_id);

void connect_gen_trg_clkrb(int mod_id);

void connect_gen_trg_osc_ch2(int mod_id);

void connect_gen_out_cntr_stop(int mod_id);

void connect_gen_out_osc_ch1(int mod_id);

void connect_gen_out_af2(int mod_id);

void connect_gen_out_probe(int mod_id);

void connect_lcr(int mod_id);

void connect_minus_pin(int mod_id, int slot, int channel, int status);

void connect_minus_calib2(int mod_id);

void connect_minus_swd_gnd(int mod_id);

void dconnect_minus_swd_gnd(int mod_id);

void connect_minus_xn_plus_1(int mod_id, int channel, int status);

void connect_osc_ch1_af2(int mod_id);

void connect_dmm50_af2(int mod_id);

void aconnect_osc_ch1_af2(int mod_id);

void aconnect_cntr_stop_af2(int mod_id);

void connect_osc_ch1_cable_measure(int mod_id);

void connect_osc_ch2_uclk(int mod_id);

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

void connect_syn_out_osc_ch1(int mod_id);

void connect_syn_out_af2(int mod_id);

void aconnect_syn_out_xmint(int mod_id);

void aconnect_vs_dmm(int mod_id, int status);

void aconnect_vo_dmm(int mod_id, int status);

void connect_dmm_probe(int mod_id);

void aconnect_ps_20_ohm_load(int mod_id, int status);

void connect_load_test_relay(int mod_id, int status);//added by sri tharan for DH-Utility load testing

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

void no_complement_gen_out(int mod_id);

void setup_gen_mode(int mod_id, int mode, int slope);

void setup_gen_levels(int mod_id, double low_level, double high_level);

void setup_gen_pwidth(int mod_id, double pulse_width);

void setup_gen_period(int mod_id, double gen_period);

void setup_gen_fixed_transition(int mod_id);

void setup_gen_transition_times(int mod_id, double tt);

void setup_gen_trg_out_delay(int mod_id, double delay);

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

void disable_syn_out(int mod_id);

void enable_syn_out(int mod_id); //added by chuanho, 13jul2011

void verify_syn(int mod_id);

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

void setup_dmm50(int mod_id, int dmm_function, double range, double resolution);

void setup_dmm_ac_band(int mod_id, int start_freq, int stop_freq);

void setup_dmm50_ac_band(int mod_id, int start_freq, int stop_freq);

void setup_dmm_aperature(int mod_id, double aperature);

void setup_dmm50_aperature(int mod_id, double aperature);

void setup_dmm_fixed_z(int mod_id, int on);

void setup_dmm50_fixed_z(int mod_id, int on);

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

void measure_dmm(int mod_id);

void measure_dmm50(int mod_id);

void close_dmm(int mod_id);

void close_dmm50(int mod_id);

void open_lcr(int mod_id);

void setup_lcr(int mod_id, int lcr_function, int lcr_display, int lcr_ckt_mode, double lcr_freq, int lcr_mult);

void measure_lcr(int mod_id);

void close_lcr(int mod_id);

void setup_cntr_defaults(int mod_id);

void open_cntr(int mod_id);

void setup_cntr_trg_levels(int mod_id, double start_level, double stop_level);

void setup_cntr_trigger_slopes(int mod_id, int start_slope, int stop_slope);

void setup_cntr_function(int mod_id, int cntr_function);

void setup_cntr_gate_time(int mod_id, double gate_time);

void setup_cntr_sample_size(int mod_id, int samples);

void setup_cntr_md4(int mod_id);

void setup_cntr_md1(int mod_id);

void setup_cntr_max_volt(int mod_id);

void setup_cntr_min_volt(int mod_id);

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

void setup_ps(int mod_id, double volts, double amps);

void setup_ps_detector_test(int mod_id, double volts, double amps); //added for 99V test NASRU

void wait_for_ps_ramp(int mod_id, double diff, double ramp_time, double * dmm_actual_voltage);

void disable_ps_out(int mod_id);

void enable_ps_out(int mod_id);

void close_ps(int mod_id);

double measure_ps_sense_voltage(int mod_id);



//added by sri on 28th August 2007 for Load Test- Dragon Heart//

void open_ps_load(int mod_id);

void setup_ps_load(int mod_id, double volts, double amps);

void wait_for_ps_ramp_load(int mod_id, double diff, double ramp_time, double * dmm_actual_voltage);

void disable_ps_out_load(int mod_id);

void enable_ps_out_load(int mod_id);

void close_ps_load(int mod_id);

double measure_ps_sense_voltage_load(int mod_id);

double measure_ps_load_current(int mod_id);

//ended here ! sri//

/*-------------------------------N6700B channel 1--------------------------*/
//Added by chuanho, 28Jun2011

void OpenResourceManager();
void CloseResourceManager();
void open_olym(int mod_id);
void verify_olym(int mod_id);
void setup_olym_ch1_volt(int mod_id, double amplitude);
void setup_olym_ch1_curr(int mod_id, double current);
void setup_olym_ch1_ocp(int mod_id, double current);
void reverse_olym_ch1_out(int mod_id);
void enable_olym_ch1_out(int mod_id);
void disable_olym_ch1_out(int mod_id);
void close_olym(int mod_id);
void connect_olym_ch1(int mod_id);
void aconnect_olym_ch1_abus1_xmint(int mod_id);
void aconnect_olym_ch1_out_dmm(int mod_id, int status);
void dconnect_olym_ch1_af2(int mod_id); //added by chuanho, 21jul2011

/*--------------------------Enf of N6700B channel 1------------------------*/



void open_dut_ps(int mod_id);

void setup_dut_ps(int mod_id, int ps_channel, double volts, double ov, double amps);

void close_dut_ps(int mod_id);

void read_dmm_stats(int mod_id, int dmm_stat);

void read_dmm50_stats(int mod_id, int dmm_stat);

void setup_dmm_readings(int mod_id, int n);

void setup_dmm50_readings(int mod_id, int n);

void calculate_dmm_stats(int mod_id);

void calculate_dmm50_stats(int mod_id);

void trigger_dmm(int mod_id);

void trigger_dmm50(int mod_id);

void wait_for_dmm_measurement(int mod_id);

void wait_for_dmm50_measurement(int mod_id);

boolean dmm_measurement_complete(void);

void hold_dmm(int mod_id);

void hold_dmm50(int mod_id);

void enable_dmm_buffer(int mod_id);

void enable_dmm50_buffer(int mod_id);

void disable_dmm_buffer(int mod_id);

void disable_dmm50_buffer(int mod_id);

void setup_dmm_memory(int mod_id, int fmat, int mem);

void setup_dmm50_memory(int mod_id, int fmat, int mem);

void read_dmm(int mod_id);

void read_dmm50(int mod_id);

void verify_ps_output(int mod_id, double diff, double ramp_time, double expected_volts);

void verify_ps_output_load(int mod_id, double diff, double ramp_time, double expected_volts);

boolean det_over_range(int mod_id);

double measure_open(int mod_id, double vplus, double vminus, double rfb);

double measure_open_non(int mod_id, double vplus, double vminus, double rfb);

void check_uprange(int mod_id, double * delta);

double input_real(char * prompt);

det_range_type sel_det_range(double dcv_input);

void test_detector_range(int mod_id, double range_voltage, int range_number, double const * test);

void test_det_atten_range(int mod_id, double range_voltage, int num_points, double const * test);

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

double calc_mod_correction(int mod_num, double sd_factor);

double get_gen_cal_delay(int mod_id);

void log_array_data(int test, int subtest, int mod_id, int slot, double * ap, int index, double data_to_log);

void setup_dmm_digitizing(int mod_id, int dmm_addr, int range, double interval, int num_samples, double aperature, double resolution, int trig_level, int slope);

void get_dmm_volt(int mod_id, int dmm_addr, int num_samples, double * waveform, int mode);

void setup_dmm_subsampling_AC_and_DC(int mod_id, int dmm_addr, int num_samples, double sampling_rate, int trig_level, int slope, int range);

void connect_gen_out_cntr_ch1(int mod_id);

void connect_cntr_ch1_af2(int mod_id);

void connect_dmm50_out_af2(int mod_id);

void check_dmm50_button(int mod_id, int usage);

int check_nasru_p10(int mod_id);

#endif

