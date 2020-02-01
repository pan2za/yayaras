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

#ifndef LZIP_H
#define LZIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <errno.h>

struct lzip{
  struct zip_file *zip_file_t;
  struct zip *zip_t;
  struct zip_stat zip_stat_t;
  struct zip_source *zip_source_t;
  void *pZipFile;
  unsigned long iZipSize;
  unsigned long iEntries;
};

struct lzip_extracted{
  void *pExtractedFile;
  unsigned long iExtractedFileSize;
};

void lzip_init(struct lzip *lzip_t){
  lzip_t->zip_file_t = NULL;
  lzip_t->zip_t = NULL;
  lzip_t->zip_source_t = NULL;
  lzip_t->pZipFile = NULL;
  lzip_t->iZipSize = 0;
  lzip_t->iEntries = 0;
}

unsigned long lzip_get_ientries(struct lzip *lzip_t){
  return lzip_t->iEntries;
}

void lzip_cleanup_zip_file_t(struct lzip *lzip_t){
  if (lzip_t->zip_file_t != NULL){
    zip_fclose(lzip_t->zip_file_t);
  }
}

void lzip_cleanup_extractedfile(struct lzip_extracted *lzip_extracted_t){
  if (lzip_extracted_t->pExtractedFile != NULL){
    free(lzip_extracted_t->pExtractedFile);
    lzip_extracted_t->iExtractedFileSize = 0;
  }
}

void lzip_pzipfile_cleanup(struct lzip *lzip_t){
  if (lzip_t->pZipFile != NULL){
    free(lzip_t->pZipFile);
  }
}

void lzip_cleanup_zip_t(struct lzip *lzip_t){
  if (lzip_t->zip_t != NULL){
    zip_close(lzip_t->zip_t);
  }
}

void lzip_cleanup(struct lzip *lzip_t){
  lzip_t->iZipSize = 0;
  lzip_t->iEntries = 0;
  lzip_cleanup_zip_t(lzip_t);
  lzip_cleanup_zip_file_t(lzip_t);
}

void _lzip_set_izipsize(unsigned long iZipSize,
                        struct lzip *lzip_t){
  lzip_t->iZipSize = iZipSize;
}

void _lzip_set_ientries(unsigned long iEntries,
                        struct lzip *lzip_t){
  lzip_t->iEntries = iEntries;
}

void _lzip_set_pzipfile(void *pZipFile,
                        struct lzip *lzip_t){
  lzip_t->pZipFile = pZipFile;
}

void lzip_open_from_memory(void *pZipFile,
                           unsigned long iZipSize,
                           struct lzip *lzip_t){
  _lzip_set_pzipfile(pZipFile, lzip_t);
  _lzip_set_izipsize(iZipSize, lzip_t);
  lzip_t->zip_source_t = zip_source_buffer_create(lzip_t->pZipFile,
                                             lzip_t->iZipSize,
                                             1,
                                             NULL);
  lzip_t->zip_t = zip_open_from_source(lzip_t->zip_source_t,
                                       0,
                                       NULL);
  lzip_t->iEntries = zip_get_num_entries(lzip_t->zip_t,
                                         ZIP_FL_UNCHANGED);
}

void lzip_stat_index(unsigned long iIndex, struct lzip *lzip_t){
  zip_stat_index(lzip_t->zip_t, iIndex, 0, &lzip_t->zip_stat_t);
}

void *lzip_extract_index_to_memory(unsigned long iIndex,
                                   struct lzip *lzip_t,
                                   struct lzip_extracted *lzip_extracted_t){
  lzip_stat_index(iIndex, lzip_t);
  lzip_extracted_t->iExtractedFileSize = lzip_t->zip_stat_t.size;
  lzip_t->zip_file_t = zip_fopen_index(lzip_t->zip_t, iIndex, 0);
  lzip_extracted_t->pExtractedFile = malloc(lzip_t->zip_stat_t.size);
  memset(lzip_extracted_t->pExtractedFile, 0, lzip_t->zip_stat_t.size);
  zip_fread(lzip_t->zip_file_t,
            lzip_extracted_t->pExtractedFile,
            lzip_t->zip_stat_t.size - 1);
  return lzip_extracted_t->pExtractedFile;
}

#endif
