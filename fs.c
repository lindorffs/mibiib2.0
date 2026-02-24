/* fs.c */
#include "fs.h"
#include "mem.h"

fs_node_t* fs_root = NULL;
fs_node_t* current_dir = NULL;

void fs_init() {
    // Create Root Directory "/"
    fs_root = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    strcpy(fs_root->name, "/");
    fs_root->type = FS_DIR;
    fs_root->parent = fs_root; // Parent of root is root
    fs_root->sibling = NULL;
    fs_root->child = NULL;
    
    current_dir = fs_root;
}

// Helper to add a node to a directory
void _fs_insert(fs_node_t* parent, fs_node_t* child) {
    if (!parent->child) {
        parent->child = child;
    } else {
        // Find end of linked list
        fs_node_t* temp = parent->child;
        while (temp->sibling) {
            temp = temp->sibling;
        }
        temp->sibling = child;
    }
}

fs_node_t* fs_mkdir(fs_node_t* parent, const char* name) {
    fs_node_t* node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    strcpy(node->name, name);
    node->type = FS_DIR;
    node->parent = parent;
    node->sibling = NULL;
    node->child = NULL;
    
    _fs_insert(parent, node);
    return node;
}

fs_node_t* fs_mkfile(fs_node_t* parent, const char* name, const char* data) {
    fs_node_t* node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    strcpy(node->name, name);
    node->type = FS_FILE;
    node->parent = parent;
    node->sibling = NULL;
    node->child = NULL;
    
    // Copy data
    size_t len = strlen(data);
    node->data = (char*)kmalloc(len + 1);
    strcpy(node->data, data);
    node->size = len;

    _fs_insert(parent, node);
    return node;
}

fs_node_t* fs_find(fs_node_t* parent, const char* name) {
    fs_node_t* iterator = parent->child;
    while (iterator) {
        if (strcmp(iterator->name, name) == 0) {
            return iterator;
        }
        iterator = iterator->sibling;
    }
    return NULL;
}

void fs_list(fs_node_t* dir, void (*printer)(const char*)) {
    if (!dir || dir->type != FS_DIR) return;
    
    fs_node_t* iterator = dir->child;
    while (iterator) {
        printer(iterator->name);
        if (iterator->type == FS_DIR) printer("/");
        printer("\n");
        iterator = iterator->sibling;
    }
}