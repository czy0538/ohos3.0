/****************************************************************************

MIT License

Copyright (c) 2021 唐佐林
WeChat : delphi_tang
EMail: delphi_tang@dt4sw.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*****************************************************************************/

#include "hi_io.h"
#include "hi_adc.h"
#include "hi_i2c.h"
#include "hi_spi.h"

unsigned int DTGpioSetFunc(unsigned int id, unsigned char val)
{
    return hi_io_set_func(id, val);
}

unsigned int DTGpioSetPull(unsigned int id, unsigned char val)
{
    return hi_io_set_pull(id, val);
}

typedef struct 
{
    unsigned int cpol;
    unsigned int cpha;
    unsigned int protocol;
    unsigned int dataWidth;
    unsigned int endian;
    unsigned int freq;
} SpiInfo;

unsigned int DTSpiInit(unsigned int id, SpiInfo info)
{
    hi_spi_cfg_basic_info hiInfo = {0};
    hi_spi_cfg_init_param param = {0};

    hiInfo.cpol = info.cpol;
    hiInfo.cpha = info.cpha;
    hiInfo.fram_mode = info.protocol;
    hiInfo.data_width = info.dataWidth;
    hiInfo.endian = info.endian;
    hiInfo.freq = info.freq;

    return hi_spi_init(id, param, &hiInfo);
}

unsigned int DTSpiDeinit(unsigned int id)
{
    return hi_spi_deinit(id);
}

unsigned int DTSpiHostWrite(unsigned int id, unsigned char *writeData, unsigned int byteLen)
{
    return hi_spi_host_write(id, writeData, byteLen);
}
unsigned int DTSpiHostRead(unsigned int id, unsigned char *readData, unsigned int byteLen)
{
    return hi_spi_host_read(id, readData, byteLen);
}

unsigned int DTSpiHostWriteRead(unsigned int id, unsigned char* writeData, unsigned char* readData, unsigned int byteLen)
{
    return hi_spi_host_writeread(id, writeData, readData, byteLen);
}

unsigned int DTSpiSetInfo(unsigned int id, SpiInfo info)
{
    hi_spi_cfg_basic_info hiInfo = {0};

    hiInfo.cpol = info.cpol;
    hiInfo.cpha = info.cpha;
    hiInfo.fram_mode = info.protocol;
    hiInfo.data_width = info.dataWidth;
    hiInfo.endian = info.endian;
    hiInfo.freq = info.freq;

    return hi_spi_set_basic_info(id, &hiInfo);
}

unsigned int DTI2cWriteRead(unsigned int id, unsigned short deviceAddr, unsigned char* txBuf, unsigned int txLen, unsigned char* rxBuf, unsigned int rxLen)
{
    hi_i2c_data data = {0};

    data.send_buf = txBuf;
    data.send_len = txLen;
    data.receive_buf = rxBuf;
    data.receive_len = rxLen;

    return hi_i2c_writeread(id, deviceAddr, &data);
}

unsigned int DTAdcRead(unsigned int channel, unsigned short* data, int equModel, int curBais, unsigned short rstCnt)
{
    return hi_adc_read(channel, data, equModel, curBais, rstCnt);
}
