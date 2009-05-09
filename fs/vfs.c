#include "gos.h"
#include "vfs.h"
#include "devfs.h"
#include "list.h"
#include "filesystem.h"

struct _vfd {
  struct _fs_node *fsnod;
  void *fs_data;
};

struct _fs_node {
  filesystem *fs;
  const char *mount_pt;
  list_node l_node;
};

struct _registered_fs_node {
  filesystem *fs;
  const char *type;
  list_node l_node;
};

/* we'd like to only have one root fs here, but special file 
   systems like devfs need to be able to be mounted
   before others, so we'll handle them in a special way in vfs, hence
   the need for a list of file systems and mount points - 
   this is basically a list of vfs mounted file systems.*/
PRIVATE list filesystems;

/* registered filesystems are those that vfs knows about and
   is able to mount. */
PRIVATE list registered_filesystems;

PRIVATE struct _fs_node * get_fs_node_for_path(const char *path);
PRIVATE filesystem * find_fs_for_type(const char*);

int vfs_init(){
  filesystems = list_init(struct _fs_node, l_node);
  registered_filesystems = list_init(struct _registered_fs_node, l_node);
}

vfd vfs_open(const char *path){
  struct _fs_node *fsnod = get_fs_node_for_path(path);
  vfd vfd_return = (vfd)kmalloc(sizeof(struct _vfd));
  vfd_return->fs_data = fsnod->fs->open(path + strlen(fsnod->mount_pt) + 1);
  vfd_return->fsnod = fsnod;
  return vfd_return;
}

int vfs_read(vfd fd, void *buf, uint32 num_bytes) {
  return fd->fsnod->fs->read(fd->fs_data, buf, num_bytes);
}

int vfs_write(vfd fd, void *buf, uint32 num_bytes){
  return fd->fsnod->fs->write(fd->fs_data, buf, num_bytes);
}

int vfs_close(vfd fd){
  return fd->fsnod->fs->close(fd->fs_data);
}

int vfs_mount(const char *type, const char *dir){
  filesystem *fs = find_fs_for_type(type);
  struct _fs_node *mountfs = (struct _fs_node*)kmalloc(sizeof(struct _fs_node));
  mountfs->fs = fs;
  mountfs->mount_pt = dir;
  list_add(filesystems, &(mountfs->l_node));
  return 0;
}

void register_fs(const char *type, filesystem *fs){
  struct _registered_fs_node *regFs = (struct _registered_fs_node*)kmalloc(sizeof(struct _registered_fs_node));
  regFs->fs = fs;
  regFs->type = type;
  list_add(registered_filesystems, &(regFs->l_node));
}

PRIVATE filesystem * find_fs_for_type(const char *type){
  list_node *curNode;
  struct _registered_fs_node *curStructNode;
  for (curNode = list_head(registered_filesystems); curNode != NULL; 
       curNode = list_next(registered_filesystems, curNode)){
    if (strcmp(type, (curStructNode = 
		      cur_obj(registered_filesystems, 
			      curNode, 
			      struct _registered_fs_node))->type) == 0)
      return curStructNode->fs;
  }
  return NULL;
}

PRIVATE struct _fs_node * get_fs_node_for_path(const char *path){
  list_node *curNode;
  struct _fs_node *curFsNode;
  for (curNode = list_head(filesystems); curNode != NULL; 
       curNode = list_next(filesystems, curNode)){
    curFsNode = cur_obj(filesystems, curNode, struct _fs_node);
    if (!strncmp(curFsNode->mount_pt, path, strlen(curFsNode->mount_pt))){
      return curFsNode;
    }
  }
  return NULL;
}
