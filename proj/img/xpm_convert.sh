echo "If this doesnt work run the following command"
echo "sudo apt install imagemagick"
mkdir xpm
cd sprites
for f in *; do
  if [[ $f =~ \.png$ ]]; then
    convert "$f" "../xpm/${f%.png}.xpm"
  else
    echo Nope!
  fi
done
