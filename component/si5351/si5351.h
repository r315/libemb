/*
 * si5351.h - Si5351 library
 *
 * Copyright (C) 2015 - 2019 Jason Milldrum <milldrum@gmail.com>
 *                           Dana H. Myers <k6jq@comcast.net>
 *
 * Many defines derived from clk-si5351.h in the Linux kernel.
 * Sebastian Hesselbarth <sebastian.hesselbarth@gmail.com>
 * Rabeeh Khoury <rabeeh@solid-run.com>
 *
 * do_div() macro derived from /include/asm-generic/div64.h in
 * the Linux kernel.
 * Copyright (C) 2003 Bernardo Innocenti <bernie@develer.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SI5351_H_
#define SI5351_H_

#include <stdint.h>
#include "i2c.h"

/* Define definitions */

#define SI5351_BUS_BASE_ADDR            0x60
#define SI5351_XTAL_FREQ                25000000
#define SI5351_PLL_FIXED                80000000000ULL
#define SI5351_FREQ_MULT                100ULL
#define SI5351_DEFAULT_CLK              1000000000ULL

#define SI5351_PLL_VCO_MIN              600000000
#define SI5351_PLL_VCO_MAX              900000000
#define SI5351_MULTISYNTH_MIN_FREQ      500000
#define SI5351_MULTISYNTH_DIVBY4_FREQ   150000000
#define SI5351_MULTISYNTH_MAX_FREQ      225000000
#define SI5351_MULTISYNTH_SHARE_MAX     100000000
#define SI5351_MULTISYNTH_SHARE_MIN     1024000
#define SI5351_MULTISYNTH67_MAX_FREQ    SI5351_MULTISYNTH_DIVBY4_FREQ
#define SI5351_CLKOUT_MIN_FREQ          4000
#define SI5351_CLKOUT_MAX_FREQ          SI5351_MULTISYNTH_MAX_FREQ
#define SI5351_CLKOUT67_MS_MIN          SI5351_PLL_VCO_MIN / SI5351_MULTISYNTH67_A_MAX
#define SI5351_CLKOUT67_MIN_FREQ        SI5351_CLKOUT67_MS_MIN / 128
#define SI5351_CLKOUT67_MAX_FREQ        SI5351_MULTISYNTH67_MAX_FREQ

#define SI5351_PLL_A_MIN                15
#define SI5351_PLL_A_MAX                90
#define SI5351_PLL_B_MAX                (SI5351_PLL_C_MAX-1)
#define SI5351_PLL_C_MAX                1048575
#define SI5351_MULTISYNTH_A_MIN         6
#define SI5351_MULTISYNTH_A_MAX         1800
#define SI5351_MULTISYNTH67_A_MAX       254
#define SI5351_MULTISYNTH_B_MAX         (SI5351_MULTISYNTH_C_MAX-1)
#define SI5351_MULTISYNTH_C_MAX         1048575
#define SI5351_MULTISYNTH_P1_MAX        ((1<<18)-1)
#define SI5351_MULTISYNTH_P2_MAX        ((1<<20)-1)
#define SI5351_MULTISYNTH_P3_MAX        ((1<<20)-1)
#define SI5351_VCXO_PULL_MIN            30
#define SI5351_VCXO_PULL_MAX            240
#define SI5351_VCXO_MARGIN              103

// Registers and register bits
#define SI5351_DEVICE_STATUS_REG        0
#define SI5351_STATUS_SYS_INIT          (1<<7)
#define SI5351_STATUS_LOL_B             (1<<6)
#define SI5351_STATUS_LOL_A             (1<<5)
#define SI5351_STATUS_LOS_CLKIN         (1<<4)
#define SI5351_STATUS_LOS_XTAL          (1<<3)
#define SI5351_INTERRUPT_STATUS_REG     1
#define SI5351_INTERRUPT_SYS_INIT_STKY  (1<<7)
#define SI5351_INTERRUPT_LOL_B_STKY     (1<<6)
#define SI5351_INTERRUPT_LOL_A_STKY     (1<<5)
#define SI5351_INTERRUPT_LOS_CLKIN_STKY (1<<4)
#define SI5351_INTERRUPT_LOS_XTAL_STKY  (1<<3)
#define SI5351_INTERRUPT_MASK_REG       2
#define SI5351_OUTPUT_ENABLE_CTRL_REG   3
#define SI5351_OEB_PIN_ENABLE_CTRL_REG  9
#define SI5351_PLL_INPUT_SOURCE_REG     15
#define SI5351_CLKIN_DIV_MASK           (3<<6)
#define SI5351_CLKIN_DIV_1              (0<<6)
#define SI5351_CLKIN_DIV_2              (1<<6)
#define SI5351_CLKIN_DIV_4              (2<<6)
#define SI5351_CLKIN_DIV_8              (3<<6)
#define SI5351_PLLB_SOURCE              (1<<3)
#define SI5351_PLLA_SOURCE              (1<<2)

#define SI5351_CLK0_CTRL                16
#define SI5351_CLK1_CTRL                17
#define SI5351_CLK2_CTRL                18
#define SI5351_CLK3_CTRL                19
#define SI5351_CLK4_CTRL                20
#define SI5351_CLK5_CTRL                21
#define SI5351_CLK6_CTRL                22
#define SI5351_CLK7_CTRL                23
#define SI5351_CLK_POWERDOWN            (1<<7)
#define SI5351_CLK_INTEGER_MODE         (1<<6)
#define SI5351_CLK_PLL_SELECT           (1<<5)
#define SI5351_CLK_INVERT               (1<<4)
#define SI5351_CLK_INPUT_MASK           (3<<2)
#define SI5351_CLK_INPUT_XTAL           (0<<2)
#define SI5351_CLK_INPUT_CLKIN          (1<<2)
#define SI5351_CLK_INPUT_MULTISYNTH_0_4 (2<<2)
#define SI5351_CLK_INPUT_MULTISYNTH_N   (3<<2)
#define SI5351_CLK_DRIVE_STRENGTH_MASK  (3<<0)
#define SI5351_CLK_DRIVE_STRENGTH_2MA   (0<<0)
#define SI5351_CLK_DRIVE_STRENGTH_4MA   (1<<0)
#define SI5351_CLK_DRIVE_STRENGTH_6MA   (2<<0)
#define SI5351_CLK_DRIVE_STRENGTH_8MA   (3<<0)

#define SI5351_CLK3_0_DISABLE_STATE     24
#define SI5351_CLK7_4_DISABLE_STATE     25
#define SI5351_CLK_DISABLE_STATE_MASK   3
#define SI5351_CLK_DISABLE_STATE_LOW    0
#define SI5351_CLK_DISABLE_STATE_HIGH   1
#define SI5351_CLK_DISABLE_STATE_FLOAT  2
#define SI5351_CLK_DISABLE_STATE_NEVER  3

#define SI5351_PARAMETERS_LENGTH        8
#define SI5351_PLLA_PARAMETERS          26
#define SI5351_PLLB_PARAMETERS          34
#define SI5351_CLK0_PARAMETERS          42
#define SI5351_CLK1_PARAMETERS          50
#define SI5351_CLK2_PARAMETERS          58
#define SI5351_CLK3_PARAMETERS          66
#define SI5351_CLK4_PARAMETERS          74
#define SI5351_CLK5_PARAMETERS          82
#define SI5351_CLK6_PARAMETERS          90
#define SI5351_CLK7_PARAMETERS          91
#define SI5351_CLK6_7_OUTPUT_DIVIDER    92
#define SI5351_OUTPUT_CLK_DIV_MASK      (7 << 4)
#define SI5351_OUTPUT_CLK6_DIV_MASK     (7 << 0)
#define SI5351_OUTPUT_CLK_DIV_SHIFT     4
#define SI5351_OUTPUT_CLK_DIV6_SHIFT    0
#define SI5351_OUTPUT_CLK_DIV_1         0
#define SI5351_OUTPUT_CLK_DIV_2         1
#define SI5351_OUTPUT_CLK_DIV_4         2
#define SI5351_OUTPUT_CLK_DIV_8         3
#define SI5351_OUTPUT_CLK_DIV_16        4
#define SI5351_OUTPUT_CLK_DIV_32        5
#define SI5351_OUTPUT_CLK_DIV_64        6
#define SI5351_OUTPUT_CLK_DIV_128       7
#define SI5351_OUTPUT_CLK_DIVBY4        (3<<2)

#define SI5351_SSC_PARAM0               149
#define SI5351_SSC_PARAM1               150
#define SI5351_SSC_PARAM2               151
#define SI5351_SSC_PARAM3               152
#define SI5351_SSC_PARAM4               153
#define SI5351_SSC_PARAM5               154
#define SI5351_SSC_PARAM6               155
#define SI5351_SSC_PARAM7               156
#define SI5351_SSC_PARAM8               157
#define SI5351_SSC_PARAM9               158
#define SI5351_SSC_PARAM10              159
#define SI5351_SSC_PARAM11              160
#define SI5351_SSC_PARAM12              161

#define SI5351_VXCO_PARAMETERS_LOW      162
#define SI5351_VXCO_PARAMETERS_MID      163
#define SI5351_VXCO_PARAMETERS_HIGH     164

#define SI5351_CLK0_PHASE_OFFSET        165
#define SI5351_CLK1_PHASE_OFFSET        166
#define SI5351_CLK2_PHASE_OFFSET        167
#define SI5351_CLK3_PHASE_OFFSET        168
#define SI5351_CLK4_PHASE_OFFSET        169
#define SI5351_CLK5_PHASE_OFFSET        170

#define SI5351_PLL_RESET                177
#define SI5351_PLL_RESET_B              (1<<7)
#define SI5351_PLL_RESET_A              (1<<5)

#define SI5351_CRYSTAL_LOAD             183
#define SI5351_CRYSTAL_LOAD_RSVD        0x12    // Reserved bits should be set to 0b10010

#define SI5351_FANOUT_ENABLE            187

/* Macro definitions */

//#define RFRAC_DENOM ((1L << 20) - 1)
#define RFRAC_DENOM 1000000ULL

/*
 * Based on former asm-ppc/div64.h and asm-m68knommu/div64.h
 *
 * The semantics of do_div() are:
 *
 * uint32_t do_div(uint64_t *n, uint32_t base)
 * {
 *      uint32_t remainder = *n % base;
 *      *n = *n / base;
 *      return remainder;
 * }
 *
 * NOTE: macro parameter n is evaluated multiple times,
 *       beware of side effects!
 */

#define do_div(n,base) ({                                      \
        uint64_t __base = (base);                               \
        uint64_t __rem;                                         \
        __rem = ((uint64_t)(n)) % __base;                       \
        (n) = ((uint64_t)(n)) / __base;                         \
        __rem;                                                  \
 })

/* Enum definitions */

/*
 * enum si5351_variant - SiLabs Si5351 chip variant
 * @SI5351_VARIANT_A: Si5351A (8 output clocks, XTAL input)
 * @SI5351_VARIANT_A3: Si5351A MSOP10 (3 output clocks, XTAL input)
 * @SI5351_VARIANT_B: Si5351B (8 output clocks, XTAL/VXCO input)
 * @SI5351_VARIANT_C: Si5351C (8 output clocks, XTAL/CLKIN input)
 */
/*
enum si5351_variant {
	SI5351_VARIANT_A = 1,
	SI5351_VARIANT_A3 = 2,
	SI5351_VARIANT_B = 3,
	SI5351_VARIANT_C = 4,
};
*/

enum si5351_clock {
    SI5351_CLK0, SI5351_CLK1, SI5351_CLK2, SI5351_CLK3,
	SI5351_CLK4, SI5351_CLK5, SI5351_CLK6, SI5351_CLK7};

enum si5351_pll {SI5351_PLLA, SI5351_PLLB};

enum si5351_drive {SI5351_DRIVE_2MA, SI5351_DRIVE_4MA, SI5351_DRIVE_6MA, SI5351_DRIVE_8MA};

enum si5351_clock_source {SI5351_CLK_SRC_XTAL, SI5351_CLK_SRC_CLKIN, SI5351_CLK_SRC_MS0, SI5351_CLK_SRC_MS};

enum si5351_clock_output_disable {SI5351_CLK_DISABLE_LOW, SI5351_CLK_DISABLE_HIGH, SI5351_CLK_DISABLE_HI_Z, SI5351_CLK_DISABLE_NEVER};

enum si5351_fanout {SI5351_FANOUT_CLKIN = 0x80, SI5351_FANOUT_XO = 0x40, SI5351_FANOUT_MS = 0x10};

enum si5351_pll_input {SI5351_PLL_INPUT_XO, SI5351_PLL_INPUT_CLKIN};

enum si5351_xtal_cl {SI5351_CL_6PF = 1, SI5351_CL_8PF, SI5351_CL_10PF};

enum si5351_clock_r_div {
    SI5351_CLK_R_DIV_1 = 0, SI5351_CLK_R_DIV_2, SI5351_CLK_R_DIV_4, SI5351_CLK_R_DIV_8,
    SI5351_CLK_R_DIV_16, SI5351_CLK_R_DIV_32, SI5351_CLK_R_DIV_64, SI5351_CLK_R_DIV_128 };

/* Struct definitions */

struct Si5351RegSet
{
	uint32_t p1;
	uint32_t p2;
	uint32_t p3;
};

struct Si5351StatusFlags
{
	uint8_t SYS_INIT;
	uint8_t LOL_B;
	uint8_t LOL_A;
	uint8_t LOS_CLKIN;
    uint8_t LOS_XTAL;
	uint8_t REVID;
};

struct Si5351IntFlags
{
	uint8_t SYS_INIT_STKY;
	uint8_t LOL_B_STKY;
	uint8_t LOL_A_STKY;
	uint8_t LOS_CLKIN_STKY;
    uint8_t LOS_XTAL_STKY;
};

typedef struct Si5351Status_s
{
    struct Si5351StatusFlags status;
    struct Si5351IntFlags intr;
}Si5351Status_t;

struct Si5251ClkPara
{
    uint8_t msb_p3;
    uint8_t lsb_p3;
    struct {
        uint8_t hsb_p1 : 2;
        uint8_t divby4 : 2;
        uint8_t r0_div : 3;
        uint8_t rsvd   : 1;
    };
    uint8_t msb_p1;
    uint8_t lsb_p1;
    struct {
        uint8_t hsb_p2 : 4;
        uint8_t hsb_p3 : 4;
    };
    uint8_t msb_p2;
    uint8_t lsb_p2;
}; // NOTE: This struct is little-endian

uint8_t Si5351_Init(i2cbus_t*, uint32_t, int32_t);
void Si5351_Reset(void);
Si5351Status_t Si5351_DeviceStatusGet(void);
uint8_t Si5351_FreqSet(enum si5351_clock, uint64_t);
uint8_t Si5351_FreqSetManual(enum si5351_clock, uint64_t, uint64_t);
void Si5351_OutputEnableSet(enum si5351_clock, uint8_t);
void Si5351_OutputDriveStrengthSet(enum si5351_clock, enum si5351_drive);
void Si5351_OutputDisableStateSet(enum si5351_clock, enum si5351_clock_output_disable);
void Si5351_ClockPwrSet(enum si5351_clock, uint8_t);
void Si5351_ClockInvertSet(enum si5351_clock, uint8_t);
void Si5351_ClockSourceSet(enum si5351_clock, enum si5351_clock_source);
void Si5351_ClockRdivSet(enum si5351_clock, enum si5351_clock_r_div);
void Si5351_FanoutSet(enum si5351_fanout, uint8_t);
void Si5351_MsSourceSet(enum si5351_clock, enum si5351_pll);

#endif /* SI5351_H_ */