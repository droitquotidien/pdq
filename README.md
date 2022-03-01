# C parser for "Droit Quotidien" data

## Parsing des données 

```xml
<TEXTELR>
<VERSIONS>
<VERSION etat="">
<LIEN_TXT debut="2999-01-01" fin="2999-01-01" id="JORFTEXT000000312994" num="48-178"/>
</VERSION>
<VERSION etat="ABROGE">
<LIEN_TXT debut="1948-02-03" fin="2022-02-16" id="LEGITEXT000006068045" num="48-178"/>
</VERSION>
</VERSIONS>
<STRUCT>
<LIEN_ART debut="1948-02-03" etat="MODIFIE" fin="2001-01-01" 
    id="LEGIARTI000006272563" num="2" origine="LEGI"/>
<LIEN_ART debut="2001-01-01" etat="ABROGE" fin="2022-02-16" 
    id="LEGIARTI000006272564" num="2" origine="LEGI"/>
</STRUCT>
</TEXTELR>

<TEXTE_VERSION>
  <META>
    <META_COMMUN>...</META_COMMUN>
    <META_SPEC>
      <META_TEXTE_CHRONICLE>...</META_TEXTE_CHRONICLE>
        <META_TEXTE_VERSION>
            <LIENS>
                <LIEN cidtexte="JORFTEXT000045167534" datesignatexte="2022-02-14" 
                    id="LEGIARTI000045168380" naturetexte="LOI" nortexte="TFPX2117307L" 
                    num="unique" numtexte="2022-171" sens="cible" 
                    typelien="ABROGE">LOI n°2022-171 du 14 février 2022 - art. unique (V)</LIEN>
            </LIENS>
        </META_TEXTE_VERSION>
    </META_SPEC>
    </META>
<VISAS><CONTENU/></VISAS>
<SIGNATAIRES><CONTENU/></SIGNATAIRES>
<TP><CONTENU/></TP>
<NOTA><CONTENU/></NOTA>
<ABRO><CONTENU/></ABRO>
<RECT><CONTENU/></RECT>
</TEXTE_VERSION>

```

