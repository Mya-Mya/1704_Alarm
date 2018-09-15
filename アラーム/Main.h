#pragma once
typedef enum {
	scene_start,
	scene_view,
	scene_add,
	scene_delete,
	scene_ringing,
	scene_end,
}main_t;

void changescene(main_t next);