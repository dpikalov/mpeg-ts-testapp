# mpeg-ts-testapp

* Counts the total number of TS packets containing Video

* Counts the number of TS packets containing PCR

* Calculates bitrate from PCR

## Build and run the app on Linux

```
# build
(cd ./src && make)

# run
./src/testapp

# start test stream
tsp -v -I http https://tsduck.io/streams/italy-sardinia-dttv/mux1rai.ts -O ip 239.0.0.1:1234
```

## VLC test

Open stream ```udp://@239.0.0.1:1234```
