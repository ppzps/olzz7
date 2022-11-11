#ifndef UNIT_H
#define UNIT_H

#include <iostream>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <string>
#include <map>
#include <cassert>
#include "3rd/json/nlohmann/json.hpp"

using njson = nlohmann::json;

// 基础攻击消耗
const int base_attack_mob = 6;

class General;
class Unit;
class Legion;

enum Weapons{assualt, polearm, shooter, swordman, skirmisher};

// 将领（待实现）
class General{
public:
    // 基本构造
    General(std::string = "");

    // 从json中构造
    General(njson);

    // 显示
    void show();

    std::string name;  // 将领名字
    bool has_unique;   // 是否有专属
    std::string unit_name; // 专属名字

    int infantry_abi;  // 步兵能力   涵盖了装备，宝石和将领本身能力
    int cavalry_abi;   // 骑兵能力
    int ranged_abi;    // 远程能力

    // 这个需要自己算然后带入
    double legion_dmg_inc1;  // 有专属的军团增伤
    double legion_dmg_inc2;  // 没有专属兵种的军团增伤  包括爵位，增伤装备宝石，减伤装备宝石, 地形
    double legion_dmg_red;  // 军团减伤

    // -------将领被动技能---------
    // 大师类技能
    double infantry_tactician;  // 步兵增伤
    double cavalry_tactician;  // 骑兵增伤
    double ranged_tactician;  // 远程增伤
    // 专家类技能
    double melee_infantry_expert;  // 近步专家
    double melee_cavalry_expert;  // 近骑专家
    double defence_infantry_expert; // 防步专家 （减伤）
    double charging_cavalry_expert; // 冲骑专家 （加冲锋率）
    // 武器类技能
    double assault_master;  // 突击大师（战斧，双手剑，双持武器）
    double polearm_master;  // 长柄武器大师
    double shooter_master;  // 射击大师 弓，弩，火枪
    double swordman_master; // 剑术大师 单手剑，骑士剑
    double skirmisher_master; // 投掷大师
};

// 部队
class Unit{
public:
    Unit(njson);
    Unit();

    // 显示部队基本信息
    void show();

    // 部队实际受损
    void injured(double);

    // 添加将领
    void add_general(const General*);

    // 判断武器类型
    Weapons judge_weapon() const;

    std::string name;
    int type;        // 部队类型 0,1,2,3,4,5
    bool is_ranged;  // 是否远程

    int level;       // 部队等级

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

    // 在编队中的位置，0,1,2 0表示前排
    int fleet;

    // 所属军团
    const Legion* plegion;

    // 所属将领
    const General* pgeneral;
    
    std::vector<std::string> skills;   // 部队自带技能
    std::unordered_set<std::string> full_skills;   // 结合军团技能后的全部技能
};

// 军团
class Legion{
public:
    Legion(std::string = "default");

    // 设定部队的全部技能
    void adjust_skills();

    // 调整军团行动力
    void adjust_mobility();

    // 调整军团攻击消耗
    void adjust_attack_mob();

    // 添加部队到最后一排，目前只需要这个功能
    void add_unit(const Unit&, const std::map<std::string, bool>&);

    // 移除部队
    void remove_unit(int, const std::map<std::string, bool>&);

    // 添加将领
    void add_general(const General*);

    // 判断军团里是否有专属兵种
    bool is_unique_unit() const;

    // 显示军团血量
    void show_troops();

    // 将领相关
    const General* pgeneral;

    // 军团移动力
    int mobility;
    int current_mobility;

    // 攻击消耗
    int attack_mobility;

    // 进攻时的标志
    bool is_charge;
    bool is_detour;

    // 军团目前的buff  待实现  着火伤害，长弓行动力+1，等等
    // std::unordered_set<std::string> legion_buffs;

    int unit_num;     // 军团部队数量
    std::string legion_name;   // 军团名称

    std::vector<Unit> units;   // [1, 2, 3] 三编
    std::unordered_set<std::string> legion_skills;    // 军团技能
    
    std::unordered_map<std::string, int> buffs; // 军团的效果
};


#endif