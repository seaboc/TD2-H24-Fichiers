#pragma once
// Structures mémoires pour une collection de films.
#include <iostream>
#include <string>
#include <istream>
using namespace std;

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class ListeFilms {
	private: 
	int capacite_ = 0;
	int nElements_ = 0;
	public:
	ListeFilms() {};
	//ListeFilms(int capacite, int nElements, Film** elements) {};
	void ajouterFilmListe(Film* film);
	void enleverFilmListe(Film* film);
	void detruireFilm(Film* film);
	void detruireListeFilms();
	void afficherFilm(const Film& film) const;
	void afficherListeFilms() const;
	void afficherActeur(const Acteur& acteur) const;
	void afficherFilmographieActeur(const string& nomActeur) const;
	Acteur* trouverActeur(string nom_acteur) const;
	Acteur* lireActeur(istream& fichier);
	Film* lireFilm(istream& fichier);
	static ListeFilms creerListe(string nomFichier);
	Film** elements = nullptr;
	
	int getCapacite() const{
		return capacite_;
	}

	int getNElements() const{
		return nElements_;
	}

	// void setCapacite(int capacite){
    // 	capacite_ = capacite;
	// }
    // void setNElements(int nElements){
	// 	nElements_ = nElements;
	// }
    // void setElements(Film** elements){
	// 	elements_ = elements;
	// }
};


struct ListeActeurs {
	int capacite, nElements;
	Acteur** elements; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
};

struct Film
{
	std::string titre, realisateur; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie, recette; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
};

struct Acteur
{
	std::string nom; int anneeNaissance; char sexe;
	ListeFilms joueDans;
};
