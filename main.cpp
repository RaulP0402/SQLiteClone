#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

class InputBuffer {
public:
    // Constructor
    InputBuffer(){
        buffer = nullptr;
        buffer_length = 0;
        input_length = 0;
    }

    // Destructor
    ~InputBuffer() {
        if (buffer != nullptr) {
            delete[] buffer;
            buffer = nullptr;
        }
    }

    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
};

void print_prompt() {
    printf("db > ");
}

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

enum MetaCommandResult {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
};

enum PrepareResult {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
};

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        delete input_buffer;
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

enum StatementType {
    STATEMENT_INSERT,
    STATEMENT_SELECT
};

struct Statement {
    StatementType type;
};

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
    switch(statement->type) {
        case (STATEMENT_INSERT):
            printf("This is where we insert.\n");
            break;
        case (STATEMENT_SELECT):
            printf("This is where we select.\n");
            break;
    }
}

int main() {
    InputBuffer* input_buffer = new InputBuffer();

    while (true) {
        print_prompt();
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
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s' .\n", input_buffer->buffer);
                continue;
        }

        execute_statement(&statement);
        printf("Executed.\n");
    }

}