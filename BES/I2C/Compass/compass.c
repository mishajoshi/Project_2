#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>

#include "HMC5883L.h"

#define DELAY (10000)
#define READ_BYTES_INDIVIDUALLY (1)
#define PRINT_RAW_BYTES (0)
#define PRINT_RANGE (0)

extern int set_i2c_register(int file, unsigned char addr,
			    unsigned char reg, unsigned char value);

extern int get_i2c_register(int file, unsigned char addr,
                            unsigned char reg, unsigned char *val);
extern int get_i2c_registers(int file, unsigned char addr,
			     unsigned char first_reg, unsigned char num_regs,
			     unsigned char *val);

typedef struct {
  short min, max;
} LIMITS_T;

LIMITS_T x_lim, y_lim, z_lim;

void init_limits(LIMITS_T * l) {
  l->min = SHRT_MAX;
  l->max = SHRT_MIN;
}

int configure_compass(int file, unsigned char address) {
  uint8_t compass_ID[3];
  
  if (get_i2c_register(file, address, ID_REG_A, &(compass_ID[0])))
    return 1;
  if (get_i2c_register(file, address, ID_REG_B, &(compass_ID[1])))
    return 1;
  if (get_i2c_register(file, address, ID_REG_C, &(compass_ID[2])))
    return 1;

  if (strncmp((char *) compass_ID, "H43", 3)) {
    printf("Compass not found. Expected H43, got %s.\n", compass_ID);
    return 1;
  } else {
    printf("Compass found.\n");
  }
  
  // 4 averages, 30 Hz, normal measurement. 0101 0100
  if (set_i2c_register(file, address, CONFIG_A_REG, 0x54))
    return 1;
  // Gain 
  if (set_i2c_register(file, address, CONFIG_B_REG, 0x00))
    return 1;
  // Operation mode
  if (set_i2c_register(file, address, MODE_REG, IDLE_MODE))
    return 1;

  return 0;
}

float calc_heading(float mx, float my) {
  float h = atan2f(my, mx)*(180/M_PI);
  if (h<0)
    h += 360; 
  return h;
}

void update_limits(short x, short y, short z) {
  if (x < x_lim.min)
    x_lim.min = x;
  if (x > x_lim.max)
    x_lim.max = x;
  if (y < y_lim.min)
    y_lim.min = y;
  if (y > y_lim.max)
    y_lim.max = y;
  if (z < z_lim.min)
    z_lim.min = z;
  if (z > z_lim.max)
    z_lim.max = z;
}

int main(int argc, char *argv[])
{
  int address, i;
  uint8_t status;
  int i2c_file;
  short x_val, y_val, z_val;
  uint8_t data[6];
  float strength;
  
  if ((i2c_file = open("/dev/i2c-1", O_RDWR)) < 0) {
    perror("Unable to open i2c controller/file");
    exit(1);
  }

  address = COMPASS_DEV_ADDR;
  if (configure_compass(i2c_file, address)) {
    printf("Unable to configure compass\n");
    return 1;
  }

  init_limits(&x_lim);
  init_limits(&y_lim);
  init_limits(&z_lim);
  
  while(1) {
    // Start measurement
    set_i2c_register(i2c_file, address, MODE_REG, SINGLE_MEASUREMENT_MODE);  
    // Await data ready
    do {
      get_i2c_register(i2c_file, address, STATUS_REG, &status);
    } while (!(status & 1));
    
    #if READ_BYTES_INDIVIDUALLY // Read each field strength byte individually
    for (i=0; i<6; i++) {
      get_i2c_register(i2c_file, address, XH_REG+i, &(data[i]));
      #if PRINT_RAW_BYTES
      printf("%02x ", data[i]);
      #endif
    }
    #else // read all field strength bytes in one transaction
    get_i2c_registers(i2c_file, address, XH_REG, 6, data);
    #if PRINT_RAW_BYTES
    for (i=0; i<6; i++) {
      printf("%02x ", data[i]);
    }
    #endif // PRINT_RAW_BYTES 
    #endif // Not READ_BYTES_INDIVIDUALLY

    x_val = (((int16_t) data[0]) << 8) + data[1];
    z_val = (((int16_t) data[2]) << 8) + data[3];
    y_val = (((int16_t) data[4]) << 8) + data[5];

    update_limits(x_val, y_val, z_val);
    strength = sqrt(x_val*x_val + y_val*y_val + z_val*z_val);
  
    printf("M:(%4d, %4d, %4d) ", x_val, y_val, z_val);
    #if PRINT_RANGE
    printf("M Range:(%4d~%3d Y: %4d~%3d Z: %4d~%3d) ",
	   x_lim.min, x_lim.max, y_lim.min, y_lim.max, z_lim.min, z_lim.max);
    #endif
    printf(" Hdg: %6.2f Str: %6.2f\r", calc_heading(x_val, y_val), strength);
  }
  return 0;  
}
