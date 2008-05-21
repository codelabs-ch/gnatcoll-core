#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef HAVE_MMAP
#include <sys/mman.h>

int gnatcoll_has_mmap() {
  return 1;
}

void *gnatcoll_mmap(void *start, long length, int prot, int flags,
                  int fd, long offset)
{
  return mmap (start, (size_t)length, prot, flags, fd, (off_t)offset);
}

int gnatcoll_munmap(void *start, long length) {
  return munmap (start, (size_t)length);
}



#else

int gnatcoll_has_mmap() {
  return 0;
}

void *gnatcoll_mmap(void *start, long length, int prot, int flags,
                  int fd, long offset)
{
  return (void*)0;
}

int gnatcoll_munmap(void *start, long length) {
  return 0;
}

#endif

int
__gnatcoll_get_logical_drive_strings (char *buffer, int len)
{
#ifdef _WIN32
  return GetLogicalDriveStringsA ((DWORD)len, (LPSTR)buffer);
#else
  return 0;
#endif
}

void
__gnatcoll_set_readable (char *file, int set)
{
#ifdef _WIN32
  /* ??? NOT CURRENTLY SUPPORTED.
     There is no support for setting a file as unreadable using the
     standard chmod routine on Windows. With this routine it is only
     possible to set a file as read-only. To set a file as unreadable it is
     required to use the more complex [Get|Set]FileSecurity Win32 API by
     setting the proper ACL. */
#else
  struct stat statbuf;

  if (!__gnat_stat (file, &statbuf))
    {
      if (set)
        chmod (file, statbuf.st_mode | S_IREAD);
      else
        chmod (file, statbuf.st_mode & (~S_IREAD));
    }
#endif
}

/**********************************************************
 ** __gnatcoll_get_tmp_dir ()
 ** Return the tmp directory.
 ** Return value must be freed by caller
 **********************************************************/

char*
__gnatcoll_get_tmp_dir (void)
{
  static char *result = NULL;

  /* test static result to see if result has already been found */
  if (result != NULL)
    return strdup (result);

/* ??? we should use windows interface to retrieve the tmp directory
 * However, we're too close to the release to change the current behavior. As
 * soon as we are ready to do so, replace the following #if 0 by #ifdef WIN32
 */
#if 0
  DWORD dwRet;

  result = malloc ((MAX_PATH + 1) * sizeof (char));
  dwRet = GetTempPath (MAX_PATH, result);
  if (dwRet > 0) {
    result[dwRet] = '\0';
    if (__gnat_is_directory (result))
      return strdup (result);
  }
  free (result);
#endif

  result = getenv ("TMPDIR");
  if (result)
    if (__gnat_is_directory (result))
      return strdup (result);

  result = getenv ("TMP");
  if (result)
    if (__gnat_is_directory (result))
      return strdup (result);

  /* On Windows systems, this is the documented way of retrieving the tmp dir.
   * However, the TMP env variable should also be defined */
  result = getenv ("TEMP");
  if (result)
    if (__gnat_is_directory (result))
      return strdup (result);

  /* need to duplicate twice: one is for caching, the second one will be freed
   * by user */
  result = strdup ("/tmp");
  return strdup (result);
}

