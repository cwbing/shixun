#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include <string.h>
#include "Coordinator.h"
#include "DebugTrace.h"

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
* ��Ҫ    �� �궨�壬����Э��ջ�����ݵĶ����ʽ����һ���ļ�˵��  
********************************************************************
* ����
*
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS]=
{
    GENERICAPP_CLUSTERID
};


//����һ��ZigBee���豸�ڵ�
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


void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );   //������Ϣ������
void GenericApp_SendTheMessage( void );     //�������ݷ��ͺ���



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ��������GenericApp_Init
* ���� �� chenweibing
* ������byte task_id
* ���أ�void
* �����������ʼ������
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void GenericApp_Init( byte task_id )
{
    GenericApp_TaskID = task_id;  //OSAL�������ȼ�
    GenericApp_TransID = 0;   //���巢���װ�Ϊ0
    GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;  //�ڵ���������ʼ��
    GenericApp_epDesc.task_id = &GenericApp_TaskID;
    GenericApp_epDesc.simpleDesc
              = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
    GenericApp_epDesc.latencyReq = noLatencyReqs;
    afRegister( &GenericApp_epDesc );  //ע��ڵ������� 
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ��������GenericApp_ProcessEvent
* ���� �� chenweibing
* ������byte task_id, UINT16 events
* ���أ�UINT16
* ��������������
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
    afIncomingMSGPacket_t *MSGpkt; //����һ��ָ�������Ϣ�ṹ���ָ��MSGpkt
    
    if ( events & SYS_EVENT_MSG )  //�������ݰ�
    {
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
        while ( MSGpkt ) 
        {
            switch ( MSGpkt->hdr.event )
            {

                case AF_INCOMING_MSG_CMD:   //�Խ��ܵ����ݽ����ж�
            GenericApp_MessageMSGCB( MSGpkt );  //�������ݵĴ���ִ����Ϣ�ص�����
            break;
        default:
            break;
            }
        osal_msg_deallocate( (uint8 *)MSGpkt );  
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
        }
        return (events ^ SYS_EVENT_MSG);  
    }
        return 0;
}

