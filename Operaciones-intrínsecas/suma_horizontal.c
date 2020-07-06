#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>

#define N 10000000

int main() {
	int i,  cantElemReg = 16/sizeof(float);
	float *vector, *resultado;
	float iteraciones,sumaTotal=0;

	__m128 reg_a, reg_b;

	if(posix_memalign((void**)&vector, 32, N*sizeof(float)) != 0)
      return 1;

	if(posix_memalign((void**)&resultado, 32, 4*sizeof(float)) != 0)
      return 1;

	//Inicializacion del vector
  	for (i = 0; i < N; i++) {
      vector[i] = i;
  	}

	//Como un float ocupa 32 bits, en un registro de 128 entran 4 floats -> Calculado en cantElemReg
	iteraciones=N/cantElemReg;

	/*
	El funcionamiento es ir acumulando en reg_a los resultados de las iteraciones
	reg_a = reg_a + reg_b
	La primera carga de reg_a se hace fuera del bucle
	*/

	//Carga inicial
	reg_a = _mm_load_ps((float const*)vector); 

	//BUCLE VECTORIAL
	for(i=1;i<iteraciones;i++){
		reg_b = _mm_load_ps((float const*)vector+(i*cantElemReg));
		reg_a = _mm_add_ps(reg_a,reg_b);				
	}

	//Se guarda en memoria el registro con las sumas parcialess
	_mm_store_ps((float*)resultado, reg_a);


	//Suma final secuencial
   	for (i = 0; i < cantElemReg; i++) {
      sumaTotal += resultado[i];
   	}

	printf("Suma total= %.2f\n",sumaTotal); //%.2f imprime solo 2 digitos decimales del numero flotante
   	
	return 0;
}






