#ifndef FLOATPARSE_H_INCLUDED
#define FLOATPARSE_H_INCLUDED


//Apparently, strtod and atof behave differently depending on the current locale, so these functions implement a hacky workaround

double hack_strtod(const char* str, char** endptr);
float hack_atof(const char* str);

#endif // FLOATPARSE_H_INCLUDED
