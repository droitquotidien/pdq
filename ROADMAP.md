# Roadmap

- [X] Mécanisme de multi-URI (uri, uri_bis, uri_ter)
    - [X] textes: stocker NUM, NOR, ID commes id alters
    - [X] articles: stocker NUM, ID commes id alters
    - [X] section: stocker SECNUM, ID commes id alters
    - [X] ajouter trois champs en fin de ligne de production URI
    - [X] utiliser `;` comme séparateur de champs (pour champ vide)
    - [X] définir les URI avec un template à trois niveaux max `XX/%s[/%s[/%s]`
- [ ] Formaliser toutes les possibilités de collision d'URI
- [ ] Normaliser les natures pour les URI
- [ ] Identifiants de section s1.2.3
- [ ] Parse des données textuelles et stockage en mémoire
- [ ] Mécanisme de relation avec le traitement des URI (co-routine)
  - [ ] yield Python pour traitement et création