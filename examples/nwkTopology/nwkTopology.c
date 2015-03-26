/**************************************************************************************************
 * Filename:       nwkTopology.c
 * Description:    This file contains nwkTopology application.
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
#include "mtSapi.h"
#include "rpcTransport.h"
#include "dbgPrint.h"
#include "hostConsole.h"

/*********************************************************************
 * MACROS
 */

#define MAX_CHILDREN 20
#define MAX_NODE_LIST 64

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
static uint8_t mtZdoMgmtLqiRspCb(MgmtLqiRspFormat_t *msg);

//SYS Callbacks

static uint8_t mtSysResetIndCb(ResetIndFormat_t *msg);

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
	        mtZdoMgmtLqiRspCb,       // MT_ZDO_MGMT_LQI_RSP
	        NULL,       // MT_ZDO_MGMT_RTG_RSP
	        NULL,      // MT_ZDO_MGMT_BIND_RSP
	        NULL,     // MT_ZDO_MGMT_LEAVE_RSP
	        NULL,     // MT_ZDO_MGMT_DIRECT_JOIN_RSP
	        NULL,     // MT_ZDO_MGMT_PERMIT_JOIN_RSP
	        mtZdoStateChangeIndCb,   // MT_ZDO_STATE_CHANGE_IND
	        NULL,   // MT_ZDO_END_DEVICE_ANNCE_IND
	        NULL,        // MT_ZDO_SRC_RTG_IND
	        NULL,	 //MT_ZDO_BEACON_NOTIFY_IND
	        NULL,			 //MT_ZDO_JOIN_CNF
	        NULL,	 //MT_ZDO_NWK_DISCOVERY_CNF
	        NULL,                    // MT_ZDO_CONCENTRATOR_IND_CB
	        NULL,         // MT_ZDO_LEAVE_IND
	        NULL,   //MT_ZDO_STATUS_ERROR_RSP
	        NULL,  //MT_ZDO_MATCH_DESC_RSP_SENT
	        NULL, NULL };

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
	ChildNode_t childs[MAX_CHILDREN];
} Node_t;

Node_t nodeList[MAX_NODE_LIST];
uint8_t nodeCount = 0;
static uint8_t mtSysResetIndCb(ResetIndFormat_t *msg)
{

	consolePrint("ZNP Version: %d.%d.%d\n", msg->MajorRel, msg->MinorRel,
	        msg->HwRev);
	return 0;
}

/********************************************************************
 * START OF ZDO CALL BACK FUNCTIONS
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

static uint8_t mtZdoMgmtLqiRspCb(MgmtLqiRspFormat_t *msg)
{
	uint8_t devType = 0;
	uint8_t devRelation = 0;
	uint8_t localNodeCount = nodeCount;
	MgmtLqiReqFormat_t req;

	if (msg->Status == MT_RPC_SUCCESS)
	{
		nodeCount++;
		nodeList[localNodeCount].NodeAddr = msg->SrcAddr;
		nodeList[localNodeCount].Type = (msg->SrcAddr == 0 ?
		DEVICETYPE_COORDINATOR :
		                                                DEVICETYPE_ROUTER);
		nodeList[localNodeCount].ChildCount = 0;
		uint32_t i;
		for (i = 0; i < msg->NeighborLqiListCount; i++)
		{
			devType = msg->NeighborLqiList[i].DevTyp_RxOnWhenIdle_Relat & 3;
			devRelation = ((msg->NeighborLqiList[i].DevTyp_RxOnWhenIdle_Relat
			        >> 4) & 7);
			if (devRelation == 1)
			{
				uint8_t cCount = nodeList[localNodeCount].ChildCount;
				nodeList[localNodeCount].childs[cCount].ChildAddr =
				        msg->NeighborLqiList[i].NetworkAddress;
				nodeList[localNodeCount].childs[cCount].Type = devType;
				nodeList[localNodeCount].ChildCount++;
				if (devType == DEVICETYPE_ROUTER)
				{
					req.DstAddr = msg->NeighborLqiList[i].NetworkAddress;
					req.StartIndex = 0;
					zdoMgmtLqiReq(&req);
				}
			}
		}
	}
	else
	{
		consolePrint("MgmtLqiRsp Status: FAIL 0x%02X\n", msg->Status);
	}

	return msg->Status;
}

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

	return 0;
}

void* appProcess(void *argument)
{
	int32_t status = 0;
	//Flush all messages from the que
	while (status != -1)
	{
		status = rpcWaitMqClientMsg(50);
	}
	//init variable
	devState = DEV_HOLD;
	gSrcEndPoint = 1;
	gDstEndPoint = 1;

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
	status = 0;
	char cmd[128];
	MgmtLqiReqFormat_t req;
	req.DstAddr = 0;
	req.StartIndex = 0;
	while (1)
	{
		consolePrint("Press Enter to discover Network Topology:\n");

		consoleGetLine(cmd, 128);
		nodeCount = 0;

		zdoMgmtLqiReq(&req);
		while (status != -1)
		{
			status = rpcWaitMqClientMsg(1000);
		}
		status = 0;
		uint8_t i;
		for (i = 0; i < nodeCount; i++)
		{
			char *devtype = (
			        nodeList[i].Type == DEVICETYPE_ROUTER ?
			                "ROUTER" : "END DEVICE");
			if (nodeList[i].Type == DEVICETYPE_COORDINATOR)
			{
				devtype = "COORDINATOR";
			}
			consolePrint("Node Address: 0x%04X   Type: %s\n",
			        nodeList[i].NodeAddr, devtype);

			consolePrint("Children: %d\n", nodeList[i].ChildCount);
			uint8_t cI;
			for (cI = 0; cI < nodeList[i].ChildCount; cI++)
			{
				uint8_t type = nodeList[i].childs[cI].Type;
				consolePrint("\tAddress: 0x%04X   Type: %s\n",
				        nodeList[i].childs[cI].ChildAddr,
				        (type == DEVICETYPE_ROUTER ? "ROUTER" : "END DEVICE"));
			}
			consolePrint("\n");
		}
	}
	return 0;
}

