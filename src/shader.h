#ifndef SHADER_LIB
#define SHADER_LIB


typedef struct _shader shader_t;
typedef enum _shader_err
{
    SH_OK,
    SH_NOT_OK
} shader_err;

shader_t* load_shader(const char*, const char*, shader_err*);
shader_err unload_shader(shader_t*);
shader_err use_shader(shader_t*);
shader_err set_uniform_float(shader_t*, float val, const char* uniform);
shader_err set_uniform_int(shader_t*, int val, const char* uniform);
shader_err set_uniform_float3(shader_t*, float val_1, float val_2, float val_3, const char* uniform);
int shader_get_id(shader_t*);

#endif
