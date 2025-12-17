#include <time.h>

#include <stdio.h>

#include <assert.h>

#include <p2c/p2c.h>



#include "libsalesshared.h"

#include "../parms_p.h"

#include "../execsup_p.h"

#include "../tests/dispense_p.h"

#include "mt9000_p.h"

#include "mt9250_p.h"

#include "mt9500_p.h"

#include "mt9750_p.h"

#include "mt10000_p.h"

#include "mt10750_p.h"



_LIBSALESSHARED_API_ void add_test_to_task_list(int test_number, int mapped_module_number, int slot_number, int * module_slot_list)

{

  FILE * test_time_file = 0;

  int const buffer_size = 80;

  char buffer[buffer_size + 1];



  if (save_lists_parm)

    wr_test_to_exec_file(test_number, mapped_module_number, slot_number, module_slot_list);



  if (test_time_file)

    test_time_file = freopen("sales.times", "a", test_time_file);

  else

    test_time_file = fopen("sales.times", "a");



  time_t test_start_time = time(0);  //record current time in seconds



  switch (test_number)

  {

    case 9000: t9000(); break;

    case 9001: t9001(mapped_module_number, slot_number); break;

    case 9002: t9002(mapped_module_number, slot_number); break;

    case 9003: t9003(mapped_module_number, slot_number); break;

    case 9004: t9004(); break;

    case 9005: t9005(); break;

    case 9006: t9006(module_slot_list); break;

    case 9007: t9007(); break;

    case 9008: t9008(); break;

    case 9010: t9010(mapped_module_number, slot_number); break;

    case 9011: t9011(mapped_module_number, slot_number); break; //newly added sri

	case 9012: t9012(mapped_module_number, slot_number); break; //newly added sri

	case 9016: t9016(mapped_module_number, slot_number); break; //newly added sri

    case 9231: t9231(mapped_module_number, slot_number); break; //newly added sri

	case 9232: t9232(mapped_module_number, slot_number); break; //newly added sri

	case 9233: t9233(mapped_module_number, slot_number); break; //newly added sri

	case 9234: t9234(mapped_module_number, slot_number); break; //newly added sri

	case 9235: t9235(mapped_module_number, slot_number); break; //newly added sri

	case 9236: t9236(mapped_module_number, slot_number); break; //newly added sri

	case 9237: t9237(mapped_module_number, slot_number); break; //newly added sri

	case 9238: t9238(mapped_module_number, slot_number); break; //newly added sri

	case 9239: t9239(mapped_module_number, slot_number); break; //newly added sri

	case 9222: t9222(mapped_module_number, slot_number); break; //newly added sri

	case 9226: t9226(mapped_module_number, slot_number); break; //newly added sri

	case 9015: t9015(); break;

    case 9020: t9020(mapped_module_number, slot_number); break;

    case 9021: t9021(mapped_module_number, slot_number); break;

    case 9030: t9030(mapped_module_number, slot_number); break;

    case 9031: t9031(mapped_module_number, slot_number); break;

    case 9032: t9032(mapped_module_number, slot_number); break;

    case 9033: t9033(mapped_module_number, slot_number); break;

    case 9034: t9034(mapped_module_number, slot_number); break;

	case 9035: t9035(mapped_module_number, slot_number); break;

    case 9040: t9040(mapped_module_number, slot_number); break;

    case 9041: t9041(mapped_module_number, slot_number); break;

    case 9042: t9042(mapped_module_number, slot_number); break;

    case 9043: t9043(mapped_module_number, slot_number); break;

    case 9044: t9044(mapped_module_number, slot_number); break;

    case 9047: t9047(mapped_module_number, slot_number); break;

    case 9048: t9048(); break;

    case 9049: t9049(); break;

    case 9050: t9050(mapped_module_number, slot_number); break;

    case 9052: t9052(mapped_module_number, slot_number); break;

    case 9053: t9053(mapped_module_number, slot_number); break;

    case 9055: t9055(mapped_module_number, slot_number); break;

    case 9056: t9056(mapped_module_number, slot_number); break;

    case 9060: t9060(mapped_module_number, slot_number); break;

    case 9061: t9061(mapped_module_number, slot_number); break;

    case 9062: t9062(mapped_module_number, slot_number); break;

    case 9065: t9065(mapped_module_number, slot_number); break;

    case 9070: t9070(mapped_module_number, slot_number); break;

    case 9071: t9071(mapped_module_number, slot_number); break;

    case 9072: t9072(mapped_module_number, slot_number); break;

    case 9075: t9075(mapped_module_number, slot_number); break;

    case 9080: t9080(mapped_module_number, slot_number); break;

    case 9081: t9081(mapped_module_number, slot_number); break;

    case 9082: t9082(mapped_module_number, slot_number); break;

    case 9090: t9090(mapped_module_number, slot_number); break;

    case 9091: t9091(mapped_module_number, slot_number); break;

    case 9092: t9092(mapped_module_number, slot_number); break;

    case 9095: t9095(mapped_module_number, slot_number); break;

    case 9100: t9100(mapped_module_number, slot_number); break;

    case 9101: t9101(mapped_module_number, slot_number); break;

    case 9102: t9102(mapped_module_number, slot_number); break;

    case 9103: t9103(mapped_module_number, slot_number); break;

    case 9104: t9104(mapped_module_number, slot_number); break;

    case 9105: t9105(mapped_module_number, slot_number); break;

    case 9106: t9106(mapped_module_number, slot_number); break;

    case 9107: t9107(mapped_module_number, slot_number); break;

    case 9111: t9111(mapped_module_number, slot_number); break;

    case 9120: t9120(mapped_module_number, slot_number); break;

    case 9121: t9121(mapped_module_number, slot_number); break;

    case 9122: t9122(mapped_module_number, slot_number); break;

    case 9125: t9125(module_slot_list); break;

    case 9126: t9126(module_slot_list); break;

    case 9127: t9127(module_slot_list); break;

    case 9129: t9129(mapped_module_number, slot_number); break;

    case 9130: t9130(mapped_module_number, slot_number); break;

    case 9131: t9131(mapped_module_number, slot_number); break;

    case 9132: t9132(mapped_module_number, slot_number); break;

    case 9133: t9133(mapped_module_number, slot_number); break;

    case 9134: t9134(mapped_module_number, slot_number); break;

    case 9135: t9135(mapped_module_number, slot_number); break;

    case 9136: t9136(mapped_module_number, slot_number); break;

    case 9137: t9137(mapped_module_number, slot_number); break;

    case 9138: t9138(mapped_module_number, slot_number); break;

    case 9140: t9140(mapped_module_number, slot_number); break;

    case 9141: t9141(mapped_module_number, slot_number); break;

    case 9142: t9142(mapped_module_number, slot_number); break;

    case 9143: t9143(mapped_module_number, slot_number); break;

    case 9144: t9144(mapped_module_number, slot_number); break;

    case 9145: t9145(mapped_module_number, slot_number); break;

    case 9146: t9146(mapped_module_number, slot_number); break;

    case 9147: t9147(mapped_module_number, slot_number); break;

    case 9148: t9148(mapped_module_number, slot_number); break;

    case 9149: t9149(mapped_module_number, slot_number); break;

    case 9150: t9150(mapped_module_number, slot_number); break;

    case 9151: t9151(mapped_module_number, slot_number); break;

    case 9152: t9152(mapped_module_number, slot_number); break;

    case 9153: t9153(mapped_module_number, slot_number); break;

    case 9155: t9155(mapped_module_number, slot_number); break;

    case 9156: t9156(mapped_module_number, slot_number); break;

    case 9160: t9160(mapped_module_number, slot_number); break;

    case 9161: t9161(mapped_module_number, slot_number); break;

    case 9162: t9162(mapped_module_number, slot_number); break;

    case 9165: t9165(mapped_module_number, slot_number); break;

    case 9166: t9166(mapped_module_number, slot_number); break;

    case 9167: t9167(mapped_module_number, slot_number); break;

    case 9170: t9170(mapped_module_number, slot_number); break;

    case 9171: t9171(mapped_module_number, slot_number); break;

    case 9172: t9172(mapped_module_number, slot_number); break;

    case 9173: t9173(mapped_module_number, slot_number); break;

    case 9180: t9180(mapped_module_number, slot_number); break;

    case 9181: t9181(mapped_module_number, slot_number); break;

    case 9182: t9182(mapped_module_number, slot_number); break;

    case 9183: t9183(mapped_module_number, slot_number); break;

    case 9184: t9184(mapped_module_number, slot_number); break;

    case 9185: t9185(mapped_module_number, slot_number); break;

    case 9186: t9186(mapped_module_number, slot_number); break;

    case 9187: t9187(mapped_module_number, slot_number); break;

    case 9188: t9188(mapped_module_number, slot_number); break;

    case 9189: t9189(mapped_module_number, slot_number); break;

    case 9191: t9191(); break;

    case 9195: t9195(); break;

    case 9196: t9196(); break;

    case 9200: t9200(mapped_module_number, slot_number); break;

    case 9201: t9201(mapped_module_number, slot_number); break;

    case 9205: t9205(mapped_module_number, slot_number); break;

    case 9206: t9206(mapped_module_number, slot_number); break;

    case 9207: t9207(mapped_module_number, slot_number); break;

    case 9208: t9208(mapped_module_number, slot_number); break;

    case 9209: t9209(mapped_module_number, slot_number); break;

    case 9210: t9210(mapped_module_number, slot_number); break;

    case 9211: t9211(mapped_module_number, slot_number); break;

    case 9217: t9217(mapped_module_number, slot_number); break;

    case 9220: t9220(mapped_module_number, slot_number); break;

    case 9221: t9221(mapped_module_number, slot_number); break;

    case 9230: t9230(mapped_module_number, slot_number); break;

    case 9240: t9240(mapped_module_number, slot_number); break;

    case 9223: t9223(mapped_module_number, slot_number); break;

	case 9241: t9241(mapped_module_number, slot_number); break;

    case 9242: t9242(mapped_module_number, slot_number); break;

    case 9224: t9224(mapped_module_number, slot_number); break;

	case 9243: t9243(mapped_module_number, slot_number); break;

    case 9244: t9244(mapped_module_number, slot_number); break;

    case 9245: t9245(mapped_module_number, slot_number); break;

    case 9246: t9246(mapped_module_number, slot_number); break;

    case 9225: t9225(mapped_module_number, slot_number); break;

	case 9247: t9247(mapped_module_number, slot_number); break;

    case 9248: t9248(mapped_module_number, slot_number); break;

    case 9249: t9249(mapped_module_number, slot_number); break;

    case 9250: t9250(mapped_module_number, slot_number); break;

    case 9252: t9252(mapped_module_number, slot_number); break;

    case 9255: t9255(mapped_module_number, slot_number); break;

    case 9256: t9256(mapped_module_number, slot_number); break;

    case 9260: t9260(mapped_module_number, slot_number); break;

    //case 9261: t9261(mapped_module_number, slot_number); break;

	//case 9262: t9262(mapped_module_number, slot_number); break;

	//case 9263: t9263(mapped_module_number, slot_number); break;

	case 9265: t9265(mapped_module_number, slot_number); break;

    case 9266: t9266(mapped_module_number, slot_number); break;

    case 9267: t9267(mapped_module_number, slot_number); break;

    case 9268: t9268(mapped_module_number, slot_number); break;

    case 9270: t9270(mapped_module_number, slot_number); break;

    case 9275: t9275(mapped_module_number, slot_number); break;

    case 9274: t9274(mapped_module_number, slot_number); break;

	case 9324: t9324(mapped_module_number, slot_number); break;

    case 9372: t9372(mapped_module_number, slot_number); break;

	case 9474: t9474(mapped_module_number, slot_number); break;

    case 9477: t9477(mapped_module_number, slot_number); break;

    case 9428: t9428(mapped_module_number, slot_number); break;

	case 9425: t9425(mapped_module_number, slot_number); break;

	case 9277: t9277(mapped_module_number, slot_number); break;

    case 9300: t9300(mapped_module_number, slot_number); break;

	case 9301: t9301(mapped_module_number, slot_number); break;

	case 9305: t9305(mapped_module_number, slot_number); break;

    case 9306: t9306(mapped_module_number, slot_number); break;

    case 9310: t9310(mapped_module_number, slot_number); break;

    case 9315: t9315(mapped_module_number, slot_number); break;

    case 9316: t9316(mapped_module_number, slot_number); break;

    case 9317: t9317(mapped_module_number, slot_number); break;

    case 9318: t9318(mapped_module_number, slot_number); break;

    case 9320: t9320(mapped_module_number, slot_number); break;

    case 9325: t9325(mapped_module_number, slot_number); break;

    case 9327: t9327(mapped_module_number, slot_number); break;

    case 9355: t9355(mapped_module_number, slot_number); break;

    case 9360: t9360(mapped_module_number, slot_number); break;

    case 9365: t9365(mapped_module_number, slot_number); break;

    case 9366: t9366(mapped_module_number, slot_number); break;

    case 9367: t9367(mapped_module_number, slot_number); break;

    case 9370: t9370(mapped_module_number, slot_number); break;

    case 9371: t9371(mapped_module_number, slot_number); break;

    case 9380: t9380(mapped_module_number, slot_number); break;

    case 9383: t9383(mapped_module_number, slot_number); break;

    case 9385: t9385(mapped_module_number, slot_number); break;

    case 9386: t9386(mapped_module_number, slot_number); break;

    case 9387: t9387(mapped_module_number, slot_number); break;

    case 9390: t9390(mapped_module_number, slot_number); break;

    case 9391: t9391(mapped_module_number, slot_number); break;

    case 9392: t9392(mapped_module_number, slot_number); break;

    case 9393: t9393(mapped_module_number, slot_number); break;

    case 9394: t9394(mapped_module_number, slot_number); break;

    case 9410: t9410(mapped_module_number, slot_number); break;

    case 9415: t9415(mapped_module_number, slot_number); break;

    case 9416: t9416(mapped_module_number, slot_number); break;

    case 9420: t9420(mapped_module_number, slot_number); break;

    case 9421: t9421(mapped_module_number, slot_number); break;

    case 9426: t9426(mapped_module_number, slot_number); break;

    case 9427: t9427(mapped_module_number, slot_number); break;

    case 9440: t9440(mapped_module_number, slot_number); break;

    case 9450: t9450(mapped_module_number, slot_number); break;

	case 9430: t9430(mapped_module_number); break;

    case 9431: t9431(mapped_module_number, slot_number); break;

    case 9451: t9451(mapped_module_number, slot_number); break;
    
	case 9452: t9452(mapped_module_number, slot_number); break;

    case 9453: t9453(mapped_module_number, slot_number); break;

    case 9454: t9454(mapped_module_number, slot_number); break;

    case 9455: t9455(mapped_module_number, slot_number); break;

    case 9456: t9456(mapped_module_number, slot_number); break;

    case 9457: t9457(mapped_module_number, slot_number); break;

    case 9458: t9458(mapped_module_number, slot_number); break;

    case 9459: t9459(mapped_module_number, slot_number); break;

    case 9460: t9460(mapped_module_number, slot_number); break;

    case 9461: t9461(mapped_module_number, slot_number); break;

    case 9462: t9462(mapped_module_number, slot_number); break;

    case 9463: t9463(mapped_module_number, slot_number); break;

    case 9465: t9465(mapped_module_number, slot_number); break;

    case 9466: t9466(mapped_module_number, slot_number); break;

    case 9470: t9470(mapped_module_number, slot_number); break;

    case 9471: t9471(mapped_module_number, slot_number); break;

    case 9472: t9472(mapped_module_number, slot_number); break;

    case 9475: t9475(mapped_module_number, slot_number); break;

    case 9476: t9476(mapped_module_number, slot_number); break;

    case 9480: t9480(mapped_module_number, slot_number); break;

    case 9481: t9481(mapped_module_number, slot_number); break;

    case 9482: t9482(mapped_module_number, slot_number); break;

    case 9500: t9500(); break;

    case 9605: t9605(mapped_module_number, slot_number); break;

    case 9620: t9620(mapped_module_number, slot_number); break;

    case 9671: t9671(mapped_module_number, slot_number); break;

    case 9685: t9685(mapped_module_number, slot_number); break;

    case 9700: t9700(mapped_module_number, slot_number); break;

    case 9701: t9701(mapped_module_number, slot_number); break;

    case 9705: t9705(mapped_module_number, slot_number); break;

    case 9706: t9706(mapped_module_number, slot_number); break;

    case 9710: t9710(mapped_module_number, slot_number); break;

    case 9711: t9711(mapped_module_number, slot_number); break;

    case 9721: t9721(mapped_module_number, slot_number); break;

    case 9730: t9730(mapped_module_number, slot_number); break;

    case 9800: t9800(mapped_module_number, slot_number); break;

    case 9801: t9801(mapped_module_number, slot_number); break;

    case 9802: t9802(mapped_module_number, slot_number); break;

    case 9803: t9803(mapped_module_number, slot_number); break;

    case 9804: t9804(mapped_module_number, slot_number); break;

    case 9805: t9805(mapped_module_number, slot_number); break;

    case 9806: t9806(mapped_module_number, slot_number); break;

    case 9807: t9807(mapped_module_number, slot_number); break;

    case 9811: t9811(mapped_module_number, slot_number); break;

    case 9820: t9820(mapped_module_number, slot_number); break;

    case 9821: t9821(mapped_module_number, slot_number); break;

    case 9823: t9823(mapped_module_number, slot_number); break;

	case 9828: t9828(mapped_module_number, slot_number); break;

    case 9829: t9829(mapped_module_number, slot_number); break;

    case 9830: t9830(mapped_module_number, slot_number); break;

    case 9832: t9832(mapped_module_number, slot_number); break;

    case 9835: t9835(mapped_module_number, slot_number); break;

	case 9836: t9836(mapped_module_number, slot_number); break;

    case 9837: t9837(mapped_module_number, slot_number); break;

    case 9840: t9840(mapped_module_number, slot_number); break;

    case 9841: t9841(mapped_module_number, slot_number); break;

    case 9844: t9844(mapped_module_number, slot_number); break;

    case 9847: t9847(mapped_module_number, slot_number); break;

    case 9848: t9848(mapped_module_number, slot_number); break;

	case 9849: t9849(mapped_module_number, slot_number); break;

    case 9850: t9850(mapped_module_number, slot_number); break;

    case 9851: t9851(mapped_module_number, slot_number); break;

    case 9852: t9852(mapped_module_number, slot_number); break;

    case 9853: t9853(mapped_module_number, slot_number); break;
		
	case 9859: t9859(mapped_module_number, slot_number); break;

    case 9860: t9860(mapped_module_number, slot_number); break;

    case 9861: t9861(mapped_module_number, slot_number); break;

    case 9862: t9862(mapped_module_number, slot_number); break;

	case 9870: t9870(mapped_module_number, slot_number); break;

    case 9871: t9871(mapped_module_number, slot_number); break;

    case 9882: t9882(mapped_module_number, slot_number); break;

    case 9883: t9883(mapped_module_number, slot_number); break;

    case 9900: t9900(mapped_module_number, slot_number); break;

    case 9901: t9901(mapped_module_number, slot_number); break;

    case 9905: t9905(mapped_module_number, slot_number); break;

    case 9906: t9906(mapped_module_number, slot_number); break;

    case 9907: t9907(mapped_module_number, slot_number); break;

    case 9909: t9909(mapped_module_number, slot_number); break;

    case 9910: t9910(mapped_module_number, slot_number); break;

    case 9911: t9911(mapped_module_number, slot_number); break;

    case 9917: t9917(mapped_module_number, slot_number); break;

    case 9920: t9920(mapped_module_number, slot_number); break;

    case 9921: t9921(mapped_module_number, slot_number); break;

    case 9930: t9930(mapped_module_number, slot_number); break;

    case 9995: t9995(mapped_module_number, slot_number); break;

 /* case 9996: t9996(mapped_module_number, slot_number); break; */

    case 9997: t9997(mapped_module_number, slot_number); break;

    case 9998: t9998(mapped_module_number, slot_number); break;

    case 9999: t9999(mapped_module_number, slot_number); break;   /*  needs to recompile  */

    case 10188: t10188(mapped_module_number, slot_number); break;

    case 10189: t10189(mapped_module_number, slot_number); break;

    case 10800: t10800(mapped_module_number, slot_number); break;

    case 10802: t10802(mapped_module_number, slot_number); break;

    case 10804: t10804(mapped_module_number, slot_number); break;

    case 10806: t10806(mapped_module_number, slot_number); break;

    case 10820: t10820(mapped_module_number, slot_number); break;

    case 10821: t10821(mapped_module_number, slot_number); break;

    case 10823: t10823(mapped_module_number, slot_number); break;

    case 10829: t10829(mapped_module_number, slot_number); break;

    case 10830: t10830(mapped_module_number, slot_number); break;

    case 10832: t10832(mapped_module_number, slot_number); break;

    case 10835: t10835(mapped_module_number, slot_number); break;

    case 10836: t10836(mapped_module_number, slot_number); break;

    case 10840: t10840(mapped_module_number, slot_number); break;

    case 10841: t10841(mapped_module_number, slot_number); break;

    case 10847: t10847(mapped_module_number, slot_number); break;

    case 10848: t10848(mapped_module_number, slot_number); break;

    case 10850: t10850(mapped_module_number, slot_number); break;

    case 10851: t10851(mapped_module_number, slot_number); break;

    case 10852: t10852(mapped_module_number, slot_number); break;

    case 10853: t10853(mapped_module_number, slot_number); break;

    case 10860: t10860(mapped_module_number, slot_number); break;

    case 10861: t10861(mapped_module_number, slot_number); break;

    case 10862: t10862(mapped_module_number, slot_number); break;

    case 10870: t10870(mapped_module_number, slot_number); break;

    case 10871: t10871(mapped_module_number, slot_number); break;

    case 10872: t10872(mapped_module_number, slot_number); break;

    case 10873: t10873(mapped_module_number, slot_number); break;

    case 10882: t10882(mapped_module_number, slot_number); break;

    case 10883: t10883(mapped_module_number, slot_number); break;

    case 10900: t10900(mapped_module_number, slot_number); break;

    case 10901: t10901(mapped_module_number, slot_number); break;

    case 10907: t10907(mapped_module_number, slot_number); break;

    case 10909: t10909(mapped_module_number, slot_number); break;

    case 10910: t10910(mapped_module_number, slot_number); break;

    case 10911: t10911(mapped_module_number, slot_number); break;

    case 10917: t10917(mapped_module_number, slot_number); break;

    case 10918: t10918(mapped_module_number, slot_number); break;

    case 10919: t10919(mapped_module_number, slot_number); break;

    case 10920: t10920(mapped_module_number, slot_number); break;

    case 10921: t10921(mapped_module_number, slot_number); break;

    case 10930: t10930(mapped_module_number, slot_number); break;

    case 10999: t10999(mapped_module_number, slot_number); break;



    case 9441: t9441(mapped_module_number, slot_number); break;

    case 9442: t9442(mapped_module_number, slot_number); break;

    case 9443: t9443(mapped_module_number, slot_number); break;

    case 9444: t9444(mapped_module_number, slot_number); break;

    case 9445: t9445(mapped_module_number, slot_number); break;

    case 9446: t9446(mapped_module_number, slot_number); break;

    case 9447: t9447(mapped_module_number, slot_number); break;

    case 9448: t9448(mapped_module_number, slot_number); break;

    case 9449: t9449(mapped_module_number, slot_number); break;





    default:

      clear_screen();

      printf("\nA test was found that is not yet in the \"test_dispense\" list.\n");

      printf("Recompile and re-run.\n");

      _Escape(1000);   /*  needs to recompile  */

      break;

  }



  reset9000 = 0;

  // while we have an instrument exception and we have reset the

  // sales rack <3 times, attempt to clear the errors.

  while (((exception_value() >= UNABLE_TO_CLOSE_FILE) && (exception_value() <= UNABLE_TO_WRITE_FILE)) && (reset9000 < 3))

  {

    display_it(mapped_module_number, "exception received = ", exception_value(), 0.1);

    set_exception_register(NO_EXCEPTION);

    ++reset9000;

    t9000();

  }



  if (reset9000 == 3)

  {

      display_prompt("HPIB device problem", true);

      display_prompt(" Hit Break to abort testing...", true);

      get_user_answer(buffer, buffer_size);

  }



  assert(test_time_file);

  fprintf(test_time_file, "%d\t%f.0\t m %d sl %d\n", test_number, difftime(time(NULL), test_start_time), mapped_module_number, slot_number);

  fclose(test_time_file);



}

