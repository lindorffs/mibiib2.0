/* fs.h */
#ifndef FS_H
#define FS_H

#include <stdint.h>

#define FS_FILE 0
#define FS_DIR  1

typedef struct fs_node {
    char name[32];
    uint8_t type;         // FS_FILE or FS_DIR
    struct fs_node* parent; // Pointer to folder containing this
    struct fs_node* sibling; // Next file in the same folder
    struct fs_node* child;   // First file inside (if this is a folder)
    
    char* data;           // File content
    uint32_t size;
} fs_node_t;

// Globals
extern fs_node_t* fs_root;
extern fs_node_t* current_dir; // For 'cd' command

// Functions
void fs_init();
fs_node_t* fs_mkdir(fs_node_t* parent, const char* name);
fs_node_t* fs_mkfile(fs_node_t* parent, const char* name, const char* data);
void fs_write(fs_node_t* file, const char* data);
fs_node_t* fs_find(fs_node_t* parent, const char* name);
void fs_list(fs_node_t* dir, void (*printer)(const char*));

#endif