**如何安装upgmake**

1.安装gcc与git
```
yum -y install gcc-c++
yum -y install git
```

2.编译并安装upgmake
```
cd /usr/local/src
git clone https://github.com/jianguankun/cz-upgrader.git
cd cz-upgrader/server
make && make install
```

3.设置环境变量
```
vim ~/.bash_profile加一句
PATH=$PATH:/usr/local/upgrade/sbin/
```
