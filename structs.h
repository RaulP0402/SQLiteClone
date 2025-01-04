#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#include "enums.h"

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define TABLE_MAX_PAGES 100

struct Row {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
};

struct Statement {
    StatementType type;
    Row row_to_insert;
};

struct Pager {
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
};

struct Table {
    uint32_t num_rows;
    Pager* pager;
};

struct InputBuffer {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
};

struct Cursor {
    Table* table;
    uint32_t row_num;
    bool end_of_table;
};

#endif