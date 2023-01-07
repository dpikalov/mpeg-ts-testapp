#!/bin/sh

#tsp -v -I http https://tsduck.io/streams/italy-sardinia-dttv/mux1rai.ts -O ip 239.0.0.1:1234

tsp -v -I file --infinite mux1rai.ts -P regulate -O ip 239.0.0.1:1234
