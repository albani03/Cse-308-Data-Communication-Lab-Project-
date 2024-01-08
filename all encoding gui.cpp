#include <windows.h>
#include <string>

// Function prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AddControls(HWND);

// Global variables for controls
HWND hInputEdit;
HWND hEncodeButton;
HWND hEncodedStatic;
HWND hDecodedStatic;
HWND hUnipolarStatic;
HWND hManchesterStatic;
HWND hDifferentialStatic;

// Helper functions to encode and decode lines

// Unipolar encoding
std::string encodeUnipolar(const std::string& line) {
    std::string encoded = "";
    for (char c : line) {
        encoded += (c == '0') ? "0" : "1";
    }
    return encoded;
}

// Manchester encoding
std::string encodeManchester(const std::string& line) {
    std::string encoded = "";
    for (char c : line) {
        encoded += (c == '0') ? "01" : "10";
    }
    return encoded;
}

// Differential Manchester encoding
std::string encodeDifferentialManchester(const std::string& line) {
    std::string encoded = "";
    char previousBit = '0'; // Assuming the initial polarity is 0
    for (char c : line) {
        if (c == '0') {
            // Invert the previous bit
            previousBit = (previousBit == '0') ? '1' : '0';
            encoded += previousBit;
        } else {
            // Keep the previous bit
            encoded += previousBit;
        }
        // Transition at the end of each bit period
        previousBit = (previousBit == '0') ? '1' : '0';
        encoded += previousBit;
    }
    return encoded;
}


std::string encodeLine(const std::string& line) {
    std::string encodedLine;
    for (size_t i = 0; i < line.length(); i++) {
        if (line[i] == '0') {
            encodedLine += "0";
        } else {
            if (i % 2 == 0) {
                encodedLine += "+";
            } else {
                encodedLine += "-";
            }
        }
    }
    return encodedLine;
}

std::string decodeLine(const std::string& encodedLine) {
    std::string decodedLine;
    for (size_t i = 0; i < encodedLine.length(); i++) {
        if (encodedLine[i] == '0') {
            decodedLine += "0";
        } else {
            decodedLine += "1";
        }
    }
    return decodedLine;
}

// Entry point of the application (WinMain)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char g_szClassName[] = "myWindowClass";
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    // Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Bipolar Encoding GUI",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 300,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

// Add controls to the main window
void AddControls(HWND hwnd) {
    hInputEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                10, 10, 430, 20,
                                hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

    hEncodeButton = CreateWindow("BUTTON", "Encode",
                                 WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                 10, 40, 430, 30,
                                 hwnd, (HMENU)2, GetModuleHandle(NULL), NULL);

    hEncodedStatic = CreateWindow("STATIC", "Bipolar Encoded Line:",
                                  WS_VISIBLE | WS_CHILD,
                                  10, 80, 430, 20,
                                  hwnd, (HMENU)3, GetModuleHandle(NULL), NULL);

    hDecodedStatic = CreateWindow("STATIC", "Bipolar Decoded Line:",
                                  WS_VISIBLE | WS_CHILD,
                                  10, 110, 430, 20,
                                  hwnd, (HMENU)4, GetModuleHandle(NULL), NULL);
    hUnipolarStatic = CreateWindow("STATIC", "Unipolar Encoded Line:",
                                  WS_VISIBLE | WS_CHILD,
                                  10, 140, 430, 20,
                                  hwnd, (HMENU)5, GetModuleHandle(NULL), NULL);

    hManchesterStatic = CreateWindow("STATIC", "Manchester Encoded Line:",
                                     WS_VISIBLE | WS_CHILD,
                                     10, 170, 430, 20,
                                     hwnd, (HMENU)6, GetModuleHandle(NULL), NULL);

    hDifferentialStatic = CreateWindow("STATIC", "Differential Manchester Encoded Line:",
                                       WS_VISIBLE | WS_CHILD,
                                       10, 200, 430, 20,
                                       hwnd, (HMENU)7, GetModuleHandle(NULL), NULL);
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_CREATE:
            AddControls(hwnd);
            break;
        case WM_COMMAND:
            if ((HWND)lParam == hEncodeButton) {
                char userInput[1024];
                GetWindowText(hInputEdit, userInput, 1024);
                std::string encodedBipolar = encodeLine(userInput);
                std::string decoded = decodeLine(encodedBipolar);
                std::string encodedUnipolar = encodeUnipolar(userInput);
                std::string encodedManchester = encodeManchester(userInput);
                std::string encodedDifferential = encodeDifferentialManchester(userInput);

                SetWindowText(hEncodedStatic, ("Bipolar Encoded Line: " + encodedBipolar).c_str());
                SetWindowText(hDecodedStatic, ("Bipolar Decoded Line: " + decoded).c_str());
                SetWindowText(hUnipolarStatic, ("Unipolar Encoded Line: " + encodedUnipolar).c_str());
                SetWindowText(hManchesterStatic, ("Manchester Encoded Line: " + encodedManchester).c_str());
                SetWindowText(hDifferentialStatic, ("Differential Manchester Encoded Line: " + encodedDifferential).c_str());
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

