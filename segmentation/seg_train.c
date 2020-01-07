#include <stdlib.h>
#include <err.h>
#include "xycut.h"
#include "../list/list.h"

size_t list_nbchars(list *l) {
    list *cur = l->next;
    size_t i = 0;
    while (cur != NULL) {
        struct txtelement *t = cur->data;
        if (t->type == 0)
            i++;
        cur = cur->next;
    }

    return i;
}

Matrix **prepare_inputs(list **queues, size_t nb_images, size_t *nb) {
    size_t length = 0;
    for (size_t i = 0; i < nb_images; i++) {
        length += list_nbchars(queues[i]);
    }
    *nb = length;

   if (length == 0)
        return NULL;

    Matrix **res = malloc(sizeof(Matrix) * length);
    if (res == NULL)
        errx(1, "not enough memory");

    size_t i = 0;
    size_t letter_count = 0;
    struct txtelement *nline = NULL;
    struct txtelement *nspace = NULL;
    struct txtelement *elt = NULL;

    for (size_t j = 0; j < nb_images; j++) {
        while(!list_is_empty(queues[j])) {
            list *cur = list_pop_front(queues[j]);
            elt = cur->data;
            if (elt->type == 0) {
                if (letter_count % 54 <= 25) {
                    res[i] = elt->matrix;
                    res[i]->columns = 1;
                    res[i]->lines = 676;
                    i++;
                }
                letter_count++;
                free(elt);
            }
            else if (elt->type == 1)
                nspace = elt;
            else if (elt->type == 2)
                nline = elt;
            free(cur);
        }

        if (nline) {
            free(nline);
            nline = NULL;
        }
        if (nspace) {
            free(nspace);
            nspace = NULL;
        }

        free(queues[j]);
    }
    free(queues);
    return res;
}

Matrix **prepare_results(size_t nbchar) {
    Matrix **res = malloc(sizeof(Matrix) * nbchar);
    if (res == NULL)
        errx(1, "not enough memory");
    for (size_t i = 0; i < nbchar; i++) {
        res[i] = init_matrix(1, nbchar, 0);
        res[i]->list[i] = 1;
    }

    return res;
}
