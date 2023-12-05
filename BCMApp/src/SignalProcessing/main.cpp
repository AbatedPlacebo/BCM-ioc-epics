#include "oscinterp.h"

int main (int argc, char* argv[])
{
  int size;
  FILE* fp;
  fp = fopen("BCM.signal.dat", "r");
  char p[100];
  fscanf(fp, "%s %d",p, &size);
  gsl_vector* arr = gsl_vector_alloc(size);
  for (int i = 0; i < size; i++){
    int val;
    fscanf(fp, "%d", &val);
    gsl_vector_set(arr, i, val);
  }
  int Nroots = find_roots(arr->data, arr->size, rootsx);

  if (argc > 1)
  {
    if (strcmp(argv[1], "-roots") == 0)
    {
      for (int i = 0; i < Nroots; i++)
      {
        printf ("%d %g\n", i, rootsx[i]);
      }
    }
  }
  interpolate();
  gsl_vector_free(arr);
  fclose(fp);
  return 0;
}

