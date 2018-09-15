#include"DxLib.h"
#include"keyboard.h"
#include"do.h"
#include"Main.h"
#include<stdio.h>
#include <time.h>

#pragma warning(disable:4996)
#define LIST_MAX 16
#define VIEW_VIEW_Y 50

/*
flag 1=リストの終端　0=リスト現存
rang 0=まだ　1=完了
*/

typedef struct {
	int flag;
	int data;
	int rang;
}list_t;
list_t LIST[LIST_MAX];

int i = 0;//項目数を収納する
int s = 0;//選択中の要素通し番号を収納する
int errorclock = 120;//エラー表示のクロック

//時刻を収納する変数の初期化
time_t timer;
struct tm *t_st;
int data_time;//時*100+分

//変数の初期化とファイル読み込み
void start_main() {
	//変数の初期化、全ての要素のflagを1に、dataを0に。
	for (i = 0; i < LIST_MAX; i++) {
		LIST[i].flag = 1;
		LIST[i].data = 0;
		LIST[i].rang = 0;
	}
	//ファイルの内容からデーターをロード
	char *filename = "list.txt";
	FILE *filehandle;

	if ((filehandle = fopen(filename, "r")) == NULL) {
		DrawFormatString(0, 0, GetColor(255, 100, 100), "ファイルオープンエラー");
		changescene(scene_end);
	}
	else {
		///ファイルからデーターを読み込む
		fread(&LIST, sizeof(list_t), LIST_MAX, filehandle);
		fclose(filehandle);///解放
	}

	changescene(scene_view);
}

//一覧表示
void view_main() {

	//時刻の取得
	time(&timer);
	t_st = localtime(&timer);
	data_time = (t_st->tm_hour * 100 + t_st->tm_min);

	i = 0;
	while (LIST[i].flag != 1) {
		DrawFormatString(30, VIEW_VIEW_Y + i * 18, GetColor(200, 200, 200), "%d      %d", LIST[i].data, LIST[i].rang);//flagが0であるdataとrangを表示
		
		if (LIST[i].rang != 1) {
			if (LIST[i].data == data_time) {//時間になってかつrangが0だったら
				LIST[i].rang = 1;//鳴動完了にする
				changescene(scene_ringing);//ここに飛ぶとき、iはdata==data_timeの要素通し番号を含む
			}
		}
		i++;
	}

	DrawBox(0, 0, 640, 40, GetColor(50, 50, 50), TRUE);
	//ここに来るとiは項目数を収納している
	DrawFormatString(0, 0, GetColor(255, 255, 255), "一覧表　Dで消去、Aで追加、↑↓で選択　全部で%d項目　\n時刻右の数値0で未鳴動、1で鳴動完了", i);

	if (s > i - 1) { s = i - 1; }
	if (s < 0) { s = 0; }

	if (Keyboard_Get(KEY_INPUT_DOWN) == 1) {//下キーが押されていたら
		if (i != 0) { s = (s + 1) % i; }//選択状態を一つ下げる
		if (s < 0) { s = 0; }
	}
	if (Keyboard_Get(KEY_INPUT_UP) == 1) {//上キーが押されていたら
		if (i != 0) { s = (s + (i - 1)) % i; }//選択状態を１上げる
		if (s > i - 1) { s = i - 1; }
	}
	DrawFormatString(0, VIEW_VIEW_Y + s * 18, GetColor(100, 255, 255), "%d", s);//選択点の表示

	if (Keyboard_Get(KEY_INPUT_D) == 1) {//Dが押されていたら
		changescene(scene_delete);//消去
	}
	if (Keyboard_Get(KEY_INPUT_A) == 1) {//Aが押されていたら
		changescene(scene_add);//追加
	}

}

//項目追加
void add_main() {
	signed int hours = 0;//時入力
	signed int minutes = 0;//分入力
	signed int add = 0;//時分入力

	 //容量限界だったら
	if (i == LIST_MAX - 1) {
		changescene(scene_view);
		return;
	}

	//時刻入力
	DrawFormatString(0, 0, GetColor(255, 255, 255), "時刻を入力せよ");
	hours = (KeyInputNumber(0, 20, 23, 0, TRUE));//0時～23時まで

	//分入力
	DrawFormatString(0, 100, GetColor(255, 255, 255), "分を入力せよ");
	minutes = (KeyInputNumber(0, 120, 59, 0, TRUE));//0分～59まで

	if (hours == -1 || hours == 24 || minutes == -1 || minutes == 60) {//キャンセルまたはエラー
		changescene(scene_view);
		return;
	}

	//時分
	add = hours * 100 + minutes;

	//iのflagを0にして、s～iのdataをそれぞれ右に移す。
	LIST[i].flag = 0;//flag0を右に1こ増設＝項目数を1増やす

	//項目数が0ではない時だけ
	int p = i - 1;//作業用変数。これがs-1までたどる

	while (p >= s) {//sまで
		LIST[p + 1].data = LIST[p].data;//p+1のdataをp+1より１個左のdataにする
		LIST[p + 1].rang = LIST[p].rang;//p+1のrangをp+1より１個左のrangにする
		p--;//pを１個左にずらす
	}

	if (i == 0) {
		LIST[0].data = add;
		LIST[0].rang = 0;
	}
	else {
		LIST[s].data = add;
		LIST[s].rang = 0;
	}
	changescene(scene_view);
}

//消去
void delete_main() {
	if (i == 0) {//項目がなかったら
		changescene(scene_view);
		return;
	}
	//s内のdataを消去、i-1のflagを1にして、s～i-1のdataをそれぞれ左に移す。
	LIST[s].data = 0;//選択中のdataを0に
	LIST[i - 1].flag = 1;//flag1を左に1個増設＝項目数を1減らす

	int p = s;//作業用変数。これがi-2までたどる

	while (p < i - 1) {//flag0の末端の手前＝i-1まで
		LIST[p].data = LIST[p + 1].data;//pのdataをpより１個右のdataにする
		LIST[p].rang = LIST[p + 1].rang;//pのrangをpより１個右のrangにする
		p++;//pを１個右にずらす
	}
	LIST[i - 1].data = 0;//新しく末端となったもののdata処理
	LIST[i - 1].rang = 0;//新しく末端となったもののrang処理

	changescene(scene_view);
}

//鳴動
int BgColorClock=0;
void ringing_main() {
	//ここに飛ぶとき、iはdata==data_timeの要素通し番号を含む

	if (BgColorClock == 0) {
		BgColorClock = 30;
	}

	if (BgColorClock > 15) {
		DrawBox(0, 0, 640, 480, GetColor(255, 150, 150), TRUE);//赤い壁紙
	}
	else {
		DrawBox(0, 0, 640, 480, GetColor(0, 0, 0), TRUE);//黒い壁紙
	}

	DrawFormatString(0, 0, GetColor(255, 255, 255), "アラームでーす！！！Escで止める");
	
	BgColorClock--;

	if (Keyboard_Get(KEY_INPUT_ESCAPE) == 1) {
		changescene(scene_view);
	}
}

//鳴動完了rangのリセット　ファイルの書き込み
void end_main() {
	for (i = 0; i < LIST_MAX; i++) {
		LIST[i].rang = 0;//鳴動完了rangのリセット
	}
	FILE *filehandle;
	filehandle = fopen("list.txt", "w");
	fwrite(&LIST, sizeof(list_t), LIST_MAX, filehandle);//書き込み
	fclose(filehandle);//閉じる
}