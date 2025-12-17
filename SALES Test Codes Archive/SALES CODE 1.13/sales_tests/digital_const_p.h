/* Output from p2c, the Pascal-to-C translator */
/* From input file "digital_const_p.p" */
#ifndef DIGITAL_CONST_P_H
#define DIGITAL_CONST_P_H

#include <p2c/p2c.h>


/* Used for the AIM card connect statement */

#define AIM_SLOT        1

/* Used for the adrv connect statement */
#define OADRV_NUM       0
#define SEQDRV_NUM      1
#define DUTCLK_NUM      2

/* Used for the aim formatter channel input lines */
#define AIMCLK_NUM      2
#define END_OF_N_NUM    3
#define PACCAL_NUM      4
#define BARF0_NUM       5
#define BARF1_NUM       6
#define PTRGIN_NUM      7
#define GLITCH_NUM      8
#define PROBE_NUM       9

/* Used for the AIM source default values */
#define UNUSED_ABLED_NUM  0
#define DISABLED_NUM    1
#define ENABLED_NUM     2

/* Used for the channel data and tri-state sources */
#define FLIP_FLOP_NUM   0
#define ADRV0_NUM       1
#define ADRV1_NUM       2

/* Used for the channel data source */
#define STATIC0_NUM     3
#define STATIC1_NUM     4

/* Used for the channel tri-state source */
#define STATIC_EN_NUM   3
#define STATIC_DIS_NUM  4

/* Used for the channel connect statement */
#define ARCV0_NUM       0
#define ARCV1_NUM       1

#define GTL_NUM         0
#define GTH_NUM         1
#define RCLK_NUM        2
#define RRESP_NUM       3
#define WERR_NUM        4
#define ER_NUM          5
#define REN_NUM         6

/* Used for the receiver Pull up and Pull down options */
#define PULL_UNUSED     0
#define PULL_UP_NUM     1
#define PULL_DOWN_NUM   2

/* Used for the receiver clamp options */
#define CLAMP_UNUSED    0
#define CLAMP_ON_NUM    1
#define CLAMP_OFF_NUM   2

/* Used for the AIM formatter commands */
#define A_NUM           1
#define B_NUM           2
#define C_NUM           4
#define ABAR_NUM        8
#define BBAR_NUM        16
#define CBAR_NUM        32

/* Used for a place holder if there are no flags */
#define NO_FLAG         0

/* Sequence flags  */
#define CRC_FLAG        1
#define SYNC_FLAG       2
#define RCEN_FLAG       4

/* Numbers corresponding to the program commands  */
#define VECT_NUM        1
#define SLS_NUM         2
#define JMP_NUM         3
#define JMPQ_NUM        4
#define LLC_NUM         5
#define SMLC_NUM        6
#define CLC_NUM         7

/* Numbers used for jumpq qualifers  */
#define FAIL_NUM        0
#define PASS_NUM        1
#define CPU_NUM         2
#define CPU_BAR_NUM     3
#define TC_NUM          4
#define TC_BAR_NUM      5

/* Numbers used for the loop counter mode*/
#define UP_NUM          0
#define DOWN_NUM        1

/* Program flags  */
#define SCE_FLAG        1
#define FE_FLAG         2
#define VCEON_FLAG      4
#define VCEOFF_FLAG     8
#define NOP_FLAG        16
#define HALT_FLAG       32
#define PAUSE_FLAG      64
#define ASS_STAT2_FLAG  128
#define NEG_STAT2_FLAG  256
#define TOG_STAT2_FLAG  512
#define ASS_STAT3_FLAG  1024
#define NEG_STAT3_FLAG  2048
#define TOG_STAT3_FLAG  4096
#define ASS_HALT_FLAG   8192
#define NEG_HALT_FLAG   16384
#define TOG_HALT_FLAG   32768
#define ASS_TSP_FLAG    65536
#define NEG_TSP_FLAG    131072
#define TOG_TSP_FLAG    262144
#define EIP_FLAG        524288
#define ASS_CHI_FLAG    1048576
#define NEG_CHI_FLAG    2097152
#define TOG_CHI_FLAG    4194304

/* Channel record definition */
#define DRIVER_NUM      0
#define RECEIVER_NUM    1

/* State defination */
#define ZERO_NUM        0
#define ZERO_TRI_NUM    1
#define ONE_NUM         2
#define ONE_TRI_NUM     3
#define KEEP_NUM        4
#define KEEP_TRI_NUM    5
#define TOGGLE_NUM      6
#define TOGGLE_TRI_NUM  7
#define Z_NUM           8

/* Clock Phase */
#define PHASE_FALLING   0
#define PHASE_RISING    1

#endif /* DIGITAL_CONST_P_H */
