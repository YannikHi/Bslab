//
// Created by Oliver Waldhorst on 20.03.20.
//  Copyright © 2017-2020 Oliver Waldhorst. All rights reserved.
//

#include "myinmemoryfs.h"

// The functions fuseGettattr(), fuseRead(), and fuseReadDir() are taken from
// an example by Mohammed Q. Hussain. Here are original copyrights & licence:

/**
 * Simple & Stupid Filesystem.
 *
 * Mohammed Q. Hussain - http://www.maastaar.net
 *
 * This is an example of using FUSE to build a simple filesystem. It is a part of a tutorial in MQH Blog with the title
 * "Writing a Simple Filesystem Using FUSE in C":
 * http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
 *
 * License: GNU GPL
 */

// For documentation of FUSE methods see https://libfuse.github.io/doxygen/structfuse__operations.html

#undef DEBUG

// TODO: Comment lines to reduce debug messages
#define DEBUG
#define DEBUG_METHODS
#define DEBUG_RETURN_VALUES

#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "macros.h"
#include "myfs.h"
#include "myfs-info.h"
#include "blockdevice.h"

/// @brief Constructor of the in-memory file system class.
///
/// You may add your own constructor code here.
MyInMemoryFS::MyInMemoryFS() : MyFS() {



    // TODO: [PART 1] Add your constructor code here

}

/// @brief Destructor of the in-memory file system class.
///
/// You may add your own destructor code here.
MyInMemoryFS::~MyInMemoryFS() {

    // TODO: [PART 1] Add your cleanup code here

}

/// @brief Create a new file.
///
/// Create a new file with given name and permissions.
/// You do not have to check file permissions, but can assume that it is always ok to access the file.
/// \param [in] path Name of the file, starting with "/".
/// \param [in] mode Permissions for file access.
/// \param [in] dev Can be ignored.
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseMknod(const char *path, mode_t mode, dev_t dev) {
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

/// @brief Delete a file.
///
/// Delete a file with given name from the file system.
/// You do not have to check file permissions, but can assume that it is always ok to access the file.
/// \param [in] path Name of the file, starting with "/".
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseUnlink(const char *path) {
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

/// @brief Rename a file.
///
/// Rename the file with with a given name to a new name.
/// Note that if a file with the new name already exists it is replaced (i.e., removed
/// before renaming the file.
/// You do not have to check file permissions, but can assume that it is always ok to access the file.
/// \param [in] path Name of the file, starting with "/".
/// \param [in] newpath  New name of the file, starting with "/".
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseRename(const char *path, const char *newpath) {
    LOGM();

    // TODO: [PART 1] Implement this!

    return 0;
}

/// @brief Get file meta data.
///
/// Get the metadata of a file (user & group id, modification times, permissions, ...).
/// \param [in] path Name of the file, starting with "/".
/// \param [out] statbuf Structure containing the meta data, for details type "man 2 stat" in a terminal.
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseGetattr(const char *path, struct stat *statbuf) {
    LOGM();

    // TODO: [PART 1] Implement this!

    LOGF( "\tAttributes of %s requested\n", path );

    // GNU's definitions of the attributes (http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html):
    // 		st_uid: 	The user ID of the file’s owner.
    //		st_gid: 	The group ID of the file.
    //		st_atime: 	This is the last access time for the file.
    //		st_mtime: 	This is the time of the last modification to the contents of the file.
    //		st_mode: 	Specifies the mode of the file. This includes file type information (see Testing File Type) and
    //		            the file permission bits (see Permission Bits).
    //		st_nlink: 	The number of hard links to the file. This count keeps track of how many directories have
    //	             	entries for this file. If the count is ever decremented to zero, then the file itself is
    //	             	discarded as soon as no process still holds it open. Symbolic links are not counted in the
    //	             	total.
    //		st_size:	This specifies the size of a regular file in bytes. For files that are really devices this field
    //		            isn’t usually meaningful. For symbolic links this specifies the length of the file name the link
    //		            refers to.

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
//  else if ( strcmp( path, "/file54" ) == 0 || ( strcmp( path, "/file349" ) == 0 ) )
//  {
//      statbuf->st_mode = S_IFREG | 0644;
//      statbuf->st_nlink = 1;
//      statbuf->st_size = 1024;
//  }
    else if ( strcmp( path+1, allMyfiles[t].name ) == 0) {
        statbuf->st_nlink = 1;
        statbuf->st_mode = allMyfiles[t].st_mode;
        statbuf->st_size = allMyfiles[t].st_size;
        //statbuf->st_gid = allMyfiles[t].st_gid;
        //statbuf->st_uid = allMyfiles[t].st_uid;
        //statbuf->st_atime = allMyfiles[t].t_atime;
        //statbuf->st_mtime = allMyfiles[t].t_mtime;
        //statbuf->st_ctime = allMyfiles[t].t_ctime;
    }
    else {
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

/// @brief Change file permissions.
///
/// Set new permissions for a file.
/// You do not have to check file permissions, but can assume that it is always ok to access the file.
/// \param [in] path Name of the file, starting with "/".
/// \param [in] mode New mode of the file.
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseChmod(const char *path, mode_t mode) {
    LOGM();

    // TODO: [PART 1] Implement this!

    RETURN(0);
}

/// @brief Change the owner of a file.
///
/// Change the user and group identifier in the meta data of a file.
/// You do not have to check file permissions, but can assume that it is always ok to access the file.
/// \param [in] path Name of the file, starting with "/".
/// \param [in] uid New user id.
/// \param [in] gid New group id.
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseChown(const char *path, uid_t uid, gid_t gid) {
    LOGM();

    // TODO: [PART 1] Implement this!

    RETURN(0);
}

/// @brief Open a file.
///
/// Open a file for reading or writing. This includes checking the permissions of the current user and incrementing the
/// open file count.
/// You do not have to check file permissions, but can assume that it is always ok to access the file.
/// \param [in] path Name of the file, starting with "/".
/// \param [out] fileInfo Can be ignored in Part 1
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseOpen(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: [PART 1] Implement this!

    RETURN(0);
}

/// @brief Read from a file.
///
/// Read a given number of bytes from a file starting form a given position.
/// You do not have to check file permissions, but can assume that it is always ok to access the file.
/// Note that the file content is an array of bytes, not a string. I.e., it is not (!) necessarily terminated by '\0'
/// and may contain an arbitrary number of '\0'at any position. Thus, you should not use strlen(), strcpy(), strcmp(),
/// ... on both the file content and buf, but explicitly store the length of the file and all buffers somewhere and use
/// memcpy(), memcmp(), ... to process the content.
/// \param [in] path Name of the file, starting with "/".
/// \param [out] buf The data read from the file is stored in this array. You can assume that the size of buffer is at
/// least 'size'
/// \param [in] size Number of bytes to read
/// \param [in] offset Starting position in the file, i.e., number of the first byte to read relative to the first byte of
/// the file
/// \param [in] fileInfo Can be ignored in Part 1
/// \return The Number of bytes read on success. This may be less than size if the file does not contain sufficient bytes.
/// -ERRNO on failure.
int MyInMemoryFS::fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: [PART 1] Implement this!

//  LOGF( "--> Trying to read %s, %lu, %lu\n", path, (unsigned long) offset, size );
//  char file54Text[] = "Hello World From File54!\n";
//  char file349Text[] = "Hello World From File349!\n";
//  char *selectedText = NULL;
//
//  // ... //
//
//  if ( strcmp( path, "/file54" ) == 0 )
//      selectedText = file54Text;
//  else if ( strcmp( path, "/file349" ) == 0 )
//      selectedText = file349Text;
//  else
//      return -ENOENT;
//
//  // ... //
//
//  memcpy( buf, selectedText + offset, size );
//
//  RETURN((int) (strlen( selectedText ) - offset));

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

/// @brief Write to a file.
///
/// Write a given number of bytes to a file starting at a given position.
/// You do not have to check file permissions, but can assume that it is always ok to access the file.
/// Note that the file content is an array of bytes, not a string. I.e., it is not (!) necessarily terminated by '\0'
/// and may contain an arbitrary number of '\0'at any position. Thus, you should not use strlen(), strcpy(), strcmp(),
/// ... on both the file content and buf, but explicitly store the length of the file and all buffers somewhere and use
/// memcpy(), memcmp(), ... to process the content.
/// \param [in] path Name of the file, starting with "/".
/// \param [in] buf An array containing the bytes that should be written.
/// \param [in] size Number of bytes to write.
/// \param [in] offset Starting position in the file, i.e., number of the first byte to read relative to the first byte of
/// the file.
/// \param [in] fileInfo Can be ignored in Part 1 .
/// \return Number of bytes written on success, -ERRNO on failure.
int MyInMemoryFS::fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: [PART 1] Implement this!
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

    RETURN(0);
}

/// @brief Close a file.
///
/// In Part 1 this includes decrementing the open file count.
/// \param [in] path Name of the file, starting with "/".
/// \param [in] fileInfo Can be ignored in Part 1 .
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseRelease(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: [PART 1] Implement this!

    RETURN(0);
}

/// @brief Truncate a file.
///
/// Set the size of a file to the new size. If the new size is smaller than the old size, spare bytes are removed. If
/// the new size is larger than the old size, the new bytes may be random.
/// You do not have to check file permissions, but can assume that it is always ok to access the file.
/// \param [in] path Name of the file, starting with "/".
/// \param [in] newSize New size of the file.
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseTruncate(const char *path, off_t newSize) {
    LOGM();

    // TODO: [PART 1] Implement this!
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

/// @brief Truncate a file.
///
/// Set the size of a file to the new size. If the new size is smaller than the old size, spare bytes are removed. If
/// the new size is larger than the old size, the new bytes may be random. This function is called for files that are
/// open.
/// You do not have to check file permissions, but can assume that it is always ok to access the file.
/// \param [in] path Name of the file, starting with "/".
/// \param [in] newSize New size of the file.
/// \param [in] fileInfo Can be ignored in Part 1.
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseTruncate(const char *path, off_t newSize, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: [PART 1] Implement this!

    RETURN(0);
}

/// @brief Read a directory.
///
/// Read the content of the (only) directory.
/// You do not have to check file permissions, but can assume that it is always ok to access the directory.
/// \param [in] path Path of the directory. Should be "/" in our case.
/// \param [out] buf A buffer for storing the directory entries.
/// \param [in] filler A function for putting entries into the buffer.
/// \param [in] offset Can be ignored.
/// \param [in] fileInfo Can be ignored.
/// \return 0 on success, -ERRNO on failure.
int MyInMemoryFS::fuseReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();

    // TODO: [PART 1] Implement this!

    LOGF( "--> Getting The List of Files of %s\n", path );

    filler( buf, ".", NULL, 0 ); // Current Directory
    filler( buf, "..", NULL, 0 ); // Parent Directory

    if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
    {
//      filler( buf, "file54", NULL, 0 );
//      filler( buf, "file349", NULL, 0 );
        for (int t = 0; t < NUM_DIR_ENTRIES; t++) {
            if (strcmp(allMyfiles[t].name, "\0" ) != 0) {
                filler( buf, allMyfiles[t].name, NULL, 0 );
            }
        }
    }

    RETURN(0);
}

/// Initialize a file system.
///
/// This function is called when the file system is mounted. You may add some initializing code here.
/// \param [in] conn Can be ignored.
/// \return 0.
void* MyInMemoryFS::fuseInit(struct fuse_conn_info *conn) {
    // Open logfile
    this->logFile= fopen(((MyFsInfo *) fuse_get_context()->private_data)->logFile, "w+");
    if(this->logFile == NULL) {
        fprintf(stderr, "ERROR: Cannot open logfile %s\n", ((MyFsInfo *) fuse_get_context()->private_data)->logFile);
    } else {
        // turn of logfile buffering
        setvbuf(this->logFile, NULL, _IOLBF, 0);

        LOG("Starting logging...\n");

        LOG("Using in-memory mode");

        // TODO: [PART 1] Implement your initialization methods here
    }

    RETURN(0);
}

/// @brief Clean up a file system.
///
/// This function is called when the file system is unmounted. You may add some cleanup code here.
void MyInMemoryFS::fuseDestroy() {
    LOGM();

    // TODO: [PART 1] Implement this!

}

// TODO: [PART 1] You may add your own additional methods here!

int MyInMemoryFS::findIndex(const char *path) {
    int ret = -2;

    for (int t = 0; t <= NUM_DIR_ENTRIES; t++) {
        if(strcmp(path+1, allMyfiles[t].name) ==0) {
            ret = t;
        }
    }
    return ret;
}

int MyInMemoryFS::findFreeSpot() {
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

// DO NOT EDIT ANYTHING BELOW THIS LINE!!!

/// @brief Set the static instance of the file system.
///
/// Do not edit this method!
void MyInMemoryFS::SetInstance() {
    MyFS::_instance= new MyInMemoryFS();
}

