# W33 Makefile
ifeq ($(CONFIG_DEV_W33), y)
tiot-w33-objs += w33_device_info.o w33_service.o

ifeq ($(CONFIG_FILE_BY_ARRAY), y)
ifneq (,$(wildcard $(TIOT_DRIVER_PATH)/device/w33/firmware/w33_fw_file_array.c))
tiot-w33-objs += firmware/w33_fw_file_array.o
TIOT_HEADER_LIST += -I$(TIOT_DRIVER_PATH)/device/w33/firmware
endif
endif

tiot-w33-objs := $(addprefix device/w33/,$(tiot-w33-objs))
tiot_driver-objs += $(tiot-w33-objs)

TIOT_HEADER_LIST += -I$(TIOT_DRIVER_PATH)/device/w33
endif