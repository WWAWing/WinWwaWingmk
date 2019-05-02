#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <olectl.h>
#include "resource.h"
#include "cdib.hpp"
#include <atlstr.h>

//##------------------------------------------------------------------
// Visual Style 対応
#pragma comment(linker, \
	"\"/manifestdependency:type='Win32' "\
	"name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' "\
	"processorArchitecture='*' "\
	"publicKeyToken='6595b64144ccf1df' "\
	"language='*'\"")

//##------------------------------------------------------------------
// 定数定義

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
#define FILE_DATA_MAX		(4000 +1000 +1000) *1024	//（マップ＋パーツ＋メッセージ領域）
#define MESSAGE_NUMBER_MAX	5000

#define MESSAGE_STR_MAX		1500+10
#define MAP_ATR_MAX			60
#define OBJECT_ATR_MAX		60

// 1画面の1辺のチップサイズ
#define SCREEN_CHIP_SIZE	21
#define DEFALUT_SCREEN_CHIP_SIZE 11
// 1チップのサイズ (ピクセル単位)
#define CHIP_SIZE			40

// パーツ選択ダイアログで表示するパーツ行数 (リソースファイルの変更も忘れずに)
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

//キャラクタ位置
int charaX, charaY;

//メッセージ用バッファ
char g_StrMessage[MESSAGE_NUMBER_MAX][MESSAGE_STR_MAX];
char g_StrMessageSystem[20][MESSAGE_STR_MAX];
char g_worldName[100];
char g_worldPassword[100];
char g_mapcgName[100];
char g_mapcgNameBmp[100];
char g_mapcgOld[100];

//マップ位置
int mapX, mapY;
int mapXtop = 0;
int mapYtop = 0;

//ステータス
int statusEnergyMax;
int statusEnergy;
int statusStrength;
int statusDefence;
int statusGold;
int itemStrength;
int itemDefence;
int gameoverXp;
int gameoverYp;

//マップデータ
short map[MAP_SIZE_MAX][MAP_SIZE_MAX];
short mapObject[MAP_SIZE_MAX][MAP_SIZE_MAX];
int objectAttribute[PARTS_NUMBER_MAX][OBJECT_ATR_MAX];
int mapAttribute[PARTS_NUMBER_MAX][MAP_ATR_MAX];
int pointer;
int itemBox[12];
int StockObjectAttribute[PARTS_NUMBER_MAX][OBJECT_ATR_MAX];
int StockMapAttribute[PARTS_NUMBER_MAX][MAP_ATR_MAX];
//Undo用
short UndoMap[MAP_SIZE_MAX][MAP_SIZE_MAX];
short UndoMapObject[MAP_SIZE_MAX][MAP_SIZE_MAX];
int UndoObjectAttribute[PARTS_NUMBER_MAX][OBJECT_ATR_MAX];
int UndoMapAttribute[PARTS_NUMBER_MAX][MAP_ATR_MAX];

unsigned char *UndoMapB;

//複写用
int g_CopyObject[OBJECT_ATR_MAX];
int g_CopyMap[MAP_ATR_MAX];
char g_CopyObjectStr[MESSAGE_STR_MAX];
char g_CopyMapStr[MESSAGE_STR_MAX];
short g_MapBuffer[SCREEN_CHIP_SIZE][SCREEN_CHIP_SIZE];
short g_ObjectBuffer[SCREEN_CHIP_SIZE][SCREEN_CHIP_SIZE];

//物体選択用
int g_SelectObjectX, g_SelectObjectY;
int g_ScrObject = 0;
int g_SelectObjectData;
int g_EditObjectData;
//背景選択用
int g_SelectMapX, g_SelectMapY;
int g_ScrMap = 0;
int g_SelectMapData;
int g_EditMapData;
//キャラクタＣＧ選択用
int g_ScrCGChara = 0;
int g_ScrCGCharaMax = 10;
int g_hModeSelectChara;
int g_iLoadCGHeight = 800;

//汎用
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

int g_AtrSelectChara;	//ＣＧ選択用
int g_EditMode;

//フラグ
BOOL g_bRestoreObjectDialog = FALSE;
BOOL g_bRestoreMapDialog = FALSE;
BOOL g_MapLineFlag = TRUE;		//境界線
BOOL g_ObjectNumberFlag = TRUE;	//物体パーツ番号表示
BOOL g_MouseDrag = FALSE;		//マウスのドラッグ判定用
BOOL g_bLoadGif = TRUE;			//GIFファイルが読み込めるか？
BOOL g_bUpdate = FALSE;			//更新確認フラグ
BOOL g_bInitial = FALSE;		//初期化済みか？

BOOL g_bFileNotFound;
BOOL g_iColorTp;

BOOL g_hugeMapSize = FALSE; // MapSizeが21 * 21か？

char g_MapData[FILE_DATA_MAX];
char PressData[FILE_DATA_MAX];

char g_szSettingFile[250] = "./WinWwamk.ini";	//設定ファイル名
char g_szSelectFile[250] = "wwamap.dat";		//ファイル名
char g_szTitleName[] = "WWA Wingマップ作成ツール Ver3.1.8";
char g_szSelectDir[250];
int g_MouseX, g_MouseY;
int g_MouseDragX, g_MouseDragY;
int g_MouseOldX, g_MouseOldY;
BOOL g_bCancel = FALSE;

//各種上限
int g_iMapSize = 501;
int g_iMapPartsMax = 200;
int g_iObjectPartsMax = 200;
int g_iMapAtrMax;
int g_iObjectAtrMax;
int g_iMesNumberMax;

//暗証番号用
BOOL g_bErrorPassword = FALSE;
char g_szInputPassword[30];


//物体データ用
struct StructOBJ {
	char *Name;
	int Object;
	int Id;
};
StructOBJ OBJ[] = {
	{ "通常物体", OBJECT_NORMAL, IDD_DIALOG_NORMAL },
	{ "メッセージ", OBJECT_MESSAGE, IDD_DIALOG_MESSAGE },
	{ "モンスター", OBJECT_MONSTER, IDD_DIALOG_MONSTER },
	{ "アイテム", OBJECT_ITEM, IDD_DIALOG_ITEM },
	{ "扉", OBJECT_DOOR, IDD_DIALOG_DOOR },
	{ "ステータス変化", OBJECT_STATUS, IDD_DIALOG_STATUS },
	{ "物を売る", OBJECT_SELL, IDD_DIALOG_SELL },
	{ "物を買う", OBJECT_BUY, IDD_DIALOG_BUY },
	{ "ＵＲＬゲート", OBJECT_URLGATE, IDD_DIALOG_URLOBJ },
	{ "スコア表示", OBJECT_SCORE, IDD_DIALOG_SCORE },
	{ "ランダム選択", OBJECT_RANDOM, IDD_DIALOG_RANDOM },
	{ "二者択一", OBJECT_SELECT, IDD_DIALOG_OBJSELECT },
	{ "ジャンプゲート", OBJECT_LOCALGATE, IDD_DIALOG_LOCALGATE_OBJ },
	{ "", END }
};
char *g_ObjectName[] = { "通常物体　　","メッセージ　　","ＵＲＬｹﾞｰﾄ　","ｽﾃｰﾀｽ変化　","アイテム　　","扉　　　　　","モンスター　","","","","","スコア表示　","","","物を売る　　","物を買う　　","ﾗﾝﾀﾞﾑ選択　　","二者択一　　","ｼﾞｬﾝﾌﾟｹﾞｰﾄ　" };
char *g_MapName[] = { "道　　　　　","壁　　　　　","ｼﾞｬﾝﾌﾟｹﾞｰﾄ　","","ＵＲＬｹﾞｰﾄ　","ｱｲﾃﾑ分岐　　" };

StructOBJ MAP[] = {
	{ "道", MAP_STREET, IDD_DIALOG_STREET },
	{ "壁", MAP_WALL, IDD_DIALOG_WALL },
	{ "ジャンプゲート", MAP_LOCALGATE, IDD_DIALOG_LOCALGATE },
	{ "ＵＲＬゲート", MAP_URLGATE, IDD_DIALOG_URLMAP },
	{ "アイテム分岐", MAP_ITEMCHECK, IDD_DIALOG_ITEMCHECK },
	{ "", END }
};

//物体編集ダイアログの内容
struct StructObject {
	int Object;
	int Atr;
	int Id;
	int Max;
	char *Name;
};
StructObject Object[] = {
	{ OBJECT_NORMAL, ATR_MODE, IDC_COMBO_MODE, 1, "物体の種類" },
	{ OBJECT_NORMAL, ATR_MOVE, IDC_COMBO_MOVE, 3, "動作属性" },

	{ OBJECT_MESSAGE, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_MESSAGE, ATR_MOVE, IDC_COMBO_MOVE, 3, "動作属性" },
	{ OBJECT_MESSAGE, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "反応するアイテム番号" },
	{ OBJECT_MESSAGE, ATR_NUMBER, IDC_EDIT_WAIT, 1000, "待ち時間" },

	{ OBJECT_SELL, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "アイテムの物体番号" },
	{ OBJECT_SELL, ATR_GOLD, IDC_EDIT_GOLD, 30000, "販売金額" },
	{ OBJECT_SELL, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_SELL, ATR_ENERGY, IDC_EDIT_ENERGY, 30000, "生命力" },
	{ OBJECT_SELL, ATR_STRENGTH, IDC_EDIT_STRENGTH, 30000, "攻撃力" },
	{ OBJECT_SELL, ATR_DEFENCE, IDC_EDIT_DEFENCE, 30000, "防御力" },
	{ OBJECT_SELL, ATR_MOVE, IDC_COMBO_MOVE, 3, "動作属性" },

	{ OBJECT_BUY, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "アイテムの物体番号" },
	{ OBJECT_BUY, ATR_GOLD, IDC_EDIT_GOLD, 30000, "金額" },
	{ OBJECT_BUY, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_BUY, ATR_MOVE, IDC_COMBO_MOVE, 3, "動作属性" },

	{ OBJECT_ITEM, ATR_NUMBER, IDC_EDIT_NUMBER, 12, "ボックスへの格納位置" },
	{ OBJECT_ITEM, ATR_STRENGTH, IDC_EDIT_STRENGTH, 30000, "攻撃力" },
	{ OBJECT_ITEM, ATR_DEFENCE, IDC_EDIT_DEFENCE, 30000, "防御力" },
	{ OBJECT_ITEM, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_ITEM, ATR_MOVE, IDC_COMBO_MOVE, 3, "動作属性" },
	{ OBJECT_ITEM, ATR_MODE, IDC_COMBO_ITEM, 3, "使用属性" },

	{ OBJECT_DOOR, ATR_MODE, IDC_COMBO_MODE2, 3, "属性" },
	{ OBJECT_DOOR, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "アイテムの物体番号" },
	{ OBJECT_DOOR, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_DOOR, ATR_MOVE, IDC_COMBO_MOVE, 3, "動作属性" },
	{ OBJECT_DOOR, ATR_NUMBER, IDC_COMBO_MODE, 1, "通行区分" },

	{ OBJECT_STATUS, ATR_ENERGY, IDC_EDIT_ENERGY, 30000, "生命力" },
	{ OBJECT_STATUS, ATR_STRENGTH, IDC_EDIT_STRENGTH, 30000, "攻撃力" },
	{ OBJECT_STATUS, ATR_DEFENCE, IDC_EDIT_DEFENCE, 30000, "防御力" },
	{ OBJECT_STATUS, ATR_GOLD, IDC_EDIT_GOLD, 30000, "所持金" },
	{ OBJECT_STATUS, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_STATUS, ATR_MOVE, IDC_COMBO_MOVE, 3, "動作属性" },

	{ OBJECT_SCORE, ATR_ENERGY, IDC_EDIT_ENERGY, 30000, "生命力" },
	{ OBJECT_SCORE, ATR_STRENGTH, IDC_EDIT_STRENGTH, 30000, "攻撃力" },
	{ OBJECT_SCORE, ATR_DEFENCE, IDC_EDIT_DEFENCE, 30000, "防御力" },
	{ OBJECT_SCORE, ATR_GOLD, IDC_EDIT_GOLD, 30000, "所持金" },
	{ OBJECT_SCORE, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },

	{ OBJECT_MONSTER, ATR_ENERGY, IDC_EDIT_ENERGY, 60000, "生命力" },
	{ OBJECT_MONSTER, ATR_STRENGTH, IDC_EDIT_STRENGTH, 60000, "攻撃力" },
	{ OBJECT_MONSTER, ATR_DEFENCE, IDC_EDIT_DEFENCE, 60000, "防御力" },
	{ OBJECT_MONSTER, ATR_GOLD, IDC_EDIT_GOLD, 30000, "所持金" },
	{ OBJECT_MONSTER, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "アイテムの物体番号" },
	{ OBJECT_MONSTER, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_MONSTER, ATR_MOVE, IDC_COMBO_MOVE, 3, "動作属性" },

	{ OBJECT_RANDOM, 10, IDC_EDIT_RANDOM1, PARTS_NUMBER_MAX-1, "物体番号" },
	{ OBJECT_RANDOM, 11, IDC_EDIT_RANDOM2, PARTS_NUMBER_MAX-1, "物体番号" },
	{ OBJECT_RANDOM, 12, IDC_EDIT_RANDOM3, PARTS_NUMBER_MAX-1, "物体番号" },
	{ OBJECT_RANDOM, 13, IDC_EDIT_RANDOM4, PARTS_NUMBER_MAX-1, "物体番号" },
	{ OBJECT_RANDOM, 14, IDC_EDIT_RANDOM5, PARTS_NUMBER_MAX-1, "物体番号" },
	{ OBJECT_RANDOM, 15, IDC_EDIT_RANDOM6, PARTS_NUMBER_MAX-1, "物体番号" },
	{ OBJECT_RANDOM, 16, IDC_EDIT_RANDOM7, PARTS_NUMBER_MAX-1, "物体番号" },
	{ OBJECT_RANDOM, 17, IDC_EDIT_RANDOM8, PARTS_NUMBER_MAX-1, "物体番号" },
	{ OBJECT_RANDOM, 18, IDC_EDIT_RANDOM9, PARTS_NUMBER_MAX-1, "物体番号" },
	{ OBJECT_RANDOM, 19, IDC_EDIT_RANDOM10, PARTS_NUMBER_MAX-1, "物体番号" },

	{ OBJECT_SELECT, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ OBJECT_SELECT, ATR_MOVE, IDC_COMBO_MOVE, 3, "動作属性" },

	{ OBJECT_LOCALGATE, ATR_JUMP_X, IDC_EDIT_X, 100, "Ｘ座標" },
	{ OBJECT_LOCALGATE, ATR_JUMP_Y, IDC_EDIT_Y, 100, "Ｙ座標" },
	{ OBJECT_LOCALGATE, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ END }
};

StructObject Map[] = {
	{ MAP_STREET, ATR_NUMBER, IDC_EDIT_WAIT, 1000, "待ち時間" },
	{ MAP_STREET, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },
	{ MAP_STREET, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "反応するアイテム番号" },

	{ MAP_LOCALGATE, ATR_JUMP_X, IDC_EDIT_X, 100, "Ｘ座標" },
	{ MAP_LOCALGATE, ATR_JUMP_Y, IDC_EDIT_Y, 100, "Ｙ座標" },
	{ MAP_LOCALGATE, ATR_SOUND, IDC_EDIT_SOUND, 0, "" },

	{ MAP_ITEMCHECK, ATR_ITEM, IDC_EDIT_ITEM, PARTS_NUMBER_MAX-1, "判定するアイテム番号" },
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
// 関数群

//BOOL LibLoad();
// メニューのチェックをはずす
void DeleteCheckMenu();
// マップデータのロード
BOOL LoadMapData( char *FileName );
// マップデータの保存
BOOL SaveMapData( char *FileName );
void saveMapString( char *str );
// ビットマップ読み込み
BOOL LoadBitmap();
void PaintWindow();
void paintMapAll( HDC hDC );
// ステータス描画
void PaintStatus( BOOL flag );

// 選択ダイアログプロシージャ
LRESULT CALLBACK SelectObjectDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK SelectMapDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
// 編集ダイアログプロシージャ
LRESULT CALLBACK EditObjectDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK EditMapDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
// クイックビューダイアログプロシージャ
LRESULT CALLBACK QuickViewDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
// キャラクタＣＧ選択ダイアログプロシージャ
LRESULT CALLBACK SelectCGCharaProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
// 基本メッセージダイアログプロシージャ
LRESULT CALLBACK DialogProcBasicMes( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

// 線と四角の描画
void DrawLine( HDC hDC, int x, int y, int x2, int y2 );
void DrawRect( HDC hDC, int x, int y, int x2, int y2 );
void DrawRect2( HDC hDC, int x, int y, int x2, int y2 );

// 編集ダイアログの表示
void DisplayObjectDialog();
void DisplayMapDialog();
// 物体データの決定
void SetObjectData( HWND hWnd, int charaNumber );
void SetMapData( HWND hWnd, int charaNumber );
// 拡張出現キャラクタの設定
void AppearChara( int mapNumber, BOOL flag );
// 拡張出現キャラクタのデータ記録
void SetAppearChara( int mapNumber, BOOL flag );
// ダイアログプロシージャ
LRESULT CALLBACK DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
// 文字データのセット
void SetMessageData( int  *point, char *str );

// マップの新規作成
void MakeNewMap();
// 基本設定ダイアログ
void EditMapFoundation();
// パスワードダイアログプロシージャ
LRESULT CALLBACK DialogProcPassword( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

// 取り消し用、データの一時保存
void StockAttributeData( int number, int mode );
// 計算ダイアログの表示
void CalculateDialog();
// キャラクタ番号の取得
int GetCharaNumber( HWND hWnd );

// 拡張出現キャラクタダイアログプロシージャ
LRESULT CALLBACK DialogProcExtraObject( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK DialogProcExtraMap( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

// 拡張子を取り除いたファイル名を取得する
CString GetHtmlFileName();
// ブラウザを起動する
BOOL ExecBrowser();

// 指定キーが押されていた場合はTRUEを返す
BOOL IsKeyDown( int virtKeyCode );

// Undo機能
void SetUndoData();
void RestoreUndoData();

//メモリ取得
// GIF画像の読み込み
BOOL ReadGifImage();


//##------------------------------------------------------------------
// イベント処理

LRESULT WINAPI MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	int x, y, x2, y2;
	int result;

	switch (message) {
		//カーソルキー入力
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
					 //最小化
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
				  //右ボタンクリック
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
				//Undoセット
				SetUndoData();
				DisplayObjectDialog();
				//編集モード変更
				DeleteCheckMenu();
				CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTOBJ, MF_CHECKED);
				g_EditMode = 1;
			}
			else {
				DestroyWindow(g_hDlgObject);
				DestroyWindow(g_hDlgMap);
				//Undoセット
				SetUndoData();
				DisplayMapDialog();
				//編集モード変更
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

		//マウス移動
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
		//指定範囲内をキャラクタで埋める。
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
			//Undoセット
			SetUndoData();
			//ドラッグ開始位置セット
			g_MouseDragX = x;
			g_MouseDragY = y;
			if ((g_EditMode == 0) || (g_EditMode == 1) || (g_EditMode == 4)) g_MouseDrag = TRUE;
		}
		//ステータスバークリック
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
		//マップ画面クリック
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
			//Undoセット
			SetUndoData();
			DisplayMapDialog();
			InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
		}
		else if (g_EditMode == 3) {
			g_SelectObjectData = mapObject[y / CHIP_SIZE + mapYtop][x / CHIP_SIZE + mapXtop];
			if (g_SelectObjectData != 0) {
				DestroyWindow(g_hDlgObject);
				DestroyWindow(g_hDlgMap);
				//Undoセット
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

		// 横方向
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
		// 縦方向
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
		// カーソルキー操作
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
			// エディットボックスのフォーカス移動
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

		// データのロード、セーブ
		if (LOWORD(wParam) == ID_MENU_LOAD) {
			int i;
			char FileName[250] = "*.dat";
			char CurrentDir[250];
			OPENFILENAME ofn;

			GetCurrentDirectory(sizeof(CurrentDir), CurrentDir);
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrTitle = "マップデータの読み込み";
			ofn.lpstrFile = FileName;
			ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
			ofn.lpstrInitialDir = CurrentDir;
			ofn.lpstrFilter = "Map Data (*.dat)\0\0";
			ofn.nMaxFile = sizeof(FileName);
			ofn.hwndOwner = g_hWnd;

			if (GetOpenFileName(&ofn)) {
				//ディレクトリ検出
				for (i = strlen(FileName); i > 0; --i) if (FileName[i] == '\\') break;
				strcpy(g_szSelectDir, FileName);
				g_szSelectDir[i + 1] = '\0';
				strcpy(g_szSelectFile, (FileName + i + 1));
				//データ読み込み
				LoadMapData(g_szSelectFile);
				if (g_bErrorPassword == TRUE) MessageBox(g_hWnd, "暗証番号が違います。", "警告！", MB_OK);
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
			ofn.lpstrTitle = "マップデータの保存";
			ofn.lpstrFile = FileName;
			ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
			ofn.lpstrInitialDir = CurrentDir;
			ofn.lpstrFilter = "Map Data (*.dat)\0\0";
			ofn.nMaxFile = sizeof(FileName);
			ofn.hwndOwner = g_hWnd;

			if (GetSaveFileName(&ofn)) {
				//ディレクトリ検出
				for (i = strlen(FileName); i > 0; --i) if (FileName[i] == '\\') break;
				strcpy(g_szSelectDir, FileName);
				g_szSelectDir[i + 1] = '\0';
				strcpy(g_szSelectFile, (FileName + i + 1));
				//データ保存
				SaveMapData(g_szSelectFile);
			}
		}
		// 基本設定ダイアログ
		else if (LOWORD(wParam) == ID_MENU_FOUNDATION) {
			EditMapFoundation();
		}
		// 基本メッセージダイアログ
		else if (LOWORD(wParam) == ID_MENU_BASICMES) {
			DestroyWindow(g_hDlgBasicMes);
			g_hDlgBasicMes = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_BASICMES), g_hWnd, (DLGPROC)DialogProcBasicMes);
		}
		// 戦闘結果の計算
		else if (LOWORD(wParam) == ID_MENU_BATTLE) {
			CalculateDialog();
		}
		// 編集モード選択切替
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
		// 上書き保存
		else if (LOWORD(wParam) == ID_MENU_SAVEA) {
			SaveMapData(g_szSelectFile);
		}
		//境界線
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
		// マップの新規作成
		else if (LOWORD(wParam) == ID_MENU_NEW) {
			MakeNewMap();
		}
		// ブラウザオープン
		else if (LOWORD(wParam) == ID_MENU_EXEC) {
			if (g_bUpdate == TRUE) {
				MessageBox(g_hWnd, "現在の状態のマップを見る場合は、\n一度ファイルに保存しておいてください。", "注意！", MB_OK);
			}
			ExecBrowser();
		}
		// クイックビューウィンドウの表示
		else if (LOWORD(wParam) == ID_MENU_QVIEW) {
			ShowWindow(g_hDlgQuickView, TRUE);
			EnableMenuItem(GetMenu(g_hWnd), ID_MENU_QVIEW, MF_GRAYED);
		}
		// 物体選択ウィンドウの表示
		else if (LOWORD(wParam) == ID_MENU_OBJWINDOW) {
			ShowWindow(g_hDlgSelectObject, TRUE);
			EnableMenuItem(GetMenu(g_hWnd), ID_MENU_OBJWINDOW, MF_GRAYED);
		}
		// 背景選択ウィンドウの表示
		else if (LOWORD(wParam) == ID_MENU_MAPWINDOW) {
			ShowWindow(g_hDlgSelectMap, TRUE);
			EnableMenuItem(GetMenu(g_hWnd), ID_MENU_MAPWINDOW, MF_GRAYED);
		}
		// マップウィンドウの拡大縮小
		else if (LOWORD(wParam) == ID_MENU_CHANGEWINDOWSIZE) {
			int SizeX, SizeY;
			int setScreenChipSize = g_hugeMapSize ? DEFALUT_SCREEN_CHIP_SIZE : SCREEN_CHIP_SIZE;
			setScreenChipSize++;
			g_hugeMapSize = !g_hugeMapSize;
			SizeX = (setScreenChipSize * CHIP_SIZE);
			SizeY = ((setScreenChipSize + 1) * CHIP_SIZE);
			SetWindowPos(g_hWnd, NULL, 0, 0, SizeX, SizeY, SWP_NOMOVE | SWP_NOZORDER);
		}
		// パーツのコピー
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
			// Undoセット
			SetUndoData();
			DestroyWindow(g_hDlgObject);
			DestroyWindow(g_hDlgMap);
			DestroyWindow(g_hDlgSelectChara);
			g_bUpdate = TRUE;
			if (g_EditMode == 0) {
				// 背景貼り付け
				for (i = 0; i < MAP_ATR_MAX; ++i) mapAttribute[g_SelectMapData][i] = g_CopyMap[i];
				SetMessageData(&mapAttribute[g_SelectMapData][ATR_STRING], g_CopyMapStr);
				InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
			}
			else if (g_EditMode == 1) {
				// 物体貼り付け
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
		// マップ画面ごとコピー
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
			// Undoセット
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
		// ツールの終了
		else if (LOWORD(wParam) == ID_MENU_END) {
			if (g_bUpdate == TRUE) {
				result = MessageBox(g_hWnd, "マップデータが未保存ですが、終了してよろしいですか？", "注意！", MB_OK | MB_OKCANCEL);
				if (result == IDCANCEL) break;
			}
			DestroyWindow(g_hWnd);
		}
		break;

	case WM_CLOSE:
		if (g_bUpdate == TRUE) {
			result = MessageBox(g_hWnd, "マップデータが未保存ですが、終了してよろしいですか？", "注意！", MB_OK | MB_OKCANCEL);
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
// メニューのチェックをはずす
void DeleteCheckMenu()
{
	CheckMenuItem( GetMenu(g_hWnd), ID_MENU_PUTMAP, MF_UNCHECKED );
	CheckMenuItem( GetMenu(g_hWnd), ID_MENU_PUTOBJ, MF_UNCHECKED );
	CheckMenuItem( GetMenu(g_hWnd), ID_MENU_EDITMAP, MF_UNCHECKED );
	CheckMenuItem( GetMenu(g_hWnd), ID_MENU_EDITOBJ, MF_UNCHECKED );
	CheckMenuItem( GetMenu(g_hWnd), ID_MENU_DELOBJ, MF_UNCHECKED );
}



//##------------------------------------------------------------------
// ＷＷＡ作成ツール
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

	// 設定ファイル読み込み
	PositionX = GetPrivateProfileInt("Main", "PositionX", CW_USEDEFAULT, g_szSettingFile);
	PositionY = GetPrivateProfileInt("Main", "PositionY", 0, g_szSettingFile);
	int window_size = SCREEN_CHIP_SIZE * CHIP_SIZE;

	// メインウィンドウ作成
	g_hWnd = CreateWindow("WWAMK", "WWA Wingマップ作成ツール",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_HSCROLL | WS_VSCROLL,
		PositionX, PositionY,
		CW_USEDEFAULT , CW_USEDEFAULT,
		NULL, NULL, hInst, NULL);

	// サイズ変更
	GetWindowRect(g_hWnd, &WindowRect);
	GetClientRect(g_hWnd, &ClientRect);
	SizeX = (WindowRect.right - WindowRect.left) - (ClientRect.right - ClientRect.left) + (DEFALUT_SCREEN_CHIP_SIZE * CHIP_SIZE);
	SizeY = (WindowRect.bottom - WindowRect.top) - (ClientRect.bottom - ClientRect.top) + (DEFALUT_SCREEN_CHIP_SIZE * CHIP_SIZE) + 20;
	SetWindowPos(g_hWnd, NULL, 0, 0, SizeX, SizeY, SWP_NOMOVE | SWP_NOZORDER);

	if (g_hWnd != NULL) {
		ShowWindow(g_hWnd, CmdShow);
		UpdateWindow(g_hWnd);
	}

	// 初期ファイル名設定
	unsigned int i, j;
	if (strlen(pszCmdLine) > 0) {
		// ダブルクォーテーションは削除
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
		// ファイル名抽出
		for (i = strlen(pszCmdLine); i > 0; --i) if (pszCmdLine[i] == '\\') break;
		strcpy(g_szSelectFile, (pszCmdLine + i + 1));
		// ディレクトリ移動
		strcpy(g_szSelectDir, pszCmdLine);
		g_szSelectDir[i + 1] = '\0';
		SetCurrentDirectory(g_szSelectDir);
	}
	else {
		SetCurrentDirectory("mapdata");
	}
	// マップデータ読み込み
	LoadMapData(g_szSelectFile);
	if (g_bErrorPassword == TRUE) MessageBox(g_hWnd, "暗証番号が違います。", "警告！", MB_OK);
	// ビットマップ読み込み
	LoadBitmap();

	// ダイアログ表示
	g_hDlgSelectObject = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EDITOBJECT), g_hWnd, (DLGPROC)SelectObjectDialogProc);
	g_hDlgSelectMap = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EDITMAP), g_hWnd, (DLGPROC)SelectMapDialogProc);
	// ダイアログ移動
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

	// クイックビュー
	g_hDlgQuickView = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_QVIEW), g_hWnd, (DLGPROC)QuickViewDialogProc);
	GetWindowRect(g_hWnd, &rectBox);
	GetWindowRect(g_hDlgQuickView, &rect);
	MoveWindow(g_hDlgQuickView, rectBox.left, rectBox.bottom, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	ShowWindow(g_hDlgQuickView, SW_SHOW);

	// 画面色数取得
	HDC hDC = GetDC(g_hWnd);
	if (GetDeviceCaps(hDC, BITSPIXEL) == 8) {
		MessageBox(g_hWnd, "このツールは、256色環境では画像がうまく表示されません。\nコントロールパネルの画面の設定で、16ビットカラー以上を選択してください。", "警告！", MB_OK);
	}
	ReleaseDC(g_hWnd, hDC);

	// フォーカス移動
	SetFocus(g_hWnd);
	// アクセラレーターキー
	HACCEL hAccel = LoadAccelerators(g_hInst, MAKEINTRESOURCE(IDR_MENU1));

	// 初期化完了
	g_bInitial = TRUE;

	if (g_hWnd != NULL) {
		while (GetMessage(&msg, NULL, 0, 0)) {
			// ダイアログメッセージ
			if (IsDialogMessage(g_hDlgBasicMes, &msg)) continue;
			// テキストボックス以外ならショートカットキー許可
			if ((GetDlgCtrlID(GetFocus()) == IDC_EDIT_MESSAGE) || (GetDlgCtrlID(GetFocus()) == IDC_EDIT_URL)) {
				if (IsDialogMessage(g_hDlgMap, &msg)) continue;
				else if (IsDialogMessage(g_hDlgObject, &msg)) continue;
			}
			if (!TranslateAccelerator(g_hWnd, hAccel, &msg)) {
				// ダイアログメッセージ
				if (IsDialogMessage(g_hDlgSelectMap, &msg)) continue;
				else if (IsDialogMessage(g_hDlgSelectObject, &msg)) continue;
				else if (IsDialogMessage(g_hDlgQuickView, &msg)) continue;
				else if (IsDialogMessage(g_hDlgExtra, &msg)) continue;
				else if (IsDialogMessage(g_hDlgCalculate, &msg)) continue;
				else if (IsDialogMessage(g_hDlgFoundation, &msg)) continue;
				else if (IsDialogMessage(g_hDlgObject, &msg)) continue;
				else if (IsDialogMessage(g_hDlgMap, &msg)) continue;

				// メッセージを送る
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return(msg.wParam);
}



//##------------------------------------------------------------------
// ビットマップ読み込み

BOOL LoadBitmap()
{
	g_pDib = new CDib;
	HDC hDC = GetDC(g_hWnd);

	// デバイスコンテキストの初期化
	if (g_hmDC == NULL) {
		g_hmDC = CreateCompatibleDC(hDC);
		g_hmDCHalf = CreateCompatibleDC(hDC);
		g_hmDCAnd = CreateCompatibleDC(hDC);
		g_hmDCOr = CreateCompatibleDC(hDC);

		g_hmDCExtra = CreateCompatibleDC(hDC);
		g_hBitmapExtra = CreateCompatibleBitmap(hDC, CHIP_SIZE * SCREEN_CHIP_SIZE, 20);
		SelectObject(g_hmDCExtra, g_hBitmapExtra);
	}
	// 現在のビットマップ名記録
	if (g_bLoadGif == TRUE) strcpy(g_mapcgOld, g_mapcgName);
	else strcpy(g_mapcgOld, g_mapcgNameBmp);

	// 読み出し中ダイアログ表示
	Rectangle(hDC, 0, 0, CHIP_SIZE * SCREEN_CHIP_SIZE, CHIP_SIZE * SCREEN_CHIP_SIZE);
	TextOut(hDC, 80, 200, "画像データ読み込み中・・・・・", 30);

	// 画像メモリにGIF描画
	g_bLoadGif = ReadGifImage();
	if (g_bFileNotFound == TRUE) return FALSE;
	// GIF読み込み失敗時
	if (g_bLoadGif == FALSE) {
		strcpy(g_mapcgOld, g_mapcgNameBmp);
		if (g_pDib->ReadFile(g_mapcgNameBmp) == FALSE) {
			MessageBox(g_hWnd, "このマップデータに対応する画像ファイル（256色BMPファイル）が読み込めません。\n「編集－基本設定の編集」で256色BMPファイルを指定してください。\n\nこのシステムでは、GIFファイルは直接読み込めないので、\n編集用に256色BMPファイルが必要になります。", "BMPファイル読み込み失敗", MB_OK);
			// デバイス解放
			ReleaseDC(g_hWnd, hDC);
			return FALSE;
		}
		// 画像丈設定
		g_iLoadCGHeight = g_pDib->GetCDibBiHeight();
		if ((g_iLoadCGHeight / CHIP_SIZE) > 17) {
			g_ScrCGCharaMax = (g_iLoadCGHeight / CHIP_SIZE) - 17;
		}
		else {
			g_ScrCGCharaMax = 0;
		}
		// クライアント領域確保
		if (g_hBitmap != NULL) DeleteObject(g_hBitmap);
		g_hBitmap = CreateCompatibleBitmap(hDC, CHIP_SIZE * 10, g_iLoadCGHeight);
		SelectObject(g_hmDC, g_hBitmap);
		// クライアントに描画
		g_pDib->DrawBits(g_hmDC, 0, 0);
	}

	// １／２サイズのＣＧ領域確保
	if (g_hBitmapHalf != NULL) DeleteObject(g_hBitmapHalf);
	g_hBitmapHalf = CreateCompatibleBitmap(hDC, 200, (g_iLoadCGHeight / 2));
	SelectObject(g_hmDCHalf, g_hBitmapHalf);
	// １／２サイズのＣＧを描画$$
	StretchBlt(g_hmDCHalf, 0, 0, 200, (g_iLoadCGHeight / 2), g_hmDC, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, SRCCOPY);

	// AND画像領域確保
	if (g_hBitmapAnd != NULL) DeleteObject(g_hBitmapAnd);
	g_hBitmapAnd = CreateBitmap(CHIP_SIZE * 10, g_iLoadCGHeight, 1, 1, NULL);
	SelectObject(g_hmDCAnd, g_hBitmapAnd);
	// AND画像作成
	if (g_iColorTp == 0) g_iColorTp = GetPixel(g_hmDC, 60, 20);
	SetBkColor(g_hmDC, g_iColorTp);
	BitBlt(g_hmDCAnd, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, g_hmDC, 0, 0, SRCCOPY);
	// AND画像反転
	BitBlt(g_hmDCAnd, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, NULL, 0, 0, DSTINVERT);
	// OR画像領域確保
	if (g_hBitmapOr != NULL) DeleteObject(g_hBitmapOr);
	g_hBitmapOr = CreateCompatibleBitmap(hDC, CHIP_SIZE * 10, g_iLoadCGHeight);
	SelectObject(g_hmDCOr, g_hBitmapOr);
	// OR画像作成
	BitBlt(g_hmDCOr, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, g_hmDC, 0, 0, SRCCOPY);
	BitBlt(g_hmDCOr, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, g_hmDCAnd, 0, 0, SRCAND);
	// AND画像反転
	BitBlt(g_hmDCAnd, 0, 0, CHIP_SIZE * 10, g_iLoadCGHeight, NULL, 0, 0, DSTINVERT);

	// デバイス解放
	ReleaseDC(g_hWnd, hDC);
	// 再描画
	InvalidateRect(g_hWnd, NULL, FALSE);
	InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
	InvalidateRect(g_hDlgSelectMap, NULL, FALSE);

	return TRUE;
}



//##------------------------------------------------------------------
// メインウィンドウへ描画
void PaintWindow()
{
	int x, y, x2, y2;
	HDC hDC;
	PAINTSTRUCT ps;
	HPEN hpen, hpenRed, hpenOld;

	hDC = BeginPaint(g_hWnd, &ps);
	paintMapAll(hDC);

	// キャラクタ境界線表示
	hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	hpenOld = (HPEN)SelectObject(hDC, hpen);
	hpenRed = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));

	// 境界線非表示
	if (g_MapLineFlag == TRUE) {
		for (x = 1; x < SCREEN_CHIP_SIZE; ++x) {
			DrawLine(hDC, x * CHIP_SIZE, YTOP, x * CHIP_SIZE, CHIP_SIZE * SCREEN_CHIP_SIZE + YTOP);
		}
		for (y = 1; y < SCREEN_CHIP_SIZE; ++y) {
			DrawLine(hDC, 0, y * CHIP_SIZE + YTOP, CHIP_SIZE * SCREEN_CHIP_SIZE, y * CHIP_SIZE + YTOP);
		}

		// マップ境界線表示
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

	// ドラッグによる四角描画
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

	// ペンの解放
	SelectObject(hDC, hpenOld);
	DeleteObject(hpen);
	DeleteObject(hpenRed);
	EndPaint(g_hWnd, &ps);

	// ステータス描画
	PaintStatus(TRUE);
}



//##------------------------------------------------------------------
// ステータス描画

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
	// ステータス非表示
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
	PaintStatusSub(x, y, 0, "背景設置");
	x += 69;
	PaintStatusSub(x, y, 1, "物体設置");
	x += 69;
	PaintStatusSub(x, y, 2, "背景編集");
	x += 69;
	PaintStatusSub(x, y, 3, "物体編集");
	x += 69;
	PaintStatusSub(x, y, 4, "物体削除");

	BitBlt(hDC, 0, 0, CHIP_SIZE * SCREEN_CHIP_SIZE, 20, g_hmDCExtra, 0, 0, SRCCOPY);
	ReleaseDC(g_hWnd, hDC);
}


//##------------------------------------------------------------------
// 数値変換
int unsignedByte(char numberByte) {
	int number;

	number = (int)numberByte;
	if (number < 0) number += 0x100;
	return number;
}


//##------------------------------------------------------------------
// 文字列の変換と読み出し
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

	// GetString が FALSE の場合はデータ取り込みはせずここで終わり
	if (GetString == FALSE) {
		AnsiString[0] = '\0';
		return;
	}
	DWORD chaLength = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, buffer, -1, AnsiString, chaLength, NULL, NULL);

	// 改行文字の検出と変換
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
// マップデータのロード
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

	// ダイアログ閉じる
	DestroyWindow(g_hDlgObject);
	DestroyWindow(g_hDlgMap);
	DestroyWindow(g_hDlgSelectChara);

	// データオープン
	hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(g_hWnd, "マップデータファイルが読み込みできません。", "注意", MB_OK);
		return FALSE;
	}
	// データ読み込み
	ReadFile(hFile, LPVOID(&PressData), sizeof(PressData), &BytesRead, 0);
	CloseHandle(hFile);

	// データ解凍
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

	// 基本設定
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

	// マップデータ
	pointer = 100;
	// 下位バージョンからの読み込みプロテクト
	if (g_MapData[DATA_VERSION] >= 29) pointer = 90;
	// 旧バージョン用にコンバート
	int MapWidth = g_iMapSize;
	if (g_MapData[DATA_VERSION] < 28) {
		MapWidth = 71;
		g_iMapSize = 71;
	}
	else if (g_MapData[DATA_VERSION] <= 29) {
		MapWidth = 101;
		g_iMapSize = 101;
	}
	// スクロールバー設定
	SetScrollRange(g_hWnd, SB_VERT, 0, (g_iMapSize - SCREEN_CHIP_SIZE), FALSE);
	SetScrollRange(g_hWnd, SB_HORZ, 0, (g_iMapSize - SCREEN_CHIP_SIZE), FALSE);

	// パーツ最大数設定
	g_iMapPartsMax = ((iDataMapCount - 1) / 50) * 50 + 50;
	if (g_iMapPartsMax < 200) g_iMapPartsMax = 200;
	g_iObjectPartsMax = ((iDataObjectCount - 1) / 50) * 50 + 50;
	if (g_iObjectPartsMax < 200) g_iObjectPartsMax = 200;
	// スクロールバー設定
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

	// 各メッセージデータが利用されているか確認する配列
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
	//マップキャラクタ
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
	//オブジェクトキャラクタ
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
	//下位互換拡張キャラクタ変換
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

	//メッセージデータの読みだし
	for (i = 0; i < MESSAGE_NUMBER_MAX; ++i) g_StrMessage[i][0] = '\0';
	pointer = point2;

	//バージョンアップ中テスト互換用
	if (g_MapData[DATA_VERSION] <= 29) {
		g_iMesNumberMax = 400;
	}
	else {
		//新暗証番号
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
	//その他データ
	loadMapString(g_worldName);

	//旧暗証番号
	if (g_MapData[DATA_VERSION] <= 29) loadMapString(g_worldPassword);
	else loadMapString(szTemp);

	loadMapString(g_mapcgNameBmp);
	loadMapString(g_mapcgName);

	if (g_MapData[DATA_VERSION] >= 30) {
		for (i = 0; i < 20; ++i) loadMapString(g_StrMessageSystem[i]);
	}

	//暗証番号のチェック
	g_bErrorPassword = FALSE;
	g_szInputPassword[0] = '\0';
	int number;
	if (atoi(g_worldPassword) != 0) {
		if (g_MapData[DATA_VERSION] >= 29) {
			//暗証番号変換
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
	//Undoセット
	SetUndoData();
	InvalidateRect(g_hDlgQuickView, NULL, FALSE);

	//暗証番号判定
	if (strcmp(g_worldPassword, g_szInputPassword) != 0) {
		ZeroMemory(&mapAttribute, sizeof(mapAttribute));
		ZeroMemory(&objectAttribute, sizeof(objectAttribute));
		ZeroMemory(&g_worldPassword, sizeof(g_worldPassword));
	}

	//タイトルテキスト設定
	char sTitle[100];
	sprintf(sTitle, "%s [%s]", g_szTitleName, FileName);
	SetWindowText(g_hWnd, sTitle);

	return TRUE;
}



//##------------------------------------------------------------------
// マップデータの保存

BOOL SaveMapData( char *FileName )
{
	//変数定義
	int i, j;
	int x, y;
	int number;
	int counter;
	int length;
	int checkData = 0;
	char szSavePassword[30];
	int xmax, ymax;

	//ダイアログ閉じる
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
	//マップ画面最大数設定
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

	//下位バージョンからの読み込みプロテクト
	pointer = 90;	//pointer = 100;

	//マップデータ
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

	//マップキャラクタ
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

	//オブジェクトキャラクタ
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
	//メッセージデータの最後尾検出
	for( i = (MESSAGE_NUMBER_MAX -1) ; i > 0 ; --i ){
		if( g_StrMessage[i][0] != '\0' ) break;
	}
	g_iMesNumberMax = i +1;
	PressData[DATA_MES_NUMBER] = (char)g_iMesNumberMax;
	PressData[DATA_MES_NUMBER +1] = (char)(g_iMesNumberMax >> 8);

	//圧縮部データ長
	length = pointer;

	//チェック用正誤番号
	for( i = 2 ; i < length ; ++i ) checkData += (PressData[i] *(i %8 +1));
	checkData %= 0x10000;
	PressData[DATA_CHECK] = (char)checkData;
	PressData[DATA_CHECK +1] = (char)(checkData >> 8);

	//マップデータ圧縮
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

	//暗証番号変換
	number = atoi( g_worldPassword );
	if( number != 0 ){
		number = ((number +2357) *17 +1197) *10 +(number %9);
		_itoa( number, szSavePassword, 10 );
	} else {
		szSavePassword[0] = '\0';
	}

	//新暗証番号
	saveMapString( szSavePassword );
	//メッセージデータの書き込み
	for (i = 0; i < g_iMesNumberMax; ++i) {
		saveMapString(g_StrMessage[i]);
	}
	//その他データ
	saveMapString( g_worldName );
	saveMapString( "" );
	saveMapString( g_mapcgNameBmp );
	saveMapString( g_mapcgName );
	for( i = 0 ; i < 20 ; ++i ) saveMapString( g_StrMessageSystem[i] );

	//データサイズの確認
	if( pointer >= FILE_DATA_MAX ){
		MessageBox( g_hWnd, "マップデータの総容量が許容値を超えています。", "注意！", MB_OK );
		return FALSE;
	}

	//データの書き込み
	HANDLE hFile;
	DWORD dwWritten;
	hFile = CreateFile( FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE ){
		MessageBox( g_hWnd, "マップデータファイルに書き込みできません。\n読み取り専用になっていないか確認してください。", "注意", MB_OK );
		return FALSE;
	}
	WriteFile( hFile, g_MapData, pointer, &dwWritten, NULL );
	CloseHandle( hFile );

	//タイトルテキスト設定
	char sTitle[100];
	sprintf( sTitle, "%s [%s]", g_szTitleName, FileName );
	SetWindowText( g_hWnd, sTitle );

	//バックアップデータ作成
	char szBackupFile[250];
	SYSTEMTIME systime;
	GetLocalTime( &systime );
	sprintf( szBackupFile, "backup\\%s%d.dat", FileName, systime.wHour );
	//書き込み
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
// マップの描画
void paintMapAll(HDC hDC)
{
	//変数定義
	int i, j;
	int mdata;
	char objectNumber[10];
	HFONT numberFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FF_DONTCARE);
	HFONT defaultFont = SelectFont(hDC, numberFont);

	//マップ描画
	for (j = 0; j < SCREEN_CHIP_SIZE; ++j) {
		for (i = 0; i < SCREEN_CHIP_SIZE; ++i) {
			//背景描画
			mdata = map[j + mapYtop][i + mapXtop];
			BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, CHIP_SIZE, CHIP_SIZE, g_hmDC, mapAttribute[mdata][ATR_X], mapAttribute[mdata][ATR_Y], SRCCOPY);

			//オブジェクト描画
			mdata = mapObject[j + mapYtop][i + mapXtop];
			if (mdata != 0) {
				BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, CHIP_SIZE, CHIP_SIZE, g_hmDCAnd, objectAttribute[mdata][ATR_X], objectAttribute[mdata][ATR_Y], SRCAND);
				BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, CHIP_SIZE, CHIP_SIZE, g_hmDCOr, objectAttribute[mdata][ATR_X], objectAttribute[mdata][ATR_Y], SRCPAINT);
			}
			//プレーヤーキヤラクタ描画
			if ((charaX == (i + mapXtop)) && (charaY == (j + mapYtop))) {
				BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, CHIP_SIZE, CHIP_SIZE, g_hmDCAnd, CHIP_SIZE * 4, 0, SRCAND);
				BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, CHIP_SIZE, CHIP_SIZE, g_hmDCOr, CHIP_SIZE * 4, 0, SRCPAINT);
			}
			//透明パーツ識別マーク描画
			if (g_ObjectNumberFlag == TRUE && mdata != 0) {
				sprintf(objectNumber, "%d", mdata);
				TextOut(hDC, i * CHIP_SIZE, j * CHIP_SIZE + YTOP, objectNumber, strlen(objectNumber));
			}
		}
	}

	SelectFont(hDC, defaultFont);
}


//##------------------------------------------------------------------
// 物体選択ダイアログの描画
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

	//イメージの描画
	for (j = 0; j < DIALOG_OBJECT_SELECT_LINE; ++j) {
		for (i = 0; i < DIALOG_OBJECT_SELECT_COLUMN; ++i) {
			x = objectAttribute[i + j * DIALOG_OBJECT_SELECT_COLUMN + g_ScrObject * DIALOG_OBJECT_SELECT_COLUMN][ATR_X];
			y = objectAttribute[i + j * DIALOG_OBJECT_SELECT_COLUMN + g_ScrObject * DIALOG_OBJECT_SELECT_COLUMN][ATR_Y];
			BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE, CHIP_SIZE, CHIP_SIZE, g_hmDC, x, y, SRCCOPY);
		}
	}

	SetDlgItemInt(hWnd, IDC_EDIT_PARTS_NUMBER, g_SelectObjectData, FALSE);
	SetDlgItemInt(hWnd, IDC_EDIT_CURRENT_POSITION, g_ScrObject * DIALOG_OBJECT_SELECT_COLUMN, FALSE);

	//境界線表示
	hpen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	hpenOld = (HPEN)SelectObject(hDC, hpen);

	//枠表示
	if ((selectY - g_ScrObject) * CHIP_SIZE < CHIP_SIZE * DIALOG_OBJECT_SELECT_LINE) {
		DrawRect(hDC, selectX * CHIP_SIZE, (selectY - g_ScrObject) * CHIP_SIZE, CHIP_SIZE, CHIP_SIZE);
		DrawRect(hDC, selectX * CHIP_SIZE + 1, (selectY - g_ScrObject) * CHIP_SIZE + 1, 38, 38);
	}

	SelectObject(hDC, hpenOld);
	DeleteObject(hpen);
	EndPaint(hWnd, &ps);
}


//##------------------------------------------------------------------
// 背景選択ダイアログの描画
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

	//イメージの描画
	for (j = 0; j < DIALOG_MAP_SELECT_LINE; ++j) {
		for (i = 0; i < DIALOG_MAP_SELECT_COLUMN; ++i) {
			x = mapAttribute[i + j * 10 + g_ScrMap * 10][ATR_X];
			y = mapAttribute[i + j * 10 + g_ScrMap * 10][ATR_Y];
			BitBlt(hDC, i * CHIP_SIZE, j * CHIP_SIZE, CHIP_SIZE, CHIP_SIZE, g_hmDC, x, y, SRCCOPY);
		}
	}
	//文字表示
	SetDlgItemInt(hWnd, IDC_EDIT_PARTS_NUMBER, g_SelectMapData, FALSE);
	SetDlgItemInt(hWnd, IDC_EDIT_CURRENT_POSITION, g_ScrMap * DIALOG_MAP_SELECT_COLUMN, FALSE);

	//境界線表示
	hpen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	hpenOld = (HPEN)SelectObject(hDC, hpen);

	//枠表示
	if ((selectY - g_ScrMap) * CHIP_SIZE < CHIP_SIZE * DIALOG_MAP_SELECT_LINE) {
		DrawRect(hDC, selectX * CHIP_SIZE, (selectY - g_ScrMap) * CHIP_SIZE, CHIP_SIZE, CHIP_SIZE);
		DrawRect(hDC, selectX * CHIP_SIZE + 1, (selectY - g_ScrMap) * CHIP_SIZE + 1, CHIP_SIZE - 2, CHIP_SIZE - 2);
	}
	SelectObject(hDC, hpenOld);
	DeleteObject(hpen);
	EndPaint(hWnd, &ps);
}



//##------------------------------------------------------------------
// 物体選択ダイアログプロシージャ
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
		// 現在選択中の物体を設定
		g_SelectObjectData = g_SelectObjectX + g_SelectObjectY * DIALOG_OBJECT_SELECT_COLUMN;
		// 編集モード変更
		DeleteCheckMenu();
		CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTOBJ, MF_CHECKED);
		g_EditMode = 1;
		PaintStatus(TRUE);
		break;
	}
	case WM_LBUTTONDBLCLK: {
		//ダイアログ表示
		DestroyWindow(g_hDlgObject);
		DestroyWindow(g_hDlgMap);
		//Undoセット
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
		// 現在選択中の物体を設定
		g_SelectObjectData = g_SelectObjectX + g_SelectObjectY * DIALOG_OBJECT_SELECT_COLUMN;
		// 編集モード変更
		DeleteCheckMenu();
		CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTOBJ, MF_CHECKED);
		g_EditMode = 1;
		PaintStatus(TRUE);
		// ダイアログ表示
		DestroyWindow(g_hDlgObject);
		DestroyWindow(g_hDlgMap);
		// Undoセット
		SetUndoData();
		DisplayObjectDialog();
		break;
	}
	case WM_COMMAND: {
		// 物体編集ダイアログの表示
		if (LOWORD(wParam) == IDC_BUTTON_EDITMAP) {
			DestroyWindow(g_hDlgObject);
			DestroyWindow(g_hDlgMap);
			// Undoセット
			SetUndoData();
			DisplayObjectDialog();
		}
		// 物体データの消去
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
// 背景選択ダイアログプロシージャ
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
		//現在選択中の背景を設定
		g_SelectMapData = g_SelectMapX + g_SelectMapY * DIALOG_MAP_SELECT_COLUMN;
		//編集モード変更
		DeleteCheckMenu();
		CheckMenuItem( GetMenu(g_hWnd), ID_MENU_PUTMAP, MF_CHECKED );
		g_EditMode = 0;
		PaintStatus( TRUE );
		break;
	}
	case WM_LBUTTONDBLCLK:{
		// ダイアログ表示
		DestroyWindow( g_hDlgObject );
		DestroyWindow( g_hDlgMap );
		// Undoセット
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
		// 現在選択中の背景を設定
		g_SelectMapData = g_SelectMapX + g_SelectMapY * DIALOG_MAP_SELECT_COLUMN;
		// 編集モード変更
		DeleteCheckMenu();
		CheckMenuItem(GetMenu(g_hWnd), ID_MENU_PUTMAP, MF_CHECKED);
		g_EditMode = 0;
		PaintStatus( TRUE );
		// ダイアログ表示
		DestroyWindow(g_hDlgObject);
		DestroyWindow(g_hDlgMap);
		// Undoセット
		SetUndoData();
		DisplayMapDialog();
		break;
	}
	case WM_COMMAND:{
		// 背景編集ダイアログの表示
		if( LOWORD(wParam) == IDC_BUTTON_EDITMAP ){
			DestroyWindow( g_hDlgObject );
			DestroyWindow( g_hDlgMap );
			// Undoセット
			SetUndoData();
			DisplayMapDialog();
		}
		// 背景データの消去
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
// 物体編集ダイアログプロシージャ
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
		//ダイアログの作成
		DestroyWindow( g_hDlgSelectChara );
		g_hModeSelectChara = 1;
		g_hDlgSelectChara = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_SELECTCHARA), hWnd, (DLGPROC)SelectCGCharaProc );
		//ダイアログ移動
		RECT rect;
		RECT rectBox;
		GetWindowRect( g_hDlgObject, &rectBox );
		GetWindowRect( g_hDlgSelectChara, &rect );
		MoveWindow( g_hDlgSelectChara, rectBox.left, rectBox.top + SelectCharaDialogY, rect.right -rect.left, rect.bottom -rect.top, TRUE );
		ShowWindow( g_hDlgSelectChara, SW_SHOW );
		break;
	}
	case WM_COMMAND:
		//物体種別の変更
		if( (HIWORD(wParam) == CBN_SELCHANGE) && (LOWORD(wParam) == IDC_COMBO_OBJECT) ){
			int number = SendMessage( GetDlgItem(hWnd,IDC_COMBO_OBJECT), CB_GETCURSEL, 0, 0 );
			objectAttribute[GetCharaNumber(hWnd)][ATR_TYPE] = OBJ[number].Object;
			g_bRestoreObjectDialog = TRUE;
			InvalidateRect( hWnd, NULL, FALSE );
		}
		//取り消し
		else if( wParam == ID_BUTTON_CANCEL ){
			int i;
			for( i = 0 ; i < OBJECT_ATR_MAX ; ++i ) objectAttribute[GetCharaNumber(hWnd)][i] = StockObjectAttribute[GetCharaNumber(hWnd)][i];
			InvalidateRect( g_hWnd, NULL, FALSE );
			InvalidateRect( g_hDlgSelectObject, NULL, FALSE );
			g_bCancel = TRUE;
			DestroyWindow( hWnd );
			return 1;
		}
		//決定
		else if( (wParam == IDOK) || (wParam == IDCANCEL) ){
			//ウィンドウ閉じる
			DestroyWindow( hWnd );
			return 1;
		}
		break;

	case WM_PAINT: {
		//オブジェクト描画
		HDC hDC = GetDC( hWnd );
		BitBlt( hDC, 5, 4, CHIP_SIZE, CHIP_SIZE, g_hmDC, objectAttribute[GetCharaNumber(hWnd)][ATR_X], objectAttribute[GetCharaNumber(hWnd)][ATR_Y], SRCCOPY );
		BitBlt( hDC, 50, 4, CHIP_SIZE, CHIP_SIZE, g_hmDC, objectAttribute[GetCharaNumber(hWnd)][ATR_X2], objectAttribute[GetCharaNumber(hWnd)][ATR_Y2], SRCCOPY );
		//文字表示
		char str[50];
		SetBkMode( hDC, TRANSPARENT );
		sprintf( str, "物体番号：%d", GetCharaNumber(hWnd) );
		TextOut( hDC,96,27,str,strlen(str) );
		ReleaseDC( hWnd, hDC );
		//ウィンドウ再描画
		if( g_bRestoreObjectDialog == TRUE ){
			DestroyWindow( hWnd );
			return 1;
		}
		break;
	}
	case WM_DESTROY:{
		if( g_bCancel == FALSE ){
			//物体データの決定
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
// 背景編集ダイアログプロシージャ
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
		//ダイアログの作成
		DestroyWindow(g_hDlgSelectChara);
		g_hModeSelectChara = 0;
		g_hDlgSelectChara = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_SELECTCHARA), hWnd, (DLGPROC)SelectCGCharaProc);
		//ダイアログ移動
		RECT rect, rectBox;
		GetWindowRect(g_hDlgMap, &rectBox);
		GetWindowRect(g_hDlgSelectChara, &rect);
		MoveWindow(g_hDlgSelectChara, rectBox.left, rectBox.top + SelectCharaDialogY, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		ShowWindow(g_hDlgSelectChara, SW_SHOW);
		break;
	}
	case WM_COMMAND: {
		//背景種別の変更
		if ((HIWORD(wParam) == CBN_SELCHANGE) && (LOWORD(wParam) == IDC_COMBO_MAP)) {
			int number = SendMessage(GetDlgItem(hWnd, IDC_COMBO_MAP), CB_GETCURSEL, 0, 0);
			mapAttribute[GetCharaNumber(hWnd)][ATR_TYPE] = MAP[number].Object;
			g_bRestoreMapDialog = TRUE;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		//取り消し
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
		// 決定
		else if ((wParam == IDOK) || (wParam == IDCANCEL)) {
			// ウィンドウ閉じる
			DestroyWindow(hWnd);
			return 1;
		}
		break;
	}
	case WM_PAINT: {
		// オブジェクト描画
		HDC hDC = GetDC(hWnd);
		BitBlt(hDC, 5, 4, CHIP_SIZE, CHIP_SIZE, g_hmDC, mapAttribute[GetCharaNumber(hWnd)][ATR_X], mapAttribute[GetCharaNumber(hWnd)][ATR_Y], SRCCOPY);
		// 文字表示
		char str[50];
		SetBkMode(hDC, TRANSPARENT);
		sprintf(str, "背景番号：%d", GetCharaNumber(hWnd));
		TextOut(hDC, 96, 27, str, strlen(str));
		ReleaseDC(hWnd, hDC);
		// ウィンドウ再描画
		if (g_bRestoreMapDialog == TRUE) {
			DestroyWindow(hWnd);
			return 1;
		}
		break;
	}
	case WM_DESTROY: {
		if (g_bCancel == FALSE) {
			// 背景データの決定
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
// クイックビューダイアログプロシージャ
LRESULT CALLBACK QuickViewDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_PAINT: {
		HDC hDC = GetDC(hWnd);
		int type;
		if (g_EditMode == 0) {
			BitBlt(hDC, 5, 4, CHIP_SIZE, CHIP_SIZE, g_hmDC, mapAttribute[g_SelectMapData][ATR_X], mapAttribute[g_SelectMapData][ATR_Y], SRCCOPY);
			SetDlgItemText(g_hDlgQuickView, IDC_EDIT_QVIEW, g_StrMessage[mapAttribute[g_SelectMapData][ATR_STRING]]);
			//文字表示
			char str[50];
			SetBkColor(hDC, GetSysColor(COLOR_3DFACE));
			sprintf(str, "背景ﾊﾟｰﾂ番号：%3d  ", g_SelectMapData);
			TextOut(hDC, 50, 5, str, strlen(str));
			type = mapAttribute[g_SelectMapData][ATR_TYPE];
			sprintf(str, "種類：%s ", g_MapName[type]);
			TextOut(hDC, 50, 26, str, strlen(str));
		}
		else if (g_EditMode == 1) {
			BitBlt(hDC, 5, 4, CHIP_SIZE, CHIP_SIZE, g_hmDC, objectAttribute[g_SelectObjectData][ATR_X], objectAttribute[g_SelectObjectData][ATR_Y], SRCCOPY);
			SetDlgItemText(g_hDlgQuickView, IDC_EDIT_QVIEW, g_StrMessage[objectAttribute[g_SelectObjectData][ATR_STRING]]);
			//文字表示
			char str[50];
			SetBkColor(hDC, GetSysColor(COLOR_3DFACE));
			sprintf(str, "物体ﾊﾟｰﾂ番号：%3d  ", g_SelectObjectData);
			TextOut(hDC, 50, 5, str, strlen(str));
			type = objectAttribute[g_SelectObjectData][ATR_TYPE];
			sprintf(str, "種類：%s ", g_ObjectName[type]);
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
// キャラクタＣＧ選択ダイアログプロシージャ
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
			//画面再描画
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
// 線と四角の描画

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
// 物体編集ダイアログの表示
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

	//取り消し用データのストック
	if( g_bRestoreObjectDialog == FALSE ){
		g_EditObjectData = g_SelectObjectData;
		StockAttributeData( g_EditObjectData, 0 );
	}
	g_bRestoreObjectDialog = FALSE;
	type = objectAttribute[g_EditObjectData][ATR_TYPE];

	if( g_EditObjectData == 0 ){
		MessageBox( g_hWnd, "パーツ番号０の物体は編集できません。\nこのパーツはマップの物体を消去するときに指定してください。", "注意！", MB_OK );
		return;
	}
	//種類の判定
	for( position = 0 ; position < 20 ; ++position ){
		if( OBJ[position].Object == END ) break;
		if( type == OBJ[position].Object ){
			id = OBJ[position].Id;
			break;
		}
	}
	if( id == 20 ) return;

	//ダイアログの作成
	g_hDlgObject = CreateDialog( g_hInst, MAKEINTRESOURCE(id), g_hWnd, (DLGPROC)EditObjectDialogProc );
	//コンボボックスにデータ挿入
	HWND hwndCombo = GetDlgItem( g_hDlgObject, IDC_COMBO_OBJECT );
	for( i = 0 ; i < 13 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)OBJ[i].Name );
	SendMessage( hwndCombo, CB_SETCURSEL, position, 0 );
	//動作属性コンボボックスにデータ挿入
	char *SetName[] = { "静止", "プレーヤー追尾", "逃げる", "うろうろする" };
	hwndCombo = GetDlgItem( g_hDlgObject, IDC_COMBO_MOVE );
	if( hwndCombo != NULL ){
		for( i = 0 ; i < 4 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)SetName[i] );
	}
	//物体属性コンボボックスにデータ挿入
	char *SetName2[] = { "通り抜け不可", "通り抜け可" };
	hwndCombo = GetDlgItem( g_hDlgObject, IDC_COMBO_MODE );
	if( hwndCombo != NULL ){
		for( i = 0 ; i < 2 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)SetName2[i] );
	}
	//扉属性コンボボックスにデータ挿入
	char *SetName3[] = { "鍵なくなる", "鍵なくならない" };
	hwndCombo = GetDlgItem( g_hDlgObject, IDC_COMBO_MODE2 );
	if( hwndCombo != NULL ){
		for( i = 0 ; i < 2 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)SetName3[i] );
	}
	//使用属性コンボボックスにデータ挿入
	char *SetName4[] = { "通常", "クリックで使用可", "使用しても無くならない" };
	hwndCombo = GetDlgItem( g_hDlgObject, IDC_COMBO_ITEM );
	if( hwndCombo != NULL ){
		for( i = 0 ; i < 3 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)SetName4[i] );
	}
	
	//メッセージボックスにテキスト挿入
	if( (type == OBJECT_MESSAGE) || (type == OBJECT_MONSTER) || (type == OBJECT_ITEM) || (type == OBJECT_SCORE)
				|| (type == OBJECT_STATUS) || (type == OBJECT_DOOR) || (type == OBJECT_SELL) || (type == OBJECT_BUY) || (type == OBJECT_SELECT) ){
		SetDlgItemText( g_hDlgObject, IDC_EDIT_MESSAGE, g_StrMessage[objectAttribute[g_EditObjectData][ATR_STRING]] );
	} else if( type == OBJECT_URLGATE ){
		strcpy( str, g_StrMessage[objectAttribute[g_EditObjectData][ATR_STRING]] );
		//文字列分割
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
	//数値データを挿入
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
			//ジャンプゲートの場合
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
			//コンボボックスの場合
			else if( (objectAttribute[g_EditObjectData][ATR_TYPE] != OBJECT_LOCALGATE) && (objectAttribute[g_EditObjectData][ATR_TYPE] != OBJECT_RANDOM) && ((Object[number].Atr == ATR_MOVE) || (Object[number].Atr == ATR_MODE))
						|| ((objectAttribute[g_EditObjectData][ATR_TYPE] == OBJECT_DOOR) && (Object[number].Atr == ATR_NUMBER)) ){
				hwndCombo = GetDlgItem( g_hDlgObject, Object[number].Id );
				SendMessage( hwndCombo, CB_SETCURSEL, status, 0 );
			}
			//エディットボックスの場合
			else {
				_itoa( status, str, 10 );
				SetDlgItemText( g_hDlgObject, Object[number].Id, str );
			}
		}
	}
	//ダイアログ移動
	GetWindowRect( g_hDlgSelectObject, &rectBox );
	GetWindowRect( g_hDlgObject, &rect );
	MoveWindow( g_hDlgObject, rectBox.right, rectBox.top, rect.right -rect.left, rect.bottom -rect.top, TRUE );
	ShowWindow( g_hDlgObject, SW_SHOW );

	//ダイアログの作成
	if( (type == OBJECT_MESSAGE) || (type == OBJECT_MONSTER) || (type == OBJECT_ITEM)
				|| (type == OBJECT_STATUS) || (type == OBJECT_DOOR) || (type == OBJECT_SELL) || (type == OBJECT_BUY) || (type == OBJECT_SELECT) || (type == OBJECT_LOCALGATE) ){
		if( type == OBJECT_SELECT ) g_hDlgExtra = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EXTRA2), g_hDlgObject, (DLGPROC)DialogProcExtraObject );
		else g_hDlgExtra = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EXTRA), g_hDlgObject, (DLGPROC)DialogProcExtraObject );

		//ダイアログ移動
		GetWindowRect( g_hDlgObject, &rectBox );
		GetWindowRect( g_hDlgExtra, &rect );
		MoveWindow( g_hDlgExtra, rectBox.left, rectBox.bottom, rect.right -rect.left, rect.bottom -rect.top, TRUE );
		ShowWindow( g_hDlgExtra, SW_SHOW );

		//フォーカス移動
		SetFocus( g_hDlgObject );
		//拡張出現キャラクタの設定
		AppearChara( g_EditObjectData, TRUE );
	}

	//タイトルバーテキスト設定
	char title[100];
	GetWindowText( g_hDlgObject, title, sizeof(title) );
	sprintf( str, "o%d.  %s", g_EditObjectData, title );
	SetWindowText( g_hDlgObject, str );
	GetWindowText( g_hDlgExtra, title, sizeof(title) );
	sprintf( str, "o%d.  %s", g_EditObjectData, title );
	SetWindowText( g_hDlgExtra, str );
}



//##------------------------------------------------------------------
// 背景編集ダイアログの表示

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

	//取り消し用データのストック
	if( g_bRestoreMapDialog == FALSE ){
		g_EditMapData = g_SelectMapData;
		StockAttributeData( g_EditMapData, 1 );
	}
	g_bRestoreMapDialog = FALSE;
	type = mapAttribute[g_EditMapData][ATR_TYPE];
	
	if( g_EditMapData == 0 ){
		MessageBox( g_hWnd, "パーツ番号０の背景は編集できません。\nこのパーツはマップの背景を消去するときに指定してください。", "注意！", MB_OK );
		return;
	}
	//種類の判定
	for( position = 0 ; position < 20 ; ++position ){
		if( MAP[position].Object == END ) break;
		if( type == MAP[position].Object ){
			id = MAP[position].Id;
			break;
		}
	}
	if( id == 20 ) return;

	//ダイアログの作成
	g_hDlgMap = CreateDialog( g_hInst, MAKEINTRESOURCE(id), g_hWnd, (DLGPROC)EditMapDialogProc );

	//コンボボックスにデータ挿入
	HWND hwndCombo = GetDlgItem( g_hDlgMap, IDC_COMBO_MAP );
	for( i = 0 ; i < 4 ; ++i ) SendMessage( hwndCombo, CB_ADDSTRING, 0, (LPARAM)MAP[i].Name );
	SendMessage( hwndCombo, CB_SETCURSEL, position, 0 );

	//メッセージボックスにテキスト挿入
	if( (type == MAP_STREET) || (type == MAP_WALL) || (type == MAP_ITEMCHECK) ){
		SetDlgItemText( g_hDlgMap, IDC_EDIT_MESSAGE, g_StrMessage[mapAttribute[g_EditMapData][ATR_STRING]] );
	} else if( type == MAP_URLGATE ){
		strcpy( str, g_StrMessage[mapAttribute[g_EditMapData][ATR_STRING]] );
		//文字列分割
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
	//数値データを挿入
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
	//ダイアログ移動
	GetWindowRect( g_hDlgSelectObject, &rectBox );
	GetWindowRect( g_hDlgMap, &rect );
	MoveWindow( g_hDlgMap, rectBox.right, rectBox.top, rect.right -rect.left, rect.bottom -rect.top, TRUE );
	ShowWindow( g_hDlgMap, SW_SHOW );

	//ダイアログの作成
	if( (type == MAP_STREET) || (type == MAP_WALL) || (type == MAP_ITEMCHECK) || (type == MAP_LOCALGATE) ){
		g_hDlgExtra = CreateDialog( g_hInst, MAKEINTRESOURCE(IDD_DIALOG_EXTRA), g_hDlgMap, (DLGPROC)DialogProcExtraMap );
		//ダイアログ移動
		GetWindowRect( g_hDlgMap, &rectBox );
		GetWindowRect( g_hDlgExtra, &rect );
		MoveWindow( g_hDlgExtra, rectBox.left, rectBox.bottom, rect.right -rect.left, rect.bottom -rect.top, TRUE );
		ShowWindow( g_hDlgExtra, SW_SHOW );
		//フォーカス移動
		SetFocus( g_hDlgMap );
		//拡張出現キャラクタの設定
		AppearChara( g_EditMapData, FALSE );
	}
	
	//タイトルバーテキスト設定
	char title[100];
	GetWindowText( g_hDlgMap, title, sizeof(title) );
	sprintf( str, "m%d.  %s", g_EditMapData, title );
	SetWindowText( g_hDlgMap, str );
	GetWindowText( g_hDlgExtra, title, sizeof(title) );
	sprintf( str, "m%d.  %s", g_EditMapData, title );
	SetWindowText( g_hDlgExtra, str );
}



//##------------------------------------------------------------------
// 物体データの決定

void SetObjectData( HWND hWnd, int charaNumber )
{
	int i;
	int number;
	int length;
	char str[MESSAGE_STR_MAX];
	char strCut[50];
	int type = objectAttribute[charaNumber][ATR_TYPE];
	int position;

	//何も設定されていなければキャンセル
	for( i = 0 ; i < OBJECT_ATR_MAX ; ++i ){
		if( objectAttribute[charaNumber][i] != 0 ) break;
	}
	if( i == OBJECT_ATR_MAX ) return;

	//パーツ番号
	objectAttribute[charaNumber][ATR_0] = charaNumber;

	if( (type == OBJECT_MESSAGE) || (type == OBJECT_MONSTER) || (type == OBJECT_ITEM) || (type == OBJECT_SCORE)
				|| (type == OBJECT_STATUS) || (type == OBJECT_DOOR) || (type == OBJECT_SELL) || (type == OBJECT_BUY) || (type == OBJECT_SELECT) ){
		//文字データのセット
		GetDlgItemText( hWnd, IDC_EDIT_MESSAGE, str, MESSAGE_STR_MAX );
		SetMessageData( &objectAttribute[charaNumber][ATR_STRING], str );
	} else if( type == OBJECT_URLGATE ){
		GetDlgItemText( hWnd, IDC_EDIT_URL, str, MESSAGE_STR_MAX );
		GetDlgItemText( hWnd, IDC_EDIT_TARGET, strCut, 50 );
		if( (strstr(strCut,"expand") != 0) || (strstr(strCut,"ＥＸＰＡＮＤ") != 0) ) strcpy( strCut, "EXPAND" );
		if( strlen(strCut) != 0 ){
			length = strlen(str);
			str[length] = 0x0D;
			str[length+1] = 0x0A;
			str[length+2] = '\0';
			strcat( str, strCut );
		}
		SetMessageData( &objectAttribute[charaNumber][ATR_STRING], str );
	}
	//数値データを設定
	for( number = 0 ; number < 100 ; ++number ){
		if( Object[number].Object == END ) break;
		if( Object[number].Object == type ){
			GetDlgItemText( hWnd, Object[number].Id, str, sizeof(str) );
			
			//マイナス数値の指定
			if( ((Object[number].Atr == ATR_ENERGY) && (Object[number].Object == OBJECT_SELL))
						|| ((Object[number].Atr == ATR_ENERGY) && (Object[number].Object == OBJECT_STATUS))
						|| ((Object[number].Atr == ATR_STRENGTH) && (Object[number].Object == OBJECT_STATUS))
						|| ((Object[number].Atr == ATR_DEFENCE) && (Object[number].Object == OBJECT_STATUS))
						|| ((Object[number].Atr == ATR_GOLD) && (Object[number].Object == OBJECT_STATUS)) ){
				if( (atoi(str) < 0) && (atoi(str) >= -30000) ){
					objectAttribute[charaNumber][Object[number].Atr] = 30000 - atoi(str);
					continue;
				} else if( atoi(str) < -30000 ){
					sprintf( str, "「%s」の数値範囲が規定値を超えています。\n -30000 以上を指定してください。", Object[number].Name );
					MessageBox( g_hWnd, str, "警告！", MB_OK );
				}
			}
			if( (objectAttribute[g_EditObjectData][ATR_TYPE] == OBJECT_SELL) && (Object[number].Atr == ATR_ITEM) ){
				if( (objectAttribute[atoi(str)][ATR_TYPE] != OBJECT_ITEM) && (objectAttribute[atoi(str)][ATR_TYPE] != OBJECT_NORMAL) && (objectAttribute[atoi(str)][ATR_TYPE] != OBJECT_MESSAGE) ){
					MessageBox( g_hWnd, "売るアイテムには、ステータス変化パーツなどのアイテム以外のパーツは指定できません。", "警告！", MB_OK );
					continue;
				}
			}
			if( (objectAttribute[g_EditObjectData][ATR_TYPE] == OBJECT_LOCALGATE) && ((Object[number].Atr == ATR_JUMP_X) || (Object[number].Atr == ATR_JUMP_Y)) ){
				int position = atoi( str );
				if( position >= g_iMapSize ){
					MessageBox( g_hWnd, "「座標」の数値範囲が規定値を超えています。\n マップサイズ以下を指定してください。", "警告！", MB_OK );
				} else if( (position < -100) || ((position > 100) && (str[0] == '+')) ){
					MessageBox( g_hWnd, "「座標」の数値範囲が規定値を超えています。\n -100以上、100以下を指定してください。", "警告！", MB_OK );
				} else {
					if( (str[0] == '+') || (str[0] == '-') ) position += 10000;
					objectAttribute[charaNumber][Object[number].Atr] = position;
				}
			} else if( (Object[number].Max != 0) && ((Object[number].Max < atoi(str)) || (atoi(str) < 0)) ){
				sprintf( str, "「%s」の数値範囲が規定値を超えています。\n 0以上 %d 以下を指定してください。", Object[number].Name, Object[number].Max );
				MessageBox( g_hWnd, str, "警告！", MB_OK );
			} else {
				if( (objectAttribute[g_EditObjectData][ATR_TYPE] != OBJECT_LOCALGATE) && (objectAttribute[g_EditObjectData][ATR_TYPE] != OBJECT_RANDOM) && ((Object[number].Atr == ATR_MOVE) || (Object[number].Atr == ATR_MODE))
							|| ((objectAttribute[g_EditObjectData][ATR_TYPE] == OBJECT_DOOR) && (Object[number].Atr == ATR_NUMBER)) ){
					//コンボボックスのデータ
					position = SendMessage( GetDlgItem(hWnd,Object[number].Id), CB_GETCURSEL, 0, 0 );
					objectAttribute[charaNumber][Object[number].Atr] = position;
				} else {
					//エディットボックス
					objectAttribute[charaNumber][Object[number].Atr] = atoi(str);
				}
			}
		}
	}
}



//##------------------------------------------------------------------
// 背景データの決定

void SetMapData( HWND hWnd, int charaNumber )
{
	int i;
	int number;
	int length;
	char str[MESSAGE_STR_MAX];
	char strCut[50];
	int type = mapAttribute[charaNumber][ATR_TYPE];

	//何も設定されていなければキャンセル
	for( i = 0 ; i < MAP_ATR_MAX ; ++i ){
		if( mapAttribute[charaNumber][i] != 0 ) break;
	}
	if( i == MAP_ATR_MAX ) return;

	//パーツ番号
	mapAttribute[charaNumber][ATR_0] = charaNumber;

	if( (type == MAP_STREET) || (type == MAP_WALL) || (type == MAP_ITEMCHECK) ){
		//文字データのセット
		GetDlgItemText( hWnd, IDC_EDIT_MESSAGE, str, MESSAGE_STR_MAX );
		SetMessageData( &mapAttribute[charaNumber][ATR_STRING], str );
	} else if( type == MAP_URLGATE ){
		GetDlgItemText( hWnd, IDC_EDIT_URL, str, MESSAGE_STR_MAX );
		GetDlgItemText( hWnd, IDC_EDIT_TARGET, strCut, 50 );
		if( (strstr(strCut,"expand") != 0) || (strstr(strCut,"ＥＸＰＡＮＤ") != 0) ) strcpy( strCut, "EXPAND" );
		if( strlen(strCut) != 0 ){
			length = strlen(str);
			str[length] = 0x0D;
			str[length+1] = 0x0A;
			str[length+2] = '\0';
			strcat( str, strCut );
		}
		SetMessageData( &mapAttribute[charaNumber][ATR_STRING], str );
	}
	//数値データを設定
	for( number = 0 ; number < 100 ; ++number ){
		if( Map[number].Object == END ) break;
		if( Map[number].Object == type ){
			GetDlgItemText( hWnd, Map[number].Id, str, sizeof(str) );
			if( (Map[number].Atr == ATR_JUMP_X) || (Map[number].Atr == ATR_JUMP_Y) ){
				int position = atoi( str );
				if( position >= g_iMapSize ){
					MessageBox( g_hWnd, "「座標」の数値範囲が規定値を超えています。\n マップサイズ以下を指定してください。", "警告！", MB_OK );
				} else if( (position < -100) || ((position > 100) && (str[0] == '+')) ){
					MessageBox( g_hWnd, "「座標」の数値範囲が規定値を超えています。\n -100以上、100以下を指定してください。", "警告！", MB_OK );
				} else {
					if( (str[0] == '+') || (str[0] == '-') ) position += 10000;
					mapAttribute[charaNumber][Map[number].Atr] = position;
				}
			} else {
				if( (Map[number].Max != 0) && ((Map[number].Max < atoi(str)) || (atoi(str) < 0)) ){
					sprintf( str, "「%s」の数値範囲が規定値を超えています。\n 0以上、%d以下を指定してください。", Map[number].Name, Map[number].Max );
					MessageBox( g_hWnd, str, "警告！", MB_OK );
				} else {
					mapAttribute[charaNumber][Map[number].Atr] = atoi( str );
				}
			}
		}
	}
}



//##------------------------------------------------------------------
// 拡張出現キャラクタの設定

void AppearChara( int mapNumber, BOOL flag )
{
	int i;
	int dataChara;
	int dataMode;
	int x, y;
	char str[30];

	//拡張キャラクタ・データコンバート
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
		//ボタン文字設定
		if( dataMode == 0 ) SetDlgItemText( g_hDlgExtra, g_EditId[30+i], "物" );
		else SetDlgItemText( g_hDlgExtra, g_EditId[30+i], "背景" );
		
		//ＸＹ座標系
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
		//すべて０なら表示しない
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
// 拡張出現キャラクタのデータ記録

void SetAppearChara( int mapNumber, BOOL flag )
{
	int i;
	int dataChara;
	int x, y;
	char str[30];
	char partsNumberStr[30];

	//拡張キャラクタ・データコンバート
	for( i = 0 ; i < 10 ; ++i ){
		GetDlgItemText( g_hDlgExtra, g_EditId[i], str, sizeof(str) );
		strcpy( partsNumberStr, str );

		// プラスマイナス記号によるパーツ番号インクリメント・デクリメント
		if (str[0] == '+') {
			dataChara = mapNumber + atoi(&str[1]);
		} else if (str[0] == '-') {
			dataChara = mapNumber - atoi(&str[1]);
		} else {
			dataChara = atoi( str );
		}

		if( (dataChara < 0) || (dataChara >= PARTS_NUMBER_MAX) ){
			MessageBox( g_hWnd, "出現パーツ指定の「番号」の数値範囲が規定値を超えています。\n 0 以上、パーツ上限数以下を指定してください。", "警告！", MB_OK );
		} else {
			if( flag == TRUE ){
				objectAttribute[mapNumber][20+i*4] = dataChara;
			} else {
				mapAttribute[mapNumber][20+i*4] = dataChara;
			}
		}
		
		//ＸＹ座標系
		GetDlgItemText( g_hDlgExtra, g_EditId[10+i], str, sizeof(str) );
		x = atoi( str );
		if( x >= g_iMapSize ){
			MessageBox( g_hWnd, "出現パーツ指定の「Ｘ座標」の数値範囲が規定値を超えています。\nマップサイズ以下で指定してください。", "警告！", MB_OK );
		} else if( (x < -100) || ((str[0] == '+') && (x > 100)) ){
			MessageBox( g_hWnd, "出現パーツ指定の「相対Ｘ座標」の数値範囲が規定値を超えています。\n -100 から +100 までで指定してください。", "警告！", MB_OK );
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
			MessageBox( g_hWnd, "出現パーツ指定の「Ｙ座標」の数値範囲が規定値を超えています。\nマップサイズ以下で指定してください。", "警告！", MB_OK );
		} else if( (y < -100) || ((str[0] == '+') && (y > 100)) ){
			MessageBox( g_hWnd, "出現パーツ指定の「相対Ｙ座標」の数値範囲が規定値を超えています。\n -100 から +100 までで指定してください。", "警告！", MB_OK );
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
// ダイアログプロシージャ

LRESULT CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_COMMAND: {
		if (LOWORD(wParam) == IDOK) {
			if (hWnd == g_hDlgFoundation) {
				int number;
				char str[30];
				//数値データ設定
				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_ENERGYMAX, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number > 10000)) MessageBox(g_hWnd, "「生命力上限」の数値範囲が規定値ではありません。\n 10000 以下を指定してください。", "警告！", MB_OK);
				else statusEnergyMax = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIENERGY, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number > 10000)) MessageBox(g_hWnd, "「生命力」の数値範囲が規定値ではありません。\n 10000 以下を指定してください。", "警告！", MB_OK);
				else statusEnergy = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INISTRENGTH, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number > 10000)) MessageBox(g_hWnd, "「攻撃力」の数値範囲が規定値ではありません。\n 10000 以下を指定してください。", "警告！", MB_OK);
				else statusStrength = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIDEFENCE, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number > 10000)) MessageBox(g_hWnd, "「防御力」の数値範囲が規定値ではありません。\n 10000 以下を指定してください。", "警告！", MB_OK);
				else statusDefence = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIGOLD, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number > 10000)) MessageBox(g_hWnd, "「所持金」の数値範囲が規定値ではありません。\n 10000 以下を指定してください。", "警告！", MB_OK);
				else statusGold = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIX, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number >= g_iMapSize)) MessageBox(g_hWnd, "「初期Ｘ座標」の数値範囲が規定値ではありません。\n マップサイズ以下を指定してください。", "警告！", MB_OK);
				else charaX = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_INIY, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number >= g_iMapSize)) MessageBox(g_hWnd, "「初期Ｙ座標」の数値範囲が規定値ではありません。\n マップサイズ以下を指定してください。", "警告！", MB_OK);
				else charaY = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_GVX, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number >= g_iMapSize)) MessageBox(g_hWnd, "「ゲームオーバーＸ座標」の数値範囲が規定値ではありません。\n マップサイズ以下を指定してください。", "警告！", MB_OK);
				else gameoverXp = number;

				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_GVY, str, sizeof(str));
				number = atoi(str);
				if ((number < 0) || (number >= g_iMapSize)) MessageBox(g_hWnd, "「ゲームオーバーＹ座標」の数値範囲が規定値ではありません。\n マップサイズ以下を指定してください。", "警告！", MB_OK);
				else gameoverYp = number;

				//テキストデータ挿入
				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_WORLDNAME, g_worldName, sizeof(g_worldName));
				//暗証番号
				GetDlgItemText(g_hDlgFoundation, IDC_EDIT_PASSWORD, g_worldPassword, sizeof(g_worldPassword));
				if ((atol(g_worldPassword) <= 0) && (g_worldPassword[0] != '\0')) {
					MessageBox(hWnd, "暗証番号には１以上の数字を入力してください。", "警告！", MB_OK);
					g_worldPassword[0] = '\0';
				}
				else if (atol(g_worldPassword) >= 1000000) {
					MessageBox(hWnd, "暗証番号は６桁までにしてください。", "警告！", MB_OK);
					g_worldPassword[0] = '\0';
				}
				else if (g_worldPassword[0] == '0') {
					MessageBox(hWnd, "暗証番号は数字として扱われますので、\n先頭に０は使えません。", "警告！", MB_OK);
					g_worldPassword[0] = '\0';
				}
				else if (atol(g_worldPassword) == 9999) {
					MessageBox(hWnd, "暗証番号は９９９９以外にしてください。", "警告！", MB_OK);
					g_worldPassword[0] = '\0';
				}
				else if ((atol(g_worldPassword) < 1000) && (atol(g_worldPassword) > 0)) {
					MessageBox(hWnd, "拡張クラスを使用しない場合や、ＣＧＩ によるステータス引継ぎをおこなわない場合は、\n保守のため暗証番号はなるべく４桁以上にしておくことをお勧めします。\n拡張クラスからの起動やステータス引継ぎができないようになります。", "推奨", MB_OK);
				}
				if (g_worldPassword[0] != '\0') ltoa(atol(g_worldPassword), g_worldPassword, 10);

				GetDlgItemText(g_hDlgFoundation, IDC_COMBO_BMP, g_mapcgNameBmp, sizeof(g_mapcgNameBmp));
				GetDlgItemText(g_hDlgFoundation, IDC_COMBO_GIF, g_mapcgName, sizeof(g_mapcgName));
				if (g_mapcgName[0] == '\0') {
					MessageBox(hWnd, "GIF画像ファイル名を入力していません。", "警告！", MB_OK);
					return 1;
				}
				DestroyWindow(hWnd);

				//ビットマップ読み込み
				if (g_bLoadGif == TRUE) {
					if (strcmp(g_mapcgName, g_mapcgOld) != 0) LoadBitmap();
				}
				else {
					if (strcmp(g_mapcgNameBmp, g_mapcgOld) != 0) LoadBitmap();
				}
				InvalidateRect(g_hWnd, NULL, FALSE);
				g_bUpdate = TRUE;
			}
			//戦闘結果の計算
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
				if (turn == 0) sprintf(str, "攻撃ターン数　--");
				else sprintf(str, "攻撃ターン数　%d", turn);
				SetDlgItemText(hWnd, IDC_STATIC_TURN, str);
				sprintf(str, "プレーヤー　%d", damagy);
				SetDlgItemText(hWnd, IDC_STATIC_RESULT1, str);
				sprintf(str, "モンスター　%d", damagyM);
				SetDlgItemText(hWnd, IDC_STATIC_RESULT2, str);
				sprintf(str, "プレーヤー　%d", attack);
				SetDlgItemText(hWnd, IDC_STATIC_RESULT3, str);
				sprintf(str, "モンスター　%d", attackM);
				SetDlgItemText(hWnd, IDC_STATIC_RESULT4, str);
			}
			return 1;
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			DestroyWindow(hWnd);
			return 1;
		}
		// マップサイズ拡張
		else if (LOWORD(wParam) == IDC_BUTTON_MAPSIZE) {
			char szStr[50];
			g_iMapSize += 50;
			if (g_iMapSize > MAP_SIZE_MAX) g_iMapSize = MAP_SIZE_MAX;
			sprintf(szStr, "%d×%d", g_iMapSize, g_iMapSize);
			SetDlgItemText(g_hDlgFoundation, IDC_EDIT_MAPSIZE, szStr);
			// スクロールバー設定
			SetScrollRange(g_hWnd, SB_VERT, 0, (g_iMapSize - SCREEN_CHIP_SIZE), FALSE);
			SetScrollRange(g_hWnd, SB_HORZ, 0, (g_iMapSize - SCREEN_CHIP_SIZE), FALSE);
		}
		// 背景パーツ最大数拡張
		else if (LOWORD(wParam) == IDC_BUTTON_MAP_PARTS) {
			g_iMapPartsMax += 50;
			if (g_iMapPartsMax > PARTS_NUMBER_MAX) g_iMapPartsMax = PARTS_NUMBER_MAX;
			SetScrollRange(g_hDlgSelectMap, SB_VERT, 0, ((g_iMapPartsMax / 10) - 3), FALSE);
			char szStr[50];
			sprintf(szStr, "%d", g_iMapPartsMax);
			SetDlgItemText(g_hDlgFoundation, IDC_EDIT_MAP_PARTS, szStr);
		}
		// 物体パーツ最大数拡張
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
// 基本メッセージダイアログプロシージャ
LRESULT CALLBACK DialogProcBasicMes(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG: {
		SetDlgItemText(hWnd, IDC_EDIT_BMES_LINK, g_StrMessage[5]);
		SetDlgItemText(hWnd, IDC_EDIT_BMES_GOLD, g_StrMessage[6]);
		SetDlgItemText(hWnd, IDC_EDIT_BMES_ITEM, g_StrMessage[7]);
		SetDlgItemText(hWnd, IDC_EDIT_BMES_USEITEM, g_StrMessage[8]);
		//拡張領域
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
			//拡張領域
			GetDlgItemText(hWnd, IDC_EDIT_BMES_CLICKBOX, g_StrMessageSystem[0], MESSAGE_STR_MAX);
			GetDlgItemText(hWnd, IDC_EDIT_BMES_ITEMFULL, g_StrMessageSystem[1], MESSAGE_STR_MAX);
			GetDlgItemText(hWnd, IDC_EDIT_BMES_SOUND, g_StrMessageSystem[2], MESSAGE_STR_MAX);
			//指定文字列修正
			int i;
			for (i = 5; i <= 9; ++i) {
				if ((strstr(g_StrMessage[i], "blank") != 0) || (strstr(g_StrMessage[i], "ＢＬＡＮＫ") != 0) || (strstr(g_StrMessage[i], "BLANK") != 0)) strcpy(g_StrMessage[i], "BLANK");
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
// パスワードダイアログプロシージャ
LRESULT CALLBACK DialogProcPassword(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG: {
		SetFocus(GetDlgItem(hWnd, IDC_EDIT_PASSWORD));
		break;
	}
	case WM_COMMAND: {
		if (LOWORD(wParam) == IDOK) {
			//パスワード入力
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
// 拡張出現キャラクタダイアログプロシージャ
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
				//物体、背景を切替
				dataMode = objectAttribute[chara][20 + i * 4 + 3];
				if (dataMode == 0) {
					SetDlgItemText(hWnd, Id[i], "背景");
					objectAttribute[chara][20 + i * 4 + 3] = 1;
				}
				else {
					SetDlgItemText(hWnd, Id[i], "物");
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
			//拡張出現キャラクタの設定
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
				//物体、背景を切替
				dataMode = mapAttribute[chara][20 + i * 4 + 3];
				if (dataMode == 0) {
					SetDlgItemText(hWnd, Id[i], "背景");
					mapAttribute[chara][20 + i * 4 + 3] = 1;
				}
				else {
					SetDlgItemText(hWnd, Id[i], "物");
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
			//拡張出現キャラクタの設定
			SetAppearChara(GetCharaNumber(hWnd), FALSE);
			return 1;
		}
		break;
	}
	}
	return 0;
}



//##------------------------------------------------------------------
// 文字データのセット
void SetMessageData(int* point, char* str)
{
	int i;
	int number = 0;

	//最大文字数の判定
	if (strlen(str) >= (MESSAGE_STR_MAX - 1 - 10)) {
		MessageBox(g_hWnd, "メッセージが最大文字数を超えています。\n1500バイトまでで入力してください。", "注意", MB_OK);
		str[MESSAGE_STR_MAX - 1 - 10] = '\0';
	}

	//メッセージナンバー設定
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
			MessageBox(g_hWnd, "メッセージが最大数を超えています。\n仕様上これ以上のメッセージは作成できません。", "注意", MB_OK);
			return;
		}
		*point = number;
		//メッセージ格納
		strcpy(g_StrMessage[*point], str);
	}
	else if (strlen(str) == 0) {
		g_StrMessage[*point][0] = '\0';
		*point = 0;
	}
	else {
		//メッセージ格納
		strcpy(g_StrMessage[*point], str);
	}
}


//##------------------------------------------------------------------
// マップの新規作成
void MakeNewMap()
{
	int i;

	//新規作成確認
	int result;
	result = MessageBox(g_hWnd, "マップの新規作成をします。\n現在、編集中のデータは失われますが、よろしいですか？", "マップの新規作成", MB_OK | MB_OKCANCEL);
	//データ取り消し
	if (result == IDCANCEL) return;

	//ダイアログ閉じる
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

	//初期化
	ZeroMemory(&mapAttribute, sizeof(mapAttribute));
	ZeroMemory(&objectAttribute, sizeof(objectAttribute));
	//メッセージデータ
	for (i = 0; i < MESSAGE_NUMBER_MAX; ++i) g_StrMessage[i][0] = '\0';

	//その他データ
	g_worldName[0] = '\0';
	g_worldPassword[0] = '\0';
	g_mapcgNameBmp[0] = '\0';
	g_mapcgName[0] = '\0';
	g_mapcgOld[0] = '\0';
	strcpy(g_szSelectFile, "newmap.dat");
	//タイトルテキスト設定
	char sTitle[100];
	sprintf(sTitle, "%s [%s]", g_szTitleName, g_szSelectFile);
	SetWindowText(g_hWnd, sTitle);

	//基本設定の編集
	EditMapFoundation();

	//画面再描画
	InvalidateRect(g_hWnd, NULL, FALSE);
	InvalidateRect(g_hDlgSelectObject, NULL, FALSE);
	InvalidateRect(g_hDlgSelectMap, NULL, FALSE);
	InvalidateRect(g_hDlgQuickView, NULL, FALSE);

	MessageBox(g_hWnd, "マップを新規作成しました。\n画面には何も表示されなくなりますがこれで正常です。\n使用するＧＩＦ画像ファイルを選択後、\n新たにパーツを作成してマップに配置していってください。", "マップの新規作成", MB_OK);
}



//##------------------------------------------------------------------
// 基本設定ダイアログ
void EditMapFoundation()
{
	char str[100];
	DestroyWindow(g_hDlgFoundation);
	g_hDlgFoundation = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_FOUNDATION), g_hWnd, (DLGPROC)DialogProc);

	//数値データ挿入
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

	//テキストデータ挿入
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_WORLDNAME, g_worldName);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_PASSWORD, g_worldPassword);

	//コンボボックスにデータ挿入
	SetDlgItemText(g_hDlgFoundation, IDC_COMBO_BMP, g_mapcgNameBmp);
	SetDlgItemText(g_hDlgFoundation, IDC_COMBO_BMP, g_mapcgNameBmp);
	if (g_bLoadGif == TRUE) {
		ShowWindow(GetDlgItem(g_hDlgFoundation, IDC_COMBO_BMP), FALSE);
		ShowWindow(GetDlgItem(g_hDlgFoundation, IDC_STATIC_BMP), FALSE);
	}
	SetDlgItemText(g_hDlgFoundation, IDC_COMBO_GIF, g_mapcgName);
	SetDlgItemText(g_hDlgFoundation, IDC_COMBO_GIF, g_mapcgName);

	//各種サイズデータ挿入
	sprintf(str, "%d×%d", g_iMapSize, g_iMapSize);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_MAPSIZE, str);
	sprintf(str, "%d", g_iMapPartsMax);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_MAP_PARTS, str);
	sprintf(str, "%d", g_iObjectPartsMax);
	SetDlgItemText(g_hDlgFoundation, IDC_EDIT_OBJ_PARTS, str);
}



//##------------------------------------------------------------------
// 取り消し用、データの一時保存

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
// 計算ダイアログの表示

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
// キャラクタ番号の取得

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
// 出力するHTMLファイル名を取得

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
// ファイルの作成とブラウザの起動

BOOL ExecBrowser()
{
	// HTML作成
	CString MapDataFileName = g_szSelectFile;
	MapDataFileName.Replace("<", "&lt;");
	MapDataFileName.Replace(">", "&gt;");

	CString MapWorldName;
	// ワールド名の作成
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

	// ファイル名作成
	CString HtmlFileName;
	HtmlFileName = GetHtmlFileName();

	// データの書き込み
	HANDLE hFile;
	DWORD dwWritten;

	hFile = CreateFile(HtmlFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		CString ErrorMessage = "「" + HtmlFileName + "」ファイルが作成または書き込みできません。";
		MessageBox(g_hWnd, ErrorMessage, "注意", MB_OK);

		CloseHandle(hFile);
		return FALSE;
	}

	WriteFile(hFile, Html, Html.GetLength(), &dwWritten, NULL);
	CloseHandle(hFile);

	// ブラウザの起動
	CString Message = "「" + HtmlFileName + "」ファイルを出力しました。\nHTMLファイルをブラウザで見ますか？\n(デバッグツールが必要になる場合があります)";
	if (MessageBox(g_hWnd, Message, "作成完了", MB_YESNO) == IDYES) {
		if ((int)ShellExecute(g_hWnd, "open", HtmlFileName, NULL, NULL, SW_SHOWNORMAL) <= 32) {
			MessageBox(g_hWnd, "ブラウザ起動エラー\nブラウザソフトウェアがインストールされているか確認してください。", "起動失敗", MB_OK);
			return FALSE;
		}
	}
	return TRUE;
}



//##------------------------------------------------------------------
// 指定キーが押されていた場合はTRUEを返す

BOOL IsKeyDown( int virtKeyCode )
{
	if ( GetAsyncKeyState(virtKeyCode) & 0x8000 ) return TRUE;
	return FALSE;
}



//##------------------------------------------------------------------
// Undo機能

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
// GIF画像の読み込み

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

	//初期化
	g_iColorTp = 0;
	g_bFileNotFound = FALSE;

	//画像ファイル読み込み
	if ((han = CreateFile(g_mapcgName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		sprintf(szStr, "GIF画像ファイル「%s」がオープンできません。\nファイルが存在するか、他のアプリケーションにより使用されていないかを確認してください。", g_mapcgName);
		MessageBox(g_hWnd, szStr, "注意", MB_OK);
		g_bFileNotFound = TRUE;
		return FALSE;
	}
	siz = GetFileSize(han, NULL);
	hgb = GlobalAlloc(GPTR, siz);
	ReadFile(han, hgb, siz, &dw, NULL);
	CloseHandle(han);
	//ストリームを作成
	CreateStreamOnHGlobal(hgb, TRUE, &ist);
	//イメージをロード
	OleLoadPicture(ist, siz, TRUE, IID_IPicture, (LPVOID*)& ipi);

	ipi->get_Width(&cx);
	ipi->get_Height(&cy);
	if ((cx == 0) || (cy == 0)) return FALSE;

	cxPerInch = GetDeviceCaps(g_hmDC, LOGPIXELSX);
	cyPerInch = GetDeviceCaps(g_hmDC, LOGPIXELSY);
	dx = MulDiv(cx, cxPerInch, HIMETRIC_INCH);
	dy = MulDiv(cy, cyPerInch, HIMETRIC_INCH);

	//画像丈設定
	g_iLoadCGHeight = dy;
	if ((g_iLoadCGHeight / CHIP_SIZE) > 17) {
		g_ScrCGCharaMax = (g_iLoadCGHeight / CHIP_SIZE) - 17;
	}
	else {
		g_ScrCGCharaMax = 0;
	}
	// クライアント領域確保
	HDC hDC = GetDC(g_hWnd);
	if (g_hBitmap != NULL) DeleteObject(g_hBitmap);
	g_hBitmap = CreateCompatibleBitmap(hDC, CHIP_SIZE * 10, g_iLoadCGHeight);
	SelectObject(g_hmDC, g_hBitmap);

	// 透過色で塗り潰し
	g_iColorTp = 0x9E9E9E;
	HBRUSH hbr = CreateSolidBrush(g_iColorTp);
	RECT rcRect;
	rcRect.left = 0;
	rcRect.top = 0;
	rcRect.right = CHIP_SIZE * 10;
	rcRect.bottom = g_iLoadCGHeight;
	FillRect(g_hmDC, &rcRect, hbr);
	// イメージ描画
	ipi->Render(g_hmDC, 0, 0, dx, dy, 0, cy, cx, -cy, NULL);

	// デバイス解放
	ReleaseDC(g_hWnd, hDC);
	ipi->Release();
	ist->Release();

	return TRUE;
}
