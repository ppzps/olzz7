# olzz7
目前对应的版本是1.7.6
这是根据目前的结论，推测得到的军团野战模拟。

version 1.0 只适用于金色6级基础兵

version 1.1 添加了大部分佣兵团和对应的技能，并增加了为军团增加将领的功能

具体添加的兵种和相关技能可以看data/explain.txt

将领目前没有实现主动攻击的功能，但是各种增伤技能，宝石，装备都有相应的考虑，具体可以查看data/explain.txt

其他关于数据的说明可以看 data/explain.txt

## 运行方法
```shell
先在build文件夹中编译
g++ -o attack --std=c++11 ..\attack.cpp ..\damage_new.cpp ..\unit.cpp
然后就可以直接运行 
.\attack.exe
或者直接用已经编译后的attack.exe，点击即可直接运行
 ```

 ## 如何自定义自己的军团和将领
 ``` shell
 查看build/compare.json，可以看出有两个兵团以及对应的将领
 不选将领可以直接填写 ""
 选择将领可以从data/general.json中查找英文名字，并放在相应的位置上
 军团的添加是从前到后的列表，只需要填对部队的英文名字即可，具体可以在data/unit2.json或者data/explain.txt中查找

因为每个人将领的配置是不同的，所以需要修改或者添加data/general.json中的数据
具体可以查看data/explain.txt中general.json的部分
 ```
 

 version 1.0的相关结果

 build/test1.xlsx 包含了对金色基础兵的简单分析（依据军团对战的输赢） 

 build/test2.xlsx 包含了对金色基础兵的简单分析（依据军团对战后的剩余血量和掉编情况）

 
