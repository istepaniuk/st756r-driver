#ifndef _SPI_H_
#define _SPI_H_

#define TRIS_SCK TRISC3
#define TRIS_SDI TRISC4
#define TRIS_SDO TRISC5


#define spi_data_flag() BF

extern void spi_init(void);
extern void spi_write(unsigned char txdata);

#endif
