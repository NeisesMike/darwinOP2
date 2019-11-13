cd ./Linux/build/ &&
echo "" &&
echo "cleaning infrastructure..." &&
echo "" &&
make clean &&
cd ../../Linux/project/mikeProj &&
echo "" &&
echo "cleaning project..." &&
echo "" &&
make clean &&
cd ../../../ &&
(
    rm game ||
    echo "skipping delete"
)
echo "done!"
