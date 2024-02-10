﻿#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>

#include "cppitertools/range.hpp"
#include "gsl/span"

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).

using namespace std;
using namespace iter;
using namespace gsl;

#pragma endregion//}

typedef uint8_t UInt8;
typedef uint16_t UInt16;

#pragma region "Fonctions de base pour lire le fichier binaire"//{
template <typename T>
T lireType(istream& fichier)
{
	T valeur{};
	fichier.read(reinterpret_cast<char*>(&valeur), sizeof(valeur));
	return valeur;
}
#define erreurFataleAssert(message) assert(false&&(message)),terminate()
static const uint8_t enteteTailleVariableDeBase = 0xA0;
size_t lireUintTailleVariable(istream& fichier)
{
	uint8_t entete = lireType<uint8_t>(fichier);
	switch (entete) {
	case enteteTailleVariableDeBase+0: return lireType<uint8_t>(fichier);
	case enteteTailleVariableDeBase+1: return lireType<uint16_t>(fichier);
	case enteteTailleVariableDeBase+2: return lireType<uint32_t>(fichier);
	default:
		erreurFataleAssert("Tentative de lire un entier de taille variable alors que le fichier contient autre chose à cet emplacement.");
	}
}

string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUintTailleVariable(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}

//TODO: Une fonction pour ajouter un Film à une ListeFilms, 
//le film existant déjà; on veut uniquement ajouter le pointeur vers le film existant.  
//Cette fonction doit doubler la taille du tableau alloué, avec au minimum un élément, 
//dans le cas où la capacité est insuffisante pour ajouter l'élément.  Il faut alors allouer un nouveau tableau plus grand, 
//copier ce qu'il y avait dans l'ancien, et éliminer l'ancien trop petit.  
//Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
void ajouterFilmListe(ListeFilms& liste){ // il y avait film mais j'ai enlever pcq ils ont dis qu'on lutilise pas 
	int nouvelle_capacite;
	if (liste.nElements == liste.capacite){
		if (liste.capacite == 0){
			nouvelle_capacite = 1;
		}
		else {
			nouvelle_capacite = liste.capacite * 2; //pour doubler la capacité
		}
		// allouer un nouveau tableau plus grand
		Film** nouveau_tableau = new Film*[nouvelle_capacite];
		for(auto i : range(liste.nElements)){
			nouveau_tableau[i] = liste.elements[i]; //copier ce qu'il y avait dans l'ancien
		}
		delete[] liste.elements; // elements est le pointeur vers le premier element : donc on delete le pointeur
	}
}

//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; 
//la fonction prenant en paramètre un pointeur vers le film à enlever.  L'ordre des films dans la liste n'a pas à être conservé.
void enleverFilmListe(ListeFilms& liste, Film* film){
	for(auto i : range(liste.nElements)){
		if (liste.elements[i] == film){
			for(auto j : range(liste.nElements - i - 1)){ //a checker urgent
				liste.elements[j + i] = liste.elements[i+ j + 1];
			}
			liste.nElements--;
			liste.elements[liste.nElements] = nullptr;
			break;
		}
	}
}

//TODO: Une fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur,
// ou nullptr si l'acteur n'est pas trouvé.  Devrait utiliser span.
Acteur* trouverActeur(const ListeFilms& liste, const string& nom_acteur){
	span<Film*> collection_films(liste.elements, liste.nElements);
	for(Film* film : collection_films){ // pour chaque pointeur de film
		span<Acteur*> collection_acteur(film -> acteurs.elements, film -> acteurs.nElements); //span de pointeurs vers des acteur du film qu'on itere
		for(Acteur* acteur: collection_acteur){ //pour chaque pointeur d'acteur
			if (acteur->nom == nom_acteur) { //si le nom de l'acteur correspond au pointeur d'acteur qui est en train d'être itéré
				return acteur; 
			}
		}
	}
	return nullptr; //si jamais aucun acteur n'a été trouvé
}

//TODO: Compléter les fonctions pour lire le fichier et créer/allouer une ListeFilms.
// La ListeFilms devra être passée entre les fonctions, pour vérifier l'existence d'un Acteur avant de l'allouer à nouveau
// (cherché par nom en utilisant la fonction ci-dessus).
Acteur* lireActeur(istream& fichier, ListeFilms& liste) // verifier 
{
	Acteur acteur = {}; 
	acteur.nom            = lireString(fichier);
	acteur.anneeNaissance = int(lireUintTailleVariable (fichier));
	acteur.sexe           = char(lireUintTailleVariable(fichier));
	Acteur* acteur_existant = trouverActeur(liste, acteur.nom); 
	if (acteur_existant != nullptr){
		return acteur_existant; // s'il existe deja
	}
	else{
		Acteur* acteur_nv = new Acteur(acteur);
		return acteur_nv;
	}
	//cout << "nom des acteurs crees: " << acteur.nom << endl;
	return {}; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations,
	// selon si l'acteur existait déjà.Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir 
	//le même nom d'acteur affiché deux fois pour la création.
}

Film* lireFilm(istream& fichier, ListeFilms& liste)
{
	Film film = {};
	film.titre       = lireString(fichier);
	film.realisateur = lireString(fichier);
	film.anneeSortie = int(lireUintTailleVariable(fichier));
	film.recette     = int(lireUintTailleVariable(fichier));
	film.acteurs.nElements = int(lireUintTailleVariable(fichier));
	Acteur** liste_acteurs = new Acteur*[film.acteurs.nElements]; //alloue tableau de pointeurs (je fais un ptr vers un tab de ptr)
	Film *nv_film = new Film(film); //alloue ptr film
	nv_film->acteurs.elements = liste_acteurs;
	//NOTE: Vous avez le droit d'allouer d'un coup le 
	//tableau pour les acteurs, sans faire de réallocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions
	// d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour 
	//réutiliser cette réallocation.
	for (int i = 0; i < nv_film->acteurs.nElements; i++) {
		Acteur* acteur = lireActeur(fichier, liste); //TODO: Placer l'acteur au bon endroit dans les acteurs du film.
		nv_film->acteurs.elements[i] = acteur;
		//TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.;
		ajouterFilmListe(acteur->joueDans);
	
	}	
	return (nv_film); //TODO: Retourner le pointeur vers le nouveau film.
}

ListeFilms creerListe(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	
	int nElements = int(lireUintTailleVariable(fichier));

	//TODO: Créer une liste de films vide.
	ListeFilms liste;
	liste.capacite = nElements;
	liste.nElements = 0;
	liste.elements = new Film* [liste.capacite];
	for (int i = 0; i < nElements; i++) {
		Film* film = lireFilm(fichier, liste); //TODO: Ajouter le film à la liste.
		ajouterFilmListe(liste); 
	}
	
	return (liste); //TODO: Retourner la liste de films.
}

//TODO: Une fonction pour détruire un film (relâcher toute la mémoire associée à ce film, et les acteurs qui ne jouent plus
// dans aucun films de la collection).  Noter qu'il faut enleve le film détruit des films dans lesquels jouent les acteurs.
//  Pour fins de débogage, affichez les noms des acteurs lors de leur destruction.
void detruireFilm(Film* film, ListeFilms& liste)
{
	span<Film*> collection_films(liste.elements, liste.nElements);
	//for(Film* chaque_film : collection_films){  check avec alex 
		span<Acteur*> collection_acteur(film -> acteurs.elements, film -> acteurs.nElements); 
		for(Acteur* chaque_acteur : collection_acteur){
			if (chaque_acteur -> joueDans.nElements == 1){
				delete chaque_acteur;
			}
			span<Film*> collection_film_acteur(chaque_acteur-> joueDans.elements, chaque_acteur-> joueDans.nElements);
			for(Film* film_acteur : collection_film_acteur) {
				if (film_acteur == film){
					enleverFilmListe(liste, film_acteur);
					delete film_acteur;
				}
			}
		}
		delete film;
}


//TODO: Une fonction pour détruire une ListeFilms et tous les films qu'elle contient.
void detruireListeFilms(ListeFilms& liste)
{
	for (auto i : range(liste.nElements)){
		detruireFilm(liste.elements[i], liste);
	}
}

void afficherActeur(const Acteur& acteur)
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

//TODO: Une fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).
void afficherFilm(const Film& film)
{
	cout<< " " << film.titre << ", " << film.realisateur << ", " << film.anneeSortie << ", " << film.recette << endl;
	for(auto i : range(film.acteurs.nElements)){
		const Acteur* acteur = film.acteurs.elements[i];
		afficherActeur(* acteur); // dereference donc c'est la valeur de l'acteur elle meme
	}
}

void afficherListeFilms(const ListeFilms& listeFilms)
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de
	// séparations dans ce progamme).
	static const string ligneDeSeparation = "----------------------------------------------------";
	cout << ligneDeSeparation;
	//TODO: Changer le for pour utiliser un span.
	span<Film*> films(listeFilms.elements,listeFilms.nElements);
		for (auto i : range(listeFilms.nElements)){
			//TODO: Afficher le film.
			afficherFilm(*listeFilms.elements[i]);
		}
		cout << ligneDeSeparation << endl;
}


void afficherFilmographieActeur(const ListeFilms& listeFilms, const string& nomActeur)
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	const Acteur* acteur = trouverActeur(listeFilms, nomActeur);
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		afficherListeFilms(acteur->joueDans);
}

int main()
{
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	//int* fuite = new int; //TODO: Enlever cette ligne après avoir vérifié qu'il y a bien un "Fuite detectee" de "4 octets" affiché à la fin de l'exécution, qui réfère à cette ligne du programme.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions écrites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.  Devrait afficher les
	// noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans votre fonction lireActeur).
	ifstream fichier("films.bin");
	ListeFilms listeFilms = creerListe("films.bin");
	
	// for (auto i : range(listeFilms.nElements)){
	// 	afficherActeur(*listeFilms.elements[i]-> acteurs.elements[0])
	// }
	lireActeur(fichier, listeFilms);

	cout << ligneDeSeparation << "Le premier film de la liste est:" << endl;
	//TODO: Afficher le premier film de la liste.  Devrait être Alien.
	afficherFilm(*listeFilms.elements[0]);
	
	cout << ligneDeSeparation << "Les films sont:" << endl;
	//TODO: Afficher la liste des films.  Il devrait y en avoir 7.
	afficherListeFilms(listeFilms);
	
	//TODO: Modifier l'année de naissance de Benedict Cumberbatch pour être 1976 (elle était 0 dans les données lues du fichier).
	// Vous ne pouvez pas supposer l'ordre des films et des acteurs dans les listes, il faut y aller par son nom.
	Acteur* benedictC = trouverActeur(listeFilms, "Benedict Cumberbatch");
	benedictC -> anneeNaissance = 1976;

	cout << ligneDeSeparation << "Liste des films où Benedict Cumberbatch joue sont:" << endl;
	//TODO: Afficher la liste des films où Benedict Cumberbatch joue.  Il devrait y avoir Le Hobbit et Le jeu de l'imitation.
	afficherListeFilms(benedictC->joueDans);
	//TODO: Détruire et enlever le premier film de la liste (Alien).  Ceci devrait "automatiquement"
	// (par ce que font vos fonctions) détruire les acteurs Tom Skerritt et John Hurt, mais pas 
	//Sigourney Weaver puisqu'elle joue aussi dans Avatar.
	detruireFilm(listeFilms.elements[0],listeFilms);
	
	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	//TODO: Afficher la liste des films.
	afficherListeFilms(listeFilms);
	//TODO: Faire les appels qui manquent pour avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge
	// dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le
	// droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.
	
	//TODO: Détruire tout avant de terminer le programme.  La bibliothèque de verification_allocation devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
}
