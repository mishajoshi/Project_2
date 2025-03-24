// file input.c
#include <gpiod.h>
#include <error.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

// Use gpiod events to indicate input bit changes rising edges to output bit.
// Uses bulk line access functions for input events.
// From lloydrochester.com/post/hardware/libgpiod-event-rpi/ with modifications.

volatile int run = 1;

void signal_handler (int signum) {
  printf("\nInterrupt received, cleaning up and exiting.\n");
  exit(EXIT_SUCCESS);
}

struct gpiod_chip *chip;
struct gpiod_line_bulk in_lines;
struct gpiod_line_bulk events;
struct gpiod_line * out_line;

int main(int argc, char *argv[]) {
  unsigned int offsets[2];
  int values[2];
  int err;
  struct timespec timeout;

  chip = gpiod_chip_open("/dev/gpiochip0");
  if(!chip)
    {
      perror("gpiod_chip_open");
      goto cleanup;
    }

  // use pin 27 as input
  offsets[0] = 27;
  values[0] = -1;
  // use pin 22 as input
  offsets[1] = 22;
  values[1] = -1;
  
  err = gpiod_chip_get_lines(chip, offsets, 1, &in_lines);
  if(err)
    {
      perror("gpiod_chip_get_lines");
      goto cleanup;
    }

  err = gpiod_line_request_bulk_rising_edge_events(&in_lines, "rising edge example");
  if(err)
    {
      perror("gpiod_line_request_bulk_rising_edge_events");
      goto cleanup;
    }

  // use pin 17 as output
  out_line = gpiod_chip_get_line(chip, 17);
  if (!out_line) {
    perror("gpiod_chip_get_line");
    goto cleanup;
  }
  
  err = gpiod_line_request_output(out_line, "gpio_state", 0);
  if (err) {
    perror("gpiod_line_request_output");
    goto cleanup;
  } 

  signal(SIGINT, signal_handler);
  
  while (run) {
    // Timeout of 60 seconds, pass in NULL to wait forever
    timeout.tv_sec = 60;
    timeout.tv_nsec = 0;

    // printf("waiting for rising edge event\n");

    err = gpiod_line_event_wait_bulk(&in_lines, &timeout, &events);
    if(err == -1) {
      perror("gpiod_line_event_wait_bulk");
      goto cleanup;
    } else if(err == 0) {
      fprintf(stderr, "wait timed out\n");
      goto cleanup;
    }

    err = gpiod_line_get_value_bulk(&events, values);
    if(err) {
      perror("gpiod_line_get_value_bulk");
      goto cleanup;
    }

    for(int i=0; i<gpiod_line_bulk_num_lines(&events); i++) {
      struct gpiod_line* line;
      line = gpiod_line_bulk_get_line(&events, i);
      if(!line) {
	fprintf(stderr, "unable to get line %d\n", i);
	continue;
      } else {
	gpiod_line_set_value(out_line, gpiod_line_get_value(line));
	// printf("line %s(%d)\n", gpiod_line_name(line), gpiod_line_offset(line));
      }
    }
  }
 cleanup:
  gpiod_line_release_bulk(&in_lines);
  gpiod_chip_close(chip);

  return EXIT_SUCCESS;
}
