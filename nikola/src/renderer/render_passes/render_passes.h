#pragma once 

#include "nikola/nikola_render.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// Light pass functions

void light_pass_init(Window* window);

void light_pass_prepare(RenderPass* pass, const FrameData& data);

void light_pass_sumbit(RenderPass* pass);

/// Light pass functions
///---------------------------------------------------------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////

