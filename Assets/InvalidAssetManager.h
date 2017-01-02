// Copyright 2015 XLGAMES Inc.
//
// Distributed under the MIT License (See
// accompanying file "LICENSE" or the website
// http://www.opensource.org/licenses/mit-license.php)

#pragma once

#include "AssetsCore.h"
#include "../Utility/StringUtils.h"
#include "../Core/Types.h"
#include <memory>
#include <vector>
#include <functional>

namespace Assets
{
    /// <summary>Records a list of invalid assets</summary>
    /// The assets in this list have been requested at runtime
    /// during this execution. But they failed for some reason.
    /// This is intended for tracking down shader compile errors.
    /// When a shader compile fails at runtime, the errors will be
    /// recorded here. In tools, then can be presented to the user
    /// in some fashion.
    class InvalidAssetManager
    {
    public:
        class AssetRef
        {
        public:
            rstring _name;
            rstring _errorString;
        };

        std::vector<AssetRef> GetAssets() const;
        
        using OnChangeEvent = std::function<void()>;
        using ChangeEventId = unsigned;
        ChangeEventId AddOnChangeEvent(OnChangeEvent evnt);
        void RemoveOnChangeEvent(ChangeEventId id);

        void MarkInvalid(StringSection<ResChar> name, StringSection<ResChar> errorString);
        void MarkValid(StringSection<ResChar> name);
        bool HasInvalidAssets() const;

        InvalidAssetManager(bool active);
        ~InvalidAssetManager();
    protected:
        class Pimpl;
        std::unique_ptr<Pimpl> _pimpl;
    };
}

