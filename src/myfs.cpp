//
//  myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 02.08.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

// The functions fuseGettattr(), fuseRead(), and fuseReadDir() are taken from
// an example by Mohammed Q. Hussain. Here are original copyrights & licence:

/**
 * Simple & Stupid Filesystem.
 *
 * Mohammed Q. Hussain - http://www.maastaar.net
 *
 * This is an example of using FUSE to build a simple filesystem. It is a part of a tutorial in MQH Blog with the title "Writing a Simple Filesystem Using FUSE in C": http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
 *
 * License: GNU GPL
 */

// For documentation of FUSE methods see https://libfuse.github.io/doxygen/structfuse__operations.html

#undef DEBUG

// TODO: Comment this to reduce debug messages
#define DEBUG
#define DEBUG_METHODS
#define DEBUG_RETURN_VALUES

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#include <sys/dtrace.h>

#include "macros.h"
#include "myfs.h"
#include "myfs-info.h"

MyFS* MyFS::_instance = NULL;

MyFS* MyFS::Instance() {
    if(_instance == NULL) {
        _instance = new MyFS();
    }
    return _instance;
}

MyFS::MyFS() {
    this->logFile= stderr;
}

MyFS::~MyFS() {

}

int MyFS::fuseGetattr(const char *path, struct stat *statbuf) {
    //LOGM();

    // TODO: Implement this!

    //LOGF( "\tAttributes of %s requested\n", path );

    // GNU's definitions of the attributes (http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html):
    // 		st_uid: 	The user ID of the file’s owner.
    //		st_gid: 	The group ID of the file.
    //		st_atime: 	This is the last access time for the file.
    //		st_mtime: 	This is the time of the last modification to the contents of the file.
    //		st_mode: 	Specifies the mode of the file. This includes file type information (see Testing File Type) and the file permission bits (see Permission Bits).
    //		st_nlink: 	The number of hard links to the file. This count keeps track of how many directories have entries for this file. If the count is ever decremented to zero, then the file itself is discarded as soon
    //						as no process still holds it open. Symbolic links are not counted in the total.
    //		st_size:	This specifies the size of a regular file in bytes. For files that are really devices this field isn’t usually meaningful. For symbolic links this specifies the length of the file name the link refers to.

    statbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
    statbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
    statbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
    statbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

    int ret= 0;
    int t = findIndex(path);

    if ( strcmp( path, "/" ) == 0 )
    {

        statbuf->st_mode = S_IFDIR | 0755;
        statbuf->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
    }

        /*else if ( strcmp( path, "/file54" ) == 0 || ( strcmp( path, "/file349" ) == 0 ) )
        {
            statbuf->st_mode = S_IFREG | 0644;
            statbuf->st_nlink = 1;
            statbuf->st_size = 1024;

        }*/
    else if ( strcmp( path+1, allMyfiles[t].name ) == 0){
        statbuf->st_nlink = 1;
        statbuf->st_mode = allMyfiles[t].st_mode;
        statbuf->st_size = allMyfiles[t].st_size;
        //statbuf->st_gid = allMyfiles[t].st_gid;
        //statbuf->st_uid = allMyfiles[t].st_uid;
        //statbuf->st_atime = allMyfiles[t].t_atime;
        //statbuf->st_mtime = allMyfiles[t].t_mtime;
        //statbuf->st_ctime = allMyfiles[t].t_ctime;
    }
    else{

        ret= -ENOENT;
    }
    /* for (int t = 0; t <= NUM_DIR_ENTRIES; t++) {
        LOG("HALLO VON UNSEREM MIST");
        if ( strcmp(path+1, allMyfiles[t].name ) == 0 ){


    statbuf->st_mode = S_IFREG | 0644;
    statbuf->st_nlink = 1;
    statbuf->st_size = 1024;
    }
    } */

    RETURN(ret);
}

int MyFS::fuseReadlink(const char *path, char *link, size_t size) {
    LOGM();
    return 0;
}

int MyFS::fuseMknod(const char *path, mode_t mode, dev_t dev) {
    LOGM();
    int ret = -ENOSPC;
    if(findIndex(path) == -2) {
        if(findFreeSpot() != -ENOSPC){
            int t = findFreeSpot();
            LOGF("mkNod sagt: Freier Platz bei %i",t);
            strcpy(allMyfiles[t].name, path+1);
            allMyfiles[t].st_mode = mode;
            allMyfiles[t].t_ctime = dev;
            //allMyfiles[t].data = NULL;
            ret = 0;
        }
    }
    return ret;

}

int MyFS::fuseMkdir(const char *path, mode_t mode) {
    LOGM();
    return 0;
}

int MyFS::fuseUnlink(const char *path) {
    LOGM();

    // TODO: Implement this!

    if(findIndex(path) != -2 ) {
        int t = findIndex(path);
        strcpy(allMyfiles[t].name,"\0");
        allMyfiles[t].st_size = 0;
        allMyfiles[t].st_mode = 0;
        allMyfiles[t].data = NULL;
        allMyfiles[t].st_uid = 0;
        allMyfiles[t].st_gid = 0;
        allMyfiles[t].t_atime = 0;
        allMyfiles[t].t_ctime = 0;
        allMyfiles[t].t_mtime = 0;
        RETURN(0);
    } else {
        return(ENOENT);
    }
}

int MyFS::fuseRmdir(const char *path) {
    LOGM();
    return 0;
}

int MyFS::fuseSymlink(const char *path, const char *link) {
    LOGM();
    return 0;
}

int MyFS::fuseRename(const char *path, const char *newpath) {
    LOGM();
    return 0;
}

int MyFS::fuseLink(const char *path, const char *newpath) {
    LOGM();
    return 0;
}

int MyFS::fuseChmod(const char *path, mode_t mode) {
    LOGM();
    return 0;
}

int MyFS::fuseChown(const char *path, uid_t uid, gid_t gid) {
    LOGM();
    return 0;
}

int MyFS::fuseTruncate(const char *path, off_t newSize) {
    LOGM();
    // TODO: Implement this!

    int t = findIndex(path);


    if (t != -2 && allMyfiles[t].data != NULL) {
        if ((off_t) allMyfiles[t].st_size != newSize) {
            char newFileData[newSize];
            memcpy(newFileData, allMyfiles[t].data, newSize);
            allMyfiles[t].data = NULL;
            allMyfiles[t].st_size = newSize;
            allMyfiles[t].data = (char *) realloc(allMyfiles[t].data, newSize);
            memcpy(allMyfiles[t].data, newFileData, newSize);
            LOG("Datei wurde angepasst");
        }
    } else if (t != -2 && allMyfiles[t].data == NULL) {
        allMyfiles[t].st_size = newSize;
        allMyfiles[t].data = (char *) (malloc(newSize));
        LOG("Datei war davor NULL und wurde jetzt reserviert.");
    } else {
        RETURN(-ENOENT);
    }

    return 0;
}

int MyFS::fuseUtime(const char *path, struct utimbuf *ubuf) {
    LOGM();
    return 0;
}

int MyFS::fuseOpen(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGM();
    // TODO: Implement this!

    RETURN(0);
}

int MyFS::fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: Implement this in Aufgabenteil 2 mit fh (filehandler) again!

    //LOGF( "--> Trying to read %s, %lu, %lu\n", path, (unsigned long) offset, size + offset );



    int t = findIndex(path);


    // ... //
    if (t != -2) {


        if(allMyfiles[t].st_size - (offset) < size){

            memcpy( buf, allMyfiles[t].data + offset, allMyfiles[t].st_size - offset );
            RETURN((int) (allMyfiles[t].st_size - offset));

        } else {

            memcpy( buf, allMyfiles[t].data + offset, size);
            offset = size;
            RETURN((int) size);
        }


    }
    else
        return -ENOENT;

}

int MyFS::fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: Implement this!
    if (findIndex(path) != -2) {
        int i = findIndex(path);
        size_t fileSize = allMyfiles[i].st_size;

        if (fileSize == 0) {                                         // Datei vorher leer
            allMyfiles[i].st_size = size;
            allMyfiles[i].data = (char *) (malloc(offset + size));
            memcpy(allMyfiles[i].data + offset, buf, size);

        } else if ((unsigned) offset == fileSize) {                             // text genau ans Ende der Datei gehängt
            allMyfiles[i].st_size = (fileSize + size);
            allMyfiles[i].data = (char *) realloc(allMyfiles[i].data, size + fileSize);
            memcpy(allMyfiles[i].data + offset, buf, size);

        } else if ((unsigned) offset > fileSize) {                            // text hinter eigentlichem Ende der Datei angehängt
            allMyfiles[i].st_size = (offset + size);
            allMyfiles[i].data = (char *) realloc(allMyfiles[i].data, offset + size);
            memcpy(allMyfiles[i].data + offset, buf, size);

        } else if((offset + size) < allMyfiles[i].st_size){
            memcpy(allMyfiles[i].data + offset, buf, size);

        } else {                // überschneidung und text geht über vorheriges Ende hinaus

            allMyfiles[i].st_size = (offset + size);
            allMyfiles[i].data = (char *) realloc(allMyfiles[i].data, offset + size - fileSize);
            memcpy(allMyfiles[i].data + offset, buf, size);
        }

        LOGF("Größe ist: %i", (unsigned) allMyfiles[i].st_size);
        allMyfiles[i].t_mtime = time(NULL);

        RETURN((unsigned) size);
    } else {
        LOG("SHIT DA KLAPPT WAT NIKT");
        RETURN(-2);
    }

}

int MyFS::fuseStatfs(const char *path, struct statvfs *statInfo) {
    //LOGM();
    return 0;
}

int MyFS::fuseFlush(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    return 0;
}

int MyFS::fuseRelease(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: Implement this!

    RETURN(0);
}

int MyFS::fuseFsync(const char *path, int datasync, struct fuse_file_info *fi) {
    LOGM();
    return 0;
}

int MyFS::fuseListxattr(const char *path, char *list, size_t size) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseRemovexattr(const char *path, const char *name) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseOpendir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();

    // (TODO: Implement this!)

    RETURN(0);

}

int MyFS::fuseReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: Implement this!

    LOGF( "--> Getting The List of Files of %s\n", path );

    filler( buf, ".", NULL, 0 ); // Current Directory
    filler( buf, "..", NULL, 0 ); // Parent Directory

    if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
    {
        //filler( buf, "file54", NULL, 0 );
        //filler( buf, "file349", NULL, 0 );
        //filler( buf, allMyfiles[0].name, NULL, 0 );
        for (int t = 0; t < NUM_DIR_ENTRIES; t++) {
            if (strcmp(allMyfiles[t].name, "\0" ) != 0) {
                filler( buf, allMyfiles[t].name, NULL, 0 );
            }

        }

    }



    RETURN(0);
}

int MyFS::fuseReleasedir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();

    // (TODO: Implement this!)

    RETURN(0);
}

int MyFS::fuseFsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseTruncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseCreate(const char *path, mode_t mode, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: Implement this!

    RETURN(0);
}

void MyFS::fuseDestroy() {
    LOGM();
}

void* MyFS::fuseInit(struct fuse_conn_info *conn) {
    // Open logfile
    this->logFile= fopen(((MyFsInfo *) fuse_get_context()->private_data)->logFile, "w+");
    if(this->logFile == NULL) {
        fprintf(stderr, "ERROR: Cannot open logfile %s\n", ((MyFsInfo *) fuse_get_context()->private_data)->logFile);
    } else {
        //    this->logFile= ((MyFsInfo *) fuse_get_context()->private_data)->logFile;

        // turn of logfile buffering
        setvbuf(this->logFile, NULL, _IOLBF, 0);

        LOG("Starting logging...\n");
        LOGM();

        // you can get the containfer file name here:
        LOGF("Container file name: %s", ((MyFsInfo *) fuse_get_context()->private_data)->contFile);

        // TODO: Implement your initialization methods here!
        /*strcpy(allMyfiles[0].name, "Anna");
        allMyfiles[0].st_size = 1024;
        allMyfiles[0].st_mode = S_IFREG | 0644;
        //allMyfiles[0].data;
        allMyfiles[0].st_uid = 1024;
        //allMyfiles[0].st_gid;
        allMyfiles[0].t_atime = 1024;
        //allMyfiles[0].t_ctime;
        //allMyfiles[0].t_mtime;

        strcpy(allMyfiles[1].name, "Theo");
        allMyfiles[1].st_size = 1024;
        allMyfiles[1].st_mode = S_IFREG | 0644;
        //allMyfiles[1].data;
        allMyfiles[1].st_uid = getuid();
        allMyfiles[1].st_gid = getgid();
        allMyfiles[1].t_atime = time( NULL );
        allMyfiles[1].t_ctime = 1024;
        allMyfiles[1].t_mtime =time( NULL );

        strcpy(allMyfiles[2].name, "Mai");
        allMyfiles[2].st_size = 1024;
        allMyfiles[2].st_mode = S_IFREG | 0644;
        //allMyfiles[2].data;
        allMyfiles[2].st_uid = 1024;
        //allMyfiles[2].st_gid;
        allMyfiles[2].t_atime = 1024;
        //allMyfiles[2].t_ctime;
        //allMyfiles[2].t_mtime;*/

    }

    RETURN(0);
}

#ifdef __APPLE__
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x) {
#else
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
#endif
    //LOGM();
    RETURN(0);
}

#ifdef __APPLE__
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x) {
#else
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size) {
#endif
    //LOGM();
    RETURN(0);
}

// TODO: Add your own additional methods here!


int MyFS::findIndex(const char *path) {
    int ret = -2;

    for (int t = 0; t <= NUM_DIR_ENTRIES; t++) {
        if(strcmp(path+1, allMyfiles[t].name) ==0) {
            ret = t;
        }
    }
    return ret;
}

int MyFS::findFreeSpot() {
    int ret = -ENOSPC;

    for (int t = 0; t < NUM_DIR_ENTRIES && t >= 0; t++) {
        if(strcmp(allMyfiles[t].name, "\0" ) == 0) {
            ret = t;
            t = NUM_DIR_ENTRIES +1;
        }
    }
    LOGF("findFreeSpot sagt: Freier Platz bei %i",ret);
    return ret;
}