#include "bootpack.h"
#include <stdio.h>


//参考文献
//http://softwaretechnique.jp/OS_Development/kernel_development12.html#recalibrate
//https://wiki.osdev.org/Floppy_Disk_Controller#Recalibrate
//http://www.osdever.net/documents/82077AA_FloppyControllerDatasheet.pdf?the_id=41
//http://oswiki.osask.jp/?cmd=read&page=%28FDC%29765A&word=FD


//IRQ6検知
extern int ReceivedIRQ = 0;
extern int motor = 0;
void inthandler26(int *esp){
    //IRQの受付を完了したことを伝える
    io_out8(PIC0_OCW2, 0x66);
    ReceivedIRQ = 1;
    return;
}

//フロッピーコントローラをリセットする
void flpc_reset(){
    //コントローラのリセットする
    io_out8(DIGITAL_OUTPUT_REGISTER, 0x00);
    //コントローラの有効化する
    io_out8(DIGITAL_OUTPUT_REGISTER, 0x04);
    //IRQ6発生まち
    flpyirq_wait();


    return;
}

//FDCのDATA FIFOレジスタの値を読み込む
unsigned char flpy_read_data(){
    unsigned char result;
    for(;;){
        if( (io_in8(MAIN_STATUS_REGISTER) & 0xc0) == 0x80){
            result = io_in8(DATA_FIFO);
            break;
        }
    }
    return result;
}

void MSR_check(){
    for(;;){
        //MSRの7bit目と6bit目をチェックする
        if( (io_in8(MAIN_STATUS_REGISTER)&0xc0) == 0x80){
            break;
        }

    }
}

//result phaseの段階をチェックする
void MSR_result_check(){
    for(;;){
        //MSRの7bit目と6bitをチェックする
        if( (io_in8(MAIN_STATUS_REGISTER) & 0x50) == 0x50){
            break;
        }
    }
}

//IRQ発生待ち
void flpyirq_wait(){

    //IRQ発生待ち
    while(ReceivedIRQ != 1){
        //何もしない
    }
    ReceivedIRQ = 0;

    return;

}

//commandを発行する  command or parameter
void issue(char command){

    //flpcの準備ができるのを待つ
    MSR_check();
    //コマンドを発行する
    io_out8(DATA_FIFO, command);

    return;

}



//motorをonにする
void motor_on(){
    //motorの0番をonにする。
    //motorが、onになっていなければ、onにする
    if(motor != 1){
        io_out8(DIGITAL_OUTPUT_REGISTER, 0x1c);
        motor = 1;
    }
    return;
}

//DMAを有効にする
void DMA_VALID(){

    //specify command
    MSR_check();
    io_out8(DATA_FIFO, 0x03);
    MSR_check();
    io_out8(DATA_FIFO, 0x00);
    MSR_check();
    io_out8(DATA_FIFO, 0x00);

    return;
}

void issue_command_read(char cylinder, char head, char sector){

    char parameter1 = (head & 0x00) << 2;

    //DMA初期化する
    dma_read_mode();

    DMA_VALID();

    //motorオンにしたあと、数秒待たないといけないが、エミュなので無視
    motor_on();

    //readコマンドを発行する
    issue(0x06);        //command
    issue(parameter1);        //head番号とドライブ番号
    issue(cylinder);    //シリンダー番号
    issue(head);        //ヘッド番号
    issue(sector);        //sector
    issue(0x02);        //sectorサイズ
    issue(0x12);        //トラック長
    issue(0x1b);        //ギャップ3サイズ何これ
    issue(0xff);        //データ長

    //IRQ待ち
    flpyirq_wait();

    //result_phaseは飛ばす。とりあえず、動いているの見たい。
    //result_phase
    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);


    return;
}

void issue_command_write(char cylinder, char head, char sector){
    char parameter1 = (head & 0x00) << 2;
    DMA_VALID();

    motor_on();

    //DMA初期化する
    dma_write_mode();

    //writeコマンドを発行する
    issue(0x05);        //command
    issue(parameter1);        //head番号とドライブ番号
    issue(cylinder);    //シリンダー番号
    issue(head);        //ヘッド番号
    issue(sector);        //sector
    issue(0x02);        //sectorサイズ
    issue(0x12);        //トラック長
    issue(0x1b);        //ギャップ3サイズ何これ
    issue(0xff);        //データ長

    //IRQ待ち
    flpyirq_wait();

    //result_phase  結果は読まない。とりあえず動かしたい
    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    MSR_result_check();
    io_in8(DATA_FIFO);

    return;
}
