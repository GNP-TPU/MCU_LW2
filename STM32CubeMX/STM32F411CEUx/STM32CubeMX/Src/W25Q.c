#include "W25Q.h"

void W25Q_Init(W25Q_t* self, void (*select_func)(bool), void (*write_func)(uint8_t), uint8_t (*read_func)(void)) {
    if (self == NULL) return;
    self->W25Q_Select    = select_func;
    self->W25Q_SPI_Write = write_func;
    self->W25Q_SPI_Read  = read_func;
}

void W25Q_WriteEnable(W25Q_t* self){
    self->W25Q_Select(true);
    uint8_t cmd = W25Q_WRITE_ENABLE;
    self->W25Q_SPI_Write(cmd);
    self->W25Q_Select(false);
}

void W25Q_WriteDisable(W25Q_t* self){
    self->W25Q_Select(true);
    uint8_t cmd = W25Q_WRITE_DISABLE;
    self->W25Q_SPI_Write(cmd);
    self->W25Q_Select(false);
}

void W25Q_ReadData(W25Q_t* self, uint8_t* buf, uint32_t address, uint32_t size){
    self->W25Q_Select(true);

    uint8_t cmd = W25Q_READ_DATA;

    self->W25Q_SPI_Write(cmd);
    self->W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)(address & 0xFF));
    for(uint32_t i = 0; i < size; i++){
        buf[i] = self->W25Q_SPI_Read();
    }

    self->W25Q_Select(false);
}

void W25Q_FastRead(W25Q_t* self, uint8_t* buf, uint32_t address, uint32_t size){
    self->W25Q_Select(true);
    uint8_t cmd = W25Q_FAST_READ;
    self->W25Q_SPI_Write(cmd);
    self->W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)(address & 0xFF));
    uint8_t dummy_byte = 0x00;
    self->W25Q_SPI_Write(dummy_byte);
    for(uint32_t i = 0; i < size; i++){
        buf[i] = self->W25Q_SPI_Read();
    }
    self->W25Q_Select(false);
}

void W25Q_PageProgram(W25Q_t* self, uint8_t* buf, uint32_t address, uint32_t size){
    if(size >= 256){
        address &= 0xFFFFFF00;
        size = 256;
    }

    W25Q_WriteEnable(self);

    self->W25Q_Select(true);
    uint8_t cmd = W25Q_PAGE_PROGRAM;
    self->W25Q_SPI_Write(cmd);
    self->W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)(address & 0xFF));
    for(uint32_t i = 0; i < size; i++){
        self->W25Q_SPI_Write(buf[i]);
    }
    self->W25Q_Select(false);
}

void W25Q_MultiPageProgram(W25Q_t* self, uint8_t* buf, uint32_t address, uint32_t size){
    if(size >= 256){
        address &= 0xFFFFFF00;
        size = 256;
    }

    W25Q_WriteEnable(self);

    self->W25Q_Select(true);
    uint8_t cmd = W25Q_PAGE_PROGRAM;
    self->W25Q_SPI_Write(cmd);
    self->W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)(address & 0xFF));
    for(uint32_t i = 0; i < size; i++){
        self->W25Q_SPI_Write(buf[i]);
    }
    self->W25Q_Select(false);
}

void W25Q_SectorErase(W25Q_t* self, uint32_t address){
    address &= 0xFFFFF000;  

    W25Q_WriteEnable(self);

    self->W25Q_Select(true);

    uint8_t cmd = W25Q_SECTOR_ERASE;
    self->W25Q_SPI_Write(cmd);
    self->W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)(address & 0xFF));

    self->W25Q_Select(false);
}

void W25Q_BlockErase(W25Q_t* self, uint32_t address){
    address &= 0xFFFF0000;  

    W25Q_WriteEnable(self);

    self->W25Q_Select(true);

    uint8_t cmd = W25Q_64K_BLOCK_ERASE;
    self->W25Q_SPI_Write(cmd);
    self->W25Q_SPI_Write((uint8_t)((address >> 16) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)((address >> 8) & 0xFF));
    self->W25Q_SPI_Write((uint8_t)(address & 0xFF));

    self->W25Q_Select(false);
}

void W25Q_ChipErase(W25Q_t* self){
    W25Q_WriteEnable(self);

    self->W25Q_Select(true);

    uint8_t cmd = W25Q_CHIP_ERASE;
    self->W25Q_SPI_Write(cmd);

    self->W25Q_Select(false);
}

uint8_t W25Q_ReadStatusRegister(W25Q_t* self, uint8_t reg){
    self->W25Q_Select(true);
        
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

    self->W25Q_SPI_Write(cmd);
    reg = self->W25Q_SPI_Read();
        
    self->W25Q_Select(false);

    return reg;
}

bool W25Q_IsBusy(W25Q_t* self){
    if(W25Q_ReadStatusRegister(self, 1)){
        return true;
    }
    return false;
}

uint32_t W25Q_ReadID(W25Q_t* self){
    if (self == NULL || self->W25Q_Select == NULL || 
        self->W25Q_SPI_Write == NULL || self->W25Q_SPI_Read == NULL) {
        return 0xDEADBEEF;
    }
    
    uint32_t id;

    self->W25Q_Select(true);
        
    uint8_t cmd = W25Q_GET_JEDEC_ID;
    self->W25Q_SPI_Write(cmd);

    uint8_t manufacturer_id = self->W25Q_SPI_Read(); 
    uint8_t memory_type_id  = self->W25Q_SPI_Read(); 
    uint8_t capacity_id     = self->W25Q_SPI_Read(); 

    id =    (uint32_t)(manufacturer_id << 16) | 
            (uint32_t)(memory_type_id  << 8)  | 
            (uint32_t)(capacity_id);

    self->W25Q_Select(false);

    return id;
}
