# Call this file from the directory in question if you would like a generic
# Makefile generated which invokes GCC for each individual file.

echo "#Awesome Makefile generated by ENIGMAsystem/Developer/automake.sh" > Makefile
echo "" >> Makefile;

for file in *.cpp ;
  do
  {
    printf ".eobjs_\$(MODE)/${file%.cpp}.o: $file" >> Makefile;
    for i in `c_incl $file | gawk '/\/usr\/include/ { next } { print } '`;
    do
      printf " $i" >> Makefile;
    done;
    echo "" >> Makefile;
    
    echo "	-mkdir .eobjs_\$(MODE)" >> Makefile;
    echo "	g++ -c $file		-o .eobjs_\$(MODE)/${file%.cpp}.o \$(FLAGS)"  >> Makefile;
  };
  done;

echo "" >> Makefile;

#generate targets for each ENIGMA mode.
for modename in Run Debug Build Release;
do
  printf "$modename: " >> Makefile;
  for file in *.cpp ;
    do printf ".eobjs_$modename/${file%.cpp}.o " >> Makefile; 
    done;
  echo "" >> Makefile;
  
  echo "	-mkdir .eobjs_$modename/" >> Makefile;
done;

echo "" >> Makefile;
echo "clean:" >> Makefile;
echo "	rm -f .eobjs*/*" >> Makefile;

