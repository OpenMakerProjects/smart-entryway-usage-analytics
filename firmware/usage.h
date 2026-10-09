#pragma once
#include <cstdint>
struct Usage {
 uint32_t hours[24]={},days[7]={},last=0;uint64_t elapsed=0,hour=0,day=0;bool seen=false,previous=false;
 void sync(){uint64_t h=elapsed/3600000,d=elapsed/86400000;while(hour<h)hours[(++hour)%24]=0;while(day<d)days[(++day)%7]=0;}
 void tick(uint32_t now,bool active){
  if(!seen){seen=true;last=now;previous=active;return;}
  uint64_t end=elapsed+uint32_t(now-last);last=now;
  while(elapsed<end){sync();uint64_t boundary=(elapsed/3600000+1)*3600000;uint64_t n=end<boundary?end:boundary;
   if(previous){hours[(elapsed/3600000)%24]+=uint32_t(n-elapsed);days[(elapsed/86400000)%7]+=uint32_t(n-elapsed);}
   elapsed=n;
  }sync();previous=active;
 }
 uint32_t currentHour()const{return hours[hour%24];}
 uint32_t currentDay()const{return days[day%7];}
 uint64_t hoursTotal()const{uint64_t s=0;for(auto x:hours)s+=x;return s;}
 uint64_t weekTotal()const{uint64_t s=0;for(auto x:days)s+=x;return s;}
};
