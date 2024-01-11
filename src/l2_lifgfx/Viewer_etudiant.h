
#ifndef VIEWER_ETUDIANT_H
#define VIEWER_ETUDIANT_H

#include "Viewer.h"

    const int nbr_arbre = 2;


class ViewerEtudiant : public Viewer
{
public:
    ViewerEtudiant();

    int init();
    int render();
    int update( const float time, const float delta );
    Vector terrainNormal(const Image& im, const int i, const int j);

protected:

    /* -----------------------------------------
     Pour creer un nouvel objet vous devez :

     1. declarer ici dans le fichier Viewer_etudiant.h
     le Mesh,
     la texture si besoin,
     une fonction 'init_votreObjet'
     une fonction 'draw_votreObjet(const Transform& T)'

     --- Rq : regarder comment cela est effectue dans le fichier Viewer.h


     2. Appeler la fonction 'init_votreObjet' dans la fonction 'init' du Viewer_etudiant.cpp
     --- Rq : regarder comment cela est effectue dans le fichier Viewer.cpp


     3. Appeler la fonction 'draw_votreObjet' dans la fonction 'render' du Viewer_etudiant.cpp
     --- Rq : regarder comment cela est effectue dans le fichier Viewer.cpp

      ----------------------------------------- */
    //Declaration pour les element de foret
    Vector tabforet[nbr_arbre];

    /// Declaration des Mesh
    Mesh m_cubemap;
    Mesh m_terrain;
    Mesh m_building1;
    Mesh m_building2;
    Mesh m_wall1;
    Mesh m_wallquad;
    Mesh m_launchpad;
    Mesh m_rocket;
    //FORMES DE BASES
    Mesh m_fcube;
    Mesh m_fcylindre;
    Mesh m_fcercle;
    Mesh m_fcone;
    Mesh m_fsphere;
    //Billboard
    Mesh m_tree;

    ///Déclaration de l'image a importer pour le terrain
    Image m_terrainAlti;
    Image m_test;

    /// Declaration des Textures
    GLuint m_terrain_texture;
    GLuint m_tree_texture;
    GLuint m_moon_texture;

    GLuint m_building1_texture;
    GLuint m_building2_texture;
    GLuint m_wallquadtext;
    GLuint m_rocket_text;
    //TEXTURE FORME DE BASE
    GLuint m_base_text;
    GLuint m_earth_text;


    /// Declaration des fonction de creation de Mesh du type init_votreObjet()
    //Billboard
    void init_foret(const Image& im);
    void init_tree();

    void init_terrain(const Image& im);
    void init_cubemap();
    void init_space();
    void init_wall1();
    void init_wallquad();
    //FORME DE BASE
    void init_fcube();
    void init_fcylindre();
    void init_fcercle();
    void init_fcone();
    void init_fsphere();

    /// Declaration des fonctions draw_votreObjet(const Transform& T)
    void draw_fcylindre(const Transform& T);
    void draw_avion(const Transform& T);
    void draw_tree(const Transform& T);
    void draw_foret(const Transform& T);

    ///Partie declaration pour les animation
    Transform m_tavion;
    Transform m_trocket;


};



#endif
