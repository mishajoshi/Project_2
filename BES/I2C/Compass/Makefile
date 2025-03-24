CC = gcc
CFLAGS = -c -Wall -g -march=armv7-a -mfpu=neon -mfloat-abi=hard -O1
LDLIBS = -lm -li2c

# compass0: compass0.o
#	$(CC) compass0.o -g $(LDLIBS) -o $@

compass: twiddler.o compass.o
	$(CC) twiddler.o compass.o -g -lm -o $@

clean:
	rm -f *.o compass compass0
