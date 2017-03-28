
typedef struct ext2_super_block {
  u32	s_inodes_count;		/* Inodes count */
  u32	s_blocks_count;		/* Blocks count */
  u32	s_r_blocks_count;	/* Reserved blocks count */
  u32	s_free_blocks_count;	/* Free blocks count */
  u32	s_free_inodes_count;	/* Free inodes count */
  u32	s_first_data_block;	/* First Data Block */
  u32	s_log_block_size;	/* Block size */
  u32	s_log_frag_size;	/* Fragment size */
  u32	s_blocks_per_group;	/* # Blocks per group */
  u32	s_frags_per_group;	/* # Fragments per group */
  u32	s_inodes_per_group;	/* # Inodes per group */
  u32	s_mtime;		/* Mount time */
  u32	s_wtime;		/* Write time */
  u16	s_mnt_count;		/* Mount count */
  u16	s_max_mnt_count;	/* Maximal mount count */
  u16	s_magic;		/* Magic signature */
  u16	s_state;		/* File system state */
  u16	s_errors;		/* Behaviour when detecting errors */
  u16	s_minor_rev_level; 	/* minor revision level */
  u32	s_lastcheck;		/* time of last check */
  u32	s_checkinterval;	/* max. time between checks */
  u32	s_creator_os;		/* OS */
  u32	s_rev_level;		/* Revision level */
  u16	s_def_resuid;		/* Default uid for reserved blocks */
  u16	s_def_resgid;		/* Default gid for reserved blocks */

  // these are for DYNAMIC ext2_fs used in 2.6 kernel
  u32   s_first_ino;
  u16   s_inode_size;           /* real inode size : OLD=128, NEW=256*/
  u16	s_block_group_nr;	/* block group # of this superblock */
  u32	s_feature_compat;	/* compatible feature set */
  u32	s_feature_incompat;	/* incompatible feature set */
  u32	s_feature_ro_compat;	/* readonly-compatible feature set */
  u8	s_uuid[16];		/* 128-bit uuid for volume */
  char	s_volume_name[16];	/* volume name */
  char	s_last_mounted[64];	/* directory where last mounted */
  u32	s_algorithm_usage_bitmap; /* For compression */

  /**** ignore the rest; not needed for booting **********/
} SUPER;

typedef struct ext2_group_desc
{
  u32  bg_block_bitmap;
  u32  bg_inode_bitmap;
  u32  bg_inode_table;
  u16  bg_free_blocks_count;
  u16  bg_free_inodes_count;
  u16  bg_used_dirs_count;
  u16  bg_pad;
  u32  bg_reserved[3];
} GD;

typedef struct ext2_inode {
  u16	i_mode;		/* File mode */
  u16	i_uid;		/* Owner Uid */
  u32	i_size;		/* Size in bytes */
  u32	i_atime;	/* Access time */
  u32	i_ctime;	/* Creation time */
  u32	i_mtime;	/* Modification time */
  u32	i_dtime;	/* Deletion Time */
  u16	i_gid;		/* Group Id */
  u16	i_links_count;	/* Links count */
  u32	i_blocks;	/* Blocks count */
  u32	i_flags;	/* File flags */
  u32   dummy;
  u32	i_block[15];    /* Pointers to blocks */
  u32   pad[7];         /* inode size MUST be 128 bytes */
} INODE;

typedef struct ext2_dir_entry_2 {
  u32	inode;			/* Inode number */
  u16	rec_len;		/* Directory entry length */
  u8	name_len;		/* Name length */
  u8	file_type;
  char	name[255];      	/* File name */
} DIR;

