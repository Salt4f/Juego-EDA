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

    struct GrupoAsalto
    {
        int middle; //ID del enano del medio
        int left;   //ID del enano de la izquieda
        int right;  //ID del enano de la derecha
    };

    //Vector usado en el BFS
    struct Coord
    {
        int x;  //Coordenada x
        int y;  //Coordenada y
        int d;  //Distancia (respecto al origen del BFS)

        Coord(int x, int y, int d) : x(x), y(y), d(d) {}
        
    };

    /*vector<vector<int> > distancias(const vector<pair<int,Pos> >& units) {
        vector<vector<int> > vec(units.size(), vector<int>(units.size(),0));
        for (int i = 0; i < units.size(); ++i) {
            for (int j = 0; j < i; ++j) {
                vec[i][j] = abs(units[i].second.i - units[j].second.i)
                +  abs(units[i].second.j - units[j].second.j);
            }
        }
        return vec;
    }*/

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
    //vector<pair<int,Pos> > my_dwarves_pos;

    //------------ Funciones ------------//

    void init() {

        my_id = me();
        boss_id = balrog_id();
        num_dwarves = 20;
        num_wizards = 5;

        update();

        /*for (int i : my_dwarves) {
            Pos pos = unit(i).pos;
            my_dwarves_pos.push_back(make_pair(i, pos));
        }

        ofstream file;
        file.open("matriz.txt");

        auto dist = distancias(my_dwarves_pos);
        for (int i = 0; i < dist.size(); ++i) {
            for (int j = 0; j < dist.size(); ++j) {
                file << dist[i][j] << "\t";
            }
            file << endl;
        }

        file.close();*/

    }

    void update() {

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

    bool bfs (int id, int& dist) {
        Pos origen = unit(id).pos;
        dist = -1;

        int n = 59;
        int m = 59;

        vector<vector<bool> > visited(n+1, vector<bool>(m+1, false));
        queue<Coord> cola;

        cola.push(Coord(origen.i, origen.j, 0));

        //cout << "Punto inicial OK" << endl;

        while (not cola.empty())
        {   
            Coord pt = cola.front();
            cola.pop();

            int i = pt.x;
            int j = pt.y;
            int d = pt.d;
                
            if (mapa[i][j] == 't') {
                dist = d;
                return true;
            }
            visited[i][j] = true;

            if (i > 0 and mapa[i-1][j] != 'X' and not visited[i-1][j]) {
                cola.push(Coord(i-1, j, d+1)); // up
            }
            if (j < m and mapa[i][j+1] != 'X' and not visited[i][j+1]) {
                cola.push(Coord(i, j+1, d+1)); // right
            }
            if (i < n and mapa[i+1][j] != 'X' and not visited[i+1][j]) {
                cola.push(Coord(i+1, j, d+1)); // down  
            }
            if (j > 0 and mapa[i][j-1] != 'X' and not visited[i][j-1]) {
                cola.push(Coord(i, j-1, d+1)); // left
            }
            
        }

        return false;

    }

    /**
     * Play method, invoked once per each round.
     */
    virtual void play () {
        if (round() == 1) init();
        else {
            update();
            for (int i : my_dwarves) {
                command(i, Top);
            }
        }
    }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
