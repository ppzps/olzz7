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
// 不考虑冲锋，迂回，闪避，多段反击等因素，就是单纯的计算伤害
double melee_calc_single(const Unit& A, const Unit& D){
    assert(!A.is_ranged);
    // 先计算基础伤害
    double dmg = uniform_rand_int(A.attack1, A.attack2) * convert_troops(A.current_troops);
    if (A.full_skills.count("Trample") > 0) dmg *= convert_defence(D.defence, 5);
    else dmg *= convert_defence(D.defence, 0);

    // 考虑技能和隐藏血量的影响
    double alpha = D.blood;
    if (A.full_skills.count("Single-handed Sword 2") > 0 && D.type == 4) alpha *= 1.2;
    if (A.full_skills.count("Halberd 2") > 0 && D.type == 3) alpha *= 1.25;
    if (A.full_skills.count("Knight Sword 2") > 0 && D.type == 5) alpha *= 1.2;
    if (A.full_skills.count("Knight Spear 2") > 0 && D.type == 0) alpha *= 1.25;

    if (D.full_skills.count("Square") > 0 && (A.type == 2 || A.type == 3)) alpha *= 0.85;
    if (D.full_skills.count("Deterrence") > 0 && A.type == 0) alpha *= 0.85;

    return dmg * alpha;
}

// 单纯计算攻击伤害 A->D 且A为远程 
// 不考虑冲锋，迂回，闪避，多段反击等因素，就是单纯的计算伤害
double ranged_calc_single(const Unit& A, const Unit& D, int fleet){
    assert(A.is_ranged);
    // 先计算基础伤害
    double dmg = uniform_rand_int(A.attack1, A.attack2) * convert_troops(A.current_troops);
    if (A.full_skills.count("Trample") > 0) dmg *= convert_defence(D.defence, 5);
    else dmg *= convert_defence(D.defence, 0);

    double alpha = D.blood;
    double ranged_alpha = 0;
    if (fleet == 0) ranged_alpha = ranged_alpha1;
    else if (fleet == 1) ranged_alpha = ranged_alpha2;
    else if (fleet == 2) ranged_alpha = ranged_alpha3;
    else {cout << "fleet error!" << endl; return -1;} 

    alpha *= ranged_alpha;       // 远程系数
    if (D.type == 1 || D.type == 2) {alpha *= 1.15;}        // 弓箭 兵器系数
    if (D.full_skills.count("Concentration") > 0) {alpha *= 0.85;}   // 集中
    if (D.full_skills.count("Top shield") > 0) {alpha *= 0.8;}       // 顶盾

    return dmg * alpha;
}

// 军团对战
// A军团进攻D军团
void legion_fight(Legion& A, Legion& D, const std::map<std::string, bool> &skill_list){
    // 三个阶段 远程攻击， 远程还击， 近战攻防
    // i <-> A  下标的默认对应关系
    // j <-> D
    
    if (A.unit_num == 0 || D.unit_num == 0){
        cout << "empty legion!" << endl;
        return;
    }
    //---------------------
    // step1  远程攻击
    //---------------------

    // A->D 记录表  如dmg_rec_ad[1][2] 表示A第2编攻击D第3编
    // step 1-1  记录远程伤害
    vector<vector<double>> dmg_rec_ad(A.unit_num, vector<double>(D.unit_num, -2.0));
    for (int i = 0; i < A.unit_num; ++i){
        if (!A.units.at(i).is_ranged) continue; // 非远程，跳过计算
        for (int j = 0; j < D.unit_num; ++j){
            double dmg = ranged_calc_single(A.units.at(i), D.units.at(j), j);
            dmg_rec_ad[i][j] = dmg;
        }
    }
    
    // step 1-2 结算进攻方远程伤害，考虑闪避值的影响
    for (int j = 0; j < D.unit_num; ++j){
        for (int i = 0; i < A.unit_num; ++i){
            if (!judge_prob(D.units.at(j).dodge))   // 闪避失败
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
            double dmg = ranged_calc_single(D.units.at(j), A.units.at(i), i);
            dmg_rec_da[j][i] = dmg;
        }
    }

    // step 2-2 结算远程反击伤害
    for (int i = 0; i < A.unit_num; ++i){
        for (int j = 0; j < D.unit_num; ++j){
            if (!judge_prob(A.units.at(i).dodge))   // 闪避失败
                A.units.at(i).injured(dmg_rec_da[j][i]);
        }
    }

    //---------------------
    // step3  近战攻防
    //---------------------

    // step3-1 清理远程攻击后 防守方死亡的单位
    int remove_num = 0;   // 用于删除部队
    for (int j = 0; j < D.unit_num; ++j){
        if (D.units.at(j).current_troops == 0){
            D.remove_unit(j - remove_num, skill_list);
            remove_num++;
        }
    }

    if (D.unit_num == 0){
        cout << D.legion_name << " is completely annihilated!" << endl;
        return;
    }

    // step3-2 近战攻击记录表
    vector<vector<double>> melee_rec_ad(A.unit_num, vector<double>(D.unit_num, -2.0));  // 近战攻击记录表
    for (int i = 0; i < A.unit_num; ++i){
        if (A.units.at(i).is_ranged) continue; // 远程，跳过计算
        for (int j = 0; j < D.unit_num; ++j){
            double dmg = melee_calc_single(A.units.at(i), D.units.at(j));
            melee_rec_ad[i][j] = dmg;
        }
    }

    // step3-3 划分进攻方向  只需考虑迂回
    vector<int> front_ids;  // 前排攻击部队序号
    vector<int> back_ids;   // 后排攻击部队序号
    vector<int> attack_is_front(3, 0);  // 1表示是前排， 0表示是后排, -1表示远程
    for (int i = 0; i < A.unit_num; ++i){
        if (A.units.at(i).is_ranged) {
            attack_is_front[i] = -1;
            continue;  // 远程跳过
        }
        if (judge_prob(A.units.at(i).detour)){  // 迂回
            back_ids.push_back(i);
        }
        else{   // 不迂回
            front_ids.push_back(i);
        }
    }
    // 添加前排单位
    if (front_ids.size() > 0){
        attack_is_front.at(front_ids[0]) = 1;
    }
    if (back_ids.size() > 0){
        attack_is_front.at(back_ids[0]) = 1;
    }


    // step3-4 近战单位按顺序进行攻防  符合一个比较复杂的表格
    // 先重置防守方的反击次数
    for (int j = 0; j < D.unit_num; ++j) D.units.at(j).counter_num = 0;
    // 开始近战攻防循环
    for (int i = 0; i < A.unit_num; ++i){
        if (A.units[i].is_ranged) continue;   // 远程单位 忽略

        // step3-4-1 确认反击单位
        int counter_id = -1;
        if (find(front_ids.begin(), front_ids.end(), i) != front_ids.end()) counter_id = 0;  // 正面
        else counter_id = D.unit_num - 1;   // 背面

        // step3-4-2 考虑是否发生冲锋
        double charge_resist = 0;   // 冲锋抵抗
        bool is_charge = false;
        if (D.units.at(counter_id).full_skills.count("Halberd 2") > 0){
            charge_resist = 35;
        }
        charge_resist = 1 - charge_resist / 100.0; 
        if (judge_prob(A.units.at(i).charge * charge_resist)) is_charge = true;
        

        // step3-4-3 结算反击方受到伤害
        vector<int> dmg_flag(D.unit_num, 0);   // 记录是否受到伤害
        D.units.at(counter_id).injured(melee_rec_ad[i][counter_id]);
        if (is_charge){
            dmg_flag[counter_id] = 1;
            for (int j = 0; j < D.unit_num; ++j){
                if (dmg_flag[j] == 0){
                    D.units.at(j).injured(melee_rec_ad[i][j]);
                    dmg_flag[j] = 1;
                }
            }
        }

        
        // step3-4-3 计算反击伤害
        if (D.units.at(counter_id).current_troops == 0) ;   // 该单位已被消灭，无法反击
        else if (D.units.at(counter_id).is_ranged) ;   // 远程单位此时不反击
        else if (D.units.at(counter_id).type == 1){   // 防步，需要考虑多段反击
            // 先计算反击伤害
            double dmg = melee_calc_single(D.units.at(counter_id), A.units.at(i));
            dmg *= defence_counter_alpha;
            // 攻击方是否为后排单位
            if (attack_is_front[i] == 1){  // 前排
                A.units.at(i).injured(dmg);
            }
            else if (attack_is_front[i] == 0){  // 后排
                if (judge_prob(D.units.at(counter_id).counter)){
                    A.units.at(i).injured(dmg);
                }
            }
        }
        else{     // 普通近战反击
            // 攻击方是否为后排单位
            if (attack_is_front[i] == 1 && D.units.at(counter_id).counter_num == 0){  // 前排
                double dmg = melee_calc_single(D.units.at(counter_id), A.units.at(i));
                A.units.at(i).injured(dmg);
                D.units.at(counter_id).counter_num++;
            }  
            // 后排不会被反击
        }

        // step3-4-4 判断防守方单位是否存在，
        remove_num = 0;
        for (int j = 0; j < D.unit_num; ++j){
            if (D.units.at(j).current_troops == 0){
                D.remove_unit(j - remove_num, skill_list);
                remove_num++;
            }
        }

        if (D.unit_num == 0){
            cout << D.legion_name << " is completely annihilated!" << endl;
            return;
        }
    }

    // step3-5 最后判断进攻方的部队损耗
    remove_num = 0;
    for (int i = 0; i < A.unit_num; ++i){
        if (A.units.at(i).current_troops == 0){
            A.remove_unit(i - remove_num, skill_list);
            remove_num++;
        }
    }
    if (A.unit_num == 0){
        cout << A.legion_name << " is hit back and gone." << endl;
        return;
    }

}
