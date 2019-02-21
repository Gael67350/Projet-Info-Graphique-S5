# Projet-Info-Graphique-S5
Projet d'informatique graphique S5 - Scène 3D animée avec OpenGL

Le projet est fait pour se construire intégralement a l'aide du cmake, tous les fichiers de shader et les ressources sont copiés automatiquement dans le fichier bin
l'exécutable.

Par ailleurs toutes les librairies utilisées étaient celles inclues de base dans le squelette .

Il a été nécéssaire de réadapter les chemins des librairies linux pour pouvoir compiler. En cas de problème dans la compilation pour linux il faut remplacer
dans le CMAKE les lignes 56 a 63.

contenant acutellement :

    SET(GLEW_INCLUDE_PATH       /usr/include  CACHE PATH "Path to GLEW include location")
    SET(SDL2_INCLUDE_PATH       /usr/include  CACHE PATH "Path to SDL2 library location")
    SET(SDL2_LIBRARY_PATH       /usr/lib/x86_64-linux-gnu)
    SET(GLEW_LIBRARY_PATH       /usr/lib/x86_64-linux-gnu)
    SET(GL_LIBRARY_PATH         /usr/lib/x86_64-linux-gnu)
    SET(GL_INCLUDE_PATH         /usr/include  CACHE PATH "Path to GL   include location")
    SET(SDL2_IMAGE_LIBRARY_PATH /usr/lib/x86_64-linux-gnu)
    SET(SDL2_IMAGE_INCLUDE_PATH /usr/include/ CACHE PATH "Path to SDL2 IMAGE include location")

  par le contenu par défaut du CMake :

    SET(GLEW_INCLUDE_PATH       /usr/include  CACHE PATH "Path to GLEW include location")
    SET(SDL2_INCLUDE_PATH       /usr/include  CACHE PATH "Path to SDL2 library location")
    SET(SDL2_LIBRARY_PATH       /usr/lib      CACHE PATH "Path to SDL2 include location")
    SET(GLEW_LIBRARY_PATH       /usr/lib      CACHE PATH "Path to GLEW library location")
    SET(GL_LIBRARY_PATH         /usr/lib      CACHE PATH "Path to GL   library location")
    SET(GL_INCLUDE_PATH         /usr/include  CACHE PATH "Path to GL   include location")
    SET(SDL2_IMAGE_LIBRARY_PATH /usr/lib      CACHE PATH "Path to SDL2 IMAGE library location")
    SET(SDL2_IMAGE_INCLUDE_PATH /usr/include/ CACHE PATH "Path to SDL2 IMAGE include location")
