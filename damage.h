#ifndef DAMAGE_H
#define DAMAGE_H

#include <cmath>
#include <cstdlib>
#include "unit.h"

// 远程伤害分布系数
const double ranged_alpha1 = 0.7;
const double ranged_alpha2 = 0.5;
const double ranged_alpha3 = 0.3;
// 防步反击系数
const double defence_counter_alpha = 1.5;

// 血量转换公式
inline double convert_troops(double h){
    return std::sqrt(h / 133.0);
}

// 防御转换公式
inline double convert_defence(double f, double a){
    return  75 / (75 + f - a);
}
// 多个无视防御的叠加
inline double convert_defence(double f, double a, double b){
    return  75 / (75 + f - a - b);
}


// 均匀分布随机数
double uniform_rand(double lower, double upper);

// 均匀分布随机整数
int uniform_rand_int(double lower, double upper);

// 判断概率事件是否发生
bool judge_prob(double thr, double lower = 0, double upper = 100);

// 近战伤害计算（根据当前血量）
double melee_calc_single(const Unit& A, const Unit& D);

// 远程伤害计算（根据当前血量，以及敌方位置）
double ranged_calc_single(const Unit& A, const Unit& D, int fleet);

// 军团实际对战
void legion_fight(Legion& A, Legion& D, const std::map<std::string, bool> &skill_list, bool verbose = true);

#endif
