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

#ifndef LYARA_H
#define LYARA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <yara.h>
#include <stdbool.h>
#include "lzip.h"
#include "file.h"
#include "common.h"

#define MAX_COMPILER_NUM 200
struct lyara{
  YR_COMPILER *compiler[MAX_COMPILER_NUM];
  YR_RULES *rules[MAX_COMPILER_NUM];
  char *pFileName;
};

extern int lyara_scan_callback(int message, void *message_data, void* user_data);

extern void lyara_init(struct lyara *lyara_t);
extern void lyara_cleanup(struct lyara *lyara_t);

extern void lyara_scan_file(char *pFileName, struct lyara *lyara_t);

extern int lyara_check_string(char *pRuleString);

extern bool lyara_check_file(char *pFileName);


extern bool lyara_compile_by_yara_file(char *pFileName, struct lyara  *lyara_t, unsigned long idx, bool bVerbose);

extern bool lyara_compile_by_zip_index(unsigned long iZipIndex,
                                struct lzip *lzip_t,
                                struct lyara *lyara_t,
                                bool bVerbose);
extern bool lyara_compile_with_zip_in_memory(void *pZipFile,
                                      unsigned long iZipFileSize,
                                      struct lyara *lyara_t,
                                      bool bVerbose);
                                    
extern void *lzip_extract_index_to_file(unsigned long iIndex,
                                   struct lzip *lzip_t,
                                   struct lzip_extracted *lzip_extracted_t);
extern void *lzip_extract_index_name(unsigned long iIndex,
                                   struct lzip *lzip_t,
                                   struct lzip_extracted *lzip_extracted_t);

#endif
