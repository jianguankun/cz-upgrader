***
**<span class="size" style="font-size:18px">如何安装upgmake</span>**
***

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
vim \~/.bash\_profile加一句

```
PATH=$PATH:/usr/local/upgrade/sbin/
```

source \~/.bash\_profile
到任意地方试一下upgmake有如下输出则表明upgmake已正常安装

***
**<span class="size" style="font-size:18px">创建项目</span>**
***

1.从模板中复制出新项目
```
cp -r  /darta/www/demo /darta/www/[NewProject]
```

2.修改[NewProject]/game下的gameinfo文件的第一句httpserver
```
local httpserver = "http://upgrade.wsqpg.com/[NewProject]"
```

3.确保游戏目录下有1.00.xml文件

4.记录新项目的后台信息

5.打开后台全局参数，修改7998与8000两个参数。
```
https://fir.im/xxx;{"hall":"http://upgrade.wsqpg.com/[ProjectName]/hall/","game": "http://upgrade.wsqpg.com/[ProjectName]/games/"}
http://ios_full_package_url;{"hall":"http://upgrade.wsqpg.com/[ProjectName]/hall/","game" :"http://upgrade.wsqpg.com/[ProjectName]/games/"}  
```

6.修改[NewProject]/hall下的config文件的project_name与tellup_url
   注意测试一下shell脚本./tellup.sh,打开后台看看全局参数的8004是否修被改正确.
   
   
***  
**<span class="size" style="font-size:18px">创建系统用户</span>**
***

1.创建用户
```
useradd [User] -g root -d /home/[User]/
```

2.设置密码
```
passwd [User]
```

3.设置用户文件目录的访问权限
```
setfacl -R -m u:[User]:r-x /data/www
setfacl -R -m u:[User]:rwx /data/www/[ProjectName]
```

4.设置用户可以直接upgmake等命令
修改/home/[User]/.bash_profile，PATH追加/usr/local/upgrade/sbin/
```
source /home/[User]/.bash_profile
```

5.添加命令防误操作的设置
打开/home/[User]/.bashrc文件，添加
```
alias rm='rm -i'
alias cp='cp -i'
alias mv='mv -i'
```
注意soucrce一下
