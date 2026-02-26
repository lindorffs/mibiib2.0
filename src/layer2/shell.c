#include <lynx/kernel.h>
#include <mibiib/shell.h>
#include <mibiib/fs.h>

void shell_prompt() {
    term_print("user:/");
    if (current_dir != fs_root) {
        term_print(current_dir->name);
    }
    term_print("> ");
}

void execute_command(char* cmd) {
    term_print("\n");
    if (strcmp(cmd, "help") == 0) {
        term_print("Available commands:\n");
        term_print("  help           - Show this message\n");
        term_print("  clear          - Clear the screen\n");
        term_print("  ls             - List all files\n");
        term_print("  cat [file]     - Read a file\n");
        term_print("  touch [file]   - Create a file\n");
        term_print("  edit [file]   - Edit a file\n");
        term_print("  reboot         - Restart System\n");
    }
    else if (strcmp(cmd, "clear") == 0) {
        term_clear();
    }
    else if (strcmp(cmd, "ls") == 0) {
        fs_list(current_dir, term_print);
    }
    // mkdir
    else if (cmd[0]=='m' && cmd[1]=='k' && cmd[2]=='d' && cmd[3]=='i' && cmd[4]=='r' && cmd[5]==' ') {
        char* name = cmd + 6;
        fs_mkdir(current_dir, name);
        term_print("Directory created.\n");
    }
    // touch
    else if (cmd[0]=='t' && cmd[1]=='o' && cmd[2]=='u' && cmd[3]=='c' && cmd[4]=='h' && cmd[5]==' ') {
        char* name = cmd + 6;
        fs_mkfile(current_dir, name, "");
        term_print("File created.\n");
    }
    // cat
    else if (cmd[0]=='c' && cmd[1]=='a' && cmd[2]=='t' && cmd[3]==' ') {
        fs_node_t* f = fs_find(current_dir, cmd + 4);
        if (f && f->type == FS_FILE) {
            term_print(f->data); term_print("\n");
        } else {
            term_print("File not found or is directory.\n");
        }
    }
    // cd
    else if (cmd[0]=='c' && cmd[1]=='d' && cmd[2]==' ') {
        char* name = cmd + 3;
        if (strcmp(name, "..") == 0) {
            if (current_dir->parent) current_dir = current_dir->parent;
        } 
        else if (strcmp(name, "/") == 0) {
            current_dir = fs_root;
        }
        else {
            fs_node_t* next = fs_find(current_dir, name);
            if (next && next->type == FS_DIR) {
                current_dir = next;
            } else {
                term_print("Directory not found.\n");
            }
        }
    }
	// write filename content
	else if (cmd[0]=='w' && cmd[1]=='r' && cmd[2]=='i' && cmd[3]=='t' && cmd[4]=='e' && cmd[5]==' ') {
		char* args = cmd + 6;

		// find first space after filename
		char* space = NULL;
		for (int i = 0; args[i]; i++) {
			if (args[i] == ' ') {
				space = args + i;
				break;
			}
		}

		if (!space) {
			term_print("Usage: write filename content\n");
			return;
		}

		*space = 0; // split string
		char* filename = args;
		char* content = space + 1;

		fs_node_t* f = fs_find(current_dir, filename);

		if (!f) {
			term_print("File not found.\n");
		}
		else if (f->type != FS_FILE) {
			term_print("Not a file.\n");
		}
		else {
			fs_write(f, content);
			term_print("File updated.\n");
		}
	}
	else if (cmd[0]=='e' && cmd[1]=='d' && cmd[2]=='i' && cmd[3]=='t' && cmd[4]==' ') {
		char* name = cmd + 5;
		fs_node_t* f = fs_find(current_dir, name);

		if (!f) {
			term_print("File not found.\n");
		}
		else if (f->type != FS_FILE) {
			term_print("Not a file.\n");
		}
		else {
		   editor_run(f);
		}
	}
    else if (strcmp(cmd, "reboot") == 0) {
        term_print("Rebooting...\n");
        // Pulse the keyboard controller reset pin
        uint8_t good = 0x02;
        while (good & 0x02) good = inb(0x64);
        outb(0x64, 0xFE);
        asm volatile("hlt");
    }
    else if (cmd[0] != 0) {
        term_print("Unknown command.\n");
    }
}

void kentry(void) {
	fs_init();
    char buffer[128];
    int idx = 0;
	term_clear();
    shell_prompt();

    while (1) {
        char c = input_get_char();

        if (c == '\n') {
            buffer[idx] = 0;
            execute_command(buffer);
            idx = 0;
            shell_prompt();
        }
        else if (c == '\b') {
            if (idx > 0) {
                idx--;
                term_putc('\b');
            }
        }
        else if (c) {
            if (idx < sizeof(buffer)-1) {
                buffer[idx++] = c;
                term_putc(c);
            }
        }
    }
}
