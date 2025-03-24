#include <stdio.h>
#include <gpiod.h>

int main() {
  struct gpiod_chip *chip;
  struct gpiod_line *out_line;
  int req, value;

  chip = gpiod_chip_open("/dev/gpiochip0");
  if (!chip)
    return -1;
  out_line = gpiod_chip_get_line(chip, 17);
  if (!out_line) {
    gpiod_chip_close(chip);
    return -1;
  }
  req = gpiod_line_request_output(out_line, "gpio_state", 0);
  if (req) {
    gpiod_chip_close(chip);
    return -1;
  }

  int i=0;
  while (1) {
    gpiod_line_set_value(out_line, (i++)&1);
  }
  gpiod_chip_close(chip);
}
