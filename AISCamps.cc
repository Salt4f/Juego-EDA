#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME SCamps


struct PLAYER_NAME : public Player {

    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    //---------- Definiciones -----------//

    struct Grupo
    {
        int wizard; //ID del mago del grupo
        int left;   //ID del enano de la izquierda
        int top;    //ID del enano de arriba
        int right;  //ID del enano de la derecha
    };
    

    //------------ Variables ------------//

    int my_id;                  //Mi ID
    int boss_id;                //Balrog ID
    Unit boss;                  //Balrog unit info
    vector<int> my_dwarves;     //Vector con los ID de mis enanos
    vector<int> my_wizards;     //Vector con los ID de mis magos
    int num_dwarves;            //Cantidad de enanos
    int num_wizards;            //Cantidad de magos
    bool dead;                  //Cierto si hemos perdido unidades
    bool newones;               //Cierto si hemos conseguido nuevas unidades

    //------------ Funciones ------------//

    void init() {
        num_dwarves = 20;
        num_wizards = 5;

        update();
    }

    void update() {

        my_id = me();
        boss_id = balrog_id();

        //----- Actualizar mis vectores -----//

        my_dwarves = dwarves(my_id);
        my_wizards = wizards(my_id);
        int size_d = my_dwarves.size();
        int size_w = my_wizards.size();

        if (size_d > num_dwarves) newones = true;
        else if (size_d < num_dwarves) dead = true;
        else newones = dead = false;

        if (size_w > num_wizards) newones = true;
        else if (size_w < num_wizards) dead = true;

        num_dwarves = size_d;
        num_wizards = size_w;

        //----- Actualizar unidades enemigas -----//

        boss = unit(boss_id);
    }

    /**
     * Play method, invoked once per each round.
     */
    virtual void play () {
        if (nb_rounds() == 1) init();
        else {
            update();
        }
    }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
