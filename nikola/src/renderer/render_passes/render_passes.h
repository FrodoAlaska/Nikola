#pragma once 

#include "nikola/nikola_render.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Shadow pass functions

void shadow_pass_init(Window* window);

void shadow_pass_prepare(RenderPass* pass, const FrameData& data);

void shadow_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue);

Mat4 shadow_pass_get_light_space(RenderPass* pass);

/// Shadow pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Light pass functions

void light_pass_init(Window* window);

void light_pass_prepare(RenderPass* pass, const FrameData& data);

void light_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue);

/// Light pass functions
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// HDR pass functions

void hdr_pass_init(Window* window);

void hdr_pass_prepare(RenderPass* pass, const FrameData& data);

void hdr_pass_sumbit(RenderPass* pass, const DynamicArray<GeometryPrimitive>& queue);

/// HDR pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

