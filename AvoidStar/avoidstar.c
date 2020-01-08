#include<stdio.h>
#include<windows.h>
#include<stdbool.h>
#include<conio.h>
#include<stdlib.h>
#include <mmsystem.h> 

#pragma comment(lib,"winmm.lib")

#define SCR_WIDTH 69 //콘솔창 가로
#define SCR_HEIGHT 30 //콘솔창 세로
#define Enemy_Num 25 //별의 개수
#define Rand_Num 15 //별 y위치 난수
#define Speed 80 //별이 떨어지는 속도

typedef struct //공용 구조체
{
	int state_; //캐릭터의 상태
	int width_; //캐릭터의 가로
	int height_; //캐릭터의 세로

	int pos_x_; //x축 위치
	int pos_y_; //y축 위치

	int speed; //별의 속도

	char* image_;
}Object, * pObject;

Object player;
Object enemy[Enemy_Num];

int score; //점수
int life; //목숨

//더블 버퍼링
char front_buffer[SCR_HEIGHT][SCR_WIDTH]; //현재 화면 상황
char back_buffer[SCR_HEIGHT][SCR_WIDTH]; //작업할 화면

void moveCursorTo(const int x, const int y);// 좌표이동함수

void drawToBackBuffer(const int i, const int j, char* image) //가상의 도화지
{
	int ix = 0;
	while (1)
	{
		if (image[ix] == '\0')break;

		back_buffer[j][i + ix] = image[ix];

		ix++;
	}
}

void render()//컴퓨터 그래픽을 만드는 단계
{
	//바뀐 부분을 다시 그림
	for (int j = 0; j < SCR_HEIGHT; j++)
		for (int i = 0; i < SCR_WIDTH; i++)
		{
			if (back_buffer[j][i] != front_buffer[j][i])
			{
				moveCursorTo(i, j);

				if (back_buffer[j][i] == '\0')
					printf("%c", ' ');
				else
					printf("%c", back_buffer[j][i]);
			}
		}
	//update frame buffer
	for (int j = 0; j < SCR_HEIGHT; j++)
		for (int i = 0; i < SCR_WIDTH; i++)
		{
			front_buffer[j][i] = back_buffer[j][i];
			back_buffer[j][i] = '\0';
		}
}

void moveCursorTo(const int x, const int y) //좌표 이동 함수
{
	const COORD pos = { x,SCR_HEIGHT - y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void setcolor(int color, int bgcolor) //색깔 변화
{
	color &= 0xf;
	bgcolor &= 0xf;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bgcolor << 4) | color);
}

void drawBoundary() //게임 공간
{
	int i = 0;

	for (int j = 0; j < SCR_WIDTH; j++)
	{
		drawToBackBuffer(i, j, "|");
	}

	i = SCR_WIDTH - 1;
	for (int j = 0; j < SCR_HEIGHT; j++)
	{
		drawToBackBuffer(i, j, "|");
	}
}

void drawAll()//그리는 함수 모음
{
	//draw boundary
	drawBoundary();

	//draw player
	drawToBackBuffer(player.pos_x_, player.pos_y_, player.image_);

	//draw enemy[i]
	for (int i = 0; i < Enemy_Num; i++)
	{
		char* current_state_image = enemy[i].image_ + (enemy[i].width_ + 1) * enemy[i].state_;

		drawToBackBuffer(enemy[i].pos_x_, enemy[i].pos_y_, current_state_image);
	}


	char* current_state_image = player.image_ + (player.width_ + 1) * player.state_;

	drawToBackBuffer(player.pos_x_, player.pos_y_, current_state_image);

	//점수 출력
	char scoretext[15];
	sprintf(scoretext, "Score: %d", score);
	drawToBackBuffer(1, 0, scoretext);

	//목숨 표시
	char lifepoint[15];
	sprintf(lifepoint, "Life: %d", life);
	drawToBackBuffer(15, 0, lifepoint);
}


void GameSpeed() //별이 떨어지는 속도
{
	if (score < 2000)
	{
		Sleep(Speed);
	}
	else if (score >= 2000 && score < 4000)
	{
		Sleep(Speed - 10); //70
	}
	else if (score >= 4000 && score < 6000)
	{
		Sleep(Speed - 20); //60
	}
	else if (score >= 6000 && score < 8000)
	{
		Sleep(Speed - 30); //50
	}
	else if (score >= 8000 && score < 10000)
	{
		Sleep(Speed - 40); //40
	}
	else if (score > 10000 && score < 12000)
	{
		Sleep(Speed - 50); //30
	}
	else if (score >= 12000)
	{
		Sleep(Speed - 60); //20
	}
}

int main()
{
	boolean Loop = true;
	//배경음악
	PlaySound(TEXT("main.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//콘솔창 크기
	system("mode con cols=69 lines=30");
	//버퍼 초기화
	for (int j = 0; j < SCR_HEIGHT; j++)
	{
		for (int i = 0; i < SCR_WIDTH; i++)
		{
			front_buffer[j][i] = '\0';
			back_buffer[j][i] = '\0';
		}
	}

	score = 0; //초기 점수
	life = 5; //목숨

	player.state_ = 0; //초기 상태
	player.image_ = "P\0X\0X\0X\0X\0X\0X\0X\0X\0X\0X\0X\0X\0X\0X"; //플레이어 상태
	player.width_ = 1;
	player.height_ = 1;
	player.pos_x_ = 32;
	player.pos_y_ = 1;

	//별 생성

	for (int i = 0; i < Enemy_Num; i++)
	{
		const int dice = rand() % (SCR_WIDTH - 2); //x좌표 난수
		const int dice2 = rand() % Rand_Num; //y좌표 난수

		enemy[i].image_ = "*"; //피할 물체
		enemy[i].width_ = 1;
		enemy[i].height_ = 1;
		enemy[i].pos_x_ = dice + 1;
		enemy[i].pos_y_ = dice2 + SCR_HEIGHT - (Rand_Num + 1);
	}

	//hide cursor
	CONSOLE_CURSOR_INFO cur_info;
	cur_info.dwSize = 1;
	cur_info.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cur_info);

	//게임 시작전 화면
	moveCursorTo(12, 20);
	printf("게임목표: 하늘에서 떨어지는 별을 피해 살아남자!");
	moveCursorTo(12, 17);
	printf("조작방법: 방향키  <-  -> ");
	moveCursorTo(12, 14);
	printf("주의사항: 별이 점점 빠르고 가깝게 떨어집니다!");
	moveCursorTo(20, 11);
	printf("아무 키나 누르면 시작합니다!");

	_getch();
	moveCursorTo(0, 0);
	system("cls");

	while (Loop) //main game loop
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //원래색

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			if (player.pos_x_ > 1)
				player.pos_x_--;
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			if (player.pos_x_ < SCR_WIDTH - player.width_ - 1)
				player.pos_x_++;
		}

		GameSpeed();

		if (player.state_ > 0)
		{
			player.state_--;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY); //빨간색
		}

		for (int i = 0; i < Enemy_Num; i++)
		{
			//move enemy[i]

			if (enemy[i].pos_y_ < SCR_HEIGHT - player.height_)
				enemy[i].pos_y_--;
			if (enemy[i].pos_y_ < player.height_)
				enemy[i].pos_y_ = player.height_;

			if (enemy[i].pos_y_ == 1) //별이 바닥에 닿을때
			{
				enemy[i].image_ = "";
				score = score + 5;
			}
			if (enemy[i].pos_y_ == player.pos_y_ && player.pos_x_ == enemy[i].pos_x_) //별과 플레이어 충돌시
			{
				enemy[i].image_ = "";
				if (life > 0)
					life--;
				player.state_ = 15;
				Beep(1000, 234);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY); //빨간색
			}

			if (strcmp(enemy[i].image_, "") == 0) //없어지면 다시 생성
			{
				const int dice = rand() % (SCR_WIDTH - 2);
				const int dice2 = rand() % Rand_Num;

				enemy[i].image_ = "*"; //피할 물체
				enemy[i].width_ = 1;
				enemy[i].height_ = 1;
				enemy[i].pos_x_ = dice + 1;
				if (score < 1000)
				{
					enemy[i].pos_y_ = dice2 + SCR_HEIGHT - (Rand_Num + 1);
				}
				else if (score >= 1000 && score < 2000)
				{
					enemy[i].pos_y_ = dice2 + SCR_HEIGHT - (Rand_Num + 2);
				}
				else if (score >= 2000 && score < 4000)
				{
					enemy[i].pos_y_ = dice2 + SCR_HEIGHT - (Rand_Num + 3);
				}
				else if (score >= 4000 && score < 6000)
				{
					enemy[i].pos_y_ = dice2 + SCR_HEIGHT - (Rand_Num + 4);
				}
				else if (score >= 6000 && score < 8000)
				{
					enemy[i].pos_y_ = dice2 + SCR_HEIGHT - (Rand_Num + 5);
				}
				else if (score >= 8000 && score < 10000)
				{
					enemy[i].pos_y_ = dice2 + SCR_HEIGHT - (Rand_Num + 6);
				}
				else if (score >= 10000)
				{
					enemy[i].pos_y_ = dice2 + SCR_HEIGHT - (Rand_Num + 7);
				}
			}

			drawAll();

			render();
		}

		if (life == 0)
		{
			PlaySound(NULL, 0, 0);
			Loop = false;
		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //원래색
	//종료화면
	system("cls");
	moveCursorTo(28, 17);
	printf("Game Over!");
	moveCursorTo(24, 14);
	printf("Your Score : %4d", score);
	_getch();
	moveCursorTo(0, 0);

	return 0;
}
