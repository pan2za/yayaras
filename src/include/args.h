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

#ifndef ARGS_H
#define ARGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "lzip.h"
#include "lyara.h"
#include "log.h"

#define ARGS_INPUT_TYPE_UNDEFINED 0
#define ARGS_INPUT_TYPE_YARA_FILE 1
#define ARGS_INPUT_TYPE_ZIP_FILE  2
#define ARGS_INPUT_TYPE_ZIP_URL   3
#define ARGS_INPUT_TYPE_YARA_URL  4
#define ARGS_INPUT_TYPE_CONFIG    5

#define ARGS_SCAN_TYPE_UNDEFINED 0
#define ARGS_SCAN_TYPE_FILE      1
#define ARGS_SCAN_TYPE_FOLDER    2

struct args{
  unsigned int iInputType;
  unsigned int iScanType;
  char *pInput;
  char *pScan;
  char *pBackup;
  char *pOutput;
  bool bAppend;
  bool bHelp;
  bool bVerbose;
  unsigned int iThreads;
  bool bExecuted;
};

void args_set_input_type(struct args *args_t){
  if (args_t->pInput != NULL &&
      common_is_file_read(args_t->pInput) == true &&
      common_is_zip_file(args_t->pInput) == true){
    args_t->iInputType = ARGS_INPUT_TYPE_ZIP_FILE;
  }
  if (args_t->pInput != NULL &&
      common_is_file_read(args_t->pInput) == true &&
      common_is_zip_file(args_t->pInput) == false){
    args_t->iInputType = ARGS_INPUT_TYPE_YARA_FILE;
  }
  if (args_t->pScan != NULL &&
      common_is_file_read(args_t->pScan) == true){
    args_t->iScanType = ARGS_SCAN_TYPE_FILE;
  }
  if (args_t->pScan != NULL &&
      common_is_folder(args_t->pScan) == true){
    args_t->iScanType = ARGS_SCAN_TYPE_FOLDER;
  }
}

void args_help(){
  printf(
         "yayaras (Yet Another YARA Scanner) v1.0\n"
         "  -h --help       Help Menu\n"
         "  -i --input      Yara Sig (ZIP|YARA)\n"
         //"  -b --backup     Backup Sigs to Folder\n"
         "  -s --scan       Scan (file)\n"
         "  -a --append     Append Output\n"
         "  -o --output     Output\n"
         //"  -t --threads    Threads\n"
         "  -v --verbose    Verbose\n"
         "Author: Lilly Chalupowski\n"
         );
}

void args_init(struct args *pArgs){
  pArgs->iScanType = ARGS_SCAN_TYPE_UNDEFINED;
  pArgs->iInputType = ARGS_INPUT_TYPE_UNDEFINED;
  pArgs->pInput = NULL;
  pArgs->pScan = NULL;
  pArgs->pBackup = NULL;
  pArgs->pOutput = NULL;
  pArgs->bAppend = false;
  pArgs->bVerbose = false;
  pArgs->bHelp = false;
  pArgs->iThreads = 1;
  pArgs->bExecuted = false;
}

bool args_module_logging(struct args *args_t, struct log *log_t){
  if (args_t->pOutput != NULL){
    if (args_t->bAppend == true){
      log_open_file(args_t->pOutput, LOG_MODE_APPEND, log_t);
      log_redir_stream(log_t, stdout);
    } else{
      log_open_file(args_t->pOutput, LOG_MODE_WRITE, log_t);
      log_redir_stream(log_t, stdout);
    }
  }
  return true;
}

bool args_module_scan_file_using_zip_file(struct args *args_t){
  if (args_t->pInput != NULL &&
      args_t->pScan != NULL &&
      args_t->iScanType == ARGS_SCAN_TYPE_FILE &&
      args_t->iInputType == ARGS_INPUT_TYPE_ZIP_FILE){
    if (args_t->bVerbose == true){
      printf("[*] %s\n", args_t->pInput);
    }
    struct file file_t;
    file_init(&file_t);
    file_set_pfilename(args_t->pInput, &file_t);
    void *pZipFile = file_read_to_memory(&file_t);
    struct lyara lyara_t;
    lyara_init(&lyara_t);
    lyara_compile_with_zip_in_memory(pZipFile, file_get_isize(&file_t), &lyara_t, args_t->bVerbose);
    if (args_t->bVerbose == true){
      printf("[*] scanning %s\n", args_t->pScan);
    }
    lyara_scan_file(args_t->pScan, &lyara_t);
    lyara_cleanup(&lyara_t);
    args_t->bExecuted = true;
    if (args_t->bVerbose == true){
      printf("[*] scan completed\n");
    }
    return true;
  }
  return false;
}

bool args_module_scan_file_using_yara_file(struct args *args_t){
  if (args_t->pInput != NULL &&
      args_t->pScan != NULL &&
      args_t->iScanType == ARGS_SCAN_TYPE_FILE &&
      args_t->iInputType == ARGS_INPUT_TYPE_YARA_FILE){
    if (args_t->bVerbose == true){
      printf("[*] %s\n", args_t->pInput);
    }
    struct lyara lyara_t;
    lyara_init(&lyara_t);
    lyara_compile_by_yara_file(args_t->pInput, &lyara_t, args_t->bVerbose);
    if (args_t->bVerbose == true){
      printf("[*] scanning %s\n", args_t->pScan);
    }
    lyara_scan_file(args_t->pScan, &lyara_t);
    lyara_cleanup(&lyara_t);
    args_t->bExecuted = true;
    if (args_t->bVerbose == true){
      printf("[*] scan completed\n");
    }
    return true;
  }
  return false;
}

void args_parse(int argc, char **argv, struct args *args_t){
  if (argc <= 1){
    args_help();
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < argc; i++){
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
      args_t->bHelp = true;
      args_help();
      exit(EXIT_SUCCESS);
    }
    if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0){
      args_t->pInput = argv[i+1];
      if (common_is_file_read(args_t->pInput) == false){
        exit(EXIT_FAILURE);
      }
    }
    if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--scan") == 0){
      args_t->pScan = argv[i+1];
    }
    if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0){
      args_t->pOutput = argv[i+1];
      if (common_is_folder(args_t->pOutput) == true){
        fprintf(stderr, "[x] output cannot be a folder\n");
        exit(EXIT_FAILURE);
      }
    }
    if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--append") == 0){
      args_t->bAppend = true;
    }
    /* if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--backup") == 0){ */
    /*   args_t->pBackup = argv[i+1]; */
    /*   if (common_is_folder(args_t->pBackup) == false){ */
    /*     exit(EXIT_FAILURE); */
    /*   } */
    /* } */
    /* if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--threads") == 0){ */
    /*   args_t->iThreads = atoi(argv[i+1]); */
    /* } */
    if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
      args_t->bVerbose = true;
    }
  }
  args_set_input_type(args_t);
  struct log log_t;
  log_init(&log_t);
  args_module_logging(args_t, &log_t);
  args_module_scan_file_using_zip_file(args_t);
  args_module_scan_file_using_yara_file(args_t);
  log_cleanup(&log_t);
  if (args_t->bExecuted == false){
    args_help();
    exit(EXIT_FAILURE);
  }
}

void args_cleanup(struct args *pArgs){
  args_init(pArgs);
}

#endif
