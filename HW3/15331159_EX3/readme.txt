编译命令为：
g++ -std=c++11 -o a.out canny.cpp HoughTransform.cpp main.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11
编译环境为：
Ubuntu 16.04 TLS 虚拟机
可执行文件：
a.out
得到的结果分别储存于dataset1与dataset2两个文件夹中，命名以"answer_"开头。
