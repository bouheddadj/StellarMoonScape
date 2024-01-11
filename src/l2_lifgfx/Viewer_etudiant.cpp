
#include <cassert>
#include <cmath>

#include <iostream>
#include <time.h>

#include "wavefront.h"
#include "orbiter.h"
#include "mesh.h"
#include "wavefront_fast.h"

#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "Viewer_etudiant.h"

using namespace std;

/////////////////////////////////////////////////////////////////////
///1. Les fonctions dans lesquelle les initialisations sont faites.//
/////////////////////////////////////////////////////////////////////

int ViewerEtudiant::init()
{
    Viewer::init();

    srand((unsigned int)time(NULL));

    m_camera.lookat( Point(0,0,0), 150 );

	///1.1 Chargement d'imaages pour les terrains
	//m_test = read_image ("../data/terrain/terrain.png");
    m_terrainAlti = read_image("../data/terrain/moon6.png");

	///1.2 Appel des fonctions init_votreObjet pour creer les Mesh
	init_cubemap();
    init_terrain(m_terrainAlti);
    //Billboard
    init_foret(m_terrainAlti);

    init_wall1();
    init_wallquad();
    //FORMES DE BASES
    init_fcube();
    init_fcylindre();
    init_fcercle();
    init_fcone();
    init_fsphere();
    init_tree();


	///1.3 Partie chargement des textures
    glEnable(GL_TEXTURE_2D);

    //TEXTURE FORME DE BASE
    m_base_text = read_texture (0,"../data/mur.png");
    m_earth_text = read_texture (0,"../data/monde.png");

    //AUTRE TEXTURE
    m_moon_texture = read_texture(0,"../data/terrain/sky10.png");
    m_terrain_texture = read_texture(0,"../data/terrain/moon6.png");

    m_building1_texture = read_texture(0,"../data/moon/buildings/Office building/Office building/Office building.png");
    m_building2_texture = read_texture(0,"../data/moon/buildings/Office building/Office building/Radar_building_.png");
    m_wallquadtext = read_texture(0,"../data/moon/wall/38816.png");
    //m_rocket_text = read_texture(0,"../data/moon/rockets/Flag_of_France.png");

    //Arbres
    m_tree_texture = read_texture(0,"../data/billboard/tree2.png");


    ///1.4 Partie importation objet pret
    m_building1 = read_mesh ("../data/moon/buildings/Office building/Office building/Office building.obj");
    m_building2 = read_mesh ("../data/moon/buildings/Radar_1_/Radar_/Radar_.obj");
    m_launchpad = read_mesh ("../data/moon/buildings/Gantry/Gantry.obj");
    m_rocket = read_mesh ("../data/obj/Saturn V.obj");

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///2. Définition des fonctions permettant la creation du Mesh de votre Objet, Aussi Les fonctions permattant l'affichage//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ViewerEtudiant::draw_tree(const Transform& T)
{
    for (int j=0; j<360; j+=22.5)
    {
        gl.alpha_texture(m_tree_texture);
       // gl.model();
        gl.model(T * Scale(1,1,1)*Translation(0,2,0)*RotationY(j));
        gl.draw(m_tree);

    }

}

void ViewerEtudiant::init_foret(const Image& im)
{
    float x,y,z;

    for (int i = 0; i <= nbr_arbre; i++)
    {
        x = (rand() % im.width());
        z = (rand() % im.height());
        y = 25.f*im(x, z).r;

        std :: cout << " arbre en " << x << " " << y << " " << z << std::endl;
        tabforet[i] = Vector(x,y,z);
    }
}

void ViewerEtudiant::draw_foret(const Transform& T)
{
    for (int i = 0; i <= nbr_arbre; i++)
    {
       // gl.model(T*Translation(tabforet[i]) );//*Translation(-96,0,-96));
        draw_tree(T*Translation(tabforet[i]));

        //Scale(0.1,0.1,0.1)*Translation(-96,0,-96)
        // * Scale(0.1,0.1,0.1)
    }
}

void ViewerEtudiant::init_tree ()
{
    m_tree = Mesh(GL_TRIANGLE_STRIP);
    m_tree.color( Color(1, 1, 1));

    m_tree.normal(  0, 0, 1 );

    m_tree.texcoord(0,0 );
    m_tree.vertex(-1, -1, 0 );

    m_tree.texcoord(1,0);
    m_tree.vertex(  1, -1, 0 );

    m_tree.texcoord(0,1);
    m_tree.vertex( -1, 1, 0 );

    m_tree.texcoord( 1,1);
    m_tree.vertex(  1,  1, 0 );

}

Vector ViewerEtudiant::terrainNormal(const Image& im, const int i, const int j)
{
// Calcul de la normale au point (i,j) de l’image
    int ip = i-1;
    int in = i+1;
    int jp = j-1;
    int jn = j+1;

    Vector a( ip, im(ip, j).r, j );
    Vector b( in, im(in, j).r, j );
    Vector c( i, im(i, jp).r, jp );
    Vector d( i, im(i, jn).r, jn );
    Vector ab = normalize(b - a);
    Vector cd = normalize(d - c);
    Vector n = cross(ab,cd);

    return -n;
}

void ViewerEtudiant::init_terrain(const Image& im)
{
    //const float h = 10;
    m_terrain = Mesh(GL_TRIANGLE_STRIP);


    for(int i=0; i<im.width()-1; ++i) // Boucle sur les i
    {

        for(int j=0; j<im.height(); ++j) // Boucle sur les j
        {

            float w = im.width();
            float h = im.height();

            m_terrain.normal( terrainNormal(im, i+1, j) );
            m_terrain.texcoord(float(i+1)/w,float(j)/h);
            m_terrain.vertex( Point(i+1, 35.f*im(i+1, j).r, j) );

            m_terrain.normal( terrainNormal(im, i, j) );
            m_terrain.texcoord(float(i)/w,float(j)/h);
            m_terrain.vertex( Point(i, 35.f*im(i, j).r, j) );
        }

        m_terrain.restart_strip(); // Affichage en triangle_strip par bande
    }

}

void ViewerEtudiant::init_cubemap(){
        // Sommets                     0           1           2       3           4           5       6           7
        static float pt[8][3] = { {-1,-0.5,-1}, {1,-0.5,-1}, {1,-0.5,1}, {-1,-0.5,1}, {-1,0.5,-1}, {1,0.5,-1}, {1,0.5,1}, {-1,0.5,1} };

        // Faces                    0         1           2           3          4         5
        static int f[6][4] = { {0,1,2,3}, {5,4,7,6}, {2,1,5,6}, {0,3,7,4}, {3,2,6,7}, {1,0,4,5} };

        // Normales
        static float n[6][3] = { {0,1,0}, {0,-1,0}, {-1,0,0}, {1,0,0}, {0,0,-1}, {0,0,1} };

        m_cubemap = Mesh(GL_TRIANGLE_STRIP);
        m_cube.color( Color(1, 1, 1) );

        // Parcours des 6 faces

		//0
        m_cubemap.texcoord(0.5,0);
        m_cubemap.vertex( pt[ f[0][1] ][0], pt[ f[0][1] ][1], pt[ f[0][1] ][2] );

        m_cubemap.texcoord(0.25,0);
        m_cubemap.vertex( pt[ f[0][0] ][0], pt[ f[0][0] ][1], pt[ f[0][0] ][2] );

        m_cubemap.texcoord(0.5,0.33);
        m_cubemap.vertex( pt[ f[0][2] ][0], pt[ f[0][2] ][1], pt[ f[0][2] ][2] );

        m_cubemap.texcoord(0.25,0.33);
        m_cubemap.vertex(pt[ f[0][3] ][0], pt[ f[0][3] ][1], pt[ f[0][3] ][2] );


        m_cubemap.restart_strip();

        //1
        m_cubemap.texcoord(0.5,0.6);
        m_cubemap.vertex( pt[ f[1][1] ][0], pt[ f[1][1] ][1], pt[ f[1][1] ][2] );

        m_cubemap.texcoord(0.25,0.6);
        m_cubemap.vertex( pt[ f[1][0] ][0], pt[ f[1][0] ][1], pt[ f[1][0] ][2] );

        m_cubemap.texcoord(0.5,1);
        m_cubemap.vertex( pt[ f[1][2] ][0], pt[ f[1][2] ][1], pt[ f[1][2] ][2] );

        m_cubemap.texcoord(0.25,1);
        m_cubemap.vertex(pt[ f[1][3] ][0], pt[ f[1][3] ][1], pt[ f[1][3] ][2] );

        m_cubemap.restart_strip();

        //2
        m_cubemap.normal(n[2][0], n[2][1], n[2][2]);

        m_cubemap.texcoord(0.75,0.33);
        m_cubemap.vertex( pt[ f[2][1] ][0], pt[ f[2][1] ][1], pt[ f[2][1] ][2] );

        m_cubemap.texcoord(0.5,0.33);
        m_cubemap.vertex( pt[ f[2][0] ][0], pt[ f[2][0] ][1], pt[ f[2][0] ][2] );

        m_cubemap.texcoord(0.75,0.6);
        m_cubemap.vertex( pt[ f[2][2] ][0], pt[ f[2][2] ][1], pt[ f[2][2] ][2] );

        m_cubemap.texcoord(0.5,0.6);
        m_cubemap.vertex(pt[ f[2][3] ][0], pt[ f[2][3] ][1], pt[ f[2][3] ][2] );


        m_cubemap.restart_strip();

        //3
        m_cubemap.normal(n[3][0], n[3][1], n[3][2]);

        m_cubemap.texcoord(0.25,0.33);
        m_cubemap.vertex( pt[ f[3][1] ][0], pt[ f[3][1] ][1], pt[ f[3][1] ][2] );

        m_cubemap.texcoord(0,0.33);
        m_cubemap.vertex( pt[ f[3][0] ][0], pt[ f[3][0] ][1], pt[ f[3][0] ][2] );

        m_cubemap.texcoord(0.25,0.6);
        m_cubemap.vertex( pt[ f[3][2] ][0], pt[ f[3][2] ][1], pt[ f[3][2] ][2] );

        m_cubemap.texcoord(0,0.6);
        m_cubemap.vertex(pt[ f[3][3] ][0], pt[ f[3][3] ][1], pt[ f[3][3] ][2] );


        m_cubemap.restart_strip();

        //4
        m_cubemap.normal(n[4][0], n[4][1], n[4][2]);

        m_cubemap.texcoord(0.5,0.33);
        m_cubemap.vertex( pt[ f[4][1] ][0], pt[ f[4][1] ][1], pt[ f[4][1] ][2] );

        m_cubemap.texcoord(0.25,0.33);
        m_cubemap.vertex( pt[ f[4][0] ][0], pt[ f[4][0] ][1], pt[ f[4][0] ][2] );

        m_cubemap.texcoord(0.5,0.6);
        m_cubemap.vertex( pt[ f[4][2] ][0], pt[ f[4][2] ][1], pt[ f[4][2] ][2] );

        m_cubemap.texcoord(0.25,0.6);
        m_cubemap.vertex(pt[ f[4][3] ][0], pt[ f[4][3] ][1], pt[ f[4][3] ][2] );

        m_cubemap.restart_strip();

        //5
        m_cubemap.normal(n[5][0], n[5][1], n[5][2]);

        m_cubemap.texcoord(1,0.33);
        m_cubemap.vertex( pt[ f[5][1] ][0], pt[ f[5][1] ][1], pt[ f[5][1] ][2] );

        m_cubemap.texcoord(0.75,0.33);
        m_cubemap.vertex( pt[ f[5][0] ][0], pt[ f[5][0] ][1], pt[ f[5][0] ][2] );

        m_cubemap.texcoord(1,0.6);
        m_cubemap.vertex( pt[ f[5][2] ][0], pt[ f[5][2] ][1], pt[ f[5][2] ][2] );

        m_cubemap.texcoord(0.75,0.6);
        m_cubemap.vertex(pt[ f[5][3] ][0], pt[ f[5][3] ][1], pt[ f[5][3] ][2] );

        m_cubemap.restart_strip();
}

void ViewerEtudiant::init_wall1(){
    // Sommets                     0           1           2       3           4           5       6           7
    static float pt[8][3] = { {-1,-1,-1}, {1,-1,-1}, {1,-1,1}, {-1,-1,1}, {-1,1,-1}, {1,1,-1}, {1,1,1}, {-1,1,1} };

    // Faces                    0         1           2           3          4         5
    static int f[6][4] = { {0,1,2,3}, {5,4,7,6}, {2,1,5,6}, {0,3,7,4}, {3,2,6,7}, {1,0,4,5} };

    // Normales
    static float n[6][3] = { {0,-1,0}, {0,1,0}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1} };

    int i;

    m_wall1 = Mesh(GL_TRIANGLE_STRIP);
    m_wall1.color( Color(1, 1, 1) );

    // Parcours des 6 faces
    for (i=0;i<6;i++)
    {
        m_wall1.normal(n[i][0], n[i][1], n[i][2]);

        m_wall1.texcoord(0,0);
        m_wall1.vertex( pt[ f[i][0] ][0], pt[ f[i][0] ][1], pt[ f[i][0] ][2] );

        m_wall1.texcoord(1,0);
        m_wall1.vertex( pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );

        m_wall1.texcoord(1,1);
        m_wall1.vertex(pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );

        m_wall1.texcoord(0,1);
        m_wall1.vertex( pt[ f[i][2] ][0], pt[ f[i][2] ][1], pt[ f[i][2] ][2] );

        m_wall1.restart_strip();
    }

}

void ViewerEtudiant::init_wallquad(){
    m_wallquad = Mesh(GL_TRIANGLE_STRIP);
    m_wallquad.color( Color(1, 1, 1));

    m_wallquad.normal(  0, 0, 1 );

    m_wallquad.texcoord(0,0 );
    m_wallquad.vertex(-1, -1, 0 );

    m_wallquad.texcoord(1,0);
    m_wallquad.vertex(  1, -1, 0 );

    m_wallquad.texcoord(0,1);
    m_wallquad.vertex( -1, 1, 0 );

    m_wallquad.texcoord( 1,1);
    m_wallquad.vertex(  1,  1, 0 );
}

void ViewerEtudiant::init_fcube(){
    // Sommets                     0           1           2       3           4           5       6           7
    static float pt[8][3] = { {-1,-1,-1}, {1,-1,-1}, {1,-1,1}, {-1,-1,1}, {-1,1,-1}, {1,1,-1}, {1,1,1}, {-1,1,1} };

    // Faces                    0         1           2           3          4         5
    static int f[6][4] = { {0,1,2,3}, {5,4,7,6}, {2,1,5,6}, {0,3,7,4}, {3,2,6,7}, {1,0,4,5} };

    // Normales
    static float n[6][3] = { {0,-1,0}, {0,1,0}, {1,0,0}, {-1,0,0}, {0,0,1}, {0,0,-1} };

    int i;

    m_fcube = Mesh(GL_TRIANGLE_STRIP);
    m_fcube.color( Color(1, 1, 1) );

    // Parcours des 6 faces
    for (i=0;i<6;i++)
    {
        m_fcube.normal(n[i][0], n[i][1], n[i][2]);

        m_fcube.texcoord(0,0);
        m_fcube.vertex( pt[ f[i][0] ][0], pt[ f[i][0] ][1], pt[ f[i][0] ][2] );

        m_fcube.texcoord(1,0);
        m_fcube.vertex( pt[ f[i][1] ][0], pt[ f[i][1] ][1], pt[ f[i][1] ][2] );

        m_fcube.texcoord(0,1);
        m_fcube.vertex(pt[ f[i][3] ][0], pt[ f[i][3] ][1], pt[ f[i][3] ][2] );

        m_fcube.texcoord(1,1);
        m_fcube.vertex( pt[ f[i][2] ][0], pt[ f[i][2] ][1], pt[ f[i][2] ][2] );

        m_fcube.restart_strip();
    }

}

void ViewerEtudiant::init_fcercle(){

// Variation de l’angle de 0 à 2!
    const int div = 25;
    float alpha;
    float step = 2.0 * M_PI / (div);

// Choix primitive OpenGL
    m_fcercle = Mesh( GL_TRIANGLE_FAN );
    m_fcercle.normal( Vector(0,1,0) ); // Normale à la surface
    m_fcercle.vertex( Point(0,0,0) ); // Point du centre du fcercle

// Variation de l’angle de 0 à 2!
    for (int i=0; i<=div; ++i)
    {
        alpha = i * step;
        m_fcercle.normal( Vector(0,1,0) );
        m_fcercle.vertex( Point(cos(alpha), 0, sin(alpha)) );
    }
}

void ViewerEtudiant::init_fcylindre(){

// Variation de l’angle de 0 à 2!
    int i;
    const int div = 25;
    float alpha;
    float step = 2.0 * M_PI / (div);

// Choix primitive OpenGL
    m_fcylindre = Mesh(GL_TRIANGLE_STRIP);

    for (int i=0; i<=div; ++i)
    {
        // Variation de l’angle de 0 à 2!
        alpha = i * step;

        m_fcylindre.normal( Vector(cos(alpha), 0, sin(alpha)) );
        m_fcylindre.texcoord (i/div,0);
        m_fcylindre.vertex( Point(cos(alpha), -1, sin(alpha)) );
        m_fcylindre.normal( Vector(cos(alpha), 0, sin(alpha)) );
        m_fcylindre.texcoord (i/div,1);
        m_fcylindre.vertex( Point(cos(alpha), 1, sin(alpha)) );
    }
}

void ViewerEtudiant::draw_fcylindre(const Transform& T)
{
//Corps du cylindre
    gl.model( T*Translation(0,7,0));
    gl.draw( m_fcylindre );

// Disque du haut
    Transform Tch = T*Translation( 0, 8, 0) * RotationX(180);
    gl.model( Tch );
    gl.draw( m_fcercle);

// Disque du bas
    Transform Tcb = T*Translation( 0, 6, 0);
    gl.model( Tcb );
    gl.draw( m_fcercle);

}

void ViewerEtudiant::init_fcone()
{
// Variation de l’angle de 0 à 2!
    const int div = 25;
    float alpha;
    float step = 2.0 * M_PI / (div);

// Choix de la primitive OpenGL
    m_fcone = Mesh(GL_TRIANGLE_STRIP);

    for (int i=0; i<=div; ++i)
    {
        alpha = i * step; // Angle varie de 0 à 2!
        m_fcone.normal(
            Vector( cos(alpha)/sqrtf(2.f),
                    1.f/sqrtf(2.f),

                    sin(alpha)/sqrtf(2.f) ));
        m_fcone.texcoord(i/div,1);
        m_fcone.vertex( Point( cos(alpha), 0, sin(alpha) ));
        m_fcone.normal(
            Vector( cos(alpha)/sqrtf(2.f),
                    1.f/sqrtf(2.f),

                    sin(alpha)/sqrtf(2.f) ));
        m_fcone.texcoord(i/div,0);
        m_fcone.vertex( Point(0, 1, 0) );
    }

}

void ViewerEtudiant::init_fsphere()
{
// Variation des angles alpha et beta
    const int divBeta = 16;
    const int divAlpha = divBeta/2;
    int i,j;
    float beta, alpha, alpha2;

// Choix de la primitive OpenGL
    m_fsphere = Mesh(GL_TRIANGLE_STRIP);

// Variation des angles alpha et beta
    for(int i=0; i<divAlpha; ++i)
    {
        alpha = -0.5f * M_PI + float(i) * M_PI / divAlpha;
        alpha2 = -0.5f * M_PI + float(i+1) * M_PI / divAlpha;

        for(int j=0; j<=divBeta; ++j)
        {
            beta = float(j) * 2.f * M_PI / (divBeta);
            m_fsphere.normal( Vector(cos(alpha)*cos(beta), sin(alpha), cos(alpha)*sin(beta)) );
            m_fsphere.texcoord (beta/(2*M_PI),0.5+(alpha/M_PI));
            m_fsphere.vertex( Point(cos(alpha)*cos(beta), sin(alpha), cos(alpha)*sin(beta)) );
            m_fsphere.normal( Vector(cos(alpha2)*cos(beta), sin(alpha2), cos(alpha2)*sin(beta)) );
            m_fsphere.texcoord (beta/(2*M_PI),0.5+(alpha2/M_PI));
            m_fsphere.vertex( Point(cos(alpha2)*cos(beta), sin(alpha2), cos(alpha2)*sin(beta)) );
        } // boucle sur les j, angle beta, dessin des sommets d’un cercle

        m_fsphere.restart_strip(); // Demande un nouveau strip
    } // boucle sur les i, angle alpha, sphère = superposition de cercles
}

void ViewerEtudiant::draw_avion(const Transform& T)
{

//Le corps de l'avion
    gl.model(T*Scale(1,1,5)*Translation(0,1,0));
    gl.draw(m_fsphere);

//Aile gauche
    gl.model(T*Scale(2.5,0.15,0.5)*Translation(1,6.5,1.2));
    gl.draw(m_fcube);

//Aile droite
    gl.model(T*Scale(2.5,0.15,0.5)*Translation(-1,6.5,1.2));
    gl.draw(m_fcube);

//Réacteur gauche
    gl.model(T*Scale(0.4,0.4,1.7)*Translation(5,1.1,0.3));
    gl.draw(m_fsphere);

//Réacteur droite
    gl.model(T*Scale(0.4,0.4,1.7)*Translation(-5,1.1,0.3));
    gl.draw(m_fsphere);

//Gouveme de direction
    gl.model(T*Scale(0.1,2,0.5)*Translation(0.03,0.4,-7.5));
    gl.draw(m_fcone);

}

/////////////////////////////////////////////////////////////////////////
///3. Appel des fonctions Permettant l'affichage de l'objet en question//
/////////////////////////////////////////////////////////////////////////

int ViewerEtudiant::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    manageCameraLight();

    gl.camera(m_camera);

    /// Appel de la fonction render() de la class Viewer
    /// pour que vous voyez apparaitre un premier objet
    /// a supprimer ensuite
    Viewer::render();


    ///3.1 Appel des fonctions du type 'draw_votreObjet'

    ///Partie formes de bases
    //CUBE AVEC TEXTURE
    gl.texture(m_base_text);
    gl.model(Scale(2,2,2)*Translation(-40,16,8));
    gl.draw(m_fcube);
    //CYLINDRE (+Disques H&B) AVEC TEXTURE
    draw_fcylindre(1*Scale(2,2,2)*Translation(-40,10,5));
    //CONE AVEC TEXTURE
    gl.model(Scale(2,2,2)*Translation(-35,15,8));
    gl.draw(m_fcone);
    //SPHERE AVEC TEXTURE
    gl.texture(m_earth_text);
    gl.model(Scale(2,2,2)*Translation(-35,15.5,4));
    gl.draw(m_fsphere);

    ///Partie Objet_complexe (Avion)
    gl.texture(0);
    draw_avion(1*Scale(4,4,4)*Translation(-18,1.1,16));

    ///Partie TERRAIN
    gl.texture(m_terrain_texture);
    gl.model(Scale(0.4,0.4,0.4)*Translation(-256,-10,-256));
    //gl.draw(m_terrain);

    ///Partie CUBEMAP
    //26
    gl.texture(m_moon_texture);
    gl.model(Scale(100,100,100)*Translation(0,0.5,0));
    gl.draw(m_cubemap);

    ///Partie BATIMENTS
    //BATIMENT 1
    gl.texture(m_building1_texture);
    gl.model(Identity());
    //gl.model(Scale(10,10,10)*Translation(-12,0.2,-6.5));
    gl.model(Scale(15,15,15)*Translation(-9,0.2,0));
    gl.draw(m_building1);

    //BATIMENT DE TYPE ROCKET LAUNCH PAD
    gl.texture(0);
    gl.model(Identity());
    gl.model(Scale(4,4,4)*Translation(-13,1,-20)*RotationY(-135));
    gl.draw(m_launchpad);

    //Partie BUILDING.2
    gl.texture(m_building2_texture);
    gl.model(Identity());
    gl.model(Scale(10,10,10)*Translation(1,0.2,-8.3)*RotationY(-90));
    gl.draw(m_building2);

    ///Partie MURS
    gl.model(Identity());

    //MUR 1
    gl.model(Scale(1,15,100)*Translation(-100,1,0));
    gl.draw(m_wall1);

    //MUR 2
    gl.model(Scale(100,15,1)*Translation(0,1,-100));
    gl.draw(m_wall1);

    ///Partie peinture du mur
    gl.texture(m_wallquadtext);
    gl.model(Identity());

    //Boucle pour appliquer la peinture au mur 1
    for (float y=5.7; y >= -8.3 ; y = y -2){
        gl.model(Scale(15,15,15)*Translation(y,1,-6.58));
        gl.draw(m_wallquad);
    }

    //Boucle pour appliquer la peinture au mur 2
    for (float y=5.7; y >= -8.3 ; y = y -2){
        gl.model(Scale(15,15,15)*Translation(-6.58,1,y)*RotationY(90));
        gl.draw(m_wallquad);
    }

    ///Partie fusil
    gl.texture(0);
    gl.model(Identity());

    gl.model(Translation(-47,30,-81)*Scale(7,7,7));
    //gl.model(Translation(-27,-5,-95)*m_trocket*Scale(6,6,6)*Translation(0,5,0));
    gl.draw(m_rocket);

    ///Partie billboard
    //draw_foret(Scale(0.1,0.1,0.1)*Translation(-96,0,-96));
    //draw_foret(1);



    return 1;

}

//Fonction dans laquelle les mises a jours sont effectuees.
int ViewerEtudiant::update( const float time, const float delta )
{
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.

        // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.

    ///POUR FAIRE TOURNER L'OBJET SUR LUI MEME
    //float ts = (time/(1000));
    //int te = int(ts);
    float angle_rot = 3600/7.0;

    //m_tavion = RotationZ(angle_rot*te);

    ///POUR DEFINIR UNE TRAJECTOIRE
    float ts = (time/(2000));
    int te = int (ts);
    int ite = te % m_anim.nb_points();
    float poids = ts - te;

    Point p0= m_anim[ite];
    int ite_suiv = (te+1)% m_anim.nb_points();

    Point p1 = m_anim[ite_suiv];
    Vector pos = Vector(p0) + ( Vector(p1) - Vector(p0) ) * poids;

    //m_tavion = Translation(pos);

    //m_tavion = Translation(pos)*RotationZ(angle_rot*te);

    ///POUR ORIENTER LOBJET CORRECTEMENT
    //float ts = (time/(1000));
    //int te = int (ts);
    //int ite = te % m_anim.nb_points();
    //float poids = ts - te;

    //int ite_suiv = (te+1)% m_anim.nb_points();
    int ite_suiv_suiv = (ite_suiv + 1)% m_anim.nb_points();

    //Point p0 = m_anim[ite];
    //Point p1 = m_anim[ite_suiv];
    Point p2 = m_anim[ite_suiv_suiv];

    ///L'interpolation lineaire
    //Vector pos = Vector(p0) + ( Vector(p1) - Vector(p0) ) * poids;
    Vector pos_suiv = Vector(p1) + ( Vector(p2) - Vector(p1) ) * poids;

    Vector dir = normalize(pos_suiv);
    Vector y (0,1,0);

    Vector codir = cross (dir,y);
    //m_tavion = Transform (dir, y, codir, pos);


    m_tavion = Transform (dir, y, codir, pos);//*Translation(pos)*RotationZ(angle_rot*te);

    m_trocket = Transform (dir, y, codir, pos);


    return 0;
}


//Constructeur.
ViewerEtudiant::ViewerEtudiant() : Viewer()
{
}

//Programme principal.
int main( int argc, char **argv )
{
    ViewerEtudiant v;
    v.run();
    return 0;
}
