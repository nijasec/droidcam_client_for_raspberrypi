#ifndef __FFJPEG_BMP_H__
#define __FFJPEG_BMP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ÀàÐÍ¶¨Òå */
/* BMP ¶ÔÏóµÄÀàÐÍ¶¨Òå */
typedef struct
{
    int   width;   /* ¿í¶È */
    int   height;  /* ¸ß¶È */
    int   stride;  /* ÐÐ×Ö½ÚÊý */
    void *pdata;   /* Ö¸ÏòÊý¾Ý */
} BMP;
typedef struct{
	int size;
	int   width;   /* ¿í¶È */
    int   height;
	unsigned char *data;
}IMAGEINF;
/* º¯ÊýÉùÃ÷ */
int  bmp_load  (BMP *pb, char *file);

IMAGEINF * bmp_tomemory(BMP *pb);
int  bmp_create(BMP *pb, int w, int h);

int  bmp_save(BMP *pb, char *file);
void bmp_free(BMP *pb);

#ifdef __cplusplus
}
#endif

#endif

