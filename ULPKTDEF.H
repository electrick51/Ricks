#ifndef _ULPKTDEF_H
#define _ULPKTDEF_H

// Note: Number of vowels in this file's name > 0

// uplink packets
// uplink packets are send to the alarm panel
// byte 0 00000000 sync bits
// byte 1 00000Sii sync, start, id
// byte 2 iiiiiitt id, pkt code = 00
// byte 3 iiiiiiii id
// byte 4 iiiipppp id, pkt type (16 types of ul pkts)
// byte 5 dddddddd data
// byte 6 tttttttt trigger value
// byte 7 tttttttt trigger value
// byte 8 cccccccc crc
// byte 9 cccccccc crc
//
// bytes 3-7 are encrypted

// note - Codes marked *** are used by TWTTPs
//        and are hard coded in the SXT alarm panel
//        DO NOT CHANGE
//
// touchscreen uplink code
// byte 4 = iiii0000
// byte 5 = ts ul code


// type_reset packets send the following codes
// also
// enrollment acks were type keypress
// now sent as type_reset
// and carry the a data byte with the twtts firmware rev
enum TsUlResetCode
{
TsStartingUp=0x26   // this code sent on startup
};

// type_keypress codes
//
// (and some status codes that
//  are sent as a key pkt)
// note - these are legacy codes sent by the TWTTP
//        and the 1st release of the TWTTS
// --- DO NOT ADD ANY CODES... DOING SO WILL CAUSE COMPATIBILTY ISSUES WITH SimonXT V1.3 ---
enum TsUlKeyCode
{
    ZeroKey=0,                  // ***
    OneKey=1,                   // ***
    TwoKey=2,                   // ***
    ThreeKey=3,                 // ***
    FourKey=4,                  // ***
    FiveKey=5,                  // ***
    SixKey=6,                   // ***
    SevenKey=7,                 // ***
    EightKey=8,                 // ***
    NineKey=9,                  // ***
    TpBusy=0xa,                 // ***
    nu_b=0xb,
    nu_c=0xc,
    nu_d=0xd,
    SupervisoryPkt=0xe,         // ***
    AckDirectedPkt=0xf,         // ***
    StatusKey=0x10,             // ***
    Bypass=0x11,
    nu_12=0x12,
    nu_13=0x13,
    EmergencyKey=0x14,          // ***
    EnrollmentAck= 0x15,        // special handling
    nu_16=0x16,
    DisarmKey=0x17,             // ***
    ArmDwKey=0x18,              // ***
    ArmMotsKey=0x19,            // ***
    ArmAllKey=0x1a,
    _LightsOnKey=0x1b,           // ***  (all lights on)
    _LightsOffKey=0x1c,          // ***  (all lights off)
    nu_1d=0x1d,
    res_lost_ = 0x1e,            // *** lost
    res_lost  = 0x1f,            // *** lost send status
    EnrollMeRequest=0x20         // specially handled, sends strong encryption enrollment pkt
};


// type_cmd codes
// new non-legacy command and keys
enum TsUlCmdCode
{
    TsLowBattery=0,
    TsBatteryOk=1,
    EmergencyFire=2,
    EmergencyPolice=3,
    EmergencyAux=4,
    ArmSilentOff=5,
    ArmSilentOn=6,
    ArmInstantOff=7,
    ArmInstantOn= 8,
    ArmingSeqStart =9,          //indicate to panel that twtts is starting an arming sequence
    ClearHistoryReq =0x0a,      //request panel to clear alarm history.
    RFTest=0xb,                 //rf sensor test
    KeyBeep=0xc
};

// note: don't confuse this field with the two bit
//       packet type code field in the 80 bit protocol
enum UL_PKT_TYPE
{
    type_keypress=0,            // warning do not change, must be 0
    type_thermostat,
    type_streamack,
	type_pass_through,
	type_reset,
    type_lighting_control,
    type_cmd,                   // new keys and commands
    type_lock_control,
    type_validate_user_code
};

struct GenericUplink
{
   uint8_t ulbyte0;
   uint8_t ulbyte1;
   uint8_t ulbyte2;
};

struct StreamAckUplink
{
   uint8_t stream_id;
   uint8_t block_seq;
   uint8_t pkt_seq;
};

#define BYTE0_SERVICE_REQUEST 0
#define BYTE0_TWTTS_RESET 1
#define BYTE0_BILLING 2
#define BYTE0_CUSTOMER_SERVICE 3
#define BYTE0_TECHSUPPORT 4
#define BYTE0_DISPLAY_VERSION_HDR 5          //NOTE this indicates next byte in the packet contains the version

struct PassThroughUplink
{
   uint8_t pass_through_byte0;      // byte 0
   uint8_t pass_through_byte1;
   uint8_t pass_through_byte2;
};

struct ThermostatUplink
{
    uint8_t schedules:1;			 //schedules: 0=schedules off, 1=schedules on
    uint8_t mode:2;					 //mode:  00=off, 01=cool  10=heat
    uint8_t fan:1;					 //fan:  0=fan auto, 1=fan on
    uint8_t thermostat_number:2;	 //thermostat number: 0-2
    uint8_t cooling_sp:6;			 //cooling set point: 0-63 (offset 40 degrees, i.e 40 to 103 degrees F)
    uint8_t heating_sp:6;			 //heating set point: 0-63 (offset 40 degrees, i.e 40 to 103 degrees F)
    uint8_t trigger_count:6;
};

struct KeysUplink
{
    enum TsUlKeyCode keycode;
    uint16_t trigger_count;
};

struct CmdUplink
{
    enum TsUlCmdCode cmdcode;
    uint16_t trigger_count;
};

#define LIGHT_ON_ 0x7
#define LIGHT_OFF_ 0
#define ALL_DEVICES 0x1f //must not exceed 5 bit value 
struct LightingControlUplink
{
    uint8_t address:5;              //light control unit addr 0 to 31 = all devices
    uint8_t light_level:3;	    	//light level 000=full off, 111=full on
    uint16_t trigger_count;
};

#define UNLOCK 0x0
#define LOCK 0x1
struct LockControlUplink
{
    uint8_t address:5;              //lock control unit addr 0 to 31 = all devices (see "ALL_DEVICES" above)
    uint8_t lock_cmd:3;	            //lock 0 = unlocked, 1= locked
    uint16_t trigger_count;
};

struct Uplink_Pkts
{
	enum UL_PKT_TYPE ulink_pkt_type;
	union UplinkPkt
	{
		struct KeysUplink key_value;
		struct ThermostatUplink thermostat;
        struct GenericUplink generic_uplink;
        struct StreamAckUplink stream_ack_uplink;
		struct PassThroughUplink pass_through_uplink;
        struct LightingControlUplink light_control_uplink;
        struct LockControlUplink lock_control_uplink;
        struct CmdUplink cmd;
	}uplink_pkts;
};
#endif
