#include <stdarg.h>
#include <stdlib.h>
/**
 * # variable param list
 *
 * inspired from va_list 
 * runs on heap so can passed to 
 * another thread
 *
 * author : xiehuc@gmail.com
 */


typedef struct {void* st;void* cur; size_t sz;}vp_list;
typedef void (*VP_CALLBACK)(void);
typedef void (*VP_DISPATCH)(VP_CALLBACK,vp_list*,void*);
typedef struct vp_command{VP_DISPATCH dsph;VP_CALLBACK func;vp_list data;struct vp_command* next;}vp_command;

#define vp_init(vp,size) do{(vp).st = (vp).cur = malloc(size);(vp).sz = size;}while(0)
#define _ptr_self_inc(ptr,sz) ((ptr+=sz)-sz)
#define vp_dump(vp,va,type) do{type t = va_arg((va),type);memcpy(_ptr_self_inc((vp).cur,sizeof(type)),&t,sizeof(type));}while(0)
#define vp_start(vp) (vp).cur = (vp).st
#define vp_arg(vp,type) *(type*)(_ptr_self_inc((vp).cur,sizeof(type)))
#define vp_end(vp) do{free((vp).st),(vp).cur=(vp).st=NULL;(vp).sz=0;}while(0)

vp_command vp_make_command(VP_DISPATCH,VP_CALLBACK,...);
void vp_do(vp_command,void* retval);
void vp_do_repeat(vp_command cmd,void* retval);
void vp_link(vp_command* head,vp_command* elem);
void vp_cancel(vp_command);
//cancel and clear memory to pass vp_do 
#define vp_cancel0(cmd) {vp_cancel(cmd);memset(&cmd,0,sizeof(cmd));}

vp_list* vp_make_params(VP_DISPATCH,...);
/**
 * p : pointer
 * i : int
 * func_[lst] param list is [lst] ret is void
 * func_[lst]_[ret] param list is [lst] ret is [ret]
 */
void vp_func_void ( VP_CALLBACK,vp_list*,void*);
void vp_func_p    ( VP_CALLBACK,vp_list*,void*);
void vp_func_2p   ( VP_CALLBACK,vp_list*,void*);
void vp_func_2pi  ( VP_CALLBACK,vp_list*,void*);
void vp_func_3p   ( VP_CALLBACK,vp_list*,void*);
void vp_func_3pi  ( VP_CALLBACK,vp_list*,void*);
void vp_func_4p   ( VP_CALLBACK,vp_list*,void*);
void vp_func_pi   ( VP_CALLBACK,vp_list*,void*);

void vp_func_p_i  ( VP_CALLBACK,vp_list*,void*);
void vp_func_2p_i ( VP_CALLBACK,vp_list*,void*);
void vp_func_3p_i ( VP_CALLBACK,vp_list*,void*);
