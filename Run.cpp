#include "DxLib.h"

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	const int WIDTH = 960, HEIGHT = 640;	// ウィンドウの幅と高さのピクセル数
	SetWindowText("ランゲーム");	// ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32);	// ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true);	// ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1;	// ライブラリ初期化　エラーが起きたら終了
	SetBackgroundColor(0, 0, 0);	// 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK);	// 描画面を裏画面にする

	int imgSky = LoadGraph("image/sky.png");
	int imgBG = LoadGraph("image/backGround.png");
	int imgWB = LoadGraph("image/woodenBox.png");
	int imgCat[2] = {
		LoadGraph("image/catWalkA.png"),
		LoadGraph("image/catWalkB.png")
	};
	int catX = 0, catY = 472;
	int timer = 0;

	while (1)	// メインループ
	{
		ClearDrawScreen();	// 画面をクリアする
		DrawGraph(0, 0, imgSky, false);
		DrawGraph(0, 0, imgBG, true);
		DrawGraph(300, 510, imgWB, true);
		DrawGraph(catX, catY, imgCat[(timer / 3) % 2], true);
		timer++;

		ScreenFlip();	// 裏画面の内容を表画面に反映させる
		WaitTimer(16);	// 一定時間待つ
		if (ProcessMessage() == -1) { break; }	// Windows から情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) { break; }	// ESCキーが押されたr終了
	}
	DxLib_End();	// DXライブラリ使用の終了処理
	return 0;
}