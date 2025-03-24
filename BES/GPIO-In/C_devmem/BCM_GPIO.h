#ifndef BCM_GPIO_H
#define BCM_GPIO_H

#define RPI3

#define MASK(w) (((uint32_t) 1) << w)
#define MODIFY_FIELD(reg, pos, wid, value) \
  ((reg) = ((reg)& (~((MASK(wid)-1) << pos))) |	\
   ( ((uint32_t)(value) & (MASK(wid)-1)) << pos))

#ifdef RPI4
  #define GPIO_BASE	0xFE200000   // on RPi 4
  #define GPIO_STRUCT   GPIO_BCM2711
#endif
#ifdef RPI3
 #define GPIO_BASE    0x3F200000   // on the RPi 2/3
 #define GPIO_STRUCT   GPIO_BCM2835
#else
 #define GPIO_BASE  0x20000000 // on RPi models other than the RPi 2,3,4
#endif

// Definitions for abstraction
// from table 63 in BCM2711 ARM Peripherals
typedef struct {
  uint32_t GPFSEL[6]; // 0x00 offset
  uint32_t reserved_1;
  uint32_t GPSET[2];  // 0x1c offset
  uint32_t reserved_2;
  uint32_t GPCLR[2];  // 0x28 offset
  uint32_t reserved_3;
  uint32_t GPLEV[2];  // 0x34 offset
  uint32_t reserved_4;
  uint32_t GPEDS[2];  // 0x40 offset
  uint32_t reserved_5;
  uint32_t GPREN[2];  // 0x4c offset
  uint32_t reserved_6;
  uint32_t GPFEN[2];  // 0x58 offset
  uint32_t reserved_7;
  uint32_t GPHEN[2];  // 0x64 offset
  uint32_t reserved_8;
  uint32_t GPLEN[2];  // 0x70 offset
  uint32_t reserved_9;
  uint32_t GPAREN[2];  // 0x7c offset
  uint32_t reserved_10;
  uint32_t GPAFEN[2];  // 0x88 offset
  uint32_t reserved_11[(0xe4-0x90)/4];
  uint32_t GPIO_PUP_PDN_CNTRL_REG[4];  // 0xe4 offset
} GPIO_BCM2711;

// from table 6-1 in BCM2835 ARM Peripherals
typedef struct {
  uint32_t GPFSEL[6]; // 0x00 offset
  uint32_t reserved_1;
  uint32_t GPSET[2];  // 0x1c offset
  uint32_t reserved_2;
  uint32_t GPCLR[2];  // 0x28 offset
  uint32_t reserved_3;
  uint32_t GPLEV[2];  // 0x34 offset
  uint32_t reserved_4;
  uint32_t GPEDS[2];  // 0x40 offset
  uint32_t reserved_5;
  uint32_t GPREN[2];  // 0x4c offset
  uint32_t reserved_6;
  uint32_t GPFEN[2];  // 0x58 offset
  uint32_t reserved_7;
  uint32_t GPHEN[2];  // 0x64 offset
  uint32_t reserved_8;
  uint32_t GPLEN[2];  // 0x70 offset
  uint32_t reserved_9;
  uint32_t GPAREN[2];  // 0x7c offset
  uint32_t reserved_10;
  uint32_t GPAFEN[2];  // 0x88 offset
  uint32_t reserved_11;
  uint32_t GPPUD;      // 0x94 offset
  uint32_t GPPUDCLK[2]; // 0x98 offset
} GPIO_BCM2835;

#endif
