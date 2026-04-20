#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <map>
using namespace std;

class Entity {
    string nombre;
    vector<int> posicion;
    int vida, nivel, recursos, vidaInicial;
public:
    Entity(string nombre, int vida, int recursos, int nivel) : nombre(nombre), posicion({0,0}), vida(vida), recursos(recursos), nivel(nivel), vidaInicial(vida) {}
    void movexy(int x, int y) {
        posicion[0] += x;
        posicion[1] += y;
    }
    void heal(int n) { vida+=n; }
    void damage(int n) { vida-=n; }
    void reset() {
        vida = vidaInicial;
        recursos = 0;
        nivel = 1;
        posicion = {0,0};
    }
    void status() {
        cout<<"Nombre: "<<nombre<<endl;
        cout<<"Nivel: "<<nivel<<endl;
        cout<<"Posicion: "<<posicion[0]<<" "<<posicion[1]<<endl;
        cout<<"Vida: "<<vida<<endl;
        cout<<"Recursos: "<<recursos<<endl;
    }
};

using Command = function<void(const list<string>&)>;

void commandDamage(Entity& entity, const list<string>& args) {
    if (args.empty()) {
        cout<<"Error, el comando damage requiere un argumento"<<endl;
        return;
    }
    try {
        entity.damage(stoi(args.front()));
    } catch (invalid_argument&) {
        cout<<"Error, el comando damage requiere un argumento numerico"<<endl;
    }
}

class HealFunctor {
    Entity& entity;
    int ejecuciones;
public:
    HealFunctor(Entity& entity) : entity(entity), ejecuciones(0) {}
    void operator() (const list<string>& args) {
        if (args.empty()) {
            cout<<"Error, el comando heal requiere un argumento"<<endl;
            return;
        }
        try {
            entity.heal(stoi(args.front()));
            ejecuciones++;
        } catch (invalid_argument&) {
            cout<<"Error, el comando heal requiere un argumento numerico"<<endl;
        }
    }
};

class CommandCenter {
    map<string, Command> commands;
    list<string> history;
    Entity& entity;
public:
    CommandCenter(Entity& entity) : entity(entity) {}
    void registerCommand(const string& nombre, Command cmd) {
        commands[nombre] = cmd;
    }
    void execute(const string& nombre, const list<string>& args) {
        map<string, Command>::iterator it = commands.find(nombre);
        if (it != commands.end()) {
            it->second(args);
            history.push_back(nombre);
        } else {
            cout<<"Error, el comando no existe"<<endl;
        }
    }
};

int main() {
    Entity jugador("Jugador", 100, 10, 1);
    CommandCenter center(jugador);
    HealFunctor healFunctor(jugador);
    center.registerCommand("damage", [&jugador](const list<string>& args) {
        commandDamage(jugador, args);
    });
    center.registerCommand("move", [&jugador](const list<string>& args) {
        if (args.size() < 2) {
            cout<<"Error, el comando requiere dos argumentos"<<endl;
            return;
        }
        auto p = args.begin();
        try {
            int x = stoi(*p);
            p++;
            int y = stoi(*p);
            jugador.movexy(x, y);

        } catch (invalid_argument&) {
            cout<<"Error, el comando requiere dos argumentos numericos"<<endl;
        }
    });
    center.registerCommand("heal", healFunctor);
    return 0;
}