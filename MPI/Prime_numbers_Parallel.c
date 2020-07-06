#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <emmintrin.h>



void mergeOrdenaMuestra(int *arr1, int *des, int cantPrimos, int world_size){
//Ordena el arreglo de primos (solo para muestra, no reemplaza posiciones en memoria). 
//Aprovechando que cada proceso envio los subArreglos en orden. 
    int menor, i, aux, j ;
    i=0;
    menor=__INT_MAX__;
    int *desAux;
    desAux= (int *) malloc(world_size*sizeof(int)); 
    
//inicializacion de arreglo que contiene los topes para los desplazamientos. 
    while (i<world_size)
    {
     desAux[i]=des[i];   
     i++;
    }

    i=0;
    
    printf ("Primos Encontrados: [");
    //Metodo de ordenamiento
    while (i<cantPrimos)
    {
        //busca el menor primo en el primer elemento de cada apuntado por cada proceso

        //Analisis del primer hasta el anteultimo procesador
        for ( j = 0; j < world_size-1; j++)
        {
            if(desAux[j]<des[j+1]){
                if(arr1[desAux[j]]<menor){
                    menor=arr1[desAux[j]];
                    aux=j;
                }
            }
        }
        
        //Analisis del ultimo procesador(no se puede incluir en el for anterior porque no tiene tope)

        j= world_size-1;
        if(desAux[j]< cantPrimos){
                if(arr1[desAux[j]]<menor){
                    menor=arr1[desAux[j]];
                    aux=j;
                }
            }

        desAux[aux]++;    
        if(i<cantPrimos-1){
        printf(" %d," , menor);}
        else
        {
        printf(" %d ]" , menor);}   
        menor=__INT_MAX__;
        i++;
    }
    
        printf("\n");

}


int primo(int n){  
    int i, res, mitad;
    i=2;
    res = n > 1;
    mitad =n/2;
    while (res!=0 && i<= mitad)
    {
        res= n%i;
        i++;
    }
    return res; // devuelve 0 cuando no es primo y 1 cuando es primo
}


double sampleTime() {
 struct timespec tv;
 clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
 return ((double)tv.tv_sec+((double)tv.tv_nsec)/1000000000.0);
}

//--------------------------------------MAIN------------------------------------------
int main(int argc, char *argv[])		
{
    int rangoAux;

	int rank,i,j,esPrimo,world_size;
   
    
    
    int datos[2];
  
    
  
 
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
   
    if (rank == 0) {
      // le enviamos el rango de numeros por parametro
	    datos[0]= atoi(argv[1]);
        datos[1]= atoi(argv[2]);
         
        

    } 
    
   //------------------------Broadcast----------------------
    MPI_Bcast(&datos, 2, MPI_INT, 0, MPI_COMM_WORLD);
     
    double tiempo=sampleTime();
    
    
    int izq, der , IRango , FRango, contIzq, contDer, cantPrimos;
    //Asignamos espacio para  se guarden los primos suficientes.
    IRango= datos[0];
    FRango= datos[1];
    int *arreglo_enviar=(int*)malloc((FRango*0.20)*sizeof(int));
    int *arreglo_parteDerecha = (int*)malloc((FRango*0.10)*sizeof(int));
    cantPrimos=0;
    contIzq = 0;
    contDer = 0; 
  


    //Como no contemplamos a los pares pero 2 si es primo, si 2 esta en el rango, hacemos que el proceso 0 lo sume una unica vez.
    if(rank==0){
    if(2>= IRango && 2<=FRango){
     arreglo_enviar[0] = 2;
     cantPrimos= 1; 
     contIzq= 1 ;
    }}


    //El desplazamiento que planificamos necesita rangos pares, por lo que si toca algun rango impar lo acomodamos (no influye en resultado)
    if(!(IRango%2 == 0)){
        IRango-- ; }
    if(!(FRango%2 == 0)){
        FRango++ ; }
    
    
    //calcula el primer lugar del rango para cada procesador 
    izq= IRango + ((2 * rank +1));
    der= FRango - ((2 * rank +1));

        while (izq <= der)
        {       
            // se fija si los estremos izq y derecho asignados son primos
            //printf ("el procesos %d esta revisando si %d y  % d son primos o no \n" ,rank , izq , der); 
            if(izq<der){     
                esPrimo=primo(izq);
                if (esPrimo==1)
                {
                    //agregar numero a arreglo izq;
                    arreglo_enviar[contIzq]= izq;
                    cantPrimos++;
                    contIzq++;
                }
                esPrimo=primo(der);
                if (esPrimo==1)
                {
                    //agregar numero a a arreglo der;
                    arreglo_parteDerecha[contDer]= der;
                    cantPrimos++;
                    contDer++;
                }
            } else {
                // si izq = derecha solo revisamos uno
                esPrimo=primo(izq);
                if (esPrimo==1)
                {
                    //agregar numero a arreglo izq;
                    arreglo_enviar[contIzq]= izq;
                    cantPrimos++;
                    contIzq++;
                } 
            } 
                //calculamos el proximo desplazamiento de cada procesador dependiendo con la cantidad que se este trabajando.
                izq = izq + (world_size * 2); 
                der = der - (world_size * 2);
            }

     
    tiempo = sampleTime() - tiempo;
    printf("Soy el proceso número: %d  mi tiempo fue de: %f \n",rank,tiempo);
    printf ("Soy el proceso %d encontro :  %d numeros primos \n" , rank, cantPrimos); 
    
    //unimos los arreglos izq y derecha de manera que queden en orden y tratando de hacer la menor cantidad de movimientos
    while (contIzq<cantPrimos)
    {
        arreglo_enviar[contIzq]=arreglo_parteDerecha[contDer-1];
        contDer--;
        contIzq++;
    }
    
   

   

  
   //-------------------------------------- PARA EL RECVCOUNT---------------------//
   //Almacena la cantidad que va a mandar cada proceso
    MPI_Barrier(MPI_COMM_WORLD); 
    
    int *arreglo_cantidades;
    if(rank==0){
    arreglo_cantidades= (int *) malloc(world_size*sizeof(int));
    }
    MPI_Gather(&cantPrimos,1, MPI_INT, arreglo_cantidades,1, MPI_INT, 0, MPI_COMM_WORLD);
   //---------------------------------PARA LOS DESPLAZAMIENTOS--------------------------------------//
   /* 
	Almacena los desplazamientos para el arreglo de destino */
    int aux = 0;
	int *arreglo_desplazamientos;
    if (rank==0){
    arreglo_desplazamientos = (int *) malloc(world_size*sizeof(int));
 	for (i = 0; i<world_size; i++){
		arreglo_desplazamientos[i]=aux;
		aux += arreglo_cantidades[i];
	}}
   //---------------------------------------Para el RECVBUFFER--------------------------------------//
   /*
	Almacena espacio para los resultados. Solo lo realiza el master
	Primero debe calcular la suma de todas las cantidades a recibir (recibidas con el gather en recvcounts)*/
    int cant_total = 0;
	int *resultado;
	if(rank == 0){
	 	for (i = 0; i<world_size; i++){
			cant_total += arreglo_cantidades[i];
		}
		resultado = (int *) malloc(cant_total*sizeof(int));
	}

    

   //               SENDBUFF     SENDCOUNT          RECVBUFFER    RECVCOUNT         DESPLAZAMIENTOS            ROOT
    MPI_Barrier(MPI_COMM_WORLD); 
    MPI_Gatherv(arreglo_enviar,cantPrimos,MPI_INT,resultado,arreglo_cantidades,arreglo_desplazamientos,MPI_INT,0,MPI_COMM_WORLD);
    
	if(rank == 0){
		printf("Cantidad de procesos: %d\n",world_size);
		printf("Cantidad de primos recibidos con gatherv: %d\n", cant_total);
        
		//Metodo para mostrar los primos, sub arreglos ordenados pero arreglo general no ordenado
        /*
        printf("Arreglo recibido: [");
		for (i = 0; i < cant_total-1 ; i++)
		{
			printf("%d,",resultado[i]);
		}
		printf("%d]\n",resultado[i]); */

        //Metodo para mostrar los primos en orden. pensado como una variable de un marge sort pero de implementacion propia pensada para el ejercicio.
        
        mergeOrdenaMuestra(resultado,arreglo_desplazamientos,cant_total,world_size);
    
    }
    

    MPI_Finalize();


	return 0;
}
