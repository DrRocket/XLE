// Copyright 2015 XLGAMES Inc.
//
// Distributed under the MIT License (See
// accompanying file "LICENSE" or the website
// http://www.opensource.org/licenses/mit-license.php)

#pragma once

#include "../Types_Forward.h"
#include "../../Utility/PtrUtils.h"
#include "../../Utility/Streams/Serialization.h"
#include "../../Core/Types.h"
#include <memory>

namespace RenderCore { namespace Assets
{
    class RawAnimationCurve 
    {
    public:
        enum InterpolationType { Linear, Bezier, Hermite, CatmullRom };

        RawAnimationCurve(  size_t keyCount, 
                            std::unique_ptr<float[], BlockSerializerDeleter<float[]>>&&  timeMarkers, 
                            DynamicArray<uint8, BlockSerializerDeleter<uint8[]>>&&       keyPositions,
                            size_t elementSize, InterpolationType interpolationType,
                            Format positionFormat, Format inTangentFormat,
                            Format outTangentFormat);
        RawAnimationCurve(RawAnimationCurve&& curve);
        RawAnimationCurve(const RawAnimationCurve& copyFrom);
        RawAnimationCurve& operator=(RawAnimationCurve&& curve);

        template<typename Serializer>
            void        Serialize(Serializer& outputSerializer) const;

        float       StartTime() const;
        float       EndTime() const;

        template<typename OutType>
            OutType        Calculate(float inputTime) const never_throws;

    protected:
        size_t                          _keyCount;
        std::unique_ptr<float[], BlockSerializerDeleter<float[]>>    _timeMarkers;
        DynamicArray<uint8, BlockSerializerDeleter<uint8[]>>         _parameterData;
        size_t                          _elementStride;
        InterpolationType               _interpolationType;

        Format       _positionFormat;
        Format       _inTangentFormat;
        Format       _outTangentFormat;
    };

    template<typename Serializer>
        void        RawAnimationCurve::Serialize(Serializer& outputSerializer) const
    {
        ::Serialize(outputSerializer, _keyCount);
        ::Serialize(outputSerializer, _timeMarkers, _keyCount);
        ::Serialize(outputSerializer, _parameterData);
        ::Serialize(outputSerializer, _elementStride);
        ::Serialize(outputSerializer, unsigned(_interpolationType));
        ::Serialize(outputSerializer, unsigned(_positionFormat));
        ::Serialize(outputSerializer, unsigned(_inTangentFormat));
        ::Serialize(outputSerializer, unsigned(_outTangentFormat));
    }

}}





