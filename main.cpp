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

int main() {
    InputBuffer* input_buffer = new InputBuffer();

    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (strcmp(input_buffer->buffer,".exit") == 0) {
            delete input_buffer;
            exit(EXIT_SUCCESS);
        } else {
            printf("Unrecognized Command '%s' .\n",input_buffer->buffer);
        }
    }

}