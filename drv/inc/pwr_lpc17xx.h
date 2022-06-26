#ifndef _pwr_lpc17xx_h_
#define _pwr_lpc17xx_h_

// PCON
#define SC_PCON_PM0             (1 << 0)
#define SC_PCON_PM1             (1 << 1)
#define SC_PCON_BODRPM          (1 << 2)
#define SC_PCON_BOGD            (1 << 3)
#define SC_PCON_BORD            (1 << 4)
#define SC_PCON_SMFLAG          (1 << 8)
#define SC_PCON_DSFLAG          (1 << 9)
#define SC_PCON_PDFLAG          (1 << 10)
#define SC_PCON_DPDFLAG         (1 << 11)

// PCONP
#define SC_PCONP_PCTIM0         (1 << 1)
#define SC_PCONP_PCTIM1         (1 << 2)
#define SC_PCONP_PCUART0        (1 << 3)
#define SC_PCONP_PCUART1        (1 << 4)
#define SC_PCONP_PCPWM1         (1 << 6)
#define SC_PCONP_PCI2C0         (1 << 7)
#define SC_PCONP_PCSPI          (1 << 8)
#define SC_PCONP_PCRTC          (1 << 9)
#define SC_PCONP_PCSSP1         (1 << 10)
#define SC_PCONP_PCADC          (1 << 12)
#define SC_PCONP_PCCAN1         (1 << 13)
#define SC_PCONP_PCCAN2         (1 << 14)
#define SC_PCONP_PCGPIO         (1 << 15)
#define SC_PCONP_PCRIT          (1 << 16)
#define SC_PCONP_PCMCPWM        (1 << 17)
#define SC_PCONP_PCQEI          (1 << 18)
#define SC_PCONP_PCI2C1         (1 << 19)
#define SC_PCONP_PCSSP0         (1 << 21)
#define SC_PCONP_PCTIM2         (1 << 22)
#define SC_PCONP_PCTIM3         (1 << 23)
#define SC_PCONP_PCUART2        (1 << 24)
#define SC_PCONP_PCUART3        (1 << 25)
#define SC_PCONP_PCI2C2         (1 << 26)
#define SC_PCONP_PCI2S          (1 << 27)
#define SC_PCONP_PCGPDMA        (1 << 29)
#define SC_PCONP_PCENET         (1 << 30)
#define SC_PCONP_PCUSB          (1 << 31)

#endif