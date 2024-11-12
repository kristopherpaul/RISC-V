#ifndef UART_H
#define UART_H

#include "utils.h"

typedef struct UART{
    u8 buffer[UART_SIZE];
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool interrupting;
} UART;

extern UART uart;

void initUART();

Result load_uart(u64 addr, u64 size);

Result store_uart(u64 addr, u64 size, u64 val);

#endif