#include <cstdint>

#define __packed __attribute__((packed))
#include "../paker.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include "telemetry.hpp"

using namespace std;
using namespace Protocol::Paker;

void PrintDiagInfo(TDiagnosticsFrame* pDiag)
{
	if(!pDiag)
		return;

	cout << "frame id: " << (int)pDiag->u3FrameId << endl;
	cout << "telemetry bits: " << (int)pDiag->u5TelemetryBits << endl;
	cout << "voltage: " << pDiag->s16Voltage/1000.0 << " V"<< endl;
	cout << "temperature: " << (int)((signed char)pDiag->s8Temperature) << " C" << endl;
	cout << endl;
}

void PrintPositionInfo(TPositionFrame* pPos)
{
	if(!pPos)
		return;

	cout << "frame id: " << (int)pPos->u3FrameId << endl;
	cout << "telemetry bits: " << (int)pPos->u5TelemetryBits << endl;
	cout << "lat: " << pPos->s32Lat << endl;
	cout << "lon: " << pPos->s32Lon << endl;
	cout << "alt: " << pPos->u16Alt << endl;
	cout << endl;
}

CFT4PacketFactory PacketFactory;
int main(int argc, char *argv[])
{
   cout << "Super Frame Decoder\n\n";
   if(argc < 2)
   {
	   cout << "usage: sfd <frame> [<frame> <frame>]" << endl;
	   return 0;
   }

   //cout << "paker format: " << CFT4PacketFactory::TFT4Format::GetPattern() << endl;

   //cout << "input data: \n";
   for(int i = 1; i < argc; i++)
   {
	  cout << "frame [" << i - 1 << "]: "<< argv[i] << endl;

	  if(!PacketFactory.AppendWsFrame(argv[i]))
	  {
		  cout << "error: ws frame len not match with format" << endl;
		  return 0;
	  }
   }

   unsigned char U8Buff[256];
   TFrameUnion& FrameUnion = *(TFrameUnion*)U8Buff;
   PacketFactory.DecodeFrames(argc - 1, U8Buff, sizeof(U8Buff) * 8);

   cout << "raw: ";
   for (int i = 0; i < sizeof(FrameUnion); ++i)
   {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(((unsigned char *)&FrameUnion)[i]);
   }

   cout << endl;
   cout << endl << dec;
   if(argc == 2)
   {
	   	PrintDiagInfo(&FrameUnion.Diagnostics);
		return 0;
   }

   if(argc == 4)
   {
	   PrintPositionInfo(&FrameUnion.SuperFrame.Position);
	   PrintDiagInfo(&FrameUnion.SuperFrame.Diagnostics);
	   return 0;
   }
	
   cout << "error: bad frame" << endl;
   return 0;
}
