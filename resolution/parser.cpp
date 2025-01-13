#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <filesystem>
#include <cctype>
#include <chrono>

using namespace std;

void make_domain() {
    string std_domain = R"((define
    (domain domain-lightsup)
    (:requirements :strips :typing :negative-preconditions)
    (:types
        line col
    )
    
    (:predicates
        (is-on ?l - line ?c - col)
        (is-visible ?l1 - line ?c1 - col ?l2 - line ?c2 - col)
        (is-numbered-adjacent ?l1 - line ?c1 - col ?l2 - line ?c2 - col)
        (counter-at ?l - line ?c - col ?cnt - counter)
    )

    (:constants i0 i1 i2 i3 i4 - counter)
    
    (:action bulb
    
        :parameters(?l - line ?c - col)
        :precondition (not (is-on ?l ?c))
        
        :effect (and

            (is-on ?l ?c)
            
            (forall (?c2 - col) (and
                ; Ativando as células da mesma linha
                (when (or 
                        (is-visible ?l ?c ?l ?c2)
                        (is-visible ?l ?c2 ?l ?c)
                    )
                        (is-on ?l ?c2)
                )

                (when (is-numbered-adjacent ?l ?c ?l ?c2) (and
                    (when (counter-at ?l ?c2 i0) (and (not (counter-at ?l ?c2 i0)) (counter-at ?l ?c2 i1)))
                    (when (counter-at ?l ?c2 i1) (and (not (counter-at ?l ?c2 i1)) (counter-at ?l ?c2 i2)))
                    (when (counter-at ?l ?c2 i2) (and (not (counter-at ?l ?c2 i2)) (counter-at ?l ?c2 i3)))
                    (when (counter-at ?l ?c2 i3) (and (not (counter-at ?l ?c2 i3)) (counter-at ?l ?c2 i4)))
                ))

            ))
            
            (forall (?l2 - line) (and
                ; Ativando as células da mesma coluna
                (when (or 
                        (is-visible ?l ?c ?l2 ?c)
                        (is-visible ?l2 ?c ?l ?c)
                    )
                        (is-on ?l2 ?c)
                )

                (when (is-numbered-adjacent ?l ?c ?l2 ?c) (and
                    (when (counter-at ?l2 ?c i0) (and (not (counter-at ?l2 ?c i0)) (counter-at ?l2 ?c i1)))
                    (when (counter-at ?l2 ?c i1) (and (not (counter-at ?l2 ?c i1)) (counter-at ?l2 ?c i2)))
                    (when (counter-at ?l2 ?c i2) (and (not (counter-at ?l2 ?c i2)) (counter-at ?l2 ?c i3)))
                    (when (counter-at ?l2 ?c i3) (and (not (counter-at ?l2 ?c i3)) (counter-at ?l2 ?c i4)))
                ))
            ))
        )
    )
))";

    ofstream domain("domain.pddl");
    domain << std_domain;
    domain.close();

}

vector<vector<char>> make_board() {

    vector<vector<char>> board; // Vetor de vetores para o board
    string line;

    // Ler line por line do input
    while (getline(cin, line)) {

        vector<char> line_vetor;

        // Processar cada caracter da line
        for (char c : line) {
            line_vetor.push_back(c);
        }

        board.push_back(line_vetor);
    }

    return board;

}

void make_problem (vector<vector<char>> board){


    // ---------------- STRING PADRÃO ----------------------------

    string std_problem = R"((define
        (problem problem-lightsup)
        (:domain domain-lightsup)
        (:objects )
        (:init )
        (:goal (and ))))";


    string OBJ = ":objects ";
    string INIT = ":init ";
    string GOAL = "(and ";

    // -----------------------------------------------------------
    // ---------------- MANIPULAÇÃO DA STRING --------------------
    // -----------------------------------------------------------

    string objects = "", init = "", goal = "";
    char c1, c2;

    // Loop para ditar visibilidade
    for (int i = 0; i < board.size(); i++) {

        string str_i = to_string(i);

        objects += "l" + str_i + " ";

        for (int j = 0; j < board[0].size(); j++) {

            string str_j = to_string(j);

            c1 = board[i][j];

            if (c1 == '-') {

                // Loopando mesma linha
                for (int jj = j + 1; jj < board[i].size(); jj++) {

                    c2 = board[i][jj];

                    if (c2 == '#' || isdigit(c2)) {
                        break;
                    }

                    init += "(is-visible l" + str_i + " c" + str_j + " l" + str_i + " c" + to_string(jj) + ")\n";
                }

                // Loopando mesma coluna
                for (int ii = i + 1; ii < board.size(); ii++) {

                    c2 = board[ii][j];

                    if (c2 == '#' || isdigit(c2)) {
                        break;
                    }

                    init += "(is-visible l" + str_i + " c" + str_j + " l" + to_string(ii) + " c" + str_j + ")\n";
                }

                goal += "(is-on l" + str_i + " c" + str_j +")\n";
            
            } else if (isdigit(c1)) {

                init += "(counter-at l" + str_i + " c" + str_j + " i0)\n";
                goal += "(counter-at l" + str_i + " c" + str_j + " i" + c1 + ")\n";

                // Verificando conexões com quadrados brancos
                // Baixo
                if (i + 1 < board.size()) {

                    c2 = board[i + 1][j];

                    if (c2 == '-') {
                        init += "(is-numbered-adjacent l" + to_string(i + 1) + " c" + str_j + " l" + str_i + " c" + str_j + ")\n";
                    }
                }

                // Cima
                if (i - 1 > -1) {

                    c2 = board[i - 1][j];

                    if (c2 == '-') {
                        init += "(is-numbered-adjacent l" + to_string(i - 1) + " c" + str_j + " l" + str_i + " c" + str_j + ")\n";
                    }
                }

                // Direita
                if (j + 1 < board[0].size()) {

                    c2 = board[i][j + 1];

                    if (c2 == '-') {
                        init += "(is-numbered-adjacent l" + str_i + " c" + to_string(j + 1) + " l" + str_i + " c" + str_j + ")\n";
                    }
                }

                // Esquerda
                if (j - 1 > -1) {

                    c2 = board[i][j - 1];
                    if (c2 == '-') {
                        init += "(is-numbered-adjacent l" + str_i + " c" + to_string(j - 1) + " l" + str_i + " c" + str_j + ")\n";
                    }
                }
            }

            // Colocando as colunas
            if (i == board.size() - 1) {
                if (j == 0) {
                    objects += "- line \n";
                }

                objects += "c" + str_j + " ";
            }
        }
    }

    objects += "- col\n";

    std_problem.replace(std_problem.find(GOAL), GOAL.size(), GOAL + goal);
    std_problem.replace(std_problem.find(INIT), INIT.size(), INIT + init);
    std_problem.replace(std_problem.find(OBJ), OBJ.size(), OBJ + objects);

    // -----------------------------------------------------------
    // ---------------- CRIANDO O ARQUIVO ------------------------
    // -----------------------------------------------------------

    ofstream problem("problem.pddl");
    problem << std_problem;
    problem.close();

}

void cat_plan() {

    if (! filesystem::exists("plan.txt")) {
        exit(120);
    }

    system("cat plan.txt | grep 'STEP' | tr -d '[:space:]' | sed -E 's/STEP[0-9]+://g; s/\\(l/ /g; s/,c/ /g; s/bu/(bu/g; s/\\)/\\);/g; s/.$//g'");
}

int main() {

    auto start = chrono::high_resolution_clock::now();

    // string exec_command = "../planners/downward-fdss23/fast-downward.py --alias lama-first --overall-time-limit 29s --plan-file plan.txt domain.pddl problem.pddl  > /dev/null";
    string exec_command = "../planners/madagascar/Mp -o plan.txt -S 1 domain.pddl problem.pddl > /dev/null";
    make_domain();
    make_problem(make_board());
    system(exec_command.c_str());
    cat_plan();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Tempo de execução: " << duration.count() << " segundos" << '\n';

    return 0;
}