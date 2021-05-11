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
* 文件名  ： GENERICAPP_MAX_CLUSTERS
* 作者    ： chenweibing
* 版本    ： V1.0.0
* 时间    ： 2021/5/11
* 简要    ： 宏定义，保持协议栈里数据的定义格式保持一致文件说明  
********************************************************************
* 副本
*
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS]=
{
    GENERICAPP_CLUSTERID
};


//描述一个ZigBee的设备节点
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


void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );   //声明消息处理函数
void GenericApp_SendTheMessage( void );     //声明数据发送函数



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 函数名：GenericApp_Init
* 作者 ： chenweibing
* 参数：byte task_id
* 返回：void
* 描述：任务初始化函数
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void GenericApp_Init( byte task_id )
{
    GenericApp_TaskID = task_id;  //OSAL分配优先级
    GenericApp_TransID = 0;   //定义发送首包为0
    GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;  //节点描述符初始化
    GenericApp_epDesc.task_id = &GenericApp_TaskID;
    GenericApp_epDesc.simpleDesc
              = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
    GenericApp_epDesc.latencyReq = noLatencyReqs;
    afRegister( &GenericApp_epDesc );  //注册节点描述符 
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 函数名：GenericApp_ProcessEvent
* 作者 ： chenweibing
* 参数：byte task_id, UINT16 events
* 返回：UINT16
* 描述：任务处理函数
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
    afIncomingMSGPacket_t *MSGpkt; //定义一个指向接收消息结构体的指针MSGpkt
    
    if ( events & SYS_EVENT_MSG )  //接收数据包
    {
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
        while ( MSGpkt ) 
        {
            switch ( MSGpkt->hdr.event )
            {

                case AF_INCOMING_MSG_CMD:   //对接受的数据进行判断
            GenericApp_MessageMSGCB( MSGpkt );  //无线数据的处理，执行信息回调函数
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

