#ifndef MT16750_H
#define MT16750_H

void static_drive_output(int mod_id, int slot, int test_number);
void driver_edge_placement(int mod_id, int slot, int test_number);
void driver_output_impedance(int mod_id, int slot, int test_number);
void reciever_input_current(int mod_id, int slot, int test_number);
void drive_current(int mod_id, int slot, int test_number);
void reciver_voltage_accuracy(int mod_id, int slot, int test_number);
void reciever_edge(int mod_id, int slot, int test_number);
inline void t16800(int mod_id, int slot) { static_drive_output(mod_id, slot, 0); }
inline void t16801(int mod_id, int slot) { static_drive_output(mod_id, slot, 1); }
inline void t16802(int mod_id, int slot) { static_drive_output(mod_id, slot, 2); }
inline void t16803(int mod_id, int slot) { static_drive_output(mod_id, slot, 3); }
inline void t16804(int mod_id, int slot) { static_drive_output(mod_id, slot, 4); }
void t16806(int mod_id, int slot);
void t16811(int mod_id, int slot);
inline void t16820(int mod_id, int slot) { driver_edge_placement(mod_id, slot, 16820); }
inline void t16821(int mod_id, int slot) { driver_edge_placement(mod_id, slot, 16821); }
inline void t16829(int mod_id, int slot) { driver_edge_placement(mod_id, slot, 16829); }
void t16832(int mod_id, int slot);
inline void t16835(int mod_id, int slot) { driver_output_impedance(mod_id, slot, 16835); }
inline void t16836(int mod_id, int slot) { driver_output_impedance(mod_id, slot, 16835); }
inline void t16840(int mod_id, int slot) { drive_current(mod_id, slot, 16840); }
inline void t16841(int mod_id, int slot) { drive_current(mod_id, slot, 16841); }
void t16847(int mod_id, int slot);
inline void t16850(int mod_id, int slot) { reciver_voltage_accuracy(mod_id, slot, 16850); }
inline void t16851(int mod_id, int slot) { reciver_voltage_accuracy(mod_id, slot, 16851); }
inline void t16852(int mod_id, int slot) { reciver_voltage_accuracy(mod_id, slot, 16852); }
inline void t16853(int mod_id, int slot) { reciver_voltage_accuracy(mod_id, slot, 16853); }
inline void t16860(int mod_id, int slot) { reciever_input_current(mod_id, slot, 16860); }
inline void t16861(int mod_id, int slot) { reciever_input_current(mod_id, slot, 16861); }
inline void t16871(int mod_id, int slot) { reciever_edge(mod_id, slot, 16871); }
inline void t16872(int mod_id, int slot) { reciever_edge(mod_id, slot, 16872); }
void t16900(int mod_id, int slot);
void t16904(int mod_id, int slot);
void t16951(int mod_id, int slot);
void t16999(int mod_id, int slot);

#endif
