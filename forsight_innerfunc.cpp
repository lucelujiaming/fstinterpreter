// #include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h" 

//        A 三角函数
//    01. double sin (double);
//    02. double cos (double);
//    03. double tan (double);
//        B 反三角函数
//    04. double asin (double);
//    05. double acos (double);
//    06. double atan (double);
//    07. double atan2 (double, double);
//        C 双曲三角函数
//    08. double sinh (double);
//    09. double cosh (double);
//    10. double tanh (double);
//        D 指数与对数
//    11. double exp (double);
//    12. double pow (double, double);
//    13. double sqrt (double);
//    14. double log (double);
//    15. double log10 (double);
//        E 取整
//    16. double ceil (double);
//    17. double floor (double);
//        F 绝对值
//    18. double fabs (double);
//        G 标准化浮点数
//    19. double frexp (double f, int *p);
//    20. double ldexp (double x, int p);
//        H 取整与取余
//    21. double modf (double, double*);
//    22. double fmod (double, double);
double call_sin  (char * valFirst, char * valSecond);
double call_cos  (char * valFirst, char * valSecond);
double call_tan  (char * valFirst, char * valSecond);
double call_asin (char * valFirst, char * valSecond);
double call_acos (char * valFirst, char * valSecond);
double call_atan (char * valFirst, char * valSecond);
double call_atan2(char * valFirst, char * valSecond);
double call_sinh (char * valFirst, char * valSecond);
double call_cosh (char * valFirst, char * valSecond);
double call_tanh (char * valFirst, char * valSecond);
double call_exp  (char * valFirst, char * valSecond);
double call_pow  (char * valFirst, char * valSecond);
double call_sqrt (char * valFirst, char * valSecond);
double call_log  (char * valFirst, char * valSecond);
double call_log10(char * valFirst, char * valSecond);
double call_ceil (char * valFirst, char * valSecond);
double call_floor(char * valFirst, char * valSecond);
double call_fabs (char * valFirst, char * valSecond);
// double call_frexp(char * valFirst, char * valSecond);
double call_ldexp(char * valFirst, char * valSecond);
double call_modf (char * valFirst, char * valSecond);
double call_fmod (char * valFirst, char * valSecond);


// This structure links a library function name   
// with a pointer to that function.   
struct intern_func_type {   
    char *f_name; // function name
    int param_num ;
    double (*p)(char* , char* = 0);   // pointer to the function   
} intern_func[] = {   
    (char *)"sin",   1, call_sin  ,
    (char *)"cos",   1, call_cos  ,
    (char *)"tan",   1, call_tan  ,
    (char *)"asin",  1, call_asin ,
    (char *)"acos",  1, call_acos ,
    (char *)"atan",  1, call_atan ,
    (char *)"atan2", 2, call_atan2,
    (char *)"sinh",  1, call_sinh ,
    (char *)"cosh",  1, call_cosh ,
    (char *)"tanh",  1, call_tanh ,
    (char *)"exp",   1, call_exp  ,
    (char *)"pow",   2, call_pow  ,
    (char *)"sqrt",  1, call_sqrt ,
    (char *)"log",   1, call_log  ,
    (char *)"log10", 1, call_log10,
    (char *)"ceil",  1, call_ceil ,
    (char *)"floor", 1, call_floor,
    (char *)"fabs",  1, call_fabs ,
//    (char *)"frexp", 2, call_frexp,
    (char *)"ldexp", 2, call_ldexp,
    (char *)"modf",  2, call_modf ,
    (char *)"fmod",  2, call_fmod ,
    (char *)"",      0, 0 , 
};

int find_internal_func(char *s)   
{   
    int i;
    for(i=0; intern_func[i].f_name[0]; i++) {
        if(!strcmp(intern_func[i].f_name, s))  return i;   
    }   
    return -1;   
}   

double call_internal_func(int index, char * valFirst, char * valSecond)   
{   
    // int i; 
    if(index >= 0)
		return (*intern_func[index].p)(valFirst, valSecond);
	else 
		return -1 ;
}

double call_sin  (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return sin(val) ;
}

double call_cos  (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return cos(val) ;
}

double call_tan  (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return tan(val) ;
}

double call_asin (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return asin(val) ;
}

double call_acos (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return acos(val) ;
}

double call_atan (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return atan(val) ;
}

double call_atan2(char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
	double valTwo = atof(valSecond);
    return atan2(val, valTwo) ;
}

double call_sinh (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return sinh(val) ;
}

double call_cosh (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return cosh(val) ;
}

double call_tanh (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return tanh(val) ;
}

double call_exp  (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return exp(val) ;
}

double call_pow  (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
	int   valTwo = atoi(valSecond);
    return pow(val, valTwo) ;
}

double call_sqrt (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return sqrt(val) ;
}

double call_log  (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return log(val) ;
}

double call_log10(char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return log10(val) ;
}

double call_ceil (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return ceil(val) ;
}

double call_floor(char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return floor(val) ;
}

double call_fabs (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
    return fabs(val) ;
}

double call_ldexp (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
	int    valTwo = atoi(valSecond);
    return ldexp(val, valTwo) ;
}

// 返回参数的小数部分, 整数部分不回传。
double call_modf (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
	double valTwo ;
    return modf(val, &valTwo) ;
}

double call_fmod (char * valFirst, char * valSecond)
{
	double val = atof(valFirst);
	double valTwo = atof(valSecond);
    return fmod(val, valTwo) ;
}
