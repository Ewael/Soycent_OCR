#include <stdlib.h>

#include "../struct/Matrix.h"

#include "character_sizes.h"

Matrix *square_matrix(Matrix *m) {
    int side = m->lines > m->columns ? m->lines : m->columns;
    Matrix *res = init_matrix(side, side, 0);
    int new_center = side / 2;
    int old_center_x = m->columns / 2;
    int old_center_y = m->lines / 2;
    for (int i = 0; i < m->lines; i++) {
        for (int j = 0; j < m->columns; j++) {
            if (m->list[i * m->columns + j] == 1) {
                int new_pos_y = i - old_center_y + new_center;
                int new_pos_x = j - old_center_x + new_center;
                res->list[new_pos_y * side + new_pos_x] = m->list[i * m->columns + j];
            }
        }
    }
    return res;
}

Matrix *character_normalize(Matrix *m)
{
    int ratio = m->lines / 26 + 1;
    if (m == 0) {
        return NULL;
    }
    Matrix *res = init_matrix(26, 26, 0);
    int x = 0;
    int y = 0;
    for(int i = 0; i < m->lines && y < 26; i += ratio) {
        x = 0;
        for (int j = 0; j < m->columns && x < 26; j += ratio) {
            if (m->list[i * m->columns + j] == 1)
                res->list[y * 26 + x] = m->list[i * m->columns + j];
            x++;
        }
        y++;
    }
    return res;
}
