#include <cstdint>

#define __packed __attribute__((packed))
#include "../paker.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include "telemetry.hpp"

using namespace std;
using namespace Protocol::Paker;

static int i32TestNr = 0;
template<class Factory, class CTestData>
void Test(Factory& PacketFactory, CTestData& RawFrame)
{
   auto const FtFramesCnt = PacketFactory.EncodeRaw((unsigned char *)&RawFrame, sizeof(RawFrame) * 8);
   cout << "\nSQ9P paker TEST NR: " << i32TestNr++ << endl;
   cout << "paker format: " << Factory::Format::GetPattern() << endl;
   cout << "frame permutations: " << Factory::Format::GetMaxPermutations() << endl;
   cout << "frame bitsize floor: " << Factory::Format::GetBitSizeFloor() << endl;
   cout << "specific base: ";
   ;

   const char *pattern = Factory::Format::GetPattern();
   for (int i = 0; i < strlen(pattern); i++)
   {
      cout << pattern[i] << "=" << Factory::Format::GetBase(pattern[i]) << " ";
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

   cout << std::dec << endl;
}

int main()
{
   TPositionFrame RawFrame;
   for(int i = 0; i < sizeof(RawFrame); i++)
      ((unsigned char*)&RawFrame)[i] = i*4 + 31;

   CWsprPacketFactory WsprPacketFactory;
   CFT4PacketFactory Ft4PacketFactory;
   CJt9PacketFactory Jt9PacketFactory;

   Test(WsprPacketFactory, RawFrame);   
   Test(Ft4PacketFactory, RawFrame);
   Test(Jt9PacketFactory, RawFrame);
}