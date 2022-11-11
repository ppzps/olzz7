#include <cstdlib>
#include <algorithm>
#include <cassert>

#include "damage.h"
#include "unit.h"

using namespace std;

// 生成均匀分布 uniform rand
double uniform_rand(double lower, double upper){
    return lower + ((double)rand() / (RAND_MAX+1.0)) * (upper - lower);
}
// 生成随机分布后取整
int uniform_rand_int(double lower, double upper){
    return static_cast<int>(uniform_rand(lower, upper));
}

// 判断各种概率是否发生
bool judge_prob(double thr, double lower, double upper){
    if (thr < 0.001) return false;  // 0概率事件

    double a = uniform_rand(lower, upper);
    if (a < thr) return true;
    else return false;
}

// 单纯计算攻击伤害 A->D 且A为近程
// 不需要考虑冲锋，迂回，闪避，多段反击等因素, 但是考虑了将领
// 现在兵种的第一技能还没有改，需要一些测试和数据 ----------------------- 

double melee_calc_single(const Unit& A, const Unit& D){
    assert(!A.is_ranged);
    // 先计算基础伤害
    double dmg = convert_troops(A.current_troops);
    if (A.full_skills.count("Sniper") > 0){
        dmg *= A.attack2;
    }
    else{
        dmg *= uniform_rand_int(A.attack1, A.attack2);
    }

    // if (A.full_skills.count("Trample") > 0) dmg *= convert_defence(D.defence, 5); 
    dmg *= convert_defence(D.defence, 0);

    // 考虑技能和隐藏血量的影响
    double alpha = D.blood;

    if (A.full_skills.count("Single-Handed Sword 3") > 0 && D.type == 4) alpha *= 1.25;
    if (A.full_skills.count("Hengdao 3") > 0 && D.type == 4) alpha *= 1.25;
    if (A.full_skills.count("Double-Handed Sword 3") > 0 && D.type == 4) alpha *= 1.3;
    if (A.full_skills.count("Dual-Wielded Weapon 3") > 0 && D.type == 4) alpha *= 1.3;
    if (A.full_skills.count("War Axe 3") > 0 && D.type == 4) alpha *= 1.3;

    if (A.full_skills.count("Modao 3") > 0 && D.type == 3) alpha *= 1.3;
    if (A.full_skills.count("Halberd 3") > 0 && D.type == 3) alpha *= 1.3;
    if (A.full_skills.count("Long Spear 3") > 0 && D.type == 3) alpha *= 1.25;

    if (A.full_skills.count("Knight Sword 3") > 0 && D.type == 5) alpha *= 1.25;

    if (A.full_skills.count("Knight Spear 3") > 0 && D.type == 0) alpha *= 1.3;
    if (A.full_skills.count("War Halberd 3") > 0 && D.type == 0) alpha *= 1.35;
    if (A.full_skills.count("Knight Halberd 3") > 0){
        if (D.type == 0) alpha *= 1.25;
        else alpha *= 1.05;
    }

    if (A.full_skills.count("Thump") > 0 && (D.type == 2 || D.type == 3)) alpha *= 1.05;

    if (A.full_skills.count("Single-Handed Sword 3") > 0 && D.type == 4) alpha *= 1.25;
    if (A.full_skills.count("Hengdao 3") > 0 && D.type == 4) alpha *= 1.25;
    if (A.full_skills.count("Dual-Wielded Weapon 3") > 0 && D.type == 4) alpha *= 1.3;
    if (A.full_skills.count("Modao 3") > 0 && D.type == 3) alpha *= 1.3;
    if (A.full_skills.count("Halberd 3") > 0 && D.type == 3) alpha *= 1.3;
    if (A.full_skills.count("Long Spear 3") > 0 && D.type == 3) alpha *= 1.25;
    if (A.full_skills.count("Knight Sword 3") > 0 && D.type == 5) alpha *= 1.25;
    if (A.full_skills.count("War Axe 3") > 0 && D.type == 4) alpha *= 1.3;
    if (A.full_skills.count("Knight Spear 3") > 0 && D.type == 0) alpha *= 1.3;
    if (A.full_skills.count("War Halberd 3") > 0 && D.type == 0) alpha *= 1.35;

    if (D.full_skills.count("Square") > 0 && (A.type == 2 || A.type == 3)) alpha *= 0.85;
    if (D.full_skills.count("Deterrence") > 0 && A.type == 0) alpha *= 0.85;
    if (D.full_skills.count("Resolute") > 0 && (A.type == 2 || A.type == 3)) alpha * 0.8;

    // 考虑将领对系数的影响
    if (A.pgeneral){
        // 军团增伤
        if (A.plegion->is_unique_unit()) alpha *= A.pgeneral->legion_dmg_inc1;
        else alpha *= A.pgeneral->legion_dmg_inc2;
        // 兵种大师增伤
        if (A.type == 0 || A.type == 1) alpha *= A.pgeneral->infantry_tactician;
        if (A.type == 2 || A.type == 3) alpha *= A.pgeneral->cavalry_tactician;
        if (A.type == 4 || A.type == 5) alpha *= A.pgeneral->ranged_tactician;
        // 兵种专家增伤
        if (A.type == 0) alpha *= A.pgeneral->melee_infantry_expert;
        if (A.type == 2) alpha *= A.pgeneral->melee_cavalry_expert;
        // 武器增伤
        if (A.judge_weapon() == swordman) alpha *= A.pgeneral->swordman_master;
        else if (A.judge_weapon() == assualt) alpha *= A.pgeneral->assault_master;
        else if (A.judge_weapon() == polearm) alpha *= A.pgeneral->polearm_master;
        else cout << "weird melee weapon" << endl;
        
    }
    if (D.pgeneral){
        alpha *= D.pgeneral->legion_dmg_red;
        if (D.type == 1) alpha *= D.pgeneral->defence_infantry_expert;
    }

    // 考虑将领对系数的影响
    ;

    return dmg * alpha;
}

// 单纯计算攻击伤害 A->D 且A为远程 
// 不需要考虑冲锋，迂回，闪避，多段反击等因素，但是考虑了将领
// 现在兵种的第一技能还没有改，需要一些测试和数据 ----------------------- 
double ranged_calc_single(const Unit& A, const Unit& D, int fleet){
    assert(A.is_ranged);
    // 先计算基础伤害
    double dmg = convert_troops(A.current_troops);
    if (A.full_skills.count("Sniper") > 0){
        dmg *= A.attack2;
    }
    else{
        dmg *= uniform_rand_int(A.attack1, A.attack2);
    }
    
    // 分类讨论无视防御的情况
    if (A.full_skills.count("Crossbow 3") > 0 && (D.type == 1 || D.type == 2)){
        dmg *= convert_defence(D.defence, 25);
    }
    else if (A.full_skills.count("Crossbow 2") > 0 && (D.type == 1 || D.type == 2)){
        dmg *= convert_defence(D.defence, 20);
    }
    else if (A.full_skills.count("Throwing Axe 3") > 0 && (D.type == 1 || D.type == 2)){
        dmg *= convert_defence(D.defence, 20);
    }
    else if (A.full_skills.count("Arquebus 1") > 0){
        dmg *= convert_defence(D.defence, 20);
    }
    else if (A.full_skills.count("Arquebus 2") > 0){
        dmg *= convert_defence(D.defence, 25);
    }
    else if (A.full_skills.count("Arquebus 3") > 0){
        dmg *= convert_defence(D.defence, 30);
    }
    else{
        dmg *= convert_defence(D.defence, 0);
    }

    double alpha = D.blood;
    double ranged_alpha = 0;
    if (fleet == 0) ranged_alpha = ranged_alpha1;
    else if (fleet == 1) ranged_alpha = ranged_alpha2;
    else if (fleet == 2) ranged_alpha = ranged_alpha3;
    else {cout << "fleet error!" << endl; return -1;} 

    alpha *= ranged_alpha;       // 远程系数

    if (A.full_skills.count("Rock Slinger 3") > 0 && D.type == 1) alpha *= 1.05; // 投石3
    if (A.full_skills.count("Rock Slinger 3") > 0 && D.type == 2) alpha *= 1.16; // 投石3

    if (A.full_skills.count("Javelin 3") > 0 && D.type == 1) alpha *= 1.05; // 标枪3
    if (A.full_skills.count("Javelin 3") > 0 && D.type == 2) alpha *= 1.16; // 标枪3

    if (A.full_skills.count("Archery 3") > 0 && D.type == 1) alpha *= 1.05;  // 弓箭3
    if (A.full_skills.count("Archery 3") > 0 && D.type == 2) alpha *= 1.2;  // 弓箭3

    if (A.full_skills.count("Longbow 3") > 0 && D.type == 1) alpha *= 1.05; // 长弓3
    if (A.full_skills.count("Longbow 3") > 0 && D.type == 2) alpha *= 1.25; // 长弓3

    if (A.full_skills.count("Flame Longbow 3") > 0 && D.type == 1) alpha *= 1.05; // 火长弓3
    if (A.full_skills.count("Flame Longbow 3") > 0 && D.type == 2) alpha *= 1.25; // 火长弓3

    if (A.full_skills.count("Throwing Axe 3") > 0 && D.type == 1) alpha *= 1.05; // 飞斧3
    if (A.full_skills.count("Throwing Axe 3") > 0 && D.type == 2) alpha *= 1.16; // 飞斧3

    if (A.full_skills.count("Crossbow 3") > 0 && D.type == 1) alpha *= 1.05; // 十字弩3
    if (A.full_skills.count("Crossbow 3") > 0 && D.type == 2) alpha *= 1.22; // 十字弩3
    if (A.full_skills.count("Crossbow 2") > 0 && D.type == 1) alpha *= 1.05; // 十字弩2
    if (A.full_skills.count("Crossbow 2") > 0 && D.type == 2) alpha *= 1.17; // 十字弩2

    if (A.full_skills.count("Arquebus 1") > 0 && D.type == 2) alpha *= 1.15; // 火枪1
    if (A.full_skills.count("Arquebus 2") > 0 && D.type == 2) alpha *= 1.2;  // 火枪2
    if (A.full_skills.count("Arquebus 3") > 0 && D.type == 2) alpha *= 1.25; // 火枪3
    if (A.full_skills.count("Arquebus 1") > 0 && D.type == 1) alpha *= 1.05; // 火枪1
    if (A.full_skills.count("Arquebus 2") > 0 && D.type == 1) alpha *= 1.05; // 火枪2
    if (A.full_skills.count("Arquebus 3") > 0 && D.type == 1) alpha *= 1.05; // 火枪3

    if (A.full_skills.count("Liannu 3") > 0 && D.type == 1) alpha *= 1.3; // 连弩3

    if (A.full_skills.count("Heavy Spear 3") > 0 && D.type == 1) alpha *= 1.16; // 骑士投枪3
    if (A.full_skills.count("Heavy Spear 3") > 0 && D.type == 2) alpha *= 1.05; // 骑士投枪3

    if (A.full_skills.count("Knight Bow 3") > 0 && D.type == 1) alpha *= 1.20; // 骑士弓3
    if (A.full_skills.count("Knight Bow 3") > 0 && D.type == 2) alpha *= 1.05; // 骑士弓3
    // ----------------

    if (D.full_skills.count("Focused Defence") > 0) {alpha *= 0.85;}   // 集中防御
    if (D.full_skills.count("Top shield") > 0) {alpha *= 0.8;}       // 顶盾
    if (D.full_skills.count("Dragon Shield") > 0) {alpha *= 0.8;}    // 龙盾
    if (D.full_skills.count("Massive Shield") > 0) {alpha *= 0.5;}   // 巨盾
    if (D.full_skills.count("Back row") > 0) {alpha *= 0.9;}   // 后排

    // 连击
    if (A.full_skills.count("Combo") > 0){
        if (judge_prob(20)) alpha *= 2.0; // 连击
    }

    // 考虑将领对系数的影响
    if (A.pgeneral){
        // 军团增伤
        if (A.plegion->is_unique_unit()) alpha *= A.pgeneral->legion_dmg_inc1;
        else alpha *= A.pgeneral->legion_dmg_inc2;
        // 兵种大师增伤
        if (A.type == 4 || A.type == 5) alpha *= A.pgeneral->ranged_tactician;
        // 武器增伤
        if (A.judge_weapon() == shooter) alpha *= A.pgeneral->shooter_master;
        else if (A.judge_weapon() == skirmisher) alpha *= A.pgeneral->skirmisher_master;
        else cout << "weird melee weapon" << endl;
        
    }
    if (D.pgeneral){ 
        alpha *= D.pgeneral->legion_dmg_red;
    }
    return dmg * alpha;
}

// 军团对战  去掉了之前进攻方前后排的错误结论
// A军团进攻D军团
void legion_fight(Legion& A, Legion& D, const std::map<std::string, bool> &skill_list, bool verbose){
    // 三个阶段 远程攻击， 远程还击， 近战攻防
    // i <-> A  下标的默认对应关系
    // j <-> D
    
    int temp_record; // 辅助删除单位的变量

    if (A.unit_num == 0 || D.unit_num == 0){ // for debug
        cout << "empty legion!" << endl;
        cout << "------------------------" << endl;
        A.show_troops();
        cout << "------------------------" << endl;
        D.show_troops();
        cout << "------------------------" << endl;
        return;
    }

    // 记录本次攻击是否发生迂回或冲锋
    A.is_charge = false;
    A.is_detour = false;
    
    //---------------------
    // step0  行动力相关  + buff 相关
    //---------------------

    // 行动力减少，这样做是为了复现圣墓技能和冲骑技能的bug
    A.current_mobility -= A.attack_mobility;
    if (A.buffs.count("Fire") > 0 && A.buffs["Fire"] > 0){   // 长弓 效果
        A.current_mobility--;
    }
    if (A.legion_skills.count("Repeated Fire") > 0 && A.current_mobility > 0){   // 技能速射
        A.current_mobility++;
    }
    
    // 行动力相关技能
    double mob_alpha = 1.0;

    if (A.legion_skills.count("Charge") > 0 && A.current_mobility > 5) mob_alpha *= 1.1;  // 冲击技能
    if (A.legion_skills.count("Guardians") > 0){    // 守护者技能
        if (A.current_mobility <= 2) mob_alpha *= 1.2;
        else if (A.current_mobility <= 5) mob_alpha *= 1.1;
        else if (A.current_mobility <= 8) mob_alpha *= 1.05;
        else ;
    }
    
    // A 给 B 加buff
    if (A.legion_skills.count("Fire") > 0){
        D.buffs["Fire"] = 1;
    }

    // buff 相关的伤害，适用于军团整体
    double buff_A_atk_alpha = 1;
    double buff_A_def_alpha = 1;
    double buff_D_atk_alpha = 1;
    double buff_D_def_alpha = 1;
    // 不朽
    {
        double temp =(100 - (double)D.buffs["Immortality"] * 10.0) / 100;
        buff_D_def_alpha *= temp;
        temp =(100 - (double)A.buffs["Immortality"] * 10.0) / 100;
        buff_A_def_alpha *= temp;
    }
    // 圣殿 和 条顿 
    if (A.legion_skills.count("Banner") > 0) buff_A_atk_alpha *= 1.05;
    if (D.legion_skills.count("Banner") > 0) buff_D_atk_alpha *= 1.05;
    if (A.legion_skills.count("Fearless") > 0) buff_A_def_alpha *= 0.95;
    if (D.legion_skills.count("Fearless") > 0) buff_D_def_alpha *= 0.95;

    //---------------------
    // step1  远程攻击
    //---------------------

    // A->D 记录表  如dmg_rec_ad[1][2] 表示A第2编攻击D第3编
    // step 1-1  记录远程伤害
    vector<vector<double>> dmg_rec_ad(A.unit_num, vector<double>(D.unit_num, -2.0));
    for (int i = 0; i < A.unit_num; ++i){
        if (!A.units.at(i).is_ranged) continue; // 非远程，跳过计算
        for (int j = 0; j < D.unit_num; ++j){
            double dmg = ranged_calc_single(A.units.at(i), D.units.at(j), j) * mob_alpha * buff_A_atk_alpha * buff_D_def_alpha;
            dmg_rec_ad[i][j] = dmg;
        }
    }
    
    // step 1-2 结算进攻方远程伤害，考虑闪避值的影响
    for (int j = 0; j < D.unit_num; ++j){
        for (int i = 0; i < A.unit_num; ++i){
            double dodge_add = 0;
            if (A.units.at(i).full_skills.count("Arrow Rain") > 0) dodge_add = -60.0;
            if (!judge_prob(D.units.at(j).dodge + dodge_add))   // 闪避失败
                D.units.at(j).injured(dmg_rec_ad[i][j]);
        }
    }

    //---------------------
    // step2  远程还击
    //---------------------

    // D->A 记录表 
    vector<vector<double>> dmg_rec_da(D.unit_num, vector<double>(A.unit_num, -2.0));
    // step 2-1 记录远程反击伤害
    for (int j = 0; j < D.unit_num; ++j){
        if (!D.units.at(j).is_ranged) continue;   // 非远程，跳过
        for (int i = 0; i < A.unit_num; ++i){
            double dmg = ranged_calc_single(D.units.at(j), A.units.at(i), i) * buff_D_atk_alpha * buff_A_def_alpha;
            dmg_rec_da[j][i] = dmg;
        }
    }

    // step 2-2 结算远程反击伤害
    for (int i = 0; i < A.unit_num; ++i){
        for (int j = 0; j < D.unit_num; ++j){
            double dodge_add = 0;
            if (D.units.at(j).full_skills.count("Arrow Rain") > 0) dodge_add = -60.0;
            if (!judge_prob(A.units.at(i).dodge + dodge_add))   // 闪避失败
                A.units.at(i).injured(dmg_rec_da[j][i]);
        }
    }

    //---------------------
    // step3  近战攻防
    //---------------------

    // step3-1 清理远程攻击后 防守方死亡的单位
    int remove_num = 0;   // 用于删除部队
    temp_record = D.unit_num;
    for (int j = 0; j < temp_record; ++j){
        if (D.units.at(j-remove_num).current_troops == 0){
            D.remove_unit(j - remove_num, skill_list);
            remove_num++;
        }
    }
    // 判断是否攻击结束
    if (D.unit_num == 0){
        if (verbose){
            cout << D.legion_name << " is completely annihilated!" << endl;
        }
        // 结算A部队剩余情况
        remove_num = 0;
        temp_record = A.unit_num;
        for (int i = 0; i < temp_record; ++i){
            if (A.units.at(i-remove_num).current_troops == 0){
                A.remove_unit(i - remove_num, skill_list);
                remove_num++;
            }
        }
        if (A.unit_num == 0){
            if (verbose){
                cout << A.legion_name << " is hit back and gone." << endl;
            }
        }
        return;  
    }

    // step3-2 开始近战攻击流程

    // step3-2-1 先判断是否迂回
    vector<int> detour_flags(3, 0);
    for (int i = 0; i < A.unit_num; ++i){
        if (A.units.at(i).is_ranged) continue;
        double detour_add = 0;
        if (A.units.at(i).full_skills.count("Thousand miles raid") > 0){  // 千里奔袭
            detour_add = 10;
        }
        if (judge_prob(A.units.at(i).detour + detour_add)){  // 迂回
            detour_flags.at(i) = 1;
            A.is_detour = true;
        }
        else{   // 不迂回
            ;
        }
    }

    // step3-2-2 重置防守方的反击次数
    for (int j = 0; j < D.unit_num; ++j) D.units.at(j).counter_num = 0;

    // step3-2-3 进攻方逐一进攻
    for (int i = 0; i < A.unit_num; ++i){
        if (A.units.at(i).is_ranged) continue;  // 远程单位 忽略
        
        // step1 确定远程单位
        int counter_id = -1;
        if (detour_flags.at(i) == 1) counter_id = D.unit_num - 1;
        else if(detour_flags.at(i) == 0) counter_id = 0;
        else {cout << "something wrong" << endl;}

        // step2 考虑是否冲锋
        double charge_resist = 0;   // 冲锋抵抗
        double charge_increase = 0; // 冲锋增加
        bool is_charge = false;
        if (A.units.at(i).full_skills.count("Heavy-armored Charge") > 0) charge_increase = 20;   // 重甲冲锋
        if (D.units.at(counter_id).full_skills.count("Halberd 3") > 0) charge_resist = 40;       // 长戟
        if (D.units.at(counter_id).full_skills.count("Long Spear 3") > 0) charge_resist = 35;    // 长枪
        charge_resist = 1 - charge_resist / 100.0; 
        if (judge_prob((A.units.at(i).charge + charge_increase) * charge_resist)) is_charge = true;
        if (is_charge) A.is_charge = true;   

        // step3 分类讨论冲锋和没冲锋
        double alpha = 1.0;
        if (detour_flags.at(i) == 1 && A.units.at(i).full_skills.count("Thousand miles raid") > 0) alpha *= 1.2;  // 千里奔袭
        if (A.units.at(i).full_skills.count("Furious Charge") > 0 && D.unit_num > 1) alpha *= 1.05;  // 狂暴冲锋
        // step3-1 如果没冲锋
        if (!is_charge){
            double dmg = melee_calc_single(A.units.at(i), D.units.at(counter_id)) * mob_alpha * alpha * buff_A_atk_alpha * buff_D_def_alpha;
            D.units.at(counter_id).injured(dmg);
        }
        // step3-2 如果冲锋了
        else{
            double dmg;
            if (A.units.at(i).full_skills.count("Heavy-armored Charge") > 0) alpha *= 1.1;    // 重甲冲锋
            for (int j = 0; j < D.unit_num; ++j){
                dmg = melee_calc_single(A.units.at(i), D.units.at(j)) * mob_alpha * alpha * buff_A_atk_alpha * buff_D_def_alpha;
                D.units.at(j).injured(dmg);
            }
        }

        // step4 结算反击伤害
        if (is_charge && D.units.at(counter_id).full_skills.count("Coordinated Counterattack") > 0){  // 协同反击发动
            for (int j = 0; j < D.unit_num; ++j){
                if (D.units.at(j).is_ranged) ;  // 远程此时不反击
                else if (D.units.at(j).current_troops == 0) ; // 单位被消灭不反击
                else{ 
                    // 计算反击基础系数
                    double alpha = (D.units.at(j).type == 1 ? defence_counter_alpha: 1);
                    if (D.units.at(j).full_skills.count("Bloodlust") > 0){ // 考虑嗜血技能
                        if (D.units.at(j).current_troops < 0.5 * D.units.at(j).troops){
                            alpha *= 1.05;
                        }
                    }
                    if (D.units.at(j).full_skills.count("Modao 3") > 0){  // 陌刀技能
                        alpha *= 1.2;
                    }
                    if (D.units.at(j).full_skills.count("Frontline Demolisher") > 0){  // 冲锋毁灭者技能
                        alpha *= 1.2;
                    }
                
                    // 预先计算反击伤害
                    double dmg = melee_calc_single(D.units.at(j), A.units.at(i)) * alpha * buff_D_atk_alpha * buff_A_def_alpha;  // 反击没有行动力相关系数

                    // 根据反击次数 判断是否反击
                    if (D.units.at(j).counter_num == 0){  // 必定发生反击
                        D.units.at(j).counter_num++;
                        A.units.at(i).injured(dmg);
                    }
                    else{ // 防步考虑多段反击概率
                        if (D.units.at(j).type == 1){
                            double counter_add = 0;
                            if (D.units.at(j).full_skills.count("Strong Counter") > 0){   // 考虑强力反击技能
                                counter_add = 20;
                            }
                            if (judge_prob(D.units.at(j).counter + counter_add)) {  // 多段反击成功
                                D.units.at(j).counter_num++;
                                A.units.at(i).injured(dmg);
                            }
                        }
                        else ; // 非防步无法反击
                    }
                }
            }
        } 
        else{   // 正常反击流程
            if (D.units.at(counter_id).is_ranged) ;  // 远程此时不反击
            else if (D.units.at(counter_id).current_troops == 0) ; // 单位被消灭不反击
            else{
                // 计算反击基础系数
                double alpha = (D.units.at(counter_id).type == 1 ? defence_counter_alpha: 1);
                if (D.units.at(counter_id).full_skills.count("Bloodlust") > 0){ // 考虑嗜血技能
                    if (D.units.at(counter_id).current_troops < 0.5 * D.units.at(counter_id).troops){
                        alpha *= 1.05;
                    }
                }
                if (D.units.at(counter_id).full_skills.count("Modao 3") > 0){  // 陌刀技能
                    alpha *= 1.2;
                }
                if (is_charge && D.units.at(counter_id).full_skills.count("Frontline Demolisher") > 0){  // 冲锋毁灭者技能
                    alpha *= 1.2;
                }
            
                // 预先计算反击伤害
                double dmg = melee_calc_single(D.units.at(counter_id), A.units.at(i)) * alpha * buff_D_atk_alpha * buff_A_def_alpha;  // 反击没有行动力相关系数

                // 根据反击次数 判断是否反击
                if (D.units.at(counter_id).counter_num == 0){  // 必定发生反击
                    D.units.at(counter_id).counter_num++;
                    A.units.at(i).injured(dmg);
                }
                else{ // 防步考虑多段反击概率
                    if (D.units.at(counter_id).type == 1){
                        double counter_add = 0;
                        if (D.units.at(counter_id).full_skills.count("Strong Counter") > 0){   // 考虑强力反击技能
                            counter_add = 20;
                        }
                        if (judge_prob(D.units.at(counter_id).counter + counter_add)) {  // 多段反击成功
                            D.units.at(counter_id).counter_num++;
                            A.units.at(i).injured(dmg);
                        }
                    }
                    else ; // 非防步无法反击
                }
            }

        }

        // step5 实时更改防守方部队死亡情况
        remove_num = 0;
        temp_record = D.unit_num;
        for (int j = 0; j < temp_record; ++j){
            if (D.units.at(j-remove_num).current_troops == 0){
                D.remove_unit(j - remove_num, skill_list);
                remove_num++;
            }
        }
        if (D.unit_num == 0){ // 全灭
            if (verbose){
                cout << D.legion_name << " is completely annihilated!" << endl;
            }
            break;
        }

    }

    // step 3-3 近战流程结束的后处理
    // 结算A部队剩余情况
    remove_num = 0;
    temp_record = A.unit_num;
    for (int i = 0; i < temp_record; ++i){
        if (A.units.at(i-remove_num).current_troops == 0){
            A.remove_unit(i - remove_num, skill_list);
            remove_num++;
        }
    }
    if (A.unit_num == 0){
        if (verbose){
            cout << A.legion_name << " is hit back and gone." << endl;
        }
    }
    // 考虑不朽被动的触发
    else{
        if(A.is_charge){
            A.buffs["Immortality"]++;
            if (A.buffs["Immortality"] > immortality_max_num){
                A.buffs["Immortality"] = immortality_max_num;
            }
        }
    }
    return; 
}
