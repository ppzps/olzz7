// 简单的伤害模拟器 
// ggzgs

#include <iostream>
#include <fstream>
#include <iomanip>

#include "3rd/json/nlohmann/json.hpp"
#include "unit.h"
#include "damage.h"
#include "time.h"

using namespace std;
using njson = nlohmann::json;

const int FLEET_NUM = 27;    // unit2.json 目前收录的部队数据

// 通过剩余血量计算得分，A存活为正分, B存活为负分
double calc_score(const Legion& A, const Legion& B){
    double is_positive;
    const Legion *p = nullptr;
    if (A.unit_num == 0) {
        is_positive = -1.0;
        p = &B;
    }
    else if (B.unit_num == 0){
        is_positive = 1.0;
        p = &A;
    } 
    else{
        cout << "calc score error!" << endl;
        return 0;
    }

    double abs_score = 0.0;
    double three = 100.0 / 3.0;
    for (int i = 0; i < p->unit_num; ++i){
        double remain = (double)p->units.at(i).current_troops / (double)p->units.at(i).troops;
        abs_score += remain * three;
    }

    abs_score -= 5 * (3 - p->unit_num);  // 掉编的额外惩罚
    if (abs_score < 0) abs_score = 0;

    return is_positive * abs_score;
}

// 将compare.json的名字 转换为数字
int convert_name(std::string name, Unit* fleet_data, int len){
    for (int i = 0; i < len; ++i){
        if (fleet_data[i].name == name) return i;
    }

    cout << "cannot convert name in convert_name! name: " << name << endl;
    return -1;
}

// 将compare.json将领的名字，转换为数字
int convert_gen_name(std::string name, General* general_data, int len){
    for (int i = 0; i < len; ++i){
        if (general_data[i].name == name) return i;
    }

    cout << "cannot convert name in convert__gen_name! name: " << name << endl;
    return -1;
}

// A 先攻击， 根据行动力进行攻击
double attack(Legion& A, Legion& B, const std::map<std::string, bool> &skill_list){
    cout << "start..." << endl;
    A.current_mobility = A.mobility;  // for debug
    B.current_mobility = B.mobility;  // for debug
    A.show_troops();
    B.show_troops();
    cout << "fight!" << endl;

    int round_max = 20; // 回合数，这里两个回合等于游戏里的一个回合
    bool is_end = false;
    for (int i = 0; i < round_max; ++i){
        Legion *pa = 0;
        Legion *pd = 0;
        if (i % 2 == 0){ // A 先攻
            pa = &A;
            pd = &B;
        }
        else{
            pa = &B;
            pd = &A;
        }

        bool trigger_black_armor = false;
        bool trigger_art_of_war = false;

        pa->current_mobility = pa->mobility;
        // 考虑技能整编
        if (pa->legion_skills.count("Integration of the army") && pa->unit_num == 3) pa->current_mobility++;
        while(pa->current_mobility > 0){
            legion_fight(*pa, *pd, skill_list);
            // 考虑技能玄甲和兵法
            if (pa->legion_skills.count("Black Armor") && pa->is_detour && !trigger_black_armor) {
                pa->current_mobility += 2;
                trigger_black_armor = true;
            }
            if (pa->legion_skills.count("Art of War") && pa->is_charge && !trigger_art_of_war) {
                pa->current_mobility += 2;
                trigger_art_of_war = true;
            }

            // output
            pa->show_troops();
            pd->show_troops();

            // 有军团被消灭，结束
            if (pa->unit_num == 0 || pd->unit_num == 0){
                is_end = true;
                break;
            }
        }

        if (is_end) break;

        // 考虑回合结束的相关事件
        if (i % 2 == 1){
            // 医院回血
            if (pa->legion_skills.count("Healing") > 0){
                for (int i = 0; i < pa->unit_num; ++i){
                    int full_troop = pa->units.at(i).troops;
                    int after_troop = pa->units.at(i).current_troops + 0.05 * full_troop;
                    if (after_troop <= full_troop) pa->units.at(i).current_troops = after_troop;
                    else pa->units.at(i).current_troops = after_troop;
                }
            }
            if (pd->legion_skills.count("Healing") > 0){
                for (int i = 0; i < pd->unit_num; ++i){
                    int full_troop = pd->units.at(i).troops;
                    int after_troop = pd->units.at(i).current_troops + 0.05 * full_troop;
                    if (after_troop <= full_troop) pd->units.at(i).current_troops = after_troop;
                    else pd->units.at(i).current_troops = after_troop;
                }
            }
        }   
    }

    if (is_end) return calc_score(A, B);
    else return 0;    // 不太可能的情况 20回合都没死
}

int main(int argc, char **argv){
    cout << "damage simulator starts..." << endl;
    ifstream f1("../data/skill.json");
    ifstream f2("../data/unit2.json");
    ifstream f3("../data/generals.json");
    ifstream f4("compare.json");

    nlohmann::json data1 = nlohmann::json::parse(f1);
    nlohmann::json data2 = nlohmann::json::parse(f2);
    nlohmann::json data3 = nlohmann::json::parse(f3);
    nlohmann::json data4 = nlohmann::json::parse(f4);

    // 随机数种子
    srand(time(0));

    // 生成技能表   data1
    std::map<std::string, bool> skill_list;
    for (njson::iterator it = data1.begin(); it != data1.end(); ++it){
        string sk = it.key();
        int is_legion = it.value();
        skill_list[sk] = (bool)is_legion;
    }

    // 生成满血部队 data2
    int unit_num = data2.size();
    Unit* fleet_data = new Unit [unit_num];
    for (int i = 0; i < unit_num; ++i){
        fleet_data[i] = Unit(data2[i]);
    }
    
    // 生成将领 data3
    int gen_num = data3.size();
    General* general_data = new General[gen_num];
    for (int i = 0; i < gen_num; ++i){
        general_data[i] = data3[i];
    }

    // 根据compare.json中的设置，进行战斗测试
    njson data41 = data4[0];
    njson data42 = data4[1];
    {
        string gen_name1 = data41["general"];
        string gen_name2 = data42["general"];
        vector<string> legions1 = data41["legion"];
        vector<string> legions2 = data42["legion"];

        Legion A("legion1");
        Legion B("legion2");
        // 为A添加部队
        for (int i = 0; i < legions1.size(); ++i){
            int id = convert_name(legions1[i], fleet_data, unit_num);
            A.add_unit(fleet_data[id], skill_list);
        }
        // 为A添加将领
        if (gen_name1 == ""); // 不填加将领
        else{
            int gid = convert_gen_name(gen_name1, general_data, gen_num);
            if (gid == -1);
            else{
                A.add_general(general_data + gid);
            }
        }

        // 为B添加部队
        for (int i = 0; i < legions2.size(); ++i){
            int id = convert_name(legions2[i], fleet_data, unit_num);
            B.add_unit(fleet_data[id], skill_list);
        }
        // 为B添加将领
        if (gen_name2 == ""); // 不填加将领
        else{
            int gid = convert_gen_name(gen_name2, general_data, gen_num);
            if (gid == -1);
            else{
                B.add_general(general_data + gid);
            }
        }

        double score = attack(A, B, skill_list);
        cout << "score: " << score << endl;
    }


    // 释放内存
    delete [] fleet_data;
    delete [] general_data;

    cout << "damage simulator is finished." << endl;
    return 0;
}
