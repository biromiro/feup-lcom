cd Attacking
for f in *; do
    sed -i 's/char/const char/g' $f
done
cd ..
cd Idle
for f in *; do
    sed -i 's/Idle Blinking/Idle_Blinking/g' $f
    sed -i 's/char/const char/g' $f
done
cd ..
cd Walking
for f in *; do
    sed -i 's/char/const char/g' $f
done
cd ..
