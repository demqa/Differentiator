#ifndef DEBUG_LIB_H

#define DEBUG_LIB_H

#include <stdio.h>

#define __OUT stderr

#define PRINT_LINE     do{ fprintf(__OUT, "I'm at %s at line %d in %s\n", __PRETTY_FUNCTION__, __LINE__, __FILE__); }while(0);

#define PRINT_PTR(ptr) do{ fprintf(__OUT, "pointer %s at %p at line %d\n", #ptr, ptr, __LINE__);                    }while(0);

#define PRINT_C(char)  do{ fprintf(__OUT, "[%s:%d] %s = %c\n", __PRETTY_FUNCTION__, __LINE__, #char, char);        }while(0);

#define PRINT_S(str)   do{ fprintf(__OUT, "[%s:%d] %s = %s\n", __PRETTY_FUNCTION__, __LINE__, #str, str);          }while(0);

#define PRINT(str)     do{ fprintf(__OUT, "%s\n", #str); }while(0);

#define PRINT_UL(num)  do{ fprintf(__OUT, "[%s:%d] %s = %lu\n", __PRETTY_FUNCTION__, __LINE__, #num, num);         }while(0);
       
#define PRINT_D(num)   do{ fprintf(__OUT, "[%s:%d] %s = %d\n", __PRETTY_FUNCTION__, __LINE__, #num, num);          }while(0);
       
#define PRINT_X(num)   do{ fprintf(__OUT, "[%s:%d] %s = %x\n", __PRETTY_FUNCTION__, __LINE__, #num, num);          }while(0);

#define PRINT_SM(s, n) do{ fprintf(__OUT, "[%d] = %.*s\n", __LINE__, n s);                                          }while(0);

#endif
