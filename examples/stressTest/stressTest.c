/**************************************************************************************************
 * Filename:       stressTest.c
 * Description:    This file contains dataSendRcv application.
 *
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "rpc.h"
#include "mtSys.h"
#include "mtZdo.h"
#include "mtAf.h"
#include "mtParser.h"
#include "rpcTransport.h"
#include "dbgPrint.h"
#include "hostConsole.h"

/*********************************************************************
 * MACROS
 */
#define MAX_TEST_NODES      10
#define TEST_EP             1
#define TEST_PRIFILE        0x0104
#define TEST_CLUSTER        0x6

/*********************************************************************
 * TYPES
 */
typedef struct
{
	uint16_t nodeAddr;
	uint8_t txSeqNum;
	uint8_t rxSeqNum;
	uint32_t passCnt;
	uint32_t errorCnt;
} testNode_t;

/*********************************************************************
 * LOCAL VARIABLE
 */

//init ZDO device state
devStates_t devState = DEV_HOLD;
testNode_t testNodes[MAX_TEST_NODES];
uint16_t transId;

char* cDevType;

/***********************************************************************/

void usage(char* exeName)
{
	consolePrint("Usage: ./%s <port>\n", exeName);
	consolePrint("Eample: ./%s /dev/ttyACM0\n", exeName);
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */
//ZDO Callbacks
static uint8_t mtZdoStateChangeIndCb(uint8_t newDevState);
static uint8_t mtZdoEndDeviceAnnceIndCb(EndDeviceAnnceIndFormat_t *msg);

//SYS Callbacks
static uint8_t mtSysResetIndCb(ResetIndFormat_t *msg);

//AF callbacks
static uint8_t mtAfDataConfirmCb(DataConfirmFormat_t *msg);
static uint8_t mtAfIncomingMsgCb(IncomingMsgFormat_t *msg);

//helper functions
static uint8_t setNVStartup(uint8_t startupOption);
static uint8_t setNVChanList(uint32_t chanList);
static uint8_t setNVPanID(uint32_t panId);
static uint8_t setNVDevType(uint8_t devType);
static int32_t startNetwork(char *cDevType, char* sCh);
static int32_t registerAf(void);
static void sendTestMsg(uint16_t nodeAddr, uint8_t txSeqNum);

/*********************************************************************
 * CALLBACK FUNCTIONS
 */

// SYS callbacks
static mtSysCb_t mtSysCb =
	{
	        //mtSysResetInd          //MT_SYS_RESET_IND
	        NULL, NULL, NULL, mtSysResetIndCb, NULL, NULL, NULL, NULL, NULL,
	        NULL, NULL, NULL, NULL, NULL };

static mtZdoCb_t mtZdoCb =
	{ NULL,       // MT_ZDO_NWK_ADDR_RSP
	        NULL,      // MT_ZDO_IEEE_ADDR_RSP
	        NULL,      // MT_ZDO_NODE_DESC_RSP
	        NULL,     // MT_ZDO_POWER_DESC_RSP
	        NULL,    // MT_ZDO_SIMPLE_DESC_RSP
	        NULL,      // MT_ZDO_ACTIVE_EP_RSP
	        NULL,     // MT_ZDO_MATCH_DESC_RSP
	        NULL,   // MT_ZDO_COMPLEX_DESC_RSP
	        NULL,      // MT_ZDO_USER_DESC_RSP
	        NULL,     // MT_ZDO_USER_DESC_CONF
	        NULL,    // MT_ZDO_SERVER_DISC_RSP
	        NULL, // MT_ZDO_END_DEVICE_BIND_RSP
	        NULL,          // MT_ZDO_BIND_RSP
	        NULL,        // MT_ZDO_UNBIND_RSP
	        NULL,   // MT_ZDO_MGMT_NWK_DISC_RSP
	        NULL,       // MT_ZDO_MGMT_LQI_RSP
	        NULL,       // MT_ZDO_MGMT_RTG_RSP
	        NULL,      // MT_ZDO_MGMT_BIND_RSP
	        NULL,     // MT_ZDO_MGMT_LEAVE_RSP
	        NULL,     // MT_ZDO_MGMT_DIRECT_JOIN_RSP
	        NULL,     // MT_ZDO_MGMT_PERMIT_JOIN_RSP
	        mtZdoStateChangeIndCb,   // MT_ZDO_STATE_CHANGE_IND
	        mtZdoEndDeviceAnnceIndCb,   // MT_ZDO_END_DEVICE_ANNCE_IND
	        NULL,        // MT_ZDO_SRC_RTG_IND
	        NULL,	 //MT_ZDO_BEACON_NOTIFY_IND
	        NULL,			 //MT_ZDO_JOIN_CNF
	        NULL,	 //MT_ZDO_NWK_DISCOVERY_CNF
	        NULL,                    // MT_ZDO_CONCENTRATOR_IND_CB
	        NULL,         // MT_ZDO_LEAVE_IND
	        NULL,   //MT_ZDO_STATUS_ERROR_RSP
	        NULL,  //MT_ZDO_MATCH_DESC_RSP_SENT
	        NULL, NULL };

static mtAfCb_t mtAfCb =
	{ mtAfDataConfirmCb,				//MT_AF_DATA_CONFIRM
	        mtAfIncomingMsgCb,				//MT_AF_INCOMING_MSG
	        NULL,				//MT_AF_INCOMING_MSG_EXT
	        NULL,			//MT_AF_DATA_RETRIEVE
	        NULL,			    //MT_AF_REFLECT_ERROR
	    };

/********************************************************************
 * START OF SYS CALL BACK FUNCTIONS
 */
static uint8_t mtSysResetIndCb(ResetIndFormat_t *msg)
{
	consolePrint("ZNP Version: %d.%d.%d\n", msg->MajorRel, msg->MinorRel,
	        msg->HwRev);
	return 0;
}

/********************************************************************
 * START OF ZDO CALL BACK FUNCTIONS
 */

/********************************************************************
 * @fn     Callback function for ZDO State Change Indication

 * @brief  receives the AREQ status and specifies the change ZDO state
 *
 * @param  uint8 zdoState
 *
 * @return SUCCESS or FAILURE
 */

static uint8_t mtZdoStateChangeIndCb(uint8_t newDevState)
{
	switch (newDevState)
	{
	case DEV_HOLD:
		dbg_print(PRINT_LEVEL_INFO,
		        "mtZdoStateChangeIndCb: Initialized - not started automatically\n");
		break;
	case DEV_INIT:
		dbg_print(PRINT_LEVEL_INFO,
		        "mtZdoStateChangeIndCb: Initialized - not connected to anything\n");
		break;
	case DEV_NWK_DISC:
		dbg_print(PRINT_LEVEL_INFO,
		        "mtZdoStateChangeIndCb: Discovering PAN's to join\n");
		consolePrint("Network Discovering\n");
		break;
	case DEV_NWK_JOINING:
		dbg_print(PRINT_LEVEL_INFO, "mtZdoStateChangeIndCb: Joining a PAN\n");
		consolePrint("Network Joining\n");
		break;
	case DEV_NWK_REJOIN:
		dbg_print(PRINT_LEVEL_INFO,
		        "mtZdoStateChangeIndCb: ReJoining a PAN, only for end devices\n");
		consolePrint("Network Rejoining\n");
		break;
	case DEV_END_DEVICE_UNAUTH:
		consolePrint("Network Authenticating\n");
		dbg_print(PRINT_LEVEL_INFO,
		        "mtZdoStateChangeIndCb: Joined but not yet authenticated by trust center\n");
		break;
	case DEV_END_DEVICE:
		consolePrint("Network Joined\n");
		dbg_print(PRINT_LEVEL_INFO,
		        "mtZdoStateChangeIndCb: Started as device after authentication\n");
		break;
	case DEV_ROUTER:
		consolePrint("Network Joined\n");
		dbg_print(PRINT_LEVEL_INFO,
		        "mtZdoStateChangeIndCb: Device joined, authenticated and is a router\n");
		break;
	case DEV_COORD_STARTING:
		consolePrint("Network Starting\n");
		dbg_print(PRINT_LEVEL_INFO,
		        "mtZdoStateChangeIndCb: Started as Zigbee Coordinator\n");
		break;
	case DEV_ZB_COORD:
		consolePrint("Network Started\n");
		dbg_print(PRINT_LEVEL_INFO,
		        "mtZdoStateChangeIndCb: Started as Zigbee Coordinator\n");
		break;
	case DEV_NWK_ORPHAN:
		consolePrint("Network Orphaned\n");
		dbg_print(PRINT_LEVEL_INFO,
		        "mtZdoStateChangeIndCb: Device has lost information about its parent\n");
		break;
	default:
		dbg_print(PRINT_LEVEL_INFO, "mtZdoStateChangeIndCb: unknown state");
		break;
	}

	devState = (devStates_t) newDevState;

	return SUCCESS;
}

static uint8_t mtZdoEndDeviceAnnceIndCb(EndDeviceAnnceIndFormat_t *msg)
{
	uint32_t nodeIdx;
	consolePrint("\nNew device joined network.\n");

	//find empty slot
	for (nodeIdx = 0; nodeIdx < MAX_TEST_NODES; nodeIdx++)
	{
		if (testNodes[nodeIdx].nodeAddr == 0)
		{
			consolePrint("found new test node: %04x \n", msg->NwkAddr);
			testNodes[nodeIdx].txSeqNum = 0;
			testNodes[nodeIdx].rxSeqNum = 0;
			testNodes[nodeIdx].nodeAddr = msg->NwkAddr;
			break;
		}
	}

	return 0;
}

/********************************************************************
 * AF CALL BACK FUNCTIONS
 */

static uint8_t mtAfDataConfirmCb(DataConfirmFormat_t *msg)
{

	if (msg->Status == MT_RPC_SUCCESS)
	{
		dbg_print(PRINT_LEVEL_INFO, "Message transmited Succesfully!\n");
	}
	else
	{
		dbg_print(PRINT_LEVEL_WARNING, "Message failed to transmit\n");
	}
	return msg->Status;
}

static uint8_t mtAfIncomingMsgCb(IncomingMsgFormat_t *msg)
{
	uint32_t nodeIdx;

	dbg_print(PRINT_LEVEL_INFO, "Incoming message\n");

	//make sure it was a test packet
	if ((msg->ClusterId == TEST_CLUSTER) && (msg->Len == 1))
	{
		if ((cDevType[0] == 'c') || (cDevType[0] == 'C'))
		{
			//find node in list and update rxSeq
			for (nodeIdx = 0; nodeIdx < MAX_TEST_NODES; nodeIdx++)
			{
				if (testNodes[nodeIdx].nodeAddr == msg->SrcAddr)
				{
					dbg_print(PRINT_LEVEL_INFO,
					        "test message ack recived from node: %04x, seq: %04x\n",
					        msg->SrcAddr, msg->Data[0]);
					testNodes[nodeIdx].rxSeqNum = (uint32_t)(msg->Data[0]);
					break;
				}
			}
		}
		else
		{
			dbg_print(PRINT_LEVEL_INFO,
			        "rx'ed test packet from: %04x seq: %04x\n", msg->SrcAddr,
			        msg->Data[0]);
			consolePrint("Sent node %04x seq: %04x\n", msg->SrcAddr,
			        msg->Data[0]);
			//echo back the data
			uint16_t nodeAddr = msg->SrcAddr;
			sendTestMsg(nodeAddr, (uint32_t)(msg->Data[0]));
		}
	}

	return 0;
}

/********************************************************************
 * HELPER FUNCTIONS
 */
// helper functions for building and sending the NV messages
static uint8_t setNVStartup(uint8_t startupOption)
{
	uint8_t status;
	OsalNvWriteFormat_t nvWrite;
	// sending startup option

	nvWrite.Id = ZCD_NV_STARTUP_OPTION;
	nvWrite.Offset = 0;
	nvWrite.Len = 1;
	nvWrite.Value[0] = startupOption;
	status = sysOsalNvWrite(&nvWrite);
//	status = sysNvWrite(ZCD_NV_STARTUP_OPTION, 0, pbuf, 1);
	dbg_print(PRINT_LEVEL_INFO, "\n");

	dbg_print(PRINT_LEVEL_INFO, "NV Write Startup Option cmd sent[%d]...\n",
	        status);

	return status;
}

static uint8_t setNVDevType(uint8_t devType)
{
	uint8_t status;
	OsalNvWriteFormat_t nvWrite;
	//uint8_t pbuf[1];
	// setting dev type
	nvWrite.Id = ZCD_NV_LOGICAL_TYPE;
	nvWrite.Offset = 0;
	nvWrite.Len = 1;
	nvWrite.Value[0] = devType;
	status = sysOsalNvWrite(&nvWrite);
	//pbuf[0] = devType;
	//status = sysNvWrite(ZCD_NV_LOGICAL_TYPE, 0, pbuf, 1);
	dbg_print(PRINT_LEVEL_INFO, "\n");
	dbg_print(PRINT_LEVEL_INFO, "NV Write Device Type cmd sent... [%d]\n",
	        status);

	return status;
}

static uint8_t setNVPanID(uint32_t panId)
{
	uint8_t status;
	OsalNvWriteFormat_t nvWrite;

	dbg_print(PRINT_LEVEL_INFO, "\n");
	dbg_print(PRINT_LEVEL_INFO, "NV Write PAN ID cmd sending...\n");

	nvWrite.Id = ZCD_NV_PANID;
	nvWrite.Offset = 0;
	nvWrite.Len = 2;
	nvWrite.Value[0] = LO_UINT16(panId);
	nvWrite.Value[1] = HI_UINT16(panId);
	status = sysOsalNvWrite(&nvWrite);
	//status = sysNvWrite(ZCD_NV_PANID, 0, pbuf, 2);
	dbg_print(PRINT_LEVEL_INFO, "\n");
	dbg_print(PRINT_LEVEL_INFO, "NV Write PAN ID cmd sent...[%d]\n", status);

	return status;
}

static uint8_t setNVChanList(uint32_t chanList)
{
	OsalNvWriteFormat_t nvWrite;
	uint8_t status;
	// setting chanList
	nvWrite.Id = ZCD_NV_CHANLIST;
	nvWrite.Offset = 0;
	nvWrite.Len = 4;
	nvWrite.Value[0] = BREAK_UINT32(chanList, 0);
	nvWrite.Value[1] = BREAK_UINT32(chanList, 1);
	nvWrite.Value[2] = BREAK_UINT32(chanList, 2);
	nvWrite.Value[3] = BREAK_UINT32(chanList, 3);
	status = sysOsalNvWrite(&nvWrite);
	//status = sysNvWrite(ZCD_NV_CHANLIST, 0, pbuf, 4);
	dbg_print(PRINT_LEVEL_INFO, "\n");
	dbg_print(PRINT_LEVEL_INFO, "NV Write Channel List cmd sent...[%d]\n",
	        status);

	return status;
}

static int32_t startNetwork(char *cDevType, char* sCh)
{
	uint8_t devType;
	int32_t status;

	status = setNVStartup(
	ZCD_STARTOPT_CLEAR_STATE | ZCD_STARTOPT_CLEAR_CONFIG);
	consolePrint("Resetting ZNP\n");
	ResetReqFormat_t resReq;
	resReq.Type = 1;
	sysResetReq(&resReq);
	//flush the rsp
	rpcWaitMqClientMsg(5000);

	if (status != MT_RPC_SUCCESS)
	{
		dbg_print(PRINT_LEVEL_WARNING, "network start failed\n");
		return 0;
	}

	switch (cDevType[0])
	{
	case 'c':
	case 'C':
		devType = DEVICETYPE_COORDINATOR;
		break;
	case 'r':
	case 'R':
		devType = DEVICETYPE_ROUTER;
		break;
	case 'e':
	case 'E':
	default:
		devType = DEVICETYPE_ENDDEVICE;
		break;
	}
	status = setNVDevType(devType);

	if (status != MT_RPC_SUCCESS)
	{
		dbg_print(PRINT_LEVEL_WARNING, "setNVDevType failed\n");
		return 0;
	}
	//Select random PAN ID for Coord and join any PAN for RTR/ED
	status = setNVPanID(0xFFFF);
	if (status != MT_RPC_SUCCESS)
	{
		dbg_print(PRINT_LEVEL_WARNING, "setNVPanID failed\n");
		return 0;
	}

	status = setNVChanList(1 << atoi(sCh));
	if (status != MT_RPC_SUCCESS)
	{
		dbg_print(PRINT_LEVEL_INFO, "setNVPanID failed\n");
		return 0;
	}

	registerAf();
	consolePrint("EndPoint: 1\n");

	status = zdoInit();
	if (status == NEW_NETWORK)
	{
		dbg_print(PRINT_LEVEL_INFO, "zdoInit NEW_NETWORK\n");
		status = MT_RPC_SUCCESS;
	}
	else if (status == RESTORED_NETWORK)
	{
		dbg_print(PRINT_LEVEL_INFO, "zdoInit RESTORED_NETWORK\n");
		consolePrint("Network Restored\n");
		status = MT_RPC_SUCCESS;
	}
	else
	{
		dbg_print(PRINT_LEVEL_INFO, "zdoInit failed\n");
		status = -1;
	}

	dbg_print(PRINT_LEVEL_INFO, "process zdoStatechange callbacks\n");

	//flush AREQ ZDO State Change messages
	while (status != -1)
	{
		status = rpcWaitMqClientMsg(5000);

		if (((devType == DEVICETYPE_COORDINATOR) && (devState == DEV_ZB_COORD))
		        || ((devType == DEVICETYPE_ROUTER) && (devState == DEV_ROUTER))
		        || ((devType == DEVICETYPE_ENDDEVICE)
		                && (devState == DEV_END_DEVICE)))
		{
			break;
		}
	}

	if (devState < DEV_END_DEVICE)
	{
		//start network failed
		return -1;
	}
	else
	{
		//set startup option back to keep configuration in case of reset
		status = setNVStartup(0);
		if (status != MT_RPC_SUCCESS)
		{
			dbg_print(PRINT_LEVEL_WARNING, "setNVStartup failed\n");
			return -1;
		}

	}

	return 0;
}

static int32_t registerAf(void)
{
	int32_t status = 0;
	RegisterFormat_t reg;

	reg.EndPoint = TEST_EP;
	reg.AppProfId = TEST_PRIFILE;
	reg.AppDeviceId = 0x0;
	reg.AppDevVer = 1;
	reg.LatencyReq = 0;
	reg.AppNumInClusters = 1;
	reg.AppInClusterList[0] = TEST_CLUSTER;
	reg.AppNumOutClusters = 1;
	reg.AppOutClusterList[0] = TEST_CLUSTER;

	status = afRegister(&reg);
	return status;
}

static void sendTestMsg(uint16_t nodeAddr, uint8_t txSeqNum)
{
	DataRequestFormat_t DataRequest;
	DataRequest.DstAddr = nodeAddr;
	DataRequest.DstEndpoint = TEST_EP;
	DataRequest.SrcEndpoint = TEST_EP;
	DataRequest.ClusterID = TEST_CLUSTER;
	DataRequest.TransID = transId++;
	DataRequest.Options = 0;
	DataRequest.Radius = 16;

	//memcpy(DataRequest.Data, &txSeqNum, 1);
	DataRequest.Data[0] = txSeqNum;
	DataRequest.Len = 1;

	afDataRequest(&DataRequest);
}

/*********************************************************************
 * INTERFACE FUNCTIONS
 */
uint32_t appInit(void)
{
	int32_t status = 0;
	uint32_t msgCnt = 0;

	//Flush all messages from the que
	while (status != -1)
	{
		status = rpcWaitMqClientMsg(10);
		if (status != -1)
		{
			msgCnt++;
		}
	}

	dbg_print(PRINT_LEVEL_INFO, "flushed %d message from msg queue\n", msgCnt);

	//Register Callbacks MT system callbacks
	sysRegisterCallbacks(mtSysCb);
	zdoRegisterCallbacks(mtZdoCb);
	afRegisterCallbacks(mtAfCb);

	//clear the node test addrs
	memset(testNodes, 0, sizeof(testNodes));

	return 0;
}

uint8_t initDone = 0;
void* appMsgProcess(void *argument)
{
	if (initDone)
	{
		rpcWaitMqClientMsg(100);
	}

	return 0;
}

void* appProcess(void *context)
{
	int32_t status;
	char** args = (char**) context;
	char* sCh = args[1];
	cDevType = args[0];
	struct timespec req;
	struct timespec rem;

	//Flush all messages from the que
	do
	{
		status = rpcWaitMqClientMsg(50);
	} while (status != -1);

	devState = DEV_HOLD;

	status = startNetwork(cDevType, sCh);
	if (status != -1)
	{
		consolePrint("Network up\n\n");
	}
	else
	{
		consolePrint("Network Error\n\n");
		return 0;
	}

	sysGetExtAddr();

	OsalNvWriteFormat_t nvWrite;
	nvWrite.Id = ZCD_NV_ZDO_DIRECT_CB;
	nvWrite.Offset = 0;
	nvWrite.Len = 1;
	nvWrite.Value[0] = TEST_EP;
	status = sysOsalNvWrite(&nvWrite);

	if ((cDevType[0] == 'c') || (cDevType[0] == 'C'))
	{
		consolePrint("Waiting for test nodes to join the network\n");
		initDone = 1;
	}

	while (1)
	{
		if ((cDevType[0] == 'c') || (cDevType[0] == 'C'))
		{
			uint32_t nodeIdx;

			for (nodeIdx = 0; nodeIdx < MAX_TEST_NODES; nodeIdx++)
			{
				if (testNodes[nodeIdx].nodeAddr != 0)
				{
					req.tv_sec = 0;
					req.tv_nsec = 500000000;

					if ((testNodes[nodeIdx].txSeqNum)
					        != testNodes[nodeIdx].rxSeqNum)
					{
						//error message lost
						dbg_print(PRINT_LEVEL_WARNING,
						        "Error: node %x sequence numbers not matching %x:%x \n",
						        testNodes[nodeIdx].nodeAddr,
						        testNodes[nodeIdx].txSeqNum,
						        testNodes[nodeIdx].rxSeqNum);

						//reset conters
						testNodes[nodeIdx].txSeqNum = 0;
						testNodes[nodeIdx].rxSeqNum = 0;
						testNodes[nodeIdx].errorCnt++;
					}
					else
					{
						testNodes[nodeIdx].passCnt++;
					}
					testNodes[nodeIdx].txSeqNum++;

					dbg_print(PRINT_LEVEL_INFO,
					        "sending node %04x test message %x \n",
					        testNodes[nodeIdx].nodeAddr,
					        testNodes[nodeIdx].txSeqNum);

					consolePrint("node:%04x: pass count:%d error count:%d\n",
					        testNodes[nodeIdx].nodeAddr,
					        testNodes[nodeIdx].passCnt,
					        testNodes[nodeIdx].errorCnt);

					sendTestMsg(testNodes[nodeIdx].nodeAddr,
					        testNodes[nodeIdx].txSeqNum);

					nanosleep(&req, &rem);
				}
			}
		}

		//let other task get context if no devices are found
		//Set sleep time to 1ms as this is the tick period on Tiva
		req.tv_sec = 0;
		req.tv_nsec = 1000000;
		nanosleep(&req, &rem);

		if ((cDevType[0] != 'c') && (cDevType[0] != 'C'))
		{
			rpcWaitMqClientMsg(1000);
		}
	}

	return 0;
}

