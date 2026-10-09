#include "../firmware/usage.h"
#include <cassert>
#include <iostream>
int main(){
 Usage u;u.tick(0,true);u.tick(3600000,true);assert(u.currentHour()==0&&u.hoursTotal()==3600000);u.tick(7200000,false);assert(u.currentDay()==7200000&&u.hoursTotal()==7200000);u.tick(86400000,false);assert(u.currentDay()==0&&u.weekTotal()==7200000);
 u.tick(604800000,false);assert(u.weekTotal()==0&&u.hoursTotal()==0);
 Usage w;w.tick(UINT32_MAX-15,true);w.tick(20,false);assert(w.currentDay()==36);w.tick(50,false);assert(w.currentDay()==36);
 Usage edge;edge.tick(0,true);edge.tick(86399900,true);edge.tick(86400100,false);assert(edge.days[0]==86400000&&edge.currentDay()==100);assert(edge.currentHour()==100);assert(edge.hoursTotal()==23*3600000+100);
 Usage toggled;toggled.tick(0,false);toggled.tick(100,true);toggled.tick(400,false);assert(toggled.currentDay()==300);
 std::cout<<"interval attribution, exact hour/day rollover, seven-day eviction, activation and timer-wrap tests passed\n";
}
