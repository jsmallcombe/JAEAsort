echo "#ifdef __CINT__" > bin/Linkdef.h
echo "" >> bin/Linkdef.h
echo "#pragma link off all globals;" >> bin/Linkdef.h
echo "#pragma link off all classes;" >> bin/Linkdef.h
echo "#pragma link off all functions;" >> bin/Linkdef.h
echo "#pragma link C++ nestedclasses;" >> bin/Linkdef.h
echo "" >> bin/Linkdef.h


for f in include/*.h
do

echo -n "#pragma link C++ defined_in \"" >> bin/Linkdef.h
echo -n $f >> bin/Linkdef.h
echo  "\";" >> bin/Linkdef.h

done

echo "" >> bin/Linkdef.h
echo "#endif" >> bin/Linkdef.h
