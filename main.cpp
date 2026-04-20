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
        cout<<"Posicion: "<<posicion[0]<<" "<<posicion[1];
        cout<<"Vida: "<<vida<<endl;
        cout<<"Recursos: "<<recursos<<endl;
    }
};

using Command = function<void(const list<string>&)>;

class CommanCenter {
    map<string, Command> commands;
    list<string> history;
    Entity& entity;
public:
    CommanCenter(Entity& entity) : entity(entity) {}
    void registerCommand(const string& nombre, Command cmd) {
        commands[nombre] = cmd;
    }
    void execute(const string& nombre, const list<string>& args) {
        map<string, Command>::iterator it = commands.find(nombre);
        if (it != commands.end()) {
            it->second(args);
            history.push_back(nombre);
        }
    }
};

int main() {
    return 0;
}