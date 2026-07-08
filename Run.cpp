#include "DxLib.h"
#include<stdlib.h>

// 定数の定義
const int WIDTH = 960, HEIGHT = 640;	// ウィンドウの幅と高さのピクセル数
enum { TITLE, PLAY, OVER, CLEAR };	// シーンを分けるための列挙定数

int catX = 0, catY = 472;
int targetY = 472;
bool reverse = false;
bool spaceOld = false;
bool isJump = false;
bool upOld = false;
bool isChanging = false;
int jumpSpeed = 0;
int obstacleX = WIDTH;
int obstacleType = GetRand(1);	// 0=低い 1=高い
int obstacleImage = GetRand(2);	// 0～2
int obstacleSide = GetRand(1);	// 0=上 1=下
int timer = 0;
int obstacleY;
int obstacleWidth;
int obstacleHeight;

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

	int imgSky = LoadGraph("image/sky.png");
	int imgBG = LoadGraph("image/backGround.png");
	int imgCat[2] = {
		LoadGraph("image/catWalkA.png"),
		LoadGraph("image/catWalkB.png")
	};
	int imgJump = LoadGraph("image/catJump.png");
	int imgLO[3] = {
		LoadGraph("image/lowObstacle.png"),
		LoadGraph("image/lowObstacle1.png"),
		LoadGraph("image/lowObstacle2.png")
	};
	int imgHO[3] = {
		LoadGraph("image/highObstacle.png"),
		LoadGraph("image/highObstacle1.png"),
		LoadGraph("image/highObstacle2.png")
	};
	int imgClear = LoadGraph("image/clear.png");
	int imgOver = LoadGraph("image/over.png");

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
			DrawText(250, 320, 0xffffff, "Press the Enter key to start", 0, 30);
			if (CheckHitKey(KEY_INPUT_RETURN) == 1) { scene = PLAY; }	// エンターキー入力でスタート
			break;

		case PLAY:
			// 空の表示
			DrawGraph(0, 0, imgSky, false);

			// 地面の表示、スクロール
			bgX = bgX - 15;
			if (bgX <= -WIDTH) { bgX = 0; }
			DrawGraph(bgX, 0, imgBG, true);
			DrawGraph(bgX + WIDTH, 0, imgBG, true);
			if (CheckHitKey(KEY_INPUT_LSHIFT) || CheckHitKey(KEY_INPUT_RSHIFT))
			{
				bgX = bgX - 15;
				if (bgX <= -WIDTH) { bgX = 0; }
				DrawGraph(bgX, 0, imgBG, true);
				DrawGraph(bgX + WIDTH, 0, imgBG, true);
			}

			if (spaceNow && !spaceOld && !isJump && !isChanging)
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

			if (CheckHitKey(KEY_INPUT_LSHIFT) || CheckHitKey(KEY_INPUT_RSHIFT))
			{
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
				scene = OVER;
			}
			break;

		case CLEAR:
			DrawGraph(0, 0, imgClear, false);
			DrawText(300, 50, 0xffffff, "GAME CLEAR!!", 0, 80);
			if (CheckHitKey(KEY_INPUT_R)) { scene = PLAY; }
			if (CheckHitKey(KEY_INPUT_T)) { scene = TITLE; }
			break;

		case OVER:
			DrawGraph(0, 0, imgOver, false);
			DrawText(300, 50, 0xff0000, "GAME OVER", 0, 80);
			DrawText(280, 290, 0xff0000, "Press the R key to restart", 0, 30);
			DrawText(300, 370, 0xff0000, "Press the T key to title", 0, 30);
			if (CheckHitKey(KEY_INPUT_R))
			{
				scene = PLAY;
			}
			if (CheckHitKey(KEY_INPUT_T)) { scene = TITLE; }
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