#define I2C_SPEED 3300

static uint8_t i2c_buffer[256];
static I2C_XFER_T xfer;

void initialize(){
    // Initialize Stuff 
    Board_I2C_Init(I2C0);
    Chip_I2C_Init(I2C0);
    Chip_I2C_SetClockRate(I2C0, I2C_SPEED);
    Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
    NVIC_EnableIRQ(I2C0_IRQn);

}

void write(int address, int data){
    // Do stuff
    xfer->slaveAddr = tmp;
    xfer->rxBuff = 0;
    xfer->txBuff = 0;
    xfer->txSz = 0;
    xfer->rxSz = 0;
    xfer->txSz = tmp;
		xfer->txBuff = data;
    Chip_I2C_MasterSend(I2C0, xfer.slaveAddr, xfer.txBuff, xfer.txSz);
}

void Change_MIC(int mux_num){
  
  
}

int read(int address){
    int data;
    int num_bytes_read;

    xfer.slaveAddr = 	0x48;
    xfer.rxBuff = 		0x00;
    xfer.txBuff = 		0x00;
    xfer.txSz = 		0x00;
    xfer.rxSz = 		0x02;
    xfer.rxBuff = &i2c_buffer;

    Chip_I2C_MasterRead(I2C0, xfer.slaveAddr, xfer.rxBuff, xfer.rxSz);

    return data;
}
static void i2c_state_handling(I2C_ID_T id)
{
    if (Chip_I2C_IsMasterActive(id))
      Chip_I2C_MasterStateHandler(id);
    else
      Chip_I2C_SlaveStateHandler(id);
}

void I2C0_IRQHandler(void)
{
	  i2c_state_handling(I2C0);
}
