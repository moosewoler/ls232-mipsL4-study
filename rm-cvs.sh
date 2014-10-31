#!/bin/sh

myfunc()
{
	for name in `ls` 
	do
		if [ -d ${name} ]
		then
			if [ "${name}" != CVS ]
			then
				cd ${name}
				myfunc
				cd ..
			else
				rmarg=`pwd`'/'${name}
				#rm -Rf ${rmarg}
				echo -Rf ${rmarg}
			fi
		fi
	done
}

myfunc
