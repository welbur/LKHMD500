/*
 * PowerBoard
 * 
 */

#ifndef _LOG_H_
#define _LOH_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "BoardConfig.h"
#include "stdio.h"



#if defined(UartPrintf)
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define LOG                    printf
#define LOG_info(fmt, ...)     printf("INFO: " fmt, ##__VA_ARGS__)
#define LOG_warn(fmt, ...)     printf("WARN: " fmt, ##__VA_ARGS__)
#define LOG_error(fmt, ...)    printf("ERROR: " fmt, ##__VA_ARGS__)
#define LOG_trace(fmt, ...)    printf("TRACE: " fmt, ##__VA_ARGS__) 
//#define LOG   LOG_info

#endif


#ifdef __cplusplus
}
#endif
#endif // !_LOG_H_