type
  What_String_Type = string[80]						;
  trig_mode = (LEVEL,AUTO)						;
  mem_fmt = ( ascii,sint,dint,sreal,dreal)				;
  mem_mode = ( moff,lifo,fifo,cont )					;
  stat_type  = ( mean,sdev,n,min,max,all_stats )			;
  det_range_type = 0..8							;
  channel_type  = 'A'..'H'                                             ;
  sschannel_type  = 0..23                                              ;
  ssvirt_card_range  = 0..3 ; { 3 indicates all cards (0,1,2) }
  mux_node_type = 0..8                                                 ;
  byte_subrange = 0..255                                               ;
  word_subrange = 0..65535                                             ;
  mod_type      = 0..1                                                 ;
  chan_relays   = array[channel_type] of a_pin_relay                   ;
  aim_x_relays  = array[channel_type] of an_aim_relay                  ;
  mux_relays    = array[mux_node_type] of chan_relays                  ;
  a_word_type   = packed record
    case integer of
      1 : (bits : packed array[0..15] of bit) ;
      2 : (data : word_subrange ) ;
      3 : (bool : packed array[0..15] of boolean) ;
    end ;
  a_byte_type   = packed record
    case integer of
      1 : (bits : packed array[0..7] of bit) ;
      2 : (data : byte_subrange ) ;
      3 : (bool : packed array[0..7] of boolean) ;
    end ;
  { a_cmd_type	= packed record
    case integer of
      1 : ( byte : byte_subrange ) ;
      2 : ( char : char ) ;
    end ; }
  edge_type     = ( rising, falling )                                  ;
  position_type = ( pre, post )                                        ;
  osc_chan_type = 1..2                                                 ;
  path_type     = packed array[1..50] of char                          ;
  cmd_type	= packed array[0..4] of char				;
  realar_type   = array[0..4] of real                                  ;

const
  what_string		= What_String_Type
  [
  '@(#)4.4       01/25/95 12:34:22 global.h'#0
  ]									;
  num_det_points_to_check= 4 ;
  num_slots		=     22					;
  max_s_length		=     80					;
  max_samples		=   8192					;
  max_std_comps		=     50					;
  { Module slot that contains the control card is defined as mac_slot }
  mac_slot		=      6					;

  { GPIB command values are listed below                                  }
  unlisten		= 63 		{GPIB unlisten command}	;
  talk			= 64	{add-on to talker's bus address}	;
  untalk		= 95						;
  listen		= 32	{add-on to listener's bus address}	;
  crlf			= #13#10{ASCII carriage-return + line-feed}	;

  { As of A.05.00, we are using different values for t0, the AutoCal tclk }
  { advance time, based on whether we are using a mac10 or mac11 control card }
  mac_10_t0_actual	= 165.9E-09					;
  mac_11_t0_actual	= 162.9E-09					;

  wait_time_for_relays	=   2.0E-03 ; //1.0E-03	>SRI 27TH March 2007 test 9476;
  standard_wait_time	=    50E-03					;
  relay_open_wait_time	=   100E-06					;
  R_max_parallel	=     1E+13					;

longpi = 3.14159265358979323846E+000 ;
  longtwo_pi		=    2.0 * longpi				;

  filter_20K		=      4					;
  filter_128		=      5					;
  filter_1024		=      6					;
  filter_8192		=      7					;
  freqa			=      0					;
  freqb			=      1					;
  dmm_dc		=      1					;
  dmm_ac		=      2					;

  cntr_5370		=      6      { bus address of counter }	;
  osc_54111		=      7      { bus address of oscilloscope }	;
  dmm_3458		=     10      { bus address of dmm }		;
  read_write		=      2					;
  irq_bit_position	=      1      { 0-7; 0 msb }			;
  det_or_bit		=     12      { 0-15; 0 = msb }			;

  measuring		= 'measuring'					;
  waiting		= 'waiting'					;

  aim_x_plus_1_mint_rlys =
                    aim_x_relays[K742,K743,K744,K745,K746,K747,K748,K741] ;
  aim_x_mint_rlys = aim_x_relays[K741,K742,K743,K744,K745,K746,K747,K748] ;
  aim_x_s_rlys    = aim_x_relays[K733,K734,K735,K736,K737,K738,K739,K740] ;
  aim_x_i_rlys    = aim_x_relays[K725,K726,K727,K728,K729,K730,K731,K732] ;
  aim_x_l_rlys    = aim_x_relays[K717,K718,K719,K720,K721,K722,K723,K724] ;
  aim_x_a_rlys    = aim_x_relays[K709,K710,K711,K712,K713,K714,K715,K716] ;
  aim_x_b_rlys    = aim_x_relays[K701,K702,K703,K704,K705,K706,K707,K708] ;
  pin_kr_rlys     = chan_relays [ KRA, KRB, KRC, KRD, KRE, KRF, KRG, KRH] ;
  pin_kd_rlys     = chan_relays [ KDA, KDB, KDC, KDD, KDE, KDF, KDG, KDH] ;
  pin_kx_rlys     = chan_relays [ KX1, KX2, KX3, KX4, KX5, KX6, KX7, KX8] ;
  pin_kdc_rlys    = chan_relays [KDC1,KDC2,KDC3,KDC4,KDC5,KDC6,KDC7,KDC8] ;
  pin_kgl_rlys    = chan_relays [KGLA,KGLB,KGLC,KGLD,KGLE,KGLF,KGLG,KGLH] ;
  pin_drvr_rlys   = mux_relays  [
                    chan_relays [ K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H],
                    chan_relays [ K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H],
                    chan_relays [ K3A, K3B, K3C, K3D, K3E, K3F, K3G, K3H],
                    chan_relays [ K4A, K4B, K4C, K4D, K4E, K4F, K4G, K4H],
                    chan_relays [ K5A, K5B, K5C, K5D, K5E, K5F, K5G, K5H],
                    chan_relays [ K6A, K6B, K6C, K6D, K6E, K6F, K6G, K6H],
                    chan_relays [ K7A, K7B, K7C, K7D, K7E, K7F, K7G, K7H],
                    chan_relays [ K8A, K8B, K8C, K8D, K8E, K8F, K8G, K8H],
                    chan_relays [ K9A, K9B, K9C, K9D, K9E, K9F, K9G, K9H]];
  pin_rcvr_rlys =   mux_relays  [
                    chan_relays [K10A,K10B,K10C,K10D,K10E,K10f,K10G,K10H],
                    chan_relays [K11A,K11B,K11C,K11D,K11E,K11f,K11G,K11H],
                    chan_relays [K12A,K12B,K12C,K12D,K12E,K12f,K12G,K12H],
                    chan_relays [K13A,K13B,K13C,K13D,K13E,K13f,K13G,K13H],
                    chan_relays [K14A,K14B,K14C,K14D,K14E,K14f,K14G,K14H],
                    chan_relays [K15A,K15B,K15C,K15D,K15E,K15f,K15G,K15H],
                    chan_relays [K16A,K16B,K16C,K16D,K16E,K16f,K16G,K16H],
                    chan_relays [K17A,K17B,K17C,K17D,K17E,K17f,K17G,K17H],
                    chan_relays [K18A,K18B,K18C,K18D,K18E,K18f,K18G,K18H]];


type
  voltage_array = array[1..num_det_points_to_check] of longreal ;
  gen_mode_type        = (norm,triggered,gated)                            ;
  slope_type           = (off,neg,pos,both)                                ;
  wave_type            = (dc,sine,square,triangle)                         ;
  dmm_function_type    = (dcv,acv,four_wire_ohms,two_wire_ohms,fixed_z,fr,per) ;
  osc_function_type    = (rise_time,fall_time,width,vmax,vmin,delta_v,
                          delta_t,duty_cycle,freq,vaverage)                ;
  lcr_function_type    = (inductance,capacitance)                          ;
  cntr_function_type   = (time_interval,trg_level,frequency,period,pwid,nwid,dcycle);
  lcr_display_type     = (Q,D)                                             ;
  lcr_ckt_mode_type    = (series,parallel)                                 ;
  timebase_mode_type   = (automatic,single)                                ;
  s_type               = string[max_s_length]                              ;
  s_ptr_type           = ^s_type                                           ;
  polarity             = 0..1                                              ;

  slot_type                   = 1..num_slots                                   ;
  one_longreal_per_channel    = array[channel_type] of longreal                ;
  std_component_array         = array[1..max_std_comps] of real                ;
  one_longreal_per_ss_channel = array[sschannel_type] of longreal              ;
  two_longreals_per_slot      = array[slot_type,edge_type] of longreal         ;
  one_longreal_per_mod        = array[0..3] of longreal                        ;
  mod_slot_channel_table      = array[0..3,slot_type,channel_type] of longreal ;
  arrform                     = array[1..max_samples] of real                  ;
  temp_array                  = packed array[1..max_samples*4] of char	;
  a_temp_type                 = record
     case Boolean of
	true	: ( c1data,c2data,c3data,c4data : char ) ;
	false	: ( rdata : real ) ;
     end ;
  cv_array			= packed array[1..max_samples] of a_temp_type ;
  database_module             = record
                               mean     : longreal                             ;
                               std      : longreal                             ;
                               sum_x    : longreal                             ;
                               sum_xx   : longreal                             ;
                               n        : integer                              ;
                               comment  : s80                                  ;
                             end                                               ;
  database_system            = array[0..3] of database_module                  ;
  database_channel           = array[channel_type] of database_module          ;
  database_slot              = array[slot_type,edge_type] of database_module   ;

var
  rx_path_offset                : longreal                             ;
  load_5_ohm_resistance         : mod_slot_channel_table               ;
  load_5_ohm_ratio              : mod_slot_channel_table               ;
  no_load_5_ohm_ratio           : mod_slot_channel_table               ;
  load_24_ohm_resistance        : mod_slot_channel_table               ;
  load_24_50_ohm_resistance     : mod_slot_channel_table               ;
  load_24_ohm_ratio             : mod_slot_channel_table               ;
  channel_delay                 : one_longreal_per_channel             ;
  ssfdcf                        : real {falling data correction factor
                                                                 t9671};
  ssrecf                        : real {rising edge correction factor
                                                                 t9620};
  ss_slot_const                 : array[slot_type] of real             ;
  ssrec_const                   : array[sschannel_type] of real        ;
  ssdrvr_const                  : array[sschannel_type] of real        ;
  slot_delay                    : two_longreals_per_slot               ;
  CLKRB_delay                   : one_longreal_per_mod                 ;
  AF2_system_delay              : one_longreal_per_mod                 ;
  AF2_start_delay               : one_longreal_per_mod                 ;
  AF2_CLKRB_delay               : one_longreal_per_mod                 ;
  probe_CLKRB_delay             : one_longreal_per_mod                 ;
  sales_rack_offset_cap         : array[mod_type,channel_type] of real ;
  sales_rack_offset_XG_cap      : array[mod_type] of real              ;
  chan1_probe_atten_delay_osc   : real                                 ;
  chan2_probe_atten_delay_osc   : real                                 ;
  error                         : Boolean                              ;


