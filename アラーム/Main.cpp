#include"DxLib.h"
#include"keyboard.h"
#include"Main.h"
#include"do.h"

static main_t now = scene_start;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	ChangeWindowMode(TRUE), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK);
	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
		Keyboard_Update();
		switch (now) {
		case scene_start:
			start_main();
			break;
		case scene_view:
			view_main();
			break;
		case scene_add:
			add_main();
			break;
		case scene_delete:
			delete_main();
			break;
		case scene_ringing:
			ringing_main();
			break;
		}
		
	}
	end_main();
	DxLib_End();
	return 0;
 }

void changescene(main_t next) {
	now = next;
}