#ifndef OCR_H
#define OCR_H

int load_start_training(char *path);

int start_training(char *path);

char *image_to_text(char *path);

#endif
