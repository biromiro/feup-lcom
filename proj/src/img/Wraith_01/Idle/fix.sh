for f in *; do
    sed -i 's/char/const char/g' $f
done
