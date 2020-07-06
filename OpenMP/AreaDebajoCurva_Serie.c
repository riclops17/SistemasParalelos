#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

double sampleTime(){
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC_RAW,&tv);
    return ((double)tv.tv_sec+((double)tv.tv_nsec)/1000000000.0);
}
double funcion(double x){
      x = x+1;
     return x;
 }
int main(){
 double t,h,aprox,x;
 int i,a,b,n;
 //puntos a y b
 n = 12500000;
 a = 1;
 b = 2000000000;
 h =(b-a)/n;
 aprox = (funcion(a)+ funcion(b))/2.0;
  t = sampleTime();
  //region
 for(i = 1; i<= n-1 ;i++){
     x = a+i*h;
     aprox = aprox + funcion(x);
 }
 aprox = h*aprox;
 t = sampleTime() - t;
 printf("el area bajo la curva es %f\n",aprox);
 printf("tiempo transcurrido: %f segundos\n",t);
 return 0;
}
