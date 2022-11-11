// 测试一些特殊情况打 0-19级龙骑兵
// by ggzgs

#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>

#include "3rd/json/nlohmann/json.hpp"
#include "unit.h"
#include "damage.h"
#include "time.h"

using namespace std;
using njson = nlohmann::json;

// 通过剩余血量计算得分，A存活为正分, B存活为负分  punish 为是否进行掉编惩罚
double calc_score(const Legion& A, const Legion& B, bool punish = false){
    double is_positive;
    const Legion *p = nullptr;
    if (A.unit_num == 0 && B.unit_num == 0) return 0;
    else if (A.unit_num == 0 && B.unit_num > 0) {
        is_positive = -1.0;
        p = &B;
    }
    else if (A.unit_num > 0 && B.unit_num == 0){
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

    if (punish){
        abs_score -= 5 * (3 - p->unit_num);  // 掉编的额外惩罚
    }
    if (abs_score < 0) abs_score = 0;

    return is_positive * abs_score;
}

// A 先攻击， 根据行动力进行攻击
// 考虑了固定伤害技能，考虑了长弓的buff
double attack(Legion& A, Legion& B, const std::map<std::string, bool> &skill_list, int first, double fix_dmg, bool verbose = true){
    if (verbose){
        cout << "start..." << endl;
    }
    A.current_mobility = A.mobility;  // for debug
    B.current_mobility = B.mobility;  // for debug
    if (verbose){
        A.show_troops();
        B.show_troops();
        cout << "fight!" << endl;
    }  

    int round_max = 40; // 回合数，这里两个回合等于游戏里的一个回合
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

        bool trigger_black_armor = false;  // 近玄的效果
        bool trigger_art_of_war = false;   // 唐冲的效果

        pa->current_mobility = pa->mobility;

        // 额外考虑固定伤害
        if (i%2 == first && fix_dmg >= 1){
            for (int k = 0; k < pd->unit_num; ++k){
                pd->units.at(k).injured(fix_dmg);
            }
            int remove_num = 0;
            int temp_record = pd->unit_num;
            for (int k = 0; k < temp_record; ++k){
                if (pd->units.at(k-remove_num).current_troops == 0){
                    pd->remove_unit(k - remove_num, skill_list);
                    remove_num++;
                }
            }
        }

        // 考虑技能整编
        if (pa->legion_skills.count("Integration of the army") && pa->unit_num == 3) pa->current_mobility++;
        while(pa->current_mobility > 0){
            legion_fight(*pa, *pd, skill_list, verbose);
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
            if(verbose){
                pa->show_troops();
                pd->show_troops();
            }        

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

            // 更改buff
            for (auto p : pa->buffs){
                if (p.first == "Immortality") p.second = 0;
                else p.second--;
            }
            for (auto p : pd->buffs){
                if (p.first == "Immortality") p.second = 0;
                else p.second--;
            }
            unordered_map<string, int>::iterator it = pa->buffs.begin();
            while(it != pa->buffs.end()){
                if (it->second <= 0){
                    it = pa->buffs.erase(it);
                }
                else it++;
            }
        }   
    }

    if (is_end) return calc_score(A, B);
    else return 0;    // 不太可能的情况 40回合都没死
}

// 根据剩余部队百分比记录表 得到最低胜利等级差
double output(vector<vector<double>>& record){
    map<int, int> diff_level;
    int attack_num = record.size();
    int defence_num = 0;
    if (attack_num > 0) {defence_num = record[0].size();}

    for (int i = 0; i < attack_num; ++i){
        std::cout << i << ": ";
        int flag = 0;
        if (record[i][0] < 0) flag = -1;
        else{
            for (int j = 1; j < defence_num; ++j){
                if (record[i][j] < 0){
                    flag = 1;
                    std::cout << j-1 << " " << j-1-i << " ";

                    diff_level[j-i-1]++;
                    break;
                }
            }
        }
        if (flag == -1){ 
            std::cout << "defeat "; 
            diff_level[-1000]++;
        }
        if (flag == 0) {
            std::cout << "vitory";
            diff_level[1000]++;
        }
        if ((i+1)%5 == 0) cout << "\n";
        else cout << "\t";
    }
    std::cout << "==================================" << endl;
    typedef map<int, int>::iterator mapiter;
    for (mapiter it = diff_level.begin(); it != diff_level.end(); ++it){
        std::cout << it->first << ": " << it->second << endl;
    }
    double score = 0;
    int num1 = 0, num2 = 0, num3 = 0;
    for (mapiter it = diff_level.begin(); it != diff_level.end(); ++it){
        if (it->first == 1000) num2++;
        else if (it->first == -1000) num3++;
        else{
            num1 += it->second;
            score += it->first * it->second;
        }
    }
    cout << "********************************************" << endl;
    if (num1 == 0){
        if (num2 > 0 && num3 == 0) score = 1000;
        else if (num2 == 0 && num3 > 0) score = -1000;
        else {
            std::cout << "score error" << endl;
        }
    }
    else score /= num1;
    std::cout << "Final score: " << score << endl;
    return score;
}

// 设置不同类型的将领和对应的装备，寻找不同的将领能弥补几级能力差距
/*
 待定
*/

int main(int argc, char **argv){
    if (argc != 3){
        cout << "prog str str" << endl;
        return 1;
    }

    cout << "damage simulator starts..." << endl;

    // 确定将领名字
    string gname1(argv[1]);
    string gname2(argv[2]);

    ifstream f1("../data/skill.json");
    ifstream f3("../data/generals.json");
    ifstream f21("../data/unit/Dragoon.json");
    // ifstream f22("../data/unit/Camel Archers.json");
    // ifstream f22("../data/unit/Conquistadors.json");
    // ifstream f22("../data/unit/Elephant Archers.json");
    // ifstream f22("../data/unit/Frank Axemen.json");
    // ifstream f22("../data/unit/Genoese Shielded Crossbowmen.json");
    // ifstream f22("../data/unit/Heavy-Armored Assault Cavalry.json");
    // ifstream f22("../data/unit/Imperial bow cavalry.json");
    // ifstream f22("../data/unit/Longbowmen.json");
    // ifstream f22("../data/unit/Mamluk Archers.json");
    // ifstream f22("../data/unit/Mongolian Raiders.json");
    // ifstream f22("../data/unit/Ravenna Elite Crossbowmen.json");
    // ifstream f22("../data/unit/Woad Raiders.json");
    ifstream f22("../data/unit/Zhuge Battalion.json");

    nlohmann::json data1 = nlohmann::json::parse(f1);
    nlohmann::json data21 = nlohmann::json::parse(f21);
    nlohmann::json data22 = nlohmann::json::parse(f22);
    nlohmann::json data3 = nlohmann::json::parse(f3);

    // 随机数种子
    srand(time(0));

    // data3 确定将领
    General *gen0 = nullptr;
    General *gen1 = nullptr;
    vector<General> generals;
    for (int i = 0; i < data3.size(); ++i){
        generals.push_back(General(data3[i]));
    }
    for (int i = 0; i < generals.size(); ++i){
        if (generals[i].name == gname1){
            gen0 = &(generals[i]);
        }
        if (generals[i].name == gname2){
            gen1 = &(generals[i]);
        }
    }
    if (!gen0 || !gen1){cout << "general name error" << endl; return 2;}

    // 生成技能表   data1
    std::map<std::string, bool> skill_list;
    for (njson::iterator it = data1.begin(); it != data1.end(); ++it){
        string sk = it.key();
        int is_legion = it.value();
        skill_list[sk] = (bool)is_legion;
    }

    // 生成满血部队 data21 -> 龙骑兵 data22 -> 其他远程
    int unit_num1 = data21.size();
    int unit_num2 = data22.size();
    Unit* fleet_data1 = new Unit [unit_num1];
    Unit* fleet_data2 = new Unit [unit_num2];
    for (int i = 0; i < unit_num1; ++i){
        fleet_data1[i] = Unit(data21[i]);
    }
    for (int i = 0; i < unit_num2; ++i){
        fleet_data2[i] = Unit(data22[i]);


        // 去除所有技能和闪避，单纯拼数据
        fleet_data2[i].dodge = 0;
        auto p = fleet_data2[i].skills.begin();
        auto pe = fleet_data2[i].skills.end();
        ++p;
        while (p != pe){
            fleet_data2[i].skills.erase(p);
            ++p;
        }
    }

    // 设定攻击和防御的对象
    int attack_num  = unit_num1; 
    int defence_num = unit_num2;
    Unit* attack_set = fleet_data1;
    Unit* defence_set = fleet_data2;
    General* attack_gen = gen0;
    General* defence_gen = gen1;
    
    // 生成记录表
    vector<vector<double> > record1(attack_num, vector<double>(defence_num, 666));
    vector<vector<double> > record2(defence_num, vector<double>(attack_num, 666));
    {
        const int LOOP_NUM = 100;
        // i 主动进攻 j 防御
        for(int i = 0; i < attack_num; ++i){
            for (int j = 0; j < defence_num; ++j){
                double record_sum1 = 0, record_sum2 = 0;
                for (int loop = 0; loop < LOOP_NUM; ++loop){
                    Legion A("legion1");
                    Legion B("legion2");
                    for(int k = 0; k < 3; ++k){
                        A.add_unit(attack_set[i], skill_list);
                        B.add_unit(defence_set[j], skill_list);
                    }
                    // 添加 general
                    if (attack_gen) A.add_general(attack_gen);
                    if (defence_gen) B.add_general(defence_gen);
                    record_sum1 += attack(A, B, skill_list, 0, 0, false);
                }
                // 调换攻守
                for (int loop = 0; loop < LOOP_NUM; ++loop){
                    Legion A("legion1");
                    Legion B("legion2");
                    for(int k = 0; k < 3; ++k){
                        A.add_unit(defence_set[j], skill_list);
                        B.add_unit(attack_set[i], skill_list);
                    }
                    // 添加 general
                    if (defence_gen) A.add_general(defence_gen);
                    if (attack_gen) B.add_general(attack_gen);
                    record_sum2 += attack(A, B, skill_list, 1, 0, false);
                }
                record1[i][j] = record_sum1 / LOOP_NUM;  
                record2[j][i] = record_sum2 / LOOP_NUM;              
            }
        }
    }

    // debug 单编测试
    // Legion A("legion1");
    // Legion B("legion2");
    // for(int i = 0; i < 3; ++i){
    //     A.add_unit(fleet_data2[0], skill_list);
    //     B.add_unit(fleet_data1[0], skill_list);
    // }
    // // B.add_general(&gen1);
    // double score = attack(A, B, skill_list, true);
    // cout << "score is: " << score << endl;


    // 输出额外的信息
    // cout << "positive: " << endl;
    // double score1 = output(record1);
    // cout << "********************************************" << endl;
    // cout << "negative: " << endl;
    // double score2 = output(record2);
    // cout << "********************************************" << endl;
    // cout << "diff level: " << (score1 - score2) / 2 << endl;
    

    // 临时输出
    for (int i = 0; i < 20; ++i){cout << i << "\t";}
    cout << endl;
    for (int i = 0; i < 20; ++i){cout << setprecision(4) << record1[i][0] << "\t";}
    cout << endl;
    for (int i = 0; i < 20; ++i){cout << setprecision(4) << record2[0][i] << "\t";}
    cout << endl;

    // // output to file1
    ofstream ofs;
    ofs.open("result.txt", ios::out);
    if (!ofs.is_open()) {cout << "cannot open file"<< endl;}
    else{
        for (int i = 0; i < attack_num; ++i){
            for (int j = 0; j < defence_num; ++j){
                ofs << setw(6) << setfill(' ') << record1[i][j] << "\t";
            }
            ofs << "\n";
        }
    }
    ofs.close();

    ofstream ofs1;
    ofs1.open("inv-result.txt", ios::out);
    if (!ofs1.is_open()) {cout << "cannot open file"<< endl;}
    else{
        for (int i = 0; i < defence_num; ++i){
            for (int j = 0; j < attack_num; ++j){
                ofs1 << setw(6) << setfill(' ') << record2[i][j] << "\t";
            }
            ofs1 << "\n";
        }
    }
    ofs1.close();

    // 释放内存
    delete [] fleet_data1;
    delete [] fleet_data2;

    cout << "damage simulator is finished." << endl;
    return 0;
}
