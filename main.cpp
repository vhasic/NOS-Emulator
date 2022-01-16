#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <wingdi.h>
#include <fstream>
#include <algorithm>
#include <chrono>


static const unsigned int NumberOfKeys = 256U;
bool previousKeyboardState[NumberOfKeys];


unsigned short memory[65536];
// Ovaj procesor je 16 bitni pa je njegova riječ 16 bita = unsigned short
// [0,2000] je ROM; >= 2000 je RAM
bool pritisnutTaster=false;
//int nizihOsamBitaSrc1;
FILE* disk;
char buffer[256];
int rezimDiska; //0=idle, 1=read, 2=write
int sektor;
int x=8,y=8;

unsigned short regs[16];
char asciikeyboard;
int videochanged = 1;
long cyclecount, totcount;
struct BitMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[256];
};
struct BitMAPINFO bi;
BYTE *pBits;
HDC memDC;
HBITMAP hBM;
HINSTANCE hinst;
HWND hwndMain;
HDC hdc;

void reset() {
    regs[15] = 0;
}

// Video memory at 0x8192
// Nama je video memorija: Tekstualna video memorija, 1 riječ po ASCII znaku, rezolucija 80x25 na adresi 8192 = 0x8192

void iscrtajZnakNaEkranu(HWND hwnd, int znak=0){
    hdc = GetDC(hwnd);
    TCHAR text[256];
    snwprintf(reinterpret_cast<wchar_t *>(text), 256, L"%c", znak);
    TextOut(hdc, x, y, text, wcslen(reinterpret_cast<const wchar_t *>(text)));
    x+=8;
    if(x>650){
        x=8;
        y+=16;
    }
    pritisnutTaster= true;
}

/// Funkcija za simulaciju mašinskih instrukcija
void mloop() {
    unsigned short ir, op, dest, src1, src2, n;
    unsigned int temp1;
    signed int temp2;

    do {
        ir = memory[regs[15]];
//     totcount++; if (totcount==1000000) { totcount=0; printf("*"); }

//     printf("Executing %4x: %4x R0=%4x R1=%4x R2=%4x R3=%4x R4=%4x R5=%4x R6=%4x  R7=%4x  R8=%4x  R13=%4x\n",
//        regs[15],ir,regs[0],regs[1],regs[2],regs[3],regs[4],regs[5],regs[6],regs[7], regs[8], regs[13]  );
        op = (ir & 0xF000) >> 12;
        dest = (ir & 0x0F00) >> 8;
        src1 = (ir & 0x00F0) >> 4;
        src2 = (ir & 0x000F);
        regs[15]++;
        switch (op) {
            case 0x0: // LOD
                // čitanje sa diska
                if(regs[src2] == 0xFFFC){ // na ovoj lokaciji je prenos podataka za/sa disk/a
                    if (rezimDiska == 1){ // ako se sa diska čita
                        std::ifstream diskFile("disk.dat", std::ios::binary);
                        diskFile.read(reinterpret_cast<char*>(&buffer),256); // učitavanje 256 bajta u buffer sa diska
                    }
                    // učitavanje iz buffera u odredište
                    for (int i = 0; i < 256; i+=2) {
                        //todo ovdje se stalno piše jedno preko drugog? da li se ovo trebalo u memoriju zapisati?
                        if(i==0){ // u dest registar se upisuju samo prva 2 bajta (prva riječ)
                            regs[dest]=(buffer[i]<<8)+buffer[i+1]; //[0] vecih 8 bita, [1] nizih 8 bita; ...
                        }
                    }
                    rezimDiska=0; // disk prelazi u stanje idle
                }


                if (regs[src2] < 0xFFF0) {
                    regs[dest] = memory[regs[src2]];
                } else if (regs[src2] == 0xFFF1) {
                    regs[dest] = asciikeyboard;
                    asciikeyboard = 0;
                }
                if (src2 == 15)
                    regs[15]++;

                break;
            case 0x1: // ADD
                regs[dest] = regs[src1] + regs[src2];
                break;
            case 0x2: // SUB
                regs[dest] = regs[src1] - regs[src2];
                break;
            case 0x3:  // AND
                regs[dest] = regs[src1] & regs[src2];
                break;
            case 0x4:  // ORA
                regs[dest] = regs[src1] | regs[src2];
                break;
            case 0x5:  // XOR
                regs[dest] = regs[src1] ^ regs[src2];
                break;
            case 0x6:  // SHR
                n = regs[src2] & 0xF;
                switch ((regs[src2] & 0x0030) >> 4) {

                    case 0:
                        temp2 = regs[src1];
                        if (regs[src1] >= 0x8000)
                            temp2 |= 0xFFFF0000;
                        regs[dest] = temp2 >> (n);
                        break;
                    case 1:
                        temp1 = regs[src1];
                        regs[dest] = temp1 >> (n);
                        break;
                    case 2:
                        regs[dest] = regs[src1] << n;
                        break;
                    case 3:
                        regs[dest] = (regs[src1] << n) |
                                     (regs[src1] >> (16 - n));
                        break;
                }
                break;
            case 0x7:  // MUL
                regs[dest] = regs[src1] * regs[src2];
                break;
            case 0x8:  // STO
			    //Ako je upis u ROM, ne raditi ništa
                if (regs[src2] < 2000) break;
                //Upis u RAM
                else if (regs[src2] > 2000 && regs[src2]<0xFEFF){ // RAM između 2000 i 65279
                    regs[dest] = memory[regs[src2]] = regs[src1];
                }
			    // upis u video memoriju
                if (regs[src2] > 8192){
                    // već je upisano u memoriju gornjim if-om
                    videochanged=1;
                    iscrtajZnakNaEkranu(hwndMain, memory[src2]);
                }
			    // upis na kontroler diska
                if (regs[src2] == 0xFFFE){ // na ovoj lokaciji je komanda za disk
                    rezimDiska=regs[src1]; // vrijednost src1 registra: 0, 1 i li 2
                }
                else if(regs[src2] == 0xFFFD){ // na ovoj lokaciji je izbor sektora
                    sektor=regs[src1]*256; // izbor sektora diska; jedan sektor je 256 bajta
                }
                else if(regs[src2] == 0xFFFC){ // na ovoj lokaciji je prenos podataka
                    if (rezimDiska == 2){ // komanda write: dopisivanje podataka na disk
                        std::ofstream diskFile("disk.dat", std::ios::ate | std::ios::binary);
                        char izlazniBuffer[256];
                        // todo ovdje vjerovatno ide iz memorije upis
                        //ako je iz memorije onda se mijenjaju nizih i visih osam bita sa zakomentarisanim linijama
//                        int pozicija = regs[src1];
                        for (int i = 0; i < 256; i+=2) {
//                            char nizihOsamBita = memory[pozicija] & 0x00FF;
//                            char visihOsamBita = memory[pozicija] & 0xFF00;
//                            pozicija++;
                            char nizihOsamBita = regs[src1] & 0x00FF;
                            char visihOsamBita = regs[src1] & 0xFF00;
                            izlazniBuffer[i]=visihOsamBita;
                            izlazniBuffer[i+1]=nizihOsamBita;
                        }
                        diskFile.write(reinterpret_cast<char*>(&izlazniBuffer),256); //upis izlaznog buffera na disk
                        rezimDiska=0; // disk prelazi u stanje idle
                    }
                }

                // stari if
/*                if (regs[src2] < 0xFFF0) {
                    regs[dest] = memory[regs[src2]] = regs[src1];
                    if (regs[src2] >= 0x8192 && regs[src2] < 0xFB00) {
                        int pos = (regs[src2] - 0x8192) * 16;
                        unsigned short val = regs[src1];

                        *(pBits + pos) = (val & 0x8000) ? 1 : 0;
                        *(pBits + pos + 1) = (val & 0x4000) ? 1 : 0;
                        *(pBits + pos + 2) = (val & 0x2000) ? 1 : 0;
                        *(pBits + pos + 3) = (val & 0x1000) ? 1 : 0;
                        *(pBits + pos + 4) = (val & 0x0800) ? 1 : 0;
                        *(pBits + pos + 5) = (val & 0x0400) ? 1 : 0;
                        *(pBits + pos + 6) = (val & 0x0200) ? 1 : 0;
                        *(pBits + pos + 7) = (val & 0x0100) ? 1 : 0;
                        *(pBits + pos + 8) = (val & 0x0080) ? 1 : 0;
                        *(pBits + pos + 9) = (val & 0x0040) ? 1 : 0;
                        *(pBits + pos + 10) = (val & 0x0020) ? 1 : 0;
                        *(pBits + pos + 11) = (val & 0x0010) ? 1 : 0;
                        *(pBits + pos + 12) = (val & 0x0008) ? 1 : 0;
                        *(pBits + pos + 13) = (val & 0x0004) ? 1 : 0;
                        *(pBits + pos + 14) = (val & 0x0002) ? 1 : 0;
                        *(pBits + pos + 15) = (val & 0x0001) ? 1 : 0;
                        videochanged = 1;

                    }
                } else if (regs[src2] == 0xFFF2) {
                    regs[dest] = regs[src1];
                    //  printf("%c",regs[src1] );
                }*/
                if (src2 == 15)
                    regs[15]++;
                break;
            case 0x9:  // MIF
                if (regs[src1] != 0) {
                    regs[dest] = regs[src2];
                }
//                regs[dest] = src1 << 4 | src2; //LDC
//                if (regs[dest] >= 128)
//                    regs[dest] |= 0xFF00;
                break;
            case 0xA: // GTU
                regs[dest] = regs[src1] > regs[src2] ? 1 : 0;
                break;
            case 0xB: // GTS
                regs[dest] = (signed short) regs[src1] > (signed short) regs[src2] ? 1 : 0;
                break;
            case 0xC:  // LTU
                regs[dest] = regs[src1] < regs[src2] ? 1 : 0;
                break;
            case 0xD:  // LTS
                regs[dest] = (signed short) regs[src1] < (signed short) regs[src2] ? 1 : 0;
                break;
            case 0xE:  // EQU
                regs[dest] = regs[src1] == regs[src2] ? 1 : 0;
                break;
            case 0xF:  // MAJ
                regs[dest] = regs[src1];
                regs[15] = regs[src2];
                break;

        }
        cyclecount--;
    } while (cyclecount > 0);

};

void DisplayDIB(HWND hwnd, HDC hdc) {
    memDC = CreateCompatibleDC(hdc);
    SelectObject(memDC, hBM);
    BitBlt(hdc, 10, 10, 640, 480, memDC, 0, 0, SRCCOPY);
    DeleteDC(memDC);
}


DWORD WINAPI EmulateCPU(void *arg) {
#define freq 1000000    //nama je frekvencija 1MHz=1000000
#define LOWFREQ
    clock_t t1, t2, t3, delayadjust;
    int loopcount;
    double delay;
    loopcount = 20000;
    delay = loopcount * (double) CLOCKS_PER_SEC / freq;
    delayadjust = delay;

    while (TRUE) {
        t1 = clock();
        t2 = t1 + delayadjust;
        cyclecount = loopcount;
        mloop();
        if (videochanged) {
            DisplayDIB(hwndMain, hdc);
            videochanged = 0;
        }
#ifdef LOWFREQ
        t3 = clock();
        if (t3 < t2)
            Sleep((t2 - t3) * CLOCKS_PER_SEC / 1000);
#else
        while (t2 <clock()) {} ;
#endif
    }
}


void DeallocateGDI() {
    ReleaseDC(hwndMain, hdc);
    free(pBits);
    DeleteObject(hBM);
}

void CreateDIB() {
    hdc = GetDC(hwndMain);
    int i;
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = 640;
    bi.bmiHeader.biHeight = -480; //negative number == top down DIB
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 8;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiHeader.biClrUsed = 2;
    bi.bmiColors[0].rgbRed = 0xE0;
    bi.bmiColors[0].rgbGreen = 0xDE;
    bi.bmiColors[0].rgbBlue = 0xBC;
    bi.bmiColors[0].rgbReserved = 0;

    bi.bmiColors[1].rgbRed = 0;
    bi.bmiColors[1].rgbGreen = 0;
    bi.bmiColors[1].rgbBlue = 0;
    bi.bmiColors[1].rgbReserved = 0;

    hBM = (HBITMAP) CreateDIBSection(hdc, (BITMAPINFO *) &bi, DIB_RGB_COLORS, (VOID **) &pBits, 0, 0);
}
/*Port na adresi 0xFF00 sadrži vrijednosti pritisnutih
tastera iz najvišeg reda (ESC do F12), svaki taster jedan bit
Port na adresi 0xFF01 sadrži vrijednosti pritisnutih tastera iz
drugog reda (akcenti, 1,2,3,4,5,6,7,8,9,0,’,+, backspace)), svaki
taster jedan bit
Port na adresi 0xFF02 sadrži vrijednosti pritisnutih tastera iz
trećeg reda (Tab, q,w,e,r,t do Enter), svaki taster jedan bit
Port na adresi 0xFF03 sadrži vrijednosti pritisnutih tastera iz
četvrtog reda (Caps lock, a,s,d, do ž), svaki taster jedan bit
Port na adresi 0xFF04 sadrži vrijednosti pritisnutih tastera iz
petog reda (od lijevog do desnog Shift), svaki taster jedan bit
Port na adresi 0xFF05 sadrži vrijednosti pritisnutih tastera iz
petog reda (od lijevog do desnog Ctrl), svaki taster jedan bit
*/
void postaviBitLokacije(WPARAM wParam){
    // 1000'0000'0000'0000,...,0000'0000'0000'0001
    unsigned int maska[16]={0x8000,0x4000,0x2000,0x1000,
                             0x0800,0x0400,0x0200,0x0100,
                             0x0080,0x0040,0x0020,0x0010,
                             0x0008,0x0004,0x0002,0x0001};
    // nisam mogao naći sve tražene znakove u dokumentaciji
    int redovi[10][16]={{VK_ESCAPE,VK_F1,VK_F2,VK_F3,VK_F4,VK_F5,VK_F6,VK_F7,VK_F8,VK_F9,VK_F10,VK_F11,VK_F12},
                        {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30},
                        {VK_TAB,0x51,0x57,0x45,0x52,0x54,0x5a,0x55,0x49,0x4f,0x50,VK_RETURN},
                        {VK_CAPITAL,0x41, 0x53,0x44,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c},
                        {VK_LSHIFT,0x59,0x58,0x43,0x56,0x42,0x4e,0x4d,VK_RSHIFT},
                        {VK_CONTROL,VK_SPACE}};
    int kod;
    int red=0;
    for (int i = 0; i < 10; ++i) {
        auto pok=std::find(redovi[i], redovi[i] + 16, wParam); // prolazi kroz redove i traži znak
        if(pok!=redovi[i] + 10){
            int x = std::distance(redovi[i], std::find(redovi[i], redovi[i]+16, *pok));
            kod=maska[x];
            break;
        }
        red++;
    }
    // jedna memorijska lokacija ima 16 bita
    // jedan bit označava jedno slovo s lijeva na desno
    if(red==0){
        memory[0xFF00] &= kod;
    } else if(red==1){
        memory[0xFF01] &= kod;
    }else if(red==2){
        memory[0xFF02] &= kod;
    }else if(red==3){
        memory[0xFF03] &= kod;
    }else if(red==4){
        memory[0xFF04] &= kod;
    }else if(red==5){
        memory[0xFF05] &= kod;
    }
}

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;

    switch (Message) {

        /* Upon destruction, tell the main thread to stop */
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        case WM_CHAR: {
            asciikeyboard = wParam;

            // prikaz pritisnutog znaka na ekranu
            iscrtajZnakNaEkranu(hwnd,asciikeyboard);
            break;
        }
        case WM_KEYDOWN:{
            asciikeyboard = wParam; // virtualni kod pritisnutog tastera wParam
            // postavljanje bita
            postaviBitLokacije(wParam);
            break;
        }
        case WM_KEYUP:{
            // ako se želi samo jedinica vratiti na 0, a da nule pređu u jedan uraditi xor sa 0xFFFF
            // vraćanje kodova na 0
            memory[0xFF00] &= 0x0000;
            memory[0xFF01] &= 0x0000;
            memory[0xFF02] &= 0x0000;
            memory[0xFF03] &= 0x0000;
            memory[0xFF04] &= 0x0000;
            memory[0xFF05] &= 0x0000;
            break;
        }
        case WM_PAINT: {
            hdc = BeginPaint(hwnd, &ps);
            videochanged = 1;
            EndPaint(hwnd, &ps);
            break;
        }
            /* All other messages (a lot of them) are processed using default procedures */
        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

// program koji učitava OS u dio RAM memorije
void loadOS() {
    FILE* prom = fopen("..\\cmake-build-debug/demo.mem", "rb");
    //FILE* prom = fopen("..\\forthgraph.mem", "rb");
    if (prom == nullptr) {
        printf("Error");
    }
    else {
        fread(memory, 1, 0x20000, prom);	// od pozicije 0x2000 je ucitan operativni sistem u RAM
        fclose(prom);
    }
}

//Works fine.
bool isKeyDown(int key)
{
    return (GetAsyncKeyState(key) & (1 << 16));
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc; /* A properties struct of our window */
    MSG msg; /* A temporary location for all messages */


    /* zero out the struct and set the stuff we want to modify */
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc; /* This is where we will send messages to */
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    /* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszClassName = "WindowClass";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }


    hwndMain = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", "SVEU16 emulator", WS_VISIBLE | WS_OVERLAPPED |
                                                                                  WS_CAPTION | WS_SYSMENU |
                                                                                  WS_MINIMIZEBOX,
                              0, /* x */
                              0, /* y */
                              680, /* width */
                              540, /* height */
                              NULL, NULL, hInstance, NULL);

    if (hwndMain == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    RECT rc;

    GetWindowRect(hwndMain, &rc);
    int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
    int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;
    SetWindowPos(hwndMain, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    UpdateWindow(hwndMain);
    CreateDIB();
    HANDLE thread = CreateThread(NULL, 0, EmulateCPU, NULL, 0, NULL);

    // Nama je zadatak: ROM 2 kiloriječi na adresi 0, demo softver
    loadOS();

    reset();

    /*
        This is the heart of our program where all input is processed and
        sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
        this loop will not produce unreasonably high CPU usage
    */
    while (GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
        TranslateMessage(&msg); /* Translate key codes to chars if present */
        DispatchMessage(&msg); /* Send it to WndProc */
    }

    DeallocateGDI();
    return msg.wParam;
}





