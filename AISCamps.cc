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
        int x;      //Coordenada x
        int y;      //Coordenada y
        int d;      //Distancia (respecto al origen del BFS)
        Cell cell;  //Casilla en la posición (x, y)

        Coord(int x, int y, int d) : x(x), y(y), d(d)
        {}

        void set_pos(int x, int y) {
            this->x = x;
            this->y = y;
        }

    };

    //Crea una struct Coord con la casilla inicializada
    Coord nueva_coord(int x, int y, int d) {
        Coord new_coord(x, y, d);
        new_coord.cell = cell(x, y);
        return new_coord;
    }

    //--------- Funciones útiles --------//

    bool caminable(Coord& pt) {
        return pt.cell.type == Cave
            or pt.cell.type == Outside;
    }

    void recalcular_casilla(Coord& pt) {
        pt.cell = cell(pt.x, pt.y);
    }

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

        /*ofstream file;
        file.open("matriz.txt");

        for (int i : my_dwarves) {
            file << i << ": " << bfs(i, 5).size() << endl;
        }

        file.close();

        for (int i : my_dwarves) {
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

    vector<pair<int, Pos> > bfs (int id, int max_d) {
        Pos origen = unit(id).pos;

        int n = 59;
        int m = 59;

        vector<vector<bool> > visited(n+1, vector<bool>(m+1, false));
        queue<Coord> cola;
        vector<pair<int,Pos> > ids;

        cola.push(nueva_coord(origen.i, origen.j, 0));
        visited[origen.i][origen.j] = true;

        while (not cola.empty())
        {   
            Coord pt = cola.front();
            cola.pop();

            if (pt.d > 0 and pt.cell.id != -1 and unit(pt.cell.id).player == my_id) {
                ids.push_back(make_pair(pt.cell.id, Pos(pt.x, pt.y)));
            }

            if (pt.d < max_d) {

                Coord nextpos = nueva_coord(pt.x - 1, pt.y, pt.d + 1);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Top
                    visited[nextpos.x][nextpos.y] = true;
                }

                nextpos.set_pos(pt.x - 1, pt.y + 1);
                recalcular_casilla(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Top-Right
                    visited[nextpos.x][nextpos.y] = true;
                }

                nextpos.set_pos(pt.x, pt.y + 1);
                recalcular_casilla(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Right
                    visited[nextpos.x][nextpos.y] = true;
                }

                nextpos.set_pos(pt.x - 1, pt.y + 1);
                recalcular_casilla(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Bottom-Right
                    visited[nextpos.x][nextpos.y] = true;
                }

                nextpos.set_pos(pt.x + 1, pt.y);
                recalcular_casilla(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Bottom
                    visited[nextpos.x][nextpos.y] = true;
                }

                nextpos.set_pos(pt.x + 1, pt.y - 1);
                recalcular_casilla(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Bottom-Left 
                    visited[nextpos.x][nextpos.y] = true;
                }

                nextpos.set_pos(pt.x, pt.y - 1);
                recalcular_casilla(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Left
                    visited[nextpos.x][nextpos.y] = true;
                }
                
                nextpos.set_pos(pt.x - 1, pt.y - 1);
                recalcular_casilla(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Top-Left
                    visited[nextpos.x][nextpos.y] = true;
                }
            
            }
        }

        return ids;

    }

    /**
     * Play method, invoked once per each round.
     */
    virtual void play () {
        if (round() == 1) init();
        else {
            update();
            for (int i : my_dwarves) {
                if (pos_ok(unit(i).pos + Top) and (cell(unit(i).pos + Top).type == Cave or cell(unit(i).pos + Top).type == Rock))
                    command(i, Top);
                else
                    command(i, Dir(random(0, LB)));
            }
        }
    }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
