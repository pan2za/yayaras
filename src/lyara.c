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
#include <yara.h>
#include <stdbool.h>
#include "lzip.h"
#include "file.h"
#include "common.h"

#include "include/lyara.h"

int lyara_scan_callback(int message, void *message_data, void* user_data){
  if (message == CALLBACK_MSG_RULE_MATCHING){
    printf("[*] match %s\n", ((struct lyara *)user_data)->pFileName);
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
  for(int i = 0; i < MAX_COMPILER_NUM; i++){
    yr_compiler_create(&lyara_t->compiler[i]);
  }
}

void lyara_cleanup(struct lyara *lyara_t){
    for(int i = 0; i < MAX_COMPILER_NUM; i++){
      yr_compiler_destroy(lyara_t->compiler[i]);
    }
}

void lyara_scan_file(char *pFileName, struct lyara *lyara_t){
  for(int i = 0; i < MAX_COMPILER_NUM; i++){
    if (yr_compiler_get_rules(lyara_t->compiler[i],
                              &lyara_t->rules[i]) == ERROR_INSUFFICIENT_MEMORY){
      fprintf(stderr, "[x] insufficient memory for YARA signatures\n");
      lyara_cleanup(lyara_t);
      yr_finalize();
      exit(EXIT_FAILURE);
    }
  }
  lyara_t->pFileName = pFileName;
  for(int i = 0; i < MAX_COMPILER_NUM; i++){
      printf(" scan compiler id %d\r\n", i);
      yr_rules_scan_file(lyara_t->rules[i],
                        pFileName,
                        0,
                        lyara_scan_callback,
                        lyara_t,
                        0);

  }
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
  int result = yr_compiler_add_file(lyara_t.compiler[0],
                                    fp,
                                    NULL,
                                    NULL);
  lyara_cleanup(&lyara_t);
  fclose(fp);
  return result;
}


bool lyara_compile_by_yara_file(char *pFileName, struct lyara  *lyara_t, unsigned long idx,bool bVerbose){
  if (idx >= MAX_COMPILER_NUM){
    return false;
  }
  if (lyara_check_file(pFileName) != 0){
    if (bVerbose == true){
      printf("[x] %s\n", pFileName);
    }
    return false;
  }
  char namespace_uuid[COMMON_UUID_SIZE];
  FILE *fp = fopen(pFileName, "rb");
  common_get_uuid(namespace_uuid);
  int result = yr_compiler_add_file(lyara_t->compiler[idx], fp, namespace_uuid, NULL);
  if(result){
    if (bVerbose == true){
      printf("[x] %d %s\n", idx, pFileName);
    }
    return false;
  }else{
    if (bVerbose == true){
      printf("[+] %d %s\n", idx, pFileName);
    }
    return true;
  }
}
bool test_yara_compile(char *pFileName, bool bVerbose){
  FILE *fp = fopen(pFileName, "rb");
  char logf[1024] = "/tmp/lyara.log";
  yr_initialize();
  struct lyara lyara_t;
  lyara_init(&lyara_t);
  yr_compiler_create(&lyara_t.compiler);
  int result = yr_compiler_add_file(lyara_t.compiler[0], fp, NULL, pFileName);
  if(result){
    if (bVerbose == true){
      printf("[x] %s %d\n", pFileName, result);
    }
    return false;
  }else{
    if (bVerbose == true){
      printf("[+] %s\n", pFileName);
    }
    lyara_scan_file("/mnt/sdb1/yayaras/eicar", &lyara_t);
    return true;
  }

}


bool lyara_unzip_by_zip_index(unsigned long iZipIndex,
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
    lzip_extract_index_to_file(iZipIndex, lzip_t, &lzip_extracted_t);
    lzip_cleanup_extractedfile(&lzip_extracted_t);
    return false;
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
  //头层的文件都是索引文件，不需要解析
  if (pFileExt != NULL 
            && (
                           strcmp(pFileExt, ".yara") == 0 ||
                           strcmp(pFileExt, ".yar") == 0)
           // && strstr(lzip_t->zip_stat_t.name, "/") != NULL
                           ){
    lzip_extract_index_name(iZipIndex, lzip_t, &lzip_extracted_t);
    lyara_compile_by_yara_file(lzip_extracted_t.name, lyara_t, iZipIndex, bVerbose);
    return false;
  }
  return false;
}

bool lyara_compile_with_zip_in_memory(void *pZipFile,
                                      unsigned long iZipFileSize,
                                      struct lyara *lyara_t,
                                      bool bVerbose){
  bool result = false;
  struct lzip lzip_t;
  lzip_init(&lzip_t);
  lzip_open_from_memory(pZipFile, iZipFileSize, &lzip_t);
  //文件全部解压缩出来，写入/tmp/目录
  for (unsigned long i = 0; i < lzip_get_ientries(&lzip_t); i++){
    if (lyara_unzip_by_zip_index(i, &lzip_t, lyara_t, bVerbose) == true){
      result = true;
    }
  }
  //按照解压缩后的文件进行读入yara规则
  for (unsigned long i = 0; i < lzip_get_ientries(&lzip_t); i++){
    if (lyara_compile_by_zip_index(i, &lzip_t, lyara_t, bVerbose) == true){
      result = true;
    }
  }
  lzip_cleanup(&lzip_t);
  return result;
}


