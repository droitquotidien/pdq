# Roadmap

## 0.1

- [X] Mécanisme de multi-URI (uri, uri_bis, uri_ter)
    - [X] textes: stocker NUM, NOR, ID commes id alters
    - [X] articles: stocker NUM, ID commes id alters
    - [X] section: stocker SECNUM, ID commes id alters
    - [X] ajouter trois champs en fin de ligne de production URI
    - [X] utiliser `;` comme séparateur de champs (pour champ vide)
    - [X] définir les URI avec un template à trois niveaux max `XX/%s[/%s[/%s]`
- [X] Support CONTEXTE (SCTA, ARTI)
- [X] Support VERSION_A_VENIR (LEGIVERS)
- [X] Support VERSIONS
- [X] Passer à un parsed_data modulaire. versions, etc, n'ont pas forcément besoin d'être mis à zero avant d'écrire dedans. Permet de gagner du temps sur le memset(). Essayer le realloc également.
- [X] Support LIENS
- [X] Support MCS_ART
- [X] Support MCS_TXT

## 0.2

- [ ] Support ENTREPRISES
- [ ] Ne plus transformer les TEXT en VERS
- [ ] Virer la notion de RID
- [ ] Passer à des BigInteger?
- [ ] Ne plus calculer d'URI
- [ ] Update auto à partir d'un répertoire

