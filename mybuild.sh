cd ./Linux/build/ &&
echo "" &&
echo "building infrastructure..." &&
echo "" &&
make clean &&
make &&
cd ../../Linux/project/mikeProj &&
echo "" &&
echo "building project..." &&
echo "" &&
make clean &&
make &&
cd ../../../ &&
rm game &&
ln -s Linux/project/mikeProj/game . &&
echo "done!"
