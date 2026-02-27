# Parseur en C et importation en base de données pour les données juridiques de la DILA

Ce programme lit les fichiers XML contenus dans les archives TGZ fournies par la DILA (https://echanges.dila.gouv.fr/OPENDATA/) et les importe dans une base de données PostgreSQL, sans écriture sur disque des fichiers XML décompressés.

## Compilation

### Ubuntu / Debian Linux

Prérequis pour compiler le code:

```bash
apt-get install cmake
apt-get install build-essentials
apt-get install gcc-12 g++-12  # ou une autre version
apt-get install libarchive-dev
apt-get install libxml2-dev libxml2-utils
apt-get install postgresql-server-dev-14  # ou une autre version
```

Compilation, dans ce répertoire, faire:

```bash
mkdir build
cd build
cmake ..
make
```

Dans le répertoire `build`, l'exécutable `pdq` est le programme d'importation principal.

### macOS

TODO

## Utilisation

Importation d'un fichier de stock JORF:

```bash
pdq -c postgresql://dqdata:dqdata@localhost:5432/dqdata \
    -d Freemium_jorf_global_20250713-140000.tar.gz \
    -l logfile.txt
```

Importation d'un fichier de flux LEGI, log sur la sortie standard:

```bash
pdq -c postgresql://dqdata:dqdata@localhost:5432/dqdata \
    -d LEGI_20250718-215845.tar.gz
```
