#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>

//1 millon de iteraciones
#define N 1000000

int main() {


	/*
		16 bytes son 128 bits, el tamaño del registro 
		Los double ocupan 8 bytes --> Entran dos double por registro
	*/

	//Variables
	int i, t = 0, doublePorReg = 16/sizeof(double);
	double *originalX, *originalY, *resX, *resY;
	double iteraciones = N/doublePorReg; 

	//Registros
	__m128d orig_x, orig_y, aux_x, aux_y, reg_normales;

	if(posix_memalign((void**)&originalX, 16, N*sizeof(double)) != 0)
      return 1;

	if(posix_memalign((void**)&originalY, 16, N*sizeof(double)) != 0)
      return 1;

	if(posix_memalign((void**)&resX, 16, N*sizeof(double)) != 0)
      return 1;

	if(posix_memalign((void**)&resY, 16, N*sizeof(double)) != 0)
      return 1;

	//Inicializacion de los vectores
   for (i = 0; i < N; i++) {
      originalX[i] = 6;
      originalY[i] = 8;
   }

	//Bucle Vectorial
   for (i = 0; i < iteraciones; i++) {
		orig_x = _mm_load_pd ((double const*) (originalX+(i*doublePorReg))); //Carga desde memoria
		orig_y = _mm_load_pd ((double const*)  (originalY+(i*doublePorReg))); //Carga desde memoria

		aux_x = _mm_mul_pd (orig_x, orig_x); //Eleva al cuadrado multiplicando x*x
		aux_y = _mm_mul_pd (orig_y, orig_y);  //Eleva al cuadrado multiplicando y*y

		reg_normales = _mm_add_pd (aux_x, aux_y); //Suma los componentes cuadraticos
		reg_normales = _mm_sqrt_pd(reg_normales); //Realiza la raiz cuadrada -> obtiene las normales

		aux_x = _mm_div_pd (orig_x, reg_normales);//Divide el componente original por la normal para calcular 'x' normalizado
		aux_y = _mm_div_pd (orig_y, reg_normales);//Divide el componente original por la normal para calcular 'y' normalizado

		_mm_store_pd ((double*) (resX+(i*doublePorReg)), aux_x); //Guarda los resultados de X en memoria
		_mm_store_pd ((double*) (resY+(i*doublePorReg)), aux_y); //Guarda los resultados de Y en memoria
   }

	//Como todos los vectores son iguales, imprime el primero y el ultimo para verificar
	printf("Vector original = (%f,%f)\n",originalX[0],originalY[0]);
	printf("Vector normalizado = (%f,%f)\n",resX[0],resY[0]);

	printf("Vector original = (%f,%f)\n",originalX[N-1],originalY[N-1]);
	printf("Vector normalizado = (%f,%f)\n",resX[N-1],resY[N-1]);

	//Liberación de memoria
	free(originalX); 
	free(originalY);
	free(resX); 
	free(resY);

   return 0;
}











