#include "fb_draw.h"

struct fb_st {
     struct fb_fix_screeninfo fix;
     struct fb_var_screeninfo var;
     unsigned long bpp;
     int fd;
     char *fbp;
};

static struct fb_st fb0;

int fb_open(void)
{
     int ret;

     fb0.fd = open("/dev/fb0", O_RDWR);
     if (-1 == fb0.fd) {
	  perror("open");
	  goto error;
     }

     /* get fb_var_screeninfo */
     ret = ioctl(fb0.fd, FBIOGET_VSCREENINFO, &fb0.var);
     if (-1 == ret) {
	  perror("ioctl(fb0.var)");
	  goto close_fd;
     }

     fb0.bpp = fb0.var.bits_per_pixel / 8;

     /* get fb_fix_screeninfo */
     ioctl(fb0.fd, FBIOGET_FSCREENINFO, &fb0.fix);
     if (-1 == ret) {
	  perror("ioctl(fb0.fix)");
	  goto close_fd;
     }

     /* get framebuffer start address */
     fb0.fbp = mmap(NULL, fb0.fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb0.fd, 0);
     if ((void *)-1 == fb0.fbp) {
	  perror("mmap");
	  goto close_fd;
     }

     return 0;

close_fd:
     close(fb0.fd);
error:
     return -1;
}

void fb_close()
{
     munmap(fb0.fbp, fb0.fix.smem_len);
     close(fb0.fd);
}

void fb_draw_point(int x, int y, int color)
{
     unsigned long offet;

 	//fb0.var.xres 
     //offet = fb0.bpp * (x + y * 1280);
    offet = fb0.bpp * (x + y * fb0.var.xres);//debug!!
     //offet = fb0.bpp * (y + x * fb0.var.xres);
     memset(fb0.fbp + offet, color, fb0.bpp);
}

int xres(void)
{
     return fb0.var.xres;//x最大
}

int yres(void)
{
     return fb0.var.yres;//y最大
}
