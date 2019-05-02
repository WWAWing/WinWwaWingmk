#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <olectl.h>
#include "resource.h"
#include "cdib.hpp"
#include <atlstr.h>

//##------------------------------------------------------------------
// Visual Style �Ή�
#pragma comment(linker, \
	"\"/manifestdependency:type='Win32' "\
	"name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' "\
	"processorArchitecture='*' "\
	"publicKeyToken='6595b64144ccf1df' "\
	"language='*'\"")

//##------------------------------------------------------------------
// �萔��`

#define HIMETRIC_INCH 2540

#define END		-1

#define ATR_0		0
#define ATR_CROP1	1
#define ATR_CROP2	2
#define ATR_TYPE	3
#define ATR_MODE	4
#define ATR_STRING	5
#define ATR_X		6
#define ATR_Y		7
#define ATR_X2		8
#define ATR_Y2		9
#define ATR_ENERGY		10
#define ATR_STRENGTH	11
#define ATR_DEFENCE		12
#define ATR_GOLD		13
#define ATR_ITEM		14
#define ATR_NUMBER		15
#define ATR_JUMP_X		16
#define ATR_JUMP_Y		17
#define ATR_MOVE		16
#define ATR_SOUND		19

#define MAP_STREET		0
#define MAP_WALL		1
#define MAP_LOCALGATE	2
#define MAP_WORLDGATE	3
#define MAP_URLGATE		4
#define MAP_ITEMCHECK	5

#define OBJECT_NORMAL	0
#define OBJECT_MESSAGE	1
#define OBJECT_URLGATE	2
#define OBJECT_STATUS	3
#define OBJECT_ITEM		4
#define OBJECT_DOOR		5
#define OBJECT_MONSTER	6
#define OBJECT_SCORE	11
#define OBJECT_DISP_PASS	12
#define OBJECT_INPUT_PASS	13
#define OBJECT_SELL			14
#define OBJECT_BUY			15
#define OBJECT_RANDOM		16
#define OBJECT_SELECT		17
#define OBJECT_LOCALGATE	18

#define STRING_MAX		60
#define XY_TOP			60
#define YTOP			20

#define DATA_CHECK			0
#define DATA_VERSION		2
#define DATA_STATUS_ENERGY		10
#define DATA_STATUS_STRENGTH	12
#define DATA_STATUS_DEFENCE		14
#define DATA_STATUS_GOLD		16
#define DATA_ITEM				20
#define DATA_STATUS_ENERGYMAX	32

#define DATA_MAP_SIZE		46
#define DATA_MES_NUMBER		48

#define MAP_SIZE_MAX		1001
#define PARTS_NUMBER_MAX	4000
#define FILE_DATA_MAX		(4000 +1000 +1000) *1024	//�i�}�b�v�{�p�[�c�{���b�Z�[�W�̈�j
#define MESSAGE_NUMBER_MAX	5000

#define MESSAGE_STR_MAX		1500+10
#define MAP_ATR_MAX			60
#define OBJECT_ATR_MAX		60

// 1��ʂ�1�ӂ̃`�b�v�T�C�Y
#define SCREEN_CHIP_SIZE	21
#define DEFALUT_SCREEN_CHIP_SIZE 11
// 1�`�b�v�̃T�C�Y (�s�N�Z���P��)
#define CHIP_SIZE			40

// �p�[�c�I���_�C�A���O�ŕ\������p�[�c�s�� (���\�[�X�t�@�C���̕ύX���Y�ꂸ��)
#define DIALOG_OBJECT_SELECT_LINE	5
#define DIALOG_OBJECT_SELECT_COLUMN	10
#define DIALOG_MAP_SELECT_LINE		5
#define DIALOG_MAP_SELECT_COLUMN	10

int DATA_MAP_COUNT;
int DATA_OBJECT_COUNT;
int DATA_CHARA_X;
int DATA_CHARA_Y;
int DATA_OVER_X;
int DATA_OVER_Y;

//�L�����N�^�ʒu
int charaX, charaY;

//���b�Z�[�W�p�o�b�t�@
char g_StrMessage[MESSAGE_NUMBER_MAX][MESSAGE_STR_MAX];
char g_StrMessageSystem[20][MESSAGE_STR_MAX];
char g_worldName[100];
char g_worldPassword[100];
char g_mapcgName[100];
char g_mapcgNameBmp[100];
char g_mapcgOld[100];

//�}�b�v�ʒu
int mapX, mapY;
int mapXtop = 0;
int mapYtop = 0;

//�X�e�[�^�X
int statusEnergyMax;
int statusEnergy;
int statusStrength;
int statusDefence;
int statusGold;
int itemStrength;
int itemDefence;
int gameoverXp;
int gameoverYp;

//�}�b�v�f�[�^
short map[MAP_SIZE_MAX][MAP_SIZE_MAX];
short mapObject[MAP_SIZE_MAX][MAP_SIZE_MAX];
int objectAttribute[PARTS_NUMBER_MAX][OBJECT_ATR_MAX];
int mapAttribute[PARTS_NUMBER_MAX][MAP_ATR_MAX];
int pointer;
int itemBox[12];
int StockObjectAttribute[PARTS_NUMBER_MAX][OBJECT_ATR_MAX];
int StockMapAttribute[PARTS_NUMBER_MAX][MAP_ATR_MAX];
//Undo�p
short UndoMap[MAP_SIZE_MAX][MAP_SIZE_MAX];
short UndoMapObject[MAP_SIZE_MAX][MAP_SIZE_MAX];
int UndoObjectAttribute[PARTS_NUMBER_MAX][OBJECT_ATR_MAX];
int UndoMapAttribute[PARTS_NUMBER_MAX][MAP_ATR_MAX];

unsigned char *UndoMapB;

//���ʗp
int g_CopyObject[OBJECT_ATR_MAX];
int g_CopyMap[MAP_ATR_MAX];
char g_CopyObjectStr[MESSAGE_STR_MAX];
char g_CopyMapStr[MESSAGE_STR_MAX];
short g_MapBuffer[SCREEN_CHIP_SIZE][SCREEN_CHIP_SIZE];
short g_ObjectBuffer[SCREEN_CHIP_SIZE][SCREEN_CHIP_SIZE];

//���̑I��p
int g_SelectObjectX, g_SelectObjectY;
int g_ScrObject = 0;
int g_SelectObjectData;
int g_EditObjectData;
//�w�i�I��p
int g_SelectMapX, g_SelectMapY;
int g_ScrMap = 0;
int g_SelectMapData;
int g_EditMapData;
//�L�����N�^�b�f�I��p
int g_ScrCGChara = 0;
int g_ScrCGCharaMax = 10;
int g_hModeSelectChara;
int g_iLoadCGHeight = 800;

//�ėp
CDib	*g_pDib;
HINSTANCE	g_hInst;
HWND 	g_hWnd;
HWND	g_hDlgObject = NULL;
HWND	g_hDlgMap = NULL;
HWND	g_hDlgSelectChara = NULL;
HWND	g_hDlgExtra = NULL;
HWND	g_hDlgSelectObject = NULL;
HWND	g_hDlgSelectMap = NULL;
HWND	g_hDlgFoundation = NULL;
HWND	g_hDlgCalculate = NULL;
HWND	g_hDlgQuickView = NULL;
HWND	g_hDlgBasicMes = NULL;
HDC		g_hmDC = NULL;
HDC		g_hmDCHalf = NULL;
HDC		g_hmDCAnd = NULL;
HDC		g_hmDCOr = NULL;
HDC		g_hmDCExtra = NULL;
HBITMAP	g_hBitmap = NULL;
HBITMAP	g_hBitmapHalf = NULL;
HBITMAP	g_hBitmapGif = NULL;
HBITMAP	g_hBitmapAnd = NULL;
HBITMAP	g_hBitmapOr = NULL;
HBITMAP	g_hBitmapExtra = NULL;

int g_AtrSelectChara;	//�b�f�I��p
int g_EditMode;

//�t���O
BOOL g_bRestoreObjectDialog = FALSE;
BOOL g_bRestoreMapDialog = FALSE;
BOOL g_MapLineFlag = TRUE;		//���E��
BOOL g_ObjectNumberFlag = TRUE;	//���̃p�[�c�ԍ��\��
BOOL g_MouseDrag = FALSE;		//�}�E�X�̃h���b�O����p
BOOL g_bLoadGif = TRUE;			//GIF�t�@�C�����ǂݍ��߂邩�H
BOOL g_bUpdate = FALSE;			//�X�V�m�F�t���O
BOOL g_bInitial = FALSE;		//�������ς݂��H

BOOL g_bFileNotFound;
BOOL g_iColorTp;

BOOL g_hugeMapSize = FALSE; // MapSize��21 * 21���H

char g_MapData[FILE_DATA_MAX];
char PressData[FILE_DATA_MAX];

char g_szSettingFile[250] = "./WinWwamk.ini";	//�ݒ�t�@�C����
char g_szSelectFile[250] = "wwamap.dat";		//�t�@�C����
char g_szTitleName[] = "WWA Wing�}�b�v�쐬�c�[�� Ver3.1.8";
char g_szSelectDir[250];
int g_MouseX, g_MouseY;
int g_MouseDragX, g_MouseDragY;
int g_MouseOldX, g_MouseOldY;
BOOL g_bCancel = FALSE;

//�e����
int g_iMapSize = 501;
int g_iMapPartsMax = 200;
int g_iObjectPartsMax = 200;
int g_iMapAtrMax;
int g_iObjectAtrMax;
int g_iMesNumberMax;

//�Ïؔԍ��p
BOOL g_bErrorPassword = FALSE;
char g_szInputPassword[30];


//���̃f�[�^�p
struct StructOBJ {
	char *Name;
	int Object;
	int Id;
};
StructOBJ OBJ[] = {
	{ "�ʏ함��", OBJECT_NORMAL, IDD_DIALOG_NORMAL },
	{ "���b�Z�[�W", OBJECT_MESSAGE, IDD_DIALOG_MESSAGE },
	{ "�����X�^�[", OBJECT_MONSTER, IDD_DIALOG_MONSTER },
	{ "�A�C�e��", OBJECT_ITEM, IDD_DIALOG_ITEM },
	{ "��", OBJECT_DOOR, IDD_DIALOG_DOOR },
	{ "�X�e�[�^�X�ω�", OBJECT_STATUS, IDD_DIALOG_STATUS },
	{ "���𔄂�", OBJECT_SELL, IDD_DIALOG_SELL },
	{ "���𔃂�", OBJECT_BUY, IDD_DIALOG_BUY },
	{ "�t�q�k�Q�[�g", OBJECT_URLGATE, IDD_DIALOG_URLOBJ },
	{ "�X�R�A�\��", OBJECT_SCORE, IDD_DIALOG_SCORE },
	{ "�����_���I��", OBJECT_RANDOM, IDD_DIALOG_RANDOM },
	{ "��ґ���", OBJECT_SELECT, IDD_DIALOG_OBJSELECT },
	{ "�W�����v�Q�[�g", OBJECT_LOCALGATE, IDD_DIALOG_LOCALGATE_OBJ },
	{ "", END }
};
char *g_ObjectName[] = { "�ʏ함�́@�@","���b�Z�[�W�@�@","�t�q�k�ްā@","�ð���ω��@","�A�C�e���@�@","���@�@�@�@�@","�����X�^�[�@","","","","","�X�R�A�\���@","","","���𔄂�@�@","���𔃂��@�@","����ёI���@�@","��ґ���@�@","�ެ��߹ްā@" };
char *g_MapName[] = { "���@�@�@�@�@","�ǁ@�@�@�@�@","�ެ��߹ްā@","","�t�q�k�ްā@","���ѕ���@�@" };

StructOBJ MAP[] = {
	{ "��", MAP_STREET, IDD_DIALOG_STREET },
	{ "��", MAP_WALL, IDD_DIALOG_WALL },
	{ "�W�����v�Q�[�g", MAP_LOCALGATE, IDD_DIALOG_LOCALGATE },
	{ "�t�q�k�Q�[�g", MAP_URLGATE, IDD_DIALOG_URLMAP },
	{ "�A�C�e������", MAP_ITEMCHECK, IDD_DIALOG_ITEMCHECK },
	{ "", END }
};

//���̕ҏW�_�C�A���O�̓��e
struct StructObject {
	int Object;
	int Atr;
	int Id;
	int Max;
	char *Name;
};
StructObject Object[] = {
	{ OBJECT_NORMAL, ATR_MODE, IDC_COMBO_MODE, 1, "���̂̎��" },
	{ OBJECT_NORMAL, ATR_MOVE, IDC_COMBO_MOVE, 3, "���쑮��" },

	{ OBJECT_MESSAGE, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_MESSAGE, ATR_MOVE, IDC_COMBO_MOVE, 3, "���쑮��" },
	{ OBJECT_MESSAGE, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "��������A�C�e���ԍ�" },
	{ OBJECT_MESSAGE, ATR_NUMBER, IDC_EDIT_WAIT, 1000, "�҂�����" },

	{ OBJECT_SELL, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "�A�C�e���̕��̔ԍ�" },
	{ OBJECT_SELL, ATR_GOLD, IDC_EDIT_GOLD, 30000, "�̔����z" },
	{ OBJECT_SELL, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_SELL, ATR_ENERGY, IDC_EDIT_ENERGY, 30000, "������" },
	{ OBJECT_SELL, ATR_STRENGTH, IDC_EDIT_STRENGTH, 30000, "�U����" },
	{ OBJECT_SELL, ATR_DEFENCE, IDC_EDIT_DEFENCE, 30000, "�h���" },
	{ OBJECT_SELL, ATR_MOVE, IDC_COMBO_MOVE, 3, "���쑮��" },

	{ OBJECT_BUY, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "�A�C�e���̕��̔ԍ�" },
	{ OBJECT_BUY, ATR_GOLD, IDC_EDIT_GOLD, 30000, "���z" },
	{ OBJECT_BUY, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_BUY, ATR_MOVE, IDC_COMBO_MOVE, 3, "���쑮��" },

	{ OBJECT_ITEM, ATR_NUMBER, IDC_EDIT_NUMBER, 12, "�{�b�N�X�ւ̊i�[�ʒu" },
	{ OBJECT_ITEM, ATR_STRENGTH, IDC_EDIT_STRENGTH, 30000, "�U����" },
	{ OBJECT_ITEM, ATR_DEFENCE, IDC_EDIT_DEFENCE, 30000, "�h���" },
	{ OBJECT_ITEM, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_ITEM, ATR_MOVE, IDC_COMBO_MOVE, 3, "���쑮��" },
	{ OBJECT_ITEM, ATR_MODE, IDC_COMBO_ITEM, 3, "�g�p����" },

	{ OBJECT_DOOR, ATR_MODE, IDC_COMBO_MODE2, 3, "����" },
	{ OBJECT_DOOR, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "�A�C�e���̕��̔ԍ�" },
	{ OBJECT_DOOR, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_DOOR, ATR_MOVE, IDC_COMBO_MOVE, 3, "���쑮��" },
	{ OBJECT_DOOR, ATR_NUMBER, IDC_COMBO_MODE, 1, "�ʍs�敪" },

	{ OBJECT_STATUS, ATR_ENERGY, IDC_EDIT_ENERGY, 30000, "������" },
	{ OBJECT_STATUS, ATR_STRENGTH, IDC_EDIT_STRENGTH, 30000, "�U����" },
	{ OBJECT_STATUS, ATR_DEFENCE, IDC_EDIT_DEFENCE, 30000, "�h���" },
	{ OBJECT_STATUS, ATR_GOLD, IDC_EDIT_GOLD, 30000, "������" },
	{ OBJECT_STATUS, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_STATUS, ATR_MOVE, IDC_COMBO_MOVE, 3, "���쑮��" },

	{ OBJECT_SCORE, ATR_ENERGY, IDC_EDIT_ENERGY, 30000, "������" },
	{ OBJECT_SCORE, ATR_STRENGTH, IDC_EDIT_STRENGTH, 30000, "�U����" },
	{ OBJECT_SCORE, ATR_DEFENCE, IDC_EDIT_DEFENCE, 30000, "�h���" },
	{ OBJECT_SCORE, ATR_GOLD, IDC_EDIT_GOLD, 30000, "������" },
	{ OBJECT_SCORE, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },

	{ OBJECT_MONSTER, ATR_ENERGY, IDC_EDIT_ENERGY, 60000, "������" },
	{ OBJECT_MONSTER, ATR_STRENGTH, IDC_EDIT_STRENGTH, 60000, "�U����" },
	{ OBJECT_MONSTER, ATR_DEFENCE, IDC_EDIT_DEFENCE, 60000, "�h���" },
	{ OBJECT_MONSTER, ATR_GOLD, IDC_EDIT_GOLD, 30000, "������" },
	{ OBJECT_MONSTER, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "�A�C�e���̕��̔ԍ�" },
	{ OBJECT_MONSTER, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_MONSTER, ATR_MOVE, IDC_COMBO_MOVE, 3, "���쑮��" },

	{ OBJECT_RANDOM, 10, IDC_EDIT_RANDOM1, PARTS_NUMBER_MAX-1, "���̔ԍ�" },
	{ OBJECT_RANDOM, 11, IDC_EDIT_RANDOM2, PARTS_NUMBER_MAX-1, "���̔ԍ�" },
	{ OBJECT_RANDOM, 12, IDC_EDIT_RANDOM3, PARTS_NUMBER_MAX-1, "���̔ԍ�" },
	{ OBJECT_RANDOM, 13, IDC_EDIT_RANDOM4, PARTS_NUMBER_MAX-1, "���̔ԍ�" },
	{ OBJECT_RANDOM, 14, IDC_EDIT_RANDOM5, PARTS_NUMBER_MAX-1, "���̔ԍ�" },
	{ OBJECT_RANDOM, 15, IDC_EDIT_RANDOM6, PARTS_NUMBER_MAX-1, "���̔ԍ�" },
	{ OBJECT_RANDOM, 16, IDC_EDIT_RANDOM7, PARTS_NUMBER_MAX-1, "���̔ԍ�" },
	{ OBJECT_RANDOM, 17, IDC_EDIT_RANDOM8, PARTS_NUMBER_MAX-1, "���̔ԍ�" },
	{ OBJECT_RANDOM, 18, IDC_EDIT_RANDOM9, PARTS_NUMBER_MAX-1, "���̔ԍ�" },
	{ OBJECT_RANDOM, 19, IDC_EDIT_RANDOM10, PARTS_NUMBER_MAX-1, "���̔ԍ�" },

	{ OBJECT_SELECT, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_SELECT, ATR_MOVE, IDC_COMBO_MOVE, 3, "���쑮��" },

	{ OBJECT_LOCALGATE, ATR_JUMP_X, IDC_EDIT_X, 100, "�w���W" },
	{ OBJECT_LOCALGATE, ATR_JUMP_Y, IDC_EDIT_Y, 100, "�x���W" },
	{ OBJECT_LOCALGATE, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ END }
};

StructObject Map[] = {
	{ MAP_STREET, ATR_NUMBER, IDC_EDIT_WAIT, 1000, "�҂�����" },
	{ MAP_STREET, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ MAP_STREET, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "��������A�C�e���ԍ�" },

	{ MAP_LOCALGATE, ATR_JUMP_X, IDC_EDIT_X, 100, "�w���W" },
	{ MAP_LOCALGATE, ATR_JUMP_Y, IDC_EDIT_Y, 100, "�x���W" },
	{ MAP_LOCALGATE, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },

	{ MAP_ITEMCHECK, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "���肷��A�C�e���ԍ�" },
	{ MAP_ITEMCHECK, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ END }
};

const int g_EditId[] = {
	IDC_EDIT_CHARA,IDC_EDIT_CHARA2,IDC_EDIT_CHARA3,IDC_EDIT_CHARA4,IDC_EDIT_CHARA5,
	IDC_EDIT_CHARA6,IDC_EDIT_CHARA7,IDC_EDIT_CHARA8,IDC_EDIT_CHARA9,IDC_EDIT_CHARA10,
	IDC_EDIT_X,IDC_EDIT_X2,IDC_EDIT_X3,IDC_EDIT_X4,IDC_EDIT_X5,
	IDC_EDIT_X6,IDC_EDIT_X7,IDC_EDIT_X8,IDC_EDIT_X9,IDC_EDIT_X10,
	IDC_EDIT_Y,IDC_EDIT_Y2,IDC_EDIT_Y3,IDC_EDIT_Y4,IDC_EDIT_Y5,
	IDC_EDIT_Y6,IDC_EDIT_Y7,IDC_EDIT_Y8,IDC_EDIT_Y9,IDC_EDIT_Y10,
	IDC_BUTTON_MODE,IDC_BUTTON_MODE2,IDC_BUTTON_MODE3,IDC_BUTTON_MODE4,IDC_BUTTON_MODE5,
	IDC_BUTTON_MODE6,IDC_BUTTON_MODE7,IDC_BUTTON_MODE8,IDC_BUTTON_MODE9,IDC_BUTTON_MODE10,
};

#define MACRO_MB(x) MessageBox(NULL,x,"",MB_OK);
#define MACRO_NUMBER(x) if(TRUE){char szNum[20];sprintf(szNum,"%d",x);MACRO_MB(szNum)};



//##API------------------------------------------------------------------
// �֐��Q

//BOOL LibLoad();
// ���j���[�̃`�F�b�N���͂���
void DeleteCheckMenu();
// �}�b�v�f�[�^�̃��[�h
BOOL LoadMapData( char *FileName );
// �}�b�v�f�[�^�̕ۑ�
BOOL SaveMapData( char *FileName );
void saveMapString( char *str );
// �r�b�g�}�b�v�ǂݍ���
BOOL LoadBitmap();
void PaintWindow();
void paintMapAll( HDC hDC );
// �X�e�[�^�X�`��
void PaintStatus( BOOL flag );

// �I���_�C�A���O�v���V�[�W��
LRESULT CALLBACK SelectObjectDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK SelectMapDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
// �ҏW�_�C�A���O�v���V�[�W��
LRESULT CALLBACK EditObjectDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK EditMapDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
// �N�C�b�N�r���[�_�C�A���O�v���V�[�W��
LRESULT CALLBACK QuickViewDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
// �L�����N�^�b�f�I���_�C�A���O�v���V�[�W��
LRESULT CALLBACK SelectCGCharaProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
// ��{���b�Z�[�W�_�C�A���O�v���V�[�W��
LRESULT CALLBACK DialogProcBasicMes( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

// ���Ǝl�p�̕`��
void DrawLine( HDC hDC, int x, int y, int x2, int y2 );
void DrawRect( HDC hDC, int x, int y, int x2, int y2 );
void DrawRect2( HDC hDC, int x, int y, int x2, int y2 );

// �ҏW�_�C�A���O�̕\��
void DisplayObjectDialog();
void DisplayMapDialog();
// ���̃f�[�^�̌���
void SetObjectData( HWND hWnd, int charaNumber );
void SetMapData( HWND hWnd, int charaNumber );
// �g���o���L�����N�^�̐ݒ�
void AppearChara( int mapNumber, BOOL flag );
// �g���o���L�����N�^�̃f�[�^�L�^
void SetAppearChara( int mapNumber, BOOL flag );
// �_�C�A���O�v���V�[�W��
LRESULT CALLBACK DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
// �����f�[�^�̃Z�b�g
void SetMessageData( int  *point, char *str );

// �}�b�v�̐V�K�쐬
void MakeNewMap();
// ��{�ݒ�_�C�A���O
void EditMapFoundation();
// �p�X���[�h�_�C�A���O�v���V�[�W��
LRESULT CALLBACK DialogProcPassword( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

// �������p�A�f�[�^�̈ꎞ�ۑ�
void StockAttributeData( int number, int mode );
// �v�Z�_�C�A���O�̕\��
void CalculateDialog();
// �L�����N�^�ԍ��̎擾
int GetCharaNumber( HWND hWnd );

// �g���o���L�����N�^�_�C�A���O�v���V�[�W��
LRESULT CALLBACK DialogProcExtraObject( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK DialogProcExtraMap( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

// �g���q����菜�����t�@�C�������擾����
CString GetHtmlFileName();
// �u���E�U���N������
BOOL ExecBrowser();

// �w��L�[��������Ă����ꍇ��TRUE��Ԃ�
BOOL IsKeyDown( int virtKeyCode );

// Undo�@�\
void SetUndoData();
void RestoreUndoData();

//�������擾
// GIF�摜�̓ǂݍ���
BOOL ReadGifImage();


//##------------------------------------------------------------------
// �C�x���g����

LRESULT WINAPI MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	int x, y, x2, y2;
	int result;

	switch (message) {
		//�J�[�\���L�[����
	case WM_KEYDOWN: {
		if (LOWORD(wParam) == VK_DOWN) {
			if (mapYtop < g_iMapSize - SCREEN_CHIP_SIZE) ++mapYtop;
			SetScrollPos(g_hWnd, SB_VERT, mapYtop, 1);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		else if (LOWORD(wParam) == VK_UP) {
			if (mapYtop > 0) --mapYtop;
			SetScrollPos(g_hWnd, SB_VERT, mapYtop, 1);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		else if (LOWORD(wParam) == VK_RIGHT) {
			if (mapXtop < g_iMapSize - SCREEN_CHIP_SIZE) ++mapXtop;
			SetScrollPos(g_hWnd, SB_HORZ, mapXtop, 1);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		else if (LOWORD(wParam) == VK_LEFT) {
			if (mapXtop > 0) --mapXtop;
			SetScrollPos(g_hWnd, SB_HORZ, mapXtop, 1);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}
					 //�ŏ���
	case WM_SIZE: {
		if (wParam == SIZE_RESTORED) {
			ShowWindow(g_hDlgSelectObject, TRUE);
			ShowWindow(g_hDlgSelectMap, TRUE);
			ShowWindow(g_hDlgObject, TRUE);
			ShowWindow(g_hDlgMap, TRUE);
			ShowWindow(g_hDlgSelectChara, TRUE);
			ShowWindow(g_hDlgExtra, TRUE);
			ShowWindow(g_hDlgQuickView, TRUE);
		}
		else if (wParam == SIZE_MINIMIZED) {
			ShowWindow(g_hDlgSelectObject, FALSE);
			ShowWindow(g_hDlgSelectMap, FALSE);
			ShowWindow(g_hDlgObject, FALSE);
			ShowWindow(g_hDlgMap, FALSE);
			ShowWindow(g_hDlgSelectChara, FALSE);
			ShowWindow(g_hDlgExtra, FALSE);
			ShowWindow(g_hDlgQuickView, FALSE);
		}
		break;
	}
				  //�E�{�^���N���b�N
	case WM_RBUTTONDOWN:
		g_MouseDrag = FALSE;
		x = LOWORD(lParam);
		y = HIWORD(lParam) - YTOP;
		if (y > (20 - YTOP)) {
			g_SelectObjectData = mapObject[y / CHIP_SIZE + mapYtop][x / CHIP_SIZE + mapXtop];
			g_SelectMapData = map[y / CHIP_SIZE + mapYtop][x / CHIP_SIZE + mapXtop];

			if (g_SelectObjectData != 0) {
				DestroyWindow(g_hDlgObject);
				DestroyWindow(g_hDlgMap);
				//Undo�Z�b�g
				SetUndoData();
				DisplayObjectDialog();
				//�ҏW���[�h�ύX
				DeleteCheckMenu();
				CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTOBJ, MF_CHECKED);
				g_EditMode = 1;
			}
			else {
				DestroyWindow(g_hDlgObject);
				DestroyWindow(g_hDlgMap);
				//Undo�Z�b�g
				SetUndoData();
				DisplayMapDialog();
				//�ҏW���[�h�ύX
				DeleteCheckMenu();
				CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTMAP, MF_CHECKED);
				g_EditMode = 0;
			}
			InvalidateRect(hWnd, NULL, FALSE);
			InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
			InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
			InvalidateRect(g_hDlgQuickView, NULL, FALSE);
		}
		break;

		//�}�E�X�ړ�
	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam) - YTOP;
		PaintStatus(FALSE);
		if (g_MouseDrag == TRUE) {
			if ((g_MouseX / CHIP_SIZE != g_MouseOldX / CHIP_SIZE) || (g_MouseY / CHIP_SIZE != g_MouseOldY / CHIP_SIZE)) {
				InvalidateRect(hWnd, NULL, FALSE);
			}
			g_MouseOldX = g_MouseX;
			g_MouseOldY = g_MouseY;
		}
		break;

	case WM_LBUTTONUP:
		if (g_MouseDrag == FALSE) break;
		g_MouseDrag = FALSE;

		x = g_MouseX / CHIP_SIZE;
		y = g_MouseY / CHIP_SIZE;
		x2 = g_MouseDragX / CHIP_SIZE;
		y2 = g_MouseDragY / CHIP_SIZE;
		if (x2 > x) {
			x = x2;
			x2 = g_MouseX / CHIP_SIZE;
		}
		if (y2 > y) {
			y = y2;
			y2 = g_MouseY / CHIP_SIZE;
		}
		//�w��͈͓����L�����N�^�Ŗ��߂�B
		if ((x != x2) || (y != y2)) {
			if (x2 > x) ++x2;
			else ++x;
			if (y2 > y) ++y2;
			else ++y;

			if (g_EditMode == 0) {
				for (i = x2; i < x; ++i) {
					for (j = y2; j < y; ++j) {
						map[j + mapYtop][i + mapXtop] = g_SelectMapData;
					}
				}
			}
			else if (g_EditMode == 1) {
				for (i = x2; i < x; ++i) {
					for (j = y2; j < y; ++j) {
						mapObject[j + mapYtop][i + mapXtop] = g_SelectObjectData;
					}
				}
			}
			else if (g_EditMode == 4) {
				for (i = x2; i < x; ++i) {
					for (j = y2; j < y; ++j) {
						mapObject[j + mapYtop][i + mapXtop] = 0;
					}
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;

	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam) - YTOP;
		int x2;

		if (y > 0) {
			//Undo�Z�b�g
			SetUndoData();
			//�h���b�O�J�n�ʒu�Z�b�g
			g_MouseDragX = x;
			g_MouseDragY = y;
			if ((g_EditMode == 0) || (g_EditMode == 1) || (g_EditMode == 4)) g_MouseDrag = TRUE;
		}
		//�X�e�[�^�X�o�[�N���b�N
		if (y < 0) {
			x2 = 96;
			if ((x > x2) && (x < (x2 + 64))) {
				DeleteCheckMenu();
				CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTMAP, MF_CHECKED);
				g_EditMode = 0;
			}
			x2 += 69;
			if ((x > x2) && (x < (x2 + 64))) {
				DeleteCheckMenu();
				CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTOBJ, MF_CHECKED);
				g_EditMode = 1;
			}
			x2 += 69;
			if ((x > x2) && (x < (x2 + 64))) {
				DeleteCheckMenu();
				CheckMenuItem(GetMenu(g_hWnd), ID_MENU_EDITMAP, MF_CHECKED);
				g_EditMode = 2;
			}
			x2 += 69;
			if ((x > x2) && (x < (x2 + 64))) {
				DeleteCheckMenu();
				CheckMenuItem(GetMenu(g_hWnd), ID_MENU_EDITOBJ, MF_CHECKED);
				g_EditMode = 3;
			}
			x2 += 69;
			if ((x > x2) && (x < (x2 + 64))) {
				DeleteCheckMenu();
				CheckMenuItem(GetMenu(g_hWnd), ID_MENU_DELOBJ, MF_CHECKED);
				g_EditMode = 4;
			}
			PaintStatus(TRUE);
		}
		//�}�b�v��ʃN���b�N
		else if (g_EditMode == 0) {
			map[y / CHIP_SIZE + mapYtop][x / CHIP_SIZE + mapXtop] = g_SelectMapData;
			g_bUpdate = TRUE;
		}
		else if (g_EditMode == 1) {
			mapObject[y / CHIP_SIZE + mapYtop][x / CHIP_SIZE + mapXtop] = g_SelectObjectData;
			g_bUpdate = TRUE;
		}
		else if (g_EditMode == 2) {
			g_SelectMapData = map[y / CHIP_SIZE + mapYtop][x / CHIP_SIZE + mapXtop];
			DestroyWindow(g_hDlgObject);
			DestroyWindow(g_hDlgMap);
			//Undo�Z�b�g
			SetUndoData();
			DisplayMapDialog();
			InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
		}
		else if (g_EditMode == 3) {
			g_SelectObjectData = mapObject[y / CHIP_SIZE + mapYtop][x / CHIP_SIZE + mapXtop];
			if (g_SelectObjectData != 0) {
				DestroyWindow(g_hDlgObject);
				DestroyWindow(g_hDlgMap);
				//Undo�Z�b�g
				SetUndoData();
				DisplayObjectDialog();
				InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
			}
		}
		else if (g_EditMode == 4) {
			mapObject[y / CHIP_SIZE + mapYtop][x / CHIP_SIZE + mapXtop] = 0;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_VSCROLL:
		if (LOWORD(wParam) == SB_LINEDOWN) {
			if (mapYtop < g_iMapSize - SCREEN_CHIP_SIZE) ++mapYtop;
			SetScrollPos(g_hWnd, SB_VERT, mapYtop, 1);
		}
		else if (LOWORD(wParam) == SB_LINEUP) {
			if (mapYtop > 0) --mapYtop;
			SetScrollPos(g_hWnd, SB_VERT, mapYtop, 1);
		}
		else if (LOWORD(wParam) == SB_THUMBPOSITION) {
			mapYtop = HIWORD(wParam);
			SetScrollPos(g_hWnd, SB_VERT, mapYtop, 1);
		}
		else if (LOWORD(wParam) == SB_THUMBTRACK) {
			mapYtop = HIWORD(wParam);
			SetScrollPos(g_hWnd, SB_VERT, mapYtop, 1);
		}
		else if (LOWORD(wParam) == SB_PAGEDOWN) {
			if (mapYtop <= g_iMapSize - (SCREEN_CHIP_SIZE + 5)) mapYtop += 5;
			SetScrollPos(g_hWnd, SB_VERT, mapYtop, 1);
		}
		else if (LOWORD(wParam) == SB_PAGEUP) {
			if (mapYtop >= 5) mapYtop -= 5;
			SetScrollPos(g_hWnd, SB_VERT, mapYtop, 1);
		}
		InvalidateRect(g_hWnd, NULL, FALSE);
		break;

	case WM_MOUSEWHEEL: {
		int scrollLines;
		int scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		BOOL parameterResult = SystemParametersInfo(SPI_GETWHEELSCROLLLINES, NULL, &scrollLines, 0);
		if (parameterResult == FALSE) {
			scrollLines = 1;
		}

		// ������
		if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT) {
			if (scrollDelta < 0) {
				mapXtop += scrollLines;
				if (mapXtop > g_iMapSize - SCREEN_CHIP_SIZE) {
					mapXtop = g_iMapSize - SCREEN_CHIP_SIZE;
				}
			}
			else if (scrollDelta > 0) {
				mapXtop -= scrollLines;
				if (mapXtop < 0) {
					mapXtop = 0;
				}
			}
			SetScrollPos(g_hWnd, SB_HORZ, mapXtop, 1);
		}
		// �c����
		else {
			if (scrollDelta < 0) {
				mapYtop += scrollLines;
				if (mapYtop > g_iMapSize - SCREEN_CHIP_SIZE) {
					mapYtop = g_iMapSize - SCREEN_CHIP_SIZE;
				}
			}
			else if (scrollDelta > 0) {
				mapYtop -= scrollLines;
				if (mapYtop < 0) {
					mapYtop = 0;
				}
			}
			SetScrollPos(g_hWnd, SB_VERT, mapYtop, 1);
		}
		InvalidateRect(g_hWnd, NULL, FALSE);
		break;
	}

	case WM_HSCROLL:
		if (LOWORD(wParam) == SB_LINEDOWN) {
			if (mapXtop < g_iMapSize - SCREEN_CHIP_SIZE) ++mapXtop;
			SetScrollPos(g_hWnd, SB_HORZ, mapXtop, 1);
		}
		else if (LOWORD(wParam) == SB_LINEUP) {
			if (mapXtop > 0) --mapXtop;
			SetScrollPos(g_hWnd, SB_HORZ, mapXtop, 1);
		}
		else if (LOWORD(wParam) == SB_THUMBPOSITION) {
			mapXtop = HIWORD(wParam);
			SetScrollPos(g_hWnd, SB_HORZ, mapXtop, 1);
		}
		else if (LOWORD(wParam) == SB_THUMBTRACK) {
			mapXtop = HIWORD(wParam);
			SetScrollPos(g_hWnd, SB_HORZ, mapXtop, 1);
		}
		else if (LOWORD(wParam) == SB_PAGEDOWN) {
			if (mapXtop <= g_iMapSize - (SCREEN_CHIP_SIZE + 5)) mapXtop += 5;
			SetScrollPos(g_hWnd, SB_HORZ, mapXtop, 1);
		}
		else if (LOWORD(wParam) == SB_PAGEUP) {
			if (mapXtop >= 5) mapXtop -= 5;
			SetScrollPos(g_hWnd, SB_HORZ, mapXtop, 1);
		}
		InvalidateRect(g_hWnd, NULL, FALSE);
		break;

	case WM_PAINT:
		PaintWindow();
		break;

	case WM_COMMAND:
		// �J�[�\���L�[����
		if (GetActiveWindow() == g_hDlgSelectMap) {
			if (LOWORD(wParam) == ID_MENU_DOWN) {
				if (g_ScrMap < ((g_iMapPartsMax / 10) - 3)) ++g_ScrMap;
				SetScrollPos(g_hDlgSelectMap, SB_VERT, g_ScrMap, 1);
				InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
				break;
			}
			else if (LOWORD(wParam) == ID_MENU_UP) {
				if (g_ScrMap > 0) --g_ScrMap;
				SetScrollPos(g_hDlgSelectMap, SB_VERT, g_ScrMap, 1);
				InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
				break;
			}
		}
		else if (GetActiveWindow() == g_hDlgSelectObject) {
			if (LOWORD(wParam) == ID_MENU_DOWN) {
				if (g_ScrObject < ((g_iObjectPartsMax / 10) - 3)) ++g_ScrObject;
				SetScrollPos(g_hDlgSelectObject, SB_VERT, g_ScrObject, 1);
				InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
				break;
			}
			else if (LOWORD(wParam) == ID_MENU_UP) {
				if (g_ScrObject > 0) --g_ScrObject;
				SetScrollPos(g_hDlgSelectObject, SB_VERT, g_ScrObject, 1);
				InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
				break;
			}
		}
		else if (GetActiveWindow() == g_hDlgExtra) {
			// �G�f�B�b�g�{�b�N�X�̃t�H�[�J�X�ړ�
			if ((LOWORD(wParam) == ID_MENU_DOWN) || (LOWORD(wParam) == ID_MENU_UP) || (LOWORD(wParam) == ID_MENU_RIGHT) || (LOWORD(wParam) == ID_MENU_LEFT)) {
				for (i = 0; i < 40; ++i) {
					if (g_EditId[i] == GetDlgCtrlID(GetFocus())) break;
				}
				if (i == 40) break;
				if (LOWORD(wParam) == ID_MENU_DOWN) i += 10;
				else if (LOWORD(wParam) == ID_MENU_UP) i -= 10;
				else if (LOWORD(wParam) == ID_MENU_RIGHT) ++i;
				else if (LOWORD(wParam) == ID_MENU_LEFT) --i;
				if (i < 0) i += 40;
				if (i >= 40) i -= 40;
				SetFocus(GetDlgItem(g_hDlgExtra, g_EditId[i]));
				break;
			}
		}

		// �f�[�^�̃��[�h�A�Z�[�u
		if (LOWORD(wParam) == ID_MENU_LOAD) {
			int i;
			char FileName[250] = "*.dat";
			char CurrentDir[250];
			OPENFILENAME ofn;

			GetCurrentDirectory(sizeof(CurrentDir), CurrentDir);
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrTitle = "�}�b�v�f�[�^�̓ǂݍ���";
			ofn.lpstrFile = FileName;
			ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
			ofn.lpstrInitialDir = CurrentDir;
			ofn.lpstrFilter = "Map Data (*.dat)\0\0";
			ofn.nMaxFile = sizeof(FileName);
			ofn.hwndOwner = g_hWnd;

			if (GetOpenFileName(&ofn)) {
				//�f�B���N�g�����o
				for (i = strlen(FileName); i > 0; --i) if (FileName[i] == '\\') break;
				strcpy(g_szSelectDir, FileName);
				g_szSelectDir[i + 1] = '\0';
				strcpy(g_szSelectFile, (FileName + i + 1));
				//�f�[�^�ǂݍ���
				LoadMapData(g_szSelectFile);
				if (g_bErrorPassword == TRUE) MessageBox(g_hWnd, "�Ïؔԍ����Ⴂ�܂��B", "�x���I", MB_OK);
				LoadBitmap();
			}
		}
		else if (LOWORD(wParam) == ID_MENU_SAVE) {
			int i;
			char FileName[250] = "*.dat";
			char CurrentDir[250];

			OPENFILENAME ofn;
			GetCurrentDirectory(sizeof(CurrentDir), CurrentDir);
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrTitle = "�}�b�v�f�[�^�̕ۑ�";
			ofn.lpstrFile = FileName;
			ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
			ofn.lpstrInitialDir = CurrentDir;
			ofn.lpstrFilter = "Map Data (*.dat)\0\0";
			ofn.nMaxFile = sizeof(FileName);
			ofn.hwndOwner = g_hWnd;

			if (GetSaveFileName(&ofn)) {
				//�f�B���N�g�����o
				for (i = strlen(FileName); i > 0; --i) if (FileName[i] == '\\') break;
				strcpy(g_szSelectDir, FileName);
				g_szSelectDir[i + 1] = '\0';
				strcpy(g_szSelectFile, (FileName + i + 1));
				//�f�[�^�ۑ�
				SaveMapData(g_szSelectFile);
			}
		}
		// ��{�ݒ�_�C�A���O
		else if (LOWORD(wParam) == ID_MENU_FOUNDATION) {
			EditMapFoundation();
		}
		// ��{���b�Z�[�W�_�C�A���O
		else if (LOWORD(wParam) == ID_MENU_BASICMES) {
			DestroyWindow(g_hDlgBasicMes);
			g_hDlgBasicMes = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_BASICMES), g_hWnd, (DLGPROC)DialogProcBasicMes);
		}
		// �퓬���ʂ̌v�Z
		else if (LOWORD(wParam) == ID_MENU_BATTLE) {
			CalculateDialog();
		}
		// �ҏW���[�h�I��ؑ�
		else if (LOWORD(wParam) == ID_MENU_PUTMAP) {
			DeleteCheckMenu();
			CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTMAP, MF_CHECKED);
			g_EditMode = 0;
			PaintStatus(TRUE);
		}
		else if (LOWORD(wParam) == ID_MENU_PUTOBJ) {
			DeleteCheckMenu();
			CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTOBJ, MF_CHECKED);
			g_EditMode = 1;
			PaintStatus(TRUE);
		}
		else if (LOWORD(wParam) == ID_MENU_EDITMAP) {
			DeleteCheckMenu();
			CheckMenuItem(GetMenu(g_hWnd), ID_MENU_EDITMAP, MF_CHECKED);
			g_EditMode = 2;
			PaintStatus(TRUE);
		}
		else if (LOWORD(wParam) == ID_MENU_EDITOBJ) {
			DeleteCheckMenu();
			CheckMenuItem(GetMenu(g_hWnd), ID_MENU_EDITOBJ, MF_CHECKED);
			g_EditMode = 3;
			PaintStatus(TRUE);
		}
		else if (LOWORD(wParam) == ID_MENU_DELOBJ) {
			DeleteCheckMenu();
			CheckMenuItem(GetMenu(g_hWnd), ID_MENU_DELOBJ, MF_CHECKED);
			g_EditMode = 4;
			PaintStatus(TRUE);
		}
		// �㏑���ۑ�
		else if (LOWORD(wParam) == ID_MENU_SAVEA) {
			SaveMapData(g_szSelectFile);
		}
		//���E��
		else if (LOWORD(wParam) == ID_MENU_LINEDRAW) {
			if (g_MapLineFlag == FALSE) g_MapLineFlag = TRUE;
			else g_MapLineFlag = FALSE;
			InvalidateRect(g_hWnd, NULL, FALSE);
		}
		else if (LOWORD(wParam) == ID_MENU_MARKDRAW) {
			if (g_ObjectNumberFlag == FALSE) g_ObjectNumberFlag = TRUE;
			else g_ObjectNumberFlag = FALSE;
			InvalidateRect(g_hWnd, NULL, FALSE);
		}
		// �}�b�v�̐V�K�쐬
		else if (LOWORD(wParam) == ID_MENU_NEW) {
			MakeNewMap();
		}
		// �u���E�U�I�[�v��
		else if (LOWORD(wParam) == ID_MENU_EXEC) {
			if (g_bUpdate == TRUE) {
				MessageBox(g_hWnd, "���݂̏�Ԃ̃}�b�v������ꍇ�́A\n��x�t�@�C���ɕۑ����Ă����Ă��������B", "���ӁI", MB_OK);
			}
			ExecBrowser();
		}
		// �N�C�b�N�r���[�E�B���h�E�̕\��
		else if (LOWORD(wParam) == ID_MENU_QVIEW) {
			ShowWindow(g_hDlgQuickView, TRUE);
			EnableMenuItem(GetMenu(g_hWnd), ID_MENU_QVIEW, MF_GRAYED);
		}
		// ���̑I���E�B���h�E�̕\��
		else if (LOWORD(wParam) == ID_MENU_OBJWINDOW) {
			ShowWindow(g_hDlgSelectObject, TRUE);
			EnableMenuItem(GetMenu(g_hWnd), ID_MENU_OBJWINDOW, MF_GRAYED);
		}
		// �w�i�I���E�B���h�E�̕\��
		else if (LOWORD(wParam) == ID_MENU_MAPWINDOW) {
			ShowWindow(g_hDlgSelectMap, TRUE);
			EnableMenuItem(GetMenu(g_hWnd), ID_MENU_MAPWINDOW, MF_GRAYED);
		}
		// �}�b�v�E�B���h�E�̊g��k��
		else if (LOWORD(wParam) == ID_MENU_CHANGEWINDOWSIZE) {
			int SizeX, SizeY;
			int setScreenChipSize = g_hugeMapSize ? DEFALUT_SCREEN_CHIP_SIZE : SCREEN_CHIP_SIZE;
			setScreenChipSize++;
			g_hugeMapSize = !g_hugeMapSize;
			SizeX = (setScreenChipSize * CHIP_SIZE);
			SizeY = ((setScreenChipSize + 1) * CHIP_SIZE);
			SetWindowPos(g_hWnd, NULL, 0, 0, SizeX, SizeY, SWP_NOMOVE | SWP_NOZORDER);
		}
		// �p�[�c�̃R�s�[
		else if (LOWORD(wParam) == ID_MENU_COPY) {
			if (g_EditMode == 0) {
				for (i = 0; i < MAP_ATR_MAX; ++i) g_CopyMap[i] = mapAttribute[g_SelectMapData][i];
				strcpy(g_CopyMapStr, g_StrMessage[mapAttribute[g_SelectMapData][ATR_STRING]]);
				g_CopyMap[ATR_STRING] = 0;
			}
			else if (g_EditMode == 1) {
				for (i = 0; i < OBJECT_ATR_MAX; ++i) g_CopyObject[i] = objectAttribute[g_SelectObjectData][i];
				strcpy(g_CopyObjectStr, g_StrMessage[objectAttribute[g_SelectObjectData][ATR_STRING]]);
				g_CopyObject[ATR_STRING] = 0;
			}
		}
		else if (LOWORD(wParam) == ID_MENU_PASTE) {
			// Undo�Z�b�g
			SetUndoData();
			DestroyWindow(g_hDlgObject);
			DestroyWindow(g_hDlgMap);
			DestroyWindow(g_hDlgSelectChara);
			g_bUpdate = TRUE;
			if (g_EditMode == 0) {
				// �w�i�\��t��
				for (i = 0; i < MAP_ATR_MAX; ++i) mapAttribute[g_SelectMapData][i] = g_CopyMap[i];
				SetMessageData(&mapAttribute[g_SelectMapData][ATR_STRING], g_CopyMapStr);
				InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
			}
			else if (g_EditMode == 1) {
				// ���̓\��t��
				for (i = 0; i < OBJECT_ATR_MAX; ++i) objectAttribute[g_SelectObjectData][i] = g_CopyObject[i];
				SetMessageData(&objectAttribute[g_SelectObjectData][ATR_STRING], g_CopyObjectStr);
				InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
			}
		}
		else if (LOWORD(wParam) == ID_MENU_UNDO) {
			DestroyWindow(g_hDlgObject);
			DestroyWindow(g_hDlgMap);
			DestroyWindow(g_hDlgSelectChara);
			g_bUpdate = TRUE;
			RestoreUndoData();
			InvalidateRect(g_hWnd, NULL, FALSE);
			InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
			InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
		}
		// �}�b�v��ʂ��ƃR�s�[
		else if (LOWORD(wParam) == ID_MENU_MAPCOPY) {
		int nowScreenChipSize = g_hugeMapSize ? SCREEN_CHIP_SIZE : DEFALUT_SCREEN_CHIP_SIZE;
			for (x = 0; x < nowScreenChipSize; ++x) {
				for (y = 0; y < nowScreenChipSize; ++y) {
					g_MapBuffer[y][x] = map[mapYtop + y][mapXtop + x];
					g_ObjectBuffer[y][x] = mapObject[mapYtop + y][mapXtop + x];
				}
			}
		}
		else if (LOWORD(wParam) == ID_MENU_MAPPASTE) {
			// Undo�Z�b�g
			SetUndoData();
			int nowScreenChipSize = g_hugeMapSize ? SCREEN_CHIP_SIZE : DEFALUT_SCREEN_CHIP_SIZE;
			for (x = 0; x < nowScreenChipSize; ++x) {
				for (y = 0; y < nowScreenChipSize; ++y) {
					map[mapYtop + y][mapXtop + x] = g_MapBuffer[y][x];
					mapObject[mapYtop + y][mapXtop + x] = g_ObjectBuffer[y][x];
				}
			}
			InvalidateRect(g_hWnd, NULL, FALSE);
		}
		// �c�[���̏I��
		else if (LOWORD(wParam) == ID_MENU_END) {
			if (g_bUpdate == TRUE) {
				result = MessageBox(g_hWnd, "�}�b�v�f�[�^�����ۑ��ł����A�I�����Ă�낵���ł����H", "���ӁI", MB_OK | MB_OKCANCEL);
				if (result == IDCANCEL) break;
			}
			DestroyWindow(g_hWnd);
		}
		break;

	case WM_CLOSE:
		if (g_bUpdate == TRUE) {
			result = MessageBox(g_hWnd, "�}�b�v�f�[�^�����ۑ��ł����A�I�����Ă�낵���ł����H", "���ӁI", MB_OK | MB_OKCANCEL);
			if (result == IDCANCEL) break;
		}
		DestroyWindow(g_hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return(DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}



//##------------------------------------------------------------------
// ���j���[�̃`�F�b�N���͂���
void DeleteCheckMenu()
{
	CheckMenuItem( GetMenu(g_hWnd), ID_MENU_PUTMAP, MF_UNCHECKED );
	CheckMenuItem( GetMenu(g_hWnd), ID_MENU_PUTOBJ, MF_UNCHECKED );
	CheckMenuItem( GetMenu(g_hWnd), ID_MENU_EDITMAP, MF_UNCHECKED );
	CheckMenuItem( GetMenu(g_hWnd), ID_MENU_EDITOBJ, MF_UNCHECKED );
	CheckMenuItem( GetMenu(g_hWnd), ID_MENU_DELOBJ, MF_UNCHECKED );
}



//##------------------------------------------------------------------
// �v�v�`�쐬�c�[��
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR pszCmdLine, int CmdShow)
{
	MSG			msg;
	WNDCLASS	wc;
	g_hInst = hInst;
	OSVERSIONINFO OsVersion;
	RECT WindowRect, ClientRect;
	int SizeX, SizeY, PositionX, PositionY;

	if (!hInstPrev) {
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(hInst, "WWA.ico");
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
		wc.lpszClassName = "WWAMK";
		if (!RegisterClass(&wc))
			return FALSE;
	}

	// �ݒ�t�@�C���ǂݍ���
	PositionX = GetPrivateProfileInt("Main", "PositionX", CW_USEDEFAULT, g_szSettingFile);
	PositionY = GetPrivateProfileInt("Main", "PositionY", 0, g_szSettingFile);
	int window_size = SCREEN_CHIP_SIZE * CHIP_SIZE;

	// ���C���E�B���h�E�쐬
	g_hWnd = CreateWindow("WWAMK", "WWA Wing�}�b�v�쐬�c�[��",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_HSCROLL | WS_VSCROLL,
		PositionX, PositionY,
		CW_USEDEFAULT , CW_USEDEFAULT,
		NULL, NULL, hInst, NULL);

	// �T�C�Y�ύX
	GetWindowRect(g_hWnd, &WindowRect);
	GetClientRect(g_hWnd, &ClientRect);
	SizeX = (WindowRect.right - WindowRect.left) - (ClientRect.right - ClientRect.left) + (DEFALUT_SCREEN_CHIP_SIZE * CHIP_SIZE);
	SizeY = (WindowRect.bottom - WindowRect.top) - (ClientRect.bottom - ClientRect.top) + (DEFALUT_SCREEN_CHIP_SIZE * CHIP_SIZE) + 20;
	SetWindowPos(g_hWnd, NULL, 0, 0, SizeX, SizeY, SWP_NOMOVE | SWP_NOZORDER);

	if (g_hWnd != NULL) {
		ShowWindow(g_hWnd, CmdShow);
		UpdateWindow(g_hWnd);
	}

	// �����t�@�C�����ݒ�
	unsigned int i, j;
	if (strlen(pszCmdLine) > 0) {
		// �_�u���N�H�[�e�[�V�����͍폜
		for (i = 0; i < strlen(pszCmdLine); ++i) {
			if (pszCmdLine[i] == '"') {
				++i;
				for (j = 0; j < (strlen(pszCmdLine) - i); ++j) {
					if (pszCmdLine[i + j] == '"') break;
					pszCmdLine[j] = pszCmdLine[i + j];
				}
				pszCmdLine[j] = '\0';
				break;
			}
		}
		// �t�@�C�������o
		for (i = strlen(pszCmdLine); i > 0; --i) if (pszCmdLine[i] == '\\') break;
		strcpy(g_szSelectFile, (pszCmdLine + i + 1));
		// �f�B���N�g���ړ�
		strcpy(g_szSelectDir, pszCmdLine);
		g_szSelectDir[i + 1] = '\0';
		SetCurrentDirectory(g_szSelectDir);
	}
	else {
		SetCurrentDirectory("mapdata");
	}
	// �}�b�v�f�[�^�ǂݍ���
	LoadMapData(g_szSelectFile);
	if (g_bErrorPassword == TRUE) MessageBox(g_hWnd, "�Ïؔԍ����Ⴂ�܂��B", "�x���I", MB_OK);
	// �r�b�g�}�b�v�ǂݍ���
	LoadBitmap();

	// �_�C�A���O�\��
	g_hDlgSelectObject = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EDITOBJECT), g_hWnd, (DLGPROC)SelectObjectDialogProc);
	g_hDlgSelectMap = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EDITMAP), g_hWnd, (DLGPROC)SelectMapDialogProc);
	// �_�C�A���O�ړ�
	RECT rect;
	RECT rectBox;
	GetWindowRect(g_hWnd, &rectBox);
	GetWindowRect(g_hDlgSelectObject, &rect);
	MoveWindow(g_hDlgSelectObject, rectBox.right, rectBox.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	GetWindowRect(g_hDlgSelectObject, &rectBox);
	GetWindowRect(g_hDlgSelectMap, &rect);
	MoveWindow(g_hDlgSelectMap, rectBox.left, rectBox.bottom, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	ShowWindow(g_hDlgSelectMap, SW_SHOW);
	ShowWindow(g_hDlgSelectObject, SW_SHOW);

	// �N�C�b�N�r���[
	g_hDlgQuickView = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_QVIEW), g_hWnd, (DLGPROC)QuickViewDialogProc);
	GetWindowRect(g_hWnd, &rectBox);
	GetWindowRect(g_hDlgQuickView, &rect);
	MoveWindow(g_hDlgQuickView, rectBox.left, rectBox.bottom, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	ShowWindow(g_hDlgQuickView, SW_SHOW);

	// ��ʐF���擾
	HDC hDC = GetDC(g_hWnd);
	if (GetDeviceCaps(hDC, BITSPIXEL) == 8) {
		MessageBox(g_hWnd, "���̃c�[���́A256�F���ł͉摜�����܂��\������܂���B\n�R���g���[���p�l���̉�ʂ̐ݒ�ŁA16�r�b�g�J���[�ȏ��I�����Ă��������B", "�x���I", MB_OK);
	}
	ReleaseDC(g_hWnd, hDC);

	// �t�H�[�J�X�ړ�
	SetFocus(g_hWnd);
	// �A�N�Z�����[�^�[�L�[
	HACCEL hAccel = LoadAccelerators(g_hInst, MAKEINTRESOURCE(IDR_MENU1));

	// ����������
	g_bInitial = TRUE;

	if (g_hWnd != NULL) {
		while (GetMessage(&msg, NULL, 0, 0)) {
			// �_�C�A���O���b�Z�[�W
			if (IsDialogMessage(g_hDlgBasicMes, &msg)) continue;
			// �e�L�X�g�{�b�N�X�ȊO�Ȃ�V���[�g�J�b�g�L�[����
			if ((GetDlgCtrlID(GetFocus()) == IDC_EDIT_MESSAGE) || (GetDlgCtrlID(GetFocus()) == IDC_EDIT_URL)) {
				if (IsDialogMessage(g_hDlgMap, &msg)) continue;
				else if (IsDialogMessage(g_hDlgObject, &msg)) continue;
			}
			if (!TranslateAccelerator(g_hWnd, hAccel, &msg)) {
				// �_�C�A���O���b�Z�[�W
				if (IsDialogMessage(g_hDlgSelectMap, &msg)) continue;
				else if (IsDialogMessage(g_hDlgSelectObject, &msg)) continue;
				else if (IsDialogMessage(g_hDlgQuickView, &msg)) continue;
				else if (IsDialogMessage(g_hDlgExtra, &msg)) continue;
				else if (IsDialogMessage(g_hDlgCalculate, &msg)) continue;
				else if (IsDialogMessage(g_hDlgFoundation, &msg)) continue;
				else if (IsDialogMessage(g_hDlgObject, &msg)) continue;
				else if (IsDialogMessage(g_hDlgMap, &msg)) continue;

				// ���b�Z�[�W�𑗂�
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return(msg.wParam);
}



//##------------------------------------------------------------------
// �r�b�g�}�b�v�ǂݍ���

BOOL LoadBitmap()
{
	g_pDib = new CDib;
	HDC hDC = GetDC(g_hWnd);

	// �f�o�C�X�R���e�L�X�g�̏�����
	if (g_hmDC == NULL) {
		g_hmDC = CreateCompatibleDC(hDC);
		g_hmDCHalf = CreateCompatibleDC(hDC);
		g_hmDCAnd = CreateCompatibleDC(hDC);
		g_hmDCOr = CreateCompatibleDC(hDC);

		g_hmDCExtra = CreateCompatibleDC(hDC);
		g_hBitmapExtra = CreateCompatibleBitmap(hDC, CHIP_SIZE * SCREEN_CHIP_SIZE, 20);
		SelectObject(g_hmDCExtra, g_hBitmapExtra);
	}
	// ���݂̃r�b�g�}�b�v���L�^
	if (g_bLoadGif == TRUE) strcpy(g_mapcgOld, g_mapcgName);
	else strcpy(g_mapcgOld, g_mapcgNameBmp);

	// �ǂݏo�����_�C�A���O�\��
	Rectangle(hDC, 0, 0, CHIP_SIZE * SCREEN_CHIP_SIZE, CHIP_SIZE * SCREEN_CHIP_SIZE);
	TextOut(hDC, 80, 200, "�摜�f�[�^�ǂݍ��ݒ��E�E�E�E�E", 30);

	// �摜��������GIF�`��
	g_bLoadGif = ReadGifImage();
	if (g_bFileNotFound == TRUE) return FALSE;
	// GIF�ǂݍ��ݎ��s��
	if (g_bLoadGif == FALSE) {
		strcpy(g_mapcgOld, g_mapcgNameBmp);
		if (g_pDib->ReadFile(g_mapcgNameBmp) == FALSE) {
			MessageBox(g_hWnd, "���̃}�b�v�f�[�^�ɑΉ�����摜�t�@�C���i256�FBMP�t�@�C���j���ǂݍ��߂܂���B\n�u�ҏW�|��{�ݒ�̕ҏW�v��256�FBMP�t�@�C�����w�肵�Ă��������B\n\n���̃V�X�e���ł́AGIF�t�@�C���͒��ړǂݍ��߂Ȃ��̂ŁA\n�ҏW�p��256�FBMP�t�@�C�����K�v�ɂȂ�܂��B", "BMP�t�@�C���ǂݍ��ݎ��s", MB_OK);
			// �f�o�C�X���
			ReleaseDC(g_hWnd, hDC);
			return FALSE;
		}
		// �摜��ݒ�
		g_iLoadCGHeight = g_pDib->GetCDibBiHeight();
		if ((g_iLoadCGHeight / CHIP_SIZE) > 17) {
			g_ScrCGCharaMax = (g_iLoadCGHeight / CHIP_SIZE) - 17;
		}
		else {
			g_ScrCGCharaMax = 0;
		}
		// �N���C�A���g�̈�m��
		if (g_hBitmap != NULL) DeleteObject(g_hBitmap);
		g_hBitmap = CreateCompatibleBitmap(hDC, CHIP_SIZE * 10, g_iLoadCGHeight);
		SelectObject(g_hmDC, g_hBitmap);
		// �N���C�A���g�ɕ`��
		g_pDib->DrawBits(g_hmDC, 0, 0);
	}

	// �P�^�Q�T�C�Y�̂b�f�̈�m��
	if (g_hBitmapHalf != NULL) DeleteObject(g_hBitmapHalf);
	g_hBitmapHalf = CreateCompatibleBitmap(hDC, 200, (g_iLoadCGHeight / 2));
	SelectObject(g_hmDCHalf, g_hBitmapHalf);
	// �P�^�Q�T�C�Y�̂b�f��`��$$
	StretchBlt(g_hmDCHalf, 0, 0, 200, (g_iLoadCGHeight / 2), g_hmDC, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, SRCCOPY);

	// AND�摜�̈�m��
	if (g_hBitmapAnd != NULL) DeleteObject(g_hBitmapAnd);
	g_hBitmapAnd = CreateBitmap(CHIP_SIZE * 10, g_iLoadCGHeight, 1, 1, NULL);
	SelectObject(g_hmDCAnd, g_hBitmapAnd);
	// AND�摜�쐬
	if (g_iColorTp == 0) g_iColorTp = GetPixel(g_hmDC, 60, 20);
	SetBkColor(g_hmDC, g_iColorTp);
	BitBlt(g_hmDCAnd, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, g_hmDC, 0, 0, SRCCOPY);
	// AND�摜���]
	BitBlt(g_hmDCAnd, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, NULL, 0, 0, DSTINVERT);
	// OR�摜�̈�m��
	if (g_hBitmapOr != NULL) DeleteObject(g_hBitmapOr);
	g_hBitmapOr = CreateCompatibleBitmap(hDC, CHIP_SIZE * 10, g_iLoadCGHeight);
	SelectObject(g_hmDCOr, g_hBitmapOr);
	// OR�摜�쐬
	BitBlt(g_hmDCOr, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, g_hmDC, 0, 0, SRCCOPY);
	BitBlt(g_hmDCOr, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, g_hmDCAnd, 0, 0, SRCAND);
	// AND�摜���]
	BitBlt(g_hmDCAnd, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, NULL, 0, 0, DSTINVERT);

	// �f�o�C�X���
	ReleaseDC(g_hWnd, hDC);
	// �ĕ`��
	InvalidateRect(g_hWnd, NULL, FALSE);
	InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
	InvalidateRect(g_hDlgSelectMap, NULL, FALSE);

	return TRUE;
}



//##------------------------------------------------------------------
// ���C���E�B���h�E�֕`��
void PaintWindow()
{
	int x, y, x2, y2;
	HDC hDC;
	PAINTSTRUCT ps;
	HPEN hpen, hpenRed, hpenOld;

	hDC = BeginPaint(g_hWnd, &ps);
	paintMapAll(hDC);

	// �L�����N�^���E���\��
	hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	hpenOld = (HPEN)SelectObject(hDC, hpen);
	hpenRed = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));

	// ���E����\��
	if (g_MapLineFlag == TRUE) {
		for (x = 1; x < SCREEN_CHIP_SIZE; ++x) {
			DrawLine(hDC, x * CHIP_SIZE, YTOP, x * CHIP_SIZE, CHIP_SIZE * SCREEN_CHIP_SIZE + YTOP);
		}
		for (y = 1; y < SCREEN_CHIP_SIZE; ++y) {
			DrawLine(hDC, 0, y * CHIP_SIZE + YTOP, CHIP_SIZE * SCREEN_CHIP_SIZE, y * CHIP_SIZE + YTOP);
		}

		// �}�b�v���E���\��
		SelectObject(hDC, hpenRed);
		DrawLine(hDC, (SCREEN_CHIP_SIZE - mapXtop % 10) * CHIP_SIZE - 20, YTOP, (SCREEN_CHIP_SIZE - mapXtop % 10) * CHIP_SIZE - 20, CHIP_SIZE * SCREEN_CHIP_SIZE + YTOP);
		DrawLine(hDC, (SCREEN_CHIP_SIZE - mapXtop % 10) * CHIP_SIZE - 19, YTOP, (SCREEN_CHIP_SIZE - mapXtop % 10) * CHIP_SIZE - 19, CHIP_SIZE * SCREEN_CHIP_SIZE + YTOP);
		DrawLine(hDC, (SCREEN_CHIP_SIZE - mapXtop % 10) * CHIP_SIZE - 420, YTOP, (SCREEN_CHIP_SIZE - mapXtop % 10) * CHIP_SIZE - 420, CHIP_SIZE * SCREEN_CHIP_SIZE + YTOP);
		DrawLine(hDC, (SCREEN_CHIP_SIZE - mapXtop % 10) * CHIP_SIZE - 419, YTOP, (SCREEN_CHIP_SIZE - mapXtop % 10) * CHIP_SIZE - 419, CHIP_SIZE * SCREEN_CHIP_SIZE + YTOP);
		DrawLine(hDC, 0, (SCREEN_CHIP_SIZE - mapYtop % 10) * CHIP_SIZE - 20 + YTOP, CHIP_SIZE * SCREEN_CHIP_SIZE, (SCREEN_CHIP_SIZE - mapYtop % 10) * CHIP_SIZE - 20 + YTOP);
		DrawLine(hDC, 0, (SCREEN_CHIP_SIZE - mapYtop % 10) * CHIP_SIZE - 19 + YTOP, CHIP_SIZE * SCREEN_CHIP_SIZE, (SCREEN_CHIP_SIZE - mapYtop % 10) * CHIP_SIZE - 19 + YTOP);
		DrawLine(hDC, 0, (SCREEN_CHIP_SIZE - mapYtop % 10) * CHIP_SIZE - 420 + YTOP, CHIP_SIZE * SCREEN_CHIP_SIZE, (SCREEN_CHIP_SIZE - mapYtop % 10) * CHIP_SIZE - 420 + YTOP);
		DrawLine(hDC, 0, (SCREEN_CHIP_SIZE - mapYtop % 10) * CHIP_SIZE - 419 + YTOP, CHIP_SIZE * SCREEN_CHIP_SIZE, (SCREEN_CHIP_SIZE - mapYtop % 10) * CHIP_SIZE - 419 + YTOP);
	}

	// �h���b�O�ɂ��l�p�`��
	SelectObject(hDC, hpenRed);
	if (g_MouseDrag == TRUE) {
		x = g_MouseX / CHIP_SIZE;
		y = g_MouseY / CHIP_SIZE;
		x2 = g_MouseDragX / CHIP_SIZE;
		y2 = g_MouseDragY / CHIP_SIZE;
		if (x2 > x) ++x2;
		else ++x;
		if (y2 > y) ++y2;
		else ++y;

		DrawRect2(hDC, x * CHIP_SIZE - 1, y * CHIP_SIZE - 1 + YTOP, x2 * CHIP_SIZE + 1, y2 * CHIP_SIZE + 1 + YTOP);
		DrawRect2(hDC, x * CHIP_SIZE - 2, y * CHIP_SIZE - 2 + YTOP, x2 * CHIP_SIZE + 2, y2 * CHIP_SIZE + 2 + YTOP);
	}

	// �y���̉��
	SelectObject(hDC, hpenOld);
	DeleteObject(hpen);
	DeleteObject(hpenRed);
	EndPaint(g_hWnd, &ps);

	// �X�e�[�^�X�`��
	PaintStatus(TRUE);
}



//##------------------------------------------------------------------
// �X�e�[�^�X�`��

void PaintStatusSub( int x, int y, int iMode, char *szStr )
{
	if( g_EditMode == iMode ){
		SetBkMode( g_hmDCExtra, OPAQUE );
		SetBkColor( g_hmDCExtra, RGB(255,128,128) );
	} else {
		SetBkMode( g_hmDCExtra, TRANSPARENT );
	}
	TextOut( g_hmDCExtra, x, y, szStr, strlen(szStr) );
}


void PaintStatus(BOOL flag)
{
	// �X�e�[�^�X��\��
	HDC hDC = GetDC(g_hWnd);
	char str[100];
	int x;
	const int y = 1;

	SetBkMode(g_hmDCExtra, TRANSPARENT);
	Rectangle(g_hmDCExtra, 0, 0, CHIP_SIZE * SCREEN_CHIP_SIZE, 20);

	sprintf(str, "X=%2d ", g_MouseX / CHIP_SIZE + mapXtop);
	TextOut(g_hmDCExtra, 4, 1, str, strlen(str));
	sprintf(str, "Y=%2d ", g_MouseY / CHIP_SIZE + mapYtop);
	TextOut(g_hmDCExtra, 50, 1, str, strlen(str));

	x = 96;
	PaintStatusSub(x, y, 0, "�w�i�ݒu");
	x += 69;
	PaintStatusSub(x, y, 1, "���̐ݒu");
	x += 69;
	PaintStatusSub(x, y, 2, "�w�i�ҏW");
	x += 69;
	PaintStatusSub(x, y, 3, "���̕ҏW");
	x += 69;
	PaintStatusSub(x, y, 4, "���̍폜");

	BitBlt(hDC, 0, 0, CHIP_SIZE * SCREEN_CHIP_SIZE, 20, g_hmDCExtra, 0, 0, SRCCOPY);
	ReleaseDC(g_hWnd, hDC);
}


//##------------------------------------------------------------------
// ���l�ϊ�
int unsignedByte(char numberByte) {
	int number;

	number = (int)numberByte;
	if (number < 0) number += 0x100;
	return number;
}


//##------------------------------------------------------------------
// ������̕ϊ��Ɠǂݏo��
void loadMapString(char* AnsiString, BOOL GetString = TRUE)
{
	int length;
	int plus = 0;
	WCHAR buffer[MESSAGE_STR_MAX];

	for (length = 0; length < (MESSAGE_STR_MAX / 2 - 1); ++length) {
		if ((PressData[pointer + length * 2] == 0) && (PressData[pointer + length * 2 + 1] == 0)) break;
		buffer[length + plus] = (UCHAR)PressData[pointer + length * 2];
		buffer[length + plus] += ((UCHAR)PressData[pointer + length * 2 + 1] << 8);
		if (buffer[length + plus] == L'\n') {
			buffer[length + plus] = L'#';
			++plus;
			buffer[length + plus] = L'#';
		}
	}
	buffer[length + plus] = '\0';
	pointer += length * 2 + 2;

	// GetString �� FALSE �̏ꍇ�̓f�[�^��荞�݂͂��������ŏI���
	if (GetString == FALSE) {
		AnsiString[0] = '\0';
		return;
	}
	DWORD chaLength = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, buffer, -1, AnsiString, chaLength, NULL, NULL);

	// ���s�����̌��o�ƕϊ�
	for (length = 0; length < MESSAGE_STR_MAX; ++length) {
		if (AnsiString[length] == NULL) break;
		if ((AnsiString[length] == '#') && (AnsiString[length + 1] == '#')) {
			AnsiString[length] = 0x0D;
			++length;
			AnsiString[length] = 0x0A;
		}
	}
}



//##------------------------------------------------------------------
// �}�b�v�f�[�^�̃��[�h
BOOL LoadMapData(char* FileName)
{
	int point2;
	HANDLE hFile;
	DWORD BytesRead;
	pointer = 0;
	g_mapcgOld[0] = '\0';
	int iDataMapCount;
	int iDataObjectCount;
	int dataChara, dataMode;
	char szTemp[100];

	// �_�C�A���O����
	DestroyWindow(g_hDlgObject);
	DestroyWindow(g_hDlgMap);
	DestroyWindow(g_hDlgSelectChara);

	// �f�[�^�I�[�v��
	hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(g_hWnd, "�}�b�v�f�[�^�t�@�C�����ǂݍ��݂ł��܂���B", "����", MB_OK);
		return FALSE;
	}
	// �f�[�^�ǂݍ���
	ReadFile(hFile, LPVOID(&PressData), sizeof(PressData), &BytesRead, 0);
	CloseHandle(hFile);

	// �f�[�^��
	int i, j;
	int x, y;
	int data;
	int counter, maxim;

	for (i = 0, j = 0, counter = 0; i < FILE_DATA_MAX; ++i) {
		if ((PressData[i] == 0) && (PressData[i + 1] == 0) && (PressData[i + 2] == 0)) break;

		g_MapData[j] = PressData[i];
		++j;
		if (PressData[i] == PressData[i + 1]) {
			maxim = unsignedByte(PressData[i + 2]);
			for (counter = 0; counter < maxim; ++counter) {
				g_MapData[j] = PressData[i];
				++j;
			}
			i += 2;
		}
	}
	point2 = i + 3;

	if (g_MapData[DATA_VERSION] <= 29) {
		DATA_MAP_COUNT = 3;
		DATA_OBJECT_COUNT = 4;
		DATA_CHARA_X = 5;
		DATA_CHARA_Y = 6;
		DATA_OVER_X = 18;
		DATA_OVER_Y = 19;
		charaX = unsignedByte(g_MapData[DATA_CHARA_X]);
		charaY = unsignedByte(g_MapData[DATA_CHARA_Y]);
		gameoverXp = unsignedByte(g_MapData[DATA_OVER_X]);
		gameoverYp = unsignedByte(g_MapData[DATA_OVER_Y]);
		iDataMapCount = unsignedByte(g_MapData[DATA_MAP_COUNT]);
		iDataObjectCount = unsignedByte(g_MapData[DATA_OBJECT_COUNT]);
	}
	DATA_MAP_COUNT = 34;
	DATA_OBJECT_COUNT = 36;
	DATA_CHARA_X = 38;
	DATA_CHARA_Y = 40;
	DATA_OVER_X = 42;
	DATA_OVER_Y = 44;

	if (g_MapData[DATA_VERSION] >= 30) {
		charaX = unsignedByte(g_MapData[DATA_CHARA_X]) + unsignedByte(g_MapData[DATA_CHARA_X + 1]) * 0x100;
		charaY = unsignedByte(g_MapData[DATA_CHARA_Y]) + unsignedByte(g_MapData[DATA_CHARA_Y + 1]) * 0x100;
		gameoverXp = unsignedByte(g_MapData[DATA_OVER_X]) + unsignedByte(g_MapData[DATA_OVER_X + 1]) * 0x100;
		gameoverYp = unsignedByte(g_MapData[DATA_OVER_Y]) + unsignedByte(g_MapData[DATA_OVER_Y + 1]) * 0x100;
		iDataMapCount = unsignedByte(g_MapData[DATA_MAP_COUNT]) + unsignedByte(g_MapData[DATA_MAP_COUNT + 1]) * 0x100;
		iDataObjectCount = unsignedByte(g_MapData[DATA_OBJECT_COUNT]) + unsignedByte(g_MapData[DATA_OBJECT_COUNT + 1]) * 0x100;
	}

	// ��{�ݒ�
	statusEnergyMax = unsignedByte(g_MapData[DATA_STATUS_ENERGYMAX]) + unsignedByte(g_MapData[DATA_STATUS_ENERGYMAX + 1]) * 0x100;
	statusEnergy = unsignedByte(g_MapData[DATA_STATUS_ENERGY]) + unsignedByte(g_MapData[DATA_STATUS_ENERGY + 1]) * 0x100;
	statusStrength = unsignedByte(g_MapData[DATA_STATUS_STRENGTH]) + unsignedByte(g_MapData[DATA_STATUS_STRENGTH + 1]) * 0x100;
	statusDefence = unsignedByte(g_MapData[DATA_STATUS_DEFENCE]) + unsignedByte(g_MapData[DATA_STATUS_DEFENCE + 1]) * 0x100;
	statusGold = unsignedByte(g_MapData[DATA_STATUS_GOLD]) + unsignedByte(g_MapData[DATA_STATUS_GOLD + 1]) * 0x100;
	g_iMapSize = unsignedByte(g_MapData[DATA_MAP_SIZE]) + unsignedByte(g_MapData[DATA_MAP_SIZE + 1]) * 0x100;
	g_iMesNumberMax = unsignedByte(g_MapData[DATA_MES_NUMBER]) + unsignedByte(g_MapData[DATA_MES_NUMBER + 1]) * 0x100;

	for (i = 0; i < 12; ++i) {
		itemBox[i] = unsignedByte(g_MapData[DATA_ITEM + i]);
	}

	// �}�b�v�f�[�^
	pointer = 100;
	// ���ʃo�[�W��������̓ǂݍ��݃v���e�N�g
	if (g_MapData[DATA_VERSION] >= 29) pointer = 90;
	// ���o�[�W�����p�ɃR���o�[�g
	int MapWidth = g_iMapSize;
	if (g_MapData[DATA_VERSION] < 28) {
		MapWidth = 71;
		g_iMapSize = 71;
	}
	else if (g_MapData[DATA_VERSION] <= 29) {
		MapWidth = 101;
		g_iMapSize = 101;
	}
	// �X�N���[���o�[�ݒ�
	SetScrollRange(g_hWnd, SB_VERT, 0, (g_iMapSize - SCREEN_CHIP_SIZE), FALSE);
	SetScrollRange(g_hWnd, SB_HORZ, 0, (g_iMapSize - SCREEN_CHIP_SIZE), FALSE);

	// �p�[�c�ő吔�ݒ�
	g_iMapPartsMax = ((iDataMapCount - 1) / 50) * 50 + 50;
	if (g_iMapPartsMax < 200) g_iMapPartsMax = 200;
	g_iObjectPartsMax = ((iDataObjectCount - 1) / 50) * 50 + 50;
	if (g_iObjectPartsMax < 200) g_iObjectPartsMax = 200;
	// �X�N���[���o�[�ݒ�
	SetScrollRange(g_hDlgSelectMap, SB_VERT, 0, ((g_iMapPartsMax / 10) - 3), FALSE);
	SetScrollRange(g_hDlgSelectObject, SB_VERT, 0, ((g_iObjectPartsMax / 10) - 3), FALSE);

	ZeroMemory(&map, sizeof(map));
	ZeroMemory(&mapObject, sizeof(mapObject));

	for (x = 0; x < MapWidth; ++x) {
		for (y = 0; y < MapWidth; ++y) {
			if (g_MapData[DATA_VERSION] <= 29) {
				map[x][y] = (unsigned char)unsignedByte(g_MapData[pointer]);
				++pointer;
			}
			else {
				map[x][y] = unsignedByte(g_MapData[pointer]) + unsignedByte(g_MapData[pointer + 1]) * 0x100;
				pointer += 2;
			}
		}
	}
	for (x = 0; x < MapWidth; ++x) {
		for (y = 0; y < MapWidth; ++y) {
			if (g_MapData[DATA_VERSION] <= 29) {
				mapObject[x][y] = (unsigned char)unsignedByte(g_MapData[pointer]);
				++pointer;
			}
			else {
				mapObject[x][y] = unsignedByte(g_MapData[pointer]) + unsignedByte(g_MapData[pointer + 1]) * 0x100;
				pointer += 2;
			}
		}
	}

	ZeroMemory(&mapAttribute, sizeof(mapAttribute));
	ZeroMemory(&objectAttribute, sizeof(objectAttribute));

	// �e���b�Z�[�W�f�[�^�����p����Ă��邩�m�F����z��
	BOOL usedMessage[MESSAGE_NUMBER_MAX];
	for (i = 0; i < MESSAGE_NUMBER_MAX; ++i) usedMessage[i] = FALSE;

	if (g_MapData[DATA_VERSION] <= 29) {
		g_iMapAtrMax = 40;
		g_iObjectAtrMax = 40;
	}
	else {
		g_iMapAtrMax = MAP_ATR_MAX;
		g_iObjectAtrMax = OBJECT_ATR_MAX;
	}
	//�}�b�v�L�����N�^
	for (i = 0; i < iDataMapCount; ++i) {
		for (j = 0; j < g_iMapAtrMax; ++j) {
			mapAttribute[i][j] = unsignedByte(g_MapData[pointer]);
			++pointer;
			data = unsignedByte(g_MapData[pointer]);
			mapAttribute[i][j] += data * 0x100;
			++pointer;
		}
		if (mapAttribute[i][ATR_STRING] != 0) {
			usedMessage[mapAttribute[i][ATR_STRING]] = TRUE;
		}
	}
	//�I�u�W�F�N�g�L�����N�^
	for (i = 0; i < iDataObjectCount; ++i) {
		for (j = 0; j < g_iObjectAtrMax; ++j) {
			objectAttribute[i][j] = unsignedByte(g_MapData[pointer]);
			++pointer;
			data = unsignedByte(g_MapData[pointer]);
			objectAttribute[i][j] += data * 0x100;
			++pointer;
		}
		if (objectAttribute[i][ATR_STRING] != 0) {
			usedMessage[objectAttribute[i][ATR_STRING]] = TRUE;
		}
	}
	//���ʌ݊��g���L�����N�^�ϊ�
	if (g_MapData[DATA_VERSION] <= 29) {
		for (j = 0; j < PARTS_NUMBER_MAX; ++j) {
			for (i = 9; i >= 0; --i) {
				dataChara = objectAttribute[j][20 + i * 2];
				dataChara = dataChara & 0xff;
				dataMode = objectAttribute[j][20 + i * 2];
				dataMode = dataMode >> 8;
				x = objectAttribute[j][20 + i * 2 + 1];
				x = x & 0xff;
				y = objectAttribute[j][20 + i * 2 + 1];
				y = y >> 8;
				if (x == 250) x = 9000;
				else if (x > 100) x += (10000 - 160);
				if (y == 250) y = 9000;
				else if (y > 100) y += (10000 - 160);

				objectAttribute[j][20 + i * 4] = dataChara;
				objectAttribute[j][20 + i * 4 + 3] = dataMode;
				objectAttribute[j][20 + i * 4 + 1] = x;
				objectAttribute[j][20 + i * 4 + 2] = y;

				dataChara = mapAttribute[j][20 + i * 2];
				dataChara = dataChara & 0xff;
				dataMode = mapAttribute[j][20 + i * 2];
				dataMode = dataMode >> 8;
				x = mapAttribute[j][20 + i * 2 + 1];
				x = x & 0xff;
				y = mapAttribute[j][20 + i * 2 + 1];
				y = y >> 8;
				if (x == 250) x = 9000;
				else if (x > 100) x += (10000 - 160);
				if (y == 250) y = 9000;
				else if (y > 100) y += (10000 - 160);

				mapAttribute[j][20 + i * 4] = dataChara;
				mapAttribute[j][20 + i * 4 + 3] = dataMode;
				mapAttribute[j][20 + i * 4 + 1] = x;
				mapAttribute[j][20 + i * 4 + 2] = y;
			}
			if (objectAttribute[j][ATR_TYPE] == OBJECT_LOCALGATE) {
				if (objectAttribute[j][ATR_JUMP_X] > 100) objectAttribute[j][ATR_JUMP_X] += (10000 - 160);
				if (objectAttribute[j][ATR_JUMP_Y] > 100) objectAttribute[j][ATR_JUMP_Y] += (10000 - 160);
			}
			if (mapAttribute[j][ATR_TYPE] == MAP_LOCALGATE) {
				if (mapAttribute[j][ATR_JUMP_X] > 100) mapAttribute[j][ATR_JUMP_X] += (10000 - 160);
				if (mapAttribute[j][ATR_JUMP_Y] > 100) mapAttribute[j][ATR_JUMP_Y] += (10000 - 160);
			}
		}
	}

	//���b�Z�[�W�f�[�^�̓ǂ݂���
	for (i = 0; i < MESSAGE_NUMBER_MAX; ++i) g_StrMessage[i][0] = '\0';
	pointer = point2;

	//�o�[�W�����A�b�v���e�X�g�݊��p
	if (g_MapData[DATA_VERSION] <= 29) {
		g_iMesNumberMax = 400;
	}
	else {
		//�V�Ïؔԍ�
		loadMapString(g_worldPassword);
	}
	for (i = 0; i < g_iMesNumberMax; ++i) {
		if (usedMessage[i] == TRUE) {
			loadMapString(g_StrMessage[i]);
		}
		else {
			loadMapString(g_StrMessage[i], FALSE);
		}
	}
	//���̑��f�[�^
	loadMapString(g_worldName);

	//���Ïؔԍ�
	if (g_MapData[DATA_VERSION] <= 29) loadMapString(g_worldPassword);
	else loadMapString(szTemp);

	loadMapString(g_mapcgNameBmp);
	loadMapString(g_mapcgName);

	if (g_MapData[DATA_VERSION] >= 30) {
		for (i = 0; i < 20; ++i) loadMapString(g_StrMessageSystem[i]);
	}

	//�Ïؔԍ��̃`�F�b�N
	g_bErrorPassword = FALSE;
	g_szInputPassword[0] = '\0';
	int number;
	if (atoi(g_worldPassword) != 0) {
		if (g_MapData[DATA_VERSION] >= 29) {
			//�Ïؔԍ��ϊ�
			number = atoi(g_worldPassword);
			number = (((number / 10) - 1197) / 17) - 2357;
			_itoa(number, g_worldPassword, 10);
		}
		ShowWindow(g_hDlgSelectObject, FALSE);
		ShowWindow(g_hDlgSelectMap, FALSE);
		ShowWindow(g_hDlgQuickView, FALSE);
		//if( g_bInitial == FALSE ) LibLoad();
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_PASSWORD), g_hWnd, (DLGPROC)DialogProcPassword);
		ShowWindow(g_hDlgSelectObject, TRUE);
		ShowWindow(g_hDlgSelectMap, TRUE);
		ShowWindow(g_hDlgQuickView, TRUE);
	}
	//Undo�Z�b�g
	SetUndoData();
	InvalidateRect(g_hDlgQuickView, NULL, FALSE);

	//�Ïؔԍ�����
	if (strcmp(g_worldPassword, g_szInputPassword) != 0) {
		ZeroMemory(&mapAttribute, sizeof(mapAttribute));
		ZeroMemory(&objectAttribute, sizeof(objectAttribute));
		ZeroMemory(&g_worldPassword, sizeof(g_worldPassword));
	}

	//�^�C�g���e�L�X�g�ݒ�
	char sTitle[100];
	sprintf(sTitle, "%s [%s]", g_szTitleName, FileName);
	SetWindowText(g_hWnd, sTitle);

	return TRUE;
}



//##------------------------------------------------------------------
// �}�b�v�f�[�^�̕ۑ�

BOOL SaveMapData( char *FileName )
{
	//�ϐ���`
	int i, j;
	int x, y;
	int number;
	int counter;
	int length;
	int checkData = 0;
	char szSavePassword[30];
	int xmax, ymax;

	//�_�C�A���O����
	DestroyWindow( g_hDlgObject );
	DestroyWindow( g_hDlgMap );
	DestroyWindow( g_hDlgSelectChara );

	for( i = 0 ; i < 100 ; ++i ) PressData[i] = 0;

	PressData[DATA_VERSION] = 31;
	PressData[DATA_STATUS_ENERGYMAX] = (char)statusEnergyMax;
	PressData[DATA_STATUS_ENERGYMAX +1] = (char)(statusEnergyMax >> 8);
	PressData[DATA_STATUS_ENERGY] = (char)statusEnergy;
	PressData[DATA_STATUS_ENERGY +1] = (char)(statusEnergy >> 8);
	PressData[DATA_STATUS_STRENGTH] = (char)statusStrength;
	PressData[DATA_STATUS_STRENGTH +1] = (char)(statusStrength >> 8);
	PressData[DATA_STATUS_DEFENCE] = (char)statusDefence;
	PressData[DATA_STATUS_DEFENCE +1] = (char)(statusDefence >> 8);
	PressData[DATA_STATUS_GOLD] = (char)statusGold;
	PressData[DATA_STATUS_GOLD +1] = (char)(statusGold >> 8);

	PressData[DATA_CHARA_X] = (char)charaX;
	PressData[DATA_CHARA_X +1] = (char)(charaX >> 8);
	PressData[DATA_CHARA_Y] = (char)charaY;
	PressData[DATA_CHARA_Y +1] = (char)(charaY >> 8);
	PressData[DATA_OVER_X] = (char)gameoverXp;
	PressData[DATA_OVER_X +1] = (char)(gameoverXp >> 8);
	PressData[DATA_OVER_Y] = (char)gameoverYp;
	PressData[DATA_OVER_Y +1] = (char)(gameoverYp >> 8);

	for( i = 0 ; i < 12 ; ++i ){
		PressData[DATA_ITEM +i] = (char)itemBox[i];
	}
	//�}�b�v��ʍő吔�ݒ�
	for( x = (MAP_SIZE_MAX-1) ; x >= 0 ; --x ){
		for( y = (MAP_SIZE_MAX-1) ; y >= 0 ; --y ){
			if( (map[x][y] != 0) || (mapObject[x][y] != 0) ) break;
		}
		if( y >= 0 ) break;
	}
	xmax = x;
	for( y = (MAP_SIZE_MAX-1) ; y >= 0 ; --y ){
		for( x = (MAP_SIZE_MAX-1) ; x >= 0 ; --x ){
			if( (map[x][y] != 0) || (mapObject[x][y] != 0) ) break;
		}
		if( x >= 0 ) break;
	}
	ymax = y;
	if( xmax < ymax ) xmax = ymax;
	g_iMapSize = ((xmax -1) /50) *50 +51;
	if( g_iMapSize < 101 ) g_iMapSize = 101;
	PressData[DATA_MAP_SIZE] = (char)g_iMapSize;
	PressData[DATA_MAP_SIZE +1] = (char)(g_iMapSize >> 8);

	//���ʃo�[�W��������̓ǂݍ��݃v���e�N�g
	pointer = 90;	//pointer = 100;

	//�}�b�v�f�[�^
	for( x = 0 ; x < g_iMapSize ; ++x ){
		for( y = 0 ; y < g_iMapSize ; ++y ){
			PressData[pointer] = (char)map[x][y];
			PressData[pointer +1] = (char)(map[x][y] >> 8);
			pointer += 2;
		}
	}
	for( x = 0 ; x < g_iMapSize ; ++x ){
		for( y = 0 ; y < g_iMapSize ; ++y ){
			PressData[pointer] = (char)mapObject[x][y];
			PressData[pointer +1] = (char)(mapObject[x][y] >> 8);
			pointer += 2;
		}
	}

	//�}�b�v�L�����N�^
	for( number = (PARTS_NUMBER_MAX -1) ; number > 0 ; --number ){
		if( mapAttribute[number][ATR_0] != 0 ) break;
	}
	++number;
	PressData[DATA_MAP_COUNT] = (char)number;
	PressData[DATA_MAP_COUNT +1] = (char)(number >> 8);

	for( i = 0 ; i < number ; ++i ){
		for( j = 0 ; j < MAP_ATR_MAX ; ++j ){
			PressData[pointer] = (char)mapAttribute[i][j];
			++pointer;
			PressData[pointer] = (char)(mapAttribute[i][j] >> 8);
			++pointer;
		}
	}

	//�I�u�W�F�N�g�L�����N�^
	for( number = (PARTS_NUMBER_MAX -1) ; number > 0 ; --number ){
		if( objectAttribute[number][ATR_0] != 0 ) break;
	}
	++number;
	PressData[DATA_OBJECT_COUNT] = (char)number;
	PressData[DATA_OBJECT_COUNT +1] = (char)(number >> 8);

	for( i = 0 ; i < number ; ++i ){
		for( j = 0 ; j < OBJECT_ATR_MAX ; ++j ){
			PressData[pointer] = (char)objectAttribute[i][j];
			++pointer;
			PressData[pointer] = (char)(objectAttribute[i][j] >> 8);
			++pointer;
		}
	}
	//���b�Z�[�W�f�[�^�̍Ō�����o
	for( i = (MESSAGE_NUMBER_MAX -1) ; i > 0 ; --i ){
		if( g_StrMessage[i][0] != '\0' ) break;
	}
	g_iMesNumberMax = i +1;
	PressData[DATA_MES_NUMBER] = (char)g_iMesNumberMax;
	PressData[DATA_MES_NUMBER +1] = (char)(g_iMesNumberMax >> 8);

	//���k���f�[�^��
	length = pointer;

	//�`�F�b�N�p����ԍ�
	for( i = 2 ; i < length ; ++i ) checkData += (PressData[i] *(i %8 +1));
	checkData %= 0x10000;
	PressData[DATA_CHECK] = (char)checkData;
	PressData[DATA_CHECK +1] = (char)(checkData >> 8);

	//�}�b�v�f�[�^���k
	for( i = 0, j = 0, counter = 0 ; i < length ; ++i ){
		if( PressData[i] == PressData[i+1] ){
			++counter;
			if( (counter == 0xff) || (i +2 > length) ){
				g_MapData[j] = PressData[i];
				g_MapData[j+1] = PressData[i];
				g_MapData[j+2] = (byte)counter;
				j += 3;
				++i;
				counter = 0;
			}
		} else {
			if( counter == 0 ){
				g_MapData[j] = PressData[i];
				++j;
			} else {
				g_MapData[j] = PressData[i];
				g_MapData[j+1] = PressData[i];
				g_MapData[j+2] = (byte)counter;
				j += 3;
			}
			counter = 0;
		}
	}
	g_MapData[j] = 0;
	g_MapData[j+1] = 0;
	g_MapData[j+2] = 0;
	pointer = j +3;

	//�Ïؔԍ��ϊ�
	number = atoi( g_worldPassword );
	if( number != 0 ){
		number = ((number +2357) *17 +1197) *10 +(number %9);
		_itoa( number, szSavePassword, 10 );
	} else {
		szSavePassword[0] = '\0';
	}

	//�V�Ïؔԍ�
	saveMapString( szSavePassword );
	//���b�Z�[�W�f�[�^�̏�������
	for (i = 0; i < g_iMesNumberMax; ++i) {
		saveMapString(g_StrMessage[i]);
	}
	//���̑��f�[�^
	saveMapString( g_worldName );
	saveMapString( "" );
	saveMapString( g_mapcgNameBmp );
	saveMapString( g_mapcgName );
	for( i = 0 ; i < 20 ; ++i ) saveMapString( g_StrMessageSystem[i] );

	//�f�[�^�T�C�Y�̊m�F
	if( pointer >= FILE_DATA_MAX ){
		MessageBox( g_hWnd, "�}�b�v�f�[�^�̑��e�ʂ����e�l�𒴂��Ă��܂��B", "���ӁI", MB_OK );
		return FALSE;
	}

	//�f�[�^�̏�������
	HANDLE hFile;
	DWORD dwWritten;
	hFile = CreateFile( FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE ){
		MessageBox( g_hWnd, "�}�b�v�f�[�^�t�@�C���ɏ������݂ł��܂���B\n�ǂݎ���p�ɂȂ��Ă��Ȃ����m�F���Ă��������B", "����", MB_OK );
		return FALSE;
	}
	WriteFile( hFile, g_MapData, pointer, &dwWritten, NULL );
	CloseHandle( hFile );

	//�^�C�g���e�L�X�g�ݒ�
	char sTitle[100];
	sprintf( sTitle, "%s [%s]", g_szTitleName, FileName );
	SetWindowText( g_hWnd, sTitle );

	//�o�b�N�A�b�v�f�[�^�쐬
	char szBackupFile[250];
	SYSTEMTIME systime;
	GetLocalTime( &systime );
	sprintf( szBackupFile, "backup\\%s%d.dat", FileName, systime.wHour );
	//��������
	hFile = CreateFile( szBackupFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile != INVALID_HANDLE_VALUE ){
		WriteFile( hFile, g_MapData, pointer, &dwWritten, NULL );
		CloseHandle( hFile );
	}

	g_bUpdate = FALSE;
	return TRUE;
}


void saveMapString( char *str )
{
	int i;
	int length;
	WCHAR buff[MESSAGE_STR_MAX];

	if( strlen(str) != 0 ){
		length = MultiByteToWideChar( CP_ACP, 0, str, strlen(str), buff, sizeof(buff) );
		buff[length] = '\0';
		
		for( i = 0 ; i < (int)wcslen(buff) ; ++i ){
			if( buff[i] == 0x0D ) continue;
			g_MapData[pointer] = (char)(buff[i] & 0xff);
			++pointer;
			g_MapData[pointer] = (char)(buff[i] >> 8);
			++pointer;
		}
	}
	g_MapData[pointer] = 0;
	++pointer;
	g_MapData[pointer] = 0;
	++pointer;
}



//##------------------------------------------------------------------
// �}�b�v�̕`��
void paintMapAll(HDC hDC)
{
	//�ϐ���`
	int i, j;
	int mdata;
	char objectNumber[10];
	HFONT numberFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FF_DONTCARE);
	HFONT defaultFont = SelectFont(hDC, numberFont);

	//�}�b�v�`��
	for (j = 0; j < SCREEN_CHIP_SIZE; ++j) {
		for (i = 0; i < SCREEN_CHIP_SIZE; ++i) {
			//�w�i�`��
			mdata = map[j + mapYtop][i + mapXtop];
			BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, CHIP_SIZE, CHIP_SIZE, g_hmDC, mapAttribute[mdata][ATR_X], mapAttribute[mdata][ATR_Y], SRCCOPY);

			//�I�u�W�F�N�g�`��
			mdata = mapObject[j + mapYtop][i + mapXtop];
			if (mdata != 0) {
				BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, CHIP_SIZE, CHIP_SIZE, g_hmDCAnd, objectAttribute[mdata][ATR_X], objectAttribute[mdata][ATR_Y], SRCAND);
				BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, CHIP_SIZE, CHIP_SIZE, g_hmDCOr, objectAttribute[mdata][ATR_X], objectAttribute[mdata][ATR_Y], SRCPAINT);
			}
			//�v���[���[�L�����N�^�`��
			if ((charaX == (i + mapXtop)) && (charaY == (j + mapYtop))) {
				BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, CHIP_SIZE, CHIP_SIZE, g_hmDCAnd, CHIP_SIZE * 4, 0, SRCAND);
				BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, CHIP_SIZE, CHIP_SIZE, g_hmDCOr, CHIP_SIZE * 4, 0, SRCPAINT);
			}
			//�����p�[�c���ʃ}�[�N�`��
			if (g_ObjectNumberFlag == TRUE && mdata != 0) {
				sprintf(objectNumber, "%d", mdata);
				TextOut(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, objectNumber, strlen(objectNumber));
			}
		}
	}

	SelectFont(hDC, defaultFont);
}


//##------------------------------------------------------------------
// ���̑I���_�C�A���O�̕`��
void PaintWindowSelectObject(HWND hWnd)
{
	int i, j;
	int x, y;
	HDC hDC;
	PAINTSTRUCT ps;
	HPEN hpen, hpenOld;
	hDC = BeginPaint(hWnd, &ps);
	int selectX = g_SelectObjectData % DIALOG_OBJECT_SELECT_COLUMN;
	int selectY = g_SelectObjectData / DIALOG_OBJECT_SELECT_COLUMN;
	char str[50];

	//�C���[�W�̕`��
	for (j = 0; j < DIALOG_OBJECT_SELECT_LINE; ++j) {
		for (i = 0; i < DIALOG_OBJECT_SELECT_COLUMN; ++i) {
			x = objectAttribute[i + j * DIALOG_OBJECT_SELECT_COLUMN + g_ScrObject * DIALOG_OBJECT_SELECT_COLUMN][ATR_X];
			y = objectAttribute[i + j * DIALOG_OBJECT_SELECT_COLUMN + g_ScrObject * DIALOG_OBJECT_SELECT_COLUMN][ATR_Y];
			BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE, CHIP_SIZE, CHIP_SIZE, g_hmDC, x, y, SRCCOPY);
		}
	}

	SetDlgItemInt(hWnd, IDC_EDIT_PARTS_NUMBER, g_SelectObjectData, FALSE);
	SetDlgItemInt(hWnd, IDC_EDIT_CURRENT_POSITION, g_ScrObject * DIALOG_OBJECT_SELECT_COLUMN, FALSE);

	//���E���\��
	hpen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	hpenOld = (HPEN)SelectObject(hDC, hpen);

	//�g�\��
	if ((selectY - g_ScrObject) * CHIP_SIZE < CHIP_SIZE * DIALOG_OBJECT_SELECT_LINE) {
		DrawRect(hDC, selectX * CHIP_SIZE, (selectY - g_ScrObject) * CHIP_SIZE, CHIP_SIZE, CHIP_SIZE);
		DrawRect(hDC, selectX * CHIP_SIZE + 1, (selectY - g_ScrObject) * CHIP_SIZE + 1, 38, 38);
	}

	SelectObject(hDC, hpenOld);
	DeleteObject(hpen);
	EndPaint(hWnd, &ps);
}


//##------------------------------------------------------------------
// �w�i�I���_�C�A���O�̕`��
void PaintWindowSelectMap(HWND hWnd)
{
	int i, j;
	int x, y;
	HDC hDC;
	PAINTSTRUCT ps;
	HPEN hpen, hpenOld;
	hDC = BeginPaint(hWnd, &ps);
	int selectX = g_SelectMapData % DIALOG_MAP_SELECT_COLUMN;
	int selectY = g_SelectMapData / DIALOG_MAP_SELECT_COLUMN;
	char str[50];

	//�C���[�W�̕`��
	for (j = 0; j < DIALOG_MAP_SELECT_LINE; ++j) {
		for (i = 0; i < DIALOG_MAP_SELECT_COLUMN; ++i) {
			x = mapAttribute[i + j * 10 + g_ScrMap * 10][ATR_X];
			y = mapAttribute[i + j * 10 + g_ScrMap * 10][ATR_Y];
			BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE, CHIP_SIZE, CHIP_SIZE, g_hmDC, x, y, SRCCOPY);
		}
	}
	//�����\��
	SetDlgItemInt(hWnd, IDC_EDIT_PARTS_NUMBER, g_SelectMapData, FALSE);
	SetDlgItemInt(hWnd, IDC_EDIT_CURRENT_POSITION, g_ScrMap * DIALOG_MAP_SELECT_COLUMN, FALSE);

	//���E���\��
	hpen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	hpenOld = (HPEN)SelectObject(hDC, hpen);

	//�g�\��
	if ((selectY - g_ScrMap) * CHIP_SIZE < CHIP_SIZE * DIALOG_MAP_SELECT_LINE) {
		DrawRect(hDC, selectX * CHIP_SIZE, (selectY - g_ScrMap) * CHIP_SIZE, CHIP_SIZE, CHIP_SIZE);
		DrawRect(hDC, selectX * CHIP_SIZE + 1, (selectY - g_ScrMap) * CHIP_SIZE + 1, CHIP_SIZE - 2, CHIP_SIZE - 2);
	}
	SelectObject(hDC, hpenOld);
	DeleteObject(hpen);
	EndPaint(hWnd, &ps);
}



//##------------------------------------------------------------------
// ���̑I���_�C�A���O�v���V�[�W��
LRESULT CALLBACK SelectObjectDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG: {
		SetScrollRange(hWnd, SB_VERT, 0, ((g_iObjectPartsMax / DIALOG_OBJECT_SELECT_COLUMN) - DIALOG_OBJECT_SELECT_LINE), FALSE);
		break;
	}
	case WM_LBUTTONDOWN: {
		int x, y;
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (x < CHIP_SIZE * DIALOG_OBJECT_SELECT_COLUMN) {
			g_SelectObjectX = x / CHIP_SIZE;
		}
		if (y < CHIP_SIZE * DIALOG_OBJECT_SELECT_LINE) {
			g_SelectObjectY = g_ScrObject + y / CHIP_SIZE;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		InvalidateRect(g_hDlgQuickView, NULL, FALSE);
		// ���ݑI�𒆂̕��̂�ݒ�
		g_SelectObjectData = g_SelectObjectX + g_SelectObjectY * DIALOG_OBJECT_SELECT_COLUMN;
		// �ҏW���[�h�ύX
		DeleteCheckMenu();
		CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTOBJ, MF_CHECKED);
		g_EditMode = 1;
		PaintStatus(TRUE);
		break;
	}
	case WM_LBUTTONDBLCLK: {
		//�_�C�A���O�\��
		DestroyWindow(g_hDlgObject);
		DestroyWindow(g_hDlgMap);
		//Undo�Z�b�g
		SetUndoData();
		DisplayObjectDialog();
		break;
	}
	case WM_RBUTTONDOWN: {
		int x, y;
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (x < CHIP_SIZE * DIALOG_OBJECT_SELECT_COLUMN) {
			g_SelectObjectX = x / CHIP_SIZE;
		}
		if (y < CHIP_SIZE * DIALOG_OBJECT_SELECT_LINE) {
			g_SelectObjectY = g_ScrObject + y / CHIP_SIZE;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		// ���ݑI�𒆂̕��̂�ݒ�
		g_SelectObjectData = g_SelectObjectX + g_SelectObjectY * DIALOG_OBJECT_SELECT_COLUMN;
		// �ҏW���[�h�ύX
		DeleteCheckMenu();
		CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTOBJ, MF_CHECKED);
		g_EditMode = 1;
		PaintStatus(TRUE);
		// �_�C�A���O�\��
		DestroyWindow(g_hDlgObject);
		DestroyWindow(g_hDlgMap);
		// Undo�Z�b�g
		SetUndoData();
		DisplayObjectDialog();
		break;
	}
	case WM_COMMAND: {
		// ���̕ҏW�_�C�A���O�̕\��
		if (LOWORD(wParam) == IDC_BUTTON_EDITMAP) {
			DestroyWindow(g_hDlgObject);
			DestroyWindow(g_hDlgMap);
			// Undo�Z�b�g
			SetUndoData();
			DisplayObjectDialog();
		}
		// ���̃f�[�^�̏���
		else if (LOWORD(wParam) == IDC_BUTTON_MAP_ERASE) {
			int i;
			const int messageIndex = objectAttribute[g_SelectObjectData][ATR_STRING];
			for (i = 0; i < OBJECT_ATR_MAX; ++i) {
				objectAttribute[g_SelectObjectData][i] = 0;
			}
			g_StrMessage[messageIndex][0] = '\0';

			InvalidateRect(hWnd, NULL, FALSE);
			InvalidateRect(g_hWnd, NULL, FALSE);
		}
		else if (wParam == IDCANCEL) {
			EnableMenuItem(GetMenu(g_hWnd), ID_MENU_OBJWINDOW, MF_ENABLED);
			ShowWindow(hWnd, FALSE);
			return 1;
		}
		break;
	}

	case WM_VSCROLL:
		if (LOWORD(wParam) == SB_LINEDOWN) {
			if (g_ScrObject < ((g_iObjectPartsMax / DIALOG_OBJECT_SELECT_COLUMN) - DIALOG_OBJECT_SELECT_LINE)) {
				++g_ScrObject;
			}
			SetScrollPos(hWnd, SB_VERT, g_ScrObject, 1);
		}
		else if (LOWORD(wParam) == SB_LINEUP) {
			if (g_ScrObject > 0) --g_ScrObject;
			SetScrollPos(hWnd, SB_VERT, g_ScrObject, 1);
		}
		else if (LOWORD(wParam) == SB_THUMBPOSITION) {
			g_ScrObject = HIWORD(wParam);
			SetScrollPos(hWnd, SB_VERT, g_ScrObject, 1);
		}
		else if (LOWORD(wParam) == SB_THUMBTRACK) {
			g_ScrObject = HIWORD(wParam);
			SetScrollPos(hWnd, SB_VERT, g_ScrObject, 1);
		}
		else if (LOWORD(wParam) == SB_PAGEUP) {
			if (g_ScrObject > 0) --g_ScrObject;
			SetScrollPos(hWnd, SB_VERT, g_ScrObject, 1);
		}
		else if (LOWORD(wParam) == SB_PAGEDOWN) {
			if (g_ScrObject < ((g_iObjectPartsMax / DIALOG_OBJECT_SELECT_COLUMN) - 4)) {
				++g_ScrObject;
			}
			SetScrollPos(hWnd, SB_VERT, g_ScrObject, 1);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_MOUSEWHEEL: {
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
			if (g_ScrObject > 0) {
				--g_ScrObject;
			}
			SetScrollPos(hWnd, SB_VERT, g_ScrObject, 1);
		}
		else {
			if (g_ScrObject < ((g_iObjectPartsMax / DIALOG_OBJECT_SELECT_COLUMN) - DIALOG_OBJECT_SELECT_LINE)) {
				++g_ScrObject;
			}
			SetScrollPos(hWnd, SB_VERT, g_ScrObject, 1);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}

	case WM_PAINT:
		PaintWindowSelectObject(hWnd);
		break;
	}
	return 0;
}



//##------------------------------------------------------------------
// �w�i�I���_�C�A���O�v���V�[�W��
LRESULT CALLBACK SelectMapDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ){
	case WM_INITDIALOG:{
		SetScrollRange( hWnd, SB_VERT, 0, ((g_iMapPartsMax / DIALOG_MAP_SELECT_COLUMN) - DIALOG_MAP_SELECT_LINE), FALSE);
		break;
	}
	case WM_LBUTTONDOWN:{
		int x, y;
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (x < CHIP_SIZE * DIALOG_MAP_SELECT_COLUMN) {
			g_SelectMapX = x / CHIP_SIZE;
		}
		if (y < CHIP_SIZE * DIALOG_MAP_SELECT_LINE) {
			g_SelectMapY = g_ScrMap + y / CHIP_SIZE;
		}
		InvalidateRect( hWnd, NULL, FALSE );
		InvalidateRect( g_hDlgQuickView, NULL, FALSE );
		//���ݑI�𒆂̔w�i��ݒ�
		g_SelectMapData = g_SelectMapX + g_SelectMapY * DIALOG_MAP_SELECT_COLUMN;
		//�ҏW���[�h�ύX
		DeleteCheckMenu();
		CheckMenuItem( GetMenu(g_hWnd), ID_MENU_PUTMAP, MF_CHECKED );
		g_EditMode = 0;
		PaintStatus( TRUE );
		break;
	}
	case WM_LBUTTONDBLCLK:{
		// �_�C�A���O�\��
		DestroyWindow( g_hDlgObject );
		DestroyWindow( g_hDlgMap );
		// Undo�Z�b�g
		SetUndoData();
		DisplayMapDialog();
		break;
	}
	case WM_RBUTTONDOWN:{
		int x, y;
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (x < CHIP_SIZE * DIALOG_MAP_SELECT_COLUMN) {
			g_SelectMapX = x / CHIP_SIZE;
		}
		if (y < CHIP_SIZE * DIALOG_MAP_SELECT_LINE) {
			g_SelectMapY = g_ScrMap + y / CHIP_SIZE;
		}
		InvalidateRect( hWnd, NULL, FALSE );
		// ���ݑI�𒆂̔w�i��ݒ�
		g_SelectMapData = g_SelectMapX + g_SelectMapY * DIALOG_MAP_SELECT_COLUMN;
		// �ҏW���[�h�ύX
		DeleteCheckMenu();
		CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTMAP, MF_CHECKED);
		g_EditMode = 0;
		PaintStatus( TRUE );
		// �_�C�A���O�\��
		DestroyWindow(g_hDlgObject);
		DestroyWindow(g_hDlgMap);
		// Undo�Z�b�g
		SetUndoData();
		DisplayMapDialog();
		break;
	}
	case WM_COMMAND:{
		// �w�i�ҏW�_�C�A���O�̕\��
		if( LOWORD(wParam) == IDC_BUTTON_EDITMAP ){
			DestroyWindow( g_hDlgObject );
			DestroyWindow( g_hDlgMap );
			// Undo�Z�b�g
			SetUndoData();
			DisplayMapDialog();
		}
		// �w�i�f�[�^�̏���
		else if( LOWORD(wParam) == IDC_BUTTON_MAP_ERASE ){
			int i;
			const int messageIndex = mapAttribute[g_SelectMapData][ATR_STRING];
			for (i = 0; i < MAP_ATR_MAX; ++i) {
				mapAttribute[g_SelectMapData][i] = 0;
			}
			g_StrMessage[messageIndex][0] = '\0';
			
			InvalidateRect(hWnd, NULL, FALSE);
			InvalidateRect(g_hWnd, NULL, FALSE);
		}
		else if( wParam == IDCANCEL ){
			EnableMenuItem(GetMenu(g_hWnd), ID_MENU_MAPWINDOW, MF_ENABLED);
			ShowWindow(hWnd, FALSE);
			return 1;
		}
		break;
	}

	case WM_VSCROLL:
		if( LOWORD(wParam) == SB_LINEDOWN ){
			if (g_ScrMap < ((g_iMapPartsMax / DIALOG_MAP_SELECT_COLUMN) - DIALOG_MAP_SELECT_LINE)) {
				++g_ScrMap;
			}
			SetScrollPos( hWnd, SB_VERT, g_ScrMap, 1 );
		} else if( LOWORD(wParam) == SB_LINEUP ){
			if( g_ScrMap > 0 ) --g_ScrMap;
			SetScrollPos( hWnd, SB_VERT, g_ScrMap, 1 );
		} else if( LOWORD(wParam) == SB_THUMBPOSITION ){
			g_ScrMap = HIWORD(wParam);
			SetScrollPos( hWnd, SB_VERT, g_ScrMap, 1 );
		} else if( LOWORD(wParam) == SB_THUMBTRACK ){
			g_ScrMap = HIWORD(wParam);
			SetScrollPos( hWnd, SB_VERT, g_ScrMap, 1 );
		} else if( LOWORD(wParam) == SB_PAGEDOWN ){
			if (g_ScrMap < ((g_iMapPartsMax / DIALOG_MAP_SELECT_COLUMN) - DIALOG_MAP_SELECT_LINE)) {
				++g_ScrMap;
			}
			SetScrollPos( hWnd, SB_VERT, g_ScrMap, 1 );
		} else if( LOWORD(wParam) == SB_PAGEUP ){
			if( g_ScrMap > 0 ) --g_ScrMap;
			SetScrollPos( hWnd, SB_VERT, g_ScrMap, 1 );
		}
		InvalidateRect( hWnd, NULL, FALSE );
		break;

	case WM_MOUSEWHEEL: {
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
			if (g_ScrMap > 0) --g_ScrMap;
			SetScrollPos(hWnd, SB_VERT, g_ScrMap, 1);
		}
		else {
			if (g_ScrMap < ((g_iMapPartsMax / DIALOG_MAP_SELECT_COLUMN) - DIALOG_MAP_SELECT_LINE)) {
				++g_ScrMap;
			}
			SetScrollPos(hWnd, SB_VERT, g_ScrMap, 1);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}

	case WM_PAINT:
		PaintWindowSelectMap( hWnd );
		break;
	}
	return 0;
}



//##------------------------------------------------------------------
// ���̕ҏW�_�C�A���O�v���V�[�W��
LRESULT CALLBACK EditObjectDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ){
	case WM_LBUTTONDOWN: {
		int x, y;
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if ((x > 5) && (x < 5 + CHIP_SIZE) && (y > 4) && (y < 4 + CHIP_SIZE)) {
			g_AtrSelectChara = ATR_X;
		}
		else if ((x > 50) && (x < 50 + CHIP_SIZE) && (y > 4) && (y < 4 + CHIP_SIZE)) {
			g_AtrSelectChara = ATR_X2;
		}
		else break;
		
		const int SelectCharaDialogY = 80;
		//�_�C�A���O�̍쐬
		DestroyWindow( g_hDlgSelectChara );
		g_hModeSelectChara = 1;
		g_hDlgSelectChara = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_SELECTCHARA), hWnd, (DLGPROC)SelectCGCharaProc );
		//�_�C�A���O�ړ�
		RECT rect;
		RECT rectBox;
		GetWindowRect( g_hDlgObject, &rectBox );
		GetWindowRect( g_hDlgSelectChara, &rect );
		MoveWindow( g_hDlgSelectChara, rectBox.left, rectBox.top + SelectCharaDialogY, rect.right -rect.left, rect.bottom -rect.top, TRUE );
		ShowWindow( g_hDlgSelectChara, SW_SHOW );
		break;
	}
	case WM_COMMAND:
		//���̎�ʂ̕ύX
		if( (HIWORD(wParam) == CBN_SELCHANGE) && (LOWORD(wParam) == IDC_COMBO_OBJECT) ){
			int number = SendMessage( GetDlgItem(hWnd,IDC_COMBO_OBJECT), CB_GETCURSEL, 0, 0 );
			objectAttribute[GetCharaNumber(hWnd)][ATR_TYPE] = OBJ[number].Object;
			g_bRestoreObjectDialog = TRUE;
			InvalidateRect( hWnd, NULL, FALSE );
		}
		//������
		else if( wParam == ID_BUTTON_CANCEL ){
			int i;
			for( i = 0 ; i < OBJECT_ATR_MAX ; ++i ) objectAttribute[GetCharaNumber(hWnd)][i] = StockObjectAttribute[GetCharaNumber(hWnd)][i];
			InvalidateRect( g_hWnd, NULL, FALSE );
			InvalidateRect( g_hDlgSelectObject, NULL, FALSE );
			g_bCancel = TRUE;
			DestroyWindow( hWnd );
			return 1;
		}
		//����
		else if( (wParam == IDOK) || (wParam == IDCANCEL) ){
			//�E�B���h�E����
			DestroyWindow( hWnd );
			return 1;
		}
		break;

	case WM_PAINT: {
		//�I�u�W�F�N�g�`��
		HDC hDC = GetDC( hWnd );
		BitBlt( hDC, 5, 4, CHIP_SIZE, CHIP_SIZE, g_hmDC, objectAttribute[GetCharaNumber(hWnd)][ATR_X], objectAttribute[GetCharaNumber(hWnd)][ATR_Y], SRCCOPY );
		BitBlt( hDC, 50, 4, CHIP_SIZE, CHIP_SIZE, g_hmDC, objectAttribute[GetCharaNumber(hWnd)][ATR_X2], objectAttribute[GetCharaNumber(hWnd)][ATR_Y2], SRCCOPY );
		//�����\��
		char str[50];
		SetBkMode( hDC, TRANSPARENT );
		sprintf( str, "���̔ԍ��F%d", GetCharaNumber(hWnd) );
		TextOut( hDC,96,27,str,strlen(str) );
		ReleaseDC( hWnd, hDC );
		//�E�B���h�E�ĕ`��
		if( g_bRestoreObjectDialog == TRUE ){
			DestroyWindow( hWnd );
			return 1;
		}
		break;
	}
	case WM_DESTROY:{
		if( g_bCancel == FALSE ){
			//���̃f�[�^�̌���
			SetObjectData( hWnd, GetCharaNumber(hWnd) );
			g_bUpdate = TRUE;
		}
		g_bCancel = FALSE;
		if( g_bRestoreObjectDialog == TRUE ) DisplayObjectDialog();
		InvalidateRect( g_hDlgQuickView, NULL, FALSE );
		return 1;
	}
	}
	return 0;
}



//##------------------------------------------------------------------
// �w�i�ҏW�_�C�A���O�v���V�[�W��
LRESULT CALLBACK EditMapDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message) {
	case WM_LBUTTONDOWN: {
		int x, y;
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if ((x > 5) && (x < 5 + CHIP_SIZE) && (y > 4) && (y < 4 + CHIP_SIZE)) {
			g_AtrSelectChara = ATR_X;
		}
		else break;

		const int SelectCharaDialogY = 80;
		//�_�C�A���O�̍쐬
		DestroyWindow(g_hDlgSelectChara);
		g_hModeSelectChara = 0;
		g_hDlgSelectChara = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_SELECTCHARA), hWnd, (DLGPROC)SelectCGCharaProc);
		//�_�C�A���O�ړ�
		RECT rect, rectBox;
		GetWindowRect(g_hDlgMap, &rectBox);
		GetWindowRect(g_hDlgSelectChara, &rect);
		MoveWindow(g_hDlgSelectChara, rectBox.left, rectBox.top + SelectCharaDialogY, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		ShowWindow(g_hDlgSelectChara, SW_SHOW);
		break;
	}
	case WM_COMMAND: {
		//�w�i��ʂ̕ύX
		if ((HIWORD(wParam) == CBN_SELCHANGE) && (LOWORD(wParam) == IDC_COMBO_MAP)) {
			int number = SendMessage(GetDlgItem(hWnd, IDC_COMBO_MAP), CB_GETCURSEL, 0, 0);
			mapAttribute[GetCharaNumber(hWnd)][ATR_TYPE] = MAP[number].Object;
			g_bRestoreMapDialog = TRUE;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		//������
		else if (wParam == ID_BUTTON_CANCEL) {
			int i;
			for (i = 0; i < MAP_ATR_MAX; ++i) {
				mapAttribute[GetCharaNumber(hWnd)][i] = StockMapAttribute[GetCharaNumber(hWnd)][i];
			}
			InvalidateRect(g_hWnd, NULL, FALSE);
			InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
			g_bCancel = TRUE;
			DestroyWindow(hWnd);
			return 1;
		}
		// ����
		else if ((wParam == IDOK) || (wParam == IDCANCEL)) {
			// �E�B���h�E����
			DestroyWindow(hWnd);
			return 1;
		}
		break;
	}
	case WM_PAINT: {
		// �I�u�W�F�N�g�`��
		HDC hDC = GetDC(hWnd);
		BitBlt(hDC, 5, 4, CHIP_SIZE, CHIP_SIZE, g_hmDC, mapAttribute[GetCharaNumber(hWnd)][ATR_X], mapAttribute[GetCharaNumber(hWnd)][ATR_Y], SRCCOPY);
		// �����\��
		char str[50];
		SetBkMode(hDC, TRANSPARENT);
		sprintf(str, "�w�i�ԍ��F%d", GetCharaNumber(hWnd));
		TextOut(hDC, 96, 27, str, strlen(str));
		ReleaseDC(hWnd, hDC);
		// �E�B���h�E�ĕ`��
		if (g_bRestoreMapDialog == TRUE) {
			DestroyWindow(hWnd);
			return 1;
		}
		break;
	}
	case WM_DESTROY: {
		if (g_bCancel == FALSE) {
			// �w�i�f�[�^�̌���
			SetMapData(hWnd, GetCharaNumber(hWnd));
			g_bUpdate = TRUE;
		}
		g_bCancel = FALSE;
		if (g_bRestoreMapDialog == TRUE) DisplayMapDialog();
		InvalidateRect(g_hDlgQuickView, NULL, FALSE);
		return 1;
	}
	}
	return 0;
}



//##------------------------------------------------------------------
// �N�C�b�N�r���[�_�C�A���O�v���V�[�W��
LRESULT CALLBACK QuickViewDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_PAINT: {
		HDC hDC = GetDC(hWnd);
		int type;
		if (g_EditMode == 0) {
			BitBlt(hDC, 5, 4, CHIP_SIZE, CHIP_SIZE, g_hmDC, mapAttribute[g_SelectMapData][ATR_X], mapAttribute[g_SelectMapData][ATR_Y], SRCCOPY);
			SetDlgItemText(g_hDlgQuickView, IDC_EDIT_QVIEW, g_StrMessage[mapAttribute[g_SelectMapData][ATR_STRING]]);
			//�����\��
			char str[50];
			SetBkColor(hDC, GetSysColor(COLOR_3DFACE));
			sprintf(str, "�w�i�߰ԍ��F%3d  ", g_SelectMapData);
			TextOut(hDC, 50, 5, str, strlen(str));
			type = mapAttribute[g_SelectMapData][ATR_TYPE];
			sprintf(str, "��ށF%s ", g_MapName[type]);
			TextOut(hDC, 50, 26, str, strlen(str));
		}
		else if (g_EditMode == 1) {
			BitBlt(hDC, 5, 4, CHIP_SIZE, CHIP_SIZE, g_hmDC, objectAttribute[g_SelectObjectData][ATR_X], objectAttribute[g_SelectObjectData][ATR_Y], SRCCOPY);
			SetDlgItemText(g_hDlgQuickView, IDC_EDIT_QVIEW, g_StrMessage[objectAttribute[g_SelectObjectData][ATR_STRING]]);
			//�����\��
			char str[50];
			SetBkColor(hDC, GetSysColor(COLOR_3DFACE));
			sprintf(str, "�����߰ԍ��F%3d  ", g_SelectObjectData);
			TextOut(hDC, 50, 5, str, strlen(str));
			type = objectAttribute[g_SelectObjectData][ATR_TYPE];
			sprintf(str, "��ށF%s ", g_ObjectName[type]);
			TextOut(hDC, 50, 26, str, strlen(str));
		}
		ReleaseDC(hWnd, hDC);
		break;
	}
	case WM_COMMAND: {
		if (wParam == IDCANCEL) {
			EnableMenuItem(GetMenu(g_hWnd), ID_MENU_QVIEW, MF_ENABLED);
			ShowWindow(hWnd, FALSE);
			return 1;
		}
		break;
	}
	}
	return 0;
}



//##------------------------------------------------------------------
// �L�����N�^�b�f�I���_�C�A���O�v���V�[�W��
LRESULT CALLBACK SelectCGCharaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x, y;

	switch (message) {
	case WM_INITDIALOG: {
		g_ScrCGChara = 0;
		if (g_ScrCGCharaMax == 0) ShowScrollBar(hWnd, SB_VERT, FALSE);
		else ShowScrollBar(hWnd, SB_VERT, TRUE);
		break;
	}
	case WM_LBUTTONDOWN: {
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if ((x < 200) && (y < (g_iLoadCGHeight / 2))) {
			if (g_hModeSelectChara == 0) {
				if (g_AtrSelectChara == ATR_X) {
					mapAttribute[g_EditMapData][ATR_X] = (x / 20) * CHIP_SIZE;
					mapAttribute[g_EditMapData][ATR_Y] = (y / 20 + g_ScrCGChara) * CHIP_SIZE;
				}
			}
			else {
				if (g_AtrSelectChara == ATR_X) {
					objectAttribute[g_EditObjectData][ATR_X] = (x / 20) * CHIP_SIZE;
					objectAttribute[g_EditObjectData][ATR_Y] = (y / 20 + g_ScrCGChara) * CHIP_SIZE;
				}
				else {
					objectAttribute[g_EditObjectData][ATR_X2] = (x / 20) * CHIP_SIZE;
					objectAttribute[g_EditObjectData][ATR_Y2] = (y / 20 + g_ScrCGChara) * CHIP_SIZE;
				}
			}
			DestroyWindow(hWnd);
			//��ʍĕ`��
			InvalidateRect(g_hWnd, NULL, FALSE);
			if (g_hModeSelectChara == 0) {
				InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
				InvalidateRect(g_hDlgMap, NULL, FALSE);
			}
			else {
				InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
				InvalidateRect(g_hDlgObject, NULL, FALSE);
			}
			return 1;
		}
		break;
	}
	case WM_VSCROLL: {
		SetScrollRange(hWnd, SB_VERT, 0, g_ScrCGCharaMax, FALSE);

		if (LOWORD(wParam) == SB_LINEDOWN) {
			if (g_ScrCGChara < g_ScrCGCharaMax) ++g_ScrCGChara;
			SetScrollPos(hWnd, SB_VERT, g_ScrCGChara, 1);
		}
		else if (LOWORD(wParam) == SB_LINEUP) {
			if (g_ScrCGChara > 0) --g_ScrCGChara;
			SetScrollPos(hWnd, SB_VERT, g_ScrCGChara, 1);
		}
		else if (LOWORD(wParam) == SB_THUMBPOSITION) {
			g_ScrCGChara = HIWORD(wParam);
			SetScrollPos(hWnd, SB_VERT, g_ScrCGChara, 1);
		}
		else if (LOWORD(wParam) == SB_THUMBTRACK) {
			g_ScrCGChara = HIWORD(wParam);
			SetScrollPos(hWnd, SB_VERT, g_ScrCGChara, 1);
		}
		else if (LOWORD(wParam) == SB_PAGEUP) {
			if (g_ScrCGChara > 0) --g_ScrCGChara;
			SetScrollPos(hWnd, SB_VERT, g_ScrCGChara, 1);
		}
		else if (LOWORD(wParam) == SB_PAGEDOWN) {
			if (g_ScrCGChara < g_ScrCGCharaMax) ++g_ScrCGChara;
			SetScrollPos(hWnd, SB_VERT, g_ScrCGChara, 1);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_MOUSEWHEEL: {
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
			if (g_ScrCGChara > 0) --g_ScrCGChara;
			SetScrollPos(hWnd, SB_VERT, g_ScrCGChara, 1);
		}
		else {
			if (g_ScrCGChara < g_ScrCGCharaMax) ++g_ScrCGChara;
			SetScrollPos(hWnd, SB_VERT, g_ScrCGChara, 1);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_PAINT: {
		HDC hDC = GetDC(hWnd);
		BitBlt(hDC, 0, 0, 200, 40 * 17, NULL, 0, 0, WHITENESS);
		BitBlt(hDC, 0, 0, 200, 40 * 17, g_hmDCHalf, 0, g_ScrCGChara * 20, SRCCOPY);
		break;
	}
	case WM_COMMAND: {
		if (wParam == IDCANCEL) {
			DestroyWindow(hWnd);
			return 1;
		}
		break;
	}
	}

	return 0;
}



//##------------------------------------------------------------------
// ���Ǝl�p�̕`��

void DrawLine( HDC hDC, int x, int y, int x2, int y2 )
{
	POINT line[2];
	
	line[0].x = x;
	line[0].y = y;
	line[1].x = x2;
	line[1].y = y2;
	Polyline( hDC, line, 2 );
}

void DrawRect( HDC hDC, int x, int y, int x2, int y2 )
{
	POINT line[2];
	
	line[0].x = x;
	line[0].y = y;
	line[1].x = x +x2 -1;
	line[1].y = y;
	Polyline( hDC, line, 2 );
	line[0].x = x +x2 -1;
	line[0].y = y +y2 -1;
	Polyline( hDC, line, 2 );
	line[1].x = x;
	line[1].y = y +y2 -1;
	Polyline( hDC, line, 2 );
	line[0].x = x;
	line[0].y = y;
	Polyline( hDC, line, 2 );
}

void DrawRect2( HDC hDC, int x, int y, int x2, int y2 )
{
	POINT line[2];
	
	line[0].x = x;
	line[0].y = y;
	line[1].x = x2;
	line[1].y = y;
	Polyline( hDC, line, 2 );
	line[0].x = x2;
	line[0].y = y2;
	Polyline( hDC, line, 2 );
	line[1].x = x;
	line[1].y = y2;
	Polyline( hDC, line, 2 );
	line[0].x = x;
	line[0].y = y;
	Polyline( hDC, line, 2 );
}



//##------------------------------------------------------------------
// ���̕ҏW�_�C�A���O�̕\��
void DisplayObjectDialog()
{
	int i;
	int number;
	char str[MESSAGE_STR_MAX];
	char strCut[50];
	int id;
	int position;
	int type;
	RECT rect;
	RECT rectBox;

	//�������p�f�[�^�̃X�g�b�N
	if( g_bRestoreObjectDialog == FALSE ){
		g_EditObjectData = g_SelectObjectData;
		StockAttributeData( g_EditObjectData, 0 );
	}
	g_bRestoreObjectDialog = FALSE;
	type = objectAttribute[g_EditObjectData][ATR_TYPE];

	if( g_EditObjectData == 0 ){
		MessageBox( g_hWnd, "�p�[�c�ԍ��O�̕��͕̂ҏW�ł��܂���B\n���̃p�[�c�̓}�b�v�̕��̂���������Ƃ��Ɏw�肵�Ă��������B", "���ӁI", MB_OK );
		return;
	}
	//��ނ̔���
	for( position = 0 ; position < 20 ; ++position ){
		if( OBJ[position].Object == END ) break;
		if( type == OBJ[position].Object ){
			id = OBJ[position].Id;
			break;
		}
	}
	if( id == 20 ) return;

	//�_�C�A���O�̍쐬
	g_hDlgObject = CreateDialog( g_hInst, MAKEINTRESOURCE(id), g_hWnd, (DLGPROC)EditObjectDialogProc );
	//�R���{�{�b�N�X�Ƀf�[�^�}��
	HWND hwndCombo = GetDlgItem( g_hDlgObject, IDC_COMBO_OBJECT );
	for( i = 0 ; i < 13 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)OBJ[i].Name );
	SendMessage( hwndCombo, CB_SETCURSEL, position, 0 );
	//���쑮���R���{�{�b�N�X�Ƀf�[�^�}��
	char *SetName[] = { "�Î~", "�v���[���[�ǔ�", "������", "���낤�낷��" };
	hwndCombo = GetDlgItem( g_hDlgObject, IDC_COMBO_MOVE );
	if( hwndCombo != NULL ){
		for( i = 0 ; i < 4 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)SetName[i] );
	}
	//���̑����R���{�{�b�N�X�Ƀf�[�^�}��
	char *SetName2[] = { "�ʂ蔲���s��", "�ʂ蔲����" };
	hwndCombo = GetDlgItem( g_hDlgObject, IDC_COMBO_MODE );
	if( hwndCombo != NULL ){
		for( i = 0 ; i < 2 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)SetName2[i] );
	}
	//�������R���{�{�b�N�X�Ƀf�[�^�}��
	char *SetName3[] = { "���Ȃ��Ȃ�", "���Ȃ��Ȃ�Ȃ�" };
	hwndCombo = GetDlgItem( g_hDlgObject, IDC_COMBO_MODE2 );
	if( hwndCombo != NULL ){
		for( i = 0 ; i < 2 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)SetName3[i] );
	}
	//�g�p�����R���{�{�b�N�X�Ƀf�[�^�}��
	char *SetName4[] = { "�ʏ�", "�N���b�N�Ŏg�p��", "�g�p���Ă������Ȃ�Ȃ�" };
	hwndCombo = GetDlgItem( g_hDlgObject, IDC_COMBO_ITEM );
	if( hwndCombo != NULL ){
		for( i = 0 ; i < 3 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)SetName4[i] );
	}
	
	//���b�Z�[�W�{�b�N�X�Ƀe�L�X�g�}��
	if( (type == OBJECT_MESSAGE) || (type == OBJECT_MONSTER) || (type == OBJECT_ITEM) || (type == OBJECT_SCORE)
				|| (type == OBJECT_STATUS) || (type == OBJECT_DOOR) || (type == OBJECT_SELL) || (type == OBJECT_BUY) || (type == OBJECT_SELECT) ){
		SetDlgItemText( g_hDlgObject, IDC_EDIT_MESSAGE, g_StrMessage[objectAttribute[g_EditObjectData][ATR_STRING]] );
	} else if( type == OBJECT_URLGATE ){
		strcpy( str, g_StrMessage[objectAttribute[g_EditObjectData][ATR_STRING]] );
		//�����񕪊�
		for( i = 0 ; i < (int)strlen(str) ; ++i ){
			if( (str[i] == 0x0D) && (str[i+1] == 0x0A) ){
				str[i] = '\0';
				strcpy( strCut, str+i+2 );
				SetDlgItemText( g_hDlgObject, IDC_EDIT_TARGET, strCut );
				break;
			}
		}
		SetDlgItemText( g_hDlgObject, IDC_EDIT_URL, str );
	}
	//���l�f�[�^��}��
	for( number = 0 ; number < 100 ; ++number ){
		if( Object[number].Object == END ) break;
		if( Object[number].Object == type ){
			int status;
			status = objectAttribute[g_EditObjectData][Object[number].Atr];
			
			if( ((Object[number].Atr == ATR_ENERGY) && (Object[number].Object == OBJECT_SELL))
						|| ((Object[number].Atr == ATR_ENERGY) && (Object[number].Object == OBJECT_STATUS))
						|| ((Object[number].Atr == ATR_STRENGTH) && (Object[number].Object == OBJECT_STATUS))
						|| ((Object[number].Atr == ATR_DEFENCE) && (Object[number].Object == OBJECT_STATUS))
						|| ((Object[number].Atr == ATR_GOLD) && (Object[number].Object == OBJECT_STATUS)) ){
				if( status > 30000 ) status = 30000 -status;
			}
			//�W�����v�Q�[�g�̏ꍇ
			if( objectAttribute[g_EditObjectData][ATR_TYPE] == OBJECT_LOCALGATE ){
				if( (Object[number].Atr == ATR_JUMP_X) || (Object[number].Atr == ATR_JUMP_Y) ){
					int position = objectAttribute[g_EditObjectData][Object[number].Atr];
					if( position > 9000 ){
						position = position - 10000;
						if( position >= 0 ) sprintf( str, "+%d", position );
						else sprintf( str, "%d", position );
					} else {
						sprintf( str, "%d", position );
					}
					SetDlgItemText( g_hDlgObject, Object[number].Id, str );
				} else {
					_itoa( objectAttribute[g_EditObjectData][Object[number].Atr], str, 10 );
					SetDlgItemText( g_hDlgObject, Object[number].Id, str );
				}
			}
			//�R���{�{�b�N�X�̏ꍇ
			else if( (objectAttribute[g_EditObjectData][ATR_TYPE] != OBJECT_LOCALGATE) && (objectAttribute[g_EditObjectData][ATR_TYPE] != OBJECT_RANDOM) && ((Object[number].Atr == ATR_MOVE) || (Object[number].Atr == ATR_MODE))
						|| ((objectAttribute[g_EditObjectData][ATR_TYPE] == OBJECT_DOOR) && (Object[number].Atr == ATR_NUMBER)) ){
				hwndCombo = GetDlgItem( g_hDlgObject, Object[number].Id );
				SendMessage( hwndCombo, CB_SETCURSEL, status, 0 );
			}
			//�G�f�B�b�g�{�b�N�X�̏ꍇ
			else {
				_itoa( status, str, 10 );
				SetDlgItemText( g_hDlgObject, Object[number].Id, str );
			}
		}
	}
	//�_�C�A���O�ړ�
	GetWindowRect( g_hDlgSelectObject, &rectBox );
	GetWindowRect( g_hDlgObject, &rect );
	MoveWindow( g_hDlgObject, rectBox.right, rectBox.top, rect.right -rect.left, rect.bottom -rect.top, TRUE );
	ShowWindow( g_hDlgObject, SW_SHOW );

	//�_�C�A���O�̍쐬
	if( (type == OBJECT_MESSAGE) || (type == OBJECT_MONSTER) || (type == OBJECT_ITEM)
				|| (type == OBJECT_STATUS) || (type == OBJECT_DOOR) || (type == OBJECT_SELL) || (type == OBJECT_BUY) || (type == OBJECT_SELECT) || (type == OBJECT_LOCALGATE) ){
		if( type == OBJECT_SELECT ) g_hDlgExtra = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EXTRA2), g_hDlgObject, (DLGPROC)DialogProcExtraObject );
		else g_hDlgExtra = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EXTRA), g_hDlgObject, (DLGPROC)DialogProcExtraObject );

		//�_�C�A���O�ړ�
		GetWindowRect( g_hDlgObject, &rectBox );
		GetWindowRect( g_hDlgExtra, &rect );
		MoveWindow( g_hDlgExtra, rectBox.left, rectBox.bottom, rect.right -rect.left, rect.bottom -rect.top, TRUE );
		ShowWindow( g_hDlgExtra, SW_SHOW );

		//�t�H�[�J�X�ړ�
		SetFocus( g_hDlgObject );
		//�g���o���L�����N�^�̐ݒ�
		AppearChara( g_EditObjectData, TRUE );
	}

	//�^�C�g���o�[�e�L�X�g�ݒ�
	char title[100];
	GetWindowText( g_hDlgObject, title, sizeof(title) );
	sprintf( str, "o%d.  %s", g_EditObjectData, title );
	SetWindowText( g_hDlgObject, str );
	GetWindowText( g_hDlgExtra, title, sizeof(title) );
	sprintf( str, "o%d.  %s", g_EditObjectData, title );
	SetWindowText( g_hDlgExtra, str );
}



//##------------------------------------------------------------------
// �w�i�ҏW�_�C�A���O�̕\��

void DisplayMapDialog()
{
	int i;
	int number;
	char str[MESSAGE_STR_MAX];
	char strCut[50];
	int id;
	int position;
	int type;
	RECT rect;
	RECT rectBox;

	//�������p�f�[�^�̃X�g�b�N
	if( g_bRestoreMapDialog == FALSE ){
		g_EditMapData = g_SelectMapData;
		StockAttributeData( g_EditMapData, 1 );
	}
	g_bRestoreMapDialog = FALSE;
	type = mapAttribute[g_EditMapData][ATR_TYPE];
	
	if( g_EditMapData == 0 ){
		MessageBox( g_hWnd, "�p�[�c�ԍ��O�̔w�i�͕ҏW�ł��܂���B\n���̃p�[�c�̓}�b�v�̔w�i����������Ƃ��Ɏw�肵�Ă��������B", "���ӁI", MB_OK );
		return;
	}
	//��ނ̔���
	for( position = 0 ; position < 20 ; ++position ){
		if( MAP[position].Object == END ) break;
		if( type == MAP[position].Object ){
			id = MAP[position].Id;
			break;
		}
	}
	if( id == 20 ) return;

	//�_�C�A���O�̍쐬
	g_hDlgMap = CreateDialog( g_hInst, MAKEINTRESOURCE(id), g_hWnd, (DLGPROC)EditMapDialogProc );

	//�R���{�{�b�N�X�Ƀf�[�^�}��
	HWND hwndCombo = GetDlgItem( g_hDlgMap, IDC_COMBO_MAP );
	for( i = 0 ; i < 4 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)MAP[i].Name );
	SendMessage( hwndCombo, CB_SETCURSEL, position, 0 );

	//���b�Z�[�W�{�b�N�X�Ƀe�L�X�g�}��
	if( (type == MAP_STREET) || (type == MAP_WALL) || (type == MAP_ITEMCHECK) ){
		SetDlgItemText( g_hDlgMap, IDC_EDIT_MESSAGE, g_StrMessage[mapAttribute[g_EditMapData][ATR_STRING]] );
	} else if( type == MAP_URLGATE ){
		strcpy( str, g_StrMessage[mapAttribute[g_EditMapData][ATR_STRING]] );
		//�����񕪊�
		for( i = 0 ; i < (int)strlen(str) ; ++i ){
			if( (str[i] == 0x0D) && (str[i+1] == 0x0A) ){
				str[i] = '\0';
				strcpy( strCut, str+i+2 );
				SetDlgItemText( g_hDlgMap, IDC_EDIT_TARGET, strCut );
				break;
			}
		}
		SetDlgItemText( g_hDlgMap, IDC_EDIT_URL, str );
	}
	//���l�f�[�^��}��
	for( number = 0 ; number < 100 ; ++number ){
		if( Map[number].Object == END ) break;
		if( Map[number].Object == type ){
			if( (Map[number].Atr == ATR_JUMP_X) || (Map[number].Atr == ATR_JUMP_Y) ){
				int position = mapAttribute[g_EditMapData][Map[number].Atr];
				if( position > 9000 ){
					position = position - 10000;
					if( position >= 0 ) sprintf( str, "+%d", position );
					else sprintf( str, "%d", position );
				} else {
					sprintf( str, "%d", position );
				}
				SetDlgItemText( g_hDlgMap, Map[number].Id, str );
			} else {
				_itoa( mapAttribute[g_EditMapData][Map[number].Atr], str, 10 );
				SetDlgItemText( g_hDlgMap, Map[number].Id, str );
			}
		}
	}
	//�_�C�A���O�ړ�
	GetWindowRect( g_hDlgSelectObject, &rectBox );
	GetWindowRect( g_hDlgMap, &rect );
	MoveWindow( g_hDlgMap, rectBox.right, rectBox.top, rect.right -rect.left, rect.bottom -rect.top, TRUE );
	ShowWindow( g_hDlgMap, SW_SHOW );

	//�_�C�A���O�̍쐬
	if( (type == MAP_STREET) || (type == MAP_WALL) || (type == MAP_ITEMCHECK) || (type == MAP_LOCALGATE) ){
		g_hDlgExtra = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EXTRA), g_hDlgMap, (DLGPROC)DialogProcExtraMap );
		//�_�C�A���O�ړ�
		GetWindowRect( g_hDlgMap, &rectBox );
		GetWindowRect( g_hDlgExtra, &rect );
		MoveWindow( g_hDlgExtra, rectBox.left, rectBox.bottom, rect.right -rect.left, rect.bottom -rect.top, TRUE );
		ShowWindow( g_hDlgExtra, SW_SHOW );
		//�t�H�[�J�X�ړ�
		SetFocus( g_hDlgMap );
		//�g���o���L�����N�^�̐ݒ�
		AppearChara( g_EditMapData, FALSE );
	}
	
	//�^�C�g���o�[�e�L�X�g�ݒ�
	char title[100];
	GetWindowText( g_hDlgMap, title, sizeof(title) );
	sprintf( str, "m%d.  %s", g_EditMapData, title );
	SetWindowText( g_hDlgMap, str );
	GetWindowText( g_hDlgExtra, title, sizeof(title) );
	sprintf( str, "m%d.  %s", g_EditMapData, title );
	SetWindowText( g_hDlgExtra, str );
}



//##------------------------------------------------------------------
// ���̃f�[�^�̌���

void SetObjectData( HWND hWnd, int charaNumber )
{
	int i;
	int number;
	int length;
	char str[MESSAGE_STR_MAX];
	char strCut[50];
	int type = objectAttribute[charaNumber][ATR_TYPE];
	int position;

	//�����ݒ肳��Ă��Ȃ���΃L�����Z��
	for( i = 0 ; i < OBJECT_ATR_MAX ; ++i ){
		if( objectAttribute[charaNumber][i] != 0 ) break;
	}
	if( i == OBJECT_ATR_MAX ) return;

	//�p�[�c�ԍ�
	objectAttribute[charaNumber][ATR_0] = charaNumber;

	if( (type == OBJECT_MESSAGE) || (type == OBJECT_MONSTER) || (type == OBJECT_ITEM) || (type == OBJECT_SCORE)
				|| (type == OBJECT_STATUS) || (type == OBJECT_DOOR) || (type == OBJECT_SELL) || (type == OBJECT_BUY) || (type == OBJECT_SELECT) ){
		//�����f�[�^�̃Z�b�g
		GetDlgItemText( hWnd, IDC_EDIT_MESSAGE, str, MESSAGE_STR_MAX );
		SetMessageData( &objectAttribute[charaNumber][ATR_STRING], str );
	} else if( type == OBJECT_URLGATE ){
		GetDlgItemText( hWnd, IDC_EDIT_URL, str, MESSAGE_STR_MAX );
		GetDlgItemText( hWnd, IDC_EDIT_TARGET, strCut, 50 );
		if( (strstr(strCut,"expand") != 0) || (strstr(strCut,"�d�w�o�`�m�c") != 0) ) strcpy( strCut, "EXPAND" );
		if( strlen(strCut) != 0 ){
			length = strlen(str);
			str[length] = 0x0D;
			str[length+1] = 0x0A;
			str[length+2] = '\0';
			strcat( str, strCut );
		}
		SetMessageData( &objectAttribute[charaNumber][ATR_STRING], str );
	}
	//���l�f�[�^��ݒ�
	for( number = 0 ; number < 100 ; ++number ){
		if( Object[number].Object == END ) break;
		if( Object[number].Object == type ){
			GetDlgItemText( hWnd, Object[number].Id, str, sizeof(str) );
			
			//�}�C�i�X���l�̎w��
			if( ((Object[number].Atr == ATR_ENERGY) && (Object[number].Object == OBJECT_SELL))
						|| ((Object[number].Atr == ATR_ENERGY) && (Object[number].Object == OBJECT_STATUS))
						|| ((Object[number].Atr == ATR_STRENGTH) && (Object[number].Object == OBJECT_STATUS))
						|| ((Object[number].Atr == ATR_DEFENCE) && (Object[number].Object == OBJECT_STATUS))
						|| ((Object[number].Atr == ATR_GOLD) && (Object[number].Object == OBJECT_STATUS)) ){
				if( (atoi(str) < 0) && (atoi(str) >= -30000) ){
					objectAttribute[charaNumber][Object[number].Atr] = 30000 - atoi(str);
					continue;
				} else if( atoi(str) < -30000 ){
					sprintf( str, "�u%s�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n -30000 �ȏ���w�肵�Ă��������B", Object[number].Name );
					MessageBox( g_hWnd, str, "�x���I", MB_OK );
				}
			}
			if( (objectAttribute[g_EditObjectData][ATR_TYPE] == OBJECT_SELL) && (Object[number].Atr == ATR_ITEM) ){
				if( (objectAttribute[atoi(str)][ATR_TYPE] != OBJECT_ITEM) && (objectAttribute[atoi(str)][ATR_TYPE] != OBJECT_NORMAL) && (objectAttribute[atoi(str)][ATR_TYPE] != OBJECT_MESSAGE) ){
					MessageBox( g_hWnd, "����A�C�e���ɂ́A�X�e�[�^�X�ω��p�[�c�Ȃǂ̃A�C�e���ȊO�̃p�[�c�͎w��ł��܂���B", "�x���I", MB_OK );
					continue;
				}
			}
			if( (objectAttribute[g_EditObjectData][ATR_TYPE] == OBJECT_LOCALGATE) && ((Object[number].Atr == ATR_JUMP_X) || (Object[number].Atr == ATR_JUMP_Y)) ){
				int position = atoi( str );
				if( position >= g_iMapSize ){
					MessageBox( g_hWnd, "�u���W�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n �}�b�v�T�C�Y�ȉ����w�肵�Ă��������B", "�x���I", MB_OK );
				} else if( (position < -100) || ((position > 100) && (str[0] == '+')) ){
					MessageBox( g_hWnd, "�u���W�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n -100�ȏ�A100�ȉ����w�肵�Ă��������B", "�x���I", MB_OK );
				} else {
					if( (str[0] == '+') || (str[0] == '-') ) position += 10000;
					objectAttribute[charaNumber][Object[number].Atr] = position;
				}
			} else if( (Object[number].Max != 0) && ((Object[number].Max < atoi(str)) || (atoi(str) < 0)) ){
				sprintf( str, "�u%s�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n 0�ȏ� %d �ȉ����w�肵�Ă��������B", Object[number].Name, Object[number].Max );
				MessageBox( g_hWnd, str, "�x���I", MB_OK );
			} else {
				if( (objectAttribute[g_EditObjectData][ATR_TYPE] != OBJECT_LOCALGATE) && (objectAttribute[g_EditObjectData][ATR_TYPE] != OBJECT_RANDOM) && ((Object[number].Atr == ATR_MOVE) || (Object[number].Atr == ATR_MODE))
							|| ((objectAttribute[g_EditObjectData][ATR_TYPE] == OBJECT_DOOR) && (Object[number].Atr == ATR_NUMBER)) ){
					//�R���{�{�b�N�X�̃f�[�^
					position = SendMessage( GetDlgItem(hWnd,Object[number].Id), CB_GETCURSEL, 0, 0 );
					objectAttribute[charaNumber][Object[number].Atr] = position;
				} else {
					//�G�f�B�b�g�{�b�N�X
					objectAttribute[charaNumber][Object[number].Atr] = atoi(str);
				}
			}
		}
	}
}



//##------------------------------------------------------------------
// �w�i�f�[�^�̌���

void SetMapData( HWND hWnd, int charaNumber )
{
	int i;
	int number;
	int length;
	char str[MESSAGE_STR_MAX];
	char strCut[50];
	int type = mapAttribute[charaNumber][ATR_TYPE];

	//�����ݒ肳��Ă��Ȃ���΃L�����Z��
	for( i = 0 ; i < MAP_ATR_MAX ; ++i ){
		if( mapAttribute[charaNumber][i] != 0 ) break;
	}
	if( i == MAP_ATR_MAX ) return;

	//�p�[�c�ԍ�
	mapAttribute[charaNumber][ATR_0] = charaNumber;

	if( (type == MAP_STREET) || (type == MAP_WALL) || (type == MAP_ITEMCHECK) ){
		//�����f�[�^�̃Z�b�g
		GetDlgItemText( hWnd, IDC_EDIT_MESSAGE, str, MESSAGE_STR_MAX );
		SetMessageData( &mapAttribute[charaNumber][ATR_STRING], str );
	} else if( type == MAP_URLGATE ){
		GetDlgItemText( hWnd, IDC_EDIT_URL, str, MESSAGE_STR_MAX );
		GetDlgItemText( hWnd, IDC_EDIT_TARGET, strCut, 50 );
		if( (strstr(strCut,"expand") != 0) || (strstr(strCut,"�d�w�o�`�m�c") != 0) ) strcpy( strCut, "EXPAND" );
		if( strlen(strCut) != 0 ){
			length = strlen(str);
			str[length] = 0x0D;
			str[length+1] = 0x0A;
			str[length+2] = '\0';
			strcat( str, strCut );
		}
		SetMessageData( &mapAttribute[charaNumber][ATR_STRING], str );
	}
	//���l�f�[�^��ݒ�
	for( number = 0 ; number < 100 ; ++number ){
		if( Map[number].Object == END ) break;
		if( Map[number].Object == type ){
			GetDlgItemText( hWnd, Map[number].Id, str, sizeof(str) );
			if( (Map[number].Atr == ATR_JUMP_X) || (Map[number].Atr == ATR_JUMP_Y) ){
				int position = atoi( str );
				if( position >= g_iMapSize ){
					MessageBox( g_hWnd, "�u���W�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n �}�b�v�T�C�Y�ȉ����w�肵�Ă��������B", "�x���I", MB_OK );
				} else if( (position < -100) || ((position > 100) && (str[0] == '+')) ){
					MessageBox( g_hWnd, "�u���W�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n -100�ȏ�A100�ȉ����w�肵�Ă��������B", "�x���I", MB_OK );
				} else {
					if( (str[0] == '+') || (str[0] == '-') ) position += 10000;
					mapAttribute[charaNumber][Map[number].Atr] = position;
				}
			} else {
				if( (Map[number].Max != 0) && ((Map[number].Max < atoi(str)) || (atoi(str) < 0)) ){
					sprintf( str, "�u%s�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n 0�ȏ�A%d�ȉ����w�肵�Ă��������B", Map[number].Name, Map[number].Max );
					MessageBox( g_hWnd, str, "�x���I", MB_OK );
				} else {
					mapAttribute[charaNumber][Map[number].Atr] = atoi( str );
				}
			}
		}
	}
}



//##------------------------------------------------------------------
// �g���o���L�����N�^�̐ݒ�

void AppearChara( int mapNumber, BOOL flag )
{
	int i;
	int dataChara;
	int dataMode;
	int x, y;
	char str[30];

	//�g���L�����N�^�E�f�[�^�R���o�[�g
	for( i = 0 ; i < 10 ; ++i ){
		if( flag == TRUE ){
			dataChara = objectAttribute[mapNumber][20+i*4];
		} else {
			dataChara = mapAttribute[mapNumber][20+i*4];
		}
		
		if( flag == TRUE ){
			dataMode = objectAttribute[mapNumber][20+i*4+3];
		} else {
			dataMode = mapAttribute[mapNumber][20+i*4+3];
		}
		//�{�^�������ݒ�
		if( dataMode == 0 ) SetDlgItemText( g_hDlgExtra, g_EditId[30+i], "��" );
		else SetDlgItemText( g_hDlgExtra, g_EditId[30+i], "�w�i" );
		
		//�w�x���W�n
		if( flag == TRUE ){
			x = objectAttribute[mapNumber][20+i*4+1];
		} else {
			x = mapAttribute[mapNumber][20+i*4+1];
		}
		if( flag == TRUE ){
			y = objectAttribute[mapNumber][20+i*4+2];
		} else {
			y = mapAttribute[mapNumber][20+i*4+2];
		}
		//���ׂĂO�Ȃ�\�����Ȃ�
		if( (x == 0) && (y == 0) && (dataChara == 0) ) continue;
		
		if( x == 9000 ){
			SetDlgItemText( g_hDlgExtra, g_EditId[10+i], "P" );
		} else if( x > 9000 ){
			x -= 10000;
			sprintf( str, "%d", x );
			if( x >= 0 ) sprintf( str, "+%d", x );
			SetDlgItemText( g_hDlgExtra, g_EditId[10+i], str );
		} else {
			sprintf( str, "%d", x );
			SetDlgItemText( g_hDlgExtra, g_EditId[10+i], str );
		}
		
		//y = y >> 8;
		if( y == 9000 ){
			SetDlgItemText( g_hDlgExtra, g_EditId[20+i], "P" );
		} else if( y > 9000 ){
			y -= 10000;
			sprintf( str, "%d", y );
			if( y >= 0 ) sprintf( str, "+%d", y );
			SetDlgItemText( g_hDlgExtra, g_EditId[20+i], str );
		} else {
			sprintf( str, "%d", y );
			SetDlgItemText( g_hDlgExtra, g_EditId[20+i], str );
		}
		
		sprintf( str, "%d", dataChara );
		SetDlgItemText( g_hDlgExtra, g_EditId[i], str );
	}
}



//##------------------------------------------------------------------
// �g���o���L�����N�^�̃f�[�^�L�^

void SetAppearChara( int mapNumber, BOOL flag )
{
	int i;
	int dataChara;
	int x, y;
	char str[30];
	char partsNumberStr[30];

	//�g���L�����N�^�E�f�[�^�R���o�[�g
	for( i = 0 ; i < 10 ; ++i ){
		GetDlgItemText( g_hDlgExtra, g_EditId[i], str, sizeof(str) );
		strcpy( partsNumberStr, str );

		// �v���X�}�C�i�X�L���ɂ��p�[�c�ԍ��C���N�������g�E�f�N�������g
		if (str[0] == '+') {
			dataChara = mapNumber + atoi(&str[1]);
		} else if (str[0] == '-') {
			dataChara = mapNumber - atoi(&str[1]);
		} else {
			dataChara = atoi( str );
		}

		if( (dataChara < 0) || (dataChara >= PARTS_NUMBER_MAX) ){
			MessageBox( g_hWnd, "�o���p�[�c�w��́u�ԍ��v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n 0 �ȏ�A�p�[�c������ȉ����w�肵�Ă��������B", "�x���I", MB_OK );
		} else {
			if( flag == TRUE ){
				objectAttribute[mapNumber][20+i*4] = dataChara;
			} else {
				mapAttribute[mapNumber][20+i*4] = dataChara;
			}
		}
		
		//�w�x���W�n
		GetDlgItemText( g_hDlgExtra, g_EditId[10+i], str, sizeof(str) );
		x = atoi( str );
		if( x >= g_iMapSize ){
			MessageBox( g_hWnd, "�o���p�[�c�w��́u�w���W�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n�}�b�v�T�C�Y�ȉ��Ŏw�肵�Ă��������B", "�x���I", MB_OK );
		} else if( (x < -100) || ((str[0] == '+') && (x > 100)) ){
			MessageBox( g_hWnd, "�o���p�[�c�w��́u���΂w���W�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n -100 ���� +100 �܂łŎw�肵�Ă��������B", "�x���I", MB_OK );
		} else {
			if( (str[0] == '+') || (str[0] == '-') ) x += 10000;
			else if( (str[0] == 'p') || (str[0] == 'P') ) x = 9000;
			else if( (str[0] == '\0') && (partsNumberStr[0] != '\0') ) x = 10000;
			
			if( flag == TRUE ){
				objectAttribute[mapNumber][20+i*4+1] = x;
			} else {
				mapAttribute[mapNumber][20+i*4+1] = x;
			}
		}
		
		GetDlgItemText( g_hDlgExtra, g_EditId[20+i], str, sizeof(str) );
		y = atoi( str );
		if( y >= g_iMapSize ){
			MessageBox( g_hWnd, "�o���p�[�c�w��́u�x���W�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n�}�b�v�T�C�Y�ȉ��Ŏw�肵�Ă��������B", "�x���I", MB_OK );
		} else if( (y < -100) || ((str[0] == '+') && (y > 100)) ){
			MessageBox( g_hWnd, "�o���p�[�c�w��́u���΂x���W�v�̐��l�͈͂��K��l�𒴂��Ă��܂��B\n -100 ���� +100 �܂łŎw�肵�Ă��������B", "�x���I", MB_OK );
		} else {
			if( (str[0] == '+') || (str[0] == '-') ) y += 10000;
			else if( (str[0] == 'p') || (str[0] == 'P') ) y = 9000;
			else if( (str[0] == '\0') && (partsNumberStr[0] != '\0') ) y = 10000;
			
			//y = y << 8;
			if( flag == TRUE ){
				objectAttribute[mapNumber][20+i*4+2] = y;
			} else {
				mapAttribute[mapNumber][20+i*4+2] = y;
			}
		}
	}
}



//##------------------------------------------------------------------
// �_�C�A���O�v���V�[�W��

LRESULT CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_COMMAND: {
		if (LOWORD(wParam) == IDOK) {
			if (hWnd == g_hDlgFoundation) {
				int number;
				char str[30];
				//���l�f�[�^�ݒ�
				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_ENERGYMAX, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number > 10000)) MessageBox(g_hWnd, "�u�����͏���v�̐��l�͈͂��K��l�ł͂���܂���B\n 10000 �ȉ����w�肵�Ă��������B", "�x���I", MB_OK);
				else statusEnergyMax = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIENERGY, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number > 10000)) MessageBox(g_hWnd, "�u�����́v�̐��l�͈͂��K��l�ł͂���܂���B\n 10000 �ȉ����w�肵�Ă��������B", "�x���I", MB_OK);
				else statusEnergy = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INISTRENGTH, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number > 10000)) MessageBox(g_hWnd, "�u�U���́v�̐��l�͈͂��K��l�ł͂���܂���B\n 10000 �ȉ����w�肵�Ă��������B", "�x���I", MB_OK);
				else statusStrength = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIDEFENCE, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number > 10000)) MessageBox(g_hWnd, "�u�h��́v�̐��l�͈͂��K��l�ł͂���܂���B\n 10000 �ȉ����w�肵�Ă��������B", "�x���I", MB_OK);
				else statusDefence = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIGOLD, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number > 10000)) MessageBox(g_hWnd, "�u�������v�̐��l�͈͂��K��l�ł͂���܂���B\n 10000 �ȉ����w�肵�Ă��������B", "�x���I", MB_OK);
				else statusGold = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIX, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number >= g_iMapSize)) MessageBox(g_hWnd, "�u�����w���W�v�̐��l�͈͂��K��l�ł͂���܂���B\n �}�b�v�T�C�Y�ȉ����w�肵�Ă��������B", "�x���I", MB_OK);
				else charaX = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIY, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number >= g_iMapSize)) MessageBox(g_hWnd, "�u�����x���W�v�̐��l�͈͂��K��l�ł͂���܂���B\n �}�b�v�T�C�Y�ȉ����w�肵�Ă��������B", "�x���I", MB_OK);
				else charaY = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_GVX, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number >= g_iMapSize)) MessageBox(g_hWnd, "�u�Q�[���I�[�o�[�w���W�v�̐��l�͈͂��K��l�ł͂���܂���B\n �}�b�v�T�C�Y�ȉ����w�肵�Ă��������B", "�x���I", MB_OK);
				else gameoverXp = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_GVY, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number >= g_iMapSize)) MessageBox(g_hWnd, "�u�Q�[���I�[�o�[�x���W�v�̐��l�͈͂��K��l�ł͂���܂���B\n �}�b�v�T�C�Y�ȉ����w�肵�Ă��������B", "�x���I", MB_OK);
				else gameoverYp = number;

				//�e�L�X�g�f�[�^�}��
				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_WORLDNAME, g_worldName, sizeof(g_worldName));
				//�Ïؔԍ�
				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_PASSWORD, g_worldPassword, sizeof(g_worldPassword));
				if ((atol(g_worldPassword) <= 0) && (g_worldPassword[0] != '\0')) {
					MessageBox(hWnd, "�Ïؔԍ��ɂ͂P�ȏ�̐�������͂��Ă��������B", "�x���I", MB_OK);
					g_worldPassword[0] = '\0';
				}
				else if (atol(g_worldPassword) >= 1000000) {
					MessageBox(hWnd, "�Ïؔԍ��͂U���܂łɂ��Ă��������B", "�x���I", MB_OK);
					g_worldPassword[0] = '\0';
				}
				else if (g_worldPassword[0] == '0') {
					MessageBox(hWnd, "�Ïؔԍ��͐����Ƃ��Ĉ����܂��̂ŁA\n�擪�ɂO�͎g���܂���B", "�x���I", MB_OK);
					g_worldPassword[0] = '\0';
				}
				else if (atol(g_worldPassword) == 9999) {
					MessageBox(hWnd, "�Ïؔԍ��͂X�X�X�X�ȊO�ɂ��Ă��������B", "�x���I", MB_OK);
					g_worldPassword[0] = '\0';
				}
				else if ((atol(g_worldPassword) < 1000) && (atol(g_worldPassword) > 0)) {
					MessageBox(hWnd, "�g���N���X���g�p���Ȃ��ꍇ��A�b�f�h �ɂ��X�e�[�^�X���p���������Ȃ�Ȃ��ꍇ�́A\n�ێ�̂��߈Ïؔԍ��͂Ȃ�ׂ��S���ȏ�ɂ��Ă������Ƃ������߂��܂��B\n�g���N���X����̋N����X�e�[�^�X���p�����ł��Ȃ��悤�ɂȂ�܂��B", "����", MB_OK);
				}
				if (g_worldPassword[0] != '\0') ltoa(atol(g_worldPassword), g_worldPassword, 10);

				GetDlgItemText(g_hDlgFoundation, IDC_COMBO_BMP, g_mapcgNameBmp, sizeof(g_mapcgNameBmp));
				GetDlgItemText(g_hDlgFoundation, IDC_COMBO_GIF, g_mapcgName, sizeof(g_mapcgName));
				if (g_mapcgName[0] == '\0') {
					MessageBox(hWnd, "GIF�摜�t�@�C��������͂��Ă��܂���B", "�x���I", MB_OK);
					return 1;
				}
				DestroyWindow(hWnd);

				//�r�b�g�}�b�v�ǂݍ���
				if (g_bLoadGif == TRUE) {
					if (strcmp(g_mapcgName, g_mapcgOld) != 0) LoadBitmap();
				}
				else {
					if (strcmp(g_mapcgNameBmp, g_mapcgOld) != 0) LoadBitmap();
				}
				InvalidateRect(g_hWnd, NULL, FALSE);
				g_bUpdate = TRUE;
			}
			//�퓬���ʂ̌v�Z
			else if (hWnd == g_hDlgCalculate) {
				char str[50];
				int strength, defence, energy;
				int strengthM, defenceM, energyM;
				int damagy = 0;
				int damagyM = 0;
				int turn = 0;
				int attack, attackM;

				GetDlgItemText(hWnd, IDC_EDIT_ENERGY, str, sizeof(str));
				energy = atoi(str);
				GetDlgItemText(hWnd, IDC_EDIT_ATTACK, str, sizeof(str));
				strength = atoi(str);
				GetDlgItemText(hWnd, IDC_EDIT_DEFENCE, str, sizeof(str));
				defence = atoi(str);
				GetDlgItemText(hWnd, IDC_EDIT_ENERGY2, str, sizeof(str));
				energyM = atoi(str);
				GetDlgItemText(hWnd, IDC_EDIT_ATTACK2, str, sizeof(str));
				strengthM = atoi(str);
				GetDlgItemText(hWnd, IDC_EDIT_DEFENCE2, str, sizeof(str));
				defenceM = atoi(str);

				attack = (strength - defenceM);
				attackM = (strengthM - defence);
				if (attack < 0) attack = 0;
				if (attackM < 0) attackM = 0;

				if ((attack != 0) || (attackM != 0)) {
					while (TRUE) {
						++turn;
						energyM -= attack;
						damagyM += attack;
						if (energyM <= 0) break;
						energy -= attackM;
						damagy += attackM;
						if (energy <= 0) break;
						if (turn > 10000) {
							turn = 0;
							break;
						}
					}
				}
				if (turn == 0) sprintf(str, "�U���^�[�����@--");
				else sprintf(str, "�U���^�[�����@%d", turn);
				SetDlgItemText(hWnd, IDC_STATIC_TURN, str);
				sprintf(str, "�v���[���[�@%d", damagy);
				SetDlgItemText(hWnd, IDC_STATIC_RESULT1, str);
				sprintf(str, "�����X�^�[�@%d", damagyM);
				SetDlgItemText(hWnd, IDC_STATIC_RESULT2, str);
				sprintf(str, "�v���[���[�@%d", attack);
				SetDlgItemText(hWnd, IDC_STATIC_RESULT3, str);
				sprintf(str, "�����X�^�[�@%d", attackM);
				SetDlgItemText(hWnd, IDC_STATIC_RESULT4, str);
			}
			return 1;
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			DestroyWindow(hWnd);
			return 1;
		}
		// �}�b�v�T�C�Y�g��
		else if (LOWORD(wParam) == IDC_BUTTON_MAPSIZE) {
			char szStr[50];
			g_iMapSize += 50;
			if (g_iMapSize > MAP_SIZE_MAX) g_iMapSize = MAP_SIZE_MAX;
			sprintf(szStr, "%d�~%d", g_iMapSize, g_iMapSize);
			SetDlgItemText(g_hDlgFoundation, IDC_EDIT_MAPSIZE, szStr);
			// �X�N���[���o�[�ݒ�
			SetScrollRange(g_hWnd, SB_VERT, 0, (g_iMapSize - SCREEN_CHIP_SIZE), FALSE);
			SetScrollRange(g_hWnd, SB_HORZ, 0, (g_iMapSize - SCREEN_CHIP_SIZE), FALSE);
		}
		// �w�i�p�[�c�ő吔�g��
		else if (LOWORD(wParam) == IDC_BUTTON_MAP_PARTS) {
			g_iMapPartsMax += 50;
			if (g_iMapPartsMax > PARTS_NUMBER_MAX) g_iMapPartsMax = PARTS_NUMBER_MAX;
			SetScrollRange(g_hDlgSelectMap, SB_VERT, 0, ((g_iMapPartsMax / 10) - 3), FALSE);
			char szStr[50];
			sprintf(szStr, "%d", g_iMapPartsMax);
			SetDlgItemText(g_hDlgFoundation, IDC_EDIT_MAP_PARTS, szStr);
		}
		// ���̃p�[�c�ő吔�g��
		else if (LOWORD(wParam) == IDC_BUTTON_OBJ_PARTS) {
			g_iObjectPartsMax += 50;
			if (g_iObjectPartsMax > PARTS_NUMBER_MAX) g_iObjectPartsMax = PARTS_NUMBER_MAX;
			SetScrollRange(g_hDlgSelectObject, SB_VERT, 0, ((g_iObjectPartsMax / 10) - 3), FALSE);
			char szStr[50];
			sprintf(szStr, "%d", g_iObjectPartsMax);
			SetDlgItemText(g_hDlgFoundation, IDC_EDIT_OBJ_PARTS, szStr);
		}
		break;
	}
	}
	return 0;
}



//##------------------------------------------------------------------
// ��{���b�Z�[�W�_�C�A���O�v���V�[�W��
LRESULT CALLBACK DialogProcBasicMes(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG: {
		SetDlgItemText(hWnd, IDC_EDIT_BMES_LINK, g_StrMessage[5]);
		SetDlgItemText(hWnd, IDC_EDIT_BMES_GOLD, g_StrMessage[6]);
		SetDlgItemText(hWnd, IDC_EDIT_BMES_ITEM, g_StrMessage[7]);
		SetDlgItemText(hWnd, IDC_EDIT_BMES_USEITEM, g_StrMessage[8]);
		//�g���̈�
		SetDlgItemText(hWnd, IDC_EDIT_BMES_CLICKBOX, g_StrMessageSystem[0]);
		SetDlgItemText(hWnd, IDC_EDIT_BMES_ITEMFULL, g_StrMessageSystem[1]);
		SetDlgItemText(hWnd, IDC_EDIT_BMES_SOUND, g_StrMessageSystem[2]);
		break;
	}
	case WM_COMMAND: {
		if (LOWORD(wParam) == IDOK) {
			GetDlgItemText(hWnd, IDC_EDIT_BMES_LINK, g_StrMessage[5], MESSAGE_STR_MAX);
			GetDlgItemText(hWnd, IDC_EDIT_BMES_GOLD, g_StrMessage[6], MESSAGE_STR_MAX);
			GetDlgItemText(hWnd, IDC_EDIT_BMES_ITEM, g_StrMessage[7], MESSAGE_STR_MAX);
			GetDlgItemText(hWnd, IDC_EDIT_BMES_USEITEM, g_StrMessage[8], MESSAGE_STR_MAX);
			//�g���̈�
			GetDlgItemText(hWnd, IDC_EDIT_BMES_CLICKBOX, g_StrMessageSystem[0], MESSAGE_STR_MAX);
			GetDlgItemText(hWnd, IDC_EDIT_BMES_ITEMFULL, g_StrMessageSystem[1], MESSAGE_STR_MAX);
			GetDlgItemText(hWnd, IDC_EDIT_BMES_SOUND, g_StrMessageSystem[2], MESSAGE_STR_MAX);
			//�w�蕶����C��
			int i;
			for (i = 5; i <= 9; ++i) {
				if ((strstr(g_StrMessage[i], "blank") != 0) || (strstr(g_StrMessage[i], "�a�k�`�m�j") != 0) || (strstr(g_StrMessage[i], "BLANK") != 0)) strcpy(g_StrMessage[i], "BLANK");
			}
			DestroyWindow(hWnd);
			return 1;
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			DestroyWindow(hWnd);
			return 1;
		}
	}
	}
	return 0;
}



//##------------------------------------------------------------------
// �p�X���[�h�_�C�A���O�v���V�[�W��
LRESULT CALLBACK DialogProcPassword(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG: {
		SetFocus(GetDlgItem(hWnd, IDC_EDIT_PASSWORD));
		break;
	}
	case WM_COMMAND: {
		if (LOWORD(wParam) == IDOK) {
			//�p�X���[�h����
			char str[30];
			GetDlgItemText(hWnd, IDC_EDIT_PASSWORD, str, sizeof(str));
			GetDlgItemText(hWnd, IDC_EDIT_PASSWORD, g_szInputPassword, sizeof(g_szInputPassword));

			if (strcmp(g_worldPassword, str) == 0) {
				g_bErrorPassword = FALSE;
				EndDialog(hWnd, 0);
			}
			else {
				g_bErrorPassword = TRUE;
				EndDialog(hWnd, 0);
				ZeroMemory(&map, sizeof(map));
				ZeroMemory(&mapObject, sizeof(mapObject));
				ZeroMemory(&mapAttribute, sizeof(mapAttribute));
				ZeroMemory(&objectAttribute, sizeof(objectAttribute));
				g_worldName[0] = '\0';
				ZeroMemory(&g_worldPassword, sizeof(g_worldPassword));
				g_mapcgNameBmp[0] = '\0';
				g_mapcgName[0] = '\0';
				g_mapcgOld[0] = '\0';
			}
			return 1;
		}
	}
	}
	return 0;
}



//##------------------------------------------------------------------
// �g���o���L�����N�^�_�C�A���O�v���V�[�W��
LRESULT CALLBACK DialogProcExtraObject(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_COMMAND: {
		int Id[] = { IDC_BUTTON_MODE, IDC_BUTTON_MODE2, IDC_BUTTON_MODE3, IDC_BUTTON_MODE4, IDC_BUTTON_MODE5, IDC_BUTTON_MODE6, IDC_BUTTON_MODE7, IDC_BUTTON_MODE8, IDC_BUTTON_MODE9, IDC_BUTTON_MODE10 };
		int i;
		int dataMode;
		int chara = GetCharaNumber(hWnd);
		for (i = 0; i < 10; ++i) {
			if (LOWORD(wParam) == Id[i]) {
				//���́A�w�i��ؑ�
				dataMode = objectAttribute[chara][20 + i * 4 + 3];
				if (dataMode == 0) {
					SetDlgItemText(hWnd, Id[i], "�w�i");
					objectAttribute[chara][20 + i * 4 + 3] = 1;
				}
				else {
					SetDlgItemText(hWnd, Id[i], "��");
					objectAttribute[chara][20 + i * 4 + 3] = 0;
				}
			}
		}
		if (wParam == IDOK) {
			DestroyWindow(g_hDlgObject);
		}
		break;
	}
	case WM_DESTROY: {
		if (g_bCancel == FALSE) {
			//�g���o���L�����N�^�̐ݒ�
			SetAppearChara(GetCharaNumber(hWnd), TRUE);
			return 1;
		}
		break;
	}
	}
	return 0;
}

LRESULT CALLBACK DialogProcExtraMap(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_COMMAND: {
		int Id[] = { IDC_BUTTON_MODE, IDC_BUTTON_MODE2, IDC_BUTTON_MODE3, IDC_BUTTON_MODE4, IDC_BUTTON_MODE5, IDC_BUTTON_MODE6, IDC_BUTTON_MODE7, IDC_BUTTON_MODE8, IDC_BUTTON_MODE9, IDC_BUTTON_MODE10 };
		int i;
		int dataMode;
		int chara = GetCharaNumber(hWnd);
		for (i = 0; i < 10; ++i) {
			if (LOWORD(wParam) == Id[i]) {
				//���́A�w�i��ؑ�
				dataMode = mapAttribute[chara][20 + i * 4 + 3];
				if (dataMode == 0) {
					SetDlgItemText(hWnd, Id[i], "�w�i");
					mapAttribute[chara][20 + i * 4 + 3] = 1;
				}
				else {
					SetDlgItemText(hWnd, Id[i], "��");
					mapAttribute[chara][20 + i * 4 + 3] = 0;
				}
			}
		}
		if (wParam == IDOK) {
			DestroyWindow(g_hDlgMap);
		}
		break;
	}
	case WM_DESTROY: {
		if (g_bCancel == FALSE) {
			//�g���o���L�����N�^�̐ݒ�
			SetAppearChara(GetCharaNumber(hWnd), FALSE);
			return 1;
		}
		break;
	}
	}
	return 0;
}



//##------------------------------------------------------------------
// �����f�[�^�̃Z�b�g
void SetMessageData(int* point, char* str)
{
	int i;
	int number = 0;

	//�ő啶�����̔���
	if (strlen(str) >= (MESSAGE_STR_MAX - 1 - 10)) {
		MessageBox(g_hWnd, "���b�Z�[�W���ő啶�����𒴂��Ă��܂��B\n1500�o�C�g�܂łœ��͂��Ă��������B", "����", MB_OK);
		str[MESSAGE_STR_MAX - 1 - 10] = '\0';
	}

	//���b�Z�[�W�i���o�[�ݒ�
	if ((strlen(str) > 0) && (*point == 0)) {
		for (number = 10; number < MESSAGE_NUMBER_MAX; ++number) {
			for (i = 0; i < PARTS_NUMBER_MAX; ++i) {
				if ((objectAttribute[i][ATR_STRING] == number) || (mapAttribute[i][ATR_STRING] == number)) {
					break;
				}
			}
			if (i == PARTS_NUMBER_MAX) break;
		}
		if (number == MESSAGE_NUMBER_MAX) {
			MessageBox(g_hWnd, "���b�Z�[�W���ő吔�𒴂��Ă��܂��B\n�d�l�ケ��ȏ�̃��b�Z�[�W�͍쐬�ł��܂���B", "����", MB_OK);
			return;
		}
		*point = number;
		//���b�Z�[�W�i�[
		strcpy(g_StrMessage[*point], str);
	}
	else if (strlen(str) == 0) {
		g_StrMessage[*point][0] = '\0';
		*point = 0;
	}
	else {
		//���b�Z�[�W�i�[
		strcpy(g_StrMessage[*point], str);
	}
}


//##------------------------------------------------------------------
// �}�b�v�̐V�K�쐬
void MakeNewMap()
{
	int i;

	//�V�K�쐬�m�F
	int result;
	result = MessageBox(g_hWnd, "�}�b�v�̐V�K�쐬�����܂��B\n���݁A�ҏW���̃f�[�^�͎����܂����A��낵���ł����H", "�}�b�v�̐V�K�쐬", MB_OK | MB_OKCANCEL);
	//�f�[�^������
	if (result == IDCANCEL) return;

	//�_�C�A���O����
	DestroyWindow(g_hDlgObject);
	DestroyWindow(g_hDlgMap);
	DestroyWindow(g_hDlgSelectChara);

	charaX = 0;
	charaY = 0;
	statusEnergyMax = 0;
	statusEnergy = 0;
	statusStrength = 0;
	statusDefence = 0;
	statusGold = 0;
	gameoverXp = 0;
	gameoverYp = 0;
	for (i = 0; i < 12; ++i) itemBox[i] = 0;

	ZeroMemory(&map, sizeof(map));
	ZeroMemory(&mapObject, sizeof(mapObject));

	//������
	ZeroMemory(&mapAttribute, sizeof(mapAttribute));
	ZeroMemory(&objectAttribute, sizeof(objectAttribute));
	//���b�Z�[�W�f�[�^
	for (i = 0; i < MESSAGE_NUMBER_MAX; ++i) g_StrMessage[i][0] = '\0';

	//���̑��f�[�^
	g_worldName[0] = '\0';
	g_worldPassword[0] = '\0';
	g_mapcgNameBmp[0] = '\0';
	g_mapcgName[0] = '\0';
	g_mapcgOld[0] = '\0';
	strcpy(g_szSelectFile, "newmap.dat");
	//�^�C�g���e�L�X�g�ݒ�
	char sTitle[100];
	sprintf(sTitle, "%s [%s]", g_szTitleName, g_szSelectFile);
	SetWindowText(g_hWnd, sTitle);

	//��{�ݒ�̕ҏW
	EditMapFoundation();

	//��ʍĕ`��
	InvalidateRect(g_hWnd, NULL, FALSE);
	InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
	InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
	InvalidateRect(g_hDlgQuickView, NULL, FALSE);

	MessageBox(g_hWnd, "�}�b�v��V�K�쐬���܂����B\n��ʂɂ͉����\������Ȃ��Ȃ�܂�������Ő���ł��B\n�g�p����f�h�e�摜�t�@�C����I����A\n�V���Ƀp�[�c���쐬���ă}�b�v�ɔz�u���Ă����Ă��������B", "�}�b�v�̐V�K�쐬", MB_OK);
}



//##------------------------------------------------------------------
// ��{�ݒ�_�C�A���O
void EditMapFoundation()
{
	char str[100];
	DestroyWindow(g_hDlgFoundation);
	g_hDlgFoundation = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_FOUNDATION), g_hWnd, (DLGPROC)DialogProc);

	//���l�f�[�^�}��
	_itoa(statusEnergyMax, str, 10);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_ENERGYMAX, str);
	_itoa(statusEnergy, str, 10);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIENERGY, str);
	_itoa(statusStrength, str, 10);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_INISTRENGTH, str);
	_itoa(statusDefence, str, 10);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIDEFENCE, str);
	_itoa(statusGold, str, 10);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIGOLD, str);
	_itoa(charaX, str, 10);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIX, str);
	_itoa(charaY, str, 10);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIY, str);
	_itoa(gameoverXp, str, 10);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_GVX, str);
	_itoa(gameoverYp, str, 10);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_GVY, str);

	//�e�L�X�g�f�[�^�}��
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_WORLDNAME, g_worldName);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_PASSWORD, g_worldPassword);

	//�R���{�{�b�N�X�Ƀf�[�^�}��
	SetDlgItemText(g_hDlgFoundation, IDC_COMBO_BMP, g_mapcgNameBmp);
	SetDlgItemText(g_hDlgFoundation, IDC_COMBO_BMP, g_mapcgNameBmp);
	if (g_bLoadGif == TRUE) {
		ShowWindow(GetDlgItem(g_hDlgFoundation, IDC_COMBO_BMP), FALSE);
		ShowWindow(GetDlgItem(g_hDlgFoundation, IDC_STATIC_BMP), FALSE);
	}
	SetDlgItemText(g_hDlgFoundation, IDC_COMBO_GIF, g_mapcgName);
	SetDlgItemText(g_hDlgFoundation, IDC_COMBO_GIF, g_mapcgName);

	//�e��T�C�Y�f�[�^�}��
	sprintf(str, "%d�~%d", g_iMapSize, g_iMapSize);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_MAPSIZE, str);
	sprintf(str, "%d", g_iMapPartsMax);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_MAP_PARTS, str);
	sprintf(str, "%d", g_iObjectPartsMax);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_OBJ_PARTS, str);
}



//##------------------------------------------------------------------
// �������p�A�f�[�^�̈ꎞ�ۑ�

void StockAttributeData(int number, int mode)
{
	int i;

	if (mode == 0) {
		for (i = 0; i < OBJECT_ATR_MAX; ++i) StockObjectAttribute[number][i] = objectAttribute[number][i];
	}
	else {
		for (i = 0; i < MAP_ATR_MAX; ++i) StockMapAttribute[number][i] = mapAttribute[number][i];
	}
}



//##------------------------------------------------------------------
// �v�Z�_�C�A���O�̕\��

void CalculateDialog()
{
	DestroyWindow(g_hDlgCalculate);
	g_hDlgCalculate = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_CALCULATE), g_hWnd, (DLGPROC)DialogProc);
	SetDlgItemText(g_hDlgCalculate, IDC_EDIT_ENERGY, "1000");
	SetDlgItemText(g_hDlgCalculate, IDC_EDIT_ATTACK, "0");
	SetDlgItemText(g_hDlgCalculate, IDC_EDIT_DEFENCE, "0");
	SetDlgItemText(g_hDlgCalculate, IDC_EDIT_ENERGY2, "0");
	SetDlgItemText(g_hDlgCalculate, IDC_EDIT_ATTACK2, "0");
	SetDlgItemText(g_hDlgCalculate, IDC_EDIT_DEFENCE2, "0");
}



//##------------------------------------------------------------------
// �L�����N�^�ԍ��̎擾

int GetCharaNumber(HWND hWnd)
{
	int i;
	char strNumber[10];
	char str[50];

	GetWindowText(hWnd, str, sizeof(str));

	for (i = 0; i < 4; ++i) {
		if (str[i + 1] == '.') break;
		strNumber[i] = str[i + 1];
	}
	strNumber[i] = '\0';

	return atoi(strNumber);
}


//##------------------------------------------------------------------
// �o�͂���HTML�t�@�C�������擾

CString GetHtmlFileName()
{
	CString MapDataFileName(g_szSelectFile);
	if (MapDataFileName.Find(".dat") == -1) {
		return MapDataFileName + ".html";
	}
	MapDataFileName.Replace(".dat", ".html");
	return MapDataFileName;
}


//##------------------------------------------------------------------
// �t�@�C���̍쐬�ƃu���E�U�̋N��

BOOL ExecBrowser()
{
	// HTML�쐬
	CString MapDataFileName = g_szSelectFile;
	MapDataFileName.Replace("<", "&lt;");
	MapDataFileName.Replace(">", "&gt;");

	CString MapWorldName;
	// ���[���h���̍쐬
	{
		WCHAR buff[200];
		char Utf8Str[100];
		int length;

		length = MultiByteToWideChar(CP_ACP, 0, g_worldName, sizeof(g_worldName), buff, sizeof(buff) / 2);
		buff[length] = '\0';
		WideCharToMultiByte(CP_UTF8, 0, buff, length + 1, Utf8Str, 100, CP_ACP, NULL);

		MapWorldName = Utf8Str;
		MapWorldName.Replace("<", "&lt;");
		MapWorldName.Replace(">", "&gt;");
	}

	CString Html =
		"<!DOCTYPE HTML>\n"
		"<html lang=\"ja\">\n"
		"<head>\n"
		"  <meta charset=\"UTF-8\">\n"
		"  <link rel=\"stylesheet\" href=\"wwa.css\">\n"
		"  <link rel=\"stylesheet\" href=\"style.css\">\n"
		"  <script src=\"wwa.js\"></script>\n"
		"  <script src=\"audio/audio.min.js\"></script>\n"
		"  <title>" + MapWorldName + "</title>\n"
		"</head>\n"
		"<body>\n"
		"  <div id=\"wrapper\">\n"
		"    <div\n"
		"      id=\"wwa-wrapper\"\n"
		"      class=\"wwa-size-box\"\n"
		"      data-wwa-mapdata=\"" + MapDataFileName + "\"\n"
		"      data-wwa-loader=\"wwaload.js\"\n"
		"      data-wwa-urlgate-enable=\"true\"\n"
		"      data-wwa-title-img=\"cover.gif\"\n"
		"    ></div>\n"
		"  </div>\n"
		"  <footer id=\"copyright\">\n"
		"    <p>Internet RPG &quot;<a href=\"http://www.wwajp.com\">World Wide Adventure</a>&quot; &copy;1996-2017 NAO</p>\n"
		"    <p>&quot;<a href=\"http://wwawing.com/\">WWA Wing</a>&quot; &copy;2013-2019 WWA Wing Team</p>\n"
		"  </footer>\n"
		"</body>\n"
		"</html>\n";

	// �t�@�C�����쐬
	CString HtmlFileName;
	HtmlFileName = GetHtmlFileName();

	// �f�[�^�̏�������
	HANDLE hFile;
	DWORD dwWritten;

	hFile = CreateFile(HtmlFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		CString ErrorMessage = "�u" + HtmlFileName + "�v�t�@�C�����쐬�܂��͏������݂ł��܂���B";
		MessageBox(g_hWnd, ErrorMessage, "����", MB_OK);

		CloseHandle(hFile);
		return FALSE;
	}

	WriteFile(hFile, Html, Html.GetLength(), &dwWritten, NULL);
	CloseHandle(hFile);

	// �u���E�U�̋N��
	CString Message = "�u" + HtmlFileName + "�v�t�@�C�����o�͂��܂����B\nHTML�t�@�C�����u���E�U�Ō��܂����H\n(�f�o�b�O�c�[�����K�v�ɂȂ�ꍇ������܂�)";
	if (MessageBox(g_hWnd, Message, "�쐬����", MB_YESNO) == IDYES) {
		if ((int)ShellExecute(g_hWnd, "open", HtmlFileName, NULL, NULL, SW_SHOWNORMAL) <= 32) {
			MessageBox(g_hWnd, "�u���E�U�N���G���[\n�u���E�U�\�t�g�E�F�A���C���X�g�[������Ă��邩�m�F���Ă��������B", "�N�����s", MB_OK);
			return FALSE;
		}
	}
	return TRUE;
}



//##------------------------------------------------------------------
// �w��L�[��������Ă����ꍇ��TRUE��Ԃ�

BOOL IsKeyDown( int virtKeyCode )
{
	if ( GetAsyncKeyState(virtKeyCode) & 0x8000 ) return TRUE;
	return FALSE;
}



//##------------------------------------------------------------------
// Undo�@�\

void SetUndoData()
{
	int i, j;

	for (i = 0; i < g_iMapSize; ++i) {
		for (j = 0; j < g_iMapSize; ++j) {
			UndoMap[i][j] = map[i][j];
			UndoMapObject[i][j] = mapObject[i][j];
		}
	}
	for (i = 0; i < g_iMapPartsMax; ++i) {
		for (j = 0; j < MAP_ATR_MAX; ++j) {
			UndoMapAttribute[i][j] = mapAttribute[i][j];
		}
	}
	for (i = 0; i < g_iObjectPartsMax; ++i) {
		for (j = 0; j < OBJECT_ATR_MAX; ++j) {
			UndoObjectAttribute[i][j] = objectAttribute[i][j];
		}
	}
}


void RestoreUndoData()
{
	int i, j;

	for (i = 0; i < g_iMapSize; ++i) {
		for (j = 0; j < g_iMapSize; ++j) {
			map[i][j] = UndoMap[i][j];
			mapObject[i][j] = UndoMapObject[i][j];
		}
	}
	for (i = 0; i < g_iMapPartsMax; ++i) {
		for (j = 0; j < MAP_ATR_MAX; ++j) {
			mapAttribute[i][j] = UndoMapAttribute[i][j];
		}
	}
	for (i = 0; i < g_iObjectPartsMax; ++i) {
		for (j = 0; j < OBJECT_ATR_MAX; ++j) {
			objectAttribute[i][j] = UndoObjectAttribute[i][j];
		}
	}
}



//##------------------------------------------------------------------
// GIF�摜�̓ǂݍ���

BOOL ReadGifImage()
{
	IStream* ist;
	IPicture* ipi;
	HGLOBAL hgb;
	HANDLE han;
	DWORD siz, dw;
	OLE_XSIZE_HIMETRIC cx = 0;
	OLE_YSIZE_HIMETRIC cy = 0;
	int cxPerInch;
	int cyPerInch;
	long dx, dy;
	char szStr[300];

	//������
	g_iColorTp = 0;
	g_bFileNotFound = FALSE;

	//�摜�t�@�C���ǂݍ���
	if ((han = CreateFile(g_mapcgName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		sprintf(szStr, "GIF�摜�t�@�C���u%s�v���I�[�v���ł��܂���B\n�t�@�C�������݂��邩�A���̃A�v���P�[�V�����ɂ��g�p����Ă��Ȃ������m�F���Ă��������B", g_mapcgName);
		MessageBox(g_hWnd, szStr, "����", MB_OK);
		g_bFileNotFound = TRUE;
		return FALSE;
	}
	siz = GetFileSize(han, NULL);
	hgb = GlobalAlloc(GPTR, siz);
	ReadFile(han, hgb, siz, &dw, NULL);
	CloseHandle(han);
	//�X�g���[�����쐬
	CreateStreamOnHGlobal(hgb, TRUE, &ist);
	//�C���[�W�����[�h
	OleLoadPicture(ist, siz, TRUE, IID_IPicture, (LPVOID*)& ipi);

	ipi->get_Width(&cx);
	ipi->get_Height(&cy);
	if ((cx == 0) || (cy == 0)) return FALSE;

	cxPerInch = GetDeviceCaps(g_hmDC, LOGPIXELSX);
	cyPerInch = GetDeviceCaps(g_hmDC, LOGPIXELSY);
	dx = MulDiv(cx, cxPerInch, HIMETRIC_INCH);
	dy = MulDiv(cy, cyPerInch, HIMETRIC_INCH);

	//�摜��ݒ�
	g_iLoadCGHeight = dy;
	if ((g_iLoadCGHeight / CHIP_SIZE) > 17) {
		g_ScrCGCharaMax = (g_iLoadCGHeight / CHIP_SIZE) - 17;
	}
	else {
		g_ScrCGCharaMax = 0;
	}
	// �N���C�A���g�̈�m��
	HDC hDC = GetDC(g_hWnd);
	if (g_hBitmap != NULL) DeleteObject(g_hBitmap);
	g_hBitmap = CreateCompatibleBitmap(hDC, CHIP_SIZE * 10, g_iLoadCGHeight);
	SelectObject(g_hmDC, g_hBitmap);

	// ���ߐF�œh��ׂ�
	g_iColorTp = 0x9E9E9E;
	HBRUSH hbr = CreateSolidBrush(g_iColorTp);
	RECT rcRect;
	rcRect.left = 0;
	rcRect.top = 0;
	rcRect.right = CHIP_SIZE * 10;
	rcRect.bottom = g_iLoadCGHeight;
	FillRect(g_hmDC, &rcRect, hbr);
	// �C���[�W�`��
	ipi->Render(g_hmDC, 0, 0, dx, dy, 0, cy, cx, -cy, NULL);

	// �f�o�C�X���
	ReleaseDC(g_hWnd, hDC);
	ipi->Release();
	ist->Release();

	return TRUE;
}
