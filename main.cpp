#include <windows.h>

//全局变量
HINSTANCE hInst;
static TCHAR szClassName[] = TEXT("Translator");  //窗口类名

//函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); //窗口过程
void RegistWindowClass(HINSTANCE hInstance);      //注册窗口类
HWND InitWindow(HINSTANCE hInstance);             //初始化窗口
HWND InitInBox(HWND hWnd,HINSTANCE hInstance);    //初始化编辑框（文本输入框）
HWND InitOutBox(HWND hWnd, HINSTANCE hInstance);  //初始化文本框（输出翻译结果）
HWND InitBotton(HWND hWnd, HINSTANCE hInstance);  //初始化按钮  （翻译按钮）
HWND InitModeGroup(HWND hWnd,HINSTANCE hInstance);//初始化分组框（模式选项）
HWND InitCheckBox(HWND hPreWnd,HINSTANCE hInstance); //初始化复选框 (智能模式)
HWND InitMode1(HWND hPreWnd, HINSTANCE hInstance);   //初始化单选框1（中译英）
HWND InitMode2(HWND hPreWnd, HINSTANCE hInstance);   //初始化单选框2 (英译中)

HFONT InitFont();                                 //初始化字体
void SetFont(HWND target,HFONT hFont);            //为窗口控件设置字体

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR szCmdLine,
    int iCmdShow
)
{
    HWND     hwnd;  //窗口句柄
    MSG      msg;   //消息

    hInst = hInstance;

	RegistWindowClass(hInstance);
	hwnd = InitWindow(hInstance);

    ShowWindow (hwnd, iCmdShow);//显示窗口
    UpdateWindow (hwnd);//更新（绘制）窗口

	//消息循环
    while( GetMessage(&msg, NULL, 0, 0) )
	{
        TranslateMessage(&msg);  //翻译消息
        DispatchMessage (&msg);  //分派消息
    }

    return msg.wParam;
}

//窗口过程
LRESULT CALLBACK WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    PAINTSTRUCT ps;
    HDC hdc;
	//HDC hdcStatic;

	static HFONT hFont;
    static HWND hOutBox;
	static HWND hInBox;
	static HWND hBotton;
	static HWND hModeGroup;
	static HWND hMode1;
	static HWND hMode2;
	static HWND hCheckBox;
	
	static HBRUSH hBrushWhite;

    switch (message){
        case  WM_CREATE:
			hBrushWhite = CreateSolidBrush(RGB(0xff, 0xff, 0xff));

			hFont = InitFont();
			hOutBox = InitOutBox(hWnd,hInst);
			hBotton = InitBotton(hWnd,hInst);
			hInBox = InitInBox(hWnd,hInst);
			hModeGroup = InitModeGroup(hWnd,hInst);
			hCheckBox = InitCheckBox(hModeGroup,hInst);
			hMode1 = InitMode1(hModeGroup,hInst);
			hMode2 = InitMode2(hModeGroup,hInst);

		


			SetFont(hOutBox,hFont);
			SetFont(hInBox, hFont);
			SetFont(hBotton ,hFont);
			SetFont(hModeGroup,hFont);
			SetFont(hCheckBox, hFont);
			SetFont(hMode1, hFont);
			SetFont(hMode2, hFont);
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            // TODO:  在此添加任意绘图代码...
            EndPaint(hWnd, &ps);
            break;
		case WM_CTLCOLORBTN:
			return (INT_PTR)hBrushWhite;  //这里必须返回画刷句柄
		case WM_CTLCOLORSTATIC:
			return (INT_PTR)hBrushWhite;  //这里必须返回画刷句柄
        case WM_DESTROY:
			DeleteObject(hFont);//删除创建的字体
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam) ;
}

void RegistWindowClass(HINSTANCE hInstance)
{
	
	WNDCLASS wndclass;  //窗口类

	//为窗口类的各个字段赋值
	wndclass.style = CS_HREDRAW | CS_VREDRAW;  //窗口风格
	wndclass.lpfnWndProc = WndProc;  //窗口过程
	wndclass.cbClsExtra = 0;  //暂时不需要理解
	wndclass.cbWndExtra = 0;  //暂时不需要理解
	wndclass.hInstance = hInstance;  //当前窗口句柄
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);  //窗口图标
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);  //鼠标样式
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  //窗口背景画刷（白色）
	wndclass.lpszMenuName = NULL;  //窗口菜单
	wndclass.lpszClassName = szClassName;  //窗口类名
	//注册窗口
	RegisterClass(&wndclass);
}

HWND InitWindow(HINSTANCE hInstance)
{
	return CreateWindow(
		szClassName,  //窗口名字
		TEXT("Translator"),  //窗口标题（出现在标题栏）
		WS_OVERLAPPEDWINDOW,  //窗口风格
		CW_USEDEFAULT,  //初始化时x轴的位置
		CW_USEDEFAULT,  //初始化时y轴的位置
		500,  //窗口宽度
		600,  //窗口高度
		NULL,  //父窗口句柄
		NULL,  //窗口菜单句柄
		hInstance,  //当前窗口的句柄
		NULL  //不使用该值
	);
}

HWND InitInBox(HWND hWnd, HINSTANCE hInstance)
{
	return CreateWindow(
		TEXT("edit"),
		TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE| ES_LOWERCASE | ES_AUTOVSCROLL,
		10, 40, 460, 200,
		hWnd, (HMENU)1, hInst, NULL
	);
}

HWND InitOutBox(HWND hWnd, HINSTANCE hInstance)
{
	return CreateWindow(
		TEXT("edit"),     //框的类名
		TEXT("C语言中文网hhhhhhhhfasff发生发士hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh大夫asfsadfsa"),  //控件的文本
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_LOWERCASE | ES_AUTOVSCROLL| ES_READONLY,
		10 /*X坐标*/, 350 /*Y坐标*/, 460 /*宽度*/, 200 /*高度*/,
		hWnd,  //父窗口句柄
		(HMENU)2,  //为控件指定一个唯一标识符
		hInstance,  //当前程序实例句柄
		NULL
	);
}

HWND InitBotton(HWND hWnd, HINSTANCE hInstance)
{
	return CreateWindow(
		TEXT("button"), //按钮控件的类名
		TEXT("翻译"),
		WS_CHILD | WS_VISIBLE | WS_BORDER | BS_FLAT/*扁平样式*/,
		320 /*X坐标*/, 270 /*Y坐标*/, 80 /*宽度*/, 50/*高度*/,
		hWnd, (HMENU)3 /*控件唯一标识符*/, hInst, NULL
	);
}

HWND InitModeGroup(HWND hWnd, HINSTANCE hInstance)
{
	return CreateWindow(
		TEXT("button"), TEXT("MODE"),
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		10, 240, 240, 105,
		hWnd, (HMENU)4, hInst, NULL
	);
}

HWND InitCheckBox(HWND hPreWnd, HINSTANCE hInstance)
{
	return CreateWindow(
			TEXT("button"), 
			TEXT("智能模式"),
			WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX/*复选框*/,
			5, 25, 100, 40,
			hPreWnd, (HMENU)5, hInst, NULL
		);
}

HWND InitMode1(HWND hPreWnd, HINSTANCE hInstance)
{
	return  CreateWindow(
		TEXT("button"), TEXT("中文 → 英文"),
		WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON ,
		110, 15, 120, 40,
		hPreWnd, (HMENU)6, hInst, NULL
	);
}

HWND InitMode2(HWND hPreWnd, HINSTANCE hInstance)
{
	return CreateWindow(
		TEXT("button"), TEXT("英文 → 中文"),
		WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
		110, 55, 120, 40,
		hPreWnd, (HMENU)7, hInst, NULL
	);
}

HFONT InitFont()
{
	return CreateFont(
		-16/*高度*/, -8/*宽度*/, 0/*不用管*/, 0/*不用管*/, 400 /*一般这个值设为400*/,
		FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
		DEFAULT_CHARSET,  //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量可用
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
		DEFAULT_QUALITY,  //默认输出质量
		FF_DONTCARE,  //不指定字体族
		TEXT("微软雅黑")  //字体名
	);
}

void SetFont(HWND target,HFONT hFont)
{
	SendMessage(
		target,  //欲设置字体的控件句柄
		WM_SETFONT,  //消息名（消息类型）
		(WPARAM)hFont,  //字体句柄
		NULL  //传空值即可
	);
}
