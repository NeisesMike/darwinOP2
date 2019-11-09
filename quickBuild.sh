cd ./Linux/project/mikeProj &&
echo "building project..." &&
echo "" &&
make clean &&
make &&
cd ../../../ &&
rm game &&
ln -s Linux/project/mikeProj/game . &&
echo "done!"

