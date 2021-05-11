#include  "OSAL.h"
#include  "AF.h"
#include  "ZDAPP.h"
#include  "ZDObject.h"
#include  "ZDProfile.h"
#include  <string.h>
#include  "Coordinator.h"
#include  "DebugTrace.h"
#if !defined(WIN32)
#include "OnBoard.h"
#endif

#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
* �ļ���  �� GENERICAPP_MAX_CLUSTERS
* ����    �� chenweibing
* �汾    �� V1.0.0
* ʱ��    �� 2021/5/11
* ��Ҫ    �� GENERICAPP_MAX_CLUSTERS�궨��  
********************************************************************
* ����
*
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] =
{
GENERICAPP_CLUSTERID
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ��������GenericApp_SimpleDesc
* ���� �� chenweibing
* ������void
* ���أ�void
* ����������˵��
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
  GENERICAPP_ENDPOINT,
  GENERICAPP_PROFID,
  GENERICAPP_DEVICEID,
  GENERICAPP_DEVICE_VERSION,
  GENERICAPP_FLAGS,
  GENERICAPP_MAX_CLUSTERS,
  (cId_t *)GenericApp_ClusterList,
  0,
  (cId_t *)NULL
};


endPointDesc_t GenericApp_epDesc;
byte GenericApp_TaskID;
byte GenericApp_TransID;


void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt );
void GenericApp_SendTheMessage(void);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ��������GenericApp_Init
* ���� �� chenweibing
* ������byte task_id
* ���أ�void
* ����������˵��
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void GenericApp_Init(byte task_id)
{
  GenericApp_TaskID              =task_id;
  GenericApp_TransID             = 0;
  GenericApp_epDesc.endPoint     =GENERICAPP_ENDPOINT;
  GenericApp_epDesc.task_id      =&GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc   =(SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq   =noLatencyReqs;
  afRegister( &GenericApp_epDesc);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ��������GenericApp_ProcessEvent
* ���� �� chenweibing
* ������ byte task_id, UINT16 events
* ���أ�UINT16 
* ����������˵��
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  if ( events & SYS_EVENT_MSG )
{
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(GenericApp_TaskID);
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
    {
        case AF_INCOMING_MSG_CMD:
          GenericApp_MessageMSGCB( MSGpkt );
          break;
        default:
          break;
    }
    osal_msg_deallocate( (uint8 *) MSGpkt );
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive
    ( GenericApp_TaskID );
   }
   return (events^SYS_EVENT_MSG);
  }
  return 0 ;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ��������GenericApp_MessageMSGCB
* ���� �� Mayecho
* ������ afIncomingMSGPacket_t *pkt
* ���أ� void
* ����������˵��
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt)
{
  unsigned char buffer[4]=" ";
  switch ( pkt->clusterId )
{
    case GENERICAPP_CLUSTERID:
      osal_memcpy(buffer,pkt->cmd.Data,3);
      if((buffer[0]=='L')||(buffer[1]=='E')||(buffer[2]=='D'))
      {
        HalLedBlink(HAL_LED_2,0,50,500);
      }
      else
      {
        HalLedSet(HAL_LED_2,HAL_LED_MODE_ON);
      }
    break;

  }
}
