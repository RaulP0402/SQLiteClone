#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "structs.h"

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define TABLE_MAX_PAGES 100

using namespace std;

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

class Table {
public:
    Table() {
        num_rows = 0;
        for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
            pages[i] = nullptr;
        }
    }

    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
};

class InputBuffer {
public:
    // Constructor
    InputBuffer(){
        buffer = nullptr;
        buffer_length = 0;
        input_length = 0;
    }

    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
};


void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(
        &(input_buffer->buffer),
        &(input_buffer->buffer_length),
        stdin
        );
    
    if (bytes_read <= 0) {
        printf("Error reading input.\n");
        exit(EXIT_FAILURE);
    }

    // Ignore trailing newline
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;

}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

void serialize_row(Row* source, void* destination) {
    memcpy(static_cast<char*>(destination) + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(static_cast<char*>(destination) + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(static_cast<char*>(destination) + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination) {
    memcpy(&(destination->id), static_cast<char*>(source) + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), static_cast<char*>(source) + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), static_cast<char*>(source) + EMAIL_OFFSET, EMAIL_SIZE);
}

void* row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = table->pages[page_num];
    if (page == nullptr) {
        // Allocate memory only when we try to access a page
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return static_cast<char*>(page) + byte_offset;
}

PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement) {
    statement->type = STATEMENT_INSERT;

    char* keyword = strtok(input_buffer->buffer, " ");
    char* id_string = strtok(nullptr, " ");
    char* username = strtok(nullptr, " ");
    char* email = strtok(nullptr, " ");

    if (id_string == nullptr || username == nullptr || email == nullptr)
        return PREPARE_SYNTAX_ERROR;
    
    int id = atoi(id_string);
    if (id < 0)
        return PREPARE_NEGATIVE_ID;
    if (strlen(username) > COLUMN_USERNAME_SIZE) 
        return PREPARE_STRING_TOO_LONG;
    if (strlen(email) > COLUMN_EMAIL_SIZE)
        return PREPARE_STRING_TOO_LONG;

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        return prepare_insert(input_buffer, statement);
    }
    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    if (table->num_rows >= TABLE_MAX_ROWS) 
        return EXECUTE_TABLE_FULL;
    
    // cout << table->num_rows << " " << TABLE_MAX_ROWS << "\n";
    Row* row_to_insert = &(statement->row_to_insert);

    serialize_row(row_to_insert, row_slot(table, table->num_rows));
    table->num_rows++;
    
    return EXECUTE_SUCCESS;
}

void print_row(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

ExecuteResult execute_select(Statement* statement, Table* table) {
    Row row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement* statement, Table* table) {
    switch(statement->type) {
        case (STATEMENT_INSERT):
            return execute_insert(statement, table);
        case (STATEMENT_SELECT):
            return execute_select(statement, table);
    }
}

int main() {
    Table* table = new Table();
    InputBuffer* input_buffer = new InputBuffer();

    while (true) {
        printf("db > ");
        read_input(input_buffer);
 
        if (input_buffer->buffer[0] == '.') {
            switch(do_meta_command(input_buffer)) {
                case(META_COMMAND_SUCCESS): 
                    continue;
                case(META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch(prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_NEGATIVE_ID):
                printf("ID must be positive.\n");
                continue;
            case (PREPARE_SYNTAX_ERROR):
                printf("Syntax error. Could not parse statement. \n");
                continue;
            case (PREPARE_STRING_TOO_LONG):
                printf("String is too long. \n");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s' .\n", input_buffer->buffer);
                continue;
        }

        switch(execute_statement(&statement, table)) {
            case (EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("Error: Table full.\n");
                break;
        }
    }

}