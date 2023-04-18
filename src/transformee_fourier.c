#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

void FourierTransform(float* tab_amplitudes_temporel,int nb_data, double* ptab_amplitude_frequenciel);


int main ()
{

  float tab[128];
  for (int i = 0; i < 128; i++){
    tab[i] = i;
  }

  double* tab_amplitude_frequenciel = malloc(2*128*sizeof(double)); //nombres comples (2 floats pour partie réelle et imaginaire)
  FourierTransform(tab,128, tab_amplitude_frequenciel);
  for (int i = 0; i < 128; i++){
    printf ("A(t)=%f,    A(f)=(%f)+i(%f)\n", tab[i], REAL(tab_amplitude_frequenciel,i), IMAG(tab_amplitude_frequenciel,i));
  }
  free(tab_amplitude_frequenciel);
  return 0;
}



void FourierTransform(float* tab_amplitudes_temporel, int nb_data, double* ptab_amplitude_frequenciel){
  /*Transformée de Fourier d'un tableau d'amplitude*/

  for (int i = 0; i < nb_data; i++) {
      REAL(ptab_amplitude_frequenciel, i) = tab_amplitudes_temporel[i];
      IMAG(ptab_amplitude_frequenciel, i) = 0.0;
  }

  gsl_fft_complex_radix2_forward(ptab_amplitude_frequenciel, 1, nb_data);
}