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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "include/file.h"

void file_init(struct file *file_t){
  file_t->pFileName = NULL;
  file_t->iSize = 0;
  file_t->pData = NULL;
}

const char *_file_get_extension(const char *pFileName){
  char *ext = strrchr(pFileName, '.');
  if (!ext){
    return NULL;
  }
  return ext;
}

void _file_debug_stats(struct file *file_t){
  printf("pFileName: %p\n", file_t->pFileName);
  printf("iSize    : %d\n", (int)file_t->iSize);
  printf("pData    : %p\n", file_t->pData);
}

void file_set_pfilename(char *pFileName, struct file *file_t){
  file_t->pFileName = pFileName;
}

void file_set_isize(unsigned long iSize, struct file *file_t){
  file_t->iSize = iSize;
}

unsigned long file_get_isize(struct file *file_t){
  return file_t->iSize;
}

void file_set_pdata(void *pData, struct file *file_t){
  file_t->pData = pData;
}

void file_cleanup(struct file *file_t){
  if (file_t->pData != NULL){
    free(file_t->pData);
  }
  file_t->iSize = 0;
  file_t->pFileName = NULL;
}

unsigned long _file_get_size_fd(FILE *fp){
  int prev = ftell(fp);
  if (fseek(fp, 0L, SEEK_END) != 0){
    fprintf(stderr, "%s\n", strerror(errno));
    return 0;
  }
  int sz = ftell(fp);
  if (fseek(fp, prev, SEEK_SET) != 0){
    fprintf(stderr, "%s\n", strerror(errno));
    return 0;
  }
  return sz;
}

unsigned long file_get_size(struct file *file_t){
  FILE *fp = fopen(file_t->pFileName, "rb");
  if (fp == NULL){
    fprintf(stderr, "%s\n", strerror(errno));
    return 0;
  }
  file_t->iSize = _file_get_size_fd(fp);
  return file_t->iSize;
}

void *file_read_to_memory(struct file *file_t){
  if (file_t->pFileName == NULL){
    fprintf(stderr, "pFileName is undefined\n");
    return NULL;
  }
  FILE *fp = fopen(file_t->pFileName, "rb");
  if (fp == NULL){
    fprintf(stderr, "%s\n", strerror(errno));
    return NULL;
  }
  file_t->iSize = _file_get_size_fd(fp);
  file_t->pData = malloc(file_t->iSize);
  memset(file_t->pData, 0, file_t->iSize);
  fread(file_t->pData, file_t->iSize, 1, fp);
  fclose(fp);
  return file_t->pData;
}


