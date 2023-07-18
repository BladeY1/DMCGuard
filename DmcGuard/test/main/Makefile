#This target is to ensure accidental execution of Makefile as a bash script will not execute commands like rm in unexpected directories and exit gracefully.
.prevent_execution:
	exit 0

CC = gcc

#remove @ for no make command prints
DEBUG = @

APP_DIR = .
APP_INCLUDE_DIRS += -I $(APP_DIR)
APP_NAME = main
APP_SRC_FILES = $(APP_NAME).c

#AWS IOT MQTT client directory
AWS_IOT_DIR = ../../lib/aws-iot-device-sdk-embedded-C

PLATFORM_DIR = $(AWS_IOT_DIR)/platform/linux/mbedtls
PLATFORM_COMMON_DIR = $(AWS_IOT_DIR)/platform/linux/common

AWS_INCLUDE_DIRS += -I $(AWS_IOT_DIR)/include
AWS_INCLUDE_DIRS += -I $(AWS_IOT_DIR)/external_libs/jsmn
AWS_INCLUDE_DIRS += -I $(PLATFORM_COMMON_DIR)
AWS_INCLUDE_DIRS += -I $(PLATFORM_DIR)

AWS_SRC_FILES += $(shell find $(AWS_IOT_DIR)/src/ -name '*.c')
AWS_SRC_FILES += $(shell find $(AWS_IOT_DIR)/external_libs/jsmn -name '*.c')
AWS_SRC_FILES += $(shell find $(PLATFORM_DIR)/ -name '*.c')
AWS_SRC_FILES += $(shell find $(PLATFORM_COMMON_DIR)/ -name '*.c')

#TLS - mbedtls
MBEDTLS_DIR = $(AWS_IOT_DIR)/external_libs/mbedTLS
TLS_LIB_DIR = $(MBEDTLS_DIR)/library
CRYPTO_LIB_DIR = $(MBEDTLS_DIR)/library
TLS_INCLUDE_DIR = -I $(MBEDTLS_DIR)/include
EXTERNAL_LIBS += -L $(TLS_LIB_DIR)
LD_FLAG += -Wl,-rpath,$(TLS_LIB_DIR)
LD_FLAG += -ldl $(TLS_LIB_DIR)/libmbedtls.a $(CRYPTO_LIB_DIR)/libmbedcrypto.a $(TLS_LIB_DIR)/libmbedx509.a -lpthread

# Homekit
# HOMEKIT_INCLUDE_DIRS += -I ../../lib/HomeKitADK-master/External/JSON
# HOMEKIT_INCLUDE_DIRS += -I ../../lib/HomeKitADK-master/External/Base64
# HOMEKIT_INCLUDE_DIRS += -I ../../lib/HomeKitADK-master/External/HTTP
# HOMEKIT_INCLUDE_DIRS += -I ../../lib/HomeKitADK-master/HAP
# HOMEKIT_INCLUDE_DIRS += -I ../../lib/HomeKitADK-master/Tests/Harness
# HOMEKIT_INCLUDE_DIRS += -I ../../lib/HomeKitADK-master/PAL
# HOMEKIT_INCLUDE_DIRS += -I ../../lib/HomeKitADK-master/PAL/POSIX
# HOMEKIT_INCLUDE_DIRS += -I ../../lib/HomeKitADK-master/PAL/Crypto/MbedTLS/Ed25519
# HOMEKIT_INCLUDE_DIRS += -I ../../src/HomeKit

# CGuard32
CGUARD32 = ../..
CG32_INCLUDE_DIR += -I $(CGUARD32)/include
CG32_SRC_DIRS += $(shell find $(CGUARD32)/src/common -name '*.c')
CG32_SRC_DIRS += $(shell find $(CGUARD32)/src/device -name '*.c')
CG32_SRC_DIRS += $(shell find $(CGUARD32)/src/dmcControl -name '*.c')
#CG32_SRC_DIRS += $(shell find $(CGUARD32)/src/dmcLog -name '*.c')
CG32_SRC_DIRS += $(shell find $(CGUARD32)/src/dmcMqtt -name '*.c')
CG32_SRC_DIRS += $(shell find $(CGUARD32)/src/dmcToken -name '*.c')
#CG32_SRC_DIRS += $(shell find $(CGUARD32)/src/vender -name '*.c')

#CG2_SRC_DIRS += $(shell find $(CGUARD32)/src/Homekit -name '*.c')
#CG2_SRC_DIRS += $(shell find $(CGUARD32)/src/TiZigbee -name '*.c')
#CG2_SRC_DIRS += $(shell find $(CGUARD32)/src/AmazonGadget -name '*.c')

CG32_SRC_DIRS += $(shell find $(CGUARD32)/src/TiZigbee -name '*.c')
CG32_SRC_DIRS += $(shell find $(CGUARD32)/src/Speaker -name '*.c')
CG32_SRC_DIRS += $(shell find $(CGUARD32)/src/Local -name '*.c')


#Aggregate all include and src directories
INCLUDE_ALL_DIRS += $(AWS_INCLUDE_DIRS)
INCLUDE_ALL_DIRS += $(TLS_INCLUDE_DIR)
INCLUDE_ALL_DIRS += $(APP_INCLUDE_DIRS)
# CGUARD32's include directories.
INCLUDE_ALL_DIRS += $(CG32_INCLUDE_DIR)
# Homekit library include directories.
#INCLUDE_ALL_DIRS += $(HOMEKIT_INCLUDE_DIRS)

SRC_FILES += $(AWS_SRC_FILES)

# Logging level control
LOG_FLAGS += -DENABLE_IOT_DEBUG
LOG_FLAGS += -DENABLE_IOT_INFO
LOG_FLAGS += -DENABLE_IOT_WARN
LOG_FLAGS += -DENABLE_IOT_ERROR

COMPILER_FLAGS += $(LOG_FLAGS)
#If the processor is big endian uncomment the compiler flag
#COMPILER_FLAGS += -DREVERSED

MBED_TLS_MAKE_CMD = $(MAKE) -C $(MBEDTLS_DIR)

PRE_MAKE_CMD = $(MBED_TLS_MAKE_CMD)
# MAKE_CMD = $(CC) $(APP_NAME).c $(CG32_SRC_DIRS) $(COMPILER_FLAGS) -o $(APP_NAME) -pthread -ldns_sd -lm  -lcrypto -L. -lwiringPi -lAwsIotSdk -L  $(LD_FLAG) $(INCLUDE_ALL_DIRS)
# MAKE_CMD = $(CC) $(APP_NAME).c $(CG32_SRC_DIRS) $(COMPILER_FLAGS) -o $(APP_NAME) -Wall -pthread -ldns_sd -lm  -lcrypto -L. -lwiringPi -lAwsIotSdk -L  $(LD_FLAG) $(INCLUDE_ALL_DIRS)
# delete homekit -ldns_sd
MAKE_CMD = $(CC) $(APP_NAME).c $(CG32_SRC_DIRS) $(COMPILER_FLAGS) -o $(APP_NAME) -Wall -pthread -lm  -lcrypto -L. -lwiringPi -lAwsIotSdk -L  $(LD_FLAG) $(INCLUDE_ALL_DIRS)

all: libAwsIotSdk.a
	$(PRE_MAKE_CMD)
	$(DEBUG)$(MAKE_CMD)
	$(POST_MAKE_CMD)

libAwsIotSdk.a: $(SRC_FILES:.c=.o)
	ar rcs $@ $^

%.o : %.c
	$(CC) -c $< -o $@ $(COMPILER_FLAGS) $(EXTERNAL_LIBS) $(INCLUDE_ALL_DIRS)

clean:
	rm -f $(APP_DIR)/$(APP_NAME)
	rm -f $(APP_DIR)/libAwsIotSdk.a
	$(MBED_TLS_MAKE_CMD) clean
