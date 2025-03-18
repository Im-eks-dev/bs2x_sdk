
	.osal_xip :
	{
	    . = ALIGN(8);
	    *libosal.a:*.o* (.text.oal_int_create .text.oal_int_delete \
	                     .text.osal_kzalloc .text.osal_kzalloc_align .text.osal_vzalloc .text.osal_kzalloc_align .text.osal_vmalloc .text.osal_vfree \
	                     .text.osal_event_init .text.osal_event_destroy \
	                     .text.osal_irq_request .text.osal_irq_free .text.osal_irq_set_priority .text.osal_irq_enable .text.osal_irq_disable \
	                     .text.osal_msg_queue_create .text.osal_msg_queue_delete \
	                     .text.osal_mutex_init .text.osal_mutex_destroy \
	                     .text.osal_sem_init .text.osal_sem_binary_sem_init .text.osal_sem_destroy \
	                     .text.osal_kthread_create .text.osal_kthread_destroy .text.osal_kthread_set_priority .text.osal_msleep \
	                     .text.osal_timer_init .text.osal_timer_destroy \
			     .text.osal_wait_init .text.osal_wait_timeout_interruptible .text.osal_wait_destroy .text.osal_wait_uninterruptible .text.osal_wait_interruptible)
	    . = ALIGN(8);
	} > FLASH_PROGRAM

	.drv_xip :
	{
	    . = ALIGN(8);
	    *i2c*.a:*.o*(.text.uapi_i2c_master_init .text.hal_i2c_*_master_init .text.hal_i2c_*_regs_init .text.hal_i2c_*_init_comp_param  .text.i2c_dma_mode_init \
	                 .text.i2c_dma_mode_init .text.uapi_i2c_slave_init .text.hal_i2c_v151_slave_init .text.i2c_port_register_irq \
	                 .text.uapi_i2c_deinit .text.hal_i2c_*_deinit .text.i2c_port_clock_enable .text.i2c_port_get_clock_value \
	                 .text.hal_i2c_v151_master_cfg_baudrate .text.hal_i2c_v151_load_ctrl_func \
	                 .text.i2c_write_pre_check .text.i2c_read_pre_check .text.i2c_fifo_check)
	    *spi*.a:*.o* (.text.uapi_spi_init .text.hal_spi_regs_init .text.hal_spi_*_init .text.uapi_spi_deinit .text.hal_spi_v151_deinit \
	                  .text.spi_port_clock_enable .text.spi_porting_set_sspi_mode .text.spi_porting_set_device_mode .text.spi_porting_base_addr_get \
	                  .text.spi_write_dma .text.uapi_spi_set_dma_mode .text.spi_dma_isr .text.spi_port_get_dma_trans_dest_handshaking \
	                  .text.spi_port_get_dma_trans_src_handshaking .text.hal_spi_ctrl_get_dma_data_addr \
	                  .text.spi_porting_clock_en .text.spi_port_tx_data_level_get .text.spi_port_rx_data_level_get)
	    *libpm_porting.a:*.o* (.text.ulp_rtc_init)
	    *libpm_sleep_porting.a:*.o* (.text.pm_wakeup_rtc_init .text.pm_port_sleep_config_int)
	    *libpm_dev.a:*.o* (.text.uapi_pm_register_dev_ops)
	    *libpm_sleep.a:*.o* (.text.uapi_pm_register_sleep_funcs)
	    *libpm_fsm.a:*.o* (.text.uapi_pm_register_fsm_handler)
	    *libpm_veto.a:*.o* (.text.uapi_pm_veto_init)
	    *libdfx_log.a:log.*.o*(.text.log_init .text.get_log_sn_number)
	    *libdfx_log.a:log_buffer.*o* (.text.log_buffer_init)
	    *libchip_porting.a:tick_timer.*o* (.rodata.HalClockStart.str1.1)
	    *libchip_porting.a:interrupt_os_adapter.*o* (.rodata.m_aucIntPri)
	    . = ALIGN(8);
	} > FLASH_PROGRAM

	.liteos_xip :
	{
	    . = ALIGN(8);
	    *libc.a:*.o* (.text.atoi .text.strtox .text.strtoull  .text.__shlim  .text.__shgetc .text.__uflow .text.__toread  .text.__errno_location \
	                  .text.memmove .text.calloc .text.malloc .text.free .text.zalloc .text.memalign .text.strncmp .text.strtol .text.strtoll .text.abs)
	    *libc.a:*intscan* (.text .text* .rodata .rodata* .srodata .srodata*)
	    /* *libsegger.a:*.o*(.text .text* .rodata .rodata* .srodata .srodata*) */
	    *libgcc.a:*.o*(.rodata .rodata* .srodata .srodata*)
	    *libbase.a:*.o* (.text.LOS_EventInit .text.LOS_EventDestroy \
	                     .text.OsHwiInit .text.OsHwiDel* .text.LOS_HwiCreate .text.LOS_HwiDelete .text.LOS_HwiSetPriority \
			     .text.LOS_HwiEnable .text.LOS_HwiDisable \
	                     .text.OsMuxInit .text.LOS_MuxCreate .text.LOS_MuxDelete \
	                     .text.OsQueueInit .text.LOS_QueueCreate .text.LOS_QueueDelete .text.LOS_QueueInfoGet \
	                     .text.OsSemInit .text.OsSemCreate .text.LOS_SemCreate .text.LOS_BinarySemCreate .text.LOS_SemDelete \
	                     .text.OsSwtmrInit .text.LOS_SwtmrStart .text.OsSwtmrStartTimer .text.OsSwtmrStart .text.LOS_SwtmrStop .text.LOS_SwtmrCreate .text.OsSwtmrDelete .text.LOS_SwtmrDelete \
	                     .text.OsTaskInit .text.OsPriQueueInit .text.OsTaskStackFree .text.LOS_TaskResRecycle \
	                     .text.OsTaskMonInit .text.LOS_CurTaskIDGet .text.OsCurTaskNameGet .text.LOS_TaskDelay .text.ErrorMsg \
	                     .text.LOS_TaskCreate* .text.OsIdleTaskCreate .text.OsTaskDelete .text.LOS_TaskDelete \
	                     .text.LOS_TaskSuspend .text.LOS_TaskResume .text.LOS_TaskInfoGet .text.OsStackWaterLineGet .text.LOS_TaskPriSet \
	                     .text.OsTickInit .text.OsSortLinkInit \
			     .text.LOS_Msleep \
	                     .text.OsMemSystemInit .text.LOS_MemInit  .text.LOS_MemPoolInit .text.OsHeapInit \
	                     .text.LOS_MemInfoGet .text.OsHeapStatisticsGet .text.OsHeapInfoDump \
	                     .text.LOS_MemTaskHeapInfoGet .text.OsHeapTaskHeapInfoGet .text.OsHeapStatAddUsed .text.OsMemstatTaskUsedInc .text.OsMemstatTaskUsedDec \
			     .text.OsMemstatTaskClear .text.OsMemTaskClear .text.OsMemTaskUsage .text.LOS_TaskMemInfoShow .text.OsMemTaskUsageShow \
	                     .text.OsSimpleStatUpdate .text.LOS_GetSimpleResStats \
			     .rodata.OsIdleTaskCreate.str1.1 .rodata.ErrorMsg.str1.1 .rodata.OsVprintf.str1.1 .rodata.OsHeapAlloc.str1.1 .rodata.OsHeapFree.str1.1 \
			     .rodata.LOS_MemFree.str1.1 .rodata.LOS_MemPoolInit.str1.1 .rodata.OsSwtmrInit.str1.1)
	     *libinterrupt.a:*.o* (.text.HalIrqVersion .rodata.HalIrqVersion.* .text.HalIrqInit .rodata.g_plicOps* .text.HalIrqSetPrio \
	                           .text.HalIrqMask .text.HalIrqUnmask)
	     *libriscv.a:*.o* ( .text.ArchExcInit  .text.ArchSetExcHook  .text.ArchSetNMIHook \
	                        .text.OsExcInfoDisplay .rodata.OsExcInfoDisplay.* .rodata.g_xRegsMap .text.ArchTaskExit .text.ArchTaskStackInit \
				.text.OsExcHandleEntry .text.OsNMIHandler .text.OsClearNMI)
	     *libriscv.a:*exception*.o* (.itcm.text .text)
	     *libriscv.a:*fault*.o* (.text.* .rodata.*)
	    . = ALIGN(8);
	} > FLASH_PROGRAM