#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "external/malloc_count/malloc_count.h"
#include "lib/Naive.h"
#include "lib/MM90.h"
#include "lib/DC3.h"
#include "lib/LCP.h"

void time_start(time_t *t_time, clock_t *c_clock)
{

  *t_time = time(NULL);
  *c_clock = clock();
}

double time_stop(time_t t_time, clock_t c_clock)
{

  double aux1 = (clock() - c_clock) / (double)(CLOCKS_PER_SEC);
  double aux2 = difftime(time(NULL), t_time);

  printf("CLOCK = %lf TIME = %lf\n", aux1, aux2);

  return aux1;
}

/****************/
int main(int argc, char *argv[])
{
  time_t t_start = 0;
  clock_t c_start = 0;

  char *Text = (char *)malloc(200 * sizeof(char));
  uint_t n;

  extern char *optarg;
  extern int optind, opterr, optopt;
  char *c_file = NULL;

  int c, alg = 0, pri = 0, time = 0, memo = 0;
  while ((c = getopt(argc, argv, "A:M:pt")) != -1)
  {
    switch (c)
    {
    case 'A':
      alg = (int)atoi(optarg);
      break;
    case 'M':
      memo = (int)atoi(optarg);
      break;
    case 'p':
      pri = 1;
      break;
    case 't':
      time = 1;
      break;
    default:
      break;
    }
  }
  free(optarg);

  if (optind + 1 == argc)
  {
    c_file = argv[optind++];
  }

  //====================
  // PREPROCESSING
  //====================

  if (time)
    time_start(&t_start, &c_start);
  printf("## PREPROCESSING ##\n");

  FILE *arq = NULL;
  arq = fopen(c_file, "r");
  if (arq == NULL)
  {
    perror("FALHA NA ABERTURA DO ARQUIVO");
    return 0;
  }
  fseek(arq, 0, SEEK_END);
  n = ftell(arq) + 1;
  rewind(arq);
  free(Text);
  Text = (char *)malloc(n * sizeof(char *));
  char *T_temp = (char *)malloc(n * sizeof(char));
  while (fscanf(arq, "%s", T_temp) != EOF)
  {
    strcat(Text, T_temp);
  }
  free(T_temp);
  fclose(arq);

  printf("N = %u bytes\n", n);
  printf("sizeof(int) = %zu bytes\n", sizeof(int_t));

  if (time)
    fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));

  // Text as int
  int_str *uText = (int_str *)malloc((n + 2) * sizeof(int_str));

  for (int_t i = 0; i < n - 1; i++)
    uText[i] = Text[i];
  uText[n - 1] = uText[n] = uText[n + 1] = '$';

  if (time)
    fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));

  //================================
  // ALLOCATION
  //================================
  int_sa *SA = (int_sa *)malloc(n * sizeof(int_sa));
  int_lcp *LCP = (int_lcp *)malloc(n * sizeof(int_lcp));
  for (int i = 0; i < n; i++)
  {
    SA[i] = 0;
    LCP[i] = 0;
  }

  switch (alg)
  {
  case 0:
    printf("## NAIVE_SA ##\n");
    //Naive_sa(Text, n, SA);
    printf("## NAIVE_LCP ##\n");
    //Naive_LCP(uText, n, SA, LCP);
    break;
  case 1:
    if(time) time_start(&t_start, &c_start);
    printf("## MM90_SA ##\n");
    MM(uText, n, SA);
    if (time) fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));
    if(time) time_start(&t_start, &c_start);
    printf("## KASAI_LCP ##\n");
    KASAI(uText, n, SA, LCP);
    if (time) fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));
    break;
  case 2:
    if(time) time_start(&t_start, &c_start);
    printf("## MM90_SA ##\n");
    MM(uText, n, SA);
    if (time) fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));
    if(time) time_start(&t_start, &c_start);
    printf("## PHI_LCP ##\n");
    PHI(uText, n, SA, LCP);
    if (time) fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));
    break;
  case 3:
    if(time) time_start(&t_start, &c_start);
    printf("## MM90_SA_+_LCP ##\n");
    MM_LCP(uText, n, SA, LCP);
    if (time) fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));
    break;
  case 4:
    if(time) time_start(&t_start, &c_start);
    printf("## DC3_SA ##\n");
    DC3(uText, n, SA, 255);
    if (time) fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));
    if(time) time_start(&t_start, &c_start);
    printf("## KASAI_LCP ##\n");
    KASAI(uText, n, SA, LCP);
    if (time) fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));
    break;
  case 5:
    if(time) time_start(&t_start, &c_start);
    printf("## DC3_SA ##\n");
    DC3(uText, n, SA, 255);
    if (time) fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));
    if(time) time_start(&t_start, &c_start);
    printf("## PHI_LCP ##\n");
    PHI(uText, n, SA, LCP);
    if (time) fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));
    break;
  case 6:
    if(time) time_start(&t_start, &c_start);
    printf("## DC3_SA_+_LCP ##\n");
    DC3_LCP(uText, n, SA, LCP, 255);
    if (time) fprintf(stderr, "%.6lf\n", time_stop(t_start, c_start));
    break;
  default:
    break;
  }

  if (pri == 1)
  {
    print(SA, LCP, uText, n);
  }

  // deallocate
  free(SA);
  free(LCP);
  free(uText);
  free(Text);

  return 0;
}

/****************/
