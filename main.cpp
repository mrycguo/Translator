#include <iostream>
#include <sstream>
#include <windows.h>
#include <curl/curl.h>  
#include <openssl/md5.h>
#include <json/json.h>

#pragma warning(disable:4996)  //使允许使用sprintf()等函数
using namespace std;

//全局变量
HINSTANCE hInst;
static TCHAR szClassName[] = TEXT("Translator");  //窗口类名
//TCHAR szInText[1000];    //待翻译文本
//TCHAR szOutText[1000];  //翻译结果
string szInText;
string szOutText;
int mode;               //翻译模式 1-智能模式 2-英译中 3-中译英


//函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); //窗口过程
void RegistWindowClass(HINSTANCE hInstance);          //注册窗口类
HWND InitWindow(HINSTANCE hInstance);                 //初始化窗口
HWND InitInBox(HWND hWnd, HINSTANCE hInstance);        //初始化编辑框（文本输入框）
HWND InitOutBox(HWND hWnd, HINSTANCE hInstance);      //初始化文本框（输出翻译结果）
HWND InitBotton(HWND hWnd, HINSTANCE hInstance);      //初始化按钮  （翻译按钮）
HWND InitModeGroup(HWND hWnd, HINSTANCE hInstance);    //初始化分组框（模式选项）
HWND InitCheckBox(HWND hPreWnd, HINSTANCE hInstance);  //初始化复选框 (智能模式)
HWND InitMode1(HWND hPreWnd, HINSTANCE hInstance);    //初始化单选框1 (英译中)
HWND InitMode2(HWND hPreWnd, HINSTANCE hInstance);    //初始化单选框2（中译英）

HFONT InitFont();                                 //初始化字体
void SetFont(HWND target, HFONT hFont);            //为窗口控件设置字体

size_t write_func(void *contents, size_t size, size_t nmemb, void * userp); //url的回调函数
std::string string_To_UTF8(const std::string & str);  //来源： zhangxueyang1的博客
int Translate();

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

  ShowWindow(hwnd, iCmdShow);//显示窗口
  UpdateWindow(hwnd);//更新（绘制）窗口

//消息循环
  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);  //翻译消息
    DispatchMessage(&msg);  //分派消息
  }

  return msg.wParam;
}

//窗口过程
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  PAINTSTRUCT ps;
  HDC hdc;
  //HDC hdcStatic;
  int wmID;//被触发的按钮id
  int wmEvent; //被触发的按钮事件

  static HFONT hFont;
  static HWND hOutBox;
  static HWND hInBox;
  static HWND hButton;
  static HWND hModeGroup;
  static HWND hMode1;
  static HWND hMode2;
  static HWND hCheckBox;

  static HBRUSH hBrushWhite;
  //获取翻译模式 (智能/中英/英中)
  if (SendMessage(hCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED)  
    mode = 1;  
  else if (SendMessage(hMode1, BM_GETCHECK, 0, 0) == BST_CHECKED) 
    mode = 2;  
  else if (SendMessage(hMode2, BM_GETCHECK, 0, 0) == BST_CHECKED)
    mode = 3;
  else
    mode = 0;
  //处理消息
  switch (message) {
  case  WM_CREATE:
    hBrushWhite = CreateSolidBrush(RGB(0xff, 0xff, 0xff)); //白色画刷

    //初始化窗口组件
    hFont = InitFont();
    hOutBox = InitOutBox(hWnd, hInst);
    hButton = InitBotton(hWnd, hInst);
    hInBox = InitInBox(hWnd, hInst);
    hModeGroup = InitModeGroup(hWnd, hInst);
    hCheckBox = InitCheckBox(hModeGroup, hInst);
    hMode1 = InitMode1(hModeGroup, hInst);
    hMode2 = InitMode2(hModeGroup, hInst);

    //字体优化
    SetFont(hOutBox, hFont);
    SetFont(hInBox, hFont);
    SetFont(hButton, hFont);
    SetFont(hModeGroup, hFont);
    SetFont(hCheckBox, hFont);
    SetFont(hMode1, hFont);
    SetFont(hMode2, hFont);

    break;
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    // TODO:  在此添加任意绘图代码...
    EndPaint(hWnd, &ps);
    break;
  case WM_CTLCOLORSTATIC:
    return (INT_PTR)hBrushWhite;  //返回画刷句柄，给静态文本框着色
  case WM_COMMAND:
    //处理按钮事件
    wmID = LOWORD(wParam); //被触发的按钮id
    wmEvent = HIWORD(wParam); //被触发的按钮事件
    
    switch (wmID)
    {
    case 3:  // 翻译按钮
      TCHAR buff[1000];
      GetWindowText(hInBox, buff, 1000);
      szInText.clear();
      szInText.append(buff);
      szInText = string_To_UTF8(szInText);
      Translate();
      SetWindowText(hOutBox, szOutText.c_str());
    default:
      //不处理的消息要交给 DefWindowProc 处理。
      return DefWindowProc(hWnd, message, wParam, lParam);
    }

  case WM_DESTROY:
    DeleteObject(hFont);//删除创建的字体

    PostQuitMessage(0);
    break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
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
    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_LOWERCASE | ES_AUTOVSCROLL,
    10, 40, 460, 200,
    hWnd, (HMENU)1, hInst, NULL
  );
}

HWND InitOutBox(HWND hWnd, HINSTANCE hInstance)
{
  return CreateWindow(
    TEXT("edit"),     //框的类名
    TEXT("C语言中文网hhhhhhhhfasff发生发士hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh大夫asfsadfsa"),  //控件的文本
    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_LOWERCASE | ES_AUTOVSCROLL | ES_READONLY,
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
    TEXT("button"), TEXT("英文 → 中文"),
    WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTORADIOBUTTON,
    110, 15, 120, 40,
    hPreWnd, (HMENU)6, hInst, NULL
  );
}

HWND InitMode2(HWND hPreWnd, HINSTANCE hInstance)
{
  return CreateWindow(
    TEXT("button"), TEXT("中文 → 英文"),
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

void SetFont(HWND target, HFONT hFont)
{
  SendMessage(
    target,  //欲设置字体的控件句柄
    WM_SETFONT,  //消息名（消息类型）
    (WPARAM)hFont,  //字体句柄
    NULL  //传空值即可
  );
}

int Translate()
{
  CURL *curl;
  CURLcode res;
  static string appid = "xxxxxxxxxxxxxxxxx";    //replace myAppid with your own appid
  static string secret_key = "xxxxxxxxxxxxxxxxx2";   //replace mySecretKey with your own mySecretKey
  string q;          //replace apple with your own text to be translate, ensure that the input text is encoded with UTF-8!
  string from;          //replace en with your own language type of input text
  string to;            //replace zh with your own language type of output text

  //模式选择
  switch (mode)
  {
  case 1:
  {
    from = "auto";
    to = "zh";
    break;
  }
  case 2:
  {
    from = "en";
    to = "zh";
    break;
  }
  case 3:
  {
    from = "zh";
    to = "en";
    break;
  }
  default:
    return 0;
  }
  //数据请求
  curl = curl_easy_init();//初始化url环境
  if (curl)
  {
    //生成签名
    stringstream sign;
    q = szInText;
    int salt = rand();
    sign << appid << q << salt << secret_key;
    //MD5加密
    unsigned char md[16];
    char tmp[3] = { '\0' }, buf[33] = { '\0' };
    MD5((unsigned char*)sign.str().c_str(), sign.str().length(), md);
    for (int i = 0; i < 16; i++)
    {
      sprintf(tmp, "%2.2x", md[i]);
      strcat(buf, tmp);
    }    //得到MD5加密后的串 buf

    //生成url请求
    stringstream myurl;
    char * q_encoded = curl_easy_escape(curl, q.c_str(), q.length());  //对待翻译数据进行URL_encode编码
    myurl << "http://api.fanyi.baidu.com/api/trans/vip/translate?"
      << "q=" << q_encoded
      << "&from=" << from
      << "&to=" << to
      << "&appid=" << appid
      << "&salt=" << salt
      << "&sign=" << buf;
   
    //设置url属性
    string json_str;
    curl_easy_setopt(curl, CURLOPT_URL, myurl.str());   //地址
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);  //回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&json_str);//内容
    res = curl_easy_perform(curl);  //执行
    //解析json串
    Json::Reader reader;
    Json::Value value;
    if (reader.parse(json_str, value))
    {
      int sz = value["trans_result"].size();
      szOutText.clear();
      for (int i = 0; i < sz; ++i)
      {
        szOutText += (value["trans_result"][i]["dst"]).asString();
        szOutText += string("\r\n");
      }
      
    }
    //清理
    curl_free(q_encoded);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
  }
  /***********************
  * 返回值说明：
  * 52000	成功
  * 52001	请求超时
  * 52002	系统错误
  * 52003	未授权用户
  * 54000	必填参数为空
  * 54001	签名错误
  * 54003	访问频率受限
  * 54004	账户余额不足
  * 54005	长query请求频繁
  * 58000	客户端IP非法
  * 58001	译文语言方向不支持
  ***********************/
  return 0;
}

size_t write_func(void * contents, size_t size, size_t nmemb, void * userp)
{
  size_t realsize = size * nmemb;
  string *mem = (string*)userp;
  (*mem).append((char*)contents, realsize);
  return realsize;
}

std::string string_To_UTF8(const std::string & str)
{
  int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

  wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
  ZeroMemory(pwBuf, nwLen * 2 + 2);

  ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

  int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

  char * pBuf = new char[nLen + 1];
  ZeroMemory(pBuf, nLen + 1);

  ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

  std::string retStr(pBuf);

  delete[]pwBuf;
  delete[]pBuf;

  pwBuf = NULL;
  pBuf = NULL;

  return retStr;
}
