#!/bin/bash
read -p "Input a number of times to run './a.out' : " T
printf "\nRun time:"

### Time Memo ###
#(s) second: date +%s
#(ms)millisecond: date +%s%3N
#(μs)microsecond: date +%s%6N
#(ns)nanosecond: date +%s%N

### Speed.sh
start=`date "+%s%6N"`
./a.out
printf "\n   Finished once."
for i in $(seq 2 ${T})
do
	./a.out
done
end=`date "+%s%6N"`
t1=$((($end-$start)/$T))
printf "\n   Avg time: %d μs\n" ${t1}

exit