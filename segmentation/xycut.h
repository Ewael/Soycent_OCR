#ifndef XYCUT_H
#define XYCUT_H

struct txtelement {
    Matrix *matrix;
    char type; // type of element: 0 = letter, 1 = space, 2 = \n
};

list *segmentate(Matrix *matrix);

#endif
