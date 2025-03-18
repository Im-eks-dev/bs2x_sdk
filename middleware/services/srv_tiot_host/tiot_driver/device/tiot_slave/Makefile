# Tiot Slave Makefile
ifeq ($(CONFIG_DEV_TIOT_SLAVE), y)
tiot-slave-objs += tiot_slave_device_info.o tiot_slave_service.o

tiot-slave-objs := $(addprefix device/tiot_slave/,$(tiot-slave-objs))
tiot_driver-objs += $(tiot-slave-objs)

TIOT_HEADER_LIST += -I$(TIOT_DRIVER_PATH)/device/tiot_slave
endif