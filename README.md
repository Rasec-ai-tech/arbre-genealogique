# Projet : Implémentation d’un Arbre Généalogique en C

Projet réalisé dans le cadre du cours Algorithmique, Structures de Données Avancées et complexités.  
Ce programme permet de créer, manipuler et analyser un arbre généalogique tout en détectant des incohérences familiales.

## 1. Objectif du projet

Écrire un programme capable de :

- créer et gérer un arbre généalogique,
- afficher les membres avec leurs relations,
- détecter les situations anormales (incestes, dates incohérentes, etc.),
- charger et sauvegarder les données dans un fichier texte.


## 2. Entrées

Le programme accepte :

- un ensemble de personnes, chacune décrite par :
  - nom
  - date de naissance
  - date de décès (ou 0 0 0 si vivant)
  - parents (0, 1 ou 2)
  - enfants (0 à 100)

Les informations peuvent être :

- saisies interactivement,
- chargées depuis un fichier.


## 3. Sorties

- Affichage textuel simple de l’arbre généalogique.
- Sauvegarde des données dans un fichier.


## 4. Structures de données utilisées

### Structure "Personne"

Contient :
- nom[50]
- date_naissance
- date_deces
- parents[2]
- enfants[100]
- compteurs nb_parents et nb_enfants

### Structure "ArbreGenealogique"

- membres[1000]
- nb_membres

### Structure "Date"

- jour, mois, annee


## 5. Principales fonctions

Voici un aperçu de leurs rôles et complexités.

- creer_personne() → création dynamique (O(1))
- ajouter_personne() → ajout dans l’arbre (O(1))
- ajouter_enfant() → relation parent–enfant (O(1))
- rechercher_personne() → recherche d’un membre (O(n))
- comparer_dates() → comparaison de dates (O(1))
- sont_freres_soeurs() → vérifie un parent commun (O(1))
- sont_cousins() → vérifie si les parents sont frères/sœurs (O(1))
- verifier_anomalies() → détecte incohérences :  
  - enfant né après décès du parent  
  - enfant plus vieux que parent  
  - parents frères/sœurs  
- enregistrer_arbre() → sauvegarde (O(n))
- charger_arbre() → reconstruction complète (O(n²))
- afficher_arbre() → affichage textuel (O(n))


## 6. Détection des anomalies

Le programme identifie plusieurs incohérences :

- enfant né avant son parent  
- enfant né plus vieux que son parent  
- enfant né plus de 10 mois après le décès d’un parent  
- parents qui sont :
  - frères et sœurs  
  - cousins directs  
- etc.

## 7. Compilation & exécution

  gcc main.c -o main
  ./main
