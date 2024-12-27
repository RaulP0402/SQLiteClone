// Structs.h

#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 32

struct Row {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
};

enum MetaCommandResult {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
};

enum PrepareResult {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR
};

enum StatementType {
    STATEMENT_INSERT,
    STATEMENT_SELECT
};

struct Statement {
    StatementType type;
    Row row_to_insert;
};

enum ExecuteResult {
    EXECUTE_TABLE_FULL,
    EXECUTE_SUCCESS
};

#endif