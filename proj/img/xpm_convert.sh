for f in *; do
  if [[ $f =~ \.png$ ]]; then
    convert "$f" "xpm/${f%.png}.xpm"
  else
    echo Nope!
  fi
done