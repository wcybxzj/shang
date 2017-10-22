elements=""
key="2016_05_01:unique:ids:set"
for i in `seq 1 10000`
do
    elements="${elements} "${i}
    if [[ $((i%1000))  == 0 ]];
    then
        redis-cli sadd ${key} ${elements}
        elements="" 
    fi
done
