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

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define LOG_MODE_WRITE   0
#define LOG_MODE_APPEND  1

struct log{
  int iLogFD;
  int iLogMode;
  char *pFileName;
};

void log_init(struct log *log_t){
  log_t->iLogFD = -1;
  log_t->iLogMode = -1;
  log_t->pFileName = NULL;
}

void log_set_filename(char *pFileName, struct log *log_t){
  log_t->pFileName = pFileName;
}

void log_set_mode(int iLogMode, struct log *log_t){
  log_t->iLogMode = iLogMode;
}

bool _log_validate_filename(char *pFileName){
  if (pFileName == NULL){
    fprintf(stderr, "[x] filename not defined\n");
    return false;
  }
  return true;
}

bool log_open_file(char *pFilename, int iLogMode, struct log *log_t){
  log_set_filename(pFilename, log_t);
  if (_log_validate_filename(log_t->pFileName) == false){
    return false;
  }
  log_set_mode(iLogMode, log_t);
  if (log_t->iLogMode == LOG_MODE_APPEND){
    log_t->iLogFD = open(log_t->pFileName, O_CREAT|O_RDWR|O_APPEND, S_IRUSR|S_IWUSR);
  }
  if (log_t->iLogMode == LOG_MODE_WRITE){
    log_t->iLogFD = open(log_t->pFileName, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
  }
  if (log_t->iLogFD < 0){
    return false;
  }
  return true;
}

bool log_redir_stream(struct log *log_t, FILE *pStream){
  if (dup2(log_t->iLogFD, fileno(pStream)) < 0){
    close(log_t->iLogFD);
    log_t->iLogFD = -1;
    return false;
  }
  return false;
}

/* int log_redirect(char *pFileName,int iFlags, int iFD){ */
/*   int fd = open(pFileName, iFlags, S_IRUSR|S_IWUSR); */
/*   if(fd != -1){ */
/*     if(dup2(fd, iFD) == -1){ */
/*       close(fd); */
/*       fd = -1; */
/*     } else { */
/*       close(fd); */
/*     } */
/*   } */
/*   return fd; */
/* } */

void log_cleanup(struct log *log_t){
  log_init(log_t);
}

#endif
