/* ponytail: temporary, see gl_counters.h */
unsigned long gldbg_shader_compiles;
unsigned long gldbg_buffers_removed;
unsigned long gldbg_depth_destroyed;
unsigned long gldbg_color_to_rdram;
unsigned long gldbg_depth_to_rdram;
unsigned long gldbg_draw_calls;
unsigned long gldbg_tex_loads;
unsigned long gldbg_texrect_flush;
unsigned long gldbg_tex_hits;
unsigned long gldbg_tex_size_reload;
unsigned long gldbg_tex_evicted;
unsigned long gldbg_tex_cache_size;
unsigned long gldbg_tex_loads_ci;
unsigned long gldbg_tex_loads_same_tmem;
unsigned long gldbg_tex_bytes;
unsigned long gldbg_tex_load_us;
unsigned long gldbg_tex_upload_us;
unsigned long gldbg_tex_crc_us;

#include <features/features_cpu.h>
unsigned long long gldbg_now_us(void)
{
   return (unsigned long long)cpu_features_get_time_usec();
}
