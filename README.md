# olzz7
目前对应的版本是1.8
这是根据目前的结论，得到的军团伤害模拟器

version 1.2 添加了所有佣兵团和对应的技能

version 1.1 添加了大部分佣兵团和对应的技能，并增加了为军团增加将领的功能

version 1.0 只适用于金色6级基础兵

具体添加的兵种和相关技能可以看data/explain.txt

目前没有实现将领的主动技能和一些特殊的被动技能，但是各种增伤技能，宝石，装备都有相应的考虑，具体可以查看data/explain.txt

其他关于数据的说明可以看 data/explain.txt

## 运行方法
```shell
直接用已经编译后的attack.exe，点击即可直接运行
 ```

## 如何自定义自己的军团和将领
核心文件是 build/compare.json，可以看出有两个兵团以及对应的将领，第一个军团先手攻击，第二个军团后手攻击

选择将领可以从 data/general.json中查找英文名字，并放在相应的位置上， 不选将领可以直接填写 ""

军团的添加是从前到后的列表，第一编对应第一个名称，只需要填对部队的英文名字即可，具体可以在data/unit2.json或者data/explain.txt中查找, 目前只支持15级佣兵

### 将装备宝石结合进将领中
只需要手动计算一下装备加宝石加爵位的增减伤，对应填入下面的legin_dmg_inc1, legion_dmg_inc2和legion_dmg_red中即可，其中legion_dmg_inc1额外考虑了有专属兵种的10%或20%增伤，legion_dmg_inc2不考虑

下面是将领示例
```c++
    {
        "name": "Richard I",
        "infantry_abi": 42,  // 步兵能力
        "cavalry_abi": 126,  // 骑兵能力
        "ranged_abi": 40,    // 远程能力

        "has_unique": 1,     // 是否有专武
        "unit_name": "Knights Templar",  // 专属对应兵种
 
        "legion_dmg_inc1": 1.8515,    // 有专属兵种时的军团增伤
        "legion_dmg_inc2": 1.6215,    // 无专属兵种时的军团增伤
        "legion_dmg_red": 0.8,        // 军团整体减伤

        // 将领的相关被动技能，具体可查看 data/explain.txt
        "cavalry_tactician": 1.15,   // 骑兵战术大师
        "polearm_master": 1.15       // 长柄武器大师
    }
```
 


 
## 其他结果
version 1.0的相关结果

save/test1.xlsx 包含了对金色基础兵的简单分析（依据军团对战的输赢） 

save/test2.xlsx 包含了对金色基础兵的简单分析（依据军团对战后的剩余血量和掉编情况）

