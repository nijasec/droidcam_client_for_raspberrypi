#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include "bmp.h"
#include <fcntl.h>
#include <assert.h>
#include "jfif.h"
#include <pthread.h>
#include <arpa/inet.h>

static int debug=0;
#define ROUND_UP_2(num)  (((num)+1)&~1)
#define ROUND_UP_4(num)  (((num)+3)&~3)
#define ROUND_UP_8(num)  (((num)+7)&~7)
#define ROUND_UP_16(num) (((num)+15)&~15)
#define ROUND_UP_32(num) (((num)+31)&~31)
#define ROUND_UP_64(num) (((num)+63)&~63)

#if 1
# define FRAME_WIDTH  640
# define FRAME_HEIGHT 480
#else
# define FRAME_WIDTH  512
# define FRAME_HEIGHT 512
#endif

#if 0
# define FRAME_FORMAT V4L2_PIX_FMT_YUYV
#else
# define FRAME_FORMAT V4L2_PIX_FMT_YVU420
#endif



typedef struct context{

int stopping;


}CTX;

typedef struct _ARGS {
int framesize;
char *path;
int port,fdwr;
CTX *ctx;
char *server;
char  *video_device;
struct sockaddr_in serv_addr;
}ARGS;
int startcapture(ARGS *args);

