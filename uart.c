#include "uart.h"

UART uart;

// UART input thread for reading from stdin.
void* uart_input_thread(void* arg){
    UART* uart_obj = (UART*)arg;
    u8 byte;
    while(1){
        if(read(STDIN_FILENO, &byte, 1) > 0){
            pthread_mutex_lock(&uart_obj->mutex);
            // Wait until UART receiver is ready
            while(uart_obj->buffer[UART_LSR-UART_BASE] & UART_LSR_RX){
                pthread_cond_wait(&uart_obj->cond, &uart_obj->mutex);
            }
            uart_obj->buffer[UART_RHR - UART_BASE] = byte;
            uart_obj->interrupting = true;
            uart_obj->buffer[UART_LSR - UART_BASE] |= UART_LSR_RX; // Set RX bit
            pthread_mutex_unlock(&uart_obj->mutex);
        }
    }
}

void initUART(){
    memset(uart.buffer, 0, UART_SIZE);
    pthread_mutex_init(&uart.mutex, NULL);
    pthread_cond_init(&uart.cond, NULL);
    uart.interrupting = false;
    // Set transmitter holding register as empty
    uart.buffer[UART_LSR-UART_BASE] |= UART_LSR_TX;
    // Create a thread for handling UART input
    pthread_t thread;
    if(pthread_create(&thread, NULL, uart_input_thread, (void*)&uart) != 0){
        fprintf(stderr, "Failed to create UART input thread");
    }
}

Result load_uart(u64 addr, u64 size){
    Result ret;
    ret.exception = NullException;
    if(size != 8){
        ret.exception = LoadAccessFault;
        return ret;
    }
    pthread_mutex_lock(&uart.mutex);
    u8 val;
    if(addr == UART_RHR){
        pthread_cond_signal(&uart.cond); // Notify waiting threads
        uart.buffer[UART_LSR-UART_BASE] &= ~UART_LSR_RX; // Clear RX bit
        val = uart.buffer[UART_RHR-UART_BASE];
    }else{
        val = uart.buffer[addr-UART_BASE];
    }
    pthread_mutex_unlock(&uart.mutex);
    ret.value = val;
    return ret;
}

Result store_uart(u64 addr, u64 size, u64 val){
    Result ret;
    ret.exception = NullException;
    if(size != 8){
        ret.exception = StoreAMOAccessFault;
        return ret;
    }
    pthread_mutex_lock(&uart.mutex);
    if(addr == UART_THR){
        putchar((char)(u8)val);
        fflush(stdout);
    }else{
        uart.buffer[addr-UART_BASE] = (u8)val;
    }
    pthread_mutex_unlock(&uart.mutex);
    return ret;
}