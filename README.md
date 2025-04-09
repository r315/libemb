# libEMB

This is my personal library for microcontrolers sensors and other components

## Makefile variables
LIB_PATH        =$(LIBEMB_PATH)/lib
CMSIS_PATH 		=$(LIBEMB_PATH)/CMSIS
DRIVERS_PATH 	=$(LIBEMB_PATH)/drv

DRIVERS_CORE    =$(CMSIS_PATH)/Core
DRIVERS_CMSIS   =$(CMSIS_PATH)/Device/<manufacturer>/<device>
DRIVERS_SOC     =$(DRIVERS_PATH)
DRIVERS_MFD     =$(DRIVERS_PATH)/SOC    <-- Needs a better name for folder
DRIVER_COMP     =$(LIBEMB_PATH)/component

libemb
|
├───bsp
│   ├───blueboard
│   └───bluepill
├───CMSIS
│   ├───Core
│   │   └───Include
│   └───Device
│       ├───ARTERY
│       │   └───AT32F4xx
│       │       ├───inc
│       │       └───src
│       ├───Gigadevice
│       │   └───GD32E23x
│       │       ├───inc
│       │       └───src
│       ├───NXP
│       │   └───LPC17xx
│       │       ├───inc
│       │       └───src
│       └───ST
│           ├───STM32F10x
│           │   ├───inc
│           │   └───src
│           ├───STM32F7xx
│           │   └───Include
│           └───STM32L4xx
│               └───Include
├───component
│   ├───accelerometer
│   ├───ds1086
│   ├───eeprom
│   ├───io_expander
│   ├───si5351
│   ├───tft
│   └───touch
├───drv
│   ├───adc
│   ├───clock
│   ├───costum
│   │   ├───inc
│   │   └───src
│   ├───dac
│   ├───dma
│   ├───gpio
│   ├───i2c
│   ├───i2s
│   ├───inc
│   ├───ir
│   ├───pwm
│   ├───rng
│   ├───SOC
│   │   ├───at32f4xx
│   │   │   ├───inc
│   │   │   └───src
│   │   └───gd32e23x
│   │       ├───inc
│   │       └───src
│   ├───spi
│   ├───storage
│   ├───timer
│   ├───tone
│   ├───uart
│   └───wdt
├───lib
│   ├───inc
│   ├───pff
│   └───src
└───tools


