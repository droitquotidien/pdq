for i in $(ls ~/Dropbox/data_cons/source_data/DILA/JORFLEGI | egrep 'JORF|jorf' | sort);
do
	echo "./cmake-build-debug/pdq -c postgresql://dqdata:dqdata@localhost:5432/dqdata -t ~/dq -d ~/Dropbox/data_cons/source_data/DILA/JORFLEGI/$i"
done
