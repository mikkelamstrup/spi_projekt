#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static const char *device = "/dev/spidev0.0";  // SPI enhed
static uint8_t mode = 0;       // SPI mode (Mode 0 som standard)
static uint8_t bits = 8;       // Bits per ord
static uint32_t speed = 500000; // SPI hastighed i Hz
static uint16_t delay = 0;     // Forsinkelse i mikrosekunder

// Fejlhåndteringsfunktion
static void pabort(const char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    int fd;
    int ret;

    // Data til overførsel
    uint8_t tx[] = { 0xDE, 0xAD, 0xBE, 0xEF };  // Data der skal sendes
    uint8_t rx[ARRAY_SIZE(tx)] = {0};           // Buffer til modtaget data

    // Åbn SPI-enheden
    fd = open(device, O_RDWR);
    if (fd < 0)
        pabort("can't open device");

    // Indstil SPI mode
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        pabort("can't set SPI mode");

    // Læs SPI mode (ikke nødvendigt for blot at sende data)
    // ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    // if (ret == -1)
    //     pabort("can't get SPI mode");

    // Indstil bits per ord
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word");

    // Læs bits per ord (ikke nødvendigt for blot at sende data)
    // ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    // if (ret == -1)
    //     pabort("can't get bits per word");

    // Indstil maksimal hastighed
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed");

    // Læs maksimal hastighed (ikke nødvendigt for blot at sende data)
    // ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    // if (ret == -1)
    //     pabort("can't get max speed");

    // Print konfigurationsdetaljer (valgfrit)
    // printf("SPI mode: %d\n", mode);
    // printf("Bits per word: %d\n", bits);
    // printf("Max speed: %d Hz\n", speed);

    // SPI overførsel
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = ARRAY_SIZE(tx),
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        pabort("can't send SPI message");

    // Print modtaget data
    for (int i = 0; i < ARRAY_SIZE(rx); i++) {
        printf("Received: 0x%.2X\n", rx[i]);
    }

    // Luk SPI-enheden
    close(fd);
    return 0;
}
