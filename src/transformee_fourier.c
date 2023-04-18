#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

void FourierTransform(short* tab_amplitudes_temporel, int nb_data, double* tab_amplitude_frequenciel){
  /*Transformée de Fourier d'un tableau d'amplitudes*/

  gsl_fft_complex_wavetable* wavetable;
  gsl_fft_complex_workspace* workspace;

  for (int i = 0; i < nb_data; i++) {
      REAL(tab_amplitude_frequenciel, i) = tab_amplitudes_temporel[i];
      IMAG(tab_amplitude_frequenciel, i) = 0.0;
  }

  wavetable = gsl_fft_complex_wavetable_alloc(nb_data);
  workspace = gsl_fft_complex_workspace_alloc(nb_data);

  gsl_fft_complex_forward(tab_amplitude_frequenciel, 1, nb_data, wavetable, workspace);


  gsl_fft_complex_wavetable_free(wavetable);
  gsl_fft_complex_workspace_free(workspace);
}

/*
int main ()
{
  float tab[44000];
  for (int i = 0; i < 44000; i++){
    tab[i] = i;
  }

  double* tab_amplitude_frequenciel = malloc(2*44000*sizeof(double)); //nombres comples (2 floats pour partie réelle et imaginaire)
  FourierTransform(tab,44000, tab_amplitude_frequenciel);
  for (int i = 0; i < 44000; i++){
    printf("i: %d , A(t)=%f,    A(f)=(%f)+i(%f)\n",i, tab[i], REAL(tab_amplitude_frequenciel,i), IMAG(tab_amplitude_frequenciel,i));
  }
  free(tab_amplitude_frequenciel);
  return 0;
}
*/
