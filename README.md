# C parser for "Droit Quotidien" data

## Ubuntu / Debian Linux

```bash
apt-get install libarchive-dev
```

## Parse data, write JSON and generate actions 

```bash
./cmake-build-debug/pdq -d ~/Dropbox/data_cons/source_data/DILA/JORFLEGI/JORF_20220406-004356.tar.gz -t ~/dq/raw
```

Importation complète du stock JORF:

```bash
(dqdata) gasilber@mulhouse:~/work/mines/pdq$ time ./cmake-build-debug/pdq \
  -d ~/Dropbox/data_cons/source_data/DILA/JORFLEGI/Freemium_jorf_global_20191011-123107.tar.gz \
  -t ~/dq/raw 2> ~/dq/JORF_20191011-123107.log | dq_build_map ~/dq/www > ~/dq/JORF_20191011-123107.www.log

real	24m51.844s
user	14m3.814s
sys	13m27.385s
```

Importation complète du stock LEGI:

```bash
(dqdata) gasilber@mulhouse:~/work/mines/pdq$ time ./cmake-build-debug/pdq \
  -d ~/Dropbox/data_cons/source_data/DILA/JORFLEGI/Freemium_Legi_global_20191011-102231.tar.gz \
  -t /data/dq/raw 2> /data/dq/LEGI_20191011-102231.log\
  | dq_build_map /data/dq/www > /data/dq/LEGI_20191011-102231.www.log
real	9m30.741s
user	7m3.437s
sys	3m52.437s
```

Le problème avec cette approche est qu'elle génère beaucoup de fichiers, épuisant le nombre d'inodes du FS.
