#include "framework.h"
#include "CASTLE DEFEND.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "ErrH.h"
#include "D2BMPLOADER.h"
#include "FCheck.h"
#include "gifresizer.h"
#include "winter.h"
#include "chrono"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "gifresizer.lib")
#pragma comment(lib, "winter.lib")

constexpr wchar_t bWinClassName[]{ L"MyTowerDefence2" };

constexpr char temp_file[]{ ".\\res\\data\\temp.dat" };
constexpr wchar_t Ltemp_file[]{ L".\\res\\data\\temp.dat" };
constexpr wchar_t snd_file[]{ L".\\res\\snd\\main.wav" };
constexpr wchar_t help_file[]{ L".\\res\\data\\help.dat" };
constexpr wchar_t record_file[]{ L".\\res\\data\\record.dat" };
constexpr wchar_t save_file[]{ L".\\res\\data\\save.dat" };

constexpr int mNew{ 1001 };
constexpr int mLvl{ 1002 };
constexpr int mExit{ 1003 };
constexpr int mSave{ 1004 };
constexpr int mLoad{ 1005 };
constexpr int mHoF{ 1006 };

constexpr int record{ 2001 };
constexpr int first_record{ 2002 };
constexpr int no_record{ 2003 };
///////////////////////////////////////////////////

WNDCLASS bWinClass{};
HINSTANCE bIns{ nullptr };
HWND bHwnd{ nullptr };
HCURSOR mainCur{ nullptr };
HCURSOR outCur{ nullptr };
HCURSOR upgradeCur{ nullptr };
HICON Icon{ nullptr };
HMENU bBar{ nullptr };
HMENU bMain{ nullptr };
HMENU bStore{ nullptr };
HDC PaintDC{ nullptr };
PAINTSTRUCT bPaint{};

MSG bMsg{};
BOOL bRet{ 0 };

UINT bTimer = 0;

POINT cur_pos{};

D2D1_RECT_F b1Rect{ 0, 0, scr_width / 3 - 20.0f, 50.0f };
D2D1_RECT_F b2Rect{ scr_width / 3, 0, scr_width * 2 / 3 - 20.0f, 50.0f };
D2D1_RECT_F b3Rect{ scr_width * 2 / 3, 0, scr_width - 20.0f, 50.0f };

/////////////////////////////////////////////////

bool pause = false;
bool show_help = false;
bool sound = true;
bool in_client = true;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;
bool name_set = false;

bool build_selected = false;
bool upgrade_selected = false;

wchar_t current_player[16]{ L"ONE TARLYO" };

int score = 0;
int level = 1;
int secs = 300;
int mins = 0;
int gold = 500;
int castle_lifes = 500;

int field_frame = 0;

/////////////////////////////////////

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };
ID2D1RadialGradientBrush* bckgBrush{ nullptr };
ID2D1SolidColorBrush* txtBrush{ nullptr };
ID2D1SolidColorBrush* hgltBrush{ nullptr };
ID2D1SolidColorBrush* inactBrush{ nullptr };
ID2D1SolidColorBrush* lifeBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmTextFormat{ nullptr };
IDWriteTextFormat* midTextFormat{ nullptr };
IDWriteTextFormat* bigTextFormat{ nullptr };

ID2D1Bitmap* bmpBase{ nullptr };
ID2D1Bitmap* bmpTurret1{ nullptr };
ID2D1Bitmap* bmpTurret2{ nullptr };
ID2D1Bitmap* bmpTurret3{ nullptr };
ID2D1Bitmap* bmpTurret4{ nullptr };
ID2D1Bitmap* bmpTurret5{ nullptr };
ID2D1Bitmap* bmpTurret6{ nullptr };

ID2D1Bitmap* bmpIntro[50]{ nullptr };
ID2D1Bitmap* bmpField[45]{ nullptr };
ID2D1Bitmap* bmpShot[33]{ nullptr };

ID2D1Bitmap* bmpEvil1[8]{ nullptr };
ID2D1Bitmap* bmpEvil2[13]{ nullptr };
ID2D1Bitmap* bmpEvil3[36]{ nullptr };
ID2D1Bitmap* bmpEvil4[14]{ nullptr };
ID2D1Bitmap* bmpEvil5[16]{ nullptr };

/////////////////////////////////////

game::RANDERER RandGenerator{};

game::SIMPLE* Castle{ nullptr };

std::vector<game::evil_ptr>vEvils;
std::vector<game::turret_ptr>vTurrets;
std::vector<game::SHOTS*>vShots;

/////////////////////////////////////

template<typename T>concept HasRelease = requires(T check_var)
{
    check_var.Release();
};
template<HasRelease T>bool FreeHeap(T** clear)
{
    if (*clear)
    {
        (*clear)->Release();
        (*clear) = nullptr;
        return true;
    }
    return false;
}
void LogError(LPCWSTR what)
{
    std::wofstream err(L".\\res\\data\\error.log", std::ios::app);
    err << what << L" Time stamp: " << std::chrono::system_clock::now() << std::endl;
    err.close();
}
void FreeResources()
{
    if (!FreeHeap(&iFactory))LogError(L"Error releasing iFactory !");
    if (!FreeHeap(&Draw))LogError(L"Error releasing Draw !");
    if (!FreeHeap(&bckgBrush))LogError(L"Error releasing bckgBrush !");
    if (!FreeHeap(&txtBrush))LogError(L"Error releasing txtBrush !");
    if (!FreeHeap(&hgltBrush))LogError(L"Error releasing hgltBrush !");
    if (!FreeHeap(&inactBrush))LogError(L"Error releasing inactBrush !");
    if (!FreeHeap(&lifeBrush))LogError(L"Error releasing lifeBrush !");

    if (!FreeHeap(&iWriteFactory))LogError(L"Error releasing iWriteFactory !");
    if (!FreeHeap(&nrmTextFormat))LogError(L"Error releasing nrmTextFormat !");
    if (!FreeHeap(&midTextFormat))LogError(L"Error releasing midTextFormat !");
    if (!FreeHeap(&bigTextFormat))LogError(L"Error releasing bigTextFormat !");

    if (!FreeHeap(&bmpBase))LogError(L"Error releasing bmpBase !");
    if (!FreeHeap(&bmpTurret1))LogError(L"Error releasing bmpTurret1 !");
    if (!FreeHeap(&bmpTurret2))LogError(L"Error releasing bmpTurret2 !");
    if (!FreeHeap(&bmpTurret3))LogError(L"Error releasing bmpTurret3 !");
    if (!FreeHeap(&bmpTurret4))LogError(L"Error releasing bmpTurret4 !");
    if (!FreeHeap(&bmpTurret5))LogError(L"Error releasing bmpTurret5 !");
    if (!FreeHeap(&bmpTurret6))LogError(L"Error releasing bmpTurret6 !");

    for (int i = 0; i < 50; i++)if (!FreeHeap(&bmpIntro[i]))LogError(L"Error releasing bmpIntro !");
    for (int i = 0; i < 45; i++)if (!FreeHeap(&bmpField[i]))LogError(L"Error releasing bmpField !");
    for (int i = 0; i < 33; i++)if (!FreeHeap(&bmpShot[i]))LogError(L"Error releasing bmpShot !");

    for (int i = 0; i < 8; i++)if (!FreeHeap(&bmpEvil1[i]))LogError(L"Error releasing bmpEvil1 !");
    for (int i = 0; i < 13; i++)if (!FreeHeap(&bmpEvil2[i]))LogError(L"Error releasing bmpEvil2 !");
    for (int i = 0; i < 36; i++)if (!FreeHeap(&bmpEvil3[i]))LogError(L"Error releasing bmpEvil3 !");
    for (int i = 0; i < 14; i++)if (!FreeHeap(&bmpEvil4[i]))LogError(L"Error releasing bmpEvil4 !");
    for (int i = 0; i < 16; i++)if (!FreeHeap(&bmpEvil5[i]))LogError(L"Error releasing bmpEvil5 !");
}
void ErrExit(int what)
{
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    std::remove(temp_file);
    FreeResources();
    exit(1);
}
void InitGame()
{
    level = 1;
    secs = 300 + level * 10;
    score = 0;
    wcscpy_s(current_player, L"ONE TARLYO");
    name_set = false;
    build_selected = false;
    upgrade_selected = false;

    castle_lifes = 500;
    gold = 500;

    if (Castle)delete Castle;
    Castle = new game::SIMPLE(5.0f, (float)(RandGenerator(60, (int)(ground) - 120)), 100.0f, 115.0f);

    if (!vEvils.empty())
        for (int i = 0; i < vEvils.size(); ++i)FreeHeap(&vEvils[i]);
    vEvils.clear();
    if (!vTurrets.empty())
        for (int i = 0; i < vTurrets.size(); ++i)FreeHeap(&vTurrets[i]);
    vTurrets.clear();
    if (!vShots.empty())
        for (int i = 0; i < vShots.size(); ++i)FreeHeap(&vShots[i]);
    vShots.clear();

}

void GameOver()
{
    PlaySound(NULL, NULL, NULL);
    KillTimer(bHwnd, bTimer);


    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(Icon));
        return true;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
            if (GetDlgItemText(hwnd, IDC_NAME, current_player, 16) < 1)
            {
                wcscpy_s(current_player, L"One Tarlyo");
                if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
                MessageBox(bHwnd, L"Името си ли забрави ?", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            EndDialog(hwnd, IDOK);
        }
        break;
    }

    return (INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        SetTimer(hwnd, bTimer, 1000, NULL);
        bBar = CreateMenu();
        bMain = CreateMenu();
        bStore = CreateMenu();
        
        AppendMenuW(bBar, MF_POPUP, (UINT_PTR)bMain, L"Основно меню");
        AppendMenuW(bBar, MF_POPUP, (UINT_PTR)bStore, L"Меню за данни");
        
        AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
        AppendMenu(bMain, MF_STRING, mLvl, L"Следващо ниво");
        AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bMain, MF_STRING, mExit, L"Изход");

        AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
        AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
        AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");
        SetMenu(hwnd, bBar);
        InitGame();
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
        if (MessageBox(hwnd, L"Ако излезеш ще изгубиш тази игра !\n\nНаистина ли излизаш ?",
            L"Изход !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(100, 100, 100)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }
            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1Rect.left && cur_pos.x <= b1Rect.right)
                {
                    if (!b1Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                        b2Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b2Rect.left && cur_pos.x <= b2Rect.right)
                {
                    if (!b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b2Hglt = true;
                        b1Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b3Rect.left && cur_pos.x <= b3Rect.right)
                {
                    if (!b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b3Hglt = true;
                        b2Hglt = false;
                        b1Hglt = false;
                    }
                }

                SetCursor(outCur);
                return true;
            }
            else
            {
                if (b1Hglt || b2Hglt || b3Hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b3Hglt = false;
                    b2Hglt = false;
                    b1Hglt = false;
                }
            }
            if (upgrade_selected)SetCursor(upgradeCur);
            else SetCursor(mainCur);
            return true;
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }
            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b3Hglt = false;
                b2Hglt = false;
                b1Hglt = false;
            }
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_TIMER:
        if (pause)break;
        ++secs;
        mins = secs / 60;
        if (castle_lifes + 5 <= 500)castle_lifes += 5;
        if (secs % 5 == 0)gold += 5;
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
            if (MessageBox(hwnd, L"Ако рестартираш ще изгубиш тази игра !\n\nНаистина ли рестартираш ?",
                L"Рестарт !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;

        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;

        }
        break;

    case WM_LBUTTONDOWN:
        if (!build_selected)
        {
            if (gold < 100)
            {
                if (sound)mciSendString(L"play.\\res\\negative.wav", NULL, NULL, NULL);
                break;
            }
            else build_selected = true;
        }
        else
        {
            game::SIMPLE Dummy((float)(LOWORD(lParam)), (float)(HIWORD(lParam)), 55.0f, 56.0f);
            bool is_ok = true;
            if (!vTurrets.empty())
            {
                for (std::vector<game::turret_ptr>::iterator it = vTurrets.begin(); it < vTurrets.end(); it++)
                {
                    if (!(Dummy.x >= (*it)->ex || Dummy.ex <= (*it)->x || Dummy.y >= (*it)->ey || Dummy.ey <= (*it)->y))
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                        is_ok = false;
                        break;
                    }
                }
            }

            if (is_ok)
            {
                gold -= 100;
                if (sound)mciSendString(L"play .\\res\\snd\\build.wav", NULL, NULL, NULL);
                vTurrets.push_back(game::TurretFactory(turret1_flag, Dummy.x, Dummy.y));
            }
        }
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            build_selected = false;
            upgrade_selected = false;
        }
        break;

    default:return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

void CreateResources()
{
    int result = 0;
    CheckFile(Ltemp_file, &result);
    if (result == FILE_EXIST)ErrExit(eStarted);
    else
    {
        std::wofstream started(Ltemp_file);
        started << L"Game started at: " << std::chrono::system_clock::now();
        started.close();
    }
    
    int win_x = (int)(GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2));
    if (GetSystemMetrics(SM_CXSCREEN) < win_x + (int)(scr_width) || GetSystemMetrics(SM_CYSCREEN) < 50 + (int)(scr_width))
        ErrExit(eScreen);
    
    Icon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 255, 255, LR_LOADFROMFILE));
    if (!Icon)ErrExit(eIcon);
    mainCur = LoadCursorFromFile(L".\\res\\main.ani");
    outCur = LoadCursorFromFile(L".\\res\\out.ani");
    upgradeCur = LoadCursorFromFile(L".\\res\\upgrade.ani");
    if (!mainCur || !outCur)ErrExit(eCursor);

    bWinClass.lpszClassName = bWinClassName;
    bWinClass.hInstance = bIns;
    bWinClass.lpfnWndProc = &WinProc;
    bWinClass.hbrBackground = CreateSolidBrush(RGB(100, 100, 100));
    bWinClass.hIcon = Icon;
    bWinClass.hCursor = mainCur;
    bWinClass.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWinClass))ErrExit(eClass);

    bHwnd = CreateWindowW(bWinClassName, L"ПОСЛЕДЕН ПОСТ !", WS_CAPTION | WS_SYSMENU, win_x, 50, (int)(scr_width),
        (int)(scr_height), NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else
    {
        ShowWindow(bHwnd, SW_SHOWDEFAULT);

        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
        if (hr != S_OK)
        {
            LogError(L"Error creating iFactory !");
            ErrExit(eD2D);
        }

        if (iFactory)
        {
            hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
                D2D1::HwndRenderTargetProperties(bHwnd, D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
            if (hr != S_OK)
            {
                LogError(L"Error creating Draw !");
                ErrExit(eD2D);
            }

            if (Draw)
            {
                D2D1_GRADIENT_STOP gStops[2]{};
                ID2D1GradientStopCollection* Collection{ nullptr };

                gStops[0].position = 0;
                gStops[0].color = D2D1::ColorF(D2D1::ColorF::Gainsboro);
                gStops[1].position = 1.0f;
                gStops[1].color = D2D1::ColorF(D2D1::ColorF::Maroon);

                hr = Draw->CreateGradientStopCollection(gStops, 2, &Collection);
                if (hr != S_OK)
                {
                    LogError(L"Error creating GradientStopCollection !");
                    ErrExit(eD2D);
                }
                if (Collection)
                {
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(scr_width / 2, 25.0f),
                        D2D1::Point2F(0, 0), scr_width / 2, 25.0f), Collection, &bckgBrush);
                    if (hr != S_OK)
                    {
                        LogError(L"Error creating RadialGradientBrush !");
                        ErrExit(eD2D);
                    }
                    FreeHeap(&Collection);
                }

                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue),&txtBrush);
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &hgltBrush);
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkCyan), &inactBrush);
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkKhaki), &lifeBrush);
                if (hr != S_OK)
                {
                    LogError(L"Error creating Text Brushes !");
                    ErrExit(eD2D);
                }

                bmpBase = Load(L".\\res\\img\\base.png", Draw);
                if (!bmpBase)
                {
                    LogError(L"Error loading bmpBase !");
                    ErrExit(eD2D);
                }

                bmpTurret1 = Load(L".\\res\\img\\turrets\\turret1.png", Draw);
                if (!bmpTurret1)
                {
                    LogError(L"Error loading bmpTurret1 !");
                    ErrExit(eD2D);
                }
                bmpTurret2 = Load(L".\\res\\img\\turrets\\turret2.png", Draw);
                if (!bmpTurret2)
                {
                    LogError(L"Error loading bmpTurret2 !");
                    ErrExit(eD2D);
                }
                bmpTurret3 = Load(L".\\res\\img\\turrets\\turret3.png", Draw);
                if (!bmpTurret3)
                {
                    LogError(L"Error loading bmpTurret3 !");
                    ErrExit(eD2D);
                }
                bmpTurret4 = Load(L".\\res\\img\\turrets\\turret4.png", Draw);
                if (!bmpTurret4)
                {
                    LogError(L"Error loading bmpTurret4 !");
                    ErrExit(eD2D);
                }
                bmpTurret5 = Load(L".\\res\\img\\turrets\\turret5.png", Draw);
                if (!bmpTurret5)
                {
                    LogError(L"Error loading bmpTurret5 !");
                    ErrExit(eD2D);
                }
                bmpTurret6 = Load(L".\\res\\img\\turrets\\turret6.png", Draw);
                if (!bmpTurret6)
                {
                    LogError(L"Error loading bmpTurret6");
                    ErrExit(eD2D);
                }
                
                for (int i = 0; i < 50; i++)
                {
                    wchar_t name[100] = L".\\res\\img\\intro\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");
                    bmpIntro[i] = Load(name, Draw);
                    if (!bmpIntro[i])
                    {
                        LogError(L"Error loading bmpIntro");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 45; i++)
                {
                    wchar_t name[100] = L".\\res\\img\\field\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");
                    bmpField[i] = Load(name, Draw);
                    if (!bmpField[i])
                    {
                        LogError(L"Error loading bmpField");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 33; i++)
                {
                    wchar_t name[100] = L".\\res\\img\\shot\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");
                    bmpShot[i] = Load(name, Draw);
                    if (!bmpShot[i])
                    {
                        LogError(L"Error loading bmpShot");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 8; i++)
                {
                    wchar_t name[100] = L".\\res\\img\\evil1\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");
                    bmpEvil1[i] = Load(name, Draw);
                    if (!bmpEvil1[i])
                    {
                        LogError(L"Error loading bmpEvil1");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 13; i++)
                {
                    wchar_t name[100] = L".\\res\\img\\evil2\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");
                    bmpEvil2[i] = Load(name, Draw);
                    if (!bmpEvil2[i])
                    {
                        LogError(L"Error loading bmpEvil2");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 36; i++)
                {
                    wchar_t name[100] = L".\\res\\img\\evil3\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");
                    bmpEvil3[i] = Load(name, Draw);
                    if (!bmpEvil3[i])
                    {
                        LogError(L"Error loading bmpEvil3");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 14; i++)
                {
                    wchar_t name[100] = L".\\res\\img\\evil4\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");
                    bmpEvil4[i] = Load(name, Draw);
                    if (!bmpEvil4[i])
                    {
                        LogError(L"Error loading bmpEvil4");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 16; i++)
                {
                    wchar_t name[100] = L".\\res\\img\\evil5\\";
                    wchar_t add[5] = L"\0";
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");
                    bmpEvil5[i] = Load(name, Draw);
                    if (!bmpEvil5[i])
                    {
                        LogError(L"Error loading bmpEvil5");
                        ErrExit(eD2D);
                    }
                }
            }
        }

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), 
            reinterpret_cast<IUnknown**>(&iWriteFactory));
        if (hr != S_OK)
        {
            LogError(L"Error creating iWriteFactory !");
            ErrExit(eD2D);
        }

        if (iWriteFactory)
        {
            hr = iWriteFactory->CreateTextFormat(L"Segoe Print", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK,
                DWRITE_FONT_STYLE_OBLIQUE, DWRITE_FONT_STRETCH_NORMAL, 18, L"", &nrmTextFormat);
            hr = iWriteFactory->CreateTextFormat(L"Segoe Print", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK,
                DWRITE_FONT_STYLE_OBLIQUE, DWRITE_FONT_STRETCH_NORMAL, 36, L"", &midTextFormat);
            hr = iWriteFactory->CreateTextFormat(L"Segoe Print", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK,
                DWRITE_FONT_STYLE_OBLIQUE, DWRITE_FONT_STRETCH_NORMAL, 72, L"", &bigTextFormat);

            if (hr != S_OK)
            {
                LogError(L"Error creating TextFormat object !");
                ErrExit(eD2D);
            }
        }
    }

    int intro_frame = 0;

    if (Draw && bigTextFormat && txtBrush)
    {
        for (int i = 0; i < 50; i++)
        {
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[intro_frame], D2D1::RectF(0, 0, scr_width, scr_height));
            ++intro_frame;
            if (intro_frame > 49)intro_frame = 0;
            if (RandGenerator(0, 3) == 2)
            {
                Draw->DrawTextW(L"ПОСЛЕДЕН ПОСТ !\n\ndev. Daniel", 29, bigTextFormat, D2D1::RectF(80.0f, 80.0f, scr_width,
                    scr_height), txtBrush);
                mciSendString(L"play .\\res\\snd\\buzz.wav", NULL, NULL, NULL);
            }
            Draw->EndDraw();
            Sleep(50);
        }
        Draw->BeginDraw();
        Draw->DrawBitmap(bmpIntro[intro_frame], D2D1::RectF(0, 0, scr_width, scr_height));
        Draw->DrawTextW(L"ПОСЛЕДЕН ПОСТ !\n\ndev. Daniel", 29, bigTextFormat, D2D1::RectF(80.0f, 80.0f, scr_width,
            scr_height), txtBrush);
        mciSendString(L"play .\\res\\snd\\buzz.wav", NULL, NULL, NULL);
        Draw->EndDraw();
        PlaySound(L".\\res\\snd\\boom.wav", NULL, SND_SYNC);
    }
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)
    {
        LogError(L"Error obtaining hInstance");
        ErrExit(eClass);
    }
    CreateResources();

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessageW(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkCyan));
            if (bigTextFormat && txtBrush)
                Draw->DrawTextW(L"ПАУЗА", 6, bigTextFormat, D2D1::RectF(scr_width / 2 - 100.0f, scr_height / 2 - 50.0f, scr_width,
                    scr_height), txtBrush);
            Draw->EndDraw();
            continue;
        }
        ////////////////////////////////

        //TURRETS ENGINE ***************

        if (!vTurrets.empty())
        {
            for (int i = 0; i < vTurrets.size(); i++)
            {
                int attack_strenght = vTurrets[i]->Attack();
                if (!vEvils.empty() && attack_strenght > 0)
                {
                    game::SIMPLE_PACK TargetPack(vEvils.size());
                    for (int i = 0; i < vEvils.size(); i++)
                    {
                        game::SIMPLE OneTarget(vEvils[i]->x, vEvils[i]->y, vEvils[i]->GetWidth(), vEvils[i]->GetHeight());
                        TargetPack.push_back(OneTarget);
                    }
                    GAME_COORD target_coord = vTurrets[i]->SortPack(TargetPack);
                    game::SHOTS* OneShot = new game::SHOTS(vTurrets[i]->x, vTurrets[i]->y, target_coord.x, target_coord.y,
                        attack_strenght * level);
                    vShots.push_back(OneShot);
                }
            }
        }
        if (!vShots.empty())
        {
            for (std::vector<game::SHOTS*>::iterator shot = vShots.begin(); shot < vShots.end(); ++shot)
            {
                unsigned char bound_reached = (*shot)->Move((float)(level));
                if (bound_reached == left_flag || bound_reached == right_flag
                    || bound_reached == up_flag || bound_reached == down_flag || bound_reached == end_flag)
                {
                    vShots.erase(shot);
                    break;
                }
                
            }
        }

        if (!vShots.empty() && !vEvils.empty())
        {
            for (std::vector<game::evil_ptr>::iterator evil = vEvils.begin(); evil < vEvils.end(); ++evil)
            {
                bool killed = false;
                for (std::vector<game::SHOTS*>::iterator shot = vShots.begin(); shot < vShots.end(); ++shot)
                {
                    if (!((*shot)->x >= (*evil)->ex || (*shot)->ex <= (*evil)->x
                        || (*shot)->y >= (*evil)->ey || (*shot)->ey <= (*evil)->y))
                    {
                        (*evil)->lifes -= (*shot)->strenght;
                        (*shot)->Release();
                        vShots.erase(shot);
                        if ((*evil)->lifes <= 0)
                        {
                            (*evil)->Release();
                            vEvils.erase(evil);
                            score += 10 * level;
                            killed = true;
                        }
                        break;
                    }
                }
                if (killed)break;
            }
        }

        //EVILS ENGINE ****************

        if (vEvils.size() < 40 + level)
        {
            switch (RandGenerator(0, 50))
            {
            case 0:
                vEvils.push_back(game::EvilFactory(evil1_flag, scr_width, (float)(RandGenerator(50, (int)(ground - 100)))));
                break;

            case 1:
            {
                int choice = RandGenerator(0, 20) - level;
                if (choice < 0)choice = abs(choice);
                if (choice==6)
                    vEvils.push_back(game::EvilFactory(evil2_flag, scr_width, (float)(RandGenerator(50, (int)(ground - 100)))));
            }
                break;

            case 2:
            {
                int choice = RandGenerator(0, 30) - level;
                if (choice < 0)choice = abs(choice);
                if (choice == 6)
                    vEvils.push_back(game::EvilFactory(evil3_flag, scr_width, (float)(RandGenerator(50, (int)(ground - 100)))));
            }
            break;

            case 3:
            {
                int choice = RandGenerator(0, 40) - level;
                if (choice < 0)choice = abs(choice);
                if (choice == 6)
                    vEvils.push_back(game::EvilFactory(evil4_flag, scr_width, (float)(RandGenerator(50, (int)(ground - 100)))));
            }
            break;

            case 4:
            {
                int choice = RandGenerator(0, 50) - level;
                if (choice < 0)choice = abs(choice);
                if (choice == 6)
                    vEvils.push_back(game::EvilFactory(evil5_flag, scr_width, (float)(RandGenerator(50, (int)(ground - 100)))));
            }
            break;

            }
        }
        if (!vEvils.empty())
        {
            for (std::vector<game::evil_ptr>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                int container_size = (int)(vTurrets.size()) + 1;
                game::SIMPLE_PACK EnemyPack(container_size);

                if (Castle)
                {
                    game::SIMPLE aCastle(Castle->x, Castle->y, Castle->GetWidth(), Castle->GetHeight());
                    EnemyPack.push_back(aCastle);
                }
                if (!vTurrets.empty())
                {
                    for (std::vector<game::turret_ptr>::iterator turret = vTurrets.begin(); turret < vTurrets.end(); turret++)
                    {
                        game::SIMPLE aTurret((*turret)->x, (*turret)->y, (*turret)->GetWidth(), (*turret)->GetHeight());
                        EnemyPack.push_back(aTurret);
                    }
                }

                (*evil)->Move((float)(level), EnemyPack);
            }
        }
        if (!vEvils.empty() && !vTurrets.empty())
        {
            for (std::vector<game::evil_ptr>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                bool killed = false;
                for (std::vector<game::turret_ptr>::iterator turret = vTurrets.begin(); turret < vTurrets.end(); turret++)
                {
                    if (!((*turret)->x >= (*evil)->ex || (*turret)->ex <= (*evil)->x
                        || (*turret)->y >= (*evil)->ey || (*turret)->ey <= (*evil)->y))
                    {
                        (*turret)->lifes -= (*evil)->Attack();
                        if ((*turret)->lifes <= 0);
                        {
                            (*turret)->Release();
                            vTurrets.erase(turret);
                            killed = true;
                            break;
                        }
                    }

                }
                if (killed)break;
            }
        }
        if (!vEvils.empty() && Castle)
        {
            for (std::vector<game::evil_ptr>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                if (!(Castle->x >= (*evil)->ex || Castle->ex <= (*evil)->x
                    || Castle->y >= (*evil)->ey || Castle->ey <= (*evil)->y))GameOver();
            }
        }

        /////////////////////////////////

        //DRAW THINGS ******************

        Draw->BeginDraw();
        Draw->DrawBitmap(bmpField[field_frame], D2D1::RectF(0, 50, scr_width, scr_height));
        ++field_frame;
        if (field_frame >= 45)field_frame = 0;
        if (nrmTextFormat && txtBrush && hgltBrush && inactBrush && bckgBrush)
        {
            Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), bckgBrush);
            if (name_set)
                Draw->DrawTextW(L"ИМЕ НА КОМАНДИР", 16, nrmTextFormat, b1Rect, inactBrush);
            else
            {
                if (!b1Hglt)Draw->DrawTextW(L"ИМЕ НА КОМАНДИР", 16, nrmTextFormat, b1Rect, txtBrush);
                else Draw->DrawTextW(L"ИМЕ НА КОМАНДИР", 16, nrmTextFormat, b1Rect, hgltBrush);
            }
            if (!b2Hglt)Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmTextFormat, b2Rect, txtBrush);
            else Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmTextFormat, b2Rect, hgltBrush);
            if (!b3Hglt)Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmTextFormat, b3Rect, txtBrush);
            else Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmTextFormat, b3Rect, hgltBrush);
        }

        if (build_selected)
            Draw->DrawBitmap(bmpTurret1, D2D1::RectF((float)(cur_pos.x - 20), (float)(cur_pos.y - 20), 
                (float)(cur_pos.x + 20), (float)(cur_pos.y + 20)));

        if (Castle)
        {
            Draw->DrawBitmap(bmpBase, D2D1::RectF(Castle->x, Castle->y, Castle->ex, Castle->ey));
            if (lifeBrush)Draw->DrawLine(D2D1::Point2F(Castle->x, Castle->ey + 15.0f), D2D1::Point2F(Castle->x + 
                (float)(castle_lifes / 5.3f), Castle->ey + 15.0f), lifeBrush, 10.0f);
        }
        if (!vEvils.empty())
        {
            for (int i = 0; i < vEvils.size(); i++)
            {
                if (vEvils[i]->GetFlag(evil1_flag))
                {
                    int a_frame = vEvils[i]->GetFrame();
                    Draw->DrawBitmap(bmpEvil1[a_frame], Resizer(bmpEvil1[a_frame], vEvils[i]->x, vEvils[i]->y));
                }
                if (vEvils[i]->GetFlag(evil2_flag))
                {
                    int a_frame = vEvils[i]->GetFrame();
                    Draw->DrawBitmap(bmpEvil2[a_frame], Resizer(bmpEvil2[a_frame], vEvils[i]->x, vEvils[i]->y));
                }
                if (vEvils[i]->GetFlag(evil3_flag))
                {
                    int a_frame = vEvils[i]->GetFrame();
                    Draw->DrawBitmap(bmpEvil3[a_frame], Resizer(bmpEvil3[a_frame], vEvils[i]->x, vEvils[i]->y));
                }
                if (vEvils[i]->GetFlag(evil4_flag))
                {
                    int a_frame = vEvils[i]->GetFrame();
                    Draw->DrawBitmap(bmpEvil4[a_frame], Resizer(bmpEvil4[a_frame], vEvils[i]->x, vEvils[i]->y));
                }
                if (vEvils[i]->GetFlag(evil5_flag))
                {
                    int a_frame = vEvils[i]->GetFrame();
                    Draw->DrawBitmap(bmpEvil5[a_frame], Resizer(bmpEvil5[a_frame], vEvils[i]->x, vEvils[i]->y));
                }
            }
        }
        if (!vTurrets.empty())
        {
            for (int i = 0; i < vTurrets.size(); i++)
            {
                if (vTurrets[i]->GetFlag(turret1_flag))
                    Draw->DrawBitmap(bmpTurret1, D2D1::RectF(vTurrets[i]->x, vTurrets[i]->y, vTurrets[i]->ex, vTurrets[i]->ey));
                if (vTurrets[i]->GetFlag(turret2_flag))
                    Draw->DrawBitmap(bmpTurret2, D2D1::RectF(vTurrets[i]->x, vTurrets[i]->y, vTurrets[i]->ex, vTurrets[i]->ey));
                if (vTurrets[i]->GetFlag(turret3_flag))
                    Draw->DrawBitmap(bmpTurret3, D2D1::RectF(vTurrets[i]->x, vTurrets[i]->y, vTurrets[i]->ex, vTurrets[i]->ey));
                if (vTurrets[i]->GetFlag(turret4_flag))
                    Draw->DrawBitmap(bmpTurret4, D2D1::RectF(vTurrets[i]->x, vTurrets[i]->y, vTurrets[i]->ex, vTurrets[i]->ey));
                if (vTurrets[i]->GetFlag(turret5_flag))
                    Draw->DrawBitmap(bmpTurret5, D2D1::RectF(vTurrets[i]->x, vTurrets[i]->y, vTurrets[i]->ex, vTurrets[i]->ey));
            }
        }
        if (!vShots.empty())
        {
            for (int i = 0; i < vShots.size(); ++i)
            {
                int a_frame = vShots[i]->GetFrame();
                Draw->DrawBitmap(bmpShot[a_frame], Resizer(bmpShot[a_frame], vShots[i]->x, vShots[i]->y));
            }
        }

        //////////////////////////////////

        Draw->EndDraw();
    }
    
    std::remove(temp_file);
    FreeResources();

    return (int) bMsg.wParam;
}