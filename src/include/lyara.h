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

struct lyara{
  YR_COMPILER *compiler;
  YR_RULES *rules;
};

int lyara_scan_callback(int message, void *message_data, void* user_data){
  if (message == CALLBACK_MSG_RULE_MATCHING){
    printf("[*] match\n");
    printf("    id   : %s\n", ((struct YR_RULE *)message_data)->identifier);
    printf("    tags :");
    const char *tag;
    yr_rule_tags_foreach(((struct YR_RULE *)message_data), tag){
      printf(" %s", tag);
    }
    printf("\n");
    YR_META *meta;
    unsigned int iMeta = 0;
    yr_rule_metas_foreach(((struct YR_RULE *)message_data), meta){
      if (meta->type == META_TYPE_STRING){
        if (iMeta == 0){
          printf("    meta : %s = %s\n", meta->identifier, meta->string);
        } else{
          printf("         : %s = %s\n", meta->identifier, meta->string);
        }
        iMeta++;
      }
    }
  }
  return CALLBACK_CONTINUE;
}

void lyara_init(struct lyara *lyara_t){
  yr_initialize();
  yr_compiler_create(&lyara_t->compiler);
}

void lyara_cleanup(struct lyara *lyara_t){
  yr_finalize();
  yr_compiler_destroy(lyara_t->compiler);
}

void lyara_scan_file(char *pFileName, struct lyara *lyara_t){
  if (yr_compiler_get_rules(lyara_t->compiler,
                            &lyara_t->rules) == ERROR_INSUFFICIENT_MEMORY){
    fprintf(stderr, "[x] insufficient memory for YARA signatures\n");
    lyara_cleanup(lyara_t);
    exit(EXIT_FAILURE);
  }
  yr_rules_scan_file(lyara_t->rules,
                     pFileName,
                     SCAN_FLAGS_FAST_MODE,
                     lyara_scan_callback,
                     NULL,
                     0);
}

int lyara_check_string(char *pRuleString){
  struct lyara lyara_t;
  lyara_init(&lyara_t);
  int result = yr_compiler_add_string(lyara_t.compiler,
                                      pRuleString,
                                      NULL);
  lyara_cleanup(&lyara_t);
  return result;
}

bool lyara_check_file(char *pFileName){
  FILE *fp = fopen(pFileName, "rb");
  struct lyara lyara_t;
  lyara_init(&lyara_t);
  int result = yr_compiler_add_file(lyara_t.compiler,
                                    fp,
                                    NULL,
                                    pFileName);
  lyara_cleanup(&lyara_t);
  fclose(fp);
  return result;
}


bool lyara_compile_by_yara_file(char *pFileName, struct lyara  *lyara_t, bool bVerbose){
  if (lyara_check_file(pFileName) == 0){
    if (bVerbose == true){
      printf("[+] %s\n", pFileName);
    }
    char namespace_uuid[COMMON_UUID_SIZE];
    FILE *fp = fopen(pFileName, "rb");
    common_get_uuid(namespace_uuid);
    yr_compiler_add_file(lyara_t->compiler, fp, namespace_uuid, pFileName);
    return true;
  }
  if (bVerbose == true){
    printf("[x] %s\n", pFileName);
  }
  return false;
}

bool lyara_compile_by_zip_index(unsigned long iZipIndex,
                                struct lzip *lzip_t,
                                struct lyara *lyara_t,
                                bool bVerbose){
  struct lzip_extracted lzip_extracted_t;
  char namespace_uuid[COMMON_UUID_SIZE];
  const char *pFileExt = NULL;
  lzip_stat_index(iZipIndex, lzip_t);
  pFileExt = _file_get_extension(lzip_t->zip_stat_t.name);
  if (pFileExt != NULL && (
                           strcmp(pFileExt, ".yara") == 0 ||
                           strcmp(pFileExt, ".yar") == 0)){
    lzip_extract_index_to_memory(iZipIndex, lzip_t, &lzip_extracted_t);
    int result = lyara_check_string((char *)lzip_extracted_t.pExtractedFile);
    if (result == 0){
      if (bVerbose == true){
        printf("[+] %s\n", lzip_t->zip_stat_t.name);
      }
      common_get_uuid(namespace_uuid);
      yr_compiler_add_string(lyara_t->compiler,
                             (char *)lzip_extracted_t.pExtractedFile,
                             namespace_uuid);
      lzip_cleanup_extractedfile(&lzip_extracted_t);
      return true;
    }
    if (bVerbose == true){
      printf("[x] %s\n", lzip_t->zip_stat_t.name);
    }
    lzip_cleanup_extractedfile(&lzip_extracted_t);
    return false;
  }
  return false;
}

bool lyara_compile_with_zip_in_memory(void *pZipFile,
                                      unsigned long iZipFileSize,
                                      struct lyara *lyara_t, bool bVerbose){
  bool result = false;
  struct lzip lzip_t;
  lzip_init(&lzip_t);
  lzip_open_from_memory(pZipFile, iZipFileSize, &lzip_t);
  for (unsigned long i = 0; i < lzip_get_ientries(&lzip_t); i++){
    if (lyara_compile_by_zip_index(i, &lzip_t, lyara_t, bVerbose) == true){
      result = true;
    }
  }
  lzip_cleanup(&lzip_t);
  return result;
}

#endif
