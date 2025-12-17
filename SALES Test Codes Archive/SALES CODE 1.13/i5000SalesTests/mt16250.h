#ifndef MT16250_H
#define MT16250_H

void t16250(int mod_id, int slot); //9050
void t16252(int mod_id, int slot); //9052
void t16256(int mod_id, int slot); //9056
void t16260(int mod_id, int slot); //9060
void t16261(int mod_id, int slot); //9061
void t16265(int mod_id, int slot); //9065
void t16275(int mod_id, int); //9075
void t16297(int mod_id); //9191
void probe_voltage_accuracy(int mod_id, int test_number); //9195 & 9196
inline void t16298(int mod_id) { probe_voltage_accuracy(mod_id, 16298); }
inline void t16299(int mod_id) { probe_voltage_accuracy(mod_id, 16299); }
void t16305(int mod_id, int slot); //9305
void t16306(int mod_id, int slot); //9306
void t16310(int mod_id, int slot); //9310
void t16315(int mod_id, int slot); //9315
void t16316(int mod_id, int slot); //9316
void aux_source_ov(int mod_id, int test_number); //9317 & 9318
inline void t16317(int mod_id) { aux_source_ov(mod_id, 16317); }
inline void t16318(int mod_id) { aux_source_ov(mod_id, 16318); }
void t16320(int mod_id, int slot); //9320
void t16325(int mod_id, int slot); //9325
void t16327(int mod_id, int slot); //9327
void t16330(int mod_id, int slot); //9255
void t16331(int mod_id, int slot); //9256
void t16332(int mod_id, int slot); //9260
void t16333(int mod_id, int slot); //9265
void t16334(int mod_id, int slot); //9266
void dc_src_ov(int mod_id, int test_number); //9267 & 9268
inline void t16335(int mod_id) { dc_src_ov(mod_id, 16335); }
inline void t16336(int mod_id) { dc_src_ov(mod_id, 16336); }
void t16337(int mod_id, int slot); //9270
void t16338(int mod_id, int slot); //9275
void t16340(int mod_id, int slot); //9277
void t16355(int mod_id, int slot); //9355
void t16360(int mod_id, int slot); //9360
void t16367(int mod_id, int slot); //9367
void t16370(int mod_id, int slot); //9370
void t16371(int mod_id, int slot); //9371 & 9383
void t16380(int mod_id, int slot); //9380
void t16386(int mod_id, int slot); //9386
void t16387(int mod_id, int slot); //9387
void t16391(int mod_id, int slot); //9391
void t16392(int mod_id, int slot); //9392
void t16393(int mod_id, int slot); //9393
void t16394(int mod_id, int slot); //9394
void t16415(int mod_id, int slot); //9415
void t16416(int mod_id, int slot); //9416
void t16420(int mod_id, int slot); //9420
void t16427(int mod_id, int slot); //9427
void t16452(int mod_id, int slot); //9452
void t16453(int mod_id, int slot); //9453
void t16456(int mod_id, int slot); //9456
void t16457(int mod_id, int slot); //9457
void t16458(int mod_id, int slot); //9458
void t16459(int mod_id, int slot); //9459
void t16460(int mod_id, int slot); //9460
void t16461(int mod_id, int slot); //9461
void t16462(int mod_id, int slot); //9462
void t16463(int mod_id, int slot); //9463
void t16465(int mod_id, int slot); //9465
void t16466(int mod_id, int slot); //9466
void t16470(int mod_id, int slot); //9470
void t16471(int mod_id, int slot); //9471
void t16472(int mod_id, int slot); //9472
void t16475(int mod_id, int slot); //9475
void t16476(int mod_id, int slot); //9476
void t16480(int mod_id, int slot); //9480
void t16481(int mod_id, int slot); //9481
void t16482(int mod_id, int slot); //9482

#endif
