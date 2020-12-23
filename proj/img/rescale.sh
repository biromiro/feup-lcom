mkdir res
cd font

for f in *; do
  if [[ $f =~ \.png$ ]]; then
    convert "$f" -resize 60% "../res/$f" 
  else
    echo Nope!
  fi
done
