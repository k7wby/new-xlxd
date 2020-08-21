//
//  cg3protocol.h
//  xlxd
//
//  Created by Marius Petrescu (YO2LOJ) on 03/06/2019.
//  Copyright © 2019 Marius Petrescu (YO2LOJ). All rights reserved.
//
// ----------------------------------------------------------------------------
//    This file is part of xlxd.
//
//    xlxd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    xlxd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef cg3protocol_h
#define cg3protocol_h

#include <string>
#include "ctimepoint.h"
#include "cprotocol.h"
#include "cdvheaderpacket.h"
#include "cdvframepacket.h"
#include "cdvlastframepacket.h"
#include "crawsocket.h"
#include "cudpmsgsocket.h"

////////////////////////////////////////////////////////////////////////////////////////

// note on the G3 terminal/AP protocol:
//
// There are 3 steps in handling an incoming connection
//
// 1 - Notification of terminal call on port UDP 12346, Presence port, aCUdpSocket.
//      - Call will be rejected if in blacklisted
//
// 2 - Destination request on port UDP 12345, Config port, a CUdpMsgSocket.
//      - Calls to specific callsigns will be accepted for a default module
//      - Repeater calls will be accepted for local modules
//      - All other calls are rejected
//
// 3 - Actual D-star flow like in Dextra to/from port 40000
//       2 distinct sockets where used in the initial protocol
//       later firmwares implement a single bidirectional socket
//
// Alive monitoring is done via a "PING" to remote port 40000. We will get an
// ICMP unreachable on terminal mode close or on station shut down if routing is done
// correctly. Otherwise a long timeout is used (e.g. 1h)


////////////////////////////////////////////////////////////////////////////////////////
// class

class CG3Protocol : public CProtocol
{
public:
	// constructor
	CG3Protocol() : m_GwAddress(0u), m_Modules("*"), m_LastModTime(0) {}

	// initialization
	bool Initialize(const char *type, const int ptype, const uint16 port, const bool has_ipv4, const bool has_ipv6);

	// close
	void Close(void);

	// task
	void Task(void);

protected:
	// threads
	void PresenceThread(void);
	void ConfigThread(void);
	void IcmpThread(void);

	// helper tasks
	void PresenceTask(void);
	void ConfigTask(void);
	void IcmpTask(void);

	// config
	void ReadOptions(void);

	// helper
	char *TrimWhiteSpaces(char *);
	void NeedReload(void);

	// queue helper
	void HandleQueue(void);

	// keepalive helpers
	void HandleKeepalives(void);

	// stream helpers
	void OnDvHeaderPacketIn(std::unique_ptr<CDvHeaderPacket> &, const CIp &);

	// packet decoding helpers
	bool IsValidDvHeaderPacket(const CBuffer &, std::unique_ptr<CDvHeaderPacket> &);
	bool IsValidDvFramePacket(const CBuffer &, std::unique_ptr<CDvFramePacket> &);
	bool IsValidDvLastFramePacket(const CBuffer &, std::unique_ptr<CDvLastFramePacket> &);

	// packet encoding helpers
	bool EncodeDvHeaderPacket(const CDvHeaderPacket &, CBuffer *) const;
	bool EncodeDvFramePacket(const CDvFramePacket &, CBuffer *) const;
	bool EncodeDvLastFramePacket(const CDvLastFramePacket &, CBuffer *) const;

protected:
	std::future<void> m_PresenceFuture, m_ConfigFuture, m_IcmpFuture;

	// time
	CTimePoint          m_LastKeepaliveTime;

	// sockets
	CUdpSocket          m_PresenceSocket;
	CUdpMsgSocket       m_ConfigSocket;
	CRawSocket          m_IcmpRawSocket;

	// optional params
	uint32              m_GwAddress;
	std::string         m_Modules;
	time_t              m_LastModTime;
};

////////////////////////////////////////////////////////////////////////////////////////
#endif /* cg3protocol_h */
