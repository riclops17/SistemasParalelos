#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>

//1 millon de iteraciones
#define N 1000000

int main() {

	/* 
	Definicion de la funcion
		x <= 5 --> x+=3
		x > 5 --> x*= 2
	Ejemplo de entrada ==> salida
		2|10|5|12  ==>  5|20|8|24 
	*/

	// Declaración de variables
	int i, t = 0;	
	double *original, *resultado; /* Punteros a memoria */
	int doublePorReg = 16/sizeof(double); /* Cantidad de valores double por registro m128d */
	double iteraciones = N/doublePorReg; /* Cantidad de iteraciones del bucle vectorial */

	/* Registros para las instrucciones vectoriales */
	__m128d reg_actual, reg_suma, reg_mult, masc_less_equal, masc_greater, const_sum, const_mul, const_comp;


	/* Asignación de memoria alineada */
	if(posix_memalign((void**)&original, 16, N*sizeof(double)) != 0)
      return 1;

	if(posix_memalign((void**)&resultado, 16, N*sizeof(double)) != 0)
      return 1;


	/* Inicializacion de los vectores */
	for (i = 0; i < N; i++) {
		//original[i]=rand()%10; //Utilizar esta instrucción si quieren valores random entre 0 y 10;
		original[i] = 2;
		original[++i] = 10;
		original[++i] = 5;
		original[++i] = 12;
	}

	/* Carga de valores constantes en registros */
	const_sum = _mm_set1_pd(3.0);
	const_mul = _mm_set1_pd(2.0);
	const_comp = _mm_set1_pd(5.0);

	/* BUCLE VECTORIAL */
	for (i = 0; i < iteraciones; i++) {
		reg_actual = _mm_load_pd((double const*) original+(i*doublePorReg)); /* Carga desde memoria */

		reg_suma = _mm_add_pd(reg_actual, const_sum); /* +3 */
		reg_mult = _mm_mul_pd(reg_actual, const_mul); /* x2 */

 		
		masc_greater = _mm_cmpgt_pd(reg_actual,const_comp); /* Máscara >5 */
		masc_less_equal = _mm_cmple_pd(reg_actual,const_comp); /* Máscara <=5 */		

		reg_suma = _mm_and_pd(reg_suma,masc_less_equal); /* Filtra resultados correctos de la suma */
		reg_mult = _mm_and_pd(reg_mult,masc_greater); /* Filtra resultados correctos de la mult */

		reg_actual = _mm_or_pd(reg_suma, reg_mult); /* Une los resultados filtrados utilizando OR*/

		_mm_store_pd((double*)(resultado+(i*doublePorReg)), reg_actual); /* Guarda los resultados en memoria */
	}


	/* Verificación de los primeros 4 valores*/	
 	for (i = 0; i < 4; i++) {
		printf("Valor-Resultado = (%f,%f)\n",original[i],resultado[i]);	
   	}
   	
	


   return 0;
}











