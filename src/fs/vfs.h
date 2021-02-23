#ifndef VALIDITY_VFS_H
#define VALIDITY_VFS_H

#include "../common.h"
#include "../multiboot.h"
#include "../panic.h"

#define VFS_FILE        0x01
#define VFS_DIR         0x02
#define VFS_CHARDEVICE  0x03
#define VFS_BLOCKDEVICE 0x04
#define VFS_PIPE        0x05
#define VFS_SYMLINK     0x06
#define VFS_MOUNTPOINT  0x08

typedef u32int (*read_type_t)(struct vfs_node*,u32int,u32int,u8int*);
typedef u32int (*write_type_t)(struct vfs_node*,u32int,u32int,u8int*);
typedef void (*open_type_t)(struct vfs_node*);
typedef void (*close_type_t)(struct vfs_node*);
typedef struct dirent * (*read_dir_type_t)(struct vfs_node*,u32int);
typedef struct vfs_node * (*find_dir_type_t)(struct vfs_node*,char *name); 

typedef struct vfs_node {
    char            name[128];     // File or directory name
    u32int          mask;          // Permissions mask
    u32int          uid;           // User who owns the file
    u32int          gid;           // Group which owns the file
    u32int          flags;         // flags (directory, file, etc)
    u32int          inode;         // 
    u32int          length;        // Filesize in bytes
    u32int          impl;          // 
    read_type_t     read;          // These are just function pointers
    write_type_t    write;
    open_type_t     open;
    close_type_t    close;
    read_dir_type_t read_dir;
    find_dir_type_t find_dir;
    struct vfs_node *symlink_ptr;  // Used by symlinks and mountpoints
} vfs_node_t;

struct dirent {
    char filename[128];     // Filename
    u32int inode_num;       // Inode number
};


//Specifiy the root node of Validity's VFS
extern vfs_node_t *vfs_root;

// These deal with the file nodes, NOT the files. Hence it is prefixed with 'vfs_'
u32int vfs_read(vfs_node_t *node, u32int offset, u32int size, u8int *buff);
u32int vfs_write(vfs_node_t *node, u32int offset, u32int size, u8int *buff);
void vfs_open(vfs_node_t *node, u8int read, u8int write);
void vfs_close(vfs_node_t *node);

struct dirent *vfs_read_dir(vfs_node_t *node, u32int index);
vfs_node_t *vfs_find_dir(vfs_node_t *node, char* name);

void init_vfs(struct multiboot *mboot_ptr);
void list_fs();
void list_file(const char* filename);

#endif // VALIDITY_VFS_H