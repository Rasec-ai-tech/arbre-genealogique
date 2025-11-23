#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MEMBRES 1000
#define MAX_ENFANTS 100
#define MAX_PARENTS 2

// Structure pour représenter une date
typedef struct {
    int annee;
    int mois;
    int jour;
} Date;

// Structure pour représenter une personne
typedef struct Personne {
    char nom[50];
    Date date_naissance;
    Date date_deces;
    struct Personne* enfants[MAX_ENFANTS];
    int nb_enfants;
    struct Personne* parents[MAX_PARENTS];
    int nb_parents;
} Personne;

// Structure pour représenter un arbre généalogique
typedef struct {
    Personne* membres[MAX_MEMBRES];
    int nb_membres;
} ArbreGenealogique;

// Fonction pour créer une personne
Personne* creer_personne(char* nom, Date naissance, Date deces) {
    Personne* p = (Personne*)malloc(sizeof(Personne));
    if (!p){
        printf("Erreur: Allocation mémoire échouée.\n");
        return NULL;
    }
    strcpy(p->nom, nom);
    p->date_naissance = naissance;
    p->date_deces = deces;
    p->nb_enfants = 0;
    p->nb_parents = 0;
    return p;
}

// Fonction pour ajouter une personne à l'arbre
void ajouter_personne(ArbreGenealogique* arbre, Personne* personne) {
    if (arbre->nb_membres < MAX_MEMBRES) {
        arbre->membres[arbre->nb_membres++] = personne;
    }
}

// Fonction pour ajouter un enfant à une personne
void ajouter_enfant(Personne* parent, Personne* enfant) {
    // Ajout de l'enfant
    if (parent->nb_enfants < MAX_ENFANTS) {
        parent->enfants[parent->nb_enfants++] = enfant;
    }
    // On précise aussi qui est son parent
    if (enfant->nb_parents < MAX_PARENTS) {
        enfant->parents[enfant->nb_parents++] = parent;
    }
}

// Fonction pour rechercher une personne dans l'arbre par son nom
Personne* rechercher_personne(ArbreGenealogique* arbre, char* nom) {
    for (int i = 0; i < arbre->nb_membres; i++) {
        if (strcmp(arbre->membres[i]->nom, nom) == 0) {
            return arbre->membres[i];
        }
    }
    return NULL;
}

// Fonction pour comparer deux dates servant à la détection d'une potentielle anomalie
int comparer_dates(Date d1, Date d2) {
    if (d1.annee != d2.annee) return d1.annee - d2.annee;
    if (d1.mois != d2.mois) return d1.mois - d2.mois;
    return d1.jour - d2.jour;
}

// Fonction pour vérifier si deux personnes sont frères/sœurs servant à la détection d'une potentielle anomalie
int sont_freres_soeurs(Personne* p1, Personne* p2) {
    for (int i = 0; i < p1->nb_parents; i++) {
        for (int j = 0; j < p2->nb_parents; j++) {
            if (p1->parents[i] == p2->parents[j]) {
                return 1;
            }
        }
    }
    return 0;
}

// Fonction pour vérifier si deux personnes sont cousins/cousines directs servant à la détection d'une potentielle anomalie
int sont_cousins_directs(Personne* p1, Personne* p2) {
    for (int i = 0; i < p1->nb_parents; i++) {
        for (int j = 0; j < p2->nb_parents; j++) {
            if (sont_freres_soeurs(p1->parents[i], p2->parents[j])) {
                return 1;
            }
        }
    }
    return 0;
}

// Fonction pour vérifier les anomalies
void verifier_anomalies(Personne* enfant) {
    // Vérifier si l'enfant est plus vieux que son père ou sa mère
    for (int i = 0; i < enfant->nb_parents; i++) {
        Personne* parent = enfant->parents[i];
        if (comparer_dates(enfant->date_naissance, parent->date_naissance) < 0) {
            printf("Anomalie: %s est plus vieux que son parent %s.\n", enfant->nom, parent->nom);
        }
    }

    // Vérifier si l'enfant est né d'un père décédé au moins 10 mois avant sa naissance
    for (int i = 0; i < enfant->nb_parents; i++) {
        Personne* parent = enfant->parents[i];
        if (parent->date_deces.annee != 0) {
            Date date_limite = parent->date_deces;
            date_limite.mois += 10;
            if (date_limite.mois > 12) {
                date_limite.mois -= 12;
                date_limite.annee += 1;
            }
            if (comparer_dates(enfant->date_naissance, date_limite) > 0) {
                printf("Anomalie: %s est ne plus de 10 mois après le deces de son parent %s.\n", enfant->nom, parent->nom);
            }
        }
    }

    // Vérifier les relations familiales interdites
    if (enfant->nb_parents == 2) {
        Personne* pere = enfant->parents[0];
        Personne* mere = enfant->parents[1];
        if (pere == mere) {
            printf("Anomalie: %s a le meme pere et mere.\n", enfant->nom);
        } else if (sont_freres_soeurs(pere, mere)) {
            printf("Anomalie: %s est ne de parents freres/soeurs.\n", enfant->nom);
        } else if (sont_cousins_directs(pere, mere)) {
            printf("Anomalie: %s est ne de parents cousins/cousines directs.\n", enfant->nom);
        }
    }
}

// Fonction pour enregistrer l'arbre dans un fichier
void enregistrer_arbre(ArbreGenealogique* arbre, const char* nom_fichier) {

    FILE* fichier = fopen(nom_fichier, "w");
    if (!fichier) {
        printf("Erreur: Impossible d'ouvrir le fichier pour l'écriture.\n");
        return;
    }

    // Écrire chaque personne dans le fichier
    for (int i = 0; i < arbre->nb_membres; i++) {
        Personne* p = arbre->membres[i];
        fprintf(fichier, "Personne\n");
        fprintf(fichier, "Nom: %s\n", p->nom);
        fprintf(fichier, "Naissance: %d %d %d\n", p->date_naissance.jour, p->date_naissance.mois, p->date_naissance.annee);
        fprintf(fichier, "Deces: %d %d %d\n", p->date_deces.jour, p->date_deces.mois, p->date_deces.annee);

        // Écrire les parents
        fprintf(fichier, "Parents: ");
        for (int j = 0; j < p->nb_parents; j++) {
            fprintf(fichier, "%s ", p->parents[j]->nom);
        }
        fprintf(fichier, "\n");

        // Écrire les enfants
        fprintf(fichier, "Enfants: ");
        for (int j = 0; j < p->nb_enfants; j++) {
            fprintf(fichier, "%s ", p->enfants[j]->nom);
        }
        fprintf(fichier, "\n");
    }

    fclose(fichier);
    printf("Arbre enregistre dans %s\n", nom_fichier);
}
// Fonction pour détecter de potentielles anomalies dans un arbre charger depuis un fichier
void detecter_anomalies(ArbreGenealogique* arbre) {
    if (arbre->nb_membres == 0) {
        printf("L'arbre est vide. Aucune anomalie à détecter.\n");
        return;
    }

    for (int i = 0; i < arbre->nb_membres; i++) {
        Personne* personne = arbre->membres[i];
        printf("Verification des anomalies pour %s...\n", personne->nom);
        verifier_anomalies(personne); // Utiliser la fonction existante
    }
}

// Fonction pour charger l'arbre à partir d'un fichier
void charger_arbre(ArbreGenealogique* arbre, const char* nom_fichier) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier) {
        printf("Erreur: Impossible d'ouvrir le fichier pour la lecture.\n");
        return;
    }

    char ligne[100];
    Personne* personne_courante = NULL;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strncmp(ligne, "Personne", 8) == 0) {
            // Nouvelle personne
            personne_courante = (Personne*)malloc(sizeof(Personne));
            personne_courante->nb_enfants = 0;
            personne_courante->nb_parents = 0;
            ajouter_personne(arbre, personne_courante);
        } else if (strncmp(ligne, "Nom: ", 5) == 0) {
            // Lire le nom
            strcpy(personne_courante->nom, ligne + 5);
            personne_courante->nom[strcspn(personne_courante->nom, "\n")] = 0;
        } else if (strncmp(ligne, "Naissance: ", 11) == 0) {
            // Lire la date de naissance
            sscanf(ligne + 11, "%d %d %d", &personne_courante->date_naissance.jour, &personne_courante->date_naissance.mois, &personne_courante->date_naissance.annee);
        } else if (strncmp(ligne, "Deces: ", 7) == 0) {
            // Lire la date de décès
            sscanf(ligne + 7, "%d %d %d", &personne_courante->date_deces.jour, &personne_courante->date_deces.mois, &personne_courante->date_deces.annee);
        } else if (strncmp(ligne, "Parents: ", 9) == 0) {
            // Lire les parents
            char* token = strtok(ligne + 9, " ");
            while (token) {
                Personne* parent = rechercher_personne(arbre, token);
                if (parent) {
                    ajouter_enfant(parent, personne_courante);
                }
                token = strtok(NULL, " ");
            }
        } else if (strncmp(ligne, "Enfants: ", 9) == 0) {
            // Lire les enfants
            char* token = strtok(ligne + 9, " ");
            while (token) {
                Personne* enfant = rechercher_personne(arbre, token);
                if (enfant) {
                    ajouter_enfant(personne_courante, enfant);
                }
                token = strtok(NULL, " ");
            }
        }
    }

    fclose(fichier);
    printf("Arbre charge depuis %s\n", nom_fichier);

    // Vérifier les anomalies après le chargement
    detecter_anomalies(arbre);
}


// Fonction pour afficher l'arbre généalogique
void afficher_arbre(ArbreGenealogique* arbre) {

    printf("\n**************************************************");
    for (int i = 0; i < arbre->nb_membres; i++) {
        Personne* p = arbre->membres[i];
        printf("\nNom: %s (Ne(e) le %d/%d/%d)\n", p->nom, p->date_naissance.jour, p->date_naissance.mois, p->date_naissance.annee);
        if (p->date_deces.annee != 0) {
            printf("Decede(e) le %d/%d/%d\n", p->date_deces.jour, p->date_deces.mois, p->date_deces.annee);
        }
        if (p->nb_parents > 0) {
            printf("Parents: ");
            for (int j = 0; j < p->nb_parents; j++) {
                printf("%s ", p->parents[j]->nom);
            }
            printf("\n");
        }
        else{
            printf("Parents : Neant");
            printf("\n");
        }
        if (p->nb_enfants > 0) {
            printf("Enfants: ");
            for (int j = 0; j < p->nb_enfants; j++) {
                printf("%s ", p->enfants[j]->nom);
            }
            printf("\n");
        }
        else{
            printf("Enfants : Neant");
            printf("\n");
        }
    }
    printf("\n**************************************************");
}

// Fonction pour entrer une date (naissance ou deces)
Date entrer_date() {
    Date d;
    printf("(jour mois annee) : ");
    scanf("%d %d %d", &d.jour, &d.mois, &d.annee);
    return d;
}
void liberer_personne(Personne* p) {
    if (!p) return;
    free(p);
}

void liberer_arbre(ArbreGenealogique* arbre) {
    for (int i = 0; i < arbre->nb_membres; i++) {
        liberer_personne(arbre->membres[i]);
    }
    arbre->nb_membres = 0;
}

// Fonction principale
int main() {
    ArbreGenealogique arbre = {{0}, 0};
    char nom[50], parent_nom[50];
    int choix;
    char nomFamille[20];
    printf("Quel est le nom de la famille dont vous souhaitez realiser l'arbre genealogique ?\n");
    scanf("%s", nomFamille);
    do {
        printf("\n**************************************************");
        printf("\nMenu :\n");
        printf("1. Ajouter une personne\n");
        printf("2. Ajouter une relation parent-enfant (ou enfant-parent)\n");
        printf("3. Afficher l'arbre\n");
        printf("4. Enregistrer l'arbre dans un fichier\n");
        printf("5. Charger l'arbre depuis un fichier\n");
        printf("6. Quitter\n");
        printf("**************************************************\n");
        printf("Votre choix : ");
        scanf("%d", &choix);
        getchar();
        printf("\n**************************************************\n");
        switch (choix) {
            case 1:
                printf("Entrez le nom : ");
                fgets(nom, sizeof(nom), stdin);
                nom[strcspn(nom, "\n")] = 0;
                printf("Date de naissance :\n");
                Date naissance = entrer_date();
                printf("Date de deces (0 0 0 si vivant) :\n");
                Date deces = entrer_date();
                // Appel de la fonction ajouter_personne pour ajouter la nouvelle personne qui vient d'être créée
                ajouter_personne(&arbre, creer_personne(nom, naissance, deces));
                break;
            case 2:
                 if (arbre.nb_membres == 0) {
                    printf("Erreur: L'arbre est vide. Ajoutez d'abord des personnes.\n");
                    break;
                }
                printf("Nom de l'enfant : ");
                fgets(nom, sizeof(nom), stdin);
                nom[strcspn(nom, "\n")] = 0;
                // On s'assure que l'enfant existe en tant que personne
                Personne* enfant = rechercher_personne(&arbre, nom);
                if (!enfant) {
                    printf("Personne introuvable.\n");
                    break;
                }
                printf("Nom du parent : ");
                fgets(parent_nom, sizeof(parent_nom), stdin);
                parent_nom[strcspn(parent_nom, "\n")] = 0;
                // On s'assure que le parent existe en tant qu'une personne
                Personne* parent = rechercher_personne(&arbre, parent_nom);
                if (!parent) {
                    printf("Personne introuvable.\n");
                    break;
                }
                // Appel de la fonction ajouter_enfant pour établir une relation enfant-parent et parent-enfant
                ajouter_enfant(parent, enfant);
                verifier_anomalies(enfant);
                break;
            case 3:
                 if (arbre.nb_membres == 0) {
                    printf("L'arbre est vide. Aucune donnee a afficher.\n");
                    break;
                }
                else{
                    printf("Les membres de la famille %s sont\n", nomFamille);
                    afficher_arbre(&arbre);
                    break;
                }
            case 4:
                if (arbre.nb_membres == 0) {
                    printf("Erreur: L'arbre est vide. Aucune donnee a enregistrer.\n");
                    break;
                }
                else{
                    printf("Entrez le nom du fichier pour enregistrer : ");
                    fgets(nom, sizeof(nom), stdin);
                    nom[strcspn(nom, "\n")] = 0;
                    enregistrer_arbre(&arbre, nom);
                    break;
                }
            case 5:
                printf("Entrez le nom du fichier pour charger : ");
                fgets(nom, sizeof(nom), stdin);
                nom[strcspn(nom, "\n")] = 0;
                charger_arbre(&arbre, nom);
                break;
            case 6:
                printf("Nous esperons avoir repondu a vos attentes. Au revoir et a bientot !\n");
                liberer_arbre(&arbre);
                break;
            default:
                printf("Choix invalide, recommencez.\n");
        }
    } while (choix != 6);
    return 0;
}
