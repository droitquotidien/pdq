DDIR=~/Dropbox/data_cons/source_data/DILA/JORFLEGI
for i in $(ls $DDIR | egrep 'JORF|jorf' | sort);
do
	echo "./cmake-build-debug/pdq -c postgresql://dqdata:dqdata@localhost:5432/dqdata -t ~/dq -d $DDIR/$i -l ~/logs/$i.log"
done
