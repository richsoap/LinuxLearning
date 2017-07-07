#stat
##mode_t st_mode;
###Detial
They are used like functions, like S_ISREG(file), which will return true if file is a normal file or return false under other cases.

S_ISREG normal file

S_ISDIR dir file

S_ISCHR char special file

S_ISBLK block special file

S_ISFIFO FIFO

S_ISLNK link file

S_ISSOCK socket file used for communication
##ino_t st_ino;
##dev_t st_dev;
##dev_t st_rdev;
##nlink_t st_nlink
##uid_t st_uid
This file belongs to which user.
##gid_t st_gid
This file belongs to which user group.
##off_t st_size
##struct timespec st_atime
##struct timespec st_mtime
##struct timespec st_ctime
##blksize_t st_blksize
##blkcnt_t st_blocks
## uid_t set_user_id
This decides the powers of this program
## gid_t set_group_id
Like set_user_id

