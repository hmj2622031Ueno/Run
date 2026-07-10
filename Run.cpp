#include "DxLib.h"
#include<stdlib.h>

// 定数の定義
const int WIDTH = 960, HEIGHT = 640;	// ウィンドウの幅と高さのピクセル数
enum { TITLE, HELP, PLAY, RESULT};	// シーンを分けるための列挙定数

int catX = 0, catY = 472;
int targetY = 472;
bool reverse = false;
bool spaceOld = false;
bool isJump = false;
bool upOld = false;
bool isChanging = false;
bool canReverse = true;
int jumpSpeed = 0;
int obstacleX = WIDTH;
int obstacleType = GetRand(1);	// 0=低い 1=高い
int obstacleImage = GetRand(2);	// 0～2
int obstacleSide = GetRand(1);	// 0=上 1=下
int timer = 0;
int obstacleY;
int obstacleWidth;
int obstacleHeight;
int resultTimer = 0;
int meter = 0;
int meterTimer = 0;
int LoadGraphWithCheck(const char* file);

void GameInit()
{
	catX = 0;
	catY = 472;
	targetY = 472;
	reverse = false;
	isJump = false;
	isChanging = false;
	spaceOld = false;
	upOld = false;
	jumpSpeed = 0;
	obstacleX = WIDTH;
	obstacleType = GetRand(1);
	obstacleImage = GetRand(2);
	obstacleSide = GetRand(1);
	timer = 0;
	resultTimer = 0;
	meter = 0;
	meterTimer = 0;

	if (obstacleType == 0)
	{
		if (obstacleSide == 0) { obstacleY = 80; }
		else { obstacleY = 510; }
		obstacleHeight = 50;
	}
	else
	{
		if (obstacleSide == 0) { obstacleY = 80; }
		else { obstacleY = 310; }
		obstacleHeight = 250;
	}

	if (obstacleImage == 0) { obstacleWidth = 50; }
	else if (obstacleImage == 1) { obstacleWidth = 100; }
	else { obstacleWidth = 150; }
}

int LoadGraphWithCheck(const char* file)
{
	int res = LoadGraph(file);
	if (res == -1) { MessageBox(GetMainWindowHandle(), file, "画像読み込みに失敗", MB_OK | MB_ICONSTOP); }
	return res;
}

int LoadSoundMemWithCheck(const char* file)
{
	int res = LoadSoundMem(file);
	if (res == -1) { MessageBox(GetMainWindowHandle(), file, "音声読み込みに失敗", MB_OK | MB_ICONSTOP); }
	return res;
}

void DrawText(int x, int y, int col, const char* txt, int val, int siz)
{
	SetFontSize(siz);
	DrawFormatString(x + 2, y + 2, 0x000000, txt, val);
	DrawFormatString(x, y, col, txt, val);
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetWindowText("ランゲーム");	// ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32);	// ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true);	// ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1;	// ライブラリ初期化　エラーが起きたら終了
	SetBackgroundColor(0, 0, 0);	// 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK);	// 描画面を裏画面にする

	int imgSky = LoadGraphWithCheck("image/sky.png");
	int imgBG = LoadGraphWithCheck("image/backGround.png");
	int imgResult = LoadGraphWithCheck("image/result.png");
	int imgCat[2] = {
		LoadGraphWithCheck("image/catWalkA.png"),
		LoadGraphWithCheck("image/catWalkB.png")
	};
	int imgJump = LoadGraphWithCheck("image/catJump.png");
	int imgLO[3] = {
		LoadGraphWithCheck("image/lowObstacle.png"),
		LoadGraphWithCheck("image/lowObstacle1.png"),
		LoadGraphWithCheck("image/lowObstacle2.png")
	};
	int imgHO[3] = {
		LoadGraphWithCheck("image/highObstacle.png"),
		LoadGraphWithCheck("image/highObstacle1.png"),
		LoadGraphWithCheck("image/highObstacle2.png")
	};

	int sndTitle = LoadSoundMemWithCheck("sound/title.wav");
	int sndBgm = LoadSoundMemWithCheck("sound/play.mp3");
	int sndResult = LoadSoundMemWithCheck("sound/result.mp3");
	int seJump = LoadSoundMemWithCheck("sound/jump.mp3");
	int seDamage = LoadSoundMemWithCheck("sound/damage.wav");


	int bgX = 0;	// 背景スクロール用の変数
	int titleBgX = 0;
	int scene = TITLE;

	if (obstacleType == 0)
	{
		if (obstacleSide == 0)
		{
			obstacleY = 80;
		}
		else
		{
			obstacleY = 510;
		}
	}
	else
	{
		if (obstacleSide == 0)
		{
			obstacleY = 80;
		}
		else
		{
			obstacleY = 310;
		}
	}

	if (obstacleType == 0)	// 低い障害物
	{
		obstacleHeight = 50;

		if (obstacleImage == 0) { obstacleWidth = 50; }
		else if (obstacleImage == 1) { obstacleWidth = 100; }
		else { obstacleWidth = 150; }
	}
	else	// 高い障害物
	{
		obstacleHeight = 250;

		if (obstacleImage == 0) { obstacleWidth = 50; }
		else if (obstacleImage == 1) { obstacleWidth = 100; }
		else { obstacleWidth = 150; }
	}

	GameInit();	// 初期化用関数
	PlaySoundMem(sndTitle, DX_PLAYTYPE_LOOP);

	while (1)	// メインループ
	{
		ClearDrawScreen();	// 画面をクリアする
		timer++;
		bool spaceNow = CheckHitKey(KEY_INPUT_SPACE);
		bool upNow = CheckHitKey(KEY_INPUT_UP);

		switch (scene)
		{
		case TITLE:
			DrawGraph(0, 0, imgSky, false);

			bgX = bgX - 5;
			if (bgX <= -WIDTH) { bgX = 0; }
			DrawGraph(bgX, 0, imgBG, true);
			DrawGraph(bgX + WIDTH, 0, imgBG, true);
			DrawText(300, 50, 0xffffff, "Run Game", 0, 80);
			DrawText(300, 250, 0xffffff, "エンターキーでスタート", 0, 30);
			DrawText(800, 30, 0xffffff, "ヘルプ(Hキー)", 0, 20);
			if (CheckHitKey(KEY_INPUT_RETURN) == 1)	// エンターキー入力でスタート
			{
				StopSoundMem(sndTitle);
				PlaySoundMem(sndBgm, DX_PLAYTYPE_LOOP);
				scene = PLAY;
			}
			else if (CheckHitKey(KEY_INPUT_H))
			{
				StopSoundMem(sndTitle);
				scene = HELP;
			}
			break;

		case HELP:
			DrawText(330, 30, 0xffffff, "操作説明", 0, 60);
			DrawText(200, 170, 0xffffff, "・上キー　:　ジャンプ", 0, 30);
			DrawText(200, 250, 0xffffff, "・スペースキー　:　上下反転", 0, 30);
			DrawText(200, 330, 0xffffff, "・Tキー　:　タイトル画面に戻る", 0, 30);
			DrawText(200, 410, 0xffffff, "・Rキー　:　リスタート", 0, 30);
			if (CheckHitKey(KEY_INPUT_RETURN) == 1)
			{
				PlaySoundMem(sndBgm, DX_PLAYTYPE_LOOP);
				scene = PLAY;
			}
			break;

		case PLAY:
			// 空の表示
			DrawGraph(0, 0, imgSky, false);

			meterTimer++;
			if (meterTimer >= 15)
			{
				meter++;
				meterTimer = 0;
			}

			// 地面の表示、スクロール
			bgX = bgX - 15;
			if (bgX <= -WIDTH) { bgX = 0; }
			DrawGraph(bgX, 0, imgBG, true);
			DrawGraph(bgX + WIDTH, 0, imgBG, true);
			DrawText(850, 20, 0xffffff, "%dm", meter, 30);

			if (obstacleType == 0)
			{
				if (obstacleX < 600 && obstacleX > -obstacleWidth)
				{
					canReverse = false;
				}
			}
			else { canReverse = true; }

			// プレイヤーの反転処理
			if (spaceNow && !spaceOld && !isJump && !isChanging && canReverse)
			{
				reverse = !reverse;
				if (reverse) { targetY = 80; }
				else { targetY = 472; }
				isChanging = true;
			}
			spaceOld = spaceNow;

			// プレイヤーのジャンプ処理
			if (upNow && !isJump && !upOld && !isChanging)
			{
				isJump = true;
				PlaySoundMem(seJump, DX_PLAYTYPE_BACK);
				if (!reverse) { jumpSpeed = -15; }	// 上にジャンプ
				else { jumpSpeed = 15; }	// 下にジャンプ
			}

			if (isJump)
			{
				if (!reverse) { jumpSpeed += 1; }
				else { jumpSpeed -= 1; }
				catY += jumpSpeed;
			}

			// プレイヤーの着地設定
			if (!reverse)
			{
				if (catY >= 472)
				{
					catY = 472;
					jumpSpeed = 0;
					isJump = false;
				}
			}
			else
			{
				if (catY <= 80)
				{
					catY = 80;
					jumpSpeed = 0;
					isJump = false;
				}
			}
			upOld = upNow;

			if (isChanging)
			{
				if (catY < targetY)
				{
					catY += 16;
					if (catY > targetY) { catY = targetY; }
				}
				else if (catY > targetY)
				{
					catY -= 16;
					if (catY < targetY) { catY = targetY; }
				}
				else { isChanging = false; }
			}

			// プレイヤーの表示
			if (isJump)
			{
				if (!reverse) { DrawGraph(catX, catY, imgJump, true); }
				else { DrawRotaGraph3(catX + 48, catY + 48, 48, 48, 1.0, -1.0, 0.0, imgJump, true); }
			}
			else
			{
				if (!reverse) { DrawGraph(catX, catY, imgCat[(timer / 3) % 2], true); }
				else { DrawRotaGraph3(catX + 48, catY + 48, 48, 48, 1.0, -1.0, 0.0, imgCat[(timer / 3) % 2], true); }
			}

			// 障害物の処理
			obstacleX -= 15;	// スクロール

			if (obstacleX <= -150)
			{
				obstacleX = WIDTH;

				obstacleType = GetRand(1);
				obstacleImage = GetRand(2);
				obstacleSide = GetRand(1);

				if (obstacleType == 0)	// 低い障害物
				{
					if (obstacleSide == 0)	// 上側
					{
						obstacleY = 80;
					}
					else	// 下側
					{
						obstacleY = 510;
					}
				}
				else	// 高い障害物
				{
					if (obstacleSide == 0)	// 上側
					{
						obstacleY = 80;
					}
					else	// 下側
					{
						obstacleY = 310;
					}
				}
			}

			if (obstacleType == 0)	// 低い障害物
			{
				DrawGraph(obstacleX, obstacleY, imgLO[obstacleImage], true);
			}
			else	// 高い障害物
			{
				DrawGraph(obstacleX, obstacleY, imgHO[obstacleImage], true);
			}

			if (obstacleType == 0)	// 低い障害物
			{
				obstacleHeight = 50;

				if (obstacleImage == 0) { obstacleWidth = 50; }
				else if (obstacleImage == 1) { obstacleWidth = 100; }
				else { obstacleWidth = 150; }
			}
			else	// 高い障害物
			{
				obstacleHeight = 250;

				if (obstacleImage == 0) { obstacleWidth = 50; }
				else if (obstacleImage == 1) { obstacleWidth = 100; }
				else { obstacleWidth = 150; }
			}

			if (catX < obstacleX + obstacleWidth && catX + 96 > obstacleX && catY < obstacleY + obstacleHeight && catY + 96 > obstacleY)
			{
				StopSoundMem(sndBgm);
				PlaySoundMem(seDamage, DX_PLAYTYPE_BACK);
				PlaySoundMem(sndResult, DX_PLAYTYPE_LOOP);
				scene = RESULT;
			}

			if (CheckHitKey(KEY_INPUT_T))
			{
				GameInit();
				StopSoundMem(sndBgm);
				PlaySoundMem(sndTitle, DX_PLAYTYPE_LOOP);
				scene = TITLE;
			}
			break;

		case RESULT:
			resultTimer++;
			DrawGraph(0, 0, imgResult, false);

			if (resultTimer >= 30) { DrawText(330, 50, 0xff8000, "RESULT", 0, 80); }
			if (resultTimer >= 90)
			{
				DrawText(330, 200, 0xffB300, "記録 : %dm", meter, 50);
			}
			if (resultTimer >= 120) { DrawText(320, 320, 0xffB300, "Rキーでリスタート", 0, 30); }
			if (resultTimer >= 150) { DrawText(270, 370, 0xffB300, "Tキーでタイトル画面に戻る", 0, 30); }
			if (CheckHitKey(KEY_INPUT_R))
			{
				StopSoundMem(sndResult);
				GameInit();
				PlaySoundMem(sndBgm, DX_PLAYTYPE_LOOP);
				scene = PLAY;
			}
			if (CheckHitKey(KEY_INPUT_T)) {
				GameInit();
				StopSoundMem(sndResult);
				PlaySoundMem(sndTitle, DX_PLAYTYPE_LOOP);
				scene = TITLE;
			}
			break;
		}

		ScreenFlip();	// 裏画面の内容を表画面に反映させる
		WaitTimer(16);	// 一定時間待つ
		if (ProcessMessage() == -1) { break; }	// Windows から情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) { break; }	// ESCキーが押されたr終了
	}
	DxLib_End();	// DXライブラリ使用の終了処理
	return 0;
}