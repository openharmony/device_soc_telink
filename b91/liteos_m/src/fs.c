#include <sys/mount.h>

#include <device_resource_if.h>
#include <hdf_device_desc.h>
#include <hdf_log.h>
#include <lfs_adapter.h>
#include <los_config.h>

#include <los_fs.h>

#include <B91/flash.h>

int _close(int fd)
{
    return close(fd);
}

ssize_t _read(int fd, void *buf, size_t nbyte)
{
    return read(fd, buf, nbyte);
}

off_t _lseek(int fd, off_t offset, int whence)
{
    return lseek(fd, offset, whence);
}

int _fstat(int fd, struct stat *buf)
{
    return fstat(fd, buf);
}