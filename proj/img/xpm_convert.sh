mkdir Idle_xpm
cd Idle
for f in *; do
  if [[ $f =~ \.png$ ]]; then
    convert "$f" "../Idle_xpm/${f%.png}.xpm"
  else
    echo Nope!
  fi
done
cd ..
mkdir IdleBlink_xpm
cd IdleBlink
for f in *; do
  if [[ $f =~ \.png$ ]]; then
    convert "$f" "../IdleBlink_xpm/${f%.png}.xpm"
  else
    echo Nope!
  fi
done
cd ..
mkdir Walking_xpm
cd Walking
for f in *; do
  if [[ $f =~ \.png$ ]]; then
    convert "$f" "../Walking_xpm/${f%.png}.xpm"
  else
    echo Nope!
  fi
done
