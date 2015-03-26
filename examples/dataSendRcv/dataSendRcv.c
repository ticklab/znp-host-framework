/**************************************************************************************************
 * Filename:       dataSendRcv.c
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

/*********************************************************************
 * TYPES
 */

/*********************************************************************
 * LOCAL VARIABLE
 */

//init ZDO device state
devStates_t devState = DEV_HOLD;
uint8_t gSrcEndPoint = 1;
uint8_t gDstEndPoint = 1;

/***********************************************************************/

/*********************************************************************
 * LOCAL FUNCTIONS
 */
//ZDO Callbacks
static uint8_t mtZdoStateChangeIndCb(uint8_t newDevState);
static uint8_t mtZdoSimpleDescRspCb(SimpleDescRspFormat_t *msg);
static uint8_t mtZdoActiveEpRspCb(ActiveEpRspFormat_t *msg);
static uint8_t mtZdoEndDeviceAnnceIndCb(EndDeviceAnnceIndFormat_t *msg);

static uint8_t mtZdoMgmtLqiRspCb(MgmtLqiRspFormat_t *msg);

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
static int32_t startNetwork(void);
static int32_t registerAf(void);

/*********************************************************************
 * CALLBACK FUNCTIONS
 */

// SYS callbacks
static mtSysCb_t mtSysCb =
	{ NULL, NULL, NULL, mtSysResetIndCb, NULL, NULL, NULL, NULL, NULL, NULL,
	        NULL, NULL, NULL, NULL };

static mtZdoCb_t mtZdoCb =
	{ NULL,       // MT_ZDO_NWK_ADDR_RSP
	        NULL,      // MT_ZDO_IEEE_ADDR_RSP
	        NULL,      // MT_ZDO_NODE_DESC_RSP
	        NULL,     // MT_ZDO_POWER_DESC_RSP
	        mtZdoSimpleDescRspCb,    // MT_ZDO_SIMPLE_DESC_RSP
	        mtZdoActiveEpRspCb,      // MT_ZDO_ACTIVE_EP_RSP
	        NULL,     // MT_ZDO_MATCH_DESC_RSP
	        NULL,   // MT_ZDO_COMPLEX_DESC_RSP
	        NULL,      // MT_ZDO_USER_DESC_RSP
	        NULL,     // MT_ZDO_USER_DESC_CONF
	        NULL,    // MT_ZDO_SERVER_DISC_RSP
	        NULL, // MT_ZDO_END_DEVICE_BIND_RSP
	        NULL,          // MT_ZDO_BIND_RSP
	        NULL,        // MT_ZDO_UNBIND_RSP
	        NULL,   // MT_ZDO_MGMT_NWK_DISC_RSP
	        mtZdoMgmtLqiRspCb,       // MT_ZDO_MGMT_LQI_RSP
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
typedef struct
{
	uint16_t ChildAddr;
	uint8_t Type;

} ChildNode_t;

typedef struct
{
	uint16_t NodeAddr;
	uint8_t Type;
	uint8_t ChildCount;
	ChildNode_t childs[256];
} Node_t;

Node_t nodeList[64];
uint8_t nodeCount = 0;

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
static uint8_t mtZdoSimpleDescRspCb(SimpleDescRspFormat_t *msg)
{

	if (msg->Status == MT_RPC_SUCCESS)
	{
		consolePrint("\tEndpoint: 0x%02X\n", msg->Endpoint);
		consolePrint("\tProfileID: 0x%04X\n", msg->ProfileID);
		consolePrint("\tDeviceID: 0x%04X\n", msg->DeviceID);
		consolePrint("\tDeviceVersion: 0x%02X\n", msg->DeviceVersion);
		consolePrint("\tNumInClusters: %d\n", msg->NumInClusters);
		uint32_t i;
		for (i = 0; i < msg->NumInClusters; i++)
		{
			consolePrint("\t\tInClusterList[%d]: 0x%04X\n", i,
			        msg->InClusterList[i]);
		}
		consolePrint("\tNumOutClusters: %d\n", msg->NumOutClusters);
		for (i = 0; i < msg->NumOutClusters; i++)
		{
			consolePrint("\t\tOutClusterList[%d]: 0x%04X\n", i,
			        msg->OutClusterList[i]);
		}
		consolePrint("\n");
	}
	else
	{
		consolePrint("SimpleDescRsp Status: FAIL 0x%02X\n", msg->Status);
	}

	return msg->Status;
}

static uint8_t mtZdoMgmtLqiRspCb(MgmtLqiRspFormat_t *msg)
{
	uint8_t devType = 0;
	uint8_t devRelation = 0;
	MgmtLqiReqFormat_t req;
	if (msg->Status == MT_RPC_SUCCESS)
	{
		nodeList[nodeCount].NodeAddr = msg->SrcAddr;
		nodeList[nodeCount].Type = (msg->SrcAddr == 0 ?
		DEVICETYPE_COORDINATOR :
		                                                DEVICETYPE_ROUTER);
		nodeList[nodeCount].ChildCount = 0;
		uint32_t i;
		for (i = 0; i < msg->NeighborLqiListCount; i++)
		{
			devType = msg->NeighborLqiList[i].DevTyp_RxOnWhenIdle_Relat & 3;
			devRelation = ((msg->NeighborLqiList[i].DevTyp_RxOnWhenIdle_Relat
			        >> 4) & 7);
			if (devRelation == 1 || devRelation == 3)
			{
				uint8_t cCount = nodeList[nodeCount].ChildCount;
				nodeList[nodeCount].childs[cCount].ChildAddr =
				        msg->NeighborLqiList[i].NetworkAddress;
				nodeList[nodeCount].childs[cCount].Type = devType;
				nodeList[nodeCount].ChildCount++;
				if (devType == DEVICETYPE_ROUTER)
				{
					req.DstAddr = msg->NeighborLqiList[i].NetworkAddress;
					req.StartIndex = 0;
					zdoMgmtLqiReq(&req);
				}
			}
		}
		nodeCount++;

	}
	else
	{
		consolePrint("MgmtLqiRsp Status: FAIL 0x%02X\n", msg->Status);
	}

	return msg->Status;
}

static uint8_t mtZdoActiveEpRspCb(ActiveEpRspFormat_t *msg)
{

	//SimpleDescReqFormat_t simReq;
	consolePrint("NwkAddr: 0x%04X\n", msg->NwkAddr);
	if (msg->Status == MT_RPC_SUCCESS)
	{
		consolePrint("Number of Endpoints: %d\nActive Endpoints: ",
		        msg->ActiveEPCount);
		uint32_t i;
		for (i = 0; i < msg->ActiveEPCount; i++)
		{
			consolePrint("0x%02X\t", msg->ActiveEPList[i]);

		}
		consolePrint("\n");
	}
	else
	{
		consolePrint("ActiveEpRsp Status: FAIL 0x%02X\n", msg->Status);
	}

	return msg->Status;
}

static uint8_t mtZdoEndDeviceAnnceIndCb(EndDeviceAnnceIndFormat_t *msg)
{

	ActiveEpReqFormat_t actReq;
	actReq.DstAddr = msg->NwkAddr;
	actReq.NwkAddrOfInterest = msg->NwkAddr;

	consolePrint("\nNew device joined network.\n");
	zdoActiveEpReq(&actReq);
	return 0;
}

/********************************************************************
 * AF CALL BACK FUNCTIONS
 */

static uint8_t mtAfDataConfirmCb(DataConfirmFormat_t *msg)
{

	if (msg->Status == MT_RPC_SUCCESS)
	{
		consolePrint("Message transmited Succesfully!\n");
	}
	else
	{
		consolePrint("Message failed to transmit\n");
	}
	return msg->Status;
}
static uint8_t mtAfIncomingMsgCb(IncomingMsgFormat_t *msg)
{

	consolePrint(
	        "\nIncoming Message from Endpoint 0x%02X and Address 0x%04X:\n",
	        msg->SrcEndpoint, msg->SrcAddr);
	msg->Data[msg->Len] = '\0';
	consolePrint("%s\n", (char*) msg->Data);
	consolePrint(
	        "\nEnter message to send or type CHANGE to change the destination \nor QUIT to exit:\n");

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

	dbg_print(PRINT_LEVEL_INFO, "\n");

	dbg_print(PRINT_LEVEL_INFO, "NV Write Startup Option cmd sent[%d]...\n",
	        status);

	return status;
}

static uint8_t setNVDevType(uint8_t devType)
{
	uint8_t status;
	OsalNvWriteFormat_t nvWrite;

	// setting dev type
	nvWrite.Id = ZCD_NV_LOGICAL_TYPE;
	nvWrite.Offset = 0;
	nvWrite.Len = 1;
	nvWrite.Value[0] = devType;
	status = sysOsalNvWrite(&nvWrite);

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

	dbg_print(PRINT_LEVEL_INFO, "\n");
	dbg_print(PRINT_LEVEL_INFO, "NV Write Channel List cmd sent...[%d]\n",
	        status);

	return status;
}

uint8_t dType;
static int32_t startNetwork(void)
{
	char cDevType;
	uint8_t devType;
	int32_t status;
	uint8_t newNwk = 0;
	char sCh[128];

	do
	{
		consolePrint("Do you wish to start/join a new network? (y/n)\n");
		consoleGetLine(sCh, 128);
		if (sCh[0] == 'n' || sCh[0] == 'N')
		{
			status = setNVStartup(0);
		}
		else if (sCh[0] == 'y' || sCh[0] == 'Y')
		{
			status = setNVStartup(
			ZCD_STARTOPT_CLEAR_STATE | ZCD_STARTOPT_CLEAR_CONFIG);
			newNwk = 1;

		}
		else
		{
			consolePrint("Incorrect input please type y or n\n");
		}
	} while (sCh[0] != 'y' && sCh[0] != 'Y' && sCh[0] != 'n' && sCh[0] != 'N');

	if (status != MT_RPC_SUCCESS)
	{
		dbg_print(PRINT_LEVEL_WARNING, "network start failed\n");
		return -1;
	}
	consolePrint("Resetting ZNP\n");
	ResetReqFormat_t resReq;
	resReq.Type = 1;
	sysResetReq(&resReq);
	//flush the rsp
	rpcWaitMqClientMsg(5000);

	if (newNwk)
	{
#ifndef CC26xx
		consolePrint(
		        "Enter device type c: Coordinator, r: Router, e: End Device:\n");
		consoleGetLine(sCh, 128);
		cDevType = sCh[0];

		switch (cDevType)
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
#endif //CC26xx
		//Select random PAN ID for Coord and join any PAN for RTR/ED
		status = setNVPanID(0xFFFF);
		if (status != MT_RPC_SUCCESS)
		{
			dbg_print(PRINT_LEVEL_WARNING, "setNVPanID failed\n");
			return -1;
		}
		consolePrint("Enter channel 11-26:\n");
		consoleGetLine(sCh, 128);

		status = setNVChanList(1 << atoi(sCh));
		if (status != MT_RPC_SUCCESS)
		{
			dbg_print(PRINT_LEVEL_INFO, "setNVPanID failed\n");
			return -1;
		}

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
	//set startup option back to keep configuration in case of reset
	status = setNVStartup(0);
	if (devState < DEV_END_DEVICE)
	{
		//start network failed
		return -1;
	}

	return 0;
}
static int32_t registerAf(void)
{
	int32_t status = 0;
	RegisterFormat_t reg;

	reg.EndPoint = 1;
	reg.AppProfId = 0x0104;
	reg.AppDeviceId = 0x0100;
	reg.AppDevVer = 1;
	reg.LatencyReq = 0;
	reg.AppNumInClusters = 1;
	reg.AppInClusterList[0] = 0x0006;
	reg.AppNumOutClusters = 0;

	status = afRegister(&reg);
	return status;
}

static void displayDevices(void)
{
	ActiveEpReqFormat_t actReq;
	int32_t status;

	MgmtLqiReqFormat_t req;

	req.DstAddr = 0;
	req.StartIndex = 0;
	nodeCount = 0;
	zdoMgmtLqiReq(&req);
	do
	{
		status = rpcWaitMqClientMsg(1000);
	} while (status != -1);

	consolePrint("\nAvailable devices:\n");
	uint8_t i;
	for (i = 0; i < nodeCount; i++)
	{
		char *devtype =
		        (nodeList[i].Type == DEVICETYPE_ROUTER ?
		                "ROUTER" : "COORDINATOR");

		consolePrint("Type: %s\n", devtype);
		actReq.DstAddr = nodeList[i].NodeAddr;
		actReq.NwkAddrOfInterest = nodeList[i].NodeAddr;
		zdoActiveEpReq(&actReq);
		rpcGetMqClientMsg();
		do
		{
			status = rpcWaitMqClientMsg(1000);
		} while (status != -1);
		uint8_t cI;
		for (cI = 0; cI < nodeList[i].ChildCount; cI++)
		{
			uint8_t type = nodeList[i].childs[cI].Type;
			if (type == DEVICETYPE_ENDDEVICE)
			{
				consolePrint("Type: END DEVICE\n");
				actReq.DstAddr = nodeList[i].childs[cI].ChildAddr;
				actReq.NwkAddrOfInterest = nodeList[i].childs[cI].ChildAddr;
				zdoActiveEpReq(&actReq);
				status = 0;
				rpcGetMqClientMsg();
				while (status != -1)
				{
					status = rpcWaitMqClientMsg(1000);
				}
			}

		}
		consolePrint("\n");

	}
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

	return 0;
}
uint8_t initDone = 0;
void* appMsgProcess(void *argument)
{

	if (initDone)
	{
		rpcWaitMqClientMsg(10000);
	}

	return 0;
}

void* appProcess(void *argument)
{
	int32_t status;
	uint32_t quit = 0;

	//Flush all messages from the que
	do
	{
		status = rpcWaitMqClientMsg(50);
	} while (status != -1);

	devState = DEV_HOLD;

	status = startNetwork();
	if (status != -1)
	{
		consolePrint("Network up\n\n");
	}
	else
	{
		consolePrint("Network Error\n\n");
	}

	sysGetExtAddr();

	OsalNvWriteFormat_t nvWrite;
	nvWrite.Id = ZCD_NV_ZDO_DIRECT_CB;
	nvWrite.Offset = 0;
	nvWrite.Len = 1;
	nvWrite.Value[0] = 1;
	status = sysOsalNvWrite(&nvWrite);

	char cmd[128];
	int attget;

	while (quit == 0)
	{
		nodeCount = 0;
		initDone = 0;
		displayDevices();
		DataRequestFormat_t DataRequest;
		consolePrint("Enter DstAddr:\n");
		consoleGetLine(cmd, 128);
		sscanf(cmd, "%x", &attget);
		DataRequest.DstAddr = (uint16_t) attget;

		consolePrint("Enter DstEndpoint:\n");
		consoleGetLine(cmd, 128);
		sscanf(cmd, "%x", &attget);
		DataRequest.DstEndpoint = (uint8_t) attget;

		DataRequest.SrcEndpoint = 1;

		DataRequest.ClusterID = 6;

		DataRequest.TransID = 5;

		DataRequest.Options = 0;

		DataRequest.Radius = 0xEE;

		initDone = 1;

		while (1)
		{
			uint8_t *data;
			//initDone = 0;

			consolePrint(
			        "Enter message to send or type CHANGE to change the destination\n");
			consolePrint("or QUIT to exit\n");

			consoleGetLine(cmd, 128);
			//initDone = 1;
			if (strcmp(cmd, "CHANGE") == 0)
			{
				break;
			}
			else if (strcmp(cmd, "QUIT") == 0)
			{
				quit = 1;
				break;
			}
			data = (uint8_t*) cmd;
			memcpy(DataRequest.Data, data, strlen(cmd));
			DataRequest.Len = strlen(cmd);
			initDone = 0;
			afDataRequest(&DataRequest);
			rpcWaitMqClientMsg(500);
			initDone = 1;
		}

	}

	return 0;
}

