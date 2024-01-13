
#include "ecumain.h"
#include "dwt_delay.h"
#include "canecu.h"
#include "adcecu.h"
#include "errors.h"
#include "uartecu.h"
#include "debug.h"
#include "output.h"
#include "timerecu.h"
#include "power.h"
#include "queue.h"
#include "watchdog.h"
#include "ecumain.h"
#include "fdcan.h"
#include "semphr.h"
#include "taskpriorities.h"
#include "eeprom.h"
#include "inverter.h"


FDCAN_HandleTypeDef * hfdcan2p = NULL;

//variables that need to be accessible in ISR's

int cancount;

SemaphoreHandle_t CANBufferUpdating, bus0TXDone, bus1TXDone;

#define CANTXSTACK_SIZE 128*8
#define CANTXTASKNAME  "CANTxTask"
StaticTask_t xCANTXTaskBuffer;
StackType_t xCANTXStack[ CANTXSTACK_SIZE ];

TaskHandle_t CANTxTaskHandle = NULL;


#define CANRXSTACK_SIZE 128*12
#define CANRXTASKNAME  "CANRxTask"
StaticTask_t xCANRXTaskBuffer;
StackType_t xCANRXStack[ CANRXSTACK_SIZE ];

TaskHandle_t CANRxTaskHandle = NULL;


/* The queue is to be created to hold a maximum of 10 uint64_t
variables. */
#define CANTxQUEUE_LENGTH    64
#define CANRxQUEUE_LENGTH    256
#define CANITEMSIZE			sizeof( struct can_msg )

#define CANTxITEMSIZE		CANITEMSIZE
#define CANRxITEMSIZE		CANITEMSIZE

/* The variable used to hold the queue's data structure. */
static StaticQueue_t CANTxStaticQueue, CANRxStaticQueue;

/* The array to use as the queue's storage area.  This must be at least
uxQueueLength * uxItemSize bytes. */
uint8_t CANTxQueueStorageArea[ CANTxQUEUE_LENGTH * CANITEMSIZE ];
uint8_t CANRxQueueStorageArea[ CANRxQUEUE_LENGTH * CANITEMSIZE ];

QueueHandle_t CANTxQueue, CANRxQueue;

#if defined( __ICCARM__ )
  #define DMA_BUFFER \
      _Pragma("location=\".dma_buffer\"")
#else
  #define DMA_BUFFER \
      __attribute__((section(".dma_buffer")))
#endif

// TODO ADC conversion buffer, should be aligned in memory for faster DMA?
DMA_BUFFER ALIGN_32BYTES (static uint8_t CANTxBuffer1[1024]);
DMA_BUFFER ALIGN_32BYTES (static uint8_t CANTxBuffer2[1024]);

static uint8_t * CANTxBuffer;

//static uint16_t CANTxBufferPos;
static uint8_t * CurCANTxBuffer;
static uint32_t CANTxLastsend;

uint32_t rxcount1 = 0;
uint32_t rxcount2 = 0;

uint8_t canload1;
uint8_t canload2;

bool processCan1Message( FDCAN_RxHeaderTypeDef *RxHeader, uint8_t CANRxData[8]);
bool processCan2Message( FDCAN_RxHeaderTypeDef *RxHeader, uint8_t CANRxData[8]);

void processCanTimeouts( void );
