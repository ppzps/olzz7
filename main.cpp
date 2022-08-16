#include <iostream>
#include <fstream>
#include <iomanip>

#include "3rd/json/nlohmann/json.hpp"
#include "unit.h"
#include "damage.h"
#include "time.h"

using namespace std;
using njson = nlohmann::json;

// 部队顺序  防步 冲骑 近骑 近步 弓骑 步弓  type 1,3,2,0,5,4

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

// A 先攻击， 攻击序列 A, D, A, D, A, D, ...  直到一方死亡
bool fight(Legion& A, Legion& D, const std::map<std::string, bool> &skill_list){
    cout << "start..." << endl;
    A.show_troops();
    D.show_troops();
    while(true){
        cout << "A->D" << endl;
        legion_fight(A, D, skill_list);
        A.show_troops();
        D.show_troops();
        if (A.unit_num == 0) return false;
        if (D.unit_num == 0) return true;

        cout << "D->A" << endl;
        legion_fight(D, A, skill_list);
        A.show_troops();
        D.show_troops();
        if (A.unit_num == 0) return false;
        if (D.unit_num == 0) return true;
    }
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

    while(true){
        legion_fight(A, B, skill_list);
        // A.show_troops();
        // B.show_troops();
        if (A.unit_num == 0) break;
        if (B.unit_num == 0) break;

        legion_fight(A, B, skill_list);
        // A.show_troops();
        // B.show_troops();
        if (A.unit_num == 0) break;
        if (B.unit_num == 0) break;

        legion_fight(B, A, skill_list);
        // A.show_troops();
        // B.show_troops();
        if (A.unit_num == 0) break;
        if (B.unit_num == 0) break;

        legion_fight(B, A, skill_list);
        // A.show_troops();
        // B.show_troops();
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


    int N = legions_id.size();
    const int FIGHT_NUM = 10;
    vector<vector<double>> res_record(N, vector<double>(N, 0.0));  // 作战记录表

    for (int k = 0; k < FIGHT_NUM; ++k){
        cout << "cnt num = " << k << endl;
        for (int i = 0; i < N; ++i){
            for (int j = 0; j < N; ++j){
                double res = fight_fromid(i, j, legions_id, fleet_data, skill_list);
                res_record[i][j] += res;
            }
        }
    }
    // 对分数取平均
    for (int i = 0; i < N; ++i){
        for (int j = 0; j < N; ++j){
            res_record[i][j] /= (double)FIGHT_NUM;
        }
    }

    // debug
    // for (int i = 0; i < legions_id.size(); ++i){
    //     for (int j = 0; j < 3; ++j){
    //         cout << legions_id[i][j] << " ";
    //     }
    //     cout << endl;
    // }


    // output to file
    ofstream ofs;
    ofs.open("result.txt", ios::out);
    if (!ofs.is_open()) {cout << "cannot open file"<< endl;}
    else{
        for (int i = 0; i < N; ++i){
            for (int j = 0; j < N; ++j){
                ofs << setprecision(6) << res_record[i][j] << "\t";
            }
            ofs << "\n";
        }
    }
    ofs.close();

    cout << "damage simulator is finished." << endl;
    return 0;
}
