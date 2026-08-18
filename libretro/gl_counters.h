#ifndef GL_COUNTERS_H
#define GL_COUNTERS_H
/* ponytail: temporary. Counts the per-frame work that can plausibly explain a
 * renderer-specific stall, so the answer comes from a measurement instead of
 * another option toggle. Remove with the report in libretro.c. */
#ifdef __cplusplus
extern "C" {
#endif
extern unsigned long gldbg_shader_compiles;
extern unsigned long gldbg_buffers_removed;
extern unsigned long gldbg_depth_destroyed;
extern unsigned long gldbg_color_to_rdram;
extern unsigned long gldbg_depth_to_rdram;
#ifdef __cplusplus
}
#endif
#endif
