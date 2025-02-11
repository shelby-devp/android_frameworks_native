/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cinttypes>

#include <ui/Size.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <utils/StrongPointer.h>

namespace android {

class Fence;
class IGraphicBufferProducer;
class String8;

/* QTI_BEGIN */
namespace surfaceflingerextension {
class QtiDisplaySurfaceExtensionIntf;
} // namespace surfaceflingerextension
/* QTI_END */

namespace compositionengine {

/**
 * An abstraction for working with a display surface (buffer queue)
 */
class DisplaySurface : public virtual RefBase {
public:
    virtual ~DisplaySurface();

    // beginFrame is called at the beginning of the composition loop, before
    // the configuration is known. The DisplaySurface should do anything it
    // needs to do to enable HWComposer to decide how to compose the frame.
    // We pass in mustRecompose so we can keep VirtualDisplaySurface's state
    // machine happy without actually queueing a buffer if nothing has changed.
    virtual status_t beginFrame(bool mustRecompose) = 0;

    // prepareFrame is called after the composition configuration is known but
    // before composition takes place. The DisplaySurface can use the
    // composition type to decide how to manage the flow of buffers between
    // GPU and HWC for this frame.
    enum class CompositionType : uint8_t { Unknown = 0, Gpu = 0b1, Hwc = 0b10, Mixed = Gpu | Hwc };
    virtual status_t prepareFrame(CompositionType) = 0;

    // Inform the surface that GPU composition is complete for this frame, and
    // the surface should make sure that HWComposer has the correct buffer for
    // this frame. Some implementations may only push a new buffer to
    // HWComposer if GPU composition took place, others need to push a new
    // buffer on every frame.
    //
    // advanceFrame must be followed by a call to  onFrameCommitted before
    // advanceFrame may be called again.
    virtual status_t advanceFrame(float hdrSdrRatio) = 0;

    // onFrameCommitted is called after the frame has been committed to the
    // hardware composer. The surface collects the release fence for this
    // frame's buffer.
    virtual void onFrameCommitted() = 0;

    virtual void dumpAsString(String8& result) const = 0;

    virtual void resizeBuffers(const ui::Size&) = 0;

    virtual const sp<Fence>& getClientTargetAcquireFence() const = 0;

    // Returns true if the render surface supports client composition prediction.
    virtual bool supportsCompositionStrategyPrediction() const;

    /* QTI_BEGIN */
#ifdef QTI_DISPLAY_EXTENSION
    virtual surfaceflingerextension::QtiDisplaySurfaceExtensionIntf* qtiGetDisplaySurfaceExtn() = 0;
    /* QTI_END */
#endif
};

} // namespace compositionengine
} // namespace android
