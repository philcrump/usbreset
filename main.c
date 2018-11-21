#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <linux/usbdevice_fs.h>

int main(int argc, char **argv)
{
    FILE *fp;
    int fd, r;
    char *ctx_strtok, *ptr_strtok;
    char *usb_bus, *usb_dev;
    char buf[64];
    char filename[64];

    if(argc != 3)
    {
        fprintf(stderr, "Usage: usbreset <vendor-id> <product-id>\n");
        return 1;
    }

    /* Construct with USB Vendor ID : Product ID */
    sprintf(buf, "lsusb -d %s:%s", argv[1], argv[2]);

    fp = popen(buf, "r");
    if(fp == NULL)
    {
        printf("Error: Failed to list USB devices.\n" );
        return 1;
    }

    // Example Output
    // Bus 001 Device 006: ID 2500:0021

    /* Read first (and only) line from output */
    if(fgets(buf, 63, fp) == NULL)
    {
        printf("Error: USB Device not found.\n" );
        pclose(fp);
        return 1;
    }
    pclose(fp);

    /* Extract USB Bus Number */
    ptr_strtok = strtok_r(buf, " ", &ctx_strtok);
    ptr_strtok = strtok_r(NULL, " ", &ctx_strtok);
    if(ptr_strtok == NULL)
    {
        printf("Error: Failed parse list of USB Buses.\n" );
        return 1;
    }
    usb_bus = strdup(ptr_strtok);

    /* Extract USB Device Number */
    ptr_strtok = strtok_r(NULL, " ", &ctx_strtok);
    ptr_strtok = strtok_r(NULL, " ", &ctx_strtok);
    if(ptr_strtok == NULL)
    {
        printf("Error: Failed parse list of USB Devices.\n" );
        return 1;
    }
    usb_dev = strdup(ptr_strtok);
    /* Strip off tailing colon */
    usb_dev[3] = '\0';

    /* Example filename: /dev/bus/usb/001/005 */
    snprintf(filename, 63, "/dev/bus/usb/%s/%s", usb_bus, usb_dev);
    free(usb_bus);
    free(usb_dev);

    fd = open(filename, O_WRONLY);
    if(fd < 0)
    {
        printf("Error: Failed to open USB Device Control file.\n" );
        return 1;
    }

    /* Reset the USB endpoint */
    r = ioctl(fd, USBDEVFS_RESET, 0);
    if(r < 0)
    {
        printf("Error: Failed to reset USB Device.\n" );
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}