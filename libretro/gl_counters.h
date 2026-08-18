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
extern unsigned long gldbg_draw_calls;
extern unsigned long gldbg_tex_loads;
extern unsigned long gldbg_texrect_flush;
extern unsigned long gldbg_tex_hits;
extern unsigned long gldbg_tex_size_reload;
extern unsigned long gldbg_tex_evicted;
extern unsigned long gldbg_tex_cache_size;
extern unsigned long gldbg_tex_loads_ci;
extern unsigned long gldbg_tex_loads_same_tmem;
extern unsigned long gldbg_tex_bytes;
extern unsigned long gldbg_tex_load_us;
extern unsigned long gldbg_tex_upload_us;
extern unsigned long gldbg_tex_crc_us;
unsigned long long gldbg_now_us(void);
#ifdef __cplusplus
}
#endif
#endif
