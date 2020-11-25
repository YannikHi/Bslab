//
//  myfs-structs.h
//  myfs
//
//  Created by Oliver Waldhorst on 07.09.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#ifndef myfs_structs_h
#define myfs_structs_h

#define NAME_LENGTH 255
#define BLOCK_SIZE 512
#define NUM_DIR_ENTRIES 64
#define NUM_OPEN_FILES 64

#define SB_START 0
#define SB_SIZE 1
#define DMAP_START 1
#define DMAP_SIZE 16
#define DMAP_END 16
#define FAT_START 17
#define FAT_SIZE 256
#define FAT_END 272
#define ROOT_START 273
#define ROOT_SIZE 64
#define ROOT_END 336
#define DATA_START 337
#define DATA_SIZE 60000

// TODO: Add structures of your file system here

struct SUPERBLOCK{
    char* superblock[BLOCK_SIZE];
};

struct DMAP{
    char* index[BLOCK_SIZE*DMAP_SIZE];
};

struct FAT{
    int index[BLOCK_SIZE*FAT_SIZE];
};

struct roodi{
    char name[NAME_LENGTH +1]{};
    unsigned int st_size;
    char* data;
    int firstBlockIndex;
    uid_t st_uid;
    gid_t st_gid;
    mode_t st_mode;
    time_t t_atime, t_ctime, t_mtime;
};

struct root{
    char name[NAME_LENGTH +1]{};
    size_t st_size;
    char* data;
    uid_t st_uid;
    gid_t st_gid;
    mode_t st_mode;
    time_t t_atime, t_ctime, t_mtime;

    root() {
        name[0] = '\0';
        st_size = 0;
        data = nullptr;
        st_uid = 0;
        st_gid = 0;
        st_mode = 0;
        t_atime = 0;
        t_ctime = 0;
        t_mtime = 0;
    }
};

struct DATA{
    char* data[BLOCK_SIZE];
};




// NUR NOCH DRIN DAS KEINE FEHLER WEGEN INMEMORYFS KOMMEN -- WIRD FÜR ONDISK NICHT BENUTZT

struct MyFsFileInfo {
    char name[NAME_LENGTH +1]{};
    size_t st_size;
    char * data;
    uid_t st_uid;
    gid_t st_gid;
    mode_t st_mode;
    time_t t_atime, t_ctime, t_mtime;

    MyFsFileInfo() {
        name[0] = '\0';
        st_size = 0;
        data = nullptr;
        st_uid = 0;
        st_gid = 0;
        st_mode = 0;
        t_atime = 0;
        t_ctime = 0;
        t_mtime = 0;
    }
};

#endif /* myfs_structs_h */
