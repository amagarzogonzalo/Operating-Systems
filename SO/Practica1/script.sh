if test -f mytar; then
	echo "El archivo mytar, se ha encontrado."


	if  test -d tmp; then
		rm -r tmp
		echo "Se ha borado el contenido de tmp/ "
	fi

	mkdir tmp
	echo "Se ha creado el directorio tmp/"
	cd tmp
	echo "Se ha accedido al directorio tmp/"

	echo "Hello world!"> fichero1.txt
	echo "Se ha creado el fichero1.txt"

	head -10 /etc/passwd > fichero2.txt 
	echo "Se ha creado el fichero2.txt"

	head -c 1024 /dev/urandom > fichero3.dat
	echo "Se ha creado el fichero3.dat"	

	../mytar -c -f filetar.mtar fichero1.txt fichero2.txt fichero3.dat
	echo "Se ha creado el archivo filetar.mtar"
	mkdir out
	echo "Se ha creado el directorio out"
	mv filetar.mtar out
	echo "Se ha movido el archivo filetar.mtar al directorio out/"

	cd out
	echo "Se ha accedido al directorio out/"
	../../mytar -x -f filetar.mtar
	echo "Se ha extraido el fichero filetar.mtar"
	
	if diff fichero1.txt ../fichero1.txt ;then
		echo "La comparacion entre los dos ficheros1.txt es correcta."
		if diff fichero2.txt ../fichero2.txt; then
			echo "La comparacion entre los dos fichero2.txt  es correcta."
			if diff fichero3.dat ../fichero3.dat; then
				echo "La comparacion entre ficheros es correcta."
				echo "CORRECT."
			fi
		fi
	fi

	cd ../../ 
	echo "Se ha regresado al directorio origen."
else
	echo "El archivo mytar, no se ha encontrado."
fi 