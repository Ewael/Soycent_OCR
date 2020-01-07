CC=gcc

CPPFLAGS=`pkg-config --cflags sdl` -MMD

CFLAGS= `pkg-config --cflags gtk+-3.0` -Wall -Wextra -Werror -std=c99 -O3

LDFLAGS=
LDLIBS=`pkg-config --libs gtk+-3.0` `pkg-config --libs sdl` -lSDL_image -lm -ldl

SRC = Matrix/Matrix.c list/list.c net/neuralnet.c savenn/savenn.c segmentation/xycut.c segmentation/seg_train.c imageload/imageload.c imageload/pixel_operations.c net/ocr_net.c ocr.c segmentation/character_sizes.c

SRC_UI = ui/main.c ui/train.c

OBJ= $(SRC:.c=.o)
DEP= $(SRC:.c=.d)

OBJ_UI= $(SRC_UI:.c=.o)
DEP_UI= $(SRC_UI:.c=.d)

all: main

main: main.o $(OBJ)

ui: main_gtk

main_gtk: main_gtk.o $(OBJ) $(OBJ_UI)

clean:
	${RM} ${OBJ} ${DEP} ${OBJ_UI} ${DEP_UI} main main.d main.o main_gtk main_gtk.o main_gtk.d

# END
