/*****************************************************************************
 * file: qsfp_iic.c
 * description: IIC interface for QSFP on the PETB.
 * device: vcu108
 * version: 0.01
 *
 * author: Irfan Punekar
 * company: Precision Optical Transceivers
 * date: 14 September 2018
 *
 * author: Shreya Patel
 * modified: Summer 2019
 *
 * author: Sidney Davis
 * modified: Summer 2019
 *****************************************************************************/

/***************************** Include Files *********************************/

#include "qsfp_iic.h"

/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define IIC_DEVICE_ID	            XPAR_IIC_0_DEVICE_ID
#define INTC_DEVICE_ID	            XPAR_INTC_0_DEVICE_ID
#define IIC_INTR_ID	                XPAR_INTC_0_IIC_0_VEC_ID

/*
 * The following constant defines the address of the IIC Slave device on the
 * IIC bus. Note that since the address is only 7 bits, this constant is the
 * address divided by 2.
 * The 7 bit IIC Slave address of the IIC EEPROM on the ML300/ML310/ML403/ML410/
 * ML501/ML505/ML507/ML510 boards is 0x50. The 7 bit IIC Slave address of the
 * IIC EEPROM on the ML605/SP601/SP605 boards is 0x54.
 * Please refer the User Guide's of the respective boards for further
 * information about the IIC slave address of IIC EEPROM's.
 */
#define EEPROM_ADDRESS              0x50	/* 0xA0 as an 8 bit number. */

/*
 * The page size determines how much data should be written at a time.
 * The ML310/ML300 board supports a page size of 32 and 16.
 * The write function should be called with this as a maximum byte count.
 */
#define PAGE_SIZE                   16

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

int EepromWriteData(u16 ByteCount);

//int EepromReadData(u8 *BufferPtr, u16 ByteCount);
int EepromReadData(u8 *BufferPtr, u16 ByteCount, AddressType_iic Address);

static int SetupInterruptSystem(XIic *IicInstPtr);

static int DisableInterruptSystem(XIic *IicInstPtr);

static void SendHandler(XIic *InstancePtr);

static void ReceiveHandler(XIic *InstancePtr);

static void StatusHandler(XIic *InstancePtr, int Event);


/************************** Variable Definitions *****************************/

XIic IicInstance;		        /* The instance of the IIC device. */
XIntc InterruptController;	    /* The instance of the Interrupt Controller. */

/*
 * Write buffer for writing a page.
 */
u8 WriteBuffer[sizeof(AddressType_iic) + PAGE_SIZE];

u8 ReadBuffer[PAGE_SIZE];	    /* Read buffer for reading a page. */

volatile u8 TransmitComplete;	/* Flag to check completion of Transmission */
volatile u8 ReceiveComplete;	/* Flag to check completion of Reception */

u8 EepromIicAddr;		        /* Variable for storing Eeprom IIC address */
u8 IIC_Device_Addr;		        /* Variable for storing Eeprom IIC address */

#define QSFP_MUX_ADDR 0x74
#define QSFP_MUX_PORT 0x04
#define QSFP_IIC_ADDR 0x50

/************************** Function Definitions *****************************/

int qsfp_startup(XIic_Config *qsfp_config){
	int i = 0;
	int Status;

	/* Set up the QSFP for I2C */
	qsfp_gpio_clear("MODSEL");
	qsfp_gpio_set("RESET");
	qsfp_gpio_clear("LPMODE");

	/*
	 * Initialize the IIC driver so that it is ready to use.
	 */
	qsfp_config = XIic_LookupConfig(IIC_DEVICE_ID);
	if (qsfp_config == NULL) {
		return XST_FAILURE;
	}

	Status = XIic_CfgInitialize(&IicInstance, qsfp_config,
			qsfp_config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Generate GPIO Reset sequence
	XIic_WriteReg(IicInstance.BaseAddress, XIIC_GPO_REG_OFFSET, 0xFFFFFFFF);  // bit0 = 1
	for(i=0;i<100000;i++);  // delay
	XIic_WriteReg(IicInstance.BaseAddress, XIIC_GPO_REG_OFFSET, 0xFFFFFFFE);  // bit0 = 0
	for(i=0;i<100000;i++);  // delay
	XIic_WriteReg(IicInstance.BaseAddress, XIIC_GPO_REG_OFFSET, 0xFFFFFFFF);  // bit0 = 1

	/*
	 * Setup the Interrupt System.
	 */
	Status = SetupInterruptSystem(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/*
	 * Set the Handlers for transmit and reception.
	 */
	XIic_SetSendHandler  (&IicInstance, &IicInstance, 		(XIic_Handler) SendHandler);
	XIic_SetRecvHandler  (&IicInstance, &IicInstance, 		(XIic_Handler) ReceiveHandler);
	XIic_SetStatusHandler(&IicInstance, &IicInstance, (XIic_StatusHandler) StatusHandler);

	return XST_SUCCESS;
}

int qsfp_shutdown(void){

	int Status = DisableInterruptSystem(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	qsfp_gpio_clear("MODSEL");
	qsfp_gpio_clear("LPMODE");
	qsfp_gpio_clear("RESET");

	return XST_SUCCESS;
}

int qsfp_iic_read_buffer(u8 *dest, AddressType_iic addr, u16 length){
	//address must be 0-255, page must be 0 - N, length < (255-addr)

	u32 Index;
	int Status;

	/*
	 * Set the Slave address to the PCA9543A IIC MUX
	 */
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE, QSFP_MUX_ADDR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Write to the IIC Switch
	 */
	WriteBuffer[0] = QSFP_MUX_PORT; // Select IIC MUX Bus
	Status = EepromWriteData(1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Initialize Write and Read Buffers
	 */
	for (Index = 0; Index < length; Index++) {
		WriteBuffer[sizeof(addr) + Index] = 0xFF;
		dest[Index] = 0;
	}

	/*
	 * Set the Slave address of the IIC Device to Test
	 */
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE, QSFP_IIC_ADDR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the test starting address for writing/reading the IIC Device
	 */
	if (sizeof(addr) == 1) {
		WriteBuffer[0] = (u8) (addr);
	} else {
		WriteBuffer[0] = (u8) (addr >> 8);
		WriteBuffer[1] = (u8) (addr);
	}


	/*
	 * Read from the EEPROM.
	 */
	Status = EepromReadData(dest, length, addr);
	return XST_SUCCESS;
}

int qsfp_iic_write_buffer(char *data, AddressType_iic addr, u16 length){
	u32 Index;
	int Status;

	/*
	 * Set the Slave address to the PCA9543A IIC MUX
	 */
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
			QSFP_MUX_ADDR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Write to the IIC Switch
	 */
	WriteBuffer[0] = QSFP_MUX_PORT; // Select IIC MUX Bus
	Status = EepromWriteData(1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Initialize Write and Read Buffers
	 */
	for (Index = 0; Index < length; Index++) {
		WriteBuffer[sizeof(addr) + Index] = data[Index];
	}

	/*
	 * Set the Slave address of the IIC Device to Test
	 */
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE, QSFP_IIC_ADDR);
	//xil_printf("EEPROM IIC Address %d\r\n", addr);								// uncomment for debugging
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the test starting address for writing/reading the IIC Device
	 */
	if (sizeof(addr) == 1) {
		WriteBuffer[0] = (u8) (addr);
	} else {
		WriteBuffer[0] = (u8) (addr >> 8);
		WriteBuffer[1] = (u8) (addr);
	}


	/*
	 * Read from the EEPROM.
	 */
	Status = EepromWriteData(sizeof(addr) + length);

	return XST_SUCCESS;
}

int qsfp_read_serial()
{
	u8 dest[PAGE_SIZE];
	AddressType_iic addr = 196;
	int length = 16;
	qsfp_iic_read_buffer(dest, addr, length);
	xil_printf("SN: ");
	int i;
	for(i = 0; i < length; i++){
		xil_printf("%c",dest[i]);
	}
	xil_printf("\r\n");

	return XST_SUCCESS;
}

int qsfp_read_rssi()
{
	int length = 8;
	u8 dest[length];
	AddressType_iic addr = 34;
	qsfp_iic_read_buffer(dest, addr, length);
	for(int chan = 0; chan < 4; chan++){
		xil_printf("RX RSSI Channel[%d]: %u (0x%x, 0x%x)\r\n",
				chan, dest[chan*2]*256 + dest[chan*2+1], dest[chan*2], dest[chan*2+1]);
	}

	return XST_SUCCESS;
}

int qsfp_read_vendor()
{
	u8 dest[PAGE_SIZE];
	AddressType_iic addr = 148;
	int length = 16;
	qsfp_iic_read_buffer(dest, addr, length);
	xil_printf("Vendor Name: ");
	for(int i = 0; i < length; i++){
		xil_printf("%c",dest[i]);
	}
//	xil_printf("\r\nVender Name (Hex): ");
//	for(int i = 0; i < length; i++){
//		xil_printf("%X ",dest[i]);
//	}
	xil_printf("\r\n");

	return XST_SUCCESS;
}

int qsfp_write_vendor(char *data)
{
	AddressType_iic addr = 148;
	int length = 16;
	qsfp_iic_write_buffer(data, addr, length);
	xil_printf("New Vendor Name: ");
	for(int i = 0; i < length; i++){
		xil_printf("%c",data[i]);
	}
	xil_printf("\r\n");

	return XST_SUCCESS;
}

int qsfp_read_password()
{
	u8 dest[PAGE_SIZE];
	AddressType_iic addr = 123;
	int length = 4;
	qsfp_iic_read_buffer(dest, addr, length);
	xil_printf("Current password: ");
	int i;
	for(i = 0; i < length; i++){
		xil_printf(" %02x ",(u8)dest[i]);
	}
	xil_printf("\r\n");

	return XST_SUCCESS;
}

int qsfp_write_password(char *pwd)
{
	AddressType_iic addr=123;
	int length = 4;
	qsfp_iic_write_buffer(pwd, addr, length);
	xil_printf("New password:     ");
	int i;
	for(i = 0; i < length; i++){
		xil_printf(" %02x ",(u8)pwd[i]);
	}
	xil_printf("\r\n");

	return XST_SUCCESS;
}

int qsfp_read_page()
{
	u8 dest[PAGE_SIZE];
	AddressType_iic addr=127;
	int length = 1;
	qsfp_iic_read_buffer(dest, addr, length);
	xil_printf("Current page: %X\r\n", dest[0]);
	return XST_SUCCESS;
}

int qsfp_write_page(int page)
{
	char data[PAGE_SIZE];
	AddressType_iic addr = 127;
	int length = 1;
	data[0] = page;
	qsfp_iic_write_buffer(data, addr, length);
	//xil_printf("New page: %X\r\n", data[0]); // uncomment for debugging
	return XST_SUCCESS;
}

int qsfp_iic_read (int page, AddressType_iic startAddr, AddressType_iic endAddr, u8 *dataRead) {
	init_platform();
	XIic_Config qsfp_config;
	qsfp_startup(&qsfp_config);

	unsigned int length = (endAddr - startAddr + 1) ;

	qsfp_write_page(page);
	qsfp_iic_read_buffer(dataRead, startAddr, length);
	qsfp_shutdown();

	return XST_SUCCESS;
}

int qsfp_iic_write (int page, AddressType_iic startAddr, char *dataType, char *dataList) {

	init_platform();
	XIic_Config qsfp_config;
	qsfp_startup(&qsfp_config);

	qsfp_write_page(page);
	
	unsigned int length = strlen(dataList);

	if (strcmp(strupr(dataType), "CHAR") == 0) {
		// The data passed thru is a string of characters
		qsfp_iic_write_buffer(dataList, startAddr, length);
	} else 
	if (strcmp(strupr(dataType), "HEX") == 0) {
		// The data passed thru is a list of hex characters
		// This list needs to be converted to a str of char
		length = (length + 1) / 3;
		char hexList[length];
		char *ptrOld, *ptrNew;
		
		hexList[0] = (char)strtol(dataList, &ptrNew, 16);
		ptrOld = ptrNew; // skip the space between numbers
		for (int i = 1; i < length; i++) {
			hexList[i] = (char)strtol(ptrOld, &ptrNew, 16);
			ptrOld = ptrNew;
		}
		qsfp_iic_write_buffer(hexList, startAddr, length);
	} else {
		xil_printf("Invalid Data Type. Try 'Char' or 'Hex'.\r\n");
		return XST_FAILURE; 
	}
	
	qsfp_shutdown();
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function writes a buffer of data to the IIC serial EEPROM.
 *
 * @param	ByteCount contains the number of bytes in the buffer to be
 *		written.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 * @note		The Byte count should not exceed the page size of the EEPROM as
 *		noted by the constant PAGE_SIZE.
 *
 ******************************************************************************/
int EepromWriteData(u16 ByteCount)
{
	int Status;

	/*
	 * Set the defaults.
	 */
	TransmitComplete = 1;
	IicInstance.Stats.TxErrors = 0;

	/*
	 * Start the IIC device.
	 */
	Status = XIic_Start(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Send the Data.
	 */
	Status = XIic_MasterSend(&IicInstance, WriteBuffer, ByteCount);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait till the transmission is completed.
	 */
	while ((TransmitComplete) || (XIic_IsIicBusy(&IicInstance) == TRUE)) {
		/*
		 * This condition is required to be checked in the case where we
		 * are writing two consecutive buffers of data to the EEPROM.
		 * The EEPROM takes about 2 milliseconds time to update the data
		 * internally after a STOP has been sent on the bus.
		 * A NACK will be generated in the case of a second write before
		 * the EEPROM updates the data internally resulting in a
		 * Transmission Error.
		 */
		if (IicInstance.Stats.TxErrors != 0) {


			/*
			 * Enable the IIC device.
			 */
			Status = XIic_Start(&IicInstance);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}


			if (!XIic_IsIicBusy(&IicInstance)) {
				/*
				 * Send the Data.
				 */
				Status = XIic_MasterSend(&IicInstance,
						WriteBuffer,
						ByteCount);
				if (Status == XST_SUCCESS) {
					IicInstance.Stats.TxErrors = 0;
				}
				else {
				}
			}
		}
	}

	/*
	 * Stop the IIC device.
	 */
	Status = XIic_Stop(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function reads data from the IIC serial EEPROM into a specified buffer.
 *
 * @param	BufferPtr contains the address of the data buffer to be filled.
 * @param	ByteCount contains the number of bytes in the buffer to be read.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 * @note		None.
 *
 ******************************************************************************/
int EepromReadData(u8 *BufferPtr, u16 ByteCount, AddressType_iic Address)
{
	int Status;

	/*
	 * Set the Defaults.
	 */
	ReceiveComplete = 1;

	/*
	 * Position the Pointer in EEPROM.
	 */
	if (sizeof(Address) == 1) {
		WriteBuffer[0] = (u8) (Address);
	}
	else {
		WriteBuffer[0] = (u8) (Address >> 8);
		WriteBuffer[1] = (u8) (Address);
	}

	Status = EepromWriteData(sizeof(Address));
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the IIC device.
	 */
	Status = XIic_Start(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Receive the Data.
	 */
	Status = XIic_MasterRecv(&IicInstance, BufferPtr, ByteCount);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait till all the data is received.
	 */
	while ((ReceiveComplete) || (XIic_IsIicBusy(&IicInstance) == TRUE)) {

	}

	/*
	 * Stop the IIC device.
	 */
	Status = XIic_Stop(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function setups the interrupt system so interrupts can occur for the
 * IIC device. The function is application-specific since the actual system may
 * or may not have an interrupt controller. The IIC device could be directly
 * connected to a processor without an interrupt controller. The user should
 * modify this function to fit the application.
 *
 * @param	IicInstPtr contains a pointer to the instance of the IIC device
 *		which is going to be connected to the interrupt controller.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 * @note		None.
 *
 ******************************************************************************/
static int SetupInterruptSystem(XIic *IicInstPtr)
{
	int Status;

	if (InterruptController.IsStarted == XIL_COMPONENT_IS_STARTED) {
		return XST_SUCCESS;
	}

	/*
	 * Initialize the interrupt controller driver so that it's ready to use.
	 */
	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(&InterruptController, IIC_INTR_ID,
			(XInterruptHandler) XIic_InterruptHandler,
			IicInstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the interrupt controller so interrupts are enabled for all
	 * devices that cause interrupts.
	 */
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable the interrupts for the IIC device.
	 */
	XIntc_Enable(&InterruptController, IIC_INTR_ID);

	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XIntc_InterruptHandler,
			&InterruptController);

	/*
	 * Enable non-critical exceptions.
	 */
	Xil_ExceptionEnable();



	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function setups the interrupt system so interrupts can occur for the
 * IIC device. The function is application-specific since the actual system may
 * or may not have an interrupt controller. The IIC device could be directly
 * connected to a processor without an interrupt controller. The user should
 * modify this function to fit the application.
 *
 * @param	IicInstPtr contains a pointer to the instance of the IIC device
 *		which is going to be connected to the interrupt controller.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 * @note		None.
 *
 ******************************************************************************/
static int DisableInterruptSystem(XIic *IicInstPtr)
{

	/*
	 * Disable the interrupt
	 */
	XIntc_Disable(&InterruptController, IIC_INTR_ID);
	/*
	 * Disconnect the driver handler.
	 */
	XIntc_Disconnect(&InterruptController, IIC_INTR_ID);

	/*
	 * Disable non-critical exceptions.
	 */
	Xil_ExceptionDisable();

	/*
	 * Remove the interrupt controller handler from the exception table.
	 */
	Xil_ExceptionRemoveHandler(XIL_EXCEPTION_ID_INT);

	/*
	 * Stop the interrupt controller such that interrupts are disabled for
	 * all devices that cause interrupts
	 */
	XIntc_Stop(&InterruptController);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This Send handler is called asynchronously from an interrupt
 * context and indicates that data in the specified buffer has been sent.
 *
 * @param	InstancePtr is not used, but contains a pointer to the IIC
 *		device driver instance which the handler is being called for.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
static void SendHandler(XIic *InstancePtr)
{
	TransmitComplete = 0;
}

/*****************************************************************************/
/**
 * This Receive handler is called asynchronously from an interrupt
 * context and indicates that data in the specified buffer has been Received.
 *
 * @param	InstancePtr is not used, but contains a pointer to the IIC
 *		device driver instance which the handler is being called for.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
static void ReceiveHandler(XIic *InstancePtr)
{
	ReceiveComplete = 0;
}

/*****************************************************************************/
/**
 * This Status handler is called asynchronously from an interrupt
 * context and indicates the events that have occurred.
 *
 * @param	InstancePtr is a pointer to the IIC driver instance for which
 *		the handler is being called for.
 * @param	Event indicates the condition that has occurred.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
static void StatusHandler(XIic *InstancePtr, int Event)
{

}
