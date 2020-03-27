
#define uint8_len	1
#define uint16_len 	2
#define uint32_len	4
#define uint64_len	8

#define ee_size	512
#define null_terminator	1

#define ee_OnSchedule       0
#define ee_OnSchedule_len   null_terminator + 8
#define ee_OffSchedule      ee_OnSchedule + ee_OnSchedule_len
#define ee_OffSchedule_len  null_terminator + 8
#define ee_mode             ee_OffSchedule + ee_OffSchedule_len
#define ee_mode_len         uint8_len
#define ee_control          ee_mode + ee_mode_len
#define ee_control_len      uint8_len
#define ee_dimmer1           ee_control + ee_control_len
#define ee_dimmer1_len       uint8_len
#define ee_ssid             ee_dimmer1 + ee_dimmer1_len
#define ee_ssid_len         null_terminator + 8
#define ee_passwrd          ee_ssid + ee_ssid_len
#define ee_passwrd_len      null_terminator + 10
#define ee_apn              ee_passwrd + ee_passwrd_len
#define ee_apn_len          null_terminator + 15
#define ee_backend          ee_apn + ee_apn_len
#define ee_backend_len      null_terminator + 20
#define ee_backend_user     ee_backend + ee_backend_len
#define ee_backend_user_len null_terminator + 20
#define ee_backend_pass     ee_backend_user + ee_backend_user_len
#define ee_backend_pass_len null_terminator + 20
#define ee_port             ee_backend_pass + ee_backend_pass_len
#define ee_port_len         null_terminator + uint32_len
#define ee_timedelay        ee_port + ee_port_len
#define ee_timedelay_len    uint8_len
#define ee_dimmer2          ee_timedelay + ee_timedelay_len
#define ee_dimmer2_len      uint8_len
#define ee_NigtSchedule     ee_dimmer2 + ee_dimmer2_len
#define ee_NightSchedule_len   null_terminator + 8

