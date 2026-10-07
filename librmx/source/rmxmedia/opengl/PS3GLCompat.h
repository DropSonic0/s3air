/*
*	PS3 (PSGL + Cg) glue for vertex attributes.
*
*	PSGL has no VAOs and Cg binds vertex attributes by semantic (POSITION, TEXCOORD0, COLOR0 ...) instead of by the
*	index used with glVertexAttribPointer. So the attribute pointers can only be set once a Cg vertex program is bound,
*	i.e. right before the draw call. This header redirects glDrawArrays to a function that does exactly that, so that
*	existing call sites (vao.draw(), and the plain glDrawArrays(...) calls in the renderer / shader classes) keep working.
*
*	Include it right after the PSGL headers (e.g. in rmxmedia.h), for PS3 only.
*/

#pragma once

#if defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3) || defined(__CELLOS_LV2__) || defined(__SNC__)

#include <PSGL/psgl.h>		// So this header does not depend on where the GL headers were included before

void ps3DrawArrays(GLenum mode, GLint first, GLsizei count);

#define glDrawArrays ps3DrawArrays

#endif
