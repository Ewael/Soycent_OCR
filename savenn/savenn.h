#ifndef SAVELIB_H
#define SAVELIB_H
#include "../Matrix/Matrix.h"

/*
 *  Saves a list of matrices in file
 */
int save_matrix_list(Matrix **l, size_t nbMatrix, char *path);

/*
 *  Loads a matrix list from file
 *  res_nbMatrix is a return value (number of matrices in the list)
 */
Matrix **load_matrix_list(char *path, size_t *res_nbMatrix);

#endif
