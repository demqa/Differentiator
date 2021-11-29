#ifndef DEBUG_LIB_H

#define DEBUG_LIB_H

#include <stdio.h>

#define OUT stderr

#define PRINT_LINE     do{ fprintf(OUT, "I'm at %s at line %d in %s\n", __PRETTY_FUNCTION__, __LINE__, __FILE__); }while(0);

#define PRINT_PTR(ptr) do{ fprintf(OUT, "pointer %s at %p at line %d\n", #ptr, ptr, __LINE__);                    }while(0);

#define PRINT_C(char)  do{ fprintf(OUT, "[%s:%lu] %s = %c\n", __PRETTY_FUNCTION__, __LINE__, #char, char);        }while(0);

#define PRINT_S(str)   do{ fprintf(OUT, "[%s:%lu] %s = %s\n", __PRETTY_FUNCTION__, __LINE__, #str, str);          }while(0);

#define PRINT(str)     do{ fprintf(OUT, "%s\n", #str); }while(0);

#define PRINT_UL(num)  do{ fprintf(OUT, "[%s:%lu] %s = %lu\n", __PRETTY_FUNCTION__, __LINE__, #num, num);         }while(0);
       
#define PRINT_D(num)   do{ fprintf(OUT, "[%s:%lu] %s = %d\n", __PRETTY_FUNCTION__, __LINE__, #num, num);          }while(0);
       
#define PRINT_X(num)   do{ fprintf(OUT, "[%s:%lu] %s = %x\n", __PRETTY_FUNCTION__, __LINE__, #num, num);          }while(0);

#define PRINT_SM(s, n) do{ fprintf(OUT, "[%d] = %.*s\n", __LINE__, n s);                                          }while(0);

#undef  OUT

#endif
