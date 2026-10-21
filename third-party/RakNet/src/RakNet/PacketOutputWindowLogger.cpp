#include "NativeFeatureIncludes.hpp"
#if _RAKNET_SUPPORT_PacketLogger==1

#if defined(UNICODE)
#include "RakWString.hpp"
#endif

#include "PacketOutputWindowLogger.hpp"
#include "RakString.hpp"
#if defined(_WIN32) && !defined(X360__) 
#include "WindowsIncludes.hpp"
#endif

using namespace RakNet;

PacketOutputWindowLogger::PacketOutputWindowLogger()
{
}
PacketOutputWindowLogger::~PacketOutputWindowLogger()
{
}
void PacketOutputWindowLogger::WriteLog(const char *str)
{
#if defined(_WIN32) && !defined(X360__) 

	#if defined(UNICODE)
		RakNet::RakWString str2 = str;
		str2+="\n";
		OutputDebugString(str2.C_String());
	#else
		RakNet::RakString str2 = str;
		str2+="\n";
		OutputDebugString(str2.C_String());
	#endif
	
#endif
}

#endif // _RAKNET_SUPPORT_*
