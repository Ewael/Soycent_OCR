#include <stdlib.h>
#include <err.h>

#include "../struct/Matrix.h"
#include "../struct/list.h"

#include "character_sizes.h"

#include "xycut.h"

int *sumlines(Matrix *matrix) {
        int *sums = malloc(sizeof(int) * matrix->lines);
        for(int i = 0; i < matrix->lines; ++i) {
            sums[i] = 0;
        }
        int pos;
        int inc = 0;
        for(int i = 0; i < matrix->length; i++) {
            pos = i % matrix->columns;
            if (pos == 0 && i != 0) {
                inc++;
            }
           if (matrix->list[i] != 2) {
              sums[inc] += matrix->list[i];
           }
        } 

        return sums;
}

int *sumcols(Matrix *matrix) {
        int *sums = malloc(matrix->columns * sizeof(int));
        for(int i = 0; i < matrix->columns; ++i) {
            sums[i] = 0;
        }

        int pos;
        for(int i = 0; i < matrix->length; i++) {
            pos = i % matrix->columns;
            if (matrix->list[i] != 2) {
                sums[pos] += matrix->list[i];
            }
        }

        return sums;
}

Matrix *cutHorizontally(Matrix *matrix, int startline, int endline) {
    Matrix *res = init_matrix(matrix->columns, endline - startline + 1, 0);
    
    int i = matrix->columns * startline;
    int j = 0;
    for (; i < matrix->columns * (endline + 1); i++) {
        res->list[j] = matrix->list[i]; 
        j++;
    }

    return res;
}

Matrix *cutVertically(Matrix *matrix, int startcol, int endcol) {
    Matrix *res = init_matrix(endcol - startcol + 1, matrix->lines, 0); 
    int x = 0;
    int i = startcol;
    for (; i <= startcol + matrix->columns * (matrix->lines - 1);
           i += matrix->columns) {
        for(int j = i; j < i + (endcol - startcol + 1); ++j) {
            res->list[x] = matrix->list[j];
            ++x;
        }
    }

    return res;
}

int getNumberOfLines(Matrix *matrix, int *sums) {
    int res = 0;

    if (sums[matrix->lines - 1] != 0) {
        res++;
    }

    int currentLineHeight = 0;

    for(int i = 1; i < matrix->lines; ++i) {
        if(sums[i] == 0 && currentLineHeight != 0) {
            currentLineHeight = 0;
            res++;
        }
        if (sums[i] != 0) {
            currentLineHeight++;
        }
    }

    return res;
}

Matrix **getlines(Matrix *matrix, size_t* noLines) {
    int *sums = sumlines(matrix);
    int noOfLines = getNumberOfLines(matrix, sums);
    *noLines = (size_t) noOfLines;
    Matrix **lines = malloc(sizeof(Matrix *) * noOfLines);
    
    int currentStart = 0;
    int currentEnd = 0; 
    int lineHeight = 0;

    int n = 0;
    
    for(int i = 0; i < matrix->lines; ++i) {
        if (sums[i] == 0 && lineHeight != 0) {
            lineHeight = 0;
            currentEnd = i - 1;
            lines[n] = cutHorizontally(matrix, currentStart, currentEnd);
            n++;
        }
        if (sums[i] != 0 && lineHeight == 0) {
            currentStart = i;
        }
        if (sums[i] != 0) {
            lineHeight++;
        }
    }

    if (lineHeight != 0) {
        lines[n] = cutHorizontally(matrix, currentStart, matrix->lines - 1);
    }

    free(sums);
    return lines;
}

void get_spaces_val(int **sumsCols, Matrix **lines, size_t noLines,
        int spaces_val[])
{
    int lower = 0;
    int upper = 0;
    
    int firstchar_passed = 0;

    int currentspace_width = 0;

    for (size_t i = 0; i < noLines; i++) {
        firstchar_passed = 0;
        currentspace_width = 0;

        for (int j = 0; j < lines[i]->columns; j++) {
            if (!firstchar_passed && sumsCols[i][j] == 0) {
                continue;
            }
            if (!firstchar_passed && sumsCols[i][j] != 0) {
                firstchar_passed = 1;
                currentspace_width = 0;
                continue;
            }

            if (sumsCols[i][j] != 0 && currentspace_width != 0) {
                if (lower == 0 && upper == 0) {
                    lower = currentspace_width;
                    upper = currentspace_width;
                }
                else {
                    if (lower == upper) {
                        if (currentspace_width < lower)
                            lower = (lower + currentspace_width) / 2;
                        else
                            upper = (upper + currentspace_width) / 2;
                    }
                    else {
                        int dist_low = abs(lower - currentspace_width);
                        int dist_upp = abs(upper - currentspace_width);
                        if (dist_low < dist_upp)
                            lower = (lower + currentspace_width) / 2;
                        else
                            upper = (upper + currentspace_width) / 2;
                    }
                }
                currentspace_width = 0;
                continue;
            }
        
            if (sumsCols[i][j] == 0) {
                currentspace_width++;
            }
        }
    }

    spaces_val[0] = lower;
    spaces_val[1] = upper;
}

Matrix *crop_matrix(Matrix *m)
{
    int *sumLines = sumlines(m);

    int upper = 0;
    int lower = 0;

    for (int i = 0; i < m->lines; i++) {
        if (sumLines[i] != 0) {
            upper = i - 1;
            break;
        }
    }
    for (int i = m->lines - 1; i >= 0; i--) {
        if (sumLines[i] != 0) {
            lower = i + 1;
            break;
        }
    }

    free(sumLines);

    if (lower >= m->lines)
        lower = m->lines - 1;

    if (upper < 0)
        upper = 0;

    if (lower == m->lines - 1 && upper == 0)
        return NULL;

    return cutHorizontally(m, upper, lower);

}

void get_chars_from_line(Matrix *line, int *sumsCols, int spaces_val[], 
        struct txtelement *space, list *res)
{
    int currentStart = 0;
    int currentEnd = 0;
    int charWidth = 0;
    int first_char_encountered = 0;

    for(int i = 0; i <= line->columns; i++) {
        // first check : if end of a char or last column
        if((i == line->columns || sumsCols[i] == 0) && charWidth != 0) {
            charWidth = 0;
            currentEnd = i - 1;
            Matrix *charact = cutVertically(line, currentStart, currentEnd);

            // we crop the character matrix
            Matrix *cropped = crop_matrix(charact);
            if (cropped != NULL) {
                free(charact);
                charact = cropped;
            }
            
            // we make the matrix into a square            
            Matrix *new_m = square_matrix(charact);
            free(charact);
            charact = new_m;

            // we resize the matrix into a 26x26 matrix
            Matrix *smaller_m = character_normalize(charact);
            if (smaller_m != NULL) {
                free(charact);
                charact = smaller_m;
            }

            // we create the text element
            struct txtelement *txtelt = malloc(sizeof(struct txtelement));
            if (txtelt == NULL)
                errx(1, "not enough memory");

            txtelt->type = 0;
            txtelt->matrix = charact;

            // we create the list element
            list *elt = malloc(sizeof(list));
            if (elt == NULL)
                errx(1, "not enough memory");
            list_init(elt);

            // we put the text element in the list
            elt->data = txtelt;

            // we add the element to the end of the list
            list_push_tail(res, elt);
        }

        if (i == line->columns)
            break;

        if (sumsCols[i] != 0 && charWidth == 0) {
            currentStart = i;

            if (first_char_encountered) {
                int dist_low = abs(spaces_val[0] 
                        - (currentStart - currentEnd - 1));
                int dist_upp = abs(spaces_val[1] 
                        - (currentStart - currentEnd - 1));

                if (dist_upp < dist_low) {
                    list *nspace_elt = malloc(sizeof(list));
                    if (nspace_elt == NULL)
                        errx(1, "not enough memory");
                    list_init(nspace_elt);
                    nspace_elt->data = space;
                    list_push_tail(res, nspace_elt);
                }
            }

            else
                first_char_encountered = 1;
        }

        if (sumsCols[i] != 0) {
            charWidth++;
        }
    } 
}



list *segmentate(Matrix *matrix)
{
    size_t noLines = 0;
    Matrix **lines = getlines(matrix, &noLines);

    list *res = malloc(sizeof(list));
    if (res == NULL)
        errx(1, "not enough memory");
    list_init(res);
    
    struct txtelement *nline = malloc(sizeof(struct txtelement));
    if (nline == NULL)
        errx(1, "not enough memory");
    
    nline->matrix = NULL;
    nline->type = 2;

    struct txtelement *space = malloc(sizeof(struct txtelement));
    if (space == NULL)
        errx(1, "not enough memory");
    
    space->matrix = NULL;
    space->type = 1;
    
    int **sumsCols = malloc(sizeof(int*) * noLines);
    if (sumsCols == NULL)
        errx(1, "not enough memory");
    
    for (size_t i = 0; i < noLines; i++) {
        sumsCols[i] = sumcols(lines[i]);
    }

    int spaces_val[2] = {0, 0};
    get_spaces_val(sumsCols, lines, noLines, spaces_val);

    for (size_t i = 0; i < noLines; i++) {
        get_chars_from_line(lines[i], sumsCols[i], spaces_val, space, res);

        list *nline_elm = malloc(sizeof(list));
        if (nline == NULL)
            errx(1, "not enough memory");
        list_init(nline_elm);
        nline_elm->data = nline;
        list_push_tail(res, nline_elm);
        free(lines[i]);
    }

    free(lines);
    return res;
}
