#include <cstdint>

#define __packed __attribute__((packed))
#include "../paker.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include "telemetry.hpp"

using namespace std;
using namespace Protocol::Paker;

CWsprPacketFactory PacketFactory;
// CWsprPacketFactory WsprPacketFactory;

int main()
{
   TSuperFrame RawFrame;
   memset(&RawFrame, 0xFF, sizeof(RawFrame));
   // RawFrame.Position.u3FrameId = TPositionFrame::FrameId;
   // RawFrame.Diagnostics.u3FrameId = TDiagnosticsFrame::FrameId;
   
   auto const FtFramesCnt = PacketFactory.EncodeRaw((unsigned char *)&RawFrame, sizeof(RawFrame) * 8);
   cout << "\npaker format: " << CFT4PacketFactory::Format::GetPattern() << endl;
   cout << "frame permutations: " << CFT4PacketFactory::Format::GetMaxPermutations() << endl;
   cout << "frame bitsize ffloor: " << CFT4PacketFactory::Format::GetBitSizeFloor() << endl;
   cout << "specific base: ";
   ;

   const char *pattern = CFT4PacketFactory::Format::GetPattern();
   for (int i = 0; i < strlen(pattern); i++)
   {
      cout << pattern[i] << "=" << CFT4PacketFactory::Format::GetBase(pattern[i]) << " ";
   }
   cout << endl
        << endl;

   cout << "test raw data: ";

   for (int i = 0; i < sizeof(RawFrame); ++i)
   {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(((unsigned char *)&RawFrame)[i]);
   }

   cout << "\n\nencoded output: \n";
   for (unsigned int i = 0; i < FtFramesCnt; i++)
   {
      cout << "frame [" << i << "]:  " << PacketFactory.GetPacket(i) << endl;
   }

   cout << "\ndecoded back to: ";
   unsigned char C8Dupa[sizeof(RawFrame) + 4] = {0};
   PacketFactory.DecodeFrames(FtFramesCnt, C8Dupa, sizeof(C8Dupa));
   for (int i = 0; i < sizeof(RawFrame); ++i)
   {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(((unsigned char *)C8Dupa)[i]);
   }

   cout << endl;
}
