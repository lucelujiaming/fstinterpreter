// #include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h" 
#include "forsight_innerfunc.h"
#include <algorithm> 

#define RAD2DEG(x) ((x)*180./PI)  // Convert radians to angles
#define DEG2RAD(x) ((x)*PI/180.)  // Convert angle to radians

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
bool call_sin  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_cos  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_tan  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_asin (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_acos (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_atan (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_atan2(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_sinh (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_cosh (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_tanh (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_exp  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_pow  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_sqrt (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_log  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_log10(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_ceil (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_floor(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_fabs (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
// bool call_frexp(char * valFirst, char * valSecond, char * valThird);
bool call_ldexp(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_modf (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_fmod (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_hypot(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_gcd  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_lcm  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
// Convert function
bool call_degrees(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_radians(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
// Year/Month/Day function
bool call_isleapyear(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_getmaxday(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_getdays(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
//        I 字符串操作函数
//    23. strlen  (char *);
//    24. findstr (char *, char *);
//    25. substr  (char *, char *, char *);
bool call_strlen     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_findstr    (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_substr     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
//    26. replace    (char *, char *, char *); // 12212, 12, 21 ->  21212 
//    27. replaceall (char *, char *, char *); // 12212, 12, 21 ->  21221 
bool call_replace    (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_replaceall (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
//    28. replacealliteration (char *, char *, char *);  // 12212, 12, 21 ->  22211
bool call_replaceall_iteration
					 (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
//    28. lower  (char *);  
//    28. upper  (char *);  
//    28. revert (char *);  
bool call_lower      (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_upper      (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_revert     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);

// String convert function of <stdlib.h>
bool call_atoi     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_atof     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_itoa     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
bool call_ftoa     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);
// bool call_gcvt     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird);

// This structure links a library function name   
// with a pointer to that function.   
struct intern_func_type {   
    char *f_name; // function name
    int param_num ;
    bool (*p)(eval_value *, const char* , const char* = NULL, const char* = NULL);   // pointer to the function   
} intern_func[] = {   
	(char *)"sin",        1, call_sin  ,
	(char *)"cos",        1, call_cos  ,
	(char *)"tan",        1, call_tan  ,
	(char *)"asin",       1, call_asin ,
	(char *)"arcsin",     1, call_asin ,
	(char *)"acos",       1, call_acos ,
	(char *)"arccos",     1, call_acos ,
	(char *)"atan",       1, call_atan ,
	(char *)"arctan",     1, call_atan ,
	(char *)"atan2",      2, call_atan2,
	(char *)"sinh",       1, call_sinh ,
	(char *)"cosh",       1, call_cosh ,
	(char *)"tanh",       1, call_tanh ,
	(char *)"exp",        1, call_exp  ,
	(char *)"pow",        2, call_pow  ,
	(char *)"sqrt",       1, call_sqrt ,
	(char *)"log",        1, call_log  ,
	(char *)"log10",      1, call_log10,
	(char *)"ceil",       1, call_ceil ,
	(char *)"floor",      1, call_floor,
	(char *)"fabs",       1, call_fabs ,
	(char *)"abs",        1, call_fabs ,
//  (char *)"frexp",      2, call_frexp,
	(char *)"ldexp",      2, call_ldexp,
	(char *)"modf",       2, call_modf ,
	(char *)"fmod",       2, call_fmod ,
	(char *)"hypot",      2, call_hypot,
	(char *)"gcd",        2, call_gcd,
	(char *)"lcm",        2, call_lcm,
    // Convert function
	(char *)"degrees",    1, call_degrees,
	(char *)"radians",    1, call_radians,
    // Convert function
	(char *)"isleapyear", 1, call_isleapyear,
	(char *)"getmaxday",  1, call_getmaxday,
	(char *)"getdays",    1, call_getdays,
	// String function
	(char *)"strlen",     1, call_strlen ,
	(char *)"findstr",    2, call_findstr ,
	(char *)"substr",     3, call_substr ,
	(char *)"replace",    3, call_replace ,
	(char *)"replaceall", 3, call_replaceall ,
	(char *)"replacealliteration", 
	                      3, call_replaceall_iteration ,
	(char *)"lower",      1, call_lower ,
	(char *)"upper",      1, call_upper ,
	(char *)"revert",     1, call_revert ,
	// String convert function of <stdlib.h>
	(char *)"atoi",      1, call_atoi ,
	(char *)"atof",      1, call_atof ,
	(char *)"itoa",      1, call_itoa ,
	(char *)"ftoa",      1, call_ftoa ,
// 	(char *)"gcvt",      1, call_gcvt ,
    (char *)"",           0, 0 , 
};

int find_internal_func(char *s)   
{   
    int i;
    for(i=0; intern_func[i].f_name[0]; i++) {
        if(!strcmp(intern_func[i].f_name, s))  return i;   
    }   
    return -1;   
}   

int get_func_params_num(int iIdx)   
{  
	if(iIdx <= (int)(sizeof(intern_func)/sizeof(struct intern_func_type)))
		return intern_func[iIdx].param_num; 
	else
	    return -1;
}

bool call_internal_func(int index, eval_value *result, char * valFirst, char * valSecond, char * valThird)   
{   
    // int i; 
    if(index >= 0)
	{
		if(intern_func[index].p != NULL)
		{
			(*intern_func[index].p)(result, valFirst, valSecond, valThird);
			return true;
		}
	}
	return false ;
}

bool call_sin  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(sin(val));
    return true ;
}

bool call_cos  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(cos(val));
    return true ;
}

bool call_tan  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(tan(val));
    return true ;
}

bool call_asin (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(asin(val));
    return true ;
}

bool call_acos (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(acos(val));
    return true ;
}

bool call_atan (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(atan(val));
    return true ;
}

bool call_atan2(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	double valTwo = atof(valSecond);
	result->setFloatValue(atan2(val, valTwo));
    return true ;
}

bool call_sinh (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(sinh(val));
    return true ;
}

bool call_cosh (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(cosh(val));
    return true ;
}

bool call_tanh (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(tanh(val));
    return true ;
}

bool call_exp  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(exp(val));
    return true ;
}

bool call_pow  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	int   valTwo = atoi(valSecond);
	result->setFloatValue(pow(val, valTwo));
    return true ;
}

bool call_sqrt (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(sqrt(val));
    return true ;
}

bool call_log  (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(log(val));
    return true ;
}

bool call_log10(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(log10(val));
    return true ;
}

bool call_ceil (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(ceil(val));
    return true ;
}

bool call_floor(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(floor(val));
    return true ;
}

bool call_fabs (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(fabs(val));
    return true ;
}

bool call_ldexp (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	int    valTwo = atoi(valSecond);
	result->setFloatValue(ldexp(val, valTwo));
    return true ;
}

// 返回参数的小数部分, 整数部分不回传。
bool call_modf (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	double valTwo ;
	result->setFloatValue(modf(val, &valTwo));
    return true ;
}

bool call_fmod (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	double valTwo = atof(valSecond);
	result->setFloatValue(fmod(val, valTwo));
    return true ;
}

bool call_hypot(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	double valTwo = atof(valSecond);
	result->setFloatValue(hypot(val, valTwo));
    return true ;
}

int Stein_GCD(int x, int y)
{
    if (x == 0) return y;
    if (y == 0) return x;
    if (x % 2 == 0 && y % 2 == 0)
        return 2 * Stein_GCD(x >> 1, y >> 1);
    else if (x % 2 == 0)
        return Stein_GCD(x >> 1, y);
    else if (y % 2 == 0)
        return Stein_GCD(x, y >> 1);
    else
        return Stein_GCD(min(x, y), fabs(x - y));
}

bool call_gcd(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	double valTwo = atof(valSecond);
	result->setFloatValue(Stein_GCD((int)val, (int)valTwo));
    return true ;
}

bool call_lcm(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	int iLcm = 0 ;
	double val = atof(valFirst);
	double valTwo = atof(valSecond);
	iLcm = (int)val * (int)valTwo /(int)Stein_GCD((int)val, (int)valTwo);
	result->setFloatValue(iLcm);
    return true ;
}

bool call_degrees(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(RAD2DEG(val));
    return true ;
}

bool call_radians(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(DEG2RAD(val));
    return true ;
}

//判断某一年份是否是闰年
int IsLeapYear(int year)
{
	return (((year % 400 == 0) || (year % 4 == 0)) && (year % 100 != 0));
}

bool call_isleapyear(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(IsLeapYear((int)val));
    return true ;
}

//获得某年、某月的最大天数
int GetMaxDay(int year,int month)
{
	switch(month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	case 2:
		return IsLeapYear(year)?29:28;		
	default:return -1;
	}
}

bool call_getmaxday(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	double valTwo = atof(valSecond);
	result->setFloatValue(GetMaxDay((int)val, (int)valTwo));
    return true ;
}

//输入某年某月某日，判断这一天是这一年的第几天？ 
/*  
程序分析：以3月5日为例，应该先把前两个月的加起来，然后再加上5天即本年的第几天，特殊 
情况，闰年且输入月份大于3时需考虑多加一天。
*/
int GetDays(int year,int month,int day)
{
	int sum = 0;
	int i;
	for(i = 1; i < month; i++)      //将前几个月天数相加
		sum += GetMaxDay(year,month);
	sum = sum + day;  //加上本月的天数，就是总天数
	return sum;
}

bool call_getdays(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	double valTwo = atof(valSecond);
	double valThr = atof(valThird);
	result->setFloatValue(GetDays((int)val, (int)valTwo, (int)valThr));
    return true ;
}

bool call_strlen (eval_value *result, const char * valFirst, 
						const char * valSecond, const char * valThird)
{
	result->setFloatValue(strlen(valFirst));
	FST_INFO("result = %f", result->getFloatValue());
    return true ;
}

bool call_findstr (eval_value *result, const char * valFirst, 
						const char * valSecond, const char * valThird)
{
    char * strRet = strstr(valFirst, valSecond);
	if(strRet == NULL)
	{
		result->setFloatValue(-1.0);
		return false ;
	}
	else 
	{
		result->setFloatValue((int)(strRet - valFirst));
		FST_INFO("result = %f", result->getFloatValue());
		return true ;
	}
}

bool call_substr (eval_value *result, const char * valFirst, 
						const char * valSecond, const char * valThird)
{
	string strVal = string(valFirst);
	int    valTwo = (int)atof(valSecond);
	int    valThr = (int)atof(valThird);
    string strRet = strVal.substr(valTwo, valThr);
	if(strRet.length() == 0)
	{
		return false ;
	}
	else 
	{
		result->setStringValue(strRet);
		FST_INFO("result = %s", result->getStringValue().c_str());
		return true ;
	}
}

// replace helper starts
string& replace(string& str, const string& old_value, const string& new_value)   
{    
    string::size_type pos(0);   
    if((pos=str.find(old_value))!=string::npos)   
    {
		str.replace(pos,old_value.length(),new_value);
	}
    return   str;   
}

string& replace_all_no_distinct(string& str, const string& old_value, const string& new_value)   
{   
    while(true){   
        string::size_type pos(0);   
        if((pos=str.find(old_value))!=string::npos)   
            str.replace(pos,old_value.length(),new_value);   
        else   
			break;   
    }   
    return   str;   
}

string& replace_all_distinct(string& str,const string& old_value,const string& new_value)   
{   
    for(string::size_type pos(0); pos!=string::npos; pos+=new_value.length()){   
        if((pos=str.find(old_value,pos))!=string::npos)   
            str.replace(pos,old_value.length(),new_value);   
        else
			break;   
    }   
    return str;   
}  
// replace helper ends

bool call_replace (eval_value *result, const char * valFirst, 
						const char * valSecond, const char * valThird)
{
	string strVal = string(valFirst);
	string strTwo = string(valSecond);  // (int)atof(valSecond);
	string strThr = string(valThird);  // (int)atof(valThird);

    string strRet = replace(strVal, strTwo, strThr);
	if(strRet == strVal)
	{
		result->setStringValue(strRet);
		return false ;
	}
	else 
	{
		result->setStringValue(strRet);
		return true ;
	}
}

bool call_replaceall (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	string strVal = string(valFirst);
	string strTwo = string(valSecond);  // (int)atof(valSecond);
	string strThr = string(valThird);  // (int)atof(valThird);
	
    string strRet = replace_all_distinct(strVal, strTwo, strThr);
	if(strRet == strVal)
	{
		result->setStringValue(strRet);
		return false ;
	}
	else 
	{
		result->setStringValue(strRet);
		return true ;
	}
}

bool call_replaceall_iteration (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	string strVal = string(valFirst);
	string strTwo = string(valSecond);  // (int)atof(valSecond);
	string strThr = string(valThird);  // (int)atof(valThird);
	
    string strRet = replace_all_no_distinct(strVal, strTwo, strThr);
	if(strRet == strVal)
	{
		result->setStringValue(strRet);
		return false ;
	}
	else 
	{
		result->setStringValue(strRet);
		return true ;
	}
}

bool call_lower(eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	string strRet = string(valFirst);
    transform(strRet.begin(), strRet.end(), strRet.begin(), ::tolower);  
	result->setStringValue(strRet);
	return true ;
}

bool call_upper      (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	string strRet = string(valFirst);
    transform(strRet.begin(), strRet.end(), strRet.begin(), ::toupper);  
	result->setStringValue(strRet);
	return true ;
}
bool call_revert     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	string strRet = string(valFirst);
	reverse(strRet.begin(),strRet.end());
	result->setStringValue(strRet);
	return true ;
}

bool call_atoi     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	int val = atoi(valFirst);
	result->setFloatValue((float)val);
    return true ;
}

bool call_atof     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	double val = atof(valFirst);
	result->setFloatValue(val);
    return true ;
}

bool call_itoa     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	string strRet = string(valFirst);
	result->setStringValue(strRet);
	return true ;
}

bool call_ftoa     (eval_value *result, const char * valFirst,const char * valSecond,const char * valThird)
{
	string strRet = string(valFirst);
	result->setStringValue(strRet);
	return true ;
}


