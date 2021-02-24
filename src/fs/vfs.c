#include "vfs.h"
// #include "pre_env/initrd.h"

// extern u32int placement_address;


// The root of the file system
vfs_node_t *fs_root = 0;

u32int vfs_read(vfs_node_t *node, u32int offset, u32int size, u8int *buff)
{
	// Does the node have a callback
	if ( node->read != 0 )
	{
		return node->read(node, offset, size, buff);  // call callback
	}
	else
	{
		return 0;  // error
	}
}

u32int vfs_write(vfs_node_t *node, u32int offset, u32int size, u8int *buff)
{
	// Does the node have a callback
	if ( node->write != 0 )
	{
		return node->write(node, offset, size, buff);  // call callback
	}
	else
	{
		return 0;
	}
}

void vfs_open(vfs_node_t *node, u8int read, u8int write)
{
	// Does the node have a callback
	if ( node->open != 0 )
	{
		node->open(node);  // call callback
	}
}

void vfs_close(vfs_node_t *node)
{
	// Does the node have a callback
	if ( node->close != 0 )
	{
		node->close(node);  // call callback
	}
}

struct dirent *vfs_read_dir(vfs_node_t *node, u32int index)
{
	// Is the node a directory and does it have a callback
	if ( ( ( node->flags & 0x7 ) == VFS_DIR ) &&
		 node->read_dir != 0 )
	{
		return node->read_dir(node, index);  // call callback
	}
	else
	{
		return 0;
	}
}

vfs_node_t *vfs_find_dir(vfs_node_t *node, char* name)
{
	// Is the node a directory and does it have a callback
	if ( ( ( node->flags & 0x7 ) == VFS_DIR ) &&
		 node->find_dir != 0 )
	{
		return node->find_dir(node, name);  // call callback
	}
	else
	{
		return 0;
	}
}

//
void init_vfs(struct multiboot *mboot_ptr)
{
	monitor_write("Setting up VFS\n", true, false);

	// Find the location of our initial ramdisk
	u32int result = ASSERT( mboot_ptr->mods_count > 0 );
	if (result == -1) {
		kprintf("Fatal error at ( %s:%d )!\nAssertion failed, initrd.img could not be found!\nContinuing without a VFS as an initrd is not required for booting...\n", __FILE__, __LINE__);
		return; //Otherwise machine will triple fault
	}

	// u32int initrd_location = *( ( u32int * )   mboot_ptr -> mods_addr );
	// u32int initrd_end      = *( ( u32int * ) ( mboot_ptr -> mods_addr + 4 ) );

	// Don't trample our module with placement accesses
	// placement_address = initrd_end;

	// Initialise paging
	// init_paging();

	// monitor_write("Loading initrd into memory\n", true, false);
	// Initialise the initial ramdisk, and set it as the filesystem root
	// fs_root = init_initrd( initrd_location );
	
	// monitor_write("Setup VFS\n", false, true);
	
}

void list_fs() {
	int i = 0;
	struct dirent *node = 0;
	debug("list_fs: ROOT_DIR:\nlist_fs: Flags: %d\nlist_fs: Name: %s\nlist_fs: GID: %d\nlist_fs: UID: %d\n", fs_root->flags, fs_root->name, fs_root->gid, fs_root->uid);
	
	op_task();
	kprintf("Listing %s dir\n", fs_root->name);

	while ( (node = vfs_read_dir(fs_root, i)) != 0 )
	{
		// monitor_write("Discovered:", false, false);
		
		vfs_node_t *fsnode = vfs_find_dir(fs_root, node->filename);

		if ((fsnode->flags & 0x07) == VFS_DIR)
		{
			kprintf("[VFS@%s] %s (directory)", node->filename, node->filename);
		}
		else {
			kprintf("[VFS] %s", node->filename);
		}

		monitor_writeln();
		i++;
	}
}

//Print the contents of a file
void list_file(const char* filename) {
	vfs_node_t *fsnode = vfs_find_dir(fs_root, (char*)filename);
	
	if (filename[0] != '/')
	{
		monitor_write("Invalid file! Must specify the root directory at minimum!\n", false, false);
		return;
	}

	monitor_write("Listing the contents of ", false, false);
	monitor_write((char*)filename, false, false);
	monitor_writeln();
	monitor_write("------------------------\n", false, false);
	char buff[256];
	u32int size = vfs_read(fsnode, 0, 256, (u8int*)buff);
	
	for (int i = 0; i < size; i++)
		monitor_put(buff[i]);
	monitor_writeln();
	monitor_write("------------------------\n", false, false);
}