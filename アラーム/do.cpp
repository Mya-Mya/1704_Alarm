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
flag 1=���X�g�̏I�[�@0=���X�g����
rang 0=�܂��@1=����
*/

typedef struct {
	int flag;
	int data;
	int rang;
}list_t;
list_t LIST[LIST_MAX];

int i = 0;//���ڐ������[����
int s = 0;//�I�𒆂̗v�f�ʂ��ԍ������[����
int errorclock = 120;//�G���[�\���̃N���b�N

//���������[����ϐ��̏�����
time_t timer;
struct tm *t_st;
int data_time;//��*100+��

//�ϐ��̏������ƃt�@�C���ǂݍ���
void start_main() {
	//�ϐ��̏������A�S�Ă̗v�f��flag��1�ɁAdata��0�ɁB
	for (i = 0; i < LIST_MAX; i++) {
		LIST[i].flag = 1;
		LIST[i].data = 0;
		LIST[i].rang = 0;
	}
	//�t�@�C���̓��e����f�[�^�[�����[�h
	char *filename = "list.txt";
	FILE *filehandle;

	if ((filehandle = fopen(filename, "r")) == NULL) {
		DrawFormatString(0, 0, GetColor(255, 100, 100), "�t�@�C���I�[�v���G���[");
		changescene(scene_end);
	}
	else {
		///�t�@�C������f�[�^�[��ǂݍ���
		fread(&LIST, sizeof(list_t), LIST_MAX, filehandle);
		fclose(filehandle);///���
	}

	changescene(scene_view);
}

//�ꗗ�\��
void view_main() {

	//�����̎擾
	time(&timer);
	t_st = localtime(&timer);
	data_time = (t_st->tm_hour * 100 + t_st->tm_min);

	i = 0;
	while (LIST[i].flag != 1) {
		DrawFormatString(30, VIEW_VIEW_Y + i * 18, GetColor(200, 200, 200), "%d      %d", LIST[i].data, LIST[i].rang);//flag��0�ł���data��rang��\��
		
		if (LIST[i].rang != 1) {
			if (LIST[i].data == data_time) {//���ԂɂȂ��Ă���rang��0��������
				LIST[i].rang = 1;//�������ɂ���
				changescene(scene_ringing);//�����ɔ�ԂƂ��Ai��data==data_time�̗v�f�ʂ��ԍ����܂�
			}
		}
		i++;
	}

	DrawBox(0, 0, 640, 40, GetColor(50, 50, 50), TRUE);
	//�����ɗ����i�͍��ڐ������[���Ă���
	DrawFormatString(0, 0, GetColor(255, 255, 255), "�ꗗ�\�@D�ŏ����AA�Œǉ��A�����őI���@�S����%d���ځ@\n�����E�̐��l0�Ŗ����A1�Ŗ�����", i);

	if (s > i - 1) { s = i - 1; }
	if (s < 0) { s = 0; }

	if (Keyboard_Get(KEY_INPUT_DOWN) == 1) {//���L�[��������Ă�����
		if (i != 0) { s = (s + 1) % i; }//�I����Ԃ��������
		if (s < 0) { s = 0; }
	}
	if (Keyboard_Get(KEY_INPUT_UP) == 1) {//��L�[��������Ă�����
		if (i != 0) { s = (s + (i - 1)) % i; }//�I����Ԃ��P�グ��
		if (s > i - 1) { s = i - 1; }
	}
	DrawFormatString(0, VIEW_VIEW_Y + s * 18, GetColor(100, 255, 255), "%d", s);//�I��_�̕\��

	if (Keyboard_Get(KEY_INPUT_D) == 1) {//D��������Ă�����
		changescene(scene_delete);//����
	}
	if (Keyboard_Get(KEY_INPUT_A) == 1) {//A��������Ă�����
		changescene(scene_add);//�ǉ�
	}

}

//���ڒǉ�
void add_main() {
	signed int hours = 0;//������
	signed int minutes = 0;//������
	signed int add = 0;//��������

	 //�e�ʌ��E��������
	if (i == LIST_MAX - 1) {
		changescene(scene_view);
		return;
	}

	//��������
	DrawFormatString(0, 0, GetColor(255, 255, 255), "��������͂���");
	hours = (KeyInputNumber(0, 20, 23, 0, TRUE));//0���`23���܂�

	//������
	DrawFormatString(0, 100, GetColor(255, 255, 255), "������͂���");
	minutes = (KeyInputNumber(0, 120, 59, 0, TRUE));//0���`59�܂�

	if (hours == -1 || hours == 24 || minutes == -1 || minutes == 60) {//�L�����Z���܂��̓G���[
		changescene(scene_view);
		return;
	}

	//����
	add = hours * 100 + minutes;

	//i��flag��0�ɂ��āAs�`i��data�����ꂼ��E�Ɉڂ��B
	LIST[i].flag = 0;//flag0���E��1�����݁����ڐ���1���₷

	//���ڐ���0�ł͂Ȃ�������
	int p = i - 1;//��Ɨp�ϐ��B���ꂪs-1�܂ł��ǂ�

	while (p >= s) {//s�܂�
		LIST[p + 1].data = LIST[p].data;//p+1��data��p+1���P����data�ɂ���
		LIST[p + 1].rang = LIST[p].rang;//p+1��rang��p+1���P����rang�ɂ���
		p--;//p���P���ɂ��炷
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

//����
void delete_main() {
	if (i == 0) {//���ڂ��Ȃ�������
		changescene(scene_view);
		return;
	}
	//s����data�������Ai-1��flag��1�ɂ��āAs�`i-1��data�����ꂼ�ꍶ�Ɉڂ��B
	LIST[s].data = 0;//�I�𒆂�data��0��
	LIST[i - 1].flag = 1;//flag1������1���݁����ڐ���1���炷

	int p = s;//��Ɨp�ϐ��B���ꂪi-2�܂ł��ǂ�

	while (p < i - 1) {//flag0�̖��[�̎�O��i-1�܂�
		LIST[p].data = LIST[p + 1].data;//p��data��p���P�E��data�ɂ���
		LIST[p].rang = LIST[p + 1].rang;//p��rang��p���P�E��rang�ɂ���
		p++;//p���P�E�ɂ��炷
	}
	LIST[i - 1].data = 0;//�V�������[�ƂȂ������̂�data����
	LIST[i - 1].rang = 0;//�V�������[�ƂȂ������̂�rang����

	changescene(scene_view);
}

//��
int BgColorClock=0;
void ringing_main() {
	//�����ɔ�ԂƂ��Ai��data==data_time�̗v�f�ʂ��ԍ����܂�

	if (BgColorClock == 0) {
		BgColorClock = 30;
	}

	if (BgColorClock > 15) {
		DrawBox(0, 0, 640, 480, GetColor(255, 150, 150), TRUE);//�Ԃ��ǎ�
	}
	else {
		DrawBox(0, 0, 640, 480, GetColor(0, 0, 0), TRUE);//�����ǎ�
	}

	DrawFormatString(0, 0, GetColor(255, 255, 255), "�A���[���Ł[���I�I�IEsc�Ŏ~�߂�");
	
	BgColorClock--;

	if (Keyboard_Get(KEY_INPUT_ESCAPE) == 1) {
		changescene(scene_view);
	}
}

//������rang�̃��Z�b�g�@�t�@�C���̏�������
void end_main() {
	for (i = 0; i < LIST_MAX; i++) {
		LIST[i].rang = 0;//������rang�̃��Z�b�g
	}
	FILE *filehandle;
	filehandle = fopen("list.txt", "w");
	fwrite(&LIST, sizeof(list_t), LIST_MAX, filehandle);//��������
	fclose(filehandle);//����
}