#pragma once
// Structures mémoires pour une collection de films.

#include <string>

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class ListeFilms {
	private: 
	int capacite_, nElements_;
	Film** elements_;
	public:
	ListeFilms();
	ListeFilms(int capacite, int nElements, Film** elements);
	void ajouterFilmListe(ListeFilms& liste, Film* film);
	void enleverFilmListe(ListeFilms& liste, Film* film);
	void detruireFilm(Film* film, ListeFilms& liste);
	void detruireListeFilms(ListeFilms& liste);
	void afficherListeFilms(const ListeFilms& listeFilms) const;
	void afficherFilmographieActeur(const ListeFilms& listeFilms, const string& nomActeur) const;
	static ListeFilms creerListe(const string nomFichier);
	 // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
};

ListeFilms::ListeFilms(int capacite, int nElements, Film** elements){
	capacite_ = capacite_;
	nElements_ = nElements;
	elements_ = elements;
}

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
