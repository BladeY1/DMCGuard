# DmcGuard
	DMC Channel Guard 

# requirement
	use package manager to install `wiringpi` package

# lib\aws-iot-device-sdk-embedded-C
	https://github.com/aws/aws-iot-device-sdk-embedded-C/releases/tag/v3.1.5

	modify lib\aws-iot-device-sdk-embedded-C\.gitignore

# external_libs/mbedTLS/*

	include mbedTLS 2.6.12

	https://github.com/ARMmbed/mbedtls/releases/tag/mbedtls-2.16.12


# AWS certs
	1. move your AWS certs to \test\main\cert
	2. modify cert infomation in test\main\AWSConfig.h


# Compile
	`Makefile` exists in `/test/main`, use `make` to get executable binary file.

# Speaker hub
	Change the IP of your speaker hub in \include\Speaker.h



