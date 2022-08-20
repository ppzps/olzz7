# olzz7
目前对应的版本是1.7.6
这是根据目前的结论，推测得到的军团野战模拟。

version 1.0 只适用于金色6级基础兵

version 1.1 添加了大部分佣兵团和对应的技能，并增加了为军团增加将领的功能

具体添加的兵种和相关技能可以看data/explain.txt

将领目前没有实现主动攻击的功能，但是各种增伤技能，宝石，装备都有相应的考虑，具体可以查看data/explain.txt

其他关于数据的说明可以看 data/explain.txt

运行方法
```shell
先在build文件夹中编译
g++ -o attack --std=c++11 ..\attack.cpp ..\damage_new.cpp ..\unit.cpp
然后就可以直接运行 
.\attack.exe
或者直接用已经编译后的attack.exe，点击即可直接运行
 ```

 version 1.0
 build/test1.xlsx 包含了对金色基础兵的简单分析（依据军团对战的输赢） 
 build/test2.xlsx 包含了对金色基础兵的简单分析（依据军团对战后的剩余血量和掉编情况）

 
