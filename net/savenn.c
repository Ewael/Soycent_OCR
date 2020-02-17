#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "../struct/Matrix.h"

#include "savenn.h"

/*
 *  Saves a list of matrices in file
 */
int save_matrix_list(Matrix **l, size_t nbMatrix, char *path)
{
    FILE *fptr;
    //opens file
    if ((fptr = fopen(path, "wb")) == NULL)
    {
        printf("Error while saving matrix.\nCan't open file");
        return 1;
    }

    //writes elm in file
    fwrite(&nbMatrix, sizeof(size_t), 1, fptr);

    Matrix *m;
    for(size_t j = 0; j < nbMatrix; ++j)
    {
        m = l[j];
        fwrite(&(m->columns), sizeof(int), 1, fptr);
        fwrite(&(m->lines), sizeof(int), 1, fptr);
        fwrite(&(m->length), sizeof(int), 1, fptr);

        for(int i = 0; i < m->length; ++i)
        {
            fwrite(&(m->list[i]), sizeof(double), 1, fptr);
        }
    }

    fclose(fptr);
    return 0;
}

/*
 *  Loads a matrix list from file
 *  res_mbMatrix is a return value (number of matrices in the list)
 */
Matrix **load_matrix_list(char *path, size_t *res_nbMatrix)
{
    FILE *fptr;
    if ((fptr = fopen(path, "rb")) == NULL)
    {
        printf("Error while loading matrix.\nCan't open file.");
        return NULL;
    }

    size_t nbMatrix;
    if (fread(&nbMatrix, sizeof(size_t), 1, fptr)){}; //avoid unused error
    *res_nbMatrix = nbMatrix;

    Matrix **res = malloc(sizeof(Matrix *) * nbMatrix);
    if (res == NULL)
        errx(1, "not enough memory");

    int currentCols;
    int currentLines;
    int currentLength;
    Matrix *currentMatrix;

    for (size_t i = 0; i < nbMatrix; ++i)
    {
        if (fread(&currentCols, sizeof(int), 1, fptr)){};
        if (fread(&currentLines, sizeof(int), 1, fptr)){};
        if (fread(&currentLength, sizeof(int), 1, fptr)){};
        currentMatrix = init_matrix(currentCols, currentLines, 0);

        for(int j = 0; j < currentLength; ++j)
        {
            if (fread(&(currentMatrix->list[j]), sizeof(double), 1, fptr)){};
        }

        res[i] = currentMatrix;
    }

    return res;
}
