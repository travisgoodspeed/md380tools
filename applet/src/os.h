/*! \file os.h
  \brief µCOS defines
*/


#ifndef MD380TOOLS_OS_H_INCLUDED
#define MD380TOOLS_OS_H_INCLUDED

typedef	unsigned char	BOOLEAN;
typedef	unsigned char	INT8U;
typedef	signed char	INT8S;
typedef	unsigned short	INT16U;
typedef	signed short	INT16S;
typedef	unsigned int	INT32U;
typedef	signed int	INT32S;
typedef	float		FP32; 
typedef	double		FP64;
typedef	unsigned int	OS_STK;
typedef	unsigned int	OS_CPU_SR;

#define OS_LOWEST_PRIO 60   // see 0x0804bb66
#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 8u + 1u)

typedef  INT8U    OS_PRIO;

typedef struct os_event {
  INT8U    OSEventType;
  void    *OSEventPtr;
  INT16U   OSEventCnt;
  OS_PRIO  OSEventGrp;
  OS_PRIO  OSEventTbl[OS_EVENT_TBL_SIZE];
  INT8U   *OSEventName;
  } OS_EVENT;   // size 24 checked with 0x0804421e and 0x0804422a .. 20015f6c 20015f84


// Sem. to sync access of global DebugLineX between display and dmr task
extern OS_EVENT* debug_line_sem;

#endif
