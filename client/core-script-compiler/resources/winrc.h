#pragma once

#define VERSION_MAJOR_MINOR_STR "1.0"
#define VERSION_MAJOR_MINOR_PATCH_STR "1.0.0"
#define VERSION_FULL_STR "1.0.0.0"
#define VERSION_RESOURCE 1, 0, 0, 0
#define VERSION_RESOURCE_STR VERSION_FULL_STR "\0"

/*
 * These properties are part of VarFileInfo.
 * For more information, please see: https://learn.microsoft.com/en-us/windows/win32/menurc/varfileinfo-block
 */
#define PRODUCT_LANGUAGE 0x0409 // en-US
#define PRODUCT_CHARSET 1200    // Unicode

#define APP_ICON "icon.ico"
#define APP_NAME "Aya\0"
#define APP_DESCRIPTION "Aya Core Script Compiler\0"
#define APP_ORGANIZATION "Aya\0"
#define APP_COPYRIGHT "Aya License\0"