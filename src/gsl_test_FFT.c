#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

void FourierTransform(float* tab_amplitudes_temporel,int nb_data, float** ptab_amplitude_frequenciel);


int main ()
{
  /*
  int i;
  double data[2*128];

  for (i = 0; i < 128; i++)
    {
       REAL(data,i) = 0.0;
       IMAG(data,i) = 0.0;
    }

  REAL(data,0) = 1.0;

  for (i = 1; i <= 10; i++)
    {
       REAL(data,i) = REAL(data,128-i) = 1.0;
    }

  for (i = 0; i < 128; i++)
    {
      printf ("%d %e %e\n", i, REAL(data,i), IMAG(data,i));
    }
  printf ("\n\n");

  gsl_fft_complex_radix2_forward (data, 1, 128);

  for (i = 0; i < 128; i++)
    {
      printf ("%d %f %f\n", i, REAL(data,i)/sqrt(128), IMAG(data,i)/sqrt(128));
    }*/

  float tab[128];
  for (int i = 0; i < 128; i++){
    tab[i] = i;
  }

  float* tab_amplitude_frequenciel = malloc(128*sizeof(float));

  FourierTransform(tab,128, &tab_amplitude_frequenciel);
  printf("Test1 : \n");
  for (int i = 0; i < 128; i++){
    printf("Test : ");
    printf ("A(t)=%f,    A(f)=(%f)+i(%f)\n", tab[i], REAL(tab_amplitude_frequenciel,i), IMAG(tab_amplitude_frequenciel,i));
  }
  free(tab_amplitude_frequenciel);
  return 0;
}





void FourierTransform(float* tab_amplitudes_temporel,int nb_data, float** ptab_amplitude_frequenciel){
  /*Transformée de fourrier d'un tableau d'amplitude*/

  double* data = malloc(sizeof(float)*2*nb_data);

  for (int i = 0; i < nb_data; i++)
    {
       REAL(data,i) = tab_amplitudes_temporel[i];
       IMAG(data,i) = 0.0;
    }

    gsl_fft_complex_radix2_forward (data, 1, nb_data);

    for (int i = 0; i < nb_data; i++)
    {
      printf("%d",i);
      REAL(*ptab_amplitude_frequenciel,i) = REAL(data,i);
      IMAG(*ptab_amplitude_frequenciel,i) = IMAG(data,i);
    }
    free(data);
}