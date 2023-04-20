# Parseur en C pour les données juridiques de "Droit Quotidien"

Ce programme lit les fichiers XML contenus dans les archives TGZ fournies
par la DILA (https://echanges.dila.gouv.fr/OPENDATA/) et les importe dans
une base de données PostgreSQL.

## Ubuntu / Debian Linux

Prérequis pour compiler le code:

```bash
apt-get install cmake
apt-get install libarchive-dev
apt-get install libxml2-dev libxml2-utils
apt-get install postgresql-server-dev-14
```

Compilation, dans ce répertoire, faire:

```bash
mkdir build
cd build
cmake ..
make
```

Dans le répertoire `build`, l'exécutable `pdq` est le programme d'importation
principal.
