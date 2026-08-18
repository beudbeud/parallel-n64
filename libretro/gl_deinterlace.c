/* Bob deinterlacing for the hardware renderers.
 *
 * The software renderer does this inside its own VI emulation: when the frame
 * is interlaced it overwrites each stale sibling line with the field just
 * rendered, so the picture carries one field rather than two woven instants
 * (n64video/vi.c, config.vi.bob_deinterlace).  The GL renderers never look at
 * the serrate bit at all -- GLideN64 reads it only to size its buffers -- so
 * the same option had nothing to act on there.
 *
 * The frame is already on the GPU, so do it there: blit the field's lines down
 * into a half-height scratch buffer, then blit that back up over the whole
 * frame.  Point sampling both ways makes the second pass an exact line double
 * of the first pass' rows.  Two blits, no shader, no readback.
 *
 * Needs the separate draw/read framebuffer binding points and
 * glBlitFramebuffer, so GLES 3.0 or desktop GL; a GLES2-only build keeps its
 * previous behaviour of ignoring interlacing.
 */

#include <stdlib.h>

/* Raw GL, deliberately NOT glsm/glsmsym.h -- see the comment in
 * aleck64_e90_gl.c: routing this file's scratch binds through glsm's tracking
 * makes them come back as the core renderer's own state on the next frame. */
#include <glsym/glsym.h>
#include <glsm/glsm.h>

#include "gl_deinterlace.h"

#if defined(HAVE_OPENGLES2) && !defined(HAVE_OPENGLES3)
#define GL_DEINTERLACE_UNAVAILABLE 1
#endif

/* Not every header mix in this tree brings the separate binding points in;
 * aleck64_e90_gl.c carries the same fallbacks for the same reason. */
#ifndef GL_DRAW_FRAMEBUFFER
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#endif
#ifndef GL_READ_FRAMEBUFFER
#define GL_READ_FRAMEBUFFER 0x8CA8
#endif
#ifndef GL_DRAW_FRAMEBUFFER_BINDING
#define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6
#endif
#ifndef GL_READ_FRAMEBUFFER_BINDING
#define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
#endif

#ifndef GL_DEINTERLACE_UNAVAILABLE

static GLuint di_fbo = 0;
static GLuint di_tex = 0;
static unsigned di_w  = 0;
static unsigned di_h  = 0;   /* height of the scratch buffer, i.e. one field */
static int      di_failed = 0;

static int di_resize(unsigned w, unsigned h)
{
   if (di_fbo && di_w == w && di_h == h)
      return 1;

   if (!di_fbo)
      glGenFramebuffers(1, &di_fbo);
   if (!di_tex)
      glGenTextures(1, &di_tex);
   if (!di_fbo || !di_tex)
      return 0;

   glBindTexture(GL_TEXTURE_2D, di_tex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, di_fbo);
   glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                          GL_TEXTURE_2D, di_tex, 0);

   if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      return 0;

   di_w = w;
   di_h = h;
   return 1;
}

void gl_deinterlace_frame(unsigned width, unsigned height, unsigned field)
{
   GLuint  frame;
   GLint   prev_draw = 0, prev_read = 0, prev_tex = 0;
   GLint   scissor = 0;
   unsigned half;

   if (di_failed || width < 2 || height < 2)
      return;

   half = height / 2;

   /* The frontend can hand back a different target between frames, so ask for
    * the live one rather than caching it. */
   frame = glsm_get_current_framebuffer();

   glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prev_draw);
   glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prev_read);
   glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_tex);
   /* Neither glIsEnabled nor glGetBooleanv is among the symbols glsym
    * resolves on the desktop profile; glGetIntegerv answers the same
    * question and is used everywhere else in this file. */
   glGetIntegerv(GL_SCISSOR_TEST, &scissor);
   if (scissor)
      glDisable(GL_SCISSOR_TEST);

   if (!di_resize(width, half))
   {
      di_failed = 1;
   }
   else
   {
      /* Down: the field's lines, one per scratch row.  Starting the source
       * rectangle one line up on the lower field is what alternates which
       * lines survive, and so what keeps this bob rather than a fixed
       * half-vertical-resolution image. */
      glBindFramebuffer(GL_READ_FRAMEBUFFER, frame);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, di_fbo);
      glBlitFramebuffer(0, (GLint)(field & 1u), (GLint)width, (GLint)height,
                        0, 0, (GLint)width, (GLint)half,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);

      /* Up: each scratch row over both of its lines. */
      glBindFramebuffer(GL_READ_FRAMEBUFFER, di_fbo);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame);
      glBlitFramebuffer(0, 0, (GLint)width, (GLint)half,
                        0, 0, (GLint)width, (GLint)height,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
   }

   if (scissor)
      glEnable(GL_SCISSOR_TEST);
   glBindTexture(GL_TEXTURE_2D, (GLuint)prev_tex);
   glBindFramebuffer(GL_READ_FRAMEBUFFER, (GLuint)prev_read);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, (GLuint)prev_draw);
}

void gl_deinterlace_context_reset(void)
{
   /* The objects died with the context; drop the ids without deleting them. */
   di_fbo    = 0;
   di_tex    = 0;
   di_w      = 0;
   di_h      = 0;
   di_failed = 0;
}

#else /* GLES2 only: no separate read/draw targets, no glBlitFramebuffer */

void gl_deinterlace_frame(unsigned width, unsigned height, unsigned field)
{
   (void)width; (void)height; (void)field;
}

void gl_deinterlace_context_reset(void) { }

#endif
