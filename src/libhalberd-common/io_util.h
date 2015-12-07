#ifndef IO_UTIL_H
#define IO_UTIL_H
#include <stdbool.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Resource path get/set
//-----------------------------------------------------------------------------

/*!
 * This gets the path to the current base data directory.
 * By default, if this function is called with no data directory then the value
 * will be set to the directory containing the executable.
 */
const char* get_base_resource_path();

/*!
 * This constructs a string containing the path to a resource file.
 * Note that the user must free the returned string when finished.
 *
 * Usage:
 * resource_location is the extension to this directory. Setting this argument
 * to NULL will result in the file being loaded from the base resource
 * directory.
 * resource_name is the actual name of the file.
 *
 * Example 1: resources/data/foo.bar
 * resource_location: "data"
 * resource_name: "foo.bar"
 *
 * Example 2: resources/foo.bar
 * resource_location: NULL
 * resource_name: "foo.bar"
 */
char* construct_extended_resource_path(const char* resource_location, const char* resource_name);

/*!
 * This sets the path of the base data directory.
 */
void set_resource_path(const char* path);

/*!
 * This sets the path of the base data directory.
 * The path argument is treated as an extension to the directory containing the
 * executable.
 */
void set_resource_path_relative(const char* path);

//-----------------------------------------------------------------------------
// Resource loading code
//-----------------------------------------------------------------------------

/*!
 * Opens a file from the resource path and extension.
 * See get_extended_resource_path(io_util.h) for usage details
 *
 * appending a ! to the end of mode will cause this function to try swapping an
 * initial r with w if the file cannot be loaded.
 */
FILE* load_resource_file(const char* resource_location, const char* resource_name, const char* mode);

/*!
 * Loads a file in the resource directory to a new buffer.
 * The user must handle freeing the returned buffer's memory.
 * See get_extended_resource_path(io_util.h) for usage details
 */
unsigned char* load_resource_to_buffer(const char* resource_location, const char* resource_name);

//-----------------------------------------------------------------------------
// Other code
//-----------------------------------------------------------------------------

/*!
 * Checks to see if the directory at path exists. If not, the directory is
 * created.
 * Returns true if a new directory was created, and false if it existed
 * already.
 */
bool ensure_directory(const char* path);

/*!
 * Returns a string with file_name's extension changed to file_extension. If
 * file_extension is NULL, file_name's extension is removed.
 *
 * Note that the resulting string must be freed by the user.
 */
char* swap_extension(const char* file_name, const char* file_extension);
#endif
