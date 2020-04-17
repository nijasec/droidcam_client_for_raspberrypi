#ifndef __FFJPEG_JFIF_H__
#define __FFJPEG_JFIF_H__

// °üº¬Í·ÎÄ¼þ
#include "bmp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* º¯ÊýÉùÃ÷ */
void* jfif_load(char *file);
void* jfif_buffer(unsigned char *buffer,long len);
int   jfif_save(void *ctxt, char *file);
void  jfif_free(void *ctxt);

int   jfif_decode(void *ctxt, BMP *pb);
void* jfif_encode(BMP *pb);

#ifdef __cplusplus
}
#endif

#endif

