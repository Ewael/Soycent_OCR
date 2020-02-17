# Makefile OCR

CC = gcc
CFLAGS = -W -Wall -Wextra -Werror -std=c99 -O3
CPPFLAGS =
LDFLAGS = -lSDL_image -lm -ldl -MMD

# libraries
LM = -lm
GTK = `pkg-config --cflags --libs gtk+-3.0`
SDL = `pkg-config --cflags --libs sdl`

SRC = struct/*.c \
      net/*.c \
      segmentation/*.c \
      imageload/*.c \
      ocr.c

SRC_UI = $(SRC) ui/*.c

all: cli ui

cli: $(SRC) main.c
	$(CC) -o main main.c \
		$(SRC) \
		$(CFLAGS) $(CPPFLAGS) $(LDFLAGS) \
		$(SDL)

ui: $(SRC_UI) main_gtk.c
	$(CC) -o main_gtk main_gtk.c \
		$(SRC_UI) \
		$(CFLAGS) $(CPPFLAGS) $(LDFLAGS) \
		$(GTK) $(SDL)

.PHONY: clean

clean:
	$(RM) main main.d
	$(RM) main_gtk main_gtk.d

# END
