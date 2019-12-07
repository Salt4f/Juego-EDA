#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Salt4f


struct PLAYER_NAME : public Player {

    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    //---------- Definiciones -----------//

    static const int INFINIT = 10000;

    struct Unidad
    {
        int id;                 //ID de la unidad
        Unit unit;              //Unidad
        bool damage;            //Cierto si la unidad ha recibido daños
        stack<Dir> path;        //Camino a seguir por la unidad

        Unidad(int id) {
            this->id = id;
            damage = false;
            //path = nullptr;
        }

        void comprueba_atacada() {
            if (unit.type == Dwarf) {
                damage = unit.health < 100;
            }
            else damage = unit.health < 50;
        }

        Dir next() {
            if (path.empty()) return rand();
            Dir next = path.top();
            path.pop();
            return next;
        }

    };
    

    /*struct Grupo
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
    };*/

    //Información usada en el BFS
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

        bool operator<(const Coord& b) {
            return (this->d < b.d);
        }

        bool operator>(const Coord& b) {
            return (this->d > b.d);
        }

    };

    //Crea una struct Coord con la casilla inicializada
    Coord nueva_coord_bfs(int x, int y, int d) {
        Coord new_coord(x, y, d);
        new_coord.cell = cell(x, y);
        return new_coord;
    }

    Coord nueva_coord(int x, int y) {
        Coord new_coord(x, y, 1);
        recalcular_distancia(new_coord);
        return new_coord;
    }

    //Struct creada para el priority_queue del Dijkstra
    struct comp
    {
        bool operator()(const Coord& a, const Coord& b)
        {
            return a.d > b.d;
        }
    };

    //--------- Funciones útiles --------//

    bool caminable(const Coord& pt) {
        return pt.cell.type == Cave
            or pt.cell.type == Outside;
    }

    bool caminable2(const Coord& pt) {
        return (pt.cell.type == Cave
            or pt.cell.type == Outside
            or pt.cell.type == Rock)
            and unit(pt.cell.id).player != my_id;
    }

    void recalcular_casilla(Coord& pt) {
        pt.cell = cell(pt.x, pt.y);
    }

    void recalcular_distancia(Coord& pt) {
        recalcular_casilla(pt);
        pt.d = 1;
        Pos pos(pt.x, pt.y);
        if (en_peligro(pos))
            pt.d = INFINIT;
        else if (pt.cell.type == Abyss or pt.cell.type == Granite) {
            pt.d = INFINIT;
        }
        else if (pt.cell.type == Rock) {
            pt.d += pt.cell.turns;
        }
        else if ((pt.cell.id != -1) and (unit(pt.cell.id).player != my_id)) {
            pt.d = INFINIT;
        }
    }

    void recalcular_distancia2(Coord& pt) {
        recalcular_casilla(pt);
        pt.d += 1;
        Pos pos(pt.x, pt.y);
        if (pt.cell.type == Rock) {
            pt.d += pt.cell.turns;
        }
        else if ((pt.cell.id != -1) and (unit(pt.cell.id).player != my_id)) {
            pt.d = INFINIT;
        }
    }

    static int distancia(const Pos& p1, const Pos& p2) {
        return max(abs(p1.i - p2.i), abs(p1.j - p2.j));
    }

    static int distancia_cuadrada(const Pos& p1, const Pos& p2) {
        return abs(p1.i - p2.i) + abs(p1.j - p2.j);
    }

    bool en_peligro(const Pos &pt) {
        return false;//distancia_cuadrada(boss.pos, pt) < distancia_seguridad;
    }

    static Dir calcular_direccion(const Pos& origen, const Pos& destino) {
        int i = destino.i - origen.i;
        int j = destino.j - origen.j;

        if (i == 1) {
            if (j == 1) return BR;
            else if (j == -1) return LB;
            else return Bottom;
        }
        else if (i == -1) {
            if (j == 1) return RT;
            else if (j == -1) return TL;
            else return Top;
        }
        else { //i == 0
            if (j == 1) return Right;
            else if (j == -1) return Left;
            else return None;
        }
    }

    static Dir rand() {
        Random_generator g;
        return Dir(g.random(Bottom, LB));
    }

    void crear_unidades(vector<Unidad>& v ) {
        v = vector<Unidad>();
        for (int id : my_dwarves) {
            Unidad u(id);
            u.unit = unit(id);
            u.comprueba_atacada();
            camino_a_lo_mas_cerca(u);
            v.push_back(u);
        }
        for (int id : my_wizards) {
            Unidad u(id);
            u.unit = unit(id);
            u.comprueba_atacada();
            camino_al_danado(u);
            v.push_back(u);
        }
    }

    void actualizar_unidad(Unidad& u) {
        u.unit = unit(u.id);
        u.comprueba_atacada();
        if (u.unit.type == Dwarf) camino_a_lo_mas_cerca(u);
        else camino_al_danado(u);
    }

    void camino_a_tesoro(Unidad& u) {
        vector<vector<int> > d,p;
        //cout << "Unidad " << u.id << ":" << endl;
        dijkstra(u.unit.pos.i, u.unit.pos.j, d, p, u.path, 50);
    }

    void camino_a_lo_mas_cerca(Unidad& u) {
        u.path = bfs_v3(u.id, 30);
    }

    void camino_al_danado(Unidad& u) {
        u.path = bfs_v2(u.id, 30);
    }

    //------------ Variables ------------//

    int my_id;                  //Mi ID
    int boss_id;                //Balrog ID
    Unit boss;                  //Balrog unit info
    vector<int> my_dwarves;     //Vector con las ID de mis enanos
    vector<int> my_wizards;     //Vector con los ID de mis magos
    vector<Unidad> my_units;    //Vector con mis unidades
    int num_dwarves;            //Cantidad de enanos
    int num_wizards;            //Cantidad de magos
    bool dead;                  //Cierto si hemos perdido unidades
    bool newones;               //Cierto si hemos conseguido nuevas unidades
    int distancia_seguridad;    //Distancia de seguridad respecto al Balrog

    //------------ Funciones ------------//

    void init() {

        my_id = me();
        boss_id = balrog_id();
        num_dwarves = 20;
        num_wizards = 5;
        distancia_seguridad = 1;

        update(true);

    }

    void update(bool init) {

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

        if (dead or newones or init) {
            crear_unidades(my_units);
        }
        else {
            for (Unidad u : my_units) {
                actualizar_unidad(u);
            }
        }

        //----- Actualizar unidades enemigas -----//

        boss = unit(boss_id);

        //---------------------------------------

        for (Unidad u : my_units) {
            command(u.id, u.next());
        }

    }

    vector<pair<int, Pos> > bfs (int id, int max_d) {
        Pos origen = unit(id).pos;

        int n = 59;
        int m = 59;

        vector<vector<bool> > visited(n+1, vector<bool>(m+1, false));
        queue<Coord> cola;
        vector<pair<int,Pos> > ids;

        cola.push(nueva_coord_bfs(origen.i, origen.j, 0));
        visited[origen.i][origen.j] = true;

        while (not cola.empty())
        {   
            Coord pt = cola.front();
            cola.pop();

            if (pt.d > 0 and pt.cell.id != -1 and unit(pt.cell.id).player == my_id) {
                ids.push_back(make_pair(pt.cell.id, Pos(pt.x, pt.y)));
            }

            if (pt.d < max_d) {

                Coord nextpos = nueva_coord_bfs(pt.x - 1, pt.y, pt.d + 1);
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

    stack<Dir> bfs_v2 (int id, int max_d) {
        Pos origen = unit(id).pos;

        int n = 59;
        int m = 59;

        vector<vector<bool> > visited(n+1, vector<bool>(m+1, false));
        vector<vector<int> > p = vector<vector<int> >(n+1, vector<int>(m+1,-1));
        queue<Coord> cola;

        stack<Dir> path;

        cola.push(nueva_coord_bfs(origen.i, origen.j, 0));
        visited[origen.i][origen.j] = true;

        while (not cola.empty())
        {   
            Coord pt = cola.front();
            cola.pop();

            if (pt.d > 0 and pt.cell.id != -1 and unit(pt.cell.id).player == my_id) {
                
                int x = pt.x;
                int y = pt.y;

                while (p[x][y] != -1) {
                    //cout << x << " " << y << endl;
                    Pos final(x,y);
                    Pos origen = final + Dir(p[x][y]);
                    x = origen.i;
                    y = origen.j;
                    path.push(calcular_direccion(origen, final));
                    //cout << calcular_direccion(origen, final);
                }

                return path;

            }

            if (pt.d < max_d) {

                Coord nextpos = nueva_coord_bfs(pt.x - 1, pt.y, pt.d + 1);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Top
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = Bottom;
                }

                nextpos.set_pos(pt.x, pt.y + 1);
                recalcular_casilla(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Right
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = Left;
                }

                nextpos.set_pos(pt.x + 1, pt.y);
                recalcular_casilla(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Bottom
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = Top;
                }

                nextpos.set_pos(pt.x, pt.y - 1);
                recalcular_casilla(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Left
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = Right;
                }
            
            }
        }

        return path;

    }

    stack<Dir> bfs_v3 (int id, int max_d) {
        Pos origen = unit(id).pos;

        int n = 59;
        int m = 59;

        vector<vector<bool> > visited(n+1, vector<bool>(m+1, false));
        vector<vector<int> > p = vector<vector<int> >(n+1, vector<int>(m+1,-1));
        queue<Coord> cola;

        stack<Dir> path;

        cola.push(nueva_coord_bfs(origen.i, origen.j, 0));
        visited[origen.i][origen.j] = true;

        while (not cola.empty())
        {   
            Coord pt = cola.front();
            cola.pop();

            if (pt.d > 0 and (pt.cell.treasure or (pt.cell.id != -1 and unit(pt.cell.id).player != my_id))) {
                
                int x = pt.x;
                int y = pt.y;

                while (p[x][y] != -1) {
                    //cout << x << " " << y << endl;
                    Pos final(x,y);
                    Pos origen = final + Dir(p[x][y]);
                    x = origen.i;
                    y = origen.j;
                    path.push(calcular_direccion(origen, final));
                    //cout << calcular_direccion(origen, final);
                }

                return path;

            }

            if (pt.d < max_d) {

                Coord nextpos = nueva_coord_bfs(pt.x - 1, pt.y, pt.d + 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable2(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Top
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = Bottom;
                }

                nextpos.set_pos(pt.x - 1, pt.y + 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable2(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Top-Right
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = LB;
                }

                nextpos.set_pos(pt.x, pt.y + 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable2(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Right
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = Left;
                }

                nextpos.set_pos(pt.x + 1, pt.y + 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable2(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Right-Bottom
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = TL;
                }

                nextpos.set_pos(pt.x + 1, pt.y);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable2(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Bottom
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = Top;
                }
                
                nextpos.set_pos(pt.x + 1, pt.y - 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable2(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Bottom-Left
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = RT;
                }

                nextpos.set_pos(pt.x, pt.y - 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable2(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Left
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = Right;
                }

                nextpos.set_pos(pt.x - 1, pt.y - 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and caminable2(nextpos) and not visited[nextpos.x][nextpos.y]) {
                    cola.push(nextpos); // Top-Left
                    visited[nextpos.x][nextpos.y] = true;
                    p[nextpos.x][nextpos.y] = BR;
                }
            
            }
        }

        return path;

    }

    void dijkstra(int i, int j, vector<vector<int> >& d, vector<vector<int> >& p, stack<Dir>& camino, int max_d) {

        int n = 60; //n = numero de filas
        int m = 60; //m = numero de columnas

        d = vector<vector<int> >(n, vector<int> (m, INFINIT)); //d = vector de distancias
        d[i][j] = 0; //el origen tiene distancia 0
        p = vector<vector<int> >(n, vector<int>(m,-1)); //p = vector de nodos previos
        vector<vector<bool> > visited(n, vector<bool>(m,false));
        priority_queue<Coord, vector<Coord>, comp > cola;
        cola.push(nueva_coord(i, j));

        int distancia = INFINIT;
        camino = stack<Dir>();

        while (not cola.empty()) {
            Coord pt = cola.top(); //pt = nodo actual
            cola.pop();

            if (d[pt.x][pt.y] < max_d) {

                if (d[pt.x][pt.y] > 0 and pt.cell.type == Cave and pt.cell.treasure) {
                    //cout << "Tesoro a " << d[pt.x][pt.y] << endl;
                    if (distancia > d[pt.x][pt.y]) {
                        camino = stack<Dir>();
                        distancia = d[pt.x][pt.y];
                        int x = pt.x;
                        int y = pt.y;

                        while (p[x][y] != -1) {
                            //cout << x << " " << y << endl;
                            Pos final(x,y);
                            Pos origen = final + Dir(p[x][y]);
                            x = origen.i;
                            y = origen.j;
                            camino.push(calcular_direccion(origen, final));
                            //cout << calcular_direccion(origen, final);
                        }
                        //cout << endl;
                    }
                }

                //-------------------------------------------------------------------------------

                Coord nextpos = nueva_coord(pt.x - 1, pt.y);
                if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                    if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + pt.d)) {
                        d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + pt.d;
                        p[nextpos.x][nextpos.y] = Bottom;
                        cola.push(nextpos); // Top
                        visited[nextpos.x][nextpos.y] = true;
                    }
                }

                nextpos.set_pos(pt.x - 1, pt.y + 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                    if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + pt.d)) {
                        d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + pt.d;
                        p[nextpos.x][nextpos.y] = LB;
                        cola.push(nextpos); // Top-Right
                        visited[nextpos.x][nextpos.y] = true;
                    }
                }

                nextpos.set_pos(pt.x, pt.y + 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                    if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + pt.d)) {
                        d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + pt.d;
                        p[nextpos.x][nextpos.y] = Left;
                        cola.push(nextpos); // Right
                        visited[nextpos.x][nextpos.y] = true;
                    }
                }

                nextpos.set_pos(pt.x - 1, pt.y + 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                    if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + pt.d)) {
                        d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + pt.d;
                        p[nextpos.x][nextpos.y] = TL;
                        cola.push(nextpos); // Bottom-Right
                        visited[nextpos.x][nextpos.y] = true;
                    }
                }

                nextpos.set_pos(pt.x + 1, pt.y);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                    if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + pt.d)) {
                        d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + pt.d;
                        p[nextpos.x][nextpos.y] = Top;
                        cola.push(nextpos); // Bottom
                        visited[nextpos.x][nextpos.y] = true;
                    }
                }

                nextpos.set_pos(pt.x + 1, pt.y - 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                    if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + pt.d)) {
                        d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + pt.d;
                        p[nextpos.x][nextpos.y] = RT;
                        cola.push(nextpos); // Bottom-Left 
                        visited[nextpos.x][nextpos.y] = true;
                    }
                }

                nextpos.set_pos(pt.x, pt.y - 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                    if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + pt.d)) {
                        d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + pt.d;
                        p[nextpos.x][nextpos.y] = Right;
                        cola.push(nextpos); // Left
                        visited[nextpos.x][nextpos.y] = true;
                    }
                }
                
                nextpos.set_pos(pt.x - 1, pt.y - 1);
                recalcular_distancia(nextpos);
                if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                    if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + pt.d)) {
                        d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + pt.d;
                        p[nextpos.x][nextpos.y] = BR;
                        cola.push(nextpos); // Top-Left
                        visited[nextpos.x][nextpos.y] = true;
                    }
                }

            }

            /*for (int i = 0; i < int(G[u].size()); ++i) { //Para todos los nodos vecinos
                int v = G[u][i].second; //v = nodo vecino
                int c = G[u][i].first; //c = distancia del nodo
                if (d[v] > d[u] + c) {  //si la distancia al nodo vecino es más grande que
                                        //la distancia al nodo actual más la verdadera distancia al nodo vecino
                    d[v] = d[u] + c;
                    p[v] = u;           //previo al nodo v es u
                    Q.push(ArcP(d[v], v));
                }
            }*/

        }
    }

    void dijkstra_v2(int i, int j, vector<vector<int> >& d, vector<vector<int> >& p, stack<Dir>& camino, int max_d) {

        int n = 60; //n = numero de filas
        int m = 60; //m = numero de columnas

        d = vector<vector<int> >(n, vector<int> (m, INFINIT)); //d = vector de distancias
        d[i][j] = 0; //el origen tiene distancia 0
        p = vector<vector<int> >(n, vector<int>(m,-1)); //p = vector de nodos previos
        vector<vector<bool> > visited(n, vector<bool>(m,false));
        priority_queue<Coord, vector<Coord>, comp > cola;
        cola.push(nueva_coord(i, j));

        bool encontrado = false;

        int distancia = INFINIT;
        camino = stack<Dir>();

        while (not cola.empty()) {
            Coord pt = cola.top(); //pt = nodo actual
            cola.pop();

            encontrado = false;

            if (d[pt.x][pt.y] < max_d) {

                if (d[pt.x][pt.y] > 0 and pt.cell.type == Cave and (pt.cell.treasure or (pt.cell.id != -1 and unit(pt.cell.id).player != my_id))) {
                    //cout << "Tesoro a " << d[pt.x][pt.y] << endl;
                    if (distancia > d[pt.x][pt.y]) {
                        camino = stack<Dir>();
                        distancia = d[pt.x][pt.y];
                        int x = pt.x;
                        int y = pt.y;

                        while (p[x][y] != -1) {
                            //cout << x << " " << y << endl;
                            Pos final(x,y);
                            Pos origen = final + Dir(p[x][y]);
                            x = origen.i;
                            y = origen.j;
                            camino.push(calcular_direccion(origen, final));
                            //cout << calcular_direccion(origen, final);
                        }
                        //cout << endl;

                        encontrado = true;
                    }
                }

                if (not encontrado) {

                    Coord nextpos = nueva_coord(pt.x - 1, pt.y);
                    if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                        if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + nextpos.d)) {
                            d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + nextpos.d;
                            p[nextpos.x][nextpos.y] = Bottom;
                            cola.push(nextpos); // Top
                            visited[nextpos.x][nextpos.y] = true;
                        }
                    }

                    nextpos.set_pos(pt.x - 1, pt.y + 1);
                    recalcular_distancia(nextpos);
                    if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                        if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + nextpos.d)) {
                            d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + nextpos.d;
                            p[nextpos.x][nextpos.y] = LB;
                            cola.push(nextpos); // Top-Right
                            visited[nextpos.x][nextpos.y] = true;
                        }
                    }

                    nextpos.set_pos(pt.x, pt.y + 1);
                    recalcular_distancia(nextpos);
                    if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                        if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + nextpos.d)) {
                            d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + nextpos.d;
                            p[nextpos.x][nextpos.y] = Left;
                            cola.push(nextpos); // Right
                            visited[nextpos.x][nextpos.y] = true;
                        }
                    }

                    nextpos.set_pos(pt.x - 1, pt.y + 1);
                    recalcular_distancia(nextpos);
                    if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                        if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + nextpos.d)) {
                            d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + nextpos.d;
                            p[nextpos.x][nextpos.y] = TL;
                            cola.push(nextpos); // Bottom-Right
                            visited[nextpos.x][nextpos.y] = true;
                        }
                    }

                    nextpos.set_pos(pt.x + 1, pt.y);
                    recalcular_distancia(nextpos);
                    if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                        if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + nextpos.d)) {
                            d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + nextpos.d;
                            p[nextpos.x][nextpos.y] = Top;
                            cola.push(nextpos); // Bottom
                            visited[nextpos.x][nextpos.y] = true;
                        }
                    }

                    nextpos.set_pos(pt.x + 1, pt.y - 1);
                    recalcular_distancia(nextpos);
                    if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                        if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + nextpos.d)) {
                            d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + nextpos.d;
                            p[nextpos.x][nextpos.y] = RT;
                            cola.push(nextpos); // Bottom-Left 
                            visited[nextpos.x][nextpos.y] = true;
                        }
                    }

                    nextpos.set_pos(pt.x, pt.y - 1);
                    recalcular_distancia(nextpos);
                    if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                        if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + nextpos.d)) {
                            d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + nextpos.d;
                            p[nextpos.x][nextpos.y] = Right;
                            cola.push(nextpos); // Left
                            visited[nextpos.x][nextpos.y] = true;
                        }
                    }
                    
                    nextpos.set_pos(pt.x - 1, pt.y - 1);
                    recalcular_distancia(nextpos);
                    if (pos_ok(nextpos.x, nextpos.y) and not visited[nextpos.x][nextpos.y]) {
                        if (d[nextpos.x][nextpos.y] > (d[pt.x][pt.y] + nextpos.d)) {
                            d[nextpos.x][nextpos.y] = d[pt.x][pt.y] + nextpos.d;
                            p[nextpos.x][nextpos.y] = BR;
                            cola.push(nextpos); // Top-Left
                            visited[nextpos.x][nextpos.y] = true;
                        }
                    }
                }

            }

        }

        /*if (unit(my_dwarves[0]).pos == Pos(i, j)) {
            stack<Dir> copia = camino;

            cout << "Unidad " << my_dwarves[0] << " en la posición " << i << " " << j << ": ";

            for (int x = 0; x < copia.size(); ++x) {
                cout << copia.top() << " ";
                copia.pop();
            }
            cout << endl;
        }*/
    }

    /**
     * Play method, invoked once per each round.
     */
    virtual void play () {
        //cout << round() << endl;
        if (round() == 1) init();
        else {
            update(false);
        }
    }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
