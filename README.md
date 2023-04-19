# telemetry
telemetry system used in the electrolab fusex rocket

## Andromede Rocket:

clone the base project:

git clone --recurse-submodules git@github.com:fusex/telemetry -b andromede

to build embedded code on rocket:

cd rocket; make; make upload

to build embedded code in the ground receiver:

cd base ; make; make upload

to build host code for a linux PC:

cd host ; make
