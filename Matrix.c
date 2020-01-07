#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Matrix.h"

/* 
 * Prints a matrix m
 */
void print_matrix(Matrix *m)
{
	for(int i=0; i < m->length; i++)
	{
		printf("%f ",m->list[i]);
    	if ((i+1) % m->columns == 0) //end of a line
		{
			printf("\n");
		}
	}
}

/* 
 * Prints a list m of 'length' (how many) matrices
 */
void print_matrices(Matrix **m, size_t length)
{
	printf("----------------\n");
	for (size_t i=0; i < length; i++)
	{
		print_matrix(m[i]);
		printf("    -     \n");
	}
	printf("----------------\n");
}

 /*
  * Gauss distrib
  */
 static double gauss(void)
 {
   double x = (double)random() / RAND_MAX,
          y = (double)random() / RAND_MAX,
          z = sqrt(-2 * log(x)) * cos(2 * M_PI * y);
   return z;
 }

/* 
 * Returns a matrix col*lines 
 * with 0 if !(random) and 
 * random double values in [-2,5;2,5] else
 */
Matrix *init_matrix(int col, int lines, int random)
{
	Matrix *m = malloc(sizeof(Matrix) + sizeof(double) * col * lines);
	m->columns = col;
	m->lines = lines;
	m->length = col*lines;

	if (!random) //matrix of 0
	{
		for(int i=0; i < m->length; i++)
		{
			m->list[i] = 0;
		}
		return m;
	}
	
	//srand must only be called once, has been moved to main
	for(int i=0; i < m->length; i++) //random double values
	{
		m->list[i] = gauss()*0.5;
		// m->list[i] = (double) rand() / RAND_MAX * (5-(-5)) + (-5);
	}

	return m;
}

/* 
 * Returns a matrix col*lines 
 * with 1 only
 */
Matrix *init_ones(int col, int lines)
{
	Matrix *m = malloc(sizeof(Matrix) + sizeof(double) * col * lines);
	m->columns = col;
	m->lines = lines;
	m->length = col*lines;
	for(int i=0; i < m->length; i++)
		{
			m->list[i] = 1;
		}
	return m;
}

/*
 * Returns a matrix 1*1 with the a value (double)
 */
Matrix *onebyone_matrix(double a)
{
	Matrix *m = init_matrix(1, 1, 0);
	m->list[0] = a;
	return m;
}

/*
 * Returns the transpose of m
 */
Matrix *transpose(Matrix *m)
{
	Matrix *res = init_matrix(m->lines, m->columns, 0);
	for (int k=0; k < m->lines; k++)
	{
		for (int i=0; i < m->columns; i++)
		{
			res->list[i*m->lines + k] = m->list[k*m->columns + i];
		}
	}
	return res;
}

/* 
 * Returns m + n matrix
 */
Matrix *add_matrix(Matrix *m, Matrix *n)
{
	//different dimensions -> return m
	if (m->columns != n->columns || m->lines != n->lines)
	{
		printf("\nAdded two matrices with different dimensions\n\n");
		return m;
	}


	Matrix *res = init_matrix(m->columns, n->lines, 0); //result
	for (int i=0; i < m->length; i++)
	{
		res->list[i] = m->list[i] + n->list[i];
	}
	return res;
	
}

/* 
 * Returns m - n matrix
 */
Matrix *sub_matrix(Matrix *m, Matrix *n)
{
	//different dimensions -> return m
	if (m->columns != n->columns || m->lines != n->lines)
	{
		printf("\nSubtracted two matrices with different dimensions\n\n");
		return m;
	}


	Matrix *res = init_matrix(m->columns, n->lines, 0); //result
	for (int i=0; i < m->length; i++)
	{
		res->list[i] = m->list[i] - n->list[i];
	}
	return res;
}

/*
 * Returns m*n matrix
 */
Matrix *multiply_matrix(Matrix *m, Matrix *n)
{
	if (m->columns != n->lines) //checking dimensions
	{
		printf("\nTrying to multiply matrices with wrong dimensions\n");
		return m;
	}
	
	Matrix *res = init_matrix(n->columns, m->lines, 0); // Result
	double sum = 0;
    int c = 0; //c is the index of each element placed in res->list
	double a = 0;
	double b = 0;

	for (int x=0; x < m->lines; x++)
	{
		for (int y=0; y < n->columns; y++)
		{
			sum = 0;
			for (int z=0; z < n->lines; z++)
			{
				a = m->list[z + x*m->columns];
				b = n->list[y + z*n->columns];
				sum += a * b;
			}
			
			res->list[c++] = sum;
		}
	}
	return res;
}

/*
 * Returns a * m where type(a) = double
 */
Matrix *mult_matrix(double a, Matrix *m)
{
	Matrix *res = init_matrix(m->columns, m->lines, 0);

	for (int i=0; i < m->length; i++)
	{
		res->list[i] = (a * m->list[i]);
	}
	return res;
}

/*
 * Returns sigmoid function on x
 */
double sigmoid(double x)
{
	double exp_value;
	double return_value;
	exp_value = exp((double) -x);
	return_value = 1 / (1 + exp_value);
	return return_value;
}

/*
 * Returns derivated sigmoid function on x
 */
double d_sigmoid(double x)
{
	double exp_value = exp((double) -x);
	return (exp_value / ((1 + exp_value)*(1 + exp_value)));
}

/*
 * Returns sigmoid function on matrix m = on each element of m
 */
Matrix *sigmoid_matrix(Matrix *m)
{
	Matrix *res = init_matrix(m->columns, m->lines, 0); //result
	for (int i=0; i < m->length; i++)
	{
		res->list[i] = sigmoid(m->list[i]);
	}
	return res;
}

/*
 * Returns derivated sigmoid function on m = on each elmt of m
 */
Matrix *d_sigmoid_matrix(Matrix *m)
{
	Matrix *res = init_matrix(m->columns, m->lines, 0); //result
	for (int i=0; i < m->length; i++)
	{
		res->list[i] = d_sigmoid(m->list[i]);
	}
	return res;
}

/*
 * Returns Hadamart product between m and n = m ⊙ n
 */
Matrix *hada_product(Matrix *m, Matrix *n)
{
	//different dimensions -> return m
	if (m->columns != n->columns || m->lines != n->lines)
	{
		printf("\nError: Two matrices have different dimensions\n\n");
		return m;
	}


	Matrix *res = init_matrix(m->columns, n->lines, 0); //result
	for (int i=0; i < m->length; i++)
	{
		res->list[i] = m->list[i] * n->list[i];
	}
	return res;
}

/*
 * Returns list of a - b where
 * a,b are the matrices in m,n
 */	
Matrix **sub_listofmatrices(Matrix **m, Matrix **n, size_t length)
{
	Matrix **res = malloc((length) * sizeof(Matrix*)); //result
	for (size_t i=0; i<length; i++)
	{
		res[i] = sub_matrix(m[i], n[i]);
	}
	return res;
}

/*
 * Returns list of a + b where
 * a,b are the matrices in m,n
 */
Matrix **add_listofmatrices(Matrix **m, Matrix **n, size_t length)
{
	Matrix **res = malloc((length) * sizeof(Matrix*)); //result
	for (size_t i=0; i<length; i++)
	{
		res[i] = add_matrix(m[i], n[i]);
	}
	return res;
}

/*
 * Returns list of a*x where
 * type(a) = double and x are the matrices in m
 */
Matrix **mult_listofmatrices(double a, Matrix **m, size_t length)
{
	Matrix **res = malloc((length) * sizeof(Matrix*)); //result
	for (size_t i=0; i<length; i++)
	{
		res[i] = mult_matrix(a, m[i]);
	}
	return res;
}

/*
 * Frees a list of matrices
 */
void free_matrices(Matrix **m, size_t length)
{
	for (size_t i=0; i<length; i++)
	{
		free(m[i]);
	}
	free(m);
}

/*
 * Frees a list of matrices (from start_index to end_index)
 */
void free_matrices_range(Matrix **m, size_t start_index, size_t end_index)
{
	for (size_t i=start_index; i<end_index; i++)
	{
		free(m[i]);
	}
	free(m);
}

/* 
 * Returns a copy of m
 */
Matrix *copy_matrix(Matrix *m)
{
	Matrix *res = init_matrix(m->columns, m->lines, 0);
	for (int i=0; i < m->length; i++)
	{
		res->list[i] = m->list[i];
	}
	return res;
}

/*
 * Returns relu function on x
 */
double relu(double x)
{
	if (x < 0)
		return 0;
	return x;
}

/*
 * Returns derivative relu function on x
 */
double d_relu(double x)
{
	if (x < 0)
		return 0;
	return 1;
}

/*
 * Returns relu function on matrix m = on each element of m
 */
Matrix *relu_matrix(Matrix *m)
{
	Matrix *res = init_matrix(m->columns, m->lines, 0); //result
	for (int i=0; i < m->length; i++)
	{
		res->list[i] = relu(m->list[i]);
	}
	return res;
}

/*
 * Returns derivated relu function on m = on each elmt of m
 */
Matrix *d_relu_matrix(Matrix *m)
{
	Matrix *res = init_matrix(m->columns, m->lines, 0); //result
	for (int i=0; i < m->length; i++)
	{
		res->list[i] = d_relu(m->list[i]);
	}
	return res;
}

/*
 * Returns derivative tanh function on x
 */
double d_tanh(double x)
{
	return (1 - (tanh(x)*tanh(x)));
}

/*
 * Returns tanh function on matrix m = on each element of m
 */
Matrix *tanh_matrix(Matrix *m)
{
	Matrix *res = init_matrix(m->columns, m->lines, 0); //result
	for (int i=0; i < m->length; i++)
	{
		res->list[i] = tanh(m->list[i]);
	}
	return res;
}

/*
 * Returns derivated tanh function on m = on each elmt of m
 */
Matrix *d_tanh_matrix(Matrix *m)
{
	Matrix *res = init_matrix(m->columns, m->lines, 0); //result
	for (int i=0; i < m->length; i++)
	{
		res->list[i] = d_tanh(m->list[i]);
	}
	return res;
}

/*
 * Prints a letter in its matrix form
 */
void print_letter(Matrix *m)
{
    for(int i=0; i < m->length; i++)
    {
        if (m->list[i] > 0)
            printf("1 ");
        else
            printf(". ");
        if ((i+1) % 26 == 0) //end of a line
        {
            printf("\n");
        }
    }
}