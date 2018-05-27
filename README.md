# cz-upgrader
一套软件更新服务器组件，比较适合用于游戏领域


1.安装gcc与git
yum -y install gcc-c++
yum -y install git

2.编译并安装upgmake
cd /usr/local/src
git clone https://github.com/jianguankun/cz-upgrader.git
cd cz-upgrader/server
make && make install

3.设置环境变量
vim ~/.bash_profile加一句
PATH=$PATH:/usr/local/upgrade/sbin/

source ~/.bash_profile
到任意地方试一下upgmake有如下输出则表明upgmake已正常安装
