/*
 * paker.hpp
 * piotr022, sq9p.peter@gmail.com
 */

#pragma once
#include <string.h>
#include <math.h>

namespace Protocol
{
   namespace Paker
   {
      struct TFormatElement
      {
         const char c8Id;
         const unsigned int u32Base;
         const char *C8CharcterMap;
      };

      // compile time format info
      template <unsigned int ElmSize, const TFormatElement (&FormatElemets)[ElmSize], const char pattern[]>
      class CFormat
      {
         static constexpr auto PatternLen = strlen(pattern);
         using ArrType = const char[PatternLen];

      public:
         static constexpr auto GetLen() { return PatternLen; }
         static constexpr auto GetPattern() { return pattern; }
         static constexpr auto GetSeqNrIdx()
         {
            for (unsigned int i = 0; i < GetLen(); i++)
            {
               if (pattern[i] == 'S')
                  return i;
            }
            return (unsigned int)UINT32_MAX;
         }

         static constexpr unsigned int GetBase(unsigned int u32Idx)
         {
            if (u32Idx >= GetLen())
               return 0;

            return GetBase(pattern[u32Idx]);
         }

         static constexpr unsigned int GetBase(const char sign)
         {
            for (const auto &element : FormatElemets)
            {
               if (element.c8Id == sign)
                  return element.u32Base;
            }

            return 1;
         }

         static constexpr auto GetMaxPermutations()
         {
            return GetPermutationsRightSide(0);
         }

         static constexpr auto GetPermutationsRightSide(unsigned int u32Idx)
         {
            unsigned long long u64Permutations = 1;
            for (unsigned int i = u32Idx; i < GetLen(); i++)
            {
               u64Permutations *= GetBase(i);
            }

            return u64Permutations;
         }

         static constexpr auto GetBitSizeFloor()
         {
            return (unsigned int)floor(log2(GetMaxPermutations()));
         }

         static constexpr const TFormatElement &GetFormatElement(unsigned int u32FormatIdx)
         {
            for (const auto &element : FormatElemets)
            {
               if (element.c8Id == pattern[u32FormatIdx])
                  return element;
            }

            return FormatElemets[0];
         }

         static constexpr auto CustomBaseMap(unsigned int u32FormatIdx, unsigned int u32Val)
         {
            const auto &PatternElement = GetFormatElement(u32FormatIdx);
            if (u32Val >= PatternElement.u32Base || !PatternElement.C8CharcterMap)
               return PatternElement.c8Id;

            return PatternElement.C8CharcterMap[u32Val];
         }

         static constexpr unsigned int CustomBaseToDec(unsigned int u32FormatIdx, char c8Input)
         {
            const auto &PatternElement = GetFormatElement(u32FormatIdx);
            if (!PatternElement.C8CharcterMap || PatternElement.c8Id == 'S')
               return 0;

            for (unsigned int i = 0; i < strlen(PatternElement.C8CharcterMap); i++)
            {
               if (PatternElement.C8CharcterMap[i] == c8Input)
               {
                  return i;
               }
            }

            return 0;
         }

         static constexpr auto SequenceNrMap(unsigned char u8SequenceNr)
         {
            auto const &SequenceFormatElem = GetFormatElement(GetSeqNrIdx());
            return SequenceFormatElem.C8CharcterMap[u8SequenceNr % strlen(SequenceFormatElem.C8CharcterMap)];
         }

         static constexpr char GetPatternCharacter(unsigned int u32Idx)
         {
            if (u32Idx >= GetLen())
            {
               return ' ';
            }

            return pattern[u32Idx];
         }
      };

      template <class FormatT, unsigned int BuffersCnt>
      class TEncoder
      {
         char (&OutBuffors)[BuffersCnt][FormatT::GetLen() + 1];

      public:
         template <unsigned int N>
         TEncoder(char (&outStrArrs)[BuffersCnt][N]) : OutBuffors(outStrArrs){};

         unsigned int EncodeBigEndian(unsigned char *p8Data, unsigned int u32BitSize) // conisering little endian arch
         {
            unsigned int u32SrcBitIdx = 0;
            const auto requeredFrames = (u32BitSize + FormatT::GetBitSizeFloor() - 1) / FormatT::GetBitSizeFloor();
            for (unsigned int u32Frame = 0; u32Frame < requeredFrames; u32Frame++)
            {
               unsigned long long u64Temp = 0;
               BitwiseCopy(&u64Temp, 0, p8Data, u32SrcBitIdx, std::min(FormatT::GetBitSizeFloor(), u32BitSize - u32SrcBitIdx));
               for (unsigned int u32SymbolIdx = 0; u32SymbolIdx < FormatT::GetLen(); u32SymbolIdx++)
               {
                  if (u32SymbolIdx == FormatT::GetSeqNrIdx())
                  {
                     OutBuffors[u32Frame][u32SymbolIdx] = FormatT::SequenceNrMap(u32Frame);
                     continue;
                  }

                  auto const SymbolBase = FormatT::GetBase(u32SymbolIdx);
                  if (SymbolBase < 2)
                  {
                     OutBuffors[u32Frame][u32SymbolIdx] = FormatT::GetPatternCharacter(u32SymbolIdx);
                     continue;
                  }

                  unsigned long long u64CurrentSymbol = u64Temp / FormatT::GetPermutationsRightSide(u32SymbolIdx + 1);
                  u64CurrentSymbol %= SymbolBase;
                  OutBuffors[u32Frame][u32SymbolIdx] = FormatT::CustomBaseMap(u32SymbolIdx, u64CurrentSymbol);
               }

               u32SrcBitIdx += FormatT::GetBitSizeFloor();
            }

            return requeredFrames;
         }

         unsigned int DecodeBigEndian(unsigned char u8FramesToDecode, unsigned char *pOutData, unsigned int u32OutBuffLen)
         {
            // unsigned int
            if (u32OutBuffLen * 8 < u8FramesToDecode * FormatT::GetBitSizeFloor())
            {
               return 0;
            }

            unsigned int u32OutDataBitIdx = 0;
            for (unsigned int u32Frame = 0; u32Frame < u8FramesToDecode; u32Frame++)
            {
               unsigned long long u64Temp = 0;
               for (unsigned int u32SymbolIdx = 0; u32SymbolIdx < FormatT::GetLen(); u32SymbolIdx++)
               {
                  if (u32SymbolIdx == FormatT::GetSeqNrIdx())
                     continue;

                  u64Temp += FormatT::CustomBaseToDec(u32SymbolIdx, OutBuffors[u32Frame][u32SymbolIdx]) *
                             FormatT::GetPermutationsRightSide(u32SymbolIdx + 1);
               }

               BitwiseCopy(pOutData, u32OutDataBitIdx, (unsigned char *)&u64Temp, 0, FormatT::GetBitSizeFloor());
               u32OutDataBitIdx += FormatT::GetBitSizeFloor();
            }

            return u32OutDataBitIdx;
         }

      private:
         void BitwiseCopy(void *pDest, unsigned int u32DestBitIdx, const unsigned char *pSource, unsigned int u32SourceBitIdx, unsigned int u32BitsToCopy)
         {
            while (u32BitsToCopy--)
            {
               unsigned char u8Bit = pSource[u32SourceBitIdx / 8] >> (u32SourceBitIdx % 8);
               u8Bit &= 1;

               unsigned char *p8Dest = reinterpret_cast<unsigned char *>(pDest);
               p8Dest[u32DestBitIdx / 8] &= ~(1 << (u32DestBitIdx % 8));
               p8Dest[u32DestBitIdx / 8] |= (u8Bit << (u32DestBitIdx % 8));
               u32DestBitIdx++;
               u32SourceBitIdx++;
            }
         }
      };

      struct TPatternFormats
      {
         TPatternFormats() = delete;
         static constexpr auto DefElmCnt = 6;
         static constexpr TFormatElement DefaultFormatElements[DefElmCnt] =
             {
                 {'0', 1, " "},          // nothing
                 {'S', 1, "0123456789"}, // sequence number
                 {'x', 26, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
                 {'X', 36, "ABCDEFGHIJKLMNOPRSTQUWXYVZ0123456789"},
                 {'l', 18, "ABCDEFGHIJKLMNOPQR"},
                 {'U', 10, "0123456789"},
         };
      };

      template <const char pattern[]>
      using CFormatWithDefaultElements = CFormat<TPatternFormats::DefElmCnt, TPatternFormats::DefaultFormatElements, pattern>;

      class CFT4PacketFactory
      {
         static constexpr char FT4FormatStr[] = "CQ 4NSxxxxxxx";

      public:
         static constexpr auto MaxFt4Subframes = 8;
         using TFT4Format = CFormatWithDefaultElements<FT4FormatStr>;
         using CFT4Encoder = TEncoder<TFT4Format, MaxFt4Subframes>;
         char C8Frames[MaxFt4Subframes][TFT4Format::GetLen() + 1];
         CFT4Encoder FT4Encoder;

         unsigned char u8EncodedFrames = 0;
         CFT4PacketFactory() : FT4Encoder(C8Frames){};

         const char *GetPacket(unsigned int u32Idx)
         {
            if (u32Idx >= MaxFt4Subframes)
               return "NOT VALID";

            return C8Frames[u32Idx];
         }

         unsigned int EncodeRaw(unsigned char *p8Data, unsigned int u32BitLen)
         {
            u8EncodedFrames = FT4Encoder.EncodeBigEndian(p8Data, u32BitLen);
            return u8EncodedFrames;
         }

         unsigned int DecodeFrames(unsigned char u8FramesToDecode, unsigned char *pOutData, unsigned int u32OutBuffLen)
         {
            return FT4Encoder.DecodeBigEndian(u8FramesToDecode, pOutData, u32OutBuffLen);
         }
      };
   }
}
