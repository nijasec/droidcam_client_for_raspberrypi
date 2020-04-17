// °üº¬Í·ÎÄ¼þ
#include <stdlib.h>
#include <stdio.h>
#include "stdefine.h"
#include "bmp.h"
#include <string.h>
#define TEST_BMP  0

// ÄÚ²¿ÀàÐÍ¶¨Òå
#pragma pack(1)
typedef struct { 
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2;
    DWORD  bfOffBits;
    DWORD  biSize;
    DWORD  biWidth;
    DWORD  biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    DWORD  biXPelsPerMeter;
    DWORD  biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} BMPFILEHEADER;
#pragma pack()

/* ÄÚ²¿º¯ÊýÊµÏÖ */
static int ALIGN(int x, int y) {
    // y must be a power of 2.
    return (x + y - 1) & ~(y - 1);
}

/* º¯ÊýÊµÏÖ */
int bmp_load(BMP *pb, char *file)
{
    BMPFILEHEADER header = {0};
    FILE         *fp     = NULL;
    BYTE         *pdata  = NULL;
    int           i;

    fp = fopen(file, "rb");
    if (!fp) return -1;

    fread(&header, sizeof(header), 1, fp);
    pb->width  = header.biWidth;
    pb->height = header.biHeight;
    pb->stride = ALIGN(header.biWidth * 3, 4);
    pb->pdata  = malloc(pb->stride * pb->height);
    if (pb->pdata) {
        pdata  = (BYTE*)pb->pdata + pb->stride * pb->height;
        for (i=0; i<pb->height; i++) {
            pdata -= pb->stride;
            fread(pdata, pb->stride, 1, fp);
        }
    }

    fclose(fp);
    return pb->pdata ? 0 : -1;
}

int bmp_create(BMP *pb, int w, int h)
{
    pb->width  = w;
    pb->height = h;
    pb->stride = ALIGN(w * 3, 4);
    pb->pdata  = malloc(pb->stride * h);
    return pb->pdata ? 0 : -1;
}

IMAGEINF * bmp_tomemory(BMP *pb)
{
	BMPFILEHEADER header = {0};
	IMAGEINF *imginf;
  //  BYTE        *memory     = NULL;
    BYTE         *pdata;
    int           i;

    header.bfType     = ('B' << 0) | ('M' << 8);
    header.bfSize     = sizeof(header) + pb->stride * pb->height;
    header.bfOffBits  = sizeof(header);
    header.biSize     = 40;
    header.biWidth    = pb->width;
    header.biHeight   = pb->height;
    header.biPlanes   = 1;
    header.biBitCount = 24;
    header.biSizeImage= pb->stride * pb->height;
	imginf=malloc(sizeof(IMAGEINF));
	imginf->size=header.bfSize;
	//printf("bfSize:%d",header.bfSize);
	imginf->data=malloc(header.bfSize);
	imginf->width=pb->width;
	imginf->height=pb->height;
	memset(imginf->data, '0',header.bfSize);
	// printf("pb->stride:%d",pb->stride);
    //FILE *fp = fopen("t.bmp", "w+");
   memcpy(imginf->data,&header,sizeof(header));
//	printf("\nsizeofheader:%d",sizeof(header));
    //    fwrite(&header, sizeof(header), 1, fp);
	char *d=imginf->data;
	imginf->data=imginf->data+sizeof(header);
//printf("h:%d",pb->height);	
        pdata = (BYTE*)pb->pdata + pb->stride * pb->height;
        for (i=0; i<pb->height; i++) {
            pdata -= pb->stride;
			memcpy(imginf->data,pdata,pb->stride);
			imginf->data+=pb->stride;
            //fwrite(pdata, pb->stride, 1, fp);
        }
      // fwrite(d,imginf->size,1,fp);
	  // fclose(fp);
    imginf->data=d;

    return imginf;
	
}
int bmp_save(BMP *pb, char *file)
{
    BMPFILEHEADER header = {0};
    FILE         *fp     = NULL;
    BYTE         *pdata;
    int           i;

    header.bfType     = ('B' << 0) | ('M' << 8);
    header.bfSize     = sizeof(header) + pb->stride * pb->height;
    header.bfOffBits  = sizeof(header);
    header.biSize     = 40;
    header.biWidth    = pb->width;
    header.biHeight   = pb->height;
    header.biPlanes   = 1;
    header.biBitCount = 24;
    header.biSizeImage= pb->stride * pb->height;
//printf("h:%d",pb->height);
    fp = fopen(file, "wb");
    if (fp) {
        fwrite(&header, sizeof(header), 1, fp);
        pdata = (BYTE*)pb->pdata + pb->stride * pb->height;
        for (i=0; i<pb->height; i++) {
            pdata -= pb->stride;
            fwrite(pdata, pb->stride, 1, fp);
        }
        fclose(fp);
    }

    return fp ? 0 : -1;
}

void bmp_free(BMP *pb)
{
    if (pb->pdata) {
        free(pb->pdata);
        pb->pdata = NULL;
    }
    pb->width  = 0;
    pb->height = 0;
    pb->stride = 0;
}


#if TEST_BMP
int main(void)
{
    BMP bmp = {0};
    bmp_load(&bmp, "test.bmp");
    bmp_save(&bmp, "save.bmp");
    bmp_free(&bmp);
    return 0;
}
#endif










