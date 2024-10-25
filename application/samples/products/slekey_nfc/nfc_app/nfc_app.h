#ifndef __NFC_APP_H__
#define __NFC_APP_H__

#include "osal_wait.h"
#include "carkey_common.h"
#include "nfc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define _BIT(x) (1 << (x))
#define E_OS_EVT_USR_BASE           _BIT(12)
#define E_NFCAPP_EVT_SLEEP          (E_OS_EVT_USR_BASE << 0)
#define E_NFCAPP_EVT_WDOG           (E_OS_EVT_USR_BASE << 1)
#define E_NFCAPP_EVT_RELEASE_CMD    (E_OS_EVT_USR_BASE << 2)
#define E_NFCAPP_EVT_TICKS_2MS      (E_OS_EVT_USR_BASE << 3)
#define E_CANAPP_EVT_NFC_LEARN      (E_OS_EVT_USR_BASE << 4)
#define E_CANAPP_EVT_RELEASE        (E_OS_EVT_USR_BASE << 5)
#define E_NFCAPP_EVT_SEND_APDU      (E_OS_EVT_USR_BASE << 6)
#define E_NFCAPP_EVT_TICKS_2S       (E_OS_EVT_USR_BASE << 7)
#define	E_NFCAPP_EVT_SELECT_DEV     (E_OS_EVT_USR_BASE << 8)
#define E_NFCAPP_EVT_SELECT_AID     (E_OS_EVT_USR_BASE << 9)

//NFC IRQ
#define E_NFCIRQ_EVT_IRQ            (E_OS_EVT_USR_BASE << 10)
#define NFC_EVENT_MASK              (E_NFCAPP_EVT_SLEEP | E_NFCAPP_EVT_WDOG | E_NFCAPP_EVT_SELECT_AID | E_NFCAPP_EVT_SEND_APDU | E_NFCAPP_EVT_SELECT_DEV | E_NFCAPP_EVT_TICKS_2MS | E_NFCIRQ_EVT_IRQ)
#define LOG_INF

enum
{
    NFC_CARD_OPTION_UNDEFINED = 0,
    NFC_CARD_OPTION_CARD_ONLINE_BUT_NO_GAC_APPLET = 1,
    NFC_CARD_OPTION_CARD_ONLINE_WITH_OWNER_GAC_APPLET = 2,
    NFC_CARD_OPTION_CARD_ONLINE_WITH_ICCE_APPLET = 3,
    NFC_CARD_OPTION_CARD_ONLINE_WITH_CCC_APPLET = 4,
    NFC_CARD_OPTION_CARD_OFFLINE = 5,
    NFC_CARD_OPTION_CARD_TEMPERATURE_TOO_HIGH = 6,
};

typedef enum
{
	NFC_KEY_CLEAR,
	NFC_KEY_ACTIVATION,
	NFC_KEY_APDU
}NFC_KEY_STATUS;

typedef enum
{
	NFC_PROTOCAL_INIT_FAIL,
	NFC_PROTOCAL_INIT_OK
}NFC_PROTOCAL_INIT_STATUS;

typedef enum
{
	OWNER_PAIR_RESET_CMD_CLOSE,
	OWNER_PAIR_RESET_CMD_OPEN
}OWNER_PAIR_RESET;

typedef enum
{
	REMOVAL_NORMAL,
	REMOVAL_TIMEOUT
}NFC_REMOVAL_STATIS;

enum
{
	NFC_CARD_TRADE_START = 0x01,
	NFC_CARD_TRADE_IN_PROGRESS = 0x02,
	NFC_CARD_TRADE_END = 0x03,
	NFC_CARD_LEAVE = 0x04
};

typedef enum
{
	NFC_RSP_SINGLE_FRAME,
	NFC_RSP_MULTI_FRAME
}NFC_RSP_FRAME_TYPE;

enum
{
	NFC_DEVICE_RSP_DATA_OVERFLOW = 0xffff
};

typedef struct
{
    uint8_t *aid;
    uint8_t len;
    uint8_t cmdtype;
    uint8_t NFCCardOption;
} SelectAidInfor;


typedef struct
{
	uint8_t TradeRecord;				//刷卡交易流程记录  : 交易开始 ，交易中， 交易完成
	uint8_t NfcKeyStatus;				//not ready: 0x00 apdu: 0x01
	unsigned int ResetCmd;				//ccc auth reset protocal
	unsigned int State;					//协议栈状态
	unsigned int nfc_sleep_Msg;			//sleep : NFC_STATE_LPCD_MODE wakeup: NFC_STATE_POLL_MODE
	uint32_t ApduTradeTime;				//apdu 数据交易时间记录
	uint16_t length;					//数据长度
	uint8_t MessageType;				//消息类型
	void (*pCop_Refresh)(void);			//看门狗喂狗
	NFC_DEVICE_FOUND NFCDeviceFound;	/* info of device found								 */
	uint8_t selDevIndex;				/* nfc device index selected by app					 */
	uint8_t	activate_retry_num;			/* retry time when activation fail occur			 */
	uint8_t transac_retry_num;			/* retry time when transaction fail occur			 */
	uint8_t removal_timeout_flag;		/* set to "0x01" when card removal detection timeout */
	uint8_t readyReleaseFlag;			/* is ready to release the network					 */
	uint8_t readySelectOkFlag;			/* is ready to select aid ok 				 */
	uint32_t relaseDelayCnt;			/* NIO requirement: delay 10s for network release    */
	uint8_t nfc_dalay_sleep_start_flg;	/* true:start false: stop*/
	uint32_t nfc_allow_sleep_time;		/* delay 4s time notify  mcu sleep */
	NFC_RSP_FRAME_TYPE nfc_rsp_frame_type;		/* 0x00 : single frame, 0x01: multi frame*/
	uint16_t nfc_rsp_array_pos;					/* muti frame data array pos*/
}nfc_source_param;


#define REMOVAL_OP_CONSUME	(500)
#define ACTIVATE_RETRY_NUM_MAX	(2)
#define TRANSAC_RETRY_NUM_MAX	(2)


void nfc_task(void *argument);
void nfc_event_queue(void);

void nfc_irq_task(void* argument);
void nfc_irq_event_queue(void);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */