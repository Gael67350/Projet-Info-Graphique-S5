# Projet-Info-Graphique-S5
Projet d'informatique graphique S5 - Scène 3D animée avec OpenGL

![Demo](../assets/campfire_cg_project_img_faded.png?raw=true)

Le projet est fait pour se construire intégralement a l'aide de CMake, tous les fichiers de shader et les ressources sont copiés automatiquement dans le dossier bin.

Par ailleurs, toutes les librairies utilisées sont celles incluses à l'origine dans le squelette.

### Systèmes Linux

Il a été nécessaire de réadapter les chemins des librairies linux pour pouvoir compiler. En cas de problème dans la compilation pour linux, il faut remplacer,
dans le fichier CMakeLists.txt, les lignes 56 à 63.

Contenant actuellement :

    SET(GLEW_INCLUDE_PATH       /usr/include  CACHE PATH "Path to GLEW include location")
    SET(SDL2_INCLUDE_PATH       /usr/include  CACHE PATH "Path to SDL2 library location")
    SET(SDL2_LIBRARY_PATH       /usr/lib/x86_64-linux-gnu)
    SET(GLEW_LIBRARY_PATH       /usr/lib/x86_64-linux-gnu)
    SET(GL_LIBRARY_PATH         /usr/lib/x86_64-linux-gnu)
    SET(GL_INCLUDE_PATH         /usr/include  CACHE PATH "Path to GL   include location")
    SET(SDL2_IMAGE_LIBRARY_PATH /usr/lib/x86_64-linux-gnu)
    SET(SDL2_IMAGE_INCLUDE_PATH /usr/include/ CACHE PATH "Path to SDL2 IMAGE include location")

par le contenu suivant :

    SET(GLEW_INCLUDE_PATH       /usr/include  CACHE PATH "Path to GLEW include location")
    SET(SDL2_INCLUDE_PATH       /usr/include  CACHE PATH "Path to SDL2 library location")
    SET(SDL2_LIBRARY_PATH       /usr/lib      CACHE PATH "Path to SDL2 include location")
    SET(GLEW_LIBRARY_PATH       /usr/lib      CACHE PATH "Path to GLEW library location")
    SET(GL_LIBRARY_PATH         /usr/lib      CACHE PATH "Path to GL   library location")
    SET(GL_INCLUDE_PATH         /usr/include  CACHE PATH "Path to GL   include location")
    SET(SDL2_IMAGE_LIBRARY_PATH /usr/lib      CACHE PATH "Path to SDL2 IMAGE library location")
    SET(SDL2_IMAGE_INCLUDE_PATH /usr/include/ CACHE PATH "Path to SDL2 IMAGE include location")

### Systèmes Windows

Sous Windows, les shaders (dans le dossier ./Shaders) peuvent ne pas compiler correctement. Pour résoudre ce problème, veillez à encoder ces fichiers au format Unix avant toute nouvelle exécution.

### Licence

Ce projet est sous licence GNU General Public License v3.0. Pour plus d'informations, consultez le fichier LICENSE.

This project is licensed under GNU General Public License v3.0. For further informations take a look 
at the LICENSE file.
