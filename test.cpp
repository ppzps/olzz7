#include <iostream>
#include <fstream>
#include <iomanip>

#include "3rd/json/nlohmann/json.hpp"
#include "unit.h"
#include "damage.h"
#include "time.h"

using namespace std;
using njson = nlohmann::json;

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

// 根据三元组 生成部队
// i win return positive, j win return negative
// 攻击序列i, i, j, j, i, i, j, j,.. 
double fight_fromid(int i, int j, const vector<vector<int>>& ids, const Unit* baseunit, const std::map<std::string, bool> &skill_list){
    Legion A(to_string(i));
    Legion B(to_string(j));
    for (int k = 0; k < 3; ++k){
        A.add_unit(baseunit[ids[i][k]], skill_list);
        B.add_unit(baseunit[ids[j][k]], skill_list);
    }

    // init troops
    cout << "init troops" << endl;
    A.show_troops();
    B.show_troops();
    cout << "------------------" << endl;

    while(true){
        legion_fight(A, B, skill_list);
        A.show_troops();
        B.show_troops();
        if (A.unit_num == 0) break;
        if (B.unit_num == 0) break;

        legion_fight(A, B, skill_list);
        A.show_troops();
        B.show_troops();
        if (A.unit_num == 0) break;
        if (B.unit_num == 0) break;

        legion_fight(B, A, skill_list);
        A.show_troops();
        B.show_troops();
        if (A.unit_num == 0) break;
        if (B.unit_num == 0) break;

        legion_fight(B, A, skill_list);
        A.show_troops();
        B.show_troops();
        if (A.unit_num == 0) break;
        if (B.unit_num == 0) break;
    }
    
    double score = calc_score(A, B);
    return score;
}


int main(int argc, char **argv){
    cout << "damage simulator starts..." << endl;
    ifstream f1("../data/skill.json");
    ifstream f2("../data/unit.json");
    ifstream f3("../data/tasks.json");

    nlohmann::json data1 = nlohmann::json::parse(f1);
    nlohmann::json data2 = nlohmann::json::parse(f2);
    nlohmann::json data3 = nlohmann::json::parse(f3);

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
    Unit fleet_data[6] = {Unit(data2[0]), Unit(data2[1]), Unit(data2[2]), 
                          Unit(data2[3]), Unit(data2[4]), Unit(data2[5])};
    
    // 生成所有军团的编号  data3
    vector<vector<int>> legions_id = data3;


    double res = fight_fromid(3, 0, legions_id, fleet_data, skill_list);
    cout << res << endl;

    cout << "damage simulator is finished." << endl;
    return 0;
}
