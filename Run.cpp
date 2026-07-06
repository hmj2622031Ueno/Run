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

	int bgX = 0;	// 背景スクロール用の変数
	int imgSky = LoadGraph("image/sky.png");
	int imgBG = LoadGraph("image/backGround.png");
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
	int obstacleX = WIDTH;
	int obstacleType = GetRand(1);	// 0=低い 1=高い
	int obstacleImage = GetRand(2);	// 0～2
	int obstacleSide = GetRand(1);	// 0=上 1=下
	int obstacleY;
	int imgCat[2] = {
		LoadGraph("image/catWalkA.png"),
		LoadGraph("image/catWalkB.png")
	};
	int imgJump = LoadGraph("image/catJump.png");
	int jumpSpeed = 0;
	int catX = 0, catY = 472;
	int timer = 0;
	int targetY = 472;
	bool reverse = false;
	bool spaceOld = false;
	bool isJump = false;
	bool upOld = false;
	bool isChanging = false;

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

	while (1)	// メインループ
	{
		ClearDrawScreen();	// 画面をクリアする

		// 空の表示
		DrawGraph(0, 0, imgSky, false);

		// 地面の表示、スクロール
		bgX = bgX - 10;
		if (bgX <= -WIDTH) { bgX = 0; }
		DrawGraph(bgX, 0, imgBG, true);
		DrawGraph(bgX + WIDTH, 0, imgBG, true);
		if (CheckHitKey(KEY_INPUT_LSHIFT) || CheckHitKey(KEY_INPUT_RSHIFT))
		{
			bgX = bgX - 10;
			if (bgX <= -WIDTH) { bgX = 0; }
			DrawGraph(bgX, 0, imgBG, true);
			DrawGraph(bgX + WIDTH, 0, imgBG, true);
		}

		bool spaceNow = CheckHitKey(KEY_INPUT_SPACE);
		if (spaceNow && !spaceOld && !isJump && !isChanging)
		{
			reverse = !reverse;
			if (reverse) { targetY = 80; }
			else { targetY = 472; }
			isChanging = true;
		}
		spaceOld = spaceNow;

		// プレイヤーのジャンプ処理
		bool upNow = CheckHitKey(KEY_INPUT_UP);
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
			timer++;
		}

		// 障害物の処理
		obstacleX -= 10;	// スクロール

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
			obstacleX -= 10;	// スクロール

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

		ScreenFlip();	// 裏画面の内容を表画面に反映させる
		WaitTimer(16);	// 一定時間待つ
		if (ProcessMessage() == -1) { break; }	// Windows から情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) { break; }	// ESCキーが押されたr終了
	}
	DxLib_End();	// DXライブラリ使用の終了処理
	return 0;
}