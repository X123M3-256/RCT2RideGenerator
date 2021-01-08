#include<stdlib.h>
#include<string.h>
#include<locale.h>
#include "floatparse.h"


double hack_strtod(const char* str, char** endptr)
{
struct lconv* loc=localeconv();

char temp[256];
strncpy(temp,str,255);
temp[255]=0;

char* point=strchr(temp,'.');
	if(point!=NULL)*point=loc->decimal_point[0];
char* comma=strchr(temp,',');
	if(comma!=NULL)*comma=loc->decimal_point[0];

double value=strtod(temp,endptr);

	if(endptr!=NULL)
	{
	char* end=*endptr;
	*endptr=(char*)(str+(end-temp));
	}
return value;
}

float hack_atof(const char* str)
{
return hack_strtod(str,NULL);
}
