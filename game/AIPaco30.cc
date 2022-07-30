#include "Player.hh"

#define PLAYER_NAME Paco30


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  
    int numero_de_barricades = barricades(me()).size();
    vector<Pos> Booked_cells;
  
    const vector<pair<int,int>> dirs = { {0,1}, {0,-1}, {1,0}, {-1,0} };
    
     //Donada la posició actual, retorna true si està dins del tauell i no hi ha un edifici.
    bool bon_vei(const pair<int,int>& u) {
        if (pos_ok(u.first,u.second)) {
            Cell c = cell(u.first,u.second);
        
            return c.type != Building and c.id == -1 and ((c.resistance != -1 and c.b_owner == me()) or (c.resistance == -1));
        }
        return false;
    }
    
    //no comprova si hi ha un enemic.
    bool bon_vei_escape(const pair<int,int>& u) {
        if (pos_ok(u.first,u.second)) {
            Cell c = cell(u.first,u.second);
        
            return c.type != Building and (c.id == -1 or citizen(c.id).player != me()) and ((c.resistance != -1 and c.b_owner == me()) or (c.resistance == -1));
        }
        return false;
    }
    
    //construir_barricada es vàlid sii es retorna true, altrament no
    bool bon_vei_barricada(const pair<int,int>& u, bool& construir_barricada) {
        if (pos_ok(u.first,u.second)) {
            Cell c = cell(u.first,u.second);
            construir_barricada = c.resistance == -1;
        
            return c.type != Building and c.id == -1 and ((c.resistance != -1 and  c.b_owner == me()) or construir_barricada) and c.weapon == NoWeapon and c.bonus == NoBonus;
        }
        return false;
    }
    
    bool bon_vei_enemics(const pair<int,int>& u, WeaponType weapon) {
        if (pos_ok(u.first,u.second)) {
            Cell c = cell(u.first,u.second);
            Pos pos_enemic;
        
            return c.type != Building and (c.id == -1 or citizen(c.id).player != me()) and (c.resistance == -1 or c.b_owner == me() or (weapon_strength_demolish(weapon) >= c.resistance and not enemic_al_voltant1(Pos(u.first,u.second),pos_enemic)));
        
            //((c.resistance == -1) or (c.resistance != -1 and c.b_owner == me()) or (c.resistance != -1 and c.b_owner != me() and weapon_strength_demolish(weapon) >= c.resistence))
        }
        return false;
    }
    
    //Retorna true si al voltant (una casella de marge) del la pos passada hi ha un enemic. Si retorna true, a pos_enemic està la pos del enemic.
    bool enemic_al_voltant1(Pos p, Pos& pos_enemic) {
        p.i = p.i - 1;
        p.j = p.j - 1;
        
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (i != 1 or j != 1) {
                    if (pos_ok(p) and cell(p).id != -1 and citizen(cell(p).id).player != me()) {
                        pos_enemic = p;
                        return true;
                    }
                }
                ++p.j;
            }
            ++p.i;
        }
        return false;
    }    
    
    //retorna la distància que hi ha entre la posicio p_ini i el enemic mes aprop (p_fi) si una lluita amb el citizen passat i aquest seria probable de perdre-la.
    //La p_ini ha de ser una posició vàlida
    
    int enemic_mes_proper(const Citizen& w, Pos p_ini, Pos& p_fi) {
        /*Citizen contrincari = citizen(cell(p_ini).id); 
        
        double probabilitat = double(weapon_strength_attack(w.weapon)) / double(weapon_strength_attack(contrincari.weapon) + weapon_strength_attack(w.weapon));
         
         if (citizen(cell(p_ini).id).player != me() and probabilitat < 0.7) {
             return 0;
         }*/
  
        vector<vector<int>> dist(board_rows(),vector<int>(board_cols(),-1));

        dist[p_ini.i][p_ini.j] = 0;
        queue<pair<pair<int,int>,pair<int,int>>> Q; //primer moviment, posicio
        Q.push({{0,0},{p_ini.i,p_ini.j}});

        bool first_time = true;
        
        while (not Q.empty()) {
            pair<pair<int,int>,pair<int,int>> u = Q.front();
            Q.pop();
            for (auto& d : dirs) {
                pair<int,int> pos(u.second.first + d.first, u.second.second + d.second); //posicio + direccio;
                pair<int,int> mov;
                
                if (first_time) mov = {d.first,d.second};
                else mov = {u.first.first,u.first.second};
                
                pair<pair<int,int>,pair<int,int>> v = {mov,pos};
                
                if (bon_vei_escape(v.second) and dist[v.second.first][v.second.second] == -1) {

                    Q.push(v);
                    dist[v.second.first][v.second.second] = dist[u.second.first][u.second.second] + 1;
                    
                    if (cell(v.second.first,v.second.second).id != -1) { 
                        Citizen contrincari = citizen(cell(v.second.first,v.second.second).id);
                        
                        double probabilitat = double(weapon_strength_attack(w.weapon)) / double(weapon_strength_attack(contrincari.weapon) + weapon_strength_attack(w.weapon));
                        
                        if (probabilitat < 0.7) {
                            //dir = {v.first.first,v.first.second};
                            return dist[v.second.first][v.second.second];
                        }
                    }
                }
            }
            first_time = false;
        }
        return -1;
    }
        
        
        
    //Hauria de detectar murs
    //Retorna true si al voltant (dos caselles de marge) del la pos passada hi ha un enemic.            
    bool enemic_al_voltant2(Pos p) { 
        p.i = p.i - 2;
        p.j = p.j - 2;
        
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                if (i != 2 or j != 2) {
                    if (pos_ok(p) and cell(p).id != -1 and citizen(cell(p).id).player != me()) return true;
                }
                ++p.j;
            }
            ++p.i;
        }
        return false;
        
    }
    
    //BFS: Retorna la distancia a la arma mes propera segons la que tinc (-1 si no n'hi puc arribar) i s'ompla la posició on hi ha la arma (si dist != -1) i el primer moviment que ha de fer per anar camí cap a ella si distancia es diferent de -1 i de 0.
    int buscar_armes(const Citizen& w,/* Pos& pos_arma, */pair<int,int>& dir) {
         if (cell(w.pos).weapon == Bazooka or (w.weapon == Hammer and cell(w.pos).weapon == Gun)) {
           // pos_arma = w.pos;
             return 0;
         }
  
        vector<vector<int>> dist(board_rows(),vector<int>(board_cols(),-1));

        dist[w.pos.i][w.pos.j] = 0;
        queue<pair<pair<int,int>,pair<int,int>>> Q; //primer moviment, posicio
        Q.push({{0,0},{w.pos.i,w.pos.j}});

        bool first_time = true;
        
        while (not Q.empty()) {
            pair<pair<int,int>,pair<int,int>> u = Q.front();
            Q.pop();
            for (auto& d : dirs) {
                pair<int,int> pos(u.second.first + d.first, u.second.second + d.second); //posicio + direccio;
                pair<int,int> mov;
                
                if (first_time) mov = {d.first,d.second};
                else mov = {u.first.first,u.first.second};
                
                pair<pair<int,int>,pair<int,int>> v = {mov,pos};
                
                if (bon_vei(v.second) and dist[v.second.first][v.second.second] == -1) {

                    Q.push(v);
                    dist[v.second.first][v.second.second] = dist[u.second.first][u.second.second] + 1;
                    
                    if (cell(v.second.first,v.second.second).weapon == Bazooka or (w.weapon == Hammer and cell(v.second.first,v.second.second).weapon == Gun)) { //aqui posar un and i afegir una funció que comprovi si hi ha algun de l'altre equip aprop;
                       // pos_arma = Pos(v.second.first,v.second.second);
                        dir = {v.first.first,v.first.second};
                        return dist[v.second.first][v.second.second];
                    }
                }
            }
            first_time = false;
        }
        return -1;
    }
  
  
    int buscar_menjar(const Citizen& w, pair<int,int>& dir) {
         if ((w.life < warrior_ini_life() and cell(w.pos).bonus == Food)) {
             return 0;
         }
  
        vector<vector<int>> dist(board_rows(),vector<int>(board_cols(),-1));

        dist[w.pos.i][w.pos.j] = 0;
        queue<pair<pair<int,int>,pair<int,int>>> Q; //primer moviment, posicio
        Q.push({{0,0},{w.pos.i,w.pos.j}});

        bool first_time = true;
        
        while (not Q.empty()) {
            pair<pair<int,int>,pair<int,int>> u = Q.front();
            Q.pop();
            for (auto& d : dirs) {
                pair<int,int> pos(u.second.first + d.first, u.second.second + d.second); //posicio + direccio;
                pair<int,int> mov;
                
                if (first_time) mov = {d.first,d.second};
                else mov = {u.first.first,u.first.second};
                
                pair<pair<int,int>,pair<int,int>> v = {mov,pos};
                
                if (bon_vei(v.second) and dist[v.second.first][v.second.second] == -1) {

                    Q.push(v);
                    dist[v.second.first][v.second.second] = dist[u.second.first][u.second.second] + 1;
                    
                    if (cell(v.second.first,v.second.second).bonus == Food) { 
                        dir = {v.first.first,v.first.second};
                        return dist[v.second.first][v.second.second];
                    }
                }
            }
            first_time = false;
        }
        return -1;
    }
  
    int buscar_monedes(const Citizen& w, pair<int,int>& dir) {
         if (cell(w.pos).bonus == Money) {
             return 0;
         }
  
        vector<vector<int>> dist(board_rows(),vector<int>(board_cols(),-1));

        dist[w.pos.i][w.pos.j] = 0;
        queue<pair<pair<int,int>,pair<int,int>>> Q; //primer moviment, posicio
        Q.push({{0,0},{w.pos.i,w.pos.j}});

        bool first_time = true;
        
        while (not Q.empty()) {
            pair<pair<int,int>,pair<int,int>> u = Q.front();
            Q.pop();
            for (auto& d : dirs) {
                pair<int,int> pos(u.second.first + d.first, u.second.second + d.second); //posicio + direccio;
                pair<int,int> mov;
                
                if (first_time) mov = {d.first,d.second};
                else mov = {u.first.first,u.first.second};
                
                pair<pair<int,int>,pair<int,int>> v = {mov,pos};
                
                if (bon_vei(v.second) and dist[v.second.first][v.second.second] == -1) {

                    Q.push(v);
                    dist[v.second.first][v.second.second] = dist[u.second.first][u.second.second] + 1;
                    
                    if (cell(v.second.first,v.second.second).bonus == Money) { 
                        dir = {v.first.first,v.first.second};
                        return dist[v.second.first][v.second.second];
                    }
                }
            }
            first_time = false;
        }
        return -1;
    }
  
    Dir pair2Dir(pair<int,int> pdir) {
        if (pdir.first == 0 and pdir.second == 1) return Right;
        else if (pdir.first == 0 and pdir.second == -1) return Left;
        else if (pdir.first == 1 and pdir.second == 0) return Down;
        else return Up; 
    }
    
    //Retorna true si no s'ha construit/reforçat la barricada, fals altrament.
    bool construir_o_reforcar_barricada(const Citizen& b) {
        /*
        cerr << "el constructor " << b.id << " ubicat a " << b.pos << " vol construir una barricada " << endl;
        cerr << "el nombre maxim de barricades és " << max_num_barricades() << endl;
        cerr << "el nombre de barricades que tinc és " << numero_de_barricades << endl;
        cerr << "el size del vector de pair<int,int> dirs és de " << dirs.size() << endl;
        */
        for (int d = 0; d < dirs.size(); ++d) {
            pair<int,int> pos = {b.pos.i + dirs[d].first, b.pos.j + dirs[d].second};
            
           // cerr << "iteracio for " << d << endl;
            
            bool construir_barricada;
            
            if (bon_vei_barricada(pos,construir_barricada) and cell(Pos(pos.first,pos.second)).resistance != barricade_max_resistance() and ((construir_barricada and max_num_barricades() != numero_de_barricades) or not construir_barricada)) {
                
                build(b.id, pair2Dir(dirs[d]));
                //cerr << "el constructor construirà la barricada a la direccio " << pair2Dir(dirs[d]) << endl;
                if (construir_barricada) ++numero_de_barricades;
                return true;
            }
        }
      //  cerr << "el constructor no construirà la barricada i passarà a buscar monedes" << endl;
        return false;
    }
    
    int buscar_enemics(const Citizen& w, pair<int,int>& dir) {
  
        vector<vector<int>> dist(board_rows(),vector<int>(board_cols(),-1));

        dist[w.pos.i][w.pos.j] = 0;
        queue<pair<pair<int,int>,pair<int,int>>> Q; //primer moviment, posicio
        Q.push({{0,0},{w.pos.i,w.pos.j}});

        bool first_time = true;
        
        while (not Q.empty()) {
            pair<pair<int,int>,pair<int,int>> u = Q.front();
            Q.pop();
            for (auto& d : dirs) {
                pair<int,int> pos(u.second.first + d.first, u.second.second + d.second); //posicio + direccio;
                pair<int,int> mov;
                
                if (first_time) mov = {d.first,d.second};
                else mov = {u.first.first,u.first.second};
                
                pair<pair<int,int>,pair<int,int>> v = {mov,pos};
                
                if (bon_vei_enemics(v.second,w.weapon) and dist[v.second.first][v.second.second] == -1) {

                    Q.push(v);
                    dist[v.second.first][v.second.second] = dist[u.second.first][u.second.second] + 1;
                    
                    Cell c = cell(Pos(pos.first,pos.second));
                    
                    if (c.id != -1) {
                        Citizen contrincari = citizen(c.id);
                            
                        double probabilitat = double(weapon_strength_attack(w.weapon)) / double(weapon_strength_attack(contrincari.weapon) + weapon_strength_attack(w.weapon));
                        
                        if (c.resistance != -1 and c.b_owner != me()) {
                            if (probabilitat >= 0.9 and not enemic_al_voltant2(Pos(pos.first,pos.second))) {
                                 dir = {v.first.first,v.first.second};
                                return dist[v.second.first][v.second.second];
                            }
                        }
                        else {
                            
                            if (probabilitat >= 0.7) {
                                dir = {v.first.first,v.first.second};
                                return dist[v.second.first][v.second.second];
                            }
                                
                          // cerr << std::setprecision(5) << "probabilitat: " << double(weapon_strength_attack(w.weapon) / (weapon_strength_attack(contrincari.weapon) + weapon_strength_attack(w.weapon))) << endl;
                        }
                    }
                }
            }
            first_time = false;
        }
        return -1;
    }
  
    //Retorna true si la barricada que hi ha a la cela passada està reservada per un altre builder/warrior
    bool reserved_cell(Pos pos) {
        for (auto i : Booked_cells) {
            if (i == pos) return true;
        }
        return false;
    }
    
    void Unbook_cells(Pos pos) {
        for (auto& i : Booked_cells) {
            if (i == pos) {
                i = Pos(-1,-1);
                return;
            }
        }
    }
  
    //No es te en compte si el citizen ja està en una barricada
    int buscar_barricades(const Citizen& w, pair<int,int>& dir) {
        /* if (cell(w.pos).b_owner == me() and not reserved_cell(Pos(pos.first,pos.second)) {
             return 0;
         }*/
  
        vector<vector<int>> dist(board_rows(),vector<int>(board_cols(),-1));

        dist[w.pos.i][w.pos.j] = 0;
        queue<pair<pair<int,int>,pair<int,int>>> Q; //primer moviment, posicio
        Q.push({{0,0},{w.pos.i,w.pos.j}});

        bool first_time = true;
        
        while (not Q.empty()) {
            pair<pair<int,int>,pair<int,int>> u = Q.front();
            Q.pop();
            for (auto& d : dirs) {
                pair<int,int> pos(u.second.first + d.first, u.second.second + d.second); //posicio + direccio;
                pair<int,int> mov;
                
                if (first_time) mov = {d.first,d.second};
                else mov = {u.first.first,u.first.second};
                
                pair<pair<int,int>,pair<int,int>> v = {mov,pos};
                
                if (bon_vei(v.second) and dist[v.second.first][v.second.second] == -1) {

                    Q.push(v);
                    dist[v.second.first][v.second.second] = dist[u.second.first][u.second.second] + 1;
                        
                    Cell c = cell(Pos(pos.first,pos.second));
                    
                    if (c.b_owner == me() and not reserved_cell(Pos(pos.first,pos.second))) { 
                        Booked_cells.push_back(Pos(pos.first,pos.second));
                        dir = {v.first.first,v.first.second};
                        return dist[v.second.first][v.second.second];
                    }
                }
            }
            first_time = false;
        }
        return -1;
    }
    
    //Returns true if the citizen has escaped for surviving
    bool escape(const Citizen& w) {
        vector<pair<int,int>> bones; //vector de dirs
        int dir_bona = 0;
        bool reservar_barricada = false;
        Pos pos_barricada;
        
        bool escape = false;
        int counter = 0;
        for (auto& d : dirs) {
                Pos posicio(w.pos.i + d.first, w.pos.j + d.second); //posicio + direccio;
                
                if (bon_vei_escape({posicio.i, posicio.j})) {
                    Cell c = cell(posicio);
                    
                    if (c.id != -1) {
                        
                        Citizen contrincari = citizen(c.id);
                    
                        double probabilitat = double(weapon_strength_attack(w.weapon)) / double(weapon_strength_attack(contrincari.weapon) + weapon_strength_attack(w.weapon));
                        
                        if (probabilitat < 0.7) { 
                        escape = true;
                        }
                    }
                    else {
                        if (c.b_owner == me()) {
                            dir_bona = counter;
                            reservar_barricada = true;
                            pos_barricada = posicio;
                        }
                        bones.push_back(d);
                        ++counter;
                    }
                }
        }
        if (escape and bones.size() != 0) {
            move(w.id,pair2Dir(bones[dir_bona]));
            if (is_night() and reservar_barricada) Booked_cells.push_back(pos_barricada);
            return true;
        }
        return false;
    }
  
  /**
   * Play method, invoked once per each round.
   */
  virtual void play () { 
    vector<int> builder = builders(me());
    vector<int> warrior = warriors(me());
    pair<int,int> pdir;
    
    if (is_day()) {
        for (int w : warrior) {
            //Pos pos_arma(-1,-1);
            //int distancia = -1;
            
            
            if ((citizen(w).weapon != Bazooka and buscar_armes(citizen(w),pdir) != -1) 
                or (citizen(w).life < warrior_ini_life() and buscar_menjar(citizen(w),pdir) != -1) 
                or (buscar_monedes(citizen(w),pdir) != -1)) {
                
                Dir dir = pair2Dir(pdir);
                move(w,dir);
            }
            else escape(citizen(w));
            //else extra, mirar que no hi hagi ningú del equip contrari aprop o apropar-me a una barricada meva
            
            //extra: lo idel és que per exemple, si va a buscar menjar, si troba monedes pel camí, que les agafi.
            
            
                /*
                cerr << "el guerrer amb id " << w << " està a la posició " << citizen(w).pos << endl;
            
                cerr << "1a posicio en la que abançarà el guerrer de dia: " << pdir.first << ' ' << pdir.second << endl;
            
                //cerr << "posició de la arma que vol anar a buscar: " << pos_arma << endl;
            
                
            
                cerr << "La direcció és :" << dir << endl;
        
            
            distancia = buscar_monedes(citizen(w),pdir);
            cerr << "distància entre la posició del guerrer i les monedes: " << distancia << endl;
            */

            
        }
        for (int b : builder) {
            //extra: que si detecta que un enemic va a agafar una arma que està aprop d'ell, que la destrueixi posant-s'hi a sobre.
            
            if (citizen(b).life < builder_ini_life() and buscar_menjar(citizen(b),pdir) != -1) {
                
                Dir dir = pair2Dir(pdir);
                move(b,dir);
            }
            else if (not construir_o_reforcar_barricada(citizen(b)) and buscar_monedes(citizen(b),pdir) != -1) {
                Dir dir = pair2Dir(pdir);
                move(b,dir);
            }
            else escape(citizen(b));
        }
    }
    else {
           
        for (int w : warrior) {
            
            if (not escape(citizen(w))) {
            
                if (buscar_enemics(citizen(w),pdir) != -1) {
                    /*Pos pos_enemic;
                    if (Pos(pdir.first,pdir.second) and  enemic_al_voltant1(,pos_enemic,Pos(pdir.first,pdir.second)) and )*/
                    Dir dir = pair2Dir(pdir);
                    move(w,dir);
                
                } else if ((citizen(w).weapon != Bazooka and buscar_armes(citizen(w),pdir) != -1)   or (citizen(w).life < warrior_ini_life() and buscar_menjar(citizen(w),pdir) != -1) or (buscar_monedes(citizen(w),pdir) != -1)) {
                    Dir dir = pair2Dir(pdir);
                    move(w,dir);
                
              /*  cerr << "el guerrer amb id " << w << " està a la posició " << citizen(w).pos << endl;
            
                cerr << "1a posicio en la que abançarà el guerrer de nit: " << pdir.first << ' ' << pdir.second << endl;
            
                cerr << "posició del enemic que vol anar a buscar: " << Pos(pdir.first,pdir.second) << endl;
  
                cerr << "La direcció és :" << dir << endl;
       
            
            
                //cerr << "distància entre la posició del guerrer i l'enemic: " << distancia << endl;*/
                }
            }
            
        }
        for (int b : builder) {
            Pos pos_b = citizen(b).pos;
           // int distancia;
            
            
            if (cell(pos_b).b_owner == me()) {
                    if (not reserved_cell(pos_b)) Booked_cells.push_back(pos_b);
                    if (not enemic_al_voltant2(pos_b) and (buscar_enemics(citizen(b),pdir) != -1 or (citizen(b).life < builder_ini_life() and buscar_menjar(citizen(b),pdir) != -1) or (buscar_monedes(citizen(b),pdir) != -1))) {
                        Unbook_cells(pos_b);
                        Dir dir = pair2Dir(pdir);
                        move(b,dir);
                    } 
                }
            else {
                if (not escape(citizen(b))) {
                    
                    if (not enemic_al_voltant2(pos_b) and (buscar_enemics(citizen(b),pdir) != -1 or (citizen(b).life < builder_ini_life() and buscar_menjar(citizen(b),pdir) != -1) or (buscar_monedes(citizen(b),pdir) != -1))) {
                    
             //       cerr << "distancia=  " << distancia << endl;
                        Dir dir = pair2Dir(pdir);
                        move(b,dir);
                    }
                    else if (buscar_barricades(citizen(b),pdir) != -1) {
                        Dir dir = pair2Dir(pdir);
                        move(b,dir);
                    }

                }
            }
            
            
        }
    numero_de_barricades = 0;
    Booked_cells = vector<Pos>();
    }
     
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
