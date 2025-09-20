#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h> 
#define TTY_PATH  "/dev/tty"  //控制终端输入输出
#define STTY_US  "stty raw -echo -F "  //将终端设置为原始模式
#define STTY_DEF  "stty -raw echo -F "  //将终端恢复为默认模式
//宏定义好处１可以增强代码可读性　　２　方便修改
#define WIDTH   15  //游戏区域的宽度,左右边界
#define HEIGHT  20  //游戏区域的高度,上下边界
 struct Point     //存储方块的相对坐标
 {
    int x;
    int y;
 };
 //图形库
struct Point shapes[19][4] = {
    {{0,0},{-1,0},{1,0},{2,0}},//横条 
    {{0,0},{0,-1},{0,1},{0,2}},//竖条 
    {{0,0},{-1,-1},{-1,0},{0,-1}},//方块  
    {{0,0},{0,-1},{0,-2},{1,0}},//正L1   
    {{0,0},{0,1},{1,0},{2,0}},//正L2
    {{0,0},{-1,0},{0,1},{0,2}},//正L3    
    {{0,0},{0,-1},{-1,0},{-2,0}},//正L4  
    {{0,0},{-1,0},{0,-1},{0,-2}},//反L1  
    {{0,0},{0,-1},{1,0},{2,0}},//反L2    
    {{0,0},{1,0},{0,1},{0,2}},//反L3 
    {{0,0},{-1,0},{-2,0},{0,1}},//反L4   
    {{0,0},{-1,0},{1,0},{0,-1}},//T1    
    {{0,0},{0,1},{0,-1},{1,0}},//T2 
    {{0,0},{-1,0},{1,0},{0,1}},//T3 
    {{0,0},{-1,0},{0,-1},{0,1}},//T4    
    {{0,0},{1,0},{0,-1},{-1,-1}},//正Z1  
    {{0,0},{1,-1},{0,1},{1,0}},//正Z2    
    {{0,0},{1,-1},{-1,0},{0,-1}},//反Z1  
    {{0,0},{-1,-1},{-1,0},{0,1}}//反Z2
 };
int map[HEIGHT][WIDTH] = {0};//表示静态图形的二维数组 0没有图形 1有图形 2边界
void drawPoint(int x, int y);//在终端绘制方块
int myget();
void drawShape();//绘制当前方块和静态地图
int userCtrl();
void initMap();//设置边界
void createShape();//生成新方块
void transform();//旋转方块
int collide(int dirX, int dirY, int index);//检测碰撞
void moveDown();//向下移动
void addToMap();//加入静态图形
void deleteLine();//消行
int isFull(int line);//判断行满
void clearLine(int line);//清除行
void moveLine(int dst, int src);
int gameOver();//判断游戏结束
void drawGameOver();//显示结束画面
int x = 10, y = 10;//图形位置
int shapeIndex = 0;//图形下标,是二位数组shapes的第一个下标
int main()
 {
    int downCD = 0;
    srand(time(NULL));
    initMap();
    createShape();//创建第一个图形
    system(STTY_US TTY_PATH);
    while(1)
    {
        system("clear");
        drawShape();//绘制图形
        if(++downCD == 10)
        {
            moveDown();//自由下落
            downCD = 0;//CD清零
        }
            
        if(userCtrl() == 0)//用户控制
        {
            break;
        }
        if(gameOver() == 1)
        {
            drawGameOver();
            system(STTY_DEF TTY_PATH);
            break;
        }
        usleep(50*1000);
    }
    return 0;
}
 //结束返回１　否则返回０
int gameOver()
 {
    int i;
    //遍历最顶行，只要有静态图形就失败
    for(i = 1;i < WIDTH-2;i++)
    {
        if(map[1][i] != 0)
        {
            return 1;
        }
    }
    return 0;
 }
 void addToMap()
 {
    //遍历图形，在ｍａｐ中找到与图形４个点对应的４个元素，将值置为１
    int i;
    for(i = 0;i < 4;i++)
    {
        int lx = shapes[shapeIndex][i].x+x;
        int ly = shapes[shapeIndex][i].y+y;
        map[ly][lx] = 1;
    }
 }
 void moveDown()
 {
    if(collide(0, 1, shapeIndex)==0)//没有碰撞就移动
        y++;
    else
    {
        addToMap();//加入静态图形
        deleteLine();//消行
        createShape();//创建新图形
    }
 }
//参数１和参数２表示要判断的方向　-1 0左　1 0右　0　1下
//参数３用于变形碰撞
//有碰撞返回１　　没碰撞返回 0
 int collide(int dirX, int dirY, int index)
 {
    int i;
    for(i = 0;i < 4;i++)
    {
        //计算图形移动后的点位置
        int collideX = shapes[index][i].x+x+dirX;
        int collideY = shapes[index][i].y+y+dirY;
        if(map[collideY][collideX] != 0)//有碰撞
        {
            return 1;
        }
    }
    return 0;
 }
//变形
void transform()
 {
    int lindex = shapeIndex;//lindex用来判断碰撞
    switch(lindex)
    {
    case 6://正Ｌ４
    case 10://反Ｌ４
    case 14://ｔ４
        lindex -= 3;
        break;
    case 1://横
    case 16://正Ｚ２
    case 18://反Ｚ２
        lindex--;
        break;
    case 2://方块
        break;
    default:
        lindex++;
    }
    
    if(collide(0, 0, lindex) == 0)//变形后没有碰撞才完成变形
    {
        shapeIndex = lindex;//完成变形
    }
 }
 //创建新图形
void createShape()
 {
    shapeIndex = rand()%19;//随机一个图形
    //初始位置
    x = WIDTH/2;
    y = 2;
 }
//初始化游戏地图,设置边界
 void initMap()
 {
    int i;
    //左右边界，遍历Ｙ轴
    for(i = 0;i < HEIGHT;i++)
    {
        map[i][0] = 2;//左边值
        map[i][WIDTH-1] = 2;//右边值
    }
    //上下边界，遍历Ｘ轴
    for(i = 0;i < WIDTH;i++)
    {
        map[0][i] = 2;//上边值
        map[HEIGHT-1][i] = 2;//下边值
    }
 }
 //用户控制　返回值１继续游戏　　０结束游戏
int userCtrl()
 {
    char input = myget();//输入
    switch(input)
    {
    case 'a'://左
        if(collide(-1, 0, shapeIndex)==0)//没有碰撞就移动
            x--;
        break;
    case 's'://下
        moveDown();
        break;
    case 'd'://右
        if(collide(1, 0, shapeIndex)==0)//没有碰撞就移动
            x++;
        break;
    case 'w'://变形
        transform();
        break;
    case 'q':
        system(STTY_DEF TTY_PATH);
        return 0;
    }
    return 1;
 }
 //画图形
void drawShape()
 {
    //绘制动态
    int i, j;
    for(i = 0;i <4;i++)
    {
    //只要shapeIndex的值变了，显示的图形就变了
        int lx = x + shapes[shapeIndex][i].x;
        int ly = y + shapes[shapeIndex][i].y;
        drawPoint(lx, ly);
    }
    
    //绘制静态图形
    for(i = 0;i < HEIGHT;i++)
    {
        for(j = 0;j < WIDTH;j++)
        {
            if(map[i][j] != 0)//有图形
            {
                drawPoint(j, i);
            }
        }
    }
    
    fflush(stdout);//清理输出缓存
}
 void drawPoint(int x, int y)
 {
    printf("\033[%d;%dH", y+1, x*2+1);
    printf("\033[1;36m■ \033[0m");
 }
void drawGameOver()
 {
    printf("\033[%d;%dH", HEIGHT/2+1, (WIDTH/2-2)*2+1);
    printf("\033[1;36mGAME OVER \033[0m");
    printf("\033[%d;%dH", HEIGHT+1, 0);
 }
 int myget() 
{
  fd_set rfds;
  struct timeval tv;
  int ch = 0;
  FD_ZERO(&rfds);
  FD_SET(0, &rfds);
  tv.tv_sec = 0;
  tv.tv_usec = 10; 
  if (select(1, &rfds, NULL, NULL, &tv) > 0)
  {
      ch = getchar(); 
  }
  return ch;
 }
 //消行
 //从底层向上遍历,调用两个函数,一个判断满,一个消除指定行
void deleteLine()
 {
    int count = 0;
    int i;
    //遍历Ｙ轴，略过上下边界
    for(i = HEIGHT-2;i > 0;i--)
    {
        if(isFull(i) == 1)//判断行满
        {
            count++;
            clearLine(i);//清理一行
        }
        else if(count > 0)
        {
            moveLine(i+count, i);//移动一行
        }
    }
 }
//满返回1  否则０
int isFull(int line)
 {
    //遍历一行，略过左右边界
    int i;
    for(i = 1;i < WIDTH-2;i++)
    {   
        if(map[line][i] == 0)
        {
            return 0;
        }
    }
    return 1;
 }
 //清理一行
void clearLine(int line)
{
 //遍历一行，略过左右边界
	int i;
	for(i = 1;i < WIDTH-2;i++)
	{   
	map[line][i] = 0;
	}
}
//下移行数据,将src行数据复制到dst行,并清空src行
 void moveLine(int dst, int src)
 {
 //遍历一行，略过左右边界
	int i;
	for(i = 1;i < WIDTH-2;i++)
	{   
	map[dst][i] = map[src][i];
	map[src][i] = 0;
	}
}
