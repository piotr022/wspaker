#pragma once

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

union __packed TFrameUnion
{
	TDiagnosticsFrame Diagnostics;
	TSuperFrame SuperFrame;
};

