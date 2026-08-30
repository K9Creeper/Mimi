#ifndef MIMI_ERROR_H
#define MIMI_ERROR_H

typedef enum mimi_err_e {
    MIMI_OK = 0,

    MIMI_ERR_GENERIC = -1,
    MIMI_ERR_UNK = -2,
    MIMI_ERR_BAD_ARG = -3,
    MIMI_ERR_NO_MEMORY = -4,
    MIMI_ERR_NOT_FOUND = -5,
    MIMI_ERR_UNSUPPORTED = -6,
    
    MIMI_6502_ERR_UNACC_DATA = -100,
    MIMI_ERR_BUS_DEV = -101,
} mimi_err_t;

#endif