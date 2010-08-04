# Call this file from the directory in question if you would like a generic
# Makefile generated which invokes GCC for each individual file.

echo "#Awesome Makefile generated by ENIGMAsystem/Developer/automake.sh" > Makefile

echo "" >> Makefile;
echo "odir:" >> Makefile;
echo "	-mkdir .objs" >> Makefile;
echo "" >> Makefile

cd ./lib/
for file in *.c ;
  do
  {
    printf ".objs/vorbis_${file%.c}.o: lib/$file" >> ../Makefile;
    for i in `c_incl $file | gawk '/\/usr\/include/ { next } { print } '`;
    do
	if [ -e $i ] ; then
      printf " lib/$i" >> ../Makefile; fi
    done;
    echo "" >> ../Makefile;
    
    echo "	gcc -c lib/$file		-o .objs/vorbis_${file%.c}.o"  >> ../Makefile;
  };
  done;

echo "" >> ../Makefile;

  printf "\$(DEST)/libvorbis.a: odir " >> ../Makefile;
  for file in *.c ;
    do printf ".objs/vorbis_${file%.c}.o " >> ../Makefile; 
    done;
  echo "" >> ../Makefile;
  echo "	ar r \$(DEST)/libvorbis.a .objs/*.o" >> ../Makefile;
  echo "" >> ../Makefile;

echo "static: \$(DEST)/libvorbis.a" >> ../Makefile;
echo "" >> ../Makefile;
echo "clean:" >> ../Makefile;
echo "" >> ../Makefile;

