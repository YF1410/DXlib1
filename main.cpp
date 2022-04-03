#include "DxLib.h"
#include "stdlib.h"
#include <cfloat>

// ウィンドウのタイトルに表示する文字列
const char TITLE[] = "学籍番号 名前:タイトル";

// ウィンドウ横幅
const int WIN_WIDTH = 600;

// ウィンドウ縦幅
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
	// ウィンドウモードに設定
	ChangeWindowMode(TRUE);

	// ウィンドウサイズを手動では変更させず、
	// かつウィンドウサイズに合わせて拡大できないようにする
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	// タイトルを変更
	SetMainWindowText(TITLE);

	// 画面サイズの最大サイズ、カラービット数を設定(モニターの解像度に合わせる)
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);

	// 画面サイズを設定(解像度との比率で設定)
	SetWindowSizeExtendRate(1.0);

	// 画面の背景色を設定する
	SetBackgroundColor(0x00, 0x00, 0xFF);

	// DXlibの初期化
	if (DxLib_Init() == -1) { return -1; }

	// (ダブルバッファ)描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);

	// 画像などのリソースデータの変数宣言と読み込み


	// ゲームループで使う変数の宣言
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

	// 最新のキーボード情報用
	char keys[256] = { 0 };

	// 1ループ(フレーム)前のキーボード情報
	char oldkeys[256] = { 0 };

	// ゲームループ
	while (1) {
		// 最新のキーボード情報だったものは1フレーム前のキーボード情報として保存
		// 最新のキーボード情報を取得
		GetHitKeyStateAll(keys);
		int mouse = GetMouseInput();
		GetMousePoint(&mousePosX, &mousePosZ);

		// 画面クリア
		ClearDrawScreen();
		//---------  ここからプログラムを記述  ----------//

		// 更新処理
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

		// 描画処理
		DrawLine(line.sPosX, line.sPosZ, mousePosX, mousePosZ, GetColor(255, 255, 255));
		DrawBox(box.minX, box.minZ, box.maxX, box.maxZ, color, true);
		//---------  ここまでにプログラムを記述  ---------//
		// (ダブルバッファ)裏面
		ScreenFlip();

		// 20ミリ秒待機(疑似60FPS)
		WaitTimer(20);

		// Windowsシステムからくる情報を処理する
		if (ProcessMessage() == -1) {
			break;
		}

		// ESCキーが押されたらループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {
			break;
		}
	}
	// Dxライブラリ終了処理
	DxLib_End();

	// 正常終了
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
				return false;	// 交差していない
		}
		else {
			// スラブとの距離を算出
			// t1が近スラブ、t2が遠スラブとの距離
			float odd = 1.0f / d[i];
			float t1 = (min[i] - sp[i]) * odd;
			float t2 = (max[i] - sp[i]) * odd;
			if (t1 > t2) {
				float tmp = t1; t1 = t2; t2 = tmp;
			}

			if (t1 > t) t = t1;
			if (t2 < t_max) t_max = t2;

			// スラブ交差チェック
			if (t >= t_max)
				return false;
		}
	}

	return true;
}