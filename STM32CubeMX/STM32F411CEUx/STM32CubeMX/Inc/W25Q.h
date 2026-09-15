#ifndef W25Q_H
#define W25Q_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define W25Q_GET_JEDEC_ID                   0x9f

#define W25Q_READ_STATUS_REGISTER_1         0x05
#define W25Q_READ_STATUS_REGISTER_2         0x35
#define W25Q_READ_STATUS_REGISTER_3         0x15

#define W25Q_WRITE_DISABLE                  0x04
#define W25Q_WRITE_ENABLE                   0x06

#define W25Q_PAGE_PROGRAM                   0x02

#define W25Q_READ_DATA                      0x03
#define W25Q_FAST_READ                      0x0B

#define W25Q_SECTOR_ERASE                   0x20
#define W25Q_32K_BLOCK_ERASE                0x52
#define W25Q_64K_BLOCK_ERASE                0xD8

#define W25Q_CHIP_ERASE                     0xC7

typedef struct{
    void    (*W25Q_Select)(bool);
    void    (*W25Q_SPI_Write)(uint8_t);
    uint8_t (*W25Q_SPI_Read)(void);
}W25Q_t;

void W25Q_WriteEnable(W25Q_t*);
void W25Q_WriteDisable(W25Q_t*);

void W25Q_ReadData(W25Q_t*, uint8_t* buf, uint32_t address, uint32_t size);
void W25Q_FastRead(W25Q_t*, uint8_t* buf, uint32_t address, uint32_t size);

void W25Q_PageProgram(W25Q_t*, uint8_t* buf, uint32_t address, uint32_t size);
void W25Q_MultiPageProgram(W25Q_t* self, uint8_t* buf, uint32_t address, uint32_t size);

void W25Q_SectorErase(W25Q_t*, uint32_t address);
void W25Q_BlockErase(W25Q_t*, uint32_t address);
void W25Q_ChipErase(W25Q_t*);

uint8_t W25Q_ReadStatusRegister(W25Q_t*, uint8_t reg);
bool W25Q_IsBusy(W25Q_t*);

uint32_t W25Q_ReadID(W25Q_t*);

/* // Прошлый вариант для .cpp
class W25Q{
private:
    void    (*W25Q_Select)(bool);
    void    (*W25Q_SPI_Write)(const uint8_t&);
    uint8_t (*W25Q_SPI_Read)(void);

public:
    W25Q(
        void    (*select_func)(bool), 
        void    (*write_func)(const uint8_t&), 
        uint8_t (*read_func)(void)
    ){
        W25Q_Select     = select_func;
        W25Q_SPI_Write  = write_func;
        W25Q_SPI_Read   = read_func;
    }

    void WriteEnable(void){
        W25Q_Select(true);
        uint8_t cmd = W25Q_WRITE_ENABLE;
        W25Q_SPI_Write(cmd);
        W25Q_Select(false);
    }

    void WriteDisable(void){
        W25Q_Select(true);
        uint8_t cmd = W25Q_WRITE_DISABLE;
        W25Q_SPI_Write(cmd);
        W25Q_Select(false);
    }

    void ReadData(uint8_t* buf, uint32_t address, uint32_t size){
        W25Q_Select(true);

        uint8_t cmd = W25Q_READ_DATA;

        W25Q_SPI_Write(cmd);
        W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
        W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
        W25Q_SPI_Write((uint8_t)(address & 0xFF));
        for(uint32_t i = 0; i < size; i++){
            buf[i] = W25Q_SPI_Read();
        }

        W25Q_Select(false);
    }

    void FastRead(uint8_t* buf, uint32_t address, uint32_t size){
        W25Q_Select(true);
        uint8_t cmd = W25Q_FAST_READ;
        W25Q_SPI_Write(cmd);
        W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
        W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
        W25Q_SPI_Write((uint8_t)(address & 0xFF));
        uint8_t dummy_byte = 0x00;
        W25Q_SPI_Write(dummy_byte);
        for(uint32_t i = 0; i < size; i++){
            buf[i] = W25Q_SPI_Read();
        }
        W25Q_Select(false);
    }

    void PageProgram(uint8_t* buf, uint32_t address, uint32_t size){
        if(size >= 256){
            address &= 0xFFFFFF00;
            size = 256;
        }

        WriteEnable();

        W25Q_Select(true);
        uint8_t cmd = W25Q_PAGE_PROGRAM;
        W25Q_SPI_Write(cmd);
        W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
        W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
        W25Q_SPI_Write((uint8_t)(address & 0xFF));
        for(uint32_t i = 0; i < size; i++){
            W25Q_SPI_Write(buf[i]);
        }
        W25Q_Select(false);
    }

    void SectorErase(uint32_t address){
        address &= 0xFFFFF000;  

        WriteEnable();

        W25Q_Select(true);

        uint8_t cmd = W25Q_SECTOR_ERASE;
        W25Q_SPI_Write(cmd);
        W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
        W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
        W25Q_SPI_Write((uint8_t)(address & 0xFF));

        W25Q_Select(false);
    }

    void BlockErase(uint32_t address){
        address &= 0xFFFF0000;  

        WriteEnable();

        W25Q_Select(true);

        uint8_t cmd = W25Q_64K_BLOCK_ERASE;
        W25Q_SPI_Write(cmd);
        W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
        W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
        W25Q_SPI_Write((uint8_t)(address & 0xFF));

        W25Q_Select(false);
    }

    void ChipErase(void){
        WriteEnable();

        W25Q_Select(true);

        uint8_t cmd = W25Q_CHIP_ERASE;
        W25Q_SPI_Write(cmd);

        W25Q_Select(false);
    }

    uint8_t ReadStatusRegister(uint8_t reg){
        W25Q_Select(true);
        
        uint8_t cmd = 0; 

        switch(reg){
            case 1: 
                cmd = W25Q_READ_STATUS_REGISTER_1;
                break;
            case 2:
                cmd = W25Q_READ_STATUS_REGISTER_2;
                break;
            case 3:
                cmd = W25Q_READ_STATUS_REGISTER_3;
                break;
            default:
                cmd = W25Q_READ_STATUS_REGISTER_1;
                break;
        }

        W25Q_SPI_Write(cmd);
        reg = W25Q_SPI_Read();
        
        W25Q_Select(false);

        return reg;
    }

    bool IsBusy(void){
        if(ReadStatusRegister(1)){
            return true;
        }
        return false;
    }

    uint32_t ReadID(void){
        uint32_t id;

        W25Q_Select(true);
        
        uint8_t cmd = W25Q_GET_JEDEC_ID;
        W25Q_SPI_Write(cmd);

        // Считываем 3 байта по очереди и сдвигаем их на свои места:
        uint8_t manufacturer_id = W25Q_SPI_Read(); 
        uint8_t memory_type_id  = W25Q_SPI_Read(); 
        uint8_t capacity_id     = W25Q_SPI_Read(); 

        // Собираем их в одно
        id = (uint32_t)(manufacturer_id << 16) | 
             (uint32_t)(memory_type_id  << 8)  | 
             (uint32_t)(capacity_id);

        W25Q_Select(false);

        return id;
    }
};
*/

#endif /* W25Q_H */