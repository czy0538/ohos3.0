/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright © 2001-2007 Red Hat, Inc.
 * Copyright © 2004-2010 David Woodhouse <dwmw2@infradead.org>
 *
 * Created by David Woodhouse <dwmw2@infradead.org>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 */

#ifndef _JFFS2_FS_I
#define _JFFS2_FS_I

#include <linux/rbtree.h>
#include <linux/kernel.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

struct jffs2_inode_info {
	/* We need an internal mutex similar to inode->i_mutex.
	   Unfortunately, we can't used the existing one, because
	   either the GC would deadlock, or we'd have to release it
	   before letting GC proceed. Or we'd have to put ugliness
	   into the GC code so it didn't attempt to obtain the i_mutex
	   for the inode(s) which are already locked */
	struct pthread_mutex sem;

	/* The highest (datanode) version number used for this ino */
	uint32_t highest_version;

	/* List of data fragments which make up the file */
	struct rb_root fragtree;

	/* There may be one datanode which isn't referenced by any of the
	   above fragments, if it contains a metadata update but no actual
	   data - or if this is a directory inode */
	/* This also holds the _only_ dnode for symlinks/device nodes,
	   etc. */
	struct jffs2_full_dnode *metadata;

	/* Directory entries */
	struct jffs2_full_dirent *dents;

	/* The target path if this is the inode of a symlink */
	unsigned char *target;

	/* Some stuff we just have to keep in-core at all times, for each inode. */
	struct jffs2_inode_cache *inocache;

	uint16_t flags;
	uint8_t usercompr;
};

struct super_block;

struct jffs2_inode {
	uint32_t i_ino;
	mode_t i_mode;
	nlink_t i_nlink;
	uid_t i_uid;
	gid_t i_gid;
	time_t i_atime;
	time_t i_mtime;
	time_t i_ctime;
	off_t i_size;
	struct super_block *i_sb;
	LOS_DL_LIST i_hashlist;
	struct jffs2_inode_info jffs2_i;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _JFFS2_FS_I */
