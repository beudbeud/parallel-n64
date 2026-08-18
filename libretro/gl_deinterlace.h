#ifndef GL_DEINTERLACE_H
#define GL_DEINTERLACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Line-doubles the given field of the frame the frontend is about to
 * composite. Call at present time, only when the VI has serrate set. */
void gl_deinterlace_frame(unsigned width, unsigned height, unsigned field);

void gl_deinterlace_context_reset(void);

#ifdef __cplusplus
}
#endif

#endif
