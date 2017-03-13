#!/usr/bin/env bash

filename=testresult
#跑测试
if [ -f "${filename}" ] ; then
rm ${filename}
fi

for((i = 0; i < 100; i++))
do
./order 10000 >> ${filename}
done
#把每次测试最快的那个找出来，统计出现次数，打印，再按照出现次数排个顺序
grep '//' ${filename} | awk 'BEGIN{FS=":"} {a[$2]++;} END{for(i in a) print i ", " a[i]}'  |sort -n -t, -k2 -r |tee result
