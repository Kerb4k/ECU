
#ifndef CANECU_H_
#define CANECU_H_

#include "ecu.h"
#include "queue.h"


#define CANBUS1					hfdcan1 //Critical canbus
#define CANBUS2					hfdcan2 //Secondary canbus

//TODO pick ids and messages from BMS
#define BMSBASE_ID				0x???
#define BMSSOC_ID				0x???

#define FLSpeed_COBID			0x71 // 112 // 0x70 orig
#define FLSpeed_BUS				CAN1
#define FRSpeed_COBID			0x70 // 113 // 0x71 orig
#define FRSpeed_BUS				CAN1

#define PDM_ID					0x520
#define MEMORATOR_ID			0x07B

#define NodeErr_ID        		0x600
#define NodeCmd_ID				0x602
#define NodeAck_ID				0x601

#define AdcSimInput_ID			0x608

//TODO look at node system ids

#define CANB1					(1)
#define CANB2					(2)


#define IVT_BUS					CANB1
#define BMS_BUS					CANB1

#define COBTPDO1_ID				(0x180)
#define COBTPDO2_ID				(0x280)
#define COBTPDO3_ID				(0x380)
#define COBTPDO4_ID				(0x480)

#define COBRPDO1_ID				(0x200)
#define COBRPDO2_ID				(0x300)
#define COBRPDO3_ID				(0x400)
#define COBRPDO4_ID				(0x500)
#define COBRPDO5_ID				(0x540)

#define COBSDOS_ID				(0x600)

#define Inverter_BUS			CANB0

#define ECU_CAN_ID				(0x020) // send +1

#define ENABLED					(true)
#define DISABLED				(false)


enum canbus { bus1 = 1, bus2 = 2, bus3 = 0 };

typedef struct can_msg {
	enum canbus bus;
	uint16_t id;
	uint8_t dlc;
	uint8_t data[8];
} can_msg;

typedef struct canfd_msg {
	enum canbus bus;
	uint16_t id;
	uint8_t dlc;
	uint8_t data[64];
} canfd_msg;

extern QueueHandle_t CanTxQueue;

extern int cancount;

typedef volatile struct CanDataType CANData;

typedef bool (*DataHandler)(const uint8_t CANRxData[8], const uint8_t DataLength, const CANData * datahandle );
typedef void (*TimeoutHandler)( uint16_t id );


typedef volatile struct CanDataType {
	volatile uint8_t *devicestate;
	uint16_t id;
	uint8_t dlcsize;
	DataHandler getData;
	TimeoutHandler doTimeout;
	uint32_t timeout;
	uint8_t  index;
	uint32_t time;
	uint16_t error;
	uint16_t receiveerr;
	bool	 errorsent;
} CANData;

int getNMTstate(volatile CANData *data );

void resetCanTx(volatile uint8_t CANTxData[8]);
int ResetCanReceived( void );
int ResetOperationCanReceived( void );
uint8_t CAN1Send( uint16_t id, uint8_t dlc, const uint8_t *pTxData );
uint8_t CAN2Send( uint16_t id, uint8_t dlc, const uint8_t *pTxData );
uint8_t CANSendSDO( enum canbus bus, uint16_t id, uint16_t idx, uint8_t sub, uint32_t data);
char CAN_NMT( uint8_t, uint8_t );
char CAN_ConfigRequest( uint8_t command, uint8_t success );
char CANKeepAlive( void );
uint8_t CANSendPDM( uint8_t highvoltage, uint8_t buzz );
#ifdef PDMSECONDMESSAGE
uint8_t CANSendPDMFAN( void );
#endif

char CAN_SendErrorStatus( char state, char substate, uint32_t errorcode );
char CAN_SendStatus( char state, char substate, uint32_t errorcode );
//char CANSendInverter( uint16_t response, uint16_t request, uint8_t inverter );
char CANLogDataFast( void );
char CANLogDataSlow( void );
char CAN_NMTSyncRequest( void );

char CAN_Send4vals( uint16_t id, uint16_t val1, uint16_t val2, uint16_t val3, uint16_t val4 );

char CAN_SendLED( void );

char CAN_SENDINVERTERERRORS( void );

void ResetCanData(volatile CANData *data );


char CAN_SendErrors( void );
char reTransmitError(uint32_t canid, const uint8_t *CANRxData, uint32_t DataLength );
char reTransmitOnCan1(uint32_t canid, const uint8_t *CANRxData, uint32_t DataLength );

char CAN_SendTimeBase( void );

char CAN_SendADCminmax( void );
char CAN_SendADC( volatile uint32_t *ADC_Data, uint8_t error );
//char CAN_SendADCVals( void );

int CheckCanError( void );

void processCANData(CANData * datahandle, uint8_t * CANRxData, uint32_t DataLength );
int receivedCANData( CANData * datahandle );


int RegisterCan1Message(CANData * CanMessage);
int RegisterCan2Message(CANData * CanMessage);

// initialisation

int initCAN( void );

#endif /* CANECU_H_ */
