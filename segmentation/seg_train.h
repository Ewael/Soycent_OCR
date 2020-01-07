#include <stdlib.h>
#include "xycut.h"
#include "../list/list.h"

size_t list_nbchars(list *l); 

Matrix **prepare_inputs(list **queues, size_t nb_images, size_t *nb);

Matrix **prepare_results(size_t nbchar);
