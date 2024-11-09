#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

// UART register offsets.
#define UART_RHR        (UART_BASE + 0)  // Receive holding register
#define UART_THR        (UART_BASE + 0)  // Transmit holding register
#define UART_LCR        (UART_BASE + 3)  // Line control register
#define UART_LSR        (UART_BASE + 5)  // Line status register

// UART line status bits
#define UART_LSR_RX     (1 << 0)         // Receiver ready
#define UART_LSR_TX     (1 << 5)         // Transmitter empty

typedef struct {
    uint8_t buffer[UART_SIZE];
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool interrupting;
} uart_t;

// Initialize a new Uart object.
uart_t* uart_new() {
    uart_t* uart = (uart_t*) malloc(sizeof(uart_t));
    if (!uart) {
        perror("Failed to allocate UART");
        return NULL;
    }
    // Initialize the UART buffer, mutex, condition variable, and interrupt flag
    memset(uart->buffer, 0, UART_SIZE);
    pthread_mutex_init(&uart->mutex, NULL);
    pthread_cond_init(&uart->cond, NULL);
    uart->interrupting = false;

    // Set transmitter holding register as empty
    uart->buffer[UART_LSR - UART_BASE] |= UART_LSR_TX;

    return uart;
}

// UART input thread for reading from stdin.
void* uart_input_thread(void* arg) {
    uart_t* uart = (uart_t*)arg;
    uint8_t byte;
    while (1) {
        if (read(STDIN_FILENO, &byte, 1) > 0) {
            pthread_mutex_lock(&uart->mutex);

            // Wait until UART receiver is ready
            while (uart->buffer[UART_LSR - UART_BASE] & UART_LSR_RX) {
                pthread_cond_wait(&uart->cond, &uart->mutex);
            }
            uart->buffer[UART_RHR - UART_BASE] = byte;
            uart->interrupting = true;
            uart->buffer[UART_LSR - UART_BASE] |= UART_LSR_RX; // Set RX bit

            pthread_mutex_unlock(&uart->mutex);
        }
    }
}

// UART initialization with input thread.
uart_t* uart_init() {
    uart_t* uart = uart_new();
    if (!uart) return NULL;

    // Create a thread for handling UART input
    pthread_t thread;
    if (pthread_create(&thread, NULL, uart_input_thread, (void*)uart) != 0) {
        perror("Failed to create UART input thread");
        free(uart);
        return NULL;
    }

    return uart;
}

// Load a byte from the UART register.
uint64_t uart_load8(uart_t* uart, uint64_t addr) {
    pthread_mutex_lock(&uart->mutex);
    uint8_t value;

    if (addr == UART_RHR) {
        pthread_cond_signal(&uart->cond); // Notify waiting threads
        uart->buffer[UART_LSR - UART_BASE] &= ~UART_LSR_RX; // Clear RX bit
        value = uart->buffer[UART_RHR - UART_BASE];
    } else {
        value = uart->buffer[addr - UART_BASE];
    }

    pthread_mutex_unlock(&uart->mutex);
    return value;
}

// Store a byte to the UART register.
void uart_store8(uart_t* uart, uint64_t addr, uint64_t value) {
    pthread_mutex_lock(&uart->mutex);

    if (addr == UART_THR) {
        putchar((char)value);
        fflush(stdout);
    } else {
        uart->buffer[addr - UART_BASE] = (uint8_t)value;
    }

    pthread_mutex_unlock(&uart->mutex);
}