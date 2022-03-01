# Roadmap

- [ ] Mécanisme de multi-URI (uri, uri_bis, uri_ter)
    - [ ] textes: stocker NUM, NOR, ID commes id alters
    - [ ] articles: stocker NUM, ID commes id alters
    - [ ] section: stocker SECNUM, ID commes id alters
    - [ ] ajouter trois champs en fin de ligne de production URI
    - [ ] utiliser `;` comme séparateur de champs (pour champ vide)
    - [ ] définir les URI avec un template à trois niveaux max `XX/%s[/%s[/%s]`
- [ ] Formaliser toutes les possibilités de collision d'URI
- [ ] Identifiants de section s1.2.3
- [ ] Parse des données textuelles et stockage en mémoire
- [ ] Mécanisme de relation avec le traitement des URI (co-routine)
  - [ ] yield Python pour traitement et création