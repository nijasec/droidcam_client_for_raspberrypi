#include "test.h"

int format_properties(const unsigned int format, const unsigned int width, const unsigned int height,
			size_t *linewidth,
			size_t *framewidth)
{
	size_t lw, fw;
	switch (format)
	{
		case V4L2_PIX_FMT_YUV420:
		case V4L2_PIX_FMT_YVU420:
			lw = width; /*??? */
			fw = ROUND_UP_4(width) *ROUND_UP_2(height);
			fw += 2 *((ROUND_UP_8(width) / 2) *(ROUND_UP_2(height) / 2));
			break;
		case V4L2_PIX_FMT_UYVY:
		case V4L2_PIX_FMT_Y41P:
		case V4L2_PIX_FMT_YUYV:
		case V4L2_PIX_FMT_YVYU:
			lw = (ROUND_UP_2(width) *2);
			fw = lw * height;
			break;
		default:
			return 0;
	}

	if (linewidth) *linewidth = lw;
	if (framewidth) *framewidth = fw;

	return 1;
}

void print_format(struct v4l2_format *vid_format)
{
	printf("	vid_format->type                =%d\n", vid_format->type);
	printf("	vid_format->fmt.pix.width       =%d\n", vid_format->fmt.pix.width);
	printf("	vid_format->fmt.pix.height      =%d\n", vid_format->fmt.pix.height);
	printf("	vid_format->fmt.pix.pixelformat =%d\n", vid_format->fmt.pix.pixelformat);
	printf("	vid_format->fmt.pix.sizeimage   =%d\n", vid_format->fmt.pix.sizeimage);
	printf("	vid_format->fmt.pix.field       =%d\n", vid_format->fmt.pix.field);
	printf("	vid_format->fmt.pix.bytesperline=%d\n", vid_format->fmt.pix.bytesperline);
	printf("	vid_format->fmt.pix.colorspace  =%d\n", vid_format->fmt.pix.colorspace);
}

int setupDevice(ARGS *args)
{
	int ret_code = 0;
	struct v4l2_capability vid_caps;
	struct v4l2_format vid_format;
	size_t framesize = 0;
	size_t linewidth = 0;

	args->fdwr = open(args->video_device, O_RDWR);
	assert(args->fdwr >= 0);

	ret_code = ioctl(args->fdwr, VIDIOC_QUERYCAP, &vid_caps);
	assert(ret_code != -1);

	memset(&vid_format, 0, sizeof(vid_format));

	ret_code = ioctl(args->fdwr, VIDIOC_G_FMT, &vid_format);
	if (debug) print_format(&vid_format);

	vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vid_format.fmt.pix.width = FRAME_WIDTH;
	vid_format.fmt.pix.height = FRAME_HEIGHT;
	vid_format.fmt.pix.pixelformat = FRAME_FORMAT;
	vid_format.fmt.pix.sizeimage = framesize;
	vid_format.fmt.pix.field = V4L2_FIELD_NONE;
	vid_format.fmt.pix.bytesperline = linewidth;
	vid_format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

	if (debug) print_format(&vid_format);
	ret_code = ioctl(args->fdwr, VIDIOC_S_FMT, &vid_format);

	assert(ret_code != -1);

	if (debug) printf("frame: format=%d\tsize=%lu\n", FRAME_FORMAT, framesize);
	print_format(&vid_format);

	if (!format_properties(vid_format.fmt.pix.pixelformat,
			vid_format.fmt.pix.width, vid_format.fmt.pix.height, &linewidth, &framesize))
	{
		printf("unable to guess correct settings for format '%d'\n", FRAME_FORMAT);
		return -1;
	}
	args->framesize = framesize * 3;

	return 0;
}
void cleanall(ARGS *args)
{
	if (args != NULL)
	{
		if (args->ctx != NULL)
		{
			free(args->ctx);
		}
		close(args->fdwr);
		free(args);
	}
}
void *threadfunc2(void *p)
{
	ARGS *args = (ARGS*) p;
	printf("i am in thread 2");
	//startaudio(args);
	return NULL;
	
}
void *threadfunc(void *param)
{
	ARGS *args = (ARGS*) param;
	printf("i am in thread");
	startcapture(args);
	return NULL;
}
void Bitmap2Yuv420p(char *destination, char *rgb,
	int width, int height)
{
	const size_t image_size = width * height;
	char *dst_y = destination;
	char *dst_u = destination + image_size;
	char *dst_v = destination + image_size + image_size / 4;

	// Y plane
	for (size_t i = 0; i < image_size; ++i)
	{
		*dst_y++ = ((66 *rgb[3 *i] + 129 *rgb[3 *i + 1] + 25 *rgb[3 *i + 2]) >> 8) + 16;
	}

	// U plane
	for (size_t y = 0; y < height; y += 2)
	{
		for (size_t x = 0; x < width; x += 2)
		{
			const size_t i = y *width + x;
			*dst_u++ = ((-38 *rgb[3 *i] + -74 *rgb[3 *i + 1] + 112 *rgb[3 *i + 2]) >> 8) + 128;
		}
	}
	// V plane
	for (size_t y = 0; y < height; y += 2)
	{
		for (size_t x = 0; x < width; x += 2)
		{
			const size_t i = y *width + x;
			*dst_v++ = ((112 *rgb[3 *i] + -94 *rgb[3 *i + 1] + -18 *rgb[3 *i + 2]) >> 8) + 128;
		}
	}
}
int startaudio(ARGS *args)
{
	int sockfd = 0, n = 0;
	char *recvBuff;
	struct sockaddr_in serv_addr;

	recvBuff = malloc(args->framesize);

	// memset(recvBuff, '0',framesize);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Error : Could not create socket \n");
		return 1;
	}
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(args->port);

	if (inet_pton(AF_INET, args->server, &serv_addr.sin_addr) <= 0)
	{
		printf("\n inet_pton error occured\n");
		return 1;
	}

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\n Error : Connect Failed \n");
		return 1;
	}
	char *http = "CMD /v2/audio";
	//char *cmd ="CMD /v2/video.3?640x480";

	send(sockfd, http, strlen(http), 0);

	
FILE *fp=fopen("a.amr","wb");
	do {

		n=read(sockfd,recvBuff,args->framesize);
		fwrite(recvBuff,n,1,fp);
	/*	for(int i=0;i<n;i++)
		{
			printf("%2x ",recvBuff[i]);
		}*/
		//close(sockfd);
	}while(n>0 && !args->ctx->stopping);
	fclose(fp);
	return 0;
	
}
int startcapture(ARGS *args)
{
	int sockfd = 0, n = 0;
	char *recvBuff;

	struct sockaddr_in serv_addr;

	recvBuff = malloc(args->framesize);

	// memset(recvBuff, '0',framesize);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Error : Could not create socket \n");
		return 1;
	}
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(args->port);

	if (inet_pton(AF_INET, args->server, &serv_addr.sin_addr) <= 0)
	{
		printf("\n inet_pton error occured\n");
		return 1;
	}

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\n Error : Connect Failed \n");
		return 1;
	}
	char *http = "GET /video \r\n\r\n";
	//char *cmd ="CMD /v2/video.3?640x480";

	send(sockfd, http, strlen(http), 0);

	int i, start = 0, valofi = 0, end = 0;
	int offset = -1;
	unsigned char *buffer;

	char *remaining;
	int rlen = 0;

	int width = 640;
	int height = 480;
	buffer = malloc(args->framesize);

	remaining = malloc(args->framesize);

	size_t byte_count = (640 * 480 *3) / 2;

	char *destination = malloc(byte_count);

	do {

		offset = -1;
		start = 0, valofi = 0, end = 0;
		if (rlen != 0)
		{
			memcpy(recvBuff, remaining, rlen);
		}
		n = read(sockfd, recvBuff + rlen, args->framesize);
		rlen = 0;

		for (i = 0; i < n; i++)
		{
			if (recvBuff[i] == 0xFF && recvBuff[i + 1] == 0xD8)
			{

				start = 1;
				valofi = i;

				break;
			}
		}
		if (start == 1)	//image start
		{
			offset = n - valofi;

			memcpy(buffer, recvBuff + valofi, offset);

			do {

				n = read(sockfd, recvBuff, args->framesize / 2);

				for (i = 0; i < n; i++)
				{
					if (recvBuff[i] == 0xFF && recvBuff[i + 1] == 0xD9)
					{
						end = 1;
						if (i < n)
						{
							rlen = n - i;
							memcpy(remaining, recvBuff + i, rlen);
						}
						break;
					}
				}

				memcpy(buffer + offset, recvBuff, i);

				offset += i;

				if (end == 1)
				{

					void *jfif = NULL;
					BMP bmp = { 0 };

					jfif = jfif_buffer(buffer, offset);
					jfif_decode(jfif, &bmp);

					jfif_free(jfif);

					char *rgb = bmp.pdata;

					Bitmap2Yuv420p(destination, rgb, width, height);
					write(args->fdwr, destination, byte_count);
					bmp_free(&bmp);

					//findex++;
					//sleep(1);
					break;
				}
			} while (n > 0 && !args->ctx->stopping);
		}
	} while (n > 0 && !args->ctx->stopping);

	return 0;
}
int main(int argc, char **argv)
{
	ARGS *args = (ARGS*) malloc(sizeof(ARGS));
	CTX *ctx = (CTX*) malloc(sizeof(CTX));
	memset(ctx, 0, sizeof(CTX));
	pthread_t mythread,thread2;

	char url[500];
	char ip[100];
	int port = 80;
	char page[100];
	char device[100];
	printf("Enter output device:");
	strcpy(device, "/dev/video0");
	//scanf("%s",device);
	args->video_device = device;
	printf("Enter mjpeg video url:");
	strcpy(url, "http://192.168.1.105:4747/video");
	//scanf("%s",url);
	sscanf(url, "http://%99[^:]:%99d/%99[^\n]", ip, &port, page);
	printf("ip = \"%s\"\n", ip);
	printf("port = \"%d\"\n", port);
	printf("path = \"%s\"\n", page);
	args->ctx = ctx;
	args->server = ip;
	args->port = port;
	args->path = page;
	if (setupDevice(args) < 0)
	{
		cleanall(args);
		return 0;
	}

	//size=startcapture(fdwr,framesize);
	pthread_create(&mythread, NULL, threadfunc, args);
	pthread_create(&thread2, NULL, threadfunc2, args);

	//pause();
	char q;
	while (!args->ctx->stopping)
	{
		printf("enter q to exit:");
		scanf("%c", &q);
		if (q == 'q')
			break;
	}

	args->ctx->stopping = 1;
	pthread_join(mythread, NULL);
	
	cleanall(args);

	return 0;
}