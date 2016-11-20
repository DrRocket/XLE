// Copyright 2015 XLGAMES Inc.
//
// Distributed under the MIT License (See
// accompanying file "LICENSE" or the website
// http://www.opensource.org/licenses/mit-license.php)

#pragma once

#include "NascentObjectGuid.h"
#include "../Math/Matrix.h"
#include "../Utility/IteratorUtils.h"
#include <vector>
#include <algorithm>

namespace RenderCore { namespace Assets { namespace GeoProc
{
    class SkeletonRegistry
    {
    public:
        bool            IsImportant(NascentObjectGuid node) const;

        using TransformMarker = unsigned;
        static const TransformMarker TransformMarker_UnSet = ~unsigned(0);
        
        class ImportantNode
        {
        public:
            NascentObjectGuid      _id;
            std::string     _bindingName;
            TransformMarker _transformMarker;
            Float4x4        _inverseBind;
            bool            _hasInverseBind;

            ImportantNode()
            : _inverseBind(Identity<Float4x4>())
            , _hasInverseBind(false)
            , _transformMarker(TransformMarker_UnSet) {}

            ImportantNode(
                NascentObjectGuid id, const std::string& bindingName, 
                TransformMarker transformMarker, 
                const Float4x4& inverseBind, bool hasInverseBind)
            : _id(id), _bindingName(bindingName)
            , _transformMarker(transformMarker), _inverseBind(inverseBind)
            , _hasInverseBind(hasInverseBind) {}
        };

        auto            GetImportantNodes() const -> IteratorRange<const ImportantNode*>;
        ImportantNode   GetNode(NascentObjectGuid node) const;
        
        bool            TryRegisterNode(NascentObjectGuid node, const char bindingName[]);
        TransformMarker GetOutputMatrixIndex(NascentObjectGuid node);

        void            AttachInverseBindMatrix(NascentObjectGuid node, const Float4x4& inverseBind);
        void            AttachMergeGeometry(NascentObjectGuid node, const Float4x4& mergeToGeometry);

        void            MarkParameterAnimated(const std::string& paramName);
        bool            IsAnimated(const std::string& paramName) const;

        SkeletonRegistry();
        ~SkeletonRegistry();
    protected:
        std::vector<ImportantNode> _importantNodes;
        std::vector<std::string> _markParameterAnimated;

        TransformMarker _nextOutputIndex;
    };
}}}

