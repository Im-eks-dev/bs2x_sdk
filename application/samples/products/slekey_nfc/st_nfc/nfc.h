/*
 * nfc_adapter_config.h
 *
 *  Created on: 2023年8月17日
 *      Author: pengxuecheng
 */

#ifndef __NFC_H__
#define __NFC_H__

//#include "platform.h"

#define NFC_DEVICES_FOUND_MAX          2U    /* Max number of devices supported */

typedef struct
{
	unsigned char R0;
	unsigned char M0;
	unsigned char V0;
}REG_BASE, *p_REG_BASE;

typedef enum
{
	//nfc protocal status
	NFC_STATE_LPCD_MODE				= 3,
	NFC_STATE_POLL_MODE 			= 10,
	NFC_STATE_POLL_SELECT			= 12,
	NFC_STATE_POLL_ACTIVATION		= 13,
	NFC_STATE_ACTIVATED             =  30,
	NFC_STATE_DEACTIVATION          =  34,
	//nfc event msg
	NFC_NFC_OTHER_TYPE				=  	40, /* Msg								 */
	NFC_PROTOCAL_READY				=  	41, /* apdu exchange					 */
	NFC_ACTIVATED_FAIL				=  	42,
	NFC_PROTOCAL_TIMEOUT			=  	43,
	NFC_DEVICE_REMOVED				=  	44, /* nfc device has removed			 */
	NFC_DEVICE_REMOVE_TIMEOUT		=  	45, /* nfc device removal detect timeout */
}NFC_STATUS;


typedef enum
{
	NFC_WORK_POLL_MODE,
	NFC_WORK_LPCD_MODE,
	NFC_WORK_POLL_RESET_MODE,
	NFC_WORK_DEV_REMOVAL_DETECT_MODE,
}NFC_WORK_MODE;

typedef struct{
	unsigned char               nfcidLen;         /*!< Device's NFCID length        */
	unsigned char               nfcid[10];        /*!< Device's NFCID               */
}NFC_DEVICE_ID;


typedef struct{
	unsigned char				totalNum;
	NFC_DEVICE_ID				device_id_list[NFC_DEVICES_FOUND_MAX];
}NFC_DEVICE_FOUND;

typedef struct
{
	 unsigned char AmpEnabled;
	 unsigned char Amp_Delta;
	 unsigned char PhaEnabled;
	 unsigned char Pha_Delta;
}ANTENNA_PARAM;


typedef struct
{
	ANTENNA_PARAM LpcdConfig;
	void (*NFCStatusChanged)(NFC_STATUS staus); //card status 根据业务适配哪些协议栈状态需要
	int (*NFCdebug_log)(const char *fmt_s, ...);
	int (*NFCplatformSpiTxRx)(const unsigned char *ptxbuff, unsigned char *prxbuff, unsigned short len);
	void (*NFCplatformSpiSelect)(void);
	void (*NFCplatformSpiDeselect)(void);
	unsigned int (*NFCplatformGetSysTick)(void);
	void (*NFCplatformDelay)(unsigned int ms);
	void (*NFCplatformProtectST25RIrqStatus)(void);
	void (*NFCplatformUnprotectST25RIrqStatus)(void);
	unsigned char (*NFCplatformIrqIsHigh)(void);
	void (*NFCGetAllFoundDevice)(NFC_DEVICE_FOUND t_device_found);
	void (*NFCFeedWdog)(void);
}nfc_cb, *p_nfc_cb;


unsigned char nfc_Init(p_nfc_cb t_param);

void nfc_2ms_cycle_handler(void); //2ms
void set_ecp_value(unsigned char *t_ecp, unsigned short t_len_ecp);//设置ecp
unsigned short apdu_transceiveblocking( unsigned char *txBuf, unsigned short txBufSize, unsigned char **rxData, unsigned short **rcvLen, unsigned int fwt);
unsigned char nfc_work_mode_switch(NFC_WORK_MODE t_WorkMode);//工作模式
void nfc_close_raf_dur_time_set(unsigned short t_time_ms); //设置reset dur时间 ms
void nfc_chip_param_reg_config(p_REG_BASE reg, unsigned char reg_index);//给出结构体
void chip_3920B_Irq_Status(void);
void nfc_select_device(unsigned char devIndex);

char *get_nfc_lib_version(void);

#endif /* MODULE_ST_NFC_NFC_CONFIG_NFC_H_ */


