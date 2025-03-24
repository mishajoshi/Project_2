#include <stdio.h>
#include <gpiod.h>
#include <signal.h>

// Use busy-wait polling and gpiod to echo input bit to output bit. Uses single line access functions.

volatile int run = 1;

void signal_handler (int signum) {
  printf("\nCtrl-C pressed, cleaning up and exiting.\n");
   run = 0;
}

int main() {
  struct gpiod_chip *chip;
  struct gpiod_line *in_line, *out_line;
  int req, value;

  chip = gpiod_chip_open("/dev/gpiochip0");
  if (!chip) {
    printf("Couldn't open chip.\n");
    return -1;
  }
  
  in_line = gpiod_chip_get_line(chip, 27);
  if (!in_line) {
    gpiod_chip_close(chip);
    printf("Couldn't get line for input. May need to unexport in /sys/class/gpio.\n");
    return -1;
  }
  
  req = gpiod_line_request_input(in_line, "gpio_state");
  if (req) {
    printf("Couldn't set line as input.\n");
    gpiod_chip_close(chip);
    return -1;
  }
  out_line = gpiod_chip_get_line(chip, 17);
  if (!out_line) {
    printf("Couldn't get line for output. May need to unexport in /sys/class/gpio.\n");
    gpiod_chip_close(chip);
    return -1;
  }
  req = gpiod_line_request_output(out_line, "gpio_state", 0);
  if (req) {
    printf("Couldn't set line as output.\n");
    gpiod_chip_close(chip);
    return -1;
  }

  // Register handler for Ctrl-C
  signal(SIGINT, signal_handler);
  
  while (run) {
    value = gpiod_line_get_value(in_line);
    gpiod_line_set_value(out_line, value);
  }
  gpiod_chip_close(chip);
}
