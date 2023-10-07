#include <cstdint>

#define __packed __attribute__((packed))
#include "../paker.hpp"
#include <iostream>
#include <string>
#include <iomanip> // Dla std::setw i std::setfill

using namespace std;
using namespace Protocol::Paker;

struct __attribute__((packed, aligned(4))) TRawFrame
{
   int32_t s32Lat;
   int32_t s32Lon;
   uint16_t u16Alt;
};

struct __packed TDiagnosticsFrame
{
   static constexpr auto FrameId = 1;
   unsigned char u3FrameId : 3;
   unsigned char u5TelemetryBits : 5;
   unsigned short s16Voltage;
   char s8Temperature;
};

struct __packed TPositionFrame
{
   static constexpr auto FrameId = 0;
   unsigned char u3FrameId : 3;
   unsigned char u5TelemetryBits : 5;
   int s32Lon;
   int s32Lat;
   unsigned short u16Alt;
};

struct __packed TSuperFrame
{
   TPositionFrame Position;
   TDiagnosticsFrame Diagnostics;
};

CFT4PacketFactory PacketFactory;
int main()
{
   TSuperFrame RawFrame;
   RawFrame.Position.u3FrameId = TPositionFrame::FrameId;
   RawFrame.Diagnostics.u3FrameId = TDiagnosticsFrame::FrameId;
   
   auto const FtFramesCnt = PacketFactory.EncodeRaw((unsigned char *)&RawFrame, sizeof(RawFrame) * 8);
   cout << "\npaker format: " << CFT4PacketFactory::TFT4Format::GetPattern() << endl;
   cout << "frame permutations: " << CFT4PacketFactory::TFT4Format::GetMaxPermutations() << endl;
   cout << "frame bitsize ffloor: " << CFT4PacketFactory::TFT4Format::GetBitSizeFloor() << endl;
   cout << "specific base: ";
   ;

   const char *pattern = CFT4PacketFactory::TFT4Format::GetPattern();
   for (int i = 0; i < strlen(pattern); i++)
   {
      cout << pattern[i] << "=" << CFT4PacketFactory::TFT4Format::GetBase(pattern[i]) << " ";
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
