#ifndef UNIT_H
#define UNIT_H

#include <iostream>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <string>
#include <map>
#include "3rd/json/nlohmann/json.hpp"

using njson = nlohmann::json;

class Unit{
public:
    Unit(njson);
    Unit();

    // 显示部队基本信息
    void show();

    // 部队实际受损
    void injured(double);

    std::string name;
    int type;        // 部队类型 0,1,2,3,4,5
    bool is_ranged;  // 是否远程

    int troops;      // 面板血量
    double blood;    // 隐藏血量

    double attack1;  // 最低攻击
    double attack2;  // 最高攻击
    double defence;  // 防御
    double charge;   // 冲锋率
    double detour;   // 迂回率
    double counter;  // 多段反击率
    double dodge;    // 闪避率
    int mobility;    // 

    int current_troops;  // 剩余血量

    // 还击次数，用于攻击中的计数
    int counter_num;
    
    std::vector<std::string> skills;   // 部队自带技能
    std::unordered_set<std::string> full_skills;   // 结合军团技能后的全部技能
};

class Legion{
public:
    Legion(std::string = "default");

    // 设定部队的全部技能
    void adjust_skills();

    // 添加部队
    void add_unit(const Unit&, const std::map<std::string, bool>&);

    // 移除部队
    void remove_unit(int, const std::map<std::string, bool>&);

    // 显示军团血量
    void show_troops();

    std::string legion_name;
    std::vector<Unit> units;   // [1, 2, 3] 三编
    std::unordered_set<std::string> legion_skills;    // 军团技能
    int unit_num;     // 军团部队数量
    
};

#endif