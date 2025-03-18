/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLEKey NFC Application Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-02-28, Create file. \n
 */
#include "nfc_app.h"
#include "platform_core.h"
#include "pinctrl_porting.h"
#include "hal_gpio.h"
#include "gpio.h"
#include "errcode.h"
#include "spi.h"
#include "common_def.h"
#include "soc_osal.h"
#include "systick.h"
#include "watchdog.h"
#include "watchdog_porting.h"
#include "osal_timer.h"
#include "osal_debug.h"
#include "pinctrl.h"
#include "non_os_utils.h"
#include "stddef.h"
#include "test_suite_uart.h"
#include "chip_io.h"
#include "pm_clock.h"

/*spi config*/

#define SPI_PIN_MISO_PINMUX     HAL_PIO_SPI2_RXD
#define SPI_PIN_MOSI_PINMUX     HAL_PIO_SPI2_TXD
#define SPI_PIN_CLK_PINMUX      HAL_PIO_SPI2_CLK
//#define SPI_PIN_CS_PINMUX       HAL_PIO_SPI2_CS0
#define SPI_PIN_CS_PINMUX       HAL_PIO_FUNC_GPIO
#define SPI_PIN_MISO        S_MGPIO12
#define SPI_PIN_MOSI        S_MGPIO11
#define SPI_PIN_CLK         S_MGPIO14
#define SPI_PIN_CS          S_MGPIO13
#define NFC_IRQ_PIN            S_MGPIO10  //irq
#define NFC_CHIRP_POWER_EN    S_MGPIO15

#define NFC_SPI SPI_BUS_2
#define BUS_CLOCK 32000000 /* 32M */
#define SPI_FREQUENCY 4

/* nfc task resource */
#define NFC_TASK_PRI 21
#define NFC_TASK_STACK_SIZE 2*1024
osal_task *nfc_task_infor;
static osal_event g_nfc_event_id;

#define NFC_IRQ_TASK_PRI 20
#define NFC_IRQ_TASK_STACK_SIZE 1024
osal_task *nfc_irq_task_infor;
static osal_event g_nfc_irq_event_id;

static nfc_cb gs_nfc;
static uint32_t nfc_irq_status = 0;

//static uint8_t m_con_NfcCardAid[19] = {0x00, 0xA4, 0x04, 0x00, 0x0D, 0xA0, 0x00, 0x00, 0x08, 0x68, 0x49, 0x43, 0x43, 0x45, 0x44, 0x4B, 0x76, 0x31, 0x00};
static uint8_t m_con_NfcCardAid[19] = {0x00, 0xA4, 0x04, 0x00, 0x0d, 0xA0, 0x00, 0x00, 0x08, 0x68, 0x49, 0x43, 0x43, 0x45, 0x44, 0x4B, 0x76, 0x32, 0x00};
//static uint8_t m_con_NfcCardAid[19] = {0x00, 0xA4, 0x04, 0x00, 0x0d, 0xA0, 0x00, 0x00, 0x08, 0x68, 0x49, 0x43, 0x43, 0x45, 0x44, 0x4B, 0x76, 0x32, 0x00};

#define MAX_APDU_TRADE_TIME_OUT (1500)
#define MAX_SIZE_RX_BUFFER (1500)

#define NUMBER_AID_INFOR (1)
static SelectAidInfor m_AidInfor[NUMBER_AID_INFOR] =
{
        {m_con_NfcCardAid, 19, 0x02, NFC_CARD_OPTION_CARD_ONLINE_WITH_ICCE_APPLET}
};

static void nfc_Osal_EventWait(osal_event *EventHandler, int *Event);
static void nfc_Osal_EventPush(osal_event *EventHandler, unsigned int Event);


static void nfc_cop_refresh(void);
void non_os_enter_critical(void);
void non_os_exit_critical(void);

static unsigned int NfcTransceiveBlocking(uint8_t *txBuf, uint16_t txBufSize);

nfc_source_param gs_nfc_param =
{
        .TradeRecord = 0,
        .NfcKeyStatus = NFC_KEY_CLEAR,
        .ResetCmd = OWNER_PAIR_RESET_CMD_CLOSE,
        .State = 0,
        .nfc_sleep_Msg = 0,
        .ApduTradeTime = 0,
        .length = 0,
        .MessageType = 0,
        .pCop_Refresh = nfc_cop_refresh,
        .selDevIndex = 0,
        .activate_retry_num = 0,
        .transac_retry_num = 0,
        .removal_timeout_flag = 0,
        .readyReleaseFlag = false,
        .readySelectOkFlag = false,
        .relaseDelayCnt = 0,
        .nfc_dalay_sleep_start_flg = false,
        .nfc_allow_sleep_time = 0,
        .nfc_rsp_frame_type = NFC_RSP_SINGLE_FRAME,
        .nfc_rsp_array_pos = 0
};

static void nfc_cop_refresh(void)
{
    //COP_Refresh(SIM);
    return;
}

static void nfc_Osal_EventWait(osal_event *EventHandler, int *Event)
{

    *Event = osal_event_read(EventHandler, NFC_EVENT_MASK, OSAL_WAIT_FOREVER, OSAL_WAITMODE_OR);
    if(*Event != OSAL_FAILURE)
    {
        osal_event_clear(EventHandler, NFC_EVENT_MASK);
    }
}


static void nfc_Osal_EventPush(osal_event *EventHandler, unsigned int Event)
{
    osal_event_write(EventHandler, Event);
}


void nfc_IRQ_Handler(pin_t pin, uintptr_t param)
{
    unused(param);
    if(pin == NFC_IRQ_PIN)
    {
        //test_suite_uart_sendf("event\r\n");
        nfc_Osal_EventPush(&g_nfc_irq_event_id, E_NFCIRQ_EVT_IRQ);
    }
}

static void nfc_wdog_config(void)
{
    uapi_watchdog_init(CHIP_WDT_TIMEOUT_2S);
    uapi_watchdog_enable(WDT_MODE_RESET);
    return;
}

static void nfc_power_io_init(void)
{
    #define NFC_POWER_EN_VALUE    (1)
    uapi_pin_init();
    uapi_gpio_init();

    uapi_pin_set_mode(NFC_CHIRP_POWER_EN, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(NFC_CHIRP_POWER_EN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(NFC_CHIRP_POWER_EN, NFC_POWER_EN_VALUE);    //3920b power on
    uapi_systick_delay_ms(20); //delay 20ms 3920B power on
    test_suite_uart_sendf("nfc en io pin: %d \r\n", uapi_gpio_get_val(NFC_CHIRP_POWER_EN));
}

static void nfc_spi_io_init(void)
{
    uapi_pin_set_mode(SPI_PIN_CS, SPI_PIN_CS_PINMUX);           /* cs */
    uapi_gpio_set_dir(SPI_PIN_CS, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(SPI_PIN_CS, GPIO_LEVEL_HIGH);

    uapi_pin_set_mode(SPI_PIN_MOSI, SPI_PIN_MOSI_PINMUX);     /* mosi */
    uapi_pin_set_mode(SPI_PIN_MISO, SPI_PIN_MISO_PINMUX);     /* miso */
    uapi_pin_set_mode(SPI_PIN_CLK, SPI_PIN_CLK_PINMUX);         /* clk */
}

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
static void app_spi_master_write_int_handler(const void *buffer, uint32_t length)
{
    unused(buffer);
    unused(length);
    osal_printk("spi master write interrupt start!\r\n");
}

static void app_spi_master_rx_callback(const void *buffer, uint32_t length, bool error)
{
    if (buffer == NULL || length == 0) {
        osal_printk("spi master transfer illegal data!\r\n");
        return;
    }
    if (error) {
        osal_printk("app_spi_master_read_int error!\r\n");
        return;
    }

    uint8_t *buff = (uint8_t *)buffer;
    for (uint32_t i = 0; i < length; i++) {
        osal_printk("buff[%d] = %x\r\n", i, buff[i]);
    }
    osal_printk("app_spi_master_read_int success!\r\n");
}
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
#include "pm_clock.h"
errcode_t nfc_spi_init(void)
{
    nfc_spi_io_init();
    spi_attr_t config = { 0 };
    spi_extra_attr_t ext_config = { 0 };


    ext_config.sspi_param.wait_cycles = 0x10;

    config.is_slave = false;
    config.slave_num = (uint32_t)1;

    config.bus_clk = BUS_CLOCK;
    config.freq_mhz = SPI_FREQUENCY;

    config.clk_polarity = SPI_CFG_CLK_CPOL_0;
    config.clk_phase = SPI_CFG_CLK_CPHA_1;

    config.frame_format = SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;

    config.frame_size = HAL_SPI_FRAME_SIZE_8;
    //config.frame_size = HAL_SPI_FRAME_SIZE_32;
    config.tmod = HAL_SPI_TRANS_MODE_TXRX;
    config.sste = 1;

#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    reg16_setbit(0x520003e0, 4);
    uapi_dma_init();
    uapi_dma_open();
#endif  /* CONFIG_SPI_SUPPORT_DMA */
    errcode_t err = uapi_spi_init(NFC_SPI, &config, &ext_config);
    test_suite_uart_sendf("spi init: %04x  %04x %04x  %04x spi clk: %04x\r\n",readw(0x5700047c),readw(0x57000480), readw(0x57000484),readw(0x57000488), readw(0x5200055c));

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    if (uapi_spi_set_irq_mode(CONFIG_SPI_MASTER_BUS_ID, true, app_spi_master_rx_callback,
        app_spi_master_write_int_handler) == ERRCODE_SUCC) {
        osal_printk("spi%d master set irq mode succ!\r\n", CONFIG_SPI_MASTER_BUS_ID);
    }
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */

    return err;
}


void nfc_irq_init(void)
{
    uapi_pin_init();
    uapi_gpio_init();

    uapi_pin_set_mode(NFC_IRQ_PIN, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(NFC_IRQ_PIN, GPIO_DIRECTION_INPUT);
    uapi_pin_set_pull(NFC_IRQ_PIN, PIN_PULL_NONE);
    uapi_pin_set_ie(NFC_IRQ_PIN, PIN_IE_1);

    if (uapi_gpio_register_isr_func(NFC_IRQ_PIN, GPIO_INTERRUPT_RISING_EDGE, nfc_IRQ_Handler) != ERRCODE_SUCC)
    {
        uapi_gpio_unregister_isr_func(NFC_IRQ_PIN);
        test_suite_uart_sendf("nfc irq init fail\r\n");

    }
    else
    {
        uapi_gpio_enable_interrupt(NFC_IRQ_PIN);
        test_suite_uart_sendf("nfc irq init ok\r\n");
    }
}

static void nfc_app_timer_handler(void)
{
    static unsigned short ts_count = 0;
    // static unsigned int test_cnt = 0;

    // test_cnt++;
    // if(test_cnt % 1000 == 0)
    // {
    //     test_suite_uart_sendf("nfc irq : %d \r\n", readw(0x57000478));
    // }

    if(((uapi_systick_get_ms() - gs_nfc_param.ApduTradeTime) > MAX_APDU_TRADE_TIME_OUT) && (NFC_CARD_TRADE_IN_PROGRESS == gs_nfc_param.TradeRecord))
    {
        //LOG_WRN("data exchange time out \r\n");
        gs_nfc_param.TradeRecord = NFC_CARD_TRADE_END;
        nfc_work_mode_switch(NFC_WORK_DEV_REMOVAL_DETECT_MODE);
    }

    if(true == gs_nfc_param.readySelectOkFlag)
    {
        ts_count++;
        if(2000 <= ts_count) //4s
        {
            ts_count = 0;
            if((NFC_CARD_TRADE_IN_PROGRESS == gs_nfc_param.TradeRecord) || (NFC_CARD_TRADE_END == gs_nfc_param.TradeRecord))
            {
                test_suite_uart_sendf("wake up by nfc 4s \r\n");
                //api_power_wakeupByNFC(1);
            }
        }

    }

    if(true == gs_nfc_param.readyReleaseFlag)
    {
        gs_nfc_param.relaseDelayCnt++;
        if(5000 <= gs_nfc_param.relaseDelayCnt) /* 10s */
        {
            test_suite_uart_sendf("nfc release can \r\n");
            gs_nfc_param.relaseDelayCnt = 0;
            gs_nfc_param.readyReleaseFlag = false;
            //api_power_wakeupByNFC(0);
        }
    }

    if(true == gs_nfc_param.nfc_dalay_sleep_start_flg)
    {
        gs_nfc_param.nfc_allow_sleep_time++;
        if(2000 < gs_nfc_param.nfc_allow_sleep_time)
        {
            gs_nfc_param.nfc_dalay_sleep_start_flg = false;
            gs_nfc_param.nfc_allow_sleep_time = 0;
            gs_nfc_param.nfc_sleep_Msg = NFC_STATE_LPCD_MODE;
            test_suite_uart_sendf("nfc sleep msg : %d \r\n", gs_nfc_param.nfc_sleep_Msg);
        }
    }
}


static uint16_t apdu_rsp_length_err(unsigned char *buffer, unsigned int sw)
{
    buffer[0] = (sw & 0xff00) >> 8;
    buffer[1] = (sw & 0x00ff);
    return 2;
}

static unsigned int NfcSelectAidCmd(uint8_t *txBuf, uint16_t txBufSize)
{
    #define MAX_SIZE_RX_SELECT_AID_BUFFER (400)
    unsigned int err;
    unsigned short result = 0xff;
    unsigned char *rxData = NULL;
    uint16_t *rxLen = NULL;
    uint32_t fwt = 0xFFFFFFFFU;
    unsigned char g_NfcCardData[MAX_SIZE_RX_SELECT_AID_BUFFER] = {0};
    uint16_t g_NfcCardDataLen = 0;

    if (txBufSize > 400)
    {
        return 0;
    }

    memset(g_NfcCardData, 0x55, MAX_SIZE_RX_SELECT_AID_BUFFER);

    err = apdu_transceiveblocking(txBuf, txBufSize, &rxData, &rxLen, fwt);
    test_suite_uart_sendf("apdu err = %d len = %d\r\n",err, *rxLen);
    if ((*rxLen > 0) && (err == 0))
    {
        result = rxData[*rxLen - 2] << 8 | (rxData[*rxLen - 1]);

        if (*rxLen <= MAX_SIZE_RX_SELECT_AID_BUFFER)
        {
            memcpy(g_NfcCardData, rxData, *rxLen);
            g_NfcCardDataLen = *rxLen;
        }
        else
        {
            g_NfcCardDataLen = apdu_rsp_length_err(g_NfcCardData, NFC_DEVICE_RSP_DATA_OVERFLOW);
        }

        test_suite_uart_sendf("apdu rsp: ");
        for(unsigned short i = 0; i < *rxLen; i++)
        {
            test_suite_uart_sendf("%02x ", g_NfcCardData[i]);
        }
        test_suite_uart_sendf("\r\n");

        if(0x9000 == result)
        {

            gs_nfc_param.readyReleaseFlag = false;
            gs_nfc_param.ApduTradeTime = uapi_systick_get_ms();
            gs_nfc_param.TradeRecord = NFC_CARD_TRADE_IN_PROGRESS;
            gs_nfc_param.readySelectOkFlag = true;
            //api_power_wakeupByNFC(1);

            //if(CANNM_Api_getNMState() == NM_STATE_BUS_SLEEP)
            {
                osal_mdelay(70);
                test_suite_uart_sendf("can sleep delay 70ms \r\n");
            }
            //NfcData_Nfc2CCC_SendToCCC(g_NfcCardData, g_NfcCardDataLen);
        }
        else
        {

        }
    }
    else
    {
        g_NfcCardDataLen = apdu_rsp_length_err(g_NfcCardData, err);
        result = err;
        gs_nfc_param.TradeRecord = NFC_CARD_TRADE_END;
        nfc_work_mode_switch(NFC_WORK_LPCD_MODE);
    }
    unused(g_NfcCardDataLen);
    return result;
}

static void test_big_data_code(void)
{
    //0x80, 0xca, 0x00, 0x03, 0x16, 0x9f, 0x1e, 0x08, 0x00, 0x22, 0x02, 0x18, 0x00, 0x00, 0x00, 0x03,0x9f, 0x37, 0x08, 0x1f, 0x6b, 0xa0, 0x77, 0x41, 0x5c, 0x13, 0xe8
    unsigned char buffer[27] = {0x80, 0xca, 0x00, 0x03, 0x16, 0x9f, 0x1e, 0x08, 0x00, 0x22, 0x02, 0x18, 0x00, 0x00, 0x00, 0x03,0x9f, 0x37, 0x08, 0x1f, 0x6b, 0xa0, 0x77, 0x41, 0x5c, 0x13, 0xe8};
    uint16_t len = 27;
    NfcTransceiveBlocking(buffer, len);
    return;
}

static void Nfc_Select_Aid(void)
{
     unsigned int result = 0;
     gs_nfc_param.selDevIndex = 0;

     for(uint8_t i = 0; i < NUMBER_AID_INFOR; i++)
     {
         result = NfcSelectAidCmd(m_AidInfor[i].aid, m_AidInfor[i].len);
         osal_mdelay(2);
         if (0x9000 == result)
         {
             if(0x00 == i)
             {
                 test_suite_uart_sendf("JH card ok\r\n");
                 test_big_data_code();
             }

         }

     }

    gs_nfc_param.TradeRecord = NFC_CARD_TRADE_END;
    nfc_work_mode_switch(NFC_WORK_LPCD_MODE);
}


static unsigned int NfcTransceiveBlocking(uint8_t *txBuf, uint16_t txBufSize)
{
    #define MAX_SEND_CNT  (20)
    unsigned int err;
    unsigned short result = 0xff;

    unsigned char *rxData = NULL;
    uint16_t *rxLen = NULL;
    uint32_t fwt = 0xFFFFFFFFU;
    unsigned char NfcCardData[MAX_SIZE_RX_BUFFER] = {0};
    uint16_t tx_len = txBufSize;


    gs_nfc_param.TradeRecord = NFC_CARD_TRADE_IN_PROGRESS;
    gs_nfc_param.ApduTradeTime = uapi_systick_get_ms();

    gs_nfc_param.nfc_rsp_frame_type = NFC_RSP_SINGLE_FRAME;
    gs_nfc_param.nfc_rsp_array_pos = 0;

    memset(NfcCardData, 0x55, MAX_SIZE_RX_BUFFER);

    err = apdu_transceiveblocking(txBuf, tx_len, &rxData, &rxLen, fwt);

    if((*rxLen > 0) && (err == 0))
    {
        test_suite_uart_sendf("apdu rsp %d :", *rxLen);
        for(unsigned short i = 0; i < *rxLen; i++)
        {
            test_suite_uart_sendf("%02x ", rxData[i]);
        }
        test_suite_uart_sendf("\r\n");
    }

    return result;
}



static void NFCApp_ReleaseReadyHandle(void)
{
    if(!((NFC_CARD_TRADE_START == gs_nfc_param.TradeRecord) && (0 == gs_nfc_param.removal_timeout_flag)))
    {
        nfc_Osal_EventPush(&g_nfc_event_id, E_NFCAPP_EVT_RELEASE_CMD);
    }
    gs_nfc_param.TradeRecord = NFC_CARD_LEAVE;
    gs_nfc_param.readySelectOkFlag = false;
}

static void NFCApp_DeviceRemoveHandle(void)
{
    if(REMOVAL_TIMEOUT == gs_nfc_param.removal_timeout_flag)
    {
        test_suite_uart_sendf("remove by lpcd\r\n");
        osal_mdelay(REMOVAL_OP_CONSUME);
    }
    else
    {
        test_suite_uart_sendf("remove normally\r\n");
        osal_mdelay(REMOVAL_OP_CONSUME);
    }


    NFCApp_ReleaseReadyHandle();
    nfc_work_mode_switch(NFC_WORK_LPCD_MODE);
}

static void reset_nfc_param(void)
{
    memset(&gs_nfc_param.NFCDeviceFound, 0, sizeof(gs_nfc_param.NFCDeviceFound));
    gs_nfc_param.selDevIndex = 0;
    gs_nfc_param.activate_retry_num = 0;
    gs_nfc_param.transac_retry_num = 0;
    return ;
}


void nfc_Status_Changed(NFC_STATUS status)
{
    static uint8_t status_last = 0;
    gs_nfc_param.State = status;
    switch(status)
    {
        case NFC_STATE_LPCD_MODE:
        {
            gs_nfc_param.nfc_dalay_sleep_start_flg = true;
            gs_nfc_param.nfc_allow_sleep_time = 0;
            reset_nfc_param();
            break;
        }
        case NFC_STATE_POLL_MODE:
        {
            gs_nfc_param.nfc_sleep_Msg = status;
            gs_nfc_param.nfc_dalay_sleep_start_flg = false;
            gs_nfc_param.nfc_allow_sleep_time = 0;
            if(REMOVAL_TIMEOUT == gs_nfc_param.removal_timeout_flag && NFC_STATE_LPCD_MODE == status_last)
            {
                NFCApp_DeviceRemoveHandle();
                gs_nfc_param.removal_timeout_flag = REMOVAL_NORMAL;
            }
            else
            {
                gs_nfc_param.TradeRecord = NFC_CARD_TRADE_START;
            }
            break;
        }

        case NFC_PROTOCAL_READY:
        {
            if(gs_nfc_param.TradeRecord != NFC_CARD_TRADE_IN_PROGRESS)
            {

                nfc_Osal_EventPush(&g_nfc_event_id, E_NFCAPP_EVT_SELECT_AID);
            }
            break;
        }

        case NFC_DEVICE_REMOVED:
        {
            NFCApp_DeviceRemoveHandle();
            break;
        }
        case NFC_DEVICE_REMOVE_TIMEOUT:
        {
            gs_nfc_param.removal_timeout_flag = REMOVAL_TIMEOUT;
            break;
        }
        default:
        {
            break;
        }
    }
    status_last = status;
}

int Nfc_debug_log(const char *fmt, ...)
{

    static char s[512] = {0};
    int32_t str_len;

    va_list args;
    va_start(args, fmt);
    str_len = vsprintf_s(s, sizeof(s), fmt, args);
    va_end(args);

    if (str_len < 0) {
        return 1;
    }
    test_suite_uart_sendf(s);
    return 0;
}

#include "spi_porting.h"
static int SPI_MasterTransfer(const uint8_t *pTxBuff, uint8_t *pRxBuff, uint16_t len)
{
#if 0
    #define NFC_TRANSFER_LEN (256)
    int ret = 0x1;
    unsigned int result = 0xff;
    int m_retry = 0x0;

    uint8_t tx_data[NFC_TRANSFER_LEN] = { 0 };
    uint8_t rx_data[NFC_TRANSFER_LEN] = { 0 };

    spi_xfer_data_t nfc_spi_data = {
        .tx_buff = NULL,
        .tx_bytes = 0,
        .rx_buff = NULL,
        .rx_bytes = 0,
    };


    if((len < NFC_TRANSFER_LEN) && (len > 0) && (NULL != pTxBuff))
    {
        memcpy(tx_data, pTxBuff, (unsigned int)len);
    }
    // else
    // {
    //     //test_suite_uart_sendf("no tx\r\n");
    // }

    nfc_spi_data.tx_buff = tx_data;
    nfc_spi_data.tx_bytes = (uint32_t)len;
    //nfc_spi_data.tx_bytes = (uint32_t) ((len%4) == 0)?(len):((len/4) + 1)*4;
    nfc_spi_data.rx_buff = pRxBuff;
    nfc_spi_data.rx_bytes = (uint32_t)len;
    //nfc_spi_data.rx_bytes = (uint32_t) ((len%4) == 0)?(len):((len/4) + 1)*4;
    //test_suite_uart_sendf("nfc data1 : %p-%p %02x len = %d \r\n", pTxBuff, pRxBuff, nfc_spi_data.tx_buff[0], nfc_spi_data.tx_bytes);
    //test_suite_uart_sendf("tx : %p %d rx:%p %0d \r\n", pTxBuff, nfc_spi_data.tx_bytes, pRxBuff, nfc_spi_data.rx_bytes);
    do
    {
        if(nfc_spi_data.tx_buff == NULL)
        {
            //read
            // test_suite_uart_sendf("rx data: %02x \r\n",  nfc_spi_data.rx_buff[0]);
            spi_porting_set_rx_mode(NFC_SPI, 1);
            // hal_opi_set_trans_mode(NFC_SPI, HAL_SPI_TRANS_MODE_RX);
            result = uapi_spi_master_read(NFC_SPI, &nfc_spi_data, 400);
        }
        else if(nfc_spi_data.rx_buff == NULL)
        {
            //write
            // test_suite_uart_sendf("tx data: %02x \r\n",  nfc_spi_data.tx_buff[0]);
            spi_porting_set_tx_mode(NFC_SPI);
            // hal_opi_set_trans_mode(NFC_SPI, HAL_SPI_TRANS_MODE_TX);
            result = uapi_spi_master_write(NFC_SPI, &nfc_spi_data, 400);
        }
        else
        {
            //write read
            test_suite_uart_sendf("tx data: %02x rx data: %02x \r\n",  nfc_spi_data.tx_buff[0], nfc_spi_data.rx_buff[0]);
            spi_porting_set_txrx_mode(NFC_SPI);
            //hal_opi_set_trans_mode(NFC_SPI, HAL_SPI_TRANS_MODE_RX);
            result = uapi_spi_master_writeread(NFC_SPI, &nfc_spi_data, 400);
        }


        if(result == ERRCODE_SUCC)
        {

            if((pRxBuff != NULL) && (nfc_spi_data.rx_bytes < NFC_TRANSFER_LEN) && (nfc_spi_data.rx_bytes > 0))
            {
                memcpy(pRxBuff, nfc_spi_data.rx_buff, nfc_spi_data.rx_bytes);
                ret = 0;
            }
            else if(nfc_spi_data.rx_bytes >= NFC_TRANSFER_LEN)
            {
                test_suite_uart_sendf("rx data more than 256\r\n");
            }

            break;
        }
        else
        {
            test_suite_uart_sendf("spi fail : %08x rx_len = %d %d\r\n", result, nfc_spi_data.rx_bytes, len);
            m_retry++;
            ret = 0x1;
        }
    }while(0);
    //}while(m_retry < 1);
    return ret;
#else

    #define NFC_TRANSFER_LEN (256)
    int ret = 0x1;
    unsigned int result = 0xff;
    int m_retry = 0x0;

    uint8_t tx_data[NFC_TRANSFER_LEN] = { 0 };
    uint8_t rx_data[NFC_TRANSFER_LEN] = { 0 };

    spi_xfer_data_t nfc_spi_data = {
        .tx_buff = NULL,
        .tx_bytes = 0,
        .rx_buff = NULL,
        .rx_bytes = 0,
    };


    if((len < NFC_TRANSFER_LEN) && (len > 0) && (NULL != pTxBuff))
    {
        memcpy(tx_data, pTxBuff, (unsigned int)len);
    }
    else
    {
        //test_suite_uart_sendf("no tx\r\n");
    }

    nfc_spi_data.tx_buff = tx_data;
    nfc_spi_data.tx_bytes = (uint32_t)len;
    //nfc_spi_data.tx_bytes = (uint32_t) ((len%4) == 0)?(len):((len/4) + 1)*4;
    nfc_spi_data.rx_buff = rx_data;
    nfc_spi_data.rx_bytes = (uint32_t)len;
    //nfc_spi_data.rx_bytes = (uint32_t) ((len%4) == 0)?(len):((len/4) + 1)*4;
    //test_suite_uart_sendf("nfc data1 : %p-%p %02x len = %d \r\n", pTxBuff, pRxBuff, nfc_spi_data.tx_buff[0], nfc_spi_data.tx_bytes);

    do
    {
        if(nfc_spi_data.tx_buff == NULL)
        {
            //read
            // test_suite_uart_sendf("rx data: %02x \r\n",  nfc_spi_data.rx_buff[0]);
            // spi_porting_set_rx_mode(NFC_SPI, 1);
            uapi_spi_set_tmod(NFC_SPI, HAL_SPI_TRANS_MODE_RX, nfc_spi_data.rx_bytes);
            // hal_opi_set_trans_mode(NFC_SPI, HAL_SPI_TRANS_MODE_RX);
            result = uapi_spi_master_read(NFC_SPI, &nfc_spi_data, 400);
        }
        else if(nfc_spi_data.rx_buff == NULL)
        {
            //write
            // test_suite_uart_sendf("tx data: %02x \r\n",  nfc_spi_data.tx_buff[0]);
            // spi_porting_set_tx_mode(NFC_SPI);
            uapi_spi_set_tmod(NFC_SPI, HAL_SPI_TRANS_MODE_TX, 0);
            // hal_opi_set_trans_mode(NFC_SPI, HAL_SPI_TRANS_MODE_TX);
            result = uapi_spi_master_write(NFC_SPI, &nfc_spi_data, 400);
        }
        else
        {
            //write read
            // test_suite_uart_sendf("tx data: %02x rx data: %02x \r\n",  nfc_spi_data.tx_buff[0], nfc_spi_data.rx_buff[0]);
            // spi_porting_set_txrx_mode(NFC_SPI);
            uapi_spi_set_tmod(NFC_SPI, HAL_SPI_TRANS_MODE_TXRX, 0);
            //hal_opi_set_trans_mode(NFC_SPI, HAL_SPI_TRANS_MODE_RX);
            result = uapi_spi_master_writeread(NFC_SPI, &nfc_spi_data, 400);
        }


        if(result == ERRCODE_SUCC)
        {

            if((pRxBuff != NULL) && (nfc_spi_data.rx_bytes < NFC_TRANSFER_LEN) && (nfc_spi_data.rx_bytes > 0))
            {
                memcpy(pRxBuff, nfc_spi_data.rx_buff, nfc_spi_data.rx_bytes);
                ret = 0;
            }
            else if(nfc_spi_data.rx_bytes >= NFC_TRANSFER_LEN)
            {
                test_suite_uart_sendf("rx data more than 256\r\n");
            }

            break;
        }
        else
        {
            test_suite_uart_sendf("spi fail : %08x rx_len = %d %d\r\n", result, nfc_spi_data.rx_bytes, len);
            m_retry++;
            ret = 0x1;
        }
    }while(0);
    //}while(m_retry < 1);

    return ret;
#endif
}

void bs21_platformGpioClear(void)
{
    uapi_gpio_set_val(SPI_PIN_CS, GPIO_LEVEL_LOW);
}

void bs21_platformGpioset(void)
{
    uapi_gpio_set_val(SPI_PIN_CS, GPIO_LEVEL_HIGH);
}

unsigned int bs21_NFCplatformGetSysTick(void)
{
    return (unsigned int)uapi_systick_get_ms();
}

void bs21_NFCplatformDelay(unsigned int ms)
{
    uapi_systick_delay_ms(ms);
    return ;
}

osal_mutex g_spi_mutex;
void bs21_NFCplatformProtectST25RIrqStatus(void)
{
    unused(nfc_irq_status);
    osal_mutex_lock(&g_spi_mutex);
    // nfc_irq_status = spi_porting_lock(SPI_PIN_CS);
    return ;
}

void bs21_NFCplatformUnprotectST25RIrqStatus(void)
{
    osal_mutex_unlock(&g_spi_mutex);
    // spi_porting_unlock(SPI_PIN_CS, nfc_irq_status);
    return ;
}

unsigned char bs21_platformIrqIsHigh(void)
{
    if(uapi_gpio_get_val(NFC_IRQ_PIN) == GPIO_LEVEL_HIGH)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void get_all_found_device(NFC_DEVICE_FOUND t_device_found)
{
    uint8_t i = 0;
    test_suite_uart_sendf("nfc found %d device.\r\n", t_device_found.totalNum);
    for(i=0; i<t_device_found.totalNum; i++)
    {
        test_suite_uart_sendf("No. %d nfcid1 is: %02x %02x %02x %02x.\r\n", i, t_device_found.device_id_list[i].nfcid[0], t_device_found.device_id_list[i].nfcid[1], t_device_found.device_id_list[i].nfcid[2], t_device_found.device_id_list[i].nfcid[3]);
    }
    gs_nfc_param.NFCDeviceFound = t_device_found;
    //memcpy(&gs_nfc_param.NFCDeviceFound, &t_device_found, siezeof(t_device_found));
}

void Nfc_Feed_Wdog(void)
{
    uapi_watchdog_kick();
}

static unsigned char nfc_protocal_adapter_init(void)
{
    unsigned char t_ret = 0xff;
    gs_nfc.LpcdConfig.AmpEnabled = 1;
    gs_nfc.LpcdConfig.Amp_Delta = 2;
    gs_nfc.LpcdConfig.PhaEnabled = 1;
    gs_nfc.LpcdConfig.Pha_Delta = 2;

    gs_nfc.NFCStatusChanged = nfc_Status_Changed;
    gs_nfc.NFCdebug_log = Nfc_debug_log;
    gs_nfc.NFCplatformSpiTxRx = SPI_MasterTransfer;
    gs_nfc.NFCplatformSpiSelect = bs21_platformGpioClear;
    gs_nfc.NFCplatformSpiDeselect = bs21_platformGpioset;
    gs_nfc.NFCplatformGetSysTick = bs21_NFCplatformGetSysTick;
    gs_nfc.NFCplatformDelay = bs21_NFCplatformDelay;
    gs_nfc.NFCplatformProtectST25RIrqStatus = bs21_NFCplatformProtectST25RIrqStatus;
    gs_nfc.NFCplatformUnprotectST25RIrqStatus = bs21_NFCplatformUnprotectST25RIrqStatus;
    gs_nfc.NFCplatformIrqIsHigh = bs21_platformIrqIsHigh;
    gs_nfc.NFCGetAllFoundDevice = get_all_found_device;
    gs_nfc.NFCFeedWdog = Nfc_Feed_Wdog;

    t_ret = nfc_Init(&gs_nfc);
    return t_ret;
}

osal_timer g_nfc_timer;
void nfc_timer_cycle_2ms(unsigned long arg)
{
	unused(arg);
    osal_timer_start(&g_nfc_timer);
    nfc_Osal_EventPush(&g_nfc_event_id,E_NFCAPP_EVT_TICKS_2MS);
}

static void nfc_timer_init(void)
{
    uint32_t ret = 0xff;
    g_nfc_timer.timer = NULL;
    g_nfc_timer.data = 0;
    g_nfc_timer.handler = nfc_timer_cycle_2ms;
    g_nfc_timer.interval = 2;
    ret = osal_timer_init(&g_nfc_timer);
    if (ret != OSAL_SUCCESS)
    {
        test_suite_uart_sendf("nfc timer create failed!\n");
    }
    else
    {
        test_suite_uart_sendf("nfc timer success\n");
    }

}

void nfc_event_queue(void)
{
    if(OSAL_SUCCESS == osal_event_init(&g_nfc_event_id))
    {
        test_suite_uart_sendf("nfc event ok!\n");
    }
    else
    {
        if(OSAL_SUCCESS == osal_event_init(&g_nfc_event_id))
        {
            test_suite_uart_sendf("nfc event ok!\n");
        }
        else
        {
            test_suite_uart_sendf("nfc event fail!\n");
        }
    }
}

void nfc_task(void *argument)
{
	unused(argument);
    unused(nfc_wdog_config);
    nfc_event_queue();
    nfc_power_io_init();
    nfc_timer_init();
    nfc_spi_init();        //hardware init
    osal_mutex_init(&g_spi_mutex);
    if(nfc_protocal_adapter_init())   //nfc protocal init
    {
        nfc_irq_init();
        test_suite_uart_sendf("NFC inited ok (verison = %s)!\r\n", get_nfc_lib_version());
        osal_timer_start(&g_nfc_timer);
    }
    else
    {
        test_suite_uart_sendf("NFC init failed (verison = %s)!\r\n", get_nfc_lib_version());
    }

    while(1)
    {

        int t_event = 0;
        nfc_Osal_EventWait(&g_nfc_event_id, &t_event);

        if(t_event & E_NFCAPP_EVT_TICKS_2MS)
        {
            nfc_2ms_cycle_handler();
            nfc_app_timer_handler();
        }

        if(t_event & E_NFCAPP_EVT_SELECT_AID)
        {
            test_suite_uart_sendf("NFC wake\r\n");
            Nfc_Select_Aid();
        }

        if(t_event & E_NFCAPP_EVT_SEND_APDU)
        {

        }

        if(t_event & E_NFCAPP_EVT_RELEASE_CMD)
        {
            gs_nfc_param.readyReleaseFlag = true;
            gs_nfc_param.relaseDelayCnt = 0;
        }
    }

}

void nfc_irq_event_queue(void)
{
    if(OSAL_SUCCESS == osal_event_init(&g_nfc_irq_event_id))
    {
        test_suite_uart_sendf("nfc irq event ok!\n");
    }
    else
    {
        if(OSAL_SUCCESS == osal_event_init(&g_nfc_irq_event_id))
        {
            test_suite_uart_sendf("nfc irq event ok!\n");
        }
        else
        {
            test_suite_uart_sendf("nfc irq event fail!\n");
        }
    }
}

void nfc_irq_task(void* argument)
{
    unused(argument);
    nfc_irq_event_queue();
    test_suite_uart_sendf("nfc irq task ok\n");
    while(1)
    {
        int t_event = 0;
        nfc_Osal_EventWait(&g_nfc_irq_event_id, &t_event);

        if(t_event & E_NFCIRQ_EVT_IRQ)
        {
            //test_suite_uart_sendf("coming\r\n");
            chip_3920B_Irq_Status();
        }
      }
}


