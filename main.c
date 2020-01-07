#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ocr.h"

int main(int argc, char *argv[]) {
    if(argc == 3) { // training
        if(strcmp(argv[1], "--train") == 0) {
            // train with argv[2] set and test with argv[3]
            if(!start_training(argv[2]))
                return 0;
        }
        if(strcmp(argv[1], "--retrain") == 0) {
            load_start_training(argv[2]);
        }
    }
    if(argc == 2) { // image to text (ocr)
        //ocr_main(argv[2]);
        char *res = image_to_text(argv[1]);
        if (res) {
            printf("\nResult:\n\n");
            printf("%s\n", res);
            return 0;
        }
    }


    printf("Usage:\n");
    printf("    ./main --train <train set folder>\n");
    printf("    ./main <path>\n");
    printf("    ./main (-h | --help)\n");
    printf("\nReport bugs to: nicolas.froger@epita.fr\n");
    printf("This software is made by the SOYCENT team.\n");

    if(argc == 2 && (strcmp(argv[1],"--help") == 0 
                || strcmp(argv[1], "-h") == 0)) {
        return 0;
    }
    return 1;
}
