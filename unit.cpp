#include "unit.h"

using namespace std;

General::General(string name) : name(name), unit_name("") {
    // cout << "default general init" << endl;
    infantry_abi = 0;
    cavalry_abi = 0;
    ranged_abi = 0;

    has_unique = false;

    legion_dmg_inc1 = 1.0;
    legion_dmg_inc2 = 1.0;
    legion_dmg_red = 1.0;
    infantry_tactician = 1.0; 
    cavalry_tactician = 1.0;  
    ranged_tactician = 1.0;  
    melee_infantry_expert = 1.0;
    melee_cavalry_expert = 1.0;
    defence_infantry_expert = 1.0;
    charging_cavalry_expert = 1.0;
    assault_master = 1.0;
    polearm_master = 1.0;
    shooter_master = 1.0;
    swordman_master = 1.0;
    skirmisher_master = 1.0;
}

General::General(njson nj) : General(){
    // 一般来说必须有的
    if (nj.count("name") > 0) name = nj["name"];
    if (nj.count("has_unique") > 0){
        int t = nj["has_unique"];
        has_unique = t;
    }
    if (has_unique && nj.count("unit_name") > 0) unit_name = nj["unit_name"];
    
    // 能力
    if (nj.count("infantry_abi") > 0) infantry_abi = nj["infantry_abi"];
    if (nj.count("cavalry_abi") > 0) cavalry_abi = nj["cavalry_abi"];
    if (nj.count("ranged_abi") > 0) ranged_abi = nj["ranged_abi"];

    // 军团增伤
    if (nj.count("legion_dmg_inc1") > 0) legion_dmg_inc1 = nj["legion_dmg_inc1"];
    if (nj.count("legion_dmg_inc2") > 0) legion_dmg_inc2 = nj["legion_dmg_inc2"];
    if (nj.count("legion_dmg_red") > 0) legion_dmg_red = nj["legion_dmg_red"];

    if (nj.count("infantry_tactician") > 0) infantry_tactician = nj["infantry_tactician"];
    if (nj.count("cavalry_tactician") > 0) cavalry_tactician = nj["cavalry_tactician"];
    if (nj.count("ranged_tactician") > 0) ranged_tactician = nj["ranged_tactician"];

    if (nj.count("melee_infantry_expert") > 0) melee_infantry_expert = nj["melee_infantry_expert"];
    if (nj.count("melee_cavalry_expert") > 0) melee_cavalry_expert = nj["melee_cavalry_expert"];
    if (nj.count("defence_infantry_expert") > 0) defence_infantry_expert = nj["defence_infantry_expert"];
    if (nj.count("charging_cavalry_expert") > 0) charging_cavalry_expert = nj["charging_cavalry_expert"];

    if (nj.count("assault_master") > 0) assault_master = nj["assault_master"];
    if (nj.count("polearm_master") > 0) polearm_master = nj["polearm_master"];
    if (nj.count("shooter_master") > 0) shooter_master = nj["shooter_master"];
    if (nj.count("swordman_master") > 0) swordman_master = nj["swordman_master"];
    if (nj.count("skirmisher_master") > 0) skirmisher_master = nj["skirmisher_master"];
}

void General::show(){
    cout << "--------------------" << endl;
    cout << "name: " << name << endl;
    cout << "unit_name" << unit_name << endl;
    cout << "infantry_abi: " << infantry_abi << endl;
    cout << "cavalry_abi: " << cavalry_abi << endl;
    cout << "ranged_abi: " << ranged_abi << endl;

    cout << "legion_dmg_inc1: " << legion_dmg_inc1 << endl;
    cout << "legion_dmg_inc2: " << legion_dmg_inc2 << endl;
    cout << "legion_dmg_red: " << legion_dmg_red << endl;

    cout << "infantry_tactician: " << infantry_tactician << endl;
    cout << "cavalry_tactician: " << cavalry_tactician << endl;
    cout << "ranged_tactician: " << ranged_tactician << endl;

    cout << "melee_infantry_expert: " << melee_infantry_expert << endl;
    cout << "melee_cavalry_expert: " << melee_cavalry_expert << endl;
    cout << "defence_infantry_expert: " << defence_infantry_expert << endl;
    cout << "charging_cavalry_expert: " << charging_cavalry_expert << endl;

    cout << "assault_master: " << assault_master << endl;
    cout << "polearm_master: " << polearm_master << endl;
    cout << "shooter_master: " << shooter_master << endl;
    cout << "swordman_master: " << swordman_master << endl;
    cout << "skirmisher_master: " << skirmisher_master << endl;
}



Unit::Unit(){
    // cout << "init empty unit" << endl;
    plegion = nullptr;
    pgeneral = nullptr;
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
    fleet = -1;
    plegion = nullptr;
    pgeneral = nullptr;

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

void Unit::add_general(const General* gen){
    if (pgeneral){
        cout << "already has general, cannot add another" << endl;
        return;
    }
    pgeneral = gen;

    // 开始修改兵种面板
    if (type == 0 || type == 1){
        attack1 += pgeneral->infantry_abi / 2;
        attack2 += pgeneral->infantry_abi / 2;
        defence += pgeneral->infantry_abi / 3;
    }
    else if (type == 2 || type == 3){
        attack1 += pgeneral->cavalry_abi / 2;
        attack2 += pgeneral->cavalry_abi / 2;
        defence += pgeneral->cavalry_abi / 3;
    }
    else{
        attack1 += pgeneral->ranged_abi / 2;
        attack2 += pgeneral->ranged_abi / 2;
        defence += pgeneral->ranged_abi / 3;
    }

    // 如果有冲锋骑兵专家
    if (type == 3){
        charge *= pgeneral->charging_cavalry_expert;
    }
}

Weapons Unit::judge_weapon() const{
    string weapon_skill = skills.at(0);

    if (weapon_skill.find("Single-handed Sword") == 0) return swordman;
    else if (weapon_skill.find("Halberd") == 0) return polearm;
    else if (weapon_skill.find("Hengdao") == 0) return swordman;
    else if (weapon_skill.find("Dual-Wielded Weapon") == 0) return assualt;
    else if (weapon_skill.find("Modao") == 0) return polearm;
    else if (weapon_skill.find("Long Spear") == 0) return polearm;
    else if (weapon_skill.find("Knight Sword") == 0) return swordman;
    else if (weapon_skill.find("War Axe") == 0) return assualt;
    else if (weapon_skill.find("War Halberd") == 0) return polearm;
    else if (weapon_skill.find("Knight Spear") == 0) return polearm;
    else if (weapon_skill.find("Archery") == 0) return shooter;
    else if (weapon_skill.find("Javelin") == 0) return skirmisher;
    else if (weapon_skill.find("Crossbow") == 0) return shooter;
    else if (weapon_skill.find("Arquebus") == 0) return shooter;
    else if (weapon_skill.find("Liannu") == 0) return shooter;
    else{
        cout << "weapon error" << endl;
        return weapon_error;
    }

    return weapon_error;
}



Legion::Legion(std::string name) :legion_name(name) {
    unit_num = 0;
    mobility = 0;

    pgeneral = nullptr;
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

void Legion::adjust_mobility(){
    if (unit_num == 0) {
        mobility = 0;
        return;
    }

    mobility = units[0].mobility;
    for (int i = 1; i < unit_num; ++i){
        if (units[i].mobility < mobility) mobility = units[i].mobility;
    }
}

void Legion::adjust_attack_mob(){
    if (legion_skills.count("Mobilize") > 0){
        attack_mobility = base_attack_mob - 1;
    }
    else attack_mobility = base_attack_mob;
}

void Legion::add_unit(const Unit& u, const std::map<std::string, bool> &skill_list){
    if (unit_num == 3){
        cout << "cannot add unit into the legion" << endl;
        return;
    }

    unit_num++;
    units.push_back(u);

    // 调整军团技能
    for (int i = 0; i < u.skills.size(); ++i){
        if(skill_list.at(u.skills.at(i)) == 1){
            legion_skills.emplace(u.skills[i]);
        }
    }

    // 确定部队编号和军团
    units.at(unit_num - 1).plegion = this;
    units.at(unit_num - 1).fleet = unit_num - 1;

    // 如果有将领就为部队添加将领
    if (pgeneral){
        units.at(unit_num - 1).add_general(pgeneral);
    }

    adjust_skills();
    adjust_mobility();
    adjust_attack_mob();
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

    // 调整军团编号
    for (int i = 0; i < unit_num; ++i){
        units.at(i).fleet = i;
    }

    adjust_skills();
    adjust_mobility();
    adjust_attack_mob();
}

void Legion::add_general(const General* gen){
    pgeneral = gen;
    for (int i = 0; i < unit_num; ++i){
        units.at(i).add_general(gen);
    }
}

bool Legion::is_unique_unit() const{
    if (!pgeneral) return false;
    if (!pgeneral->has_unique) return false;
    for (int i = 0; i < unit_num; ++i){
        if (units.at(i).name == pgeneral->unit_name) return true;
    }
    return false;
}

void Legion::show_troops(){
    cout << legion_name << " cur troops: ";
    for (int i = 0; i < unit_num; ++i){
        cout << "(";
        cout << units.at(i).type << ", ";
        cout << units.at(i).current_troops <<  ") ";
    }
    cout << " general: ";
    if (pgeneral) cout << pgeneral->name;
    else cout << "no";
    cout << " rem mobility: " << to_string(current_mobility) << endl;
}

