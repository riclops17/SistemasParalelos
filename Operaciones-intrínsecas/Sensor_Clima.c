#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>
#define NROSENSOR 2000 
#define N 86400 // segundos en un dia
int main() {
 double a;
 int i,j, t = 0;	
  double *vectorHumedadAire,*vectorTemperatura,*resultadoHumedad,*resultadoTemperatura; /* Punteros a memoria */
	int doublePorReg = 16/sizeof(double); 
  int puntDoublePorREg= 16 /sizeof(double*);
  double **contenedorSensor ; 
  double **resultados; 
   double it = NROSENSOR / puntDoublePorREg; 
  double it2 = N / doublePorReg ; 

	double iteraciones = N/doublePorReg; /* Cantidad de iteraciones del bucle vectorial */
    __m128d reg_actual, reg_2,reg_3,reg_4,reg_5,mayor_que1,menor_que2, mayor_que2,humedad1, temperatura1 ,temperatura2, valor1;
    __m128d contS, sens, sens2, temp1, temp2, temp3 , reg30 ;

    if(posix_memalign((void**)&vectorTemperatura,32,N*sizeof(double)) != 0)
      return 1;

    if(posix_memalign((void**)&resultadoTemperatura,32,N*sizeof(double)) != 0)
      return 1;  
    if(posix_memalign((void**)&vectorHumedadAire,32,N*sizeof(double)) != 0)
      return 1;

	 if(posix_memalign((void**)&resultadoHumedad,32,N*sizeof(double)) != 0)
      return 1;
   if(posix_memalign((void**)&contenedorSensor, 16, NROSENSOR*sizeof(double*)) != 0)
      return 1;
   
   if(posix_memalign((void**)&resultados, 16, NROSENSOR*sizeof(double)) != 0)
      return 1;

   srand(1u);


   /* Se debe respetar el orden siguiente de inicializacion:
   (1) sensor de temperatura aire, (2) sensor de humedad aire, (3) sensores de humedad suelo. */

   //Numeros para sensor de temperatura del aire [-40 ... 50]
   a = 90.0;
   for(i=0;i<N;i++){
       vectorTemperatura[i] =(((double)rand()/(double)(RAND_MAX)) * a - 40);
    }
   //Numeros para sensor de humedad del aire [0 ... 100]
   a = 100.0;
   for(i=0;i< N;i++){
   vectorHumedadAire[i] = (((double)rand()/(double)(RAND_MAX)) * a);
   }
    //Numeros para sensor de humedad del suelo [0 ... 100]
   a = 100.0;
   for( i=0 ; i < NROSENSOR; i++){
        double *sensor;
        if(posix_memalign((void**)&sensor, 16, N*sizeof(double)) != 0){/// modifico 16 por 32
        return 1;}
        for(j = 0 ; j < N; j++){
            
            
               sensor[j] =(((double)rand()/(double)(RAND_MAX)) * a) ; 
           
            
        } 
            contenedorSensor[i] = sensor;  
        }
    // humedad del suelo
    reg30 = _mm_set1_pd(30.00);
    valor1 = _mm_set1_pd(1.00);

//temperatura 20 y 30 grados celcius
	 temperatura1 = _mm_set1_pd(20.0);
	 temperatura2 = _mm_set1_pd(30.0);
 //BUCLE PARA LA TEMPERATURA
 for(j = 0; j < iteraciones; j++) {
        reg_actual = _mm_load_pd((double const*)vectorTemperatura+(j*doublePorReg)); ///////////////////////////////////////
         //_mm_cmpgt_pd 
        mayor_que2 = _mm_cmpgt_pd(reg_actual,temperatura1); // xffff si es mayor a 20 y 0 si es menor
		   menor_que2 = _mm_cmplt_pd(reg_actual,temperatura2); //xfff si es menor a 30 y 0  lo contrario

		   reg_3 = _mm_and_pd(valor1,mayor_que2);
        reg_4 = _mm_and_pd(valor1,menor_que2);

		  reg_5 = _mm_and_pd(reg_3, reg_4);

	   _mm_store_pd((double*)(resultadoTemperatura+(j*doublePorReg)),reg_5); /////////////////////////////////
		
 }


   // humedad de 75 %
    humedad1 = _mm_set1_pd(75.0);

    
    // sensor de humedad del aire
   for(i = 0; i < iteraciones; i++) {
        reg_actual = _mm_load_pd((double const*)vectorHumedadAire+(i*doublePorReg)); ///////////////////////////////////////
         //_mm_cmpgt_pd 
        mayor_que1 = _mm_cmpgt_pd(reg_actual,humedad1); // xffffff si es mayor a 75 y 0 si es menor
        reg_2 = _mm_and_pd(valor1,mayor_que1);
	   _mm_store_pd((double*)(resultadoHumedad+(i*doublePorReg)),reg_2); /////////////////////////////////
		
 }
 // bucle para sensor de la humedad del suelo
 int cont=0;
    for( i = 0; i < NROSENSOR; i++) {
        double* senRes; 
        if(posix_memalign((void**)&senRes, 16, N*sizeof(double)) != 0){
        return 1;}
        double *s = contenedorSensor[i]; 
      
        for( j = 0; j < it2; j++)
        {
            sens= _mm_load_pd((double const*)s + (j*doublePorReg));
            //printf(" %f ", sens[0]); 
            temp1= _mm_cmpgt_pd(sens,reg30);
            temp2= _mm_and_pd(valor1,temp1);
            //printf(" %f ", temp2[0]); 
           

            
            _mm_store_pd((double*)(senRes +(j*doublePorReg)),temp2);
             //printf("res %.2f \n", *(resultados+  cont * doublePorReg)); 
                
        }
        resultados[i]=senRes;
    }
        int contCORR = 0 ;
   


     for(i = 0; i < 5; i++) {
		printf("Valor-ResultadoT= (%f,%f)\n",vectorTemperatura[i],  resultadoTemperatura[i]);	
		
   	}
    
	   int cant3= 0;
	   int cant4=0;
	   for (i = 0; i < 5; i++) {
		 if(resultadoTemperatura[i] == 0.0){
			 cant3++;
		 }else{
			 cant4++;
		 }
		
   	}
    printf("la cantidad de valores incorrectos del sensor  temperatura es %d \n",cant3);

	printf("la cantidad de valores correctos del sensor  temperatura  %d",cant4);
    printf("//////////////////////////////////////////////\n");

    for(i = 0; i < 5; i++) {
		printf("Valor-ResultadoH = (%f,%f)\n",vectorHumedadAire[i],  resultadoHumedad[i]);	
		
   	}
    
	   int cant1= 0;
	   int cant2=0;
	   for(i = 0; i < 5; i++) {
		 if(resultadoHumedad[i] == 0.0){
			 cant1++;
		 }else{
			 cant2++;
		 }
		
   	}
    printf("la cantidad de valores incorrectos  del sensor humedadA es %d \n",cant1);

	  printf("la cantidad de valores correctos del sensor humedadA  %d \n",cant2);

    for(i = 0; i < 8; i++){///////////// cambio NroSENSOR POR 8 
        double* senRes;
        double* medicion;
        int corxSen=0;
        medicion = contenedorSensor[i];
        senRes = resultados[i];
        printf (" SENSOR  =  %d \n" , i ); 
        for( j = 0; j< 5 ; j++){ /// Cambio N por 5
            printf(" %2.f  =  %2.f \n" , medicion[j] , senRes[j] );
            if(senRes[j]==1.0){
                contCORR++;
                corxSen++;
            }
        }
        printf("CORRECTOS EN SENSOR %d =   %d \n" , i , corxSen);
    }
        
    printf("EN TOTAL HAY %d MEDICIONES CORRECTAS EN LOS SENSORES " , contCORR );  

   return 0;
}
