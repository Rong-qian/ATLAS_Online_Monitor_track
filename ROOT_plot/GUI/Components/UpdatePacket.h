#pragma once

#include <string>

struct UpdatePacket {

	int packetCount       = 0 ;
	int totalEventCount   = 0 ;
	int passEventCount    = 0 ;
	int emptyEventCount   = 0 ;
	int errorCount        = 0 ;
	int decodeError       = 0 ;
	int warningCount      = 0 ;
	int lostPackets       = 0 ;
	int droppedSignals    = 0 ;
	int droppedEvents     = 0 ;

	std::string statusTag = "";

};