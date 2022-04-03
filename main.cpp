#include "DxLib.h"
#include "stdlib.h"
#include <cfloat>

// �E�B���h�E�̃^�C�g���ɕ\�����镶����
const char TITLE[] = "�w�Дԍ� ���O:�^�C�g��";

// �E�B���h�E����
const int WIN_WIDTH = 600;

// �E�B���h�E�c��
const int WIN_HEIGHT = 400;

struct Line {
	float sPosX;
	float sPosZ;
	float ePosX;
	float ePosZ;
};

struct Box {
	float minX;
	float minZ;
	float maxX;
	float maxZ;
};

bool a(const Line& line, const Box& box);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	// �E�B���h�E���[�h�ɐݒ�
	ChangeWindowMode(TRUE);

	// �E�B���h�E�T�C�Y���蓮�ł͕ύX�������A
	// ���E�B���h�E�T�C�Y�ɍ��킹�Ċg��ł��Ȃ��悤�ɂ���
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	// �^�C�g����ύX
	SetMainWindowText(TITLE);

	// ��ʃT�C�Y�̍ő�T�C�Y�A�J���[�r�b�g����ݒ�(���j�^�[�̉𑜓x�ɍ��킹��)
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);

	// ��ʃT�C�Y��ݒ�(�𑜓x�Ƃ̔䗦�Őݒ�)
	SetWindowSizeExtendRate(1.0);

	// ��ʂ̔w�i�F��ݒ肷��
	SetBackgroundColor(0x00, 0x00, 0xFF);

	// DXlib�̏�����
	if (DxLib_Init() == -1) { return -1; }

	// (�_�u���o�b�t�@)�`���O���t�B�b�N�̈�͗��ʂ��w��
	SetDrawScreen(DX_SCREEN_BACK);

	// �摜�Ȃǂ̃��\�[�X�f�[�^�̕ϐ��錾�Ɠǂݍ���


	// �Q�[�����[�v�Ŏg���ϐ��̐錾
	int moveX = 0;
	int moveZ = 0;
	int mousePosX = 0;
	int mousePosZ = 0;
	int color = GetColor(255, 255, 255);

	Line line;
	Box box;
	box.minX = 200;
	box.minZ = 100;
	box.maxX = 400;
	box.maxZ = 300;

	// �ŐV�̃L�[�{�[�h���p
	char keys[256] = { 0 };

	// 1���[�v(�t���[��)�O�̃L�[�{�[�h���
	char oldkeys[256] = { 0 };

	// �Q�[�����[�v
	while (1) {
		// �ŐV�̃L�[�{�[�h��񂾂������̂�1�t���[���O�̃L�[�{�[�h���Ƃ��ĕۑ�
		// �ŐV�̃L�[�{�[�h�����擾
		GetHitKeyStateAll(keys);
		int mouse = GetMouseInput();
		GetMousePoint(&mousePosX, &mousePosZ);

		// ��ʃN���A
		ClearDrawScreen();
		//---------  ��������v���O�������L�q  ----------//

		// �X�V����
		color = GetColor(255, 0, 0);

		if (keys[KEY_INPUT_W]) {
			moveZ -= 2;
		}
		if (keys[KEY_INPUT_S]) {
			moveZ += 2;
		}
		if (keys[KEY_INPUT_A]) {
			moveX -= 2;
		}
		if (keys[KEY_INPUT_D]) {
			moveX += 2;
		}

		line.sPosX = moveX;
		line.sPosZ = moveZ;
		line.ePosX = mousePosX;
		line.ePosZ = mousePosZ;

		if (a(line, box)) {
			color = GetColor(255, 0, 0);
		}
		else {
			color = GetColor(255, 255, 255);
		}

		// �`�揈��
		DrawLine(line.sPosX, line.sPosZ, mousePosX, mousePosZ, GetColor(255, 255, 255));
		DrawBox(box.minX, box.minZ, box.maxX, box.maxZ, color, true);
		//---------  �����܂łɃv���O�������L�q  ---------//
		// (�_�u���o�b�t�@)����
		ScreenFlip();

		// 20�~���b�ҋ@(�^��60FPS)
		WaitTimer(20);

		// Windows�V�X�e�����炭�������������
		if (ProcessMessage() == -1) {
			break;
		}

		// ESC�L�[�������ꂽ�烋�[�v���甲����
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {
			break;
		}
	}
	// Dx���C�u�����I������
	DxLib_End();

	// ����I��
	return 0;
}

bool a(const Line& line, const Box& box) {
	float sp[2], ep[2], d[2], min[2], max[2];
	sp[0] = line.sPosX;
	//p[1] = line.start.y;
	sp[1] = line.sPosZ;

	ep[0] = line.ePosX;
	ep[1] = line.ePosZ;

	d[0] = ep[0] - sp[0];
	//d[1] = line.end.y - line.start.y;
	d[1] = ep[1] - sp[1];

	min[0] = box.minX;
	min[1] = box.minZ;

	max[0] = box.maxX;
	max[1] = box.maxZ;

	float t = -FLT_MAX;
	float t_max = FLT_MAX;

	if (d[0] < FLT_EPSILON) {
		float tmp = sp[0]; sp[0] = ep[0]; ep[0] = tmp;
	}

	if (d[1] < FLT_EPSILON) {
		float tmp = sp[1]; sp[1] = ep[1]; ep[1] = tmp;
	}

	d[0] = ep[0] - sp[0];
	//d[1] = line.end.y - line.start.y;
	d[1] = ep[1] - sp[1];

	//if (max[0] < p[0] || max[1] < p[1]) {
	//	return false;
	//}

	if ((max[0] < sp[0] || max[1] < sp[1]) || (ep[0] < min[0] || ep[1] < min[1])) {
		return false;
	}


	for (int i = 0; i < 2; ++i) {
		if (abs(static_cast<int>(d[i])) < FLT_EPSILON) {
			if (sp[i] < min[i] || sp[i] > max[i])
				return false;	// �������Ă��Ȃ�
		}
		else {
			// �X���u�Ƃ̋������Z�o
			// t1���߃X���u�At2�����X���u�Ƃ̋���
			float odd = 1.0f / d[i];
			float t1 = (min[i] - sp[i]) * odd;
			float t2 = (max[i] - sp[i]) * odd;
			if (t1 > t2) {
				float tmp = t1; t1 = t2; t2 = tmp;
			}

			if (t1 > t) t = t1;
			if (t2 < t_max) t_max = t2;

			// �X���u�����`�F�b�N
			if (t >= t_max)
				return false;
		}
	}

	return true;
}