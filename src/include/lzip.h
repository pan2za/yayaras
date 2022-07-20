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

extern void lzip_init(struct lzip *lzip_t);

extern unsigned long lzip_get_ientries(struct lzip *lzip_t);
extern void lzip_cleanup_zip_file_t(struct lzip *lzip_t);

extern void lzip_cleanup_extractedfile(struct lzip_extracted *lzip_extracted_t);
extern void lzip_pzipfile_cleanup(struct lzip *lzip_t);
extern void lzip_cleanup_zip_t(struct lzip *lzip_t);
extern void lzip_cleanup(struct lzip *lzip_t);

extern void _lzip_set_izipsize(unsigned long iZipSize,
                        struct lzip *lzip_t);
extern void _lzip_set_ientries(unsigned long iEntries,
                        struct lzip *lzip_t);
extern void _lzip_set_pzipfile(void *pZipFile,
                        struct lzip *lzip_t);
extern void lzip_open_from_memory(void *pZipFile,
                           unsigned long iZipSize,
                           struct lzip *lzip_t);
extern void lzip_stat_index(unsigned long iIndex, struct lzip *lzip_t);

extern void *lzip_extract_index_to_memory(unsigned long iIndex,
                                   struct lzip *lzip_t,
                                   struct lzip_extracted *lzip_extracted_t);
#endif
