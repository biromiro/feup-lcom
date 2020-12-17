cd xpm
for f in *; do
    sed -i 's/#000000/#F6D55C/g' $f
done

cd ..
ls
