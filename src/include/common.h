/*
New BSD License
---------------

Copyright © 2020 Lilly Chalupowski All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Lilly Chalupowski nor the names of its contributors may be used to
  endorse or promote products derived from this software without specific prior
  written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef COMMON_H
#define COMMON_H

#define COMMON_UUID_SIZE 37

#define COMMON_MAGIC_RESULT_SIZE 128

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <uuid/uuid.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <magic.h>

extern bool common_get_magic_file(char *pFileName,
                           char *pResult,
                           unsigned int iResultSize);

extern bool common_get_magic_buffer(void *pBuffer,
                             unsigned long iBufferSize,
                             const char *magic);

extern bool common_string_starts_with(const char *a,
                               const char *b);

extern bool common_is_zip_file(char *pFileName);

extern bool common_is_folder(char *pFolder);

extern bool common_is_file_read(char *pFile);

extern bool common_is_file_write(char *pFile);

extern bool common_get_uuid(char *uuid);

#endif
