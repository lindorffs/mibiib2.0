#include <lynx/kernel.h>
#include <mibiib/fs.h>

#define EDITOR_BUFFER_SIZE 8192

void editor_run(fs_node_t* file) {
    if (!file || file->type != FS_FILE) return;

    char* buffer = kmalloc(EDITOR_BUFFER_SIZE);

    int index = 0;

    term_print("---- EDIT MODE (ESC to save & exit) ----\n");

    // Preload existing content
    if (file->data) {
        int len = strlen(file->data);

        if (len >= EDITOR_BUFFER_SIZE)
            len = EDITOR_BUFFER_SIZE - 1;

        strcpy(buffer, file->data);
        index = len;
        term_print(file->data);
    }

    while (1) {
        char c = input_get_char();

        if (c == 27) { // ESC
            buffer[index] = 0;

            fs_write(file, buffer);
            term_print("\n[Saved]\n");

            break;
        }
        else if (c == '\n') {
            if (index < EDITOR_BUFFER_SIZE - 1) {
                buffer[index++] = '\n';
                term_putc('\n');
            }
        }
        else if (c == '\b') {
            if (index > 0) {
                index--;
                term_putc('\b');
            }
        }
        else if (c) {
            if (index < EDITOR_BUFFER_SIZE - 1) {
                buffer[index++] = c;
                term_putc(c);
            }
        }
    }
}

