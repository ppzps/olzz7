#include "unit.h"

using namespace std;

Unit::Unit(){
    cout << "init empty unit" << endl;
}

Unit::Unit(njson nj) : name(nj["name"]) {
    type = nj["type"];
    int ranged = nj["ranged"];
    is_ranged = ranged;
    troops = nj["troops"];
    blood = nj["blood"];

    attack1 = nj["attack1"];
    attack2 = nj["attack2"];
    defence = nj["defence"];
    charge = nj["charge"];
    detour = nj["detour"];
    counter = nj["counter"];
    dodge = nj["dodge"];
    mobility = nj["mobility"];

    current_troops = troops;
    counter_num = 0;

    for(int i = 0; i < nj["skills"].size(); ++i){
        skills.emplace_back(string(nj["skills"][i]));
        full_skills.insert(string(nj["skills"][i]));
    }
}

void Unit::show(){
    std::cout << "-----------------------" << std::endl;
    std::cout << "name: " << name << std::endl;
    std::cout << "type: " << type << std::endl;
    std::cout << "is_ranged: " << is_ranged << std::endl;
    std::cout << "troops: " << troops << std::endl;
    std::cout << "blood: " << blood << std::endl;
    std::cout << "attack1: " << attack1 << std::endl;
    std::cout << "attack2: " << attack2 << std::endl;
    std::cout << "defence: " << defence << std::endl;
    std::cout << "charge: " << charge << std::endl;
    std::cout << "detour: " << detour << std::endl;
    std::cout << "counter: " << counter << std::endl;
    std::cout << "dodge: " << dodge << std::endl;
    std::cout << "mobility: " << mobility << std::endl;
    std::cout << "skills: [";
    for(int i = 0; i < skills.size(); ++i){
        std::cout << skills[i];
        if (i < skills.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void Unit::injured(double dmg){
    int dmg_int = 0;
    if (dmg > 1) dmg_int = dmg;
    else if (dmg > -1) dmg_int = 1;   // 尊严伤害
    else dmg_int = 0;   // 不需要考虑的伤害

    
    current_troops -= dmg_int;
    if (current_troops < 0){
        current_troops = 0;
    }
    return;
}

Legion::Legion(std::string name) :legion_name(name) {
    unit_num = 0;

}

void Legion::adjust_skills(){
    typedef unordered_set<string>::iterator uiter;
    for (int i = 0; i < unit_num; ++i){
        units[i].full_skills.clear();

        for (int j = 0; j < units[i].skills.size(); ++j){
            units[i].full_skills.insert(units[i].skills[j]);
        }

        for (uiter it = legion_skills.begin(); it != legion_skills.end(); ++it){
            units[i].full_skills.insert(*it);
        }
    }
}

void Legion::add_unit(const Unit& u, const std::map<std::string, bool> &skill_list){
    if (unit_num == 3){
        cout << "cannot add unit into the legion" << endl;
        return;
    }

    unit_num++;
    // units.resize(unit_num);
    // units[unit_num - 1] = u;
    units.push_back(u);

    // 调整军团技能
    for (int i = 0; i < u.skills.size(); ++i){
        if(skill_list.at(u.skills.at(i)) == 1){
            legion_skills.emplace(u.skills[i]);
        }
    }

    adjust_skills();
}

void Legion::remove_unit(int fleet, const std::map<std::string, bool> &skill_list){
    if (unit_num == 0){
        cout << "cannot remove unit from the legion" << endl;
        return;
    }

    unit_num--;
    units.erase(units.begin()+fleet);

    // 调整军团技能
    legion_skills.clear();
    for (int i = 0; i < unit_num; ++i){
        for (int j = 0; j < units.at(i).skills.size(); ++j){
            if (skill_list.at(units.at(i).skills.at(j)) == 1){
                legion_skills.emplace(units.at(i).skills[i]);
            }
        }
    }

    adjust_skills();
}

void Legion::show_troops(){
    cout << legion_name << ":  ";
    for (int i = 0; i < unit_num; ++i){
        cout << "(";
        cout << units.at(i).type << ", ";
        cout << units.at(i).current_troops <<  ") ";
    }
    cout << endl;
}

