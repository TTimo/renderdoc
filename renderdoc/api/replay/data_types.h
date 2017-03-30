/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2017 Baldur Karlsson
 * Copyright (c) 2014 Crytek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#pragma once

#include "replay_enums.h"

struct FloatVector
{
  FloatVector() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
  FloatVector(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
  float x, y, z, w;
};

DECLARE_REFLECTION_STRUCT(FloatVector);

struct DirectoryFile
{
  rdctype::str filename;
  FileProperty flags;
  uint32_t lastmod;
  uint64_t size;
};

DECLARE_REFLECTION_STRUCT(DirectoryFile);

struct ResourceFormat
{
  ResourceFormat()
  {
    special = true;
    specialFormat = SpecialFormat::Unknown;

    compCount = compByteWidth = 0;
    compType = CompType::Float;

    bgraOrder = false;
    srgbCorrected = false;
  }

  bool operator==(const ResourceFormat &r) const
  {
    if(special || r.special)
      return special == r.special && specialFormat == r.specialFormat && compType == r.compType;

    return compCount == r.compCount && compByteWidth == r.compByteWidth && compType == r.compType &&
           bgraOrder == r.bgraOrder && srgbCorrected == r.srgbCorrected;
  }

  bool operator!=(const ResourceFormat &r) const { return !(*this == r); }
  // indicates it's not a type represented with the members below
  // usually this means non-uniform across components or block compressed
  bool32 special;
  SpecialFormat specialFormat;

  rdctype::str strname;

  uint32_t compCount;
  uint32_t compByteWidth;
  CompType compType;

  bool32 bgraOrder;
  bool32 srgbCorrected;
};

DECLARE_REFLECTION_STRUCT(ResourceFormat);

struct BufferDescription
{
  ResourceId ID;
  rdctype::str name;
  bool32 customName;
  BufferCategory creationFlags;
  uint64_t length;
};

DECLARE_REFLECTION_STRUCT(BufferDescription);

struct TextureDescription
{
  rdctype::str name;
  bool32 customName;
  ResourceFormat format;
  uint32_t dimension;
  TextureDim resType;
  uint32_t width, height, depth;
  ResourceId ID;
  bool32 cubemap;
  uint32_t mips;
  uint32_t arraysize;
  TextureCategory creationFlags;
  uint32_t msQual, msSamp;
  uint64_t byteSize;
};

DECLARE_REFLECTION_STRUCT(TextureDescription);

struct APIEvent
{
  uint32_t eventID;

  rdctype::array<uint64_t> callstack;

  rdctype::str eventDesc;

  uint64_t fileOffset;
};

DECLARE_REFLECTION_STRUCT(APIEvent);

struct DebugMessage
{
  uint32_t eventID;
  MessageCategory category;
  MessageSeverity severity;
  MessageSource source;
  uint32_t messageID;
  rdctype::str description;
};

DECLARE_REFLECTION_STRUCT(DebugMessage);

enum class BucketRecordType : int
{
  Linear,
  Pow2,
};

struct ConstantBindStats
{
  static const BucketRecordType BucketType = BucketRecordType::Pow2;
  static const size_t BucketCount = 31;

  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
  rdctype::array<uint32_t> bindslots;
  rdctype::array<uint32_t> sizes;
};

DECLARE_REFLECTION_STRUCT(ConstantBindStats);

struct SamplerBindStats
{
  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
  rdctype::array<uint32_t> bindslots;
};

DECLARE_REFLECTION_STRUCT(SamplerBindStats);

struct ResourceBindStats
{
  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
  rdctype::array<uint32_t> types;
  rdctype::array<uint32_t> bindslots;
};

DECLARE_REFLECTION_STRUCT(ResourceBindStats);

struct ResourceUpdateStats
{
  static const BucketRecordType BucketType = BucketRecordType::Pow2;
  static const size_t BucketCount = 31;

  uint32_t calls;
  uint32_t clients;
  uint32_t servers;
  rdctype::array<uint32_t> types;
  rdctype::array<uint32_t> sizes;
};

DECLARE_REFLECTION_STRUCT(ResourceUpdateStats);

struct DrawcallStats
{
  static const BucketRecordType BucketType = BucketRecordType::Linear;
  static const size_t BucketSize = 1;
  static const size_t BucketCount = 16;

  uint32_t calls;
  uint32_t instanced;
  uint32_t indirect;
  rdctype::array<uint32_t> counts;
};

DECLARE_REFLECTION_STRUCT(DrawcallStats);

struct DispatchStats
{
  uint32_t calls;
  uint32_t indirect;
};

DECLARE_REFLECTION_STRUCT(DispatchStats);

struct IndexBindStats
{
  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
};

DECLARE_REFLECTION_STRUCT(IndexBindStats);

struct VertexBindStats
{
  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
  rdctype::array<uint32_t> bindslots;
};

DECLARE_REFLECTION_STRUCT(VertexBindStats);

struct LayoutBindStats
{
  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
};

DECLARE_REFLECTION_STRUCT(LayoutBindStats);

struct ShaderChangeStats
{
  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
  uint32_t redundants;
};

DECLARE_REFLECTION_STRUCT(ShaderChangeStats);

struct BlendStats
{
  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
  uint32_t redundants;
};

DECLARE_REFLECTION_STRUCT(BlendStats);

struct DepthStencilStats
{
  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
  uint32_t redundants;
};

DECLARE_REFLECTION_STRUCT(DepthStencilStats);

struct RasterizationStats
{
  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
  uint32_t redundants;
  rdctype::array<uint32_t> viewports;
  rdctype::array<uint32_t> rects;
};

DECLARE_REFLECTION_STRUCT(RasterizationStats);

struct OutputTargetStats
{
  uint32_t calls;
  uint32_t sets;
  uint32_t nulls;
  rdctype::array<uint32_t> bindslots;
};

DECLARE_REFLECTION_STRUCT(OutputTargetStats);

struct FrameStatistics
{
  bool32 recorded;
  ConstantBindStats constants[ENUM_ARRAY_SIZE(ShaderStage)];
  SamplerBindStats samplers[ENUM_ARRAY_SIZE(ShaderStage)];
  ResourceBindStats resources[ENUM_ARRAY_SIZE(ShaderStage)];
  ResourceUpdateStats updates;
  DrawcallStats draws;
  DispatchStats dispatches;
  IndexBindStats indices;
  VertexBindStats vertices;
  LayoutBindStats layouts;
  ShaderChangeStats shaders[ENUM_ARRAY_SIZE(ShaderStage)];
  BlendStats blends;
  DepthStencilStats depths;
  RasterizationStats rasters;
  OutputTargetStats outputs;
};

DECLARE_REFLECTION_STRUCT(FrameStatistics);

struct FrameDescription
{
  FrameDescription()
      : frameNumber(0),
        fileOffset(0),
        uncompressedFileSize(0),
        compressedFileSize(0),
        persistentSize(0),
        initDataSize(0),
        captureTime(0)
  {
  }

  uint32_t frameNumber;
  uint64_t fileOffset;
  uint64_t uncompressedFileSize;
  uint64_t compressedFileSize;
  uint64_t persistentSize;
  uint64_t initDataSize;
  uint64_t captureTime;
  FrameStatistics stats;
  rdctype::array<DebugMessage> debugMessages;
};

DECLARE_REFLECTION_STRUCT(FrameDescription);

struct EventUsage
{
  EventUsage() : eventID(0), usage(ResourceUsage::Unused) {}
  EventUsage(uint32_t e, ResourceUsage u) : eventID(e), usage(u) {}
  EventUsage(uint32_t e, ResourceUsage u, ResourceId v) : eventID(e), usage(u), view(v) {}
  bool operator<(const EventUsage &o) const
  {
    if(eventID != o.eventID)
      return eventID < o.eventID;
    return usage < o.usage;
  }

  bool operator==(const EventUsage &o) const { return eventID == o.eventID && usage == o.usage; }
  uint32_t eventID;
  ResourceUsage usage;
  ResourceId view;
};

DECLARE_REFLECTION_STRUCT(EventUsage);

struct DrawcallDescription
{
  DrawcallDescription() { Reset(); }
  void Reset()
  {
    eventID = 0;
    drawcallID = 0;
    flags = DrawFlags::NoFlags;
    markerColour[0] = markerColour[1] = markerColour[2] = markerColour[3] = 0.0f;
    numIndices = 0;
    numInstances = 0;
    indexOffset = 0;
    baseVertex = 0;
    vertexOffset = 0;
    instanceOffset = 0;

    dispatchDimension[0] = dispatchDimension[1] = dispatchDimension[2] = 0;
    dispatchThreadsDimension[0] = dispatchThreadsDimension[1] = dispatchThreadsDimension[2] = 0;

    indexByteWidth = 0;
    topology = Topology::Unknown;

    copySource = ResourceId();
    copyDestination = ResourceId();

    parent = 0;
    previous = 0;
    next = 0;

    for(int i = 0; i < 8; i++)
      outputs[i] = ResourceId();
    depthOut = ResourceId();
  }

  uint32_t eventID, drawcallID;

  rdctype::str name;

  DrawFlags flags;

  float markerColour[4];

  uint32_t numIndices;
  uint32_t numInstances;
  int32_t baseVertex;
  uint32_t indexOffset;
  uint32_t vertexOffset;
  uint32_t instanceOffset;

  uint32_t dispatchDimension[3];
  uint32_t dispatchThreadsDimension[3];

  uint32_t indexByteWidth;
  Topology topology;

  ResourceId copySource;
  ResourceId copyDestination;

  int64_t parent;

  int64_t previous;
  int64_t next;

  ResourceId outputs[8];
  ResourceId depthOut;

  rdctype::array<APIEvent> events;
  rdctype::array<DrawcallDescription> children;
};

DECLARE_REFLECTION_STRUCT(DrawcallDescription);

struct APIProperties
{
  // the pipeline type of the actual log/capture
  GraphicsAPI pipelineType;
  // the renderer used to render the log. For remote replay this could
  // be different to the above, and lets the UI make decisions e.g. to
  // flip rendering of images
  GraphicsAPI localRenderer;
  bool32 degraded;
};

DECLARE_REFLECTION_STRUCT(APIProperties);

struct CounterDescription
{
  GPUCounter counterID;
  rdctype::str name;
  rdctype::str description;
  CompType resultType;
  uint32_t resultByteWidth;
  CounterUnit unit;
};

DECLARE_REFLECTION_STRUCT(CounterDescription);

union CounterValue
{
  float f;
  double d;
  uint32_t u32;
  uint64_t u64;
};

struct CounterResult
{
  CounterResult() : eventID(0), counterID(GPUCounter::EventGPUDuration) { value.u64 = 0; }
  CounterResult(uint32_t EID, GPUCounter c, float data) : eventID(EID), counterID(c)
  {
    value.f = data;
  }
  CounterResult(uint32_t EID, GPUCounter c, double data) : eventID(EID), counterID(c)
  {
    value.d = data;
  }
  CounterResult(uint32_t EID, GPUCounter c, uint32_t data) : eventID(EID), counterID(c)
  {
    value.u32 = data;
  }
  CounterResult(uint32_t EID, GPUCounter c, uint64_t data) : eventID(EID), counterID(c)
  {
    value.u64 = data;
  }

  bool operator<(const CounterResult &o) const
  {
    if(eventID != o.eventID)
      return eventID < o.eventID;
    if(counterID != o.counterID)
      return counterID < o.counterID;

    // don't compare values, just consider equal
    return false;
  }

  bool operator==(const CounterResult &o) const
  {
    // don't compare values, just consider equal by EID/counterID
    return eventID == o.eventID && counterID == o.counterID;
  }

  uint32_t eventID;
  GPUCounter counterID;
  CounterValue value;
};

DECLARE_REFLECTION_STRUCT(CounterResult);

union PixelValue
{
  float value_f[4];
  uint32_t value_u[4];
  int32_t value_i[4];
  uint16_t value_u16[4];
};

struct ModificationValue
{
  PixelValue col;
  float depth;
  int32_t stencil;
};

DECLARE_REFLECTION_STRUCT(ModificationValue);

struct PixelModification
{
  uint32_t eventID;

  bool32 directShaderWrite;
  bool32 unboundPS;

  uint32_t fragIndex;
  uint32_t primitiveID;

  ModificationValue preMod;
  ModificationValue shaderOut;
  ModificationValue postMod;

  bool32 sampleMasked;
  bool32 backfaceCulled;
  bool32 depthClipped;
  bool32 viewClipped;
  bool32 scissorClipped;
  bool32 shaderDiscarded;
  bool32 depthTestFailed;
  bool32 stencilTestFailed;

  bool passed() const
  {
    return !sampleMasked && !backfaceCulled && !depthClipped && !viewClipped && !scissorClipped &&
           !shaderDiscarded && !depthTestFailed && !stencilTestFailed;
  }
};

DECLARE_REFLECTION_STRUCT(PixelModification);
