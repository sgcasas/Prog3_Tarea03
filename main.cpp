#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <sstream>
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
    int getVida() {
        return vida;
    };
    int getRecursos() {
        return recursos;
    };
    int getNivel() {
        return nivel;
    };
    vector<int> getPosicion() {
        return posicion;
    };


    void status() {
        cout<<"Nombre: "<<nombre<<endl;
        cout<<"Nivel: "<<nivel<<endl;
        cout<<"Posicion: "<<posicion[0]<<", "<<posicion[1]<<endl;
        cout<<"Vida: "<<vida<<endl;
        cout<<"Recursos: "<<recursos<<endl;
    }
};

using Command = std :: function < void ( const std :: list < std :: string >&) >;

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
struct Macrostep {
    string name;
    list<string> args;
};

class CommandCenter {
    map<string, Command> commands;
    list<string> history;
    Entity& entity;
    map<string, list<Macrostep>> macros;
public:
    CommandCenter(Entity& entity) : entity(entity) {}
    void registerCommand(const string& nombre, Command cmd) {
        commands[nombre] = cmd;
    }
    void execute(const string& nombre, const list<string>& args) {
        map<string, Command>::iterator it = commands.find(nombre);
        if (it != commands.end()) {
            std::ostringstream antes;
            antes << "Vida = " << entity.getVida() << " Nivel = " << entity.getNivel() <<  " Recursos = " << entity.getRecursos() << " Posicion = " << entity.getPosicion()[0] << "," << entity.getPosicion()[1] << endl;
            it->second(args);
            std ::ostringstream despues;
            despues << "Vida = " << entity.getVida() << " Nivel = " << entity.getNivel() <<  " Recursos = " << entity.getRecursos() << " Posicion = " << entity.getPosicion()[0] << "," << entity.getPosicion()[1] << endl;
            std::ostringstream registro;
            registro << nombre;
            for (const auto& arg : args) {
                registro << " " << arg;
            }
            registro << " |Antes: " << antes.str() << ""<< "\n          |Despues: " << despues.str() << "";
            history.push_back(registro.str());
        } else {
            cout<<"Error, el comando no existe"<<endl;
        }
    }
    void historial() {
        cout << "Historial:" << endl;
        std::list<std::string>::iterator it;
        for (it = history.begin(); it != history.end(); ++it) {
            cout << *it << endl;
        }
    }
    void removeCommand(const string& nombre) {
        auto it = commands.find(nombre);
        if (it != commands.end()) {
            commands.erase(it);
            cout << "Comando "<< nombre << " eliminado." << endl;
        }
        else cout << "Error, el comando no existe." << endl;
    }
    void registerMacro (const string & name, const list <pair<string , list <string>>>& steps){
        list<Macrostep> macro;
        for (const auto& step : steps) {
            macro.push_back({step.first, step.second});
        }
        macros[name] = macro;
    };
    void executeMacro ( const string & name) {
        auto it = macros.find(name);

        if (it == macros.end()) {
            cout << "Error, macro no existe" << endl;
            return;
        }

        list<Macrostep>::iterator stepIt;
        for (stepIt = it->second.begin(); stepIt != it->second.end(); ++stepIt) {
            auto cmdIt = commands.find(stepIt->name);
            if (cmdIt == commands.end()) {
                cout<<"Error, el comando "<<stepIt->name<<" no existe"<<endl;
                return;
            }
            execute(stepIt->name, stepIt->args);
        }
    } ;
};


    int main() {
        Entity jugador("Player", 100, 10, 1);
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
        // Comandos validos
        center.execute("move", {"2", "3"});
        center.execute("damage", {"20"});
        center.execute("heal", {"10"});
        //Comandos invalidos
        center.execute("move", {"a,", "b"});
        center.execute("damage", {});
        center.execute("inexistente", {"10"});

        // macrocomando 1
        center.registerMacro("macro1", {
            {"move", {"1", "1"}},
            {"damage", {"5"}},
            {"heal", {"3"}}
        });
        // macrocomando 2
        center.registerMacro("macro2", {
            {"move", {"-2", "4"}},
            {"damage", {"15"}}
        });
        // macrocomando 3 (con error)
        center.registerMacro("macroError", {
            {"move", {"1", "1"}},
            {"comandoInvalido", {"10"}},
            {"heal", {"5"}}
        });
        // Ejecutar macros
        center.executeMacro("macro1");
        center.executeMacro("macro2");
        center.executeMacro("macroError");
        // Estado Final
        jugador.status();
        // Mostrar Historial
        center.historial();
    }