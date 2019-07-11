
#include "bootpack.h"
#include <stdio.h>



//DMACを初期化する
void init_dma_flp(short addr, short size){
    ///_/_/_/_/_/_フロッピーのためのDMA初期化
    //DMA初期化
    //maskする
    io_out8(DEF_PORT_DMAC0_SINGLE_MASK, 0x06);
    //DMAのリセットする
    io_out8(DEF_PORT_DMAC0_CLEAR_BP, 0xFF);
    //転送先のアドレスを指定する
    //io_out8(channel_dma_flp_base_adr, addr && 0x00FF);
    io_out8(0x04, addr & 0x00FF);
    //io_out8(channel_dma_flp_base_adr, addr >> 8 && 0xFF00);
    io_out8(0x04, addr & 0xFF00 >> 8);
    io_out8(0xd8, 0xff);
    //転送サイズを指定する
    //bufferを用意する
    io_out8(0x05, size & 0x00FF);
    io_out8(0x05, size & 0xFF00 >> 8);

    //この一行書いたら、なぜか動いた。これ書かないと読み書きできない。これなんなの？
    io_out8(0x81, 0);

    //mask解除する
    io_out8(DEF_PORT_DMAC0_SINGLE_MASK, 0x02);

    return;
}

void dma_read_mode(){
    //readモードにする
    io_out8(DEF_PORT_DMAC0_SINGLE_MASK, 0x06);
    io_out8(DEF_PORT_DMAC0_MODE, 0x56);
    io_out8(DEF_PORT_DMAC0_SINGLE_MASK, 0x02);

}

void dma_write_mode(){
    //writeモードにする
    io_out8(DEF_PORT_DMAC0_SINGLE_MASK, 0x06);
    io_out8(DEF_PORT_DMAC0_MODE, 0x5a);
    io_out8(DEF_PORT_DMAC0_SINGLE_MASK, 0x02);

}
