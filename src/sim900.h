#ifndef SIM900_H
#define SIM900_H

#include "stdint.h"

//https://vsblogs.wordpress.com/2013/11/28/tcp-connection-over-gprs-using-sim900-and-at-commands

//+CMTI: "SM",1 - Unsolicited notification of the SMS arriving 

//AT+CBAND?  GET AND SET MOBILE OPERATION BAND
//AT+CSPN?   GET SERVICE PROVIDER NAME FROM SIM 

//AT+CGATT? /**Check if GPRS is attached or not**/
//AT+CIPSHUT /**Reset the IP session if any**/
//AT+CIPSTATUS /**Check if the IP stack is initialized**/
//AT+CIPMUX=0 
//AT+CSTT="internet.mts.ru", "mts", "mts"
//AT+CIICR /**Now bring up the wireless. Please note, the response to this might take some time**/
//AT+CIFSR /**Get the local IP address. Some people say that this step is not required, but if I do not issue this, it was not working for my case. So I made this mandatory, no harm.**/

//+RECEIVE,1,53: - if AT+CIPMUX = 1
// validate email http://www.ex-parrot.com/~pdw/Mail-RFC822-Address.html


typedef enum
{
    SIM300_OK = 0x1,
    SIM300_ERROR,
    SIM300_SHUT_OK,
    SIM300_ERRFRAME,
    SIM300_SEND_OK,
    SIM300_SEND_FAIL,
    SIM300_CLOSE_OK,
}sim_status_t;

/**
  *READY      MT is not pending for any password 
  *SIM PIN     MT is waiting SIM PIN to be given 
  *SIM PUK    MT is waiting for SIM PUK to be given 
  *PH_SIM PIN    ME is waiting for phone to SIM card (antitheft) 
  *PH_SIM PUK  ME is waiting for SIM PUK (antitheft) 
  *SIM PIN2  PIN2, e.g. for editing the FDN book possible only if preceding Command was acknowledged with +CME ERROR:17 
  *SIM PUK2  possible only if preceding Command was acknowledged with error +CME ERROR: 18. 
*/

typedef enum
{
    SIM_PIN_UNKNOW = -1,
    PIN_READY = 0,
    SIM_PIN,
    SIM_PUK,
    PH_SIM_PIN,
    PH_SIM_PUK,
    SIM_PIN2,
    SIM_PUK2,
}sim_pin_status_t;

/**
 * 0 Not registered, MT is not currently searching a new operator to register to 
 * 1 Registered, home network 
 * 2 Not registered, but MT is currently searching a new operator to register to 
 * 3 Registration denied  
 * 4 Unknown 
 * 5 Registered,roaming
 */
typedef enum
{
    NET_ERR = -1,
    NET_NOT_REGISTERED_NO_SEARCHING = 0,
    NET_REGISTERED_HOME_NETWORK,
    NET_NOT_REGISTERED_SEARCHING,
    NET_REGISTRATION_DENIED,
    NET_UNKNOWN,
    NET_REGISTERED_ROAMING,
}sim_reg_network_t;
/*
0   IP INITIAL 
1   IP START 
2   IP CONFIG 
3   IP GPRSACT 
4   IP STATUS 
5   TCP CONNECTING/UDP CONNECTING/SERVER LISTENING 
6   CONNECT OK 
7   TCP CLOSING/UDP CLOSING 
8   TCP CLOSED/UDP CLOSED 
9   PDP DEACT */
typedef enum
{
    CIP_IP_INITIAL = 0x0,
    CIP_IP_START,
    CIP_IP_CONFIG,
    CIP_IP_GPRSACT,
    CIP_IP_STATUS,
    CIP_CONNECT_OK,
    CIP_CONN_LISTENING,
    CIP_CLOSING,
    CIP_CLOSED,
    CIP_PDP_DEACT,
}sim_cip_state_t;

typedef enum
{
    SIM_SMS_PDU = 0,
    SIM_SMS_TEXT = 1,
}sim_sms_mode_t;

typedef enum
{
    SIM_TCP,
    SIM_UDP,
}sim_tcp_mode_t;

typedef enum
{
    SIM_TCP_CONNECT_OK,
    SIM_TCP_ALREADY_CONNECT,
    SIM_TCP_CONNECT_FAIL,
    SIM_TCP_CLOSED,
}sim_con_status_t;

typedef enum
{
    SIM_SMS_REC_UNREAD = 0,
    SIM_SMS_REC_READ,
    SIM_SMS_STO_UNSENT,
    SIM_SMS_STO_SENT,
    SIM_SMS_ALL,
}sim_sms_status_t;

typedef enum
{
    SIM_GSM = 0,
    SIM_UCS2,
    SIM_IRA,
    SIM_HEX,
    SIM_PCCP,
    SIM_PCDN,
    SIM_8859_1,
}sim_TE_chaster_t;

typedef struct
{
    uint8_t addr0;
    uint8_t addr1;
    uint8_t addr2;
    uint8_t addr3;
}sim_ip_t;

typedef struct
{
    sim_status_t status;
    uint32_t user_data;
    void *user_pointer;
}sim_reply_t;

typedef struct
{
    sim_sms_status_t status;
    char number[13];
    char mt[17];
    char date[22];
    char *msg;
    uint16_t msg_length;
}sim_sms_t;

#ifdef __cplusplus
 extern "C" {
#endif
 
void simx_receive(uint8_t byte);
void simx_wait_reply();
uint8_t simx_is_receive();

void simx_callback_send(uint8_t *data, uint16_t length);
void simx_callback_tcp_msg(sim_con_status_t con_status, uint8_t n);
void simx_callback_tcp_data(uint8_t *data, uint16_t length, uint8_t n);
void simx_callback_sms_received(uint16_t number);
void simx_callback_pdp_deact();

void simx_test(sim_reply_t *reply);
void simx_pin_is_required(sim_reply_t *reply); // AT+CPIN?
void simx_network_registration(sim_reply_t *reply);

void simx_sim_inserted_status(sim_reply_t *reply);

void simx_signal_quality_report(sim_reply_t *reply, uint8_t *lvl);

void simx_set_TE_character(sim_reply_t *reply, sim_TE_chaster_t chaster);

/********GPRS************/
void simx_is_attach_to_GPRS(sim_reply_t *reply);
void simx_deactivate_gprs_pdp(sim_reply_t *reply);
void simx_set_gprs_config(sim_reply_t *reply, const char* apn, const char* username, const char* pass);
void simx_bring_up_wireless_connection(sim_reply_t *reply);
void simx_get_local_ip(sim_reply_t *reply, sim_ip_t *ip);

/**********TCP IP***************/
void simx_current_connection_status(sim_reply_t *reply);
void simx_multiple_connection(sim_reply_t *reply, uint8_t mode);
void simx_tcp_connect(sim_reply_t *reply, sim_tcp_mode_t tcp_mode, char *address, uint16_t port, int n);
void simx_tcp_send_data(sim_reply_t *reply, uint8_t *data, uint16_t length, int n);
void simx_tcp_close(sim_reply_t *reply, int n);
void simx_tcp_head_enable(sim_reply_t *reply, uint8_t is_enable);

/********SMS************/
void simx_sms_mode(sim_reply_t *reply, sim_sms_mode_t mode);
void simx_send_sms(sim_reply_t *reply, const char* number, const char* msg);
void simx_read_sms(sim_reply_t *reply, sim_sms_t *sim_sms, uint16_t n);
//void simx_list_sms(sim_reply_t *reply, );

sim_pin_status_t sim_pin_required();
sim_reg_network_t sim_network_registration();
int8_t sim_is_inserted();
int8_t gprs_is_attach();
sim_cip_state_t current_connection_status();
uint8_t sim_cip_mux_mode();
#ifdef __cplusplus
}
#endif

#endif
