#include <cstdint>

#define __packed __attribute__((packed))
#include "paker.hpp"

using namespace Protocol::Paker;

extern "C"
{
   void DecodeSuperframe(char **p8Frames, uint8_t u8FramesCount, uint8_t *pOut, uint16_t u16BuffLen)
   {
      CFT4PacketFactory PacketFactory;

      if (!p8Frames ||
          !u8FramesCount ||
          u8FramesCount > CFT4PacketFactory::MaxFt4Subframes ||
          !pOut ||
          u16BuffLen * 8 < u8FramesCount * CFT4PacketFactory::TFT4Format::GetBitSizeFloor())
      {
         return;
      }

      memset(PacketFactory.C8Frames, 0, sizeof(PacketFactory.C8Frames));
      for (uint8_t i = 0; i < u8FramesCount; i++)
      {
         memcpy(PacketFactory.C8Frames, p8Frames[i],
            std::min(strlen(p8Frames[i]), CFT4PacketFactory::TFT4Format::GetLen()));
      }

      PacketFactory.DecodeFrames(u8FramesCount, pOut, u16BuffLen);
   }
}